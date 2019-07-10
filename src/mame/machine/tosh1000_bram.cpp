// license:BSD-3-Clause
// copyright-holders:Sergey Svishchev
/***************************************************************************

    Toshiba 1000 Backup RAM

    Simulation of interface provided by 80C50 keyboard controller.

***************************************************************************/

#include "emu.h"
#include "tosh1000_bram.h"

#include <algorithm>


DEFINE_DEVICE_TYPE(TOSH1000_BRAM, tosh1000_bram_device, "tosh1000_bram", "Toshiba T1000 Backup RAM")

//-------------------------------------------------
//  ctor
//-------------------------------------------------

tosh1000_bram_device::tosh1000_bram_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, TOSH1000_BRAM, tag, owner, clock)
	, device_nvram_interface(mconfig, *this)
{
}

uint8_t tosh1000_bram_device::read(int offset)
{
	assert(BRAM_SIZE > offset);
	return m_bram[offset];
}

void tosh1000_bram_device::write(int offset, uint8_t data)
{
	assert(BRAM_SIZE > offset);
	m_bram[offset] = data;
}

READ8_MEMBER( tosh1000_bram_device::read )
{
	assert(BRAM_SIZE > offset);
	return m_bram[offset];
}

WRITE8_MEMBER( tosh1000_bram_device::write )
{
	assert(BRAM_SIZE > offset);
	m_bram[offset] = data;
}

//-------------------------------------------------
//  nvram_default - called to initialize NVRAM to
//  its default state
//-------------------------------------------------

void tosh1000_bram_device::nvram_default()
{
	std::fill(std::begin(m_bram), std::end(m_bram), 0x1a);
}

//-------------------------------------------------
//  nvram_read - called to read NVRAM from the
//  .nv file
//-------------------------------------------------

void tosh1000_bram_device::nvram_read(emu_file &file)
{
	file.read(m_bram, BRAM_SIZE);
}

//-------------------------------------------------
//  nvram_write - called to write NVRAM to the
//  .nv file
//-------------------------------------------------

void tosh1000_bram_device::nvram_write(emu_file &file)
{
	file.write(m_bram, BRAM_SIZE);
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------
void tosh1000_bram_device::device_start()
{
	/* register for save states */
	save_item(NAME(m_bram));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void tosh1000_bram_device::device_reset()
{
}
