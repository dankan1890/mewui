// license:BSD-3-Clause
// copyright-holders:David Haywood
/***************************************************************************

Zero Hour / Red Clash

runs on hardware similar to Lady Bug

driver by inkling

Notes:
- In the Tehkan set (redclsha) the ship doesn't move during attract mode. Earlier version?
  Gameplay is different too.

TODO:
- Colors might be right, need screen shots to verify

- Some graphical problems in both games, but without screenshots its hard to
  know what we're aiming for

- Sound (analog, schematics available for Zero Hour)

***************************************************************************/

#include "emu.h"
#include "includes/redclash.h"

#include "cpu/z80/z80.h"
#include "sound/sn76496.h"
#include "screen.h"
#include "speaker.h"


WRITE8_MEMBER( redclash_state::irqack_w )
{
	m_maincpu->set_input_line(0, CLEAR_LINE);
}

template <unsigned B> WRITE8_MEMBER(redclash_state::star_w)
{
	m_stars->set_speed(BIT(data, 0) << B, 1U << B);
}

void redclash_state::zerohour_map(address_map &map)
{
	map(0x0000, 0x2fff).rom();
	map(0x3000, 0x37ff).ram();
	map(0x3800, 0x3bff).ram().share(m_spriteram);
	map(0x4000, 0x43ff).ram().w(FUNC(redclash_state::videoram_w)).share(m_videoram);
	map(0x4800, 0x4800).portr("IN0");    /* IN0 */
	map(0x4801, 0x4801).portr("IN1");    /* IN1 */
	map(0x4802, 0x4802).portr("DSW1");   /* DSW0 */
	map(0x4803, 0x4803).portr("DSW2");   /* DSW1 */
	map(0x5000, 0x5007).nopw();    /* to sound board */
	map(0x5800, 0x5800).w(FUNC(redclash_state::star_w<0>));
	map(0x5801, 0x5804).nopw();    /* to sound board */
	map(0x5805, 0x5805).w(FUNC(redclash_state::star_w<1>));
	map(0x5806, 0x5806).w(FUNC(redclash_state::star_w<2>));
	map(0x5807, 0x5807).w(FUNC(redclash_state::flipscreen_w));
	map(0x7000, 0x7000).w(FUNC(redclash_state::star_reset_w));
	map(0x7800, 0x7800).w(FUNC(redclash_state::irqack_w));
}

void redclash_state::redclash_map(address_map &map)
{
	map(0x0000, 0x2fff).rom();
//  map(0x3000, 0x3000).set_nopw();
//  map(0x3800, 0x3800).set_nopw();
	map(0x4000, 0x43ff).ram().w(FUNC(redclash_state::videoram_w)).share(m_videoram);
	map(0x4800, 0x4800).portr("IN0");    /* IN0 */
	map(0x4801, 0x4801).portr("IN1");    /* IN1 */
	map(0x4802, 0x4802).portr("DSW1");   /* DSW0 */
	map(0x4803, 0x4803).portr("DSW2");   /* DSW1 */
	map(0x5000, 0x5007).nopw();    /* to sound board */
	map(0x5800, 0x5800).w(FUNC(redclash_state::star_w<0>));
	map(0x5801, 0x5801).w(FUNC(redclash_state::gfxbank_w));
	map(0x5805, 0x5805).w(FUNC(redclash_state::star_w<1>));
	map(0x5806, 0x5806).w(FUNC(redclash_state::star_w<2>));
	map(0x5807, 0x5807).w(FUNC(redclash_state::flipscreen_w));
	map(0x6000, 0x67ff).ram();
	map(0x6800, 0x6bff).ram().share(m_spriteram);
	map(0x7000, 0x7000).w(FUNC(redclash_state::star_reset_w));
	map(0x7800, 0x7800).w(FUNC(redclash_state::irqack_w));
}

/*
  This game doesn't have VBlank interrupts.
  Interrupts are still used, but they are related to coin
  slots. Left slot generates an IRQ, Right slot a NMI.
*/
INPUT_CHANGED_MEMBER( redclash_state::left_coin_inserted )
{
	if(newval)
		m_maincpu->set_input_line(0, ASSERT_LINE);
}

INPUT_CHANGED_MEMBER( redclash_state::right_coin_inserted )
{
	if(newval)
		m_maincpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
}

static INPUT_PORTS_START( redclash )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	/* Note that there are TWO VBlank inputs, one is active low, the other active */
	/* high. There are probably other differences in the hardware, but emulating */
	/* them this way is enough to get the game running. */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_VBLANK("screen")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_VBLANK("screen")

	PORT_START("DSW1")
	PORT_DIPNAME( 0x03, 0x03, "Difficulty?" )
	PORT_DIPSETTING(    0x03, "Easy?" )
	PORT_DIPSETTING(    0x02, "Medium?" )
	PORT_DIPSETTING(    0x01, "Hard?" )
	PORT_DIPSETTING(    0x00, "Hardest?" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x10, 0x10, "High Score" )
	PORT_DIPSETTING(    0x00, "0" )
	PORT_DIPSETTING(    0x10, "10000" )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x80, "5" )
	PORT_DIPSETTING(    0x40, "7" )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_8C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_9C ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_7C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_8C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_9C ) )

	PORT_START("FAKE")
	/* The coin slots are not memory mapped. Coin Left causes a NMI, */
	/* Coin Right an IRQ. This fake input port is used by the interrupt */
	/* handler to be notified of coin insertions. We use IMPULSE to */
	/* trigger exactly one interrupt, without having to check when the */
	/* user releases the key. */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, redclash_state, left_coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, redclash_state, right_coin_inserted, 0)
INPUT_PORTS_END

static INPUT_PORTS_START( zerohour )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	/* Note that there are TWO VBlank inputs, one is active low, the other active */
	/* high. There are probably other differences in the hardware, but emulating */
	/* them this way is enough to get the game running. */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_VBLANK("screen")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_VBLANK("screen")

	PORT_START("DSW1")
	PORT_DIPUNUSED_DIPLOC( 0x01, 0x01, "SW1:8" )    /* Switches 6-8 are not used */
	PORT_DIPUNUSED_DIPLOC( 0x02, 0x02, "SW1:7" )
	PORT_DIPUNUSED_DIPLOC( 0x04, 0x04, "SW1:6" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:5")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW1:4,3")   /* Also determines the default topscore, 0 for "No Bonus" */
	PORT_DIPSETTING(    0x00, "No Bonus" )
	PORT_DIPSETTING(    0x30, "5000" )
	PORT_DIPSETTING(    0x20, "8000" )
	PORT_DIPSETTING(    0x10, "10000" )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW1:2,1")
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0xc0, "3" )
	PORT_DIPSETTING(    0x80, "4" )
	PORT_DIPSETTING(    0x40, "5" )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW2:4,3,2,1")
	PORT_DIPSETTING(    0x06, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )    /* all other combinations give 1C_1C */
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW2:8,7,6,5")
	PORT_DIPSETTING(    0x60, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )    /* all other combinations give 1C_1C */
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )

	PORT_START("FAKE")
	/* The coin slots are not memory mapped. Coin Left causes a NMI, */
	/* Coin Right an IRQ. This fake input port is used by the interrupt */
	/* handler to be notified of coin insertions. We use IMPULSE to */
	/* trigger exactly one interrupt, without having to check when the */
	/* user releases the key. */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, redclash_state, left_coin_inserted, 0)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, redclash_state, right_coin_inserted, 0)
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 8*8, 0 },
	{ 7, 6, 5, 4, 3, 2, 1, 0 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8
};

static const gfx_layout spritelayout8x8 =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 1, 0 },
	{ STEP8(0,2) },
	{ STEP8(7*16,-16) },
	16*8
};

static const gfx_layout spritelayout16x16 =
{
	16,16,
	RGN_FRAC(1,1),
	2,
	{ 1, 0 },
	{ STEP8(24*2,2), STEP8(8*64+24*2,2) },
	{ STEP8(23*64,-64), STEP8(7*64,-64) },
	64*32
};

static const gfx_layout spritelayout24x24 =
{
	24,24,
	RGN_FRAC(1,1),
	2,
	{ 1, 0 },
	{ STEP8(0,2), STEP8(8*2,2), STEP8(16*2,2) },
	{ STEP8(23*64,-64), STEP8(15*64,-64), STEP8(7*64,-64) },
	64*32
};

static const gfx_layout spritelayout16x16bis =
{
	16,16,
	RGN_FRAC(1,1),
	2,
	{ 1, 0 },
	{ STEP8(0,2), STEP8(8*2,2) },
	{ STEP8(15*64,-64), STEP8(7*64,-64) },
	32*32
};

static GFXDECODE_START( gfx_redclash )
	GFXDECODE_ENTRY( "gfx1", 0x0000, charlayout,          0,  8 )
	GFXDECODE_ENTRY( "gfx3", 0x0000, spritelayout8x8,   4*8, 16 )
	GFXDECODE_ENTRY( "gfx2", 0x0000, spritelayout16x16, 4*8, 16 )
	GFXDECODE_ENTRY( "gfx2", 0x0000, spritelayout24x24, 4*8, 16 )
	GFXDECODE_ENTRY( "gfx2", 0x0000, spritelayout16x16bis, 4*8, 16 )
	GFXDECODE_ENTRY( "gfx2", 0x0004, spritelayout16x16bis, 4*8, 16 )
GFXDECODE_END


void redclash_state::machine_start()
{
	save_item(NAME(m_gfxbank));
}

void redclash_state::machine_reset()
{
	m_gfxbank = 0;
}

void redclash_state::zerohour(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 4000000);  /* 4 MHz */
	m_maincpu->set_addrmap(AS_PROGRAM, &redclash_state::zerohour_map);

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	screen.set_size(32*8, 32*8);
	screen.set_visarea(1*8, 31*8-1, 4*8, 28*8-1);
	screen.set_screen_update(FUNC(redclash_state::screen_update));
	screen.screen_vblank().set(FUNC(redclash_state::screen_vblank));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_redclash);
	PALETTE(config, m_palette, FUNC(redclash_state::palette), 4*8 + 4*16 + 32, 32 + 32);

	ZEROHOUR_STARS(config, m_stars, 0);

	/* sound hardware */
}


void redclash_state::redclash(machine_config &config)
{
	zerohour(config);

	m_maincpu->set_addrmap(AS_PROGRAM, &redclash_state::redclash_map);
}



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( zerohour )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ze1_1.c8", 0x0000, 0x0800, CRC(0dff4b48) SHA1(4911255f953851d0e5c2b66090b95254ac59ac9e) )
	ROM_LOAD( "ze2_2.c7", 0x0800, 0x0800, CRC(cf41b6ac) SHA1(263794e6be22c20e2b10fe9099e475097475df7b) )
	ROM_LOAD( "ze3_3.c6", 0x1000, 0x0800, CRC(5ef48b67) SHA1(ae291aa84b109e6a51eebdd5526abca1d901b7b9) )
	ROM_LOAD( "ze4_4.c5", 0x1800, 0x0800, CRC(25c5872d) SHA1(df008db607b72a92c4284d6a8127eafec2432ca4) )
	ROM_LOAD( "ze5_5.c4", 0x2000, 0x0800, CRC(d7ce3add) SHA1(d8dd7ad98e7a0a4f35de181549b2e88a9e0a73d6) )
	ROM_LOAD( "ze6_6.c3", 0x2800, 0x0800, CRC(8a93ae6e) SHA1(a66f05bb27e67b755c64ac8b68fa38ffe4cd961c) )

	ROM_REGION( 0x0800, "gfx1", 0 )
	ROM_LOAD( "z9", 0x0000, 0x0800, CRC(17ae6f13) SHA1(ce7a02f4e1aa2e5292d3807a0cfed6d92752fc7a) )

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "z7", 0x0000, 0x0800, CRC(4c12f59d) SHA1(b99a21415bff0e59b6130df60182f05b1a5d0811) )
	ROM_LOAD( "z8", 0x0800, 0x0800, CRC(6b9a6b6e) SHA1(f80d893b1b26c75c297e1da1c20db04e7129c92a) )

	ROM_REGION( 0x1000, "gfx3", ROMREGION_ERASE00 )
	/* gfx data will be rearranged here for 8x8 sprites */

	ROM_REGION( 0x0600, "proms", 0 )
	ROM_LOAD( "z1.ic2", 0x0000, 0x0020, CRC(b55aee56) SHA1(33e4767c8afbb7b3af67517ea1dfd69bf692cac7) ) /* 82S123, palette */
	ROM_LOAD( "z2.n2",  0x0020, 0x0020, CRC(9adabf46) SHA1(f3538fdbc4280b6be46a4d7ebb4c34bd1a1ce2b7) ) /* MM6330, sprite color lookup table */
	ROM_LOAD( "z3.u6",  0x0040, 0x0020, CRC(27fa3a50) SHA1(7cf59b7a37c156640d6ea91554d1c4276c1780e0) ) /* MM6330, Unknown purpose */
ROM_END

ROM_START( zerohoura ) /* Earlier version? */
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "z1_1.c8", 0x0000, 0x0800, CRC(77418b5d) SHA1(2ff7da1f0d9b311a32736c4784b1ba6acfc29512) )
	ROM_LOAD( "z2_2.c7", 0x0800, 0x0800, CRC(9b23b5ac) SHA1(2baca7a9ef65d5fe8216dce891a933d39bcdb1b1) )
	ROM_LOAD( "z3_3.c6", 0x1000, 0x0800, CRC(7aa25c95) SHA1(0bab20cabeb6ffe77d3122f2b489348b5efbdcb1) )
	ROM_LOAD( "z4_4.c5", 0x1800, 0x0800, CRC(b0a26dea) SHA1(69b00a5c4971fc161453efe6126bd36711420f99) )
	ROM_LOAD( "z5_5.c4", 0x2000, 0x0800, CRC(d7ce3add) SHA1(d8dd7ad98e7a0a4f35de181549b2e88a9e0a73d6) ) /* Same as above set */
	ROM_LOAD( "z6_6.c3", 0x2800, 0x0800, CRC(8a93ae6e) SHA1(a66f05bb27e67b755c64ac8b68fa38ffe4cd961c) ) /* Same as above set */

	ROM_REGION( 0x0800, "gfx1", 0 )
	ROM_LOAD( "z9", 0x0000, 0x0800, CRC(17ae6f13) SHA1(ce7a02f4e1aa2e5292d3807a0cfed6d92752fc7a) ) /* Same as above set */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "z7", 0x0000, 0x0800, CRC(4c12f59d) SHA1(b99a21415bff0e59b6130df60182f05b1a5d0811) ) /* Same as above set */
	ROM_LOAD( "z8", 0x0800, 0x0800, CRC(6b9a6b6e) SHA1(f80d893b1b26c75c297e1da1c20db04e7129c92a) ) /* Same as above set */

	ROM_REGION( 0x1000, "gfx3", ROMREGION_ERASE00 )
	/* gfx data will be rearranged here for 8x8 sprites */

	ROM_REGION( 0x0600, "proms", 0 )
	ROM_LOAD( "z1.ic2", 0x0000, 0x0020, CRC(b55aee56) SHA1(33e4767c8afbb7b3af67517ea1dfd69bf692cac7) ) /* 82S123, palette */
	ROM_LOAD( "z2.n2",  0x0020, 0x0020, CRC(9adabf46) SHA1(f3538fdbc4280b6be46a4d7ebb4c34bd1a1ce2b7) ) /* MM6330, sprite color lookup table */
	ROM_LOAD( "z3.u6",  0x0040, 0x0020, CRC(27fa3a50) SHA1(7cf59b7a37c156640d6ea91554d1c4276c1780e0) ) /* MM6330, Unknown purpose */
ROM_END

// k7, k5 and c4 did not give consistent reads, k7/k5 are graphics and clearly meant to be the same
//
// c4 was more damaged (lots of bytes replaced with 0x00) possibly UV exposed.
// I'm not sure where this rom is used by the game (wp 2000,800,r and bp 2000,800 catch nothing) for now I'm assuming it was meant to match.
// the game seems to be based off the 'zerohour' set but like many Inder sets it has some code inserted at the end of the last rom that is called by the game (bp 2fe0)
// Inder boards with the regular Universal code have also been seen.
ROM_START( zerohouri )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "zhi.c8", 0x0000, 0x0800, CRC(0dff4b48) SHA1(4911255f953851d0e5c2b66090b95254ac59ac9e) )
	ROM_LOAD( "zhi.c7", 0x0800, 0x0800, CRC(dcfefb4c) SHA1(9d8f57f5f09368225bae06a64971b82a848b1a7e) )
	ROM_LOAD( "zhi.c6", 0x1000, 0x0800, CRC(ddc66d36) SHA1(1cbf6e27e7b2ccd39199c1a26783d53d6d90d195) )
	ROM_LOAD( "zhi.c5", 0x1800, 0x0800, CRC(25c5872d) SHA1(df008db607b72a92c4284d6a8127eafec2432ca4) ) // == ze4_4.c5
//  ROM_LOAD( "zhi.c4", 0x2000, 0x0800, CRC(9b70464b) SHA1(ccd173e12630ba044fe659915dfce21f2b5e0e39) ) // corrupt
	ROM_LOAD( "zhi.c4", 0x2000, 0x0800, CRC(d7ce3add) SHA1(d8dd7ad98e7a0a4f35de181549b2e88a9e0a73d6) ) // use rom from above sets instead
	ROM_LOAD( "zhi.c3", 0x2800, 0x0800, CRC(29dee5e4) SHA1(13c1778d427a11f5c24ce8116fe55d60e98e3d83) )

	ROM_REGION( 0x0800, "gfx1", 0 )
	ROM_LOAD( "zhi.k7", 0x0000, 0x0800, CRC(17ae6f13) SHA1(ce7a02f4e1aa2e5292d3807a0cfed6d92752fc7a) ) /* Same as above set */

	ROM_REGION( 0x1000, "gfx2", 0 )
	ROM_LOAD( "zhi.k4", 0x0000, 0x0800, CRC(4c12f59d) SHA1(b99a21415bff0e59b6130df60182f05b1a5d0811) ) /* Same as above set */
	ROM_LOAD( "zhi.k5", 0x0800, 0x0800, CRC(6b9a6b6e) SHA1(f80d893b1b26c75c297e1da1c20db04e7129c92a) ) /* Same as above set */

	ROM_REGION( 0x1000, "gfx3", ROMREGION_ERASE00 )
	/* gfx data will be rearranged here for 8x8 sprites */

	ROM_REGION( 0x0600, "proms", 0 )
	ROM_LOAD( "z1.ic2", 0x0000, 0x0020, CRC(b55aee56) SHA1(33e4767c8afbb7b3af67517ea1dfd69bf692cac7) ) /* 82S123, palette */
	ROM_LOAD( "z2.n2",  0x0020, 0x0020, CRC(9adabf46) SHA1(f3538fdbc4280b6be46a4d7ebb4c34bd1a1ce2b7) ) /* MM6330, sprite color lookup table */
	ROM_LOAD( "z3.u6",  0x0040, 0x0020, CRC(27fa3a50) SHA1(7cf59b7a37c156640d6ea91554d1c4276c1780e0) ) /* MM6330, Unknown purpose */
ROM_END

ROM_START( redclash )
	ROM_REGION(0x10000, "maincpu", 0 )
	ROM_LOAD( "rc1.8c",       0x0000, 0x0800, CRC(fd90622a) SHA1(a65a32d519e7fee89b160f8152322df20b6af4ea) )
	ROM_LOAD( "rc2.7c",       0x0800, 0x0800, CRC(c8f33440) SHA1(60d1faee415faa13102b8e744f444f1480b8bd73) )
	ROM_LOAD( "rc3.6c",       0x1000, 0x0800, CRC(2172b1e9) SHA1(b6f7ee8924bda9f8da13baaa2db3ffb7d623236c) )
	ROM_LOAD( "rc4.5c",       0x1800, 0x0800, CRC(55c0d1b5) SHA1(2f1d729d29184de8f8bb914015352730325cda12) )
	ROM_LOAD( "rc5.4c",       0x2000, 0x0800, CRC(744b5261) SHA1(6c5de2f91f57c463230e0ea04b336347840161a3) )
	ROM_LOAD( "rc6.3c",       0x2800, 0x0800, CRC(fa507e17) SHA1(dd0e27b08e902b91c5e9552351206c671ed2f3c0) )

	ROM_REGION( 0x0800, "gfx1", 0 )
	ROM_LOAD( "rc6.12a",      0x0000, 0x0800, CRC(da9bbcc2) SHA1(4cbe03c7f5e99cc2f124e0089ea3c392156b5d92) ) /* rc9.7m */

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "rc4.4m",       0x0000, 0x0800, CRC(483a1293) SHA1(e7812475c7509389bcf8fee35598e9894428eb37) )
	ROM_CONTINUE(             0x1000, 0x0800 )
	ROM_LOAD( "rc5.5m",       0x0800, 0x0800, CRC(c45d9601) SHA1(2f156ad61161d65284df0cc55eb1b3b990eb41cb) )
	ROM_CONTINUE(             0x1800, 0x0800 )

	ROM_REGION( 0x2000, "gfx3", ROMREGION_ERASE00 )
	/* gfx data will be rearranged here for 8x8 sprites */

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "1.12f",        0x0000, 0x0020, CRC(43989681) SHA1(0d471e6f499294f2f62f27392b8370e2af8e38a3) ) /* 6331.7e */
	ROM_LOAD( "2.4a",         0x0020, 0x0020, CRC(9adabf46) SHA1(f3538fdbc4280b6be46a4d7ebb4c34bd1a1ce2b7) ) /* 6331.2r */
	ROM_LOAD( "3.11e",        0x0040, 0x0020, CRC(27fa3a50) SHA1(7cf59b7a37c156640d6ea91554d1c4276c1780e0) ) /* 6331.6w */
ROM_END

ROM_START( redclasht )
	ROM_REGION(0x10000, "maincpu", 0 )
	ROM_LOAD( "11.11c",       0x0000, 0x1000, CRC(695e070e) SHA1(8d0451a05572f62e0f282ab96bdd26d08b77a6c9) )
	ROM_LOAD( "13.7c",        0x1000, 0x1000, CRC(c2090318) SHA1(71725cdf51aedf5f29fa1dd1a41ad5e62c9a580d) )
	ROM_LOAD( "12.9c",        0x2000, 0x1000, CRC(b60e5ada) SHA1(37440f382c5e8852d804fa9837c36cc1e9d94d1d) )

	ROM_REGION(0x0800, "gfx1", 0 )
	ROM_LOAD( "6.12a",        0x0000, 0x0800, CRC(da9bbcc2) SHA1(4cbe03c7f5e99cc2f124e0089ea3c392156b5d92) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "14.3e",        0x0000, 0x0800, CRC(483a1293) SHA1(e7812475c7509389bcf8fee35598e9894428eb37) )
	ROM_CONTINUE(             0x1000, 0x0800 )
	ROM_LOAD( "15.3d",        0x0800, 0x0800, CRC(c45d9601) SHA1(2f156ad61161d65284df0cc55eb1b3b990eb41cb) )
	ROM_CONTINUE(             0x1800, 0x0800 )

	ROM_REGION( 0x2000, "gfx3", ROMREGION_ERASE00 )
	/* gfx data will be rearranged here for 8x8 sprites */

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "1.12f",        0x0000, 0x0020, CRC(43989681) SHA1(0d471e6f499294f2f62f27392b8370e2af8e38a3) ) /* palette */
	ROM_LOAD( "2.4a",         0x0020, 0x0020, CRC(9adabf46) SHA1(f3538fdbc4280b6be46a4d7ebb4c34bd1a1ce2b7) ) /* sprite color lookup table */
	ROM_LOAD( "3.11e",        0x0040, 0x0020, CRC(27fa3a50) SHA1(7cf59b7a37c156640d6ea91554d1c4276c1780e0) ) /* ?? */
ROM_END

ROM_START( redclashta )
	ROM_REGION(0x10000, "maincpu", 0 )
	ROM_LOAD( "rc1.11c",      0x0000, 0x1000, CRC(5b62ff5a) SHA1(981d3c72f28b7d136a0bad9243d39fd1ba3abc97) )
	ROM_LOAD( "rc3.7c",       0x1000, 0x1000, CRC(409c4ee7) SHA1(15c03a4093d7695751a143aa749229fcb7721f46) )
	ROM_LOAD( "rc2.9c",       0x2000, 0x1000, CRC(5f215c9a) SHA1(c305f7be19f6a052c08feb0b63a0326b6a1bd808) )

	ROM_REGION(0x0800, "gfx1", 0 )
	ROM_LOAD( "rc6.12a",      0x0000, 0x0800, CRC(da9bbcc2) SHA1(4cbe03c7f5e99cc2f124e0089ea3c392156b5d92) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "rc4.3e",       0x0000, 0x0800, CRC(64ca8b63) SHA1(5fd1ca9b81f66b4d2041674900718dc8c94c2a97) )
	ROM_CONTINUE(             0x1000, 0x0800 )
	ROM_LOAD( "rc5.3d",       0x0800, 0x0800, CRC(fce610a2) SHA1(0be829c6f6f5c3a19056ba1594141c1965c7aa2a) )
	ROM_CONTINUE(             0x1800, 0x0800 )

	ROM_REGION( 0x2000, "gfx3", ROMREGION_ERASE00 )
	/* gfx data will be rearranged here for 8x8 sprites */

	ROM_REGION( 0x0060, "proms", 0 )
	ROM_LOAD( "1.12f",        0x0000, 0x0020, CRC(43989681) SHA1(0d471e6f499294f2f62f27392b8370e2af8e38a3) ) /* palette */
	ROM_LOAD( "2.4a",         0x0020, 0x0020, CRC(9adabf46) SHA1(f3538fdbc4280b6be46a4d7ebb4c34bd1a1ce2b7) ) /* sprite color lookup table */
	ROM_LOAD( "3.11e",        0x0040, 0x0020, CRC(27fa3a50) SHA1(7cf59b7a37c156640d6ea91554d1c4276c1780e0) ) /* ?? */
ROM_END

// 2 PCB set (K-00A and K-00B)
ROM_START( redclashs )
	ROM_REGION(0x10000, "maincpu", 0 )
	ROM_LOAD( "1.11c",       0x0000, 0x1000, CRC(62275f85) SHA1(8f5d7113a012cc29e3729d54c4a0319c838a7c0d) )
	ROM_LOAD( "3.7c",        0x1000, 0x1000, CRC(c2090318) SHA1(71725cdf51aedf5f29fa1dd1a41ad5e62c9a580d) )
	ROM_LOAD( "2.9c",        0x2000, 0x1000, CRC(b60e5ada) SHA1(37440f382c5e8852d804fa9837c36cc1e9d94d1d) )

	ROM_REGION(0x0800, "gfx1", 0 )
	ROM_LOAD( "6.a12",        0x0000, 0x0800, CRC(da9bbcc2) SHA1(4cbe03c7f5e99cc2f124e0089ea3c392156b5d92) )

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "4.3e",        0x0000, 0x0800, CRC(483a1293) SHA1(e7812475c7509389bcf8fee35598e9894428eb37) )
	ROM_CONTINUE(            0x1000, 0x0800 )
	ROM_LOAD( "5.3d",        0x0800, 0x0800, CRC(c45d9601) SHA1(2f156ad61161d65284df0cc55eb1b3b990eb41cb) )
	ROM_CONTINUE(            0x1800, 0x0800 )

	ROM_REGION( 0x2000, "gfx3", ROMREGION_ERASE00 )
	/* gfx data will be rearranged here for 8x8 sprites */

	ROM_REGION( 0x0060, "proms", 0 ) // not dumped for this set
	ROM_LOAD( "1.12f",        0x0000, 0x0020, CRC(43989681) SHA1(0d471e6f499294f2f62f27392b8370e2af8e38a3) ) /* palette */
	ROM_LOAD( "2.4a",         0x0020, 0x0020, CRC(9adabf46) SHA1(f3538fdbc4280b6be46a4d7ebb4c34bd1a1ce2b7) ) /* sprite color lookup table */
	ROM_LOAD( "3.11e",        0x0040, 0x0020, CRC(27fa3a50) SHA1(7cf59b7a37c156640d6ea91554d1c4276c1780e0) ) /* ?? */
ROM_END

void redclash_state::init_redclash()
{
	uint8_t const *const src = memregion("gfx2")->base();
	uint8_t *const dst = memregion("gfx3")->base();
	int const len = memregion("gfx3")->bytes();

	/* rearrange the sprite graphics */
	for (int i = 0; i < len; i++)
	{
		int const j = (i & ~0x003e) | ((i & 0x0e) << 2) | ((i & 0x30) >> 3);
		dst[i] = src[j];
	}
}


GAME( 1980, zerohour,  0,        zerohour, zerohour, redclash_state, init_redclash, ROT270, "Universal",                   "Zero Hour (set 1)",      MACHINE_NO_SOUND | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1980, zerohoura, zerohour, zerohour, zerohour, redclash_state, init_redclash, ROT270, "Universal",                   "Zero Hour (set 2)",      MACHINE_NO_SOUND | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1980, zerohouri, zerohour, zerohour, zerohour, redclash_state, init_redclash, ROT270, "bootleg (Inder SA)",          "Zero Hour (Inder)",      MACHINE_NO_SOUND | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )

GAME( 1981, redclash,   0,        redclash, redclash, redclash_state, init_redclash, ROT270, "Kaneko",                      "Red Clash",                 MACHINE_NO_SOUND | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1981, redclasht,  redclash, redclash, redclash, redclash_state, init_redclash, ROT270, "Kaneko (Tehkan license)",     "Red Clash (Tehkan, set 1)", MACHINE_NO_SOUND | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1981, redclashta, redclash, redclash, redclash, redclash_state, init_redclash, ROT270, "Kaneko (Tehkan license)",     "Red Clash (Tehkan, set 2)", MACHINE_NO_SOUND | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1982, redclashs,  redclash, redclash, redclash, redclash_state, init_redclash, ROT270, "Kaneko (Suntronics license)", "Red Clash (Suntronics)",    MACHINE_NO_SOUND | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
