// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    dis32031.c
    Disassembler for the portable TMS32C031 emulator.
    Written by Aaron Giles

***************************************************************************/

#ifndef MAME_CPU_TMS32031_DIS32031_H
#define MAME_CPU_TMS32031_DIS32031_H

#pragma once

class tms32031_disassembler : public util::disasm_interface
{
public:
	tms32031_disassembler() = default;
	virtual ~tms32031_disassembler() = default;

	virtual u32 opcode_alignment() const override;
	virtual offs_t disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params) override;

private:
	enum {
		INTEGER         = 0,
		FLOAT           = 1,
		NODEST          = 2,
		NOSOURCE        = 4,
		NOSOURCE1       = NOSOURCE,
		NOSOURCE2       = 8,
		SWAPSRCDST      = 16,
		UNSIGNED        = 32
	};

	static const char *const regname[32];
	static const char *const condition[32];
	void append_indirect(uint8_t ma, int8_t disp, std::ostream &stream);
	std::string get_indirect(uint8_t ma, int8_t disp);
	void append_immediate(uint16_t data, int is_float, int is_unsigned, std::ostream &stream);
	void disasm_general(const char *opstring, uint32_t op, int flags, std::ostream &stream);
	void disasm_3op(const char *opstring, uint32_t op, int flags, std::ostream &stream);
	void disasm_conditional(const char *opstring, uint32_t op, int flags, std::ostream &stream);
	void disasm_parallel_3op3op(const char *opstring1, const char *opstring2, uint32_t op, int flags, const uint8_t *srctable, std::ostream &stream);
	void disasm_parallel_3opstore(const char *opstring1, const char *opstring2, uint32_t op, int flags, std::ostream &stream);
	void disasm_parallel_loadload(const char *opstring1, const char *opstring2, uint32_t op, int flags, std::ostream &stream);
	void disasm_parallel_storestore(const char *opstring1, const char *opstring2, uint32_t op, int flags, std::ostream &stream);
};

#endif
