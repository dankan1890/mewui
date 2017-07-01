// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
#ifndef MAME_DEVICES_TERMINAL_H
#define MAME_DEVICES_TERMINAL_H

#include "machine/keyboard.h"
#include "sound/beep.h"

#define TERMINAL_SCREEN_TAG "terminal_screen"

/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define MCFG_GENERIC_TERMINAL_KEYBOARD_CB(_devcb) \
	devcb = &generic_terminal_device::set_keyboard_callback(*device, DEVCB_##_devcb);

/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 24

INPUT_PORTS_EXTERN( generic_terminal );

class generic_terminal_device : public device_t
{
public:
	generic_terminal_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);
	generic_terminal_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template<class _Object> static devcb_base &set_keyboard_callback(device_t &device, _Object object) { return downcast<generic_terminal_device &>(device).m_keyboard_cb.set_callback(object); }

	DECLARE_WRITE8_MEMBER(write) { term_write(data); }
	DECLARE_WRITE8_MEMBER(kbd_put);
	uint32_t update(screen_device &device, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	virtual ioport_constructor device_input_ports() const override;
	virtual machine_config_constructor device_mconfig_additions() const override;

protected:
	enum { BELL_TIMER_ID = 20'000 };

	virtual void term_write(uint8_t data);
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	virtual void send_key(uint8_t code) { m_keyboard_cb((offs_t)0, code); }

	optional_device<palette_device> m_palette;
	required_ioport m_io_term_conf;

	uint8_t m_buffer[TERMINAL_WIDTH * 50]; // make big enough for teleprinter
	uint8_t m_x_pos;

private:
	void scroll_line();
	void write_char(uint8_t data);
	void clear();

	uint8_t m_framecnt;
	uint8_t m_y_pos;

	emu_timer *m_bell_timer;
	required_device<beep_device> m_beeper;
	devcb_write8 m_keyboard_cb;
};

extern const device_type GENERIC_TERMINAL;

#endif /* MAME_DEVICES_TERMINAL_H */
