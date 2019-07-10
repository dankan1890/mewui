// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    Opus Challenger 3-in-1

**********************************************************************/


#ifndef MAME_BUS_BBC_1MHZBUS_OPUS3_H
#define MAME_BUS_BBC_1MHZBUS_OPUS3_H

#include "1mhzbus.h"
#include "imagedev/floppy.h"
#include "machine/ram.h"
#include "machine/wd_fdc.h"
#include "formats/acorn_dsk.h"
#include "formats/fsd_dsk.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class bbc_opus3_device:
	public device_t,
	public device_bbc_1mhzbus_interface
{
public:
	// construction/destruction
	bbc_opus3_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	bbc_opus3_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	virtual uint8_t fred_r(offs_t offset) override;
	virtual void fred_w(offs_t offset, uint8_t data) override;
	virtual uint8_t jim_r(offs_t offset) override;
	virtual void jim_w(offs_t offset, uint8_t data) override;

private:
	DECLARE_FLOPPY_FORMATS(floppy_formats);

	DECLARE_WRITE_LINE_MEMBER(fdc_drq_w);

	required_device<ram_device> m_ramdisk;
	required_device<wd1770_device> m_fdc;
	required_device<floppy_connector> m_floppy0;
	optional_device<floppy_connector> m_floppy1;

	int m_fdc_ie;
	int m_fdc_drq;
	uint16_t m_ramdisk_page;
};


class bbc_opusa_device : public bbc_opus3_device
{
public:
	// construction/destruction
	bbc_opusa_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_OPUS3, bbc_opus3_device)
DECLARE_DEVICE_TYPE(BBC_OPUSA, bbc_opusa_device)


#endif // MAME_BUS_BBC_1MHZBUS_OPUS3_H
