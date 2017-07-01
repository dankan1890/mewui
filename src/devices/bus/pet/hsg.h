// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    CBM 8000 High Speed Graphics (324402-01) card emulation

**********************************************************************/

#pragma once

#ifndef __CBM8000_HSG__
#define __CBM8000_HSG__

#include "emu.h"
#include "exp.h"
#include "video/ef9365.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> cbm8000_hsg_t

class cbm8000_hsg_t : public device_t,
						public device_pet_expansion_card_interface
{
public:
	// construction/destruction
	cbm8000_hsg_t(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;

	// device_pet_expansion_card_interface overrides
	virtual int pet_norom_r(address_space &space, offs_t offset, int sel) override;
	virtual uint8_t pet_bd_r(address_space &space, offs_t offset, uint8_t data, int &sel) override;
	virtual void pet_bd_w(address_space &space, offs_t offset, uint8_t data, int &sel) override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	required_device<ef9365_device> m_gdc;
	required_memory_region m_9000;
	required_memory_region m_a000;
};


// ======================> cbm8000_hsg_a_t

class cbm8000_hsg_a_t :  public cbm8000_hsg_t
{
public:
	// construction/destruction
	cbm8000_hsg_a_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;
};


// ======================> cbm8000_hsg_b_t

class cbm8000_hsg_b_t :  public cbm8000_hsg_t
{
public:
	// construction/destruction
	cbm8000_hsg_b_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;
};


// device type definition
extern const device_type CBM8000_HSG_A;
extern const device_type CBM8000_HSG_B;



#endif
