// license:BSD-3-Clause
// copyright-holders:smf
/***************************************************************************

    ataintf.h

    ATA Interface implementation.

***************************************************************************/

#ifndef MAME_MACHINE_ATAINTF_H
#define MAME_MACHINE_ATAINTF_H

#pragma once

#include "atadev.h"

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

// ======================> ata_slot_device

class ata_slot_device : public device_t,
						public device_slot_interface
{
public:
	// construction/destruction
	ata_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	device_ata_interface *dev() { return m_dev; }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_config_complete() override;

private:
	device_ata_interface *m_dev;
};

// device type definition
DECLARE_DEVICE_TYPE(ATA_SLOT, ata_slot_device)

void ata_devices(device_slot_interface &device);

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

/* ----- device interface ----- */

class abstract_ata_interface_device : public device_t
{
public:
	// static configuration helpers
	auto irq_handler() { return m_irq_handler.bind(); }
	auto dmarq_handler() { return m_dmarq_handler.bind(); }
	auto dasp_handler() { return m_dasp_handler.bind(); }

	template <typename T> abstract_ata_interface_device &set_slot_options(int index, T &&opts, const char *dflt, bool fixed)
	{
		ata_slot_device &dev = slot(index);
		dev.option_reset();
		opts(dev);
		dev.set_default_option(dflt);
		dev.set_fixed(fixed);
		return *this;
	}
	template <typename T> abstract_ata_interface_device &master(T &&opts, const char *dflt = nullptr, bool fixed = false)
	{
		set_slot(0, std::forward<T>(opts), dflt, fixed);
		return *this;
	}
	template <typename T> abstract_ata_interface_device &slave(T &&opts, const char *dflt = nullptr, bool fixed = false)
	{
		set_slot(1, std::forward<T>(opts), dflt, fixed);
		return *this;
	}
	template <typename T> abstract_ata_interface_device &options(T &&opts, const char *master_default = nullptr, const char *slave_default = nullptr, bool fixed = false)
	{
		set_slot_options(0, std::forward<T>(opts), master_default, fixed);
		set_slot_options(1, std::forward<T>(opts), slave_default, fixed);
		return *this;
	}

	ata_slot_device &slot(int index);
	virtual void set_default_ata_devices(const char* _master, const char* _slave);

	uint16_t read_dma();
	void write_dma(uint16_t data);
	DECLARE_WRITE_LINE_MEMBER(write_dmack);

protected:
	abstract_ata_interface_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	uint16_t internal_read_cs0(offs_t offset, uint16_t mem_mask = 0xffff);
	uint16_t internal_read_cs1(offs_t offset, uint16_t mem_mask = 0xffff);
	void internal_write_cs0(offs_t offset, uint16_t data, uint16_t mem_mask = 0xffff);
	void internal_write_cs1(offs_t offset, uint16_t data, uint16_t mem_mask = 0xffff);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;

	virtual void set_irq(int state);
	virtual void set_dmarq(int state);
	virtual void set_dasp(int state);

	enum : size_t
	{
		SLOT_MASTER,
		SLOT_SLAVE,

		SLOT_COUNT
	};

private:
	DECLARE_WRITE_LINE_MEMBER(irq0_write_line);
	DECLARE_WRITE_LINE_MEMBER(dmarq0_write_line);
	DECLARE_WRITE_LINE_MEMBER(dasp0_write_line);
	DECLARE_WRITE_LINE_MEMBER(pdiag0_write_line);

	DECLARE_WRITE_LINE_MEMBER(irq1_write_line);
	DECLARE_WRITE_LINE_MEMBER(dmarq1_write_line);
	DECLARE_WRITE_LINE_MEMBER(dasp1_write_line);
	DECLARE_WRITE_LINE_MEMBER(pdiag1_write_line);

	required_device_array<ata_slot_device, SLOT_COUNT> m_slot;
	int m_irq[SLOT_COUNT];
	int m_dmarq[SLOT_COUNT];
	int m_dasp[SLOT_COUNT];
	int m_pdiag[SLOT_COUNT];

	devcb_write_line m_irq_handler;
	devcb_write_line m_dmarq_handler;
	devcb_write_line m_dasp_handler;
};

class ata_interface_device : public abstract_ata_interface_device
{
public:
	ata_interface_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	template <typename T> ata_interface_device &master(T &&opts, const char *dflt = nullptr, bool fixed = false)
	{
		abstract_ata_interface_device::master(std::forward<T>(opts), dflt, fixed);
		return *this;
	}
	template <typename T> ata_interface_device &slave(T &&opts, const char *dflt = nullptr, bool fixed = false)
	{
		abstract_ata_interface_device::slave(std::forward<T>(opts), dflt, fixed);
		return *this;
	}
	template <typename T> ata_interface_device &options(T &&opts, const char *master_default = nullptr, const char *slave_default = nullptr, bool fixed = false)
	{
		abstract_ata_interface_device::options(std::forward<T>(opts), master_default, slave_default, fixed);
		return *this;
	}
	template <typename T> ata_interface_device &set_slot_options(int index, T &&opts, const char *dflt, bool fixed)
	{
		abstract_ata_interface_device::set_slot_options(index, std::forward<T>(opts), dflt, fixed);
		return *this;
	}

	uint16_t read_cs0(offs_t offset, uint16_t mem_mask = 0xffff) { return internal_read_cs0(offset, mem_mask); }
	uint16_t read_cs1(offs_t offset, uint16_t mem_mask = 0xffff) { return internal_read_cs1(offset, mem_mask); }
	void write_cs0(offs_t offset, uint16_t data, uint16_t mem_mask = 0xffff) { internal_write_cs0(offset, data, mem_mask); }
	void write_cs1(offs_t offset, uint16_t data, uint16_t mem_mask = 0xffff) { internal_write_cs1(offset, data, mem_mask); }

	DECLARE_READ16_MEMBER(cs0_r) { return read_cs0(offset, mem_mask); }
	DECLARE_READ16_MEMBER(cs1_r) { return read_cs1(offset, mem_mask); }
	DECLARE_WRITE16_MEMBER(cs0_w) { write_cs0(offset, data, mem_mask); }
	DECLARE_WRITE16_MEMBER(cs1_w) { write_cs1(offset, data, mem_mask); }
};

DECLARE_DEVICE_TYPE(ATA_INTERFACE, ata_interface_device)

#endif // MAME_MACHINE_ATAINTF_H
