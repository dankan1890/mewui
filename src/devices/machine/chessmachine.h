// license:BSD-3-Clause
// copyright-holders:hap, Sandro Ronco
/*

  The ChessMachine by Tasc

*/

#ifndef MAME_MACHINE_CHESSMACHINE_H
#define MAME_MACHINE_CHESSMACHINE_H

#pragma once

#include "cpu/arm/arm.h"
#include "machine/timer.h"


class chessmachine_device : public device_t
{
public:
	chessmachine_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	// configuration helpers
	auto data_out() { return m_data_out.bind(); } // data_r

	// external read/write lines
	int data_r() { return m_latch[1]; }
	void data0_w(int state);
	void data1_w(int state);
	void reset_w(int state);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset_after_children() override { reset_w(1); }
	virtual void device_add_mconfig(machine_config &config) override;

	virtual const tiny_rom_entry *device_rom_region() const override;

private:
	required_device<arm_cpu_device> m_maincpu;
	required_region_ptr<u32> m_bootstrap;
	required_shared_ptr<u32> m_ram;
	required_device<timer_device> m_disable_bootstrap;

	devcb_write_line m_data_out;

	u8 m_latch[2];
	void sync0_callback(void *ptr, s32 param);
	void sync1_callback(void *ptr, s32 param);

	bool m_bootstrap_enabled;
	TIMER_DEVICE_CALLBACK_MEMBER(disable_bootstrap) { m_bootstrap_enabled = false; }
	u32 disable_bootstrap_r();
	u32 bootstrap_r(offs_t offset);

	u8 internal_r() { return m_latch[0]; }
	void internal_w(u8 data) { m_latch[1] = data & 1; m_data_out(m_latch[1]); }

	void main_map(address_map &map);
};


DECLARE_DEVICE_TYPE(CHESSMACHINE, chessmachine_device)

#endif // MAME_MACHINE_CHESSMACHINE_H
