// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_INCLUDES_BFM_AD5_H
#define MAME_INCLUDES_BFM_AD5_H

#pragma once

#include "cpu/m68000/m68000.h"


class adder5_state : public driver_device
{
public:
	adder5_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{ }

	void init_ad5();
	void bfm_ad5(machine_config &config);

protected:
	INTERRUPT_GEN_MEMBER(ad5_fake_timer_int);
	void ad5_map(address_map &map);

private:
	// devices
	required_device<cpu_device> m_maincpu;
};

#endif // MAME_INCLUDES_BFM_AD5_H
