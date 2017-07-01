// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Sega Master System "Gender Adapter" emulation

The Gender Adapter is not an official Sega product. It is produced since 2006
by the SMSPower website to permit to plug a cartridge on the expansion slot
on any SMS 1 model. This includes the Japanese SMS, which has FM sound, so
it is a way to get FM music of western cartridges that have FM code but were
not released in Japan. Some games have compatibility issues, confirmed on the
real hardware, when run plugged-in to the SMS expansion slot.

**********************************************************************/

#include "gender.h"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type SMS_GENDER_ADAPTER = &device_creator<sms_gender_adapter_device>;



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  sms_gender_adapter_device - constructor
//-------------------------------------------------

sms_gender_adapter_device::sms_gender_adapter_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, SMS_GENDER_ADAPTER, "Gender Adapter", tag, owner, clock, "sms_gender_adapter", __FILE__),
	device_sms_expansion_slot_interface(mconfig, *this),
	m_subslot(*this, "subslot")
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void sms_gender_adapter_device::device_start()
{
	if (m_subslot->m_cart)
		m_subslot->m_cart->save_ram();
}


//-------------------------------------------------
// read
//-------------------------------------------------

READ8_MEMBER(sms_gender_adapter_device::read)
{
	return m_subslot->read_cart(space, offset);
}

READ8_MEMBER(sms_gender_adapter_device::read_ram)
{
	return m_subslot->read_ram(space, offset);
}

int sms_gender_adapter_device::get_lphaser_xoffs()
{
	if (m_subslot->m_cart)
		return m_subslot->m_cart->get_lphaser_xoffs();
	else
		return 0;
}


//-------------------------------------------------
// write
//-------------------------------------------------

WRITE8_MEMBER(sms_gender_adapter_device::write_mapper)
{
	m_subslot->write_mapper(space, offset, data);
}

WRITE8_MEMBER(sms_gender_adapter_device::write)
{
	m_subslot->write_cart(space, offset, data);
}

WRITE8_MEMBER(sms_gender_adapter_device::write_ram)
{
	m_subslot->write_ram(space, offset, data);
}

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

static MACHINE_CONFIG_FRAGMENT( genderadp_slot )
	MCFG_SMS_CARTRIDGE_ADD("subslot", sms_cart, nullptr)
MACHINE_CONFIG_END


machine_config_constructor sms_gender_adapter_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( genderadp_slot );
}
