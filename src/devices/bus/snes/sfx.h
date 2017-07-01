// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
#ifndef __SNS_SFX_H
#define __SNS_SFX_H

#include "snes_slot.h"
#include "rom.h"
#include "cpu/superfx/superfx.h"


// ======================> sns_rom_superfx_device

class sns_rom_superfx_device : public sns_rom_device
{
public:
	// construction/destruction
	sns_rom_superfx_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual machine_config_constructor device_mconfig_additions() const override;

	required_device<superfx_device> m_superfx;

	// additional reading and writing
	virtual DECLARE_READ8_MEMBER(read_l) override;
	virtual DECLARE_READ8_MEMBER(read_h) override;
	virtual DECLARE_READ8_MEMBER(read_ram) override;
	virtual DECLARE_WRITE8_MEMBER(write_ram) override;
	virtual DECLARE_READ8_MEMBER(chip_read) override;
	virtual DECLARE_WRITE8_MEMBER(chip_write) override;

	virtual DECLARE_READ8_MEMBER(superfx_r_bank1);
	virtual DECLARE_READ8_MEMBER(superfx_r_bank2);
	virtual DECLARE_READ8_MEMBER(superfx_r_bank3);
	virtual DECLARE_WRITE8_MEMBER(superfx_w_bank1);
	virtual DECLARE_WRITE8_MEMBER(superfx_w_bank2);
	virtual DECLARE_WRITE8_MEMBER(superfx_w_bank3);
	virtual DECLARE_WRITE_LINE_MEMBER(snes_extern_irq_w);


	uint8_t sfx_ram[0x200000];
};


// device type definition
extern const device_type SNS_LOROM_SUPERFX;

#endif
