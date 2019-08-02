// license:BSD-3-Clause
// copyright-holders:David Haywood
/**********************************************************************

    Rotronics Wafadrive

**********************************************************************/

#ifndef MAME_BUS_SPECTRUM_WAFA_H
#define MAME_BUS_SPECTRUM_WAFA_H

#include "exp.h"
#include "imagedev/wafadrive.h"

#include "softlist.h"
#include "bus/generic/slot.h"
#include "bus/generic/carts.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class spectrum_wafa_device:
	public device_t,
	public device_spectrum_expansion_interface
{
public:
	// construction/destruction
	spectrum_wafa_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	static constexpr feature_type unemulated_features() { return feature::DISK | feature::LAN; }

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	virtual void opcode_fetch(offs_t offset) override;
	virtual void opcode_fetch_post(offs_t offset) override;
	virtual uint8_t mreq_r(offs_t offset) override;
	virtual void mreq_w(offs_t offset, uint8_t data) override;
	virtual uint8_t iorq_r(offs_t offset) override;
	virtual void iorq_w(offs_t offset, uint8_t data) override;
	virtual DECLARE_READ_LINE_MEMBER(romcs) override;

private:
	required_device<spectrum_expansion_slot_device> m_exp;
	required_memory_region m_rom;
	required_device<wafadrive_image_device> m_wafa1;
	required_device<wafadrive_image_device> m_wafa2;

	int m_romcs;
};


// device type definition
DECLARE_DEVICE_TYPE(SPECTRUM_WAFA, spectrum_wafa_device)


#endif /* MAME_BUS_SPECTRUM_WAFA_H */
