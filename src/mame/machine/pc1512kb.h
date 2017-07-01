// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Amstrad PC1512 Keyboard emulation

**********************************************************************/

#pragma once

#ifndef __PC1512_KEYBOARD__
#define __PC1512_KEYBOARD__


#include "emu.h"
#include "bus/vcs_ctrl/ctrl.h"
#include "cpu/mcs48/mcs48.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define PC1512_KEYBOARD_TAG "kb"



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_PC1512_KEYBOARD_CLOCK_CALLBACK(_write) \
	devcb = &pc1512_keyboard_t::set_clock_wr_callback(*device, DEVCB_##_write);

#define MCFG_PC1512_KEYBOARD_DATA_CALLBACK(_write) \
	devcb = &pc1512_keyboard_t::set_data_wr_callback(*device, DEVCB_##_write);



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> pc1512_keyboard_t

class pc1512_keyboard_t :  public device_t
{
public:
	// construction/destruction
	pc1512_keyboard_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template<class _Object> static devcb_base &set_clock_wr_callback(device_t &device, _Object object) { return downcast<pc1512_keyboard_t &>(device).m_write_clock.set_callback(object); }
	template<class _Object> static devcb_base &set_data_wr_callback(device_t &device, _Object object) { return downcast<pc1512_keyboard_t &>(device).m_write_data.set_callback(object); }

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual machine_config_constructor device_mconfig_additions() const override;
	virtual ioport_constructor device_input_ports() const override;

	DECLARE_WRITE_LINE_MEMBER( data_w );
	DECLARE_WRITE_LINE_MEMBER( clock_w );
	DECLARE_WRITE_LINE_MEMBER( m1_w );
	DECLARE_WRITE_LINE_MEMBER( m2_w );

	DECLARE_READ8_MEMBER( kb_bus_r );
	DECLARE_WRITE8_MEMBER( kb_p1_w );
	DECLARE_READ8_MEMBER( kb_p2_r );
	DECLARE_WRITE8_MEMBER( kb_p2_w );
	DECLARE_READ8_MEMBER( kb_t0_r );
	DECLARE_READ8_MEMBER( kb_t1_r );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	enum
	{
		LED_CAPS = 0,
		LED_NUM
	};

	required_device<cpu_device> m_maincpu;
	required_device<vcs_control_port_device> m_joy;

	required_ioport_array<11> m_y;

	devcb_write_line   m_write_clock;
	devcb_write_line   m_write_data;

	int m_data_in;
	int m_clock_in;
	int m_kb_y;
	int m_joy_com;
	int m_m1;
	int m_m2;

	emu_timer *m_reset_timer;
};


// device type definition
extern const device_type PC1512_KEYBOARD;



#endif
