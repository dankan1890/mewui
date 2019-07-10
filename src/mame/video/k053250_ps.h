// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#ifndef MAME_VIDEO_K053250PS_H
#define MAME_VIDEO_K053250PS_H

#pragma once

//
//  Konami 053250 road generator (Pirate Ship version)
//


class k053250ps_device :  public device_t,
						public device_gfx_interface,
						public device_video_interface
{
public:
	template <typename T, typename U>
	k053250ps_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&palette_tag, U &&screen_tag, int offx, int offy)
		: k053250ps_device(mconfig, tag, owner, clock)
	{
		set_palette(std::forward<T>(palette_tag));
		set_screen(std::forward<U>(screen_tag));
		set_offsets(offx, offy);
	}

	k053250ps_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_offsets(int offx, int offy)
	{
		m_offx = offx;
		m_offy = offy;
	}
	auto dmairq_cb() { return m_dmairq_cb.bind(); }

	DECLARE_READ16_MEMBER(reg_r);
	DECLARE_WRITE16_MEMBER(reg_w);
	DECLARE_READ16_MEMBER(ram_r);
	DECLARE_WRITE16_MEMBER(ram_w);
	DECLARE_READ16_MEMBER(rom_r);
	DECLARE_WRITE_LINE_MEMBER(vblank_w);
	DECLARE_READ_LINE_MEMBER(dmairq_r);

	void draw( bitmap_rgb32 &bitmap, const rectangle &cliprect, int colorbase, int flags, bitmap_ind8 &priority_bitmap, int priority );

protected:
	// device-level overrides
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	virtual void device_start() override;
	virtual void device_reset() override;

private:

	enum {
		OD_IDLE,
		OD_WAIT_START,
		OD_WAIT_END
	};

	devcb_write_line m_dmairq_cb;
	int m_timer_lvcdma_state;
	bool m_dmairq_on;

	// configuration
	int m_offx, m_offy;

	emu_timer *m_timer_lvcdma;

	// internal state
	required_region_ptr<uint8_t> m_rom;
	std::vector<uint8_t> m_unpacked_rom;
	std::vector<uint16_t> m_ram;
	uint16_t *m_buffer[2];
	uint8_t m_regs[8];
	uint8_t m_page;

	// internal helpers
	void unpack_nibbles();
	static void pdraw_scanline32(bitmap_rgb32 &bitmap, const pen_t *pal_base, uint8_t *source,
									const rectangle &cliprect, int linepos, int scroll, int zoom,
									uint32_t clipmask, uint32_t wrapmask, uint32_t orientation, bitmap_ind8 &priority, uint8_t pri);
};

DECLARE_DEVICE_TYPE(K053250PS, k053250ps_device)

#endif // MAME_VIDEO_K053250PS_H
