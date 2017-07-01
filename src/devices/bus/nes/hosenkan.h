// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
#ifndef __NES_HOSENKAN_H
#define __NES_HOSENKAN_H

#include "nxrom.h"


// ======================> nes_hosenkan_device

class nes_hosenkan_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_hosenkan_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual DECLARE_WRITE8_MEMBER(write_h) override;


	virtual void hblank_irq(int scanline, int vblank, int blanked) override;
	virtual void pcb_reset() override;

private:
	uint16_t m_irq_count, m_irq_count_latch;
	uint8_t m_irq_clear;
	int m_irq_enable;

	uint8_t m_latch;
};



// device type definition
extern const device_type NES_HOSENKAN;


#endif
