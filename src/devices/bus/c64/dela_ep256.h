// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Dela 256KB EPROM cartridge emulation

**********************************************************************/

#pragma once

#ifndef __DELA_EP256__
#define __DELA_EP256__


#include "emu.h"
#include "exp.h"
#include "bus/generic/slot.h"
#include "bus/generic/carts.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_dela_ep256_cartridge_device

class c64_dela_ep256_cartridge_device : public device_t,
										public device_c64_expansion_card_interface
{
public:
	// construction/destruction
	c64_dela_ep256_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_c64_expansion_card_interface overrides
	virtual uint8_t c64_cd_r(address_space &space, offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;
	virtual void c64_cd_w(address_space &space, offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;

private:
	generic_slot_device *m_eproms[8];

	uint8_t m_bank, m_socket;
	int m_reset;
};


// device type definition
extern const device_type C64_DELA_EP256;



#endif
