// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/**********************************************************************

    TI TMS9927 and compatible CRT controller emulation

**********************************************************************/

#ifndef MAME_VIDEO_TMS9927_H
#define MAME_VIDEO_TMS9927_H


class tms9927_device : public device_t, public device_video_interface
{
public:
	tms9927_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto vsyn_callback() { return m_write_vsyn.bind(); }
	auto hsyn_callback() { return m_write_hsyn.bind(); }

	void set_char_width(int pixels) { m_hpixels_per_column = pixels; }
	void set_region_tag(const char *tag) { m_selfload.set_tag(tag); }
	void set_overscan(int left, int right, int top, int bottom) {
		m_overscan_left = left;
		m_overscan_right = right;
		m_overscan_top = top;
		m_overscan_bottom = bottom;
	}
	void set_visarea(s16 minx, s16 maxx, s16 miny, s16 maxy) { m_custom_visarea.set(minx, maxx, miny, maxy); }

	DECLARE_WRITE8_MEMBER(write);
	DECLARE_READ8_MEMBER(read);

	DECLARE_READ_LINE_MEMBER(bl_r);

	bool screen_reset() const { return m_reset; }
	int upscroll_offset() const { return m_start_datarow; }
	bool cursor_bounds(rectangle &bounds) const;

protected:
	tms9927_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_stop() override;
	virtual void device_reset() override;
	virtual void device_clock_changed() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	virtual void device_post_load() override;

private:
	enum
	{
		TIMER_VSYNC,
		TIMER_HSYNC
	};

	void recompute_parameters(bool postload);
	void generic_access(address_space &space, offs_t offset);

	devcb_write_line m_write_vsyn;
	devcb_write_line m_write_hsyn;

	int m_hpixels_per_column;         /* number of pixels per video memory address */
	uint16_t  m_overscan_left;
	uint16_t  m_overscan_right;
	uint16_t  m_overscan_top;
	uint16_t  m_overscan_bottom;

	// internal state
	optional_region_ptr<uint8_t> m_selfload;

	/* live state */
	uint8_t   m_reg[9];
	uint8_t   m_start_datarow;
	bool      m_reset;
	bool      m_vsyn;
	bool      m_hsyn;

	/* derived state; no need to save */
	bool      m_valid_config;
	uint16_t  m_total_hpix, m_total_vpix;
	uint16_t  m_visible_hpix, m_visible_vpix;
	rectangle m_custom_visarea;
	uint16_t  m_vsyn_start, m_vsyn_end;
	uint16_t  m_hsyn_start, m_hsyn_end;

	emu_timer *m_vsync_timer;
	emu_timer *m_hsync_timer;
};


class crt5027_device : public tms9927_device
{
public:
	crt5027_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class crt5037_device : public tms9927_device
{
public:
	crt5037_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class crt5057_device : public tms9927_device
{
public:
	crt5057_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


DECLARE_DEVICE_TYPE(TMS9927, tms9927_device)
DECLARE_DEVICE_TYPE(CRT5027, crt5027_device)
DECLARE_DEVICE_TYPE(CRT5037, crt5037_device)
DECLARE_DEVICE_TYPE(CRT5057, crt5057_device)

#endif // MAME_VIDEO_TMS9927_H
