// license:BSD-3-Clause
// copyright-holders:Manuel Abadia
#include "cpu/m68000/m68000.h"
#include "video/bufsprite.h"
#include "machine/74259.h"
#include "machine/eepromser.h"
#include "machine/timer.h"
#include "emupal.h"

class gaelco2_state : public driver_device
{
public:
	gaelco2_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_mainlatch(*this, "mainlatch"),
		m_spriteram(*this, "spriteram"),
		m_eeprom(*this, "eeprom"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_global_spritexoff(0),
		m_vregs(*this, "vregs"),
		m_snowboar_protection(*this, "snowboar_prot"),
		m_paletteram(*this, "paletteram"),
		m_shareram(*this, "shareram")
	{ }

	void maniacsq_d5002fp(machine_config &config);
	void play2000(machine_config &config);
	void alighunt(machine_config &config);
	void touchgo(machine_config &config);
	void alighunt_d5002fp(machine_config &config);
	void snowboar(machine_config &config);
	void maniacsq(machine_config &config);
	void maniacsqs(machine_config &config);
	void touchgo_d5002fp(machine_config &config);
	void saltcrdi(machine_config &config);

	void init_touchgo();
	void init_snowboar();
	void init_snowboara();
	void init_alighunt();
	void init_wrally2();
	void init_play2000();

	DECLARE_WRITE_LINE_MEMBER(coin1_counter_w);
	DECLARE_WRITE_LINE_MEMBER(coin2_counter_w);

	DECLARE_VIDEO_START(gaelco2);
	DECLARE_VIDEO_START(gaelco2_dual);

	u32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_left(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_right(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

protected:
	required_device<m68000_device> m_maincpu;
	optional_device<ls259_device> m_mainlatch;
	required_device<buffered_spriteram16_device> m_spriteram;
	optional_device<eeprom_serial_93cxx_device> m_eeprom;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;

	void vram_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void vregs_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void palette_w(offs_t offset, u16 data, u16 mem_mask = ~0);

	void wrally2_latch_w(offs_t offset, u16 data);

	void mcu_hostmem_map(address_map &map);

private:
	void shareram_w(offs_t offset, u8 data);
	u8 shareram_r(offs_t offset);
	void alighunt_coin_w(u16 data);
	DECLARE_WRITE_LINE_MEMBER(coin3_counter_w);
	DECLARE_WRITE_LINE_MEMBER(coin4_counter_w);
	u16 snowboar_protection_r();
	void snowboar_protection_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	template<unsigned Layer> TILE_GET_INFO_MEMBER(get_tile_info);
	template<unsigned Layer> TILE_GET_INFO_MEMBER(get_tile_info_dual);
	void draw_sprites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int mask);
	u32 dual_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int index);
	void ROM16_split_gfx(const char *src_reg, const char *dst_reg, int start, int length, int dest1, int dest2);

	void alighunt_map(address_map &map);
	void maniacsq_map(address_map &map);
	void play2000_map(address_map &map);
	void snowboar_map(address_map &map);
	void touchgo_map(address_map &map);
	void saltcrdi_map(address_map &map);

	u32 m_snowboard_latch;

	u16 *m_videoram;
	tilemap_t *m_pant[2];
	int m_dual_monitor;
	int m_global_spritexoff;

	required_shared_ptr<u16> m_vregs;
	optional_shared_ptr<u16> m_snowboar_protection;
	required_shared_ptr<u16> m_paletteram;
	optional_shared_ptr<u16> m_shareram;
};


class bang_state : public gaelco2_state
{
public:
	bang_state(const machine_config &mconfig, device_type type, const char *tag)
		: gaelco2_state(mconfig, type, tag)
		, m_light0_x(*this, "LIGHT0_X")
		, m_light0_y(*this, "LIGHT0_Y")
		, m_light1_x(*this, "LIGHT1_X")
		, m_light1_y(*this, "LIGHT1_Y")
	{}

	void bang(machine_config &config);

	void init_bang();

private:
	required_ioport m_light0_x;
	required_ioport m_light0_y;
	required_ioport m_light1_x;
	required_ioport m_light1_y;

	int m_clr_gun_int;

	u16 p1_gun_x();
	u16 p1_gun_y();
	u16 p2_gun_x();
	u16 p2_gun_y();
	void bang_clr_gun_int_w(u16 data);
	TIMER_DEVICE_CALLBACK_MEMBER(bang_irq);
	void bang_map(address_map &map);
};


class wrally2_state : public gaelco2_state
{
public:
	wrally2_state(const machine_config &mconfig, device_type type, const char *tag)
		: gaelco2_state(mconfig, type, tag)
		, m_analog0(*this, "ANALOG0")
		, m_analog1(*this, "ANALOG1")
	{}

	void wrally2(machine_config &config);

	DECLARE_CUSTOM_INPUT_MEMBER(wrally2_analog_bit_r);

private:
	required_ioport m_analog0;
	required_ioport m_analog1;

	uint8_t m_analog_ports[2];

	DECLARE_WRITE_LINE_MEMBER(wrally2_adc_clk);
	DECLARE_WRITE_LINE_MEMBER(wrally2_adc_cs);
	void wrally2_map(address_map &map);
};
