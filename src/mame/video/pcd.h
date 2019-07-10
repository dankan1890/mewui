// license:BSD-3-Clause
// copyright-holders:Carl
#ifndef MAME_VIDEO_PCD_H
#define MAME_VIDEO_PCD_H

#pragma once

#include "machine/pic8259.h"
#include "machine/timer.h"
#include "video/scn2674.h"
#include "emupal.h"

#include "diserial.h"


class pcdx_video_device : public device_t, public device_gfx_interface
{
public:
	virtual void map(address_map &map) = 0;
	DECLARE_READ8_MEMBER(detect_r);
	DECLARE_WRITE8_MEMBER(detect_w);

protected:
	pcdx_video_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	void pcdx_palette(palette_device &palette) const;

	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_mcu;
	required_device<pic8259_device> m_pic2;
};

class pcd_video_device : public pcdx_video_device
{
public:
	pcd_video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void map(address_map &map) override;
	DECLARE_WRITE8_MEMBER(vram_sw_w);
	DECLARE_READ8_MEMBER(vram_r);
	DECLARE_WRITE8_MEMBER(vram_w);

protected:
	void device_start() override;
	void device_reset() override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

private:
	required_device<scn2674_device> m_crtc;
	required_ioport m_mouse_btn;
	required_ioport m_mouse_x;
	required_ioport m_mouse_y;

	std::vector<uint8_t> m_vram;
	std::vector<uint8_t> m_charram;
	uint8_t m_vram_sw, m_t1, m_p2;

	struct
	{
		int phase;
		int x;
		int y;
		int prev_x;
		int prev_y;
		int xa;
		int xb;
		int ya;
		int yb;
	} m_mouse;

	DECLARE_READ_LINE_MEMBER(t1_r);
	DECLARE_READ8_MEMBER(p1_r);
	DECLARE_WRITE8_MEMBER(p2_w);
	TIMER_DEVICE_CALLBACK_MEMBER(mouse_timer);

	SCN2674_DRAW_CHARACTER_MEMBER(display_pixels);
};

class pcx_video_device : public pcdx_video_device,
							public device_serial_interface
{
public:
	pcx_video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	auto txd_handler() { return m_txd_handler.bind(); }

	virtual void map(address_map &map) override;
	DECLARE_READ8_MEMBER(term_r);
	DECLARE_WRITE8_MEMBER(term_w);
	DECLARE_READ8_MEMBER(term_mcu_r);
	DECLARE_WRITE8_MEMBER(term_mcu_w);
	DECLARE_READ8_MEMBER(unk_r);
	DECLARE_WRITE8_MEMBER(p1_w);

	void pcx_vid_io(address_map &map);
	void pcx_vid_map(address_map &map);
	void pcx_char_ram(address_map &map);
	void pcx_attr_ram(address_map &map);
protected:
	void device_start() override;
	void device_reset() override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	void tra_callback() override;
	void rcv_complete() override;

private:
	required_device<scn2672_device> m_crtc;

	required_region_ptr<uint8_t> m_charrom;
	devcb_write_line m_txd_handler;
	uint8_t m_term_key, m_term_char, m_term_stat, m_p1;

	DECLARE_READ8_MEMBER(rx_callback);
	DECLARE_WRITE8_MEMBER(tx_callback);

	SCN2672_DRAW_CHARACTER_MEMBER(display_pixels);
};

DECLARE_DEVICE_TYPE(PCD_VIDEO, pcd_video_device)
DECLARE_DEVICE_TYPE(PCX_VIDEO, pcx_video_device)

#endif // MAME_VIDEO_PCD_H
