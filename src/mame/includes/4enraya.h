// license:BSD-3-Clause
// copyright-holders:Tomasz Slanina, Roberto Fresca
/*************************************************************************

    IDSA 4 En Raya

*************************************************************************/

#ifndef MAME_INCLUDES_4ENRAYA_H
#define MAME_INCLUDES_4ENRAYA_H

#pragma once

#include "sound/ay8910.h"
#include "emupal.h"

class _4enraya_state : public driver_device
{
public:
	_4enraya_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_ay(*this, "aysnd")
		, m_gfxdecode(*this, "gfxdecode")
		, m_palette(*this, "palette")
		, m_prom(*this, "pal_prom")
		, m_rom(*this, "maincpu")
	{
	}

	void _4enraya(machine_config &config);

	DECLARE_WRITE8_MEMBER(fenraya_videoram_w);

protected:
	DECLARE_WRITE8_MEMBER(sound_data_w);
	DECLARE_READ8_MEMBER(fenraya_custom_map_r);
	DECLARE_WRITE8_MEMBER(fenraya_custom_map_w);
	DECLARE_WRITE8_MEMBER(sound_control_w);
	TILE_GET_INFO_MEMBER(get_tile_info);
	uint32_t screen_update_4enraya(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void main_map(address_map &map);
	void main_portmap(address_map &map);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

	required_device<cpu_device> m_maincpu;
	required_device<ay8910_device> m_ay;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;

	/* memory pointers */
	uint8_t m_videoram[0x1000];
	uint8_t m_workram[0x1000];

	optional_region_ptr<uint8_t> m_prom;
	optional_region_ptr<uint8_t> m_rom;

	/* video-related */
	tilemap_t *m_bg_tilemap;

	/* sound-related */
	uint8_t m_soundlatch;
};

class unk_gambl_state : public _4enraya_state
{
public:
	unk_gambl_state(const machine_config &mconfig, device_type type, const char *tag)
		: _4enraya_state(mconfig, type, tag)
	{
	}

	void unkpacg(machine_config &config);

private:
	void unkpacg_main_map(address_map &map);
	void unkpacg_main_portmap(address_map &map);

	void driver_init() override;
};

#endif // MAME_INCLUDES_4ENRAYA_H
