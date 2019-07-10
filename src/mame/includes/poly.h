// license:BSD-3-Clause
// copyright-holders:Robbbert,Nigel Barnes
// thanks-to:Andrew Trotman
/***************************************************************************

    Poly/Proteus (New Zealand)

    10/07/2011 Skeleton driver.

    http://www.cs.otago.ac.nz/homepages/andrew/poly/Poly.htm

    Andrew has supplied the roms for -bios 1

    It uses a 6809 for all main functions. There is a Z80 for CP/M, but all
    of the roms are 6809 code.

    The keyboard controller is one of those custom XR devices.
    Will use the terminal keyboard instead.

    With bios 1, after entering your userid and password, you get a black
    screen. This is normal, because it joins to a network which isn't there.

    ToDo:
    - Almost Everything!
    - Connect up the device ports & lines
    - Find out about graphics mode and how it is selected
    - Fix Keyboard so that the Enter key tells BASIC to do something
    - Find out how to make 2nd teletext screen to display
    - Banking

****************************************************************************/

#ifndef MAME_INCLUDES_POLY_H
#define MAME_INCLUDES_POLY_H

#pragma once

#include "cpu/m6809/m6809.h"
#include "imagedev/floppy.h"
#include "machine/6821pia.h"
#include "machine/6840ptm.h"
#include "machine/6850acia.h"
#include "machine/kr2376.h"
#include "machine/clock.h"
#include "machine/mc6854.h"
#include "machine/bankdev.h"
#include "machine/ram.h"
#include "machine/input_merger.h"
#include "machine/wd_fdc.h"
#include "machine/keyboard.h"
#include "sound/spkrdev.h"
#include "video/saa5050.h"
#include "screen.h"
#include "speaker.h"

//#include "bus/poly/network.h"


class poly_state : public driver_device
{
public:
	poly_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_bankdev(*this, "bankdev")
		, m_ram(*this, RAM_TAG)
		, m_trom(*this, "saa5050_%u", 1)
		, m_pia(*this, "pia%u", 0)
		, m_adlc(*this, "mc6854")
		, m_ptm(*this, "ptm")
		, m_irqs(*this, "irqs")
//      , m_kr2376(*this, "kr2376")
		, m_kbd(*this, "X%u", 0)
		, m_modifiers(*this, "MODIFIERS")
		, m_speaker(*this, "speaker")
		, m_user(*this, "user")
		, m_system(*this, "system")
		, m_videoram(*this, "videoram")
		, m_dat(*this, "dat")
		, m_acia(*this, "acia")
		, m_acia_clock(*this, "acia_clock")
	{
	}

	static constexpr feature_type imperfect_features() { return feature::KEYBOARD; }

	void poly(machine_config &config);
	void poly2(machine_config &config);

	void init_poly();

	virtual void poly_bank(address_map &map);

private:
	DECLARE_READ8_MEMBER( logical_mem_r );
	DECLARE_WRITE8_MEMBER( logical_mem_w );
	DECLARE_READ8_MEMBER( vector_r );
	void kbd_put(u8 data); // remove when KR2376 is implemented
	DECLARE_READ_LINE_MEMBER( kbd_shift_r );
	DECLARE_READ_LINE_MEMBER( kbd_control_r );
	DECLARE_WRITE8_MEMBER( pia0_pa_w );
	DECLARE_WRITE8_MEMBER( pia0_pb_w );
	DECLARE_READ8_MEMBER( pia1_b_in );
	DECLARE_READ8_MEMBER( videoram_1_r );
	DECLARE_READ8_MEMBER( videoram_2_r );
	DECLARE_WRITE_LINE_MEMBER( ptm_o2_callback );
	DECLARE_WRITE_LINE_MEMBER( ptm_o3_callback );
	DECLARE_WRITE8_MEMBER( baud_rate_w );
	TIMER_CALLBACK_MEMBER( set_protect );
	DECLARE_WRITE8_MEMBER( set_protect_w );
	DECLARE_READ8_MEMBER( select_map_r );
	DECLARE_WRITE8_MEMBER( select_map1_w );
	DECLARE_WRITE8_MEMBER( select_map2_w );
	DECLARE_READ8_MEMBER( network_r );
	DECLARE_WRITE8_MEMBER( network_w );
	DECLARE_WRITE_LINE_MEMBER( network_clk_w );

	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	void poly_mem(address_map &map);

	virtual void machine_start() override;
	virtual void machine_reset() override;

	required_device<cpu_device> m_maincpu;
	required_device<address_map_bank_device> m_bankdev;
	required_device<ram_device> m_ram;
	required_device_array<saa5050_device, 2> m_trom;
	required_device_array<pia6821_device, 2> m_pia;
	required_device<mc6854_device> m_adlc;
	required_device<ptm6840_device> m_ptm;
	required_device<input_merger_device> m_irqs;
	//required_device<kr2376_device> m_kr2376;
	required_ioport_array<8> m_kbd;
	required_ioport m_modifiers;
	required_device<speaker_sound_device> m_speaker;
	required_memory_region m_user;
	required_memory_region m_system;
	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_dat;
	optional_device<acia6850_device> m_acia;
	optional_device<clock_device> m_acia_clock;
	uint8_t m_video_pa, m_video_pb;
	uint8_t m_term_data;

	inline offs_t physical(offs_t offset);

	int m_dat_bank;
};


class polydev_state : public poly_state
{
public:
	polydev_state(const machine_config &mconfig, device_type type, const char *tag)
		: poly_state(mconfig, type, tag)
		, m_fdc(*this, "fdc")
		, m_floppy(*this, "fdc:%u", 0)
		, m_current_floppy(nullptr)
	{
	}

	static constexpr feature_type imperfect_features() { return feature::KEYBOARD; }

	void polydev(machine_config &config);

private:
	DECLARE_WRITE8_MEMBER(drive_register_w);
	DECLARE_READ8_MEMBER(drive_register_r);
	DECLARE_WRITE_LINE_MEMBER(motor_w);
	DECLARE_READ8_MEMBER(fdc_inv_r);
	DECLARE_WRITE8_MEMBER(fdc_inv_w);


	virtual void poly_bank(address_map &map) override;

	DECLARE_FLOPPY_FORMATS(floppy_formats);

	required_device<fd1771_device> m_fdc;
	required_device_array<floppy_connector, 2> m_floppy;
	floppy_image_device *m_current_floppy;
};

#endif // MAME_INCLUDES_POLY_H
