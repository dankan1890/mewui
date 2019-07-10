// license:BSD-3-Clause
// copyright-holders:Luca Elia
/***************************************************************************

                              -= Tetris Plus 2 =-

                    driver by   Luca Elia (l.elia@tin.it)


    [ 2 Scrolling Layers ]

    The Background is a 64 x 64 Tilemap with 16 x 16 x 8 tiles (1024 x 1024).
    The Foreground is a 64 x 64 Tilemap with 8 x 8 x 8 tiles (512 x 512).
    Each tile needs 4 bytes.

    [ 1024? Sprites ]

    Sprites are "nearly" tile based: if the data in the ROMs is decoded
    as 8x8 tiles, and each 32 consecutive tiles are drawn like a column
    of 256 pixels, it turns out that every 32 x 32 tiles form a picture
    of 256 x 256 pixels (a "page").

    Sprites are portions of any size from those page's graphics rendered
    to screen.

To Do:

-   There is a 3rd unimplemented layer capable of rotation (not used by
    the game, can be tested in service mode).
-   Priority RAM is not properly taken into account.
-   Can the Tetris Plus 2 sprites zoom, or is this an MS32 only feature?

***************************************************************************/

#include "emu.h"
#include "includes/tetrisp2.h"
#include "machine/jalcrpt.h"
#include "screen.h"


/***************************************************************************


                                    Palette


***************************************************************************/

/* BBBBBGGGGGRRRRRx xxxxxxxxxxxxxxxx */
WRITE16_MEMBER(tetrisp2_state::tetrisp2_palette_w)
{
	data = COMBINE_DATA(&m_paletteram[offset]);
	if ((offset & 1) == 0)
		m_palette->set_pen_color(offset/2,pal5bit(data >> 1),pal5bit(data >> 6),pal5bit(data >> 11));
}

WRITE16_MEMBER(tetrisp2_state::rocknms_sub_palette_w)
{
	data = COMBINE_DATA(&m_sub_paletteram[offset]);
	if ((offset & 1) == 0)
		m_sub_palette->set_pen_color(offset/2,pal5bit(data >> 1),pal5bit(data >> 6),pal5bit(data >> 11));
}



/***************************************************************************


                                    Priority


***************************************************************************/

WRITE16_MEMBER(tetrisp2_state::tetrisp2_priority_w)
{
	if (ACCESSING_BITS_0_7)
		m_priority[offset] = data;
	else
		m_priority[offset] = data >> 8;
}

READ16_MEMBER(tetrisp2_state::tetrisp2_priority_r)
{
	return m_priority[offset] | 0xff00;
}

WRITE16_MEMBER(tetrisp2_state::rocknms_sub_priority_w)
{
	if (ACCESSING_BITS_0_7)
		m_rocknms_sub_priority[offset] = data;
	else
		m_rocknms_sub_priority[offset] = data >> 8;
}


/***************************************************************************


                                    Tilemaps


    Offset:     Bits:                   Value:

        0.w                             Code
        2.w     fedc ba98 7654 ----
                ---- ---- ---- 3210     Color


***************************************************************************/


#define NX_0  (0x40)
#define NY_0  (0x40)

TILE_GET_INFO_MEMBER(tetrisp2_state::get_tile_info_bg)
{
	uint16_t code_hi = m_vram_bg[ 2 * tile_index + 0];
	uint16_t code_lo = m_vram_bg[ 2 * tile_index + 1];
	SET_TILE_INFO_MEMBER(1,
			code_hi,
			code_lo & 0xf,
			0);
}

WRITE16_MEMBER(tetrisp2_state::tetrisp2_vram_bg_w)
{
	COMBINE_DATA(&m_vram_bg[offset]);
	m_tilemap_bg->mark_tile_dirty(offset/2);
}


#define NX_1  (0x40)
#define NY_1  (0x40)

TILE_GET_INFO_MEMBER(tetrisp2_state::get_tile_info_fg)
{
	uint16_t code_hi = m_vram_fg[ 2 * tile_index + 0];
	uint16_t code_lo = m_vram_fg[ 2 * tile_index + 1];
	SET_TILE_INFO_MEMBER(3,
			code_hi,
			code_lo & 0xf,
			0);
}

WRITE16_MEMBER(tetrisp2_state::tetrisp2_vram_fg_w)
{
	COMBINE_DATA(&m_vram_fg[offset]);
	m_tilemap_fg->mark_tile_dirty(offset/2);
}


TILE_GET_INFO_MEMBER(tetrisp2_state::get_tile_info_rot)
{
	uint16_t code_hi = m_vram_rot[ 2 * tile_index + 0];
	uint16_t code_lo = m_vram_rot[ 2 * tile_index + 1];
	SET_TILE_INFO_MEMBER(2,
			code_hi,
			code_lo & 0xf,
			0);
}

WRITE16_MEMBER(tetrisp2_state::tetrisp2_vram_rot_w)
{
	COMBINE_DATA(&m_vram_rot[offset]);
	m_tilemap_rot->mark_tile_dirty(offset/2);
}

TILE_GET_INFO_MEMBER(tetrisp2_state::get_tile_info_rocknms_sub_bg)
{
	uint16_t code_hi = m_rocknms_sub_vram_bg[ 2 * tile_index + 0];
	uint16_t code_lo = m_rocknms_sub_vram_bg[ 2 * tile_index + 1];
	SET_TILE_INFO_MEMBER(1,
			code_hi,
			code_lo & 0xf,
			0);
}

WRITE16_MEMBER(tetrisp2_state::rocknms_sub_vram_bg_w)
{
	COMBINE_DATA(&m_rocknms_sub_vram_bg[offset]);
	m_tilemap_sub_bg->mark_tile_dirty(offset/2);
}


TILE_GET_INFO_MEMBER(tetrisp2_state::get_tile_info_rocknms_sub_fg)
{
	uint16_t code_hi = m_rocknms_sub_vram_fg[ 2 * tile_index + 0];
	uint16_t code_lo = m_rocknms_sub_vram_fg[ 2 * tile_index + 1];
	SET_TILE_INFO_MEMBER(3,
			code_hi,
			code_lo & 0xf,
			0);
}

WRITE16_MEMBER(tetrisp2_state::rocknms_sub_vram_fg_w)
{
	COMBINE_DATA(&m_rocknms_sub_vram_fg[offset]);
	m_tilemap_sub_fg->mark_tile_dirty(offset/2);
}


TILE_GET_INFO_MEMBER(tetrisp2_state::get_tile_info_rocknms_sub_rot)
{
	uint16_t code_hi = m_rocknms_sub_vram_rot[ 2 * tile_index + 0];
	uint16_t code_lo = m_rocknms_sub_vram_rot[ 2 * tile_index + 1];
	SET_TILE_INFO_MEMBER(2,
			code_hi,
			code_lo & 0xf,
			0);
}

WRITE16_MEMBER(tetrisp2_state::rocknms_sub_vram_rot_w)
{
	COMBINE_DATA(&m_rocknms_sub_vram_rot[offset]);
	m_tilemap_sub_rot->mark_tile_dirty(offset/2);
}



VIDEO_START_MEMBER(tetrisp2_state,tetrisp2)
{
	m_flipscreen_old = -1;

	m_tilemap_bg = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_bg),this),TILEMAP_SCAN_ROWS,16,16,NX_0,NY_0);
	m_tilemap_fg = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_fg),this),TILEMAP_SCAN_ROWS,8,8,NX_1,NY_1);
	m_tilemap_rot = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_rot),this),TILEMAP_SCAN_ROWS,16,16,NX_0*2,NY_0*2);
	m_tilemap_bg->set_transparent_pen(0);
	m_tilemap_fg->set_transparent_pen(0);
	m_tilemap_rot->set_transparent_pen(0);

	// should be smaller and mirrored like m32 I guess
	m_priority = std::make_unique<uint8_t[]>(0x40000);

	save_item(NAME(m_flipscreen_old));
	save_pointer(NAME(m_priority), 0x40000);
}

VIDEO_START_MEMBER(tetrisp2_state,nndmseal)
{
	VIDEO_START_CALL_MEMBER( tetrisp2 );
	m_tilemap_bg->set_scrolldx(-4,-4);
}

VIDEO_START_MEMBER(tetrisp2_state,rockntread)
{
	m_flipscreen_old = -1;

	m_tilemap_bg = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_bg),this),TILEMAP_SCAN_ROWS,16, 16, 256, 16);   // rockn ms(main),1,2,3,4
	m_tilemap_fg = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_fg),this),TILEMAP_SCAN_ROWS,8, 8, 64, 64);
	m_tilemap_rot = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_rot),this),TILEMAP_SCAN_ROWS,16, 16, 128, 128);

	m_tilemap_bg->set_transparent_pen(0);
	m_tilemap_fg->set_transparent_pen(0);
	m_tilemap_rot->set_transparent_pen(0);

	// should be smaller and mirrored like m32 I guess
	m_priority = std::make_unique<uint8_t[]>(0x40000);

	save_item(NAME(m_flipscreen_old));
	save_pointer(NAME(m_priority), 0x40000);
}


VIDEO_START_MEMBER(tetrisp2_state,rocknms)
{
	VIDEO_START_CALL_MEMBER( rockntread );

	m_tilemap_sub_bg = &machine().tilemap().create(*m_sub_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_rocknms_sub_bg),this),TILEMAP_SCAN_ROWS,16, 16, 32, 256);   // rockn ms(sub)
	m_tilemap_sub_fg = &machine().tilemap().create(*m_sub_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_rocknms_sub_fg),this),TILEMAP_SCAN_ROWS,8, 8, 64, 64);
	m_tilemap_sub_rot = &machine().tilemap().create(*m_sub_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_rocknms_sub_rot),this),TILEMAP_SCAN_ROWS,16, 16, 128, 128);

	m_tilemap_sub_bg->set_transparent_pen(0);
	m_tilemap_sub_fg->set_transparent_pen(0);
	m_tilemap_sub_rot->set_transparent_pen(0);
}


/***************************************************************************


                                Sprites Drawing

    Offset:     Bits:                   Meaning:

    0.w         fedc ba98 ---- ----
                ---- ---- 7654 ----     Priority
                ---- ---- ---- 3---
                ---- ---- ---- -2--     Draw this sprite
                ---- ---- ---- --1-     Flip Y
                ---- ---- ---- ---0     Flip X

    2.w         fedc ba98 ---- ----     Tile's Y position in the tile page (*)
                ---- ---- 7654 3210     Tile's X position in the tile page (*)

    4.w         fedc ---- ---- ----     Color
                ---- ba98 7654 3210     Tile Page (32x32 tiles = 256x256 pixels each)

    6.w         fedc ba98 ---- ----     Y Size - 1 (*)
                ---- ---- 7654 3210     X Size - 1 (*)

    8.w         fedc ba-- ---- ----
                ---- --98 7654 3210     Y (Signed)

    A.w         fedc b--- ---- ----
                ---- -a98 7654 3210     X (Signed)

    C.w         fedc ba98 7654 3210     ZOOM X (unused)

    E.w         fedc ba98 7654 3210     ZOOM Y (unused)

(*) 1 pixel granularity

***************************************************************************/

/* this is also used by ms32.cpp */
/* sprites should be able to create shadows too, how?
  -- it appears that sprites which should be shadows are often rendered *UNDER* the tilemaps, maybe related?
*/
template<class _BitmapClass>
static void tetrisp2_draw_sprites(_BitmapClass &bitmap, bitmap_ind8 &bitmap_pri, const rectangle &cliprect, uint8_t* priority_ram,
									uint16_t *sprram_top, size_t sprram_size, gfx_element *gfx, int flip, bool is_yuv    )
{
	int tx, ty, sx, sy, flipx, flipy;
	int xsize, ysize;
	int code, attr, color, size;
	int pri;
	int xzoom, yzoom;
	uint32_t primask;

	uint16_t  *source =   sprram_top;
	uint16_t  *finish =   sprram_top + (sprram_size - 0x10) / 2;

	for (; source<finish; source+=8)
	{
		attr    =   source[ 0 ];

		pri = (attr & 0x00f0);

		if ((attr & 0x0004) == 0)
			continue;

		flipx   =   attr & 1;
		flipy   =   attr & 2;

		code    =   source[ 1 ];
		color   =   source[ 2 ];

		tx      =   (code >> 0) & 0xff;
		ty      =   (code >> 8) & 0xff;

		code    =   (color & 0x0fff);
		// encoded to first word when YUV sprites are used
		if(is_yuv)
			color   =   (attr & 0x7f00) >> 8;
		else
			color   =   (color >> 12) & 0xf;
		size    =   source[ 3 ];

		xsize   =   ((size >> 0) & 0xff) + 1;
		ysize   =   ((size >> 8) & 0xff) + 1;

		sx      =   (source[5] & 0x3ff) - (source[5] & 0x400);
		sy      =   (source[4] & 0x1ff) - (source[4] & 0x200);

		xzoom   =   1 << 16;
		yzoom   =   1 << 16;

		if (xsize > 0x100 - tx)
			xsize = 0x100 - tx;

		if (ysize > 0x100 - ty)
			ysize = 0x100 - ty;

		gfx->set_source_clip(tx, xsize, ty, ysize);

		{
			primask = 0;
			if (priority_ram[(pri | 0x0a00 | 0x1500) / 2] & 0x38) primask |= 1 << 0;
			if (priority_ram[(pri | 0x0a00 | 0x1400) / 2] & 0x38) primask |= 1 << 1;
			if (priority_ram[(pri | 0x0a00 | 0x1100) / 2] & 0x38) primask |= 1 << 2;
			if (priority_ram[(pri | 0x0a00 | 0x1000) / 2] & 0x38) primask |= 1 << 3;
			if (priority_ram[(pri | 0x0a00 | 0x0500) / 2] & 0x38) primask |= 1 << 4;
			if (priority_ram[(pri | 0x0a00 | 0x0400) / 2] & 0x38) primask |= 1 << 5;
			if (priority_ram[(pri | 0x0a00 | 0x0100) / 2] & 0x38) primask |= 1 << 6;
			if (priority_ram[(pri | 0x0a00 | 0x0000) / 2] & 0x38) primask |= 1 << 7;


			gfx->prio_zoom_transpen(bitmap,cliprect,
					code,
					color,
					flipx, flipy,
					sx,sy,
					xzoom, yzoom, bitmap_pri,primask, 0);

		}


	}   /* end sprite loop */
}


/***************************************************************************


                                Screen Drawing


***************************************************************************/

uint32_t tetrisp2_state::screen_update_tetrisp2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int flipscreen;
	int asc_pri;
	int scr_pri;
	int rot_pri;
	int rot_ofsx, rot_ofsy;

	flipscreen = (m_systemregs[0x00] & 0x02);

	/* Black background color */
	bitmap.fill(0, cliprect);
	screen.priority().fill(0);

	/* Flip Screen */
	if (flipscreen != m_flipscreen_old)
	{
		m_flipscreen_old = flipscreen;
		machine().tilemap().set_flip_all(flipscreen ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0);
	}

	/* Flip Screen */
	if (flipscreen)
	{
		rot_ofsx = 0x053f;
		rot_ofsy = 0x04df;
	}
	else
	{
		rot_ofsx = 0x400;
		rot_ofsy = 0x400;
	}

	m_tilemap_bg->set_scrollx(0, (((m_scroll_bg[ 0 ] + 0x0014) + m_scroll_bg[ 2 ] ) & 0xffff));
	m_tilemap_bg->set_scrolly(0, (((m_scroll_bg[ 3 ] + 0x0000) + m_scroll_bg[ 5 ] ) & 0xffff));

	m_tilemap_fg->set_scrollx(0, m_scroll_fg[ 2 ]);
	m_tilemap_fg->set_scrolly(0, m_scroll_fg[ 5 ]);

	m_tilemap_rot->set_scrollx(0, (m_rotregs[ 0 ] - rot_ofsx));
	m_tilemap_rot->set_scrolly(0, (m_rotregs[ 2 ] - rot_ofsy));

	asc_pri = scr_pri = rot_pri = 0;

	if((m_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if((m_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if((m_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 0)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 0)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 1)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 1)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 1)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 2)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 2)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 2)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	tetrisp2_draw_sprites(bitmap, screen.priority(), cliprect, m_priority.get(),
							m_spriteram, m_spriteram.bytes(), m_gfxdecode->gfx(0), (m_systemregs[0x00] & 0x02), false    );
	return 0;
}

uint32_t tetrisp2_state::screen_update_rockntread(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int flipscreen;
	int asc_pri;
	int scr_pri;
	int rot_pri;
	int rot_ofsx, rot_ofsy;

	flipscreen = (m_systemregs[0x00] & 0x02);

	/* Black background color */
	bitmap.fill(0, cliprect);
	screen.priority().fill(0);

	/* Flip Screen */
	if (flipscreen != m_flipscreen_old)
	{
		m_flipscreen_old = flipscreen;
		machine().tilemap().set_flip_all(flipscreen ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0);
	}

	/* Flip Screen */
	if (flipscreen)
	{
		rot_ofsx = 0x053f;
		rot_ofsy = 0x04df;
	}
	else
	{
		rot_ofsx = 0x400;
		rot_ofsy = 0x400;
	}

	m_tilemap_bg->set_scrollx(0, (((m_scroll_bg[ 0 ] + 0x0014) + m_scroll_bg[ 2 ] ) & 0xffff));
	m_tilemap_bg->set_scrolly(0, (((m_scroll_bg[ 3 ] + 0x0000) + m_scroll_bg[ 5 ] ) & 0xffff));

	m_tilemap_fg->set_scrollx(0, m_scroll_fg[ 2 ]);
	m_tilemap_fg->set_scrolly(0, m_scroll_fg[ 5 ]);

	m_tilemap_rot->set_scrollx(0, (m_rotregs[ 0 ] - rot_ofsx));
	m_tilemap_rot->set_scrolly(0, (m_rotregs[ 2 ] - rot_ofsy));

	asc_pri = scr_pri = rot_pri = 0;

	if((m_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if((m_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if((m_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 0)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 0)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 1)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 1)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 1)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 2)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 2)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 2)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	tetrisp2_draw_sprites(bitmap, screen.priority(), cliprect, m_priority.get(),
							m_spriteram, m_spriteram.bytes(), m_gfxdecode->gfx(0), (m_systemregs[0x00] & 0x02), false    );
	return 0;
}




uint32_t tetrisp2_state::screen_update_rocknms_left(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	int asc_pri;
	int scr_pri;
	int rot_pri;

	m_tilemap_sub_bg->set_scrollx(0, m_rocknms_sub_scroll_bg[ 2 ] + 0x000);
	m_tilemap_sub_bg->set_scrolly(0, m_rocknms_sub_scroll_bg[ 5 ] + 0x000);
	m_tilemap_sub_fg->set_scrollx(0, m_rocknms_sub_scroll_fg[ 2 ] + 0x000);
	m_tilemap_sub_fg->set_scrolly(0, m_rocknms_sub_scroll_fg[ 5 ] + 0x000);
	m_tilemap_sub_rot->set_scrollx(0, m_rocknms_sub_rotregs[ 0 ] + 0x400);
	m_tilemap_sub_rot->set_scrolly(0, m_rocknms_sub_rotregs[ 2 ] + 0x400);

	bitmap.fill(m_palette->pen(0x0000), cliprect);
	screen.priority().fill(0, cliprect);

	asc_pri = scr_pri = rot_pri = 0;

	if((m_rocknms_sub_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if((m_rocknms_sub_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if((m_rocknms_sub_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		m_tilemap_sub_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 0)
		m_tilemap_sub_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 0)
		m_tilemap_sub_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 1)
		m_tilemap_sub_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 1)
		m_tilemap_sub_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 1)
		m_tilemap_sub_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 2)
		m_tilemap_sub_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 2)
		m_tilemap_sub_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 2)
		m_tilemap_sub_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	tetrisp2_draw_sprites(bitmap, screen.priority(), cliprect, m_priority.get(),
							m_spriteram2, m_spriteram2.bytes(), m_sub_gfxdecode->gfx(0), (m_systemregs[0x00] & 0x02), false  );

	return 0;
}

uint32_t tetrisp2_state::screen_update_rocknms_right(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	int asc_pri;
	int scr_pri;
	int rot_pri;

	m_tilemap_bg->set_scrollx(0, m_scroll_bg[ 2 ] + 0x000);
	m_tilemap_bg->set_scrolly(0, m_scroll_bg[ 5 ] + 0x000);
	m_tilemap_fg->set_scrollx(0, m_scroll_fg[ 2 ] + 0x000);
	m_tilemap_fg->set_scrolly(0, m_scroll_fg[ 5 ] + 0x000);
	m_tilemap_rot->set_scrollx(0, m_rotregs[ 0 ] + 0x400);
	m_tilemap_rot->set_scrolly(0, m_rotregs[ 2 ] + 0x400);

	/* Black background color */
	bitmap.fill(m_palette->pen(0x0000), cliprect);
	screen.priority().fill(0, cliprect);

	asc_pri = scr_pri = rot_pri = 0;

	if((m_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if((m_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if((m_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 0)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 0)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 1)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 1)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 1)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 2)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 2)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 2)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	tetrisp2_draw_sprites(bitmap, screen.priority(), cliprect, m_priority.get(),
							m_spriteram, m_spriteram.bytes(), m_gfxdecode->gfx(0), (m_systemregs[0x00] & 0x02), false    );

	return 0;
}

/***************************************************************************

                              Stepping Stage

***************************************************************************/

// Temporary hack for stpestag: unaltered ASCII bytes are written in the most significant byte
// of code_hi, one of the CPUs probably reads them and writes the actual tile codes somewhere.
TILE_GET_INFO_MEMBER(tetrisp2_state::stepstag_get_tile_info_fg)
{
	uint16_t const code_hi = m_vram_fg[ 2 * tile_index ] >> 8;
	uint16_t const code_lo = m_vram_fg[ 2 * tile_index ] & 0xf;
	//logerror("tile_idx[$%2x]=$%3x, palette=$%2x\n", tile_index, code_hi, code_lo);////
	if (m_vram_fg[2 * tile_index + 1] != 0)
		logerror("VRAM ASCII Haut-Mot Non-Zero!!!\n");/////////
	SET_TILE_INFO_MEMBER(3,
			code_hi,
			code_lo,
			0);
}

VIDEO_START_MEMBER(stepstag_state,stepstag)
{
	m_flipscreen_old = -1;

	m_tilemap_bg = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_bg),this),TILEMAP_SCAN_ROWS,16,16,NX_0,NY_0);
	m_tilemap_fg = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::stepstag_get_tile_info_fg),this),TILEMAP_SCAN_ROWS,8,8,NX_1,NY_1);
	m_tilemap_rot = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(tetrisp2_state::get_tile_info_rot),this),TILEMAP_SCAN_ROWS,16,16,NX_0*2,NY_0*2);
	m_tilemap_bg->set_transparent_pen(0);
	m_tilemap_fg->set_transparent_pen(0);
	m_tilemap_rot->set_transparent_pen(0);

	// should be smaller and mirrored like m32 I guess
	m_priority = std::make_unique<uint8_t[]>(0x40000);
}

uint32_t stepstag_state::screen_update_stepstag_left(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	screen.priority().fill(0);

	tetrisp2_draw_sprites(
			bitmap, screen.priority(), cliprect, m_priority.get(),
			m_spriteram1, m_spriteram1.bytes(), m_vj_gfxdecode_l->gfx(0), (m_systemregs[0x00] & 0x02), true);

	return 0;
}

uint32_t stepstag_state::screen_update_stepstag_mid(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	screen.priority().fill(0);

	tetrisp2_draw_sprites(
			bitmap, screen.priority(), cliprect, m_priority.get(),
			m_spriteram2, m_spriteram2.bytes(), m_vj_gfxdecode_m->gfx(0), (m_systemregs[0x00] & 0x02), true);

//  m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
//  m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	return 0;
}

uint32_t stepstag_state::screen_update_stepstag_right(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	// TODO: doesn't like pen 0?
	bitmap.fill(m_vj_palette_r->black_pen(), cliprect);
	screen.priority().fill(0);

	tetrisp2_draw_sprites(
			bitmap, screen.priority(), cliprect, m_priority.get(),
			m_spriteram3, m_spriteram3.bytes(), m_vj_gfxdecode_r->gfx(0), (m_systemregs[0x00] & 0x02), true);

	return 0;
}

uint32_t stepstag_state::screen_update_stepstag_main(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bool const flipscreen = bool(m_systemregs[0x00] & 0x02);

	/* Black background color */
	bitmap.fill(0, cliprect);
	screen.priority().fill(0);

	/* Flip Screen */
	if (flipscreen != m_flipscreen_old)
	{
		m_flipscreen_old = flipscreen;
		machine().tilemap().set_flip_all(flipscreen ? (TILEMAP_FLIPX | TILEMAP_FLIPY) : 0);
	}

	/* Flip Screen */
	int const rot_ofsx = flipscreen ? 0x053f : 0x400;
	int const rot_ofsy = flipscreen ? 0x04df : 0x400;

	m_tilemap_bg->set_scrollx(0, (((m_scroll_bg[ 0 ] + 0x0014) + m_scroll_bg[ 2 ] ) & 0xffff));
	m_tilemap_bg->set_scrolly(0, (((m_scroll_bg[ 3 ] + 0x0000) + m_scroll_bg[ 5 ] ) & 0xffff));

	m_tilemap_fg->set_scrollx(0, m_scroll_fg[ 2 ]);
	m_tilemap_fg->set_scrolly(0, m_scroll_fg[ 5 ]);

	m_tilemap_rot->set_scrollx(0, (m_rotregs[ 0 ] - rot_ofsx));
	m_tilemap_rot->set_scrolly(0, (m_rotregs[ 2 ] - rot_ofsy));

	int asc_pri = 0, scr_pri = 0, rot_pri = 0;

	if ((m_priority[0x2b00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		rot_pri++;

	if ((m_priority[0x2e00 / 2] & 0x00ff) == 0x0034)
		asc_pri++;
	else
		scr_pri++;

	if ((m_priority[0x3a00 / 2] & 0x00ff) == 0x000c)
		scr_pri++;
	else
		rot_pri++;

	if (rot_pri == 0)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 0)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 0)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 1)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 1)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 1)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	if (rot_pri == 2)
		m_tilemap_rot->draw(screen, bitmap, cliprect, 0, 1 << 1);
	else if (scr_pri == 2)
		m_tilemap_bg->draw(screen, bitmap, cliprect, 0, 1 << 0);
	else if (asc_pri == 2)
		m_tilemap_fg->draw(screen, bitmap, cliprect, 0, 1 << 2);

	tetrisp2_draw_sprites(
			bitmap, screen.priority(), cliprect, m_priority.get(),
			m_spriteram, m_spriteram.bytes(), m_gfxdecode->gfx(0), (m_systemregs[0x00] & 0x02), false);

	return 0;
}

// Stepping Stage encodes palette as YUV422.
// Convert them on the fly
void stepstag_state::convert_yuv422_to_rgb888(palette_device *paldev, uint16_t *palram, uint32_t offset)
{
	uint8_t u =  palram[offset/4*4+0] & 0xff;
	uint8_t y1 = palram[offset/4*4+1] & 0xff;
	uint8_t v =  palram[offset/4*4+2] & 0xff;
	//uint8_t y2 = palram[offset/4*4+3] & 0xff;
	double bf = y1+1.772*(u - 128);
	double gf = y1-0.334*(u - 128) - 0.714 * (v - 128);
	double rf = y1+1.772*(v - 128);
	// clamp to 0-255 range
	rf = std::min(rf,255.0);
	rf = std::max(rf,0.0);
	gf = std::min(gf,255.0);
	gf = std::max(gf,0.0);
	bf = std::min(bf,255.0);
	bf = std::max(bf,0.0);

	uint8_t r = (uint8_t)rf;
	uint8_t g = (uint8_t)gf;
	uint8_t b = (uint8_t)bf;

	paldev->set_pen_color(offset/4, r, g, b);
}

WRITE16_MEMBER(stepstag_state::stepstag_palette_left_w)
{
	COMBINE_DATA(&m_vj_paletteram_l[offset]);
	convert_yuv422_to_rgb888(m_vj_palette_l,m_vj_paletteram_l,offset);
}

WRITE16_MEMBER(stepstag_state::stepstag_palette_mid_w)
{
	COMBINE_DATA(&m_vj_paletteram_m[offset]);
	convert_yuv422_to_rgb888(m_vj_palette_m,m_vj_paletteram_m,offset);
}

WRITE16_MEMBER(stepstag_state::stepstag_palette_right_w)
{
	COMBINE_DATA(&m_vj_paletteram_r[offset]);
	convert_yuv422_to_rgb888(m_vj_palette_r,m_vj_paletteram_r,offset);
}
