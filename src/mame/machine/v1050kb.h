// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Visual 1050 keyboard emulation

*********************************************************************/

#ifndef MAME_MACHINE_V1050KB_H
#define MAME_MACHINE_V1050KB_H

#pragma once

#include "cpu/mcs48/mcs48.h"
#include "sound/discrete.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> V1050_keyboard_device

class v1050_keyboard_device :  public device_t
{
public:
	// construction/destruction
	v1050_keyboard_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device flags
	static constexpr feature_type imperfect_features() { return feature::KEYBOARD; }

	auto out_tx_handler() { return m_out_tx_handler.bind(); }

	DECLARE_WRITE_LINE_MEMBER( si_w );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

private:
	required_device<i8049_device> m_maincpu;
	required_device<discrete_sound_device> m_discrete;
	required_ioport_array<12> m_y;
	devcb_write_line   m_out_tx_handler;
	output_finder<> m_led;

	uint8_t m_keylatch;

	DECLARE_READ8_MEMBER( kb_p1_r );
	DECLARE_WRITE8_MEMBER( kb_p1_w );
	DECLARE_WRITE8_MEMBER( kb_p2_w );
};


// device type definition
DECLARE_DEVICE_TYPE(V1050_KEYBOARD, v1050_keyboard_device)

#endif // MAME_MACHINE_V1050KB_H
