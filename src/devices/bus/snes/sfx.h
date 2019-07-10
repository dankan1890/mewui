// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
#ifndef MAME_BUS_SNES_SFX_H
#define MAME_BUS_SNES_SFX_H

#include "snes_slot.h"
#include "rom.h"
#include "cpu/superfx/superfx.h"


// ======================> sns_rom_superfx_device

class sns_rom_superfx_device : public sns_rom_device
{
public:
	// construction/destruction
	sns_rom_superfx_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void device_add_mconfig(machine_config &config) override;

	virtual DECLARE_WRITE_LINE_MEMBER(snes_extern_irq_w);

	// additional reading and writing
	virtual uint8_t read_l(offs_t offset) override;
	virtual uint8_t read_h(offs_t offset) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;
	virtual uint8_t chip_read(offs_t offset) override;
	virtual void chip_write(offs_t offset, uint8_t data) override;

	uint8_t superfx_r_bank1(offs_t offset);
	uint8_t superfx_r_bank2(offs_t offset);
	uint8_t superfx_r_bank3(offs_t offset);
	void superfx_w_bank1(offs_t offset, uint8_t data);
	void superfx_w_bank2(offs_t offset, uint8_t data);
	void superfx_w_bank3(offs_t offset, uint8_t data);

private:
	required_device<superfx_device> m_superfx;

	uint8_t sfx_ram[0x200000];

	void sfx_map(address_map &map);
};


// device type definition
DECLARE_DEVICE_TYPE(SNS_LOROM_SUPERFX, sns_rom_superfx_device)

#endif // MAME_BUS_SNES_SFX_H
