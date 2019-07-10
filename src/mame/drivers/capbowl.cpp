// license:BSD-3-Clause
// copyright-holders:Zsolt Vasvari
/***************************************************************************

    Coors Light Bowling/Bowl-O-Rama hardware

    driver by Zsolt Vasvari

    Games supported:
        * Capcom/Coors Light Bowling
        * Bowlorama

    Known issues:
        * none

****************************************************************************

    CPU Board:

    0000-3fff     3 Graphics ROMS mapped in using 0x4800 (Coors Light Bowling only)
    0000-001f       Turbo board area (Bowl-O-Rama only) See Below.
    4000          Display row selected
    4800          Graphics ROM select
    5000-57ff     Battery backed up RAM (Saves machine state after shut off)
               Enter setup menu by holding down the F2 key on the
               high score screen
    5800-5fff       TMS34061 area

               First 0x20 bytes of each row provide a 16 color palette for this
               row. 2 bytes per color: 0000RRRR GGGGBBBB.

               Remaining 0xe0 bytes contain 2 pixels each for a total of
               448 pixels, but only 360 seem to be displayed.
               (Each row appears vertically because the monitor is rotated)

    6000          Sound command
    6800          Trackball Reset. Double duties as a watchdog.
    7000          Input port 1  Bit 0-3 Trackball Vertical Position
                                Bit 4   Player 2 Hook Left
                                Bit 5   Player 2 Hook Right
                                Bit 6   Upright/Cocktail DIP Switch
                                Bit 7   Coin 2
    7800          Input port 2  Bit 0-3 Trackball Horizontal Positon
                                Bit 4   Player 1 Hook Left
                                Bit 5   Player 1 Hook Right
                                Bit 6   Start
                                Bit 7   Coin 1
    8000-ffff     ROM


    Sound Board:

    0000-07ff       RAM
    1000-1001       YM2203
                Port A D7 Read  is ticket sensor
                Port B D7 Write is ticket dispenser enable
                Port B D6 Write is Sound OK LED
    2000            Sound watchdog clear
    6000            DAC write
    7000            Sound command read (0x34 is used to dispense a ticket)
    8000-ffff       ROM


    Turbo Board Layout (Plugs in place of GR0):

    Bowl-O-Rama Copyright 1991 P&P Marketing
                Marquee says "EXIT Entertainment"

                This portion: Mike Appolo with the help of Andrew Pines.
                Andrew was one of the game designers for Capcom Bowling,
                Coors Light Bowling, Strata Bowling, and Bowl-O-Rama.

                This game was an upgrade for Capcom Bowling and included a
                "Turbo PCB" that had a GAL address decoder / data mask

    Memory Map for turbo board (where GR0 is on Capcom Bowling PCBs:

    0000        Read Mask
    0001-0003       Unused
    0004        Read Data
    0005-0007       Unused
    0008        GR Address High Byte (GR17-16)
    0009-0016       Unused
    0017            GR Address Middle Byte (GR15-0 written as a word to 0017-0018)
    0018        GR address Low byte
    0019-001f       Unused

***************************************************************************/

#include "emu.h"
#include "includes/capbowl.h"

#include "machine/ticket.h"
#include "cpu/m6809/m6809.h"
#include "sound/2203intf.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "speaker.h"

#define MASTER_CLOCK        XTAL(8'000'000)


/*************************************
 *
 *  NMI is to trigger the self test.
 *  We use a fake input port to tie
 *  that event to a keypress
 *
 *************************************/

INTERRUPT_GEN_MEMBER(capbowl_state::interrupt)
{
	if (ioport("SERVICE")->read() & 1)                      /* get status of the F2 key */
		device.execute().pulse_input_line(INPUT_LINE_NMI, attotime::zero);    /* trigger self test */
}



/*************************************
 *
 *  Partial updating
 *
 *************************************/

void capbowl_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_UPDATE:
		update(ptr, param);
		break;
	default:
		assert_always(false, "Unknown id in capbowl_state::device_timer");
	}
}


TIMER_CALLBACK_MEMBER(capbowl_state::update)
{
	int scanline = param;

	m_screen->update_partial(scanline - 1);
	scanline += 32;
	if (scanline > 240) scanline = 32;
	m_update_timer->adjust(m_screen->time_until_pos(scanline), scanline);
}


/*************************************
 *
 *  Graphics ROM banking
 *
 *************************************/

WRITE8_MEMBER(capbowl_state::capbowl_rom_select_w)
{
	// 2009-11 FP: shall we add a check to be sure that bank < 6?
	membank("bank1")->set_entry(((data & 0x0c) >> 1) + (data & 0x01));
}



/*************************************
 *
 *  Trackball input handlers
 *
 *************************************/

READ8_MEMBER(capbowl_state::track_0_r)
{
	return (ioport("IN0")->read() & 0xf0) | ((ioport("TRACKY")->read() - m_last_trackball_val[0]) & 0x0f);
}


READ8_MEMBER(capbowl_state::track_1_r)
{
	return (ioport("IN1")->read() & 0xf0) | ((ioport("TRACKX")->read() - m_last_trackball_val[1]) & 0x0f);
}


WRITE8_MEMBER(capbowl_state::track_reset_w)
{
	/* reset the trackball counters */
	m_last_trackball_val[0] = ioport("TRACKY")->read();
	m_last_trackball_val[1] = ioport("TRACKX")->read();

	m_watchdog->watchdog_reset();
}



/*************************************
 *
 *  Sound commands
 *
 *************************************/

WRITE8_MEMBER(capbowl_state::sndcmd_w)
{
	m_audiocpu->set_input_line(M6809_IRQ_LINE, HOLD_LINE);
	m_soundlatch->write(data);
}


/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

void capbowl_state::capbowl_map(address_map &map)
{
	map(0x0000, 0x3fff).bankr("bank1");
	map(0x4000, 0x4000).writeonly().share("rowaddress");
	map(0x4800, 0x4800).w(FUNC(capbowl_state::capbowl_rom_select_w));
	map(0x5000, 0x57ff).ram().share("nvram");
	map(0x5800, 0x5fff).rw(FUNC(capbowl_state::tms34061_r), FUNC(capbowl_state::tms34061_w));
	map(0x6000, 0x6000).w(FUNC(capbowl_state::sndcmd_w));
	map(0x6800, 0x6800).w(FUNC(capbowl_state::track_reset_w)).nopr();   /* + watchdog */
	map(0x7000, 0x7000).r(FUNC(capbowl_state::track_0_r));         /* + other inputs */
	map(0x7800, 0x7800).r(FUNC(capbowl_state::track_1_r));         /* + other inputs */
	map(0x8000, 0xffff).rom();
}


void capbowl_state::bowlrama_map(address_map &map)
{
	map(0x0000, 0x001f).rw(FUNC(capbowl_state::bowlrama_blitter_r), FUNC(capbowl_state::bowlrama_blitter_w));
	map(0x4000, 0x4000).writeonly().share("rowaddress");
	map(0x5000, 0x57ff).ram().share("nvram");
	map(0x5800, 0x5fff).rw(FUNC(capbowl_state::tms34061_r), FUNC(capbowl_state::tms34061_w));
	map(0x6000, 0x6000).w(FUNC(capbowl_state::sndcmd_w));
	map(0x6800, 0x6800).w(FUNC(capbowl_state::track_reset_w)).nopr();    /* + watchdog */
	map(0x7000, 0x7000).r(FUNC(capbowl_state::track_0_r));         /* + other inputs */
	map(0x7800, 0x7800).r(FUNC(capbowl_state::track_1_r));         /* + other inputs */
	map(0x8000, 0xffff).rom();
}



/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

void capbowl_state::sound_map(address_map &map)
{
	map(0x0000, 0x07ff).ram();
	map(0x1000, 0x1001).rw("ymsnd", FUNC(ym2203_device::read), FUNC(ym2203_device::write));
	map(0x2000, 0x2000).nopw(); /* watchdog */
	map(0x6000, 0x6000).w("dac", FUNC(dac_byte_interface::data_w));
	map(0x7000, 0x7000).r(m_soundlatch, FUNC(generic_latch_8_device::read));
	map(0x8000, 0xffff).rom();
}



/*************************************
 *
 *  Port definitions
 *
 *************************************/

static INPUT_PORTS_START( capbowl )
	PORT_START("IN0")
	/* low 4 bits are for the trackball */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_COCKTAIL
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Cabinet ) ) /* This version of Bowl-O-Rama */
	PORT_DIPSETTING(    0x40, DEF_STR( Upright ) )             /* is Upright only */
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("IN1")
	/* low 4 bits are for the trackball */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("TRACKY")
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_Y ) PORT_SENSITIVITY(20) PORT_KEYDELTA(40) PORT_REVERSE

	PORT_START("TRACKX")
	PORT_BIT( 0xff, 0x00, IPT_TRACKBALL_X ) PORT_SENSITIVITY(20) PORT_KEYDELTA(40)

	PORT_START("SERVICE")
	/* This fake input port is used to get the status of the F2 key, */
	/* and activate the test mode, which is triggered by a NMI */
	PORT_SERVICE_NO_TOGGLE( 0x01, IP_ACTIVE_HIGH )
INPUT_PORTS_END


/*************************************
 *
 *  Machine driver
 *
 *************************************/

void capbowl_state::machine_start()
{
	m_update_timer = timer_alloc(TIMER_UPDATE);

	save_item(NAME(m_blitter_addr));
	save_item(NAME(m_last_trackball_val));
}

void capbowl_state::machine_reset()
{
	m_update_timer->adjust(m_screen->time_until_pos(32), 32);

	m_blitter_addr = 0;
	m_last_trackball_val[0] = 0;
	m_last_trackball_val[1] = 0;
}


void capbowl_state::capbowl(machine_config &config)
{
	/* basic machine hardware */
	MC6809E(config, m_maincpu, MASTER_CLOCK / 4); // MC68B09EP
	m_maincpu->set_addrmap(AS_PROGRAM, &capbowl_state::capbowl_map);
	m_maincpu->set_vblank_int("screen", FUNC(capbowl_state::interrupt));

	// watchdog: 555 timer 16 cycles, edge triggered, ~0.3s
	attotime const period = PERIOD_OF_555_ASTABLE(100000.0, 100000.0, 0.1e-6);
	WATCHDOG_TIMER(config, m_watchdog).set_time(period * 16 - period / 2);

	MC6809E(config, m_audiocpu, MASTER_CLOCK / 4); // MC68B09EP
	m_audiocpu->set_addrmap(AS_PROGRAM, &capbowl_state::sound_map);

	NVRAM(config, "nvram", nvram_device::DEFAULT_RANDOM);

	TICKET_DISPENSER(config, "ticket", attotime::from_msec(100), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_LOW);

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_size(360, 256);
	m_screen->set_visarea(0, 359, 0, 244);
	m_screen->set_refresh_hz(57);
	m_screen->set_screen_update(FUNC(capbowl_state::screen_update));

	TMS34061(config, m_tms34061, 0);
	m_tms34061->set_rowshift(8);  /* VRAM address is (row << rowshift) | col */
	m_tms34061->set_vram_size(0x10000);
	m_tms34061->int_callback().set_inputline("maincpu", M6809_FIRQ_LINE);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();

	GENERIC_LATCH_8(config, m_soundlatch);

	ym2203_device &ymsnd(YM2203(config, "ymsnd", MASTER_CLOCK / 2));
	ymsnd.irq_handler().set_inputline(m_audiocpu, M6809_FIRQ_LINE);
	ymsnd.port_a_read_callback().set("ticket", FUNC(ticket_dispenser_device::line_r)).lshift(7);
	ymsnd.port_b_write_callback().set("ticket", FUNC(ticket_dispenser_device::motor_w)).bit(7); // Also a status LED. See memory map above
	ymsnd.add_route(0, "speaker", 0.07);
	ymsnd.add_route(1, "speaker", 0.07);
	ymsnd.add_route(2, "speaker", 0.07);
	ymsnd.add_route(3, "speaker", 0.75);

	DAC0832(config, "dac", 0).add_route(ALL_OUTPUTS, "speaker", 0.5);
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref"));
	vref.add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
	vref.add_route(0, "dac", -1.0, DAC_VREF_NEG_INPUT);
}


void capbowl_state::bowlrama(machine_config &config)
{
	capbowl(config);

	/* basic machine hardware */

	m_maincpu->set_addrmap(AS_PROGRAM, &capbowl_state::bowlrama_map);

	/* video hardware */
	m_screen->set_visarea(0, 359, 0, 239);
}



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( capbowl )
	ROM_REGION( 0x28000, "maincpu", 0 )
	ROM_LOAD( "u6",  0x08000, 0x8000, CRC(14924c96) SHA1(d436c5115873c9c2bc7657acff1cf7d99c0c5d6d) )
	ROM_LOAD( "gr0", 0x10000, 0x8000, CRC(ef53ca7a) SHA1(219dc342595bfd23c1336f3e167e40ff0c5e7994) )
	ROM_LOAD( "gr1", 0x18000, 0x8000, CRC(27ede6ce) SHA1(14aa31cbcf089419b5b2ea8d57e82fc51895fc2e) )
	ROM_LOAD( "gr2", 0x20000, 0x8000, CRC(e49238f4) SHA1(ac76f1a761d6b0765437fb7367442667da7bb373) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sound.u30", 0x8000, 0x8000, CRC(8c9c3b8a) SHA1(f3cdf42ef19012817e6b7966845f9ede39f61b07) )
ROM_END


ROM_START( capbowl2 )
	ROM_REGION( 0x28000, "maincpu", 0 )
	ROM_LOAD( "program_rev_3_u6.u6", 0x08000, 0x8000, CRC(9162934a) SHA1(7542dd68a2aa55ad4f03b23ae2313ed6a34ae145) )
	ROM_LOAD( "gr0",                 0x10000, 0x8000, CRC(ef53ca7a) SHA1(219dc342595bfd23c1336f3e167e40ff0c5e7994) )
	ROM_LOAD( "gr1",                 0x18000, 0x8000, CRC(27ede6ce) SHA1(14aa31cbcf089419b5b2ea8d57e82fc51895fc2e) )
	ROM_LOAD( "gr2",                 0x20000, 0x8000, CRC(e49238f4) SHA1(ac76f1a761d6b0765437fb7367442667da7bb373) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sound.u30", 0x8000, 0x8000, CRC(8c9c3b8a) SHA1(f3cdf42ef19012817e6b7966845f9ede39f61b07) )
ROM_END


ROM_START( capbowl3 )
	ROM_REGION( 0x28000, "maincpu", 0 )
	ROM_LOAD( "3.0_bowl.u6",   0x08000, 0x8000, CRC(32e30928) SHA1(db47b6ace949d86aa1cdd1e5c7a5981f30b590af) ) /* Capcom label, labeled as "3.0 BOWL" */
	ROM_LOAD( "grom0-gr0.gr0", 0x10000, 0x8000, CRC(2b5eb091) SHA1(43976bfa9fbe9694c7274f113641f671fa32bbb7) ) /* I.T. label */
	ROM_LOAD( "grom1-gr1.gr1", 0x18000, 0x8000, CRC(880e4e1c) SHA1(9f88b26877596667f1ac4e0083795bf266712879) ) /* I.T. label */
	ROM_LOAD( "grom2-gr2.gr2", 0x20000, 0x8000, CRC(f3d2468d) SHA1(0348ee5d0000b753ad90a525048d05bfb552bee1) ) /* I.T. label */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "sound_r2_u30.u30", 0x8000, 0x8000, CRC(43ac1658) SHA1(1fab23d649d0c565ef1a7f45b30806f9d1bb4afd) ) /* labeled as "SOUND (R-2) U30" */
ROM_END


ROM_START( capbowl4 )
	ROM_REGION( 0x28000, "maincpu", 0 )
	ROM_LOAD( "bfb.u6",        0x08000, 0x8000, CRC(79f1d083) SHA1(36e9a90403fc9b876d7660ee46c5fbb855321769) )
	ROM_LOAD( "grom0-gr0.gr0", 0x10000, 0x8000, CRC(2b5eb091) SHA1(43976bfa9fbe9694c7274f113641f671fa32bbb7) ) /* I.T. label */
	ROM_LOAD( "grom1-gr1.gr1", 0x18000, 0x8000, CRC(880e4e1c) SHA1(9f88b26877596667f1ac4e0083795bf266712879) ) /* I.T. label */
	ROM_LOAD( "grom2-gr2.gr2", 0x20000, 0x8000, CRC(f3d2468d) SHA1(0348ee5d0000b753ad90a525048d05bfb552bee1) ) /* I.T. label */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "bfb.u30",     0x8000, 0x8000, CRC(6fe2c4ff) SHA1(862823264d243be590fd29a228a32e7a0a818e57) )
ROM_END


ROM_START( clbowl )
	ROM_REGION( 0x28000, "maincpu", 0 )
	ROM_LOAD( "cb8_prg.u6",              0x08000, 0x8000, CRC(91e06bc4) SHA1(efa54328417f971cc482a4529d05331a3baffc1a) ) /* Capcom label */
	ROM_LOAD( "coors_bowling_grom0.gr0", 0x10000, 0x8000, CRC(899c8f15) SHA1(dbb4a9c015b5e64c62140f0c99b87da2793ae5c1) ) /* I.T. label */
	ROM_LOAD( "coors_bowling_grom1.gr1", 0x18000, 0x8000, CRC(0ac0dc4c) SHA1(61afa3af1f84818b940b5c6f6a8cfb58ca557551) ) /* I.T. label */
	ROM_LOAD( "coors_bowling_grom2.gr2", 0x20000, 0x8000, CRC(251f5da5) SHA1(063001cfb68e3ec35baa24eed186214e26d55b82) ) /* I.T. label */

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "coors_bowling_sound.u30", 0x8000, 0x8000, CRC(1eba501e) SHA1(684bdc18cf5e01a86d8018a3e228ec34e5dec57d) )
ROM_END


ROM_START( bowlrama )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "bowl-o-rama_rev_1.0_u6.u6",   0x08000, 0x08000, CRC(7103ad55) SHA1(92dccc5e6df3e18fc8cdcb67ef14d50ce5eb8b2c) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "bowl-o-rama_rev_1.0_u30.u30", 0x08000, 0x08000, CRC(f3168834) SHA1(40b7fbe9c15cc4442f4394b71c0666185afe4c8d) )

	ROM_REGION( 0x40000, "gfx1", 0 )
	ROM_LOAD( "bowl-o-rama_rev_1.0_ux7.ux7", 0x00000, 0x40000, CRC(8727432a) SHA1(a81d366c5f8df0bdb97e795bba7752e6526ddba0) ) /* located on daughter card add-on */
ROM_END



/*************************************
 *
 *  Driver init
 *
 *************************************/

void capbowl_state::init_capbowl()
{
	uint8_t *ROM = memregion("maincpu")->base();

	/* configure ROM banks in 0x0000-0x3fff */
	membank("bank1")->configure_entries(0, 6, &ROM[0x10000], 0x4000);
}


/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1988, capbowl,  0,       capbowl,  capbowl, capbowl_state, init_capbowl, ROT270, "Incredible Technologies / Capcom", "Capcom Bowling (set 1)", MACHINE_SUPPORTS_SAVE )
GAME( 1988, capbowl2, capbowl, capbowl,  capbowl, capbowl_state, init_capbowl, ROT270, "Incredible Technologies / Capcom", "Capcom Bowling (set 2)", MACHINE_SUPPORTS_SAVE )
GAME( 1988, capbowl3, capbowl, capbowl,  capbowl, capbowl_state, init_capbowl, ROT270, "Incredible Technologies / Capcom", "Capcom Bowling (set 3)", MACHINE_SUPPORTS_SAVE )
GAME( 1988, capbowl4, capbowl, capbowl,  capbowl, capbowl_state, init_capbowl, ROT270, "Incredible Technologies / Capcom", "Capcom Bowling (set 4)", MACHINE_SUPPORTS_SAVE )
GAME( 1989, clbowl,   capbowl, capbowl,  capbowl, capbowl_state, init_capbowl, ROT270, "Incredible Technologies / Capcom", "Coors Light Bowling",    MACHINE_SUPPORTS_SAVE )
GAME( 1991, bowlrama, 0,       bowlrama, capbowl, capbowl_state, empty_init,   ROT270, "P&P Marketing",                    "Bowl-O-Rama Rev 1.0",    MACHINE_SUPPORTS_SAVE )
