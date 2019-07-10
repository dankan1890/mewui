// license:BSD-3-Clause
// copyright-holders:Luca Elia
#ifndef MAME_INCLUDES_FANTLAND_H
#define MAME_INCLUDES_FANTLAND_H

#pragma once

#include "machine/gen_latch.h"
#include "sound/msm5205.h"
#include "emupal.h"
#include "screen.h"

class fantland_state : public driver_device
{
public:
	fantland_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),
		m_soundlatch(*this, "soundlatch"),
		m_spriteram(*this, "spriteram", 0),
		m_spriteram2(*this, "spriteram2", 0)
	{ }

	void fantland(machine_config &config);
	void wheelrun(machine_config &config);
	void galaxygn(machine_config &config);

	DECLARE_CUSTOM_INPUT_MEMBER(wheelrun_wheel_r);

protected:
	/* misc */
	uint8_t    m_nmi_enable;

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device<generic_latch_8_device> m_soundlatch;

	/* memory pointers */
	required_shared_ptr<uint8_t> m_spriteram;
	required_shared_ptr<uint8_t> m_spriteram2;

	DECLARE_WRITE8_MEMBER(nmi_enable_w);
	DECLARE_WRITE8_MEMBER(soundlatch_w);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(vblank_irq);
	void draw_sprites(bitmap_ind16 &bitmap,const rectangle &cliprect);

private:
	DECLARE_READ8_MEMBER(spriteram_r);
	DECLARE_READ8_MEMBER(spriteram2_r);
	DECLARE_WRITE8_MEMBER(spriteram_w);
	DECLARE_WRITE8_MEMBER(spriteram2_w);
	DECLARE_WRITE_LINE_MEMBER(galaxygn_sound_irq);
	INTERRUPT_GEN_MEMBER(fantland_sound_irq);
	void fantland_map(address_map &map);
	void fantland_sound_iomap(address_map &map);
	void fantland_sound_map(address_map &map);
	void galaxygn_map(address_map &map);
	void galaxygn_sound_iomap(address_map &map);
	void wheelrun_map(address_map &map);
	void wheelrun_sound_map(address_map &map);
};

class borntofi_state : public fantland_state
{
public:
	borntofi_state(const machine_config &mconfig, device_type type, const char *tag) :
		fantland_state(mconfig, type, tag),
		m_msm(*this, "msm%u", 1U),
		m_adpcm_rom(*this, "adpcm")
	{
	}

	void borntofi(machine_config &config);

private:
	/* misc */
	int        m_old_x[2];
	int        m_old_y[2];
	int        m_old_f[2];
	uint8_t    m_input_ret[2];
	int        m_adpcm_playing[4];
	int        m_adpcm_addr[2][4];
	int        m_adpcm_nibble[4];

	/* devices */
	required_device_array<msm5205_device, 4> m_msm;
	required_region_ptr<uint8_t> m_adpcm_rom;

	DECLARE_READ8_MEMBER(inputs_r);
	DECLARE_WRITE8_MEMBER(msm5205_w);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	template<int Voice> DECLARE_WRITE_LINE_MEMBER(adpcm_int);
	void adpcm_start(int voice);
	void adpcm_stop(int voice);
	void main_map(address_map &map);
	void sound_map(address_map &map);
};

#endif // MAME_INCLUDES_FANTLAND_H
