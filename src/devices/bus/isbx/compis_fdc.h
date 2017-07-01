// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    TeleNova Compis Floppy Disk Controller (119 106/1) emulation

**********************************************************************/

#pragma once

#ifndef __COMPIS_FDC__
#define __COMPIS_FDC__

#include "emu.h"
#include "isbx.h"
#include "formats/cpis_dsk.h"
#include "machine/upd765.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> compis_fdc_device

class compis_fdc_device : public device_t,
							public device_isbx_card_interface
{
public:
	// construction/destruction
	compis_fdc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;

	DECLARE_WRITE_LINE_MEMBER( fdc_irq );
	DECLARE_WRITE_LINE_MEMBER( fdc_drq );
	DECLARE_FLOPPY_FORMATS( floppy_formats );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_isbx_card_interface overrides
	virtual uint8_t mcs0_r(address_space &space, offs_t offset) override;
	virtual void mcs0_w(address_space &space, offs_t offset, uint8_t data) override;
	virtual uint8_t mdack_r(address_space &space, offs_t offset) override;
	virtual void mdack_w(address_space &space, offs_t offset, uint8_t data) override;
	virtual void opt0_w(int state) override;
	virtual void opt1_w(int state) override;

private:
	required_device<i8272a_device> m_fdc;
	required_device<floppy_connector> m_floppy0;
	required_device<floppy_connector> m_floppy1;
};


// device type definition
extern const device_type COMPIS_FDC;


#endif
