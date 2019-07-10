// license:BSD-3-Clause
// copyright-holders:Curt Coder, Robbbert
/*****************************************************************************************

    ETI project 660, featured in the Australian magazine Electronics Today International.

    Commands:
    R - Reset (use this before any other instruction)
    S - Step
    0 - Enter modify memory mode
    2 - Save memory to cassette
    4 - Load memory from cassette
    6 - start binary program
    8 - start chip-8 program.

    To modify memory, press R, 0, enter 4-digit address, S, enter data, S, continue on.
    R to escape.

    To save a tape, enter the start address into 0400,0401 (big endian), and the end
    address into 0402,0403. Press R. Press record on the tape, press 2.

    To load a tape, enter the start and end addresses as above. Press R, 4. Screen goes
    black. Press play on tape. If the screen is still black after the tape ends, press R.

    All chip-8 programs start at 0600. The manual says the max end address is 7FF (gives
    512 bytes), but you can fill up all of memory (gives 2560 bytes).

    TODO:
    - sometimes there's no sound when started. You may need to hard reset until it beeps.
    - doesn't run programs for other chip-8 computers (this might be normal?)
    - we support BIN files, but have none to test with.
    - possible CPU bugs?:
      - in Invaders, can't shoot them
      - in Maze, the result is rubbish (works in Emma02 emulator v1.21, but not in v1.30)

**************************************************************************************************/

#include "emu.h"
#include "includes/eti660.h"
#include "screen.h"
#include "speaker.h"


/* Read/Write Handlers */
// Schematic is wrong, PCB layout is correct: D0-7 swapped around on PIA.
// There's still a bug in the PIA: if ca2 is instructed to go low, nothing happens.
READ8_MEMBER( eti660_state::pia_r )
{
	uint8_t pia_offset = m_maincpu->get_memory_address() & 0x03;

	return bitswap<8>(m_pia->read(pia_offset), 0,1,2,3,4,5,6,7);
}

WRITE8_MEMBER( eti660_state::pia_w )
{
	uint8_t pia_offset = m_maincpu->get_memory_address() & 0x03;
	data = bitswap<8>(data,0,1,2,3,4,5,6,7);
	m_pia->write(pia_offset, data);

	// handle bug in PIA
	if ((pia_offset == 1) && ((data & 0x30) == 0x30))
		ca2_w(BIT(data, 3));
}

WRITE_LINE_MEMBER( eti660_state::ca2_w ) // test with Wipeout game - it should start up in colour
{
	m_color_on = !state;
	m_cti->con_w(state);
}

WRITE8_MEMBER( eti660_state::colorram_w )
{
	offset = m_maincpu->get_memory_address() - 0xc80;

	uint8_t colorram_offset = (((offset & 0x1f0) >> 1) | (offset & 0x07));

	if (colorram_offset < 0xc0)
		m_color_ram[colorram_offset] = data;
}

/* Memory Maps */

void eti660_state::mem_map(address_map &map)
{
	map.global_mask(0xfff);
	map(0x0000, 0x03ff).rom();
	map(0x0400, 0x047f).ram();
	map(0x0480, 0x05ff).ram().share("videoram");
	map(0x0600, 0x0fff).ram();
}

void eti660_state::io_map(address_map &map)
{
	map(0x01, 0x01).rw(m_cti, FUNC(cdp1864_device::dispon_r), FUNC(cdp1864_device::step_bgcolor_w));
	map(0x02, 0x02).rw(FUNC(eti660_state::pia_r), FUNC(eti660_state::pia_w));
	map(0x03, 0x03).w(FUNC(eti660_state::colorram_w));
	map(0x04, 0x04).rw(m_cti, FUNC(cdp1864_device::dispoff_r), FUNC(cdp1864_device::tone_latch_w));
}

/* Input Ports */
static INPUT_PORTS_START( eti660 )
	PORT_START("KEY.0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_3) PORT_CHAR('3')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_2) PORT_CHAR('2')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_1) PORT_CHAR('1')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY.1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_7) PORT_CHAR('7')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_6) PORT_CHAR('6')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_5) PORT_CHAR('5')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_4) PORT_CHAR('4')
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY.2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_9) PORT_CHAR('9')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_8) PORT_CHAR('8')
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY.3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("SPECIAL")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_NAME("RESET") PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYPAD ) PORT_NAME("STEP") PORT_CODE(KEYCODE_S)
INPUT_PORTS_END

/* CDP1802 Interface */

READ_LINE_MEMBER( eti660_state::clear_r )
{
	// A hack to make the machine reset itself on
	// boot, like the real one does.
	if (m_resetcnt < 0xffff)
		m_resetcnt++;
	if (m_resetcnt == 0xf000)
		return 0;
	return BIT(m_special->read(), 0); // R key
}

READ_LINE_MEMBER( eti660_state::ef2_r )
{
	return m_cassette->input() < 0;
}

READ_LINE_MEMBER( eti660_state::ef4_r )
{
	return BIT(m_special->read(), 1); // S key
}

WRITE_LINE_MEMBER( eti660_state::q_w )
{
	/* CDP1864 audio output enable */
	m_cti->aoe_w(state);

	/* PULSE led */
	m_leds[LED_PULSE] = state ? 1 : 0;

	/* tape output */
	m_cassette->output(state ? 1.0 : -1.0);
}

WRITE8_MEMBER( eti660_state::dma_w )
{
	offset -= 0x480;

	m_color = 7;

	if (m_color_on)
	{
		uint8_t colorram_offset = ((offset & 0x1f0) >> 1) | (offset & 0x07);

		if (colorram_offset < 0xc0)
			m_color = m_color_ram[colorram_offset];
	}
	else
		m_color = m_p_videoram[offset] ? 7 : 0;

	m_cti->dma_w(space, offset, data);
}

/* PIA6821 Interface */

READ8_MEMBER( eti660_state::pia_pa_r )
{
	/*

	    bit     description

	    PA0     keyboard row 0
	    PA1     keyboard row 1
	    PA2     keyboard row 2
	    PA3     keyboard row 3
	    PA4     keyboard column 0
	    PA5     keyboard column 1
	    PA6     keyboard column 2
	    PA7     keyboard column 3

	*/

	uint8_t i, data = 0xff;

	for (i = 0; i < 4; i++)
		if (BIT(m_keylatch, i))
			return m_io_keyboard[i]->read();

	return data;
}

WRITE8_MEMBER( eti660_state::pia_pa_w )
{
	/*

	    bit     description

	    PA0     keyboard row 0
	    PA1     keyboard row 1
	    PA2     keyboard row 2
	    PA3     keyboard row 3
	    PA4     keyboard column 0
	    PA5     keyboard column 1
	    PA6     keyboard column 2
	    PA7     keyboard column 3

	*/

	m_keylatch = bitswap<8>(data,0,1,2,3,4,5,6,7) ^ 0xff;
}

void eti660_state::machine_reset()
{
	m_resetcnt = 0;
	m_color_on = 0;
	m_cti->con_w(0);
	m_maincpu->reset();  // needed
}

void eti660_state::machine_start()
{
	m_leds.resolve();

	save_item(NAME(m_color_ram));
	save_item(NAME(m_color));
	save_item(NAME(m_color_on));
	save_item(NAME(m_keylatch));
	save_item(NAME(m_resetcnt));
}

QUICKLOAD_LOAD_MEMBER(eti660_state::quickload_cb)
{
	address_space &space = m_maincpu->space(AS_PROGRAM);
	int i;
	int quick_addr = 0x600;
	int quick_length;
	std::vector<uint8_t> quick_data;
	int read_;
	image_init_result result = image_init_result::FAIL;

	quick_length = image.length();
	quick_data.resize(quick_length);
	read_ = image.fread( &quick_data[0], quick_length);
	if (read_ != quick_length)
	{
		image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Cannot read the file");
		image.message(" Cannot read the file");
	}
	else
	{
		for (i = 0; i < quick_length; i++)
			if ((quick_addr + i) < 0x1000)
				space.write_byte(i + quick_addr, quick_data[i]);

		/* display a message about the loaded quickload */
		if (image.is_filetype("bin"))
			image.message(" Quickload: size=%04X : start=%04X : end=%04X : Press 6 to start",quick_length,quick_addr,quick_addr+quick_length);
		else
			image.message(" Quickload: size=%04X : start=%04X : end=%04X : Press 8 to start",quick_length,quick_addr,quick_addr+quick_length);

		result = image_init_result::PASS;
	}

	return result;
}

/* Machine Drivers */

void eti660_state::eti660(machine_config &config)
{
	/* basic machine hardware */
	CDP1802(config, m_maincpu, XTAL(8'867'238)/5);
	m_maincpu->set_addrmap(AS_PROGRAM, &eti660_state::mem_map);
	m_maincpu->set_addrmap(AS_IO, &eti660_state::io_map);
	m_maincpu->wait_cb().set_constant(1);
	m_maincpu->clear_cb().set(FUNC(eti660_state::clear_r));
	m_maincpu->ef2_cb().set(FUNC(eti660_state::ef2_r));
	m_maincpu->ef4_cb().set(FUNC(eti660_state::ef4_r));
	m_maincpu->q_cb().set(FUNC(eti660_state::q_w));
	m_maincpu->dma_wr_cb().set(FUNC(eti660_state::dma_w));

	/* video hardware */
	SCREEN(config, SCREEN_TAG, SCREEN_TYPE_RASTER);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
	CDP1864(config, m_cti, XTAL(8'867'238)/5).set_screen(SCREEN_TAG);
	m_cti->inlace_cb().set_constant(0);
	m_cti->int_cb().set_inputline(m_maincpu, COSMAC_INPUT_LINE_INT);
	m_cti->dma_out_cb().set_inputline(m_maincpu, COSMAC_INPUT_LINE_DMAOUT);
	m_cti->efx_cb().set_inputline(m_maincpu, COSMAC_INPUT_LINE_EF1);
	m_cti->rdata_cb().set([this] () { return BIT(m_color, 0); });
	m_cti->bdata_cb().set([this] () { return BIT(m_color, 1); });
	m_cti->gdata_cb().set([this] () { return BIT(m_color, 2); });
	m_cti->set_chrominance(RES_K(2.2), RES_K(1), RES_K(4.7), RES_K(4.7)); // R7, R5, R6, R4
	m_cti->add_route(ALL_OUTPUTS, "mono", 0.25);

	/* devices */
	PIA6821(config, m_pia, 0);
	m_pia->readpa_handler().set(FUNC(eti660_state::pia_pa_r));
	m_pia->writepa_handler().set(FUNC(eti660_state::pia_pa_w));
	m_pia->ca2_handler().set(FUNC(eti660_state::ca2_w));  // not working, bug in pia
	m_pia->irqa_handler().set_inputline(m_maincpu, COSMAC_INPUT_LINE_INT).invert(); // FIXME: use an input merger for these lines
	m_pia->irqb_handler().set_inputline(m_maincpu, COSMAC_INPUT_LINE_INT).invert();

	CASSETTE(config, m_cassette);
	m_cassette->set_default_state(CASSETTE_STOPPED | CASSETTE_MOTOR_ENABLED | CASSETTE_SPEAKER_ENABLED);
	m_cassette->add_route(ALL_OUTPUTS, "mono", 0.05);

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("3K");

	/* quickload */
	QUICKLOAD(config, "quickload", "bin,c8,ch8", attotime::from_seconds(2)).set_load_callback(FUNC(eti660_state::quickload_cb), this);
}

/* ROMs */

ROM_START( eti660 )
	ROM_REGION( 0x10000, CDP1802_TAG, 0 )
	ROM_LOAD( "eti660.bin", 0x0000, 0x0400, CRC(811dfa62) SHA1(c0c4951e02f873f15560bdc3f35cdf3f99653922) )
ROM_END

//    YEAR  NAME    PARENT  COMPAT  MACHINE  INPUT   CLASS         INIT        COMPANY                            FULLNAME   FLAGS
COMP( 1981, eti660, 0,      0,      eti660,  eti660, eti660_state, empty_init, "Electronics Today International", "ETI-660", 0 )
