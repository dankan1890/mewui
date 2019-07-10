// license:BSD-3-Clause
// copyright-holders:Robbbert
/*

Micromon 7141 ECG unit for hospitals.

No manuals or schematic found.

From the photos, we can see a CDP1802ACE CPU, 2.4576 crystal,
a GM76C28A-10 2Kx8 static RAM (like 6116), 2 ROMS and a bunch of small chips.
There's also 4 dipswitches in a single package. 1=on,2=off,3=off,4=on.
The unit has an audible alarm, presumably a speaker, but it doesn't appear in any photo.

The front panel has a CRT display (looks like cyan on black), some buttons with unknown
symbols, a 5-position rotary switch, a plug for the ECG device, a socket called "Sync"
and some LEDs.

Date of manufacture unknown, however the chips have date codes of 1994 and 1995.

*****************************************************************************************/

#include "emu.h"
#include "cpu/cosmac/cosmac.h"


class micromon_state : public driver_device
{
public:
	micromon_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{ }

	void micromon(machine_config &config);

private:
	DECLARE_READ_LINE_MEMBER(clear_r);

	void micromon_io(address_map &map);
	void micromon_mem(address_map &map);

	virtual void machine_reset() override;
	uint8_t m_resetcnt;
	required_device<cosmac_device> m_maincpu;
};

void micromon_state::micromon_mem(address_map &map)
{
	map(0x0000, 0x1fff).rom().region("maincpu", 0);
	map(0x2000, 0x3fff).ram();
}

void micromon_state::micromon_io(address_map &map)
{
}

static INPUT_PORTS_START( micromon )
INPUT_PORTS_END

READ_LINE_MEMBER( micromon_state::clear_r )
{
	if (m_resetcnt < 0x10)
		m_maincpu->set_state_int(cosmac_device::COSMAC_R0, 0x0000);
	if (m_resetcnt < 0x20)
		m_resetcnt++;
	// set reset pin to normal
	return 1;
}

void micromon_state::machine_reset()
{
	m_resetcnt = 0;
}


void micromon_state::micromon(machine_config &config)
{
	// basic machine hardware
	CDP1802(config, m_maincpu, 2457600);
	m_maincpu->set_addrmap(AS_PROGRAM, &micromon_state::micromon_mem);
	m_maincpu->set_addrmap(AS_IO, &micromon_state::micromon_io);
	m_maincpu->wait_cb().set_constant(1);
	m_maincpu->clear_cb().set(FUNC(micromon_state::clear_r));

	// video hardware

	// sound
}

ROM_START( micromon7141 )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "721421_rev4.0_25_7_95.ic2",  0x0000, 0x1000, CRC(b2a26439) SHA1(66a65d19b3cff185e82b10fc7ecb965c51751b7c) )
	ROM_LOAD( "702423_rev4.0_25_7_95.ic41", 0x1000, 0x1000, CRC(5efe6b4b) SHA1(b3670c53e2527e824cc22e4a54db9abf5a07239f) )
ROM_END

SYST( 1995?, micromon7141, 0, 0, micromon, micromon, micromon_state, empty_init, "Kontron Instruments",  "Micromon 7141 ECG unit",  MACHINE_IS_SKELETON )
