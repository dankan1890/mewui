// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/**********************************************************************

    SGI CRIME skeleton device

**********************************************************************/

#ifndef MAME_VIDEO_CRIME_H
#define MAME_VIDEO_CRIME_H

#pragma once

#include "cpu/mips/mips3.h"

class crime_device : public device_t
{
public:
	template <typename T>
	crime_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu_tag)
		: crime_device(mconfig, tag, owner, (uint32_t)0)
	{
		m_maincpu.set_tag(std::forward<T>(cpu_tag));
	}

	crime_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void map(address_map &map);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;

	DECLARE_READ64_MEMBER(base_r);
	DECLARE_WRITE64_MEMBER(base_w);

	required_device<mips3_device> m_maincpu;

	uint64_t m_mem_bank_ctrl[8];
};

DECLARE_DEVICE_TYPE(SGI_CRIME, crime_device)

#endif // MAME_VIDEO_CRIME_H
