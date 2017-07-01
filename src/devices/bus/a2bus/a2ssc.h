// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2ssc.h

    Apple II Super Serial Card

*********************************************************************/

#ifndef __A2BUS_SSC__
#define __A2BUS_SSC__

#include "a2bus.h"
#include "machine/mos6551.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_ssc_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_ssc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	a2bus_ssc_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;

	required_ioport m_dsw1, m_dsw2;

	DECLARE_WRITE_LINE_MEMBER( acia_irq_w );

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual uint8_t read_c0nx(address_space &space, uint8_t offset) override;
	virtual void write_c0nx(address_space &space, uint8_t offset, uint8_t data) override;
	virtual uint8_t read_cnxx(address_space &space, uint8_t offset) override;
	virtual uint8_t read_c800(address_space &space, uint16_t offset) override;

	required_device<mos6551_device> m_acia;

private:
	uint8_t *m_rom;
	bool m_started;
};

// device type definition
extern const device_type A2BUS_SSC;

#endif  /* __A2BUS_SSC__ */
