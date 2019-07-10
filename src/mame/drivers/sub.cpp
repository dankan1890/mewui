// license:BSD-3-Clause
// copyright-holders:Angelo Salese, David Haywood
/*************************************************************************************

Submarine (c) 1985 Sigma

driver by David Haywood & Angelo Salese

TODO:
- finish dip-switches;
- a bunch of unemulated writes at 0xe***
- sound gets screwy if you coin it up with demo sounds on and during demo play (sound
  overlaps);

======================================================================================

 2 PCBs

 PCB1 - Bottom? (video?) board

|------------------------------------------------------------------- --|
|           B                                                          |
|     A           C     D     E     F     -     H     -    J           |
|                                                                      |
|                                                                      |
|1                                             (rom)                   |
|                                               OBJ1             T     |
|2                           Sony                                e    --
|                            CX23001           (rom)             x    --
|3                                              OBJ2             t    --
|                                                                     --C
|4                           (prom)            (rom)                  --N
|                              E4               OBJ3                  --3
|5                           (prom)                                   --
|                              E5                                     --
|6                           Sony                                     --
|                            CX23001                                  --
|7              (prom)                                                 |
|                 C7                                                   |
|8              (prom)                                                 |
|                 C8                                                   |
|9  (prom)                                                            --
|    A9    R                                                          --
|10 (prom) e                                                          --
|    A10   s                         HM6116P-3                        --C
|11 (prom)                                                            --N
|    A11                             (rom)                            --4
|12                                  VRAM1                            --
|                                                                     --
|13                                  (rom)                            --
|                                    VRAM2                            --
|14                                                                    |
|                                    (rom)                             |
|15                                  VRAM3                             |
|                                                                      |
|                                                                      |
|----------------------------------------------------------------------|

 Text = sigma enterprises, inc.
                 JAPAN F-021BCRT
         (rotated 90 degress left)

  Res = a bunch of resistors (colour weighting?)


PCB2  (Top board, CPU board)

|----------------------------------------------------------------------|
|                                                                      |
|         A       B       C       D      E       F       G      H      |
--|                                                        16000  18.432
  |                                                        OSC1    OSC2|
  |1           - - - - - - - - - - -                                   |
--|           |                                                    T   |
--|2                 NECD780        |                              e  --
--|           |      (z80 CPU)                                     x  --
--|  3                              |                              t  --
--|            - - - - - - - - - - -                                  --C
--|  4                                                                --N
--|                                                                   --3
--|C 5          T         T        T                                  --
  |N            E(rom)    E(rom)   E(rom)                             --
  |2 6          M         M        M                                  --
--|             P         P        P                                  --
|    7          1         2        3                                   |
--|C                                                                   |
  |N 8                                                                 |
  |1                                                                   |
--|  9                                                                --
--|                                                                   --
--| 10                                                                --
--|                                                                   --C
--| 11                                                                --N
--|                                                                   --4
--| 12                             DSW1 DSW2                          --
--|                                                                   --
--| 13                                                                --
  |                                                                   --
  | 14                                                         L       |
--|                                AY8910     R(rom)           H(z80A) |
|          15                                 O                0       |
|                                  AY8910     M                0       |
|          16                                 M                8       |
|                                                              0A      |
|----------------------------------------------------------------------|

 Text = F-020   CPU1 JAPAN   sigma enterprises, inc.

*************************************************************************************/

#include "emu.h"
#include "includes/sub.h"

void sub_state::machine_start()
{
	save_item(NAME(m_int_en));
	save_item(NAME(m_nmi_en));
}


WRITE_LINE_MEMBER(sub_state::int_mask_w)
{
	m_int_en = state;
	if (!m_int_en)
		m_maincpu->set_input_line(0, CLEAR_LINE);
}

void sub_state::subm_map(address_map &map)
{
	map(0x0000, 0xafff).rom();
	map(0xb000, 0xbfff).ram();
	map(0xc000, 0xc3ff).ram().w(FUNC(sub_state::attr_w)).share("attr");
	map(0xc400, 0xc7ff).ram().w(FUNC(sub_state::vram_w)).share("vram");
	map(0xd000, 0xd03f).ram().share("spriteram");
	map(0xd800, 0xd83f).ram().share("spriteram2");
	map(0xd840, 0xd85f).ram().w(FUNC(sub_state::scrolly_w)).share("scrolly");

	map(0xe000, 0xe000).w("watchdog", FUNC(watchdog_timer_device::reset_w));
	map(0xe800, 0xe807).w("mainlatch", FUNC(ls259_device::write_d0));

	map(0xf000, 0xf000).portr("DSW0"); // DSW0?
	map(0xf020, 0xf020).portr("DSW1"); // DSW1?
	map(0xf040, 0xf040).portr("SYSTEM");
	map(0xf060, 0xf060).portr("IN0");
}

WRITE8_MEMBER(sub_state::nmi_mask_w)
{
	m_nmi_en = data & 1;
	if (!m_nmi_en)
		m_soundcpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}

void sub_state::subm_io(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x00).r("soundlatch2", FUNC(generic_latch_8_device::read)).w(m_soundlatch, FUNC(generic_latch_8_device::write)); // to/from sound CPU
}

void sub_state::subm_sound_map(address_map &map)
{
	map(0x0000, 0x3fff).rom();
	map(0x4000, 0x47ff).ram();
	map(0x6000, 0x6000).w(FUNC(sub_state::nmi_mask_w));
}

void sub_state::subm_sound_io(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x00).r(m_soundlatch, FUNC(generic_latch_8_device::read)).w("soundlatch2", FUNC(generic_latch_8_device::write)); // to/from main CPU
	map(0x40, 0x41).rw("ay1", FUNC(ay8910_device::data_r), FUNC(ay8910_device::address_data_w));
	map(0x80, 0x81).rw("ay2", FUNC(ay8910_device::data_r), FUNC(ay8910_device::address_data_w));
}


static INPUT_PORTS_START( sub )
	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 )

	PORT_START("DSW0")
	PORT_DIPNAME( 0x01, 0x01, "DSWC" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0xf0, 0x10, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 4C_5C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
// Duplicates
//  PORT_DIPSETTING(    0xa0, DEF_STR( 1C_3C ) )
//  PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
//  PORT_DIPSETTING(    0xc0, DEF_STR( 1C_6C ) )
//  PORT_DIPSETTING(    0xd0, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Upright ) )  /* separate controls for each player */
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) ) /* Controls via player 1 for both, but need to get x/y screen flip working to fully test */
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END



static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 7*8, 6*8, 5*8, 4*8, 3*8, 2*8, 1*8, 0*8 },
	8*8
};

static const gfx_layout tiles16x32_layout =
{
	16,32,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), RGN_FRAC(0,3) },
	{ 64+0, 64+1, 64+2, 64+3, 64+4, 64+5, 64+6, 64+7, 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 55*8, 54*8, 53*8, 52*8, 51*8, 50*8, 49*8, 48*8,
		39*8, 38*8, 37*8, 36*8, 35*8, 34*8, 33*8, 32*8,
		23*8, 22*8, 21*8, 20*8, 19*8, 18*8, 17*8, 16*8,
		7*8,  6*8,  5*8,  4*8,  3*8,  2*8,  1*8,  0*8
	},
	64*8
};

static GFXDECODE_START( gfx_sub )
	GFXDECODE_ENTRY( "gfx1", 0, tiles8x8_layout, 0, 0x80 )
	GFXDECODE_ENTRY( "gfx2", 0, tiles16x32_layout, 0, 0x80 )
GFXDECODE_END

WRITE_LINE_MEMBER(sub_state::main_irq)
{
	if (state && m_int_en)
		m_maincpu->set_input_line(0, ASSERT_LINE);
}

WRITE_LINE_MEMBER(sub_state::flipscreen_w)
{
	flip_screen_set(!(state & 1));
}

INTERRUPT_GEN_MEMBER(sub_state::sound_irq)
{
	if (m_nmi_en)
		m_soundcpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}



void sub_state::sub(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, MASTER_CLOCK/6);      /* ? MHz */
	m_maincpu->set_addrmap(AS_PROGRAM, &sub_state::subm_map);
	m_maincpu->set_addrmap(AS_IO, &sub_state::subm_io);

	Z80(config, m_soundcpu, MASTER_CLOCK/6);         /* ? MHz */
	m_soundcpu->set_addrmap(AS_PROGRAM, &sub_state::subm_sound_map);
	m_soundcpu->set_addrmap(AS_IO, &sub_state::subm_sound_io);
	m_soundcpu->set_periodic_int(FUNC(sub_state::sound_irq), attotime::from_hz(120)); //???

	ls259_device &mainlatch(LS259(config, "mainlatch"));
	mainlatch.q_out_cb<0>().set(FUNC(sub_state::int_mask_w));
	mainlatch.q_out_cb<1>().set(FUNC(sub_state::flipscreen_w));
	mainlatch.q_out_cb<3>().set_nop(); // same as Q0?
	mainlatch.q_out_cb<5>().set_nop();

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(256, 256);
	screen.set_visarea(0, 256-1, 16, 256-16-1);
	screen.set_screen_update(FUNC(sub_state::screen_update));
	screen.set_palette(m_palette);
	screen.screen_vblank().set(FUNC(sub_state::main_irq));

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_sub);
	PALETTE(config, m_palette, FUNC(sub_state::sub_palette), 0x400, 0x100);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, m_soundlatch);
	m_soundlatch->data_pending_callback().set_inputline(m_soundcpu, 0);

	GENERIC_LATCH_8(config, "soundlatch2");

	AY8910(config, "ay1", MASTER_CLOCK/6/2).add_route(ALL_OUTPUTS, "mono", 0.23); /* ? Mhz */

	AY8910(config, "ay2", MASTER_CLOCK/6/2).add_route(ALL_OUTPUTS, "mono", 0.23); /* ? Mhz */
}


ROM_START( sub )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "temp 1 pos b6 27128.bin",  0x0000, 0x4000, CRC(6875b31d) SHA1(e7607e53687f1331cc97de939de144a7954ca3c3) )
	ROM_LOAD( "temp 2 pos c6 27128.bin",  0x4000, 0x4000, CRC(bc7f8f43) SHA1(088156a66acb2214c638d9d1ad18e9836b27eff0) )
	ROM_LOAD( "temp 3 pos d6 2764.bin",   0x8000, 0x2000, CRC(3546c226) SHA1(35e53c0db75c89e8e222d2139b841e77f5cc282c) )

	ROM_REGION( 0x10000, "soundcpu", 0 )
	ROM_LOAD( "m sound pos f14 2764.bin", 0x0000, 0x2000, CRC(61536a97) SHA1(84effc2251bf7c91e0bb670a651117503de8940d) )
	ROM_RELOAD( 0x2000, 0x2000 )

	ROM_REGION( 0xc000, "gfx1", 0)
	ROM_LOAD( "vram 1 pos f12 27128  version3.bin",   0x0000, 0x4000, CRC(8d176ba0) SHA1(b0bf4af97e991545d6b38e8159eb909376e6df35) )
	ROM_LOAD( "vram 2 pos f14 27128  version3.bin",   0x4000, 0x4000, CRC(0677cf3a) SHA1(072e9391f6a230b78124e820da0f0d27ffa45dc3) )
	ROM_LOAD( "vram 3 pos f15 27128  version3.bin",   0x8000, 0x4000, CRC(9a4cd1a0) SHA1(a321b88386424d73d7d73a7f321317b0f21d2eb6) )

	ROM_REGION( 0xc000, "gfx2", 0 )
	ROM_LOAD( "obj 1 pos h1 27128  version3.bin",     0x0000, 0x4000, CRC(63173e65) SHA1(2be3776c0e08d2c876cfce842e02345389e1fba0) )
	ROM_LOAD( "obj 2 pos h3 27128  version3.bin",     0x4000, 0x4000, CRC(3898d1a8) SHA1(acd3d7695a0fe9faa5e4315032c65e131d24a3ce) )
	ROM_LOAD( "obj 3 pos h4 27128  version3.bin",     0x8000, 0x4000, CRC(304e2145) SHA1(d4eb49b5502872718d64e53f02acd2150f6bf713) )

	ROM_REGION( 0x300, "proms", 0 ) // color proms
	ROM_LOAD( "prom pos a9 n82s129",      0x0200, 0x100, CRC(8df9cefe) SHA1(86320eb8135932d79c4478929b9fd90ffba55712) )
	ROM_LOAD( "prom pos a10 n82s129",     0x0100, 0x100, CRC(3c834094) SHA1(4d681431376a8ed071566d74d4accc737bf965dd) )
	ROM_LOAD( "prom pos a11 n82s129",     0x0000, 0x100, CRC(339afa95) SHA1(ff4ff712960f41c26419a681e8dcceaeef75d2e3) )

	ROM_REGION( 0x800, "proms2", 0 ) // look-up tables
	ROM_LOAD( "prom pos e5 n82s131",      0x0000, 0x200, CRC(0024b5dd) SHA1(7d623f8e8964336d643820850cef0fb641e52e22) )
	ROM_LOAD( "prom pos c7 n82s129",      0x0200, 0x100, CRC(9072d259) SHA1(9679fa01372d14a866836c9193204ff6e33cf67c) )
	ROM_LOAD( "prom pos e4 n82s131",      0x0400, 0x200, CRC(307aa2cf) SHA1(839eccf1d34adaf9a5006bfb30e3524bc19a9b41) )
	ROM_LOAD( "prom pos c8 n82s129",      0x0600, 0x100, CRC(351e1ef8) SHA1(530c9012ff5abda1c4ba9787ca999ca1ae1a893d) )
ROM_END

GAME( 1985, sub, 0, sub, sub, sub_state, empty_init, ROT270, "Sigma Enterprises Inc.", "Submarine (Sigma)", MACHINE_SUPPORTS_SAVE )
