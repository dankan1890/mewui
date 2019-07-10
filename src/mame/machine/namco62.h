// license:BSD-3-Clause
// copyright-holders:Philip Bennett
#ifndef MAME_MACHINE_NAMCO62_H
#define MAME_MACHINE_NAMCO62_H

#include "cpu/mb88xx/mb88xx.h"

class namco_62xx_device : public device_t
{
public:
	namco_62xx_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template <unsigned N> auto input_callback() { return m_in[N].bind(); }

	template <unsigned N> auto output_callback() { return m_out[N].bind(); }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	// internal state
	required_device<mb88_cpu_device> m_cpu;
	devcb_read8 m_in[4];
	devcb_write8 m_out[2];
};

DECLARE_DEVICE_TYPE(NAMCO_62XX, namco_62xx_device)

#endif // MAME_MACHINE_NAMCO62_H
