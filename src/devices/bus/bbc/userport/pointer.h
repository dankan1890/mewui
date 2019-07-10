// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    Mouse emulation

**********************************************************************/


#ifndef MAME_BUS_BBC_USERPORT_POINTER_H
#define MAME_BUS_BBC_USERPORT_POINTER_H

#pragma once

#include "userport.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class bbc_pointer_device :
	public device_t,
	public device_bbc_userport_interface
{
public:
	DECLARE_INPUT_CHANGED_MEMBER(pointer_changed);

protected:
	// construction/destruction
	bbc_pointer_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	required_ioport m_pointer_x;
	required_ioport m_pointer_y;
	required_ioport m_buttons;

	// quadrature output
	int m_xdir, m_ydir;

	// internal quadrature state
	int m_direction_x, m_direction_y;
	int m_distance_x, m_distance_y;
	int m_phase_x, m_phase_y;

	TIMER_CALLBACK_MEMBER(pointer_poll);
	emu_timer *m_pointer_timer;
};


// ======================> bbc_amxmouse_device

class bbc_amxmouse_device : public bbc_pointer_device
{
public:
	// construction/destruction
	bbc_amxmouse_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint8_t pb_r() override;

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;
};


// ======================> bbc_m512mouse_device

class bbc_m512mouse_device : public bbc_pointer_device
{
public:
	// construction/destruction
	bbc_m512mouse_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint8_t pb_r() override;

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;
};


// ======================> bbc_tracker_device

class bbc_tracker_device : public bbc_pointer_device
{
public:
	// construction/destruction
	bbc_tracker_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint8_t pb_r() override;

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_AMXMOUSE,  bbc_amxmouse_device)
DECLARE_DEVICE_TYPE(BBC_M512MOUSE, bbc_m512mouse_device)
DECLARE_DEVICE_TYPE(BBC_TRACKER,   bbc_tracker_device)


#endif // MAME_BUS_BBC_USERPORT_POINTER_H
