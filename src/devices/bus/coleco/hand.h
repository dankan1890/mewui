// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    ColecoVision Hand Controller emulation

**********************************************************************/

#pragma once

#ifndef __COLECO_HAND_CONTROLLER__
#define __COLECO_HAND_CONTROLLER__

#include "emu.h"
#include "ctrl.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> coleco_hand_controller_t

class coleco_hand_controller_t : public device_t,
									public device_colecovision_control_port_interface
{
public:
	// construction/destruction
	coleco_hand_controller_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;

	DECLARE_CUSTOM_INPUT_MEMBER( keypad_r );

protected:
	// device-level overrides
	virtual void device_start() override;

	// device_vcs_control_port_interface overrides
	virtual uint8_t joy_r() override;

private:
	required_ioport m_io_common0;
	required_ioport m_io_common1;
	required_ioport m_io_keypad;
};


// device type definition
extern const device_type COLECO_HAND_CONTROLLER;


#endif
