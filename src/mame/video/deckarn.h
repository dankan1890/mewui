// license:BSD-3-Clause
// copyright-holders:Bryan McPhail,David Haywood
#ifndef MAME_VIDEO_DECKARN_H
#define MAME_VIDEO_DECKARN_H

#pragma once

#include "screen.h"

typedef device_delegate<void (u32 &colour, u32 &pri_mask)> deckarn_colpri_cb_delegate;

class deco_karnovsprites_device : public device_t
{
public:
	deco_karnovsprites_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);
	void draw_sprites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, gfx_element *gfx, u16* spriteram, int size);
	void set_flip_screen(bool flip) { m_flip_screen = flip; }

	template <typename... T> void set_colpri_callback(T &&... args) { m_colpri_cb = deckarn_colpri_cb_delegate(std::forward<T>(args)...); }

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	deckarn_colpri_cb_delegate m_colpri_cb;
	bool m_flip_screen;
};

DECLARE_DEVICE_TYPE(DECO_KARNOVSPRITES, deco_karnovsprites_device)

#endif // MAME_VIDEO_DECKARN_H
