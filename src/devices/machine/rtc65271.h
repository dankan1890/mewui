// license:BSD-3-Clause
// copyright-holders:Raphael Nabet, R. Belmont
/*
    rtc65271.h: include file for rtc65271.cpp
*/

#ifndef MAME_MACHINE_RTC65271_H
#define MAME_MACHINE_RTC65271_H

#pragma once


// ======================> rtc65271_device

class rtc65271_device : public device_t,
						public device_nvram_interface
{
public:
	// construction/destruction
	rtc65271_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto interrupt_cb() { return m_interrupt_cb.bind(); }

	uint8_t rtc_r(offs_t offset);
	uint8_t xram_r(offs_t offset);
	void rtc_w(offs_t offset, uint8_t data);
	void xram_w(offs_t offset, uint8_t data);

protected:
	// device-level overrides
	virtual void device_start() override;
	// device_nvram_interface overrides
	virtual void nvram_default() override;
	virtual void nvram_read(emu_file &file) override;
	virtual void nvram_write(emu_file &file) override;

private:
	uint8_t read(int xramsel, offs_t offset);
	void write(int xramsel, offs_t offset, uint8_t data);
	void field_interrupts();

	TIMER_CALLBACK_MEMBER(rtc_SQW_cb);
	TIMER_CALLBACK_MEMBER(rtc_begin_update_cb);
	TIMER_CALLBACK_MEMBER(rtc_end_update_cb);
	/* 64 8-bit registers (10 clock registers, 4 control/status registers, and
	50 bytes of user RAM) */
	uint8_t m_regs[64];
	uint8_t m_cur_reg;

	/* extended RAM: 4kbytes of battery-backed RAM (in pages of 32 bytes) */
	uint8_t m_xram[4096];
	uint8_t m_cur_xram_page;

	/* update timer: called every second */
	emu_timer *m_update_timer;

	/* SQW timer: called every periodic clock half-period */
	emu_timer *m_SQW_timer;
	uint8_t m_SQW_internal_state;

	/* callback called when interrupt pin state changes (may be nullptr) */
	devcb_write_line    m_interrupt_cb;
};

// device type definition
DECLARE_DEVICE_TYPE(RTC65271, rtc65271_device)

#endif // MAME_MACHINE_RTC65271_H
