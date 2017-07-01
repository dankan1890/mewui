// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Wang PC-PM040-B Remote Telecommunication controller emulation

**********************************************************************/

#pragma once

#ifndef __WANGPC_RTC__
#define __WANGPC_RTC__

#include "emu.h"
#include "wangpc.h"
#include "cpu/z80/z80.h"
#include "machine/am9517a.h"
#include "machine/z80ctc.h"
#include "machine/z80dart.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> wangpc_rtc_device

class wangpc_rtc_device : public device_t,
							public device_wangpcbus_card_interface
{
public:
	// construction/destruction
	wangpc_rtc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual machine_config_constructor device_mconfig_additions() const override;
	virtual ioport_constructor device_input_ports() const override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_wangpcbus_card_interface overrides
	virtual uint16_t wangpcbus_mrdc_r(address_space &space, offs_t offset, uint16_t mem_mask) override;
	virtual void wangpcbus_amwc_w(address_space &space, offs_t offset, uint16_t mem_mask, uint16_t data) override;
	virtual uint16_t wangpcbus_iorc_r(address_space &space, offs_t offset, uint16_t mem_mask) override;
	virtual void wangpcbus_aiowc_w(address_space &space, offs_t offset, uint16_t mem_mask, uint16_t data) override;

private:
	required_device<cpu_device> m_maincpu;
	required_device<am9517a_device> m_dmac;
	required_device<z80ctc_device> m_ctc0;
	required_device<z80ctc_device> m_ctc1;
	required_device<z80dart_device> m_sio;
	optional_shared_ptr<uint8_t> m_char_ram;
};


// device type definition
extern const device_type WANGPC_RTC;


#endif
