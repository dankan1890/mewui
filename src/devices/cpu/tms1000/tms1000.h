// license:BSD-3-Clause
// copyright-holders:hap
/*

  TMS1000 family - TMS1000, TMS1070, TMS1040, TMS1200

*/

#ifndef _TMS1000_H_
#define _TMS1000_H_

#include "tms1k_base.h"


class tms1000_cpu_device : public tms1k_base_device
{
public:
	tms1000_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	tms1000_cpu_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, uint8_t o_pins, uint8_t r_pins, uint8_t pc_bits, uint8_t byte_bits, uint8_t x_bits, int prgwidth, address_map_constructor program, int datawidth, address_map_constructor data, const char *shortname, const char *source);

protected:
	// overrides
	virtual void device_reset() override;
	virtual machine_config_constructor device_mconfig_additions() const override;


	virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;
};

class tms1070_cpu_device : public tms1000_cpu_device
{
public:
	tms1070_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};

class tms1040_cpu_device : public tms1000_cpu_device
{
public:
	tms1040_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class tms1200_cpu_device : public tms1000_cpu_device
{
public:
	tms1200_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class tms1700_cpu_device : public tms1000_cpu_device
{
public:
	tms1700_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};

class tms1730_cpu_device : public tms1000_cpu_device
{
public:
	tms1730_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


extern const device_type TMS1000;
extern const device_type TMS1070;
extern const device_type TMS1040;
extern const device_type TMS1200;
extern const device_type TMS1700;
extern const device_type TMS1730;

#endif /* _TMS1000_H_ */
