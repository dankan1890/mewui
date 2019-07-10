// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    Atari CAGE Audio Board

****************************************************************************/

#ifndef MAME_AUDIO_CAGE_H
#define MAME_AUDIO_CAGE_H

#pragma once

#include "cpu/tms32031/tms32031.h"
#include "machine/gen_latch.h"
#include "machine/timer.h"
#include "sound/dmadac.h"


class atari_cage_device : public device_t
{
public:
	enum
	{
		CAGE_IRQ_REASON_DATA_READY = 0x01,
		CAGE_IRQ_REASON_BUFFER_EMPTY = 0x02
	};

	// construction/destruction
	atari_cage_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_speedup(offs_t speedup) { m_speedup = speedup; }
	auto irq_handler() { return m_irqhandler.bind(); }

	void reset_w(int state);

	uint16_t main_r();
	void main_w(uint16_t data);

	uint16_t control_r();
	void control_w(uint16_t data);

	void update_dma_state(address_space &space);
	void update_timer(int which);
	void update_serial();
	READ32_MEMBER( tms32031_io_r );
	WRITE32_MEMBER( tms32031_io_w );
	void update_control_lines();
	READ32_MEMBER( cage_from_main_r );
	WRITE32_MEMBER( cage_from_main_ack_w );
	WRITE32_MEMBER( cage_to_main_w );
	READ32_MEMBER( cage_io_status_r );
	TIMER_CALLBACK_MEMBER( cage_deferred_w );
	WRITE32_MEMBER( speedup_w );

	void cage_map(address_map &map);
protected:
	atari_cage_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;

	TIMER_DEVICE_CALLBACK_MEMBER( dma_timer_callback );
	TIMER_DEVICE_CALLBACK_MEMBER( cage_timer_callback );

	required_device<tms32031_device> m_cpu;

private:
	required_shared_ptr<uint32_t> m_cageram;
	required_device<generic_latch_16_device> m_soundlatch;
	required_device<timer_device> m_dma_timer;
	required_device_array<timer_device, 2> m_timer;
	optional_device_array<dmadac_sound_device, 4> m_dmadac;

	required_memory_bank m_bootbank;
	required_memory_bank m_mainbank;

	required_memory_region m_bootrom;
	required_memory_region m_mainrom;

	attotime m_cpu_h1_clock_period;

	uint8_t m_cpu_to_cage_ready;
	uint8_t m_cage_to_cpu_ready;

	devcb_write8 m_irqhandler;

	attotime m_serial_period_per_word;

	uint8_t m_dma_enabled;
	uint8_t m_dma_timer_enabled;

	uint8_t m_timer_enabled[2];

	uint32_t m_tms32031_io_regs[0x100];
	uint16_t m_from_main;
	uint16_t m_control;

	uint32_t *m_speedup_ram;

	offs_t m_speedup;
};


// device type definition
DECLARE_DEVICE_TYPE(ATARI_CAGE, atari_cage_device)

class atari_cage_seattle_device : public atari_cage_device
{
public:
	// construction/destruction
	atari_cage_seattle_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void cage_map_seattle(address_map &map);
protected:
	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

};

// device type definition
DECLARE_DEVICE_TYPE(ATARI_CAGE_SEATTLE, atari_cage_seattle_device)

#endif // MAME_AUDIO_CAGE_H
