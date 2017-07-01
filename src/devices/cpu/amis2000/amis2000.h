// license:BSD-3-Clause
// copyright-holders:hap
/*

  AMI S2000-family MCU cores

*/

#ifndef _AMIS2000_H_
#define _AMIS2000_H_

#include "emu.h"


// generic input pins (4 bits each)
#define MCFG_AMI_S2000_READ_K_CB(_devcb) \
	amis2000_base_device::set_read_k_callback(*device, DEVCB_##_devcb);

#define MCFG_AMI_S2000_READ_I_CB(_devcb) \
	amis2000_base_device::set_read_i_callback(*device, DEVCB_##_devcb);

// 8-bit external databus coupled as input/output pins
#define MCFG_AMI_S2000_READ_D_CB(_devcb) \
	amis2000_base_device::set_read_d_callback(*device, DEVCB_##_devcb);

#define MCFG_AMI_S2000_WRITE_D_CB(_devcb) \
	amis2000_base_device::set_write_d_callback(*device, DEVCB_##_devcb);

// 13-bit external address bus coupled as output pins
#define MCFG_AMI_S2000_WRITE_A_CB(_devcb) \
	amis2000_base_device::set_write_a_callback(*device, DEVCB_##_devcb);

// F_out pin (only for S2152)
#define MCFG_AMI_S2152_FOUT_CB(_devcb) \
	amis2000_base_device::set_write_f_callback(*device, DEVCB_##_devcb);

// S2000 has a hardcoded 7seg table, that (unlike S2200) is officially
// uncustomizable, but wildfire proves to be an exception to that rule.
#define MCFG_AMI_S2000_7SEG_DECODER(_ptr) \
	amis2000_base_device::set_7seg_table(*device, _ptr);


class amis2000_base_device : public cpu_device
{
public:
	// construction/destruction
	amis2000_base_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, uint8_t bu_bits, uint8_t callstack_bits, uint8_t callstack_depth, int prgwidth, address_map_constructor program, int datawidth, address_map_constructor data, const char *shortname, const char *source)
		: cpu_device(mconfig, type, name, tag, owner, clock, shortname, source)
		, m_program_config("program", ENDIANNESS_BIG, 8, prgwidth, 0, program)
		, m_data_config("data", ENDIANNESS_BIG, 8, datawidth, 0, data)
		, m_bu_bits(bu_bits)
		, m_callstack_bits(callstack_bits)
		, m_callstack_depth(callstack_depth)
		, m_7seg_table(nullptr)
		, m_read_k(*this)
		, m_read_i(*this)
		, m_read_d(*this)
		, m_write_d(*this)
		, m_write_a(*this)
		, m_write_f(*this)
	{ }

	// static configuration helpers
	template<class _Object> static devcb_base &set_read_k_callback(device_t &device, _Object object) { return downcast<amis2000_base_device &>(device).m_read_k.set_callback(object); }
	template<class _Object> static devcb_base &set_read_i_callback(device_t &device, _Object object) { return downcast<amis2000_base_device &>(device).m_read_i.set_callback(object); }
	template<class _Object> static devcb_base &set_read_d_callback(device_t &device, _Object object) { return downcast<amis2000_base_device &>(device).m_read_d.set_callback(object); }
	template<class _Object> static devcb_base &set_write_d_callback(device_t &device, _Object object) { return downcast<amis2000_base_device &>(device).m_write_d.set_callback(object); }
	template<class _Object> static devcb_base &set_write_a_callback(device_t &device, _Object object) { return downcast<amis2000_base_device &>(device).m_write_a.set_callback(object); }
	template<class _Object> static devcb_base &set_write_f_callback(device_t &device, _Object object) { return downcast<amis2000_base_device &>(device).m_write_f.set_callback(object); }
	static void set_7seg_table(device_t &device, const uint8_t *ptr) { downcast<amis2000_base_device &>(device).m_7seg_table = ptr; }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual uint64_t execute_clocks_to_cycles(uint64_t clocks) const override { return (clocks + 4 - 1) / 4; } // 4 cycles per machine cycle
	virtual uint64_t execute_cycles_to_clocks(uint64_t cycles) const override { return (cycles * 4); } // "
	virtual uint32_t execute_min_cycles() const override { return 1; }
	virtual uint32_t execute_max_cycles() const override { return 2; }
	virtual uint32_t execute_input_lines() const override { return 1; }
	virtual void execute_run() override;

	// device_memory_interface overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const override { return(spacenum == AS_PROGRAM) ? &m_program_config : ((spacenum == AS_DATA) ? &m_data_config : nullptr); }

	// device_disasm_interface overrides
	virtual uint32_t disasm_min_opcode_bytes() const override { return 1; }
	virtual uint32_t disasm_max_opcode_bytes() const override { return 1; }
	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;

	// device_state_interface overrides
	virtual void state_string_export(const device_state_entry &entry, std::string &str) const override;

	address_space_config m_program_config;
	address_space_config m_data_config;
	address_space *m_program;
	address_space *m_data;

	uint8_t m_bu_bits;
	uint16_t m_bu_mask;
	uint8_t m_callstack_bits;     // number of program counter bits held in callstack
	uint16_t m_callstack_mask;
	uint8_t m_callstack_depth;    // callstack levels: 3 on 2000/2150, 5 on 2200/2400
	uint16_t m_callstack[5];      // max 5
	int m_icount;
	uint16_t m_pc;                // 13-bit program counter
	uint8_t m_ppr;                // prepared page register (PP 1)
	uint8_t m_pbr;                // prepared bank register (PP 2)
	bool m_skip;                // skip next opcode, including PP prefixes
	uint8_t m_op;
	uint8_t m_prev_op;            // previous opcode, needed for PP, LAI, LB*
	uint8_t m_f;                  // generic flags: 2 on 2000/2150, 6 on 2200/2400
	uint8_t m_carry;              // carry flag
	uint8_t m_bl;                 // 4-bit ram index x
	uint8_t m_bu;                 // 2/3-bit ram index y
	uint8_t m_acc;                // 4-bit accumulator
	uint8_t m_e;                  // 4-bit generic register
	uint8_t m_ki_mask;            // 4-bit k/i-pins select latch
	uint8_t m_d;                  // 8-bit d-pins latch
	bool m_d_active;            // d-pins available for direct i/o(floating), or outputting d-latch
	uint8_t m_d_polarity;         // invert d-latch output
	uint16_t m_a;                 // 13-bit a-pins latch (master strobe latch)

	// i/o handlers
	const uint8_t *m_7seg_table;
	devcb_read8 m_read_k;
	devcb_read8 m_read_i;
	devcb_read8 m_read_d;
	devcb_write8 m_write_d;
	devcb_write16 m_write_a;
	devcb_write_line m_write_f;

	// misc internal helpers
	uint8_t ram_r();
	void ram_w(uint8_t data);
	void pop_callstack();
	void push_callstack();
	void d_latch_out(bool active);

	// opcode handlers
	virtual void op_lai();
	virtual void op_lab();
	virtual void op_lae();
	virtual void op_xab();
	virtual void op_xabu();
	virtual void op_xae();
	virtual void op_lbe();
	virtual void op_lbep();
	virtual void op_lbz();
	virtual void op_lbf();

	virtual void op_lam();
	virtual void op_xc();
	virtual void op_xci();
	virtual void op_xcd();
	virtual void op_stm();
	virtual void op_rsm();

	virtual void op_inp();
	virtual void op_out();
	virtual void op_disb();
	virtual void op_disn();
	virtual void op_mvs();
	virtual void op_psh();
	virtual void op_psl();
	virtual void op_eur();

	virtual void op_pp();
	virtual void op_jmp();
	virtual void op_jms();
	virtual void op_rt();
	virtual void op_rts();
	virtual void op_nop();
	virtual void op_halt();

	virtual void op_szc();
	virtual void op_szm();
	virtual void op_szi();
	virtual void op_szk();
	virtual void op_sbe();
	virtual void op_sam();
	virtual void op_sos();
	virtual void op_tf1();
	virtual void op_tf2();

	virtual void op_adcs();
	virtual void op_adis();
	virtual void op_add();
	virtual void op_and();
	virtual void op_xor();
	virtual void op_stc();
	virtual void op_rsc();
	virtual void op_cma();
	virtual void op_sf1();
	virtual void op_rf1();
	virtual void op_sf2();
	virtual void op_rf2();
};


class amis2000_cpu_device : public amis2000_base_device
{
public:
	amis2000_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class amis2150_cpu_device : public amis2000_base_device
{
public:
	amis2150_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class amis2152_cpu_device : public amis2000_base_device
{
public:
	amis2152_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// digital-to-frequency converter
	uint8_t m_d2f_latch;
	emu_timer *m_d2f_timer;
	int m_fout_state;

	void d2f_timer_clock();
	TIMER_CALLBACK_MEMBER(d2f_timer_cb);

	// opcode handlers
	virtual void op_szk() override;
};



extern const device_type AMI_S2000;
extern const device_type AMI_S2150;
extern const device_type AMI_S2152;


#endif /* _AMIS2000_H_ */
