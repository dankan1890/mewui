// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/*********************************************************************

    Romantic Robot Multiface One/128/3

*********************************************************************/

#include "emu.h"
#include "mface.h"


/***************************************************************************
    DEVICE DEFINITIONS
***************************************************************************/

DEFINE_DEVICE_TYPE(SPECTRUM_MFACE1, spectrum_mface1_device, "spectrum_mface1", "Multiface One")
DEFINE_DEVICE_TYPE(SPECTRUM_MFACE128, spectrum_mface128_device, "spectrum_mface128", "Multiface 128")
DEFINE_DEVICE_TYPE(SPECTRUM_MFACE3, spectrum_mface3_device, "spectrum_mface3", "Multiface 3")
DEFINE_DEVICE_TYPE(SPECTRUM_MPRINT, spectrum_mprint_device, "spectrum_mprint", "MultiPrint")


//-------------------------------------------------
//  INPUT_PORTS( mface )
//-------------------------------------------------

INPUT_PORTS_START(mface)
	PORT_START("BUTTON")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_BUTTON1) PORT_NAME("Multiface") PORT_CODE(KEYCODE_F12) PORT_CHANGED_MEMBER(DEVICE_SELF, spectrum_mface1_device, magic_button, 0)
INPUT_PORTS_END

//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor spectrum_mface1_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(mface);
}

//-------------------------------------------------
//  ROM( mface1 )
//-------------------------------------------------

ROM_START(mface1)
	ROM_REGION(0x2000, "rom", 0)
	ROM_DEFAULT_BIOS("mu21e7")
	ROM_SYSTEM_BIOS(0, "mu20fe", "MU 2.0 FE")
	ROMX_LOAD("mf1_20_fe.rom", 0x0000, 0x2000, CRC(fa1b8b0d) SHA1(20cd508b0143166558a7238c7a9ccfbe37b90b0d), ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "mu2167", "MU 2.1 67")
	ROMX_LOAD("mf1_21_67.rom", 0x0000, 0x2000, CRC(d720ec1b) SHA1(91a40d8f503ef825df3e2ed712897dbf4ca3671d), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "mu21e4", "MU 2.1 E4")
	ROMX_LOAD("mf1_21_e4.rom", 0x0000, 0x2000, CRC(4b31a971) SHA1(ba28754a3cc31a4ca579829ed4310c313409cf5d), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(3, "mu21e7", "MU 2.1 E7")
	ROMX_LOAD("mf1_21_e7.rom", 0x0000, 0x2000, CRC(670f0ec2) SHA1(50fba2d628f3a2e9219f72980e4efd62fc9ec1f8), ROM_BIOS(3))
ROM_END

ROM_START(mface128)
	ROM_REGION(0x2000, "rom", 0)
	ROM_DEFAULT_BIOS("v363c")
	ROM_SYSTEM_BIOS(0, "v363c", "87.2 V36 3C")
	ROMX_LOAD("mf128_36_3c.rom", 0x0000, 0x2000, CRC(78ec8cfd) SHA1(8df204ab490b87c389971ce0c7fb5f9cbd281f14), ROM_BIOS(0))
ROM_END

ROM_START(mface3)
	ROM_REGION(0x2000, "rom", 0)
	ROM_DEFAULT_BIOS("v50fe")
	ROM_SYSTEM_BIOS(0, "v5013", "V50 13")
	ROMX_LOAD("mf3_50_13.rom", 0x0000, 0x2000, CRC(2d594640) SHA1(5d74d2e2e5a537639da92ff120f8a6d86f474495), ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "v50fe", "V50 FE")
	ROMX_LOAD("mf3_50_fe.rom", 0x0000, 0x2000, CRC(b5c00f28) SHA1(983699a07665186f498f5827f9b35c442c2178ba), ROM_BIOS(1))
ROM_END

ROM_START(mprint)
	ROM_REGION(0x2000, "rom", 0)
	ROM_DEFAULT_BIOS("mpa8")
	ROM_SYSTEM_BIOS(0, "mp5a", "MP 5A")
	ROMX_LOAD("mprint_5a.rom", 0x0000, 0x2000, CRC(3a26e84b) SHA1(4714469bf25f69291f61188f52bfb11fbb8d0b33), ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "mpa8", "MP A8")
	ROMX_LOAD("mprint_a8.rom", 0x0000, 0x2000, CRC(a5c58022) SHA1(1356bfae3264b952f83a33e25af536c0f13f50e7), ROM_BIOS(1))
ROM_END

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void spectrum_mface1_device::device_add_mconfig(machine_config &config)
{
	/* passthru */
	SPECTRUM_EXPANSION_SLOT(config, m_exp, spectrum_expansion_devices, nullptr);
	m_exp->irq_handler().set(DEVICE_SELF_OWNER, FUNC(spectrum_expansion_slot_device::irq_w));
	m_exp->nmi_handler().set(DEVICE_SELF_OWNER, FUNC(spectrum_expansion_slot_device::nmi_w));
}

const tiny_rom_entry *spectrum_mface1_device::device_rom_region() const
{
	return ROM_NAME(mface1);
}

const tiny_rom_entry *spectrum_mface128_device::device_rom_region() const
{
	return ROM_NAME(mface128);
}

const tiny_rom_entry *spectrum_mface3_device::device_rom_region() const
{
	return ROM_NAME(mface3);
}

const tiny_rom_entry *spectrum_mprint_device::device_rom_region() const
{
	return ROM_NAME(mprint);
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  spectrum_opus_device - constructor
//-------------------------------------------------

spectrum_mface1_device::spectrum_mface1_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, type, tag, owner, clock)
	, device_spectrum_expansion_interface(mconfig, *this)
	, m_rom(*this, "rom")
	, m_exp(*this, "exp")
{
}

spectrum_mface1_device::spectrum_mface1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: spectrum_mface1_device(mconfig, SPECTRUM_MFACE1, tag, owner, clock)
{
}

spectrum_mface128_device::spectrum_mface128_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: spectrum_mface1_device(mconfig, SPECTRUM_MFACE128, tag, owner, clock)
{
}

spectrum_mface3_device::spectrum_mface3_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: spectrum_mface1_device(mconfig, SPECTRUM_MFACE3, tag, owner, clock)
{
}

spectrum_mprint_device::spectrum_mprint_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: spectrum_mface1_device(mconfig, SPECTRUM_MPRINT, tag, owner, clock)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void spectrum_mface1_device::device_start()
{
	save_item(NAME(m_romcs));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void spectrum_mface1_device::device_reset()
{
	m_romcs = 0;
}


//**************************************************************************
//  IMPLEMENTATION
//**************************************************************************

READ_LINE_MEMBER(spectrum_mface1_device::romcs)
{
	return m_romcs | m_exp->romcs();
}

void spectrum_mface1_device::opcode_fetch(offs_t offset)
{
	m_exp->opcode_fetch(offset);

	if (!machine().side_effects_disabled())
	{
		if (offset == 0x0066)
			m_romcs = 1;
	}
}

uint8_t spectrum_mface1_device::iorq_r(offs_t offset)
{
	uint8_t data = m_exp->iorq_r(offset);

	if (!machine().side_effects_disabled())
	{
		switch (offset & 0xff)
		{
		case 0x1f:
			m_romcs = 0;
			break;
		case 0x9f:
			m_romcs = 1;
			break;
		}
	}
	return data;
}

uint8_t spectrum_mface128_device::iorq_r(offs_t offset)
{
	uint8_t data = m_exp->iorq_r(offset);

	if (!machine().side_effects_disabled())
	{
		switch (offset & 0xff)
		{
		case 0xbf:
			m_romcs = 1;
			break;
		case 0x3f:
			m_romcs = 0;
			break;
		}
	}
	return data;
}

uint8_t spectrum_mface3_device::iorq_r(offs_t offset)
{
	uint8_t data = m_exp->iorq_r(offset);

	if (!machine().side_effects_disabled())
	{
		switch (offset & 0xff)
		{
		case 0x3f:
			m_romcs = 1;
			break;
		case 0xbf:
			m_romcs = 0;
			break;
		}
	}
	return data;
}

uint8_t spectrum_mprint_device::iorq_r(offs_t offset)
{
	uint8_t data = m_exp->iorq_r(offset);

	if (!machine().side_effects_disabled())
	{
		switch (offset & 0xff)
		{
		case 0xbb:
			m_romcs = 1;
			break;
		case 0xbf:
			m_romcs = 0;
			break;
		}
	}
	return data;
}

void spectrum_mface1_device::iorq_w(offs_t offset, uint8_t data)
{
	m_exp->iorq_w(offset, data);
}

uint8_t spectrum_mface1_device::mreq_r(offs_t offset)
{
	uint8_t data = 0xff;

	if (m_romcs)
	{
		switch (offset & 0xe000)
		{
		case 0x0000:
			data = m_rom->base()[offset & 0x1fff];
			break;
		case 0x2000:
			data = m_ram[offset & 0x1fff];
			break;
		}
	}

	if (m_exp->romcs())
		data &= m_exp->mreq_r(offset);

	return data;
}

void spectrum_mface1_device::mreq_w(offs_t offset, uint8_t data)
{
	if (m_romcs)
	{
		switch (offset & 0xe000)
		{
		case 0x2000:
			m_ram[offset & 0x1fff] = data;
			break;
		}
	}

	if (m_exp->romcs())
		m_exp->mreq_w(offset, data);
}

INPUT_CHANGED_MEMBER(spectrum_mface1_device::magic_button)
{
	if (newval && !oldval)
	{
		m_slot->nmi_w(ASSERT_LINE);
	}
	else
	{
		m_slot->nmi_w(CLEAR_LINE);
	}
}
