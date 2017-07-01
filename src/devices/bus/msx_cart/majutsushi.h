// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#ifndef __MSX_CART_MAJUTSUSHI_H
#define __MSX_CART_MAJUTSUSHI_H

#include "bus/msx_cart/cartridge.h"
#include "sound/dac.h"


extern const device_type MSX_CART_MAJUTSUSHI;


class msx_cart_majutsushi : public device_t
						, public msx_cart_interface
{
public:
	msx_cart_majutsushi(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual machine_config_constructor device_mconfig_additions() const override;

	virtual void initialize_cartridge() override;

	virtual DECLARE_READ8_MEMBER(read_cart) override;
	virtual DECLARE_WRITE8_MEMBER(write_cart) override;

	void restore_banks();

private:
	required_device<dac_byte_interface> m_dac;

	uint8_t m_selected_bank[4];
	uint8_t *m_bank_base[8];
};


#endif
