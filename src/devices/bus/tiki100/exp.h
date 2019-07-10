// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    TIKI-100 expansion bus emulation

**********************************************************************


**********************************************************************/

#ifndef MAME_BUS_TIKI100_EXP_H
#define MAME_BUS_TIKI100_EXP_H

#pragma once

#include "machine/z80daisy.h"



//**************************************************************************
//  CONSTANTS
//**************************************************************************

#define TIKI100_BUS_TAG      "tiki100bus"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> tiki100_bus_slot_device

class tiki100_bus_device;
class tiki100_bus_slot_device;


// ======================> device_tiki100bus_card_interface

class device_tiki100bus_card_interface : public device_slot_card_interface
{
	friend class tiki100_bus_device;
	template <class ElementType> friend class simple_list;

public:
	device_tiki100bus_card_interface *next() const { return m_next; }

	// memory access
	virtual uint8_t mrq_r(offs_t offset, uint8_t data, bool &mdis) { mdis = 1; return data; }
	virtual void mrq_w(offs_t offset, uint8_t data) { }

	// I/O access
	virtual uint8_t iorq_r(offs_t offset, uint8_t data) { return data; }
	virtual void iorq_w(offs_t offset, uint8_t data) { }

	virtual void busak_w(int state) { m_busak = state; }

	// Z80 daisy chain
	virtual int z80daisy_irq_state() { return 0; }
	virtual int z80daisy_irq_ack() { return 0; }
	virtual void z80daisy_irq_reti() { }

protected:
	// construction/destruction
	device_tiki100bus_card_interface(const machine_config &mconfig, device_t &device);

	tiki100_bus_device  *m_bus;
	tiki100_bus_slot_device *m_slot;
	int m_busak;

private:
	device_tiki100bus_card_interface *m_next;
};


// ======================> tiki100_bus_slot_device

class tiki100_bus_slot_device : public device_t,
							public device_slot_interface,
							public device_z80daisy_interface
{
public:
	// construction/destruction
	template <typename T>
	tiki100_bus_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&opts, const char *dflt)
		: tiki100_bus_slot_device(mconfig, tag, owner, 0)
	{
		option_reset();
		opts(*this);
		set_default_option(dflt);
		set_fixed(false);
	}

	tiki100_bus_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	// device-level overrides
	virtual void device_start() override;

protected:
	// device_z80daisy_interface overrides
	virtual int z80daisy_irq_state() override { return get_card_device() ? m_card->z80daisy_irq_state() : 0; }
	virtual int z80daisy_irq_ack() override { return get_card_device() ? m_card->z80daisy_irq_ack() : 0; }
	virtual void z80daisy_irq_reti() override { if (get_card_device()) m_card->z80daisy_irq_reti(); }

private:
	// configuration
	tiki100_bus_device  *m_bus;
	device_tiki100bus_card_interface *m_card;
};


// device type definition
DECLARE_DEVICE_TYPE(TIKI100_BUS_SLOT, tiki100_bus_slot_device)


// ======================> tiki100_bus_device

class tiki100_bus_device : public device_t
{
public:
	// construction/destruction
	tiki100_bus_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	~tiki100_bus_device() { m_device_list.detach_all(); }

	auto irq_wr_callback() { return m_irq_cb.bind(); }
	auto nmi_wr_callback() { return m_nmi_cb.bind(); }
	auto busrq_wr_callback() { return m_busrq_cb.bind(); }
	auto mrq_rd_callback() { return m_in_mrq_cb.bind(); }
	auto mrq_wr_callback() { return m_out_mrq_cb.bind(); }

	void add_card(device_tiki100bus_card_interface *card);

	// computer interface
	uint8_t mrq_r(offs_t offset, uint8_t data, bool &mdis);
	void mrq_w(offs_t offset, uint8_t data);

	uint8_t iorq_r(offs_t offset, uint8_t data);
	void iorq_w(offs_t offset, uint8_t data);

	DECLARE_WRITE_LINE_MEMBER( busak_w );

	// peripheral interface
	DECLARE_WRITE_LINE_MEMBER( irq_w ) { m_irq_cb(state); }
	DECLARE_WRITE_LINE_MEMBER( nmi_w ) { m_nmi_cb(state); }
	DECLARE_WRITE_LINE_MEMBER( busrq_w ) { m_busrq_cb(state); }
	uint8_t exin_mrq_r(offs_t offset) { return m_in_mrq_cb(offset); }
	void exin_mrq_w(offs_t offset, uint8_t data) { m_out_mrq_cb(offset, data); }

protected:
	// device-level overrides
	virtual void device_start() override;

private:
	devcb_write_line   m_irq_cb;
	devcb_write_line   m_nmi_cb;
	devcb_write_line   m_busrq_cb;
	devcb_read8        m_in_mrq_cb;
	devcb_write8       m_out_mrq_cb;

	simple_list<device_tiki100bus_card_interface> m_device_list;
};


// device type definition
DECLARE_DEVICE_TYPE(TIKI100_BUS, tiki100_bus_device)



void tiki100_cards(device_slot_interface &device);

#endif // MAME_BUS_TIKI100_EXP_H
