// license:GPL-2.0+
// copyright-holders:Raphael Nabet
#ifndef MAME_VIDEO_733_ASR
#define MAME_VIDEO_733_ASR

#pragma once

#include "emupal.h"
#include "screen.h"
#define asr733_chr_region ":gfx1"

class asr733_device : public device_t, public device_gfx_interface
{
public:
	enum
	{
		/* 8 bytes per character definition */
		single_char_len = 8,
		chr_region_len   = 128*single_char_len
	};

	asr733_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_READ8_MEMBER(cru_r);
	DECLARE_WRITE8_MEMBER(cru_w);

	auto keyint_cb() { return m_keyint_line.bind(); }
	auto lineint_cb() { return m_lineint_line.bind(); }

protected:
	// device-level overrides
	void device_start() override;
	void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	ioport_constructor device_input_ports() const override;

private:
	// internal state
#if 0
	uint8_t m_OutQueue[ASROutQueueSize];
	int m_OutQueueHead;
	int m_OutQueueLen;
#endif

	void check_keyboard();
	void refresh(bitmap_ind16 &bitmap, int x, int y);

	void set_interrupt_line();
	void draw_char(int character, int x, int y, int color);
	void linefeed();
	void transmit(uint8_t data);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	required_device<screen_device> m_screen;

	emu_timer *m_line_timer;                // screen line timer

	uint8_t   m_recv_buf;
	uint8_t   m_xmit_buf;

	uint8_t   m_status;
	uint8_t   m_mode;
	uint8_t   m_last_key_pressed;
	int     m_last_modifier_state;

	unsigned char m_repeat_timer;
	int     m_new_status_flag;

	int     m_x;

	std::unique_ptr<bitmap_ind16>       m_bitmap;

	devcb_write_line                   m_keyint_line;
	devcb_write_line                   m_lineint_line;
};

DECLARE_DEVICE_TYPE(ASR733, asr733_device)

#endif // MAME_VIDEO_733_ASR
