// license:BSD-3-Clause
// copyright-holders:Darkstar
/**********************************************************************
 *
 *    Adaptec AHA-1542{C,CF,CP} SCSI Controller
 *
 **********************************************************************



 **********************************************************************/

#ifndef MAME_BUS_AHA1542C_H
#define MAME_BUS_AHA1542C_H

#pragma once


#include "isa.h"
#include "machine/eepromser.h"

//*********************************************************************
//   TYPE DEFINITIONS
//*********************************************************************

// ====================> aha1542cf_device

class aha1542c_device : public device_t,
						public device_isa16_card_interface
{
public:
	static constexpr feature_type unemulated_features() { return feature::DISK; }
	// construction/destruction
	aha1542c_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	DECLARE_READ8_MEMBER( aha1542_r );
	DECLARE_WRITE8_MEMBER( aha1542_w );

protected:
	aha1542c_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	void local_latch_w(u8 data);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	required_device<eeprom_serial_93cxx_device> m_eeprom;

private:
	void z84c0010_mem(address_map &map);
};

// ====================> aha1542cf_device

class aha1542cf_device : public aha1542c_device
{
public:
	// construction/destruction
	aha1542cf_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
};

// ====================> aha1542cp_device

class aha1542cp_device : public aha1542c_device
{
public:
	// construction/destruction
	aha1542cp_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

private:
	u8 eeprom_r();
	void eeprom_w(u8 data);

	void local_mem(address_map &map);
};

// device type definitions
DECLARE_DEVICE_TYPE(AHA1542C, aha1542c_device)
DECLARE_DEVICE_TYPE(AHA1542CF, aha1542cf_device)
DECLARE_DEVICE_TYPE(AHA1542CP, aha1542cp_device)

#endif // MAME_BUS_AHA1542C_H
