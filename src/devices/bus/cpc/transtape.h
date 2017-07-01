// license:BSD-3-Clause
// copyright-holders:Barry Rodewald
/*
 * transtape.c  --  Hard Micro SA Transtape
 *
 * Spanish hacking device
 *
 */

#ifndef TRANSTAPE_H_
#define TRANSTAPE_H_

#include "emu.h"
#include "cpcexp.h"

class cpc_transtape_device  : public device_t,
						public device_cpc_expansion_card_interface
{
public:
	// construction/destruction
	cpc_transtape_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;

	virtual void set_mapping(uint8_t type) override;
	virtual WRITE_LINE_MEMBER( romen_w ) override { m_romen = state; }

	DECLARE_READ8_MEMBER(input_r);
	DECLARE_WRITE8_MEMBER(output_w);
	DECLARE_INPUT_CHANGED_MEMBER(button_red_w);
	DECLARE_INPUT_CHANGED_MEMBER(button_black_w);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	cpc_expansion_slot_device *m_slot;
	cpu_device* m_cpu;
	address_space* m_space;
	std::unique_ptr<uint8_t[]> m_ram;  // 8kB internal RAM
	bool m_rom_active;
	bool m_romen;
	uint8_t m_output;

	void map_enable();
};

// device type definition
extern const device_type CPC_TRANSTAPE;

#endif /* TRANSTAPE_H_ */
