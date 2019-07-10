// license:BSD-3-Clause
// copyright-holders:Luca Elia
#ifndef MAME_CPU_TLCS90_TLCS90_H
#define MAME_CPU_TLCS90_TLCS90_H

#pragma once

enum tlcs90_e_irq {    INTSWI = 0, INTNMI, INTWD,  INT0,   INTT0,  INTT1,  INTT2,  INTT3,  INTT4,  INT1,   INTT5,  INT2,   INTRX,  INTTX,  INTMAX  };
DECLARE_ENUM_INCDEC_OPERATORS(tlcs90_e_irq)

class tlcs90_device : public cpu_device
{
	static constexpr int MAX_PORTS = 9;
	//static constexpr int MAX_ANALOG_INPUTS = 16;

public:
	DECLARE_READ8_MEMBER( t90_internal_registers_r );
	DECLARE_WRITE8_MEMBER( t90_internal_registers_w );

	TIMER_CALLBACK_MEMBER( t90_timer_callback );
	TIMER_CALLBACK_MEMBER( t90_timer4_callback );

	void tmp90840_mem(address_map &map);
	void tmp90841_mem(address_map &map);
	void tmp90ph44_mem(address_map &map);
	void tmp91640_mem(address_map &map);
	void tmp91641_mem(address_map &map);
protected:
	enum _e_op {    UNKNOWN,    NOP,    EX,     EXX,    LD,     LDW,    LDA,    LDI,    LDIR,   LDD,    LDDR,   CPI,    CPIR,   CPD,    CPDR,   PUSH,   POP,    JP,     JR,     CALL,   CALLR,      RET,    RETI,   HALT,   DI,     EI,     SWI,    DAA,    CPL,    NEG,    LDAR,   RCF,    SCF,    CCF,    TSET,   BIT,    SET,    RES,    INC,    DEC,    INCX,   DECX,   INCW,   DECW,   ADD,    ADC,    SUB,    SBC,    AND,    XOR,    OR,     CP,     RLC,    RRC,    RL,     RR,     SLA,    SRA,    SLL,    SRL,    RLD,    RRD,    DJNZ,   MUL,    DIV     };


public:
	// configuration
	template <size_t Port> auto port_read() { return m_port_read_cb[Port].bind(); }
	template <size_t Port> auto port_write() { return m_port_write_cb[Port].bind(); }

protected:
	// construction/destruction
	tlcs90_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, address_map_constructor program_map);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual uint32_t execute_min_cycles() const override { return 2; }
	virtual uint32_t execute_max_cycles() const override { return 26; }
	virtual uint32_t execute_input_lines() const override { return 1; }
	virtual bool execute_input_edge_triggered(int inputnum) const override { return inputnum == INPUT_LINE_NMI; }
	virtual void execute_run() override;
	virtual void execute_set_input(int inputnum, int state) override;
	virtual void execute_burn(int32_t cycles) override;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;

	// device_state_interface overrides
	virtual void state_string_export(const device_state_entry &entry, std::string &str) const override;

	// device_disasm_interface overrides
	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;

private:
	enum class e_mode : u8 {
		NONE,   BIT8,   CC,
		I8,     D8,     R8,
		I16,    D16,    R16,
		MI16,   MR16,   MR16D8, MR16R8,
		R16D8,  R16R8
	};

	address_space_config m_program_config;

	devcb_read8 m_port_read_cb[MAX_PORTS];
	devcb_write8 m_port_write_cb[MAX_PORTS];

	PAIR        m_prvpc,m_pc,m_sp,m_af,m_bc,m_de,m_hl,m_ix,m_iy;
	PAIR        m_af2,m_bc2,m_de2,m_hl2;
	uint8_t       m_halt, m_after_EI;
	uint16_t      m_irq_state, m_irq_line_state, m_irq_mask;
	address_space *m_program;
	int     m_icount;
	int         m_extra_cycles;       // extra cycles for interrupts

	uint8_t m_port_latch[MAX_PORTS];

	uint8_t m_p4cr;
	uint8_t m_p67cr;
	uint8_t m_p8cr;
	uint8_t m_smmod;

	uint32_t      m_ixbase,m_iybase;

	// Timers: 4 x 8-bit + 1 x 16-bit
	emu_timer   *m_timer[4+1];
	uint8_t       m_timer_value[4];
	uint16_t      m_timer4_value;
	attotime    m_timer_period;
	uint8_t m_tmod;
	uint8_t m_tclk;
	uint8_t m_trun;
	uint8_t m_treg_8bit[4];
	uint8_t m_t4mod;
	uint16_t m_treg_16bit[2];

	// Work registers
	uint8_t        m_op;

	e_mode  m_mode1;
	uint16_t  m_r1,m_r1b;

	e_mode  m_mode2;
	uint16_t  m_r2,m_r2b;

	int m_cyc_t,m_cyc_f;

	uint32_t  m_addr;

	inline uint8_t  RM8 (uint32_t a);
	inline uint16_t RM16(uint32_t a);
	inline void WM8 (uint32_t a, uint8_t  v);
	inline void WM16(uint32_t a, uint16_t v);
	inline uint8_t  RX8 (uint32_t a, uint32_t base);
	inline uint16_t RX16(uint32_t a, uint32_t base);
	inline void WX8 (uint32_t a, uint8_t  v, uint32_t base);
	inline void WX16(uint32_t a, uint16_t v, uint32_t base);
	inline uint8_t  READ8();
	inline uint16_t READ16();
	void decode();
	inline uint16_t r8( const uint16_t r );
	inline void w8( const uint16_t r, uint16_t value );
	inline uint16_t r16( const uint16_t r );
	inline void w16( const uint16_t r, uint16_t value );
	inline uint8_t Read1_8();
	inline uint16_t Read1_16();
	inline uint8_t Read2_8();
	inline uint16_t Read2_16();
	inline void Write1_8( uint8_t value );
	inline void Write1_16( uint16_t value );
	inline void Write2_8( uint8_t value );
	inline void Write2_16( uint16_t value );
	inline int Test( uint8_t cond );
	inline void Push( uint16_t rr );
	inline void Pop( uint16_t rr );
	inline void leave_halt();
	inline void raise_irq(int irq);
	inline void clear_irq(int irq);
	void take_interrupt(tlcs90_e_irq irq);
	void check_interrupts();
	inline void Cyc();
	inline void Cyc_f();
	void t90_start_timer(int i);
	void t90_start_timer4();
	void t90_stop_timer(int i);
	void t90_stop_timer4();
	void set_irq_line(int irq, int state);
};


class tmp90840_device : public tlcs90_device
{
public:
	// construction/destruction
	tmp90840_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class tmp90841_device : public tlcs90_device
{
public:
	// construction/destruction
	tmp90841_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class tmp90845_device : public tlcs90_device
{
public:
	// construction/destruction
	tmp90845_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class tmp91640_device : public tlcs90_device
{
public:
	// construction/destruction
	tmp91640_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class tmp91641_device : public tlcs90_device
{
public:
	// construction/destruction
	tmp91641_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class tmp90ph44_device : public tlcs90_device
{
public:
	// construction/destruction
	tmp90ph44_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


DECLARE_DEVICE_TYPE(TMP90840,  tmp90840_device)
DECLARE_DEVICE_TYPE(TMP90841,  tmp90841_device)
DECLARE_DEVICE_TYPE(TMP90845,  tmp90845_device)
DECLARE_DEVICE_TYPE(TMP91640,  tmp91640_device)
DECLARE_DEVICE_TYPE(TMP91641,  tmp91641_device)
DECLARE_DEVICE_TYPE(TMP90PH44, tmp90ph44_device)

#endif // MAME_CPU_TLCS90_TLCS90_H
