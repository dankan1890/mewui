// license:BSD-3-Clause
// copyright-holders:Ville Linde
// TMS32082 MP Disassembler

#ifndef MAME_CPU_TMS32082_DIS_MP_H
#define MAME_CPU_TMS32082_DIS_MP_H

#pragma once

class tms32082_mp_disassembler : public util::disasm_interface
{
public:
	tms32082_mp_disassembler() = default;
	virtual ~tms32082_mp_disassembler() = default;

	virtual u32 opcode_alignment() const override;
	virtual offs_t disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params) override;

private:
	static char const *const BCND_CONDITION[32];
	static char const *const BITNUM_CONDITION[32];
	static char const *const MEMOP_S[2];
	static char const *const MEMOP_M[2];
	static char const *const FLOATOP_PRECISION[4];
	static char const *const ACC_SEL[4];
	static char const *const FLOATOP_ROUND[4];

	std::ostream *output;

	uint32_t fetch(offs_t &pos, const data_buffer &opcodes);
	std::string get_creg_name(uint32_t reg);
	std::string format_vector_op(uint32_t op, uint32_t imm32);
};

#endif
