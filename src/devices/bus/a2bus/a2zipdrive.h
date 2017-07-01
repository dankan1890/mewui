// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2zipdrive.h

    ZIP Technologies ZipDrive IDE card

    See important NOTE at the top of a2zipdrive.c!

*********************************************************************/

#ifndef __A2BUS_ZIPDRIVE__
#define __A2BUS_ZIPDRIVE__

#include "emu.h"
#include "a2bus.h"
#include "machine/ataintf.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_zipdrivebase_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_zipdrivebase_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	// overrides of standard a2bus slot functions
	virtual uint8_t read_c0nx(address_space &space, uint8_t offset) override;
	virtual void write_c0nx(address_space &space, uint8_t offset, uint8_t data) override;
	virtual uint8_t read_cnxx(address_space &space, uint8_t offset) override;
	virtual uint8_t read_c800(address_space &space, uint16_t offset) override;

	required_device<ata_interface_device> m_ata;

	uint8_t *m_rom;

private:
	uint16_t m_lastdata;
};

class a2bus_zipdrive_device : public a2bus_zipdrivebase_device
{
public:
	a2bus_zipdrive_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};

// device type definition
extern const device_type A2BUS_ZIPDRIVE;

#endif /* __A2BUS_ZIPDRIVE__ */
