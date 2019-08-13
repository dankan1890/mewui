// license:BSD-3-Clause
// copyright-holders:R. Belmont
/***************************************************************************

    Fujitsu Micro F2MC-16 series

***************************************************************************/

#ifndef MAME_CPU_F2MC16_F2MC16_H
#define MAME_CPU_F2MC16_F2MC16_H 1

#pragma once

class f2mc16_device : public cpu_device
{
public:
	friend class mb9061x_device;

	enum
	{
		F2MC16_PC, F2MC16_PS, F2MC16_USP, F2MC16_SSP, F2MC16_ACC,
		F2MC16_PCB, F2MC16_DTB, F2MC16_USB, F2MC16_SSB, F2MC16_ADB, F2MC16_DPR,
		F2MC16_RW0, F2MC16_RW1, F2MC16_RW2, F2MC16_RW3,
		F2MC16_RW4, F2MC16_RW5, F2MC16_RW6, F2MC16_RW7,
		F2MC16_RL0, F2MC16_RL1, F2MC16_RL2, F2MC16_RL3,
		F2MC16_R0, F2MC16_R1, F2MC16_R2, F2MC16_R3, F2MC16_R4, F2MC16_R5, F2MC16_R6, F2MC16_R7
	};

	enum
	{
		F_I = 0x40,
		F_S = 0x20,
		F_T = 0x10,
		F_N = 0x08,
		F_Z = 0x04,
		F_V = 0x02,
		F_C = 0x01
	};

	// construction/destruction
	f2mc16_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	f2mc16_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void state_import(const device_state_entry &entry) override;
	virtual void state_export(const device_state_entry &entry) override;
	virtual void state_string_export(const device_state_entry &entry, std::string &str) const override;

	// device_execute_interface overrides
	virtual void execute_run() override;
	virtual void execute_set_input(int inputnum, int state) override;

	// device_disasm_interface overrides
	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;

private:
	address_space_config m_program_config;
	address_space *m_program;

	u16 m_pc, m_usp, m_ssp, m_ps, m_tmp16, m_tmpea, m_tmp16aux;
	u8 m_pcb, m_dtb, m_usb, m_ssb, m_adb, m_dpr, m_tmp8, m_prefix;
	u32 m_acc, m_temp, m_tmp32;
	s32 m_icount;
	u64 m_tmp64;
	bool m_prefix_valid;

	inline u8 read_8(u32 addr)
	{
		return m_program->read_byte(addr);
	}
	inline u16 read_16(u32 addr)
	{
		if (addr & 1)
		{
			return m_program->read_byte(addr) | (m_program->read_byte(addr+1)<<8);
		}
		else
		{
			return m_program->read_word(addr);
		}
	}
	inline u32 read_32(u32 addr)
	{
		if (addr & 3)
		{
			return m_program->read_byte(addr) | (m_program->read_byte(addr+1)<<8) | (m_program->read_byte(addr+2)<<16) | (m_program->read_byte(addr+3)<<24);
		}
		else
		{
			return m_program->read_dword(addr);
		}
	}
	inline void write_8(u32 addr, u8 data)
	{
//      printf("write %02x to %08x\n", data, addr);
		m_program->write_byte(addr, data);
	}
	inline void write_16(u32 addr, u16 data)
	{
//      printf("write %04x to %08x\n", data, addr);
		if (addr & 1)
		{
			m_program->write_byte(addr, data & 0xff);
			m_program->write_byte(addr+1, (data>>8) & 0xff);
		}
		else
		{
			m_program->write_word(addr, data);
		}
	}
	inline void write_32(u32 addr, u32 data)
	{
		//printf("write %08x to %08x\n", data, addr);
		if (addr & 3)
		{
			m_program->write_byte(addr, data & 0xff);
			m_program->write_byte(addr+1, (data>>8) & 0xff);
			m_program->write_byte(addr+2, (data>>16) & 0xff);
			m_program->write_byte(addr+3, (data>>24) & 0xff);
		}
		else
		{
			m_program->write_dword(addr, data);
		}
	}
	inline u8 read_rX(int reg)
	{
		reg &= 7;
		return m_temp = read_8(reg + 0x188 + (((m_ps>>8)&0x1f)*0x10));
	}
	inline u16 read_rwX(int reg)
	{
		reg &= 7;
		return m_temp = read_16((reg<<1) + 0x180 + (((m_ps>>8)&0x1f)*0x10));
	}
	inline u32 read_rlX(int reg)
	{
		reg &= 3;
		return m_temp = read_32((reg<<2) + 0x180 + (((m_ps>>8)&0x1f)*0x10));
	}
	inline void write_rX(int reg, u8 val)
	{
		reg &= 7;
		write_8(reg + 0x188 + (((m_ps>>8)&0x1f)*0x10), val);
	}
	inline void write_rwX(int reg, u16 val)
	{
		reg &= 7;
		write_16((reg<<1) + 0x180 + (((m_ps>>8)&0x1f)*0x10), val);
	}
	inline void write_rlX(int reg, u32 val)
	{
		reg &= 3;
		write_32((reg<<2) + 0x180 + (((m_ps>>8)&0x1f)*0x10), val);
	}

	inline void setNZ_8(u8 uVal) { m_ps &= ~(F_N|F_Z); m_ps |= (uVal == 0) ? F_Z : 0; m_ps |= (uVal & 0x80) ? F_N : 0; }
	inline void setNZ_16(u16 uVal) { m_ps &= ~(F_N|F_Z); m_ps |= (uVal == 0) ? F_Z : 0; m_ps |= (uVal & 0x8000) ? F_N : 0; }
	inline void setNZ_32(u32 uVal) { m_ps &= ~(F_N|F_Z); m_ps |= (uVal == 0) ? F_Z : 0; m_ps |= (uVal & 0x80000000) ? F_N : 0; }

	// get the full 24 bit address for an @RWx access
	inline u32 getRWbank(int iReg, u16 uBankAddr)
	{
		if (m_prefix_valid)
		{
			m_prefix_valid = false;
			return (m_prefix<<16) | uBankAddr;
		}

		iReg &= 7;
		switch (iReg)
		{
			case 0:
			case 1:
			case 4:
			case 5:
				return (m_dtb<<16) | uBankAddr;

			case 2:
			case 6:
				return (m_adb<<16) | uBankAddr;

			case 3:
			case 7:
				return (m_usb<<16) | uBankAddr;
		}

		// this can't happen, but GCC insists
		return (m_dtb<<16) | uBankAddr;
	}

	inline void push_8(u8 val)
	{
		if (m_ps & F_S)
		{
			m_ssp--;
			write_8((m_ssb << 16) | m_ssp, val);
		}
		else
		{
			m_usp--;
			write_8((m_usb << 16) | m_usp, val);
		}
	}

	inline void push_16(u16 val)
	{
		if (m_ps & F_S)
		{
			m_ssp-=2;
			write_16((m_ssb << 16) | m_ssp, val);
		}
		else
		{
			m_usp-=2;
			write_16((m_usb << 16) | m_usp, val);
		}
	}

	inline void push_16_ssp(u16 val)
	{
		m_ssp-=2;
		write_16((m_ssb << 16) | m_ssp, val);
	}

	inline void push_32(u32 val)
	{
		if (m_ps & F_S)
		{
			m_ssp-=4;
			write_32((m_ssb << 16) | m_ssp, val);
		}
		else
		{
			m_usp-=4;
			write_32((m_usb << 16) | m_usp, val);
		}
	}

	inline u8 pull_8()
	{
		u8 rv = 0;
		if (m_ps & F_S)
		{
			rv = read_8((m_ssb << 16) | m_ssp);
			m_ssp ++;
		}
		else
		{
			rv = read_8((m_usb << 16) | m_usp);
			m_usp ++;
		}

		return rv;
	}

	inline u16 pull_16()
	{
		u16 rv = 0;
		if (m_ps & F_S)
		{
			rv = read_16((m_ssb << 16) | m_ssp);
			m_ssp += 2;
		}
		else
		{
			rv = read_16((m_usb << 16) | m_usp);
			m_usp += 2;
		}

		return rv;
	}

	inline u16 pull_16_ssp()
	{
		u16 rv = read_16((m_ssb << 16) | m_ssp);
		m_ssp += 2;

		return rv;
	}

	inline u32 pull_32()
	{
		u32 rv = 0;
		if (m_ps & F_S)
		{
			rv = read_32((m_ssb << 16) | m_ssp);
			m_ssp += 4;
		}
		else
		{
			rv = read_32((m_usb << 16) | m_usp);
			m_usp += 4;
		}

		return rv;
	}

	inline void doCMP_8(u8 lhs, u8 rhs)
	{
		m_tmp16 = lhs - rhs;
		setNZ_16(m_tmp16 & 0xff);
		m_ps &= ~(F_C|F_V);
		if (m_tmp16 & 0x100)
		{
			m_ps |= F_C;
		}
		if ((lhs ^ rhs) & (lhs ^ (m_tmp16 & 0xff)) & 0x80)
		{
			m_ps |= F_V;
		}
	}
	inline void doCMP_16(u16 lhs, u16 rhs)
	{
		m_tmp32 = lhs - rhs;
		setNZ_16(m_tmp32 & 0xffff);
		m_ps &= ~(F_C|F_V);
		if (m_tmp32 & 0x10000)
		{
			m_ps |= F_C;
		}
		if ((lhs ^ rhs) & (lhs ^ (m_tmp32 & 0xffff)) & 0x8000)
		{
			m_ps |= F_V;
		}
	}
	inline void doCMP_32(u32 lhs, u32 rhs)
	{
		m_tmp64 = lhs - rhs;
		setNZ_32(m_tmp64 & 0xffffffff);
		m_ps &= ~(F_C|F_V);
		if (m_tmp64 & 0x100000000)
		{
			m_ps |= F_C;
		}
		if ((lhs ^ rhs) & (lhs ^ (m_tmp64 & 0xffffffff)) & 0x80000000)
		{
			m_ps |= F_V;
		}
	}

	inline u8 doSUB_8(u8 lhs, u8 rhs)
	{
		m_tmp16 = lhs - rhs;
		setNZ_16(m_tmp16 & 0xff);
		m_ps &= ~(F_C|F_V);
		if (m_tmp16 & 0x100)
		{
			m_ps |= F_C;
		}
		if ((lhs ^ rhs) & (lhs ^ (m_tmp16 & 0xff)) & 0x80)
		{
			m_ps |= F_V;
		}

		return m_tmp16 & 0xff;
	}
	inline u16 doSUB_16(u16 lhs, u16 rhs)
	{
		m_tmp32 = lhs - rhs;
		setNZ_16(m_tmp32 & 0xffff);
		m_ps &= ~(F_C|F_V);
		if (m_tmp32 & 0x10000)
		{
			m_ps |= F_C;
		}
		if ((lhs ^ rhs) & (lhs ^ (m_tmp32 & 0xffff)) & 0x8000)
		{
			m_ps |= F_V;
		}

		return m_tmp32 & 0xffff;
	}
	inline u32 doSUB_32(u32 lhs, u32 rhs)
	{
		m_tmp64 = lhs - rhs;
		setNZ_32(m_tmp64 & 0xffffffff);
		m_ps &= ~(F_C|F_V);
		if (m_tmp64 & 0x100000000)
		{
			m_ps |= F_C;
		}
		if ((lhs ^ rhs) & (lhs ^ (m_tmp64 & 0xffffffff)) & 0x8000000)
		{
			m_ps |= F_V;
		}

		return m_tmp64 & 0xffffffff;
	}
	inline u16 doADD_16(u16 lhs, u16 rhs)
	{
		m_tmp32 = lhs + rhs;
		m_ps &= ~(F_C|F_V);
		if ((m_tmp32 ^ lhs) & (m_tmp32 ^ rhs) & 0x8000)
		{
			m_ps |= F_V;
		}
		if (m_tmp32 > 0xffff)
		{
			m_ps |= F_C;
		}
		setNZ_16(m_tmp32 & 0xffff);

		return m_tmp32 & 0xffff;
	}
	inline u32 doADD_32(u32 lhs, u32 rhs)
	{
		m_tmp64 = lhs + rhs;
		m_ps &= ~(F_C|F_V);
		if ((m_tmp64 ^ lhs) & (m_tmp64 ^ rhs) & 0x80000000)
		{
			m_ps |= F_V;
		}
		if (m_tmp64 > 0xffffffff)
		{
			m_ps |= F_C;
		}
		setNZ_32(m_tmp64 & 0xffffffff);

		return m_tmp64 & 0xffffffff;
	}

	inline void take_branch()
	{
		m_tmp8 = read_8((m_pcb << 16) | (m_pc+1));
		m_pc = m_pc + 2 + (s8)m_tmp8;
		m_icount -= 4;
	}

	void opcodes_bo6c(u8 operand);
	void opcodes_str6e(u8 operand);
	void opcodes_2b6f(u8 operand);
	void opcodes_ea70(u8 operand);
	void opcodes_ea71(u8 operand);
	void opcodes_ea72(u8 operand);
	void opcodes_ea73(u8 operand);
	void opcodes_ea74(u8 operand);
	void opcodes_ea75(u8 operand);
	void opcodes_ea76(u8 operand);
	void opcodes_ea77(u8 operand);
	void opcodes_ea78(u8 operand);

	void set_irq(int vector, int level);
	void clear_irq(int vector);
	void take_irq(int vector, int level);

	int m_vector_level[256];
	int m_outstanding_irqs;
};

DECLARE_DEVICE_TYPE(F2MC16, f2mc16_device)

#endif // MAME_CPU_F2MC16_F2MC16_H
