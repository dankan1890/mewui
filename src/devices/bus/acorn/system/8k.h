// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    Acorn 8K Static Memory Board

    http://chrisacorns.computinghistory.org.uk/8bit_Upgrades/Acorn_Memory.html

**********************************************************************/


#ifndef MAME_BUS_ACORN_SYSTEM_8K_H
#define MAME_BUS_ACORN_SYSTEM_8K_H

#pragma once

#include "bus/acorn/bus.h"
#include "bus/generic/slot.h"
#include "bus/generic/carts.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class acorn_8k_device :
	public device_t,
	public device_acorn_bus_interface
{
public:
	// construction/destruction
	acorn_8k_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

private:
	image_init_result load_rom(device_image_interface &image, generic_slot_device *slot);
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(rom0_load) { return load_rom(image, m_rom[0]); }
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(rom1_load) { return load_rom(image, m_rom[1]); }

	required_device_array<generic_slot_device, 2> m_rom;
	required_ioport m_links;
};


// device type definition
DECLARE_DEVICE_TYPE(ACORN_8K, acorn_8k_device)


#endif // MAME_BUS_ACORN_SYSTEM_8K_H
