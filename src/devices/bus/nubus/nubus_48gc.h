// license:BSD-3-Clause
// copyright-holders:R. Belmont
#pragma once

#ifndef __NUBUS_48GC_H__
#define __NUBUS_48GC_H__

#include "emu.h"
#include "nubus.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> jmfb_device

class jmfb_device :
		public device_t,
		public device_video_interface,
		public device_nubus_card_interface
{
public:
		// construction/destruction
		jmfb_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const override;
		virtual const tiny_rom_entry *device_rom_region() const override;
		virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

		uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

		screen_device *m_screen;
		emu_timer *m_timer;
protected:
		// device-level overrides
		virtual void device_start() override;
		virtual void device_reset() override;

		DECLARE_READ32_MEMBER(mac_48gc_r);
		DECLARE_WRITE32_MEMBER(mac_48gc_w);

public:
		std::vector<uint8_t> m_vram;
		uint32_t m_mode, m_vbl_disable, m_toggle, m_stride, m_base;
		uint32_t m_palette[256], m_colors[3], m_count, m_clutoffs;
		uint32_t m_registers[0x100];
		int m_xres, m_yres;
		bool m_is824;
		std::string m_assembled_tag;
};

class nubus_48gc_device : public jmfb_device
{
public:
	nubus_48gc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};

class nubus_824gc_device : public jmfb_device
{
public:
	nubus_824gc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual const tiny_rom_entry *device_rom_region() const override;
};

// device type definition
extern const device_type NUBUS_48GC;
extern const device_type NUBUS_824GC;

#endif  /* __NUBUS_48GC_H__ */
