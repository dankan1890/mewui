// license:BSD-3-Clause
// copyright-holders:R. Belmont
#pragma once

#ifndef __DECOBSMT_H__
#define __DECOBSMT_H__

#include "emu.h"
#include "cpu/m6809/m6809.h"
#include "sound/bsmt2000.h"

#define DECOBSMT_TAG "decobsmt"

#define MCFG_DECOBSMT_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, DECOBSMT, 0)

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class decobsmt_device : public device_t
{
public:
		// construction/destruction
		decobsmt_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

		DECLARE_WRITE8_MEMBER(bsmt_reset_w);
		DECLARE_READ8_MEMBER(bsmt_status_r);
		DECLARE_WRITE8_MEMBER(bsmt0_w);
		DECLARE_WRITE8_MEMBER(bsmt1_w);
		DECLARE_READ8_MEMBER(bsmt_comms_r);
		DECLARE_WRITE8_MEMBER(bsmt_comms_w);

		DECLARE_WRITE_LINE_MEMBER(bsmt_reset_line);

		INTERRUPT_GEN_MEMBER(decobsmt_firq_interrupt);

		required_device<cpu_device> m_ourcpu;
		required_device<bsmt2000_device> m_bsmt;

protected:
		// device-level overrides
		virtual void device_start() override;
		virtual void device_reset() override;
		virtual machine_config_constructor device_mconfig_additions() const override;

private:
		uint8_t m_bsmt_latch;
		uint8_t m_bsmt_reset;
		uint8_t m_bsmt_comms;
};


// device type definition
extern const device_type DECOBSMT;

#endif  /* __DECOBSMT_H__ */
