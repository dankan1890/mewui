// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Wang PC PM-001B Medium-Resolution Video Controller emulation

**********************************************************************/

#pragma once

#ifndef __WANGPC_TIG__
#define __WANGPC_TIG__

#include "emu.h"
#include "wangpc.h"
#include "video/upd7220.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> wangpc_tig_device

class wangpc_tig_device : public device_t,
							public device_wangpcbus_card_interface
{
public:
	// construction/destruction
	wangpc_tig_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual machine_config_constructor device_mconfig_additions() const override;
	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	UPD7220_DRAW_TEXT_LINE_MEMBER( hgdc_draw_text );
	UPD7220_DISPLAY_PIXELS_MEMBER( hgdc_display_pixels );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_wangpcbus_card_interface overrides
	virtual uint16_t wangpcbus_iorc_r(address_space &space, offs_t offset, uint16_t mem_mask) override;
	virtual void wangpcbus_aiowc_w(address_space &space, offs_t offset, uint16_t mem_mask, uint16_t data) override;
	virtual uint8_t wangpcbus_dack_r(address_space &space, int line) override;
	virtual void wangpcbus_dack_w(address_space &space, int line, uint8_t data) override;
	virtual bool wangpcbus_have_dack(int line) override;

private:
	// internal state
	required_device<upd7220_device> m_hgdc0;
	required_device<upd7220_device> m_hgdc1;

	uint8_t m_option;
	uint8_t m_attr[16];
	uint8_t m_underline;
	required_device<palette_device> m_palette;
};


// device type definition
extern const device_type WANGPC_TIG;


#endif
