// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#ifndef MAME_MACHINE_JVS13551_H
#define MAME_MACHINE_JVS13551_H

#pragma once

#include "machine/jvsdev.h"


class jvs_host;

class sega_837_13551_device : public jvs_device
{
public:
	template <typename T>
	sega_837_13551_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&jvs_host_tag)
		: sega_837_13551_device(mconfig, tag, owner, clock)
	{
		host.set_tag(std::forward<T>(jvs_host_tag));
	}

	sega_837_13551_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template <uint8_t Which, typename T>
	void set_port_tag(T &&port_tag) { port[Which].set_tag(std::forward<T>(port_tag)); }

	virtual const tiny_rom_entry *device_rom_region() const override;

	DECLARE_WRITE_LINE_MEMBER(jvs13551_coin_1_w);
	DECLARE_WRITE_LINE_MEMBER(jvs13551_coin_2_w);
	void inc_coin(int coin);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

	// JVS device overrides
	virtual const char *device_id() override;
	virtual uint8_t command_format_version() override;
	virtual uint8_t jvs_standard_version() override;
	virtual uint8_t comm_method_version() override;
	virtual void function_list(uint8_t *&buf) override;
	virtual bool switches(uint8_t *&buf, uint8_t count_players, uint8_t bytes_per_switch) override;
	virtual bool coin_counters(uint8_t *&buf, uint8_t count) override;
	virtual bool coin_add(uint8_t slot, int32_t count) override;
	virtual bool analogs(uint8_t *&buf, uint8_t count) override;
	virtual bool swoutputs(uint8_t count, const uint8_t *vals) override;
	virtual bool swoutputs(uint8_t id, uint8_t val) override;

private:
	optional_ioport_array<12> port;
	uint16_t coin_counter[2];
};

DECLARE_DEVICE_TYPE(SEGA_837_13551, sega_837_13551_device)

#endif // MAME_MACHINE_JVS13551_H
