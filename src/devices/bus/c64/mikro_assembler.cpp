// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Mikro Assembler cartridge emulation

**********************************************************************/

#include "mikro_assembler.h"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type C64_MIKRO_ASSEMBLER = &device_creator<c64_mikro_assembler_cartridge_device>;



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  c64_mikro_assembler_cartridge_device - constructor
//-------------------------------------------------

c64_mikro_assembler_cartridge_device::c64_mikro_assembler_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, C64_MIKRO_ASSEMBLER, "C64 Mikro Assembler cartridge", tag, owner, clock, "c64_mikro_assembler", __FILE__),
	device_c64_expansion_card_interface(mconfig, *this)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void c64_mikro_assembler_cartridge_device::device_start()
{
}


//-------------------------------------------------
//  c64_cd_r - cartridge data read
//-------------------------------------------------

uint8_t c64_mikro_assembler_cartridge_device::c64_cd_r(address_space &space, offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	if (!roml || !romh || !io1 || !io2)
	{
		data = m_roml[offset & 0x1fff];
	}

	return data;
}
