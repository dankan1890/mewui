// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#ifndef MAME_BUS_MSX_SLOT_SONY08_H
#define MAME_BUS_MSX_SLOT_SONY08_H

#pragma once

#include "slot.h"
#include "machine/nvram.h"


DECLARE_DEVICE_TYPE(MSX_SLOT_SONY08, msx_slot_sony08_device)


class msx_slot_sony08_device : public device_t, public msx_internal_slot_interface
{
public:
	msx_slot_sony08_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration helpers
	void set_rom_start(const char *region, uint32_t offset) { m_rom_region.set_tag(region); m_region_offset = offset; }

	virtual uint8_t read(offs_t offset) override;
	virtual void write(offs_t offset, uint8_t data) override;

protected:
	virtual void device_start() override;
	virtual void device_post_load() override;

	virtual void device_add_mconfig(machine_config &config) override;

	void restore_banks();

private:
	required_device<nvram_device> m_nvram;
	required_memory_region m_rom_region;
	uint32_t m_region_offset;
	const uint8_t *m_rom;
	uint8_t m_selected_bank[8];
	const uint8_t *m_bank_base[8];
	uint8_t m_sram[0x4000];

	void map_bank(int bank);
};


#endif // MAME_BUS_MSX_SLOT_SONY08_H
