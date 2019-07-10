// license:BSD-3-Clause
// copyright-holders: F. Ulivi
/*********************************************************************

    hp9885.h

    HP9885M 8" floppy drive

*********************************************************************/

#ifndef MAME_BUS_HP9845_IO_HP9885_H
#define MAME_BUS_HP9845_IO_HP9885_H

#pragma once

#include "98032.h"
#include "imagedev/floppy.h"
#include "machine/fdc_pll.h"

class hp9885_device : public hp98032_gpio_card_device
{
public:
	hp9885_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~hp9885_device();

	// hp98032_gpio_card_device overrides
	virtual uint16_t get_jumpers() const override;
	virtual uint16_t input_r() const override;
	virtual uint8_t ext_status_r() const override;
	virtual void output_w(uint16_t data) override;
	virtual void ext_control_w(uint8_t data) override;
	virtual DECLARE_WRITE_LINE_MEMBER(pctl_w) override;
	virtual DECLARE_WRITE_LINE_MEMBER(io_w) override;
	virtual DECLARE_WRITE_LINE_MEMBER(preset_w) override;

protected:
	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	// FSM states
	enum {
		FSM_IDLE,
		FSM_RECALIBRATING,
		FSM_SETTLING,
		FSM_GOT_PW,
		FSM_POSITIONING,
		FSM_SEEKING,
		FSM_STATUS_DELAY,
		FSM_RD_STATUS1,
		FSM_RD_STATUS2,
		FSM_WAIT_ID_AM,
		FSM_RD_ID,
		FSM_WAIT_DATA_AM,
		FSM_RD_DATA,
		FSM_WR_DATA
	};

	// Head states
	enum {
		HEAD_UNLOADED,
		HEAD_SETTLING,
		HEAD_LOADED
	};

	// Operations
	enum {
		OP_NONE,
		OP_READ,
		OP_WRITE,
		OP_STEP_IN,
		OP_GET_STATUS
	};

	required_device<floppy_connector> m_drive_connector;
	floppy_image_device *m_drive;
	uint16_t m_input;
	uint16_t m_output;
	uint16_t m_status;
	int m_fsm_state;
	int m_head_state;
	int m_op;
	bool m_pctl;
	bool m_ibf;
	bool m_obf;
	bool m_outputting;
	bool m_had_transition;
	bool m_dskchg;
	unsigned m_track;
	unsigned m_seek_track;
	unsigned m_seek_sector;
	unsigned m_sector_cnt;
	unsigned m_word_cnt;
	unsigned m_rev_cnt;
	uint32_t m_am_detector;
	uint16_t m_crc; // x^15 is stored in LSB

	// Timers
	emu_timer *m_fsm_timer;
	emu_timer *m_head_timer;
	emu_timer *m_bit_byte_timer;

	// PLL
	fdc_pll_t m_pll;

	void floppy_ready_cb(floppy_image_device *floppy , int state);
	void floppy_index_cb(floppy_image_device *floppy , int state);

	void set_state(int new_state);
	void init_status(unsigned unit_no);
	void encode_error(bool writing);
	void set_error(unsigned error_code);
	void new_word();
	void do_FSM();
	bool load_head();
	void recalibrate();
	void one_step(bool outward);
	void adv_sector();
	void start_rd();
	void stop_rdwr();
	uint16_t rd_word();
	void wr_word(uint16_t word);
	void preset_crc();
	void update_crc(bool bit);
	void set_ibf(bool state);
	void set_output();
	void output_status(bool delayed = false);
	void update_busy();
};

DECLARE_DEVICE_TYPE(HP9885 , hp9885_device)

#endif /* MAME_BUS_HP9845_IO_HP9885_H */
