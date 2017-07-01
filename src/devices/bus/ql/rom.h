// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Sinclair QL ROM cartridge port emulation

**********************************************************************

                              A     B
                              *  1  *   VDD
                        A12   *  2  *   A14
                         A7   *  3  *   A13
                         A6   *  4  *   A8
                         A5   *  5  *   A9
                       SLOT      6      SLOT
                         A4   *  7  *   A11
                         A3   *  8  *   ROMOEH
                         A2   *  9  *   A10
                         A1   * 10  *   A15
                         A0   * 11  *   D7
                         D0   * 12  *   D6
                         D1   * 13  *   D5
                         D2   * 14  *   D4
                        GND   * 15  *   D3

**********************************************************************/

#pragma once

#ifndef __QL_ROM_CARTRIDGE_SLOT__
#define __QL_ROM_CARTRIDGE_SLOT__

#include "emu.h"
#include "softlist_dev.h"


//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_QL_ROM_CARTRIDGE_SLOT_ADD(_tag, _slot_intf, _def_slot) \
	MCFG_DEVICE_ADD(_tag, QL_ROM_CARTRIDGE_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, false)



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> device_ql_rom_cartridge_card_interface

class ql_rom_cartridge_slot_t;

class device_ql_rom_cartridge_card_interface : public device_slot_card_interface
{
	friend class ql_rom_cartridge_slot_t;

public:
	// construction/destruction
	device_ql_rom_cartridge_card_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_ql_rom_cartridge_card_interface();

	virtual void romoeh_w(int state) { m_romoeh = state; }
	virtual uint8_t read(address_space &space, offs_t offset, uint8_t data) { return data; }
	virtual void write(address_space &space, offs_t offset, uint8_t data) { }

protected:
	ql_rom_cartridge_slot_t *m_slot;

	optional_shared_ptr<uint8_t> m_rom;

	int m_romoeh;
};


// ======================> ql_rom_cartridge_slot_t

class ql_rom_cartridge_slot_t : public device_t,
								public device_slot_interface,
								public device_image_interface
{
public:
	// construction/destruction
	ql_rom_cartridge_slot_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// computer interface
	uint8_t read(address_space &space, offs_t offset, uint8_t data) { if (m_card) data = m_card->read(space, offset, data); return data; }
	void write(address_space &space, offs_t offset, uint8_t data) { if (m_card) m_card->write(space, offset, data); }
	DECLARE_WRITE_LINE_MEMBER( romoeh_w ) { if (m_card) m_card->romoeh_w(state); }

protected:
	// device-level overrides
	virtual void device_config_complete() override { update_names(); }
	virtual void device_start() override;

	// image-level overrides
	virtual image_init_result call_load() override;
	virtual const software_list_loader &get_software_list_loader() const override { return rom_software_list_loader::instance(); }

	virtual iodevice_t image_type() const override { return IO_CARTSLOT; }

	virtual bool is_readable()  const override { return 1; }
	virtual bool is_writeable() const override { return 0; }
	virtual bool is_creatable() const override { return 0; }
	virtual bool must_be_loaded() const override { return 0; }
	virtual bool is_reset_on_load() const override { return 1; }
	virtual const char *image_interface() const override { return "ql_cart"; }
	virtual const char *file_extensions() const override { return "rom,bin"; }

	// slot interface overrides
	virtual std::string get_default_card_software() override;

	device_ql_rom_cartridge_card_interface *m_card;
};


// device type definition
extern const device_type QL_ROM_CARTRIDGE_SLOT;

SLOT_INTERFACE_EXTERN( ql_rom_cartridge_cards );



#endif
