// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_MACHINE_CEDAR_MAGNET_PLANE_H
#define MAME_MACHINE_CEDAR_MAGNET_PLANE_H

#pragma once


#include "machine/cedar_magnet_board.h"
#include "machine/z80pio.h"

DECLARE_DEVICE_TYPE(CEDAR_MAGNET_PLANE, cedar_magnet_plane_device)

class cedar_magnet_plane_device : public device_t, public cedar_magnet_board_interface
{
public:
	// construction/destruction
	cedar_magnet_plane_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	void plane_portcc_w(u8 data);
	void plane_portcd_w(u8 data);
	void plane_portce_w(u8 data);
	void plane_portcf_w(u8 data);

	INTERRUPT_GEN_MEMBER(vblank_irq);

	u32 draw(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int palbase);

	void cedar_magnet_plane_io(address_map &map);
	void cedar_magnet_plane_map(address_map &map);
protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;

private:
	std::unique_ptr<u8[]> m_framebuffer;
	int m_curline;
	int m_lineoffset;

	u8 m_pio0_pa_data;
	u8 m_pio0_pb_data;
	u8 m_scrollx;
	u8 m_scrolly;
	int m_direction;

	u8 m_cd_data;
	u8 m_cf_data;

	u8 pio0_pa_r();
	void pio0_pa_w(u8 data);
//  DECLARE_READ8_MEMBER(pio0_pb_r);
	void pio0_pb_w(u8 data);

//  DECLARE_READ8_MEMBER(pio1_pa_r);
	void pio1_pa_w(u8 data);
//  DECLARE_READ8_MEMBER(pio1_pb_r);
	void pio1_pb_w(u8 data);
};

#endif // MAME_MACHINE_CEDAR_MAGNET_PLANE_H
