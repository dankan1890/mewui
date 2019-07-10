// license:BSD-3-Clause
// copyright-holders:Luca Elia, David Haywood
/***************************************************************************

    (legacy metro.cpp, currently contains Blazing Tornado overrides,
     to be moved into its own driver file!)

***************************************************************************/

#include "emu.h"
#include "includes/metro.h"

TILE_GET_INFO_MEMBER(metro_state::k053936_get_tile_info)
{
	int code = m_k053936_ram[tile_index];

	SET_TILE_INFO_MEMBER(0,
			code & 0x7fff,
			0xe,
			0);
}

TILE_GET_INFO_MEMBER(metro_state::k053936_gstrik2_get_tile_info)
{
	int code = m_k053936_ram[tile_index];

	SET_TILE_INFO_MEMBER(0,
			(code & 0x7fff)>>2,
			0xe,
			0);
}

WRITE16_MEMBER(metro_state::k053936_w)
{
	COMBINE_DATA(&m_k053936_ram[offset]);
	m_k053936_tilemap->mark_tile_dirty(offset);
}

TILEMAP_MAPPER_MEMBER(metro_state::tilemap_scan_gstrik2)
{
	/* logical (col,row) -> memory offset */
	return ((row & 0x40) >> 6) | (col << 1) | ((row & 0x80) << 1) | ((row & 0x3f) << 9);
}

VIDEO_START_MEMBER(metro_state,blzntrnd)
{
	m_k053936_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(metro_state::k053936_get_tile_info),this), TILEMAP_SCAN_ROWS, 8, 8, 256, 512);

	m_screen->register_screen_bitmap(m_vdp_bitmap);
}

VIDEO_START_MEMBER(metro_state,gstrik2)
{
	m_k053936_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(metro_state::k053936_gstrik2_get_tile_info),this), tilemap_mapper_delegate(FUNC(metro_state::tilemap_scan_gstrik2),this), 16, 16, 128, 256);

	m_screen->register_screen_bitmap(m_vdp_bitmap);
}

uint32_t metro_state::screen_update_psac_vdp2_mix(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	/* TODO: bit 5 of reg 7 is off when ROZ is supposed to be disabled
	 * (Blazing Tornado title screen/character select/ending and Grand Striker 2 title/how to play transition)
	 */

	bitmap.fill(m_vdp2->get_background_pen(), cliprect);
	m_k053936->zoom_draw(screen, bitmap, cliprect, m_k053936_tilemap, 0, 0, 1);
	m_vdp2->screen_update(screen, m_vdp_bitmap, cliprect);
	copybitmap_trans(bitmap, m_vdp_bitmap, 0, 0, 0, 0, cliprect, m_vdp2->get_background_pen());

	return 0;
}
