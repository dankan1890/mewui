// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2videoterm.h

    Implementation of the Apple II Memory Expansion Card

*********************************************************************/

#ifndef __A2BUS_VIDEOTERM__
#define __A2BUS_VIDEOTERM__

#include "emu.h"
#include "a2bus.h"
#include "video/mc6845.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_videx80_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_videx80_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;

	DECLARE_WRITE_LINE_MEMBER(vsync_changed);
	MC6845_UPDATE_ROW(crtc_update_row);

	uint8_t *m_rom, *m_chrrom;
	uint8_t m_ram[512*4];
	int m_framecnt;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	// overrides of standard a2bus slot functions
	virtual uint8_t read_c0nx(address_space &space, uint8_t offset) override;
	virtual void write_c0nx(address_space &space, uint8_t offset, uint8_t data) override;
	virtual uint8_t read_cnxx(address_space &space, uint8_t offset) override;
	virtual void write_cnxx(address_space &space, uint8_t offset, uint8_t data) override;
	virtual uint8_t read_c800(address_space &space, uint16_t offset) override;
	virtual void write_c800(address_space &space, uint16_t offset, uint8_t data) override;

	required_device<mc6845_device> m_crtc;

private:
	int m_rambank;
public:
	required_device<palette_device> m_palette;
};

class a2bus_videoterm_device : public a2bus_videx80_device
{
public:
	a2bus_videoterm_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;
};

class a2bus_ap16_device : public a2bus_videx80_device
{
public:
	a2bus_ap16_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;

	virtual uint8_t read_cnxx(address_space &space, uint8_t offset) override;
};


class a2bus_ap16alt_device : public a2bus_videx80_device
{
public:
	a2bus_ap16alt_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;

	virtual uint8_t read_cnxx(address_space &space, uint8_t offset) override;
};

class a2bus_vtc1_device : public a2bus_videx80_device
{
public:
	a2bus_vtc1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;
};

class a2bus_vtc2_device : public a2bus_videx80_device
{
public:
	a2bus_vtc2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;
};

class a2bus_aevm80_device : public a2bus_videx80_device
{
public:
	a2bus_aevm80_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;
};

// device type definition
extern const device_type A2BUS_VIDEOTERM;
extern const device_type A2BUS_IBSAP16;
extern const device_type A2BUS_IBSAP16ALT;
extern const device_type A2BUS_VTC1;
extern const device_type A2BUS_VTC2;
extern const device_type A2BUS_AEVIEWMASTER80;

#endif /* __A2BUS_VIDEOTERM__ */
