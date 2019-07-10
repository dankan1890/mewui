// license:BSD-3-Clause
// copyright-holders:Antoine Mine
/**********************************************************************

  Copyright (C) Antoine Mine' 2008

   Hewlett Packard HP48 S/SX & G/GX and HP49 G

**********************************************************************/
#ifndef MAME_INCLUDES_HP84_H
#define MAME_INCLUDES_HP84_H

#pragma once

#include "cpu/saturn/saturn.h"
#include "machine/hp48_port.h"
#include "sound/dac.h"
#include "emupal.h"
#include "screen.h"

/* model */
typedef enum {
	HP48_S,
	HP48_SX,
	HP48_G,
	HP48_GX,
	HP48_GP,
	HP49_G
} hp48_models;

/* memory module configuration */
typedef struct
{
	/* static part */
	uint32_t off_mask;             /* offset bit-mask, indicates the real size */
	read8_delegate read;
	const char *read_name;
	write8_delegate write;
	void* data;                  /* non-NULL for banks */
	int isnop;

	/* configurable part */
	uint8_t  state;                /* one of HP48_MODULE_ */
	uint32_t base;                 /* base address */
	uint32_t mask;                 /* often improperly called size, it is an address select mask */

} hp48_module;


/* screen image averaging */
#define HP48_NB_SCREENS 3

class hp48_state : public driver_device
{
public:
	hp48_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_dac(*this, "dac")
		, m_palette(*this, "palette")
		, m_screen(*this, "screen")
		, m_port(*this, "port%u", 1U)
	{
	}

	void hp48s(machine_config &config);
	void hp48gp(machine_config &config);
	void hp48sx(machine_config &config);
	void hp48g(machine_config &config);
	void hp48gx(machine_config &config);
	void hp49g(machine_config &config);

	void init_hp48();

	/* from highest to lowest priority: HDW, NCE2, CE1, CE2, NCE3, NCE1 */
	hp48_module m_modules[6];

	void decode_nibble(uint8_t* dst, uint8_t* src, int size);
	void encode_nibble(uint8_t* dst, uint8_t* src, int size);

	void apply_modules();

private:
	virtual void machine_reset() override;
	void base_machine_start(hp48_models model);

	void hp48_palette(palette_device &palette) const;
	DECLARE_MACHINE_START(hp49g);
	DECLARE_MACHINE_START(hp48gx);
	DECLARE_MACHINE_START(hp48g);
	DECLARE_MACHINE_START(hp48gp);
	DECLARE_MACHINE_START(hp48sx);
	DECLARE_MACHINE_START(hp48s);
	uint32_t screen_update_hp48(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE8_MEMBER(io_w);
	DECLARE_READ8_MEMBER(io_r);
	DECLARE_READ8_MEMBER(bank_r);
	DECLARE_WRITE8_MEMBER(hp49_bank_w);
	TIMER_CALLBACK_MEMBER(rs232_byte_recv_cb);
	TIMER_CALLBACK_MEMBER(rs232_byte_sent_cb);
	TIMER_CALLBACK_MEMBER(kbd_cb);
	TIMER_CALLBACK_MEMBER(timer1_cb);
	TIMER_CALLBACK_MEMBER(timer2_cb);
	void update_annunciators();
	void pulse_irq(int irq_line);
	void rs232_start_recv_byte(uint8_t data);
	void rs232_send_byte();
	int get_in();
	void update_kdn();
	void reset_modules();

	/* memory controller */
	DECLARE_WRITE_LINE_MEMBER(mem_reset);
	DECLARE_WRITE32_MEMBER(mem_config);
	DECLARE_WRITE32_MEMBER(mem_unconfig);
	DECLARE_READ32_MEMBER(mem_id);

	/* CRC computation */
	DECLARE_WRITE32_MEMBER(mem_crc);

	/* IN/OUT registers */
	DECLARE_READ32_MEMBER(reg_in);
	DECLARE_WRITE32_MEMBER(reg_out);

	/* keyboard interrupt system */
	DECLARE_WRITE_LINE_MEMBER(rsi);
	void hp48_common(machine_config &config);
	void hp48(address_map &map);

	required_device<saturn_device> m_maincpu;
	required_device<dac_bit_interface> m_dac;
	required_device<palette_device> m_palette;
	required_device<screen_device> m_screen;

	uint8_t *m_videoram;
	uint8_t m_io[64];
	hp48_models m_model;

	/* OUT register from SATURN (actually 12-bit) */
	uint16_t m_out;

	/* keyboard interrupt */
	uint8_t m_kdn;

	/* RAM/ROM extensions, GX/SX only
	   port1: SX/GX: 32/128 KB
	   port2: SX:32/128KB, GX:128/512/4096 KB
	*/
	optional_device_array<hp48_port_image_device, 2> m_port;

	uint32_t m_bank_switch;
	uint32_t m_io_addr;
	uint16_t m_crc;
	uint8_t m_timer1;
	uint32_t m_timer2;
	uint8_t m_screens[HP48_NB_SCREENS][64][144];
	int m_cur_screen;
	uint8_t* m_rom;
	emu_timer *m_1st_timer;
	emu_timer *m_2nd_timer;
	emu_timer *m_kbd_timer;
};


/***************************************************************************
    MACROS
***************************************************************************/

/* read from I/O memory */
#define HP48_IO_4(x)   (m_io[(x)])
#define HP48_IO_8(x)   (m_io[(x)] | (m_io[(x)+1] << 4))
#define HP48_IO_12(x)  (m_io[(x)] | (m_io[(x)+1] << 4) | (m_io[(x)+2] << 8))
#define HP48_IO_20(x)  (m_io[(x)] | (m_io[(x)+1] << 4) | (m_io[(x)+2] << 8) | \
						(m_io[(x)+3] << 12) | (m_io[(x)+4] << 16))


/*----------- defined in machine/hp48.c -----------*/

/***************************************************************************
    GLOBAL VARIABLES & CONSTANTS
***************************************************************************/

/* I/O memory */



/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

/* list of memory modules from highest to lowest priority */
#define HP48_HDW  0
#define HP48_NCE2 1
#define HP48_CE1  2
#define HP48_CE2  3
#define HP48_NCE3 4
#define HP48_NCE1 5

#endif // MAME_INCLUDES_HP84_H
