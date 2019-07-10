// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Coleco Adam Expansion Port emulation

**********************************************************************/

#ifndef MAME_BUS_ADAM_EXP_H
#define MAME_BUS_ADAM_EXP_H

#pragma once

#include "softlist_dev.h"


//**************************************************************************
//  CONSTANTS
//**************************************************************************

#define ADAM_LEFT_EXPANSION_SLOT_TAG        "slot1"
#define ADAM_CENTER_EXPANSION_SLOT_TAG      "slot2"
#define ADAM_RIGHT_EXPANSION_SLOT_TAG       "slot3"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> adam_expansion_slot_device

class device_adam_expansion_slot_card_interface;

class adam_expansion_slot_device : public device_t,
									public device_slot_interface,
									public device_image_interface
{
public:
	// construction/destruction
	template <typename T>
	adam_expansion_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock, T &&opts, char const *dflt)
		: adam_expansion_slot_device(mconfig, tag, owner, clock)
	{
		option_reset();
		opts(*this);
		set_default_option(dflt);
		set_fixed(false);
	}
	adam_expansion_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~adam_expansion_slot_device() { }

	auto irq() { return m_write_irq.bind(); }

	// computer interface
	uint8_t bd_r(offs_t offset, uint8_t data, int bmreq, int biorq, int aux_rom_cs, int cas1, int cas2);
	void bd_w(offs_t offset, uint8_t data, int bmreq, int biorq, int aux_rom_cs, int cas1, int cas2);

	// cartridge interface
	DECLARE_WRITE_LINE_MEMBER( irq_w ) { m_write_irq(state); }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// image-level overrides
	virtual image_init_result call_load() override;
	virtual const software_list_loader &get_software_list_loader() const override { return rom_software_list_loader::instance(); }

	virtual iodevice_t image_type() const override { return IO_CARTSLOT; }

	virtual bool is_readable()  const override { return 1; }
	virtual bool is_writeable() const override { return 0; }
	virtual bool is_creatable() const override { return 0; }
	virtual bool must_be_loaded() const override { return 0; }
	virtual bool is_reset_on_load() const override { return 1; }
	virtual const char *image_interface() const override { return "adam_rom"; }
	virtual const char *file_extensions() const override { return "bin,rom"; }

	// slot interface overrides
	virtual std::string get_default_card_software(get_default_card_software_hook &hook) const override;

	devcb_write_line   m_write_irq;

	device_adam_expansion_slot_card_interface *m_card;
};


// ======================> device_adam_expansion_slot_card_interface

class device_adam_expansion_slot_card_interface : public device_slot_card_interface
{
	friend class adam_expansion_slot_device;

protected:
	// construction/destruction
	device_adam_expansion_slot_card_interface(const machine_config &mconfig, device_t &device);

	// runtime
	virtual uint8_t adam_bd_r(offs_t offset, uint8_t data, int bmreq, int biorq, int aux_rom_cs, int cas1, int cas2) { return data; }
	virtual void adam_bd_w(offs_t offset, uint8_t data, int bmreq, int biorq, int aux_rom_cs, int cas1, int cas2) { }

	adam_expansion_slot_device *m_slot;

	optional_shared_ptr<uint8_t> m_rom;
};


// device type definition
DECLARE_DEVICE_TYPE(ADAM_EXPANSION_SLOT, adam_expansion_slot_device)


void adam_slot1_devices(device_slot_interface &device);
void adam_slot2_devices(device_slot_interface &device);
void adam_slot3_devices(device_slot_interface &device);

#endif // MAME_BUS_ADAM_EXP_H
