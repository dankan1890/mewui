// license:BSD-3-Clause
// copyright-holders:Mathis Rosenhauer
/*****************************************************************************
 *
 * includes/vectrex.h
 *
 ****************************************************************************/
#ifndef MAME_INCLUDES_VECTREX_H
#define MAME_INCLUDES_VECTREX_H

#pragma once

#include "machine/6522via.h"
#include "sound/dac.h"
#include "sound/ay8910.h"
#include "video/vector.h"

#include "bus/vectrex/slot.h"
#include "bus/vectrex/rom.h"

#include "screen.h"

#define NVECT 10000

class vectrex_base_state : public driver_device
{
public:
	enum
	{
		TIMER_VECTREX_IMAGER_CHANGE_COLOR,
		TIMER_UPDATE_LEVEL,
		TIMER_VECTREX_IMAGER_EYE,
		TIMER_LIGHTPEN_TRIGGER,
		TIMER_VECTREX_REFRESH,
		TIMER_VECTREX_ZERO_INTEGRATORS,
		TIMER_UPDATE_SIGNAL
	};

	void vectrex_cart(device_slot_interface &device);

protected:
	vectrex_base_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_cart(*this, "cartslot"),
		m_via6522_0(*this, "via6522_0"),
		m_gce_vectorram(*this, "gce_vectorram"),
		m_dac(*this, "dac"),
		m_ay8912(*this, "ay8912"),
		m_vector(*this, "vector"),
		m_io_contr(*this, {"CONTR1X", "CONTR1Y", "CONTR2X", "CONTR2Y"}),
		m_io_buttons(*this, "BUTTONS"),
		m_io_3dconf(*this, "3DCONF"),
		m_io_lpenconf(*this, "LPENCONF"),
		m_io_lpenx(*this, "LPENX"),
		m_io_lpeny(*this, "LPENY"),
		m_screen(*this, "screen")
	{ }

	DECLARE_WRITE8_MEMBER(vectrex_psg_port_w);
	DECLARE_READ8_MEMBER(vectrex_via_r);
	DECLARE_WRITE8_MEMBER(vectrex_via_w);
	virtual void driver_start() override;
	virtual void video_start() override;
	uint32_t screen_update_vectrex(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(vectrex_imager_change_color);
	TIMER_CALLBACK_MEMBER(update_level);
	TIMER_CALLBACK_MEMBER(vectrex_imager_eye);
	TIMER_CALLBACK_MEMBER(lightpen_trigger);
	TIMER_CALLBACK_MEMBER(vectrex_refresh);
	TIMER_CALLBACK_MEMBER(vectrex_zero_integrators);
	TIMER_CALLBACK_MEMBER(update_signal);
	DECLARE_READ8_MEMBER(vectrex_via_pb_r);
	DECLARE_READ8_MEMBER(vectrex_via_pa_r);
	DECLARE_WRITE8_MEMBER(v_via_pb_w);
	DECLARE_WRITE8_MEMBER(v_via_pa_w);
	DECLARE_WRITE_LINE_MEMBER(v_via_ca2_w);
	DECLARE_WRITE_LINE_MEMBER(v_via_cb2_w);
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(cart_load);
	DECLARE_WRITE_LINE_MEMBER(vectrex_via_irq);

	void vectrex_base(machine_config &config);

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	void configure_imager(bool reset_refresh, const double *imager_angles);
	void vectrex_configuration();
	void vectrex_multiplexer(int mux);
	void vectrex_add_point(int x, int y, rgb_t color, int intensity);
	void vectrex_add_point_stereo(int x, int y, rgb_t color, int intensity);

	unsigned char m_via_out[2];

	required_device<cpu_device> m_maincpu;
	optional_device<vectrex_cart_slot_device> m_cart;

	double m_imager_freq;
	emu_timer *m_imager_timer;
	emu_timer *m_lp_t;

	required_device<via6522_device> m_via6522_0;

private:

	struct vectrex_point
	{
		int x; int y;
		rgb_t col;
		int intensity;
	};

	required_shared_ptr<uint8_t> m_gce_vectorram;
	int m_imager_status;
	uint32_t m_beam_color;
	int m_lightpen_port;
	int m_reset_refresh;
	const double *m_imager_angles;
	rgb_t m_imager_colors[6];
	unsigned char m_imager_pinlevel;
	int m_old_mcontrol;
	double m_sl;
	double m_pwl;
	int m_x_center;
	int m_y_center;
	int m_x_max;
	int m_y_max;
	int m_x_int;
	int m_y_int;
	int m_lightpen_down;
	int m_pen_x;
	int m_pen_y;
	emu_timer *m_refresh;
	uint8_t m_blank;
	uint8_t m_ramp;
	int8_t m_analog[5];
	int m_point_index;
	int m_display_start;
	int m_display_end;
	vectrex_point m_points[NVECT];
	uint16_t m_via_timer2;
	attotime m_vector_start_time;
	uint8_t m_cb2;
	void (vectrex_base_state::*vector_add_point_function)(int, int, rgb_t, int);

	required_device<mc1408_device> m_dac;
	required_device<ay8910_device> m_ay8912;
	required_device<vector_device> m_vector;
	optional_ioport_array<4> m_io_contr;
	required_ioport m_io_buttons;
	required_ioport m_io_3dconf;
	required_ioport m_io_lpenconf;
	required_ioport m_io_lpenx;
	required_ioport m_io_lpeny;
	required_device<screen_device> m_screen;
};


class vectrex_state : public vectrex_base_state
{
public:
	vectrex_state(const machine_config &mconfig, device_type type, const char *tag) :
		vectrex_base_state(mconfig, type, tag)
	{ }

	void vectrex(machine_config &config);

protected:
	virtual void video_start() override;
	virtual void machine_start() override;

	void vectrex_map(address_map &map);
};


class raaspec_state : public vectrex_base_state
{
public:
	raaspec_state(const machine_config &mconfig, device_type type, const char *tag) :
		vectrex_base_state(mconfig, type, tag),
		m_io_coin(*this, "COIN")
	{ }

	void raaspec(machine_config &config);

protected:
	DECLARE_WRITE8_MEMBER(raaspec_led_w);
	DECLARE_READ8_MEMBER(vectrex_s1_via_pb_r);

	void raaspec_map(address_map &map);

private:
	required_ioport m_io_coin;
};

#endif // MAME_INCLUDES_VECTREX_H
