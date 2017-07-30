// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
#include "machine/namcoio.h"
#include "sound/dac.h"
#include "sound/namco.h"
#include "screen.h"

class mappy_state : public driver_device
{
public:
	enum
	{
		TIMER_IO_RUN
	};

	mappy_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_spriteram(*this, "spriteram"),
		m_maincpu(*this, "maincpu"),
		m_subcpu(*this, "sub"),
		m_subcpu2(*this, "sub2"),
		m_namcoio(*this, "namcoio_%u", 1),
		m_namco_15xx(*this, "namco"),
		m_dac(*this, "dac"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette")  { }

	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_spriteram;

	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_subcpu;
	optional_device<cpu_device> m_subcpu2;
	required_device_array<namcoio_device, 2> m_namcoio;
	required_device<namco_15xx_device> m_namco_15xx;
	optional_device<dac_byte_interface> m_dac;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

	tilemap_t *m_bg_tilemap;
	bitmap_ind16 m_sprite_bitmap;

	uint8_t m_scroll;

	uint8_t m_main_irq_mask;
	uint8_t m_sub_irq_mask;
	uint8_t m_sub2_irq_mask;

	DECLARE_WRITE_LINE_MEMBER(int_on_w);
	DECLARE_WRITE_LINE_MEMBER(int_on_2_w);
	DECLARE_WRITE_LINE_MEMBER(int_on_3_w);
	DECLARE_WRITE_LINE_MEMBER(mappy_flip_w);
	DECLARE_WRITE8_MEMBER(superpac_videoram_w);
	DECLARE_WRITE8_MEMBER(mappy_videoram_w);
	DECLARE_WRITE8_MEMBER(superpac_flipscreen_w);
	DECLARE_READ8_MEMBER(superpac_flipscreen_r);
	DECLARE_WRITE8_MEMBER(mappy_scroll_w);
	DECLARE_WRITE8_MEMBER(out_lamps);
	TILEMAP_MAPPER_MEMBER(superpac_tilemap_scan);
	TILEMAP_MAPPER_MEMBER(mappy_tilemap_scan);
	TILE_GET_INFO_MEMBER(superpac_get_tile_info);
	TILE_GET_INFO_MEMBER(phozon_get_tile_info);
	TILE_GET_INFO_MEMBER(mappy_get_tile_info);
	virtual void machine_start() override;
	DECLARE_VIDEO_START(superpac);
	DECLARE_PALETTE_INIT(superpac);
	DECLARE_VIDEO_START(phozon);
	DECLARE_PALETTE_INIT(phozon);
	DECLARE_VIDEO_START(mappy);
	DECLARE_PALETTE_INIT(mappy);
	uint32_t screen_update_superpac(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_phozon(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_mappy(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(main_vblank_irq);
	INTERRUPT_GEN_MEMBER(sub_vblank_irq);
	INTERRUPT_GEN_MEMBER(sub2_vblank_irq);
	DECLARE_DRIVER_INIT(grobda);
	DECLARE_DRIVER_INIT(digdug2);
	void mappy_draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, uint8_t *spriteram_base);
	void phozon_draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, uint8_t *spriteram_base);

protected:
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
};
