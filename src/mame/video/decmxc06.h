// license:BSD-3-Clause
// copyright-holders:Bryan McPhail, David Haywood
#ifndef MAME_VIDEO_DECMXC06_H
#define MAME_VIDEO_DECMXC06_H

#pragma once

#include "screen.h"

typedef device_delegate<void (u32 &colour, u32 &pri_mask)> decmxc06_colpri_cb_delegate;

class deco_mxc06_device : public device_t, public device_video_interface
{
public:
	deco_mxc06_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration
	template <typename... T> void set_colpri_callback(T &&... args) { m_colpri_cb = decmxc06_colpri_cb_delegate(std::forward<T>(args)...); }

	void draw_sprites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, gfx_element *gfx, u16* spriteram, int size);
	void draw_sprites_bootleg(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, gfx_element *gfx, u16* spriteram, int size);
	void set_flip_screen(bool flip) { m_flip_screen = flip; }

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	struct sprite_t
	{
		int height;
		u32 code[8], colour;
		int x[8], y[8];
		bool flipx, flipy;
		u32 pri_mask;
	};
	decmxc06_colpri_cb_delegate m_colpri_cb;
	bool m_flip_screen;
	std::unique_ptr<struct sprite_t[]> m_spritelist;
};

DECLARE_DEVICE_TYPE(DECO_MXC06, deco_mxc06_device)

#endif // MAME_VIDEO_DECMXC06_H
