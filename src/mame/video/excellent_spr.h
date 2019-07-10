// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_VIDEO_EXCELLENT_SPR_H
#define MAME_VIDEO_EXCELLENT_SPR_H

#pragma once

typedef device_delegate<void (u32 &colour, u32 &pri_mask)> excellent_spr_colpri_cb_delegate;

class excellent_spr_device : public device_t, public device_gfx_interface, public device_video_interface
{
public:
	excellent_spr_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_color_base(u16 base) { m_colbase = base; }
	template <typename... T> void set_colpri_callback(T &&... args) { m_colpri_cb = excellent_spr_colpri_cb_delegate(std::forward<T>(args)...); }

	u8 read(offs_t offset);
	void write(offs_t offset, u8 data);

	void aquarium_draw_sprites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int y_offs);
	void gcpinbal_draw_sprites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int y_offs);

protected:
	std::unique_ptr<u8[]> m_ram;

	virtual void device_start() override;
	virtual void device_reset() override;

private:
	excellent_spr_colpri_cb_delegate m_colpri_cb;
	required_memory_region m_gfx_region;
	u16 m_colbase;
};

DECLARE_DEVICE_TYPE(EXCELLENT_SPRITE, excellent_spr_device)

#endif // MAME_VIDEO_EXCELLENT_SPR_H
