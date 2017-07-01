// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Commodore VIC-1010 Expansion Module emulation

**********************************************************************/

#pragma once

#ifndef __VIC1010__
#define __VIC1010__

#include "emu.h"
#include "exp.h"



//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

#define MAX_SLOTS 6



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> vic1010_device

class vic1010_device :  public device_t,
						public device_vic20_expansion_card_interface
{
public:
	// construction/destruction
	vic1010_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_vic20_expansion_card_interface overrides
	virtual uint8_t vic20_cd_r(address_space &space, offs_t offset, uint8_t data, int ram1, int ram2, int ram3, int blk1, int blk2, int blk3, int blk5, int io2, int io3) override;
	virtual void vic20_cd_w(address_space &space, offs_t offset, uint8_t data, int ram1, int ram2, int ram3, int blk1, int blk2, int blk3, int blk5, int io2, int io3) override;

private:
	required_device<vic20_expansion_slot_device> m_slot1;
	required_device<vic20_expansion_slot_device> m_slot2;
	required_device<vic20_expansion_slot_device> m_slot3;
	required_device<vic20_expansion_slot_device> m_slot4;
	required_device<vic20_expansion_slot_device> m_slot5;
	required_device<vic20_expansion_slot_device> m_slot6;

	vic20_expansion_slot_device *m_expansion_slot[MAX_SLOTS];
};


// device type definition
extern const device_type VIC1010;



#endif
