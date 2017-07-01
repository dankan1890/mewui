// license:BSD-3-Clause
// copyright-holders:Tony La Porta
	/**************************************************************************\
	*                      Microchip PIC16C5x Emulator                         *
	*                                                                          *
	*                    Copyright Tony La Porta                               *
	*                 Originally written for the MAME project.                 *
	*                                                                          *
	*                                                                          *
	*      Addressing architecture is based on the Harvard addressing scheme.  *
	*                                                                          *
	\**************************************************************************/

#pragma once

#ifndef __PIC16C5X_H__
#define __PIC16C5X_H__


// i/o ports
enum
{
	PIC16C5x_PORTA = 0,
	PIC16C5x_PORTB,
	PIC16C5x_PORTC
};

// port a, 4 bits, 2-way
#define MCFG_PIC16C5x_READ_A_CB(_devcb) \
	pic16c5x_device::set_read_a_callback(*device, DEVCB_##_devcb);
#define MCFG_PIC16C5x_WRITE_A_CB(_devcb) \
	pic16c5x_device::set_write_a_callback(*device, DEVCB_##_devcb);

// port b, 8 bits, 2-way
#define MCFG_PIC16C5x_READ_B_CB(_devcb) \
	pic16c5x_device::set_read_b_callback(*device, DEVCB_##_devcb);
#define MCFG_PIC16C5x_WRITE_B_CB(_devcb) \
	pic16c5x_device::set_write_b_callback(*device, DEVCB_##_devcb);

// port c, 8 bits, 2-way
#define MCFG_PIC16C5x_READ_C_CB(_devcb) \
	pic16c5x_device::set_read_c_callback(*device, DEVCB_##_devcb);
#define MCFG_PIC16C5x_WRITE_C_CB(_devcb) \
	pic16c5x_device::set_write_c_callback(*device, DEVCB_##_devcb);

// T0 pin (readline)
#define MCFG_PIC16C5x_T0_CB(_devcb) \
	pic16c5x_device::set_t0_callback(*device, DEVCB_##_devcb);

// CONFIG register
#define MCFG_PIC16C5x_SET_CONFIG(_data) \
	pic16c5x_device::set_config_static(*device, _data);



extern const device_type PIC16C54;
extern const device_type PIC16C55;
extern const device_type PIC16C56;
extern const device_type PIC16C57;
extern const device_type PIC16C58;


class pic16c5x_device : public cpu_device
{
public:
	// construction/destruction
	pic16c5x_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, int program_width, int data_width, int picmodel);

	// static configuration helpers
	template<class _Object> static devcb_base &set_read_a_callback(device_t &device, _Object object) { return downcast<pic16c5x_device &>(device).m_read_a.set_callback(object); }
	template<class _Object> static devcb_base &set_read_b_callback(device_t &device, _Object object) { return downcast<pic16c5x_device &>(device).m_read_b.set_callback(object); }
	template<class _Object> static devcb_base &set_read_c_callback(device_t &device, _Object object) { return downcast<pic16c5x_device &>(device).m_read_c.set_callback(object); }

	template<class _Object> static devcb_base &set_write_a_callback(device_t &device, _Object object) { return downcast<pic16c5x_device &>(device).m_write_a.set_callback(object); }
	template<class _Object> static devcb_base &set_write_b_callback(device_t &device, _Object object) { return downcast<pic16c5x_device &>(device).m_write_b.set_callback(object); }
	template<class _Object> static devcb_base &set_write_c_callback(device_t &device, _Object object) { return downcast<pic16c5x_device &>(device).m_write_c.set_callback(object); }

	template<class _Object> static devcb_base &set_t0_callback(device_t &device, _Object object) { return downcast<pic16c5x_device &>(device).m_read_t0.set_callback(object); }

	/****************************************************************************
	 *  Function to configure the CONFIG register. This is actually hard-wired
	 *  during ROM programming, so should be called in the driver INIT, with
	 *  the value if known (available in HEX dumps of the ROM).
	 */
	void pic16c5x_set_config(uint16_t data);

	// or with a macro
	static void set_config_static(device_t &device, uint16_t data) { downcast<pic16c5x_device &>(device).m_temp_config = data; }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	/**************************************************************************
	 *  Internal Clock divisor
	 *
	 *  External Clock is divided internally by 4 for the instruction cycle
	 *  times. (Each instruction cycle passes through 4 machine states). This
	 *  is handled by the cpu execution engine.
	 */
	virtual uint64_t execute_clocks_to_cycles(uint64_t clocks) const override { return (clocks + 4 - 1) / 4; }
	virtual uint64_t execute_cycles_to_clocks(uint64_t cycles) const override { return (cycles * 4); }
	virtual uint32_t execute_min_cycles() const override { return 1; }
	virtual uint32_t execute_max_cycles() const override { return 2; }
	virtual uint32_t execute_input_lines() const override { return 1; }
	virtual uint32_t execute_default_irq_vector() const override { return 0; }
	virtual void execute_run() override;

	// device_memory_interface overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const override
	{
		return (spacenum == AS_PROGRAM) ? &m_program_config : ( (spacenum == AS_DATA) ? &m_data_config : nullptr );
	}

	// device_state_interface overrides
	virtual void state_import(const device_state_entry &entry) override;
	virtual void state_export(const device_state_entry &entry) override;
	virtual void state_string_export(const device_state_entry &entry, std::string &str) const override;

	// device_disasm_interface overrides
	virtual uint32_t disasm_min_opcode_bytes() const override { return 2; }
	virtual uint32_t disasm_max_opcode_bytes() const override { return 2; }
	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;

private:
	address_space_config m_program_config;
	address_space_config m_data_config;

	/******************** CPU Internal Registers *******************/
	uint16_t  m_PC;
	uint16_t  m_PREVPC;     /* previous program counter */
	uint8_t   m_W;
	uint8_t   m_OPTION;
	uint16_t  m_CONFIG;
	uint8_t   m_ALU;
	uint16_t  m_WDT;
	uint8_t   m_TRISA;
	uint8_t   m_TRISB;
	uint8_t   m_TRISC;
	uint16_t  m_STACK[2];
	uint16_t  m_prescaler;  /* Note: this is really an 8-bit register */
	PAIR    m_opcode;
	uint8_t   *m_internalram;

	int     m_icount;
	int     m_reset_vector;
	int     m_picmodel;
	int     m_delay_timer;
	uint16_t  m_temp_config;
	uint8_t   m_old_T0;
	int8_t    m_old_data;
	uint8_t   m_picRAMmask;
	int     m_inst_cycles;

	address_space *m_program;
	direct_read_data *m_direct;
	address_space *m_data;

	// i/o handlers
	devcb_read8 m_read_a;
	devcb_read8 m_read_b;
	devcb_read8 m_read_c;
	devcb_write8 m_write_a;
	devcb_write8 m_write_b;
	devcb_write8 m_write_c;
	devcb_read_line m_read_t0;

	// For debugger
	int m_debugger_temp;

	/* opcode table entry */
	typedef void (pic16c5x_device::*pic16c5x_ophandler)();
	struct pic16c5x_opcode
	{
		uint8_t   cycles;
		pic16c5x_ophandler function;
	};
	static const pic16c5x_opcode s_opcode_main[256];
	static const pic16c5x_opcode s_opcode_00x[16];

	void update_internalram_ptr();
	void CALCULATE_Z_FLAG();
	void CALCULATE_ADD_CARRY();
	void CALCULATE_ADD_DIGITCARRY();
	void CALCULATE_SUB_CARRY();
	void CALCULATE_SUB_DIGITCARRY();
	uint16_t POP_STACK();
	void PUSH_STACK(uint16_t data);
	uint8_t GET_REGFILE(offs_t addr);
	void STORE_REGFILE(offs_t addr, uint8_t data);
	void STORE_RESULT(offs_t addr, uint8_t data);
	void illegal();
	void addwf();
	void andwf();
	void andlw();
	void bcf();
	void bsf();
	void btfss();
	void btfsc();
	void call();
	void clrw();
	void clrf();
	void clrwdt();
	void comf();
	void decf();
	void decfsz();
	void goto_op();
	void incf();
	void incfsz();
	void iorlw();
	void iorwf();
	void movf();
	void movlw();
	void movwf();
	void nop();
	void option();
	void retlw();
	void rlf();
	void rrf();
	void sleepic();
	void subwf();
	void swapf();
	void tris();
	void xorlw();
	void xorwf();
	void pic16c5x_reset_regs();
	void pic16c5x_soft_reset();
	void pic16c5x_update_watchdog(int counts);
	void pic16c5x_update_timer(int counts);

};


class pic16c54_device : public pic16c5x_device
{
public:
	// construction/destruction
	pic16c54_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class pic16c55_device : public pic16c5x_device
{
public:
	// construction/destruction
	pic16c55_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class pic16c56_device : public pic16c5x_device
{
public:
	// construction/destruction
	pic16c56_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class pic16c57_device : public pic16c5x_device
{
public:
	// construction/destruction
	pic16c57_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class pic16c58_device : public pic16c5x_device
{
public:
	// construction/destruction
	pic16c58_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};

#endif  /* __PIC16C5X_H__ */
