// license:BSD-3-Clause
// copyright-holders:Curt Coder
#pragma once

#ifndef MAME_INCLUDES_TMC600_H
#define MAME_INCLUDES_TMC600_H

#include "cpu/cosmac/cosmac.h"
#include "imagedev/cassette.h"
#include "imagedev/snapquik.h"
#include "bus/centronics/ctronics.h"
#include "bus/tmc600/euro.h"
#include "machine/cdp1852.h"
#include "machine/ram.h"
#include "machine/timer.h"
#include "sound/cdp1869.h"
#include "speaker.h"

#define SCREEN_TAG          "screen"
#define CDP1802_TAG         "cdp1802"
#define CDP1869_TAG         "cdp1869"
#define CDP1852_KB_TAG      "cdp1852_kb"
#define CDP1852_BUS_TAG     "cdp1852_bus"
#define CDP1852_TMC700_TAG  "cdp1852_printer"
#define CENTRONICS_TAG      "centronics"

#define TMC600_PAGE_RAM_SIZE    0x400
#define TMC600_PAGE_RAM_MASK    0x3ff

class tmc600_state : public driver_device
{
public:
	tmc600_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, CDP1802_TAG),
		m_vis(*this, CDP1869_TAG),
		m_bwio(*this, CDP1852_KB_TAG),
		m_cassette(*this, "cassette"),
		m_centronics(*this, "centronics"),
		m_bus(*this, TMC600_EURO_BUS_TAG),
		m_ram(*this, RAM_TAG),
		m_char_rom(*this, "chargen"),
		m_page_ram(*this, "page_ram"),
		m_color_ram(*this, "color_ram"),
		m_run(*this, "RUN"),
		m_key_row(*this, "Y%u", 0)
	{ }

	void tmc600(machine_config &config);
	void tmc600_video(machine_config &config);

private:
	required_device<cosmac_device> m_maincpu;
	required_device<cdp1869_device> m_vis;
	required_device<cdp1852_device> m_bwio;
	required_device<cassette_image_device> m_cassette;
	required_device<centronics_device> m_centronics;
	required_device<tmc600_euro_bus_slot_t> m_bus;
	required_device<ram_device> m_ram;
	required_region_ptr<uint8_t> m_char_rom;
	required_shared_ptr<uint8_t> m_page_ram;
	optional_shared_ptr<uint8_t> m_color_ram;
	required_ioport m_run;
	required_ioport_array<8> m_key_row;

	virtual void video_start() override;

	DECLARE_READ8_MEMBER( rtc_r );
	DECLARE_WRITE8_MEMBER( printer_w );
	DECLARE_WRITE8_MEMBER( vismac_register_w );
	DECLARE_WRITE8_MEMBER( vismac_data_w );
	DECLARE_WRITE8_MEMBER( page_ram_w );
	DECLARE_READ_LINE_MEMBER( clear_r );
	DECLARE_READ_LINE_MEMBER( ef2_r );
	DECLARE_READ_LINE_MEMBER( ef3_r );
	DECLARE_WRITE_LINE_MEMBER( q_w );
	DECLARE_WRITE8_MEMBER( sc_w );
	DECLARE_WRITE8_MEMBER( out3_w );
	DECLARE_WRITE_LINE_MEMBER( prd_w );

	uint8_t get_color(uint16_t pma);

	// video state
	int m_vismac_reg_latch;     // video register latch
	int m_vismac_color_latch;   // color latch
	bool m_blink;                // cursor blink
	int m_frame;
	bool m_rtc_int;
	u8 m_out3;

	TIMER_DEVICE_CALLBACK_MEMBER(blink_tick);
	CDP1869_CHAR_RAM_READ_MEMBER(tmc600_char_ram_r);
	CDP1869_PCB_READ_MEMBER(tmc600_pcb_r);

	void cdp1869_page_ram(address_map &map);
	void tmc600_io_map(address_map &map);
	void tmc600_map(address_map &map);
};

#endif
