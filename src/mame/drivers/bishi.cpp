// license:BSD-3-Clause
// copyright-holders:R. Belmont
/***************************************************************************

 Bishi Bashi Champ Mini Game Senshuken (c) 1996 Konami
 Super Bishi Bashi Championship        (c) 1998 Konami

 Driver by R. Belmont

 WORKING: ram/rom test passes, IRQs, sound/music, inputs, colors.
 TODO: "bishi" needs a ROM redumped, K056832 needs per-tile priority


*****************************************************************************


Super Bishi Bashi Champ (Korean version)
Konami, 1998

[Identical]
Bishi Bashi Champ (Korean version)
Konami, 1996

PCB Layout                     ROM
----------              Daughterboard (on top)
                                ||
GS562 PWB(A)400625A             \/
|----------------------|-------------------|
|   VOL SM5875 675KAA01|24MHz  MB3790  CN4 |
|16.9344MHz            |                   |
|   YMZ280B    675KAA02|  6264             |
|  056232              |                CN3|
|              675KAA03|  6264    68000    |
|                      |                   |
|              675KAA04|            62256  |
|            PAL       |                   |
|CN1         DSW1(8)   |  058143    62256  |
|  056879    DSW2(8)   |--------------------
|                                   62256  |
|                                675KAA07  |
|                         056832           |
|                                675KAA09  |
|         6264                             |
|                                675KAA08  |
|         6264            055555           |
|  056766                        675KAA10  |
|056820   6264                             |
|                                          |
|------------------------------------------|
Notes:
      68000  - clock 12.000MHz (24/2)
      YMZ280 - clock 16.9344MHz
      CN3/4  - connector for ROM daugterboard
      CN1    - large flat cable connector for power/controls
      675KAA01 to 04 - 27C040 EPROMs
      675KAA07 to 10 - 27C240 EPROMs

      Konami Custom ICs -
                          056143 (QFP160)
                          056832 (QFP144)
                          055555 (QFP240)
                          056879 (QFP144)
                          056232 (ceramic SIL14)
                          056820 (ceramic SIL13)


Daughterboard
-------------
PWB 402005(B) ROM BOARD
|-------------------|
|CN7 4AK16     CN8  |
|              PAL  |
|                   |
|CN5 4AK16          |
|                   |
|         675KAA05  |
|    4AK16          |
|CN6      675KAA06  |
|-------------------|
Notes:
      CN5/6 - 15 pin connector for lights maybe?
      CN7/8 - 8 pin connector for power
      4AK16 - power amp IC tied to CN5/6
      675*  - 27C240 EPROMs

***************************************************************************/

#include "emu.h"
#include "includes/bishi.h"

#include "cpu/m68000/m68000.h"
#include "sound/ymz280b.h"
#include "speaker.h"


READ16_MEMBER(bishi_state::control_r)
{
	return m_cur_control;
}

WRITE16_MEMBER(bishi_state::control_w)
{
	// bit 8 = interrupt gate
	COMBINE_DATA(&m_cur_control);
}

WRITE16_MEMBER(bishi_state::control2_w)
{
	// bit 12 = part of the banking calculation for the K056832 ROM readback
	COMBINE_DATA(&m_cur_control2);
}

TIMER_DEVICE_CALLBACK_MEMBER(bishi_state::bishi_scanline)
{
	int scanline = param;

	if (m_cur_control & 0x800)
	{
		if(scanline == 240) // vblank-out irq
			m_maincpu->set_input_line(M68K_IRQ_3, HOLD_LINE);

		if(scanline == 0) // vblank-in irq
			m_maincpu->set_input_line(M68K_IRQ_4, HOLD_LINE);
	}
}

/* compensate for a bug in the ram/rom test */
READ16_MEMBER(bishi_state::bishi_mirror_r)
{
	return m_palette->basemem().read16(offset);
}

READ16_MEMBER(bishi_state::bishi_K056832_rom_r)
{
	uint16_t ouroffs;

	ouroffs = (offset >> 1) * 8;
	if (offset & 1)
		ouroffs++;

	if (m_cur_control2 & 0x1000)
		ouroffs += 4;

	return m_k056832->bishi_rom_word_r(ouroffs);
}

void bishi_state::main_map(address_map &map)
{
	map(0x000000, 0x0fffff).rom();
	map(0x400000, 0x407fff).ram();                     // Work RAM
	map(0x800000, 0x800001).rw(FUNC(bishi_state::control_r), FUNC(bishi_state::control_w));
	map(0x800004, 0x800005).portr("DSW");
	map(0x800006, 0x800007).portr("SYSTEM");
	map(0x800008, 0x800009).portr("INPUTS");
	map(0x810000, 0x810003).w(FUNC(bishi_state::control2_w));       // bank switch for K056832 character ROM test
	map(0x820000, 0x820001).nopw();            // lamps (see lamp test in service menu)
	map(0x830000, 0x83003f).w(m_k056832, FUNC(k056832_device::word_w));
	map(0x840000, 0x840007).w(m_k056832, FUNC(k056832_device::b_word_w));    // VSCCS
	map(0x850000, 0x85001f).w(m_k054338, FUNC(k054338_device::word_w));  // CLTC
	map(0x870000, 0x8700ff).w(m_k055555, FUNC(k055555_device::K055555_word_w));  // PCU2
	map(0x880000, 0x880003).rw("ymz", FUNC(ymz280b_device::read), FUNC(ymz280b_device::write)).umask16(0xff00);
	map(0xa00000, 0xa01fff).rw(m_k056832, FUNC(k056832_device::ram_word_r), FUNC(k056832_device::ram_word_w));  // Graphic planes
	map(0xb00000, 0xb03fff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0xb04000, 0xb047ff).r(FUNC(bishi_state::bishi_mirror_r));    // bug in the ram/rom test?
	map(0xc00000, 0xc01fff).r(FUNC(bishi_state::bishi_K056832_rom_r));
}

static INPUT_PORTS_START( bishi )
	/* Currently, this "IN0" is not read */
	// TODO: leftover?
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE4 )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0007, 0x0004, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0007, DEF_STR( Easiest ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(      0x0005, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Medium ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( Medium_Hard ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Very_Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0038, 0x0028, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0038, "1" )
	PORT_DIPSETTING(      0x0030, "2" )
	PORT_DIPSETTING(      0x0028, "3" )
	PORT_DIPSETTING(      0x0020, "4" )
	PORT_DIPSETTING(      0x0018, "5" )
	PORT_DIPSETTING(      0x0010, "6" )
	PORT_DIPSETTING(      0x0008, "7" )
	PORT_DIPSETTING(      0x0000, "8" )
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x00c0, "All The Time" )
	PORT_DIPSETTING(      0x0080, "Loop At 2 Times" )
	PORT_DIPSETTING(      0x0040, "Loop At 4 Times" )
	PORT_DIPSETTING(      0x0000, "No Sounds" )
	PORT_DIPNAME( 0x0f00, 0x0f00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0500, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(      0x0f00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_5C ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(      0x0700, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0600, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(      0x0d00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0b00, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0a00, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0900, DEF_STR( 1C_7C ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Free_Play ))
	PORT_DIPSETTING(      0x1000, DEF_STR(No))
	PORT_DIPSETTING(      0x0000, DEF_STR(Yes))
	PORT_DIPNAME( 0x2000, 0x2000, "Slack Difficulty")
	PORT_DIPSETTING(      0x2000, DEF_STR(Off))
	PORT_DIPSETTING(      0x0000, DEF_STR(On))
	PORT_DIPNAME( 0x4000, 0x0000, "Title Demo")
	PORT_DIPSETTING(      0x4000, "At 1 Loop")
	PORT_DIPSETTING(      0x0000, "At Every Gamedemo")
	PORT_DIPNAME( 0x8000, 0x0000, "Gamedemo")
	PORT_DIPSETTING(      0x8000, "4 Kinds")
	PORT_DIPSETTING(      0x0000, "7 Kinds")

	PORT_START("SYSTEM")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3) PORT_NAME("P3 Blue")
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3) PORT_NAME("P3 Green")
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3) PORT_NAME("P3 Red")
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_SERVICE_NO_TOGGLE(0x0040, IP_ACTIVE_LOW)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_NAME("P1 Blue")
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("P1 Green")
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("P1 Red")
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_NAME("P2 Blue")
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("P2 Green")
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("P2 Red")
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )
INPUT_PORTS_END

/* The game will respond to the 'player 2' inputs from the normal
   input define if mapped, however, the game will function in an abnormal way
   as the game code isn't designed to handle it.  The 'player 3' inputs from
   the above input define are the actual ones used for player 2 on this */
static INPUT_PORTS_START( bishi2p )
	/* Currently, this "IN0" is not read */
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE4 )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0007, 0x0004, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0007, DEF_STR( Easiest ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( Very_Easy) )
	PORT_DIPSETTING(      0x0005, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Medium ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( Medium_Hard ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Very_Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0038, 0x0028, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0038, "1" )
	PORT_DIPSETTING(      0x0030, "2" )
	PORT_DIPSETTING(      0x0028, "3" )
	PORT_DIPSETTING(      0x0020, "4" )
	PORT_DIPSETTING(      0x0018, "5" )
	PORT_DIPSETTING(      0x0010, "6" )
	PORT_DIPSETTING(      0x0008, "7" )
	PORT_DIPSETTING(      0x0000, "8" )
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x00c0, "All The Time" )
	PORT_DIPSETTING(      0x0080, "Loop At 2 Times" )
	PORT_DIPSETTING(      0x0040, "Loop At 4 Times" )
	PORT_DIPSETTING(      0x0000, "No Sounds" )
	PORT_DIPNAME( 0x0f00, 0x0f00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0500, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(      0x0f00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_5C ) )
	PORT_DIPSETTING(      0x0300, DEF_STR( 3C_4C ) )
	PORT_DIPSETTING(      0x0700, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0600, DEF_STR( 2C_5C ) )
	PORT_DIPSETTING(      0x0d00, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0b00, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0a00, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(      0x0900, DEF_STR( 1C_7C ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Free_Play ))
	PORT_DIPSETTING(      0x1000, DEF_STR(No))
	PORT_DIPSETTING(      0x0000, DEF_STR(Yes))
	PORT_DIPNAME( 0x2000, 0x2000, "Slack Difficulty")
	PORT_DIPSETTING(      0x2000, DEF_STR(Off))
	PORT_DIPSETTING(      0x0000, DEF_STR(On))
	PORT_DIPNAME( 0x4000, 0x0000, "Title Demo")
	PORT_DIPSETTING(      0x4000, "At 1 Loop")
	PORT_DIPSETTING(      0x0000, "At Every Gamedemo")
	PORT_DIPNAME( 0x8000, 0x0000, "Gamedemo")
	PORT_DIPSETTING(      0x8000, "4 Kinds")
	PORT_DIPSETTING(      0x0000, "7 Kinds")

	PORT_START("SYSTEM")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("INPUTS")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_NAME("P2 Blue")
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("P2 Green")
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("P2 Red")
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_SERVICE_NO_TOGGLE(0x0040, IP_ACTIVE_LOW)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_NAME("P1 Blue")
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("P1 Green")
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("P1 Red")
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNUSED ) // 'p2' A
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNUSED ) // 'p2' B
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNUSED ) // 'p2' C
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED ) // 'p2' START
INPUT_PORTS_END

static INPUT_PORTS_START( dobouchn )
	PORT_INCLUDE( bishi )

	PORT_MODIFY("IN0")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("INPUTS")
	PORT_BIT( 0x001f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Shoot")
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_MODIFY("SYSTEM")
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("M. Ack") // ???
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_NAME("Coin 2 (Medal)")
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0xf000, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(    0x00, "5 Coins / 2 Credits" )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPUNUSED_DIPLOC( 0x08, 0x08, "SW1:4" )
	PORT_DIPNAME( 0x70, 0x70, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:5,6,7")
	PORT_DIPSETTING(    0x00, "15 Medals" )
	PORT_DIPSETTING(    0x10, "13 Medals" )
	PORT_DIPSETTING(    0x20, "11 Medals" )
	PORT_DIPSETTING(    0x30, "9 Medals" )
	PORT_DIPSETTING(    0x40, "7 Medals" )
	PORT_DIPSETTING(    0x50, "5 Medals" )
	PORT_DIPSETTING(    0x60, "3 Medals" )
	PORT_DIPSETTING(    0x70, "2 Medals" )
	PORT_DIPUNUSED_DIPLOC( 0x80, 0x80, "SW1:8" )

	PORT_DIPNAME( 0x0f00, 0x0f00, "Payout" ) PORT_DIPLOCATION("SW2:1,2,3,4")
	PORT_DIPSETTING(    0x000, "90%" )
	PORT_DIPSETTING(    0x100, "85%" )
	PORT_DIPSETTING(    0x200, "80%" )
	PORT_DIPSETTING(    0x300, "75%" )
	PORT_DIPSETTING(    0x400, "70%" )
	PORT_DIPSETTING(    0x500, "65%" )
	PORT_DIPSETTING(    0x600, "60%" )
	PORT_DIPSETTING(    0x700, "55%" )
	PORT_DIPSETTING(    0x800, "50%" )
	PORT_DIPSETTING(    0x900, "45%" )
	PORT_DIPSETTING(    0xa00, "40%" )
	PORT_DIPSETTING(    0xb00, "35%" )
	PORT_DIPSETTING(    0xc00, "30%" )
	PORT_DIPSETTING(    0xd00, "25%" )
	PORT_DIPSETTING(    0xe00, "20%" )
	PORT_DIPSETTING(    0xf00, "15%" )
	PORT_DIPNAME( 0x3000, 0x3000, "Play Timer" ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x0000, "30 seconds" )
	PORT_DIPSETTING(    0x1000, "24 seconds" )
	PORT_DIPSETTING(    0x2000, "18 seconds" )
	PORT_DIPSETTING(    0x3000, "12 seconds" )
	// TODO: needs NVRAM hookup and default hookup
	PORT_DIPNAME( 0x4000, 0x0000, "Backup RAM clear" ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(      0x4000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x8000, 0x0000, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x0000, DEF_STR( On ) )
INPUT_PORTS_END

void bishi_state::machine_start()
{
	save_item(NAME(m_cur_control));
	save_item(NAME(m_cur_control2));
}

void bishi_state::machine_reset()
{
	m_cur_control = 0;
	m_cur_control2 = 0;
}

void bishi_state::bishi(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, CPU_CLOCK); /* 12MHz (24MHz OSC / 2 ) */
	m_maincpu->set_addrmap(AS_PROGRAM, &bishi_state::main_map);
	TIMER(config, "scantimer").configure_scanline(FUNC(bishi_state::bishi_scanline), "screen", 0, 1);

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_AFTER_VBLANK);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(1200));
	m_screen->set_size(64*8, 32*8);
	m_screen->set_visarea(29, 29+288-1, 16, 16+224-1);
	m_screen->set_screen_update(FUNC(bishi_state::screen_update_bishi));

	PALETTE(config, m_palette).set_format(palette_device::xBGR_888, 4096);
	m_palette->enable_shadows();
	m_palette->enable_hilights();

	K056832(config, m_k056832, 0);
	m_k056832->set_tile_callback(FUNC(bishi_state::tile_callback), this);
	m_k056832->set_config(K056832_BPP_8, 1, 0);
	m_k056832->set_palette(m_palette);

	K054338(config, m_k054338, 0);
	// FP 201404: any reason why this is not connected to the k055555 below?

	K055555(config, m_k055555, 0);

	/* sound hardware */
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	ymz280b_device &ymz(YMZ280B(config, "ymz", SOUND_CLOCK)); /* 16.9344MHz */
	ymz.irq_handler().set_inputline("maincpu", M68K_IRQ_1);
	ymz.add_route(0, "lspeaker", 1.0);
	ymz.add_route(1, "rspeaker", 1.0);
}

void bishi_state::dobouchn(machine_config &config)
{
	bishi(config);
//  TODO: change accordingly (ASCII charset definitely not 8bpp, 5bpp perhaps?)
//  m_k056832->set_tile_callback(FUNC(bishi_state::dobouchn_tile_callback), this);
	m_k056832->set_config(K056832_BPP_8, 1, 0);
}

// ROM definitions


ROM_START( bishi )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "575jaa05.12e", 0x000000, 0x80000, CRC(7d354567) SHA1(7fc11585693c91c0ef7a8e00df4f2f01b356210f) )
	ROM_LOAD16_WORD_SWAP( "575jaa06.15e", 0x080000, 0x80000, CRC(9b2f7fbb) SHA1(26c828085c44a9c4d4e713e8fcc0bc8fc973d107) )

	ROM_REGION( 0x200000, "k056832", 0 )
	ROM_LOAD16_BYTE( "575jaa07.14n", 0x000000, 0x080000, CRC(37bbf387) SHA1(dcf7b151b865d251f3122611b6339dd84eb1f990) )
	ROM_LOAD16_BYTE( "575jaa08.17n", 0x000001, 0x080000, CRC(47ecd559) SHA1(7baac23557d40cccc21b93f181606563924244b0) )
	ROM_LOAD16_BYTE( "575jaa09.19n", 0x100000, 0x080000, CRC(c1db6e68) SHA1(e951661e3b39a83db21aed484764e032adcf3c2a) )
	ROM_LOAD16_BYTE( "575jaa10.22n", 0x100001, 0x080000, BAD_DUMP CRC(c8b145d6) SHA1(15cb3e4bebb999f1791fafa7a2ce3875a56991ff) )  // both halves identical (bad)

	// dummy region (game has no sprites, but we want to use the GX mixer)
	ROM_REGION( 0x80000, "gfx2", ROMREGION_ERASE00 )

	ROM_REGION( 0x200000, "ymz", 0 )
	ROM_LOAD( "575jaa01.2f", 0x000000, 0x080000, CRC(e1e9f7b2) SHA1(4da93e384a6018d829cbb02cfde98fc3662c5267) )
	ROM_LOAD( "575jaa02.4f", 0x080000, 0x080000, CRC(d228eb06) SHA1(075bd48242b5f590bfbfc45bc430578375fad70f) )
	ROM_LOAD( "575jaa03.6f", 0x100000, 0x080000, CRC(9ec0321f) SHA1(03999dc415f556d0cd58e6358f826b97e85b477b) )
	ROM_LOAD( "575jaa04.8f", 0x180000, 0x080000, CRC(0120967f) SHA1(14cc2b9269f46859d1de418c8d4c76a6bdb09d16) )
ROM_END

ROM_START( sbishi )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "675jaa05.12e", 0x000000, 0x80000, CRC(28a09c01) SHA1(627f6c9b9e88434ff3198c778ae5c57d9cda82c5) )
	ROM_LOAD16_WORD_SWAP( "675jaa06.15e", 0x080000, 0x80000, CRC(e4998b33) SHA1(3012f7661542b38b1a113c5c10e2729c6a37e709) )

	ROM_REGION( 0x200000, "k056832", 0 )
	ROM_LOAD16_BYTE( "675jaa07.14n", 0x000000, 0x080000, CRC(6fe7c658) SHA1(a786a417053a5fc62f967bdd564e8d3bdc89f958) )
	ROM_LOAD16_BYTE( "675jaa08.17n", 0x000001, 0x080000, CRC(c230afc9) SHA1(f23c64ed08e77960beb0f8db2605622a3887e5f8) )
	ROM_LOAD16_BYTE( "675jaa09.19n", 0x100000, 0x080000, CRC(63fe85a5) SHA1(e5ef1f3fc634264260d5fc3a669646abf1601b23) )
	ROM_LOAD16_BYTE( "675jaa10.22n", 0x100001, 0x080000, CRC(703ac462) SHA1(6dd05b2a78517a46b9ae8322c6b94bddbe91e848) )

	// dummy region (game has no sprites, but we want to use the GX mixer)
	ROM_REGION( 0x80000, "gfx2", ROMREGION_ERASE00 )

	ROM_REGION( 0x200000, "ymz", 0 )
	ROM_LOAD( "675jaa01.2f", 0x000000, 0x080000, CRC(67910b15) SHA1(6566e2344ebe9d61c584a1ab9ecbc8e7dd0a9a5b) )
	ROM_LOAD( "675jaa02.4f", 0x080000, 0x080000, CRC(3313a7ae) SHA1(a49df87446a5b1bbf77fdf13a298ed486d7d7476) )
	ROM_LOAD( "675jaa03.6f", 0x100000, 0x080000, CRC(ec977e6a) SHA1(9beb13e716d1694a64ce787fa3db4ba986a07d51) )
	ROM_LOAD( "675jaa04.8f", 0x180000, 0x080000, CRC(1d1de34e) SHA1(1671216545cc0842cf8c128eaa0c612e6d91875c) )
ROM_END

ROM_START( sbishik )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "kab05.12e", 0x000000, 0x80000, CRC(749063ca) SHA1(ef551132410248ef0b858fb8bcf6f8dd1115ad71) )
	ROM_LOAD16_WORD_SWAP( "kab06.15e", 0x080000, 0x80000, CRC(089e0f37) SHA1(9cd64ebfab716bbaf0ba420ad8168a33601699a9) )

	ROM_REGION( 0x200000, "k056832", 0 )
	ROM_LOAD16_BYTE( "675kaa07.14n", 0x000000, 0x080000, CRC(1177c1f8) SHA1(42c6f3c3a6bd0adb7d927386fd99f1497e5df30c) )
	ROM_LOAD16_BYTE( "675kaa08.17n", 0x000001, 0x080000, CRC(7117e9cd) SHA1(5a9b4b7427edcc10725d5936869927874fef6463) )
	ROM_LOAD16_BYTE( "675kaa09.19n", 0x100000, 0x080000, CRC(8d49c765) SHA1(7921f8f3671fbbc3d5ea529234268a1e23ea622c) )
	ROM_LOAD16_BYTE( "675kaa10.22n", 0x100001, 0x080000, CRC(c16acf32) SHA1(df3eeb5ab3bab8e707eaa79ffc500e1dc2332a82) )

	// dummy region (game has no sprites, but we want to use the GX mixer)
	ROM_REGION( 0x80000, "gfx2", ROMREGION_ERASE00 )

	ROM_REGION( 0x200000, "ymz", 0 )
	ROM_LOAD( "675kaa01.2f", 0x000000, 0x080000, CRC(73ac6ae6) SHA1(37e4722647a13275c5f51d2bfa50df3e12ea1ebf) )
	ROM_LOAD( "675kaa02.4f", 0x080000, 0x080000, CRC(4c341e7c) SHA1(b944ea59d94f9ea5cea8ed8ad68da2a52c4bbfd7) )
	ROM_LOAD( "675kaa03.6f", 0x100000, 0x080000, CRC(83f91beb) SHA1(3af95f503f26fc88e75c786a9fef8a333c21d1d6) )
	ROM_LOAD( "675kaa04.8f", 0x180000, 0x080000, CRC(ebcbd813) SHA1(d67540d0ea303f09866f4a766e2d5162f05cd4ac) )
ROM_END


ROM_START( sbishika )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "675kaa05.12e", 0x000000, 0x80000, CRC(23600e1d) SHA1(b3224c84e41e3077425a60232bb91775107f37a8) )
	ROM_LOAD16_WORD_SWAP( "675kaa06.15e", 0x080000, 0x80000, CRC(bd1091f5) SHA1(29872abc49fe8209d0f414ca40a34fc494ff9b96) )

	ROM_REGION( 0x200000, "k056832", 0 )
	ROM_LOAD16_BYTE( "675kaa07.14n", 0x000000, 0x080000, CRC(1177c1f8) SHA1(42c6f3c3a6bd0adb7d927386fd99f1497e5df30c) )
	ROM_LOAD16_BYTE( "675kaa08.17n", 0x000001, 0x080000, CRC(7117e9cd) SHA1(5a9b4b7427edcc10725d5936869927874fef6463) )
	ROM_LOAD16_BYTE( "675kaa09.19n", 0x100000, 0x080000, CRC(8d49c765) SHA1(7921f8f3671fbbc3d5ea529234268a1e23ea622c) )
	ROM_LOAD16_BYTE( "675kaa10.22n", 0x100001, 0x080000, CRC(c16acf32) SHA1(df3eeb5ab3bab8e707eaa79ffc500e1dc2332a82) )

	// dummy region (game has no sprites, but we want to use the GX mixer)
	ROM_REGION( 0x80000, "gfx2", ROMREGION_ERASE00 )

	ROM_REGION( 0x200000, "ymz", 0 )
	ROM_LOAD( "675kaa01.2f", 0x000000, 0x080000, CRC(73ac6ae6) SHA1(37e4722647a13275c5f51d2bfa50df3e12ea1ebf) )
	ROM_LOAD( "675kaa02.4f", 0x080000, 0x080000, CRC(4c341e7c) SHA1(b944ea59d94f9ea5cea8ed8ad68da2a52c4bbfd7) )
	ROM_LOAD( "675kaa03.6f", 0x100000, 0x080000, CRC(83f91beb) SHA1(3af95f503f26fc88e75c786a9fef8a333c21d1d6) )
	ROM_LOAD( "675kaa04.8f", 0x180000, 0x080000, CRC(ebcbd813) SHA1(d67540d0ea303f09866f4a766e2d5162f05cd4ac) )
ROM_END

ROM_START( dobouchn )
	ROM_REGION( 0x100000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "640-a05-2n.bin", 0x000000, 0x080000, CRC(7643dbc6) SHA1(3b55a782f04a741088b93954279b35c1c90af622) )

	ROM_REGION( 0x200000, "k056832", 0 )
	// makes GFX ROM test to pass
	// must return 0xf0c3 and 0xee9e, check snippets at 0x2e1c and 0x2e42
	// TODO: might require a different K056832_rom_r callback instead
	ROM_LOAD16_BYTE( "640-a06-14n.bin", 0x000000, 0x080000, CRC(c6c5016c) SHA1(ad0b5258e2c1d0ba95dfc0d8fc6332b524f2c1e2) )
	ROM_RELOAD(                         0x100001, 0x080000 )
	ROM_LOAD16_BYTE( "640-a07-17n.bin", 0x000001, 0x080000, CRC(614fee32) SHA1(080fea72c0417752eb0a0b109b524d87379b2921) )
	ROM_RELOAD(                         0x100000, 0x080000 )

	// dummy region (game has no sprites, but we want to use the GX mixer)
	ROM_REGION( 0x80000, "gfx2", ROMREGION_ERASE00 )

	ROM_REGION( 0x200000, "ymz", 0 )
	ROM_LOAD( "640-a01-2f.bin", 0x000000, 0x080000, CRC(326e2844) SHA1(62ce14ffe5d0a35c37c9a5a98c9c3a5df63d4512) )
	ROM_LOAD( "640-a02-4f.bin", 0x080000, 0x080000, CRC(ab6593f5) SHA1(95907ee4a2cdf3bf27b7c0c1283b2bc36b868d9d) )
ROM_END

GAME( 1996, bishi,    0,      bishi,    bishi,    bishi_state, empty_init, ROT0, "Konami", "Bishi Bashi Championship Mini Game Senshuken (ver JAA, 3 Players)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1998, sbishi,   0,      bishi,    bishi2p,  bishi_state, empty_init, ROT0, "Konami", "Super Bishi Bashi Championship (ver JAA, 2 Players)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1998, sbishik,  sbishi, bishi,    bishi,    bishi_state, empty_init, ROT0, "Konami", "Super Bishi Bashi Championship (ver KAB, 3 Players)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1998, sbishika, sbishi, bishi,    bishi,    bishi_state, empty_init, ROT0, "Konami", "Super Bishi Bashi Championship (ver KAA, 3 Players)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1996, dobouchn, 0,      dobouchn, dobouchn, bishi_state, empty_init, ROT0, "Konami", "Dobou-Chan (ver JAA)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
