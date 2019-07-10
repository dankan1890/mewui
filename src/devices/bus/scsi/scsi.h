// license:BSD-3-Clause
// copyright-holders:smf

#ifndef MAME_BUS_SCSI_SCSI_H
#define MAME_BUS_SCSI_SCSI_H

#pragma once

#include "machine/buffer.h"
#include "machine/output_latch.h"

#define SCSI_PORT_DEVICE1 "1"
#define SCSI_PORT_DEVICE2 "2"
#define SCSI_PORT_DEVICE3 "3"
#define SCSI_PORT_DEVICE4 "4"
#define SCSI_PORT_DEVICE5 "5"
#define SCSI_PORT_DEVICE6 "6"
#define SCSI_PORT_DEVICE7 "7"


class scsi_port_slot_device;
class scsi_port_interface;

class scsi_port_device : public device_t
{
	friend class scsi_port_interface;

public:
	// construction/destruction
	scsi_port_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	auto bsy_handler() { return m_bsy_handler.bind(); }
	auto sel_handler() { return m_sel_handler.bind(); }
	auto cd_handler() { return m_cd_handler.bind(); }
	auto io_handler() { return m_io_handler.bind(); }
	auto msg_handler() { return m_msg_handler.bind(); }
	auto req_handler() { return m_req_handler.bind(); }
	auto ack_handler() { return m_ack_handler.bind(); }
	auto atn_handler() { return m_atn_handler.bind(); }
	auto rst_handler() { return m_rst_handler.bind(); }
	auto data0_handler() { return m_data0_handler.bind(); }
	auto data1_handler() { return m_data1_handler.bind(); }
	auto data2_handler() { return m_data2_handler.bind(); }
	auto data3_handler() { return m_data3_handler.bind(); }
	auto data4_handler() { return m_data4_handler.bind(); }
	auto data5_handler() { return m_data5_handler.bind(); }
	auto data6_handler() { return m_data6_handler.bind(); }
	auto data7_handler() { return m_data7_handler.bind(); }

	template <typename T>
	void set_data_input_buffer(T &&tag)
	{
		data0_handler().set(tag, FUNC(input_buffer_device::write_bit0));
		data1_handler().set(tag, FUNC(input_buffer_device::write_bit1));
		data2_handler().set(tag, FUNC(input_buffer_device::write_bit2));
		data3_handler().set(tag, FUNC(input_buffer_device::write_bit3));
		data4_handler().set(tag, FUNC(input_buffer_device::write_bit4));
		data5_handler().set(tag, FUNC(input_buffer_device::write_bit5));
		data6_handler().set(tag, FUNC(input_buffer_device::write_bit6));
		data7_handler().set(tag, FUNC(input_buffer_device::write_bit7));
	}

	void set_output_latch(output_latch_device &latch);

	DECLARE_WRITE_LINE_MEMBER( write_bsy );
	DECLARE_WRITE_LINE_MEMBER( write_sel );
	DECLARE_WRITE_LINE_MEMBER( write_cd );
	DECLARE_WRITE_LINE_MEMBER( write_io );
	DECLARE_WRITE_LINE_MEMBER( write_msg );
	DECLARE_WRITE_LINE_MEMBER( write_req );
	DECLARE_WRITE_LINE_MEMBER( write_ack );
	DECLARE_WRITE_LINE_MEMBER( write_atn );
	DECLARE_WRITE_LINE_MEMBER( write_rst );
	DECLARE_WRITE_LINE_MEMBER( write_data0 );
	DECLARE_WRITE_LINE_MEMBER( write_data1 );
	DECLARE_WRITE_LINE_MEMBER( write_data2 );
	DECLARE_WRITE_LINE_MEMBER( write_data3 );
	DECLARE_WRITE_LINE_MEMBER( write_data4 );
	DECLARE_WRITE_LINE_MEMBER( write_data5 );
	DECLARE_WRITE_LINE_MEMBER( write_data6 );
	DECLARE_WRITE_LINE_MEMBER( write_data7 );

	scsi_port_slot_device &slot(int index);
	void set_slot_device(int index, const char *option, const device_type &type, const input_device_default *id);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;

	void update_bsy();
	void update_sel();
	void update_cd();
	void update_io();
	void update_msg();
	void update_req();
	void update_ack();
	void update_atn();
	void update_rst();
	void update_data0();
	void update_data1();
	void update_data2();
	void update_data3();
	void update_data4();
	void update_data5();
	void update_data6();
	void update_data7();

private:
	devcb_write_line m_bsy_handler;
	devcb_write_line m_sel_handler;
	devcb_write_line m_cd_handler;
	devcb_write_line m_io_handler;
	devcb_write_line m_msg_handler;
	devcb_write_line m_req_handler;
	devcb_write_line m_ack_handler;
	devcb_write_line m_atn_handler;
	devcb_write_line m_rst_handler;
	devcb_write_line m_data0_handler;
	devcb_write_line m_data1_handler;
	devcb_write_line m_data2_handler;
	devcb_write_line m_data3_handler;
	devcb_write_line m_data4_handler;
	devcb_write_line m_data5_handler;
	devcb_write_line m_data6_handler;
	devcb_write_line m_data7_handler;

	optional_device_array<scsi_port_slot_device, 7> m_slot;
	int m_device_count;

	int m_bsy_in;
	int m_sel_in;
	int m_cd_in;
	int m_io_in;
	int m_msg_in;
	int m_req_in;
	int m_ack_in;
	int m_atn_in;
	int m_rst_in;
	int m_data0_in;
	int m_data1_in;
	int m_data2_in;
	int m_data3_in;
	int m_data4_in;
	int m_data5_in;
	int m_data6_in;
	int m_data7_in;
	int m_bsy_out;
	int m_sel_out;
	int m_cd_out;
	int m_io_out;
	int m_msg_out;
	int m_req_out;
	int m_ack_out;
	int m_atn_out;
	int m_rst_out;
	int m_data0_out;
	int m_data1_out;
	int m_data2_out;
	int m_data3_out;
	int m_data4_out;
	int m_data5_out;
	int m_data6_out;
	int m_data7_out;
};

DECLARE_DEVICE_TYPE(SCSI_PORT, scsi_port_device)

class scsi_port_interface;

class scsi_port_slot_device : public device_t,
	public device_slot_interface
{
	friend class scsi_port_device;
	friend class scsi_port_interface;

public:
	scsi_port_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	scsi_port_interface *dev() { return m_dev; }
	scsi_port_device *port() { return m_port; }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_config_complete() override;

protected:
	scsi_port_interface *m_dev;
	scsi_port_device *m_port;

	int m_bsy;
	int m_sel;
	int m_cd;
	int m_io;
	int m_msg;
	int m_req;
	int m_ack;
	int m_atn;
	int m_rst;
	int m_data0;
	int m_data1;
	int m_data2;
	int m_data3;
	int m_data4;
	int m_data5;
	int m_data6;
	int m_data7;
};

DECLARE_DEVICE_TYPE(SCSI_PORT_SLOT, scsi_port_slot_device)

class scsi_port_interface : public device_slot_card_interface
{
public:
	virtual ~scsi_port_interface();

	virtual DECLARE_WRITE_LINE_MEMBER( input_bsy ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_sel ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_cd ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_io ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_msg ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_req ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_ack ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_atn ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_rst ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data0 ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data1 ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data2 ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data3 ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data4 ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data5 ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data6 ) { }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data7 ) { }

	DECLARE_WRITE_LINE_MEMBER( output_bsy ) { if (m_slot->m_bsy != state) { m_slot->m_bsy = state; m_slot->port()->update_bsy(); } }
	DECLARE_WRITE_LINE_MEMBER( output_sel ) { if (m_slot->m_sel != state) { m_slot->m_sel = state; m_slot->port()->update_sel(); } }
	DECLARE_WRITE_LINE_MEMBER( output_cd ) { if (m_slot->m_cd != state) { m_slot->m_cd = state; m_slot->port()->update_cd(); } }
	DECLARE_WRITE_LINE_MEMBER( output_io ) { if (m_slot->m_io != state) { m_slot->m_io = state; m_slot->port()->update_io(); } }
	DECLARE_WRITE_LINE_MEMBER( output_msg ) { if (m_slot->m_msg != state) { m_slot->m_msg = state; m_slot->port()->update_msg(); } }
	DECLARE_WRITE_LINE_MEMBER( output_req ) { if (m_slot->m_req != state) { m_slot->m_req = state; m_slot->port()->update_req(); } }
	DECLARE_WRITE_LINE_MEMBER( output_ack ) { if (m_slot->m_ack != state) { m_slot->m_ack = state; m_slot->port()->update_ack(); } }
	DECLARE_WRITE_LINE_MEMBER( output_atn ) { if (m_slot->m_atn != state) { m_slot->m_atn = state; m_slot->port()->update_atn(); } }
	DECLARE_WRITE_LINE_MEMBER( output_rst ) { if (m_slot->m_rst != state) { m_slot->m_rst = state; m_slot->port()->update_rst(); } }
	DECLARE_WRITE_LINE_MEMBER( output_data0 ) { if (m_slot->m_data0 != state) { m_slot->m_data0 = state; m_slot->port()->update_data0(); } }
	DECLARE_WRITE_LINE_MEMBER( output_data1 ) { if (m_slot->m_data1 != state) { m_slot->m_data1 = state; m_slot->port()->update_data1(); } }
	DECLARE_WRITE_LINE_MEMBER( output_data2 ) { if (m_slot->m_data2 != state) { m_slot->m_data2 = state; m_slot->port()->update_data2(); } }
	DECLARE_WRITE_LINE_MEMBER( output_data3 ) { if (m_slot->m_data3 != state) { m_slot->m_data3 = state; m_slot->port()->update_data3(); } }
	DECLARE_WRITE_LINE_MEMBER( output_data4 ) { if (m_slot->m_data4 != state) { m_slot->m_data4 = state; m_slot->port()->update_data4(); } }
	DECLARE_WRITE_LINE_MEMBER( output_data5 ) { if (m_slot->m_data5 != state) { m_slot->m_data5 = state; m_slot->port()->update_data5(); } }
	DECLARE_WRITE_LINE_MEMBER( output_data6 ) { if (m_slot->m_data6 != state) { m_slot->m_data6 = state; m_slot->port()->update_data6(); } }
	DECLARE_WRITE_LINE_MEMBER( output_data7 ) { if (m_slot->m_data7 != state) { m_slot->m_data7 = state; m_slot->port()->update_data7(); } }

protected:
	scsi_port_interface(const machine_config &mconfig, device_t &device);

private:
	scsi_port_slot_device *m_slot;
};

#endif // MAME_BUS_SCSI_SCSI_H
