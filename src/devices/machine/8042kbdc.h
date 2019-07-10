// license:BSD-3-Clause
// copyright-holders:Peter Trauner
/**********************************************************************

    8042 Keyboard Controller Emulation

    This is the keyboard controller used in the IBM AT and further
    models.  It is a popular controller for PC style keyboards

**********************************************************************/

#ifndef MAME_MACHINE_8042KBDC_H
#define MAME_MACHINE_8042KBDC_H

#pragma once

#include "machine/pckeybrd.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> kbdc8042_device

class kbdc8042_device : public device_t
{
public:
	enum kbdc8042_type_t
	{
		KBDC8042_STANDARD,
		KBDC8042_PS2,       /* another timing of integrated controller */
		KBDC8042_AT386      /* hack for at386 driver */
	};

	// construction/destruction
	kbdc8042_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	void set_keyboard_type(kbdc8042_type_t keybtype) { m_keybtype = keybtype; }
	auto system_reset_callback() { return m_system_reset_cb.bind(); }
	auto gate_a20_callback() { return m_gate_a20_cb.bind(); }
	auto input_buffer_full_callback() { return m_input_buffer_full_cb.bind(); }
	auto output_buffer_empty_callback() { return m_output_buffer_empty_cb.bind(); }
	auto speaker_callback() { return m_speaker_cb.bind(); }

	DECLARE_READ8_MEMBER( data_r );
	DECLARE_WRITE8_MEMBER( data_w );

	DECLARE_WRITE_LINE_MEMBER( write_out2 );

	void at_8042_set_outport(uint8_t data, int initial);
	void at_8042_receive(uint8_t data, bool mouse = false);
	void at_8042_check_keyboard();
	void at_8042_check_mouse();
	void at_8042_clear_keyboard_received();

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	virtual ioport_constructor device_input_ports() const override;

	static const device_timer_id TIMER_UPDATE = 0;

private:
	uint8_t m_inport;
	uint8_t m_outport;
	uint8_t m_data;
	uint8_t m_command;

	struct {
		int received;
		int on;
	} m_keyboard;
	struct {
		int received;
		int on;
		uint8_t sample_rate;
		bool receiving_sample_rate;
		uint8_t transmit_buf[8];
		uint8_t to_transmit;
	} m_mouse;

	int m_last_write_to_control;
	int m_sending;
	int m_send_to_mouse;

	int m_operation_write_state;
	int m_status_read_mode;

	int m_speaker;
	int m_out2;

	/* temporary hack */
	int m_offset1;

	int m_poll_delay;

	required_device<at_keyboard_device> m_keyboard_dev;
	required_ioport m_mousex_port;
	required_ioport m_mousey_port;
	required_ioport m_mousebtn_port;

	kbdc8042_type_t     m_keybtype;

	devcb_write_line    m_system_reset_cb;
	devcb_write_line    m_gate_a20_cb;
	devcb_write_line    m_input_buffer_full_cb;
	devcb_write_line    m_output_buffer_empty_cb;

	devcb_write8        m_speaker_cb;

	uint16_t            m_mouse_x;
	uint16_t            m_mouse_y;
	uint8_t             m_mouse_btn;

	emu_timer *         m_update_timer;

	DECLARE_WRITE_LINE_MEMBER( keyboard_w );
};

// device type definition
DECLARE_DEVICE_TYPE(KBDC8042, kbdc8042_device)

#endif // MAME_MACHINE_8042KBDC_H
