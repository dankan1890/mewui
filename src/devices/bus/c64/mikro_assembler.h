// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Mikro Assembler cartridge emulation

**********************************************************************/

#pragma once

#ifndef __MIKRO_ASSEMBLER__
#define __MIKRO_ASSEMBLER__


#include "emu.h"
#include "exp.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_mikro_assembler_cartridge_device

class c64_mikro_assembler_cartridge_device : public device_t,
												public device_c64_expansion_card_interface
{
public:
	// construction/destruction
	c64_mikro_assembler_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// device_c64_expansion_card_interface overrides
	virtual uint8_t c64_cd_r(address_space &space, offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;
};


// device type definition
extern const device_type C64_MIKRO_ASSEMBLER;


#endif
