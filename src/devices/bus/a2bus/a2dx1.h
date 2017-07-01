// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2dx1.h

    Implementation of the Decillionix DX-1 sampler card

*********************************************************************/

#ifndef __A2BUS_DX1__
#define __A2BUS_DX1__

#include "emu.h"
#include "a2bus.h"
#include "sound/dac.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_dx1_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_dx1_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);
	a2bus_dx1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;

	required_device<dac_byte_interface> m_dac;
	required_device<dac_byte_interface> m_dacvol;

protected:
	virtual void device_start() override;

	// overrides of standard a2bus slot functions
	virtual uint8_t read_c0nx(address_space &space, uint8_t offset) override;
	virtual void write_c0nx(address_space &space, uint8_t offset, uint8_t data) override;
	virtual bool take_c800() override;
};

// device type definition
extern const device_type A2BUS_DX1;

#endif /* __A2BUS_DX1__ */
