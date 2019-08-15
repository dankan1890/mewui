// license:BSD-3-Clause
// copyright-holders:smf
#include "emu.h"
#include "ds128x.h"

DEFINE_DEVICE_TYPE(DS12885, ds12885_device, "ds12885", "DS12885 RTC/NVRAM")

//-------------------------------------------------
//  ds12885_device - constructor
//-------------------------------------------------

ds12885_device::ds12885_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: ds12885_device(mconfig, DS12885, tag, owner, clock)
{
}

ds12885_device::ds12885_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: mc146818_device(mconfig, type, tag, owner, clock)
{
}

int ds12885_device::get_timer_bypass() const
{
	if( !( m_data[REG_A] & REG_A_DV0 ) ) //DV0 must be 0 for timekeeping
	{
		return 7; // Fixed at 1 Hz with clock at 32768Hz
	}

	return 22; // No tick
}

DEFINE_DEVICE_TYPE(DS12885EXT, ds12885ext_device, "ds12885ext", "DS12885 RTC/NVRAM size 256 bytes")

//-------------------------------------------------
//  ds12885ext_device - constructor
//-------------------------------------------------

ds12885ext_device::ds12885ext_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: ds12885_device(mconfig, DS12885EXT, tag, owner, clock)
{
}

//-------------------------------------------------
//  ds12885ext_device - handlers that allow acces to extended memory size
//-------------------------------------------------

uint8_t ds12885ext_device::read_extended(offs_t offset)
{
	switch (offset)
	{
	case 0:
	case 1:
		return read(offset);
		break;
	case 2:
	case 3:
		return read(offset - 2);
		break;
	default:
		return 0xff;
	}
}

void ds12885ext_device::write_extended(offs_t offset, uint8_t data)
{
	switch (offset)
	{
	case 0:
		write(offset, data & 127);
		break;
	case 1:
		write(offset, data);
		break;
	case 2:
		write(offset - 2, data);
		break;
	case 3:
		write(offset - 2, data);
		break;
	}
}
