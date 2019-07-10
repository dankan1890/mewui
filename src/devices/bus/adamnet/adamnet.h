// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Coleco ADAMnet bus emulation

**********************************************************************/

#ifndef MAME_BUS_ADAMNET_ADAMNET_H
#define MAME_BUS_ADAMNET_ADAMNET_H

#pragma once




//**************************************************************************
//  CONSTANTS
//**************************************************************************

#define ADAMNET_TAG     "adamnet"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class device_adamnet_card_interface;

// ======================> adamnet_device

class adamnet_device : public device_t
{
public:
	// construction/destruction
	adamnet_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void add_device(device_t *target);

	DECLARE_READ_LINE_MEMBER( rxd_r );
	int rxd_r(device_t *device);
	DECLARE_WRITE_LINE_MEMBER( txd_w );
	void txd_w(device_t *device, int state);

	DECLARE_READ_LINE_MEMBER( reset_r );
	DECLARE_WRITE_LINE_MEMBER( reset_w );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_stop() override;

private:
	class daisy_entry
	{
	public:
		daisy_entry(device_t *device);
		daisy_entry *next() const { return m_next; }

		daisy_entry *               m_next;         // next device
		device_t *                  m_device;       // associated device
		device_adamnet_card_interface * m_interface;    // associated device's daisy interface

		int m_txd;
	};

	simple_list<daisy_entry> m_device_list;

	int m_txd;
	int m_reset;
};


// ======================> adamnet_slot_device

class adamnet_slot_device : public device_t,
							public device_slot_interface
{
public:
	// construction/destruction
	template <typename T>
	adamnet_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, T &&opts, char const *dflt)
		: adamnet_slot_device(mconfig, tag, owner, (uint32_t)0)
	{
		option_reset();
		opts(*this);
		set_default_option(dflt);
		set_fixed(false);
	}
	adamnet_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;

protected:
	// configuration
	adamnet_device  *m_bus;
};


// ======================> device_adamnet_card_interface

class device_adamnet_card_interface : public device_slot_card_interface
{
	friend class adamnet_device;

public:
	// construction/destruction
	virtual ~device_adamnet_card_interface();

	virtual void adamnet_reset_w(int state) = 0;

protected:
	device_adamnet_card_interface(const machine_config &mconfig, device_t &device);

	adamnet_device  *m_bus;
};


// device type definitions
DECLARE_DEVICE_TYPE(ADAMNET,      adamnet_device)
DECLARE_DEVICE_TYPE(ADAMNET_SLOT, adamnet_slot_device)


void adamnet_devices(device_slot_interface &device);

#endif // MAME_BUS_ADAMNET_ADAMNET_H
