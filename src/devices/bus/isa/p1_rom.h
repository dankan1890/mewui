// license:BSD-3-Clause
// copyright-holders:Sergey Svishchev
/**********************************************************************

    Poisk-1 ROM cartridge device

**********************************************************************/

#pragma once

#ifndef __P1_ROM__
#define __P1_ROM__

#include "emu.h"
#include "isa.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class p1_rom_device : public device_t,
	public device_isa8_card_interface
{
public:
	// construction/destruction
	p1_rom_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
};


// device type definition
extern const device_type P1_ROM;


#endif
