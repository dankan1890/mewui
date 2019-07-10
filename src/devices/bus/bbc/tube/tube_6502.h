// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    Acorn ANC01 6502 2nd Processor

    http://chrisacorns.computinghistory.org.uk/8bit_Upgrades/Acorn_ANC01_65022ndproc.html

    Acorn ADC06 65C102 Co-processor

    http://chrisacorns.computinghistory.org.uk/8bit_Upgrades/Acorn_ADC06_65C102CoPro.html

**********************************************************************/


#ifndef MAME_BUS_BBC_TUBE_6502_H
#define MAME_BUS_BBC_TUBE_6502_H

#include "tube.h"
#include "cpu/m6502/m65c02.h"
#include "machine/bankdev.h"
#include "machine/ram.h"
#include "machine/tube.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> bbc_tube_6502_device

class bbc_tube_6502_device :
	public device_t,
	public device_bbc_tube_interface
{
public:
	// construction/destruction
	bbc_tube_6502_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	bbc_tube_6502_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void tube_6502_bank(address_map &map);
	void tube_6502_mem(address_map &map);

	void add_common_devices(machine_config &config);

	virtual uint8_t host_r(offs_t offset) override;
	virtual void host_w(offs_t offset, uint8_t data) override;

	virtual uint8_t tube_r(offs_t offset);
	virtual void tube_w(offs_t offset, uint8_t data);

	required_device<cpu_device> m_maincpu;
	required_device<address_map_bank_device> m_bankdev;
	required_device<tube_device> m_ula;
	required_device<ram_device> m_ram;
	required_memory_region m_rom;
};


class bbc_tube_65c102_device : public bbc_tube_6502_device
{
public:
	bbc_tube_65c102_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_TUBE_6502, bbc_tube_6502_device)
DECLARE_DEVICE_TYPE(BBC_TUBE_65C102, bbc_tube_65c102_device)


#endif /* MAME_BUS_BBC_TUBE_6502_H */
