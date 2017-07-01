// license:BSD-3-Clause
// copyright-holders:R. Belmont
#ifndef ESQVFD_H
#define ESQVFD_H

#include "emu.h"

#define MCFG_ESQ1x22_ADD(_tag)  \
	MCFG_DEVICE_ADD(_tag, ESQ1x22, 60)

#define MCFG_ESQ1x22_REMOVE(_tag) \
	MCFG_DEVICE_REMOVE(_tag)

#define MCFG_ESQ2x40_ADD(_tag)  \
	MCFG_DEVICE_ADD(_tag, ESQ2x40, 60)

#define MCFG_ESQ2x40_REMOVE(_tag) \
	MCFG_DEVICE_REMOVE(_tag)

#define MCFG_ESQ2x40_SQ1_ADD(_tag)  \
	MCFG_DEVICE_ADD(_tag, ESQ2x40_SQ1, 60)

#define MCFG_ESQ2x40_SQ1_REMOVE(_tag) \
	MCFG_DEVICE_REMOVE(_tag)

class esqvfd_t : public device_t {
public:
	esqvfd_t(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

	DECLARE_WRITE8_MEMBER( write ) { write_char(data); }

	virtual void write_char(int data) = 0;
	virtual void update_display();

	uint32_t conv_segments(uint16_t segin);

protected:
	static const uint8_t AT_NORMAL      = 0x00;
	static const uint8_t AT_BOLD        = 0x01;
	static const uint8_t AT_UNDERLINE   = 0x02;
	static const uint8_t AT_BLINK       = 0x04;
	static const uint8_t AT_BLINKED     = 0x80;   // set when character should be blinked off

	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	int m_cursx, m_cursy;
	int m_savedx, m_savedy;
	int m_rows, m_cols;
	uint8_t m_curattr;
	uint8_t m_lastchar;
	uint8_t m_chars[2][40];
	uint8_t m_attrs[2][40];
	uint8_t m_dirty[2][40];
};

class esq1x22_t : public esqvfd_t {
public:
	esq1x22_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void write_char(int data) override;

protected:
	virtual machine_config_constructor device_mconfig_additions() const override;

private:
};

class esq2x40_t : public esqvfd_t {
public:
	esq2x40_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void write_char(int data) override;

protected:
	virtual machine_config_constructor device_mconfig_additions() const override;

private:
};

class esq2x40_sq1_t : public esqvfd_t {
public:
	esq2x40_sq1_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void write_char(int data) override;

protected:
	virtual machine_config_constructor device_mconfig_additions() const override;

private:
	bool m_Wait87Shift, m_Wait88Shift;
};

extern const device_type ESQ1x22;
extern const device_type ESQ2x40;
extern const device_type ESQ2x40_SQ1;

#endif
