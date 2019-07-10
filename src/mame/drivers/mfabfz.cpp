// license:BSD-3-Clause
// copyright-holders:rfka01, Robbbert
/***************************************************************************

Mikrocomputer fuer Ausbildung
Berufsfoerdungszentrum Essen
Information found on Wikipedia:
- System is a backbone upon which all functions are available on plug-in cards
- 32k RAM, 32k ROM
- Serial is via CPU SID/SOD pins, but can be replaced by 8251 on RS232 card
- Protocol is COM1, 4800, 8N1
- Timer card uses 8253
- PIO card uses 8255 and has a printer interface
- Cassette interface uses RS232 card
- Optional floppy (both sizes); and a EPROM burner
- OS: MAT85

Manuals have no schematics and no mention of the 8253 or 8255. The bios
doesn't try communicating with them either.

Commands:
A     Assembler
B     Set Breakpoint
D     Disassembler
G     Go
H     Help
I     Inport
M     Print/Modify memory (A=ascii, B=bit, H=hex)
N     Turn on tracer & step to next instruction
O     Outport
P     Display memory contents in various formats
R     Set initial register contents
T     Trace interval

Pressing enter will change the prompt from KMD > to KMD+> and pressing
space will change it back.

mfabfz85 -bios 1 : produces no output
others are working

****************************************************************************/

#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "machine/clock.h"
#include "machine/i8251.h"
#include "bus/rs232/rs232.h"


class mfabfz_state : public driver_device
{
public:
	mfabfz_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{ }

	void mfabfz85(machine_config &config);
	void mfabfz(machine_config &config);

private:
	void mfabfz85_io(address_map &map);
	void mfabfz_io(address_map &map);
	void mfabfz_mem(address_map &map);
	virtual void machine_reset() override;
	required_device<cpu_device> m_maincpu;
};


void mfabfz_state::mfabfz_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x7fff).rom().region("roms", 0);
	map(0x8000, 0xffff).ram();
}

void mfabfz_state::mfabfz_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	map(0xbe, 0xbf).rw("uart1", FUNC(i8251_device::read), FUNC(i8251_device::write));
	map(0xfe, 0xff).rw("uart2", FUNC(i8251_device::read), FUNC(i8251_device::write));
}

void mfabfz_state::mfabfz85_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	map(0xfe, 0xff).rw("uart2", FUNC(i8251_device::read), FUNC(i8251_device::write));
}

/* Input ports */
static INPUT_PORTS_START( mfabfz )
INPUT_PORTS_END


void mfabfz_state::machine_reset()
{
}


void mfabfz_state::mfabfz(machine_config &config)
{
	/* basic machine hardware */
	I8085A(config, m_maincpu, 4_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &mfabfz_state::mfabfz_mem);
	m_maincpu->set_addrmap(AS_IO, &mfabfz_state::mfabfz_io);

	// uart1 - terminal - clock hardware unknown
	clock_device &uart1_clock(CLOCK(config, "uart1_clock", 153600));
	uart1_clock.signal_handler().set("uart1", FUNC(i8251_device::write_txc));
	uart1_clock.signal_handler().append("uart1", FUNC(i8251_device::write_rxc));

	i8251_device &uart1(I8251(config, "uart1", 0));
	uart1.txd_handler().set("rs232", FUNC(rs232_port_device::write_txd));
	uart1.dtr_handler().set("rs232", FUNC(rs232_port_device::write_dtr));
	uart1.rts_handler().set("rs232", FUNC(rs232_port_device::write_rts));

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, "terminal"));
	rs232.rxd_handler().set("uart1", FUNC(i8251_device::write_rxd));
	rs232.dsr_handler().set("uart1", FUNC(i8251_device::write_dsr));
	rs232.cts_handler().set("uart1", FUNC(i8251_device::write_cts));

	// uart2 - cassette - clock comes from 2MHz through a divider consisting of 4 chips and some jumpers.
	I8251(config, "uart2", 0);
}

static DEVICE_INPUT_DEFAULTS_START( terminal )
	DEVICE_INPUT_DEFAULTS( "RS232_RXBAUD", 0xff, RS232_BAUD_4800 )
	DEVICE_INPUT_DEFAULTS( "RS232_TXBAUD", 0xff, RS232_BAUD_4800 )
	DEVICE_INPUT_DEFAULTS( "RS232_STARTBITS", 0xff, RS232_STARTBITS_1 )
	DEVICE_INPUT_DEFAULTS( "RS232_DATABITS", 0xff, RS232_DATABITS_8 )
	DEVICE_INPUT_DEFAULTS( "RS232_PARITY", 0xff, RS232_PARITY_NONE )
	DEVICE_INPUT_DEFAULTS( "RS232_STOPBITS", 0xff, RS232_STOPBITS_2 )
DEVICE_INPUT_DEFAULTS_END

void mfabfz_state::mfabfz85(machine_config &config)
{
	/* basic machine hardware */
	i8085a_cpu_device &maincpu(I8085A(config, m_maincpu, 4_MHz_XTAL / 2));
	maincpu.set_addrmap(AS_PROGRAM, &mfabfz_state::mfabfz_mem);
	maincpu.set_addrmap(AS_IO, &mfabfz_state::mfabfz85_io);
	maincpu.in_sid_func().set("rs232", FUNC(rs232_port_device::rxd_r));
	maincpu.out_sod_func().set("rs232", FUNC(rs232_port_device::write_txd)).invert();

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, "terminal"));
	rs232.set_option_device_input_defaults("terminal", DEVICE_INPUT_DEFAULTS_NAME(terminal));

	I8251(config, "uart2", 0);
}


/* ROM definition */
ROM_START( mfabfz )
	ROM_REGION( 0x8000, "roms", 0 ) // MAT32K, 1986, works
	ROM_LOAD( "mfa_mat32k_vers.1.8-t_ic0.bin", 0x0000, 0x8000, CRC(6cba989e) SHA1(81611b6250a5319e5d28af5ce3a1e261af8315ae) )
ROM_END

ROM_START( mfabfz85 )
	ROM_REGION( 0x8000, "roms", 0 )
	ROM_SYSTEM_BIOS( 0, "32k", "MAT32K v1.8s" ) // 1982, not working
	ROMX_LOAD( "mfa_mat32k_vers.1.8-s_ic0.bin", 0x0000, 0x8000, CRC(021d7dff) SHA1(aa34b3a8bac52fc7746d35f5ffc6328734788cc2), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "8k", "MAT85 8k" ) // 1982, not working
	ROMX_LOAD( "mfa_mat_1_0000.bin", 0x0000, 0x0800, CRC(73b588ea) SHA1(2b9570fe44c3c19d6aa7c7c11ecf390fa5d48998), ROM_BIOS(1) )
	ROMX_LOAD( "mfa_mat_2_0800.bin", 0x0800, 0x0800, CRC(13f5be91) SHA1(2b9d64600679bab319a37381fc84e874c3b2a877), ROM_BIOS(1) )
	ROMX_LOAD( "mfa_mat_3_1000.bin", 0x1000, 0x0800, CRC(c9b91bb4) SHA1(ef829964f507b1f6bbcf3c557c274fe728636efe), ROM_BIOS(1) )
	ROMX_LOAD( "mfa_mat_4_1800.bin", 0x1800, 0x0800, CRC(649cd7f0) SHA1(e92f29c053234b36f22d525fe92e61bf24476f14), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 2, "16k_set1", "MAT85+ 16k set1" )
	ROMX_LOAD( "mfa_mat85_0x0000-0x07ff.bin", 0x0000, 0x0800, CRC(73b588ea) SHA1(2b9570fe44c3c19d6aa7c7c11ecf390fa5d48998), ROM_BIOS(2) )
	ROMX_LOAD( "mfa_mat85_0x0800-0x0fff.bin", 0x0800, 0x0800, CRC(13f5be91) SHA1(2b9d64600679bab319a37381fc84e874c3b2a877), ROM_BIOS(2) )
	ROMX_LOAD( "mfa_mat85_0x1000-0x17ff.bin", 0x1000, 0x0800, CRC(c9b91bb4) SHA1(ef829964f507b1f6bbcf3c557c274fe728636efe), ROM_BIOS(2) )
	ROMX_LOAD( "mfa_mat85_0x1800-0x1fff.bin", 0x1800, 0x0800, CRC(649cd7f0) SHA1(e92f29c053234b36f22d525fe92e61bf24476f14), ROM_BIOS(2) )
	ROMX_LOAD( "mfa_mat85_0x2000-0x27ff.bin", 0x2000, 0x0800, CRC(d3592915) SHA1(68daec6c5c63692bc147b1710b9c45ca780f2c7b), ROM_BIOS(2) )
	ROMX_LOAD( "mfa_mat85_0x2800-0x2fff.bin", 0x2800, 0x0800, CRC(9a6aafa9) SHA1(af897e91cc2ce5d6e49fa88c920ad85e1f0209bf), ROM_BIOS(2) )
	ROMX_LOAD( "mfa_mat85_0x3000-0x37ff.bin", 0x3000, 0x0800, CRC(eae4e3d5) SHA1(f7112965874417bbfc4a32f31f84e1db83249ab7), ROM_BIOS(2) )
	ROMX_LOAD( "mfa_mat85_0x3800-0x3fff.bin", 0x3800, 0x0800, CRC(536db0e3) SHA1(328ccc18455f710390c29c0fd0f4b0713a4a69ae), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 3, "16k_set2", "MAT85+ 16k set2" )
	ROMX_LOAD( "mat85_1_1of8.bin", 0x0000, 0x0800, CRC(73b588ea) SHA1(2b9570fe44c3c19d6aa7c7c11ecf390fa5d48998), ROM_BIOS(3) )
	ROMX_LOAD( "mat85_2_2of8.bin", 0x0800, 0x0800, CRC(c97acc82) SHA1(eedb27c19a2d21b5ec5bca6cafeb25584e21e500), ROM_BIOS(3) )
	ROMX_LOAD( "mat85_3_3of8.bin", 0x1000, 0x0800, CRC(c9b91bb4) SHA1(ef829964f507b1f6bbcf3c557c274fe728636efe), ROM_BIOS(3) )
	ROMX_LOAD( "mat85_4_4of8.bin", 0x1800, 0x0800, CRC(649cd7f0) SHA1(e92f29c053234b36f22d525fe92e61bf24476f14), ROM_BIOS(3) )
	ROMX_LOAD( "soft_1_5of8.bin",  0x2000, 0x0800, CRC(98d9e86e) SHA1(af78b370fe97a6017b192dadec4059256ee4f4c7), ROM_BIOS(3) )
	ROMX_LOAD( "soft_2_6of8.bin",  0x2800, 0x0800, CRC(81fc3b24) SHA1(186dbd389fd700c5af1ef7c37948e11701ec596e), ROM_BIOS(3) )
	ROMX_LOAD( "soft_3_7of8.bin",  0x3000, 0x0800, CRC(eae4e3d5) SHA1(f7112965874417bbfc4a32f31f84e1db83249ab7), ROM_BIOS(3) )
	ROMX_LOAD( "soft_4_8of8.bin",  0x3800, 0x0800, CRC(536db0e3) SHA1(328ccc18455f710390c29c0fd0f4b0713a4a69ae), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS (4, "32k_dtp", "MAT32K dtp" )
	ROMX_LOAD( "mfa_mat85_sp1_ed_kpl_dtp_terminal.bin", 0x0000, 0x8000, CRC(ed432c19) SHA1(31cbc06d276dbb201d50967f4ddba26a42560753), ROM_BIOS(4) )
ROM_END

/*    YEAR  NAME      PARENT  COMPAT  MACHINE   INPUT   CLASS,        INIT        COMPANY                         FULLNAME                               FLAGS */
COMP( 1979, mfabfz,   0,      0,      mfabfz,   mfabfz, mfabfz_state, empty_init, "Berufsfoerdungszentrum Essen", "Mikrocomputer fuer Ausbildung",       MACHINE_NOT_WORKING | MACHINE_NO_SOUND_HW)
COMP( 1979, mfabfz85, mfabfz, 0,      mfabfz85, mfabfz, mfabfz_state, empty_init, "Berufsfoerdungszentrum Essen", "Mikrocomputer fuer Ausbildung MAT85", MACHINE_NOT_WORKING | MACHINE_NO_SOUND_HW)
