// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Cinemaware Warp Speed cartridge emulation

**********************************************************************/

#pragma once

#ifndef __WARP_SPEED__
#define __WARP_SPEED__


#include "emu.h"
#include "exp.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_warp_speed_cartridge_device

class c64_warp_speed_cartridge_device : public device_t,
										public device_c64_expansion_card_interface
{
public:
	// construction/destruction
	c64_warp_speed_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_c64_expansion_card_interface overrides
	virtual uint8_t c64_cd_r(address_space &space, offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;
	virtual void c64_cd_w(address_space &space, offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;
};


// device type definition
extern const device_type C64_WARP_SPEED;


#endif
