// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#ifndef __MSX_CART_SUPER_SWANGI_H
#define __MSX_CART_SUPER_SWANGI_H

#include "bus/msx_cart/cartridge.h"


extern const device_type MSX_CART_SUPER_SWANGI;


class msx_cart_super_swangi : public device_t
						, public msx_cart_interface
{
public:
	msx_cart_super_swangi(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void initialize_cartridge() override;

	virtual DECLARE_READ8_MEMBER(read_cart) override;
	virtual DECLARE_WRITE8_MEMBER(write_cart) override;

	void restore_banks();

private:
	uint8_t m_selected_bank;
	uint8_t *m_bank_base[2];
};


#endif
