// license:BSD-3-Clause
// copyright-holders:Carlos A. Lozano
/******************************************************************

Cabal  (c)1998 Tad

driver by Carlos A. Lozano Baides

68000 + Z80

The original uses 2xYM3931 for sound
The bootleg uses YM2151 + 2xZ80 used as ADPCM players


MEMORY MAP
0x00000 - 0x3ffff   ROM
0x40000 - 0x4ffff   RAM
[of which: 0x43800 - 0x43fff   VRAM (Sprites)]
0x60000 - 0x607ff   VRAM (Tiles)
0x80000 - 0x803ff   VRAM (Background)
0xa0000 - 0xa000f   Input Ports
0xc0040 - 0xc0040   Watchdog??
0xc0080 - 0xc0080   Screen Flip (+ others?)
0xe0000 - 0xe07ff   COLORRAM (----BBBBGGGGRRRR)
0xe8000 - 0xe800f   Communication with sound CPU (also coins)

VRAM (Background)
0x80000 - 0x801ff  (16x16 of 16x16 tiles, 2 bytes per tile)
0x80200 - 0x803ff  unused foreground layer??

VRAM (Text)
0x60000 - 0x607ff  (32x32 of 8x8 tiles, 2 bytes per tile)

VRAM (Sprites)
0x43800 - 0x43bff  (128 sprites, 8 bytes every sprite)

COLORRAM (Colors)
0xe0000 - 0xe07ff  (1024 colors, ----BBBBGGGGRRRR)


2008-07
Dip locations verified with Fabtek manual for the trackball version

******************************************************************/

#include "emu.h"
#include "includes/cabal.h"

#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "machine/gen_latch.h"
#include "machine/upd4701.h"
#include "sound/ym2151.h"
#include "sound/msm5205.h"
#include "screen.h"
#include "speaker.h"


MACHINE_START_MEMBER(cabal_state,cabalbl)
{
	save_item(NAME(m_sound_command1));
	save_item(NAME(m_sound_command2));
}

MACHINE_RESET_MEMBER(cabal_state,cabalbl)
{
	m_sound_command1 = m_sound_command2 = 0xff;
}


/******************************************************************************************/

WRITE16_MEMBER(cabal_state::cabalbl_sndcmd_w)
{
	switch (offset)
	{
		case 0x0:
			m_sound_command1 = data;
			break;

		case 0x1: /* ?? */
			m_sound_command2 = data & 0xff;
			break;
	}
}



void cabal_state::sound_irq_trigger_word_w(offs_t, u16 data, u16 mem_mask)
{
	if (ACCESSING_BITS_0_7)
		m_seibu_sound->main_w(4, data & 0x00ff);

	/* spin for a while to let the Z80 read the command, otherwise coins "stick" */
	m_maincpu->spin_until_time(attotime::from_usec(50));
}

WRITE16_MEMBER(cabal_state::cabalbl_sound_irq_trigger_word_w)
{
	m_audiocpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
}



void cabal_state::main_map(address_map &map)
{
	map(0x00000, 0x3ffff).rom();
	map(0x40000, 0x437ff).ram();
	map(0x43800, 0x43fff).ram().share("spriteram");
	map(0x44000, 0x4ffff).ram();
	map(0x60000, 0x607ff).ram().w(FUNC(cabal_state::text_videoram_w)).share("colorram");
	map(0x80000, 0x801ff).ram().w(FUNC(cabal_state::background_videoram_w)).share("videoram");
	map(0x80200, 0x803ff).ram();
	map(0xa0000, 0xa0001).portr("DSW");
	map(0xa0008, 0xa0009).portr("IN2");
	map(0xa0010, 0xa0011).portr("INPUTS");
	map(0xc0040, 0xc0041).nopw(); /* ??? */
	map(0xc0080, 0xc0081).w(FUNC(cabal_state::flipscreen_w));
	map(0xe0000, 0xe07ff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0xe8000, 0xe800d).rw(m_seibu_sound, FUNC(seibu_sound_device::main_r), FUNC(seibu_sound_device::main_w)).umask16(0x00ff);
	map(0xe8008, 0xe8009).w(FUNC(cabal_state::sound_irq_trigger_word_w)); // fix coin insertion
}



void cabal_state::trackball_main_map(address_map &map)
{
	main_map(map);
	map(0xa0008, 0xa000f).r("upd4701l", FUNC(upd4701_device::read_xy)).umask16(0x00ff);
	map(0xa0008, 0xa000f).r("upd4701h", FUNC(upd4701_device::read_xy)).umask16(0xff00);
	map(0xc0001, 0xc0001).w("upd4701l", FUNC(upd4701_device::reset_xy_w));
	map(0xc0000, 0xc0000).w("upd4701h", FUNC(upd4701_device::reset_xy_w));
}



void cabal_state::cabalbl_main_map(address_map &map)
{
	map(0x00000, 0x3ffff).rom();
	map(0x40000, 0x437ff).ram();
	map(0x43800, 0x43fff).ram().share("spriteram");
	map(0x44000, 0x4ffff).ram();
	map(0x60000, 0x607ff).ram().w(FUNC(cabal_state::text_videoram_w)).share("colorram");
	map(0x80000, 0x801ff).ram().w(FUNC(cabal_state::background_videoram_w)).share("videoram");
	map(0x80200, 0x803ff).ram();
	map(0xa0000, 0xa0001).portr("DSW");
	map(0xa0008, 0xa0009).portr("JOY");
	map(0xa0010, 0xa0011).portr("INPUTS");
	map(0xc0040, 0xc0041).nopw(); /* ??? */
	map(0xc0080, 0xc0081).w(FUNC(cabal_state::flipscreen_w));
	map(0xe0000, 0xe07ff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0xe8000, 0xe8003).w(FUNC(cabal_state::cabalbl_sndcmd_w));
	map(0xe8005, 0xe8005).r("soundlatch", FUNC(generic_latch_8_device::read));
	map(0xe8008, 0xe8009).w(FUNC(cabal_state::cabalbl_sound_irq_trigger_word_w));
}

/*********************************************************************/


READ8_MEMBER(cabal_state::cabalbl_snd2_r)
{
	return bitswap<8>(m_sound_command2, 7,2,4,5,3,6,1,0);
}

READ8_MEMBER(cabal_state::cabalbl_snd1_r)
{
	return bitswap<8>(m_sound_command1, 7,2,4,5,3,6,1,0);
}

WRITE8_MEMBER(cabal_state::cabalbl_coin_w)
{
	machine().bookkeeping().coin_counter_w(0, data & 1);
	machine().bookkeeping().coin_counter_w(1, data & 2);

	//data & 0x40? video enable?
}

void cabal_state::sound_map(address_map &map)
{
	map(0x0000, 0x1fff).r("sei80bu", FUNC(sei80bu_device::data_r));
	map(0x2000, 0x27ff).ram();
	map(0x4001, 0x4001).w(m_seibu_sound, FUNC(seibu_sound_device::irq_clear_w));
	map(0x4002, 0x4002).w(m_seibu_sound, FUNC(seibu_sound_device::rst10_ack_w));
	map(0x4003, 0x4003).w(m_seibu_sound, FUNC(seibu_sound_device::rst18_ack_w));
	map(0x4005, 0x4006).w(m_adpcm1, FUNC(seibu_adpcm_device::adr_w));
	map(0x4008, 0x4009).rw(m_seibu_sound, FUNC(seibu_sound_device::ym_r), FUNC(seibu_sound_device::ym_w));
	map(0x4010, 0x4011).r(m_seibu_sound, FUNC(seibu_sound_device::soundlatch_r));
	map(0x4012, 0x4012).r(m_seibu_sound, FUNC(seibu_sound_device::main_data_pending_r));
	map(0x4013, 0x4013).portr("COIN");
	map(0x4018, 0x4019).w(m_seibu_sound, FUNC(seibu_sound_device::main_data_w));
	map(0x401a, 0x401a).w(m_adpcm1, FUNC(seibu_adpcm_device::ctl_w));
	map(0x401b, 0x401b).w(m_seibu_sound, FUNC(seibu_sound_device::coin_w));
	map(0x6005, 0x6006).w(m_adpcm2, FUNC(seibu_adpcm_device::adr_w));
	map(0x601a, 0x601a).w(m_adpcm2, FUNC(seibu_adpcm_device::ctl_w));
	map(0x8000, 0xffff).rom();
}

void cabal_state::sound_decrypted_opcodes_map(address_map &map)
{
	map(0x0000, 0x1fff).r("sei80bu", FUNC(sei80bu_device::opcode_r));
	map(0x8000, 0xffff).rom().region("audiocpu", 0x8000);
}

void cabal_state::cabalbl_sound_map(address_map &map)
{
	map(0x0000, 0x1fff).rom();
	map(0x2000, 0x2fff).ram();
	map(0x4000, 0x4000).w("soundlatch2", FUNC(generic_latch_8_device::write));
	map(0x4002, 0x4002).w("soundlatch3", FUNC(generic_latch_8_device::write));
	map(0x4004, 0x4004).w(FUNC(cabal_state::cabalbl_coin_w));
	map(0x4006, 0x4006).portr("COIN");
	map(0x4008, 0x4008).r(FUNC(cabal_state::cabalbl_snd2_r));
	map(0x400a, 0x400a).r(FUNC(cabal_state::cabalbl_snd1_r));
	map(0x400c, 0x400c).w("soundlatch", FUNC(generic_latch_8_device::write));
	map(0x400e, 0x400f).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0x6000, 0x6000).nopw();  /* ??? */
	map(0x8000, 0xffff).rom();
}

void cabal_state::cabalbl2_sound_map(address_map &map)
{
	map(0x0000, 0x1fff).rom();
	map(0x2000, 0x27ff).ram();
	map(0x4001, 0x4001).w(m_seibu_sound, FUNC(seibu_sound_device::irq_clear_w));
	map(0x4002, 0x4002).w(m_seibu_sound, FUNC(seibu_sound_device::rst10_ack_w));
	map(0x4003, 0x4003).w(m_seibu_sound, FUNC(seibu_sound_device::rst18_ack_w));
	map(0x4005, 0x4006).w(m_adpcm1, FUNC(seibu_adpcm_device::adr_w));
	map(0x4008, 0x4009).rw(m_seibu_sound, FUNC(seibu_sound_device::ym_r), FUNC(seibu_sound_device::ym_w));
	map(0x4010, 0x4011).r(m_seibu_sound, FUNC(seibu_sound_device::soundlatch_r));
	map(0x4012, 0x4012).r(m_seibu_sound, FUNC(seibu_sound_device::main_data_pending_r));
	map(0x4013, 0x4013).portr("COIN");
	map(0x4018, 0x4019).w(m_seibu_sound, FUNC(seibu_sound_device::main_data_w));
	map(0x401a, 0x401a).w(m_adpcm1, FUNC(seibu_adpcm_device::ctl_w));
	map(0x401b, 0x401b).w(m_seibu_sound, FUNC(seibu_sound_device::coin_w));
	map(0x6005, 0x6006).w(m_adpcm2, FUNC(seibu_adpcm_device::adr_w));
	map(0x601a, 0x601a).w(m_adpcm2, FUNC(seibu_adpcm_device::ctl_w));
	map(0x8000, 0xffff).rom();
}

void cabal_state::cabalbl2_predecrypted_opcodes_map(address_map &map)
{
	map(0x0000, 0x1fff).rom().region("audiocpu", 0x2000);
	map(0x8000, 0xffff).rom().region("audiocpu", 0x8000);
}

/* the bootleg has 2x z80 sample players */

WRITE8_MEMBER(cabal_state::cabalbl_1_adpcm_w)
{
	m_msm1->reset_w(BIT(data, 7));
	/* ?? bit 6?? */
	m_msm1->write_data(data);
	m_msm1->vclk_w(1);
	m_msm1->vclk_w(0);
}
WRITE8_MEMBER(cabal_state::cabalbl_2_adpcm_w)
{
	m_msm2->reset_w(BIT(data, 7));
	/* ?? bit 6?? */
	m_msm2->write_data(data);
	m_msm2->vclk_w(1);
	m_msm2->vclk_w(0);
}
void cabal_state::cabalbl_talk1_map(address_map &map)
{
	map(0x0000, 0xffff).rom().nopw();
}

void cabal_state::cabalbl_talk1_portmap(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x00).r("soundlatch2", FUNC(generic_latch_8_device::read));
	map(0x01, 0x01).w(FUNC(cabal_state::cabalbl_1_adpcm_w));
}

void cabal_state::cabalbl_talk2_map(address_map &map)
{
	map(0x0000, 0xffff).rom().nopw();
}

void cabal_state::cabalbl_talk2_portmap(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x00).r("soundlatch3", FUNC(generic_latch_8_device::read));
	map(0x01, 0x01).w(FUNC(cabal_state::cabalbl_2_adpcm_w));
}

/***************************************************************************/

static INPUT_PORTS_START( common )
	PORT_START("DSW")
	PORT_DIPNAME( 0x000f, 0x000f, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW1:1,2,3,4") PORT_CONDITION("DSW", 0x0010, NOTEQUALS, 0x00)
	PORT_DIPSETTING(      0x000a, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(      0x000b, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x000d, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 8C_3C ) )
	PORT_DIPSETTING(      0x000e, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 5C_3C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0x000f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0009, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0007, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0005, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Coin_A )) PORT_DIPLOCATION("SW1:1,2") PORT_CONDITION("DSW", 0x0010, EQUALS, 0x00)
	PORT_DIPSETTING(      0x0000, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x000c, 0x000c, DEF_STR( Coin_B )) PORT_DIPLOCATION("SW1:3,4") PORT_CONDITION("DSW", 0x0010, EQUALS, 0x00)
	PORT_DIPSETTING(      0x000c, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0010, 0x0010, "Coin Mode" ) PORT_DIPLOCATION("SW1:5")
	PORT_DIPSETTING(      0x0010, "Mode 1" )
	PORT_DIPSETTING(      0x0000, "Mode 2" )
	PORT_DIPNAME( 0x0020, 0x0020, "Invert Buttons" ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Trackball ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(      0x0080, "Small" )
	PORT_DIPSETTING(      0x0000, "Large" )
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0200, "2" )
	PORT_DIPSETTING(      0x0300, "3" )
	PORT_DIPSETTING(      0x0100, "5" )
	PORT_DIPSETTING(      0x0000, "121 (Cheat)")
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0c00, "150k 650k 500k+" )
	PORT_DIPSETTING(      0x0800, "200k 800k 600k+" )
	PORT_DIPSETTING(      0x0400, "300k 1000k 700k+" )
	PORT_DIPSETTING(      0x0000, "300k Only" )
	PORT_DIPNAME( 0x3000, 0x2000, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(      0x3000, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Very_Hard ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x4000, 0x4000, "SW2:7" )   /* Left blank in the manual */
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( On ) )

	PORT_START("COIN")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(4) /* read through sound cpu */
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(4) /* read through sound cpu */
INPUT_PORTS_END

static INPUT_PORTS_START( cabalt )
	PORT_INCLUDE( common )

	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0ff0, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("IN0")
	PORT_BIT( 0x0fff, 0x0000, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_RESET PORT_PLAYER(1)

	PORT_START("IN1")
	PORT_BIT( 0x0fff, 0x0000, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_REVERSE PORT_RESET PORT_PLAYER(1)

	PORT_START("IN2")
	PORT_BIT( 0x0fff, 0x0000, IPT_TRACKBALL_X ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_RESET PORT_PLAYER(2)

	PORT_START("IN3")
	PORT_BIT( 0x0fff, 0x0000, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(100) PORT_KEYDELTA(30) PORT_RESET PORT_PLAYER(2)
INPUT_PORTS_END



static INPUT_PORTS_START( cabalj )
	PORT_INCLUDE( common )

	PORT_MODIFY("COIN")
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNKNOWN )

	/* Since the Trackball version was produced first, and it doesn't use
	   the third button,  Pin 24 of the JAMMA connector ('JAMMA button 3')
	   has no trace on the pcb.  To work around this design issue the
	   manufacturer had to use pin 15 which is usually the test / service
	   button
	*/
	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0ff0, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) /* the 3rd button connects to the service switch */
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START1 )

	/* The joystick version has a PCB marked "Joystick sub" containing a 74ls245. It plugs in the
	   sockets of the two D4701AC */
	PORT_START("IN0")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)

	PORT_START("IN3")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( cabalbl )
	PORT_INCLUDE( common )

	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x00f0, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0f00, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("JOY")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
INPUT_PORTS_END

static const gfx_layout text_layout =
{
	8,8,
	RGN_FRAC(1,1),
	2,
	{ 0,4 },
	{ 3, 2, 1, 0, 8+3, 8+2, 8+1, 8+0},
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static const gfx_layout tile_layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 8, 12, 0, 4 },
	{ 3, 2, 1, 0, 16+3, 16+2, 16+1, 16+0,
			32*16+3, 32*16+2, 32*16+1, 32*16+0, 33*16+3, 33*16+2, 33*16+1, 33*16+0 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32,  11*32,  12*32,  13*32, 14*32,  15*32 },
	64*16
};

static const gfx_layout sprite_layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 8, 12, 0, 4 },
	{ 3, 2, 1, 0, 16+3, 16+2, 16+1, 16+0,
			32+3, 32+2, 32+1, 32+0, 48+3, 48+2, 48+1, 48+0 },
	{ 30*32, 28*32, 26*32, 24*32, 22*32, 20*32, 18*32, 16*32,
			14*32, 12*32, 10*32,  8*32,  6*32,  4*32,  2*32,  0*32 },
	64*16
};



static GFXDECODE_START( gfx_cabal )
	GFXDECODE_ENTRY( "gfx1", 0x000000, text_layout,   0, 1024/4 )
	GFXDECODE_ENTRY( "gfx2", 0x000000, tile_layout,   32*16, 16 )
	GFXDECODE_ENTRY( "gfx3", 0x000000, sprite_layout, 16*16, 16 )
GFXDECODE_END


void cabal_state::cabal(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, XTAL(20'000'000)/2); /* verified on pcb */
	m_maincpu->set_addrmap(AS_PROGRAM, &cabal_state::main_map);
	m_maincpu->set_vblank_int("screen", FUNC(cabal_state::irq1_line_hold));

	Z80(config, m_audiocpu, XTAL(3'579'545)); /* verified on pcb */
	m_audiocpu->set_addrmap(AS_PROGRAM, &cabal_state::sound_map);
	m_audiocpu->set_addrmap(AS_OPCODES, &cabal_state::sound_decrypted_opcodes_map);
	m_audiocpu->set_irq_acknowledge_callback("seibu_sound", FUNC(seibu_sound_device::im0_vector_cb));

	SEI80BU(config, "sei80bu", 0).set_device_rom_tag("audiocpu");

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(59.60);   /* verified on pcb */
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	screen.set_size(256, 256);
	screen.set_visarea(0*8, 32*8-1, 2*8, 30*8-1);
	screen.set_screen_update(FUNC(cabal_state::screen_update));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_cabal);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_444, 1024);

	/* sound hardware */
	SEIBU_SOUND(config, m_seibu_sound, 0);
	m_seibu_sound->int_callback().set_inputline(m_audiocpu, 0);
	m_seibu_sound->set_rom_tag("audiocpu");
	m_seibu_sound->ym_read_callback().set("ymsnd", FUNC(ym2151_device::read));
	m_seibu_sound->ym_write_callback().set("ymsnd", FUNC(ym2151_device::write));

	SPEAKER(config, "mono").front_center();

	ym2151_device &ymsnd(YM2151(config, "ymsnd", XTAL(3'579'545))); /* verified on pcb */
	ymsnd.irq_handler().set(m_seibu_sound, FUNC(seibu_sound_device::fm_irqhandler));
	ymsnd.add_route(ALL_OUTPUTS, "mono", 0.80);

	SEIBU_ADPCM(config, m_adpcm1, 8000).add_route(ALL_OUTPUTS, "mono", 0.40); /* it should use the msm5205 */
	SEIBU_ADPCM(config, m_adpcm2, 8000).add_route(ALL_OUTPUTS, "mono", 0.40); /* it should use the msm5205 */
}

void cabal_state::cabalt(machine_config &config)
{
	cabal(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &cabal_state::trackball_main_map);

	upd4701_device &upd4701l(UPD4701A(config, "upd4701l"));
	upd4701l.set_portx_tag("IN0");
	upd4701l.set_porty_tag("IN1");

	upd4701_device &upd4701h(UPD4701A(config, "upd4701h"));
	upd4701h.set_portx_tag("IN2");
	upd4701h.set_porty_tag("IN3");
}

void cabal_state::cabalbl2(machine_config &config)
{
	cabal(config);
	config.device_remove("sei80bu");

	m_audiocpu->set_addrmap(AS_PROGRAM, &cabal_state::cabalbl2_sound_map);
	m_audiocpu->set_addrmap(AS_OPCODES, &cabal_state::cabalbl2_predecrypted_opcodes_map);
}


/* the bootleg has different sound hardware (2 extra Z80s for ADPCM playback) */
void cabal_state::cabalbl(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, XTAL(20'000'000)/2); /* verified on pcb */
	m_maincpu->set_addrmap(AS_PROGRAM, &cabal_state::cabalbl_main_map);
	m_maincpu->set_vblank_int("screen", FUNC(cabal_state::irq1_line_hold));

	Z80(config, m_audiocpu, XTAL(3'579'545)); /* verified on pcb */
	m_audiocpu->set_addrmap(AS_PROGRAM, &cabal_state::cabalbl_sound_map);

	/* there are 2x z80s for the ADPCM */
	z80_device &adpcm_1(Z80(config, "adpcm_1", XTAL(3'579'545))); /* verified on pcb */
	adpcm_1.set_addrmap(AS_PROGRAM, &cabal_state::cabalbl_talk1_map);
	adpcm_1.set_addrmap(AS_IO, &cabal_state::cabalbl_talk1_portmap);
	adpcm_1.set_periodic_int(FUNC(cabal_state::irq0_line_hold), attotime::from_hz(8000));

	z80_device &adpcm_2(Z80(config, "adpcm_2", XTAL(3'579'545))); /* verified on pcb */
	adpcm_2.set_addrmap(AS_PROGRAM, &cabal_state::cabalbl_talk2_map);
	adpcm_2.set_addrmap(AS_IO, &cabal_state::cabalbl_talk2_portmap);
	adpcm_2.set_periodic_int(FUNC(cabal_state::irq0_line_hold), attotime::from_hz(8000));

	config.m_minimum_quantum = attotime::from_hz(600);

	MCFG_MACHINE_START_OVERRIDE(cabal_state,cabalbl)
	MCFG_MACHINE_RESET_OVERRIDE(cabal_state,cabalbl)

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	screen.set_size(256, 256);
	screen.set_visarea(0*8, 32*8-1, 2*8, 30*8-1);
	screen.set_screen_update(FUNC(cabal_state::screen_update));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_cabal);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_444, 1024);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, "soundlatch");
	GENERIC_LATCH_8(config, "soundlatch2");
	GENERIC_LATCH_8(config, "soundlatch3");

	ym2151_device &ymsnd(YM2151(config, "ymsnd", XTAL(3'579'545))); /* verified on pcb */
	ymsnd.irq_handler().set_inputline(m_audiocpu, 0);
	ymsnd.add_route(ALL_OUTPUTS, "mono", 0.80);

	MSM5205(config, m_msm1, XTAL(12'000'000)/32); /* verified on pcb (no resonator) */
	m_msm1->set_prescaler_selector(msm5205_device::SEX_4B);
	m_msm1->add_route(ALL_OUTPUTS, "mono", 0.60);

	MSM5205(config, m_msm2, XTAL(12'000'000)/32); /* verified on pcb (no resonator)*/
	m_msm2->set_prescaler_selector(msm5205_device::SEX_4B);
	m_msm2->add_route(ALL_OUTPUTS, "mono", 0.60);
}

ROM_START( cabal )
	ROM_REGION( 0x50000, "maincpu", 0 ) /* 64k for cpu code */
	ROM_LOAD16_BYTE( "13.7h",    0x00000, 0x10000, CRC(00abbe0c) SHA1(bacf17444abfb4f56248ff56e37b0aa2b1a3800d) )
	ROM_LOAD16_BYTE( "11.6h",    0x00001, 0x10000, CRC(44736281) SHA1(1d6da95ef96d9c02aea70791e1cb87b70097d5ed) )
	ROM_LOAD16_BYTE( "12.7j",    0x20000, 0x10000, CRC(d763a47c) SHA1(146d8082a404b6eddaf2dc9ba41a997949c17f8a) )
	ROM_LOAD16_BYTE( "10.6j",    0x20001, 0x10000, CRC(96d5e8af) SHA1(ed7d854f08e87db5ae6cf526eafa029dfd2bfb9f) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound cpu code */
	ROM_LOAD( "4-3n",         0x0000, 0x2000, CRC(4038eff2) SHA1(0bcafc1b78c3bef9a0e9b822c482ea4a942fd180) )
	ROM_LOAD( "3-3p",         0x8000, 0x8000, CRC(d9defcbf) SHA1(f26b10b1dbe5aa6446f70fd18e5f1379455578ec) )

	ROM_REGION( 0x4000,  "gfx1", 0 )
	ROM_LOAD( "5-6s",           0x00000, 0x04000, CRC(6a76955a) SHA1(733cb4b862b5dac97c2641b58f2362471e62fcf2) ) /* characters */

	/* The Joystick versions use a sub-board instead of the mask roms
	   the content is the same as the mask roms */
	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "bg_rom1.bin",   0x00000, 0x10000, CRC(1023319b) SHA1(38fcc8159776b82779b3163329b07c61be939fae) )
	ROM_LOAD16_BYTE( "bg_rom2.bin",   0x00001, 0x10000, CRC(3b6d2b09) SHA1(4cdcd22836dce4ee6348c4e6df7c6360d12ef912) )
	ROM_LOAD16_BYTE( "bg_rom3.bin",   0x20000, 0x10000, CRC(420b0801) SHA1(175be6e3ca3cb98672e4cdbc9b5f5b007bc531c9) )
	ROM_LOAD16_BYTE( "bg_rom4.bin",   0x20001, 0x10000, CRC(77bc7a60) SHA1(4d148241835f6a6b63f66494636c09a1fc1d3c06) )
	ROM_LOAD16_BYTE( "bg_rom5.bin",   0x40000, 0x10000, CRC(543fcb37) SHA1(78c40f6a78a8b9ca9f73fc67fc87f78b15e7abbe) )
	ROM_LOAD16_BYTE( "bg_rom6.bin",   0x40001, 0x10000, CRC(0bc50075) SHA1(565eb59b41f71fb69f62397f9747f5ae18b83009) )
	ROM_LOAD16_BYTE( "bg_rom7.bin",   0x60000, 0x10000, CRC(d28d921e) SHA1(e133de5129a33ca9ff449948a959621bbfc58c11) )
	ROM_LOAD16_BYTE( "bg_rom8.bin",   0x60001, 0x10000, CRC(67e4fe47) SHA1(15620fc5e985a249677da333b77331e40d2b24ab) )

	ROM_REGION( 0x80000, "gfx3", 0 )
	ROM_LOAD16_BYTE( "sp_rom1.bin",   0x00000, 0x10000, CRC(34d3cac8) SHA1(a6a2304fb576267db2c72cfbf0a3f66740ebe60e) )
	ROM_LOAD16_BYTE( "sp_rom2.bin",   0x00001, 0x10000, CRC(4e49c28e) SHA1(ea74443a9423b14611a1f97e44692badfedd0ead) )
	ROM_LOAD16_BYTE( "sp_rom3.bin",   0x20000, 0x10000, CRC(7065e840) SHA1(baa8cd28be60c678d782ecfabde6cd5e36480415) )
	ROM_LOAD16_BYTE( "sp_rom4.bin",   0x20001, 0x10000, CRC(6a0e739d) SHA1(e3f4f5b4587f573426ec00417f33e94a257c77e6) )
	ROM_LOAD16_BYTE( "sp_rom5.bin",   0x40000, 0x10000, CRC(0e1ec30e) SHA1(4b1f092fc1e92da0f92e55d1548db7961a13f717) )
	ROM_LOAD16_BYTE( "sp_rom6.bin",   0x40001, 0x10000, CRC(581a50c1) SHA1(5afd65c15a0a63a54727e6d882011f0718a9fefc) )
	ROM_LOAD16_BYTE( "sp_rom7.bin",   0x60000, 0x10000, CRC(55c44764) SHA1(7fad1f2084664b5b4d1384c8081371b0c79c4f5e) )
	ROM_LOAD16_BYTE( "sp_rom8.bin",   0x60001, 0x10000, CRC(702735c9) SHA1(e4ac799dc85ff5b7c8e578611605989c78f9e8b3) )

	ROM_REGION( 0x10000, "adpcm1", 0 )  /* Samples */
	ROM_LOAD( "2-1s",           0x00000, 0x10000, CRC(850406b4) SHA1(23ac1650c6d6f35607a5264b3aa89868401a645a) )

	ROM_REGION( 0x10000, "adpcm2", 0 )  /* Samples */
	ROM_LOAD( "1-1u",           0x00000, 0x10000, CRC(8b3e0789) SHA1(b1450db1b1bada237c90930623e4def321099f13) )
ROM_END

ROM_START( cabala )
	ROM_REGION( 0x50000, "maincpu", 0 ) /* 64k for cpu code */
	ROM_LOAD16_BYTE( "epr-a-9.7h",    0x00000, 0x10000, CRC(00abbe0c) SHA1(bacf17444abfb4f56248ff56e37b0aa2b1a3800d) )
	ROM_LOAD16_BYTE( "epr-a-7.6h",    0x00001, 0x10000, CRC(c89608db) SHA1(a56e77526227af5b693eea9ef74da0d9d57cc55c) )
	ROM_LOAD16_BYTE( "epr-a-8.7k",    0x20000, 0x08000, CRC(fe84788a) SHA1(29c49ebbe62357c27befcdcc4c19841a8bf32b2d) )
	ROM_RELOAD(0x30000,0x08000)
	ROM_LOAD16_BYTE( "epr-a-6.6k",    0x20001, 0x08000, CRC(81eb1355) SHA1(bbf926d40164d78319e982da0e8fb8ec4d4f8b87) )
	ROM_RELOAD(0x30001,0x08000)

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound cpu code */
	ROM_LOAD( "epr-a-4.3n",         0x0000, 0x2000, CRC(4038eff2) SHA1(0bcafc1b78c3bef9a0e9b822c482ea4a942fd180) )
	ROM_LOAD( "epr-a-3.3p",         0x8000, 0x4000, CRC(c0097c55) SHA1(874f813c1b466dab2d15a707e340b9bdb200246c) )

	ROM_REGION( 0x8000,  "gfx1", 0 )
	ROM_LOAD( "epr-a-5.6s",           0x00000, 0x08000, CRC(189033fd) SHA1(814f0cbc5f72345c04922d6d7c986f99d57335fa) ) /* characters */

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD( "tad-2.7s",       0x00000, 0x80000, CRC(13ca7ae1) SHA1(b26bb4876a6518e3809e0fa4d442616508b3e7e8) ) /* tiles */

	ROM_REGION( 0x80000, "gfx3", 0 )
	ROM_LOAD( "tad-1.5e",       0x00000, 0x80000, CRC(8324a7fe) SHA1(aed4470df35ec18e65e35bddc9c217a5019fdcbf) ) /* sprites */

	ROM_REGION( 0x10000, "adpcm1", 0 )  /* Samples */
	ROM_LOAD( "epr-a-2.1s",           0x00000, 0x10000, CRC(850406b4) SHA1(23ac1650c6d6f35607a5264b3aa89868401a645a) )

	ROM_REGION( 0x10000, "adpcm2", 0 )  /* Samples */
	ROM_LOAD( "epr-a-1.1u",           0x00000, 0x10000, CRC(8b3e0789) SHA1(b1450db1b1bada237c90930623e4def321099f13) )
ROM_END



ROM_START( cabaluk )
	ROM_REGION( 0x50000, "maincpu", 0 ) /* 64k for cpu code */
	ROM_LOAD16_BYTE( "9-7h.bin",    0x00000, 0x10000, CRC(f66378e5) SHA1(b3802f24863f857506ae1aeddc4e5c2908810695) )
	ROM_LOAD16_BYTE( "7-6h.bin",    0x00001, 0x10000, CRC(960991ac) SHA1(7e3ab0673585424206d791e8b0ed6af38e2ae8a9) )
	ROM_LOAD16_BYTE( "8-7k.bin",    0x20000, 0x10000, CRC(82160ab0) SHA1(a486f30ec3068025b690da4c1ae7295e79e7cd74) )
	ROM_LOAD16_BYTE( "6-6k.bin",    0x20001, 0x10000, CRC(7ef2ecc7) SHA1(43d621e2e7cfea8d906a968047817e23a3e4d047) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound cpu code */
	ROM_LOAD( "4-3n",         0x0000, 0x2000, CRC(4038eff2) SHA1(0bcafc1b78c3bef9a0e9b822c482ea4a942fd180) )
	ROM_LOAD( "3-3p",         0x8000, 0x8000, CRC(d9defcbf) SHA1(f26b10b1dbe5aa6446f70fd18e5f1379455578ec) )

	ROM_REGION( 0x4000,  "gfx1", 0 )
	ROM_LOAD( "5-6s",           0x00000, 0x04000, CRC(6a76955a) SHA1(733cb4b862b5dac97c2641b58f2362471e62fcf2) ) /* characters */

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD( "tad-2.7s",       0x00000, 0x80000, CRC(13ca7ae1) SHA1(b26bb4876a6518e3809e0fa4d442616508b3e7e8) ) /* tiles */

	ROM_REGION( 0x80000, "gfx3", 0 )
	ROM_LOAD( "tad-1.5e",       0x00000, 0x80000, CRC(8324a7fe) SHA1(aed4470df35ec18e65e35bddc9c217a5019fdcbf) ) /* sprites */


	ROM_REGION( 0x10000, "adpcm1", 0 )  /* Samples */
	ROM_LOAD( "2-1s",           0x00000, 0x10000, CRC(850406b4) SHA1(23ac1650c6d6f35607a5264b3aa89868401a645a) )

	ROM_REGION( 0x10000, "adpcm2", 0 )  /* Samples */
	ROM_LOAD( "1-1u",           0x00000, 0x10000, CRC(8b3e0789) SHA1(b1450db1b1bada237c90930623e4def321099f13) )
ROM_END

ROM_START( cabalukj )
	ROM_REGION( 0x50000, "maincpu", 0 ) /* 64k for cpu code */
	ROM_LOAD16_BYTE( "13.7h",    0x00000, 0x10000, CRC(00abbe0c) SHA1(bacf17444abfb4f56248ff56e37b0aa2b1a3800d) )
	ROM_LOAD16_BYTE( "14.6h",    0x00001, 0x10000, CRC(5b04b101) SHA1(fc58b3a3854dbbf65251486a009035060349a66c) )
	ROM_LOAD16_BYTE( "12.7j",    0x20000, 0x10000, CRC(d763a47c) SHA1(146d8082a404b6eddaf2dc9ba41a997949c17f8a) )
	ROM_LOAD16_BYTE( "10.6j",    0x20001, 0x10000, CRC(96d5e8af) SHA1(ed7d854f08e87db5ae6cf526eafa029dfd2bfb9f) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound cpu code */
	ROM_LOAD( "4-3n",         0x0000, 0x2000, CRC(4038eff2) SHA1(0bcafc1b78c3bef9a0e9b822c482ea4a942fd180) )
	ROM_LOAD( "3-3p",         0x8000, 0x8000, CRC(d9defcbf) SHA1(f26b10b1dbe5aa6446f70fd18e5f1379455578ec) )

	ROM_REGION( 0x4000,  "gfx1", 0 )
	ROM_LOAD( "5-6s",           0x00000, 0x04000, CRC(6a76955a) SHA1(733cb4b862b5dac97c2641b58f2362471e62fcf2) ) /* characters */

	/* The Joystick versions use a sub-board instead of the mask roms
	   the content is the same as the mask roms */
	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "bg_rom1.bin",   0x00000, 0x10000, CRC(1023319b) SHA1(38fcc8159776b82779b3163329b07c61be939fae) )
	ROM_LOAD16_BYTE( "bg_rom2.bin",   0x00001, 0x10000, CRC(3b6d2b09) SHA1(4cdcd22836dce4ee6348c4e6df7c6360d12ef912) )
	ROM_LOAD16_BYTE( "bg_rom3.bin",   0x20000, 0x10000, CRC(420b0801) SHA1(175be6e3ca3cb98672e4cdbc9b5f5b007bc531c9) )
	ROM_LOAD16_BYTE( "bg_rom4.bin",   0x20001, 0x10000, CRC(77bc7a60) SHA1(4d148241835f6a6b63f66494636c09a1fc1d3c06) )
	ROM_LOAD16_BYTE( "bg_rom5.bin",   0x40000, 0x10000, CRC(543fcb37) SHA1(78c40f6a78a8b9ca9f73fc67fc87f78b15e7abbe) )
	ROM_LOAD16_BYTE( "bg_rom6.bin",   0x40001, 0x10000, CRC(0bc50075) SHA1(565eb59b41f71fb69f62397f9747f5ae18b83009) )
	ROM_LOAD16_BYTE( "bg_rom7.bin",   0x60000, 0x10000, CRC(d28d921e) SHA1(e133de5129a33ca9ff449948a959621bbfc58c11) )
	ROM_LOAD16_BYTE( "bg_rom8.bin",   0x60001, 0x10000, CRC(67e4fe47) SHA1(15620fc5e985a249677da333b77331e40d2b24ab) )

	ROM_REGION( 0x80000, "gfx3", 0 )
	ROM_LOAD16_BYTE( "sp_rom1.bin",   0x00000, 0x10000, CRC(34d3cac8) SHA1(a6a2304fb576267db2c72cfbf0a3f66740ebe60e) )
	ROM_LOAD16_BYTE( "sp_rom2.bin",   0x00001, 0x10000, CRC(4e49c28e) SHA1(ea74443a9423b14611a1f97e44692badfedd0ead) )
	ROM_LOAD16_BYTE( "sp_rom3.bin",   0x20000, 0x10000, CRC(7065e840) SHA1(baa8cd28be60c678d782ecfabde6cd5e36480415) )
	ROM_LOAD16_BYTE( "sp_rom4.bin",   0x20001, 0x10000, CRC(6a0e739d) SHA1(e3f4f5b4587f573426ec00417f33e94a257c77e6) )
	ROM_LOAD16_BYTE( "sp_rom5.bin",   0x40000, 0x10000, CRC(0e1ec30e) SHA1(4b1f092fc1e92da0f92e55d1548db7961a13f717) )
	ROM_LOAD16_BYTE( "sp_rom6.bin",   0x40001, 0x10000, CRC(581a50c1) SHA1(5afd65c15a0a63a54727e6d882011f0718a9fefc) )
	ROM_LOAD16_BYTE( "sp_rom7.bin",   0x60000, 0x10000, CRC(55c44764) SHA1(7fad1f2084664b5b4d1384c8081371b0c79c4f5e) )
	ROM_LOAD16_BYTE( "sp_rom8.bin",   0x60001, 0x10000, CRC(702735c9) SHA1(e4ac799dc85ff5b7c8e578611605989c78f9e8b3) )

	ROM_REGION( 0x10000, "adpcm1", 0 )  /* Samples */
	ROM_LOAD( "2-1s",           0x00000, 0x10000, CRC(850406b4) SHA1(23ac1650c6d6f35607a5264b3aa89868401a645a) )

	ROM_REGION( 0x10000, "adpcm2", 0 )  /* Samples */
	ROM_LOAD( "1-1u",           0x00000, 0x10000, CRC(8b3e0789) SHA1(b1450db1b1bada237c90930623e4def321099f13) )
ROM_END

ROM_START( cabalus )
	ROM_REGION( 0x50000, "maincpu", 0 ) /* 64k for cpu code */
	ROM_LOAD16_BYTE( "h7_512.bin",      0x00000, 0x10000, CRC(8fe16fb4) SHA1(fedb2d0c6c21516f68cfa99093772fe8fa862389) )
	ROM_LOAD16_BYTE( "h6_512.bin",      0x00001, 0x10000, CRC(6968101c) SHA1(d65005ac235dae5c32bbcd182cb365e8fa067fe7) )
	ROM_LOAD16_BYTE( "k7_512.bin",      0x20000, 0x10000, CRC(562031a2) SHA1(ed5ef50a66c7797a7f345e479162cf83d6777f7c) )
	ROM_LOAD16_BYTE( "k6_512.bin",      0x20001, 0x10000, CRC(4fda2856) SHA1(a213cb7443cdccbad3f2610e8d42b2e149cbedb9) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound cpu code */
	ROM_LOAD( "4-3n",         0x0000, 0x2000, CRC(4038eff2) SHA1(0bcafc1b78c3bef9a0e9b822c482ea4a942fd180) )
	ROM_LOAD( "3-3p",         0x8000, 0x8000, CRC(d9defcbf) SHA1(f26b10b1dbe5aa6446f70fd18e5f1379455578ec) )

	ROM_REGION( 0x4000,  "gfx1", 0 )
	ROM_LOAD( "t6_128.bin",     0x00000, 0x04000, CRC(1ccee214) SHA1(7c842bc1c6002ec90693160fd5407345092420bb) ) /* characters */

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD( "tad-2.7s",       0x00000, 0x80000, CRC(13ca7ae1) SHA1(b26bb4876a6518e3809e0fa4d442616508b3e7e8) ) /* tiles */

	ROM_REGION( 0x80000, "gfx3", 0 )
	ROM_LOAD( "tad-1.5e",       0x00000, 0x80000, CRC(8324a7fe) SHA1(aed4470df35ec18e65e35bddc9c217a5019fdcbf) ) /* sprites */

	ROM_REGION( 0x10000, "adpcm1", 0 )  /* Samples? */
	ROM_LOAD( "2-1s",           0x00000, 0x10000, CRC(850406b4) SHA1(23ac1650c6d6f35607a5264b3aa89868401a645a) )

	ROM_REGION( 0x10000, "adpcm2", 0 )  /* Samples */
	ROM_LOAD( "1-1u",           0x00000, 0x10000, CRC(8b3e0789) SHA1(b1450db1b1bada237c90930623e4def321099f13) )

	ROM_REGION( 0x0200, "proms", 0 )    /* unknown */
	ROM_LOAD( "prom05.8e",      0x0000, 0x0100, CRC(a94b18c2) SHA1(e7db4c1efc9e313e36eef3f53ae5b2e573a38920) )
	ROM_LOAD( "prom10.4j",      0x0100, 0x0100, CRC(261c93bc) SHA1(942470198143d584d3766f28587d1879abd912c1) )
ROM_END

ROM_START( cabalus2 )
	ROM_REGION( 0x50000, "maincpu", 0 ) /* 64k for cpu code */
	ROM_LOAD16_BYTE( "9-7h",            0x00000, 0x10000, CRC(ebbb9484) SHA1(2c77d5b4acdc37720dc7ccab526862981bf8da51) )
	ROM_LOAD16_BYTE( "7-6h",            0x00001, 0x10000, CRC(51aeb49e) SHA1(df38dc58d8c6fa3d35904bf34e29111e7bd523ad) )
	ROM_LOAD16_BYTE( "8-7k",            0x20000, 0x10000, CRC(4c24ed9a) SHA1(f0fc25c3e7dc8ac71fdad3e91ab618cd7a037123) )
	ROM_LOAD16_BYTE( "6-6k",            0x20001, 0x10000, CRC(681620e8) SHA1(c9eacfb55059986dbecc2fae1339069a852f917b) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound cpu code */
	ROM_LOAD( "4-3n",         0x0000, 0x2000, CRC(4038eff2) SHA1(0bcafc1b78c3bef9a0e9b822c482ea4a942fd180) )
	ROM_LOAD( "3-3p",         0x8000, 0x8000, CRC(d9defcbf) SHA1(f26b10b1dbe5aa6446f70fd18e5f1379455578ec) )

	ROM_REGION( 0x4000,  "gfx1", 0 )
	ROM_LOAD( "5-6s",           0x00000, 0x04000, CRC(6a76955a) SHA1(733cb4b862b5dac97c2641b58f2362471e62fcf2) ) /* characters */

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD( "tad-2.7s",       0x00000, 0x80000, CRC(13ca7ae1) SHA1(b26bb4876a6518e3809e0fa4d442616508b3e7e8) ) /* tiles */

	ROM_REGION( 0x80000, "gfx3", 0 )
	ROM_LOAD( "tad-1.5e",       0x00000, 0x80000, CRC(8324a7fe) SHA1(aed4470df35ec18e65e35bddc9c217a5019fdcbf) ) /* sprites */

	ROM_REGION( 0x10000, "adpcm1", 0 )  /* Samples */
	ROM_LOAD( "2-1s",           0x00000, 0x10000, CRC(850406b4) SHA1(23ac1650c6d6f35607a5264b3aa89868401a645a) )

	ROM_REGION( 0x10000, "adpcm2", 0 )  /* Samples */
	ROM_LOAD( "1-1u",           0x00000, 0x10000, CRC(8b3e0789) SHA1(b1450db1b1bada237c90930623e4def321099f13) )

	ROM_REGION( 0x0200, "proms", 0 )    /* unknown */
	ROM_LOAD( "prom05.8e",      0x0000, 0x0100, CRC(a94b18c2) SHA1(e7db4c1efc9e313e36eef3f53ae5b2e573a38920) )
	ROM_LOAD( "prom10.4j",      0x0100, 0x0100, CRC(261c93bc) SHA1(942470198143d584d3766f28587d1879abd912c1) )
ROM_END

/*

cabal - tad corporation ? - (clone)

2 boards

1st board

(prg)
1 x 68000
from cabal_21 to cabal_24

(snd)
1 x z80
1 x ym2151
cabal_11

(gfx ?)
from cabal_12 to cabal_19

(?)
2 x z80
cabal_09 and cabal_10

(?)
cabal_20 (near the snd area)

2nd board

(gfx)
from cabal_01 to cabal_08

Note: The bootleg has *3* Z80s

*/

ROM_START( cabalbl )
	ROM_REGION( 0x50000, "maincpu", 0 ) /* 64k for cpu code */
	ROM_LOAD16_BYTE( "cabal_24.bin",    0x00000, 0x10000, CRC(00abbe0c) SHA1(bacf17444abfb4f56248ff56e37b0aa2b1a3800d) )
	ROM_LOAD16_BYTE( "cabal_22.bin",    0x00001, 0x10000, CRC(78c4af27) SHA1(31049d1ec76d76284682de7a0592f63d97019240) )
	ROM_LOAD16_BYTE( "cabal_23.bin",    0x20000, 0x10000, CRC(d763a47c) SHA1(146d8082a404b6eddaf2dc9ba41a997949c17f8a) )
	ROM_LOAD16_BYTE( "cabal_21.bin",    0x20001, 0x10000, CRC(96d5e8af) SHA1(ed7d854f08e87db5ae6cf526eafa029dfd2bfb9f) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound cpu code */
	ROM_LOAD( "cabal_11.bin",    0x0000, 0x10000, CRC(d308a543) SHA1(4f45db42512f83266001daee55d06f49e7908e35) )

	ROM_REGION( 0x8000,  "gfx1", 0 )
	ROM_LOAD( "cabal_20.bin",           0x00000, 0x08000, CRC(189033fd) SHA1(814f0cbc5f72345c04922d6d7c986f99d57335fa) ) /* characters */

	/* The bootleg versions use a sub-board instead of the mask roms
	   the content is the same as the mask roms */
	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "cabal_15.bin",   0x00000, 0x10000, CRC(1023319b) SHA1(38fcc8159776b82779b3163329b07c61be939fae) )
	ROM_LOAD16_BYTE( "cabal_17.bin",   0x00001, 0x10000, CRC(3b6d2b09) SHA1(4cdcd22836dce4ee6348c4e6df7c6360d12ef912) )
	ROM_LOAD16_BYTE( "cabal_14.bin",   0x20000, 0x10000, CRC(420b0801) SHA1(175be6e3ca3cb98672e4cdbc9b5f5b007bc531c9) )
	ROM_LOAD16_BYTE( "cabal_16.bin",   0x20001, 0x10000, CRC(77bc7a60) SHA1(4d148241835f6a6b63f66494636c09a1fc1d3c06) )
	ROM_LOAD16_BYTE( "cabal_12.bin",   0x40000, 0x10000, CRC(543fcb37) SHA1(78c40f6a78a8b9ca9f73fc67fc87f78b15e7abbe) )
	ROM_LOAD16_BYTE( "cabal_18.bin",   0x40001, 0x10000, CRC(0bc50075) SHA1(565eb59b41f71fb69f62397f9747f5ae18b83009) )
	ROM_LOAD16_BYTE( "cabal_13.bin",   0x60000, 0x10000, CRC(d28d921e) SHA1(e133de5129a33ca9ff449948a959621bbfc58c11) )
	ROM_LOAD16_BYTE( "cabal_19.bin",   0x60001, 0x10000, CRC(67e4fe47) SHA1(15620fc5e985a249677da333b77331e40d2b24ab) )

	ROM_REGION( 0x80000, "gfx3", 0 )
	ROM_LOAD16_BYTE( "cabal_04.bin",   0x00000, 0x10000, CRC(34d3cac8) SHA1(a6a2304fb576267db2c72cfbf0a3f66740ebe60e) )
	ROM_LOAD16_BYTE( "cabal_05.bin",   0x00001, 0x10000, CRC(4e49c28e) SHA1(ea74443a9423b14611a1f97e44692badfedd0ead) )
	ROM_LOAD16_BYTE( "cabal_03.bin",   0x20000, 0x10000, CRC(7065e840) SHA1(baa8cd28be60c678d782ecfabde6cd5e36480415) )
	ROM_LOAD16_BYTE( "cabal_06.bin",   0x20001, 0x10000, CRC(6a0e739d) SHA1(e3f4f5b4587f573426ec00417f33e94a257c77e6) )
	ROM_LOAD16_BYTE( "cabal_02.bin",   0x40000, 0x10000, CRC(0e1ec30e) SHA1(4b1f092fc1e92da0f92e55d1548db7961a13f717) )
	ROM_LOAD16_BYTE( "cabal_07.bin",   0x40001, 0x10000, CRC(581a50c1) SHA1(5afd65c15a0a63a54727e6d882011f0718a9fefc) )
	ROM_LOAD16_BYTE( "cabal_01.bin",   0x60000, 0x10000, CRC(55c44764) SHA1(7fad1f2084664b5b4d1384c8081371b0c79c4f5e) )
	ROM_LOAD16_BYTE( "cabal_08.bin",   0x60001, 0x10000, CRC(702735c9) SHA1(e4ac799dc85ff5b7c8e578611605989c78f9e8b3) )

	ROM_REGION( 0x10000, "adpcm_1", 0 )
	ROM_LOAD( "cabal_09.bin",   0x00000, 0x10000, CRC(4ffa7fe3) SHA1(381d8e765a7b94678fb3308965c748bbe9f8e247) ) /* Z80 code/adpcm data */

	ROM_REGION( 0x10000, "adpcm_2", 0 )
	ROM_LOAD( "cabal_10.bin",   0x00000, 0x10000, CRC(958789b6) SHA1(344c3ee8a1e272b56499e5c0415bb714aec0ddcf) ) /* Z80 code/adpcm data */
ROM_END


// alternate bootleg
// this is much closer to the original, the only real difference is the soundcpu has been pre-decrypted,
// with the encrypted/decrypted data split across the rom
// based on stickers present on the board it appears to have been manufactured by 'TAB-Austria' and is marked 'CA02'

ROM_START( cabalbl2 )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 64k for cpu code */
	ROM_LOAD16_BYTE( "c9.bin",    0x00000, 0x10000, CRC(00abbe0c) SHA1(bacf17444abfb4f56248ff56e37b0aa2b1a3800d) )
	ROM_LOAD16_BYTE( "c7.bin",    0x00001, 0x10000, CRC(44736281) SHA1(1d6da95ef96d9c02aea70791e1cb87b70097d5ed) )
	ROM_LOAD16_BYTE( "c8.bin",    0x20000, 0x10000, CRC(d763a47c) SHA1(146d8082a404b6eddaf2dc9ba41a997949c17f8a) )
	ROM_LOAD16_BYTE( "c6.bin",    0x20001, 0x10000, CRC(96d5e8af) SHA1(ed7d854f08e87db5ae6cf526eafa029dfd2bfb9f) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for sound cpu code */
	ROM_LOAD( "c4.bin",    0x2000, 0x2000, CRC(82f9f296) SHA1(2769ffdc28f003684e77d4806be07b87d50be31c) )
	ROM_CONTINUE(0x0000,0x2000)
	ROM_IGNORE(0x4000)
	ROM_LOAD( "c3.bin",    0x8000, 0x8000,  CRC(d9defcbf) SHA1(f26b10b1dbe5aa6446f70fd18e5f1379455578ec) )

	ROM_REGION( 0x4000,  "gfx1", 0 )
	ROM_LOAD( "c5.bin",           0x00000, 0x04000, CRC(183e4834) SHA1(05ab0c388be8701930a9de437978206cda6fed68) ) /* characters */
	ROM_CONTINUE(0x0000,0x4000)

	ROM_REGION( 0x80000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "c14.bin",   0x00000, 0x10000, CRC(1023319b) SHA1(38fcc8159776b82779b3163329b07c61be939fae) )
	ROM_LOAD16_BYTE( "c10.bin",   0x00001, 0x10000, CRC(3b6d2b09) SHA1(4cdcd22836dce4ee6348c4e6df7c6360d12ef912) )
	ROM_LOAD16_BYTE( "c15.bin",   0x20000, 0x10000, CRC(420b0801) SHA1(175be6e3ca3cb98672e4cdbc9b5f5b007bc531c9) )
	ROM_LOAD16_BYTE( "c11.bin",   0x20001, 0x10000, CRC(77bc7a60) SHA1(4d148241835f6a6b63f66494636c09a1fc1d3c06) )
	ROM_LOAD16_BYTE( "c16.bin",   0x40000, 0x10000, CRC(543fcb37) SHA1(78c40f6a78a8b9ca9f73fc67fc87f78b15e7abbe) )
	ROM_LOAD16_BYTE( "c12.bin",   0x40001, 0x10000, CRC(0bc50075) SHA1(565eb59b41f71fb69f62397f9747f5ae18b83009) )
	ROM_LOAD16_BYTE( "c17.bin",   0x60000, 0x10000, CRC(d28d921e) SHA1(e133de5129a33ca9ff449948a959621bbfc58c11) )
	ROM_LOAD16_BYTE( "c13.bin",   0x60001, 0x10000, CRC(67e4fe47) SHA1(15620fc5e985a249677da333b77331e40d2b24ab) )

	ROM_REGION( 0x80000, "gfx3", 0 )
	ROM_LOAD16_BYTE( "c18.bin",   0x00000, 0x10000, CRC(34d3cac8) SHA1(a6a2304fb576267db2c72cfbf0a3f66740ebe60e) )
	ROM_LOAD16_BYTE( "c22.bin",   0x00001, 0x10000, CRC(4e49c28e) SHA1(ea74443a9423b14611a1f97e44692badfedd0ead) )
	ROM_LOAD16_BYTE( "c19.bin",   0x20000, 0x10000, CRC(7065e840) SHA1(baa8cd28be60c678d782ecfabde6cd5e36480415) )
	ROM_LOAD16_BYTE( "c23.bin",   0x20001, 0x10000, CRC(6a0e739d) SHA1(e3f4f5b4587f573426ec00417f33e94a257c77e6) )
	ROM_LOAD16_BYTE( "c20.bin",   0x40000, 0x10000, CRC(0e1ec30e) SHA1(4b1f092fc1e92da0f92e55d1548db7961a13f717) )
	ROM_LOAD16_BYTE( "c24.bin",   0x40001, 0x10000, CRC(581a50c1) SHA1(5afd65c15a0a63a54727e6d882011f0718a9fefc) )
	ROM_LOAD16_BYTE( "c21.bin",   0x60000, 0x10000, CRC(55c44764) SHA1(7fad1f2084664b5b4d1384c8081371b0c79c4f5e) )
	ROM_LOAD16_BYTE( "c25.bin",   0x60001, 0x10000, CRC(702735c9) SHA1(e4ac799dc85ff5b7c8e578611605989c78f9e8b3) )

	ROM_REGION( 0x10000, "adpcm1", 0 )  /* Samples */
	ROM_LOAD( "c2.bin",           0x00000, 0x10000, CRC(850406b4) SHA1(23ac1650c6d6f35607a5264b3aa89868401a645a) )

	ROM_REGION( 0x10000, "adpcm2", 0 )  /* Samples */
	ROM_LOAD( "c1.bin",           0x00000, 0x10000, CRC(8b3e0789) SHA1(b1450db1b1bada237c90930623e4def321099f13) )
ROM_END


void cabal_state::init_cabal()
{
	m_adpcm1->decrypt();
	m_adpcm2->decrypt();
}


GAME( 1988, cabal,    0,     cabal,    cabalj,  cabal_state, init_cabal, ROT0, "TAD Corporation",                         "Cabal (World, Joystick)", MACHINE_SUPPORTS_SAVE )
GAME( 1989, cabala,   cabal, cabal,    cabalj,  cabal_state, init_cabal, ROT0, "TAD Corporation (Alpha Trading license)", "Cabal (Korea?, Joystick)", MACHINE_SUPPORTS_SAVE ) // korea?
GAME( 1989, cabalukj, cabal, cabal,    cabalj,  cabal_state, init_cabal, ROT0, "TAD Corporation (Electrocoin license)",   "Cabal (UK, Joystick)", MACHINE_SUPPORTS_SAVE )
GAME( 1988, cabalbl,  cabal, cabalbl,  cabalbl, cabal_state, empty_init, ROT0, "bootleg (Red Corporation)",               "Cabal (bootleg of Joystick version, set 1, alternate sound hardware)", MACHINE_IMPERFECT_SOUND | MACHINE_SUPPORTS_SAVE )
GAME( 1988, cabalbl2, cabal, cabalbl2, cabalj,  cabal_state, init_cabal, ROT0, "bootleg",                                 "Cabal (bootleg of Joystick version, set 2)", MACHINE_SUPPORTS_SAVE )

GAME( 1988, cabalus,  cabal, cabalt,   cabalt,  cabal_state, init_cabal, ROT0, "TAD Corporation (Fabtek license)",        "Cabal (US set 1, Trackball)", MACHINE_SUPPORTS_SAVE )
GAME( 1988, cabalus2, cabal, cabalt,   cabalt,  cabal_state, init_cabal, ROT0, "TAD Corporation (Fabtek license)",        "Cabal (US set 2, Trackball)", MACHINE_SUPPORTS_SAVE )
GAME( 1989, cabaluk,  cabal, cabalt,   cabalt,  cabal_state, init_cabal, ROT0, "TAD Corporation (Electrocoin license)",   "Cabal (UK, Trackball)", MACHINE_SUPPORTS_SAVE )
