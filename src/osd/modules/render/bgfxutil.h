// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
#ifndef RENDER_BGFX_UTIL
#define RENDER_BGFX_UTIL

#pragma once

#include <bgfx/bgfx.h>

/* sdl_info is the information about SDL for the current screen */
class bgfx_util
{
public:
	static const bgfx::Memory* mame_texture_data_to_bgfx_texture_data(uint32_t format, int width, int height, int rowpixels, const rgb_t *palette, void *base);
	static uint64_t get_blend_state(uint32_t blend);
};

#endif // RENDER_BGFX_UTIL
