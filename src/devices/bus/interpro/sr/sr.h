// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay

#ifndef MAME_BUS_INTERPRO_SR_SR_H
#define MAME_BUS_INTERPRO_SR_SR_H

#pragma once

class interpro_bus_device : public device_t
{
public:
	// space configuration
	template <typename T> void set_main_space(T &&tag, int spacenum) { m_main_space.set_tag(std::forward<T>(tag), spacenum); }
	template <typename T> void set_io_space(T &&tag, int spacenum) { m_io_space.set_tag(std::forward<T>(tag), spacenum); }

	// callback configuration
	auto out_irq0_cb() { return m_out_irq0_cb.bind(); }
	auto out_irq1_cb() { return m_out_irq1_cb.bind(); }
	auto out_irq2_cb() { return m_out_irq2_cb.bind(); }
	auto out_irq3_cb() { return m_out_irq3_cb.bind(); }

	DECLARE_WRITE_LINE_MEMBER(irq0_w) { m_out_irq0_cb(state); }
	DECLARE_WRITE_LINE_MEMBER(irq1_w) { m_out_irq1_cb(state); }
	DECLARE_WRITE_LINE_MEMBER(irq2_w) { m_out_irq2_cb(state); }
	DECLARE_WRITE_LINE_MEMBER(irq3_w) { m_out_irq3_cb(state); }

protected:
	// construction/destruction
	interpro_bus_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock)
		: device_t(mconfig, type, tag, owner, clock)
		, m_main_space(*this, finder_base::DUMMY_TAG, -1)
		, m_io_space(*this, finder_base::DUMMY_TAG, -1)
		, m_out_irq0_cb(*this)
		, m_out_irq1_cb(*this)
		, m_out_irq2_cb(*this)
		, m_out_irq3_cb(*this)
	{
	}

	// device-level overrides
	virtual void device_resolve_objects() override;

	// internal state
	required_address_space m_main_space;
	required_address_space m_io_space;

private:
	devcb_write_line m_out_irq0_cb;
	devcb_write_line m_out_irq1_cb;
	devcb_write_line m_out_irq2_cb;
	devcb_write_line m_out_irq3_cb;
};

class device_cbus_card_interface;

class cbus_bus_device : public interpro_bus_device
{
public:
	// construction/destruction
	cbus_bus_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	static const u32 CBUS_BASE = 0x87000000;
	static const u32 CBUS_SIZE = 0x01000000;
	static const u32 CBUS_STRIDE = 0x08000000;
	static const int CBUS_COUNT = 16;

	// installation function for card devices
	template <typename T> void install_card(T &device, memory_region *idprom, void (T::*map)(address_map &map))
	{
		// record the device in the next free slot
		m_slot[m_slot_count] = &device;

		// compute slot base address
		offs_t start = CBUS_BASE + m_slot_count * CBUS_STRIDE;
		offs_t end = start + (CBUS_SIZE - 1);

		// install the idprom region
		read32_delegate idprom_r([idprom](address_space &space, offs_t offset, u32 mem_mask) { return idprom->as_u32(offset); }, idprom->name());
		m_main_space->install_read_handler(start, start | 0x7f, idprom_r);
		m_io_space->install_read_handler(start, start | 0x7f, idprom_r);

		// install the device address map
		m_main_space->install_device(start, end, device, map);
		m_io_space->install_device(start, end, device, map);

		m_slot_count++;
	}

protected:
	// device-level overrides
	virtual void device_start() override;

private:
	device_cbus_card_interface *m_slot[CBUS_COUNT];
	int m_slot_count;
};

class cbus_slot_device : public device_t, public device_slot_interface
{
public:
	// construction/destruction
	template <typename T, typename U>
	cbus_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock, T &&bus_tag, U &&slot_options, const char *default_option, const bool fixed)
		: cbus_slot_device(mconfig, tag, owner, clock)
	{
		m_bus.set_tag(std::forward<T>(bus_tag));
		option_reset();
		slot_options(*this);
		set_default_option(default_option);
		set_fixed(fixed);
	}
	cbus_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	// device-level overrides
	virtual void device_resolve_objects() override;
	virtual void device_start() override;

private:
	required_device<cbus_bus_device> m_bus;
};

class device_cbus_card_interface : public device_slot_card_interface
{
protected:
	friend class cbus_slot_device;

public:
	DECLARE_WRITE_LINE_MEMBER(irq0) { m_bus->irq0_w(state); }
	DECLARE_WRITE_LINE_MEMBER(irq1) { m_bus->irq1_w(state); }
	DECLARE_WRITE_LINE_MEMBER(irq2) { m_bus->irq2_w(state); }
	DECLARE_WRITE_LINE_MEMBER(irq3) { m_bus->irq3_w(state); }

protected:
	device_cbus_card_interface(const machine_config &mconfig, device_t &device, const char *idprom_region = "idprom")
		: device_slot_card_interface(mconfig, device)
		, m_bus(nullptr)
		, m_idprom_region(idprom_region)
	{
	}

	virtual void map(address_map &map) = 0;

private:
	void set_bus_device(cbus_bus_device &bus_device);

	cbus_bus_device *m_bus;
	const char *const m_idprom_region;
};

class device_srx_card_interface;

class srx_bus_device : public interpro_bus_device
{
public:
	// construction/destruction
	srx_bus_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	static const u32 SRX_BASE = 0x8f000000;
	static const u32 SRX_SIZE = 0x8000;
	static const int SRX_COUNT = 32;

	// installation function for card devices
	template <typename T> void install_card(T &device, memory_region *idprom, void (T::*map)(address_map &map))
	{
		// record the device in the next free slot
		m_slot[m_slot_count] = &device;

		// compute slot base address
		offs_t start = SRX_BASE + m_slot_count * SRX_SIZE;
		offs_t end = start + (SRX_SIZE - 1);

		// install the idprom region
		read32_delegate idprom_r([idprom](address_space &space, offs_t offset, u32 mem_mask) { return idprom->as_u32(offset); }, idprom->name());
		m_main_space->install_read_handler(start | 0x7f80, start | 0x7fff, idprom_r);
		m_io_space->install_read_handler(start | 0x7f80, start | 0x7fff, idprom_r);

		// install the device address map
		m_main_space->install_device(start, end, device, map);
		m_io_space->install_device(start, end, device, map);

		m_slot_count++;
	}

	template <typename T> T *get_card()
	{
		for (auto device : m_slot)
			if (dynamic_cast<T *>(device) != nullptr)
				return dynamic_cast<T *>(device);

		return nullptr;
	}

	template <typename T> void install_map(T &device, offs_t start, offs_t end, void (T::*map)(address_map &map))
	{
		// install the device address map
		m_main_space->install_device(start, end, device, map);
		m_io_space->install_device(start, end, device, map);
	}

protected:
	// device-level overrides
	virtual void device_start() override;

private:
	device_srx_card_interface *m_slot[SRX_COUNT];
	int m_slot_count;
};

class srx_slot_device : public device_t, public device_slot_interface
{
public:
	// construction/destruction
	template <typename T, typename U>
	srx_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock, T &&bus_tag, U &&slot_options, const char *default_option, const bool fixed)
		: srx_slot_device(mconfig, tag, owner, clock)
	{
		m_bus.set_tag(std::forward<T>(bus_tag));
		option_reset();
		slot_options(*this);
		set_default_option(default_option);
		set_fixed(fixed);
	}
	srx_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	// device-level overrides
	virtual void device_resolve_objects() override;
	virtual void device_start() override;

private:
	required_device<srx_bus_device> m_bus;
};

class device_srx_card_interface : public device_slot_card_interface
{
protected:
	friend class srx_slot_device;

public:
	DECLARE_WRITE_LINE_MEMBER(irq0) { m_bus->irq0_w(state); }
	DECLARE_WRITE_LINE_MEMBER(irq1) { m_bus->irq1_w(state); }
	DECLARE_WRITE_LINE_MEMBER(irq2) { m_bus->irq2_w(state); }
	DECLARE_WRITE_LINE_MEMBER(irq3) { m_bus->irq3_w(state); }

protected:
	device_srx_card_interface(const machine_config &mconfig, device_t &device, const char *idprom_region = "idprom")
		: device_slot_card_interface(mconfig, device)
		, m_bus(nullptr)
		, m_idprom_region(idprom_region)
	{
	}

	virtual void map(address_map &map) = 0;

// FIXME: when we sort out the EDGE cards this stuff will be private
//private:
	void set_bus_device(srx_bus_device &bus_device);

	srx_bus_device *m_bus;
	const char *const m_idprom_region;
};

DECLARE_DEVICE_TYPE(CBUS_BUS, cbus_bus_device)
DECLARE_DEVICE_TYPE(CBUS_SLOT, cbus_slot_device)
DECLARE_DEVICE_TYPE(SRX_BUS, srx_bus_device)
DECLARE_DEVICE_TYPE(SRX_SLOT, srx_slot_device)

#endif // MAME_BUS_INTERPRO_SR_SR_H
