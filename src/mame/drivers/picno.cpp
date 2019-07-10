// license:BSD-3-Clause
// copyright-holders:Robbbert
/******************************************************************************************************************************

Konami Picno and Picno2

Skeleton driver started on 2017-11-30, can be claimed by anyone interested.

Information provided by Team Europe.

Chips: HD6435328F10 (H8/532 CPU with inbuilt ROM), HN62334BP (27c040 ROM), Konami custom chip 054715 (rectangular 100 pins),
       HM538121JP-10, M514256B-70J, OKI M6585.
Crystals: D200L2 (Y1) and D214A3 (Y2), frequencies unknown.

The hardware of the Picno 1 and Picno 2 is completely the same. The Picno 1 has an Audio-Line-Out, which the Picno 2 does not have.

Maskrom of PICNO 1: RX001-Z8-V3J
Maskrom of PICNO 2: RX001-Z8-V4J

The size of the address space and other things is controlled by the 3 mode pins. It's assumed we are in Mode 4.

Can't do anything until the internal ROM is dumped.

******************************************************************************************************************************/

#include "emu.h"
#include "cpu/h8/h83002.h"

#include "bus/generic/slot.h"
#include "bus/generic/carts.h"

//#include "sound/multipcm.h"
//#include "screen.h"
//#include "speaker.h"

class picno_state : public driver_device
{
public:
	picno_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{ }

	void picno(machine_config &config);

private:
	void io_map(address_map &map);
	void mem_map(address_map &map);

	required_device<cpu_device> m_maincpu;
};

void picno_state::mem_map(address_map &map)
{
	map(0x00000, 0x07fff).rom().region("roms", 0); // 32kb internal rom
	map(0x0fb80, 0x0ff7f).ram(); // internal ram
	map(0x0ff80, 0x0ffff); // internal controls
	map(0x10000, 0x8ffff).rom().region("roms", 0x8000); // guess
}

void picno_state::io_map(address_map &map)
{
//  ADDRESS_MAP_GLOBAL_MASK(0xff)
}

static INPUT_PORTS_START( picno )
INPUT_PORTS_END

void picno_state::picno(machine_config &config)
{
	/* basic machine hardware */
	H83002(config, m_maincpu, XTAL(20'000'000)); /* TODO: correct CPU type (H8/532), crystal is a guess, divided by 2 in the cpu */
	m_maincpu->set_addrmap(AS_PROGRAM, &picno_state::mem_map);
	m_maincpu->set_addrmap(AS_IO, &picno_state::io_map);

	//MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker") // no speaker in the unit, but there's a couple of sockets on the back
	//MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	//MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)

	GENERIC_CARTSLOT(config, "cartslot", generic_linear_slot, "picno_cart");

	SOFTWARE_LIST(config, "cart_list").set_original("picno");
}

ROM_START( picno )
	ROM_REGION(0x88000, "roms", 0)
	ROM_LOAD( "hd6435328f10.u5", 0x00000, 0x08000, NO_DUMP ) // internal rom
	ROM_LOAD( "rx001-z8-v3j.u2",    0x08000, 0x80000, CRC(e3c8929d) SHA1(1716f09b0a594b3782d257330282d77b6ca6fa0d) ) //HN62334BP
ROM_END

ROM_START( picno2 )
	ROM_REGION(0x88000, "roms", 0)
	ROM_LOAD( "hd6435328f10.u5", 0x00000, 0x08000, NO_DUMP ) // internal rom
	ROM_LOAD( "rx001-z8-v4j.u2",    0x08000, 0x80000, CRC(ae89a9a5) SHA1(51ed458ffd151e19019beb23517263efce4be272) ) //HN62334BP
ROM_END

//    YEAR  NAME    PARENT  COMPAT  MACHINE  INPUT  CLASS        INIT        COMPANY   FULLNAME   FLAGS
CONS( 1993, picno,  0,      0,      picno,   picno, picno_state, empty_init, "Konami", "Picno",   MACHINE_IS_SKELETON )
CONS( 1993, picno2, 0,      0,      picno,   picno, picno_state, empty_init, "Konami", "Picno 2", MACHINE_IS_SKELETON )
