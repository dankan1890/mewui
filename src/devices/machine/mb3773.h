// license:BSD-3-Clause
// copyright-holders:smf
/***************************************************************************

    Fujistu MB3773

    Power Supply Monitor with Watch Dog Timer (i.e. Reset IC)

***************************************************************************/

#ifndef MAME_MACHINE_MB3773_H
#define MAME_MACHINE_MB3773_H

#pragma once

class mb3773_device : public device_t
{
public:
	// construction/destruction
	mb3773_device( const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	// I/O operations
	WRITE_LINE_MEMBER( write_line_ck );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	void reset_timer();

	// internal state
	emu_timer *m_watchdog_timer;
	int m_ck;
};

DECLARE_DEVICE_TYPE(MB3773, mb3773_device)

#endif // MAME_MACHINE_MB3773_H
