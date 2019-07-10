// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
#ifndef MAME_BUS_ASTROCDE_LIGHTPEN_H
#define MAME_BUS_ASTROCDE_LIGHTPEN_H

#pragma once

#include "accessory.h"


/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/

// ======================> astrocade_lightpen_device

class astrocade_lightpen_device : public device_t,
								  public device_astrocade_accessory_interface
{
public:
	// construction/destruction
	astrocade_lightpen_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock = 0U);
	virtual ~astrocade_lightpen_device();

	DECLARE_INPUT_CHANGED_MEMBER( trigger );

protected:
	// device_t implementation
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	static const device_timer_id TIMER_TRIGGER = 0;

private:
	required_ioport m_trigger;
	required_ioport m_lightx;
	required_ioport m_lighty;
	emu_timer *m_pen_timer;
	bool m_retrigger;
};


/***************************************************************************
 DEVICE TYPES
 ***************************************************************************/

DECLARE_DEVICE_TYPE(ASTROCADE_LIGHTPEN, astrocade_lightpen_device)

#endif // MAME_BUS_ASTROCDE_LIGHTPEN_H
