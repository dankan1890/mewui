// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Telenova Compis keyboard emulation

*********************************************************************/

#pragma once

#ifndef __COMPIS_KEYBOARD__
#define __COMPIS_KEYBOARD__

#include "emu.h"
#include "cpu/mcs48/mcs48.h"
#include "sound/speaker.h"



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_COMPIS_KEYBOARD_OUT_TX_HANDLER(_devcb) \
	devcb = &compis_keyboard_device::set_out_tx_handler(*device, DEVCB_##_devcb);




//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> compis_keyboard_device

class compis_keyboard_device :  public device_t
{
public:
	// construction/destruction
	compis_keyboard_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template<class _Object> static devcb_base &set_out_tx_handler(device_t &device, _Object object) { return downcast<compis_keyboard_device &>(device).m_out_tx_handler.set_callback(object); }

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual machine_config_constructor device_mconfig_additions() const override;
	virtual ioport_constructor device_input_ports() const override;

	DECLARE_WRITE_LINE_MEMBER( si_w );

	DECLARE_READ8_MEMBER( bus_r );
	DECLARE_WRITE8_MEMBER( bus_w );
	DECLARE_READ8_MEMBER( p1_r );
	DECLARE_READ8_MEMBER( p2_r );

protected:
	// device-level overrides
	virtual void device_start() override;

private:
	enum
	{
		LED_CAPS
	};

	required_device<cpu_device> m_maincpu;
	required_device<speaker_sound_device> m_speaker;
	required_ioport_array<9> m_y;
	required_ioport m_special;
	devcb_write_line   m_out_tx_handler;

	uint8_t m_bus;
	uint8_t m_keylatch;
};


// device type definition
extern const device_type COMPIS_KEYBOARD;



#endif
