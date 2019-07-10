// license:BSD-3-Clause
// copyright-holders:Steve Ellenoff, Pierpaolo Prazzoli
#ifndef MAME_INCLUDES_PORTRAIT_H
#define MAME_INCLUDES_PORTRAIT_H

#pragma once

#include "sound/tms5220.h"
#include "emupal.h"

class portrait_state : public driver_device
{
public:
	portrait_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_gfxdecode(*this, "gfxdecode")
		, m_palette(*this, "palette")
		, m_tms(*this, "tms")
		, m_bgvideoram(*this, "bgvideoram")
		, m_fgvideoram(*this, "fgvideoram")
		, m_spriteram(*this, "spriteram")
		, m_lamps(*this, "lamp%u", 0U)
	{ }

	static constexpr feature_type unemulated_features() { return feature::CAMERA; }

	void portrait(machine_config &config);

protected:
	virtual void machine_start() override { m_lamps.resolve(); }
	virtual void video_start() override;

private:
	DECLARE_WRITE8_MEMBER(ctrl_w);
	DECLARE_WRITE8_MEMBER(positive_scroll_w);
	DECLARE_WRITE8_MEMBER(negative_scroll_w);
	DECLARE_WRITE8_MEMBER(bgvideo_write);
	DECLARE_WRITE8_MEMBER(fgvideo_write);

	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);

	void portrait_palette(palette_device &palette) const;

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	inline void get_tile_info( tile_data &tileinfo, int tile_index, const uint8_t *source );
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void portrait_map(address_map &map);
	void portrait_sound_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<tms5200_device> m_tms;

	required_shared_ptr<uint8_t> m_bgvideoram;
	required_shared_ptr<uint8_t> m_fgvideoram;
	required_shared_ptr<uint8_t> m_spriteram;
	output_finder<2> m_lamps;

	int m_scroll;
	tilemap_t *m_foreground;
	tilemap_t *m_background;
};

#endif // MAME_INCLUDES_PORTRAIT_H
