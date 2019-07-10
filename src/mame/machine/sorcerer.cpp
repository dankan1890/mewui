// license:GPL-2.0+
// copyright-holders:Kevin Thacker, Robbbert
/******************************************************************************

  Exidy Sorcerer machine functions

*******************************************************************************/

#include "emu.h"
#include "includes/sorcerer.h"
#include "machine/z80bin.h"


/* timer for sorcerer serial chip transmit and receive */

TIMER_CALLBACK_MEMBER(sorcerer_state::serial_tc)
{
	/* if rs232 is enabled, uart is connected to clock defined by bit6 of port fe.
	Transmit and receive clocks are connected to the same clock. */

	/* if rs232 is disabled, receive clock is linked to cassette hardware */
	if (BIT(m_fe, 7))
	{
		/* connect to rs232 */
		m_rs232->write_txd(m_uart->so_r());
		m_uart->write_si(m_rs232->rxd_r());
	}
}


void sorcerer_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_SERIAL:
		serial_tc(ptr, param);
		break;
	case TIMER_CASSETTE:
		cassette_tc(ptr, param);
		break;
	case TIMER_RESET:
		sorcerer_reset(ptr, param);
		break;
	default:
		assert_always(false, "Unknown id in sorcerer_state::device_timer");
	}
}


/* timer to read cassette waveforms */

TIMER_CALLBACK_MEMBER(sorcerer_state::cassette_tc)
{
	uint8_t cass_ws = 0;
	switch (m_fe & 0xc0)        /*/ bit 7 low indicates cassette */
	{
		case 0x00:              /* Cassette 300 baud */

			/* loading a tape - this is basically the same as the super80.
			               We convert the 1200/2400 Hz signal to a 0 or 1, and send it to the uart. */

			m_cass_data.input.length++;

			cass_ws = ((((m_fe & 0x20) ? m_cassette2 : m_cassette1))->input() > +0.02) ? 1 : 0;

			if (cass_ws != m_cass_data.input.level)
			{
				m_cass_data.input.level = cass_ws;
				m_cass_data.input.bit = ((m_cass_data.input.length < 0x6) || (m_cass_data.input.length > 0x20)) ? 1 : 0;
				m_cass_data.input.length = 0;
				m_uart->write_si(m_cass_data.input.bit);
			}

			/* saving a tape - convert the serial stream from the uart, into 1200 and 2400 Hz frequencies.
			               Synchronisation of the frequency pulses to the uart is extremely important. */

			m_cass_data.output.length++;
			if (!(m_cass_data.output.length & 0x1f))
			{
				cass_ws = m_uart->so_r();
				if (cass_ws != m_cass_data.output.bit)
				{
					m_cass_data.output.bit = cass_ws;
					m_cass_data.output.length = 0;
				}
			}

			if (!(m_cass_data.output.length & 3))
			{
				if (!((m_cass_data.output.bit == 0) && (m_cass_data.output.length & 4)))
				{
					m_cass_data.output.level ^= 1;          // toggle output this, except on 2nd half of low bit
					((m_fe & 0x20) ? m_cassette2 : m_cassette1)->output(m_cass_data.output.level ? -1.0 : +1.0);
				}
			}
			return;

		case 0x40:          /* Cassette 1200 baud */
			/* loading a tape */
			m_cass_data.input.length++;

			cass_ws = ((((m_fe & 0x20) ? m_cassette2 : m_cassette1))->input() > +0.02) ? 1 : 0;

			if (cass_ws != m_cass_data.input.level || m_cass_data.input.length == 10)
			{
				m_cass_data.input.bit = ((m_cass_data.input.length < 10) || (m_cass_data.input.length > 0x20)) ? 1 : 0;
				if ( cass_ws != m_cass_data.input.level )
				{
					m_cass_data.input.length = 0;
					m_cass_data.input.level = cass_ws;
				}
				m_uart->write_si(m_cass_data.input.bit);
			}

			/* saving a tape - convert the serial stream from the uart, into 600 and 1200 Hz frequencies. */

			m_cass_data.output.length++;
			if (!(m_cass_data.output.length & 7))
			{
				cass_ws = m_uart->so_r();
				if (cass_ws != m_cass_data.output.bit)
				{
					m_cass_data.output.bit = cass_ws;
					m_cass_data.output.length = 0;
				}
			}

			if (!(m_cass_data.output.length & 7))
			{
				if (!((m_cass_data.output.bit == 0) && (m_cass_data.output.length & 8)))
				{
					m_cass_data.output.level ^= 1;          // toggle output this, except on 2nd half of low bit
					((m_fe & 0x20) ? m_cassette2 : m_cassette1)->output(m_cass_data.output.level ? -1.0 : +1.0);
				}
			}
			return;
	}
}


/* after the first 4 bytes have been read from ROM, switch the ram back in */
TIMER_CALLBACK_MEMBER(sorcerer_state::sorcerer_reset)
{
	membank("boot")->set_entry(0);
}


// The floppy sector has been read. Enable CPU.
WRITE_LINE_MEMBER(sorcerer_state::intrq_w)
{
	m_intrq_off = state ? false : true;
	if (state)
	{
		m_maincpu->set_input_line(Z80_INPUT_LINE_WAIT, CLEAR_LINE);
		m_wait = false;
	}
	else
	if (BIT(m_2c, 0) && m_drq_off && !m_wait)
	{
		m_maincpu->set_input_line(Z80_INPUT_LINE_WAIT, ASSERT_LINE);
		m_wait = true;
	}
}

// The next byte from floppy is available. Enable CPU so it can get the byte.
WRITE_LINE_MEMBER(sorcerer_state::drq_w)
{
	m_drq_off = state ? false : true;
	if (state)
	{
		m_maincpu->set_input_line(Z80_INPUT_LINE_WAIT, CLEAR_LINE);
		m_wait = false;
	}
	else
	if (BIT(m_2c, 0) && m_intrq_off && !m_wait)
	{
		m_maincpu->set_input_line(Z80_INPUT_LINE_WAIT, ASSERT_LINE);
		m_wait = true;
	}
}

// Port 2C control signals for the video/disk unit's floppy disks
// Signals are unknown so guess
// It outputs 24 or 25 when booting, so suppose that
// bit 0 = enable wait generator, bit 2 = drive 0 select, bit 5 = ??
WRITE8_MEMBER(sorcerer_state::port_2c_w)
{
	m_2c = data;

	if (BIT(data, 0))
	{
		if (!m_wait && m_drq_off && m_intrq_off)
		{
			m_maincpu->set_input_line(Z80_INPUT_LINE_WAIT, ASSERT_LINE);
			m_wait = true;
		}
	}

	floppy_image_device *floppy = nullptr;

	if (BIT(data, 2)) floppy = m_floppy20->get_device();
	if (BIT(data, 3)) floppy = m_floppy21->get_device();

	m_fdc2->set_floppy(floppy);

	if (floppy)
	{
		floppy->mon_w(0);
		floppy->ss_w(0);     // assume side 0 ? // BIT(data, 4));
	}

	m_fdc2->dden_w(0);   // assume double density ? //!BIT(data, 0));
}

WRITE8_MEMBER(sorcerer_state::port_fd_w)
{
	/* Translate data to control signals */

	m_uart->write_cs(0);
	m_uart->write_nb1(BIT(data, 0));
	m_uart->write_nb2(BIT(data, 1));
	m_uart->write_tsb(BIT(data, 2));
	m_uart->write_eps(BIT(data, 3));
	m_uart->write_np(BIT(data, 4));
	m_uart->write_cs(1);
}

WRITE8_MEMBER(sorcerer_state::port_fe_w)
{
	uint8_t changed_bits = (m_fe ^ data) & 0xf0;
	m_fe = data;

	/* bits 0..3 */
	m_keyboard_line = data & 0x0f;

	if (!changed_bits) return;

	/* bits 4..5 */
	/* does user want to hear the sound? */

	if (!BIT(data, 7)) // cassette operations
	{
		m_serial_timer->adjust(attotime::zero);

		bool sound = BIT(m_iop_config->read(), 3);

		m_cassette1->change_state(
			(BIT(data,4) && sound) ? CASSETTE_SPEAKER_ENABLED : CASSETTE_SPEAKER_MUTED, CASSETTE_MASK_SPEAKER);

		m_cassette2->change_state(
			(BIT(data,5) && sound) ? CASSETTE_SPEAKER_ENABLED : CASSETTE_SPEAKER_MUTED, CASSETTE_MASK_SPEAKER);

		/* cassette 1 motor */
		m_cassette1->change_state(
			(BIT(data,4)) ? CASSETTE_MOTOR_ENABLED : CASSETTE_MOTOR_DISABLED, CASSETTE_MASK_MOTOR);

		/* cassette 2 motor */
		m_cassette2->change_state(
			(BIT(data,5)) ? CASSETTE_MOTOR_ENABLED : CASSETTE_MOTOR_DISABLED, CASSETTE_MASK_MOTOR);

		if (data & 0x30)
			m_cassette_timer->adjust(attotime::zero, 0, attotime::from_hz(ES_UART_CLOCK*4));
		else
			m_cassette_timer->adjust(attotime::zero);
	}
	else
	{
		m_serial_timer->adjust(attotime::zero, 0, attotime::from_hz(ES_UART_CLOCK*4));
		m_cassette_timer->adjust(attotime::zero);
	}

	// bit 6 baud rate */
	if (BIT(changed_bits, 6))
	{
		m_uart_clock->set_unscaled_clock(BIT(data, 6) ? ES_UART_CLOCK*4 : ES_UART_CLOCK);
	}
}

WRITE8_MEMBER(sorcerer_state::port_ff_w)
{
	/// TODO: create a sorcerer parallel slot with a 7 bit and 8 bit centronics adapter as two of the options
	/// TODO: figure out what role FE plays http://www.trailingedge.com/exidy/exidych7.html
	m_centronics->write_data0(BIT(data, 0));
	m_centronics->write_data1(BIT(data, 1));
	m_centronics->write_data2(BIT(data, 2));
	m_centronics->write_data3(BIT(data, 3));
	m_centronics->write_data4(BIT(data, 4));
	m_centronics->write_data5(BIT(data, 5));
	m_centronics->write_data6(BIT(data, 6));

	/* reading the config switch */
	switch (m_iop_config->read() & 0x02)
	{
	case 0: /* 7-bit port */
		/* bit 7 = strobe, bit 6..0 = data */
		m_centronics->write_data7(0);
		m_centronics->write_strobe(BIT(data, 7));
		break;

	case 2: /* 8-bit port */
		/* hardware strobe driven from port select, bit 7..0 = data */
		m_centronics->write_data7(BIT(data, 7));
		m_centronics->write_strobe(0);
		m_centronics->write_strobe(1);
		break;
	}
}

READ8_MEMBER(sorcerer_state::port_fd_r)
{
	/* set unused bits high */
	uint8_t data = 0xe0;

	m_uart->write_swe(0);
	data |= m_uart->tbmt_r() ? 0x01 : 0;
	data |= m_uart->dav_r( ) ? 0x02 : 0;
	data |= m_uart->or_r(  ) ? 0x04 : 0;
	data |= m_uart->fe_r(  ) ? 0x08 : 0;
	data |= m_uart->pe_r(  ) ? 0x10 : 0;
	m_uart->write_swe(1);

	return data;
}

READ8_MEMBER(sorcerer_state::port_fe_r)
{
	/* bits 6..7
	 - hardware handshakes from user port
	 - not emulated
	 - tied high, allowing PARIN and PAROUT bios routines to run */

	uint8_t data = 0xc0;

	/* bit 5 - vsync */
	data |= m_iop_vs->read();

	/* bits 4..0 - keyboard data */
	data |= m_iop_x[m_keyboard_line]->read();

	return data;
}

/******************************************************************************
 Snapshot Handling
******************************************************************************/

SNAPSHOT_LOAD_MEMBER(sorcerer_state::snapshot_cb)
{
	uint8_t *RAM = memregion(m_maincpu->tag())->base();
	address_space &space = m_maincpu->space(AS_PROGRAM);
	uint8_t header[28];
	unsigned char s_byte;

	/* check size */
	if (snapshot_size != 0x1001c)
	{
		image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Snapshot must be 65564 bytes");
		image.message("Snapshot must be 65564 bytes");
		return image_init_result::FAIL;
	}

	/* get the header */
	image.fread( &header, sizeof(header));

	/* write it to ram */
	for (int i = 0; i < 0xc000; i++)
	{
		image.fread( &s_byte, 1);
		space.write_byte(i, s_byte);
	}
	image.fread( RAM+0xc000, 0x4000);

	/* patch CPU registers */
	m_maincpu->set_state_int(Z80_I, header[0]);
	m_maincpu->set_state_int(Z80_HL2, header[1] | (header[2] << 8));
	m_maincpu->set_state_int(Z80_DE2, header[3] | (header[4] << 8));
	m_maincpu->set_state_int(Z80_BC2, header[5] | (header[6] << 8));
	m_maincpu->set_state_int(Z80_AF2, header[7] | (header[8] << 8));
	m_maincpu->set_state_int(Z80_HL, header[9] | (header[10] << 8));
	m_maincpu->set_state_int(Z80_DE, header[11] | (header[12] << 8));
	m_maincpu->set_state_int(Z80_BC, header[13] | (header[14] << 8));
	m_maincpu->set_state_int(Z80_IY, header[15] | (header[16] << 8));
	m_maincpu->set_state_int(Z80_IX, header[17] | (header[18] << 8));
	m_maincpu->set_state_int(Z80_IFF1, header[19]&2 ? 1 : 0);
	m_maincpu->set_state_int(Z80_IFF2, header[19]&4 ? 1 : 0);
	m_maincpu->set_state_int(Z80_R, header[20]);
	m_maincpu->set_state_int(Z80_AF, header[21] | (header[22] << 8));
	m_maincpu->set_state_int(STATE_GENSP, header[23] | (header[24] << 8));
	m_maincpu->set_state_int(Z80_IM, header[25]);
	m_maincpu->set_pc(header[26] | (header[27] << 8));

	return image_init_result::PASS;
}

void sorcerer_state::machine_start()
{
	m_cassette_timer = timer_alloc(TIMER_CASSETTE);
	m_serial_timer = timer_alloc(TIMER_SERIAL);

	uint16_t endmem = 0xbfff;

	address_space &space = m_maincpu->space(AS_PROGRAM);
	/* configure RAM */
	switch (m_ram->size())
	{
	case 8*1024:
		space.unmap_readwrite(0x2000, endmem);
		break;

	case 16*1024:
		space.unmap_readwrite(0x4000, endmem);
		break;

	case 32*1024:
		space.unmap_readwrite(0x8000, endmem);
		break;
	}

	if (m_cart->exists())
		space.install_read_handler(0xc000, 0xdfff, read8sm_delegate(FUNC(generic_slot_device::read_rom),(generic_slot_device*)m_cart));
}

MACHINE_START_MEMBER(sorcerer_state,sorcererd)
{
	m_cassette_timer = timer_alloc(TIMER_CASSETTE);
	m_serial_timer = timer_alloc(TIMER_SERIAL);

	uint16_t endmem = 0xbbff;

	address_space &space = m_maincpu->space(AS_PROGRAM);
	/* configure RAM */
	switch (m_ram->size())
	{
	case 8*1024:
		space.unmap_readwrite(0x2000, endmem);
		break;

	case 16*1024:
		space.unmap_readwrite(0x4000, endmem);
		break;

	case 32*1024:
		space.unmap_readwrite(0x8000, endmem);
		break;
	}

	if (m_cart->exists())
		space.install_read_handler(0xc000, 0xdfff, read8sm_delegate(FUNC(generic_slot_device::read_rom),(generic_slot_device*)m_cart));
}

void sorcerer_state::machine_reset()
{
	address_space &space = m_maincpu->space(AS_PROGRAM);

	/* Initialize cassette interface */
	m_cass_data.output.length = 0;
	m_cass_data.output.level = 1;
	m_cass_data.input.length = 0;
	m_cass_data.input.bit = 1;

	m_drq_off = true;
	m_intrq_off = true;
	m_wait = false;
	m_fe = 0xff;
	m_2c = 0;
	port_fe_w(space, 0, 0, 0xff);

	membank("boot")->set_entry(1);
	timer_set(attotime::from_usec(10), TIMER_RESET);
}


/*-------------------------------------------------
    QUICKLOAD_LOAD_MEMBER( sorcerer_state, sorcerer )
-------------------------------------------------*/

QUICKLOAD_LOAD_MEMBER(sorcerer_state::quickload_cb)
{
	uint16_t execute_address, start_address, end_address;
	int autorun;
	address_space &space = m_maincpu->space(AS_PROGRAM);

	/* load the binary into memory */
	if (z80bin_load_file(&image, space, file_type, &execute_address, &start_address, &end_address) != image_init_result::PASS)
		return image_init_result::FAIL;

	/* is this file executable? */
	if (execute_address != 0xffff)
	{
		/* check to see if autorun is on */
		autorun = m_iop_config->read() & 1;

		if ((execute_address >= 0xc000) && (execute_address <= 0xdfff) && (space.read_byte(0xdffa) != 0xc3))
			return image_init_result::FAIL;     /* can't run a program if the cartridge isn't in */

		/* Since Exidy Basic is by Microsoft, it needs some preprocessing before it can be run.
		1. A start address of 01D5 indicates a basic program which needs its pointers fixed up.
		2. If autorunning, jump to C689 (command processor), else jump to C3DD (READY prompt).
		Important addresses:
		    01D5 = start (load) address of a conventional basic program
		    C858 = an autorun basic program will have this exec address on the tape
		    C3DD = part of basic that displays READY and lets user enter input */

		if (((start_address == 0x1d5) || (execute_address == 0xc858)) && (space.read_byte(0xdffa) == 0xc3))
		{
			uint8_t i;
			static const uint8_t data[]={
				0xcd, 0x26, 0xc4,   // CALL C426    ;set up other pointers
				0x21, 0xd4, 1,      // LD HL,01D4   ;start of program address (used by C689)
				0x36, 0,        // LD (HL),00   ;make sure dummy end-of-line is there
				0xc3, 0x89, 0xc6    // JP C689  ;run program
			};

			for (i = 0; i < ARRAY_LENGTH(data); i++)
				space.write_byte(0xf01f + i, data[i]);

			if (!autorun)
				space.write_word(0xf028,0xc3dd);

			/* tell BASIC where program ends */
			space.write_byte(0x1b7, end_address & 0xff);
			space.write_byte(0x1b8, (end_address >> 8) & 0xff);

			if ((execute_address != 0xc858) && autorun)
				space.write_word(0xf028, execute_address);

			m_maincpu->set_pc(0xf01f);
		}
		else
		{
			if (autorun)
				m_maincpu->set_pc(execute_address);
		}

	}

	return image_init_result::PASS;
}
