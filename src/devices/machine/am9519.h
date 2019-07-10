// license:BSD-3-Clause
// copyright-holders:Wilbert Pol,Carl
/***************************************************************************

    AMD AM9519

    Universal Interrupt Controller

                            _____   _____
                   _CS   1 |*    \_/     | 28  VCC
                   _WR   2 |             | 27  C/_D
                   _RD   3 |             | 26  _IACK
                   DB7   4 |             | 25  IREQ7
                   DB6   5 |             | 24  IREQ6
                   DB5   6 |             | 23  IREQ5
                   DB4   7 |    AM9519   | 22  IREQ4
                   DB3   8 |             | 21  IREQ3
                   DB2   9 |             | 20  IREQ2
                   DB1  10 |             | 19  IREQ1
                   DB0  11 |             | 18  IREQ0
                  _RIP  12 |             | 17  GINT
                    EI  13 |             | 16  _EO
                   GND  14 |_____________| 15  _PAUSE

***************************************************************************/

#ifndef MAME_MACHINE_AM9519_H
#define MAME_MACHINE_AM9519_H

#pragma once

class am9519_device : public device_t
{
public:
	am9519_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	auto out_int_callback() { return m_out_int_func.bind(); }

	u8 stat_r();
	u8 data_r();
	void cmd_w(u8 data);
	void data_w(u8 data);
	u32 acknowledge();

	DECLARE_WRITE_LINE_MEMBER( ireq0_w ) { set_irq_line(0, state); }
	DECLARE_WRITE_LINE_MEMBER( ireq1_w ) { set_irq_line(1, state); }
	DECLARE_WRITE_LINE_MEMBER( ireq2_w ) { set_irq_line(2, state); }
	DECLARE_WRITE_LINE_MEMBER( ireq3_w ) { set_irq_line(3, state); }
	DECLARE_WRITE_LINE_MEMBER( ireq4_w ) { set_irq_line(4, state); }
	DECLARE_WRITE_LINE_MEMBER( ireq5_w ) { set_irq_line(5, state); }
	DECLARE_WRITE_LINE_MEMBER( ireq6_w ) { set_irq_line(6, state); }
	DECLARE_WRITE_LINE_MEMBER( ireq7_w ) { set_irq_line(7, state); }

	IRQ_CALLBACK_MEMBER(iack_cb);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	static constexpr device_timer_id TIMER_CHECK_IRQ = 0;

	inline void set_timer() { timer_set(attotime::zero, TIMER_CHECK_IRQ); }
	void set_irq_line(int irq, int state);

	devcb_write_line m_out_int_func;

	u8 m_isr;
	u8 m_irr;
	u8 m_prio;
	u8 m_imr;
	u8 m_irq_lines;

	u8 m_curcnt;
	u8 m_mode;
	u8 m_cmd;
	u8 m_aclear;
	u8 m_count[8];
	u8 m_resp[8][4];
};

DECLARE_DEVICE_TYPE(AM9519, am9519_device)

#endif // MAME_MACHINE_AM9519_H
