// license:BSD-3-Clause
// copyright-holders:MetalliC
#pragma once

#ifndef __M3COMM_H__
#define __M3COMM_H__

#include "machine/ram.h"
#include "cpu/m68000/m68000.h"

#define MCFG_M3COMM_ADD(_tag ) \
	MCFG_DEVICE_ADD(_tag, M3COMM, 0)

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class m3comm_device : public device_t
{
public:
	// construction/destruction
	m3comm_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const override;

	required_shared_ptr<uint16_t> m68k_ram;
	required_device<m68000_device> m_commcpu;

	DECLARE_ADDRESS_MAP(m3_map, 32);

	DECLARE_READ16_MEMBER(ctrl_r);
	DECLARE_WRITE16_MEMBER(ctrl_w);

	DECLARE_READ16_MEMBER(ioregs_r);
	DECLARE_WRITE16_MEMBER(ioregs_w);

	DECLARE_READ16_MEMBER(m3_m68k_ram_r);
	DECLARE_WRITE16_MEMBER(m3_m68k_ram_w);
	DECLARE_READ8_MEMBER(m3_comm_ram_r);
	DECLARE_WRITE8_MEMBER(m3_comm_ram_w);
	DECLARE_READ16_MEMBER(m3_ioregs_r);
	DECLARE_WRITE16_MEMBER(m3_ioregs_w);

	DECLARE_READ16_MEMBER(naomi_r);
	DECLARE_WRITE16_MEMBER(naomi_w);

protected:
	enum { TIMER_IRQ5 = 1 };

	required_device<ram_device> m_ram;

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_reset_after_children() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
private:
	uint16_t naomi_control;
	uint16_t naomi_offset;
	uint16_t m_status0;
	uint16_t m_status1;
	uint16_t m_commbank;

	uint16_t recv_offset;
	uint16_t recv_size;
	uint16_t send_offset;
	uint16_t send_size;


	emu_file m_line_rx;    // rx line - can be either differential, simple serial or toslink
	emu_file m_line_tx;    // tx line - is differential, simple serial and toslink
	char m_localhost[256];
	char m_remotehost[256];

	emu_timer *timer;
};

// device type definition
extern const device_type M3COMM;

#endif  /* __M3COMM_H__ */
