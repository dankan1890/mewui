// license:BSD-3-Clause
// copyright-holders:David Haywood, Peter Wilhelmsen, Kevtris

#ifndef MAME_VIDEO_GAMATE_H
#define MAME_VIDEO_GAMATE_H

#pragma once

#include "emupal.h"

DECLARE_DEVICE_TYPE(GAMATE_VIDEO, gamate_video_device)

class gamate_video_device : public device_t,
	public device_memory_interface
{
public:
	// construction/destruction
	gamate_video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_WRITE8_MEMBER(lcdcon_w);
	DECLARE_WRITE8_MEMBER(xscroll_w);
	DECLARE_WRITE8_MEMBER(yscroll_w);
	DECLARE_WRITE8_MEMBER(xpos_w);
	DECLARE_WRITE8_MEMBER(ypos_w);
	DECLARE_READ8_MEMBER(vram_r);
	DECLARE_WRITE8_MEMBER(vram_w);

	void regs_map(address_map &map);
	void vram_map(address_map &map);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual space_config_vector memory_space_config() const override;

	address_space *m_vramspace;

private:
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void gamate_palette(palette_device &palette) const;
	const address_space_config m_vram_space_config;
	required_shared_ptr<uint8_t> m_vram;

	void set_vram_addr_lower_5bits(uint8_t data);
	void set_vram_addr_upper_8bits(uint8_t data);
	void increment_vram_address();

	void get_real_x_and_y(int &ret_x, int &ret_y, int scanline);
	int get_pixel_from_vram(int x, int y);

	int m_vramaddress;
	int m_bitplaneselect;
	int m_scrollx;
	int m_scrolly;
	int m_window;
	int m_swapplanes;
	int m_incrementdir;
	int m_displayblank;
};

#endif // MAME_VIDEO_GAMATE_H
