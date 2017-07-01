// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Sega Mark III "Joypad" / Master System "Control Pad" emulation

**********************************************************************/

#pragma once

#ifndef __SMS_JOYPAD__
#define __SMS_JOYPAD__


#include "emu.h"
#include "smsctrl.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sms_joypad_device

class sms_joypad_device : public device_t,
							public device_sms_control_port_interface
{
public:
	// construction/destruction
	sms_joypad_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;

protected:
	// device-level overrides
	virtual void device_start() override;

	// device_sms_control_port_interface overrides
	virtual uint8_t peripheral_r() override;

private:
	required_ioport m_joypad;
};


// device type definition
extern const device_type SMS_JOYPAD;


#endif
