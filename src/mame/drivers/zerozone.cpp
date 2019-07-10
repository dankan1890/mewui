// license:BSD-3-Clause
// copyright-holders:Brad Oliver
/*********************************************************************************

    Zero Zone memory map

    driver by Brad Oliver

    CPU 1 : 68000, uses irq 1

    0x000000 - 0x01ffff : ROM
    0x080000 - 0x08000f : input ports and dipswitches
    0x088000 - 0x0881ff : palette RAM, 256 total colors
    0x09ce00 - 0x09d9ff : video ram, 48x32
    0x0c0000 - 0x0cffff : RAM
    0x0f8000 - 0x0f87ff : RAM (unused?)

    Stephh's notes :

      IMO, the game only has 2 buttons (1 to rotate the pieces and 1 for help).
      The 3rd button (when the Dip Switch is activated) subs one "line"
      (0x0c0966 for player 1 and 0x0c1082 for player 2) each time it is pressed.
      As I don't see why such thing would REALLY exist, I've added the
      IPF_CHEAT flag for the Dip Switch and the 3rd button of each player.

    TODO:
        * adpcm samples don't seem to be playing at the proper tempo - too fast?


*********************************************************************************/


#include "emu.h"
#include "includes/zerozone.h"

#include "cpu/m68000/m68000.h"
#include "sound/okim6295.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"


WRITE16_MEMBER( zerozone_state::sound_w )
{
	if (ACCESSING_BITS_8_15)
	{
		m_soundlatch->write(data >> 8);
		m_audiocpu->set_input_line_and_vector(0, HOLD_LINE, 0xff); // Z80
	}
}


void zerozone_state::main_map(address_map &map)
{
	map(0x000000, 0x01ffff).rom();
	map(0x080000, 0x080001).portr("SYSTEM");
	map(0x080002, 0x080003).portr("INPUTS");
	map(0x080008, 0x080009).portr("DSWB");
	map(0x08000a, 0x08000b).portr("DSWA");
	map(0x084000, 0x084001).w(FUNC(zerozone_state::sound_w));
	map(0x088000, 0x0881ff).ram().w("palette", FUNC(palette_device::write16)).share("palette");
	map(0x098000, 0x098001).ram();     /* Watchdog? */
	map(0x09ce00, 0x09ffff).ram().w(FUNC(zerozone_state::tilemap_w)).share("videoram");
	map(0x0b4000, 0x0b4001).w(FUNC(zerozone_state::tilebank_w));
	map(0x0c0000, 0x0cffff).ram();
	map(0x0f8000, 0x0f87ff).ram();     /* Never read from */
}

void zerozone_state::sound_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x87ff).ram();
	map(0x9800, 0x9800).rw("oki", FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0xa000, 0xa000).r(m_soundlatch, FUNC(generic_latch_8_device::read));
}


static INPUT_PORTS_START( zerozone )
	PORT_START("SYSTEM")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_NAME("P1 Score Line (Cheat)")
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_NAME("P2 Score Line (Cheat)")
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSWA")
	PORT_DIPNAME( 0x0007, 0x0007, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0007, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0005, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0008, "In Game Default" )       // 130, 162 or 255 "lines"
	PORT_DIPSETTING(      0x0000, "Always Hard" )           // 255 "lines"
	PORT_DIPNAME( 0x0010, 0x0010, "Speed" )
	PORT_DIPSETTING(      0x0010, DEF_STR( Normal ) )           // Drop every 20 frames
	PORT_DIPSETTING(      0x0000, "Fast" )              // Drop every 18 frames
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPUNUSED( 0x0040, IP_ACTIVE_LOW )
	PORT_DIPUNUSED( 0x0080, IP_ACTIVE_LOW )

	PORT_START("DSWB")
	PORT_DIPUNUSED( 0x0100, IP_ACTIVE_LOW )
	PORT_DIPUNUSED( 0x0200, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0400, 0x0400, "Helps" )
	PORT_DIPSETTING(      0x0400, "1" )
	PORT_DIPSETTING(      0x0000, "2" )
	PORT_DIPNAME( 0x0800, 0x0800, "Bonus Help" )
	PORT_DIPSETTING(      0x0000, "30000" )
	PORT_DIPSETTING(      0x0800, DEF_STR( None ) )
	PORT_DIPNAME( 0x1000, 0x1000, "Activate 'Score Line'? (Cheat)")
	PORT_DIPSETTING(      0x1000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Yes ) )
	PORT_DIPUNUSED( 0x2000, IP_ACTIVE_LOW )
	PORT_DIPUNUSED( 0x4000, IP_ACTIVE_LOW )
	PORT_SERVICE( 0x8000, IP_ACTIVE_LOW )
INPUT_PORTS_END


static const gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	RGN_FRAC(1,1),  /* 4096 characters */
	4,  /* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 0, 4, 8+0, 8+4, 16+0, 16+4, 24+0, 24+4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8    /* every sprite takes 32 consecutive bytes */
};


static GFXDECODE_START( gfx_zerozone )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout, 0, 256 )         /* sprites & playfield */
GFXDECODE_END


void zerozone_state::machine_start()
{
	save_item(NAME(m_tilebank));
}

void zerozone_state::machine_reset()
{
	m_tilebank = 0;
}

WRITE_LINE_MEMBER(zerozone_state::vblank_w)
{
	// TODO: Not accurate, find vblank acknowledge
	if (state)
		m_maincpu->set_input_line(1, HOLD_LINE);
}

void zerozone_state::zerozone(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 10000000);   /* 10 MHz */
	m_maincpu->set_addrmap(AS_PROGRAM, &zerozone_state::main_map);

	Z80(config, m_audiocpu, 1000000);  /* 1 MHz ??? */
	m_audiocpu->set_addrmap(AS_PROGRAM, &zerozone_state::sound_map);

	config.m_minimum_quantum = attotime::from_hz(600);

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(64*8, 32*8);
	screen.set_visarea(1*8, 47*8-1, 2*8, 30*8-1);
	screen.set_palette("palette");
	screen.set_screen_update(FUNC(zerozone_state::screen_update));
	screen.screen_vblank().set(FUNC(zerozone_state::vblank_w));

	GFXDECODE(config, m_gfxdecode, "palette", gfx_zerozone);

	PALETTE(config, "palette").set_format(palette_device::RRRRGGGGBBBBRGBx, 256);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, m_soundlatch);

	okim6295_device &oki(OKIM6295(config, "oki", 1056000, okim6295_device::PIN7_HIGH)); // clock frequency & pin 7 not verified
	oki.add_route(ALL_OUTPUTS, "mono", 1.0);
}



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( zerozone )
	ROM_REGION( 0x20000, "maincpu", 0 )     /* 128k for 68000 code */
	ROM_LOAD16_BYTE( "zz-4.rom", 0x0000, 0x10000, CRC(83718b9b) SHA1(b3fc6da5816142b9c92a7b8615eb5bcb2c78ea46) )
	ROM_LOAD16_BYTE( "zz-5.rom", 0x0001, 0x10000, CRC(18557f41) SHA1(6ef908732b7775c1ea2b33f799635075db5756de) )

	ROM_REGION( 0x10000, "audiocpu", 0 )      /* sound cpu */
	ROM_LOAD( "zz-1.rom", 0x00000, 0x08000, CRC(223ccce5) SHA1(3aa25ca914960b929dc853d07a958ed874e42fee) )

	ROM_REGION( 0x080000, "gfx1", 0 )
	ROM_LOAD( "zz-6.rom", 0x00000, 0x80000, CRC(c8b906b9) SHA1(1775d69df6397d6772b20c65751d44556d76c033) )

	ROM_REGION( 0x40000, "oki", 0 )      /* ADPCM samples */
	ROM_LOAD( "zz-2.rom", 0x00000, 0x20000, CRC(c7551e81) SHA1(520de3074fa6a71fef10d5a76cba5580fd1cbbae) )
	ROM_LOAD( "zz-3.rom", 0x20000, 0x20000, CRC(e348ff5e) SHA1(6d2755d9b31366f4c2ddd296790234deb8f821c8) )
ROM_END

ROM_START( lvgirl94 )
	ROM_REGION( 0x20000, "maincpu", 0 )     /* 128k for 68000 code */
	ROM_LOAD16_BYTE( "rom4", 0x0000, 0x10000, CRC(c4fb449e) SHA1(dd1c567ba2cf951267dd622e2e9af265e742f246) )
	ROM_LOAD16_BYTE( "rom5", 0x0001, 0x10000, CRC(5d446a1a) SHA1(2d7ea25e5b86e7cf4eb7f10daa1eaaaed6830a53) )

	ROM_REGION( 0x080000, "gfx1", 0 )
	ROM_LOAD( "rom6", 0x00000, 0x40000, CRC(eeeb94ba) SHA1(9da09312c090ef2d40f596247d9a7decf3724e54) )

	/* sound roms are the same as zerozone */
	ROM_REGION( 0x10000, "audiocpu", 0 )      /* sound cpu */
	ROM_LOAD( "rom1", 0x00000, 0x08000, CRC(223ccce5) SHA1(3aa25ca914960b929dc853d07a958ed874e42fee) )

	ROM_REGION( 0x40000, "oki", 0 )      /* ADPCM samples */
	ROM_LOAD( "rom2", 0x00000, 0x20000, CRC(c7551e81) SHA1(520de3074fa6a71fef10d5a76cba5580fd1cbbae) )
	ROM_LOAD( "rom3", 0x20000, 0x20000, CRC(e348ff5e) SHA1(6d2755d9b31366f4c2ddd296790234deb8f821c8) )
ROM_END


GAME( 1993, zerozone, 0, zerozone, zerozone, zerozone_state, empty_init, ROT0, "Comad", "Zero Zone", MACHINE_SUPPORTS_SAVE )
GAME( 1994, lvgirl94, 0, zerozone, zerozone, zerozone_state, empty_init, ROT0, "Comad", "Las Vegas Girl (Girl '94)", MACHINE_SUPPORTS_SAVE )
