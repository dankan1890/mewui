// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/***************************************************************************

    r65c02.h

    Rockwell 65c02, CMOS variant with bitwise instructions

***************************************************************************/

#ifndef __R65C02_H__
#define __R65C02_H__

#include "m65c02.h"

class r65c02_device : public m65c02_device {
public:
	r65c02_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	r65c02_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

	static const disasm_entry disasm_entries[0x100];

	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;
	virtual void do_exec_full() override;
	virtual void do_exec_partial() override;
};

enum {
	R65C02_IRQ_LINE = m6502_device::IRQ_LINE,
	R65C02_NMI_LINE = m6502_device::NMI_LINE,
	R65C02_SET_OVERFLOW = m6502_device::V_LINE
};

extern const device_type R65C02;

#endif
