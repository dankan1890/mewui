// license:BSD-3-Clause
// copyright-holders:Alex W. Jackson
#ifndef MAME_VIDEO_NAMCO_C116_H
#define MAME_VIDEO_NAMCO_C116_H

#pragma once



//***************************************************************************
//  TYPE DEFINITIONS
//***************************************************************************

class namco_c116_device :
	public device_t,
	public device_palette_interface,
	public device_video_interface
{
public:
	//construction/destruction
	namco_c116_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration
	void enable_shadows() { m_enable_shadows = true; }

	//read/write handlers
	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );

	//getters
	uint16_t get_reg(int reg) { return m_regs[reg]; }

protected:
	// device-level overrides
	virtual void device_start() override;

	// device_palette_interface overrides
	virtual uint32_t palette_entries() const override { return 0x2000; }
	virtual bool palette_shadows_enabled() const override { return m_enable_shadows; }

private:
	// internal state
	std::vector<uint8_t> m_ram_r;
	std::vector<uint8_t> m_ram_g;
	std::vector<uint8_t> m_ram_b;
	uint16_t m_regs[8];
	bool                m_enable_shadows;       // are shadows enabled?
};

DECLARE_DEVICE_TYPE(NAMCO_C116, namco_c116_device)

#endif // MAME_VIDEO_NAMCO_C116_H
