// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Midway MCR system

**************************************************************************/
#ifndef MAME_INCLUDES_MCR_H
#define MAME_INCLUDES_MCR_H

#pragma once

#include "cpu/z80/z80.h"
#include "machine/z80daisy.h"
#include "machine/timer.h"
#include "machine/z80ctc.h"
#include "machine/z80pio.h"
#include "machine/z80dart.h"
#include "machine/watchdog.h"
#include "audio/bally.h"
#include "audio/midway.h"
#include "audio/csd.h"
#include "sound/samples.h"
#include "emupal.h"

/* constants */
#define MAIN_OSC_MCR_I      XTAL(19'968'000)


class mcr_state : public driver_device
{
public:
	mcr_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_spriteram(*this, "spriteram"),
		m_videoram(*this, "videoram"),
		m_paletteram(*this, "paletteram"),
		m_ctc(*this, "ctc"),
		m_ssio(*this, "ssio"),
		m_cheap_squeak_deluxe(*this, "csd"),
		m_sounds_good(*this, "sg"),
		m_turbo_cheap_squeak(*this, "tcs"),
		m_squawk_n_talk(*this, "snt"),
		m_samples(*this, "samples"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette")
	{ }

	DECLARE_WRITE8_MEMBER(mcr_control_port_w);
	DECLARE_WRITE8_MEMBER(mcr_paletteram9_w);
	DECLARE_WRITE8_MEMBER(mcr_90009_videoram_w);
	DECLARE_WRITE8_MEMBER(mcr_90010_videoram_w);
	DECLARE_READ8_MEMBER(twotiger_videoram_r);
	DECLARE_WRITE8_MEMBER(twotiger_videoram_w);
	DECLARE_WRITE8_MEMBER(mcr_91490_videoram_w);
	DECLARE_READ8_MEMBER(solarfox_ip0_r);
	DECLARE_READ8_MEMBER(solarfox_ip1_r);
	DECLARE_READ8_MEMBER(kick_ip1_r);
	DECLARE_WRITE8_MEMBER(wacko_op4_w);
	DECLARE_READ8_MEMBER(wacko_ip1_r);
	DECLARE_READ8_MEMBER(wacko_ip2_r);
	DECLARE_READ8_MEMBER(kroozr_ip1_r);
	DECLARE_WRITE8_MEMBER(kroozr_op4_w);
	DECLARE_WRITE8_MEMBER(journey_op4_w);
	DECLARE_WRITE8_MEMBER(twotiger_op4_w);
	DECLARE_WRITE8_MEMBER(dotron_op4_w);
	DECLARE_READ8_MEMBER(demoderb_ip1_r);
	DECLARE_READ8_MEMBER(demoderb_ip2_r);
	DECLARE_WRITE8_MEMBER(demoderb_op4_w);

	void init_mcr_91490();
	void init_kroozr();
	void init_solarfox();
	void init_kick();
	void init_twotiger();
	void init_demoderb();
	void init_wacko();
	void init_mcr_90010();
	void init_dotrone();
	void init_journey();

	uint32_t screen_update_mcr(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_DEVICE_CALLBACK_MEMBER(mcr_interrupt);

	void mcr_91490_tcs(machine_config &config);
	void mcr_91490_snt(machine_config &config);
	void mcr_91490(machine_config &config);
	void mcr_90009(machine_config &config);
	void mcr_90010_tt(machine_config &config);
	void mcr_91475(machine_config &config);
	void mcr_90010(machine_config &config);
	void cpu_90009_map(address_map &map);
	void cpu_90009_portmap(address_map &map);
	void cpu_90010_map(address_map &map);
	void cpu_90010_portmap(address_map &map);
	void cpu_91490_map(address_map &map);
	void cpu_91490_portmap(address_map &map);
protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

	TILE_GET_INFO_MEMBER(mcr_90009_get_tile_info);
	TILE_GET_INFO_MEMBER(mcr_90010_get_tile_info);
	TILE_GET_INFO_MEMBER(mcr_91490_get_tile_info);
	void mcr_set_color(int index, int data);
	void journey_set_color(int index, int data);
	void render_sprites_91399(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void render_sprites_91464(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int primask, int sprmask, int colormask);
	void mcr_init(int cpuboard, int vidboard, int ssioboard);

	int8_t m_mcr12_sprite_xoffs_flip;
	uint8_t m_input_mux;
	uint8_t m_last_op4;
	tilemap_t *m_bg_tilemap;

	uint8_t m_mcr_cocktail_flip;

	required_device<z80_device> m_maincpu;
	optional_shared_ptr<uint8_t> m_spriteram;
	optional_shared_ptr<uint8_t> m_videoram;
	optional_shared_ptr<uint8_t> m_paletteram;

	required_device<z80ctc_device> m_ctc;
	optional_device<midway_ssio_device> m_ssio;
	optional_device<midway_cheap_squeak_deluxe_device> m_cheap_squeak_deluxe;
	optional_device<midway_sounds_good_device> m_sounds_good;
	optional_device<midway_turbo_cheap_squeak_device> m_turbo_cheap_squeak;
	optional_device<bally_squawk_n_talk_device> m_squawk_n_talk;
	optional_device<samples_device> m_samples;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;

private:
	uint32_t m_mcr_cpu_board;
	uint32_t m_mcr_sprite_board;

	int8_t m_mcr12_sprite_xoffs;
};

class mcr_dpoker_state : public mcr_state
{
public:
	mcr_dpoker_state(const machine_config &mconfig, device_type type, const char *tag) :
		mcr_state(mconfig, type, tag),
		m_coin_in_timer(*this, "coinin"),
		m_hopper_timer(*this, "hopper"),
		m_lamps(*this, "lamp%u", 0U)
	{ }

	DECLARE_READ8_MEMBER(ip0_r);
	DECLARE_WRITE8_MEMBER(lamps1_w);
	DECLARE_WRITE8_MEMBER(lamps2_w);
	DECLARE_WRITE8_MEMBER(output_w);
	DECLARE_WRITE8_MEMBER(meters_w);

	DECLARE_INPUT_CHANGED_MEMBER(coin_in_hit);

	TIMER_DEVICE_CALLBACK_MEMBER(hopper_callback);
	TIMER_DEVICE_CALLBACK_MEMBER(coin_in_callback);

	void init_dpoker();

	void mcr_90009_dp(machine_config &config);

protected:
	virtual void machine_start() override { mcr_state::machine_start(); m_lamps.resolve(); }

private:
	uint8_t m_coin_status;
	uint8_t m_output;

	required_device<timer_device> m_coin_in_timer;
	required_device<timer_device> m_hopper_timer;
	output_finder<14> m_lamps;
};

class mcr_nflfoot_state : public mcr_state
{
public:
	mcr_nflfoot_state(const machine_config &mconfig, device_type type, const char *tag) :
		mcr_state(mconfig, type, tag),
		m_ipu(*this, "ipu"),
		m_ipu_sio(*this, "ipu_sio"),
		m_ipu_ctc(*this, "ipu_ctc"),
		m_ipu_pio0(*this, "ipu_pio0"),
		m_ipu_pio1(*this, "ipu_pio1")
	{ }

	DECLARE_WRITE_LINE_MEMBER(sio_txda_w);
	DECLARE_WRITE_LINE_MEMBER(sio_txdb_w);
	DECLARE_WRITE8_MEMBER(ipu_laserdisk_w);
	DECLARE_READ8_MEMBER(ipu_watchdog_r);
	DECLARE_WRITE8_MEMBER(ipu_watchdog_w);
	DECLARE_READ8_MEMBER(ip2_r);
	DECLARE_WRITE8_MEMBER(op4_w);

	TIMER_CALLBACK_MEMBER(ipu_watchdog_reset);
	TIMER_DEVICE_CALLBACK_MEMBER(ipu_interrupt);

	void init_nflfoot();

	void mcr_91490_ipu(machine_config &config);
	void ipu_91695_map(address_map &map);
	void ipu_91695_portmap(address_map &map);
protected:
	virtual void machine_start() override;

private:
	int m_ipu_sio_txda;
	int m_ipu_sio_txdb;
	emu_timer *m_ipu_watchdog_timer;

	required_device<z80_device> m_ipu;
	required_device<z80dart_device> m_ipu_sio;
	required_device<z80ctc_device> m_ipu_ctc;
	required_device<z80pio_device> m_ipu_pio0;
	required_device<z80pio_device> m_ipu_pio1;
};

/*----------- defined in machine/mcr.cpp -----------*/

extern const z80_daisy_config mcr_daisy_chain[];
extern const z80_daisy_config mcr_ipu_daisy_chain[];

extern const gfx_layout mcr_bg_layout;
extern const gfx_layout mcr_sprite_layout;

#endif // MAME_INCLUDES_MCR_H
