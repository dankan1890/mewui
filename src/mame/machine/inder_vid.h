// license:BSD-3-Clause
// copyright-holders:David Haywood
/* Inder / Dinamic Video */


/* */


#ifndef MAME_MACHINE_INDER_VID_H
#define MAME_MACHINE_INDER_VID_H

#pragma once


#include "video/ramdac.h"
#include "cpu/tms34010/tms34010.h"
#include "emupal.h"

DECLARE_DEVICE_TYPE(INDER_VIDEO, inder_vid_device)


class inder_vid_device : public device_t
/*  public device_video_interface */
{
public:
	// construction/destruction
	inder_vid_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// probably set by a register somewhere either on TMS side or 68k side
	void set_bpp(int bpp)
	{
		m_bpp_mode = bpp;
	}

	void megaphx_tms_map(address_map &map);
	void ramdac_map(address_map &map);
protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	required_shared_ptr<uint16_t> m_vram;
	required_device<palette_device> m_palette;
	required_device<tms34010_device> m_tms;

	int m_shiftfull; // this might be a driver specific hack for a TMS bug.

	TMS340X0_TO_SHIFTREG_CB_MEMBER(to_shiftreg);
	TMS340X0_FROM_SHIFTREG_CB_MEMBER(from_shiftreg);
	TMS340X0_SCANLINE_RGB32_CB_MEMBER(scanline);

	int m_bpp_mode;
};

#endif // MAME_MACHINE_INDER_VID_H
