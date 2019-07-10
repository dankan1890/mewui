// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/***************************************************************************

    Time Pilot

    driver by Nicola Salmoria

****************************************************************************

    memory map (preliminary)

    Main processor memory map.
    0000-5fff ROM
    a000-a3ff Color RAM
    a400-a7ff Video RAM
    a800-afff RAM
    b000-b7ff sprite RAM (only areas 0xb010 and 0xb410 are used).

    memory mapped ports:

    read:
    c000      video scan line. This is used by the program to multiplex the cloud
              sprites, drawing them twice offset by 128 pixels.
    c200      DSW2
    c300      IN0
    c320      IN1
    c340      IN2
    c360      DSW1

    write:
    c000      command for the audio CPU
    c200      watchdog reset
    c300      interrupt enable
    c302      flip screen
    c304      trigger interrupt on audio CPU
    c308      video enable (?). Protection ??? Stuffs in some values computed
              from ROM content
    c30a      coin counter 1
    c30c      coin counter 2

    interrupts:
    standard NMI at 0x66

    SOUND BOARD:
    same as Pooyan

***************************************************************************/

#include "emu.h"
#include "includes/timeplt.h"
#include "includes/konamipt.h"
#include "audio/timeplt.h"

#include "cpu/z80/z80.h"
#include "machine/watchdog.h"
#include "sound/ay8910.h"

#define MASTER_CLOCK         XTAL(18'432'000)

/*************************************
 *
 *  Interrupts
 *
 *************************************/

WRITE_LINE_MEMBER(timeplt_state::vblank_irq)
{
	if (state && m_nmi_enable)
		m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}


WRITE_LINE_MEMBER(timeplt_state::nmi_enable_w)
{
	m_nmi_enable = state;
	if (!m_nmi_enable)
		m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}



/*************************************
 *
 *  I/O
 *
 *************************************/

WRITE_LINE_MEMBER(timeplt_state::coin_counter_1_w)
{
	machine().bookkeeping().coin_counter_w(0, state);
}

WRITE_LINE_MEMBER(timeplt_state::coin_counter_2_w)
{
	machine().bookkeeping().coin_counter_w(1, state);
}

READ8_MEMBER(timeplt_state::psurge_protection_r)
{
	return 0x80;
}

// chkun has access to an extra soundchip via ay2 port a
WRITE8_MEMBER(timeplt_state::chkun_sound_w)
{
	// d0-d3: P0-P3
	// d5: /R (unused?)
	// d6: /W
	if (~data & 0x40)
		m_tc8830f->write_p(data & 0xf);

	// d4 (or d7?): /ACL
	if (~data & 0x10)
		m_tc8830f->reset();
}

CUSTOM_INPUT_MEMBER(timeplt_state::chkun_hopper_status_r)
{
	// temp workaround, needs hopper
	return machine().rand();
}



/*************************************
 *
 *  Memory maps
 *
 *************************************/

void timeplt_state::timeplt_main_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x5fff).rom();
	map(0xa000, 0xa3ff).ram().w(FUNC(timeplt_state::colorram_w)).share("colorram");
	map(0xa400, 0xa7ff).ram().w(FUNC(timeplt_state::videoram_w)).share("videoram");
	map(0xa800, 0xafff).ram();
	map(0xb000, 0xb0ff).mirror(0x0b00).ram().share("spriteram");
	map(0xb400, 0xb4ff).mirror(0x0b00).ram().share("spriteram2");
	map(0xc000, 0xc000).mirror(0x0cff).r(FUNC(timeplt_state::scanline_r)).w("timeplt_audio", FUNC(timeplt_audio_device::sound_data_w));
	map(0xc200, 0xc200).mirror(0x0cff).portr("DSW1").w("watchdog", FUNC(watchdog_timer_device::reset_w));
	map(0xc300, 0xc300).mirror(0x0c9f).portr("IN0");
	map(0xc300, 0xc30f).lw8("mainlatch_w", [this](offs_t offset, u8 data) { m_mainlatch->write_d0(offset >> 1, data); });
	map(0xc320, 0xc320).mirror(0x0c9f).portr("IN1");
	map(0xc340, 0xc340).mirror(0x0c9f).portr("IN2");
	map(0xc360, 0xc360).mirror(0x0c9f).portr("DSW0");
}

void timeplt_state::psurge_main_map(address_map &map)
{
	timeplt_main_map(map);
	map(0x6004, 0x6004).r(FUNC(timeplt_state::psurge_protection_r));
}

void timeplt_state::chkun_main_map(address_map &map)
{
	timeplt_main_map(map);
	map(0x6000, 0x67ff).ram();
}



/*************************************
 *
 *  Port definitions
 *
 *************************************/

static INPUT_PORTS_START( timeplt )
	PORT_START("IN0")
	KONAMI8_SYSTEM_UNK

	PORT_START("IN1")
	KONAMI8_MONO_B1_UNK

	PORT_START("IN2")
	KONAMI8_COCKTAIL_B1_UNK

	PORT_START("DSW0")
	KONAMI_COINAGE_LOC(DEF_STR( Free_Play ), DEF_STR( Free_Play ), SW1)

	PORT_START("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )        PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x00, "255 (Cheat)")
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )      PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Bonus_Life ) )   PORT_DIPLOCATION("SW2:4")
	PORT_DIPSETTING(    0x08, "10000 50000" )
	PORT_DIPSETTING(    0x00, "20000 60000" )
	PORT_DIPNAME( 0x70, 0x40, DEF_STR( Difficulty ) )   PORT_DIPLOCATION("SW2:5,6,7")
	PORT_DIPSETTING(    0x70, "1 (Easiest)" )
	PORT_DIPSETTING(    0x60, "2" )
	PORT_DIPSETTING(    0x50, "3" )
	PORT_DIPSETTING(    0x40, "4" )
	PORT_DIPSETTING(    0x30, "5" )
	PORT_DIPSETTING(    0x20, "6" )
	PORT_DIPSETTING(    0x10, "7" )
	PORT_DIPSETTING(    0x00, "8 (Difficult)" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( psurge )
	PORT_INCLUDE(timeplt)

	PORT_MODIFY("DSW0")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Initial Energy" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPSETTING(    0x08, "6" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x30, "3" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPSETTING(    0x10, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x10, 0x10, "Infinite Shots (Cheat)")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "Stop at Junctions" )
	PORT_DIPSETTING(    0x80, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
INPUT_PORTS_END

static INPUT_PORTS_START( chkun )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_GAMBLE_KEYOUT )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Bet 3B")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, timeplt_state, chkun_hopper_status_r, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Bet 1B")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Bet 2B")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Bet HR")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW0") // 12m
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x01, "SW1:1" )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x02, "SW1:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "SW1:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW1:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x10, "SW1:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "SW1:6" )
	PORT_SERVICE_DIPLOC( 0x40, IP_ACTIVE_LOW, "SW1:7" )
	PORT_DIPNAME( 0x80, 0x00, "Freeze" )            PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START("DSW1") // 13m
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Coin_A ) )   PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x02, "SW2:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "SW2:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW2:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x10, "SW2:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "SW2:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x40, 0x40, "SW2:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x80, 0x80, "SW2:8" )
INPUT_PORTS_END

static INPUT_PORTS_START( bikkuric )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_GAMBLE_KEYOUT )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, timeplt_state, chkun_hopper_status_r, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW0") // 12m
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x01, "SW1:1" )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x02, "SW1:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "SW1:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW1:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x10, "SW1:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "SW1:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x40, 0x40, "SW1:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x80, 0x80, "SW1:8" )

	PORT_START("DSW1") // 13m
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Coin_A ) )   PORT_DIPLOCATION("SW2:1")
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x02, 0x02, "SW2:2" )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "SW2:3" )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW2:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x10, "SW2:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "SW2:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x40, 0x40, "SW2:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x80, 0x80, "SW2:8" )
INPUT_PORTS_END



/*************************************
 *
 *  Graphics layouts
 *
 *************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 4, 0 },
	{ STEP4(0,1), STEP4(8*8,1) },
	{ STEP8(0,8) },
	16*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	2,
	{ 4, 0 },
	{ STEP4(0,1), STEP4(8*8,1), STEP4(16*8,1), STEP4(24*8,1) },
	{ STEP8(0,8), STEP8(32*8,8) },
	64*8
};


static GFXDECODE_START( gfx_timeplt )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,        0, 32 )
	GFXDECODE_ENTRY( "gfx2", 0, spritelayout,   32*4, 64 )
GFXDECODE_END

static const gfx_layout chkun_spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	2,
	{ 0, 4 },
	{ STEP4(0,1), STEP4(8*8,1), STEP4(16*8,1), STEP4(24*8,1) },
	{ STEP8(0,8), STEP8(32*8,8) },
	64*8
};

static GFXDECODE_START( gfx_chkun )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,        0, 32 )
	GFXDECODE_ENTRY( "gfx2", 0, chkun_spritelayout,   32*4, 64 )
GFXDECODE_END

/*************************************
 *
 *  Machine drivers
 *
 *************************************/

void timeplt_state::machine_start()
{
	save_item(NAME(m_nmi_enable));
}

void timeplt_state::machine_reset()
{
}

void timeplt_state::timeplt(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, MASTER_CLOCK/3/2);  /* not confirmed, but common for Konami games of the era */
	m_maincpu->set_addrmap(AS_PROGRAM, &timeplt_state::timeplt_main_map);

	LS259(config, m_mainlatch); // B3
	m_mainlatch->q_out_cb<0>().set(FUNC(timeplt_state::nmi_enable_w));
	m_mainlatch->q_out_cb<1>().set(FUNC(timeplt_state::flipscreen_w));
	m_mainlatch->q_out_cb<2>().set("timeplt_audio", FUNC(timeplt_audio_device::sh_irqtrigger_w));
	m_mainlatch->q_out_cb<3>().set("timeplt_audio", FUNC(timeplt_audio_device::mute_w));
	m_mainlatch->q_out_cb<4>().set(FUNC(timeplt_state::video_enable_w));
	m_mainlatch->q_out_cb<5>().set(FUNC(timeplt_state::coin_counter_1_w));
	m_mainlatch->q_out_cb<6>().set(FUNC(timeplt_state::coin_counter_2_w));
	m_mainlatch->q_out_cb<7>().set_nop(); // PAY OUT - not used

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_SCANLINE);
	m_screen->set_refresh_hz(60);
	m_screen->set_size(32*8, 32*8);
	m_screen->set_visarea(0*8, 32*8-1, 2*8, 30*8-1);
	m_screen->set_screen_update(FUNC(timeplt_state::screen_update));
	m_screen->set_palette(m_palette);
	m_screen->screen_vblank().set(FUNC(timeplt_state::vblank_irq));

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_timeplt);
	PALETTE(config, m_palette, FUNC(timeplt_state::timeplt_palette), 32*4 + 64*4);

	/* sound hardware */
	TIMEPLT_AUDIO(config, "timeplt_audio");
}

void timeplt_state::psurge(machine_config &config)
{
	timeplt(config);

	/* basic machine hardware */
	m_maincpu->set_addrmap(AS_PROGRAM, &timeplt_state::psurge_main_map);

	m_screen->screen_vblank().set_inputline("maincpu", INPUT_LINE_NMI);

	m_mainlatch->q_out_cb<0>().set_nop();
	m_mainlatch->q_out_cb<4>().set_nop();
	m_mainlatch->q_out_cb<5>().set_nop();
	m_mainlatch->q_out_cb<6>().set_nop();

	MCFG_VIDEO_START_OVERRIDE(timeplt_state,psurge)
}

void timeplt_state::bikkuric(machine_config &config)
{
	timeplt(config);

	m_gfxdecode->set_info(gfx_chkun);

	/* basic machine hardware */
	m_maincpu->set_addrmap(AS_PROGRAM, &timeplt_state::chkun_main_map);

	MCFG_VIDEO_START_OVERRIDE(timeplt_state,chkun)
}

void timeplt_state::chkun(machine_config &config)
{
	bikkuric(config);

	m_gfxdecode->set_info(gfx_chkun);

	/* sound hardware */
	subdevice<ay8910_device>("timeplt_audio:ay2")->port_a_write_callback().set(FUNC(timeplt_state::chkun_sound_w));

	TC8830F(config, m_tc8830f, XTAL(512'000));
	m_tc8830f->add_route(ALL_OUTPUTS, "timeplt_audio:mono", 0.10);
}



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( timeplt )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tm1",          0x0000, 0x2000, CRC(1551f1b9) SHA1(c72f30988ac00cbe6549b71c3bcb414511e8b997) )
	ROM_LOAD( "tm2",          0x2000, 0x2000, CRC(58636cb5) SHA1(ab517efa93ae7be780af55faea82a6e83edd828c) )
	ROM_LOAD( "tm3",          0x4000, 0x2000, CRC(ff4e0d83) SHA1(ef98a1abb45b22d7498a0aca520f43bbee248b22) )

	ROM_REGION( 0x10000, "timeplt_audio:tpsound", 0 )
	ROM_LOAD( "tm7",          0x0000, 0x1000, CRC(d66da813) SHA1(408fca4515e8af84211df3e204c8776b2f8adb23) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "tm6",          0x0000, 0x2000, CRC(c2507f40) SHA1(07221875e3f81d9def67c57a7ccd82d52ce65e01) )

	ROM_REGION( 0x4000, "gfx2", 0 )
	ROM_LOAD( "tm4",          0x0000, 0x2000, CRC(7e437c3e) SHA1(cbe2ccd2cd503af62f009cd5aab73aa7366230b1) )
	ROM_LOAD( "tm5",          0x2000, 0x2000, CRC(e8ca87b9) SHA1(5dd30d3fb9fd8cf9e6a8e37e7ea858c7fd038a7e) )

	ROM_REGION( 0x0240, "proms", 0 )
	ROM_LOAD( "timeplt.b4",   0x0000, 0x0020, CRC(34c91839) SHA1(f62e279e21fce171231d3139be7adabe1f4b8c2e) ) /* palette */
	ROM_LOAD( "timeplt.b5",   0x0020, 0x0020, CRC(463b2b07) SHA1(9ad275365eba4869f94749f39ff8705d92056a10) ) /* palette */
	ROM_LOAD( "timeplt.e9",   0x0040, 0x0100, CRC(4bbb2150) SHA1(678433b21aae1daa938e32d3293eeed529a42ef9) ) /* sprite lookup table */
	ROM_LOAD( "timeplt.e12",  0x0140, 0x0100, CRC(f7b7663e) SHA1(151bd2dff4e4ef76d6438c1ab2cae71f987b9dad) ) /* char lookup table */
ROM_END

ROM_START( timeplta )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cd_e1.bin",         0x0000, 0x2000, CRC(a4513b35) SHA1(1b1944ec5317d71af86e21e0691caae180dee7b5) )
	ROM_LOAD( "cd_e2.bin",         0x2000, 0x2000, CRC(38b0c72a) SHA1(8f0950deb2f9e2b65714318b9e837a1c837f52a9) )
	ROM_LOAD( "cd_e3.bin",         0x4000, 0x2000, CRC(83846870) SHA1(b1741e7e5674f9e63e113ead0cb7f5ef874eac5f) )

	ROM_REGION( 0x10000, "timeplt_audio:tpsound", 0 )
	ROM_LOAD( "tm7",          0x0000, 0x1000, CRC(d66da813) SHA1(408fca4515e8af84211df3e204c8776b2f8adb23) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "tm6",          0x0000, 0x2000, CRC(c2507f40) SHA1(07221875e3f81d9def67c57a7ccd82d52ce65e01) )

	ROM_REGION( 0x4000, "gfx2", 0 )
	ROM_LOAD( "tm4",          0x0000, 0x2000, CRC(7e437c3e) SHA1(cbe2ccd2cd503af62f009cd5aab73aa7366230b1) )
	ROM_LOAD( "tm5",          0x2000, 0x2000, CRC(e8ca87b9) SHA1(5dd30d3fb9fd8cf9e6a8e37e7ea858c7fd038a7e) )

	ROM_REGION( 0x0240, "proms", 0 )
	ROM_LOAD( "timeplt.b4",   0x0000, 0x0020, CRC(34c91839) SHA1(f62e279e21fce171231d3139be7adabe1f4b8c2e) ) /* palette */
	ROM_LOAD( "timeplt.b5",   0x0020, 0x0020, CRC(463b2b07) SHA1(9ad275365eba4869f94749f39ff8705d92056a10) ) /* palette */
	ROM_LOAD( "timeplt.e9",   0x0040, 0x0100, CRC(4bbb2150) SHA1(678433b21aae1daa938e32d3293eeed529a42ef9) ) /* sprite lookup table */
	ROM_LOAD( "timeplt.e12",  0x0140, 0x0100, CRC(f7b7663e) SHA1(151bd2dff4e4ef76d6438c1ab2cae71f987b9dad) ) /* char lookup table */
ROM_END

ROM_START( timepltc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cd1y",         0x0000, 0x2000, CRC(83ec72c2) SHA1(f3dbc8362f6bdad1baa65cf5d95611e79de381a4) )
	ROM_LOAD( "cd2y",         0x2000, 0x2000, CRC(0dcf5287) SHA1(c36628367e81ac07f5ace72b45ebb7140b6aa116) )
	ROM_LOAD( "cd3y",         0x4000, 0x2000, CRC(c789b912) SHA1(dead7b20a40769e48738fccc3a17e2266aac445d) )

	ROM_REGION( 0x10000, "timeplt_audio:tpsound", 0 )
	ROM_LOAD( "tm7",          0x0000, 0x1000, CRC(d66da813) SHA1(408fca4515e8af84211df3e204c8776b2f8adb23) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "tm6",          0x0000, 0x2000, CRC(c2507f40) SHA1(07221875e3f81d9def67c57a7ccd82d52ce65e01) )

	ROM_REGION( 0x4000, "gfx2", 0 )
	ROM_LOAD( "tm4",          0x0000, 0x2000, CRC(7e437c3e) SHA1(cbe2ccd2cd503af62f009cd5aab73aa7366230b1) )
	ROM_LOAD( "tm5",          0x2000, 0x2000, CRC(e8ca87b9) SHA1(5dd30d3fb9fd8cf9e6a8e37e7ea858c7fd038a7e) )

	ROM_REGION( 0x0240, "proms", 0 )
	ROM_LOAD( "timeplt.b4",   0x0000, 0x0020, CRC(34c91839) SHA1(f62e279e21fce171231d3139be7adabe1f4b8c2e) ) /* palette */
	ROM_LOAD( "timeplt.b5",   0x0020, 0x0020, CRC(463b2b07) SHA1(9ad275365eba4869f94749f39ff8705d92056a10) ) /* palette */
	ROM_LOAD( "timeplt.e9",   0x0040, 0x0100, CRC(4bbb2150) SHA1(678433b21aae1daa938e32d3293eeed529a42ef9) ) /* sprite lookup table */
	ROM_LOAD( "timeplt.e12",  0x0140, 0x0100, CRC(f7b7663e) SHA1(151bd2dff4e4ef76d6438c1ab2cae71f987b9dad) ) /* char lookup table */
ROM_END

ROM_START( spaceplt )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sp1",          0x0000, 0x2000, CRC(ac8ca3ae) SHA1(9781138becd17aa70e877138e126ebb1fbff6192) )
	ROM_LOAD( "sp2",          0x2000, 0x2000, CRC(1f0308ef) SHA1(dd88378fc4cefe473f310d4730268c98354a4a44) )
	ROM_LOAD( "sp3",          0x4000, 0x2000, CRC(90aeca50) SHA1(9c6fddfeafa84f5284ec8f7c9d46216b110badc1) )

	ROM_REGION( 0x10000, "timeplt_audio:tpsound", 0 )
	ROM_LOAD( "tm7",          0x0000, 0x1000, CRC(d66da813) SHA1(408fca4515e8af84211df3e204c8776b2f8adb23) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "sp6",          0x0000, 0x2000, CRC(76caa8af) SHA1(f81bb73877d415a6587a32bddaad6db8a8fd4941) )

	ROM_REGION( 0x4000, "gfx2", 0 )
	ROM_LOAD( "sp4",          0x0000, 0x2000, CRC(3781ce7a) SHA1(68bb73f67494c3b24f7fd0d79153c9793f4b3a5b) )
	ROM_LOAD( "tm5",          0x2000, 0x2000, CRC(e8ca87b9) SHA1(5dd30d3fb9fd8cf9e6a8e37e7ea858c7fd038a7e) )

	ROM_REGION( 0x0240, "proms", 0 )
	ROM_LOAD( "timeplt.b4",   0x0000, 0x0020, CRC(34c91839) SHA1(f62e279e21fce171231d3139be7adabe1f4b8c2e) ) /* palette */
	ROM_LOAD( "timeplt.b5",   0x0020, 0x0020, CRC(463b2b07) SHA1(9ad275365eba4869f94749f39ff8705d92056a10) ) /* palette */
	ROM_LOAD( "timeplt.e9",   0x0040, 0x0100, CRC(4bbb2150) SHA1(678433b21aae1daa938e32d3293eeed529a42ef9) ) /* sprite lookup table */
	ROM_LOAD( "timeplt.e12",  0x0140, 0x0100, CRC(f7b7663e) SHA1(151bd2dff4e4ef76d6438c1ab2cae71f987b9dad) ) /* char lookup table */
ROM_END


ROM_START( psurge )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "p1",           0x0000, 0x2000, CRC(05f9ba12) SHA1(ad88838d1a0c64830281e425d4ad2498ba959098) )
	ROM_LOAD( "p2",           0x2000, 0x2000, CRC(3ff41576) SHA1(9bdbad31c65dff76942967b5a334407b0326f752) )
	ROM_LOAD( "p3",           0x4000, 0x2000, CRC(e8fe120a) SHA1(b6320c9cb1a67097692aa0de7d88b0dfb63dedd7) )

	ROM_REGION( 0x10000, "timeplt_audio:tpsound", 0 )
	ROM_LOAD( "p6",           0x0000, 0x1000, CRC(b52d01fa) SHA1(9b6cf9ea51d3a87c174f34d42a4b1b5f38b48723) )
	ROM_LOAD( "p7",           0x1000, 0x1000, CRC(9db5c0ce) SHA1(b5bc1d89a7f7d7a0baae64390c37ee11f69a0e76) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "p4",           0x0000, 0x2000, CRC(26fd7f81) SHA1(eb282313a37d7d611bf90f9b0b527adee9ae283f) )

	ROM_REGION( 0x4000, "gfx2", 0 )
	ROM_LOAD( "p5",           0x0000, 0x2000, CRC(6066ec8e) SHA1(7f1155cf8a2d63c0740a4b56f1e09e7dfc749302) )
	ROM_LOAD( "tm5",          0x2000, 0x2000, CRC(e8ca87b9) SHA1(5dd30d3fb9fd8cf9e6a8e37e7ea858c7fd038a7e) )

	ROM_REGION( 0x0240, "proms", 0 )
	ROM_LOAD( "timeplt.b4",   0x0000, 0x0020, BAD_DUMP CRC(34c91839) SHA1(f62e279e21fce171231d3139be7adabe1f4b8c2e)  ) /* palette */
	ROM_LOAD( "timeplt.b5",   0x0020, 0x0020, BAD_DUMP CRC(463b2b07) SHA1(9ad275365eba4869f94749f39ff8705d92056a10)  ) /* palette */
	ROM_LOAD( "timeplt.e9",   0x0040, 0x0100, BAD_DUMP CRC(4bbb2150) SHA1(678433b21aae1daa938e32d3293eeed529a42ef9)  ) /* sprite lookup table */
	ROM_LOAD( "timeplt.e12",  0x0140, 0x0100, BAD_DUMP CRC(f7b7663e) SHA1(151bd2dff4e4ef76d6438c1ab2cae71f987b9dad)  ) /* char lookup table */
ROM_END

ROM_START( chkun )
	ROM_REGION( 0x12000, "maincpu", 0 )
	ROM_LOAD( "n1.16a",   0x0000, 0x4000, CRC(c5879f9b) SHA1(68e3a87dfe6b3d1e0cdadd1ed8ad115a9d3055f9) )
	ROM_LOAD( "12.14a",   0x4000, 0x2000, CRC(80cc55da) SHA1(68727721479624cd0d38d895b98dcef4edac13e9) )

	ROM_REGION( 0x12000, "timeplt_audio:tpsound", 0 )
	ROM_LOAD( "15.3l",    0x0000, 0x2000, CRC(1f1463ca) SHA1(870abbca35236fcce6a2f640a238e20b9e57f10f))

	ROM_REGION( 0x4000, "gfx1", 0 )
	ROM_LOAD( "13.4d",   0x0000, 0x4000, CRC(776427c0) SHA1(1e8387685f7e86aad31577f2186596b2a2dfc4de) )

	ROM_REGION( 0x4000, "gfx2", 0 )
	ROM_LOAD( "14.8h",   0x0000, 0x4000, CRC(0cb76a48) SHA1(0beebbc3d30eb978f6fe7b15c0a7b0c2152815b7) )

	ROM_REGION( 0x20000, "tc8830f", 0 )
	ROM_LOAD( "v1.8k",   0x00000, 0x10000, CRC(d5ca802d) SHA1(0c2867c86132745063e36d03f41e6c3e150fd3ad) )
	ROM_LOAD( "v2.9k",   0x10000, 0x10000, CRC(70e902eb) SHA1(e1b2392446f2878f9e811a63bbbbdc56fd517a9c) )

	ROM_REGION( 0x0240, "proms", 0 )
	ROM_LOAD( "3.2j",        0x0000, 0x0020, CRC(34c91839) SHA1(f62e279e21fce171231d3139be7adabe1f4b8c2e) ) /* palette */
	ROM_LOAD( "2.1h",        0x0020, 0x0020, CRC(463b2b07) SHA1(9ad275365eba4869f94749f39ff8705d92056a10) ) /* palette */
	ROM_LOAD( "4.10h",       0x0040, 0x0100, CRC(4bbb2150) SHA1(678433b21aae1daa938e32d3293eeed529a42ef9) ) /* sprite lookup table */
	ROM_LOAD( "mb7114e.2b",  0x0140, 0x0100, CRC(adfa399a) SHA1(3b18971ddaae7734f0aaa0fcc82d4ddccc282959) ) /* char lookup table */

	ROM_REGION( 0x0200, "pld", 0 )
	ROM_LOAD( "a.16c",  0x0000, 0x00eb, CRC(e0d54999) SHA1(9e1c749873572ade2b925ce1519d31a6e19f841f) )
	ROM_LOAD( "b.9f",   0x0100, 0x00eb, CRC(e3857f83) SHA1(674e70dc960fc02a9fbda4a0ef0770eb8214c466) )
ROM_END

ROM_START( bikkuric )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "1.a16", 0x00000, 0x04000, CRC(e8d595ab) SHA1(01f6a5321274befcd03a0ec18ed9770aca4527b6) )
	ROM_LOAD( "2.a14", 0x04000, 0x02000, CRC(63fd7d53) SHA1(b1ef666453c5c9e344bee544a0673068d60158fa) )

	ROM_REGION( 0x10000, "timeplt_audio:tpsound", 0 )
	ROM_LOAD( "5.l3",  0x00000, 0x02000, CRC(bc438531) SHA1(e19badc417b0538010cf535d3f733acc54b0cd96) )

	ROM_REGION( 0x8000, "gfx1", 0 )
	ROM_LOAD( "3.d4",  0x00000, 0x08000, CRC(74e8a64b) SHA1(b2542e1f6f4b54d8f7aec8f673cedcf5bff5e429) ) // 1st and 2nd identical, confirmed to be like this

	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "4.h8",  0x00000, 0x02000, CRC(d303942d) SHA1(688d43e6dbe505d44fc41fdde74858a02910080d) )

	ROM_REGION( 0x0240, "proms", 0 ) // not dumped, roms taken from timeplt
	ROM_LOAD( "3.2j",  0x0000, 0x0020, BAD_DUMP CRC(34c91839) SHA1(f62e279e21fce171231d3139be7adabe1f4b8c2e) ) /* palette */
	ROM_LOAD( "2.1h",  0x0020, 0x0020, BAD_DUMP CRC(463b2b07) SHA1(9ad275365eba4869f94749f39ff8705d92056a10) ) /* palette */
	ROM_LOAD( "4.10h", 0x0040, 0x0100, BAD_DUMP CRC(4bbb2150) SHA1(678433b21aae1daa938e32d3293eeed529a42ef9) ) /* sprite lookup table */
	ROM_LOAD( "1.2b",  0x0140, 0x0100, BAD_DUMP CRC(f7b7663e) SHA1(151bd2dff4e4ef76d6438c1ab2cae71f987b9dad) ) /* char lookup table */

	ROM_REGION( 0x0200, "pld", 0 )
	ROM_LOAD( "a.16c",  0x0000, 0x00eb, NO_DUMP )
	ROM_LOAD( "b.9f",   0x0100, 0x00eb, NO_DUMP )
ROM_END



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1982, timeplt,  0,       timeplt,  timeplt,  timeplt_state, empty_init, ROT90,  "Konami", "Time Pilot", MACHINE_SUPPORTS_SAVE )
GAME( 1982, timepltc, timeplt, timeplt,  timeplt,  timeplt_state, empty_init, ROT90,  "Konami (Centuri license)", "Time Pilot (Centuri)", MACHINE_SUPPORTS_SAVE )
GAME( 1982, timeplta, timeplt, timeplt,  timeplt,  timeplt_state, empty_init, ROT90,  "Konami (Atari license)", "Time Pilot (Atari)", MACHINE_SUPPORTS_SAVE )
GAME( 1982, spaceplt, timeplt, timeplt,  timeplt,  timeplt_state, empty_init, ROT90,  "bootleg", "Space Pilot", MACHINE_SUPPORTS_SAVE )

GAME( 1988, psurge,   0,       psurge,   psurge,   timeplt_state, empty_init, ROT270, "Vision Electronics", "Power Surge", MACHINE_SUPPORTS_SAVE )

GAME( 1988, chkun,    0,       chkun,    chkun,    timeplt_state, empty_init, ROT90,  "Peni", "Chance Kun (Japan)", MACHINE_SUPPORTS_SAVE | MACHINE_IMPERFECT_SOUND )
GAME( 1987, bikkuric, 0,       bikkuric, bikkuric, timeplt_state, empty_init, ROT90,  "Peni", "Bikkuri Card (Japan)", MACHINE_SUPPORTS_SAVE )
