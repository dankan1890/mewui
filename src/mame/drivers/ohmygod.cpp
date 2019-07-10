// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/***************************************************************************

Oh My God!       (c) 1993 Atlus
Naname de Magic! (c) 1994 Atlus

driver by Nicola Salmoria

Notes:
- not sure about the scroll registers
- lots of unknown RAM, maybe other gfx planes not used by this game

***************************************************************************/

#include "emu.h"
#include "includes/ohmygod.h"

#include "cpu/m68000/m68000.h"
#include "machine/watchdog.h"
#include "sound/okim6295.h"
#include "screen.h"
#include "speaker.h"


WRITE16_MEMBER(ohmygod_state::ohmygod_ctrl_w)
{
	if (ACCESSING_BITS_0_7)
	{
		/* ADPCM bank switch */
		if (m_sndbank != ((data >> m_adpcm_bank_shift) & 0x0f))
		{
			m_sndbank = (data >> m_adpcm_bank_shift) & 0x0f;
			membank("okibank")->set_entry(m_sndbank);
		}
	}
	if (ACCESSING_BITS_8_15)
	{
		machine().bookkeeping().coin_counter_w(0, data & 0x1000);
		machine().bookkeeping().coin_counter_w(1, data & 0x2000);
	}
}

void ohmygod_state::ohmygod_map(address_map &map)
{
	map(0x000000, 0x07ffff).rom();
	map(0x300000, 0x303fff).ram();
	map(0x304000, 0x307fff).ram().w(FUNC(ohmygod_state::ohmygod_videoram_w)).share("videoram");
	map(0x308000, 0x30ffff).ram();
	map(0x400000, 0x400001).w(FUNC(ohmygod_state::ohmygod_scrollx_w));
	map(0x400002, 0x400003).w(FUNC(ohmygod_state::ohmygod_scrolly_w));
	map(0x600000, 0x6007ff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x700000, 0x703fff).ram().share("spriteram");
	map(0x704000, 0x707fff).ram();
	map(0x708000, 0x70ffff).ram();     /* Work RAM */
	map(0x800000, 0x800001).portr("P1");
	map(0x800002, 0x800003).portr("P2");
	map(0x900000, 0x900001).w(FUNC(ohmygod_state::ohmygod_ctrl_w));
	map(0xa00000, 0xa00001).portr("DSW1");
	map(0xa00002, 0xa00003).portr("DSW2");
	map(0xb00001, 0xb00001).rw("oki", FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0xc00000, 0xc00001).r("watchdog", FUNC(watchdog_timer_device::reset16_r));
	map(0xd00000, 0xd00001).w(FUNC(ohmygod_state::ohmygod_spritebank_w));
}

void ohmygod_state::oki_map(address_map &map)
{
	map(0x00000, 0x1ffff).rom();
	map(0x20000, 0x3ffff).bankr("okibank");
}

static INPUT_PORTS_START( ohmygod )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_SERVICE_NO_TOGGLE( 0x0200, IP_ACTIVE_LOW )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW1")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x0f00, 0x0f00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0700, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0500, "6 Coins/3 Credits" )
	PORT_DIPSETTING(      0x0900, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(      0x0f00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0300, "5 Coins/6 Credits" )
	PORT_DIPSETTING(      0x0200, DEF_STR( 4C_5C ) )
//  PORT_DIPSETTING(      0x0600, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0d00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0b00, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0a00, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf000, 0xf000, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x7000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x5000, "6 Coins/3 Credits" )
	PORT_DIPSETTING(      0x9000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(      0xf000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x3000, "5 Coins/6 Credits" )
	PORT_DIPSETTING(      0x2000, DEF_STR( 4C_5C ) )
//  PORT_DIPSETTING(      0x6000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xd000, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0xb000, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )

	PORT_START("DSW2")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x0300, 0x0300, "1P Difficulty" )
	PORT_DIPSETTING(      0x0200, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x0c00, 0x0c00, "VS Difficulty" )
	PORT_DIPSETTING(      0x0c00, "Normal Jake" )
	PORT_DIPSETTING(      0x0800, "Hard Jake" )
	PORT_DIPSETTING(      0x0400, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x1000, 0x1000, "Vs Matches/Credit" )
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPSETTING(      0x1000, "3" )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, "Balls Have Eyes" )
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x8000, 0x8000, "Test Mode" )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( naname )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_SERVICE_NO_TOGGLE( 0x0200, IP_ACTIVE_LOW )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW1")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x0f00, 0x0f00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0700, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0500, "6 Coins/3 Credits" )
	PORT_DIPSETTING(      0x0900, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(      0x0f00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0300, "5 Coins/6 Credits" )
	PORT_DIPSETTING(      0x0200, DEF_STR( 4C_5C ) )
//  PORT_DIPSETTING(      0x0600, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0d00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0b00, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0a00, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf000, 0xf000, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x7000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x5000, "6 Coins/3 Credits" )
	PORT_DIPSETTING(      0x9000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(      0xf000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x3000, "5 Coins/6 Credits" )
	PORT_DIPSETTING(      0x2000, DEF_STR( 4C_5C ) )
//  PORT_DIPSETTING(      0x6000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0xd000, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0xb000, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )

	PORT_START("DSW2")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0c00, 0x0c00, "Time Difficulty" )
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x1000, 0x1000, "Vs Matches/Credit" )
	PORT_DIPSETTING(      0x1000, "1" )
	PORT_DIPSETTING(      0x0000, "3" )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, "Freeze" )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4,
			8*4, 9*4, 10*4, 11*4, 12*4, 13*4, 14*4, 15*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
			8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8
};

static GFXDECODE_START( gfx_ohmygod )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,     0, 16 ) /* colors   0-255 */
	GFXDECODE_ENTRY( "gfx2", 0, spritelayout, 512, 16 ) /* colors 512-767 */
GFXDECODE_END



void ohmygod_state::machine_start()
{
	membank("okibank")->configure_entries(0, 16, memregion("oki")->base(), 0x20000);

	save_item(NAME(m_spritebank));
	save_item(NAME(m_scrollx));
	save_item(NAME(m_scrolly));
	save_item(NAME(m_sndbank));
}

void ohmygod_state::machine_reset()
{
	m_sndbank = 0;
	m_spritebank = 0;
	m_scrollx = 0;
	m_scrolly = 0;
}

void ohmygod_state::ohmygod(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 12000000);
	m_maincpu->set_addrmap(AS_PROGRAM, &ohmygod_state::ohmygod_map);
	m_maincpu->set_vblank_int("screen", FUNC(ohmygod_state::irq1_line_hold));

	WATCHDOG_TIMER(config, "watchdog").set_time(attotime::from_seconds(3));  /* a guess, and certainly wrong */


	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(64*8, 32*8);
	screen.set_visarea(12*8, (64-12)*8-1, 0*8, 30*8-1 );
	screen.set_screen_update(FUNC(ohmygod_state::screen_update_ohmygod));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_ohmygod);

	PALETTE(config, m_palette).set_format(palette_device::xGRB_555, 1024);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	okim6295_device &oki(OKIM6295(config, "oki", 14000000/8, okim6295_device::PIN7_HIGH));
	oki.set_addrmap(0, &ohmygod_state::oki_map);
	oki.add_route(ALL_OUTPUTS, "mono", 1.0);
}


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( ohmygod )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "omg-p.114", 0x00000, 0x80000, CRC(48fa40ca) SHA1(b1d91e1a4a888526febbe53a12b73e375f604f2b) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "omg-b.117",    0x00000, 0x80000, CRC(73621fa6) SHA1(de28c123eeaab78af83ab673431f90c97569450b) )

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD( "omg-s.120",    0x00000, 0x80000, CRC(6413bd36) SHA1(52c455d727496eae80bfab9460127c4c5a874e32) )

	ROM_REGION( 0x200000, "oki", 0 )
	ROM_LOAD( "omg-g.107",    0x00000, 0x200000, CRC(7405573c) SHA1(f4e7318c0a58f43d3c6370490637aea53b28547e) ) /* 00000-1ffff is fixed, 20000-3ffff is banked */
ROM_END

ROM_START( naname )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "036-prg.114", 0x00000, 0x80000, CRC(3b7362f7) SHA1(ba16ec9df8569bacd387561ef2b3ea5b17cb650c) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "036-bg.117",    0x00000, 0x80000, CRC(f53e8da5) SHA1(efaec4bb90cad75380ac6eb6859379cdefd187ac) )

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD( "036-spr.120",   0x00000, 0x80000, CRC(e36d8731) SHA1(652709d7884d40459c95761c8abcb394c4b712bf) )

	ROM_REGION( 0x200000, "oki", 0 )
	ROM_LOAD( "036-snd.107",  0x00000, 0x200000, CRC(a3e0caf4) SHA1(35b0eb4ae5b9df1b7c99ec2476a6d834ea50d2e3) ) /* 00000-1ffff is fixed, 20000-3ffff is banked */
ROM_END



void ohmygod_state::init_ohmygod()
{
	m_adpcm_bank_shift = 4;
}

void ohmygod_state::init_naname()
{
	m_adpcm_bank_shift = 0;
}


GAME( 1993, ohmygod, 0, ohmygod, ohmygod, ohmygod_state, init_ohmygod, ROT0, "Atlus", "Oh My God! (Japan)",       MACHINE_NO_COCKTAIL | MACHINE_SUPPORTS_SAVE )
GAME( 1994, naname,  0, ohmygod, naname,  ohmygod_state, init_naname,  ROT0, "Atlus", "Naname de Magic! (Japan)", MACHINE_NO_COCKTAIL | MACHINE_SUPPORTS_SAVE )
