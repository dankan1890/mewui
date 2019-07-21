// license:BSD-3-Clause
// copyright-holders:Robbbert
/***************************************************************************

        BINBUG

        2013-01-14 Driver created

        All input must be in uppercase.

        Commands:
        A - See and alter memory
        B - Set breakpoint (2 permitted)
        C - Clear breakpoint
        D - cassette save
        G - Go to address, run
        L - cassette load
        S - See and alter registers

        BINBUG is an alternate bios to PIPBUG, however it uses its own
        video output. Method of output is through a DG640 board.

        Keyboard input, like PIPBUG, is via a serial device.
        The baud rate is 300, 8N1.

        The SENSE and FLAG lines are used for 300 baud cassette, in
        conjunction with unknown hardware.

        There are 3 versions of BINBUG:

        - 3.6 300 baud tape routines, 300 baud keyboard, memory-mapped VDU

        - 4.4 300 baud keyboard, ACOS tape system, advanced video routines

        - 5.2 ACOS tape system, 1200 baud terminal


        ToDo:
        - Need dumps of 4.4 and 5.2.

****************************************************************************/

#include "emu.h"
#include "bus/rs232/keyboard.h"
#include "cpu/s2650/s2650.h"
#include "imagedev/cassette.h"
#include "imagedev/snapquik.h"
#include "machine/clock.h"
#include "machine/timer.h"
#include "bus/s100/s100.h"
#include "bus/s100/dg640.h"
#include "speaker.h"


class binbug_state : public driver_device
{
public:
	binbug_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_cass(*this, "cassette")
		, m_rs232(*this, "keyboard")
		, m_clock(*this, "cass_clock")
		, m_s100(*this, "s100")
	{ }

	void binbug(machine_config &config);

private:
	DECLARE_READ8_MEMBER(mem_r);
	DECLARE_WRITE8_MEMBER(mem_w);
	DECLARE_WRITE_LINE_MEMBER(kansas_w);
	DECLARE_READ_LINE_MEMBER(binbug_serial_r);
	DECLARE_WRITE_LINE_MEMBER(binbug_serial_w);
	DECLARE_QUICKLOAD_LOAD_MEMBER(quickload_cb);
	TIMER_DEVICE_CALLBACK_MEMBER(kansas_r);

	u8 m_cass_data[4];
	bool m_cassold, m_cassinbit, m_cassoutbit;

	void binbug_mem(address_map &map);

	required_device<cpu_device> m_maincpu;
	required_device<cassette_image_device> m_cass;
	required_device<rs232_port_device> m_rs232;
	required_device<clock_device> m_clock;
	required_device<s100_bus_device> m_s100;
};

WRITE_LINE_MEMBER( binbug_state::kansas_w )
{
	if ((m_cass->get_state() & CASSETTE_MASK_UISTATE) != CASSETTE_RECORD)
		return;

	u8 twobit = m_cass_data[3] & 15;

	if (state)
	{
		if (twobit == 0)
			m_cassold = m_cassoutbit;

		if (m_cassold)
			m_cass->output(BIT(m_cass_data[3], 0) ? -1.0 : +1.0); // 2400Hz
		else
			m_cass->output(BIT(m_cass_data[3], 1) ? -1.0 : +1.0); // 1200Hz

		m_cass_data[3]++;
	}
}

TIMER_DEVICE_CALLBACK_MEMBER( binbug_state::kansas_r )
{
	// no tape - set to idle
	m_cass_data[1]++;
	if (m_cass_data[1] > 32)
	{
		m_cass_data[1] = 32;
		m_cassinbit = 1;
	}

	if ((m_cass->get_state() & CASSETTE_MASK_UISTATE) != CASSETTE_PLAY)
		return;

	/* cassette - turn 1200/2400Hz to a bit */
	uint8_t cass_ws = (m_cass->input() > +0.04) ? 1 : 0;

	if (cass_ws != m_cass_data[0])
	{
		m_cass_data[0] = cass_ws;
		m_cassinbit = (m_cass_data[1] < 12) ? 1 : 0;
		m_cass_data[1] = 0;
	}
}

READ_LINE_MEMBER( binbug_state::binbug_serial_r )
{
	return m_rs232->rxd_r() & m_cassinbit;
}

WRITE_LINE_MEMBER( binbug_state::binbug_serial_w )
{
	m_cassoutbit = state;
}

READ8_MEMBER( binbug_state::mem_r )
{
	return m_s100->smemr_r(offset + 0x7800);
}

WRITE8_MEMBER( binbug_state::mem_w )
{
	m_s100->mwrt_w(offset + 0x7800, data);
}

void binbug_state::binbug_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x03ff).rom();
	map(0x0400, 0x77ff).ram();
	map(0x7800, 0x7fff).rw(FUNC(binbug_state::mem_r),FUNC(binbug_state::mem_w));
}


/* Input ports */
static INPUT_PORTS_START( binbug )
INPUT_PORTS_END


QUICKLOAD_LOAD_MEMBER(binbug_state::quickload_cb)
{
	address_space &space = m_maincpu->space(AS_PROGRAM);
	int i;
	int quick_addr = 0x440;
	int exec_addr;
	int quick_length;
	std::vector<uint8_t> quick_data;
	int read_;
	image_init_result result = image_init_result::FAIL;

	quick_length = image.length();
	if (quick_length < 0x0444)
	{
		image.seterror(IMAGE_ERROR_INVALIDIMAGE, "File too short");
		image.message(" File too short");
	}
	else if (quick_length > 0x8000)
	{
		image.seterror(IMAGE_ERROR_INVALIDIMAGE, "File too long");
		image.message(" File too long");
	}
	else
	{
		quick_data.resize(quick_length);
		read_ = image.fread( &quick_data[0], quick_length);
		if (read_ != quick_length)
		{
			image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Cannot read the file");
			image.message(" Cannot read the file");
		}
		else if (quick_data[0] != 0xc4)
		{
			image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Invalid header");
			image.message(" Invalid header");
		}
		else
		{
			exec_addr = quick_data[1] * 256 + quick_data[2];

			if (exec_addr >= quick_length)
			{
				image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Exec address beyond end of file");
				image.message(" Exec address beyond end of file");
			}
			else
			{
				for (i = quick_addr; i < read_; i++)
					space.write_byte(i, quick_data[i]);

				/* display a message about the loaded quickload */
				image.message(" Quickload: size=%04X : exec=%04X",quick_length,exec_addr);

				// Start the quickload
				m_maincpu->set_state_int(S2650_PC, exec_addr);

				result = image_init_result::PASS;
			}
		}
	}

	return result;
}

static DEVICE_INPUT_DEFAULTS_START( keyboard )
	DEVICE_INPUT_DEFAULTS( "RS232_TXBAUD", 0xff, RS232_BAUD_300 )
	DEVICE_INPUT_DEFAULTS( "RS232_STARTBITS", 0xff, RS232_STARTBITS_1 )
	DEVICE_INPUT_DEFAULTS( "RS232_DATABITS", 0xff, RS232_DATABITS_8 )
	DEVICE_INPUT_DEFAULTS( "RS232_PARITY", 0xff, RS232_PARITY_NONE )
	DEVICE_INPUT_DEFAULTS( "RS232_STOPBITS", 0xff, RS232_STOPBITS_1 )
DEVICE_INPUT_DEFAULTS_END

static void binbug_s100_devices(device_slot_interface &device)
{
	device.option_add("dg640", S100_DG640);
}

void binbug_state::binbug(machine_config &config)
{
	SPEAKER(config, "mono").front_center();

	/* Cassette */
	CASSETTE(config, m_cass);
	m_cass->set_default_state(CASSETTE_STOPPED | CASSETTE_MOTOR_ENABLED | CASSETTE_SPEAKER_ENABLED);
	m_cass->add_route(ALL_OUTPUTS, "mono", 0.05);
	TIMER(config, "kansas_r").configure_periodic(FUNC(binbug_state::kansas_r), attotime::from_hz(40000));

	CLOCK(config, m_clock, 4'800); // 300 baud x 16(divider) = 4800
	m_clock->signal_handler().set(FUNC(binbug_state::kansas_w));

	/* basic machine hardware */
	s2650_device &maincpu(S2650(config, m_maincpu, XTAL(1'000'000)));
	maincpu.set_addrmap(AS_PROGRAM, &binbug_state::binbug_mem);
	maincpu.sense_handler().set(FUNC(binbug_state::binbug_serial_r));
	maincpu.flag_handler().set(FUNC(binbug_state::binbug_serial_w));

	/* Keyboard */
	RS232_PORT(config, m_rs232, default_rs232_devices, "keyboard").set_option_device_input_defaults("keyboard", DEVICE_INPUT_DEFAULTS_NAME(keyboard));

	/* quickload */
	QUICKLOAD(config, "quickload", "pgm", attotime::from_seconds(1)).set_load_callback(FUNC(binbug_state::quickload_cb), this);

	S100_BUS(config, m_s100, 0);
	S100_SLOT(config, "s100:1", binbug_s100_devices, "dg640");
}


/* ROM definition */
ROM_START( binbug )
	ROM_REGION( 0x8000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "binbug.rom", 0x0000, 0x0400, CRC(2cb1ac6e) SHA1(a969883fc767484d6b0fa103cfa4b4129b90441b) )
ROM_END

/* Driver */

//    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT   CLASS         INIT        COMPANY      FULLNAME      FLAGS
COMP( 1980, binbug, pipbug,   0,     binbug,    binbug, binbug_state, empty_init, "MicroByte", "BINBUG 3.6", 0 )

