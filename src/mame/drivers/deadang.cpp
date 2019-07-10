// license:BSD-3-Clause
// copyright-holders:Bryan McPhail, David Haywood
/***************************************************************************

    Pop'N Run                       (c) 1987 Seibu Kaihatsu & Yukai Tsukai
    Dead Angle                      (c) 1988 Seibu Kaihatsu
    Gang Hunter                     (c) 1988 Seibu Kaihatsu

***************************************************************************/

/*

    TODO:

    - ghunter trackball input is broken
    - coin lockouts
    - popnrun: inputs, can't coin it up, needs gfxs dumped and sorted out
      (SIP modules like airraid);


Lead Angle
Seibu, 1988

Clocks
------
Top board - SEI-8712A
Two crystals on top board 12MHz and 16MHz
V30(x2) - both 8MHz [16/2]

Bottom board - SEI-8712B
One crystal on bottom board 14.31818MHz
Z80 - 3.579545MHz [14.31818/4]
OKI M5205(x2) - 375kHz [12/32]
YM2203(x2) 3.579545MHz [14.31818/4]
Seibu  SEI0100 YM3931 - 3.579545MHz [14.31818/4]
VSync 60Hz
HSync 15.37kHz

Gang Hunter as an additional daughter card attached to the top board call SEI-8712 GUN

2008-08
Dip locations and factory settings verified with US manual

*/

#include "emu.h"
#include "includes/deadang.h"

#include "cpu/nec/nec.h"
#include "machine/watchdog.h"
#include "sound/2203intf.h"
#include "sound/msm5205.h"
#include "screen.h"
#include "speaker.h"


/* Read/Write Handlers */

READ16_MEMBER(deadang_state::ghunter_trackball_low_r)
{
	return (ioport("TRACKX")->read() & 0xff) | ((ioport("TRACKY")->read() & 0xff) << 8);
}
READ16_MEMBER(deadang_state::ghunter_trackball_high_r)
{
	return ((ioport("TRACKX")->read() & 0x0f00) >> 4) | (ioport("TRACKY")->read() & 0x0f00);
}

/* Memory Maps */

void deadang_state::main_map(address_map &map)
{
	map(0x00000, 0x037ff).ram();
	map(0x03800, 0x03fff).ram().share("spriteram");
	map(0x04000, 0x04fff).ram().share("share1");
	map(0x05000, 0x05fff).writeonly();
	map(0x06000, 0x0600f).rw(m_seibu_sound, FUNC(seibu_sound_device::main_r), FUNC(seibu_sound_device::main_w)).umask16(0x00ff);
	map(0x06010, 0x07fff).writeonly();
	map(0x08000, 0x087ff).w(FUNC(deadang_state::text_w)).share("videoram");
	map(0x08800, 0x0bfff).writeonly();
	map(0x0a000, 0x0a001).portr("P1_P2");
	map(0x0a002, 0x0a003).portr("DSW");
	map(0x0c000, 0x0cfff).w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x0d000, 0x0dfff).writeonly();
	map(0x0e000, 0x0e0ff).ram().share("scroll_ram");
	map(0x0e100, 0x0ffff).writeonly();
	map(0xc0000, 0xfffff).rom();
}

void popnrun_state::popnrun_main_map(address_map &map)
{
	map(0x00000, 0x03bff).ram();
	map(0x03c00, 0x03dff).ram().share("spriteram");
	map(0x03e00, 0x03fff).ram();
	map(0x04000, 0x04fff).ram().share("share1");
	map(0x05000, 0x05fff).writeonly();
	map(0x06000, 0x0600f).rw(m_seibu_sound, FUNC(seibu_sound_device::main_r), FUNC(seibu_sound_device::main_w)).umask16(0x00ff);
	map(0x06010, 0x07fff).writeonly();
	map(0x08000, 0x08fff).ram().w(FUNC(popnrun_state::popnrun_text_w)).share("videoram");
	map(0x0a000, 0x0a001).portr("P1_P2");
	map(0x0a002, 0x0a003).portr("DSW");
	map(0x0c000, 0x0cfff).w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x0d000, 0x0dfff).writeonly();
	map(0x0e000, 0x0e0ff).ram().share("scroll_ram");
	map(0x0e100, 0x0ffff).writeonly();
	map(0xc0000, 0xfffff).rom();
}

void deadang_state::sub_map(address_map &map)
{
	map(0x00000, 0x037ff).ram();
	map(0x03800, 0x03fff).ram().w(FUNC(deadang_state::foreground_w)).share("video_data");
	map(0x04000, 0x04fff).ram().share("share1");
	map(0x08000, 0x08001).w(FUNC(deadang_state::bank_w));
	map(0x0c000, 0x0c001).w("watchdog", FUNC(watchdog_timer_device::reset16_w));
	map(0xe0000, 0xfffff).rom();
}

void popnrun_state::popnrun_sub_map(address_map &map)
{
	map(0x00000, 0x003ff).ram().w(FUNC(deadang_state::foreground_w)).share("video_data");
	map(0x00400, 0x03fff).ram();
	map(0x04000, 0x04fff).ram().share("share1");
	map(0xe0000, 0xfffff).rom();
}

void deadang_state::sound_map(address_map &map)
{
	map(0x0000, 0x1fff).r("sei80bu", FUNC(sei80bu_device::data_r));
	map(0x2000, 0x27ff).ram();
	map(0x4000, 0x4000).w(m_seibu_sound, FUNC(seibu_sound_device::pending_w));
	map(0x4001, 0x4001).w(m_seibu_sound, FUNC(seibu_sound_device::irq_clear_w));
	map(0x4002, 0x4002).w(m_seibu_sound, FUNC(seibu_sound_device::rst10_ack_w));
	map(0x4003, 0x4003).w(m_seibu_sound, FUNC(seibu_sound_device::rst18_ack_w));
	map(0x4005, 0x4006).w(m_adpcm1, FUNC(seibu_adpcm_device::adr_w));
	map(0x4007, 0x4007).w(m_seibu_sound, FUNC(seibu_sound_device::bank_w));
	map(0x4008, 0x4009).rw(m_seibu_sound, FUNC(seibu_sound_device::ym_r), FUNC(seibu_sound_device::ym_w));
	map(0x4010, 0x4011).r(m_seibu_sound, FUNC(seibu_sound_device::soundlatch_r));
	map(0x4012, 0x4012).r(m_seibu_sound, FUNC(seibu_sound_device::main_data_pending_r));
	map(0x4013, 0x4013).portr("COIN");
	map(0x4018, 0x4019).w(m_seibu_sound, FUNC(seibu_sound_device::main_data_w));
	map(0x401a, 0x401a).w(m_adpcm1, FUNC(seibu_adpcm_device::ctl_w));
	map(0x401b, 0x401b).w(m_seibu_sound, FUNC(seibu_sound_device::coin_w));
	map(0x6005, 0x6006).w(m_adpcm2, FUNC(seibu_adpcm_device::adr_w));
	map(0x6008, 0x6009).rw("ym2", FUNC(ym2203_device::read), FUNC(ym2203_device::write));
	map(0x601a, 0x601a).w(m_adpcm2, FUNC(seibu_adpcm_device::ctl_w));
	map(0x8000, 0xffff).bankr("seibu_bank1");
}

// Air Raid sound config with extra ROM bank
void popnrun_state::popnrun_sound_map(address_map &map)
{
	map(0x0000, 0x1fff).r("sei80bu", FUNC(sei80bu_device::data_r));
	map(0x2000, 0x27ff).ram();
	map(0x4000, 0x4000).w(m_seibu_sound, FUNC(seibu_sound_device::pending_w));
	map(0x4001, 0x4001).w(m_seibu_sound, FUNC(seibu_sound_device::irq_clear_w));
	map(0x4002, 0x4002).w(m_seibu_sound, FUNC(seibu_sound_device::rst10_ack_w));
	map(0x4003, 0x4003).w(m_seibu_sound, FUNC(seibu_sound_device::rst18_ack_w));
	map(0x4007, 0x4007).w(m_seibu_sound, FUNC(seibu_sound_device::bank_w));
	map(0x4008, 0x4009).rw(m_seibu_sound, FUNC(seibu_sound_device::ym_r), FUNC(seibu_sound_device::ym_w));
	map(0x4010, 0x4011).r(m_seibu_sound, FUNC(seibu_sound_device::soundlatch_r));
	map(0x4012, 0x4012).r(m_seibu_sound, FUNC(seibu_sound_device::main_data_pending_r));
	map(0x4013, 0x4013).portr("COIN");
	map(0x4018, 0x4019).w(m_seibu_sound, FUNC(seibu_sound_device::main_data_w));
	map(0x401b, 0x401b).w(m_seibu_sound, FUNC(seibu_sound_device::coin_w));
	map(0x8000, 0xffff).bankr("seibu_bank1");
}

void deadang_state::sound_decrypted_opcodes_map(address_map &map)
{
	map(0x0000, 0x1fff).r("sei80bu", FUNC(sei80bu_device::opcode_r));
	map(0x8000, 0xffff).bankr("seibu_bank1");
}


/* Input Ports */

static INPUT_PORTS_START( deadang )
	SEIBU_COIN_INPUTS   /* coin inputs read through sound cpu */

	PORT_START("P1_P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x00c0, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("DSW")
	/* Dip switch A */
	PORT_DIPNAME( 0x0007, 0x0007, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(    0x0000, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x0004, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x0002, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x0006, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0007, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0003, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0005, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0001, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x0018, 0x0018, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:4,5")
	PORT_DIPSETTING(    0x0010, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0018, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0008, DEF_STR( 1C_2C ) )
	PORT_SERVICE_DIPLOC( 0x0020, IP_ACTIVE_LOW, "SW1:6" )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(    0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(    0x0080, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( Cocktail ) )
	/* Dip switch B */
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x0200, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x0300, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x0100, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(    0x0800, "20K 50K" )
	PORT_DIPSETTING(    0x0c00, "30K 100K" )
	PORT_DIPSETTING(    0x0400, "50K 150K" )
	PORT_DIPSETTING(    0x0000, "100K 200K" )
	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x2000, "1" )
	PORT_DIPSETTING(    0x1000, "2" )
	PORT_DIPSETTING(    0x3000, "3" )
	PORT_DIPSETTING(    0x0000, "4" )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(    0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x4000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, "User Mode" ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x0000, DEF_STR( Japan ) )
	PORT_DIPSETTING(    0x8000, "Overseas" )
INPUT_PORTS_END

static INPUT_PORTS_START( ghunter )
	PORT_INCLUDE( deadang )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x0400, "50K 150K" )
	PORT_DIPSETTING(    0x0000, "100K 200K" )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Controller ) ) PORT_DIPLOCATION("SW2:4")
	PORT_DIPSETTING(    0x0800, DEF_STR( Trackball ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( Joystick ) )

	PORT_START("TRACKX")
	PORT_BIT( 0x0fff, 0x0000, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_PLAYER(1)

	PORT_START("TRACKY")
	PORT_BIT( 0x0fff, 0x0000, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_PLAYER(1)
INPUT_PORTS_END

/* Graphics Layouts */

static const gfx_layout charlayout =
{
	8,8,        /* 8*8 characters */
	RGN_FRAC(1,2),
	4,          /* 4 bits per pixel */
	{ RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4, 0, 4 },
	{ STEP4(3,-1), STEP4(11,-1) },
	{ STEP8(0,16) },
	128
};

static const gfx_layout spritelayout =
{
	16,16,  /* 16*16 tiles */
	RGN_FRAC(1,1),
	4,      /* 4 bits per pixel */
	{ 8,12,0,4},
	{ STEP4(3,-1), STEP4(19,-1), STEP4(512+3,-1), STEP4(512+19,-1) },
	{ STEP16(0,32) },
	1024
};

static const gfx_layout popnrun_charlayout =
{
	8,8,        /* 8*8 characters */
	RGN_FRAC(1,1),
	2,          /* 2 bits per pixel */
	{ 4, 0 },
	{ STEP4(8,1), STEP4(0,1) },
	{ STEP8(0,16) },
	128
};

#ifdef UNUSED_DEFINITION
// TODO: this is wrong
static const gfx_layout popnrun_spritelayout =
{
	16,16,  /* 16*16 tiles */
	RGN_FRAC(1,1),
	2,      /* 4 bits per pixel */
	{ 0,4 },
	{ STEP4(0,1), STEP4(16,1), STEP4(512,1), STEP4(512+16,1) },
	{ STEP16(0,32) },
	1024
};
#endif

/* Graphics Decode Information */

static GFXDECODE_START( gfx_deadang )
	GFXDECODE_ENTRY( "gfx1", 0x000000, charlayout,    512, 16 )
	GFXDECODE_ENTRY( "gfx2", 0x000000, spritelayout,  768, 16 )
	GFXDECODE_ENTRY( "gfx3", 0x000000, spritelayout, 1024, 16 )
	GFXDECODE_ENTRY( "gfx4", 0x000000, spritelayout,  256, 16 )
	GFXDECODE_ENTRY( "gfx5", 0x000000, spritelayout,    0, 16 )
GFXDECODE_END

static GFXDECODE_START( gfx_popnrun )
	GFXDECODE_ENTRY( "gfx1", 0x000000, popnrun_charlayout,   0x20, 4 )
	// TODO: probably runs on ROM based palette or just uses the first three entries?
	GFXDECODE_ENTRY( "gfx2", 0x000000, spritelayout,    0, 8 )
	GFXDECODE_ENTRY( "gfx3", 0x000000, spritelayout, 1024, 16 )
	GFXDECODE_ENTRY( "gfx4", 0x000000, spritelayout,  256, 16 )
	GFXDECODE_ENTRY( "gfx5", 0x000000, spritelayout,    0, 16 )
GFXDECODE_END

/* Interrupt Generators */

TIMER_DEVICE_CALLBACK_MEMBER(deadang_state::main_scanline)
{
	int scanline = param;

	if(scanline == 240) // vblank-out irq
		m_maincpu->set_input_line_and_vector(0, HOLD_LINE,0xc4/4); // V30

	if(scanline == 0) // vblank-in irq
		m_maincpu->set_input_line_and_vector(0, HOLD_LINE,0xc8/4); // V30
}

TIMER_DEVICE_CALLBACK_MEMBER(deadang_state::sub_scanline)
{
	int scanline = param;

	if(scanline == 240) // vblank-out irq
		m_subcpu->set_input_line_and_vector(0, HOLD_LINE,0xc4/4); // V30

	if(scanline == 0) // vblank-in irq
		m_subcpu->set_input_line_and_vector(0, HOLD_LINE,0xc8/4); // V30
}

/* Machine Drivers */

void deadang_state::deadang(machine_config &config)
{
	/* basic machine hardware */
	V30(config, m_maincpu, XTAL(16'000'000)/2); /* Sony 8623h9 CXQ70116D-8 (V30 compatible) */
	m_maincpu->set_addrmap(AS_PROGRAM, &deadang_state::main_map);
	TIMER(config, "scantimer1").configure_scanline(FUNC(deadang_state::main_scanline), "screen", 0, 1);

	V30(config, m_subcpu, XTAL(16'000'000)/2); /* Sony 8623h9 CXQ70116D-8 (V30 compatible) */
	m_subcpu->set_addrmap(AS_PROGRAM, &deadang_state::sub_map);
	TIMER(config, "scantimer2").configure_scanline(FUNC(deadang_state::sub_scanline), "screen", 0, 1);

	Z80(config, m_audiocpu, XTAL(14'318'181)/4);
	m_audiocpu->set_addrmap(AS_PROGRAM, &deadang_state::sound_map);
	m_audiocpu->set_addrmap(AS_OPCODES, &deadang_state::sound_decrypted_opcodes_map);
	m_audiocpu->set_irq_acknowledge_callback("seibu_sound", FUNC(seibu_sound_device::im0_vector_cb));

	SEI80BU(config, "sei80bu", 0).set_device_rom_tag("audiocpu");

	config.m_minimum_quantum = attotime::from_hz(60); // the game stops working with higher interleave rates..

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	m_screen->set_size(32*8, 32*8);
	m_screen->set_visarea(0*8, 32*8-1, 2*8, 30*8-1);
	m_screen->set_screen_update(FUNC(deadang_state::screen_update));
	m_screen->set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_deadang);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_444, 2048);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	SEIBU_SOUND(config, m_seibu_sound, 0);
	m_seibu_sound->int_callback().set_inputline(m_audiocpu, 0);
	m_seibu_sound->set_rom_tag("audiocpu");
	m_seibu_sound->set_rombank_tag("seibu_bank1");
	m_seibu_sound->ym_read_callback().set("ym1", FUNC(ym2203_device::read));
	m_seibu_sound->ym_write_callback().set("ym1", FUNC(ym2203_device::write));

	ym2203_device &ym1(YM2203(config, "ym1", XTAL(14'318'181)/4));
	ym1.irq_handler().set("seibu_sound", FUNC(seibu_sound_device::fm_irqhandler));
	ym1.add_route(ALL_OUTPUTS, "mono", 0.15);

	ym2203_device &ym2(YM2203(config, "ym2", XTAL(14'318'181)/4));
	ym2.add_route(ALL_OUTPUTS, "mono", 0.15);

	SEIBU_ADPCM(config, m_adpcm1, 8000).add_route(ALL_OUTPUTS, "mono", 0.40);

	SEIBU_ADPCM(config, m_adpcm2, 8000).add_route(ALL_OUTPUTS, "mono", 0.40);
}

void popnrun_state::popnrun(machine_config &config)
{
	deadang(config);

	m_maincpu->set_addrmap(AS_PROGRAM, &popnrun_state::popnrun_main_map);

	m_subcpu->set_addrmap(AS_PROGRAM, &popnrun_state::popnrun_sub_map);

	m_audiocpu->set_addrmap(AS_PROGRAM, &popnrun_state::popnrun_sound_map);
	m_audiocpu->set_addrmap(AS_OPCODES, &popnrun_state::sound_decrypted_opcodes_map);

	m_screen->set_screen_update(FUNC(popnrun_state::popnrun_screen_update));

	config.device_remove("watchdog");

	m_gfxdecode->set_info(gfx_popnrun);

	config.device_remove("ym1");
	config.device_remove("ym2");
	config.device_remove("adpcm1");
	config.device_remove("adpcm2");

	m_seibu_sound->ym_read_callback().set("ymsnd", FUNC(ym2151_device::read));
	m_seibu_sound->ym_write_callback().set("ymsnd", FUNC(ym2151_device::write));

	ym2151_device &ymsnd(YM2151(config, "ymsnd", XTAL(14'318'181)/4));
	ymsnd.irq_handler().set(m_seibu_sound, FUNC(seibu_sound_device::fm_irqhandler));
	ymsnd.add_route(0, "mono", 0.50);
	ymsnd.add_route(1, "mono", 0.50);
}


/* ROMs */
ROM_START( popnrun )
	ROM_REGION( 0x100000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "popnrun-27512-1-6e.bin", 0xe0001, 0x010000, CRC(cf800494) SHA1(eaed51212c91ebb16e326f8133b60a0ecf0055e5) )
	ROM_LOAD16_BYTE( "popnrun-27512-2-6h.bin", 0xe0000, 0x010000, CRC(bad47dc4) SHA1(279236cfe5102b4724f9fb4405f514dba011ae3d) )

	ROM_REGION( 0x100000, "sub", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "popnrun-27256-3-17e.bin", 0xf0001, 0x008000, CRC(93f811aa) SHA1(75998375081d3cc5faa7533d39dd2d29a4ef3e7d) )
	ROM_LOAD16_BYTE( "popnrun-27256-4-17h.bin", 0xf0000, 0x008000, CRC(8fe0c064) SHA1(2a854238b8335a85cbe75c901480b51d479273a0) )

	ROM_REGION( 0x20000, "audiocpu", ROMREGION_ERASE00 )
	ROM_LOAD( "popnrun-2764-5-22c.bin", 0x000000, 0x002000, CRC(768a2ec7) SHA1(abc02ec6c6a495e612e8708377d9e7ca98981de4) )
	ROM_LOAD( "popnrun-27512-6-20c.bin", 0x010000, 0x010000, CRC(47d168ce) SHA1(36c16a400408834fcf0561c3f097e84a287560bd) )

	ROM_REGION( 0x2000, "gfx1", ROMREGION_ERASE00 )
	ROM_LOAD( "popnrun-2764-7-1a.bin", 0x000000, 0x002000, CRC(5e508b8e) SHA1(3e49e8d25a3db83178965382295e7c437441b5fe) )

	ROM_REGION( 0x80000, "gfx2", ROMREGION_ERASEFF ) /* Sprites */
	ROM_LOAD( "gfx2.bin",  0x0000, 0x80000, NO_DUMP )
	// debugging fill, remove me
	ROM_FILL(              0x0000, 0x80000, 0x33 )

	ROM_REGION( 0x100000, "gfx3", ROMREGION_ERASE00 ) /* pf1 layer */
	ROM_LOAD( "gfx3.bin",  0x0000, 0x100000, NO_DUMP )

	ROM_REGION( 0x40000, "gfx4", ROMREGION_ERASE00 ) // pf2 layer
	ROM_LOAD( "gfx4.bin",  0x0000, 0x40000, NO_DUMP )

	ROM_REGION( 0x40000, "gfx5", ROMREGION_ERASE00) // pf3 layer
	ROM_LOAD( "gfx5.bin",  0x0000, 0x40000, NO_DUMP )

	ROM_REGION16_BE( 0x10000, "gfx6", ROMREGION_ERASE00 )   /* background map data */
	ROM_LOAD( "gfx6.bin",  0x0000, 0x10000, NO_DUMP )

	ROM_REGION16_BE( 0x10000, "gfx7", ROMREGION_ERASE00 )   /* background map data */
	ROM_LOAD( "gfx7.bin",  0x0000, 0x10000, NO_DUMP )

	ROM_REGION( 0x10000, "adpcm1", ROMREGION_ERASE00 )

	ROM_REGION( 0x10000, "adpcm2", ROMREGION_ERASE00 )

	ROM_REGION( 0x0100, "prom", ROMREGION_ERASE00 )
	ROM_LOAD( "popnrun-63s281-9b.bin", 0x000000, 0x000100, CRC(208d17ca) SHA1(a77d56337bcac8d9a7bc3411239dfb3045e069ec) )
ROM_END

ROM_START( popnruna )
	ROM_REGION( 0x100000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "1.e5.27512",   0xe0001, 0x010000, CRC(fe45b6c4) SHA1(efa0d4ce6c5963f25ca1195cd2d5745e730b3b95) )
	ROM_LOAD16_BYTE( "2.h5.27512",   0xe0000, 0x010000, CRC(1e325398) SHA1(ca481c1447de4bffdea695deb9bb46c269272c68) )

	ROM_REGION( 0x100000, "sub", ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "popnrun-27256-3-17e.bin", 0xf0001, 0x008000, CRC(93f811aa) SHA1(75998375081d3cc5faa7533d39dd2d29a4ef3e7d) )
	ROM_LOAD16_BYTE( "popnrun-27256-4-17h.bin", 0xf0000, 0x008000, CRC(8fe0c064) SHA1(2a854238b8335a85cbe75c901480b51d479273a0) )

	ROM_REGION( 0x20000, "audiocpu", ROMREGION_ERASE00 )
	ROM_LOAD( "popnrun-2764-5-22c.bin", 0x000000, 0x002000, CRC(768a2ec7) SHA1(abc02ec6c6a495e612e8708377d9e7ca98981de4) )
	ROM_LOAD( "popnrun-27512-6-20c.bin", 0x010000, 0x010000, CRC(47d168ce) SHA1(36c16a400408834fcf0561c3f097e84a287560bd) )

	ROM_REGION( 0x2000, "gfx1", ROMREGION_ERASE00 )
	ROM_LOAD( "popnrun-2764-7-1a.bin", 0x000000, 0x002000, CRC(5e508b8e) SHA1(3e49e8d25a3db83178965382295e7c437441b5fe) )

	ROM_REGION( 0x80000, "gfx2", ROMREGION_ERASEFF ) /* Sprites */
	ROM_LOAD( "gfx2.bin",  0x0000, 0x80000, NO_DUMP )
	// debugging fill, remove me
	ROM_FILL(              0x0000, 0x80000, 0x33 )

	ROM_REGION( 0x100000, "gfx3", ROMREGION_ERASE00 ) /* pf1 layer */
	ROM_LOAD( "gfx3.bin",  0x0000, 0x100000, NO_DUMP )

	ROM_REGION( 0x40000, "gfx4", ROMREGION_ERASE00 ) // pf2 layer
	ROM_LOAD( "gfx4.bin",  0x0000, 0x40000, NO_DUMP )

	ROM_REGION( 0x40000, "gfx5", ROMREGION_ERASE00) // pf3 layer
	ROM_LOAD( "gfx5.bin",  0x0000, 0x40000, NO_DUMP )

	ROM_REGION16_BE( 0x10000, "gfx6", ROMREGION_ERASE00 )   /* background map data */
	ROM_LOAD( "gfx6.bin",  0x0000, 0x10000, NO_DUMP )

	ROM_REGION16_BE( 0x10000, "gfx7", ROMREGION_ERASE00 )   /* background map data */
	ROM_LOAD( "gfx7.bin",  0x0000, 0x10000, NO_DUMP )

	ROM_REGION( 0x0100, "prom", ROMREGION_ERASE00 )
	ROM_LOAD( "popnrun-63s281-9b.bin", 0x000000, 0x000100, CRC(208d17ca) SHA1(a77d56337bcac8d9a7bc3411239dfb3045e069ec) )
ROM_END

ROM_START( deadang )
	ROM_REGION( 0x100000, "maincpu", 0 ) /* v30 main cpu */
	ROM_LOAD16_BYTE("2.18h",   0x0c0000, 0x10000, CRC(1bc05b7e) SHA1(21833150a1f5ab543999a67f5b3bfbaf703e5508) )
	ROM_LOAD16_BYTE("4.22h",   0x0c0001, 0x10000, CRC(5751d4e7) SHA1(2e1a30c20199461fd876849f7563fef1d9a80c2d) )
	ROM_LOAD16_BYTE("1.18f",   0x0e0000, 0x10000, CRC(8e7b15cc) SHA1(7e4766953c1adf04be18207a2aa6f5e861ea5f6c) )
	ROM_LOAD16_BYTE("3.21f",   0x0e0001, 0x10000, CRC(e784b1fa) SHA1(3f41d31e0b36b9a2fab5e9998bb4146dfa0a97eb) )

	ROM_REGION( 0x100000, "sub", 0 ) /* v30 sub cpu */
	ROM_LOAD16_BYTE("5.6b",   0x0e0000, 0x10000, CRC(9c69eb35) SHA1(d5a9714f279b71c419b4bae0f142c4cb1cc8d30e) )
	ROM_LOAD16_BYTE("6.9b",   0x0e0001, 0x10000, CRC(34a44ce5) SHA1(621c69d8778d4c96ac3be06b033a5931a6a23da2) )

	ROM_REGION( 0x20000, "audiocpu", 0 ) /* sound Z80 */
	ROM_LOAD( "13.b1", 0x000000, 0x02000, CRC(13b956fb) SHA1(f7c21ad5e988ac59073659a427b1fa66ff49b0c1) ) /* Encrypted */
	ROM_LOAD( "14.c1", 0x010000, 0x10000, CRC(98837d57) SHA1(291769a11478291a65c959d119d19960b100d135) ) /* Banked */

	ROM_REGION( 0x08000, "gfx1", 0 ) /* Chars */
	ROM_LOAD( "7.21j", 0x000000, 0x4000, CRC(fe615fcd) SHA1(d67ee5e877b937173f4c188829d5bcbd354ceb29) )
	ROM_LOAD( "8.21l", 0x004000, 0x4000, CRC(905d6b27) SHA1(952f1879e6c27dc87234a4dc572e0453dc2d59fa) )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "l12", 0x000000, 0x80000, CRC(c94d5cd2) SHA1(25ded13faaed90886c9fe40f85969dab2f511e31) )

	ROM_REGION( 0x100000, "gfx3", 0 ) /* pf1 layer */
	ROM_LOAD( "16n", 0x000000, 0x80000, CRC(fd70e1a5) SHA1(c3d1233f4dfe08f686ec99a556889f9ed6a21da3) ) // bank 0 (0x1000 tiles)
	ROM_LOAD( "16r", 0x080000, 0x80000, CRC(92f5e382) SHA1(2097b9e9bf3cd37c8613847e7aed677b5aeab7f9) ) // bank 1 (0x1000 tiles)

	ROM_REGION( 0x40000, "gfx4", 0 ) // pf2 layer
	ROM_LOAD( "11m", 0x000000, 0x40000, CRC(a366659a) SHA1(e2fcd82b0b2d4e3adcdf50c710984907d26acd04) ) // fixed (0x800 tiles)

	ROM_REGION( 0x40000, "gfx5", 0 ) // pf3 layer
	ROM_LOAD( "11k", 0x000000, 0x40000, CRC(9cf5bcc7) SHA1(cf96592e601fc373b1bf322d9b576668799130a5) ) // fixed (0x800 tiles)

	ROM_REGION16_BE( 0x10000, "gfx6", 0 )   /* background map data */
	ROM_LOAD16_BYTE( "10.6l",  0x00000, 0x8000, CRC(ca99176b) SHA1(283e3769a1ff579c78a008b65cb8267e5770ba1f) )
	ROM_LOAD16_BYTE( "9.6m",   0x00001, 0x8000, CRC(51d868ca) SHA1(3e9a4e6bc4bc68773c4ba18c5f4110e6c595d0c9) )

	ROM_REGION16_BE( 0x10000, "gfx7", 0 )   /* background map data */
	ROM_LOAD16_BYTE( "12.6j",  0x00000, 0x8000, CRC(2674d23f) SHA1(0533d80a23d917e20a703aeb833dcaccfa3a1967) )
	ROM_LOAD16_BYTE( "11.6k",  0x00001, 0x8000, CRC(3dd4d81d) SHA1(94f0a13a8d3812f6879819ca186abf3a8665f7cb) )

	ROM_REGION( 0x10000, "adpcm1", 0 )
	ROM_LOAD( "15.b11", 0x000000, 0x10000, CRC(fabd74f2) SHA1(ac70e952a8b38287613b384cdc7ca00a7f155a13) )

	ROM_REGION( 0x10000, "adpcm2", 0 )
	ROM_LOAD( "16.11a", 0x000000, 0x10000, CRC(a8d46fc9) SHA1(3ba51bdec4057413396a152b35015f9d95253e3f) )
ROM_END

ROM_START( leadang )
	ROM_REGION( 0x100000, "maincpu", 0 ) /* v30 main cpu */
	ROM_LOAD16_BYTE("2.18h",   0x0c0000, 0x10000, CRC(611247e0) SHA1(1b9ad50f67ba3a3a9e5a0d6e33f4d4be2fc20446) ) // sldh
	ROM_LOAD16_BYTE("4.22h",   0x0c0001, 0x10000, CRC(348c1201) SHA1(277dd77dcbc950299de0fd56a4f66db8f90752ad) ) // sldh
	ROM_LOAD16_BYTE("1.18f",   0x0e0000, 0x10000, CRC(fb952d71) SHA1(c6578cddf019872e6005c3a9e8e3e024d17d8c6e) ) // sldh
	ROM_LOAD16_BYTE("3.22f",   0x0e0001, 0x10000, CRC(2271c6df) SHA1(774a92bb698606e58d0c74ea07d7eaecf766dddf) )

	ROM_REGION( 0x100000, "sub", 0 ) /* v30 sub cpu */
	ROM_LOAD16_BYTE("5.6b",    0x0e0000, 0x10000, CRC(9c69eb35) SHA1(d5a9714f279b71c419b4bae0f142c4cb1cc8d30e) )
	ROM_LOAD16_BYTE("6.9b",    0x0e0001, 0x10000, CRC(34a44ce5) SHA1(621c69d8778d4c96ac3be06b033a5931a6a23da2) )

	ROM_REGION( 0x20000, "audiocpu", 0 ) /* sound Z80 */
	ROM_LOAD( "13.b1", 0x000000, 0x02000, CRC(13b956fb) SHA1(f7c21ad5e988ac59073659a427b1fa66ff49b0c1) ) /* Encrypted */
	ROM_LOAD( "14.c1", 0x010000, 0x10000, CRC(98837d57) SHA1(291769a11478291a65c959d119d19960b100d135) ) /* Banked */

	ROM_REGION( 0x08000, "gfx1", 0 ) /* Chars */
	ROM_LOAD( "7.22k", 0x000000, 0x4000, CRC(490701e7) SHA1(2f5cbc0407d7fe41b9e7683c7531656fda7bf9f7) )
	ROM_LOAD( "8.22l", 0x004000, 0x4000, CRC(18024c5e) SHA1(b02bcaa1ba6e7c188f3d2a6b20b52b2dcb8215e0) )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "l12", 0x000000, 0x80000, CRC(c94d5cd2) SHA1(25ded13faaed90886c9fe40f85969dab2f511e31) )

	ROM_REGION( 0x100000, "gfx3", 0 ) /* pf1 layer */
	ROM_LOAD( "16n", 0x000000, 0x80000, CRC(fd70e1a5) SHA1(c3d1233f4dfe08f686ec99a556889f9ed6a21da3) ) // bank 0 (0x1000 tiles)
	ROM_LOAD( "16r", 0x080000, 0x80000, CRC(92f5e382) SHA1(2097b9e9bf3cd37c8613847e7aed677b5aeab7f9) ) // bank 1 (0x1000 tiles)

	ROM_REGION( 0x40000, "gfx4", 0 ) // pf2 layer
	ROM_LOAD( "11m", 0x000000, 0x40000, CRC(a366659a) SHA1(e2fcd82b0b2d4e3adcdf50c710984907d26acd04) ) // fixed (0x800 tiles)

	ROM_REGION( 0x40000, "gfx5", 0 ) // pf3 layer
	ROM_LOAD( "11k", 0x000000, 0x40000, CRC(9cf5bcc7) SHA1(cf96592e601fc373b1bf322d9b576668799130a5) ) // fixed (0x800 tiles)

	ROM_REGION16_BE( 0x10000, "gfx6", 0 )   /* background map data */
	ROM_LOAD16_BYTE( "10.6l",  0x00000, 0x8000, CRC(ca99176b) SHA1(283e3769a1ff579c78a008b65cb8267e5770ba1f) )
	ROM_LOAD16_BYTE( "9.6m",   0x00001, 0x8000, CRC(51d868ca) SHA1(3e9a4e6bc4bc68773c4ba18c5f4110e6c595d0c9) )

	ROM_REGION16_BE( 0x10000, "gfx7", 0 )   /* background map data */
	ROM_LOAD16_BYTE( "12.6j",  0x00000, 0x8000, CRC(2674d23f) SHA1(0533d80a23d917e20a703aeb833dcaccfa3a1967) )
	ROM_LOAD16_BYTE( "11.6k",  0x00001, 0x8000, CRC(3dd4d81d) SHA1(94f0a13a8d3812f6879819ca186abf3a8665f7cb) )

	ROM_REGION( 0x10000, "adpcm1", 0 )
	ROM_LOAD( "15.b11", 0x000000, 0x10000, CRC(fabd74f2) SHA1(ac70e952a8b38287613b384cdc7ca00a7f155a13) )

	ROM_REGION( 0x10000, "adpcm2", 0 )
	ROM_LOAD( "16.11a", 0x000000, 0x10000, CRC(a8d46fc9) SHA1(3ba51bdec4057413396a152b35015f9d95253e3f) )
ROM_END

ROM_START( ghunter )
	ROM_REGION( 0x100000, "maincpu", 0 ) /* v30 main cpu */
	ROM_LOAD16_BYTE("2.19h",   0x0c0000, 0x10000, CRC(5a511500) SHA1(69185a9efee0c3ee4d65643651eb9c613bc5f759) )
	ROM_LOAD16_BYTE("4.22h",   0x0c0001, 0x10000, CRC(df5704f4) SHA1(a40848f1222253921982320155e6f7a01d2bb17f) ) // sldh
	ROM_LOAD16_BYTE("1.19f",   0x0e0000, 0x10000, CRC(30deb018) SHA1(099ab1f227d7e28f3e56a61d015813905a2dbc29) )
	ROM_LOAD16_BYTE("3.22f",   0x0e0001, 0x10000, CRC(95f587c5) SHA1(b1431dd56200a5f849314b34daed5d3570633a77) ) // sldh

	ROM_REGION( 0x100000, "sub", 0 ) /* v30 sub cpu */
	ROM_LOAD16_BYTE("5.6b",   0x0e0000, 0x10000, CRC(c40bb5e5) SHA1(2a618f7db6fe6cd8d1a0e7eed91a831b721fec62) ) // sldh
	ROM_LOAD16_BYTE("6.10b",  0x0e0001, 0x10000, CRC(373f86a7) SHA1(6f7d219a3bc34d74fdadd812319a5387d217dffb) ) // sldh

	ROM_REGION( 0x20000, "audiocpu", 0 ) /* sound Z80 */
	ROM_LOAD( "13.b1", 0x000000, 0x02000, CRC(13b956fb) SHA1(f7c21ad5e988ac59073659a427b1fa66ff49b0c1) ) /* Encrypted */
	ROM_LOAD( "14.c1", 0x010000, 0x10000, CRC(98837d57) SHA1(291769a11478291a65c959d119d19960b100d135) ) /* Banked */

	ROM_REGION( 0x08000, "gfx1", 0 ) /* Chars */
	ROM_LOAD( "7.22k", 0x000000, 0x4000, CRC(490701e7) SHA1(2f5cbc0407d7fe41b9e7683c7531656fda7bf9f7) )
	ROM_LOAD( "8.22l", 0x004000, 0x4000, CRC(18024c5e) SHA1(b02bcaa1ba6e7c188f3d2a6b20b52b2dcb8215e0) )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "l12", 0x000000, 0x80000, CRC(c94d5cd2) SHA1(25ded13faaed90886c9fe40f85969dab2f511e31) )

	ROM_REGION( 0x100000, "gfx3", 0 ) /* pf1 layer */
	ROM_LOAD( "16n", 0x000000, 0x80000, CRC(fd70e1a5) SHA1(c3d1233f4dfe08f686ec99a556889f9ed6a21da3) ) // bank 0 (0x1000 tiles)
	ROM_LOAD( "16r", 0x080000, 0x80000, CRC(92f5e382) SHA1(2097b9e9bf3cd37c8613847e7aed677b5aeab7f9) ) // bank 1 (0x1000 tiles)

	ROM_REGION( 0x40000, "gfx4", 0 ) // pf2 layer
	ROM_LOAD( "11m", 0x000000, 0x40000, CRC(a366659a) SHA1(e2fcd82b0b2d4e3adcdf50c710984907d26acd04) ) // fixed (0x800 tiles)

	ROM_REGION( 0x40000, "gfx5", 0 ) // pf3 layer
	ROM_LOAD( "11k", 0x000000, 0x40000, CRC(9cf5bcc7) SHA1(cf96592e601fc373b1bf322d9b576668799130a5) ) // fixed (0x800 tiles)

	ROM_REGION16_BE( 0x10000, "gfx6", 0 )   /* background map data */
	ROM_LOAD16_BYTE( "10.6l",  0x00000, 0x8000, CRC(ca99176b) SHA1(283e3769a1ff579c78a008b65cb8267e5770ba1f) )
	ROM_LOAD16_BYTE( "9.6m",   0x00001, 0x8000, CRC(51d868ca) SHA1(3e9a4e6bc4bc68773c4ba18c5f4110e6c595d0c9) )

	ROM_REGION16_BE( 0x10000, "gfx7", 0 )   /* background map data */
	ROM_LOAD16_BYTE( "12.6j",  0x00000, 0x8000, CRC(2674d23f) SHA1(0533d80a23d917e20a703aeb833dcaccfa3a1967) )
	ROM_LOAD16_BYTE( "11.6k",  0x00001, 0x8000, CRC(3dd4d81d) SHA1(94f0a13a8d3812f6879819ca186abf3a8665f7cb) )

	ROM_REGION( 0x10000, "adpcm1", 0 )
	ROM_LOAD( "15.b11", 0x000000, 0x10000, CRC(fabd74f2) SHA1(ac70e952a8b38287613b384cdc7ca00a7f155a13) )

	ROM_REGION( 0x10000, "adpcm2", 0 )
	ROM_LOAD( "16.11a", 0x000000, 0x10000, CRC(a8d46fc9) SHA1(3ba51bdec4057413396a152b35015f9d95253e3f) )
ROM_END

ROM_START( ghunters )
	ROM_REGION( 0x100000, "maincpu", 0 ) /* v30 main cpu */
	ROM_LOAD16_BYTE("ggh-2.h18",   0x0c0000, 0x10000, CRC(7ccc6fee) SHA1(bccc283d82f080157f0521457b04fdd1d63caafe) )
	ROM_LOAD16_BYTE("ggh-4.h22",   0x0c0001, 0x10000, CRC(d1f23ad7) SHA1(2668729af797ccab52ac2bf519d43ab2fa9e54ce) )
	ROM_LOAD16_BYTE("ggh-1.f18",   0x0e0000, 0x10000, CRC(0d6ff111) SHA1(209d26170446b43d1d463737b447e30aaca614a7) )
	ROM_LOAD16_BYTE("ggh-3.f22",   0x0e0001, 0x10000, CRC(66dec38d) SHA1(78dd3143265c3da90d1a0ab2c4f42b4e32716af8) )

	ROM_REGION( 0x100000, "sub", 0 ) /* v30 sub cpu */
	ROM_LOAD16_BYTE("ggh-5.b6",   0x0e0000, 0x10000, CRC(1f612f3b) SHA1(71840fa0e988828a819d371f082ce31d5a5e3a30) )
	ROM_LOAD16_BYTE("ggh-6.b10",  0x0e0001, 0x10000, CRC(63e18e56) SHA1(5183d0909a7c795e76540723fb710a5a75730298) )

	ROM_REGION( 0x20000, "audiocpu", 0 ) /* sound Z80 */
	ROM_LOAD( "13.b1", 0x000000, 0x02000, CRC(13b956fb) SHA1(f7c21ad5e988ac59073659a427b1fa66ff49b0c1) ) /* Encrypted */
	ROM_LOAD( "14.c1", 0x010000, 0x10000, CRC(98837d57) SHA1(291769a11478291a65c959d119d19960b100d135) ) /* Banked */

	ROM_REGION( 0x08000, "gfx1", 0 ) /* Chars */
	ROM_LOAD( "7.21j", 0x000000, 0x4000, CRC(fe615fcd) SHA1(d67ee5e877b937173f4c188829d5bcbd354ceb29) )
	ROM_LOAD( "8.21l", 0x004000, 0x4000, CRC(905d6b27) SHA1(952f1879e6c27dc87234a4dc572e0453dc2d59fa) )

	ROM_REGION( 0x80000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "l12", 0x000000, 0x80000, CRC(c94d5cd2) SHA1(25ded13faaed90886c9fe40f85969dab2f511e31) )

	ROM_REGION( 0x100000, "gfx3", 0 ) /* pf1 layer */
	ROM_LOAD( "16n", 0x000000, 0x80000, CRC(fd70e1a5) SHA1(c3d1233f4dfe08f686ec99a556889f9ed6a21da3) ) // bank 0 (0x1000 tiles)
	ROM_LOAD( "16r", 0x080000, 0x80000, CRC(92f5e382) SHA1(2097b9e9bf3cd37c8613847e7aed677b5aeab7f9) ) // bank 1 (0x1000 tiles)

	ROM_REGION( 0x40000, "gfx4", 0 ) // pf2 layer
	ROM_LOAD( "11m", 0x000000, 0x40000, CRC(a366659a) SHA1(e2fcd82b0b2d4e3adcdf50c710984907d26acd04) ) // fixed (0x800 tiles)

	ROM_REGION( 0x40000, "gfx5", 0 ) // pf3 layer
	ROM_LOAD( "11k", 0x000000, 0x40000, CRC(9cf5bcc7) SHA1(cf96592e601fc373b1bf322d9b576668799130a5) ) // fixed (0x800 tiles)

	ROM_REGION16_BE( 0x10000, "gfx6", 0 )   /* background map data */
	ROM_LOAD16_BYTE( "10.6l",  0x00000, 0x8000, CRC(ca99176b) SHA1(283e3769a1ff579c78a008b65cb8267e5770ba1f) )
	ROM_LOAD16_BYTE( "9.6m",   0x00001, 0x8000, CRC(51d868ca) SHA1(3e9a4e6bc4bc68773c4ba18c5f4110e6c595d0c9) )

	ROM_REGION16_BE( 0x10000, "gfx7", 0 )   /* background map data */
	ROM_LOAD16_BYTE( "12.6j",  0x00000, 0x8000, CRC(2674d23f) SHA1(0533d80a23d917e20a703aeb833dcaccfa3a1967) )
	ROM_LOAD16_BYTE( "11.6k",  0x00001, 0x8000, CRC(3dd4d81d) SHA1(94f0a13a8d3812f6879819ca186abf3a8665f7cb) )

	ROM_REGION( 0x10000, "adpcm1", 0 )
	ROM_LOAD( "15.b11", 0x000000, 0x10000, CRC(fabd74f2) SHA1(ac70e952a8b38287613b384cdc7ca00a7f155a13) )

	ROM_REGION( 0x10000, "adpcm2", 0 )
	ROM_LOAD( "16.11a", 0x000000, 0x10000, CRC(a8d46fc9) SHA1(3ba51bdec4057413396a152b35015f9d95253e3f) )
ROM_END

/* Driver Initialization */

void deadang_state::init_deadang()
{
	m_adpcm1->decrypt();
	m_adpcm2->decrypt();
}

void popnrun_state::init_popnrun()
{
//  m_adpcm1->decrypt();
//  m_adpcm2->decrypt();
}

void deadang_state::init_ghunter()
{
	m_adpcm1->decrypt();
	m_adpcm2->decrypt();

	m_maincpu->space(AS_PROGRAM).install_read_handler(0x80000, 0x80001, read16_delegate(FUNC(deadang_state::ghunter_trackball_low_r),this));
	m_maincpu->space(AS_PROGRAM).install_read_handler(0xb0000, 0xb0001, read16_delegate(FUNC(deadang_state::ghunter_trackball_high_r),this));
}

/* Game Drivers */
GAME( 1987, popnrun,  0,       popnrun, deadang, popnrun_state, init_popnrun, ROT0, "Seibu Kaihatsu / Yukai Tsukai",         "Pop'n Run - The Videogame (set 1)", MACHINE_NOT_WORKING | MACHINE_SUPPORTS_SAVE )
GAME( 1987, popnruna, popnrun, popnrun, deadang, popnrun_state, init_popnrun, ROT0, "Seibu Kaihatsu / Yukai Tsukai",         "Pop'n Run - The Videogame (set 2)", MACHINE_NOT_WORKING | MACHINE_SUPPORTS_SAVE )

GAME( 1988, deadang,  0,       deadang, deadang, deadang_state, init_deadang, ROT0, "Seibu Kaihatsu",                        "Dead Angle",                        MACHINE_SUPPORTS_SAVE )
GAME( 1988, leadang,  deadang, deadang, deadang, deadang_state, init_deadang, ROT0, "Seibu Kaihatsu",                        "Lead Angle (Japan)",                MACHINE_SUPPORTS_SAVE )
GAME( 1988, ghunter,  deadang, deadang, ghunter, deadang_state, init_ghunter, ROT0, "Seibu Kaihatsu",                        "Gang Hunter / Dead Angle",          MACHINE_SUPPORTS_SAVE ) // Title is 'Gang Hunter' or 'Dead Angle' depending on control method dipswitch
GAME( 1988, ghunters, deadang, deadang, ghunter, deadang_state, init_ghunter, ROT0, "Seibu Kaihatsu (Segasa/Sonic license)", "Gang Hunter / Dead Angle (Spain)",  MACHINE_SUPPORTS_SAVE )
