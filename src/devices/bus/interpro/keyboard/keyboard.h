// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay
#ifndef MAME_BUS_INTERPRO_KEYBOARD_KEYBOARD_H
#define MAME_BUS_INTERPRO_KEYBOARD_KEYBOARD_H

#pragma once

class device_interpro_keyboard_port_interface;

class interpro_keyboard_port_device : public device_t, public device_slot_interface
{
	friend class device_interpro_keyboard_port_interface;

public:
	template <typename T>
	interpro_keyboard_port_device(machine_config const &mconfig, char const *tag, device_t *owner, T &&slot_options, const char *default_option)
		: interpro_keyboard_port_device(mconfig, tag, owner)
	{
		option_reset();
		slot_options(*this);
		set_default_option(default_option);
		set_fixed(false);
	}

	interpro_keyboard_port_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock = 0);

	// callback configuration
	auto rxd_handler_cb() { return m_rxd_handler.bind(); }

	// input lines
	DECLARE_WRITE_LINE_MEMBER(write_txd);

protected:
	virtual void device_start() override;
	virtual void device_config_complete() override;

private:
	devcb_write_line m_rxd_handler;

	device_interpro_keyboard_port_interface *m_dev;
};

class device_interpro_keyboard_port_interface : public device_slot_card_interface
{
	friend class interpro_keyboard_port_device;

public:
	// input lines
	virtual DECLARE_WRITE_LINE_MEMBER(input_txd) = 0;
	DECLARE_WRITE_LINE_MEMBER(output_rxd) { m_port->m_rxd_handler(state); }

protected:
	device_interpro_keyboard_port_interface(machine_config const &mconfig, device_t &device);

private:
	interpro_keyboard_port_device *m_port;
};

DECLARE_DEVICE_TYPE(INTERPRO_KEYBOARD_PORT, interpro_keyboard_port_device)

void interpro_keyboard_devices(device_slot_interface &device);

#endif // MAME_BUS_INTERPRO_KEYBOARD_KEYBOARD_H
