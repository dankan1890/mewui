// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Atari "Round" hardware

*************************************************************************/
#ifndef MAME_INCLUDES_RELIEF_H
#define MAME_INCLUDES_RELIEF_H

#pragma once

#include "sound/okim6295.h"
#include "sound/ym2413.h"
#include "video/atarimo.h"
#include "video/atarivad.h"
#include "screen.h"

class relief_state : public driver_device
{
public:
	relief_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_vad(*this, "vad"),
		m_oki(*this, "oki"),
		m_ym2413(*this, "ymsnd"),
		m_okibank(*this, "okibank")
	{ }

	void relief(machine_config &config);

	void init_relief();

private:
	virtual void machine_reset() override;
	virtual void video_start() override;
	DECLARE_READ16_MEMBER(special_port2_r);
	DECLARE_WRITE16_MEMBER(audio_control_w);
	DECLARE_WRITE16_MEMBER(audio_volume_w);
	TILE_GET_INFO_MEMBER(get_playfield_tile_info);
	TILE_GET_INFO_MEMBER(get_playfield2_tile_info);
	uint32_t screen_update_relief(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void main_map(address_map &map);
	void oki_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<atari_vad_device> m_vad;
	required_device<okim6295_device> m_oki;
	required_device<ym2413_device> m_ym2413;
	required_memory_bank m_okibank;

	uint8_t           m_ym2413_volume;
	uint8_t           m_overall_volume;
	uint8_t           m_adpcm_bank;

	static const atari_motion_objects_config s_mob_config;
};

#endif // MAME_INCLUDES_RELIEF_H
