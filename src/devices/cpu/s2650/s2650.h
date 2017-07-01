// license:BSD-3-Clause
// copyright-holders:Juergen Buchmueller
#pragma once

#ifndef __S2650_H__
#define __S2650_H__


#define S2650_SENSE_LINE INPUT_LINE_IRQ1

enum
{
	S2650_PC=1, S2650_PS, S2650_R0, S2650_R1, S2650_R2, S2650_R3,
	S2650_R1A, S2650_R2A, S2650_R3A,
	S2650_HALT, S2650_SI, S2650_FO
};

/* fake I/O space ports */
enum
{
	S2650_EXT_PORT      = 0x00ff,   /* M/~IO=0 D/~C=x E/~NE=1 */
	S2650_CTRL_PORT     = 0x0100,   /* M/~IO=0 D/~C=0 E/~NE=0 */
	S2650_DATA_PORT     = 0x0101,   /* M/~IO=0 D/~C=1 E/~NE=0 */
	S2650_SENSE_PORT    = 0x0102    /* Fake Sense Line */
};


extern const device_type S2650;


#define MCFG_S2650_FLAG_HANDLER(_devcb) \
	devcb = &s2650_device::set_flag_handler(*device, DEVCB_##_devcb);

#define MCFG_S2650_INTACK_HANDLER(_devcb) \
	devcb = &s2650_device::set_intack_handler(*device, DEVCB_##_devcb);

class s2650_device : public cpu_device
{
public:
	// construction/destruction
	s2650_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// static configuration helpers
	template<class _Object> static devcb_base &set_flag_handler(device_t &device, _Object object) { return downcast<s2650_device &>(device).m_flag_handler.set_callback(object); }
	template<class _Object> static devcb_base &set_intack_handler(device_t &device, _Object object) { return downcast<s2650_device &>(device).m_intack_handler.set_callback(object); }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual uint32_t execute_min_cycles() const override { return 5; }
	virtual uint32_t execute_max_cycles() const override { return 13; }
	virtual uint32_t execute_input_lines() const override { return 2; }
	virtual uint32_t execute_default_irq_vector() const override { return 0; }
	virtual void execute_run() override;
	virtual void execute_set_input(int inputnum, int state) override;

	// device_memory_interface overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const override
	{
		return (spacenum == AS_PROGRAM) ? &m_program_config : ( (spacenum == AS_IO) ? &m_io_config : nullptr );
	}

	// device_state_interface overrides
	virtual void state_import(const device_state_entry &entry) override;
	virtual void state_export(const device_state_entry &entry) override;
	virtual void state_string_export(const device_state_entry &entry, std::string &str) const override;

	// device_disasm_interface overrides
	virtual uint32_t disasm_min_opcode_bytes() const override { return 1; }
	virtual uint32_t disasm_max_opcode_bytes() const override { return 3; }
	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;

private:
	address_space_config m_program_config;
	address_space_config m_io_config;

	devcb_write_line m_flag_handler;
	devcb_write_line m_intack_handler;

	uint16_t  m_ppc;    /* previous program counter (page + iar) */
	uint16_t  m_page;   /* 8K page select register (A14..A13) */
	uint16_t  m_iar;    /* instruction address register (A12..A0) */
	uint16_t  m_ea;     /* effective address (A14..A0) */
	uint8_t   m_psl;    /* processor status lower */
	uint8_t   m_psu;    /* processor status upper */
	uint8_t   m_r;      /* absolute addressing dst/src register */
	uint8_t   m_reg[7]; /* 7 general purpose registers */
	uint8_t   m_halt;   /* 1 if cpu is halted */
	uint8_t   m_ir;     /* instruction register */
	uint16_t  m_ras[8]; /* 8 return address stack entries */
	uint8_t   m_irq_state;

	int     m_icount;
	address_space *m_program;
	direct_read_data *m_direct;
	address_space *m_io;

	// For debugger
	uint16_t  m_debugger_temp;

	inline void set_psu(uint8_t new_val);
	inline uint8_t get_sp();
	inline void set_sp(uint8_t new_sp);
	inline int check_irq_line();
	inline uint8_t ROP();
	inline uint8_t ARG();
	void s2650_set_flag(int state);
	int s2650_get_flag();
	void s2650_set_sense(int state);
};


#endif /* __S2650_H__ */
