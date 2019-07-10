// license:BSD-3-Clause
// copyright-holders:Nathan Woods
#ifndef MAME_BUS_COCO_COCO_PAK_H
#define MAME_BUS_COCO_COCO_PAK_H

#pragma once

#include "cococart.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> coco_pak_device

class coco_pak_device :
		public device_t,
		public device_cococart_interface
{
public:
	// construction/destruction
	coco_pak_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;

	virtual uint8_t* get_cart_base() override;
	virtual uint32_t get_cart_size() override;
	virtual memory_region* get_cart_memregion() override;

protected:
	coco_pak_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual DECLARE_READ8_MEMBER(cts_read) override;

	// internal state
	device_image_interface *m_cart;
	required_memory_region m_eprom;
	optional_ioport m_autostart;
};


// ======================> coco_pak_banked_device

class coco_pak_banked_device : public coco_pak_device
{
public:
	// construction/destruction
	coco_pak_banked_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	coco_pak_banked_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual uint8_t *get_cart_base() override;
	virtual uint32_t get_cart_size() override;
	virtual DECLARE_READ8_MEMBER(cts_read) override;
	virtual DECLARE_WRITE8_MEMBER(scs_write) override;

private:
	uint8_t m_pos;
};


// device type definitions
DECLARE_DEVICE_TYPE(COCO_PAK, coco_pak_device)
DECLARE_DEVICE_TYPE(COCO_PAK_BANKED, coco_pak_banked_device)

#endif // MAME_BUS_COCO_COCO_PAK_H
