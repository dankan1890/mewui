// license:BSD-3-Clause
// copyright-holders:Curt Coder, Robbbert
/***************************************************************************

        Sanyo PHC-25


    http://www.geocities.jp/sanyo_phc_25/
    http://www.phc25.com/

    Z80 @ 4 MHz
    MC6847 video
    3x 8KB bios ROM
    1x 4KB chargen ROM
    16KB RAM
    6KB video RAM

    LOCK key (CAPSLOCK) selects upper-case/lower-case on international version
    (phc25), and selects hiragana/upper-case on Japanese version (phc25j).



    TODO:

    - sound isn't working
    - screen attribute bit 7 is unknown



10 SCREEN3,1,1:COLOR,,1:CLS
20 X1=INT(RND(1)*256):Y1=INT(RND(1)*192):X2=INT(RND(1)*256):Y2=INT(RND(1)*192):C=INT(RND(1)*4)+1:LINE(X1,Y1)-(X2,Y2),C:GOTO 20
RUN


10 SCREEN2,1,1:CLS:FORX=0TO8:LINE(X*24,0)-(X*24+16,191),X,BF:NEXT
RUN

*****************************************************************************/

#include "emu.h"
#include "includes/phc25.h"

#include "speaker.h"


/* Read/Write Handlers */

READ8_MEMBER( phc25_state::port40_r )
{
	/*

	    bit     description

	    0
	    1
	    2
	    3
	    4       vertical sync
	    5       cassette read
	    6       centronics busy
	    7       horizontal sync

	*/

	uint8_t data = 0;

	/* vertical sync */
	data |= !m_vdg->fs_r() << 4;

	/* cassette read */
	data |= ((m_cassette)->input() < +0.3) << 5;

	/* centronics busy */
	data |= m_centronics_busy << 6;

	/* horizontal sync */
	data |= !m_vdg->hs_r() << 7;

	return data;
}

WRITE8_MEMBER( phc25_state::port40_w )
{
	/*

	    bit     description

	    0       cassette output
	    1       cassette motor
	    2       LED in the LOCK button (on = capslock)
	    3       centronics strobe
	    4
	    5       MC6847 GM0
	    6       MC6847 CSS
	    7       MC6847 A/G

	*/

	/* cassette output */
	m_cassette->output( BIT(data, 0) ? -1.0 : +1.0);

	/* cassette motor */
	m_cassette->change_state(BIT(data,1) ? CASSETTE_MOTOR_DISABLED : CASSETTE_MOTOR_ENABLED, CASSETTE_MASK_MOTOR);

	/* centronics strobe */
	m_centronics->write_strobe(BIT(data, 3));

	/* MC6847 */
	m_vdg->gm0_w(BIT(data, 5));
	m_vdg->css_w(BIT(data, 6));
	m_vdg->ag_w(BIT(data, 7));
	m_port40 = data;
}

WRITE_LINE_MEMBER( phc25_state::write_centronics_busy )
{
	m_centronics_busy = state;
}

/* Memory Maps */

void phc25_state::phc25_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x5fff).rom().region(Z80_TAG, 0);
	map(0x6000, 0x77ff).ram().share("video_ram");
	map(0xc000, 0xffff).ram();
}

void phc25_state::phc25_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	map(0x00, 0x00).w("cent_data_out", FUNC(output_latch_device::bus_w));
	map(0x40, 0x40).rw(FUNC(phc25_state::port40_r), FUNC(phc25_state::port40_w));
	map(0x80, 0x80).portr("KEY0");
	map(0x81, 0x81).portr("KEY1");
	map(0x82, 0x82).portr("KEY2");
	map(0x83, 0x83).portr("KEY3");
	map(0x84, 0x84).portr("KEY4");
	map(0x85, 0x85).portr("KEY5");
	map(0x86, 0x86).portr("KEY6");
	map(0x87, 0x87).portr("KEY7");
	map(0x88, 0x88).portr("KEY8");
	map(0xc0, 0xc0).rw(AY8910_TAG, FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
	map(0xc1, 0xc1).w(AY8910_TAG, FUNC(ay8910_device::address_w));
}

/* Input Ports */

static INPUT_PORTS_START( phc25 )
	PORT_START("KEY0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W) PORT_CHAR('w') PORT_CHAR('W')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S) PORT_CHAR('s') PORT_CHAR('S')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X) PORT_CHAR('x') PORT_CHAR('X')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(UTF8_UP) PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("INS DEL") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) // unlabeled key

	PORT_START("KEY1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("ESC") PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q) PORT_CHAR('q') PORT_CHAR('Q')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A) PORT_CHAR('a') PORT_CHAR('A')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z) PORT_CHAR('z') PORT_CHAR('Z')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(UTF8_DOWN) PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("RETURN") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COLON) PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/') PORT_CHAR('?')

	PORT_START("KEY2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R) PORT_CHAR('r') PORT_CHAR('R')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F) PORT_CHAR('f') PORT_CHAR('F')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V) PORT_CHAR('v') PORT_CHAR('V')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(UTF8_LEFT) PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR('^') PORT_CHAR('~')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR('[')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E) PORT_CHAR('e') PORT_CHAR('E')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D) PORT_CHAR('d') PORT_CHAR('D')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C) PORT_CHAR('c') PORT_CHAR('C')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME(UTF8_RIGHT) PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_TILDE) PORT_CHAR('\\') PORT_CHAR('|')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR(']')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("SPACE") PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')

	PORT_START("KEY4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y) PORT_CHAR('y') PORT_CHAR('Y')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H) PORT_CHAR('h') PORT_CHAR('H')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N) PORT_CHAR('n') PORT_CHAR('N')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F3") PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P) PORT_CHAR('p') PORT_CHAR('P')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T) PORT_CHAR('t') PORT_CHAR('T')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G) PORT_CHAR('g') PORT_CHAR('G')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B) PORT_CHAR('b') PORT_CHAR('B')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F4") PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('@')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY6")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U) PORT_CHAR('u') PORT_CHAR('U')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J) PORT_CHAR('j') PORT_CHAR('J')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M) PORT_CHAR('m') PORT_CHAR('M')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F2") PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O) PORT_CHAR('o') PORT_CHAR('O')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY7")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I) PORT_CHAR('i') PORT_CHAR('I')
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K) PORT_CHAR('k') PORT_CHAR('K')
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F1") PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L) PORT_CHAR('l') PORT_CHAR('L')
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_STOP) PORT_CHAR('.') PORT_CHAR('>')

	PORT_START("KEY8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("GRAPH") PORT_CODE(KEYCODE_LALT) PORT_CHAR(UCHAR_MAMEKEY(LALT))
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("SHIFT") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("CTRL") PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("LOCK") PORT_CODE(KEYCODE_CAPSLOCK) PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("JOY0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("JOY1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( phc25j )
	PORT_INCLUDE( phc25 )
INPUT_PORTS_END

/* Video */

READ8_MEMBER( phc25_state::video_ram_r )
{
	if (BIT(m_port40, 7)) // graphics
	{
		return m_video_ram[offset];
	}
	else    // text
	{
		offset &= 0x7ff;
		m_vdg->inv_w(BIT(m_video_ram[offset | 0x800], 0)); // cursor attribute
		m_vdg->as_w(BIT(m_video_ram[offset | 0x800], 1)); // screen2 lores attribute
		m_vdg->css_w(BIT(m_video_ram[offset | 0x800], 2)); // css attribute
		// bit 7 is set for all text (not spaces), meaning is unknown
		return m_video_ram[offset];
	}
}

MC6847_GET_CHARROM_MEMBER(phc25_state::pal_char_rom_r)
{
	return m_char_rom[(((ch - 2) * 12) + line + 4) & 0xfff];
}

// irq is inverted in emulation, so we need this trampoline
WRITE_LINE_MEMBER( phc25_state::irq_w )
{
	m_maincpu->set_input_line(0, state ? CLEAR_LINE : ASSERT_LINE);
}

MC6847_GET_CHARROM_MEMBER(phc25_state::ntsc_char_rom_r)
{
	return m_char_rom[(ch * 16 + line) & 0xfff];
}

void phc25_state::video_start()
{
	/* find memory regions */
	m_char_rom = memregion(Z80_TAG)->base() + 0x5000;
	m_port40 = 0;
}

/* Machine Driver */

void phc25_state::phc25(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, XTAL(4'000'000));
	m_maincpu->set_addrmap(AS_PROGRAM, &phc25_state::phc25_mem);
	m_maincpu->set_addrmap(AS_IO, &phc25_state::phc25_io);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
	ay8910_device &psg(AY8910(config, AY8910_TAG, 1996750));
	psg.port_a_read_callback().set_ioport("JOY0");
	psg.port_b_read_callback().set_ioport("JOY1");
	psg.add_route(ALL_OUTPUTS, "mono", 1.00);

	/* devices */
	CASSETTE(config, m_cassette);
	m_cassette->set_formats(phc25_cassette_formats);
	m_cassette->set_default_state(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_ENABLED);
	m_cassette->add_route(ALL_OUTPUTS, "mono", 0.15);
	m_cassette->set_interface("phc25_cass");

	CENTRONICS(config, m_centronics, centronics_devices, "printer");
	m_centronics->busy_handler().set(FUNC(phc25_state::write_centronics_busy));

	output_latch_device &cent_data_out(OUTPUT_LATCH(config, "cent_data_out"));
	m_centronics->set_output_latch(cent_data_out);

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("16K");

	/* software lists */
	SOFTWARE_LIST(config, "cass_list").set_original("phc25_cass");
}

void phc25_state::pal(machine_config &config)
{
	phc25(config);
	/* video hardware */
	SCREEN(config, "screen", SCREEN_TYPE_RASTER);

	MC6847_PAL(config, m_vdg, XTAL(4'433'619));
	m_vdg->set_screen("screen");
	m_vdg->fsync_wr_callback().set(FUNC(phc25_state::irq_w));
	m_vdg->input_callback().set(FUNC(phc25_state::video_ram_r));
	m_vdg->set_get_char_rom(FUNC(phc25_state::pal_char_rom_r));
	m_vdg->set_get_fixed_mode(mc6847_pal_device::MODE_GM2 | mc6847_pal_device::MODE_GM1 | mc6847_pal_device::MODE_INTEXT);
	// other lines not connected
}

void phc25_state::ntsc(machine_config &config)
{
	phc25(config);
	/* video hardware */
	SCREEN(config, "screen", SCREEN_TYPE_RASTER);

	MC6847_NTSC(config, m_vdg, XTAL(3'579'545));
	m_vdg->set_screen("screen");
	m_vdg->fsync_wr_callback().set(FUNC(phc25_state::irq_w));
	m_vdg->input_callback().set(FUNC(phc25_state::video_ram_r));
	m_vdg->set_get_char_rom(FUNC(phc25_state::ntsc_char_rom_r));
	m_vdg->set_get_fixed_mode(mc6847_ntsc_device::MODE_GM2 | mc6847_ntsc_device::MODE_GM1 | mc6847_ntsc_device::MODE_INTEXT);
	// other lines not connected
}

/* ROMs */

ROM_START( phc25 )
	ROM_REGION( 0x6000, Z80_TAG, 0 )
	ROM_LOAD( "phc25rom.0", 0x0000, 0x2000, CRC(fa28336b) SHA1(582376bee455e124de24ba4ac02326c8a592fa5a)) // 031_00aa.ic13 ?
	ROM_LOAD( "phc25rom.1", 0x2000, 0x2000, CRC(38fd578b) SHA1(dc3db78c0cdc89f1605200d39535be65a4091705)) // 031_01a.ic14 ?
	ROM_LOAD( "phc25rom.2", 0x4000, 0x2000, CRC(54392b27) SHA1(1587827fe9438780b50164727ce3fdea1b98078a)) // 031_02a.ic15 ?
	ROM_REGION( 0x1000, "chargen", 0 )
	ROM_LOAD( "031_04a.ic6", 0x0000, 0x1000, CRC(e56fb8c5) SHA1(6fc388c17fb43debfbc1464f767d0ce1375ce27b))
ROM_END

ROM_START( phc25j )
	ROM_REGION( 0x6000, Z80_TAG, 0 )
	ROM_LOAD( "phc25-11.0", 0x0000, 0x2000, CRC(287e83b0) SHA1(9fe960a8245f28efc04defeeeaceb1e5ec6793b8))
	ROM_LOAD( "phc25-11.1", 0x2000, 0x2000, CRC(6223f945) SHA1(5d44b883b6264cb5d2e21b2269308630c62e0e56))
	ROM_LOAD( "phc25-11.2", 0x4000, 0x2000, CRC(da859ae4) SHA1(6121e85947921e434d0157c378de3d81537f6b9f))
	//ROM_LOAD( "022 00aa.ic", 0x0000, 0x2000, NO_DUMP )
	//ROM_LOAD( "022 01aa.ic", 0x2000, 0x2000, NO_DUMP )
	//ROM_LOAD( "022 02aa.ic", 0x4000, 0x2000, NO_DUMP )
	//ROM_REGION( 0x1000, "chargen", 0 )
	//ROM_LOAD( "022 04a.ic", 0x0000, 0x1000, NO_DUMP )
ROM_END

/* Driver */

//    YEAR  NAME    PARENT  COMPAT  MACHINE  INPUT   CLASS        INIT        COMPANY  FULLNAME           FLAGS
COMP( 1983, phc25,  0,      0,      pal,     phc25,  phc25_state, empty_init, "Sanyo", "PHC-25 (Europe)", MACHINE_NO_SOUND )
COMP( 1983, phc25j, phc25,  0,      ntsc,    phc25j, phc25_state, empty_init, "Sanyo", "PHC-25 (Japan)",  MACHINE_NO_SOUND )
