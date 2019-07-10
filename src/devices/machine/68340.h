// license:BSD-3-Clause
// copyright-holders:David Haywood
/* 68340 */
#ifndef MAME_MACHINE_68340_H
#define MAME_MACHINE_68340_H

#pragma once

#include "cpu/m68000/m68000.h"

#include "68340sim.h"
#include "68340dma.h"
#include "68340ser.h"
#include "68340tmu.h"


class m68340_cpu_device : public fscpu32_device
{
	friend class mc68340_serial_module_device;
	friend class mc68340_timer_module_device;

public:
	m68340_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto pa_in_callback() { return m_pa_in_cb.bind(); }
	auto pa_out_callback() { return m_pa_out_cb.bind(); }
	auto pb_in_callback() { return m_pb_in_cb.bind(); }
	auto pb_out_callback() { return m_pb_out_cb.bind(); }

	auto tout1_out_callback() { return m_timer[0]->m_tout_out_cb.bind(); }
	auto tin1_in_callback() { return m_timer[0]->m_tin_in_cb.bind(); }
	auto tgate1_in_callback() { return m_timer[0]->m_tgate_in_cb.bind(); }
	auto tout2_out_callback() { return m_timer[1]->m_tout_out_cb.bind(); }
	auto tin2_in_callback() { return m_timer[1]->m_tin_in_cb.bind(); }
	auto tgate2_in_callback() { return m_timer[1]->m_tgate_in_cb.bind(); }

	uint16_t get_cs(offs_t address);

	void set_crystal(const XTAL &crystal) { set_crystal(crystal.value()); }

	// Timer input methods, can be used instead of the corresponding polling MCFG callbacks
	DECLARE_WRITE_LINE_MEMBER( tin1_w )  { m_timer[0]->tin_w(state);  }
	DECLARE_WRITE_LINE_MEMBER( tgate1_w ){ m_timer[0]->tgate_w(state); }
	DECLARE_WRITE_LINE_MEMBER( tin2_w )  { m_timer[1]->tin_w(state);  }
	DECLARE_WRITE_LINE_MEMBER( tgate2_w ){ m_timer[1]->tgate_w(state); }

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;

	virtual void m68k_reset_peripherals() override;

private:
	required_device<mc68340_serial_module_device> m_serial;
	required_device_array<mc68340_timer_module_device, 2> m_timer;

	void update_ipl();
	void internal_vectors_r(address_map &map);
	uint8_t int_ack(offs_t offset);

	TIMER_CALLBACK_MEMBER(periodic_interrupt_timer_callback);

	void start_68340_sim();
	void do_tick_pit();
	uint8_t pit_irq_level() const;
	uint8_t pit_arbitrate(uint8_t level) const;
	uint8_t pit_iack();

	int calc_cs(offs_t address) const;
	int get_timer_index(mc68340_timer_module_device *timer) { return (timer == m_timer[0].target()) ? 0 : 1; }

	int m_currentcs;
	uint32_t m_clock_mode;
	uint32_t m_modck;
	uint32_t m_crystal;
	uint32_t m_extal;

	// TODO: Support Limp mode and external clock with no PLL
	void set_crystal(int crystal)
	{
		m_crystal = crystal;
		m_clock_mode |= (m68340_sim::CLOCK_MODCK | m68340_sim::CLOCK_PLL);
	}

	READ32_MEMBER( m68340_internal_base_r );
	WRITE32_MEMBER( m68340_internal_base_w );
	READ32_MEMBER( m68340_internal_dma_r );
	WRITE32_MEMBER( m68340_internal_dma_w );
	READ16_MEMBER( m68340_internal_sim_r );
	READ8_MEMBER( m68340_internal_sim_ports_r );
	READ32_MEMBER( m68340_internal_sim_cs_r );
	WRITE16_MEMBER( m68340_internal_sim_w );
	WRITE8_MEMBER( m68340_internal_sim_ports_w );
	WRITE32_MEMBER( m68340_internal_sim_cs_w );

	// Clock/VCO setting TODO: support external clock with PLL and Limp mode
	DECLARE_WRITE_LINE_MEMBER( set_modck );
	DECLARE_WRITE_LINE_MEMBER( extal_w );

	void m68340_internal_map(address_map &map);

	/* 68340 peripheral modules */
	m68340_sim*    m_m68340SIM;
	m68340_dma*    m_m68340DMA;

	uint32_t m_m68340_base;

	emu_timer *m_irq_timer;

	uint8_t m_ipl;

	devcb_write8        m_pa_out_cb;
	devcb_read8         m_pa_in_cb;
	devcb_write8        m_pb_out_cb;
	devcb_read8         m_pb_in_cb;
};

DECLARE_DEVICE_TYPE(M68340, m68340_cpu_device)

#endif // MAME_MACHINE_68340_H
