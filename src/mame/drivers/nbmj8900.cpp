// license:BSD-3-Clause
// copyright-holders:Takahiro Nogi
/******************************************************************************

    nbmj8900 - Nichibutsu Mahjong games for years 1989

    Driver by Takahiro Nogi <nogi@kt.rim.or.jp> 2007/05/13 -

******************************************************************************/
/******************************************************************************

Notes:

TODO:

- Real machine has ROMs for protection, but I don't know how to access the ROM,
  so I'm doing something that works but is probably wrong.
  The interesting thing about that ROM is that it comes from other, older games,
  so it isn't needed, it's just verified for protection.

- Some games display "GFXROM BANK OVER!!" or "GFXROM ADDRESS OVER!!"
  in Debug build.

- Screen flipping is not perfect.

******************************************************************************/

#include "emu.h"
#include "includes/nbmj8900.h"

#include "cpu/z80/z80.h"
#include "sound/3812intf.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "speaker.h"


void nbmj8900_state::init_ohpaipee()
{
#if 0
	uint8_t *prot = memregion("protdata")->base();
	int i;

	/* this is one possible way to rearrange the protection ROM data to get the
	   expected 0x8374 checksum. It's probably completely wrong! But since the
	   game doesn't do anything else with that ROM, this is more than enough. I
	   could just fill this are with fake data, the only thing that matters is
	   the checksum. */

	for (i = 0;i < 0x20000;i++)
	{
		prot[i] = bitswap<8>(prot[i],2,7,3,5,0,6,4,1);
	}
#else
	unsigned char *ROM = memregion("maincpu")->base();

	// Protection ROM check skip
	ROM[0x00e4] = 0x00;
	ROM[0x00e5] = 0x00;
	ROM[0x00e6] = 0x00;
	// Program ROM SUM check skip
	ROM[0x025c] = 0x00;
	ROM[0x025d] = 0x00;
#endif
}

void nbmj8900_state::init_togenkyo()
{
#if 0
	uint8_t *prot = memregion("protdata")->base();
	int i;

	/* this is one possible way to rearrange the protection ROM data to get the
	   expected 0x5ece checksum. It's probably completely wrong! But since the
	   game doesn't do anything else with that ROM, this is more than enough. I
	   could just fill this are with fake data, the only thing that matters is
	   the checksum. */
	for (int i = 0; i < 0x20000; i++)
	{
		prot[i] = bitswap<8>(prot[i],2,7,3,5,0,6,4,1);
	}
#else
	unsigned char *ROM = memregion("maincpu")->base();

	// Protection ROM check skip
	ROM[0x010b] = 0x00;
	ROM[0x010c] = 0x00;
	ROM[0x010d] = 0x00;
	// Program ROM SUM check skip
//  ROM[0x025c] = 0x00;
//  ROM[0x025d] = 0x00;
#endif
}


void nbmj8900_state::ohpaipee_map(address_map &map)
{
	map(0x0000, 0xefff).rom();
	map(0xf000, 0xf00f).rw(FUNC(nbmj8900_state::clut_r), FUNC(nbmj8900_state::clut_w));
	map(0xf400, 0xf5ff).rw(FUNC(nbmj8900_state::palette_type1_r), FUNC(nbmj8900_state::palette_type1_w));
	map(0xf800, 0xffff).ram();
}

void nbmj8900_state::togenkyo_map(address_map &map)
{
	map(0x0000, 0xefff).rom();
	map(0xf000, 0xf00f).rw(FUNC(nbmj8900_state::clut_r), FUNC(nbmj8900_state::clut_w));
	map(0xf400, 0xf5ff).rw(FUNC(nbmj8900_state::palette_type1_r), FUNC(nbmj8900_state::palette_type1_w));
	map(0xf800, 0xffff).ram();
}

void nbmj8900_state::ohpaipee_io_map(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x7f).r(m_nb1413m3, FUNC(nb1413m3_device::sndrom_r));
	map(0x00, 0x00).w(m_nb1413m3, FUNC(nb1413m3_device::nmi_clock_w));
	map(0x20, 0x27).w(FUNC(nbmj8900_state::blitter_w));

	map(0x40, 0x40).w(FUNC(nbmj8900_state::clutsel_w));
	map(0x60, 0x60).w(FUNC(nbmj8900_state::romsel_w));
	map(0x70, 0x70).w(FUNC(nbmj8900_state::scrolly_w));

	map(0x80, 0x81).rw("ymsnd", FUNC(ym3812_device::read), FUNC(ym3812_device::write));

	map(0x90, 0x90).r(m_nb1413m3, FUNC(nb1413m3_device::inputport0_r));

	map(0xa0, 0xa0).rw(m_nb1413m3, FUNC(nb1413m3_device::inputport1_r), FUNC(nb1413m3_device::inputportsel_w));
	map(0xb0, 0xb0).rw(m_nb1413m3, FUNC(nb1413m3_device::inputport2_r), FUNC(nb1413m3_device::sndrombank1_w));
	map(0xc0, 0xc0).r(m_nb1413m3, FUNC(nb1413m3_device::inputport3_r));
	map(0xd0, 0xd0).w("dac", FUNC(dac_byte_interface::data_w));
	map(0xe0, 0xe0).w(FUNC(nbmj8900_state::vramsel_w));
	map(0xf0, 0xf0).r(m_nb1413m3, FUNC(nb1413m3_device::dipsw1_r));
	map(0xf1, 0xf1).rw(m_nb1413m3, FUNC(nb1413m3_device::dipsw2_r), FUNC(nb1413m3_device::outcoin_w));
}


static INPUT_PORTS_START( ohpaipee )
	PORT_START("DSWA")
	PORT_DIPNAME( 0x01, 0x01, "DIPSW 1-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "DIPSW 1-2" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Game Sounds" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "DIPSW 1-7" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Character Display Test" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSWB")
	PORT_DIPNAME( 0x01, 0x01, "DIPSW 2-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "DIPSW 2-2" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "DIPSW 2-3" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "DIPSW 2-4" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "DIPSW 2-5" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "DIPSW 2-6" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "DIPSW 2-7" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "DIPSW 2-8" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )         // DRAW BUSY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE( 0x10, IP_ACTIVE_LOW )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 )        PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )        PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 )        PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )        PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(2) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(2) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_PLAYER(2) PORT_4WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( togenkyo )
	PORT_START("DSWA")
	PORT_DIPNAME( 0x01, 0x01, "DIPSW 1-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "DIPSW 1-2" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Game Sounds" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "DIPSW 1-7" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Character Display Test" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSWB")
	PORT_DIPNAME( 0x01, 0x01, "DIPSW 2-1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "DIPSW 2-2" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "DIPSW 2-3" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "DIPSW 2-4" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, "DIPSW 2-5" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "DIPSW 2-6" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "DIPSW 2-7" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "DIPSW 2-8" )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )         // DRAW BUSY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE( 0x10, IP_ACTIVE_LOW )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 )        PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )        PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_PLAYER(1) PORT_4WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON2 )        PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON1 )        PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_PLAYER(2) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_PLAYER(2) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_PLAYER(2) PORT_4WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END



void nbmj8900_state::ohpaipee(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 20000000/4);    /* 5.00 MHz ? */
	m_maincpu->set_addrmap(AS_PROGRAM, &nbmj8900_state::ohpaipee_map);
	m_maincpu->set_addrmap(AS_IO, &nbmj8900_state::ohpaipee_io_map);
	m_maincpu->set_vblank_int("screen", FUNC(nbmj8900_state::irq0_line_hold));

	NB1413M3(config, m_nb1413m3, 0, NB1413M3_OHPAIPEE);

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(0));
	m_screen->set_size(512, 256);
	m_screen->set_visarea(0, 512-1, 8, 248-1);
	m_screen->set_screen_update(FUNC(nbmj8900_state::screen_update));
	m_screen->set_palette("palette");

	PALETTE(config, "palette").set_entries(256);


	/* sound hardware */
	SPEAKER(config, "speaker").front_center();

	YM3812(config, "ymsnd", 2500000).add_route(ALL_OUTPUTS, "speaker", 0.7);

	DAC_8BIT_R2R(config, "dac", 0).add_route(ALL_OUTPUTS, "speaker", 0.42); // unknown DAC
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref"));
	vref.add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
	vref.add_route(0, "dac", -1.0, DAC_VREF_NEG_INPUT);
}

void nbmj8900_state::togenkyo(machine_config &config)
{
	ohpaipee(config);

	/* basic machine hardware */
	m_maincpu->set_addrmap(AS_PROGRAM, &nbmj8900_state::togenkyo_map);

	m_nb1413m3->set_type(NB1413M3_TOGENKYO);
}


ROM_START( ohpaipee )
	ROM_REGION( 0x10000, "maincpu", 0 ) /* program */
	ROM_LOAD( "02.3h",  0x00000, 0x10000, CRC(2b6c9afc) SHA1(591a7016ebd99d4a2bfdef5e99da3a1ac9d30d75) )

	ROM_REGION( 0x20000, "voice", 0 ) /* voice */
	ROM_LOAD( "01.2k",  0x00000, 0x10000, CRC(6ea76e01) SHA1(194a80e4a3a9d660aea0a9790ce1f4e295bae7ab) )

	ROM_REGION( 0x200000, "gfx", 0 ) /* gfx */
	ROM_LOAD( "03.8c",  0x000000, 0x10000, CRC(33b12763) SHA1(62b9753b65bebad9255a60375d2cf257496a085d) )
	ROM_LOAD( "04.8d",  0x010000, 0x10000, CRC(303fcf10) SHA1(6275a38f319665c4352ca6814f8bf2b3d1739b41) )
	ROM_LOAD( "05.8f",  0x020000, 0x10000, CRC(ce394575) SHA1(bdfcafed983b705474f3be3ce8f9a5aea8b33bc1) )
	ROM_LOAD( "06.8f",  0x030000, 0x10000, CRC(9d943b6e) SHA1(d605f6e95cbc09124a73987941d17c53bd4fabf2) )
	ROM_LOAD( "07.8h",  0x040000, 0x10000, CRC(40c25d2f) SHA1(11fe84be8f15a37a505dd8d5c82dbaf0366f266a) )
	ROM_LOAD( "08.8j",  0x050000, 0x10000, CRC(65520a0e) SHA1(b62cfc3d1ee00e309196a16645ff58a31cb45081) )
	ROM_LOAD( "09.8k",  0x060000, 0x10000, CRC(3f4940f9) SHA1(410ab6e429b65d577eccef3ffcfdec912442a4b0) )
	ROM_LOAD( "10.8l",  0x070000, 0x10000, CRC(325c80ff) SHA1(c9612db209b74a56fd40ddd534d24a44e4df3874) )
	ROM_LOAD( "11.8m",  0x080000, 0x10000, CRC(d779661b) SHA1(914f29a1dde2861542ced28735441b05a520409a) )

	ROM_REGION( 0x40000, "protdata", 0 ) /* protection data */
	ROM_LOAD( "4i.bin", 0x000000, 0x40000, CRC(88f33049) SHA1(8b2d019b09ed854f40a8b0c7782645f50b1f2900) )   // same as housemnq/4i.bin gfx data
ROM_END

ROM_START( togenkyo )
	ROM_REGION( 0x10000, "maincpu", 0 ) /* program */
	ROM_LOAD( "02.3h",  0x00000, 0x10000, CRC(a0cc6700) SHA1(49132e00d15aa00f065bbac5e850d08032845ac7) )

	ROM_REGION( 0x20000, "voice", 0 ) /* voice */
	ROM_LOAD( "01.2k",  0x00000, 0x10000, CRC(5346786b) SHA1(7141b38339ec4f2b1c6d0a604b5e70cb9beccdf1) )

	ROM_REGION( 0x200000, "gfx", 0 ) /* gfx */
	ROM_LOAD( "03.8c",  0x000000, 0x10000, CRC(ce64cb8b) SHA1(b1f99991e19be49d1aac774c79acc527c9379245) )
	ROM_LOAD( "04.8d",  0x010000, 0x10000, CRC(50dd908f) SHA1(28bd5824c55e16c1d62c24577831723c9aded057) )
	ROM_LOAD( "05.8f",  0x020000, 0x10000, CRC(903004e0) SHA1(c017af37cb90b5335bd53b5b62e883be590353fb) )
	ROM_LOAD( "06.8f",  0x030000, 0x10000, CRC(fa47c1e6) SHA1(ebd365fec250056366422361d3c722ae7b30a0a4) )
	ROM_LOAD( "07.8h",  0x040000, 0x10000, CRC(741bde2a) SHA1(9bf1680dc93def5ea6de929eebf05697f6ea43d1) )
	ROM_LOAD( "08.8j",  0x050000, 0x10000, CRC(c3dd2339) SHA1(6cae7b26ff09dae3758c0c8060731f6c777c5b40) )
	ROM_LOAD( "09.8k",  0x060000, 0x10000, CRC(afb1c766) SHA1(58d4c6d00276ebebaca7a42fc47350d5ea310b8e) )
	ROM_LOAD( "10.8l",  0x070000, 0x10000, CRC(18e2a1d4) SHA1(82750e34ba28e10ae3ab935eafd49643d3d057cc) )
	ROM_LOAD( "11.8m",  0x080000, 0x10000, CRC(811682c2) SHA1(5dfc78ce409d8932cf078ced2616e950a52d5d0e) )
	ROM_LOAD( "12.8n",  0x090000, 0x10000, CRC(97808a68) SHA1(3738b2d0ec0dcd1aea19103ffccafde0e84d6c71) )
	ROM_LOAD( "13.10c", 0x0a0000, 0x10000, CRC(ac61612d) SHA1(29854c5e758a2962daa6e281f7c6af87624c53d8) )
	ROM_LOAD( "14.10d", 0x0b0000, 0x10000, CRC(cb472acc) SHA1(82b4089412ecded903745e5382a301c53a483698) )

	ROM_REGION( 0x40000, "protdata", 0 ) /* protection data */
	ROM_LOAD( "4i.bin", 0x000000, 0x40000, CRC(88f33049) SHA1(8b2d019b09ed854f40a8b0c7782645f50b1f2900) )   // same as housemnq/4i.bin gfx data
ROM_END

//    YEAR,     NAME,   PARENT,  MACHINE,    INPUT,     INIT, MONITOR,COMPANY,FULLNAME,FLAGS)
GAME( 1989, ohpaipee,        0, ohpaipee, ohpaipee, nbmj8900_state, init_ohpaipee,  ROT270, "Nichibutsu", "Oh! Paipee (Japan 890227)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE )
GAME( 1989, togenkyo,        0, togenkyo, togenkyo, nbmj8900_state, init_togenkyo,    ROT0, "Nichibutsu", "Tougenkyou (Japan 890418)", MACHINE_SUPPORTS_SAVE )
