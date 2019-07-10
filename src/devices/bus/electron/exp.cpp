// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

        Electron Expansion Port emulation

**********************************************************************/

#include "emu.h"
#include "exp.h"


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(ELECTRON_EXPANSION_SLOT, electron_expansion_slot_device, "electron_expansion_slot", "Acorn Electron Expansion port")


//**************************************************************************
//  DEVICE ELECTRON_EXPANSION CARD INTERFACE
//**************************************************************************

//-------------------------------------------------
//  device_electron_expansion_interface - constructor
//-------------------------------------------------

device_electron_expansion_interface::device_electron_expansion_interface(const machine_config &mconfig, device_t &device)
	: device_slot_card_interface(mconfig, device)
{
	m_slot = dynamic_cast<electron_expansion_slot_device *>(device.owner());
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  electron_expansion_slot_device - constructor
//-------------------------------------------------

electron_expansion_slot_device::electron_expansion_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, ELECTRON_EXPANSION_SLOT, tag, owner, clock),
	device_slot_interface(mconfig, *this),
	m_card(nullptr),
	m_irq_handler(*this),
	m_nmi_handler(*this)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void electron_expansion_slot_device::device_start()
{
	m_card = dynamic_cast<device_electron_expansion_interface *>(get_card_device());

	// resolve callbacks
	m_irq_handler.resolve_safe();
	m_nmi_handler.resolve_safe();
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void electron_expansion_slot_device::device_reset()
{
}

//-------------------------------------------------
//  expbus_r - expansion data read
//-------------------------------------------------

uint8_t electron_expansion_slot_device::expbus_r(offs_t offset)
{
	uint8_t data = 0xff;

	if (m_card != nullptr)
	{
		data = m_card->expbus_r(offset);
	}

	return data;
}

//-------------------------------------------------
//  expbus_w - expansion data write
//-------------------------------------------------

void electron_expansion_slot_device::expbus_w(offs_t offset, uint8_t data)
{
	if (m_card != nullptr)
	{
		m_card->expbus_w(offset, data);
	}
}


//-------------------------------------------------
//  SLOT_INTERFACE( electron_expansion_devices )
//-------------------------------------------------


// slot devices
#include "fbjoy.h"
//#include "fbprint.h"
//#include "jafamode7.h"
#include "plus1.h"
#include "plus2.h"
#include "plus3.h"
#include "pwrjoy.h"
#include "rombox.h"
#include "romboxp.h"
#include "m2105.h"
//#include "voxbox.h"


void electron_expansion_devices(device_slot_interface &device)
{
	device.option_add("fbjoy", ELECTRON_FBJOY);
	//device.option_add("fbprint", ELECTRON_FBPRINT);
	//device.option_add("jafamode7", ELECTRON_JAFAMODE7);
	device.option_add("plus1", ELECTRON_PLUS1);
	device.option_add("plus2", ELECTRON_PLUS2);
	device.option_add("plus3", ELECTRON_PLUS3);
	device.option_add("pwrjoy", ELECTRON_PWRJOY);
	device.option_add("rombox", ELECTRON_ROMBOX);
	device.option_add("romboxp", ELECTRON_ROMBOXP);
	device.option_add("m2105", ELECTRON_M2105);
	//device.option_add("voxbox", ELECTRON_VOXBOX);
}
