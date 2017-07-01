// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Miracle Hard Disk emulation

**********************************************************************/

#pragma once

#ifndef __MIRACLE_HARD_DISK__
#define __MIRACLE_HARD_DISK__

#include "rom.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> miracle_hard_disk_t

class miracle_hard_disk_t : public device_t,
							public device_ql_rom_cartridge_card_interface
{
public:
	// construction/destruction
	miracle_hard_disk_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;

protected:
	// device-level overrides
	virtual void device_start() override;

	// device_ql_rom_cartridge_card_interface overrides
	virtual uint8_t read(address_space &space, offs_t offset, uint8_t data) override;
	virtual void write(address_space &space, offs_t offset, uint8_t data) override;
};



// device type definition
extern const device_type MIRACLE_HARD_DISK;



#endif
