// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/***************************************************************************

    BBC Micro Sideways RAM emulation

***************************************************************************/

#ifndef MAME_BUS_BBC_ROM_RAM_H
#define MAME_BUS_BBC_ROM_RAM_H

#pragma once

#include "slot.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> bbc_ram_device

class bbc_ram_device : public device_t,
							public device_bbc_rom_interface
{
public:
	// construction/destruction
	bbc_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// device_bbc_rom_interface overrides
	virtual uint8_t read(offs_t offset) override;
	virtual void write(offs_t offset, uint8_t data) override;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_RAM, bbc_ram_device)

#endif // MAME_BUS_BBC_ROM_RAM_H
