// license:BSD-3-Clause
// copyright-holders:Joseba Epalza
#ifndef MAME_INCLUDES_SPEEDBAL_H
#define MAME_INCLUDES_SPEEDBAL_H

#pragma once

#include "emupal.h"

class speedbal_state : public driver_device
{
public:
	speedbal_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_gfxdecode(*this, "gfxdecode")
		, m_palette(*this, "palette")
		, m_spriteram(*this, "spriteram")
		, m_background_videoram(*this, "bg_videoram")
		, m_foreground_videoram(*this, "fg_videoram")
		, m_digits(*this, "digit%u", 0U)
	{ }

	void speedbal(machine_config &config);

	void init_speedbal();
	void init_musicbal();

protected:
	virtual void machine_start() override;
	virtual void video_start() override;

private:
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;

	required_shared_ptr<uint8_t> m_spriteram;
	required_shared_ptr<uint8_t> m_background_videoram;
	required_shared_ptr<uint8_t> m_foreground_videoram;
	output_finder<73> m_digits;

	bool m_leds_start;
	uint32_t m_leds_shiftreg;
	tilemap_t *m_bg_tilemap;
	tilemap_t *m_fg_tilemap;

	DECLARE_WRITE8_MEMBER(coincounter_w);
	DECLARE_WRITE8_MEMBER(foreground_videoram_w);
	DECLARE_WRITE8_MEMBER(background_videoram_w);
	DECLARE_WRITE8_MEMBER(maincpu_50_w);
	DECLARE_WRITE8_MEMBER(leds_output_block);
	DECLARE_WRITE8_MEMBER(leds_start_block);
	DECLARE_WRITE8_MEMBER(leds_shift_bit);

	TILE_GET_INFO_MEMBER(get_tile_info_bg);
	TILE_GET_INFO_MEMBER(get_tile_info_fg);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);

	void main_cpu_io_map(address_map &map);
	void main_cpu_map(address_map &map);
	void sound_cpu_io_map(address_map &map);
	void sound_cpu_map(address_map &map);
};

#endif // MAME_INCLUDES_SPEEDBAL_H
