// license:BSD-3-Clause
// copyright-holders:Steve Ellenoff
/*****************************************************************************
 *
 *   i8051dasm.c
 *   Portable MCS-51 Family Emulator
 *
 *   Chips in the family:
 *   8051 Product Line (8031,8051,8751)
 *   8052 Product Line (8032,8052,8752)
 *   8054 Product Line (8054)
 *   8058 Product Line (8058)
 *
 *   Copyright Steve Ellenoff, all rights reserved.
 *
 *  This work is based on:
 *  #1) 'Intel(tm) MC51 Microcontroller Family Users Manual' and
 *  #2) 8051 simulator by Travis Marlatte
 *  #3) Portable UPI-41/8041/8741/8042/8742 emulator V0.1 by Juergen Buchmueller (MAME CORE)
 *
 *****************************************************************************
 * Symbol Memory Name Tables borrowed from:
 * D52 8052 Disassembler - Copyright Jeffery L. Post
 *****************************************************************************/

#ifndef MAME_CPU_MCS51_MCS51DASM_H
#define MAME_CPU_MCS51_MCS51DASM_H

#pragma once

#include <unordered_map>


class mcs51_disassembler : public util::disasm_interface
{
public:
	struct mem_info {
		int addr;
		const char *name;
	};

	static const mem_info default_names[];
	static const mem_info i8052_names[];
	static const mem_info i80c52_names[];
	static const mem_info ds5002fp_names[];
	static const mem_info i8xc751_names[];

	template<typename ...Names> mcs51_disassembler(Names &&... names) : mcs51_disassembler() {
		add_names(names...);
	}

	mcs51_disassembler();
	virtual ~mcs51_disassembler() = default;

	template<typename ...Names> void add_names(const mem_info *info, Names &&... names)
	{
		add_names(names...);
		add_names(info);
	}

	void add_names(const mem_info *info);

	virtual u32 opcode_alignment() const override;
	virtual offs_t disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params) override;

protected:
	virtual offs_t disassemble_op(std::ostream &stream, unsigned PC, offs_t pc, const data_buffer &opcodes, const data_buffer &params, uint8_t op);

	std::string get_data_address( uint8_t arg ) const;
	std::string get_bit_address( uint8_t arg ) const;
private:
	std::unordered_map<offs_t, const char *> m_names;

};

class i8051_disassembler : public mcs51_disassembler
{
public:
	i8051_disassembler();
	virtual ~i8051_disassembler() = default;
};

class i8052_disassembler : public mcs51_disassembler
{
public:
	i8052_disassembler();
	virtual ~i8052_disassembler() = default;
};

class i80c51_disassembler : public mcs51_disassembler
{
public:
	i80c51_disassembler();
	virtual ~i80c51_disassembler() = default;
};

class i80c52_disassembler : public mcs51_disassembler
{
public:
	i80c52_disassembler();
	virtual ~i80c52_disassembler() = default;
};

class ds5002fp_disassembler : public mcs51_disassembler
{
public:
	ds5002fp_disassembler();
	virtual ~ds5002fp_disassembler() = default;
};


#endif
