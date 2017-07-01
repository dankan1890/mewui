// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Furrtek's homemade multitap emulation

**********************************************************************/

#pragma once

#ifndef __SMS_MULTITAP__
#define __SMS_MULTITAP__


#include "emu.h"
#include "smsctrl.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sms_multitap_device

class sms_multitap_device : public device_t,
							public device_sms_control_port_interface
{
public:
	// construction/destruction
	sms_multitap_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_READ32_MEMBER(pixel_r);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual machine_config_constructor device_mconfig_additions() const override;

	// device_sms_control_port_interface overrides
	virtual uint8_t peripheral_r() override;
	virtual void peripheral_w(uint8_t data) override;

private:
	required_device<sms_control_port_device> m_subctrl1_port;
	required_device<sms_control_port_device> m_subctrl2_port;
	required_device<sms_control_port_device> m_subctrl3_port;
	required_device<sms_control_port_device> m_subctrl4_port;

	uint8_t m_read_state;
	uint8_t m_last_data;
};


// device type definition
extern const device_type SMS_MULTITAP;


#endif
