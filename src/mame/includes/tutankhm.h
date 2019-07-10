// license:BSD-3-Clause
// copyright-holders:Mirko Buffoni
#ifndef MAME_INCLUDES_TUTANKHM_H
#define MAME_INCLUDES_TUTANKHM_H

#pragma once

#include "audio/timeplt.h"
#include "emupal.h"
#include "machine/timer.h"
#include "screen.h"

static constexpr int GALAXIAN_XSCALE = 3;
static constexpr XTAL GALAXIAN_MASTER_CLOCK(18.432_MHz_XTAL);
static constexpr XTAL GALAXIAN_PIXEL_CLOCK(GALAXIAN_XSCALE*GALAXIAN_MASTER_CLOCK / 3);
static constexpr int GALAXIAN_HTOTAL  = (384 * GALAXIAN_XSCALE);
static constexpr int GALAXIAN_HBEND   = (0 * GALAXIAN_XSCALE);
//static constexpr int GALAXIAN_H0START = (6*GALAXIAN_XSCALE)
//static constexpr int GALAXIAN_HBSTART = (264*GALAXIAN_XSCALE)
static constexpr int GALAXIAN_H0START = (0 * GALAXIAN_XSCALE);
static constexpr int GALAXIAN_HBSTART = (256 * GALAXIAN_XSCALE);

static constexpr int GALAXIAN_VTOTAL  = (264);
static constexpr int GALAXIAN_VBEND   = (16);
static constexpr int GALAXIAN_VBSTART = (224 + 16);

class tutankhm_state : public driver_device
{
public:
	tutankhm_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_scroll(*this, "scroll"),
		m_maincpu(*this, "maincpu"),
		m_palette(*this, "palette"),
		m_screen(*this, "screen"),
		m_timeplt_audio(*this, "timeplt_audio"),
		m_stars_config(*this, "STARS")
	{
	}

	void tutankhm(machine_config &config);

protected:
	DECLARE_WRITE_LINE_MEMBER(irq_enable_w);
	DECLARE_WRITE8_MEMBER(tutankhm_bankselect_w);
	DECLARE_WRITE_LINE_MEMBER(coin_counter_1_w);
	DECLARE_WRITE_LINE_MEMBER(coin_counter_2_w);
	DECLARE_WRITE8_MEMBER(sound_on_w);
	DECLARE_WRITE_LINE_MEMBER(flip_screen_x_w);
	DECLARE_WRITE_LINE_MEMBER(flip_screen_y_w);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	uint32_t screen_update_tutankhm_bootleg(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_tutankhm_scramble(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_tutankhm(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(vblank_irq);
	void main_map(address_map &map);
	virtual void video_start() override;
	void galaxian_palette(palette_device &palette);
	static rgb_t raw_to_rgb_func(u32 raw);

	/* memory pointers */
	required_shared_ptr<uint8_t> m_videoram;
	optional_shared_ptr<uint8_t> m_scroll;

	/* video-related */
	tilemap_t  *m_bg_tilemap;
	uint8_t     m_flipscreen_x;
	uint8_t     m_flipscreen_y;

	/* misc */
	uint8_t    m_irq_toggle;
	uint8_t    m_irq_enable;

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<palette_device> m_palette;
	required_device<screen_device> m_screen;
	optional_device<timeplt_audio_device> m_timeplt_audio;
	optional_ioport m_stars_config;

	TIMER_DEVICE_CALLBACK_MEMBER(scramble_stars_blink_timer);
	DECLARE_WRITE8_MEMBER(galaxian_stars_enable_w);
	void stars_init();
	void stars_init_scramble();
	void stars_init_bootleg();
	void stars_draw_row(bitmap_rgb32 &bitmap, int maxx, int y, uint32_t star_offs);
	void scramble_draw_stars(bitmap_rgb32 &bitmap, const rectangle &cliprect, int maxx);
	void scramble_draw_background(bitmap_rgb32 &bitmap, const rectangle &cliprect);

	uint8_t m_star_mode;
	rgb_t m_star_color[64];
	std::unique_ptr<uint8_t[]> m_stars;
	uint8_t m_stars_enabled;
	uint8_t m_stars_blink_state;
};

#endif // MAME_INCLUDES_TUTANKHM_H
