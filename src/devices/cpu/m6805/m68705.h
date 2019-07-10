// license:BSD-3-Clause
// copyright-holders:Vas Crabb
#ifndef MAME_CPU_M6805_M68705_H
#define MAME_CPU_M6805_M68705_H

#pragma once

#include "m6805.h"

DECLARE_DEVICE_TYPE(M6805P2, m6805p2_device)
DECLARE_DEVICE_TYPE(M6805P6, m6805p6_device)
DECLARE_DEVICE_TYPE(M6805R2, m6805r2_device)
DECLARE_DEVICE_TYPE(M6805R3, m6805r3_device)
//DECLARE_DEVICE_TYPE(M6805S2, m6805s2_device) // A/D, SPI, multiple timers
//DECLARE_DEVICE_TYPE(M6805S3, m6805s3_device) // A/D, SPI, multiple timers
DECLARE_DEVICE_TYPE(M6805U2, m6805u2_device)
DECLARE_DEVICE_TYPE(M6805U3, m6805u3_device)

DECLARE_DEVICE_TYPE(M68705P3, m68705p3_device)
DECLARE_DEVICE_TYPE(M68705P5, m68705p5_device)
DECLARE_DEVICE_TYPE(M68705R3, m68705r3_device)
//DECLARE_DEVICE_TYPE(M68705R5, m68705r5_device) // A/D, Secured EPROM
//DECLARE_DEVICE_TYPE(M68705S3, m68705s3_device) // A/D, SPI, multiple timers
DECLARE_DEVICE_TYPE(M68705U3, m68705u3_device)
//DECLARE_DEVICE_TYPE(M68705U5, m68705u5_device) // Secured EPROM

class m6805_timer
{
public:
	enum timer_options : u8
	{
		TIMER_PGM = 0x01, // programmable source and divisor
		TIMER_MOR = 0x02, // configured by mask option rom
		TIMER_NPC = 0x04, // no prescaler clear
	};
	enum timer_source : unsigned
	{
		CLOCK       = 0, // internal clock
		CLOCK_TIMER = 1, // internal clock AND external input
		DISABLED    = 2,
		TIMER       = 3, // external input
	};

	m6805_timer(m6805_base_device &parent)
		: m_parent(parent)
		, m_options(0)
		, m_divisor(7)
		, m_source(CLOCK)
		, m_timer(false)
	{
	}

	// configuration helpers
	void set_options(timer_options options) { m_options = options; }
	void set_divisor(unsigned divisor) { m_divisor = divisor & 7; }
	void set_source(timer_source source) { m_source = source; }

	void start(unsigned base = 0)
	{
		m_parent.save_item(NAME(m_timer));
		m_parent.save_item(NAME(m_timer_edges));
		m_parent.save_item(NAME(m_prescale));
		m_parent.save_item(NAME(m_tdr));
		m_parent.save_item(NAME(m_tcr));

		m_parent.state_add(base + 0, "PS", m_prescale);
		m_parent.state_add(base + 1, "TDR", m_tdr);
		m_parent.state_add(base + 2, "TCR", m_tcr);
	}

	void reset()
	{
		m_timer_edges = 0;
		m_prescale = 0x7f;
		m_tdr = 0xff;
		m_tcr = 0x7f;
	}

	u8 tdr_r() { return m_tdr; }
	void tdr_w(u8 data) { m_tdr = data; }

	u8 tcr_r() { return (m_options & TIMER_MOR) ? m_tcr | TCR_PSC : m_tcr & ~TCR_PSC; }
	void tcr_w(u8 data);

	void update(unsigned count);
	void timer_w(int state);

private:
	enum tcr_mask : u8
	{
		TCR_PS  = 0x07, // prescaler value
		TCR_PSC = 0x08, // prescaler clear (write only, read as zero)
		TCR_TIE = 0x10, // timer external input enable
		TCR_TIN = 0x20, // timer input select
		TCR_TIM = 0x40, // timer interrupt mask
		TCR_TIR = 0x80, // timer interrupt request
	};

	// configuration state
	cpu_device &m_parent;
	u8 m_options;

	// internal state
	unsigned m_divisor;
	timer_source m_source;
	bool m_timer;
	unsigned m_timer_edges;
	u8 m_prescale;

	// visible state
	u8 m_tdr;
	u8 m_tcr;
};

// abstract device classes
class m6805_hmos_device : public m6805_base_device
{
public:
	// configuration helpers
	auto porta_r() { return m_port_cb_r[0].bind(); }
	auto portb_r() { return m_port_cb_r[1].bind(); }
	auto portc_r() { return m_port_cb_r[2].bind(); }
	auto portd_r() { return m_port_cb_r[3].bind(); }
	auto porta_w() { return m_port_cb_w[0].bind(); }
	auto portb_w() { return m_port_cb_w[1].bind(); }
	auto portc_w() { return m_port_cb_w[2].bind(); }

	WRITE_LINE_MEMBER(timer_w) { m_timer.timer_w(state); }

protected:
	// state index constants
	enum
	{
		M68705_A = M6805_A,
		M68705_PC = M6805_PC,
		M68705_S = M6805_S,
		M68705_X = M6805_X,
		M68705_CC = M6805_CC,
		M68705_IRQ_STATE = M6805_IRQ_STATE,

		M68705_LATCHA = 0x10,
		M68705_LATCHB,
		M68705_LATCHC,
		M68705_LATCHD,
		M68705_DDRA,
		M68705_DDRB,
		M68705_DDRC,
		M68705_DDRD,

		M6805_PS,
		M6805_TDR,
		M6805_TCR,

		M68705_PCR,
		M68705_PLD,
		M68705_PLA,

		M68705_MOR
	};

	static unsigned const PORT_COUNT = 4;

	m6805_hmos_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock, device_type type, u32 addr_width, unsigned ram_size);

	void map(address_map &map) { internal_map(map); }
	virtual void internal_map(address_map &map);

	template <std::size_t N> void set_port_open_drain(bool value);
	template <std::size_t N> void set_port_mask(u8 mask);
	template <std::size_t N> DECLARE_WRITE8_MEMBER(port_input_w) { m_port_input[N] = data & ~m_port_mask[N]; }
	template <std::size_t N> DECLARE_READ8_MEMBER(port_r);
	template <std::size_t N> DECLARE_WRITE8_MEMBER(port_latch_w);
	template <std::size_t N> DECLARE_WRITE8_MEMBER(port_ddr_w);
	template <std::size_t N> void port_cb_w();

	DECLARE_READ8_MEMBER(misc_r) { return m_mr; }
	DECLARE_WRITE8_MEMBER(misc_w) { m_mr = data; }

	// A/D converter
	DECLARE_READ8_MEMBER(acr_r);
	DECLARE_WRITE8_MEMBER(acr_w);
	DECLARE_READ8_MEMBER(arr_r);
	DECLARE_WRITE8_MEMBER(arr_w);

	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void execute_set_input(int inputnum, int state) override;

	virtual void interrupt() override;
	virtual void burn_cycles(unsigned count) override;

	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;

	template <std::size_t N> void add_port_latch_state();
	template <std::size_t N> void add_port_ddr_state();

	// timer/counter
	m6805_timer m_timer;

private:
	// digital I/O
	bool            m_port_open_drain[PORT_COUNT];
	u8              m_port_mask[PORT_COUNT];
	u8              m_port_input[PORT_COUNT];
	u8              m_port_latch[PORT_COUNT];
	u8              m_port_ddr[PORT_COUNT];
	devcb_read8     m_port_cb_r[PORT_COUNT];
	devcb_write8    m_port_cb_w[PORT_COUNT];

	// miscellaneous register
	enum mr_mask : u8
	{
		MR_INT2_MSK = 0x40, // INT2 interrupt mask
		MR_INT2_REQ = 0x80, // INT2 interrupt request
	};
	u8 m_mr;

	unsigned const m_ram_size;
};

class m6805_mrom_device : public m6805_hmos_device
{
protected:
	m6805_mrom_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock, device_type type, u32 addr_width, unsigned ram_size)
		: m6805_hmos_device(mconfig, tag, owner, clock, type, addr_width, ram_size)
	{
	}

	virtual void internal_map(address_map &map) override;
};

class m68705_device : public m6805_hmos_device, public device_nvram_interface
{
public:
	enum mor_mask : u8
	{
		MOR_PS   = 0x07, // prescaler divisor
		MOR_TIE  = 0x10, // timer external enable
		MOR_CLS  = 0x20, // clock source
		MOR_TOPT = 0x40, // timer option
		MOR_CLK  = 0x80, // oscillator type
	};

protected:
	virtual void internal_map(address_map &map) override;

	m68705_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock, device_type type, u32 addr_width, unsigned ram_size);

	template <offs_t B> DECLARE_READ8_MEMBER(eprom_r);
	template <offs_t B> DECLARE_WRITE8_MEMBER(eprom_w);

	DECLARE_READ8_MEMBER(pcr_r);
	DECLARE_WRITE8_MEMBER(pcr_w);

	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void execute_set_input(int inputnum, int state) override;
	virtual void nvram_default() override;
	virtual void nvram_read(emu_file &file) override;
	virtual void nvram_write(emu_file &file) override;

	u8 *const get_user_rom() const { return &m_user_rom[0]; }
	virtual u8 get_mask_options() const = 0;

private:
	bool    pcr_vpon() const    { return !BIT(m_pcr, 2); }
	bool    pcr_pge() const     { return !BIT(m_pcr, 1); }
	bool    pcr_ple() const     { return !BIT(m_pcr, 0); }

	required_region_ptr<u8> m_user_rom;

	// EPROM control
	u8  m_vihtp;
	u8  m_pcr;
	u8  m_pl_data;
	u16 m_pl_addr;
};

class m68705p_device : public m68705_device
{
public:
	DECLARE_WRITE8_MEMBER(pa_w) { port_input_w<0>(space, offset, data, mem_mask); }
	DECLARE_WRITE8_MEMBER(pb_w) { port_input_w<1>(space, offset, data, mem_mask); }
	DECLARE_WRITE8_MEMBER(pc_w) { port_input_w<2>(space, offset, data, mem_mask); }

protected:
	virtual void internal_map(address_map &map) override;

	m68705p_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock, device_type type);

	virtual void device_start() override;

	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;
};

class m68705u_device : public m68705_device
{
public:
	DECLARE_WRITE8_MEMBER(pa_w) { port_input_w<0>(space, offset, data, mem_mask); }
	DECLARE_WRITE8_MEMBER(pb_w) { port_input_w<1>(space, offset, data, mem_mask); }
	DECLARE_WRITE8_MEMBER(pc_w) { port_input_w<2>(space, offset, data, mem_mask); }
	DECLARE_WRITE8_MEMBER(pd_w) { port_input_w<3>(space, offset, data, mem_mask); } // TODO: PD6 is also /INT2

protected:
	virtual void internal_map(address_map &map) override;

	m68705u_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock, device_type type);

	virtual void device_start() override;

	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;
};

class m68705r_device : public m68705u_device
{
public:
	// TODO: voltage inputs for ADC (shared with digital port D pins)

protected:
	virtual void internal_map(address_map &map) override;

	m68705r_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock, device_type type);

	virtual void device_start() override;

	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;
};

// concrete device classes
class m6805p2_device : public m6805_mrom_device
{
public:
	m6805p2_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);
};

class m6805p6_device : public m6805_mrom_device
{
public:
	m6805p6_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);
};

class m6805r2_device : public m6805_mrom_device
{
public:
	m6805r2_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

protected:
	virtual void internal_map(address_map &map) override;
};

class m6805r3_device : public m6805_mrom_device
{
public:
	m6805r3_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

protected:
	virtual void internal_map(address_map &map) override;
};

class m6805u2_device : public m6805_mrom_device
{
public:
	m6805u2_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);
};

class m6805u3_device : public m6805_mrom_device
{
public:
	m6805u3_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

protected:
};

class m68705p3_device : public m68705p_device
{
public:
	m68705p3_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

protected:
	virtual tiny_rom_entry const *device_rom_region() const override;

	virtual u8 get_mask_options() const override;
};

class m68705p5_device : public m68705p_device
{
public:
	m68705p5_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

protected:
	virtual tiny_rom_entry const *device_rom_region() const override;

	virtual u8 get_mask_options() const override;
};

class m68705r3_device : public m68705r_device
{
public:
	m68705r3_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

protected:
	virtual tiny_rom_entry const *device_rom_region() const override;

	virtual u8 get_mask_options() const override;
};

class m68705u3_device : public m68705u_device
{
public:
	m68705u3_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

protected:
	virtual tiny_rom_entry const *device_rom_region() const override;

	virtual u8 get_mask_options() const override;
};

#define M6805_INT_TIMER             (M6805_IRQ_LINE + 1)

#define M68705_IRQ_LINE             (M6805_IRQ_LINE + 0)
#define M68705_INT_TIMER            (M6805_IRQ_LINE + 1)
#define M68705_VPP_LINE             (M6805_IRQ_LINE + 2)
#define M68705_VIHTP_LINE           (M6805_IRQ_LINE + 3)

#endif // MAME_CPU_M6805_M68705_H
