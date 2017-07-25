// license:BSD-3-Clause
// copyright-holders:Aaron Giles
#include "screen.h"

class tp84_state : public driver_device
{
public:
	tp84_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "cpu1"),
		m_audiocpu(*this, "audiocpu"),
		m_palette_bank(*this, "palette_bank"),
		m_scroll_x(*this, "scroll_x"),
		m_scroll_y(*this, "scroll_y"),
		m_bg_videoram(*this, "bg_videoram"),
		m_fg_videoram(*this, "fg_videoram"),
		m_bg_colorram(*this, "bg_colorram"),
		m_fg_colorram(*this, "fg_colorram"),
		m_spriteram(*this, "spriteram"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette") { }

	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_shared_ptr<uint8_t> m_palette_bank;
	required_shared_ptr<uint8_t> m_scroll_x;
	required_shared_ptr<uint8_t> m_scroll_y;
	required_shared_ptr<uint8_t> m_bg_videoram;
	required_shared_ptr<uint8_t> m_fg_videoram;
	required_shared_ptr<uint8_t> m_bg_colorram;
	required_shared_ptr<uint8_t> m_fg_colorram;
	required_shared_ptr<uint8_t> m_spriteram;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	tilemap_t *m_bg_tilemap;
	tilemap_t *m_fg_tilemap;
	bool m_flipscreen_x;
	bool m_flipscreen_y;

	bool m_irq_enable;
	bool m_sub_irq_mask;

	DECLARE_WRITE_LINE_MEMBER(irq_enable_w);
	DECLARE_WRITE_LINE_MEMBER(coin_counter_1_w);
	DECLARE_WRITE_LINE_MEMBER(coin_counter_2_w);
	DECLARE_WRITE_LINE_MEMBER(flip_screen_x_w);
	DECLARE_WRITE_LINE_MEMBER(flip_screen_y_w);
	DECLARE_READ8_MEMBER(tp84_sh_timer_r);
	DECLARE_WRITE8_MEMBER(tp84_filter_w);
	DECLARE_WRITE8_MEMBER(tp84_sh_irqtrigger_w);
	DECLARE_WRITE8_MEMBER(sub_irq_mask_w);
	DECLARE_WRITE8_MEMBER(tp84_spriteram_w);
	DECLARE_READ8_MEMBER(tp84_scanline_r);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	virtual void machine_start() override;
	virtual void video_start() override;
	DECLARE_PALETTE_INIT(tp84);
	uint32_t screen_update_tp84(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(main_vblank_irq);
	INTERRUPT_GEN_MEMBER(sub_vblank_irq);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
};
