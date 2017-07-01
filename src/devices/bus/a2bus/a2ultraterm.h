// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2ultraterm.h

    Implementation of the Videx UltraTerm 80/132/160-column video card

*********************************************************************/

#ifndef __A2BUS_ULTRATERM__
#define __A2BUS_ULTRATERM__

#include "emu.h"
#include "a2bus.h"
#include "video/mc6845.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_videx160_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_videx160_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;

	DECLARE_WRITE_LINE_MEMBER(vsync_changed);
	MC6845_UPDATE_ROW(crtc_update_row);

	uint8_t *m_rom, *m_chrrom;
	uint8_t m_ram[256*16];
	int m_framecnt;
	uint8_t m_ctrl1, m_ctrl2;

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
};

class a2bus_ultraterm_device : public a2bus_videx160_device
{
public:
	a2bus_ultraterm_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;
};

class a2bus_ultratermenh_device : public a2bus_videx160_device
{
public:
	a2bus_ultratermenh_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;
};

// device type definition
extern const device_type A2BUS_ULTRATERM;
extern const device_type A2BUS_ULTRATERMENH;

#endif /* __A2BUS_ULTRATERM__ */
