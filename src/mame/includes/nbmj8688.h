// license:BSD-3-Clause
// copyright-holders:Takahiro Nogi
#ifndef MAME_INCLUDES_NBMJ8688_H
#define MAME_INCLUDES_NBMJ8688_H

#pragma once

#include "video/hd61830.h"
#include "machine/nb1413m3.h"
#include "emupal.h"

class nbmj8688_state : public driver_device
{
public:
	enum
	{
		TIMER_BLITTER
	};

	nbmj8688_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_nb1413m3(*this, "nb1413m3"),
		m_lcdc0(*this, "lcdc0"),
		m_lcdc1(*this, "lcdc1")
	{ }

	void NBMJDRV_4096(machine_config &config);
	void NBMJDRV_256(machine_config &config);
	void NBMJDRV_65536(machine_config &config);
	void mbmj_h12bit(machine_config &config);
	void mbmj_p12bit(machine_config &config);
	void mbmj_p16bit(machine_config &config);
	void mbmj_p16bit_LCD(machine_config &config);
	void swinggal(machine_config &config);
	void korinai(machine_config &config);
	void livegal(machine_config &config);
	void apparel(machine_config &config);
	void kyuhito(machine_config &config);
	void bijokkoy(machine_config &config);
	void barline(machine_config &config);
	void bijokkog(machine_config &config);
	void korinaim(machine_config &config);
	void ryuuha(machine_config &config);
	void seiham(machine_config &config);
	void orangeci(machine_config &config);
	void citylove(machine_config &config);
	void otonano(machine_config &config);
	void ojousanm(machine_config &config);
	void mcitylov(machine_config &config);
	void iemotom(machine_config &config);
	void crystalg(machine_config &config);
	void crystal2(machine_config &config);
	void secolove(machine_config &config);
	void orangec(machine_config &config);
	void mjsikaku(machine_config &config);
	void housemn2(machine_config &config);
	void kanatuen(machine_config &config);
	void nightlov(machine_config &config);
	void kaguya2(machine_config &config);
	void mjgaiden(machine_config &config);
	void mjcamera(machine_config &config);
	void mmsikaku(machine_config &config);
	void housemnq(machine_config &config);
	void idhimitu(machine_config &config);
	void iemoto(machine_config &config);
	void kaguya(machine_config &config);
	void vipclub(machine_config &config);
	void ojousan(machine_config &config);
	void seiha(machine_config &config);
	void bikkuri(machine_config &config);

	void init_kyuhito();
	void init_idhimitu();
	void init_kaguya2();
	void init_mjcamera();
	void init_kanatuen();

private:
	required_device<cpu_device> m_maincpu;
	required_device<nb1413m3_device> m_nb1413m3;
	optional_device<hd61830_device> m_lcdc0;
	optional_device<hd61830_device> m_lcdc1;

	// defined in video_start
	int m_gfxmode;

	int m_scrolly;
	int m_blitter_destx;
	int m_blitter_desty;
	int m_blitter_sizex;
	int m_blitter_sizey;
	int m_blitter_direction_x;
	int m_blitter_direction_y;
	int m_blitter_src_addr;
	int m_gfxrom;
	int m_dispflag;
	int m_gfxflag2;
	int m_gfxflag3;
	int m_flipscreen;
	int m_screen_refresh;
	std::unique_ptr<bitmap_ind16> m_tmpbitmap;
	std::unique_ptr<uint16_t[]> m_videoram;
	std::unique_ptr<uint8_t[]> m_clut;
	int m_flipscreen_old;
	emu_timer *m_blitter_timer;

	// common
	DECLARE_READ8_MEMBER(ff_r);
	DECLARE_WRITE8_MEMBER(clut_w);
	DECLARE_WRITE8_MEMBER(blitter_w);
	DECLARE_WRITE8_MEMBER(scrolly_w);

	DECLARE_WRITE8_MEMBER(mjsikaku_gfxflag2_w);
	DECLARE_WRITE8_MEMBER(mjsikaku_gfxflag3_w);
	DECLARE_WRITE8_MEMBER(mjsikaku_romsel_w);
	DECLARE_WRITE8_MEMBER(secolove_romsel_w);
	DECLARE_WRITE8_MEMBER(crystalg_romsel_w);
	DECLARE_WRITE8_MEMBER(seiha_romsel_w);
	DECLARE_WRITE8_MEMBER(HD61830B_both_instr_w);
	DECLARE_WRITE8_MEMBER(HD61830B_both_data_w);
	DECLARE_READ8_MEMBER(dipsw1_r);
	DECLARE_READ8_MEMBER(dipsw2_r);
	DECLARE_WRITE8_MEMBER(barline_output_w);

	DECLARE_VIDEO_START(mbmj8688_pure_12bit);
	void mbmj8688_12bit(palette_device &palette) const;
	DECLARE_VIDEO_START(mbmj8688_pure_16bit_LCD);
	void mbmj8688_16bit(palette_device &palette) const;
	void mbmj8688_lcd(palette_device &palette) const;
	DECLARE_VIDEO_START(mbmj8688_8bit);
	void mbmj8688_8bit(palette_device &palette) const;
	DECLARE_VIDEO_START(mbmj8688_hybrid_16bit);
	DECLARE_VIDEO_START(mbmj8688_hybrid_12bit);
	DECLARE_VIDEO_START(mbmj8688_pure_16bit);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void vramflip();
	void update_pixel(int x, int y);
	void writeram_low(int x, int y, int color);
	void writeram_high(int x, int y, int color);
	void gfxdraw(int gfxtype);
	void common_video_start();
	void postload();

	void barline_io_map(address_map &map);
	void bikkuri_map(address_map &map);
	void bikkuri_io_map(address_map &map);
	void crystalg_io_map(address_map &map);
	void iemoto_io_map(address_map &map);
	void kaguya_io_map(address_map &map);
	void mjgaiden_io_map(address_map &map);
	void mjsikaku_io_map(address_map &map);
	void mjsikaku_map(address_map &map);
	void mmsikaku_io_map(address_map &map);
	void ojousan_map(address_map &map);
	void otonano_io_map(address_map &map);
	void p16bit_LCD_io_map(address_map &map);
	void secolove_io_map(address_map &map);
	void secolove_map(address_map &map);
	void seiha_io_map(address_map &map);

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
};

#endif // MAME_INCLUDES_NBMJ8688_H
