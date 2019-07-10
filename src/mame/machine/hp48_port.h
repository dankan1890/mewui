// license:BSD-3-Clause
// copyright-holders:Antoine Mine
/**********************************************************************

  Copyright (C) Antoine Mine' 2008

   Hewlett Packard HP48 S/SX & G/GX and HP49 G

**********************************************************************/

#ifndef MAME_MACHINE_HP84_PORT_H
#define MAME_MACHINE_HP84_PORT_H

#pragma once

/****************************** cards ********************************/

class hp48_state;

class hp48_port_image_device :  public device_t, public device_image_interface
{
public:
	// construction/destruction
	hp48_port_image_device(const machine_config &mconfig, const char *tag, device_t *owner, int module, int max_size)
		: hp48_port_image_device(mconfig, tag, owner, 0)
	{
		set_port_config(module, max_size);
	}

	hp48_port_image_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_port_config(int module, int max_size)
	{
		m_module = module;
		m_max_size = max_size;
	}

	// image-level overrides
	virtual iodevice_t image_type() const override { return IO_MEMCARD; }

	virtual bool is_readable()  const override { return 1; }
	virtual bool is_writeable() const override { return 1; }
	virtual bool is_creatable() const override { return 1; }
	virtual bool must_be_loaded() const override { return 0; }
	virtual bool is_reset_on_load() const override { return 0; }
	virtual const char *file_extensions() const override { return "crd"; }
	virtual const char *custom_instance_name() const override { return "port"; }
	virtual const char *custom_brief_instance_name() const override { return "p"; }

	virtual image_init_result call_load() override;
	virtual void call_unload() override;
	virtual image_init_result call_create(int format_type, util::option_resolution *format_options) override;

	uint32_t port_size() const { return m_port_size; }
	bool port_write() const { return m_port_write; }
	uint8_t *port_data() const { return m_port_data.get(); }

protected:
	// device-level overrides
	virtual void device_start() override;
private:
	void fill_port();
	void unfill_port();

	int m_module;               /* memory module where the port is visible */
	int m_max_size;             /* maximum size, in bytes 128 KB or 4 GB */

	uint32_t m_port_size;
	bool m_port_write;
	std::unique_ptr<uint8_t[]> m_port_data; // each uint8_t stores one nibble

	required_device<hp48_state> m_hp48;
};

// device type definition
DECLARE_DEVICE_TYPE(HP48_PORT, hp48_port_image_device)

#endif // MAME_MACHINE_HP84_PORT_H
