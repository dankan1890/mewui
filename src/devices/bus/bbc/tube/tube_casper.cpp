// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    Casper 68000 2nd Processor

**********************************************************************/


#include "emu.h"
#include "tube_casper.h"
#include "softlist_dev.h"


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(BBC_TUBE_CASPER, bbc_tube_casper_device, "bbc_tube_casper", "Casper 68000 2nd Processor")


//-------------------------------------------------
//  ADDRESS_MAP( tube_casper_mem )
//-------------------------------------------------

void bbc_tube_casper_device::tube_casper_mem(address_map &map)
{
	map(0x00000, 0x03fff).rom().region("casper_rom", 0);
	map(0x10000, 0x1001f).m("via6522_1", FUNC(via6522_device::map)).umask16(0x00ff);
	map(0x20000, 0x3ffff).ram();
}

//-------------------------------------------------
//  ROM( tube_casper )
//-------------------------------------------------

ROM_START( tube_casper )
	ROM_REGION(0x4000, "casper_rom", 0)
	ROM_LOAD16_BYTE("casper.ic9",  0x0000, 0x2000, CRC(4105cbf4) SHA1(a3efeb6fb144da55b47c718239967ed0af4fff72))
	ROM_LOAD16_BYTE("casper.ic10", 0x0001, 0x2000, CRC(f25bc320) SHA1(297db56283bb3164c31c21331837213cea426837))

	ROM_REGION(0x8000, "exp_rom", 0)
	ROM_LOAD("rom1.rom", 0x0000, 0x4000, CRC(602b6a36) SHA1(7b24746dbcacb8772468532e92832d5c7f6648fd))
	ROM_LOAD("rom2.rom", 0x4000, 0x4000, CRC(7c9efb43) SHA1(4195ce1ed928178fd645a267872a5b4f325d078a))
ROM_END

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void bbc_tube_casper_device::device_add_mconfig(machine_config &config)
{
	M68000(config, m_m68000, 4_MHz_XTAL);
	m_m68000->set_addrmap(AS_PROGRAM, &bbc_tube_casper_device::tube_casper_mem);

	VIA6522(config, m_via6522_0, 4_MHz_XTAL / 2);
	m_via6522_0->writepb_handler().set(m_via6522_1, FUNC(via6522_device::write_pa));
	m_via6522_0->ca2_handler().set(m_via6522_1, FUNC(via6522_device::write_cb1));
	m_via6522_0->cb2_handler().set(m_via6522_1, FUNC(via6522_device::write_ca1));
	m_via6522_0->irq_handler().set(DEVICE_SELF_OWNER, FUNC(bbc_tube_slot_device::irq_w));

	VIA6522(config, m_via6522_1, 4_MHz_XTAL / 2);
	m_via6522_1->writepb_handler().set(m_via6522_0, FUNC(via6522_device::write_pa));
	m_via6522_1->ca2_handler().set(m_via6522_0, FUNC(via6522_device::write_cb1));
	m_via6522_1->cb2_handler().set(m_via6522_0, FUNC(via6522_device::write_ca1));
	m_via6522_1->irq_handler().set_inputline(m_m68000, M68K_IRQ_1);

	/* software lists */
	SOFTWARE_LIST(config, "flop_ls_casper").set_original("bbc_flop_68000");
}

//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *bbc_tube_casper_device::device_rom_region() const
{
	return ROM_NAME( tube_casper );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  bbc_tube_casper_device - constructor
//-------------------------------------------------

bbc_tube_casper_device::bbc_tube_casper_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, BBC_TUBE_CASPER, tag, owner, clock),
		device_bbc_tube_interface(mconfig, *this),
		m_m68000(*this, "m68000"),
		m_via6522_0(*this, "via6522_0"),
		m_via6522_1(*this, "via6522_1"),
		m_casper_rom(*this, "casper_rom")
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void bbc_tube_casper_device::device_start()
{
}


//**************************************************************************
//  IMPLEMENTATION
//**************************************************************************

uint8_t bbc_tube_casper_device::host_r(offs_t offset)
{
	return m_via6522_0->read(offset & 0xf);
}

void bbc_tube_casper_device::host_w(offs_t offset, uint8_t data)
{
	m_via6522_0->write(offset & 0xf, data);
}
