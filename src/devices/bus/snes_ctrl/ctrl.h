// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Nintendo Super Famicom & SNES controller port emulation

**********************************************************************/

#ifndef MAME_BUS_SNES_CTRL_CTRL_H
#define MAME_BUS_SNES_CTRL_CTRL_H

#pragma once


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class snes_control_port_device;

// ======================> device_snes_control_port_interface

class device_snes_control_port_interface : public device_slot_card_interface
{
public:
	// construction/destruction
	device_snes_control_port_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_snes_control_port_interface();

	virtual uint8_t read_pin4() { return 0; }
	virtual uint8_t read_pin5() { return 0; }
	virtual void write_pin6(uint8_t data) { }
	virtual void write_strobe(uint8_t data) { }
	virtual void port_poll() { }

protected:
	snes_control_port_device *m_port;
};

typedef device_delegate<bool (int16_t x, int16_t y)> snesctrl_onscreen_delegate;
#define SNESCTRL_ONSCREEN_CB(name)  bool name(int16_t x, int16_t y)

typedef device_delegate<void (int16_t x, int16_t y)> snesctrl_gunlatch_delegate;
#define SNESCTRL_GUNLATCH_CB(name)  void name(int16_t x, int16_t y)

// ======================> snes_control_port_device

class snes_control_port_device : public device_t, public device_slot_interface
{
public:
	// construction/destruction
	template <typename T>
	snes_control_port_device(machine_config const &mconfig, char const *tag, device_t *owner, T &&opts, char const *dflt)
		: snes_control_port_device(mconfig, tag, owner, 0)
	{
		option_reset();
		opts(*this);
		set_default_option(dflt);
		set_fixed(false);
	}
	snes_control_port_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);
	virtual ~snes_control_port_device();

	template <typename... T> void set_onscreen_callback(T &&... args) { m_onscreen_cb = snesctrl_onscreen_delegate(std::forward<T>(args)...); }
	template <typename... T> void set_gunlatch_callback(T &&... args) { m_gunlatch_cb = snesctrl_gunlatch_delegate(std::forward<T>(args)...); }

	uint8_t read_pin4();
	uint8_t read_pin5();
	void write_pin6(uint8_t data);
	void write_strobe(uint8_t data);
	void port_poll();

	bool onscreen_cb(int16_t x, int16_t y) { return m_onscreen_cb.isnull() ? true : m_onscreen_cb(x, y); }
	void gunlatch_cb(int16_t x, int16_t y) { if (!m_gunlatch_cb.isnull()) m_gunlatch_cb(x, y); }

protected:
	// device-level overrides
	virtual void device_start() override;

	snesctrl_onscreen_delegate m_onscreen_cb;
	snesctrl_gunlatch_delegate m_gunlatch_cb;

	device_snes_control_port_interface *m_device;
};


// device type definition
DECLARE_DEVICE_TYPE(SNES_CONTROL_PORT, snes_control_port_device)

void snes_control_port_devices(device_slot_interface &device);


#endif // MAME_BUS_SNES_CTRL_CTRL_H
