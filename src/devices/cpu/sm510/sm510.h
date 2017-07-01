// license:BSD-3-Clause
// copyright-holders:hap
/*

  Sharp SM510 MCU family cores

*/

#ifndef _SM510_H_
#define _SM510_H_

#include "emu.h"


// I/O ports setup

// 4-bit K input port (pull-down)
#define MCFG_SM510_READ_K_CB(_devcb) \
	sm510_base_device::set_read_k_callback(*device, DEVCB_##_devcb);
// when in halt state, any K input going High can wake up the CPU,
// driver is required to use execute_set_input(SM510_INPUT_LINE_K, state)
#define SM510_INPUT_LINE_K 0

// 1-bit BA(aka alpha) input pin (pull-up)
#define MCFG_SM510_READ_BA_CB(_devcb) \
	sm510_base_device::set_read_ba_callback(*device, DEVCB_##_devcb);

// 1-bit B(beta) input pin (pull-up)
#define MCFG_SM510_READ_B_CB(_devcb) \
	sm510_base_device::set_read_b_callback(*device, DEVCB_##_devcb);

// 8-bit S strobe output port
#define MCFG_SM510_WRITE_S_CB(_devcb) \
	sm510_base_device::set_write_s_callback(*device, DEVCB_##_devcb);

// 2-bit R melody output port
#define MCFG_SM510_WRITE_R_CB(_devcb) \
	sm510_base_device::set_write_r_callback(*device, DEVCB_##_devcb);

// LCD segment outputs: H1-4 as offset(low), a/b/c 1-16 as data d0-d15
#define MCFG_SM510_WRITE_SEGA_CB(_devcb) \
	sm510_base_device::set_write_sega_callback(*device, DEVCB_##_devcb);
#define MCFG_SM510_WRITE_SEGB_CB(_devcb) \
	sm510_base_device::set_write_segb_callback(*device, DEVCB_##_devcb);
#define MCFG_SM510_WRITE_SEGC_CB(_devcb) \
	sm510_base_device::set_write_segc_callback(*device, DEVCB_##_devcb);

// LCD bs output: same as above, but only up to 2 bits used
#define MCFG_SM510_WRITE_SEGBS_CB(_devcb) \
	sm510_base_device::set_write_segbs_callback(*device, DEVCB_##_devcb);

enum
{
	SM510_PORT_SEGA = 0x00,
	SM510_PORT_SEGB = 0x04,
	SM510_PORT_SEGBS = 0x08,
	SM510_PORT_SEGC = 0x0c
};


// pinout reference

/*

        b2 a3 b3 a4 b4 a5 b5 GNDa6 b6 a7 b7 a8 b8 a9
        45 44 43 42 41 40 39 38 37 36 35 34 33 32 31
       ______________________________________________
      |                                              |
a2 46 |                                              | 30 b9
b1 47 |                                              | 29 a10
a1 48 |                                              | 28 b10
H4 49 |                                              | 27 a11
H3 50 |                                              | 26 b11
H2 51 |                                              | 25 a12
H1 52 |                    SM510                     | 24 b12
S1 53 |                    SM511                     | 23 a13
S2 54 |                                              | 22 b13
S3 55 |                                              | 21 a14
S4 56 |                                              | 20 b14
S5 57 |                                              | 19 a15
S6 58 |                                              | 18 b15
S7 59 |                                              | 17 a16
S8 60 | *                                            | 16 b16
      |______________________________________________/

         1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
         T K1 K2 K3 K4 ACL | GND |OSCin| bt R1 R2 bs   note: bt = beta symbol
                           BA  OSCout Vdd
*/

class sm510_base_device : public cpu_device
{
public:
	// construction/destruction
	sm510_base_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, int stack_levels, int prgwidth, address_map_constructor program, int datawidth, address_map_constructor data, const char *shortname, const char *source)
		: cpu_device(mconfig, type, name, tag, owner, clock, shortname, source)
		, m_program_config("program", ENDIANNESS_LITTLE, 8, prgwidth, 0, program)
		, m_data_config("data", ENDIANNESS_LITTLE, 8, datawidth, 0, data)
		, m_prgwidth(prgwidth)
		, m_datawidth(datawidth)
		, m_stack_levels(stack_levels)
		, m_lcd_ram_a(*this, "lcd_ram_a"), m_lcd_ram_b(*this, "lcd_ram_b"), m_lcd_ram_c(*this, "lcd_ram_c")
		, m_write_sega(*this), m_write_segb(*this), m_write_segc(*this), m_write_segbs(*this)
		, m_melody_rom(*this, "music")
		, m_read_k(*this)
		, m_read_ba(*this), m_read_b(*this)
		, m_write_s(*this)
		, m_write_r(*this)
	{ }

	// static configuration helpers
	template<class _Object> static devcb_base &set_read_k_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_read_k.set_callback(object); }
	template<class _Object> static devcb_base &set_read_ba_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_read_ba.set_callback(object); }
	template<class _Object> static devcb_base &set_read_b_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_read_b.set_callback(object); }
	template<class _Object> static devcb_base &set_write_s_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_write_s.set_callback(object); }
	template<class _Object> static devcb_base &set_write_r_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_write_r.set_callback(object); }

	template<class _Object> static devcb_base &set_write_sega_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_write_sega.set_callback(object); }
	template<class _Object> static devcb_base &set_write_segb_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_write_segb.set_callback(object); }
	template<class _Object> static devcb_base &set_write_segc_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_write_segc.set_callback(object); }
	template<class _Object> static devcb_base &set_write_segbs_callback(device_t &device, _Object object) { return downcast<sm510_base_device &>(device).m_write_segbs.set_callback(object); }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual uint64_t execute_clocks_to_cycles(uint64_t clocks) const override { return (clocks + 2 - 1) / 2; } // default 2 cycles per machine cycle
	virtual uint64_t execute_cycles_to_clocks(uint64_t cycles) const override { return (cycles * 2); } // "
	virtual uint32_t execute_min_cycles() const override { return 1; }
	virtual uint32_t execute_max_cycles() const override { return 2; }
	virtual uint32_t execute_input_lines() const override { return 1; }
	virtual void execute_set_input(int line, int state) override;
	virtual void execute_run() override;
	virtual void execute_one() { } // -> child class

	// device_memory_interface overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const override { return(spacenum == AS_PROGRAM) ? &m_program_config : ((spacenum == AS_DATA) ? &m_data_config : nullptr); }

	// device_disasm_interface overrides
	virtual uint32_t disasm_min_opcode_bytes() const override { return 1; }
	virtual uint32_t disasm_max_opcode_bytes() const override { return 0x40; } // actually 2, but debugger doesn't like non-linear pc

	address_space_config m_program_config;
	address_space_config m_data_config;
	address_space *m_program;
	address_space *m_data;

	int m_prgwidth;
	int m_datawidth;
	int m_prgmask;
	int m_datamask;

	uint16_t m_pc, m_prev_pc;
	uint16_t m_op, m_prev_op;
	uint8_t m_param;
	int m_stack_levels;
	uint16_t m_stack[2];
	int m_icount;

	uint8_t m_acc;
	uint8_t m_bl;
	uint8_t m_bm;
	bool m_sbm;
	uint8_t m_c;
	bool m_skip;
	uint8_t m_w;
	uint8_t m_r;
	bool m_k_active;
	bool m_halt;

	// lcd driver
	optional_shared_ptr<uint8_t> m_lcd_ram_a, m_lcd_ram_b, m_lcd_ram_c;
	devcb_write16 m_write_sega, m_write_segb, m_write_segc, m_write_segbs;
	emu_timer *m_lcd_timer;
	uint8_t m_l, m_x;
	uint8_t m_y;
	bool m_bp;
	bool m_bc;

	uint16_t get_lcd_row(int column, uint8_t* ram);
	TIMER_CALLBACK_MEMBER(lcd_timer_cb);
	virtual void init_lcd_driver();

	// melody controller
	optional_region_ptr<uint8_t> m_melody_rom;
	uint8_t m_melody_rd;
	uint8_t m_melody_step_count;
	uint8_t m_melody_duty_count;
	uint8_t m_melody_duty_index;
	uint8_t m_melody_address;

	void clock_melody();
	void init_melody();

	// interrupt/divider
	emu_timer *m_div_timer;
	uint16_t m_div;
	bool m_1s;

	bool wake_me_up();
	void init_divider();
	TIMER_CALLBACK_MEMBER(div_timer_cb);

	// other i/o handlers
	devcb_read8 m_read_k;
	devcb_read_line m_read_ba;
	devcb_read_line m_read_b;
	devcb_write8 m_write_s;
	devcb_write8 m_write_r;

	// misc internal helpers
	void increment_pc();
	virtual void get_opcode_param() { }
	virtual void update_w_latch() { }

	uint8_t ram_r();
	void ram_w(uint8_t data);
	void pop_stack();
	void push_stack();
	void do_branch(uint8_t pu, uint8_t pm, uint8_t pl);
	uint8_t bitmask(uint16_t param);

	// opcode handlers
	virtual void op_lb();
	virtual void op_lbl();
	virtual void op_sbm();
	virtual void op_exbla();
	virtual void op_incb();
	virtual void op_decb();

	virtual void op_atpl();
	virtual void op_rtn0();
	virtual void op_rtn1();
	virtual void op_tl();
	virtual void op_tml();
	virtual void op_tm();
	virtual void op_t();

	virtual void op_exc();
	virtual void op_bdc();
	virtual void op_exci();
	virtual void op_excd();
	virtual void op_lda();
	virtual void op_lax();
	virtual void op_ptw();
	virtual void op_wr();
	virtual void op_ws();

	virtual void op_kta();
	virtual void op_atbp();
	virtual void op_atx();
	virtual void op_atl();
	virtual void op_atfc();
	virtual void op_atr();

	virtual void op_add();
	virtual void op_add11();
	virtual void op_adx();
	virtual void op_coma();
	virtual void op_rot();
	virtual void op_rc();
	virtual void op_sc();

	virtual void op_tb();
	virtual void op_tc();
	virtual void op_tam();
	virtual void op_tmi();
	virtual void op_ta0();
	virtual void op_tabl();
	virtual void op_tis();
	virtual void op_tal();
	virtual void op_tf1();
	virtual void op_tf4();

	virtual void op_rm();
	virtual void op_sm();

	virtual void op_pre();
	virtual void op_sme();
	virtual void op_rme();
	virtual void op_tmel();

	virtual void op_skip();
	virtual void op_cend();
	virtual void op_idiv();
	virtual void op_dr();
	virtual void op_dta();

	void op_illegal();
};


class sm510_device : public sm510_base_device
{
public:
	sm510_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;
	virtual void execute_one() override;
	virtual void get_opcode_param() override;

	virtual void update_w_latch() override { m_write_s(0, m_w, 0xff); } // W is connected directly to S
};


class sm511_device : public sm510_base_device
{
public:
	sm511_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	sm511_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, int stack_levels, int prgwidth, address_map_constructor program, int datawidth, address_map_constructor data, const char *shortname, const char *source);

protected:
	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;
	virtual void execute_one() override;
	virtual void get_opcode_param() override;
};

class sm512_device : public sm511_device
{
public:
	sm512_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};



extern const device_type SM510;
extern const device_type SM511;
extern const device_type SM512;


#endif /* _SM510_H_ */
