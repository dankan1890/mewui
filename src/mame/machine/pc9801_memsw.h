// license:BSD-3-Clause
// copyright-holders:Angelo Salese
/***************************************************************************

Template for skeleton device

***************************************************************************/

#ifndef MAME_MACHINE_PC9801_MEMSW_H
#define MAME_MACHINE_PC9801_MEMSW_H

#pragma once

#include "machine/nvram.h"


//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> pc9801_memsw_device

class pc9801_memsw_device : public device_t,
							public device_nvram_interface
{
public:
	// construction/destruction
	pc9801_memsw_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint8_t read(uint8_t offset);
	void write(uint8_t offset, uint8_t data);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void nvram_default() override;
	virtual void nvram_read(emu_file &file) override;
	virtual void nvram_write(emu_file &file) override;

private:
	uint8_t m_bram[0x10];
	const uint8_t m_bram_size = 0x10;
};


// device type definition
DECLARE_DEVICE_TYPE(PC9801_MEMSW, pc9801_memsw_device)

#endif // MAME_MACHINE_PC9801_MEMSW_H
