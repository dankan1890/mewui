// license:BSD-3-Clause
// copyright-holders:Mirko Buffoni
#ifndef MAME_INCLUDES_MARIO_H
#define MAME_INCLUDES_MARIO_H

#pragma once

#include "machine/gen_latch.h"
#include "machine/z80dma.h"
#include "emupal.h"

#define OLD_SOUND   (0)

#if !OLD_SOUND
#include "machine/netlist.h"
#include "netlist/devices/net_lib.h"
#else
#include "sound/discrete.h"
#endif

/*
 * From the schematics:
 *
 * Video generation like dkong/dkongjr. However, clock is 24MHZ
 * 7C -> 100 => 256 - 124 = 132 ==> 264 Scanlines
 */

#define MASTER_CLOCK            XTAL(24'000'000)
#define PIXEL_CLOCK             (MASTER_CLOCK / 4)
#define CLOCK_1H                (MASTER_CLOCK / 8)
#define CLOCK_16H               (CLOCK_1H / 16)
#define CLOCK_1VF               ((CLOCK_16H) / 12 / 2)
#define CLOCK_2VF               ((CLOCK_1VF) / 2)

#define HTOTAL                  (384)
#define HBSTART                 (256)
#define HBEND                   (0)
#define VTOTAL                  (264)
#define VBSTART                 (240)
#define VBEND                   (16)

#define Z80_MASTER_CLOCK        XTAL(8'000'000)
#define Z80_CLOCK               (Z80_MASTER_CLOCK / 2) /* verified on pcb */

#define I8035_MASTER_CLOCK      XTAL(11'000'000) /* verified on pcb: 730Khz */
#define I8035_CLOCK             (I8035_MASTER_CLOCK)

class mario_state : public driver_device
{
public:
	mario_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_z80dma(*this, "z80dma"),
		m_soundlatch(*this, "soundlatch"),
		m_soundlatch2(*this, "soundlatch2"),
		m_soundlatch3(*this, "soundlatch3"),
		m_soundlatch4(*this, "soundlatch4"),
#if OLD_SOUND
		m_discrete(*this, "discrete"),
#else
		m_audio_snd0(*this, "snd_nl:snd0"),
		m_audio_snd1(*this, "snd_nl:snd1"),
		m_audio_snd7(*this, "snd_nl:snd7"),
		m_audio_dac(*this, "snd_nl:dac"),
#endif
		m_spriteram(*this, "spriteram"),
		m_videoram(*this, "videoram"),
		m_monitor(0)
	{ }

	void mario_base(machine_config &config);
	void masao(machine_config &config);
	void masao_audio(machine_config &config);
	void mario(machine_config &config);
	void mario_audio(machine_config &config);

private:
	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<z80dma_device> m_z80dma;
	optional_device<generic_latch_8_device> m_soundlatch;
	optional_device<generic_latch_8_device> m_soundlatch2;
	optional_device<generic_latch_8_device> m_soundlatch3;
	optional_device<generic_latch_8_device> m_soundlatch4;
#if OLD_SOUND
	optional_device<discrete_sound_device> m_discrete;
#else
	optional_device<netlist_mame_logic_input_device> m_audio_snd0;
	optional_device<netlist_mame_logic_input_device> m_audio_snd1;
	optional_device<netlist_mame_logic_input_device> m_audio_snd7;
	optional_device<netlist_mame_int_input_device> m_audio_dac;
#endif

	/* memory pointers */
	required_shared_ptr<uint8_t> m_spriteram;
	required_shared_ptr<uint8_t> m_videoram;

	/* sound state */
	uint8_t   m_last;
	uint8_t   m_portT;
	const char *m_eabank;

	/* video state */
	uint8_t   m_gfx_bank;
	uint8_t   m_palette_bank;
	uint16_t  m_gfx_scroll;
	uint8_t   m_flip;
	tilemap_t *m_bg_tilemap;
	int m_monitor;

	bool      m_nmi_mask;
	DECLARE_WRITE_LINE_MEMBER(nmi_mask_w);
	DECLARE_WRITE_LINE_MEMBER(coin_counter_1_w);
	DECLARE_WRITE_LINE_MEMBER(coin_counter_2_w);
	DECLARE_WRITE8_MEMBER(mario_videoram_w);
	DECLARE_WRITE_LINE_MEMBER(gfx_bank_w);
	DECLARE_WRITE_LINE_MEMBER(palette_bank_w);
	DECLARE_WRITE8_MEMBER(mario_scroll_w);
	DECLARE_WRITE_LINE_MEMBER(flip_w);
	DECLARE_READ8_MEMBER(mario_sh_p1_r);
	DECLARE_READ8_MEMBER(mario_sh_p2_r);
	DECLARE_READ_LINE_MEMBER(mario_sh_t0_r);
	DECLARE_READ_LINE_MEMBER(mario_sh_t1_r);
	DECLARE_READ8_MEMBER(mario_sh_tune_r);
	DECLARE_WRITE8_MEMBER(mario_sh_p1_w);
	DECLARE_WRITE8_MEMBER(mario_sh_p2_w);
	DECLARE_WRITE8_MEMBER(masao_sh_irqtrigger_w);
	DECLARE_WRITE8_MEMBER(mario_sh_tuneselect_w);
	DECLARE_WRITE8_MEMBER(mario_sh3_w);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	virtual void video_start() override;
	virtual void sound_start() override;
	virtual void sound_reset() override;
	void mario_palette(palette_device &palette) const;
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(vblank_irq);
	DECLARE_WRITE8_MEMBER(mario_sh_sound_w);
	DECLARE_WRITE8_MEMBER(mario_sh1_w);
	DECLARE_WRITE8_MEMBER(mario_sh2_w);
	DECLARE_READ8_MEMBER(memory_read_byte);
	DECLARE_WRITE8_MEMBER(memory_write_byte);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void set_ea(int ea);
	void mario_io_map(address_map &map);
	void mario_map(address_map &map);
	void mario_sound_io_map(address_map &map);
	void mario_sound_map(address_map &map);
	void masao_map(address_map &map);
	void masao_sound_map(address_map &map);
};

#endif // MAME_INCLUDES_MARIO_H
