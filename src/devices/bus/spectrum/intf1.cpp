// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    ZX Interface 1

    The ZX Interface 1 combines the three functions of microdrive
    controller, local area network and RS232 interface.

**********************************************************************/


#include "emu.h"
#include "intf1.h"


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(SPECTRUM_INTF1, spectrum_intf1_device, "spectrum_intf1", "ZX Interface 1")


//-------------------------------------------------
//  MACHINE_DRIVER( intf1 )
//-------------------------------------------------

ROM_START( intf1 )
	ROM_REGION( 0x2000, "rom", 0 )
	ROM_DEFAULT_BIOS("v2")
	ROM_SYSTEM_BIOS(0, "v1", "v1")
	ROMX_LOAD("if1-1.rom", 0x0000, 0x2000, CRC(e72a12ae) SHA1(4ffd9ed9c00cdc6f92ce69fdd8b618ef1203f48e), ROM_BIOS(0))

	ROM_SYSTEM_BIOS(1, "v2", "v2")
	ROMX_LOAD("if1-2.rom", 0x0000, 0x2000, CRC(bb66dd1e) SHA1(5cfb6bca4177c45fefd571734576b55e3a127c08), ROM_BIOS(1))
ROM_END

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void spectrum_intf1_device::device_add_mconfig(machine_config &config)
{
	/* rs232 */
	RS232_PORT(config, m_rs232, default_rs232_devices, nullptr);

	/* microdrive */
	MICRODRIVE(config, m_mdv1);
	m_mdv1->comms_out_wr_callback().set(m_mdv2, FUNC(microdrive_image_device::comms_in_w));
	MICRODRIVE(config, m_mdv2);

	/* passthru */
	SPECTRUM_EXPANSION_SLOT(config, m_exp, spectrum_expansion_devices, nullptr);
	m_exp->irq_handler().set(DEVICE_SELF_OWNER, FUNC(spectrum_expansion_slot_device::irq_w));
	m_exp->nmi_handler().set(DEVICE_SELF_OWNER, FUNC(spectrum_expansion_slot_device::nmi_w));

	SOFTWARE_LIST(config, "microdrive_list").set_original("spectrum_microdrive");
}

const tiny_rom_entry *spectrum_intf1_device::device_rom_region() const
{
	return ROM_NAME( intf1 );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  spectrum_intf1_device - constructor
//-------------------------------------------------

spectrum_intf1_device::spectrum_intf1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, SPECTRUM_INTF1, tag, owner, clock)
	, device_spectrum_expansion_interface(mconfig, *this)
	, m_exp(*this, "exp")
	, m_rs232(*this, "rs232")
	, m_mdv1(*this, "mdv1")
	, m_mdv2(*this, "mdv2")
	, m_rom(*this, "rom")
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void spectrum_intf1_device::device_start()
{
	save_item(NAME(m_romcs));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void spectrum_intf1_device::device_reset()
{
	m_romcs = 0;
}

//**************************************************************************
//  IMPLEMENTATION
//**************************************************************************

READ_LINE_MEMBER(spectrum_intf1_device::romcs)
{
	return m_romcs | m_exp->romcs();
}

// the Interface 1 looks for specific bus conditions to enable / disable the expansion overlay ROM

// the enable must occur BEFORE the opcode is fetched, as the opcode must be fetched from the expansion ROM
void spectrum_intf1_device::opcode_fetch(offs_t offset)
{
	m_exp->opcode_fetch(offset);

	if (!machine().side_effects_disabled())
	{
		switch (offset)
		{
		case 0x0008: case 0x1708:
			m_romcs = 1;
			break;
		}
	}
}

// the disable must occur AFTER the opcode fetch, or the incorrect opcode is fetched for 0x0700
void spectrum_intf1_device::opcode_fetch_post(offs_t offset)
{
	m_exp->opcode_fetch_post(offset);

	if (!machine().side_effects_disabled())
	{
		switch (offset)
		{
		case 0x0700:
			m_romcs = 0;
			break;
		}
	}
}


uint8_t spectrum_intf1_device::mreq_r(offs_t offset)
{
	uint8_t data = 0xff;

	if (m_romcs)
		data &= m_rom->base()[offset & 0x1fff];

	if (m_exp->romcs())
		data &= m_exp->mreq_r(offset);

	return data;
}

void spectrum_intf1_device::mreq_w(offs_t offset, uint8_t data)
{
	if (m_exp->romcs())
		m_exp->mreq_w(offset, data);
}

uint8_t spectrum_intf1_device::iorq_r(offs_t offset)
{
	uint8_t data = 0xff;

	if ((offset & 0x18) != 0x18)
	{
		switch (offset & 0x18)
		{
		case 0x00:
			logerror("%s: iorq_r (microdrive) %04x\n", machine().describe_context(), offset);
			break;
		case 0x08:
			logerror("%s: iorq_r (control) %04x\n", machine().describe_context(), offset);
			break;
		case 0x10:
			logerror("%s: iorq_r (network) %04x\n", machine().describe_context(), offset);
			break;
		}
	}

	data &= m_exp->iorq_r(offset);
	return data;
}

void spectrum_intf1_device::iorq_w(offs_t offset, uint8_t data)
{
	if ((offset & 0x18) != 0x18)
	{
		switch (offset & 0x18)
		{
		case 0x00:
			logerror("%s: iorq_w (microdrive) %04x: %02x\n", machine().describe_context(), offset, data);
			break;
		case 0x08:
			logerror("%s: iorq_w (control) %04x: %02x\n", machine().describe_context(), offset, data);
			break;
		case 0x10:
			logerror("%s: iorq_w (network) %04x: %02x\n", machine().describe_context(), offset, data);
			break;
		}
	}

	m_exp->iorq_w(offset, data);
}
