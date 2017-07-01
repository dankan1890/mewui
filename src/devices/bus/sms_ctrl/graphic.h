// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Sega Master System "Graphic Board" emulation

**********************************************************************/

#pragma once

#ifndef __SMS_GRAPHIC__
#define __SMS_GRAPHIC__


#include "emu.h"
#include "smsctrl.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sms_graphic_device

class sms_graphic_device : public device_t,
							public device_sms_control_port_interface
{
public:
	// construction/destruction
	sms_graphic_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;

protected:
	// device-level overrides
	virtual void device_start() override;

	// device_sms_control_port_interface overrides
	virtual uint8_t peripheral_r() override;
	virtual void peripheral_w(uint8_t data) override;

private:
	required_ioport m_buttons;
	required_ioport m_x;
	required_ioport m_y;

	int m_index;
	uint8_t m_previous_write;
	uint8_t m_pressure;
};


// device type definition
extern const device_type SMS_GRAPHIC;


#endif
