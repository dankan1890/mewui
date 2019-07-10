// License:BSD-3-Clause
// copyright-holders:F. Ulivi
/*********************************************************************

    hp98x5_io_sys.h

    HP98x5 I/O sub-system

*********************************************************************/

#ifndef MAME_MACHINE_HP98X5_IO_SYS_H
#define MAME_MACHINE_HP98X5_IO_SYS_H

#pragma once

class hp98x5_io_sys_device : public device_t
{
public:
	// construction/destruction
	hp98x5_io_sys_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	auto irl() { return m_irl_handler.bind(); }
	auto irh() { return m_irh_handler.bind(); }
	auto sts() { return m_sts_handler.bind(); }
	auto flg() { return m_flg_handler.bind(); }
	auto dmar() { return m_dmar_handler.bind(); }

	IRQ_CALLBACK_MEMBER(irq_callback);
	DECLARE_WRITE8_MEMBER(pa_w);

	void set_irq(uint8_t sc , int state);
	void set_sts(uint8_t sc , int state);
	void set_flg(uint8_t sc , int state);
	void set_dmar(uint8_t sc , int state);

	bool is_irq_pending(uint8_t sc) const;
private:
	devcb_write_line m_irl_handler;
	devcb_write_line m_irh_handler;
	devcb_write_line m_flg_handler;
	devcb_write_line m_sts_handler;
	devcb_write_line m_dmar_handler;

	// Interrupt handling
	uint16_t m_irq_pending;

	// FLG/STS handling
	uint8_t m_pa;
	uint16_t m_flg_status;
	uint16_t m_sts_status;

	// DMAR handling
	uint16_t m_dmar_status;

	void update_irq();
	void update_flg_sts();
	void update_dmar();
};

// device type definition
DECLARE_DEVICE_TYPE(HP98X5_IO_SYS, hp98x5_io_sys_device)

#endif // MAME_MACHINE_HP98X5_IO_SYS_H
