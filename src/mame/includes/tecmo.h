// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
#ifndef MAME_INCLUDES_TECMO_H
#define MAME_INCLUDES_TECMO_H

#pragma once

#include "sound/msm5205.h"
#include "video/tecmo_spr.h"
#include "emupal.h"
#include "screen.h"

class tecmo_state : public driver_device
{
public:
	tecmo_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_soundcpu(*this, "soundcpu"),
		m_msm(*this, "msm"),
		m_screen(*this, "screen"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_sprgen(*this, "spritegen"),
		m_txvideoram(*this, "txvideoram"),
		m_fgvideoram(*this, "fgvideoram"),
		m_bgvideoram(*this, "bgvideoram"),
		m_spriteram(*this, "spriteram"),
		m_fgscroll(*this, "fgscroll"),
		m_bgscroll(*this, "bgscroll"),
		m_adpcm_rom(*this, "adpcm"),
		m_mainbank(*this, "mainbank")
	{ }

	void geminib(machine_config &config);
	void backfirt(machine_config &config);
	void silkworm(machine_config &config);
	void gemini(machine_config &config);
	void rygar(machine_config &config);
	void silkwormp(machine_config &config);

	void init_silkworm();
	void init_rygar();
	void init_gemini();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

private:
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_soundcpu;
	optional_device<msm5205_device> m_msm;
	required_device<screen_device> m_screen;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<tecmo_spr_device> m_sprgen;

	required_shared_ptr<uint8_t> m_txvideoram;
	required_shared_ptr<uint8_t> m_fgvideoram;
	required_shared_ptr<uint8_t> m_bgvideoram;
	required_shared_ptr<uint8_t> m_spriteram;
	required_shared_ptr<uint8_t> m_fgscroll;
	required_shared_ptr<uint8_t> m_bgscroll;

	optional_region_ptr<uint8_t> m_adpcm_rom;
	required_memory_bank m_mainbank;

	tilemap_t *m_tx_tilemap;
	tilemap_t *m_fg_tilemap;
	tilemap_t *m_bg_tilemap;
	int m_adpcm_pos;
	int m_adpcm_end;
	int m_adpcm_data;
	int m_video_type;

	DECLARE_WRITE8_MEMBER(bankswitch_w);
	DECLARE_WRITE8_MEMBER(adpcm_end_w);
	DECLARE_READ8_MEMBER(dswa_l_r);
	DECLARE_READ8_MEMBER(dswa_h_r);
	DECLARE_READ8_MEMBER(dswb_l_r);
	DECLARE_READ8_MEMBER(dswb_h_r);
	DECLARE_WRITE8_MEMBER(txvideoram_w);
	DECLARE_WRITE8_MEMBER(fgvideoram_w);
	DECLARE_WRITE8_MEMBER(bgvideoram_w);
	DECLARE_WRITE8_MEMBER(fgscroll_w);
	DECLARE_WRITE8_MEMBER(bgscroll_w);
	DECLARE_WRITE8_MEMBER(flipscreen_w);
	DECLARE_WRITE8_MEMBER(adpcm_start_w);
	DECLARE_WRITE8_MEMBER(adpcm_vol_w);
	DECLARE_WRITE_LINE_MEMBER(adpcm_int);

	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(gemini_get_bg_tile_info);
	TILE_GET_INFO_MEMBER(gemini_get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_tx_tile_info);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void gemini_map(address_map &map);
	void rygar_map(address_map &map);
	void rygar_sound_map(address_map &map);
	void silkworm_map(address_map &map);
	void tecmo_sound_map(address_map &map);
	void silkwormp_sound_map(address_map &map);
	void backfirt_sound_map(address_map &map);
};

#endif // MAME_INCLUDES_TECMO_H
