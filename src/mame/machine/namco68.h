// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_MACHINE_NAMCO68_H
#define MAME_MACHINE_NAMCO68_H

#pragma once

#include "machine/bankdev.h"
#include "cpu/m6502/m3745x.h"

DECLARE_DEVICE_TYPE(NAMCOC68, namcoc68_device)


class namcoc68_device : public device_t
{
public:
	// construction/destruction
	namcoc68_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto in_pb_callback() { return m_in_pb_cb.bind(); }

	auto in_pc_callback() { return m_in_pc_cb.bind(); }
	auto in_ph_callback() { return m_in_ph_cb.bind(); }
	auto in_pdsw_callback() { return m_in_pdsw_cb.bind(); }

	auto an0_in_cb() { return m_port_analog_in_cb[0].bind(); }
	auto an1_in_cb() { return m_port_analog_in_cb[1].bind(); }
	auto an2_in_cb() { return m_port_analog_in_cb[2].bind(); }
	auto an3_in_cb() { return m_port_analog_in_cb[3].bind(); }
	auto an4_in_cb() { return m_port_analog_in_cb[4].bind(); }
	auto an5_in_cb() { return m_port_analog_in_cb[5].bind(); }
	auto an6_in_cb() { return m_port_analog_in_cb[6].bind(); }
	auto an7_in_cb() { return m_port_analog_in_cb[7].bind(); }

	auto di0_in_cb() { return m_port_dial_in_cb[0].bind(); }
	auto di1_in_cb() { return m_port_dial_in_cb[1].bind(); }
	auto di2_in_cb() { return m_port_dial_in_cb[2].bind(); }
	auto di3_in_cb() { return m_port_dial_in_cb[3].bind(); }

	auto dp_in_callback() { return m_dp_in.bind(); }
	auto dp_out_callback() { return m_dp_out.bind(); }

	void ext_interrupt(int state) { m_mcu->set_input_line(0, state); }   // 37450 maps INT1 to irq0 as it's the first external interrupt on that chip
	void ext_reset(int state) { m_mcu->set_input_line(INPUT_LINE_RESET, state); }

protected:
	void c68_default_am(address_map &map);

	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_resolve_objects() override;
	virtual void device_start() override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_reset() override;

private:
	required_device<m37450_device> m_mcu;

	devcb_read8        m_in_pb_cb;

	devcb_read8        m_in_pc_cb;
	devcb_read8        m_in_ph_cb;
	devcb_read8        m_in_pdsw_cb;

	devcb_read8   m_port_analog_in_cb[8];
	devcb_read8   m_port_dial_in_cb[4];

	devcb_read8        m_dp_in;
	devcb_write8       m_dp_out;

	DECLARE_READ8_MEMBER(c68_p5_r);
	DECLARE_WRITE8_MEMBER(c68_p3_w);
	DECLARE_READ8_MEMBER(ack_mcu_vbl_r);

	DECLARE_READ8_MEMBER(dpram_byte_r);
	DECLARE_WRITE8_MEMBER(dpram_byte_w);

	DECLARE_READ8_MEMBER(unk_r);
	DECLARE_READ8_MEMBER(mcuc_r);

	DECLARE_READ8_MEMBER(mcudsw_r) { return m_in_pdsw_cb(); }

	DECLARE_READ8_MEMBER(mcudi0_r) { return m_port_dial_in_cb[0](); }
	DECLARE_READ8_MEMBER(mcudi1_r) { return m_port_dial_in_cb[1](); }
	DECLARE_READ8_MEMBER(mcudi2_r) { return m_port_dial_in_cb[2](); }
	DECLARE_READ8_MEMBER(mcudi3_r) { return m_port_dial_in_cb[3](); }

	DECLARE_READ8_MEMBER(mcuan0_r) { return m_port_analog_in_cb[0](); }
	DECLARE_READ8_MEMBER(mcuan1_r) { return m_port_analog_in_cb[1](); }
	DECLARE_READ8_MEMBER(mcuan2_r) { return m_port_analog_in_cb[2](); }
	DECLARE_READ8_MEMBER(mcuan3_r) { return m_port_analog_in_cb[3](); }
	DECLARE_READ8_MEMBER(mcuan4_r) { return m_port_analog_in_cb[4](); }
	DECLARE_READ8_MEMBER(mcuan5_r) { return m_port_analog_in_cb[5](); }
	DECLARE_READ8_MEMBER(mcuan6_r) { return m_port_analog_in_cb[6](); }
	DECLARE_READ8_MEMBER(mcuan7_r) { return m_port_analog_in_cb[7](); }

	uint8_t m_player_mux;
};

#endif // MAME_MACHINE_NAMCO68_H
