// license:BSD-3-Clause
// copyright-holders:Pierpaolo Prazzoli, Luca Elia
#ifndef MAME_VIDEO_IGS017_IGS031_H
#define MAME_VIDEO_IGS017_IGS031_H

#pragma once

#include "machine/i8255.h"
#include "emupal.h"

typedef device_delegate<u16 (u16)> igs017_igs031_palette_scramble_delegate;

class igs017_igs031_device : public device_t,
							public device_gfx_interface,
							public device_video_interface,
							public device_memory_interface
{
public:
	igs017_igs031_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	template <typename T> void set_i8255_tag(T &&tag) { m_i8255.set_tag(std::forward<T>(tag)); }
	template <typename... T> void set_palette_scramble_cb(T &&... args) { m_palette_scramble_cb = igs017_igs031_palette_scramble_delegate(std::forward<T>(args)...); }

	void set_text_reverse_bits()
	{
		m_revbits = true;
	}

	u16 palette_callback_straight(u16 bgr) const;

	igs017_igs031_palette_scramble_delegate m_palette_scramble_cb;

	void map(address_map &map);

	u8 read(offs_t offset);
	void write(offs_t offset, u8 data);

	bool get_nmi_enable() { return m_nmi_enable; }
	bool get_irq_enable() { return m_irq_enable; }

	void palram_w(offs_t offset, u8 data);
	u8 i8255_r(offs_t offset);

	void video_disable_w(u8 data);

	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);

	void fg_w(offs_t offset, u8 data);
	void bg_w(offs_t offset, u8 data);

	void expand_sprites();
	void draw_sprite(bitmap_ind16 &bitmap, const rectangle &cliprect, int offsx, int offsy, int dimx, int dimy, int flipx, int flipy, u32 color, u32 addr);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	int debug_viewer(bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void nmi_enable_w(u8 data);
	void irq_enable_w(u8 data);
	virtual void video_start();

protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual space_config_vector memory_space_config() const override;

	DECLARE_GFXDECODE_MEMBER(gfxinfo);

private:
	address_space_config        m_space_config;

	required_shared_ptr<u8> m_spriteram;
	required_shared_ptr<u8> m_fg_videoram;
	required_shared_ptr<u8> m_bg_videoram;
	required_shared_ptr<u8> m_palram;
	optional_device<i8255_device> m_i8255;
	required_device<palette_device> m_palette;

	// the gfx roms were often hooked up with the bits backwards, allow us to handle it here to save doing it in every driver.
	bool m_revbits;

	u8 m_toggle;
	int m_debug_addr;
	int m_debug_width;
	bool m_video_disable;
	tilemap_t *m_fg_tilemap;
	tilemap_t *m_bg_tilemap;
	std::unique_ptr<u8[]> m_sprites_gfx;
	u32 m_sprites_gfx_size;

	bool m_nmi_enable;
	bool m_irq_enable;
};

DECLARE_DEVICE_TYPE(IGS017_IGS031, igs017_igs031_device)

#endif // MAME_VIDEO_IGS017_IGS031_H
