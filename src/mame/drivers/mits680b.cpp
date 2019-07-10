// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic, Robbbert
/***************************************************************************

MITS Altair 680b

2009-12-03 Skeleton driver.
2011-06-08 Connected to a terminal

Monitor Commands:
J
L switch to terminal mode
M
N modify memory in a limited way
P this does a rti and causes a momentary crash. Weird.


ToDo:


****************************************************************************/

#include "emu.h"
#include "cpu/m6800/m6800.h"
#include "machine/6850acia.h"
#include "bus/rs232/rs232.h"
#include "machine/clock.h"


class mits680b_state : public driver_device
{
public:
	mits680b_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{ }

	void mits680b(machine_config &config);

private:
	DECLARE_READ8_MEMBER(status_check_r);

	void mem_map(address_map &map);

	required_device<cpu_device> m_maincpu;
};

READ8_MEMBER( mits680b_state::status_check_r )
{
	return 0; // crashes at start if bit 7 high
}


void mits680b_state::mem_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x03ff).ram(); // 1024 bytes RAM
	map(0xf000, 0xf001).rw("acia", FUNC(acia6850_device::read), FUNC(acia6850_device::write));
	map(0xf002, 0xf002).r(FUNC(mits680b_state::status_check_r));
	map(0xff00, 0xffff).rom().region("roms", 0);
}

/* Input ports */
static INPUT_PORTS_START( mits680b )
INPUT_PORTS_END


void mits680b_state::mits680b(machine_config &config)
{
	/* basic machine hardware */
	M6800(config, m_maincpu, XTAL(1'000'000) / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &mits680b_state::mem_map);

	clock_device &uart_clock(CLOCK(config, "uart_clock", 153600));
	uart_clock.signal_handler().set("acia", FUNC(acia6850_device::write_txc));
	uart_clock.signal_handler().append("acia", FUNC(acia6850_device::write_rxc));

	acia6850_device &acia(ACIA6850(config, "acia", 0));
	acia.txd_handler().set("rs232", FUNC(rs232_port_device::write_txd));
	acia.rts_handler().set("rs232", FUNC(rs232_port_device::write_rts));

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, "terminal"));
	rs232.rxd_handler().set("acia", FUNC(acia6850_device::write_rxd));
	rs232.cts_handler().set("acia", FUNC(acia6850_device::write_cts));
}

/* ROM definition */
ROM_START( mits680b )
	ROM_REGION( 0x100, "roms", ROMREGION_ERASEFF )
	ROM_LOAD( "mits680b.bin", 0x0000, 0x0100, CRC(397e717f) SHA1(257d3eb1343b8611dc05455aeed33615d581f29c))
ROM_END

/* Driver */

//    YEAR  NAME      PARENT  COMPAT  MACHINE   INPUT     CLASS           INIT        COMPANY  FULLNAME       FLAGS
COMP( 1976, mits680b, 0,      0,      mits680b, mits680b, mits680b_state, empty_init, "MITS",  "Altair 680b", MACHINE_NOT_WORKING | MACHINE_NO_SOUND)
