// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Nintendo Super Famicom - Sunsoft Pachinko Controller

**********************************************************************/

#pragma once

#ifndef __SNES_PACHINKO__
#define __SNES_PACHINKO__


#include "emu.h"
#include "ctrl.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> snes_pachinko_device

class snes_pachinko_device : public device_t,
							public device_snes_control_port_interface
{
public:
	// construction/destruction
	snes_pachinko_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_snes_control_port_interface overrides
	virtual uint8_t read_pin4() override;
	virtual void write_strobe(uint8_t data) override;
	virtual void port_poll() override;

private:
	required_ioport m_dial;
	required_ioport m_button;
	int m_strobe;
	uint32_t m_latch;
};


// device type definition
extern const device_type SNES_PACHINKO;


#endif
