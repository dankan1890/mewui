// license:BSD-3-Clause
// copyright-holders:Quench, David Haywood
/* GP9001 Video Controller */
#ifndef MAME_VIDEO_GP9001_H
#define MAME_VIDEO_GP9001_H

#pragma once

#include "video/bufsprite.h"

class gp9001vdp_device : public device_t,
							public device_gfx_interface,
							public device_video_interface,
							public device_memory_interface
{
	enum
	{
		TIMER_RAISE_IRQ
	};

public:
	gp9001vdp_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	typedef device_delegate<void (u8 layer, u32 &code)> gp9001_cb_delegate;
	void set_tile_callback(gp9001_cb_delegate cb) { m_gp9001_cb = cb; }

	auto vint_out_cb() { return m_vint_out_cb.bind(); }

	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, const u8* primap);
	void draw_custom_tilemap(bitmap_ind16 &bitmap, const rectangle &cliprect, int layer, const u8* priremap, const u8* pri_enable);
	void render_vdp(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void screen_eof();
	void create_tilemaps();
	void init_scroll_regs();

	bitmap_ind8 *custom_priority_bitmap;

	void map(address_map &map);

	// game-specific hack stuff
	void disable_sprite_buffer() { m_sp.use_sprite_buffer = 0; }
	void set_tm_extra_offsets(int layer, int xn, int yn, int xf, int yf) { m_tm[layer].set_extra_offsets(xn, yn, xf, yf); }
	void set_sp_extra_offsets(int xn, int yn, int xf, int yf) { m_sp.set_extra_offsets(xn, yn, xf, yf); }

	// ROM banking control
	void set_dirty() { m_gfxrom_bank_dirty = true; }

	// access to VDP
	u16 read(offs_t offset, u16 mem_mask = ~0);
	void write(offs_t offset, u16 data, u16 mem_mask = ~0);

	DECLARE_READ_LINE_MEMBER(hsync_r);
	DECLARE_READ_LINE_MEMBER(vsync_r);
	DECLARE_READ_LINE_MEMBER(fblank_r);

	// this bootleg has strange access
	u16 pipibibi_bootleg_videoram16_r(offs_t offset);
	void pipibibi_bootleg_videoram16_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	u16 pipibibi_bootleg_spriteram16_r(offs_t offset);
	void pipibibi_bootleg_spriteram16_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void pipibibi_bootleg_scroll_w(offs_t offset, u16 data, u16 mem_mask = ~0);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	virtual space_config_vector memory_space_config() const override;

	address_space_config        m_space_config;

	template<int Layer> TILE_GET_INFO_MEMBER(get_tile_info);

private:
	// internal handlers
	template<int Layer> void tmap_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	static constexpr unsigned BG_VRAM_SIZE   = 0x1000;   /* Background RAM size */
	static constexpr unsigned FG_VRAM_SIZE   = 0x1000;   /* Foreground RAM size */
	static constexpr unsigned TOP_VRAM_SIZE  = 0x1000;   /* Top Layer  RAM size */
	static constexpr unsigned SPRITERAM_SIZE = 0x0800;   /* Sprite     RAM size */
	static constexpr unsigned SPRITE_FLIPX   = 0x1000;   /* Sprite flip flags */
	static constexpr unsigned SPRITE_FLIPY   = 0x2000;

	struct gp9001layeroffsets
	{
		int normal;
		int flipped;
	};

	struct gp9001layer
	{
		void set_extra_offsets(int xn, int yn, int xf, int yf)
		{
			extra_xoffset.normal = xn;
			extra_yoffset.normal = yn;
			extra_xoffset.flipped = xf;
			extra_yoffset.flipped = yf;
		}

		u16 flip;
		u16 scrollx;
		u16 scrolly;

		gp9001layeroffsets extra_xoffset;
		gp9001layeroffsets extra_yoffset;
	};

	struct tilemaplayer : gp9001layer
	{
		void set_scrollx_and_flip_reg(u16 data, u16 mem_mask, bool f);
		void set_scrolly_and_flip_reg(u16 data, u16 mem_mask, bool f);

		tilemap_t *tmap;
	};

	struct spritelayer : gp9001layer
	{
		void set_scrollx_and_flip_reg(u16 data, u16 mem_mask, bool f);
		void set_scrolly_and_flip_reg(u16 data, u16 mem_mask, bool f);

		bool use_sprite_buffer;
	};

	DECLARE_GFXDECODE_MEMBER(gfxinfo);

	void voffs_w(u16 data, u16 mem_mask = ~0);
	int videoram16_r(void);
	void videoram16_w(u16 data, u16 mem_mask = ~0);
	u16 vdpstatus_r(void);
	void scroll_reg_select_w(u16 data, u16 mem_mask = ~0);
	void scroll_reg_data_w(u16 data, u16 mem_mask = ~0);

	u16 m_voffs;
	u16 m_scroll_reg;

	tilemaplayer m_tm[3];
	spritelayer m_sp;

	// technically this is just rom banking, allowing the chip to see more graphic ROM, however it's easier to handle it
	// in the chip implementation than externally for now (which would require dynamic decoding of the entire charsets every
	// time the bank was changed)
	bool m_gfxrom_bank_dirty;       /* dirty flag of object bank (for Batrider) */

	required_shared_ptr_array<u16, 3> m_vram;
	required_device<buffered_spriteram16_device> m_spriteram;

	gp9001_cb_delegate m_gp9001_cb;
	devcb_write_line m_vint_out_cb;
	emu_timer *m_raise_irq_timer;
};

DECLARE_DEVICE_TYPE(GP9001_VDP, gp9001vdp_device)

#endif // MAME_VIDEO_GP9001_H
