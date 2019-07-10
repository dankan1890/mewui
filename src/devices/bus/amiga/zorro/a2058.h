// license:GPL-2.0+
// copyright-holders:Dirk Best
/***************************************************************************

    Commodore A2058

    Zorro-II RAM Expansion (2, 4 or 8 MB)

***************************************************************************/

#ifndef MAME_BUS_AMIGA_ZORRO_A2058_H
#define MAME_BUS_AMIGA_ZORRO_A2058_H

#pragma once

#include "zorro.h"
#include "machine/autoconfig.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> a2058_device

class a2058_device : public device_t, public device_zorro2_card_interface, public amiga_autoconfig
{
public:
	// construction/destruction
	a2058_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_start() override;

	// device_zorro2_card_interface overrides
	virtual DECLARE_WRITE_LINE_MEMBER( cfgin_w ) override;

	// amiga_autoconfig overrides
	virtual void autoconfig_base_address(offs_t address) override;

private:
	required_ioport m_config;
	std::unique_ptr<uint16_t[]> m_ram;
	int m_ram_size;
};

// device type definition
DECLARE_DEVICE_TYPE(A2058, a2058_device)

#endif // MAME_BUS_AMIGA_ZORRO_A2058_H
