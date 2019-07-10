// license:BSD-3-Clause
// copyright-holders:Zsolt Vasvari
/*************************************************************************

    Mermaid

*************************************************************************/
#ifndef MAME_INCLUDES_MERMAID_H
#define MAME_INCLUDES_MERMAID_H

#pragma once

#include "machine/74259.h"
#include "machine/ripple_counter.h"
#include "sound/msm5205.h"
#include "sound/ay8910.h"
#include "emupal.h"
#include "screen.h"

class mermaid_state : public driver_device
{
public:
	mermaid_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_videoram2(*this, "videoram2"),
		m_videoram(*this, "videoram"),
		m_bg_scrollram(*this, "bg_scrollram"),
		m_fg_scrollram(*this, "fg_scrollram"),
		m_spriteram(*this, "spriteram"),
		m_colorram(*this, "colorram"),
		m_maincpu(*this, "maincpu"),
		m_adpcm(*this, "adpcm"),
		m_adpcm_counter(*this, "adpcm_counter"),
		m_ay8910(*this, "ay%u", 1),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),
		m_latch(*this, "latch%u", 1U)
	{
	}

	void rougien(machine_config &config);
	void mermaid(machine_config &config);

private:
	/* memory pointers */
	required_shared_ptr<uint8_t> m_videoram2;
	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_bg_scrollram;
	required_shared_ptr<uint8_t> m_fg_scrollram;
	required_shared_ptr<uint8_t> m_spriteram;
	required_shared_ptr<uint8_t> m_colorram;

	/* video-related */
	tilemap_t *m_bg_tilemap;
	tilemap_t *m_fg_tilemap;
	bitmap_ind16 m_helper;
	bitmap_ind16 m_helper2;
	int m_coll_bit0;
	int m_coll_bit1;
	int m_coll_bit2;
	int m_coll_bit3;
	int m_coll_bit6;
	int m_rougien_gfxbank1;
	int m_rougien_gfxbank2;

	/* sound-related */
	uint8_t    m_adpcm_idle;
	int      m_adpcm_data;
	uint8_t    m_adpcm_trigger;
	uint8_t    m_adpcm_rom_sel;
	bool       m_ay8910_enable[2];

	/* devices */
	required_device<cpu_device> m_maincpu;
	optional_device<msm5205_device> m_adpcm;
	optional_device<ripple_counter_device> m_adpcm_counter;
	required_device_array<ay8910_device, 2> m_ay8910;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device_array<ls259_device, 2> m_latch;

	uint8_t    m_nmi_mask;
	DECLARE_WRITE8_MEMBER(mermaid_ay8910_write_port_w);
	DECLARE_WRITE8_MEMBER(mermaid_ay8910_control_port_w);
	DECLARE_WRITE_LINE_MEMBER(ay1_enable_w);
	DECLARE_WRITE_LINE_MEMBER(ay2_enable_w);
	DECLARE_WRITE_LINE_MEMBER(nmi_mask_w);
	DECLARE_WRITE_LINE_MEMBER(rougien_sample_rom_lo_w);
	DECLARE_WRITE_LINE_MEMBER(rougien_sample_rom_hi_w);
	DECLARE_WRITE_LINE_MEMBER(rougien_sample_playback_w);
	DECLARE_WRITE8_MEMBER(adpcm_data_w);
	DECLARE_WRITE8_MEMBER(mermaid_videoram2_w);
	DECLARE_WRITE8_MEMBER(mermaid_videoram_w);
	DECLARE_WRITE8_MEMBER(mermaid_colorram_w);
	DECLARE_WRITE_LINE_MEMBER(flip_screen_x_w);
	DECLARE_WRITE_LINE_MEMBER(flip_screen_y_w);
	DECLARE_WRITE8_MEMBER(mermaid_bg_scroll_w);
	DECLARE_WRITE8_MEMBER(mermaid_fg_scroll_w);
	DECLARE_WRITE_LINE_MEMBER(rougien_gfxbankswitch1_w);
	DECLARE_WRITE_LINE_MEMBER(rougien_gfxbankswitch2_w);
	DECLARE_READ8_MEMBER(mermaid_collision_r);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	void common_palette(palette_device &palette) const;
	void mermaid_palette(palette_device &palette) const;
	void rougien_palette(palette_device &palette) const;
	uint32_t screen_update_mermaid(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(screen_vblank_mermaid);
	void draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect );
	uint8_t collision_check( rectangle& rect );
	void collision_update();
	DECLARE_WRITE_LINE_MEMBER(rougien_adpcm_int);
	void mermaid_map(address_map &map);
};

#endif // MAME_INCLUDES_MERMAID_H
