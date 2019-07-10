// license:BSD-3-Clause
// copyright-holders: F. Ulivi
/*********************************************************************

    82937.h

    82937 module (HPIB interface)

*********************************************************************/

#ifndef MAME_BUS_HP80_IO_82937_H
#define MAME_BUS_HP80_IO_82937_H

#pragma once

#include "hp80_io.h"
#include "cpu/mcs48/mcs48.h"
#include "bus/ieee488/ieee488.h"
#include "machine/1mb5.h"

class hp82937_io_card_device : public hp80_io_card_device
{
public:
	// construction/destruction
	hp82937_io_card_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~hp82937_io_card_device();

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	// device-level overrides
	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	virtual void install_read_write_handlers(address_space& space , uint16_t base_addr) override;

	virtual void inten() override;
	virtual void clear_service() override;

private:
	required_device<i8049_device> m_cpu;
	required_device<hp_1mb5_device> m_translator;
	required_ioport m_sw1;
	required_device<ieee488_device> m_ieee488;

	bool m_dio_out; // U8-4
	bool m_talker_out;  // U7-6
	bool m_iatn;
	uint8_t m_latch;    // U3
	bool m_updating;

	DECLARE_WRITE_LINE_MEMBER(reset_w);
	DECLARE_READ_LINE_MEMBER(t0_r);
	DECLARE_READ8_MEMBER(p1_r);
	DECLARE_WRITE8_MEMBER(p1_w);
	DECLARE_READ8_MEMBER(dio_r);
	DECLARE_WRITE8_MEMBER(dio_w);
	DECLARE_WRITE_LINE_MEMBER(ieee488_ctrl_w);
	DECLARE_READ8_MEMBER(switch_r);
	DECLARE_WRITE8_MEMBER(latch_w);

	void cpu_io_map(address_map &map);

	void update_data_out();
	void update_signals();
};

// device type definition
DECLARE_DEVICE_TYPE(HP82937_IO_CARD, hp82937_io_card_device)

#endif // MAME_BUS_HP80_IO_82937_H
