// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    COMX-35 F&M Joycard emulation

**********************************************************************/

#pragma once

#ifndef __COMX_JOY__
#define __COMX_JOY__

#include "emu.h"
#include "exp.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> comx_joy_device

class comx_joy_device : public device_t,
						public device_comx_expansion_card_interface
{
public:
	// construction/destruction
	comx_joy_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_comx_expansion_card_interface overrides
	virtual uint8_t comx_io_r(address_space &space, offs_t offset) override;

private:
	required_ioport m_joy1;
	required_ioport m_joy2;
};


// device type definition
extern const device_type COMX_JOY;


#endif
