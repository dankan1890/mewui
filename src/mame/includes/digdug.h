// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
#ifndef MAME_INCLUDES_DIGDUG_H
#define MAME_INCLUDES_DIGDUG_H

#pragma once

#include "machine/er2055.h"

class digdug_state : public galaga_state
{
public:
	digdug_state(const machine_config &mconfig, device_type type, const char *tag) :
		galaga_state(mconfig, type, tag),
		m_earom(*this, "earom"),
		m_digdug_objram(*this, "digdug_objram"),
		m_digdug_posram(*this, "digdug_posram"),
		m_digdug_flpram(*this, "digdug_flpram")
	{ }

	void dzigzag(machine_config &config);
	void digdug(machine_config &config);

private:
	required_device<er2055_device> m_earom;
	required_shared_ptr<uint8_t> m_digdug_objram;
	required_shared_ptr<uint8_t> m_digdug_posram;
	required_shared_ptr<uint8_t> m_digdug_flpram;

	uint8_t m_bg_select;
	uint8_t m_tx_color_mode;
	uint8_t m_bg_disable;
	uint8_t m_bg_color_bank;

	TILEMAP_MAPPER_MEMBER(tilemap_scan);
	TILE_GET_INFO_MEMBER(bg_get_tile_info);
	TILE_GET_INFO_MEMBER(tx_get_tile_info);
	DECLARE_VIDEO_START(digdug);
	void digdug_palette(palette_device &palette) const;
	uint32_t screen_update_digdug(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE8_MEMBER(digdug_videoram_w);
	DECLARE_WRITE8_MEMBER(bg_select_w);
	DECLARE_WRITE_LINE_MEMBER(tx_color_mode_w);
	DECLARE_WRITE_LINE_MEMBER(bg_disable_w);

	DECLARE_READ8_MEMBER(earom_read);
	DECLARE_WRITE8_MEMBER(earom_write);
	DECLARE_WRITE8_MEMBER(earom_control_w);
	virtual void machine_start() override;

	void digdug_map(address_map &map);
};

#endif // MAME_INCLUDES_DIGDUG_H
