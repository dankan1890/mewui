// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
#ifndef MAME_BUS_COCO_DRAGON_AMTOR_H
#define MAME_BUS_COCO_DRAGON_AMTOR_H

#pragma once

#include "cococart.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> dragon_amtor_device

class dragon_amtor_device :
		public device_t,
		public device_cococart_interface
{
public:
	// construction/destruction
	dragon_amtor_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual uint8_t* get_cart_base() override;
	virtual memory_region* get_cart_memregion() override;

	virtual DECLARE_READ8_MEMBER(cts_read) override;

private:
	required_memory_region m_eprom;
	required_ioport m_switch;
};


// device type definitions
DECLARE_DEVICE_TYPE(DRAGON_AMTOR, dragon_amtor_device)

#endif // MAME_BUS_COCO_DRAGON_AMTOR_H
