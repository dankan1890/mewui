// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
#ifndef __VCS_SLOT_H
#define __VCS_SLOT_H

#include "softlist_dev.h"


/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/


/* PCB */
enum
{
	A26_2K = 0,
	A26_4K,
	A26_F4,
	A26_F6,
	A26_F8,
	A26_F8SW,
	A26_FA,
	A26_FE,
	A26_3E,     // to test
	A26_3F,
	A26_E0,
	A26_E7,
	A26_UA,
	A26_DC,
	A26_CV,
	A26_FV,
	A26_JVP,    // to test
	A26_32IN1,
	A26_8IN1,
	A26_4IN1,
	A26_DPC,
	A26_SS,
	A26_CM,
	A26_X07,
	A26_HARMONY,
};


// ======================> device_vcs_cart_interface

class device_vcs_cart_interface : public device_slot_card_interface
{
public:
	// construction/destruction
	device_vcs_cart_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_vcs_cart_interface();

	// reading from ROM
	virtual DECLARE_READ8_MEMBER(read_rom) { return 0xff; }
	// writing to RAM chips (sometimes it is in a different range than write_bank!)
	virtual DECLARE_WRITE8_MEMBER(write_ram) {}

	// read/write to bankswitch address
	virtual DECLARE_READ8_MEMBER(read_bank) { return 0xff; }
	virtual DECLARE_WRITE8_MEMBER(write_bank) {}

	// direct update handler
	virtual DECLARE_DIRECT_UPDATE_MEMBER(cart_opbase) { return address; }

	virtual void setup_addon_ptr(uint8_t *ptr) {}

	void rom_alloc(uint32_t size, const char *tag);
	void ram_alloc(uint32_t size);
	uint8_t* get_rom_base() { return m_rom; }
	uint8_t*  get_ram_base() { return &m_ram[0]; }
	uint32_t  get_rom_size() { return m_rom_size; }
	uint32_t  get_ram_size() { return m_ram.size(); }

protected:
	// internal state
	uint8_t *m_rom;
	uint32_t m_rom_size;
	std::vector<uint8_t> m_ram;
};


// ======================> vcs_cart_slot_device

class vcs_cart_slot_device : public device_t,
								public device_image_interface,
								public device_slot_interface
{
public:
	// construction/destruction
	vcs_cart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~vcs_cart_slot_device();

	// device-level overrides
	virtual void device_start() override;
	virtual void device_config_complete() override;

	// image-level overrides
	virtual image_init_result call_load() override;
	virtual void call_unload() override;
	virtual const software_list_loader &get_software_list_loader() const override { return rom_software_list_loader::instance(); }

	int get_cart_type() { return m_type; };
	int identify_cart_type(uint8_t *ROM, uint32_t len);

	virtual iodevice_t image_type() const override { return IO_CARTSLOT; }
	virtual bool is_readable()  const override { return 1; }
	virtual bool is_writeable() const override { return 0; }
	virtual bool is_creatable() const override { return 0; }
	virtual bool must_be_loaded() const override { return 1; }
	virtual bool is_reset_on_load() const override { return 1; }
	virtual const char *image_interface() const override { return "a2600_cart"; }
	virtual const char *file_extensions() const override { return "bin,a26"; }

	// slot interface overrides
	virtual std::string get_default_card_software() override;

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_rom);
	virtual DECLARE_READ8_MEMBER(read_bank);
	virtual DECLARE_WRITE8_MEMBER(write_bank);
	virtual DECLARE_WRITE8_MEMBER(write_ram);
	virtual DECLARE_DIRECT_UPDATE_MEMBER(cart_opbase);

private:
	device_vcs_cart_interface*       m_cart;
	int m_type;

	int detect_snowhite(uint8_t *cart, uint32_t len);
	int detect_modeDC(uint8_t *cart, uint32_t len);
	int detect_modeF6(uint8_t *cart, uint32_t len);
	int detect_mode3E(uint8_t *cart, uint32_t len);
	int detect_modeSS(uint8_t *cart, uint32_t len);
	int detect_modeFE(uint8_t *cart, uint32_t len);
	int detect_modeE0(uint8_t *cart, uint32_t len);
	int detect_modeCV(uint8_t *cart, uint32_t len);
	int detect_modeFV(uint8_t *cart, uint32_t len);
	int detect_modeJVP(uint8_t *cart, uint32_t len);
	int detect_modeE7(uint8_t *cart, uint32_t len);
	int detect_modeUA(uint8_t *cart, uint32_t len);
	int detect_8K_mode3F(uint8_t *cart, uint32_t len);
	int detect_32K_mode3F(uint8_t *cart, uint32_t len);
	int detect_super_chip(uint8_t *cart, uint32_t len);
};


// device type definition
extern const device_type VCS_CART_SLOT;


/***************************************************************************
 DEVICE CONFIGURATION MACROS
 ***************************************************************************/

#define A26SLOT_ROM_REGION_TAG ":cart:rom"


#define MCFG_VCS_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot) \
	MCFG_DEVICE_ADD(_tag, VCS_CART_SLOT, 0)  \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, false)


#endif
