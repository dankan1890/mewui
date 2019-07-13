// license:BSD-3-Clause
// copyright-holders:David Haywood, Nicola Salmoria
#ifndef MAME_INCLUDES_USGAMES_H
#define MAME_INCLUDES_USGAMES_H

#pragma once

#include "emupal.h"
#include "video/mc6845.h"

class usgames_state : public driver_device
{
public:
	usgames_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_videoram(*this, "videoram"),
		m_charram(*this, "charram"),
		m_leds(*this, "led%u", 0U),
		m_palette(*this, "palette")
	{ }

	void usg32(machine_config &config);
	void usg185(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void video_start() override;

private:
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;

	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_charram;

	output_finder<5> m_leds;
	required_device<palette_device> m_palette;

	DECLARE_WRITE8_MEMBER(rombank_w);
	DECLARE_WRITE8_MEMBER(lamps1_w);
	DECLARE_WRITE8_MEMBER(lamps2_w);
	DECLARE_WRITE8_MEMBER(charram_w);

	void usgames_palette(palette_device &palette) const;

	void usg185_map(address_map &map);
	void usgames_map(address_map &map);
	MC6845_UPDATE_ROW(update_row);
};

#endif // MAME_INCLUDES_USGAMES_H
