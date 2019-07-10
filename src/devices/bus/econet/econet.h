// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Acorn Computers Econet local area network emulation

**********************************************************************/

#ifndef MAME_BUS_ECONET_ECONET_H
#define MAME_BUS_ECONET_ECONET_H

#pragma once




//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define ECONET_TAG          "econet"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> econet_device

class device_econet_interface;

class econet_device : public device_t
{
public:
	// construction/destruction
	econet_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto clk_wr_callback() { return m_write_clk.bind(); }
	auto data_wr_callback() { return m_write_data.bind(); }

	void add_device(device_t *target, int address);

	// writes for host (driver_device)
	DECLARE_WRITE_LINE_MEMBER( host_clk_w );
	DECLARE_WRITE_LINE_MEMBER( host_data_w );

	// writes for peripherals (device_t)
	void clk_w(device_t *device, int state);
	void data_w(device_t *device, int state);

protected:
	enum
	{
		CLK = 0,
		DATA,
		SIGNAL_COUNT
	};

	// device-level overrides
	virtual void device_start() override;
	virtual void device_stop() override;

	class daisy_entry
	{
	public:
		daisy_entry(device_t *device);
		daisy_entry *next() const { return m_next; }

		daisy_entry *               m_next;         // next device
		device_t *                  m_device;       // associated device
		device_econet_interface *   m_interface;    // associated device's daisy interface

		int m_line[SIGNAL_COUNT];
	};

	simple_list<daisy_entry> m_device_list;

private:
	devcb_write_line   m_write_clk;
	devcb_write_line   m_write_data;

	inline void set_signal(device_t *device, int signal, int state);
	inline int get_signal(int signal);

	int m_line[SIGNAL_COUNT];
};


// ======================> econet_slot_device

class econet_slot_device : public device_t,
							public device_slot_interface
{
public:
	// construction/destruction
	econet_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;

	// inline configuration
	void set_slot(int address) { m_address = address; }

private:
	// configuration
	uint8_t m_address;
	econet_device  *m_econet;
};


// ======================> device_econet_interface

class device_econet_interface : public device_slot_card_interface
{
	friend class econet_device;
	template <class ElementType> friend class simple_list;

public:
	device_econet_interface *next() const { return m_next; }

	virtual void econet_clk(int state) = 0;
	virtual void econet_data(int state) = 0;

protected:
	// construction/destruction
	device_econet_interface(const machine_config &mconfig, device_t &device);

	econet_device  *m_econet;
	uint8_t m_address;

private:
	device_econet_interface *m_next;
};


// device type definition
DECLARE_DEVICE_TYPE(ECONET,      econet_device)
DECLARE_DEVICE_TYPE(ECONET_SLOT, econet_slot_device)

void econet_devices(device_slot_interface &device);

#endif // MAME_BUS_ECONET_ECONET_H
