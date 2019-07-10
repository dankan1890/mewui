// license:BSD-3-Clause
// copyright-holders:Paul Leaman, Couriersud
/***************************************************************************

    1942

***************************************************************************/
#ifndef MAME_INCLUDES_1942_H
#define MAME_INCLUDES_1942_H

#pragma once

#include "machine/gen_latch.h"
#include "machine/timer.h"
#include "emupal.h"

class _1942_state : public driver_device
{
public:
	_1942_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_spriteram(*this, "spriteram")
		, m_fg_videoram(*this, "fg_videoram")
		, m_bg_videoram(*this, "bg_videoram")
		, m_audiocpu(*this, "audiocpu")
		, m_maincpu(*this, "maincpu")
		, m_gfxdecode(*this, "gfxdecode")
		, m_palette(*this, "palette")
		, m_soundlatch(*this, "soundlatch")
	{ }

	void driver_init() override;

	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);

	void _1942(machine_config &config);

protected:
	void machine_start() override;
	void machine_reset() override;
	void video_start() override;

	void _1942_map(address_map &map);
	void sound_map(address_map &map);

	DECLARE_WRITE8_MEMBER(_1942_bankswitch_w);
	DECLARE_WRITE8_MEMBER(_1942_fgvideoram_w);
	DECLARE_WRITE8_MEMBER(_1942_bgvideoram_w);
	DECLARE_WRITE8_MEMBER(_1942_palette_bank_w);
	DECLARE_WRITE8_MEMBER(_1942_scroll_w);
	DECLARE_WRITE8_MEMBER(_1942_c804_w);
	void _1942_palette(palette_device &palette) const;
	TIMER_DEVICE_CALLBACK_MEMBER(_1942_scanline);
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	virtual void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);

	/* memory pointers */
	required_shared_ptr<uint8_t> m_spriteram;
	required_shared_ptr<uint8_t> m_fg_videoram;
	required_shared_ptr<uint8_t> m_bg_videoram;

	required_device<cpu_device> m_audiocpu;
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<generic_latch_8_device> m_soundlatch;

	/* video-related */
	tilemap_t *m_fg_tilemap;
	tilemap_t *m_bg_tilemap;
	int m_palette_bank;
	uint8_t m_scroll[2];
	void create_palette(palette_device &palette) const;
};

class _1942p_state : public _1942_state
{
public:
	_1942p_state(const machine_config &mconfig, device_type type, const char *tag)
		: _1942_state(mconfig, type, tag)
		, m_protopal(*this, "protopal")
	{ }

	void _1942p(machine_config &config);

protected:
	void video_start() override;

	void _1942p_map(address_map &map);
	void _1942p_sound_io(address_map &map);
	void _1942p_sound_map(address_map &map);

	DECLARE_WRITE8_MEMBER(_1942p_f600_w);
	DECLARE_WRITE8_MEMBER(_1942p_palette_w);

	void _1942p_palette(palette_device &palette) const;

	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect) override;

	required_shared_ptr<uint8_t> m_protopal;
};

#endif // MAME_INCLUDES_1942_H
