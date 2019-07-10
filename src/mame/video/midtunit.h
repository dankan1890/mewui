// license:BSD-3-Clause
// copyright-holders:Alex Pasadyn, Zsolt Vasvari, Ernesto Corvi, Aaron Giles
// thanks-to:Kurt Mahan
/*************************************************************************

    Video Emulation for Midway T-unit, W-unit, and X-unit games.

**************************************************************************/

#ifndef MAME_VIDEO_MIDTUNIT_H
#define MAME_VIDEO_MIDTUNIT_H

#pragma once

#include "emu.h"
#include "cpu/tms34010/tms34010.h"
#include "emupal.h"

#define DEBUG_MIDTUNIT_BLITTER      (0)

class midtunit_video_device : public device_t
{
public:
	// construction/destruction
	template <typename T, typename U, typename V>
	midtunit_video_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu_tag, U &&palette_tag, V &&gfxrom_tag)
		: midtunit_video_device(mconfig, tag, owner, (uint32_t)0)
	{
		m_maincpu.set_tag(std::forward<T>(cpu_tag));
		m_palette.set_tag(std::forward<U>(palette_tag));
		m_gfxrom.set_tag(std::forward<V>(gfxrom_tag));
	}

	midtunit_video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	TMS340X0_TO_SHIFTREG_CB_MEMBER(to_shiftreg);
	TMS340X0_FROM_SHIFTREG_CB_MEMBER(from_shiftreg);
	TMS340X0_SCANLINE_IND16_CB_MEMBER(scanline_update);

	DECLARE_READ16_MEMBER(midtunit_vram_r);
	DECLARE_WRITE16_MEMBER(midtunit_vram_w);
	DECLARE_READ16_MEMBER(midtunit_gfxrom_r);
	DECLARE_WRITE16_MEMBER(midtunit_vram_data_w);
	DECLARE_WRITE16_MEMBER(midtunit_vram_color_w);
	DECLARE_READ16_MEMBER(midtunit_vram_data_r);
	DECLARE_READ16_MEMBER(midtunit_vram_color_r);

	DECLARE_READ16_MEMBER(midtunit_dma_r);
	DECLARE_WRITE16_MEMBER(midtunit_dma_w);

	DECLARE_WRITE16_MEMBER(midtunit_control_w);

	void set_gfx_rom_large(bool gfx_rom_large) { m_gfx_rom_large = gfx_rom_large; }

	enum op_type_t
	{
		PIXEL_SKIP  = 0,
		PIXEL_COLOR = 1,
		PIXEL_COPY  = 2
	};

protected:
	// construction/destruction
	midtunit_video_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock = 0);

	virtual void device_start() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	static const device_timer_id TIMER_DMA = 0;

	required_device<tms340x0_device> m_maincpu;
	required_device<palette_device> m_palette;
	required_memory_region m_gfxrom;

	emu_timer *m_dma_timer;

	/* constants for the DMA chip */
	static constexpr uint32_t XPOSMASK = 0x3ff;
	static constexpr uint32_t YPOSMASK = 0x1ff;

	template <int BitsPerPixel, bool XFlip, bool Skip, bool Scale, op_type_t Zero, op_type_t NonZero> void dma_draw();
	void dma_draw_none() {};

	typedef void (midtunit_video_device::*draw_func)();
	draw_func m_dma_draw_skip_scale[8*32];
	draw_func m_dma_draw_noskip_scale[8*32];
	draw_func m_dma_draw_skip_noscale[8*32];
	draw_func m_dma_draw_noskip_noscale[8*32];

	enum
	{
		DMA_LRSKIP = 0,
		DMA_COMMAND,
		DMA_OFFSETLO,
		DMA_OFFSETHI,
		DMA_XSTART,
		DMA_YSTART,
		DMA_WIDTH,
		DMA_HEIGHT,
		DMA_PALETTE,
		DMA_COLOR,
		DMA_SCALE_X,
		DMA_SCALE_Y,
		DMA_TOPCLIP,
		DMA_BOTCLIP,
		DMA_UNKNOWN_E,  /* MK1/2 never write here; NBA only writes 0 */
		DMA_CONFIG,
		DMA_LEFTCLIP,   /* pseudo-register */
		DMA_RIGHTCLIP   /* pseudo-register */
	};

	/* graphics-related variables */
	uint16_t    m_midtunit_control;
	bool        m_gfx_rom_large;

	/* videoram-related variables */
	uint32_t    m_gfxbank_offset[2];
	std::unique_ptr<uint16_t[]> m_local_videoram;
	uint8_t     m_videobank_select;

	/* DMA-related variables */
	uint16_t    m_dma_register[18];
	struct dma_state
	{
		uint8_t *     gfxrom;

		uint32_t      offset;         /* source offset, in bits */
		int32_t       rowbits;        /* source bits to skip each row */
		int32_t       xpos;           /* x position, clipped */
		int32_t       ypos;           /* y position, clipped */
		int32_t       width;          /* horizontal pixel count */
		int32_t       height;         /* vertical pixel count */
		uint16_t      palette;        /* palette base */
		uint16_t      color;          /* current foreground color with palette */

		uint8_t       yflip;          /* yflip? */
		uint8_t       preskip;        /* preskip scale */
		uint8_t       postskip;       /* postskip scale */
		int32_t       topclip;        /* top clipping scanline */
		int32_t       botclip;        /* bottom clipping scanline */
		int32_t       leftclip;       /* left clipping column */
		int32_t       rightclip;      /* right clipping column */
		int32_t       startskip;      /* pixels to skip at start */
		int32_t       endskip;        /* pixels to skip at end */
		uint16_t      xstep;          /* 8.8 fixed number scale x factor */
		uint16_t      ystep;          /* 8.8 fixed number scale y factor */
	};
	dma_state   m_dma_state;

#if DEBUG_MIDTUNIT_BLITTER
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;

	uint32_t debug_screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	void do_debug_blit();
	void do_dma_debug_inputs();

	required_device<palette_device> m_debug_palette;
	std::unique_ptr<uint16_t[]> m_debug_videoram;
	bool m_dma_debug;
	bool m_doing_debug_dma;
	dma_state m_debug_dma_state;
	int32_t m_debug_dma_bpp;
	int32_t m_debug_dma_mode;
	int32_t m_debug_dma_command;
#endif

	char m_log_path[2048];
	bool m_log_png;
	bool m_log_json;
	std::unique_ptr<uint64_t[]> m_logged_rom;
	bitmap_argb32 m_log_bitmap;

	void debug_init();
	void debug_commands(int ref, const std::vector<std::string> &params);
	void debug_help_command(int ref, const std::vector<std::string> &params);
	void debug_png_dma_command(int ref, const std::vector<std::string> &params);
	void log_bitmap(int command, int bpp, bool skip);
};

class midwunit_video_device : public midtunit_video_device
{
public:
	// construction/destruction
	template <typename T, typename U, typename V>
	midwunit_video_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu_tag, U &&palette_tag, V &&gfxrom_tag)
		: midwunit_video_device(mconfig, tag, owner, (uint32_t)0)
	{
		m_maincpu.set_tag(std::forward<T>(cpu_tag));
		m_palette.set_tag(std::forward<U>(palette_tag));
		m_gfxrom.set_tag(std::forward<V>(gfxrom_tag));
	}
	midwunit_video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	DECLARE_READ16_MEMBER(midwunit_gfxrom_r);

	DECLARE_WRITE16_MEMBER(midwunit_control_w);
	DECLARE_READ16_MEMBER(midwunit_control_r);

protected:
	midwunit_video_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock = 0);

	virtual void device_start() override;
#if DEBUG_MIDTUNIT_BLITTER
	virtual void device_add_mconfig(machine_config &config) override;
#endif
};

class midxunit_video_device : public midwunit_video_device
{
public:
	// construction/destruction
	template <typename T, typename U, typename V>
	midxunit_video_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu_tag, U &&palette_tag, V &&gfxrom_tag)
		: midwunit_video_device(mconfig, tag, owner, (uint32_t)0)
	{
		m_maincpu.set_tag(std::forward<T>(cpu_tag));
		m_palette.set_tag(std::forward<U>(palette_tag));
		m_gfxrom.set_tag(std::forward<V>(gfxrom_tag));
	}

	midxunit_video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	DECLARE_WRITE16_MEMBER(midxunit_paletteram_w);
	DECLARE_READ16_MEMBER(midxunit_paletteram_r);

	TMS340X0_SCANLINE_IND16_CB_MEMBER(scanline_update);

protected:
	virtual void device_start() override;
};

DECLARE_DEVICE_TYPE(MIDTUNIT_VIDEO, midtunit_video_device)
DECLARE_DEVICE_TYPE(MIDWUNIT_VIDEO, midwunit_video_device)
DECLARE_DEVICE_TYPE(MIDXUNIT_VIDEO, midxunit_video_device)

#endif // MAME_VIDEO_MIDTUNIT_H
