// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    Acorn Econet Interface

    http://chrisacorns.computinghistory.org.uk/8bit_Upgrades/Acorn_Econet.html

**********************************************************************/


#ifndef MAME_BUS_ACORN_SYSTEM_ECONET_H
#define MAME_BUS_ACORN_SYSTEM_ECONET_H

#pragma once

#include "bus/acorn/bus.h"
#include "bus/econet/econet.h"
#include "machine/mc6854.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class acorn_econet_device :
	public device_t,
	public device_acorn_bus_interface
{
public:
	// construction/destruction
	acorn_econet_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

private:
	DECLARE_READ8_MEMBER(statid_r);
	DECLARE_WRITE_LINE_MEMBER(bus_irq_w);

	required_device<mc6854_device> m_adlc;
	required_device<econet_device> m_econet;
};


// device type definition
DECLARE_DEVICE_TYPE(ACORN_ECONET, acorn_econet_device)


#endif // MAME_BUS_ACORN_SYSTEM_ECONET_H
