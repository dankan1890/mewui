// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    Atari Klax hardware

    driver by Aaron Giles

    Games supported:
        * Klax (1989) [6 original sets + 2 bootleg sets]

    Known bugs:
        * Bootleg sets don't work

****************************************************************************

    Memory map (TBA)

***************************************************************************/


#include "emu.h"
#include "includes/klax.h"

#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "machine/eeprompar.h"
#include "machine/watchdog.h"
#include "sound/okim6295.h"
#include "sound/msm5205.h"
#include "emupal.h"
#include "speaker.h"


/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

void klax_state::update_interrupts()
{
	m_maincpu->set_input_line(4, m_video_int_state || m_scanline_int_state ? ASSERT_LINE : CLEAR_LINE);
}


void klax_state::scanline_update(screen_device &screen, int scanline)
{
	/* generate 32V signals */
	if ((scanline & 32) == 0 && !m_screen->vblank() && !(m_p1->read() & 0x800))
		scanline_int_write_line(1);
}


void klax_state::interrupt_ack_w(u16 data)
{
	scanline_int_ack_w();
	video_int_ack_w();
}


/*************************************
 *
 *  Initialization
 *
 *************************************/

void klax_state::machine_reset()
{
	atarigen_state::machine_reset();
	scanline_timer_reset(*m_screen, 32);
}


/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

void klax_state::klax_map(address_map &map)
{
	map(0x000000, 0x03ffff).rom();
	map(0x0e0000, 0x0e0fff).rw("eeprom", FUNC(eeprom_parallel_28xx_device::read), FUNC(eeprom_parallel_28xx_device::write)).umask16(0x00ff);
	map(0x1f0000, 0x1fffff).w("eeprom", FUNC(eeprom_parallel_28xx_device::unlock_write16));
	map(0x260000, 0x260001).portr("P1").w(FUNC(klax_state::klax_latch_w));
	map(0x260002, 0x260003).portr("P2");
	map(0x270001, 0x270001).rw("oki", FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0x2e0000, 0x2e0001).w("watchdog", FUNC(watchdog_timer_device::reset16_w));
	map(0x360000, 0x360001).w(FUNC(klax_state::interrupt_ack_w));
	map(0x3e0000, 0x3e07ff).rw("palette", FUNC(palette_device::read8), FUNC(palette_device::write8)).umask16(0xff00).share("palette");
	map(0x3f0000, 0x3f0f7f).ram().w(m_playfield_tilemap, FUNC(tilemap_device::write16)).share("playfield");
	map(0x3f0f80, 0x3f0fff).ram().share("mob:slip");
	map(0x3f1000, 0x3f1fff).ram().w(m_playfield_tilemap, FUNC(tilemap_device::write16_ext)).share("playfield_ext");
	map(0x3f2000, 0x3f27ff).ram().share("mob");
	map(0x3f2800, 0x3f3fff).ram();
}

void klax_state::klax2bl_map(address_map &map)
{
	map(0x000000, 0x03ffff).rom();
	map(0x0e0000, 0x0e0fff).rw("eeprom", FUNC(eeprom_parallel_28xx_device::read), FUNC(eeprom_parallel_28xx_device::write)).umask16(0x00ff);
	map(0x1f0000, 0x1fffff).w("eeprom", FUNC(eeprom_parallel_28xx_device::unlock_write16));
	map(0x260000, 0x260001).portr("P1").w(FUNC(klax_state::klax_latch_w));
	map(0x260002, 0x260003).portr("P2");
	map(0x260006, 0x260007).w(FUNC(klax_state::interrupt_ack_w));
//  map(0x270001, 0x270001).rw("oki", FUNC(okim6295_device::read), FUNC(okim6295_device::write)); // no OKI here
	map(0x2e0000, 0x2e0001).w("watchdog", FUNC(watchdog_timer_device::reset16_w));
	map(0x3e0000, 0x3e07ff).rw("palette", FUNC(palette_device::read8), FUNC(palette_device::write8)).umask16(0xff00).share("palette");
	map(0x3f0000, 0x3f0f7f).ram().w(m_playfield_tilemap, FUNC(tilemap_device::write16)).share("playfield");
	map(0x3f0f80, 0x3f0fff).ram().share("mob:slip");
	map(0x3f1000, 0x3f1fff).ram().w(m_playfield_tilemap, FUNC(tilemap_device::write16_ext)).share("playfield_ext");
	map(0x3f2000, 0x3f27ff).ram().share("mob");
	map(0x3f2800, 0x3f3fff).ram();
}

/*************************************
 *
 *  Port definitions
 *
 *************************************/

static INPUT_PORTS_START( klax )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x00fc, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0600, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0800, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_VBLANK("screen")
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(1)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(1)

	PORT_START("P2")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0600, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE( 0x0800, IP_ACTIVE_LOW )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_PLAYER(2)
INPUT_PORTS_END


/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout pfmolayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,1) },
	{ STEP8(0,4) },
	{ STEP8(0,4*8) },
	8*8*4
};


static GFXDECODE_START( gfx_klax )
	GFXDECODE_ENTRY( "gfx1", 0, pfmolayout, 256, 16 ) /* playfield */
	GFXDECODE_ENTRY( "gfx2", 0, pfmolayout,   0, 16 ) /* sprites */
GFXDECODE_END

static const gfx_layout bootleg_layout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,8) },
	{ STEP8(0,1) },
	{ STEP8(0,8*4) },
	8*8*4
};

static GFXDECODE_START( gfx_klax2bl )
	GFXDECODE_ENTRY( "gfx1", 0, bootleg_layout, 256, 16 ) /* playfield */
	GFXDECODE_ENTRY( "gfx2", 0, pfmolayout,       0, 16 ) /* sprites */
GFXDECODE_END


/*************************************
 *
 *  Machine driver
 *
 *************************************/

void klax_state::klax(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, ATARI_CLOCK_14MHz/2);
	m_maincpu->set_addrmap(AS_PROGRAM, &klax_state::klax_map);

	EEPROM_2816(config, "eeprom").lock_after_write(true);

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	GFXDECODE(config, m_gfxdecode, "palette", gfx_klax);
	PALETTE(config, "palette").set_format(palette_device::IRGB_1555, 512).set_membits(8);

	TILEMAP(config, m_playfield_tilemap, m_gfxdecode, 2, 8, 8, TILEMAP_SCAN_COLS, 64, 32);
	m_playfield_tilemap->set_info_callback(FUNC(klax_state::get_playfield_tile_info));

	ATARI_MOTION_OBJECTS(config, m_mob, 0, m_screen, klax_state::s_mob_config);
	m_mob->set_gfxdecode(m_gfxdecode);

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	/* note: these parameters are from published specs, not derived */
	/* the board uses an SOS-2 chip to generate video signals */
	m_screen->set_raw(ATARI_CLOCK_14MHz/2, 456, 0, 336, 262, 0, 240);
	m_screen->set_screen_update(FUNC(klax_state::screen_update));
	m_screen->set_palette("palette");
	m_screen->screen_vblank().set(FUNC(klax_state::video_int_write_line));

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	OKIM6295(config, "oki", ATARI_CLOCK_14MHz/4/4, okim6295_device::PIN7_HIGH).add_route(ALL_OUTPUTS, "mono", 1.0);
}

void klax_state::bootleg_sound_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
}

void klax_state::klax2bl(machine_config &config)
{
	klax(config);

	m_maincpu->set_addrmap(AS_PROGRAM, &klax_state::klax2bl_map);

	config.device_remove("oki"); // no 6295 here

	z80_device &audiocpu(Z80(config, "audiocpu", 6000000)); /* ? */
	audiocpu.set_addrmap(AS_PROGRAM, &klax_state::bootleg_sound_map);

	m_gfxdecode->set_info(gfx_klax2bl);

	// guess, probably something like this
	// 2 x msm at least on bootleg set 2 (ic18 and ic19)
	MSM5205(config, "msm", 375000);     /* ? */
//  msm.vck_legacy_callback().set(FUNC(klax_state::m5205_int1));    /* interrupt function */
//  msm.set_prescaler_selector(msm5205_device::MSM5205_S96_4B);     /* 4KHz 4-bit */
//  msm.add_route(ALL_OUTPUTS, "mono", 0.25);
}

/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( klax )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075-6006.3n", 0x00000, 0x10000, CRC(e8991709) SHA1(90d69b0712e68e842a8b946539f1f43ef165e8de) )
	ROM_LOAD16_BYTE( "136075-6005.1n", 0x00001, 0x10000, CRC(72b8c510) SHA1(f79d3a2de4deaabbcec632e8be9a1d5f6c0c3740) )
	ROM_LOAD16_BYTE( "136075-6008.3k", 0x20000, 0x10000, CRC(c7c91a9d) SHA1(9f79ca689ec635f8113a74162e81f253c88992f5) )
	ROM_LOAD16_BYTE( "136075-6007.1k", 0x20001, 0x10000, CRC(d2021a88) SHA1(0f8a0dcc3bb5ca433601b1abfc796c98791facf6) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "136075-2010.17x", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD16_BYTE( "136075-2009.17u", 0x00001, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD16_BYTE( "136075-2012.12x", 0x20000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD16_BYTE( "136075-2011.12u", 0x20001, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "136075-2014.17y", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD16_BYTE( "136075-2013.17w", 0x00001, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x40000, "oki", 0 ) /* ADPCM data */
	ROM_LOAD( "136075-1015.14b", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1016.12b", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )

	ROM_REGION( 0x00573, "pals", 0 ) /* Lattice GAL16V8A-25LP GAL's */
	ROM_LOAD( "136075-1000.11c.bin", 0x0000, 0x0117, CRC(fb86e94a) SHA1(b16f037c49766ab734e47c8e1b16b5178809b8a3) )
	ROM_LOAD( "136075-1001.18l.bin", 0x0000, 0x0117, CRC(cd21acfe) SHA1(14bd9e2f1b50a1da550933e3fdc16e3f09b65e92) )
	ROM_LOAD( "136075-1002.8w.bin",  0x0000, 0x0117, CRC(4a7b6c44) SHA1(9579e098af3e5cd19bd14c361d3b1c5cb9047171) )
	ROM_LOAD( "136075-1003.9w.bin",  0x0000, 0x0117, CRC(72f7f904) SHA1(f792b5bcc313c5f3338a569a6f376a3ebb1eabf7) )
	ROM_LOAD( "136075-1004.6w.bin",  0x0000, 0x0117, CRC(6cd3270d) SHA1(84854b5beee539a80fc94f6e4637aa1c2543a1cb) )
ROM_END


ROM_START( klax5 )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "13607-5006.3n", 0x00000, 0x10000, CRC(05c98fc0) SHA1(84880d3d65c46c96c739063b3f61b1663989c56e) )
	ROM_LOAD16_BYTE( "13607-5005.1n", 0x00001, 0x10000, CRC(d461e1ee) SHA1(73e8615a742555f74c1086c0b745afc7e94a478f) )
	ROM_LOAD16_BYTE( "13607-5008.3k", 0x20000, 0x10000, CRC(f1b8e588) SHA1(080511f90aecb7526ab2107c196e73cb881a2bb5) )
	ROM_LOAD16_BYTE( "13607-5007.1k", 0x20001, 0x10000, CRC(adbe33a8) SHA1(c6c4f9ea5224169dbf4dda1062954563ebab18d4) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "136075-2010.17x", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD16_BYTE( "136075-2009.17u", 0x00001, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD16_BYTE( "136075-2012.12x", 0x20000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD16_BYTE( "136075-2011.12u", 0x20001, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "136075-2014.17y", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD16_BYTE( "136075-2013.17w", 0x00001, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x40000, "oki", 0 ) /* ADPCM data */
	ROM_LOAD( "136075-1015.14b", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1016.12b", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )

	ROM_REGION( 0x00573, "pals", 0 ) /* Lattice GAL16V8A-25LP GAL's */
	ROM_LOAD( "136075-1000.11c.bin", 0x0000, 0x0117, CRC(fb86e94a) SHA1(b16f037c49766ab734e47c8e1b16b5178809b8a3) )
	ROM_LOAD( "136075-1001.18l.bin", 0x0000, 0x0117, CRC(cd21acfe) SHA1(14bd9e2f1b50a1da550933e3fdc16e3f09b65e92) )
	ROM_LOAD( "136075-1002.8w.bin",  0x0000, 0x0117, CRC(4a7b6c44) SHA1(9579e098af3e5cd19bd14c361d3b1c5cb9047171) )
	ROM_LOAD( "136075-1003.9w.bin",  0x0000, 0x0117, CRC(72f7f904) SHA1(f792b5bcc313c5f3338a569a6f376a3ebb1eabf7) )
	ROM_LOAD( "136075-1004.6w.bin",  0x0000, 0x0117, CRC(6cd3270d) SHA1(84854b5beee539a80fc94f6e4637aa1c2543a1cb) )
ROM_END

ROM_START( klax5bl ) // derived from 'klax5' set
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "6.bin", 0x00000, 0x10000, CRC(3cfd2748) SHA1(165c446bab9df6517746451d056330386cb5212c) )
	ROM_LOAD16_BYTE( "2.bin", 0x00001, 0x10000, CRC(910e5bf9) SHA1(2b5af427e7cbad8d4ed2a202900f227295e1dea9) )
	ROM_LOAD16_BYTE( "5.bin", 0x20000, 0x10000, CRC(4fcacf88) SHA1(4ad87b03ac4cdf763586f8bf5d54bee950b6779c) )
	ROM_LOAD16_BYTE( "1.bin", 0x20001, 0x10000, CRC(ed0e3585) SHA1(5dfdcca15fee6ec3ae8a47fff4d066860e902082) )

	ROM_REGION( 0x40000, "audiocpu", 0 )
	ROM_LOAD( "3.bin", 0x00000, 0x10000, CRC(b0441f1c) SHA1(edced52b86641ce6db934ba05435f1221a12809a) )
	ROM_LOAD( "4.bin", 0x10000, 0x10000, CRC(a245e005) SHA1(8843edfa9deec405f491647d40007d0a38c25262) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD32_BYTE( "9.bin",  0x00000, 0x10000, CRC(ebe4bd96) SHA1(31f941e39aeaed6a64b35827df4d234cd641b47d) )
	ROM_LOAD32_BYTE( "10.bin", 0x00001, 0x10000, CRC(e7ad1cbd) SHA1(4b37cbe5d3168e532b00e8e34e7b8cf6d69e3487) )
	ROM_LOAD32_BYTE( "11.bin", 0x00002, 0x10000, CRC(ef7712fd) SHA1(9308b37a8b024837b32d10e358a5205fdc582214) )
	ROM_LOAD32_BYTE( "12.bin", 0x00003, 0x10000, CRC(1e0c1262) SHA1(960d61b9751276e4d0dbfd3f07cadc1329079abc) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "7.bin", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD16_BYTE( "8.bin", 0x00001, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )
ROM_END

ROM_START( klax5bl2 ) // derived from 'klax5' set, closer than klax2bl
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "3.ic31", 0x00000, 0x10000, CRC(e43699f3) SHA1(2a78959ad065e1c0f69cc2ba4146a50102ccfd7e) )
	ROM_LOAD16_BYTE( "1.ic13", 0x00001, 0x10000, CRC(dc67f13a) SHA1(6021f48b53f9000983bcd786b8366ba8638174de) )
	ROM_LOAD16_BYTE( "4.ic30", 0x20000, 0x10000, CRC(f1b8e588) SHA1(080511f90aecb7526ab2107c196e73cb881a2bb5) )
	ROM_LOAD16_BYTE( "2.ic12", 0x20001, 0x10000, CRC(adbe33a8) SHA1(c6c4f9ea5224169dbf4dda1062954563ebab18d4) )

	ROM_REGION( 0x40000, "audiocpu", 0 )
	ROM_LOAD( "6.ic22", 0x00000, 0x10000, CRC(edd4c42c) SHA1(22f992615afa24a7a671ed2f5cf08f25965d5b3a) )
	ROM_LOAD( "5.ic23", 0x10000, 0x10000, CRC(a245e005) SHA1(8843edfa9deec405f491647d40007d0a38c25262) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD32_BYTE( "8.ic116",  0x00000, 0x10000, CRC(ebe4bd96) SHA1(31f941e39aeaed6a64b35827df4d234cd641b47d) )
	ROM_LOAD32_BYTE( "7.ic117",  0x00001, 0x10000, CRC(3b79c0d3) SHA1(f6910f2526e1d92eae260b5eb73b1672db891f4b) )
	ROM_LOAD32_BYTE( "12.ic134", 0x00002, 0x10000, CRC(ef7712fd) SHA1(9308b37a8b024837b32d10e358a5205fdc582214) )
	ROM_LOAD32_BYTE( "11.ic135", 0x00003, 0x10000, CRC(c2d8ce0c) SHA1(6b2f3c3f5f238dc00501646230dc8787dd862ed4) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "10.ic101", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD16_BYTE( "9.ic102",  0x00001, 0x10000, CRC(29708e34) SHA1(6bea1527ad941fbb1abfad59ef3d78900dcd7f27) )

	ROM_REGION( 0x800, "plds", 0) // protected
	ROM_LOAD( "palce16v8.ic67", 0x000, 0x117, NO_DUMP )
	ROM_LOAD( "palce16v8.ic91", 0x200, 0x117, NO_DUMP )
	ROM_LOAD( "gal16v8.ic24",   0x400, 0x117, NO_DUMP )
	ROM_LOAD( "gal16v8.ic29",   0x600, 0x117, NO_DUMP )
ROM_END

ROM_START( klax4 )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075-5006.3n", 0x00000, 0x10000, CRC(65eb9a31) SHA1(3f47d58fe9eb154ab14ac282919f92679b5c7922) )
	ROM_LOAD16_BYTE( "136075-5005.1n", 0x00001, 0x10000, CRC(7be27349) SHA1(79eef2b7f4a0fb6991d81f6543d5ae00de9f2452) )
	ROM_LOAD16_BYTE( "136075-4008.3k", 0x20000, 0x10000, CRC(f3c79106) SHA1(c315159020d5bc6f919c3fb975fb8b228584f88c) )
	ROM_LOAD16_BYTE( "136075-4007.1k", 0x20001, 0x10000, CRC(a23cde5d) SHA1(51afadc900524d73ff7906b003fdf801f5d1f1fd) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "136075-2010.17x", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD16_BYTE( "136075-2009.17u", 0x00001, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD16_BYTE( "136075-2012.12x", 0x20000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD16_BYTE( "136075-2011.12u", 0x20001, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "136075-2014.17y", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD16_BYTE( "136075-2013.17w", 0x00001, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x40000, "oki", 0 ) /* ADPCM data */
	ROM_LOAD( "136075-1015.14b", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1016.12b", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )

	ROM_REGION( 0x00573, "pals", 0 ) /* Lattice GAL16V8A-25LP GAL's */
	ROM_LOAD( "136075-1000.11c.bin", 0x0000, 0x0117, CRC(fb86e94a) SHA1(b16f037c49766ab734e47c8e1b16b5178809b8a3) )
	ROM_LOAD( "136075-1001.18l.bin", 0x0000, 0x0117, CRC(cd21acfe) SHA1(14bd9e2f1b50a1da550933e3fdc16e3f09b65e92) )
	ROM_LOAD( "136075-1002.8w.bin",  0x0000, 0x0117, CRC(4a7b6c44) SHA1(9579e098af3e5cd19bd14c361d3b1c5cb9047171) )
	ROM_LOAD( "136075-1003.9w.bin",  0x0000, 0x0117, CRC(72f7f904) SHA1(f792b5bcc313c5f3338a569a6f376a3ebb1eabf7) )
	ROM_LOAD( "136075-1004.6w.bin",  0x0000, 0x0117, CRC(6cd3270d) SHA1(84854b5beee539a80fc94f6e4637aa1c2543a1cb) )
ROM_END

ROM_START( klaxj4 )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075-4406.3n", 0x00000, 0x10000, CRC(fc4045ec) SHA1(58441ffeb58c1dc9ef18f3c6381eec52923ffe03) )
	ROM_LOAD16_BYTE( "136075-4405.1n", 0x00001, 0x10000, CRC(f017461a) SHA1(a0acd66a48c2a964c3e8f2bdacd94908bfc84843) )
	ROM_LOAD16_BYTE( "136075-4408.3k", 0x20000, 0x10000, CRC(23231159) SHA1(a0ac57d358078f7fbec95964a2608213f79e4b6f) )
	ROM_LOAD16_BYTE( "136075-4407.1k", 0x20001, 0x10000, CRC(8d8158b2) SHA1(299570f16a6019c34f210bffe39ff8489f3f11f1) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "136075-2010.17x", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD16_BYTE( "136075-2009.17u", 0x00001, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD16_BYTE( "136075-2012.12x", 0x20000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD16_BYTE( "136075-2011.12u", 0x20001, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "136075-2014.17y", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD16_BYTE( "136075-2013.17w", 0x00001, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x40000, "oki", 0 ) /* ADPCM data */
	ROM_LOAD( "136075-1015.14b", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1016.12b", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )

	ROM_REGION( 0x00573, "pals", 0 ) /* Lattice GAL16V8A-25LP GAL's */
	ROM_LOAD( "136075-1000.11c.bin", 0x0000, 0x0117, CRC(fb86e94a) SHA1(b16f037c49766ab734e47c8e1b16b5178809b8a3) )
	ROM_LOAD( "136075-1001.18l.bin", 0x0000, 0x0117, CRC(cd21acfe) SHA1(14bd9e2f1b50a1da550933e3fdc16e3f09b65e92) )
	ROM_LOAD( "136075-1002.8w.bin",  0x0000, 0x0117, CRC(4a7b6c44) SHA1(9579e098af3e5cd19bd14c361d3b1c5cb9047171) )
	ROM_LOAD( "136075-1003.9w.bin",  0x0000, 0x0117, CRC(72f7f904) SHA1(f792b5bcc313c5f3338a569a6f376a3ebb1eabf7) )
	ROM_LOAD( "136075-1004.6w.bin",  0x0000, 0x0117, CRC(6cd3270d) SHA1(84854b5beee539a80fc94f6e4637aa1c2543a1cb) )
ROM_END

ROM_START( klaxj3 )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075-3406.3n", 0x00000, 0x10000, CRC(ab2aa50b) SHA1(0ebffc8b4724eb8c4423e0b1f62b0fff7cc30aab) )
	ROM_LOAD16_BYTE( "136075-3405.1n", 0x00001, 0x10000, CRC(9dc9a590) SHA1(4c77b1ad9c083325f33520f2b6aa598dde247ad8) )
	ROM_LOAD16_BYTE( "136075-2408.3k", 0x20000, 0x10000, CRC(89d515ce) SHA1(4991b859a53f34776671f660dbdb18a746259549) )
	ROM_LOAD16_BYTE( "136075-2407.1k", 0x20001, 0x10000, CRC(48ce4edb) SHA1(014f879298408295a338c19c2d518524b41491cb) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "136075-2010.17x", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD16_BYTE( "136075-2009.17u", 0x00001, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD16_BYTE( "136075-2012.12x", 0x20000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD16_BYTE( "136075-2011.12u", 0x20001, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "136075-2014.17y", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD16_BYTE( "136075-2013.17w", 0x00001, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x40000, "oki", 0 ) /* ADPCM data */
	ROM_LOAD( "136075-1015.14b", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1016.12b", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )

	ROM_REGION( 0x00573, "pals", 0 ) /* Lattice GAL16V8A-25LP GAL's */
	ROM_LOAD( "136075-1000.11c.bin", 0x0000, 0x0117, CRC(fb86e94a) SHA1(b16f037c49766ab734e47c8e1b16b5178809b8a3) )
	ROM_LOAD( "136075-1001.18l.bin", 0x0000, 0x0117, CRC(cd21acfe) SHA1(14bd9e2f1b50a1da550933e3fdc16e3f09b65e92) )
	ROM_LOAD( "136075-1002.8w.bin",  0x0000, 0x0117, CRC(4a7b6c44) SHA1(9579e098af3e5cd19bd14c361d3b1c5cb9047171) )
	ROM_LOAD( "136075-1003.9w.bin",  0x0000, 0x0117, CRC(72f7f904) SHA1(f792b5bcc313c5f3338a569a6f376a3ebb1eabf7) )
	ROM_LOAD( "136075-1004.6w.bin",  0x0000, 0x0117, CRC(6cd3270d) SHA1(84854b5beee539a80fc94f6e4637aa1c2543a1cb) )
ROM_END


ROM_START( klaxd2 )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136075-2206.3n", 0x00000, 0x10000, CRC(9d1a713b) SHA1(6e60a43934bd8959c5c07dd12e087c63ea791bb9) )
	ROM_LOAD16_BYTE( "136075-1205.1n", 0x00001, 0x10000, CRC(45065a5a) SHA1(77339ca04e54a04489ce9d6e11816475e57d1311) )
	ROM_LOAD16_BYTE( "136075-1208.3k", 0x20000, 0x10000, CRC(b4019b32) SHA1(83fba82a9100af14cddd812be9f3dbd58d8511d2) )
	ROM_LOAD16_BYTE( "136075-1207.1k", 0x20001, 0x10000, CRC(14550a75) SHA1(35599a339e6978682a09db4fb78c76bb3d3b6bc7) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "136075-2010.17x", 0x00000, 0x10000, CRC(15290a0d) SHA1(e1338f3fb298aae19735548f4b597d1c33944960) )
	ROM_LOAD16_BYTE( "136075-2009.17u", 0x00001, 0x10000, CRC(6368dbaf) SHA1(fa8b5cf6777108c0b1e38a3650ee4cdb2ec76810) )
	ROM_LOAD16_BYTE( "136075-2012.12x", 0x20000, 0x10000, CRC(c0d9eb0f) SHA1(aa68b9ad435eeaa8b43693e237cc7f9a53d94dfc) )
	ROM_LOAD16_BYTE( "136075-2011.12u", 0x20001, 0x10000, CRC(e83cca91) SHA1(45f1155d51ab3e2cc08aad1ec4e557d132085cc6) )

	ROM_REGION( 0x20000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "136075-2014.17y", 0x00000, 0x10000, CRC(5c551e92) SHA1(cbff8fc4f4d370b6db2b4953ecbedd249916b891) )
	ROM_LOAD16_BYTE( "136075-2013.17w", 0x00001, 0x10000, CRC(36764bbc) SHA1(5762996a327b5f7f93f42dad7eccb6297b3e4c0b) )

	ROM_REGION( 0x40000, "oki", 0 ) /* ADPCM data */
	ROM_LOAD( "136075-1015.14b", 0x00000, 0x10000, CRC(4d24c768) SHA1(da102105a4d8c552e3594b8ffb1903ecbaa69415) )
	ROM_LOAD( "136075-1016.12b", 0x10000, 0x10000, CRC(12e9b4b7) SHA1(2447f116cd865e46e61022143a2668beca99d5d1) )

	ROM_REGION( 0x00573, "pals", 0 ) /* Lattice GAL16V8A-25LP GAL's */
	ROM_LOAD( "136075-1000.11c.bin", 0x0000, 0x0117, CRC(fb86e94a) SHA1(b16f037c49766ab734e47c8e1b16b5178809b8a3) )
	ROM_LOAD( "136075-1001.18l.bin", 0x0000, 0x0117, CRC(cd21acfe) SHA1(14bd9e2f1b50a1da550933e3fdc16e3f09b65e92) )
	ROM_LOAD( "136075-1002.8w.bin",  0x0000, 0x0117, CRC(4a7b6c44) SHA1(9579e098af3e5cd19bd14c361d3b1c5cb9047171) )
	ROM_LOAD( "136075-1003.9w.bin",  0x0000, 0x0117, CRC(72f7f904) SHA1(f792b5bcc313c5f3338a569a6f376a3ebb1eabf7) )
	ROM_LOAD( "136075-1004.6w.bin",  0x0000, 0x0117, CRC(6cd3270d) SHA1(84854b5beee539a80fc94f6e4637aa1c2543a1cb) )
ROM_END


/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1989, klax,     0,    klax,    klax, klax_state, empty_init, ROT0, "Atari Games", "Klax (version 6)", 0 )
GAME( 1989, klax5,    klax, klax,    klax, klax_state, empty_init, ROT0, "Atari Games", "Klax (version 5)", 0 )
GAME( 1989, klax4,    klax, klax,    klax, klax_state, empty_init, ROT0, "Atari Games", "Klax (version 4)", 0 )
GAME( 1989, klaxj4,   klax, klax,    klax, klax_state, empty_init, ROT0, "Atari Games", "Klax (Japan, version 4)", 0 )
GAME( 1989, klaxj3,   klax, klax,    klax, klax_state, empty_init, ROT0, "Atari Games", "Klax (Japan, version 3)", 0 )
GAME( 1989, klaxd2,   klax, klax,    klax, klax_state, empty_init, ROT0, "Atari Games", "Klax (Germany, version 2)", 0 )

GAME( 1989, klax5bl,  klax, klax2bl, klax, klax_state, empty_init, ROT0, "bootleg",     "Klax (version 5, bootleg set 1)", MACHINE_NOT_WORKING )
GAME( 1989, klax5bl2, klax, klax2bl, klax, klax_state, empty_init, ROT0, "bootleg",     "Klax (version 5, bootleg set 2)", MACHINE_NOT_WORKING )
