// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_MACHINE_CEDAR_MAGNET_SPRITE_H
#define MAME_MACHINE_CEDAR_MAGNET_SPRITE_H

#pragma once

#include "machine/cedar_magnet_board.h"

#include "cpu/z80/z80.h"
#include "machine/z80daisy.h"
#include "machine/z80pio.h"
#include "machine/bankdev.h"

DECLARE_DEVICE_TYPE(CEDAR_MAGNET_SPRITE, cedar_magnet_sprite_device)

class cedar_magnet_sprite_device : public device_t, public cedar_magnet_board_interface
{
public:
	// construction/destruction
	cedar_magnet_sprite_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	void sprite_port80_w(u8 data);
	void sprite_port84_w(u8 data);
	void sprite_port88_w(u8 data);
	void sprite_port8c_w(u8 data);
	void sprite_port9c_w(u8 data);

	u8 exzisus_hack_r(offs_t offset);

	INTERRUPT_GEN_MEMBER(irq);

	u32 draw(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int palbase);

	void cedar_magnet_sprite_io(address_map &map);
	void cedar_magnet_sprite_map(address_map &map);
	void cedar_magnet_sprite_sub_ram_map(address_map &map);
protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	void do_blit();

	std::unique_ptr<u8[]> m_framebuffer;
	u8 m_pio2_pb_data;

	required_device<address_map_bank_device> m_sprite_ram_bankdev;

	u8 m_upperaddr;
	u8 m_loweraddr;

	u8 m_spritesize;
	u8 m_pio0_pb_data;
	u8 m_spritecodelow;
	u8 m_spritecodehigh;

	int m_high_write;
	u8 m_uppersprite;

	u8 pio0_pa_r();
	void pio0_pa_w(u8 data);
//  DECLARE_READ8_MEMBER(pio0_pb_r);
	void pio0_pb_w(u8 data);

//  DECLARE_READ8_MEMBER(pio1_pa_r);
	void pio1_pa_w(u8 data);
//  DECLARE_READ8_MEMBER(pio1_pb_r);
	void pio1_pb_w(u8 data);

//  DECLARE_READ8_MEMBER(pio2_pa_r);
	void pio2_pa_w(u8 data);
//  DECLARE_READ8_MEMBER(pio2_pb_r);
	void pio2_pb_w(u8 data);

	required_device_array<z80pio_device, 3> m_pio;
};

#endif // MAME_MACHINE_CEDAR_MAGNET_SPRITE_H
