// license:BSD-3-Clause
// copyright-holders:smf
/**********************************************************************

    Starbyte Software Tie Break Adaptor emulation

**********************************************************************/

#pragma once

#ifndef __C64_4TBA__
#define __C64_4TBA__


#include "emu.h"
#include "user.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_4tba_device

class c64_4tba_device : public device_t,
	public device_pet_user_port_interface
{
public:
	// construction/destruction
	c64_4tba_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;

	// device_pet_user_port_interface overrides
	virtual WRITE_LINE_MEMBER( input_4 ) override { output_6(state); }
	virtual WRITE_LINE_MEMBER( input_6 ) override { output_4(state); }

protected:
	// device-level overrides
	virtual void device_start() override;
};


// device type definition
extern const device_type C64_4TBA;


#endif
