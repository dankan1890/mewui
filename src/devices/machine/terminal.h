// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
#ifndef MAME_MACHINE_TERMINAL_H
#define MAME_MACHINE_TERMINAL_H

#pragma once

#include "machine/keyboard.h"
#include "sound/beep.h"


#define TERMINAL_SCREEN_TAG "terminal_screen"


/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

INPUT_PORTS_EXTERN( generic_terminal );

class generic_terminal_device : public device_t
{
public:
	generic_terminal_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template <class FunctionClass>
	void set_keyboard_callback(void (FunctionClass::*callback)(u8 character), const char *name)
	{
		set_keyboard_callback(generic_keyboard_device::output_delegate(callback, name, nullptr, static_cast<FunctionClass *>(nullptr)));
	}
	// FIXME: this should be aware of current device for resolving the tag
	template <class FunctionClass>
	void set_keyboard_callback(const char *devname, void (FunctionClass::*callback)(u8 character), const char *name)
	{
		set_keyboard_callback(generic_keyboard_device::output_delegate(callback, name, devname, static_cast<FunctionClass *>(nullptr)));
	}
	void set_keyboard_callback(generic_keyboard_device::output_delegate callback) { m_keyboard_cb = callback; }

	void write(u8 data) { term_write(data); }

	void kbd_put(u8 data);

protected:
	enum { BELL_TIMER_ID = 20'000 };

	generic_terminal_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, unsigned w, unsigned h);

	virtual void term_write(uint8_t data);
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void send_key(uint8_t code) { if (!m_keyboard_cb.isnull()) m_keyboard_cb(code); }

	required_ioport m_io_term_conf;

	static constexpr unsigned TERMINAL_WIDTH = 80;
	static constexpr unsigned TERMINAL_HEIGHT = 24;

	unsigned const m_width;
	unsigned const m_height;
	std::unique_ptr<uint8_t []> m_buffer;
	uint8_t m_x_pos;

private:
	void scroll_line();
	void write_char(uint8_t data);
	void clear();
	uint32_t update(screen_device &device, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	uint8_t m_framecnt;
	uint8_t m_y_pos;

	emu_timer *m_bell_timer;
	required_device<beep_device> m_beeper;
	generic_keyboard_device::output_delegate m_keyboard_cb;
};

DECLARE_DEVICE_TYPE(GENERIC_TERMINAL, generic_terminal_device)

#endif // MAME_DEVICES_TERMINAL_H
