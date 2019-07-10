// license:BSD-3-Clause
// copyright-holders:Manuel Abadia
/***************************************************************************

    Crime Fighters (Konami GX821) (c) 1989 Konami

    Preliminary driver by:
        Manuel Abadia <emumanu+mame@gmail.com>


    2008-08
    Dip locations verified with manual (US)

***************************************************************************/

#include "emu.h"
#include "includes/konamipt.h"
#include "includes/crimfght.h"

#include "cpu/z80/z80.h"
#include "machine/watchdog.h"
#include "sound/ym2151.h"
#include "speaker.h"


WRITE8_MEMBER(crimfght_state::crimfght_coin_w)
{
	machine().bookkeeping().coin_counter_w(0, data & 1);
	machine().bookkeeping().coin_counter_w(1, data & 2);
}

READ8_MEMBER(crimfght_state::k052109_051960_r)
{
	if (m_k052109->get_rmrd_line() == CLEAR_LINE)
	{
		if (offset >= 0x3800 && offset < 0x3808)
			return m_k051960->k051937_r(offset - 0x3800);
		else if (offset < 0x3c00)
			return m_k052109->read(offset);
		else
			return m_k051960->k051960_r(offset - 0x3c00);
	}
	else
		return m_k052109->read(offset);
}

WRITE8_MEMBER(crimfght_state::k052109_051960_w)
{
	if (offset >= 0x3800 && offset < 0x3808)
		m_k051960->k051937_w(offset - 0x3800, data);
	else if (offset < 0x3c00)
		m_k052109->write(offset, data);
	else
		m_k051960->k051960_w(offset - 0x3c00, data);
}

WRITE8_MEMBER(crimfght_state::sound_w)
{
	// writing the latch asserts the irq line
	m_soundlatch->write(data);
	m_audiocpu->set_input_line(INPUT_LINE_IRQ0, ASSERT_LINE);
}

IRQ_CALLBACK_MEMBER( crimfght_state::audiocpu_irq_ack )
{
	// irq ack cycle clears irq via flip-flop u86
	m_audiocpu->set_input_line(INPUT_LINE_IRQ0, CLEAR_LINE);
	return 0xff;
}

WRITE8_MEMBER(crimfght_state::ym2151_ct_w)
{
	// ne output from the 007232 is connected to a ls399 which
	// has inputs connected to the ct1 and ct2 outputs from
	// the ym2151 used to select the bank

	int bank_a = BIT(data, 1);
	int bank_b = BIT(data, 0);

	m_k007232->set_bank(bank_a, bank_b);
}

void crimfght_state::crimfght_map(address_map &map)
{
	map(0x0000, 0x03ff).m(m_bank0000, FUNC(address_map_bank_device::amap8));
	map(0x0400, 0x1fff).ram();
	map(0x2000, 0x5fff).rw(FUNC(crimfght_state::k052109_051960_r), FUNC(crimfght_state::k052109_051960_w));   /* video RAM + sprite RAM */
	map(0x3f80, 0x3f80).portr("SYSTEM");
	map(0x3f81, 0x3f81).portr("P1");
	map(0x3f82, 0x3f82).portr("P2");
	map(0x3f83, 0x3f83).portr("DSW2");
	map(0x3f84, 0x3f84).portr("DSW3");
	map(0x3f85, 0x3f85).portr("P3");
	map(0x3f86, 0x3f86).portr("P4");
	map(0x3f87, 0x3f87).portr("DSW1");
	map(0x3f88, 0x3f88).mirror(0x03).r("watchdog", FUNC(watchdog_timer_device::reset_r)).w(FUNC(crimfght_state::crimfght_coin_w)); // 051550
	map(0x3f8c, 0x3f8c).mirror(0x03).w(FUNC(crimfght_state::sound_w));
	map(0x6000, 0x7fff).bankr("rombank");                        /* banked ROM */
	map(0x8000, 0xffff).rom().region("maincpu", 0x18000);
}

void crimfght_state::bank0000_map(address_map &map)
{
	map(0x0000, 0x03ff).ram();
	map(0x0400, 0x07ff).ram().w(m_palette, FUNC(palette_device::write8)).share("palette");
}

// full memory map derived from schematics
void crimfght_state::crimfght_sound_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x87ff).mirror(0x1800).ram();
	map(0xa000, 0xa001).mirror(0x1ffe).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0xc000, 0xc000).mirror(0x1fff).r(m_soundlatch, FUNC(generic_latch_8_device::read));
	map(0xe000, 0xe00f).mirror(0x1ff0).rw(m_k007232, FUNC(k007232_device::read), FUNC(k007232_device::write));
}

/***************************************************************************

    Input Ports

***************************************************************************/

static INPUT_PORTS_START( crimfght )
	PORT_START("DSW1")
	PORT_DIPNAME(0x0f, 0x0f, DEF_STR( Coin_A )) PORT_DIPLOCATION("SW1:1,2,3,4")
	PORT_DIPSETTING(   0x02, DEF_STR( 4C_1C ))
	PORT_DIPSETTING(   0x05, DEF_STR( 3C_1C ))
	PORT_DIPSETTING(   0x08, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(   0x04, DEF_STR( 3C_2C ))
	PORT_DIPSETTING(   0x01, DEF_STR( 4C_3C ))
	PORT_DIPSETTING(   0x0f, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(   0x03, DEF_STR( 3C_4C ))
	PORT_DIPSETTING(   0x07, DEF_STR( 2C_3C ))
	PORT_DIPSETTING(   0x0e, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(   0x06, DEF_STR( 2C_5C ))
	PORT_DIPSETTING(   0x0d, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(   0x0c, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(   0x0b, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(   0x0a, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(   0x09, DEF_STR( 1C_7C ))
	PORT_DIPSETTING(   0x00, DEF_STR( Free_Play ))
	PORT_DIPNAME(0xf0, 0xf0, "Coin B") PORT_DIPLOCATION("SW1:5,6,7,8")
	PORT_DIPSETTING(   0x20, DEF_STR( 4C_1C ))
	PORT_DIPSETTING(   0x50, DEF_STR( 3C_1C ))
	PORT_DIPSETTING(   0x80, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(   0x40, DEF_STR( 3C_2C ))
	PORT_DIPSETTING(   0x10, DEF_STR( 4C_3C ))
	PORT_DIPSETTING(   0xf0, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(   0x30, DEF_STR( 3C_4C ))
	PORT_DIPSETTING(   0x70, DEF_STR( 2C_3C ))
	PORT_DIPSETTING(   0xe0, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(   0x60, DEF_STR( 2C_5C ))
	PORT_DIPSETTING(   0xd0, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(   0xc0, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(   0xb0, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(   0xa0, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(   0x90, DEF_STR( 1C_7C ))
	PORT_DIPSETTING(   0x00, DEF_STR( Unused ))

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:1,2")
		PORT_DIPSETTING(    0x03, "1" )
		PORT_DIPSETTING(    0x02, "2" )
		PORT_DIPSETTING(    0x01, "3" )
		PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPUNUSED_DIPLOC(0x04, 0x04, "SW2:3")
	PORT_DIPUNUSED_DIPLOC(0x08, 0x08, "SW2:4")
	PORT_DIPUNUSED_DIPLOC(0x10, 0x10, "SW2:5")
	PORT_DIPNAME(0x60, 0x40, DEF_STR( Difficulty ))  PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(   0x60, DEF_STR( Easy ))
	PORT_DIPSETTING(   0x40, DEF_STR( Normal ))
	PORT_DIPSETTING(   0x20, DEF_STR( Difficult ))
	PORT_DIPSETTING(   0x00, DEF_STR( Very_Difficult ))
	PORT_DIPNAME(0x80, 0x00, DEF_STR( Demo_Sounds )) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(   0x80, DEF_STR( Off ))
	PORT_DIPSETTING(   0x00, DEF_STR( On ))

	PORT_START("DSW3")
	PORT_DIPNAME(0x01, 0x01, DEF_STR( Flip_Screen )) PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(   0x01, DEF_STR( Off ))
	PORT_DIPSETTING(   0x00, DEF_STR( On ))
	PORT_DIPUNUSED_DIPLOC(0x02, IP_ACTIVE_LOW, "SW3:2")
	PORT_SERVICE_DIPLOC(  0x04, IP_ACTIVE_LOW, "SW3:3")
	PORT_DIPUNUSED_DIPLOC(0x08, IP_ACTIVE_LOW, "SW3:4")
	PORT_BIT(0xf0, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_CUSTOM_MEMBER(DEVICE_SELF, crimfght_state, system_r, nullptr)

	PORT_START("P1")
	KONAMI8_B123_START(1)

	PORT_START("P2")
	KONAMI8_B123_START(2)

	PORT_START("P3")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P4")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SYSTEM")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_COIN1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_COIN2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_SERVICE1)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_SERVICE2)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)
INPUT_PORTS_END

static INPUT_PORTS_START( crimfghtu )
	PORT_INCLUDE( crimfght )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME(0xf0, 0x00, "Coin B (Unused)") PORT_DIPLOCATION("SW1:5,6,7,8")
		PORT_DIPSETTING(   0x20, DEF_STR( 4C_1C ))
		PORT_DIPSETTING(   0x50, DEF_STR( 3C_1C ))
		PORT_DIPSETTING(   0x80, DEF_STR( 2C_1C ))
		PORT_DIPSETTING(   0x40, DEF_STR( 3C_2C ))
		PORT_DIPSETTING(   0x10, DEF_STR( 4C_3C ))
		PORT_DIPSETTING(   0xf0, DEF_STR( 1C_1C ))
		PORT_DIPSETTING(   0x30, DEF_STR( 3C_4C ))
		PORT_DIPSETTING(   0x70, DEF_STR( 2C_3C ))
		PORT_DIPSETTING(   0xe0, DEF_STR( 1C_2C ))
		PORT_DIPSETTING(   0x60, DEF_STR( 2C_5C ))
		PORT_DIPSETTING(   0xd0, DEF_STR( 1C_3C ))
		PORT_DIPSETTING(   0xc0, DEF_STR( 1C_4C ))
		PORT_DIPSETTING(   0xb0, DEF_STR( 1C_5C ))
		PORT_DIPSETTING(   0xa0, DEF_STR( 1C_6C ))
		PORT_DIPSETTING(   0x90, DEF_STR( 1C_7C ))
		PORT_DIPSETTING(   0x00, DEF_STR( Unused ))

	PORT_MODIFY("DSW2")
		PORT_DIPUNUSED_DIPLOC(0x01, 0x01, "SW2:1")
		PORT_DIPUNUSED_DIPLOC(0x02, 0x02, "SW2:2")

		PORT_MODIFY("P1")
		KONAMI8_B12_UNK(1)

		PORT_MODIFY("P2")
		KONAMI8_B12_UNK(2)

		PORT_MODIFY("P3")
		KONAMI8_B12_UNK(3)

		PORT_MODIFY("P4")
		KONAMI8_B12_UNK(4)

	PORT_MODIFY("SYSTEM")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE4 )
INPUT_PORTS_END


/***************************************************************************

    Machine Driver

***************************************************************************/

WRITE8_MEMBER(crimfght_state::volume_callback)
{
	m_k007232->set_volume(0, (data & 0x0f) * 0x11, 0);
	m_k007232->set_volume(1, 0, (data >> 4) * 0x11);
}

void crimfght_state::machine_start()
{
	m_rombank->configure_entries(0, 16, memregion("maincpu")->base(), 0x2000);
	m_rombank->set_entry(0);
}

WRITE8_MEMBER( crimfght_state::banking_callback )
{
	m_rombank->set_entry(data & 0x0f);

	/* bit 5 = select work RAM or palette */
	m_woco = BIT(data, 5);
	m_bank0000->set_bank(m_woco);

	/* bit 6 = enable char ROM reading through the video RAM */
	m_rmrd = BIT(data, 6);
	m_k052109->set_rmrd_line(m_rmrd ? ASSERT_LINE : CLEAR_LINE);

	m_init = BIT(data, 7);
}

CUSTOM_INPUT_MEMBER( crimfght_state::system_r )
{
	uint8_t data = 0;

	data |= 1 << 4; // VCC
	data |= m_woco << 5;
	data |= m_rmrd << 6;
	data |= m_init << 7;

	return data >> 4;
}

void crimfght_state::crimfght(machine_config &config)
{
	/* basic machine hardware */
	KONAMI(config, m_maincpu, XTAL(24'000'000)/8); /* 052001 (verified on pcb) */
	m_maincpu->set_addrmap(AS_PROGRAM, &crimfght_state::crimfght_map);
	m_maincpu->line().set(FUNC(crimfght_state::banking_callback));

	Z80(config, m_audiocpu, XTAL(3'579'545)); /* verified on pcb */
	m_audiocpu->set_addrmap(AS_PROGRAM, &crimfght_state::crimfght_sound_map);
	m_audiocpu->set_irq_acknowledge_callback(FUNC(crimfght_state::audiocpu_irq_ack));

	ADDRESS_MAP_BANK(config, "bank0000").set_map(&crimfght_state::bank0000_map).set_options(ENDIANNESS_BIG, 8, 11, 0x400);

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_raw(XTAL(24'000'000)/3, 528, 96, 416, 256, 16, 240); // measured 59.17
//  6MHz dotclock is more realistic, however needs drawing updates. replace when ready
//  screen.set_raw(XTAL(24'000'000)/4, 396, hbend, hbstart, 256, 16, 240);
	screen.set_screen_update(FUNC(crimfght_state::screen_update_crimfght));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_format(palette_device::xBGR_555, 512);
	m_palette->enable_shadows();

	K052109(config, m_k052109, 0);
	m_k052109->set_palette(m_palette);
	m_k052109->set_tile_callback(FUNC(crimfght_state::tile_callback), this);

	K051960(config, m_k051960, 0);
	m_k051960->set_palette(m_palette);
	m_k051960->set_screen_tag("screen");
	m_k051960->set_sprite_callback(FUNC(crimfght_state::sprite_callback), this);
	m_k051960->irq_handler().set_inputline(m_maincpu, KONAMI_IRQ_LINE);

	/* sound hardware */
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	GENERIC_LATCH_8(config, m_soundlatch);

	ym2151_device &ymsnd(YM2151(config, "ymsnd", XTAL(3'579'545)));  /* verified on pcb */
	ymsnd.port_write_handler().set(FUNC(crimfght_state::ym2151_ct_w));
	ymsnd.add_route(0, "lspeaker", 1.0);
	ymsnd.add_route(1, "rspeaker", 1.0);

	K007232(config, m_k007232, XTAL(3'579'545)); /* verified on pcb */
	m_k007232->port_write().set(FUNC(crimfght_state::volume_callback));
	m_k007232->add_route(0, "lspeaker", 0.20);
	m_k007232->add_route(0, "rspeaker", 0.20);
	m_k007232->add_route(1, "lspeaker", 0.20);
	m_k007232->add_route(1, "rspeaker", 0.20);
}

/***************************************************************************

  Game ROMs

***************************************************************************/

ROM_START( crimfght )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* code + banked roms */
	ROM_LOAD( "821r02.f24", 0x00000, 0x20000, CRC(4ecdd923) SHA1(78e5260c4bb9b18d7818fb6300d7e1d3a577fb63) )

	ROM_REGION( 0x10000, "audiocpu", 0 ) /* 64k for the sound CPU */
	ROM_LOAD( "821l01.h4",  0x0000, 0x8000, CRC(0faca89e) SHA1(21c9c6d736b398a29e8709e1187c5bf3cacdc99d) )

	ROM_REGION( 0x080000, "k052109", 0 )    /* tiles */
	ROM_LOAD32_WORD( "821k06.k13", 0x000000, 0x040000, CRC(a1eadb24) SHA1(ca305b904b34e03918ad07281fda86ad63caa44f) )
	ROM_LOAD32_WORD( "821k07.k19", 0x000002, 0x040000, CRC(060019fa) SHA1(c3bca007aaa5f1c534d2a75fe4f96d01a740dd58) )

	ROM_REGION( 0x100000, "k051960", 0 )    /* sprites */
	ROM_LOAD32_WORD( "821k04.k2",  0x000000, 0x080000, CRC(00e0291b) SHA1(39d5db6cf36826e47cdf5308eff9bfa8afc82050) )
	ROM_LOAD32_WORD( "821k05.k8",  0x000002, 0x080000, CRC(e09ea05d) SHA1(50ac9a2117ce63fe774c48d769ec445a83f1269e) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "821a08.i15", 0x0000, 0x0100, CRC(7da55800) SHA1(3826f73569c8ae0431510a355bdfa082152b74a5) )  /* priority encoder (not used) */

	ROM_REGION( 0x40000, "k007232", 0 ) /* data for the 007232 */
	ROM_LOAD( "821k03.e5",  0x00000, 0x40000, CRC(fef8505a) SHA1(5c5121609f69001838963e961cb227d6b64e4f5f) )
ROM_END

ROM_START( crimfghtj )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* code + banked roms */
	ROM_LOAD( "821p02.f24", 0x00000, 0x20000, CRC(f33fa2e1) SHA1(00fc9e8250fa51386f3af2fca0f137bec9e1c220) )

	ROM_REGION( 0x10000, "audiocpu", 0 ) /* 64k for the sound CPU */
	ROM_LOAD( "821l01.h4",  0x0000, 0x8000, CRC(0faca89e) SHA1(21c9c6d736b398a29e8709e1187c5bf3cacdc99d) )

	ROM_REGION( 0x080000, "k052109", 0 )    /* tiles */
	ROM_LOAD32_WORD( "821k06.k13", 0x000000, 0x040000, CRC(a1eadb24) SHA1(ca305b904b34e03918ad07281fda86ad63caa44f) )
	ROM_LOAD32_WORD( "821k07.k19", 0x000002, 0x040000, CRC(060019fa) SHA1(c3bca007aaa5f1c534d2a75fe4f96d01a740dd58) )

	ROM_REGION( 0x100000, "k051960", 0 )    /* sprites */
	ROM_LOAD32_WORD( "821k04.k2",  0x000000, 0x080000, CRC(00e0291b) SHA1(39d5db6cf36826e47cdf5308eff9bfa8afc82050) )  /* sprites */
	ROM_LOAD32_WORD( "821k05.k8",  0x000002, 0x080000, CRC(e09ea05d) SHA1(50ac9a2117ce63fe774c48d769ec445a83f1269e) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "821a08.i15", 0x0000, 0x0100, CRC(7da55800) SHA1(3826f73569c8ae0431510a355bdfa082152b74a5) )  /* priority encoder (not used) */

	ROM_REGION( 0x40000, "k007232", 0 ) /* data for the 007232 */
	ROM_LOAD( "821k03.e5",  0x00000, 0x40000, CRC(fef8505a) SHA1(5c5121609f69001838963e961cb227d6b64e4f5f) )
ROM_END

ROM_START( crimfghtu )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* code + banked roms */
		ROM_LOAD( "821l02.f24", 0x00000, 0x20000, CRC(588e7da6) SHA1(285febb3bcca31f82b34af3695a59eafae01cd30) )

	ROM_REGION( 0x10000, "audiocpu", 0 ) /* 64k for the sound CPU */
	ROM_LOAD( "821l01.h4",  0x0000, 0x8000, CRC(0faca89e) SHA1(21c9c6d736b398a29e8709e1187c5bf3cacdc99d) )

	ROM_REGION( 0x080000, "k052109", 0 )    /* tiles */
	ROM_LOAD32_WORD( "821k06.k13", 0x000000, 0x040000, CRC(a1eadb24) SHA1(ca305b904b34e03918ad07281fda86ad63caa44f) )
	ROM_LOAD32_WORD( "821k07.k19", 0x000002, 0x040000, CRC(060019fa) SHA1(c3bca007aaa5f1c534d2a75fe4f96d01a740dd58) )

	ROM_REGION( 0x100000, "k051960", 0 )    /* sprites */
	ROM_LOAD32_WORD( "821k04.k2",  0x000000, 0x080000, CRC(00e0291b) SHA1(39d5db6cf36826e47cdf5308eff9bfa8afc82050) )  /* sprites */
	ROM_LOAD32_WORD( "821k05.k8",  0x000002, 0x080000, CRC(e09ea05d) SHA1(50ac9a2117ce63fe774c48d769ec445a83f1269e) )

	ROM_REGION( 0x0100, "proms", 0 )
	ROM_LOAD( "821a08.i15", 0x0000, 0x0100, CRC(7da55800) SHA1(3826f73569c8ae0431510a355bdfa082152b74a5) )  /* priority encoder (not used) */

	ROM_REGION( 0x40000, "k007232", 0 ) /* data for the 007232 */
	ROM_LOAD( "821k03.e5",  0x00000, 0x40000, CRC(fef8505a) SHA1(5c5121609f69001838963e961cb227d6b64e4f5f) )
ROM_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

GAME( 1989, crimfght,  0,        crimfght, crimfght,  crimfght_state, empty_init, ROT0, "Konami", "Crime Fighters (World 2 players)", MACHINE_SUPPORTS_SAVE )
GAME( 1989, crimfghtu, crimfght, crimfght, crimfghtu, crimfght_state, empty_init, ROT0, "Konami", "Crime Fighters (US 4 Players)",    MACHINE_SUPPORTS_SAVE )
GAME( 1989, crimfghtj, crimfght, crimfght, crimfght,  crimfght_state, empty_init, ROT0, "Konami", "Crime Fighters (Japan 2 Players)", MACHINE_SUPPORTS_SAVE )
