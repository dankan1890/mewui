// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
/**********************************************************************

    ColecoVision X-in-1 cartridge emulation

**********************************************************************/

#pragma once

#ifndef __COLECOVISION_XIN1_CARTRIDGE__
#define __COLECOVISION_XIN1_CARTRIDGE__

#include "exp.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> colecovision_xin1_cartridge_device

class colecovision_xin1_cartridge_device : public device_t,
												public device_colecovision_cartridge_interface
{
public:
	// construction/destruction
	colecovision_xin1_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_colecovision_expansion_card_interface overrides
	virtual uint8_t bd_r(address_space &space, offs_t offset, uint8_t data, int _8000, int _a000, int _c000, int _e000) override;

private:
	uint32_t m_current_offset;
};


// device type definition
extern const device_type COLECOVISION_XIN1;


#endif
