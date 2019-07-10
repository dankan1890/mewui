// license:BSD-3-Clause
// copyright-holders:smf, David Haywood
#ifndef MAME_INCLUDES_M62_H
#define MAME_INCLUDES_M62_H

#pragma once

#include "audio/irem.h"
#include "emupal.h"

class m62_state : public driver_device
{
public:
	m62_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_spriteram(*this, "spriteram"),
		m_m62_tileram(*this, "m62_tileram"),
		m_m62_textram(*this, "m62_textram"),
		m_scrollram(*this, "scrollram"),
		m_sprite_height_prom(*this, "spr_height_prom"),
		m_sprite_color_proms(*this, "spr_color_proms"),
		m_chr_color_proms(*this, "chr_color_proms"),
		m_fg_color_proms(*this, "fg_color_proms"),
		m_maincpu(*this, "maincpu"),
		m_fg_decode(*this, "fg_decode"),
		m_spr_decode(*this, "spr_decode"),
		m_chr_decode(*this, "chr_decode"),
		m_fg_palette(*this, "fg_palette"),
		m_spr_palette(*this, "spr_palette"),
		m_chr_palette(*this, "chr_palette"),
		m_audio(*this, "irem_audio")
	{ }

	void ldrun2(machine_config &config);
	void lotlot(machine_config &config);
	void ldrun3(machine_config &config);
	void battroad(machine_config &config);
	void horizon(machine_config &config);
	void ldrun4(machine_config &config);
	void spelunk2(machine_config &config);
	void youjyudn(machine_config &config);
	void kungfum(machine_config &config);
	void spelunkr(machine_config &config);
	void ldrun(machine_config &config);
	void kidniki(machine_config &config);

	void init_youjyudn();
	void init_spelunkr();
	void init_ldrun2();
	void init_ldrun4();
	void init_spelunk2();
	void init_kidniki();
	void init_battroad();

private:
	/* memory pointers */
	required_shared_ptr<uint8_t> m_spriteram;

	required_shared_ptr<uint8_t> m_m62_tileram;
	optional_shared_ptr<uint8_t> m_m62_textram;
	optional_shared_ptr<uint8_t> m_scrollram;

	/* video-related */
	tilemap_t*             m_bg_tilemap;
	tilemap_t*             m_fg_tilemap;
	int                  m_flipscreen;
	required_region_ptr<uint8_t> m_sprite_height_prom;
	required_region_ptr<uint8_t> m_sprite_color_proms;
	required_region_ptr<uint8_t> m_chr_color_proms;
	optional_region_ptr<uint8_t> m_fg_color_proms;
	int32_t                m_m62_background_hscroll;
	int32_t                m_m62_background_vscroll;
	uint8_t                m_kidniki_background_bank;
	int32_t                m_kidniki_text_vscroll;
	int                  m_ldrun3_topbottom_mask;
	int32_t                m_spelunkr_palbank;

	/* misc */
	int                 m_ldrun2_bankswap;  //ldrun2
	int                 m_bankcontrol[2];   //ldrun2
	DECLARE_READ8_MEMBER(ldrun2_bankswitch_r);
	DECLARE_WRITE8_MEMBER(ldrun2_bankswitch_w);
	DECLARE_READ8_MEMBER(ldrun3_prot_5_r);
	DECLARE_READ8_MEMBER(ldrun3_prot_7_r);
	DECLARE_WRITE8_MEMBER(ldrun4_bankswitch_w);
	DECLARE_WRITE8_MEMBER(kidniki_bankswitch_w);
	DECLARE_WRITE8_MEMBER(spelunkr_bankswitch_w);
	DECLARE_WRITE8_MEMBER(spelunk2_bankswitch_w);
	DECLARE_WRITE8_MEMBER(youjyudn_bankswitch_w);
	DECLARE_WRITE8_MEMBER(m62_flipscreen_w);
	DECLARE_WRITE8_MEMBER(m62_hscroll_low_w);
	DECLARE_WRITE8_MEMBER(m62_hscroll_high_w);
	DECLARE_WRITE8_MEMBER(m62_vscroll_low_w);
	DECLARE_WRITE8_MEMBER(m62_vscroll_high_w);
	DECLARE_WRITE8_MEMBER(m62_tileram_w);
	DECLARE_WRITE8_MEMBER(m62_textram_w);
	DECLARE_WRITE8_MEMBER(kungfum_tileram_w);
	DECLARE_WRITE8_MEMBER(ldrun3_topbottom_mask_w);
	DECLARE_WRITE8_MEMBER(kidniki_text_vscroll_low_w);
	DECLARE_WRITE8_MEMBER(kidniki_text_vscroll_high_w);
	DECLARE_WRITE8_MEMBER(kidniki_background_bank_w);
	DECLARE_WRITE8_MEMBER(spelunkr_palbank_w);
	DECLARE_WRITE8_MEMBER(spelunk2_gfxport_w);
	DECLARE_WRITE8_MEMBER(horizon_scrollram_w);
	TILE_GET_INFO_MEMBER(get_kungfum_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_ldrun_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_ldrun2_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_battroad_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_battroad_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_ldrun4_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_lotlot_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_lotlot_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_kidniki_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_kidniki_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_spelunkr_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_spelunkr_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_spelunk2_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_youjyudn_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_youjyudn_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_horizon_bg_tile_info);
	DECLARE_MACHINE_START(battroad);
	void machine_init_save();
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	void m62_spr(palette_device &palette) const;
	void m62_chr(palette_device &palette) const;
	void m62_lotlot_fg(palette_device &palette) const;
	void m62_battroad_fg(palette_device &palette) const;
	DECLARE_VIDEO_START(kungfum);
	DECLARE_VIDEO_START(battroad);
	DECLARE_VIDEO_START(ldrun2);
	DECLARE_VIDEO_START(ldrun4);
	DECLARE_VIDEO_START(lotlot);
	DECLARE_VIDEO_START(kidniki);
	DECLARE_VIDEO_START(spelunkr);
	DECLARE_VIDEO_START(spelunk2);
	void spelunk2_palette(palette_device &palette) const;
	DECLARE_VIDEO_START(youjyudn);
	DECLARE_VIDEO_START(horizon);
	uint32_t screen_update_ldrun(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_kungfum(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_battroad(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_ldrun3(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_ldrun4(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_lotlot(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_kidniki(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_spelunkr(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_spelunk2(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_youjyudn(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_horizon(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void m62_amplify_contrast(bool include_fg);
	void register_savestate();
	void draw_sprites(bitmap_rgb32 &bitmap, const rectangle &cliprect, int colormask, int prioritymask, int priority);
	void m62_start(tilemap_get_info_delegate tile_get_info, int rows, int cols, int x1, int y1, int x2, int y2);
	void m62_textlayer(tilemap_get_info_delegate tile_get_info, int rows, int cols, int x1, int y1, int x2, int y2);
	required_device<cpu_device> m_maincpu;
	optional_device<gfxdecode_device> m_fg_decode;
	required_device<gfxdecode_device> m_spr_decode;
	required_device<gfxdecode_device> m_chr_decode;
	optional_device<palette_device> m_fg_palette;
	required_device<palette_device> m_spr_palette;
	required_device<palette_device> m_chr_palette;
	required_device<irem_audio_device> m_audio;

	void battroad_io_map(address_map &map);
	void battroad_map(address_map &map);
	void horizon_map(address_map &map);
	void kidniki_io_map(address_map &map);
	void kidniki_map(address_map &map);
	void kungfum_io_map(address_map &map);
	void kungfum_map(address_map &map);
	void ldrun2_io_map(address_map &map);
	void ldrun2_map(address_map &map);
	void ldrun3_io_map(address_map &map);
	void ldrun3_map(address_map &map);
	void ldrun4_io_map(address_map &map);
	void ldrun4_map(address_map &map);
	void ldrun_map(address_map &map);
	void lotlot_map(address_map &map);
	void spelunk2_map(address_map &map);
	void spelunkr_map(address_map &map);
	void youjyudn_io_map(address_map &map);
	void youjyudn_map(address_map &map);
};

#endif // MAME_INCLUDES_M62_H
