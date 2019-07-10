// license:BSD-3-Clause
// copyright-holders:Manuel Abadia
/***************************************************************************

    Fast Lane (GX752) (c) 1987 Konami

    Driver by Manuel Abadia <emumanu+mame@gmail.com>

    TODO:
        - verify that sound is correct (volume and bank switching)

***************************************************************************/

#include "emu.h"
#include "includes/fastlane.h"
#include "includes/konamipt.h"

#include "cpu/m6809/hd6309.h"
#include "machine/watchdog.h"

#include "speaker.h"


TIMER_DEVICE_CALLBACK_MEMBER(fastlane_state::fastlane_scanline)
{
	int scanline = param;

	if(scanline == 240 && m_k007121->ctrlram_r(7) & 0x02) // vblank irq
		m_maincpu->set_input_line(HD6309_IRQ_LINE, HOLD_LINE);
	else if(((scanline % 32) == 0) && m_k007121->ctrlram_r(7) & 0x01) // timer irq
		m_maincpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
}


WRITE8_MEMBER(fastlane_state::k007121_registers_w)
{
	if (offset < 8)
		m_k007121->ctrl_w(space, offset, data);
	else    /* scroll registers */
		m_k007121_regs[offset] = data;
}

WRITE8_MEMBER(fastlane_state::fastlane_bankswitch_w)
{
	/* bits 0 & 1 coin counters */
	machine().bookkeeping().coin_counter_w(0,data & 0x01);
	machine().bookkeeping().coin_counter_w(1,data & 0x02);

	/* bits 2 & 3 = bank number */
	membank("bank1")->set_entry((data & 0x0c) >> 2);

	/* bit 4: bank # for the 007232 (chip 2) */
	m_k007232_2->set_bank(0 + ((data & 0x10) >> 4), 2 + ((data & 0x10) >> 4));

	/* other bits seems to be unused */
}

/* Read and write handlers for one K007232 chip:
   even and odd register are mapped swapped */

READ8_MEMBER(fastlane_state::fastlane_k1_k007232_r)
{
	return m_k007232_1->read(space, offset ^ 1);
}

WRITE8_MEMBER(fastlane_state::fastlane_k1_k007232_w)
{
	m_k007232_1->write(space, offset ^ 1, data);
}

READ8_MEMBER(fastlane_state::fastlane_k2_k007232_r)
{
	return m_k007232_2->read(space, offset ^ 1);
}

WRITE8_MEMBER(fastlane_state::fastlane_k2_k007232_w)
{
	m_k007232_2->write(space, offset ^ 1, data);
}
void fastlane_state::fastlane_map(address_map &map)
{
	map(0x0000, 0x005f).ram().w(FUNC(fastlane_state::k007121_registers_w)).share("k007121_regs"); /* 007121 registers */
	map(0x0800, 0x0800).portr("DSW3");
	map(0x0801, 0x0801).portr("P2");
	map(0x0802, 0x0802).portr("P1");
	map(0x0803, 0x0803).portr("SYSTEM");
	map(0x0900, 0x0900).portr("DSW1");
	map(0x0901, 0x0901).portr("DSW2");
	map(0x0b00, 0x0b00).w("watchdog", FUNC(watchdog_timer_device::reset_w));
	map(0x0c00, 0x0c00).w(FUNC(fastlane_state::fastlane_bankswitch_w));                                    /* bankswitch control */
	map(0x0d00, 0x0d0d).rw(FUNC(fastlane_state::fastlane_k1_k007232_r), FUNC(fastlane_state::fastlane_k1_k007232_w)); /* 007232 registers (chip 1) */
	map(0x0e00, 0x0e0d).rw(FUNC(fastlane_state::fastlane_k2_k007232_r), FUNC(fastlane_state::fastlane_k2_k007232_w)); /* 007232 registers (chip 2) */
	map(0x0f00, 0x0f1f).rw("k051733", FUNC(k051733_device::read), FUNC(k051733_device::write));                                    /* 051733 (protection) */
	map(0x1000, 0x17ff).ram().w(m_palette, FUNC(palette_device::write_indirect)).share("palette");
	map(0x1800, 0x1fff).ram();                                                             /* Work RAM */
	map(0x2000, 0x27ff).ram().w(FUNC(fastlane_state::fastlane_vram1_w)).share("videoram1");       /* Video RAM (chip 1) */
	map(0x2800, 0x2fff).ram().w(FUNC(fastlane_state::fastlane_vram2_w)).share("videoram2");       /* Video RAM (chip 2) */
	map(0x3000, 0x3fff).ram().share("spriteram");                                           /* Sprite RAM */
	map(0x4000, 0x7fff).bankr("bank1");                                                        /* banked ROM */
	map(0x8000, 0xffff).rom();                                                             /* ROM */
}

/***************************************************************************

    Input Ports

***************************************************************************/

/* verified from HD6309 code */
static INPUT_PORTS_START( fastlane )
	PORT_START("DSW1")
	KONAMI_COINAGE_LOC(DEF_STR( Free_Play ), "No Coin B", SW1)
	/* "No Coin B" = coins produce sound, but no effect on coin counter */

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )            PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )          PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Cocktail ) )
	/* The bonus life affects the starting high score too, 20000 or 30000 */
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x18, "20k 100k 200k 400k 800k" )   PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(    0x10, "30k 150k 300k 600k" )
	PORT_DIPSETTING(    0x08, "20k only" )
	PORT_DIPSETTING(    0x00, "30k only" )
	PORT_DIPNAME( 0x60, 0x40, DEF_STR( Difficulty ) )       PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(    0x60, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )      PORT_DIPLOCATION("SW2:8") // seems it doesn't work (same on pcb)
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW3")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )      PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "Upright Controls" )          PORT_DIPLOCATION("SW3:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Single ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Dual ) )
	PORT_SERVICE_DIPLOC( 0x04, IP_ACTIVE_LOW, "SW3:3" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Allow_Continue ) )   PORT_DIPLOCATION("SW3:4")
	PORT_DIPSETTING(    0x08, "3 Times" )
	PORT_DIPSETTING(    0x00, DEF_STR( Infinite ) )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("SYSTEM")
	KONAMI8_SYSTEM_UNK

	PORT_START("P1")
	KONAMI8_B12_UNK(1)

	PORT_START("P2")
	KONAMI8_B12_UNK(2)
INPUT_PORTS_END

static const gfx_layout gfxlayout =
{
	8,8,
	0x80000/32,
	4,
	{ 0, 1, 2, 3 },
	{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static GFXDECODE_START( gfx_fastlane )
	GFXDECODE_ENTRY( "gfx1", 0, gfxlayout, 0, 64*16 )
GFXDECODE_END

/***************************************************************************

    Machine Driver

***************************************************************************/

WRITE8_MEMBER(fastlane_state::volume_callback0)
{
	m_k007232_1->set_volume(0, (data >> 4) * 0x11, 0);
	m_k007232_1->set_volume(1, 0, (data & 0x0f) * 0x11);
}

WRITE8_MEMBER(fastlane_state::volume_callback1)
{
	m_k007232_2->set_volume(0, (data >> 4) * 0x11, 0);
	m_k007232_2->set_volume(1, 0, (data & 0x0f) * 0x11);
}

void fastlane_state::machine_start()
{
	uint8_t *ROM = memregion("maincpu")->base();

	membank("bank1")->configure_entries(0, 4, &ROM[0x10000], 0x4000);
}

void fastlane_state::fastlane(machine_config &config)
{
	/* basic machine hardware */
	HD6309(config, m_maincpu, XTAL(24'000'000)/2); // 3MHz(XTAL(24'000'000)/8) internally
	m_maincpu->set_addrmap(AS_PROGRAM, &fastlane_state::fastlane_map);
	TIMER(config, "scantimer").configure_scanline(FUNC(fastlane_state::fastlane_scanline), "screen", 0, 1);

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(59.17); // measured
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(0));
	m_screen->set_size(37*8, 32*8);
	m_screen->set_visarea(0*8, 35*8-1, 2*8, 30*8-1);
	m_screen->set_screen_update(FUNC(fastlane_state::screen_update_fastlane));
	m_screen->set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_fastlane);
	PALETTE(config, m_palette, FUNC(fastlane_state::fastlane_palette)).set_format(palette_device::xBGR_555, 1024*16, 0x400);

	K007121(config, m_k007121, 0);
	m_k007121->set_palette_tag(m_palette);

	K051733(config, "k051733", 0);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	K007232(config, m_k007232_1, XTAL(3'579'545));
	m_k007232_1->port_write().set(FUNC(fastlane_state::volume_callback0));
	m_k007232_1->add_route(0, "mono", 0.50);
	m_k007232_1->add_route(1, "mono", 0.50);

	K007232(config, m_k007232_2, XTAL(3'579'545));
	m_k007232_2->port_write().set(FUNC(fastlane_state::volume_callback1));
	m_k007232_2->add_route(0, "mono", 0.50);
	m_k007232_2->add_route(1, "mono", 0.50);
}


/***************************************************************************

  Game ROMs

***************************************************************************/

ROM_START( fastlane )
	ROM_REGION( 0x21000, "maincpu", 0 ) /* code + banked roms */
	ROM_LOAD( "752_m02.9h",  0x08000, 0x08000, CRC(e1004489) SHA1(615b608d22abc3611f1620503cd6a8c9a6218db8) )  /* fixed ROM */
	ROM_LOAD( "752_e01.10h", 0x10000, 0x10000, CRC(ff4d6029) SHA1(b5c5d8654ce728300d268628bd3dd878570ba7b8) )  /* banked ROM */

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "752e04.2i",   0x00000, 0x80000, CRC(a126e82d) SHA1(6663230c2c36dec563969bccad8c62e3d454d240) )  /* tiles + sprites */

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "752e03.6h",   0x0000, 0x0100, CRC(44300aeb) SHA1(580c6e88cbb3b6d8156ea0b9103834f199ec2747) )

	ROM_REGION( 0x20000, "k007232_1", 0 ) /* 007232 data */
	ROM_LOAD( "752e06.4c",   0x00000, 0x20000, CRC(85d691ed) SHA1(7f8d05562a68c75672141fc80ce7e7acb80588b9) ) /* chip 1 */

	ROM_REGION( 0x80000, "k007232_2", 0 ) /* 007232 data */
	ROM_LOAD( "752e05.12b",  0x00000, 0x80000, CRC(119e9cbf) SHA1(21e3def9ab10b210632df11b6df4699140c473db) ) /* chip 2 */
ROM_END


GAME( 1987, fastlane, 0, fastlane, fastlane, fastlane_state, empty_init, ROT90, "Konami", "Fast Lane", MACHINE_NO_COCKTAIL | MACHINE_SUPPORTS_SAVE )
