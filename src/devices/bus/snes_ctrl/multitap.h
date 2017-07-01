// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Nintendo Super Famicom & SNES Multitap Adapter

**********************************************************************/

#pragma once

#ifndef __SNES_MULTITAP__
#define __SNES_MULTITAP__


#include "emu.h"
#include "ctrl.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> snes_multitap_device

class snes_multitap_device : public device_t,
							public device_snes_control_port_interface
{
public:
	// construction/destruction
	snes_multitap_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;
	virtual machine_config_constructor device_mconfig_additions() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_snes_control_port_interface overrides
	virtual uint8_t read_pin4() override;
	virtual uint8_t read_pin5() override;
	virtual void write_strobe(uint8_t data) override;
	virtual void write_pin6(uint8_t data) override;
	virtual void port_poll() override;

private:
	required_device<snes_control_port_device> m_port1;
	required_device<snes_control_port_device> m_port2;
	required_device<snes_control_port_device> m_port3;
	required_device<snes_control_port_device> m_port4;
	required_ioport m_cfg;
	int m_select;
};


// device type definition
extern const device_type SNES_MULTITAP;


#endif
