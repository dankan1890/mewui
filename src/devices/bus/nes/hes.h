// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
#ifndef __NES_HES_H
#define __NES_HES_H

#include "nxrom.h"


// ======================> nes_hes_device

class nes_hes_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_hes_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual DECLARE_WRITE8_MEMBER(write_l) override;

	virtual void pcb_reset() override;
};


// device type definition
extern const device_type NES_HES;

#endif
