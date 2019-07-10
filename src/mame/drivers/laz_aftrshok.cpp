// license:BSD-3-Clause
// copyright-holders:David Haywood
/*

After Shock by Laser Tron

redemption game

devices are 27c512

--------------------------

file Aftrshk.upd is the updated eprom for an update version.
The update version uses a small (appx 2" x 4" ) pcb to turn on
the playfield motor.  If you don't have this small pcb, then don't use
this .upd version software.  The small pcb is numbered

"pcb100067"
"Lazer Tron driver pcb V.02"

This "kit" will correct aproblem with the Allegro UCN5801A chip
on the main pcb.
full instructions available from Lazer-Tron aka Arcade Planet


a video of this in action can be seen at
https://www.youtube.com/watch?v=9DIhuOEVwf4


*/

#include "emu.h"
#include "cpu/mcs51/mcs51.h"
#include "sound/okim6295.h"
#include "speaker.h"


class aftrshok_state : public driver_device
{
public:
	aftrshok_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_oki(*this, "oki")
	{ }

	void aftrshok(machine_config &config);

private:
	void sound_data_w(u8 data);
	void mcu_p3_w(u8 data);

	void prog_map(address_map &map);
	void ext_map(address_map &map);

	virtual void machine_start() override;
	virtual void machine_reset() override;

	required_device<mcs51_cpu_device> m_maincpu;
	required_device<okim6295_device> m_oki;

	u8 m_sound_data;
};

static INPUT_PORTS_START( aftrshok )
	PORT_START("IN0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)

	PORT_START("IN1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)

	PORT_START("IN2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)

	PORT_START("IN3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)

	PORT_START("IN4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNKNOWN)
INPUT_PORTS_END



void aftrshok_state::machine_start()
{
	save_item(NAME(m_sound_data));
}

void aftrshok_state::machine_reset()
{
	m_sound_data = 0;
}

void aftrshok_state::sound_data_w(u8 data)
{
	m_sound_data = data;
}

void aftrshok_state::mcu_p3_w(u8 data)
{
	if (!BIT(data, 1) && !BIT(data, 2))
		m_oki->write(m_sound_data);
}

void aftrshok_state::prog_map(address_map &map)
{
	map(0x0000, 0xffff).rom().region("maincpu", 0);
}

void aftrshok_state::ext_map(address_map &map)
{
	map(0x0700, 0x0700).nopw();
	map(0x0800, 0x0800).nopw();
	map(0x1000, 0x1000).w(FUNC(aftrshok_state::sound_data_w));
	map(0x1800, 0x1800).nopw();
	map(0x2000, 0x2000).nopw();
	map(0x2800, 0x2800).nopw();
	map(0x4000, 0x4000).portr("IN0");
	map(0x4800, 0x4800).portr("IN1");
	map(0x5000, 0x5000).portr("IN2");
	map(0x5800, 0x5800).portr("IN3");
	map(0x6000, 0x6000).portr("IN4");
	map(0x8000, 0x9fff).ram();
}


void aftrshok_state::aftrshok(machine_config &config)
{
	/* basic machine hardware */
	I8031(config, m_maincpu, 12_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &aftrshok_state::prog_map);
	m_maincpu->set_addrmap(AS_IO, &aftrshok_state::ext_map);
	m_maincpu->port_out_cb<3>().set(FUNC(aftrshok_state::mcu_p3_w));

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	OKIM6295(config, m_oki, 2.097152_MHz_XTAL, okim6295_device::PIN7_HIGH);
	m_oki->add_route(ALL_OUTPUTS, "mono", 1.0);
}



ROM_START( aftrshok )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "aftrshok.u3.upd", 0x00000, 0x10000, CRC(779fad60) SHA1(6be3b99cea95b5320c6d500616a703cdab126d9c) ) // see note at top of driver about this update

	ROM_REGION( 0xc0000, "oki", 0 )
	ROM_LOAD( "aftrshok.u27", 0x00000, 0x10000, CRC(2d0061ef) SHA1(cc674ea020ef9e6b3baecfdb72f9766fef89bed8) )
	ROM_LOAD( "aftrshok.u26", 0x10000, 0x10000, CRC(d2b55dc1) SHA1(2684bfc65628a550fcbaa6726b5dab488e7ede5a) )
	ROM_LOAD( "aftrshok.u25", 0x20000, 0x10000, CRC(d5d1c606) SHA1(ad72a00c211ee7f5bc0772d6f469d59047131095) )
ROM_END

ROM_START( aftrshoka )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "aftrshok.u3", 0x00000, 0x10000, CRC(841828b9) SHA1(51a88ce7466fcbc6d5192d738425ae1d37c1b88e) )

	ROM_REGION( 0xc0000, "oki", 0 )
	ROM_LOAD( "aftrshok.u27", 0x00000, 0x10000, CRC(2d0061ef) SHA1(cc674ea020ef9e6b3baecfdb72f9766fef89bed8) )
	ROM_LOAD( "aftrshok.u26", 0x10000, 0x10000, CRC(d2b55dc1) SHA1(2684bfc65628a550fcbaa6726b5dab488e7ede5a) )
	ROM_LOAD( "aftrshok.u25", 0x20000, 0x10000, CRC(d5d1c606) SHA1(ad72a00c211ee7f5bc0772d6f469d59047131095) )
ROM_END


GAME( 19??, aftrshok,  0,        aftrshok, aftrshok, aftrshok_state, empty_init, ROT0, "Lazer-tron", "After Shock (Lazer-tron, set 1)", MACHINE_NOT_WORKING | MACHINE_MECHANICAL )
GAME( 19??, aftrshoka, aftrshok, aftrshok, aftrshok, aftrshok_state, empty_init, ROT0, "Lazer-tron", "After Shock (Lazer-tron, set 2)", MACHINE_NOT_WORKING | MACHINE_MECHANICAL )
