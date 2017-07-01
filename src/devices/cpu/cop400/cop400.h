// license:BSD-3-Clause
// copyright-holders:Curt Coder
/***************************************************************************

    cop400.h

    National Semiconductor COPS Emulator.

***************************************************************************/

#pragma once

#ifndef __COP400__
#define __COP400__

// i/o pins

// L pins: 8-bit bi-directional
#define MCFG_COP400_READ_L_CB(_devcb) \
	cop400_cpu_device::set_read_l_callback(*device, DEVCB_##_devcb);
#define MCFG_COP400_WRITE_L_CB(_devcb) \
	cop400_cpu_device::set_write_l_callback(*device, DEVCB_##_devcb);

// G pins: 4-bit bi-directional
#define MCFG_COP400_READ_G_CB(_devcb) \
	cop400_cpu_device::set_read_g_callback(*device, DEVCB_##_devcb);
#define MCFG_COP400_WRITE_G_CB(_devcb) \
	cop400_cpu_device::set_write_g_callback(*device, DEVCB_##_devcb);

// D outputs: 4-bit general purpose output
#define MCFG_COP400_WRITE_D_CB(_devcb) \
	cop400_cpu_device::set_write_d_callback(*device, DEVCB_##_devcb);

// IN inputs: 4-bit general purpose input
#define MCFG_COP400_READ_IN_CB(_devcb) \
	cop400_cpu_device::set_read_in_callback(*device, DEVCB_##_devcb);

// SI/SO lines: serial in/out or counter/gen.purpose
#define MCFG_COP400_READ_SI_CB(_devcb) \
	cop400_cpu_device::set_read_si_callback(*device, DEVCB_##_devcb);
#define MCFG_COP400_WRITE_SO_CB(_devcb) \
	cop400_cpu_device::set_write_so_callback(*device, DEVCB_##_devcb);

// SK output line: logic-controlled clock or gen.purpose
#define MCFG_COP400_WRITE_SK_CB(_devcb) \
	cop400_cpu_device::set_write_sk_callback(*device, DEVCB_##_devcb);

// CKI/CKO lines: only CKO input here
#define MCFG_COP400_READ_CKO_CB(_devcb) \
	cop400_cpu_device::set_read_cko_callback(*device, DEVCB_##_devcb);


/***************************************************************************
    CONSTANTS
***************************************************************************/

/* register access indexes */
enum
{
	COP400_PC,
	COP400_SA,
	COP400_SB,
	COP400_SC,
	COP400_N,
	COP400_A,
	COP400_B,
	COP400_C,
	COP400_G,
	COP400_H,
	COP400_Q,
	COP400_R,
	COP400_EN,
	COP400_SIO,
	COP400_SKL,
	COP400_T
};

/* input lines */
enum
{
	/* COP420 */
	COP400_IN0 = 0,
	COP400_IN1,
	COP400_IN2,
	COP400_IN3,

	/* COP404 */
	COP400_MB,
	COP400_DUAL,
	COP400_SEL10,
	COP400_SEL20
};

/* CKI bonding options */
enum cop400_cki_bond {
	COP400_CKI_DIVISOR_4 = 4,
	COP400_CKI_DIVISOR_8 = 8,
	COP400_CKI_DIVISOR_16 = 16,
	COP400_CKI_DIVISOR_32 = 32
};

/* CKO bonding options */
enum cop400_cko_bond {
	COP400_CKO_OSCILLATOR_OUTPUT = 0,
	COP400_CKO_RAM_POWER_SUPPLY,
	COP400_CKO_HALT_IO_PORT,
	COP400_CKO_SYNC_INPUT,
	COP400_CKO_GENERAL_PURPOSE_INPUT
};


#define MCFG_COP400_CONFIG(_cki, _cko, _microbus) \
	cop400_cpu_device::set_cki(*device, _cki); \
	cop400_cpu_device::set_cko(*device, _cko); \
	cop400_cpu_device::set_microbus(*device, _microbus);


class cop400_cpu_device : public cpu_device
{
public:
	// construction/destruction
	cop400_cpu_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source, uint8_t program_addr_bits, uint8_t data_addr_bits, uint8_t featuremask, uint8_t g_mask, uint8_t d_mask, uint8_t in_mask, bool has_counter, bool has_inil, address_map_constructor internal_map_program, address_map_constructor internal_map_data);

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// static configuration helpers
	template<class _Object> static devcb_base &set_read_l_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_read_l.set_callback(object); }
	template<class _Object> static devcb_base &set_write_l_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_write_l.set_callback(object); }
	template<class _Object> static devcb_base &set_read_g_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_read_g.set_callback(object); }
	template<class _Object> static devcb_base &set_write_g_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_write_g.set_callback(object); }
	template<class _Object> static devcb_base &set_write_d_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_write_d.set_callback(object); }
	template<class _Object> static devcb_base &set_read_in_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_read_in.set_callback(object); }
	template<class _Object> static devcb_base &set_read_si_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_read_si.set_callback(object); }
	template<class _Object> static devcb_base &set_write_so_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_write_so.set_callback(object); }
	template<class _Object> static devcb_base &set_write_sk_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_write_sk.set_callback(object); }
	template<class _Object> static devcb_base &set_read_cko_callback(device_t &device, _Object object) { return downcast<cop400_cpu_device &>(device).m_read_cko.set_callback(object); }

	static void set_cki(device_t &device, cop400_cki_bond cki) { downcast<cop400_cpu_device &>(device).m_cki = cki; }
	static void set_cko(device_t &device, cop400_cko_bond cko) { downcast<cop400_cpu_device &>(device).m_cko = cko; }
	static void set_microbus(device_t &device, bool has_microbus) { downcast<cop400_cpu_device &>(device).m_has_microbus = has_microbus; }

	DECLARE_READ8_MEMBER( microbus_rd );
	DECLARE_WRITE8_MEMBER( microbus_wr );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual uint64_t execute_clocks_to_cycles(uint64_t clocks) const override { return (clocks + m_cki - 1) / m_cki; }
	virtual uint64_t execute_cycles_to_clocks(uint64_t cycles) const override { return (cycles * m_cki); }
	virtual uint32_t execute_min_cycles() const override { return 1; }
	virtual uint32_t execute_max_cycles() const override { return 2; }
	virtual uint32_t execute_input_lines() const override { return 0; }
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
	virtual uint32_t disasm_min_opcode_bytes() const override { return 1; }
	virtual uint32_t disasm_max_opcode_bytes() const override { return 2; }
	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;

	address_space_config m_program_config;
	address_space_config m_data_config;

	// i/o handlers
	devcb_read8 m_read_l;
	devcb_write8 m_write_l;
	devcb_read8 m_read_g;
	devcb_write8 m_write_g;
	devcb_write8 m_write_d;
	devcb_read8 m_read_in;
	devcb_read_line m_read_si;
	devcb_write_line m_write_so;
	devcb_write_line m_write_sk;
	devcb_read_line m_read_cko;

	cop400_cki_bond m_cki;
	cop400_cko_bond m_cko;
	bool m_has_microbus;

	bool m_has_counter;
	bool m_has_inil;

	address_space *m_program;
	direct_read_data *m_direct;
	address_space *m_data;

	uint8_t m_featuremask;

	/* registers */
	uint16_t  m_pc;             /* 9/10/11-bit ROM address program counter */
	uint16_t  m_prevpc;         /* previous value of program counter */
	uint8_t   m_a;              /* 4-bit accumulator */
	uint8_t   m_b;              /* 5/6/7-bit RAM address register */
	int     m_c;              /* 1-bit carry register */
	uint8_t   m_n;              /* 2-bit stack pointer (COP440 only) */
	uint8_t   m_en;             /* 4-bit enable register */
	uint8_t   m_g;              /* 4-bit general purpose I/O port */
	uint8_t   m_q;              /* 8-bit latch for L port */
	uint16_t  m_sa, m_sb, m_sc; /* subroutine save registers (not present in COP440) */
	uint8_t   m_sio;            /* 4-bit shift register and counter */
	int     m_skl;            /* 1-bit latch for SK output */
	uint8_t   m_h;              /* 4-bit general purpose I/O port (COP440 only) */
	uint8_t   m_r;              /* 8-bit general purpose I/O port (COP440 only) */
	uint8_t   m_flags;          // used for I/O only

	/* counter */
	uint8_t   m_t;              /* 8-bit timer */
	int     m_skt_latch;      /* timer overflow latch */

	/* input/output ports */
	uint8_t   m_g_mask;         /* G port mask */
	uint8_t   m_d_mask;         /* D port mask */
	uint8_t   m_in_mask;        /* IN port mask */
	uint8_t   m_il;             /* IN latch */
	uint8_t   m_in[4];          /* IN port shift register */
	uint8_t   m_si;             /* serial input */

	/* skipping logic */
	int m_skip;               /* skip next instruction */
	int m_skip_lbi;           /* skip until next non-LBI instruction */
	int m_last_skip;          /* last value of skip */
	int m_halt;               /* halt mode */
	int m_idle;               /* idle mode */

	/* execution logic */
	int m_InstLen[256];       /* instruction length in bytes */
	int m_icount;             /* instruction counter */

	/* timers */
	emu_timer *m_serial_timer;
	emu_timer *m_counter_timer;
	emu_timer *m_inil_timer;

	typedef void ( cop400_cpu_device::*cop400_opcode_func ) (uint8_t opcode);

	/* The opcode table now is a combination of cycle counts and function pointers */
	struct cop400_opcode_map {
		uint32_t cycles;
		cop400_opcode_func function;
	};

	const cop400_opcode_map *m_opcode_map;

	static const cop400_opcode_map COP410_OPCODE_23_MAP[256];
	static const cop400_opcode_map COP410_OPCODE_33_MAP[256];
	static const cop400_opcode_map COP410_OPCODE_MAP[256];
	static const cop400_opcode_map COP420_OPCODE_23_MAP[256];
	static const cop400_opcode_map COP420_OPCODE_33_MAP[256];
	static const cop400_opcode_map COP420_OPCODE_MAP[256];
	static const cop400_opcode_map COP444_OPCODE_23_MAP[256];
	static const cop400_opcode_map COP444_OPCODE_33_MAP[256];
	static const cop400_opcode_map COP444_OPCODE_MAP[256];

	void serial_tick();
	void counter_tick();
	void inil_tick();

	void PUSH(uint16_t data);
	void POP();
	void WRITE_Q(uint8_t data);
	void WRITE_G(uint8_t data);

	void illegal(uint8_t opcode);
	void asc(uint8_t opcode);
	void add(uint8_t opcode);
	void aisc(uint8_t opcode);
	void clra(uint8_t opcode);
	void comp(uint8_t opcode);
	void nop(uint8_t opcode);
	void rc(uint8_t opcode);
	void sc(uint8_t opcode);
	void xor_(uint8_t opcode);
	void adt(uint8_t opcode);
	void casc(uint8_t opcode);
	void jid(uint8_t opcode);
	void jmp(uint8_t opcode);
	void jp(uint8_t opcode);
	void jsr(uint8_t opcode);
	void ret(uint8_t opcode);
	void cop420_ret(uint8_t opcode);
	void retsk(uint8_t opcode);
	void halt(uint8_t opcode);
	void it(uint8_t opcode);
	void camq(uint8_t opcode);
	void ld(uint8_t opcode);
	void lqid(uint8_t opcode);
	void rmb0(uint8_t opcode);
	void rmb1(uint8_t opcode);
	void rmb2(uint8_t opcode);
	void rmb3(uint8_t opcode);
	void smb0(uint8_t opcode);
	void smb1(uint8_t opcode);
	void smb2(uint8_t opcode);
	void smb3(uint8_t opcode);
	void stii(uint8_t opcode);
	void x(uint8_t opcode);
	void xad(uint8_t opcode);
	void xds(uint8_t opcode);
	void xis(uint8_t opcode);
	void cqma(uint8_t opcode);
	void ldd(uint8_t opcode);
	void camt(uint8_t opcode);
	void ctma(uint8_t opcode);
	void cab(uint8_t opcode);
	void cba(uint8_t opcode);
	void lbi(uint8_t opcode);
	void lei(uint8_t opcode);
	void xabr(uint8_t opcode);
	void cop444_xabr(uint8_t opcode);
	void skc(uint8_t opcode);
	void ske(uint8_t opcode);
	void skgz(uint8_t opcode);
	void skgbz0(uint8_t opcode);
	void skgbz1(uint8_t opcode);
	void skgbz2(uint8_t opcode);
	void skgbz3(uint8_t opcode);
	void skmbz0(uint8_t opcode);
	void skmbz1(uint8_t opcode);
	void skmbz2(uint8_t opcode);
	void skmbz3(uint8_t opcode);
	void skt(uint8_t opcode);
	void ing(uint8_t opcode);
	void inl(uint8_t opcode);
	void obd(uint8_t opcode);
	void omg(uint8_t opcode);
	void xas(uint8_t opcode);
	void inin(uint8_t opcode);
	void cop402m_inin(uint8_t opcode);
	void inil(uint8_t opcode);
	void ogi(uint8_t opcode);
	void cop410_op23(uint8_t opcode);
	void cop410_op33(uint8_t opcode);
	void cop420_op23(uint8_t opcode);
	void cop420_op33(uint8_t opcode);
	void cop444_op23(uint8_t opcode);
	void cop444_op33(uint8_t opcode);
	void skgbz(int bit);
	void skmbz(int bit);

};


/* COP410 family */
// COP401 is a ROMless version of the COP410
class cop401_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop401_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class cop410_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop410_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


// COP411 is a 20-pin package version of the COP410, missing D2/D3/G3/CKO
class cop411_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop411_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


/* COP420 family */
// COP402 is a ROMless version of the COP420
class cop402_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop402_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class cop420_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop420_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


// COP421 is a 24-pin package version of the COP420, lacking the IN ports
class cop421_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop421_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


// COP422 is a 20-pin package version of the COP420, lacking G0/G1, D0/D1, and the IN ports
class cop422_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop422_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


/* COP444 family */
// COP404 is a ROMless version of the COP444, which can emulate a COP410C/COP411C, COP424C/COP425C, or a COP444C/COP445C
class cop404_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop404_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


// COP424 is functionally equivalent to COP444, with only 1K ROM and 64x4 bytes RAM
class cop424_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop424_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


// COP425 is a 24-pin package version of the COP424, lacking the IN ports
class cop425_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop425_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


// COP426 is a 20-pin package version of the COP424, with only L0-L7, G2-G3, D2-D3 ports
class cop426_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop426_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class cop444_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop444_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


// COP445 is a 24-pin package version of the COP444, lacking the IN ports
class cop445_cpu_device : public cop400_cpu_device
{
public:
	// construction/destruction
	cop445_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


extern const device_type COP401;
extern const device_type COP410;
extern const device_type COP411;
extern const device_type COP402;
extern const device_type COP420;
extern const device_type COP421;
extern const device_type COP422;
extern const device_type COP404;
extern const device_type COP424;
extern const device_type COP425;
extern const device_type COP426;
extern const device_type COP444;
extern const device_type COP445;


#endif  /* __COP400__ */
