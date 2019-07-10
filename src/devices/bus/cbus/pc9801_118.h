// license:BSD-3-Clause
// copyright-holders:Angelo Salese
/***************************************************************************

    NEC PC-9801-118

***************************************************************************/

#ifndef MAME_BUS_CBUS_PC9801_118_H
#define MAME_BUS_CBUS_PC9801_118_H

#pragma once

#include "bus/cbus/pc9801_cbus.h"
#include "sound/2608intf.h"
#include "pc9801_snd.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> pc9801_118_device

class pc9801_118_device : public pc9801_snd_device
{
public:
	// construction/destruction
	pc9801_118_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_READ8_MEMBER(opn3_r);
	DECLARE_WRITE8_MEMBER(opn3_w);
	DECLARE_READ8_MEMBER(id_r);
	DECLARE_WRITE8_MEMBER(ext_w);

protected:
	// device-level overrides
	virtual void device_validity_check(validity_checker &valid) const override;
	virtual void device_start() override;
	virtual void device_reset() override;
	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;

private:
	required_device<pc9801_slot_device> m_bus;
	required_device<ym2608_device>  m_opn3;

	uint8_t m_ext_reg;

	DECLARE_WRITE_LINE_MEMBER(sound_irq);
};


// device type definition
DECLARE_DEVICE_TYPE(PC9801_118, pc9801_118_device)



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************



#endif // MAME_BUS_CBUS_PC9801_118_H
