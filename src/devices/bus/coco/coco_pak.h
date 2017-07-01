// license:BSD-3-Clause
// copyright-holders:Nathan Woods
#pragma once

#ifndef __COCO_PAK_H__
#define __COCO_PAK_H__

#include "emu.h"
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
		coco_pak_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const override;
		virtual const tiny_rom_entry *device_rom_region() const override;

		virtual uint8_t* get_cart_base() override;
protected:
		// device-level overrides
		virtual void device_start() override;
		virtual void device_reset() override;

		// internal state
		device_image_interface *m_cart;
		cococart_slot_device *m_owner;

		optional_ioport m_autostart;
};


// device type definition
extern const device_type COCO_PAK;

// ======================> coco_pak_banked_device

class coco_pak_banked_device :
		public coco_pak_device
{
public:
		// construction/destruction
		coco_pak_banked_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
		// device-level overrides
		virtual void device_reset() override;
		virtual DECLARE_WRITE8_MEMBER(write) override;
private:
		void banked_pak_set_bank(uint32_t bank);
};


// device type definition
extern const device_type COCO_PAK_BANKED;
#endif  /* __COCO_PAK_H__ */
