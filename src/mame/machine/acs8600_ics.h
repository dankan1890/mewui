// license:BSD-3-Clause
// copyright-holders:Carl

#ifndef MAME_MACHINE_ACS8600_ICS_H
#define MAME_MACHINE_ACS8600_ICS_H

#pragma once

#include "cpu/z80/z80.h"

class acs8600_ics_device : public device_t
{
public:
	acs8600_ics_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_WRITE_LINE_MEMBER(attn_w);

	auto irq1_callback() { return m_out_irq1_func.bind(); }
	auto irq2_callback() { return m_out_irq2_func.bind(); }
	template <typename T> void set_host_space(T &&tag, int index) { m_host_space.set_tag(std::forward<T>(tag), index); }
	const tiny_rom_entry *device_rom_region() const override;

protected:
	virtual void device_resolve_objects() override;
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;

	DECLARE_WRITE8_MEMBER(hiaddr_w);
	DECLARE_WRITE8_MEMBER(ctrl_w);
	DECLARE_READ8_MEMBER(hostram_r);
	DECLARE_WRITE8_MEMBER(hostram_w);

	void ics_io(address_map &map);
	void ics_mem(address_map &map);

private:
	required_device<z80_device> m_icscpu;
	devcb_write_line m_out_irq1_func;
	devcb_write_line m_out_irq2_func;
	required_address_space m_host_space;
	u8 m_hiaddr;
	u8 m_ctrl;
};

DECLARE_DEVICE_TYPE(ACS8600_ICS, acs8600_ics_device)

#endif // MAME_MACHINE_ACS8600_ICS_H
