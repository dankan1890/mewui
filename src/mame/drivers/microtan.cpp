// license:GPL-2.0+
// copyright-holders:Juergen Buchmueller
/******************************************************************************
 *  Microtan 65
 *
 *  system driver
 *
 *  Juergen Buchmueller <pullmoll@t-online.de>, Jul 2000
 *
 *  Thanks go to Geoff Macdonald <mail@geoff.org.uk>
 *  for his site http://www.geoff.org.uk/microtan/index.htm
 *  and to Fabrice Frances <frances@ensica.fr>
 *  for his site http://oric.free.fr/microtan.html
 *
 *  Microtan65 memory map
 *
 *  range     short     description
 *  0000-01ff SYSRAM    system ram
 *                      0000-003f variables
 *                      0040-00ff basic
 *                      0100-01ff stack
 *  0200-03ff VIDEORAM  display
 *  0400-afff RAM       main memory
 *  bc00-bc01 AY8912-0  sound chip #0
 *  bc02-bc03 AY8912-1  sound chip #1
 *  bc04      SPACEINV  space invasion sound (?)
 *  bfc0-bfcf VIA6522-0 VIA 6522 #0
 *  bfd0-bfd3 SIO       serial i/o
 *  bfe0-bfef VIA6522-1 VIA 6522 #1
 *  bff0      GFX_KBD   R: chunky graphics on W: reset KBD interrupt
 *  bff1      NMI       W: start delayed NMI
 *  bff2      HEX       W: hex. keypad column
 *  bff3      KBD_GFX   R: ASCII KBD / hex. keypad row W: chunky graphics off
 *  c000-e7ff BASIC     BASIC Interpreter ROM
 *  f000-f7ff XBUG      XBUG ROM
 *  f800-ffff TANBUG    TANBUG ROM
 *
 *  Tanbug commands:
 *  B         Set breakpoint
 *  C         copy (move) memory block
 *  G         Go
 *  L         Hex dump
 *  M         Modify memory
 *  N         Exit single-step mode
 *  O         Hex calculator
 *  P         Step once
 *  R         Register examine/modify
 *  S         Enter single-step mode
 *  BAS       Start BASIC (You need to choose maximum memory via dipswitches)
 *  WAR       Re-enter BASIC (warm start)
 *
 *****************************************************************************/

/* Core includes */
#include "emu.h"
#include "includes/microtan.h"

/* Components */
#include "cpu/m6502/m6502.h"
#include "machine/mos6551.h"


#include "emupal.h"
#include "screen.h"
#include "speaker.h"
#include "softlist.h"


void microtan_state::main_map(address_map &map)
{
	map(0x0000, 0x01ff).ram();
	map(0x0200, 0x03ff).ram().w(FUNC(microtan_state::videoram_w)).share(m_videoram);
	map(0xbc00, 0xbc00).w(m_ay8910[0], FUNC(ay8910_device::address_w));
	map(0xbc01, 0xbc01).rw(m_ay8910[0], FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
	map(0xbc02, 0xbc02).w(m_ay8910[1], FUNC(ay8910_device::address_w));
	map(0xbc03, 0xbc03).rw(m_ay8910[1], FUNC(ay8910_device::data_r), FUNC(ay8910_device::data_w));
	map(0xbfc0, 0xbfcf).m(m_via6522[0], FUNC(via6522_device::map));
	map(0xbfd0, 0xbfd3).rw("acia", FUNC(mos6551_device::read), FUNC(mos6551_device::write));
	map(0xbfe0, 0xbfef).m(m_via6522[1], FUNC(via6522_device::map));
	map(0xbff0, 0xbfff).rw(FUNC(microtan_state::bffx_r), FUNC(microtan_state::bffx_w));
	map(0xc000, 0xe7ff).rom();
	map(0xf000, 0xffff).rom();
}

static INPUT_PORTS_START( microtan )
	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x00, "Memory size" )
	PORT_DIPSETTING(    0x00, "1K" )
	PORT_DIPSETTING(    0x01, "1K + 7K TANEX" )
	PORT_DIPSETTING(    0x02, "1K + 7K TANEX + 40K TANRAM" )
	PORT_BIT( 0xfc, 0xfc, IPT_UNUSED )

	PORT_START("ROW0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("ESC") PORT_CODE(KEYCODE_ESC)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("1 !") PORT_CODE(KEYCODE_1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("2 \"") PORT_CODE(KEYCODE_2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("3 #") PORT_CODE(KEYCODE_3)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("4 $") PORT_CODE(KEYCODE_4)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("5 %") PORT_CODE(KEYCODE_5)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("6 &") PORT_CODE(KEYCODE_6)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("7 '") PORT_CODE(KEYCODE_7)

	PORT_START("ROW1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("8 *") PORT_CODE(KEYCODE_8)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("9 (") PORT_CODE(KEYCODE_9)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("0 )") PORT_CODE(KEYCODE_0)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("- _") PORT_CODE(KEYCODE_MINUS)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("= +") PORT_CODE(KEYCODE_EQUALS)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("` ~") PORT_CODE(KEYCODE_TILDE)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("BACK SPACE") PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("TAB") PORT_CODE(KEYCODE_TAB)

	PORT_START("ROW2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q) PORT_CHAR('q') PORT_CHAR('Q')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W) PORT_CHAR('w') PORT_CHAR('W')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E) PORT_CHAR('e') PORT_CHAR('E')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R) PORT_CHAR('R') PORT_CHAR('r')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T) PORT_CHAR('T') PORT_CHAR('t')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y') PORT_CHAR('y')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U) PORT_CHAR('U') PORT_CHAR('u')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I) PORT_CHAR('I') PORT_CHAR('i')

	PORT_START("ROW3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O) PORT_CHAR('O') PORT_CHAR('o')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P) PORT_CHAR('P') PORT_CHAR('p')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("[ {") PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("] }") PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("RETURN") PORT_CODE(KEYCODE_ENTER)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("DEL") PORT_CODE(KEYCODE_DEL)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("CTRL") PORT_CODE(KEYCODE_LCONTROL)
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_KEYBOARD ) PORT_NAME("CAPS LOCK") PORT_CODE(KEYCODE_CAPSLOCK) PORT_TOGGLE

	PORT_START("ROW4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A) PORT_CHAR('A') PORT_CHAR('a')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S) PORT_CHAR('S') PORT_CHAR('s')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D) PORT_CHAR('D') PORT_CHAR('d')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F) PORT_CHAR('F') PORT_CHAR('f')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G) PORT_CHAR('G') PORT_CHAR('g')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H) PORT_CHAR('H') PORT_CHAR('h')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J) PORT_CHAR('J') PORT_CHAR('j')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K) PORT_CHAR('K') PORT_CHAR('k')

	PORT_START("ROW5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L) PORT_CHAR('L') PORT_CHAR('l')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("; :") PORT_CODE(KEYCODE_COLON)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("' \"") PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("\\ |") PORT_CODE(KEYCODE_ASTERISK)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("SHIFT (L)") PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z') PORT_CHAR('z')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X) PORT_CHAR('X') PORT_CHAR('x')
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C) PORT_CHAR('C') PORT_CHAR('c')

	PORT_START("ROW6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V) PORT_CHAR('V') PORT_CHAR('v')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B) PORT_CHAR('B') PORT_CHAR('b')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N) PORT_CHAR('N') PORT_CHAR('n')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M) PORT_CHAR('M') PORT_CHAR('m')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME(", <") PORT_CODE(KEYCODE_COMMA)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME(". >") PORT_CODE(KEYCODE_STOP)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("/ ?") PORT_CODE(KEYCODE_SLASH)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("SHIFT (R)") PORT_CODE(KEYCODE_RSHIFT)

	PORT_START("ROW7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("LINE FEED")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("SPACE") PORT_CODE(KEYCODE_SPACE)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("- (KP)") PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME(", (KP)") PORT_CODE(KEYCODE_PLUS_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("ENTER (KP)") PORT_CODE(KEYCODE_ENTER_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME(". (KP)") PORT_CODE(KEYCODE_DEL_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("0 (KP)") PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("1 (KP)") PORT_CODE(KEYCODE_1_PAD)

	PORT_START("ROW8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("2 (KP)") PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("3 (KP)") PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("4 (KP)") PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("5 (KP)") PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("6 (KP)") PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("7 (KP)") PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("8 (KP)") PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("9 (KP)") PORT_CODE(KEYCODE_9_PAD)

	PORT_START("JOY") // VIA #1 PORT A
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )  PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )    PORT_4WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )  PORT_4WAY
INPUT_PORTS_END

static const gfx_layout char_layout =
{
	8, 16,      /* 8 x 16 graphics */
	128,        /* 128 codes */
	1,          /* 1 bit per pixel */
	{ 0 },      /* no bitplanes */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		8*8, 9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	8 * 16      /* code takes 8 times 16 bits */
};

static const gfx_layout chunky_layout =
{
	8, 16,      /* 8 x 16 graphics */
	256,        /* 256 codes */
	1,          /* 1 bit per pixel */
	{ 0 },      /* no bitplanes */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		8*8, 9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	8 * 16      /* code takes 8 times 16 bits */
};

static GFXDECODE_START( gfx_microtan )
	GFXDECODE_ENTRY( "gfx1", 0, char_layout, 0, 1 )
	GFXDECODE_ENTRY( "gfx2", 0, chunky_layout, 0, 1 )
GFXDECODE_END


void microtan_state::microtan(machine_config &config)
{
	/* basic machine hardware */
	M6502(config, m_maincpu, 6_MHz_XTAL / 8);  // 750 kHz
	m_maincpu->set_addrmap(AS_PROGRAM, &microtan_state::main_map);
	m_maincpu->set_vblank_int("screen", FUNC(microtan_state::interrupt));

	// The 6502 IRQ line is active low and probably driven by open collector outputs (guess).
	INPUT_MERGER_ANY_HIGH(config, m_irq_line).output_handler().set_inputline(m_maincpu, 0);

	/* video hardware - include overscan */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	screen.set_size(32*8, 16*16);
	screen.set_visarea(0*8, 32*8-1, 0*16, 16*16-1);
	screen.set_screen_update(FUNC(microtan_state::screen_update));
	screen.set_palette("palette");

	GFXDECODE(config, m_gfxdecode, "palette", gfx_microtan);

	PALETTE(config, "palette", palette_device::MONOCHROME);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	AY8910(config, m_ay8910[0], 1000000).add_route(ALL_OUTPUTS, "speaker", 0.5);
	AY8910(config, m_ay8910[1], 1000000).add_route(ALL_OUTPUTS, "speaker", 0.5);

	/* snapshot/quickload */
	snapshot_image_device &snapshot(SNAPSHOT(config, "snapshot", "dmp,m65"));
	snapshot.set_load_callback(FUNC(microtan_state::snapshot_cb), this);
	snapshot.set_interface("mt65_snap");
	QUICKLOAD(config, "quickload", "hex").set_load_callback(FUNC(microtan_state::quickload_cb), this);

	/* cassette */
	CASSETTE(config, m_cassette);
	m_cassette->add_route(ALL_OUTPUTS, "speaker", 0.05);
	TIMER(config, "read_cassette").configure_periodic(FUNC(microtan_state::read_cassette), attotime::from_hz(20000)); // cass read

	/* acia */
	mos6551_device &acia(MOS6551(config, "acia", 0));
	acia.set_xtal(1.8432_MHz_XTAL);

	/* via */
	VIA6522(config, m_via6522[0], 6_MHz_XTAL / 8);
	m_via6522[0]->readpa_handler().set(FUNC(microtan_state::via_0_in_a));
	m_via6522[0]->writepa_handler().set(FUNC(microtan_state::via_0_out_a));
	m_via6522[0]->writepb_handler().set(FUNC(microtan_state::via_0_out_b));
	m_via6522[0]->ca2_handler().set(FUNC(microtan_state::via_0_out_ca2));
	m_via6522[0]->cb2_handler().set(FUNC(microtan_state::via_0_out_cb2));
	m_via6522[0]->irq_handler().set(m_irq_line, FUNC(input_merger_device::in_w<IRQ_VIA_0>));

	VIA6522(config, m_via6522[1], 6_MHz_XTAL / 8);
	m_via6522[1]->writepa_handler().set(FUNC(microtan_state::via_1_out_a));
	m_via6522[1]->writepb_handler().set(FUNC(microtan_state::via_1_out_b));
	m_via6522[1]->ca2_handler().set(FUNC(microtan_state::via_1_out_ca2));
	m_via6522[1]->cb2_handler().set(FUNC(microtan_state::via_1_out_cb2));
	m_via6522[1]->irq_handler().set(m_irq_line, FUNC(input_merger_device::in_w<IRQ_VIA_1>));

	/* software lists */
	SOFTWARE_LIST(config, "snap_list").set_original("mt65_snap");
}

ROM_START( microtan )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tanex_j2.rom", 0xc000, 0x1000, CRC(3e09d384) SHA1(15a98941a672ff16242cc73f1dcf1d81fccd8910) )
	ROM_LOAD( "tanex_h2.rom", 0xd000, 0x1000, CRC(75105113) SHA1(c6fea4d65b7c52f43aa1589cace9467349a0f290) )
	ROM_LOAD( "tanex_d3.rom", 0xe000, 0x0800, CRC(ee6e8412) SHA1(7e1bca84bab79d94a4ab8554d23e2bc28ccd0384) )
	ROM_LOAD( "tanex_e2.rom", 0xe800, 0x0800, CRC(bd87fd34) SHA1(f41895df4a733dddfaf1c89ecff5040addcab804) )
	ROM_LOAD( "tanex_g2.rom", 0xf000, 0x0800, CRC(9fd233ee) SHA1(7b0be2d0402229ec80b062f7a0bed793686bcbf9) )
	ROM_LOAD( "tanbug_2.rom", 0xf800, 0x0400, CRC(7e215313) SHA1(c8fb3d33ce2beaf624dc75ec57d34c216b086274) )
	ROM_LOAD( "tanbug.rom",   0xfc00, 0x0400, CRC(c8221d9e) SHA1(c7fe4c174523aaaab30be7a8c9baf2bc08b33968) )

	ROM_REGION( 0x00800, "gfx1", 0 )
	ROM_LOAD( "charset.rom",  0x0000, 0x0800, CRC(3b3c5360) SHA1(a3a2f74149107f8b8f35b15069c71f3aa843d12f) )

	ROM_REGION( 0x01000, "gfx2", ROMREGION_ERASEFF )
	// initialized in init_microtan
ROM_END


//    YEAR  NAME      PARENT  COMPAT  MACHINE   INPUT     CLASS           INIT           COMPANY      FULLNAME        FLAGS
COMP( 1979, microtan, 0,      0,      microtan, microtan, microtan_state, init_microtan, "Tangerine", "Microtan 65" , 0 )
