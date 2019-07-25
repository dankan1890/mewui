// license:BSD-3-Clause
// copyright-holders:AJR

#ifndef MAME_BUS_NSCSI_CDD2000_H
#define MAME_BUS_NSCSI_CDD2000_H 1

#pragma once

#include "machine/nscsi_bus.h"

class cdd2000_device : public device_t, public nscsi_slot_card_interface
{
public:
	cdd2000_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	static constexpr feature_type unemulated_features() { return feature::DISK; }

protected:
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

private:
	void mem_map(address_map &map);

	required_device<cpu_device> m_cdcpu;
};

DECLARE_DEVICE_TYPE(CDD2000, cdd2000_device)

#endif // MAME_BUS_NSCSI_CDD2000_H
