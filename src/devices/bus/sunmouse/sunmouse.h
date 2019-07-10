// license:BSD-3-Clause
// copyright-holders:Vas Crabb
#ifndef MAME_BUS_SUNMOUSE_SUNMOUSE_H
#define MAME_BUS_SUNMOUSE_SUNMOUSE_H

#pragma once

#include "diserial.h"


class device_sun_mouse_port_interface;


class sun_mouse_port_device : public device_t, public device_slot_interface
{
	friend class device_sun_mouse_port_interface;

public:
	template <typename T>
	sun_mouse_port_device(machine_config const &mconfig, char const *tag, device_t *owner, T &&opts, char const *dflt)
		: sun_mouse_port_device(mconfig, tag, owner, 0)
	{
		option_reset();
		opts(*this);
		set_default_option(dflt);
		set_fixed(false);
	}
	sun_mouse_port_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock = 0);
	virtual ~sun_mouse_port_device();

	// configuration helpers
	auto rxd_handler() { return m_rxd_handler.bind(); }

	DECLARE_WRITE_LINE_MEMBER( write_txd );

	DECLARE_READ_LINE_MEMBER( rxd_r ) { return m_rxd; }

protected:
	sun_mouse_port_device(machine_config const &mconfig, device_type type, char const *tag, device_t *owner, uint32_t clock);

	virtual void device_config_complete() override;
	virtual void device_validity_check(validity_checker &valid) const override;
	virtual void device_resolve_objects() override;
	virtual void device_start() override;

	int m_rxd;

	devcb_write_line m_rxd_handler;

private:
	device_sun_mouse_port_interface *m_dev;
};


class device_sun_mouse_port_interface : public device_slot_card_interface
{
	friend class sun_mouse_port_device;

public:
	virtual ~device_sun_mouse_port_interface() override;

protected:
	device_sun_mouse_port_interface(machine_config const &mconfig, device_t &device);

	virtual DECLARE_WRITE_LINE_MEMBER( input_txd ) { }

	DECLARE_WRITE_LINE_MEMBER( output_rxd ) { m_port->m_rxd_handler(m_port->m_rxd = state ? 0 : 1); }

	sun_mouse_port_device *m_port;

	static constexpr int START_BIT_COUNT = 1;
	static constexpr int DATA_BIT_COUNT = 8;
	static constexpr device_serial_interface::parity_t PARITY = device_serial_interface::PARITY_NONE;
	static constexpr device_serial_interface::stop_bits_t STOP_BITS = device_serial_interface::STOP_BITS_1;
	static constexpr int BAUD = 1'200;
};


DECLARE_DEVICE_TYPE(SUNMOUSE_PORT, sun_mouse_port_device)


void default_sun_mouse_devices(device_slot_interface &device);

#endif // MAME_BUS_SUNMOUSE_SUNMOUSE_H
