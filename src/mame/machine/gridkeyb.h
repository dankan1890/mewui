// license:BSD-3-Clause
// copyright-holders:Vas Crabb, Sergey Svishchev
#ifndef MAME_MACHINE_GRIDKEYB_H
#define MAME_MACHINE_GRIDKEYB_H

#pragma once

#include "machine/keyboard.h"


/***************************************************************************
    DEVICE TYPE GLOBALS
***************************************************************************/

DECLARE_DEVICE_TYPE(GRID_KEYBOARD, grid_keyboard_device)



/***************************************************************************
    REUSABLE I/O PORTS
***************************************************************************/

INPUT_PORTS_EXTERN( grid_keyboard );



/***************************************************************************
    TYPE DECLARATIONS
***************************************************************************/


class grid_keyboard_device : public device_t, protected device_matrix_keyboard_interface<4U>
{
public:
	typedef device_delegate<void (u16 character)> output_delegate;

	grid_keyboard_device(
			const machine_config &mconfig,
			const char *tag,
			device_t *owner,
			u32 clock);

	template <class FunctionClass>
	void set_keyboard_callback(void (FunctionClass::*callback)(u16 character), const char *name)
	{
		set_keyboard_callback(output_delegate(callback, name, nullptr, static_cast<FunctionClass *>(nullptr)));
	}
	void set_keyboard_callback(output_delegate callback) { m_keyboard_cb = callback; }

	virtual ioport_constructor device_input_ports() const override;

protected:
	grid_keyboard_device(
			const machine_config &mconfig,
			device_type type,
			char const *tag,
			device_t *owner,
			u32 clock);
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void key_make(u8 row, u8 column) override;
	virtual void key_repeat(u8 row, u8 column) override;
	virtual void send_key(u16 code);
	virtual bool translate(u8 code, u16 &translated) const;

	required_ioport m_config;
	required_ioport m_modifiers;

private:
	virtual void will_scan_row(u8 row) override;

	void typematic();
	void send_translated(u8 code);
	attotime typematic_delay() const;
	attotime typematic_period() const;

	u16             m_last_modifiers;
	output_delegate m_keyboard_cb;
};

#endif // MAME_MACHINE_GRIDKEYB_H
