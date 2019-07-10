// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_INCLUDES_PGM2_H
#define MAME_INCLUDES_PGM2_H

#pragma once

#include "cpu/arm7/arm7.h"
#include "cpu/arm7/arm7core.h"
#include "sound/ymz770.h"
#include "machine/igs036crypt.h"
#include "screen.h"
#include "speaker.h"
#include "machine/nvram.h"
#include "machine/timer.h"
#include "machine/atmel_arm_aic.h"
#include "machine/pgm2_memcard.h"
#include "emupal.h"

struct kov3_module_key
{
	u8 key[8];
	u8 sum[8];
	u32 addr_xor; // 22bit
	u16 data_xor;
};

class pgm2_state : public driver_device
{
public:
	pgm2_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_screen(*this, "screen"),
		m_lineram(*this, "lineram"),
		m_sp_zoom(*this, "sp_zoom"),
		m_mainram(*this, "mainram"),
		m_romboard_ram(*this, "romboard_ram"),
		m_fg_videoram(*this, "fg_videoram"),
		m_bg_videoram(*this, "bg_videoram"),
		m_sp_videoram(*this, "sp_videoram"),
		m_bgscroll(*this, "bgscroll"),
		m_fgscroll(*this, "fgscroll"),
		m_vidmode(*this, "vidmode"),
		m_gfxdecode2(*this, "gfxdecode2"),
		m_gfxdecode3(*this, "gfxdecode3"),
		m_arm_aic(*this, "arm_aic"),
		m_sprites_mask(*this, "sprites_mask"),
		m_sprites_colour(*this, "sprites_colour"),
		m_sp_palette(*this, "sp_palette"),
		m_bg_palette(*this, "bg_palette"),
		m_tx_palette(*this, "tx_palette"),
		m_mcu_timer(*this, "mcu_timer"),
		m_memcard(*this, "memcard_p%u", 1U),
		m_mainrom(*this, "mainrom")
	{ }

	void init_kov2nl();
	void init_orleg2();
	void init_ddpdojt();
	void init_kov3();
	void init_kov3_104();
	void init_kov3_102();
	void init_kov3_101();
	void init_kov3_100();
	void init_kof98umh();

	void pgm2_ramrom(machine_config &config);
	void pgm2_lores(machine_config &config);
	void pgm2(machine_config &config);
	void pgm2_hires(machine_config &config);
	void pgm2_map(address_map &map);
	void pgm2_module_rom_map(address_map &map);
	void pgm2_ram_rom_map(address_map &map);
	void pgm2_rom_map(address_map &map);
private:
	DECLARE_READ32_MEMBER(unk_startup_r);
	DECLARE_READ32_MEMBER(rtc_r);
	DECLARE_READ32_MEMBER(mcu_r);
	DECLARE_WRITE32_MEMBER(fg_videoram_w);
	DECLARE_WRITE32_MEMBER(bg_videoram_w);
	DECLARE_WRITE32_MEMBER(mcu_w);
	DECLARE_WRITE16_MEMBER(share_bank_w);
	DECLARE_READ8_MEMBER(shareram_r);
	DECLARE_WRITE8_MEMBER(shareram_w);
	DECLARE_WRITE16_MEMBER(vbl_ack_w);
	DECLARE_WRITE16_MEMBER(unk30120014_w);

	DECLARE_WRITE32_MEMBER(pio_sodr_w);
	DECLARE_WRITE32_MEMBER(pio_codr_w);
	DECLARE_READ32_MEMBER(pio_pdsr_r);
	DECLARE_WRITE16_MEMBER(module_rom_w);
	DECLARE_READ16_MEMBER(module_rom_r);
	int module_data_r();
	void module_data_w(int state);
	void module_clk_w(int state);

	DECLARE_READ32_MEMBER(orleg2_speedup_r);
	DECLARE_READ32_MEMBER(kov2nl_speedup_r);
	DECLARE_READ32_MEMBER(kof98umh_speedup_r);
	DECLARE_READ32_MEMBER(ddpdojt_speedup_r);
	DECLARE_READ32_MEMBER(ddpdojt_speedup2_r);
	DECLARE_READ32_MEMBER(kov3_speedup_r);

	DECLARE_READ8_MEMBER(encryption_r);
	DECLARE_WRITE8_MEMBER(encryption_w);
	DECLARE_WRITE32_MEMBER(encryption_do_w);
	DECLARE_WRITE32_MEMBER(sprite_encryption_w);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	virtual void device_post_load() override;

	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);

	u32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(screen_vblank);
	DECLARE_WRITE_LINE_MEMBER(irq);

	TIMER_DEVICE_CALLBACK_MEMBER(mcu_interrupt);

	void decrypt_kov3_module(u32 addrxor, u16 dataxor);

	tilemap_t    *m_fg_tilemap;
	tilemap_t    *m_bg_tilemap;

	bitmap_ind16 m_sprite_bitmap;

	void skip_sprite_chunk(u32 &palette_offset, u32 maskdata, bool reverse);
	void draw_sprite_pixel(const rectangle &cliprect, u32 palette_offset, s16 realx, s16 realy, u16 pal);
	void draw_sprite_chunk(const rectangle &cliprect, u32 &palette_offset, s16 x, s16 realy,
			u16 sizex, int xdraw, u16 pal, u32 maskdata, u32 zoomx_bits, u8 repeats, s16 &realxdraw, s8 realdraw_inc, s8 palette_inc);
	void draw_sprite_line(const rectangle &cliprect, u32 &mask_offset, u32 &palette_offset, s16 x, s16 realy,
			bool flipx, bool reverse, u16 sizex, u16 pal, u8 zoomybit, u32 zoomx_bits, u8 xrepeats);
	void draw_sprites(const rectangle &cliprect);
	void copy_sprites_from_bitmap(bitmap_rgb32 &bitmap, const rectangle &cliprect, u16 pri);

	void common_encryption_init();
	u8 m_encryption_table[0x100];
	bool m_has_decrypted;    // so we only do it once.
	bool m_has_decrypted_kov3_module;
	u32 m_spritekey;
	u32 m_realspritekey;
	bool m_sprite_predecrypted;

	u8 m_shareram[0x100];
	u16 m_share_bank;
	u32 m_mcu_regs[8];
	u32 m_mcu_result0;
	u32 m_mcu_result1;
	u8 m_mcu_last_cmd;
	void mcu_command(bool is_command);

	std::vector<u8> m_encrypted_copy;

	u32 m_pio_out_data;
	const kov3_module_key *module_key;
	bool module_sum_read;
	u32 module_in_latch;
	u32 module_out_latch;
	int module_prev_state;
	int module_clk_cnt;
	u8 module_rcv_buf[10];
	u8 module_send_buf[9];

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<screen_device> m_screen;
	required_shared_ptr<u32> m_lineram;
	required_shared_ptr<u32> m_sp_zoom;
	required_shared_ptr<u32> m_mainram;
	optional_shared_ptr<u32> m_romboard_ram;
	required_shared_ptr<u32> m_fg_videoram;
	required_shared_ptr<u32> m_bg_videoram;
	required_shared_ptr<u32> m_sp_videoram;
	required_shared_ptr<u32> m_bgscroll;
	required_shared_ptr<u32> m_fgscroll;
	required_shared_ptr<u32> m_vidmode;
	required_device<gfxdecode_device> m_gfxdecode2;
	required_device<gfxdecode_device> m_gfxdecode3;
	required_device<arm_aic_device> m_arm_aic;
	required_region_ptr<u8> m_sprites_mask;
	required_region_ptr<u8> m_sprites_colour;
	required_device<palette_device> m_sp_palette;
	required_device<palette_device> m_bg_palette;
	required_device<palette_device> m_tx_palette;
	required_device<timer_device> m_mcu_timer;

	optional_device_array<pgm2_memcard_device, 4> m_memcard;

	required_memory_region m_mainrom;
};

#endif
