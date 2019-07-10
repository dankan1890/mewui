// license:GPL-2.0+
// copyright-holders:Raphael Nabet, Robbbert
#ifndef MAME_INCLUDES_APEXC
#define MAME_INCLUDES_APEXC

#pragma once

#include "cpu/apexc/apexc.h"
#include "machine/apexc.h"
#include "emupal.h"
#include "screen.h"

class apexc_state : public driver_device
{
public:
	apexc_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_screen(*this, "screen")
		, m_gfxdecode(*this, "gfxdecode")
		, m_palette(*this, "palette")
		, m_chargen_region(*this, "chargen")
		, m_cylinder(*this, "cylinder")
		, m_tape_puncher(*this, "tape_puncher")
		, m_tape_reader(*this, "tape_reader")
		, m_panel_port(*this, "panel")
		, m_data_port(*this, "data")
		, m_input_timer(nullptr)
	{ }

	void init_apexc();

	void apexc(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void video_start() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	void check_inputs();

private:
	void apexc_palette(palette_device &palette) const;
	uint32_t screen_update_apexc(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(apexc_interrupt);
	DECLARE_WRITE8_MEMBER(tape_write);
	void draw_led(bitmap_ind16 &bitmap, int x, int y, int state);
	void draw_char(bitmap_ind16 &bitmap, char character, int x, int y, int color);
	void draw_string(bitmap_ind16 &bitmap, const char *buf, int x, int y, int color);
	void teletyper_init();
	void teletyper_linefeed();
	void teletyper_putchar(int character);

	void mem(address_map &map);

	uint32_t m_panel_data_reg;    /* value of a data register on the control panel which can
	                            be edited - the existence of this register is a personnal
	                            guess */

	std::unique_ptr<bitmap_ind16> m_bitmap;

	uint32_t m_old_edit_keys;
	int m_old_control_keys;

	int m_letters;
	int m_pos;

	required_device<apexc_cpu_device> m_maincpu;
	required_device<screen_device> m_screen;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_memory_region m_chargen_region;
	required_device<apexc_cylinder_image_device> m_cylinder;
	required_device<apexc_tape_puncher_image_device> m_tape_puncher;
	required_device<apexc_tape_reader_image_device> m_tape_reader;
	required_ioport m_panel_port;
	required_ioport m_data_port;

	emu_timer *m_input_timer;

	static const device_timer_id TIMER_POLL_INPUTS;
	static const rgb_t palette_table[4];
};

#endif // MAME_INCLUDES_APEXC
