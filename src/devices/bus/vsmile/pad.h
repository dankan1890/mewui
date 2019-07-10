// license:BSD-3-Clause
// copyright-holders:Ryan Holtz, Vas Crabb
#ifndef MAME_BUS_VSMILE_PAD_H
#define MAME_BUS_VSMILE_PAD_H

#pragma once

#include "vsmile_ctrl.h"

/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/

// ======================> vsmile_pad_device

class vsmile_pad_device : public vsmile_ctrl_device_base
{
public:
	enum stale_inputs : uint8_t
	{
		STALE_NONE          = 0U,
		STALE_LEFT_RIGHT    = 1U << 0,
		STALE_UP_DOWN       = 1U << 1,
		STALE_COLORS        = 1U << 2,
		STALE_OK            = 1U << 3,
		STALE_QUIT          = 1U << 4,
		STALE_HELP          = 1U << 5,
		STALE_ABC           = 1U << 6,

		STALE_JOY           = STALE_LEFT_RIGHT | STALE_UP_DOWN,
		STALE_BUTTONS       = STALE_OK | STALE_QUIT | STALE_HELP | STALE_ABC,
		STALE_ALL           = STALE_JOY | STALE_COLORS | STALE_BUTTONS
	};

	// construction/destruction
	vsmile_pad_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock = 0U);
	virtual ~vsmile_pad_device();

	// input handlers
	DECLARE_INPUT_CHANGED_MEMBER(pad_joy_changed);
	DECLARE_INPUT_CHANGED_MEMBER(pad_color_changed);
	DECLARE_INPUT_CHANGED_MEMBER(pad_button_changed);

protected:
	// device_t implementation
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_start() override;

	// vsmile_ctrl_device_base implementation
	virtual void tx_complete() override;
	virtual void tx_timeout() override;
	virtual void rx_complete(uint8_t data, bool cts) override;

private:
	void uart_tx_fifo_push(uint8_t data);

	TIMER_CALLBACK_MEMBER(handle_idle);

	required_ioport m_io_joy;
	required_ioport m_io_colors;
	required_ioport m_io_buttons;

	emu_timer *m_idle_timer;

	uint8_t m_sent_joy, m_sent_colors, m_sent_buttons;
	stale_inputs m_stale;
	bool m_active;
	uint8_t m_ctrl_probe_history[2];
};


/***************************************************************************
 DEVICE TYPES
 ***************************************************************************/

DECLARE_DEVICE_TYPE(VSMILE_PAD, vsmile_pad_device)

#endif // MAME_BUS_VSMILE_PAD_H
