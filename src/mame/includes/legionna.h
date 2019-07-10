// license:BSD-3-Clause
// copyright-holders:David Graves, Angelo Salese, David Haywood, Tomasz Slanina
#ifndef MAME_INCLUDES_LEGIONNA_H
#define MAME_INCLUDES_LEGIONNA_H

#pragma once

#include "sound/okim6295.h"
#include "audio/seibu.h"
#include "machine/gen_latch.h"
#include "machine/seibucop/seibucop.h"
#include "video/seibu_crtc.h"
#include "emupal.h"

#include <algorithm>

class legionna_state : public driver_device, public seibu_sound_common
{
public:
	legionna_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_spriteram(*this, "spriteram")
		, m_swappal(*this, "swappal")
		, m_layer_disable(0)
		, m_back_gfx_bank(0)
		, m_fore_gfx_bank(0)
		, m_mid_gfx_bank(0)
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_seibu_sound(*this, "seibu_sound")
		, m_oki(*this, "oki")
		, m_gfxdecode(*this, "gfxdecode")
		, m_palette(*this, "palette")
		, m_crtc(*this, "crtc")
		, m_raiden2cop(*this, "raiden2cop")
	{
		std::fill(std::begin(m_scrollvals), std::end(m_scrollvals), 0);
	}

	void cupsocs(machine_config &config);
	void heatbrl(machine_config &config);
	void cupsoc(machine_config &config);
	void grainbow(machine_config &config);
	void legionna(machine_config &config);
	void godzilla(machine_config &config);
	void denjinmk(machine_config &config);

	void init_legiongfx();
	void init_godzilla();
	void init_cupsoc_debug();
	void init_cupsoc();
	void init_cupsocs();
	void init_olysoc92();

private:
	required_shared_ptr<u16> m_spriteram;
	optional_shared_ptr<u16> m_swappal;
	std::unique_ptr<u16[]> m_back_data;
	std::unique_ptr<u16[]> m_fore_data;
	std::unique_ptr<u16[]> m_mid_data;
	std::unique_ptr<u16[]> m_textram;
	std::unique_ptr<u16[]> m_scrollram16;
	std::unique_ptr<u16[]> m_paletteram;
	u16 m_layer_disable;
	std::unique_ptr<u16[]> m_layer_config;
	int m_sprite_xoffs;
	int m_sprite_yoffs;
	tilemap_t *m_background_layer;
	tilemap_t *m_foreground_layer;
	tilemap_t *m_midground_layer;
	tilemap_t *m_text_layer;
	bool m_has_extended_banking;
	bool m_has_extended_priority;
	u16 m_sprite_pri_mask[4];
	u16 m_back_gfx_bank;
	u16 m_fore_gfx_bank;
	u16 m_mid_gfx_bank;
	u16 m_scrollvals[6];
	void tilemap_enable_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void tile_scroll_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void tile_scroll_base_w(offs_t offset, u16 data);
	void tile_vreg_1a_w(u16 data);
	void videowrite_cb_w(offs_t offset, u16 data);
	void background_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void midground_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void foreground_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void text_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	u8 denjinmk_sound_comms_r(offs_t offset);
	void godzilla_oki_bank_w(u8 data);
	void denjinmk_setgfxbank(u16 data);
	void heatbrl_setgfxbank(u16 data);
	void grainbow_layer_config_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void palette_swap_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	TILE_GET_INFO_MEMBER(get_back_tile_info);
	TILE_GET_INFO_MEMBER(get_mid_tile_info_split);
	TILE_GET_INFO_MEMBER(get_mid_tile_info_share_bgrom);
	TILE_GET_INFO_MEMBER(get_fore_tile_info);
	TILE_GET_INFO_MEMBER(get_text_tile_info);
	DECLARE_VIDEO_START(legionna);
	DECLARE_VIDEO_START(heatbrl);
	DECLARE_VIDEO_START(godzilla);
	DECLARE_VIDEO_START(denjinmk);
	DECLARE_VIDEO_START(grainbow);
	DECLARE_VIDEO_START(cupsoc);
	u32 screen_update_legionna(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_heatbrl(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_godzilla(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_grainbow(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_sprites(screen_device &screen, bitmap_ind16 &bitmap,const rectangle &cliprect);
	void descramble_legionnaire_gfx(u8* src);
	void common_video_start(bool split, bool has_extended_banking, bool has_extended_priority);
	void common_video_allocate_ptr();
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	optional_device<seibu_sound_device> m_seibu_sound;
	required_device<okim6295_device> m_oki;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<seibu_crtc_device> m_crtc;
	optional_device<raiden2cop_device> m_raiden2cop;
	void cupsoc_map(address_map &map);
	void cupsocs_map(address_map &map);
	void denjinmk_map(address_map &map);
	void godzilla_map(address_map &map);
	void grainbow_map(address_map &map);
	void heatbrl_map(address_map &map);
	void legionna_cop_map(address_map &map);
	void legionna_map(address_map &map);
	void godzilla_sound_io_map(address_map &map);
};

#endif // MAME_INCLUDES_LEGIONNA_H
