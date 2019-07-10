// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/*************************************************************************

    Irem M72 hardware

*************************************************************************/
#ifndef MAME_INCLUDES_M72_H
#define MAME_INCLUDES_M72_H

#pragma once

#include "audio/m72.h"
#include "cpu/mcs51/mcs51.h"
#include "machine/mb8421.h"
#include "machine/pic8259.h"
#include "machine/upd4701.h"
#include "video/bufsprite.h"
#include "sound/dac.h"
#include "emupal.h"
#include "screen.h"

#define M81_B_B_JUMPER_J3_S \
	PORT_START("JumperJ3") \
	PORT_CONFNAME( 0x0001, 0x0000, "M81-B-B Jumper J3" ) \
	PORT_CONFSETTING(      0x0000, "S" ) \
	/* PORT_CONFSETTING(      0x0001, "W" ) */

#define M81_B_B_JUMPER_J3_W \
	PORT_START("JumperJ3") \
	PORT_CONFNAME( 0x0001, 0x0001, "M81-B-B Jumper J3" ) \
	/* PORT_CONFSETTING(      0x0000, "S" ) */ \
	PORT_CONFSETTING(      0x0001, "W" )

class m72_state : public driver_device
{
public:
	m72_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_soundcpu(*this, "soundcpu"),
		m_mcu(*this, "mcu"),
		m_dpram(*this, "dpram"),
		m_dac(*this, "dac"),
		m_audio(*this, "m72"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),
		m_spriteram(*this, "spriteram"),
		m_videoram(*this, "videoram%u", 1U),
		m_m82_rowscrollram(*this, "majtitle_rowscr"),
		m_spriteram2(*this, "spriteram2"),
		m_soundram(*this, "soundram"),
		m_paletteram(*this, "paletteram%u", 1U),
		m_upd71059c(*this, "upd71059c"),
		m_upd4701(*this, {"upd4701l", "upd4701h"}),
		m_samples_region(*this, "samples"),
		m_io_dsw(*this, "DSW"),
		m_fg_source(0),
		m_bg_source(0),
		m_m81_b_b_j3(*this, "JumperJ3"),
		m_m82_rowscroll(0),
		m_m82_tmcontrol(0)
	{ }

	void m72_base(machine_config &config);
	void m72_audio_chips(machine_config &config);
	void m72_xmultipl(machine_config &config);
	void m72_dbreed(machine_config &config);
	void cosmccop(machine_config &config);
	void poundfor(machine_config &config);
	void m72(machine_config &config);
	void m81_hharry(machine_config &config);
	void m81_xmultipl(machine_config &config);
	void kengo(machine_config &config);
	void m81_dbreed(machine_config &config);
	void m72_8751(machine_config &config);
	void hharryu(machine_config &config);
	void rtype2(machine_config &config);
	void m82(machine_config &config);
	void rtype(machine_config &config);
	void imgfightb(machine_config &config);
	void lohtb(machine_config &config);

	void init_dkgenm72();
	void init_bchopper();
	void init_gallop();
	void init_m72_8751();
	void init_dbreedm72();
	void init_airduelm72();
	void init_nspirit();
	void init_loht();
	void init_imgfight();

private:
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_soundcpu;
	optional_device<cpu_device> m_mcu;
	optional_device<mb8421_mb8431_16_device> m_dpram;
	optional_device<dac_byte_interface> m_dac;
	optional_device<m72_audio_device> m_audio;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device<buffered_spriteram16_device> m_spriteram;

	required_shared_ptr_array<u16, 2> m_videoram;
	optional_shared_ptr<u16> m_m82_rowscrollram;
	optional_shared_ptr<u16> m_spriteram2;
	optional_shared_ptr<u8> m_soundram;
	required_shared_ptr_array<u16, 2> m_paletteram;

	optional_device<pic8259_device> m_upd71059c;
	optional_device_array<upd4701_device, 2> m_upd4701;

	optional_region_ptr<u8> m_samples_region;

	optional_ioport m_io_dsw;

	std::unique_ptr<u16[]> m_protection_ram;
	emu_timer *m_scanline_timer;
	const u8 *m_protection_code;
	const u8 *m_protection_crc;
	u32 m_raster_irq_position;
	tilemap_t *m_fg_tilemap;
	tilemap_t *m_bg_tilemap;
	tilemap_t *m_bg_tilemap_large;
	s32 m_scrollx[2];
	s32 m_scrolly[2];
	s32 m_video_off;

	int m_fg_source;
	int m_bg_source;
	optional_ioport m_m81_b_b_j3;

	// majtitle specific
	int m_m82_rowscroll;
	u16 m_m82_tmcontrol;

	// m72_i8751 specific
	u8 m_mcu_sample_latch;
	u32 m_mcu_sample_addr;

	// common
	template<unsigned N> u16 palette_r(offs_t offset);
	template<unsigned N> void palette_w(offs_t offset, u16 data, u16 mem_mask);
	void videoram1_w(offs_t offset, u16 data, u16 mem_mask);
	void videoram2_w(offs_t offset, u16 data, u16 mem_mask);
	void irq_line_w(u16 data);
	template<unsigned N> void scrollx_w(offs_t offset, u16 data, u16 mem_mask);
	template<unsigned N> void scrolly_w(offs_t offset, u16 data, u16 mem_mask);
	void dmaon_w(u8 data);
	void port02_w(u8 data);
	u8 soundram_r(offs_t offset);
	void soundram_w(offs_t offset, u8 data);

	// m72_i8751 specific
	DECLARE_WRITE16_MEMBER(main_mcu_sound_w);
	void main_mcu_w(offs_t offset, u16 data, u16 mem_mask);
	void mcu_data_w(offs_t offset, u8 data);
	u8 mcu_data_r(offs_t offset);
	u8 mcu_sample_r();
	void mcu_port1_w(u8 data);
	void mcu_low_w(u8 data);
	void mcu_high_w(u8 data);
	u8 snd_cpu_sample_r();

	DECLARE_READ16_MEMBER(protection_r);
	DECLARE_WRITE16_MEMBER(protection_w);

	// game specific
	DECLARE_WRITE16_MEMBER(bchopper_sample_trigger_w);
	DECLARE_WRITE16_MEMBER(nspirit_sample_trigger_w);
	DECLARE_WRITE16_MEMBER(imgfight_sample_trigger_w);
	DECLARE_WRITE16_MEMBER(loht_sample_trigger_w);
	DECLARE_WRITE16_MEMBER(dbreedm72_sample_trigger_w);
	DECLARE_WRITE16_MEMBER(airduelm72_sample_trigger_w);
	DECLARE_WRITE16_MEMBER(dkgenm72_sample_trigger_w);
	DECLARE_WRITE16_MEMBER(gallop_sample_trigger_w);
	void rtype2_port02_w(u8 data);
	void poundfor_port02_w(u8 data);
	void m82_gfx_ctrl_w(offs_t offset, u16 data, u16 mem_mask);
	void m82_tm_ctrl_w(offs_t offset, u16 data, u16 mem_mask);

	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);

	template<unsigned N> TILE_GET_INFO_MEMBER(rtype2_get_tile_info);

	TILEMAP_MAPPER_MEMBER(m82_scan_rows);

	void machine_start() override;
	void machine_reset() override;
	DECLARE_VIDEO_START(m72);
	DECLARE_VIDEO_START(xmultipl);
	DECLARE_VIDEO_START(hharry);
	DECLARE_VIDEO_START(rtype2);
	DECLARE_VIDEO_START(m82);
	DECLARE_VIDEO_START(hharryu);
	DECLARE_VIDEO_START(poundfor);
	DECLARE_MACHINE_START(kengo);
	DECLARE_MACHINE_RESET(kengo);

	INTERRUPT_GEN_MEMBER(fake_nmi);
	TIMER_CALLBACK_MEMBER(synch_callback);
	TIMER_CALLBACK_MEMBER(scanline_interrupt);
	TIMER_CALLBACK_MEMBER(kengo_scanline_interrupt);
	TIMER_CALLBACK_MEMBER(delayed_ram16_w);
	TIMER_CALLBACK_MEMBER(delayed_ram8_w);

	u32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_m81(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_m82(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	inline void m72_m81_get_tile_info(tile_data &tileinfo,int tile_index,const u16 *vram,int gfxnum);
	void register_savestate();
	inline void changecolor(offs_t color, u8 r, u8 g, u8 b);
	void draw_sprites(bitmap_ind16 &bitmap,const rectangle &cliprect);
	void majtitle_draw_sprites(bitmap_ind16 &bitmap,const rectangle &cliprect);
	void copy_le(u16 *dest, const u8 *src, u8 bytes);
	void install_protection_handler(const u8 *code,const u8 *crc);

	void dbreed_map(address_map &map);
	void dbreedm72_map(address_map &map);
	void hharry_map(address_map &map);
	void hharryu_map(address_map &map);
	void kengo_map(address_map &map);
	void lohtb_map(address_map &map);
	void lohtb_portmap(address_map &map);
	void m72_cpu1_common_map(address_map &map);
	void m72_map(address_map &map);
	void m72_protected_map(address_map &map);
	void m72_portmap(address_map &map);
	void m72_protected_portmap(address_map &map);
	void m81_cpu1_common_map(address_map &map);
	void m81_portmap(address_map &map);
	void m82_map(address_map &map);
	void m82_portmap(address_map &map);
	void m84_cpu1_common_map(address_map &map);
	void m84_portmap(address_map &map);
	void m84_v33_portmap(address_map &map);
	void i80c31_mem_map(address_map &map);
	void mcu_io_map(address_map &map);
	void poundfor_portmap(address_map &map);
	void poundfor_sound_portmap(address_map &map);
	void rtype2_map(address_map &map);
	void rtype2_sound_portmap(address_map &map);
	void rtype_map(address_map &map);
	void rtype_sound_portmap(address_map &map);
	void sound_portmap(address_map &map);
	void sound_protected_portmap(address_map &map);
	void sound_ram_map(address_map &map);
	void sound_rom_map(address_map &map);
	void xmultipl_map(address_map &map);
	void xmultiplm72_map(address_map &map);
};

#endif // MAME_INCLUDES_M72_H
