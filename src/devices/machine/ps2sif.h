// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/******************************************************************************
*
*   Sony PlayStation 2 SIF device skeleton
*
*   To Do:
*     Everything
*
*/

#ifndef MAME_MACHINE_PS2SIF_H
#define MAME_MACHINE_PS2SIF_H

#pragma once

#include "ps2intc.h"

class ps2_sif_device : public device_t
{
public:
	template <typename T>
	ps2_sif_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&intc_tag)
		: ps2_sif_device(mconfig, tag, owner, (uint32_t)0)
	{
		m_intc.set_tag(std::forward<T>(intc_tag));
	}

	ps2_sif_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~ps2_sif_device() override;

	DECLARE_READ32_MEMBER(ee_r);
	DECLARE_WRITE32_MEMBER(ee_w);
	DECLARE_READ32_MEMBER(iop_r);
	DECLARE_WRITE32_MEMBER(iop_w);

	uint32_t fifo_depth(uint32_t channel);
	void fifo_push(uint32_t channel, uint32_t value);
	uint32_t fifo_pop(uint32_t channel);

	static const size_t MAX_FIFO_DEPTH;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	required_device<ps2_intc_device> m_intc;

	uint32_t m_ms_mailbox;
	uint32_t m_sm_mailbox;
	uint32_t m_ms_flag;
	uint32_t m_sm_flag;
	uint32_t m_ctrl;

	std::unique_ptr<uint32_t[]> m_fifo[2];
	uint32_t m_fifo_curr[2];
};

DECLARE_DEVICE_TYPE(SONYPS2_SIF, ps2_sif_device)

#endif // MAME_MACHINE_PS2SIF_H
