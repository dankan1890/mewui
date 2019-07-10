// license:GPL-2.0+
// copyright-holders:Dirk Best
/***************************************************************************

    Chips & Technologies CS4031 chipset

    Chipset for 486 based PC/AT compatible systems. Consists of two
    individual chips:

    * F84031
        - DRAM controller
        - ISA-bus controller
        - VESA VL-BUS controller

    * F84035 (82C206 IPC core)
        - 2x 8257 DMA controller
        - 2x 8259 interrupt controller
        - 8254 timer
        - MC14818 RTC

***************************************************************************/

#ifndef MAME_MACHINE_CS4031_H
#define MAME_MACHINE_CS4031_H

#pragma once

#include "machine/am9517a.h"
#include "machine/pic8259.h"
#include "machine/pit8253.h"
#include "machine/ds128x.h"
#include "machine/at_keybc.h"

class cs4031_device : public device_t
{
public:
	// construction/destruction
	template <typename T, typename U, typename V, typename W>
	cs4031_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&cputag, U &&isatag, V &&biostag, W &&keybctag)
		: cs4031_device(mconfig, tag, owner, clock)
	{
		set_cputag(std::forward<T>(cputag));
		set_isatag(std::forward<U>(isatag));
		set_biostag(std::forward<V>(biostag));
		set_keybctag(std::forward<W>(keybctag));
	}

	cs4031_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// callbacks
	auto ior() { return m_read_ior.bind(); }
	auto iow() { return m_write_iow.bind(); }
	auto tc() { return m_write_tc.bind(); }
	auto hold() { return m_write_hold.bind(); }
	auto cpureset() { return m_write_cpureset.bind(); }
	auto nmi() { return m_write_nmi.bind(); }
	auto intr() { return m_write_intr.bind(); }
	auto a20m() { return m_write_a20m.bind(); }
	auto spkr() { return m_write_spkr.bind(); }

	// internal io
	void config_address_w(uint8_t data);
	uint8_t config_data_r();
	void config_data_w(uint8_t data);
	uint8_t portb_r();
	void portb_w(uint8_t data);
	void rtc_w(offs_t offset, uint8_t data);
	void sysctrl_w(uint8_t data);
	uint8_t sysctrl_r();
	uint8_t dma_page_r(offs_t offset) { return m_dma_page[offset]; }
	void dma_page_w(offs_t offset, uint8_t data) { m_dma_page[offset] = data; }
	uint8_t dma2_r(offs_t offset) { return m_dma2->read(offset / 2); }
	void dma2_w(offs_t offset, uint8_t data) { m_dma2->write(offset / 2, data); }
	uint8_t keyb_data_r();
	void keyb_data_w(uint8_t data);
	uint8_t keyb_status_r();
	void keyb_command_w(uint8_t data);
	void keyb_command_blocked_w(uint8_t data);

	// input lines
	DECLARE_WRITE_LINE_MEMBER( irq01_w ) { m_intc1->ir1_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq03_w ) { m_intc1->ir3_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq04_w ) { m_intc1->ir4_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq05_w ) { m_intc1->ir5_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq06_w ) { m_intc1->ir6_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq07_w ) { m_intc1->ir7_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq09_w ) { m_intc2->ir1_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq10_w ) { m_intc2->ir2_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq11_w ) { m_intc2->ir3_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq12_w ) { m_intc2->ir4_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq13_w ) { m_intc2->ir5_w(state); } // also FERR#
	DECLARE_WRITE_LINE_MEMBER( irq14_w ) { m_intc2->ir6_w(state); }
	DECLARE_WRITE_LINE_MEMBER( irq15_w ) { m_intc2->ir7_w(state); }
	DECLARE_WRITE_LINE_MEMBER( dreq0_w ) { m_dma1->dreq0_w(state); }
	DECLARE_WRITE_LINE_MEMBER( dreq1_w ) { m_dma1->dreq1_w(state); }
	DECLARE_WRITE_LINE_MEMBER( dreq2_w ) { m_dma1->dreq2_w(state); }
	DECLARE_WRITE_LINE_MEMBER( dreq3_w ) { m_dma1->dreq3_w(state); }
	DECLARE_WRITE_LINE_MEMBER( dreq5_w ) { m_dma2->dreq1_w(state); }
	DECLARE_WRITE_LINE_MEMBER( dreq6_w ) { m_dma2->dreq2_w(state); }
	DECLARE_WRITE_LINE_MEMBER( dreq7_w ) { m_dma2->dreq3_w(state); }
	DECLARE_WRITE_LINE_MEMBER( hlda_w ) { m_dma2->hack_w(state); }
	DECLARE_WRITE_LINE_MEMBER( iochck_w );
	DECLARE_WRITE_LINE_MEMBER( gatea20_w );
	DECLARE_WRITE_LINE_MEMBER( kbrst_w );

	IRQ_CALLBACK_MEMBER(int_ack_r) { return m_intc1->acknowledge(); }

	// inline configuration
	template <typename T> void set_cputag(T &&tag) { m_cpu.set_tag(std::forward<T>(tag)); }
	template <typename T> void set_isatag(T &&tag) { m_isa.set_tag(std::forward<T>(tag)); }
	template <typename T> void set_biostag(T &&tag) { m_bios.set_tag(std::forward<T>(tag)); }
	template <typename T> void set_keybctag(T &&tag) { m_keybc.set_tag(std::forward<T>(tag)); }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_reset_after_children() override;
	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

private:
	devcb_read16 m_read_ior;
	devcb_write16 m_write_iow;
	devcb_write8 m_write_tc;
	devcb_write_line m_write_hold;
	devcb_write_line m_write_nmi;
	devcb_write_line m_write_intr;
	devcb_write_line m_write_cpureset;
	devcb_write_line m_write_a20m;
	devcb_write_line m_write_spkr;

	offs_t page_offset();
	void set_dma_channel(int channel, bool state);
	void update_dma_clock();

	void trigger_nmi();
	void update_a20m();

	void emulated_kbreset(int state);
	void emulated_gatea20(int state);
	void fast_gatea20(int state);
	void keyboard_gatea20(int state);

	void update_read_region(int index, const char *region, offs_t start, offs_t end);
	void update_write_region(int index, const char *region, offs_t start, offs_t end);
	void update_read_regions();
	void update_write_regions();

	// internal state
	required_device<device_memory_interface> m_cpu;
	required_device<at_kbc_device_base> m_keybc;
	required_region_ptr<uint8_t> m_isa;
	required_region_ptr<uint8_t> m_bios;

	address_space *m_space;
	address_space *m_space_io;
	uint8_t *m_ram;

	// ipc core devices
	required_device<am9517a_device> m_dma1;
	required_device<am9517a_device> m_dma2;
	required_device<pic8259_device> m_intc1;
	required_device<pic8259_device> m_intc2;
	required_device<pit8254_device> m_ctc;
	required_device<ds12885_device> m_rtc;

	int m_dma_eop;
	uint8_t m_dma_page[0x10];
	uint8_t m_dma_high_byte;
	int m_dma_channel;

	uint8_t m_portb;
	int m_refresh_toggle;
	int m_iochck;
	int m_nmi_mask;

	// keyboard
	int m_cpureset;
	int m_kbrst;
	int m_ext_gatea20;
	int m_fast_gatea20;
	int m_emu_gatea20;
	bool m_keybc_d1_written;
	bool m_keybc_data_blocked;

	// chipset configuration
	static const char* const m_register_names[];
	static const float m_dma_clock_divider[];

	enum
	{
		DMA_WAIT_STATE = 0x01,
		PERFORMANCE = 0x08,
		F84035_MISC = 0x09,
		DMA_CLOCK = 0x0a,
		SHADOW_READ = 0x19,
		SHADOW_WRITE = 0x1a,
		ROMCS = 0x1b,
		SOFT_RESET_AND_GATEA20 = 0x1c
	};

	uint8_t m_address;
	bool m_address_valid;

	uint8_t m_registers[0x20];

	uint8_t dma_read_byte(offs_t offset);
	void dma_write_byte(offs_t offset, uint8_t data);
	uint8_t dma_read_word(offs_t offset);
	void dma_write_word(offs_t offset, uint8_t data);
	DECLARE_WRITE_LINE_MEMBER( dma1_eop_w );
	uint8_t dma1_ior0_r() { return m_read_ior(0); }
	uint8_t dma1_ior1_r() { return m_read_ior(1); }
	uint8_t dma1_ior2_r() { return m_read_ior(2); }
	uint8_t dma1_ior3_r() { return m_read_ior(3); }
	uint8_t dma2_ior1_r() { uint16_t const result = m_read_ior(5); m_dma_high_byte = result >> 8; return result; }
	uint8_t dma2_ior2_r() { uint16_t const result = m_read_ior(6); m_dma_high_byte = result >> 8; return result; }
	uint8_t dma2_ior3_r() { uint16_t const result = m_read_ior(7); m_dma_high_byte = result >> 8; return result; }
	void dma1_iow0_w(uint8_t data) { m_write_iow(0, data, 0xffff); }
	void dma1_iow1_w(uint8_t data) { m_write_iow(1, data, 0xffff); }
	void dma1_iow2_w(uint8_t data) { m_write_iow(2, data, 0xffff); }
	void dma1_iow3_w(uint8_t data) { m_write_iow(3, data, 0xffff); }
	void dma2_iow1_w(uint8_t data) { m_write_iow(5, (m_dma_high_byte << 8) | data, 0xffff); }
	void dma2_iow2_w(uint8_t data) { m_write_iow(6, (m_dma_high_byte << 8) | data, 0xffff); }
	void dma2_iow3_w(uint8_t data) { m_write_iow(7, (m_dma_high_byte << 8) | data, 0xffff); }
	DECLARE_WRITE_LINE_MEMBER( dma1_dack0_w ) { set_dma_channel(0, state); }
	DECLARE_WRITE_LINE_MEMBER( dma1_dack1_w ) { set_dma_channel(1, state); }
	DECLARE_WRITE_LINE_MEMBER( dma1_dack2_w ) { set_dma_channel(2, state); }
	DECLARE_WRITE_LINE_MEMBER( dma1_dack3_w ) { set_dma_channel(3, state); }
	DECLARE_WRITE_LINE_MEMBER( dma2_dack0_w );
	DECLARE_WRITE_LINE_MEMBER( dma2_dack1_w ) { set_dma_channel(5, state); }
	DECLARE_WRITE_LINE_MEMBER( dma2_dack2_w ) { set_dma_channel(6, state); }
	DECLARE_WRITE_LINE_MEMBER( dma2_dack3_w ) { set_dma_channel(7, state); }
	DECLARE_WRITE_LINE_MEMBER( dma2_hreq_w ) { m_write_hold(state); }
	DECLARE_WRITE_LINE_MEMBER( intc1_int_w ) { m_write_intr(state); }
	uint8_t intc1_slave_ack_r(offs_t offset);
	DECLARE_WRITE_LINE_MEMBER( ctc_out1_w );
	DECLARE_WRITE_LINE_MEMBER( ctc_out2_w );
};

DECLARE_DEVICE_TYPE(CS4031, cs4031_device)

#endif // MAME_MACHINE_CS4031_H
