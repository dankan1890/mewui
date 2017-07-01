// license:BSD-3-Clause
// copyright-holders:hap, Igor
/*

  KB1013VK1-2

*/

#ifndef _KB1013VK12_H_
#define _KB1013VK12_H_

#include "sm500.h"


// I/O ports setup

// ..





class kb1013vk12_device : public sm500_device
{
public:
	kb1013vk12_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;
	virtual void execute_one() override;

	// opcode handlers
	virtual void op_bs0();
	virtual void op_bs1();
};


extern const device_type KB1013VK12;


#endif /* _KB1013VK12_H_ */
