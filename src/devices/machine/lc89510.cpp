// license:BSD-3-Clause
// copyright-holders:David Haywood
/* LC89510 CD Controller
 based off old NeoCD emulator code, adapted to SegaCD, needs reworking to work with NeoCD again

*/


#include "emu.h"
#include "lc89510.h"

const device_type LC89510 = &device_creator<lc89510_device>;

lc89510_device::lc89510_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, LC89510, "lc89510_device", tag, owner, clock, "lc89510", __FILE__)
{
}


void lc89510_device::device_start()
{
}

void lc89510_device::device_reset()
{
}
