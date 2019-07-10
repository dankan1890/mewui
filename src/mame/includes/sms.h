// license:BSD-3-Clause
// copyright-holders:Wilbert Pol, Charles MacDonald,Mathis Rosenhauer,Brad Oliver,Michael Luong,Fabio Priuli,Enik Land
/*****************************************************************************
 *
 * includes/sms.h
 *
 ****************************************************************************/

#ifndef MAME_INCLUDES_SMS_H
#define MAME_INCLUDES_SMS_H

#define LOG_REG
#define LOG_PAGING
#define LOG_COLOR

#define NVRAM_SIZE             (0x08000)
#define CPU_ADDRESSABLE_SIZE   (0x10000)

#define MAX_CARTRIDGES        16

#define CONTROL1_TAG   "ctrl1"
#define CONTROL2_TAG   "ctrl2"

#include "bus/gamegear/ggext.h"
#include "bus/sega8/sega8_slot.h"
#include "bus/sg1000_exp/sg1000exp.h"
#include "bus/sms_ctrl/smsctrl.h"
#include "bus/sms_exp/smsexp.h"
#include "sound/ym2413.h"
#include "video/315_5124.h"

#include "screen.h"
#include "machine/timer.h"


class sms_state : public driver_device
{
public:
	sms_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_vdp(*this, "sms_vdp"),
		m_main_scr(*this, "screen"),
		m_ym(*this, "ym2413"),
		m_port_ctrl1(*this, CONTROL1_TAG),
		m_port_ctrl2(*this, CONTROL2_TAG),
		m_port_gg_ext(*this, "ext"),
		m_port_gg_dc(*this, "GG_PORT_DC"),
		m_port_pause(*this, "PAUSE"),
		m_port_reset(*this, "RESET"),
		m_port_rapid(*this, "RAPID"),
		m_port_start(*this, "START"),
		m_port_scope(*this, "SEGASCOPE"),
		m_port_scope_binocular(*this, "SSCOPE_BINOCULAR"),
		m_port_persist(*this, "PERSISTENCE"),
		m_led_pwr(*this, "led_pwr"),
		m_region_maincpu(*this, "maincpu"),
		m_mainram(nullptr),
		m_left_lcd(*this, "left_lcd"),
		m_right_lcd(*this, "right_lcd"),
		m_is_gamegear(false),
		m_is_smsj(false),
		m_is_mark_iii(false),
		m_ioctrl_region_is_japan(false),
		m_has_bios_0400(false),
		m_has_bios_2000(false),
		m_has_bios_full(false),
		m_has_jpn_sms_cart_slot(false),
		m_has_pwr_led(false),
		m_slot(*this, "slot"),
		m_cardslot(*this, "mycard"),
		m_smsexpslot(*this, "smsexp"),
		m_sgexpslot(*this, "sgexp")
	{ }

	void sms_base(machine_config &config);
	void sms_ntsc_base(machine_config &config);
	void sms_pal_base(machine_config &config);
	void sms_paln_base(machine_config &config);
	void sms_br_base(machine_config &config);
	void sms3_br(machine_config &config);
	void sg1000m3(machine_config &config);
	void smsj(machine_config &config);
	void sms1_paln(machine_config &config);
	void sms1_ntsc(machine_config &config);
	void gamegear(machine_config &config);
	void gamegeaj(machine_config &config);
	void sms3_paln(machine_config &config);
	void sms1_pal(machine_config &config);
	void sms2_pal(machine_config &config);
	void sms2_kr(machine_config &config);
	void sms1_br(machine_config &config);
	void sms2_ntsc(machine_config &config);
	void sms1_kr(machine_config &config);

	DECLARE_WRITE_LINE_MEMBER(gg_pause_callback);

	uint32_t screen_update_sms(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

protected:
	template <typename X> static void screen_sms_pal_raw_params(screen_device &screen, X &&pixelclock);
	template <typename X> static void screen_sms_ntsc_raw_params(screen_device &screen, X &&pixelclock);
	template <typename X> static void screen_gg_raw_params(screen_device &screen, X &&pixelclock);

	DECLARE_READ8_MEMBER(read_0000);
	DECLARE_READ8_MEMBER(read_4000);
	DECLARE_READ8_MEMBER(read_8000);
	DECLARE_READ8_MEMBER(read_ram);
	DECLARE_WRITE8_MEMBER(write_ram);
	DECLARE_WRITE8_MEMBER(write_cart);

	DECLARE_READ8_MEMBER(sms_mapper_r);
	DECLARE_WRITE8_MEMBER(sms_mapper_w);
	DECLARE_WRITE8_MEMBER(sms_mem_control_w);
	DECLARE_WRITE8_MEMBER(sms_io_control_w);
	DECLARE_READ8_MEMBER(sms_count_r);
	DECLARE_READ8_MEMBER(sms_input_port_dc_r);
	DECLARE_READ8_MEMBER(sms_input_port_dd_r);
	DECLARE_READ8_MEMBER(gg_input_port_00_r);
	DECLARE_READ8_MEMBER(sg1000m3_peripheral_r);
	DECLARE_WRITE8_MEMBER(sg1000m3_peripheral_w);
	DECLARE_READ8_MEMBER(gg_sio_r);
	DECLARE_WRITE8_MEMBER(gg_sio_w);
	DECLARE_WRITE8_MEMBER(gg_psg_stereo_w);
	DECLARE_READ8_MEMBER(smsj_audio_control_r);
	DECLARE_WRITE8_MEMBER(smsj_audio_control_w);
	DECLARE_WRITE8_MEMBER(smsj_ym2413_register_port_w);
	DECLARE_WRITE8_MEMBER(smsj_ym2413_data_port_w);
	DECLARE_READ8_MEMBER(sms_sscope_r);
	DECLARE_WRITE8_MEMBER(sms_sscope_w);

	DECLARE_WRITE_LINE_MEMBER(sms_n_csync_callback);
	DECLARE_WRITE_LINE_MEMBER(sms_ctrl1_th_input);
	DECLARE_WRITE_LINE_MEMBER(sms_ctrl2_th_input);
	DECLARE_WRITE_LINE_MEMBER(gg_ext_th_input);

	DECLARE_VIDEO_START(gamegear);
	DECLARE_VIDEO_RESET(gamegear);
	DECLARE_VIDEO_START(sms1);
	DECLARE_VIDEO_RESET(sms1);

	uint32_t screen_update_sms1(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_sms1_left(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_sms1_right(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_gamegear(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void screen_gg_sms_mode_scaling(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(screen_vblank_sms1);

	void gg_io(address_map &map);
	void sg1000m3_io(address_map &map);
	void sms1_mem(address_map &map);
	void sms_io(address_map &map);
	void sms_mem(address_map &map);
	void smsj_io(address_map &map);
	void smskr_io(address_map &map);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	uint8_t read_bus(address_space &space, unsigned int bank, uint16_t base_addr, uint16_t offset);
	void setup_bios();
	void setup_media_slots();
	void setup_enabled_slots();
	void lphaser_hcount_latch();
	void sms_get_inputs();
	void smsj_set_audio_control(uint8_t data);

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<sega315_5124_device> m_vdp;
	required_device<screen_device> m_main_scr;
	optional_device<ym2413_device> m_ym;
	optional_device<sms_control_port_device> m_port_ctrl1;
	optional_device<sms_control_port_device> m_port_ctrl2;
	optional_device<gg_ext_port_device> m_port_gg_ext;

	optional_ioport m_port_gg_dc;
	optional_ioport m_port_pause;
	optional_ioport m_port_reset;
	optional_ioport m_port_rapid;
	optional_ioport m_port_start;
	optional_ioport m_port_scope;
	optional_ioport m_port_scope_binocular;
	optional_ioport m_port_persist;

	output_finder<> m_led_pwr;

	required_memory_region m_region_maincpu;
	address_space *m_space;
	std::unique_ptr<uint8_t[]> m_mainram;
	uint8_t *m_BIOS;

	// for 3D glass binocular hack
	optional_device<screen_device> m_left_lcd;
	optional_device<screen_device> m_right_lcd;
	bitmap_rgb32 m_prevleft_bitmap;
	bitmap_rgb32 m_prevright_bitmap;

	// for gamegear LCD persistence hack
	bitmap_rgb32 m_prev_bitmap;
	bool m_prev_bitmap_copied;

	// for gamegear SMS mode scaling
	bitmap_rgb32 m_gg_sms_mode_bitmap;
	// line_buffer will be used to hold 4 lines of line data as a kind of cache for
	// vertical scaling in the gamegear sms compatibility mode.
	std::unique_ptr<int[]> m_line_buffer;

	// model identifiers
	bool m_is_gamegear;
	bool m_is_smsj;
	bool m_is_mark_iii;
	bool m_ioctrl_region_is_japan;
	bool m_has_bios_0400;
	bool m_has_bios_2000;
	bool m_has_bios_full;
	bool m_has_jpn_sms_cart_slot;
	bool m_has_pwr_led;

	// [0] for 0x400-0x3fff, [1] for 0x4000-0x7fff, [2] for 0x8000-0xffff, [3] for 0x0000-0x0400
	uint8_t m_bios_page[4];

	uint8_t m_bios_page_count;
	uint8_t m_mapper[4];
	uint8_t m_io_ctrl_reg;
	uint8_t m_mem_ctrl_reg;
	uint8_t m_mem_device_enabled;
	uint8_t m_smsj_audio_control;
	uint8_t m_port_dc_reg;
	uint8_t m_port_dd_reg;
	uint8_t m_gg_sio[5];
	int m_gg_paused;

	uint8_t m_ctrl1_th_state;
	uint8_t m_ctrl2_th_state;
	uint8_t m_ctrl1_th_latch;
	uint8_t m_ctrl2_th_latch;

	// Data needed for Light Phaser
	int m_lphaser_x_offs;   /* Needed to 'calibrate' lphaser; set at cart loading */
	emu_timer *m_lphaser_th_timer;
	TIMER_CALLBACK_MEMBER(lphaser_th_generate);

	// Data needed for SegaScope (3D glasses)
	uint8_t m_sscope_state;
	uint8_t m_frame_sscope_state;

	// Data needed for Rapid button (smsj, sms1kr, sms1krfm)
	uint16_t m_csync_counter;
	uint8_t m_rapid_mode;
	uint8_t m_rapid_read_state;
	uint8_t m_rapid_last_dc;
	uint8_t m_rapid_last_dd;

	// slot devices
	sega8_cart_slot_device *m_cartslot;
	optional_device<sega8_cart_slot_device> m_slot;
	optional_device<sega8_card_slot_device> m_cardslot;
	optional_device<sms_expansion_slot_device> m_smsexpslot;
	optional_device<sg1000_expansion_slot_device> m_sgexpslot;
};

class smssdisp_state : public sms_state
{
public:
	smssdisp_state(const machine_config &mconfig, device_type type, const char *tag) :
		sms_state(mconfig, type, tag),
		m_control_cpu(*this, "control"),
		m_slots(*this, {"slot", "slot2", "slot3", "slot4", "slot5", "slot6", "slot7", "slot8", "slot9", "slot10", "slot11", "slot12", "slot13", "slot14", "slot15", "slot16"}),
		m_cards(*this, "slot%u", 17U),
		m_store_cart_selection_data(0)
	{ }

	void sms_sdisp(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void device_post_load() override;

private:
	DECLARE_READ8_MEMBER(sms_store_cart_select_r);
	DECLARE_WRITE8_MEMBER(sms_store_cart_select_w);
	void store_select_cart(uint8_t data);
	DECLARE_WRITE8_MEMBER(sms_store_control_w);

	DECLARE_READ8_MEMBER(store_cart_peek);

	DECLARE_WRITE_LINE_MEMBER(sms_store_int_callback);
	void sms_store_mem(address_map &map);

	required_device<cpu_device> m_control_cpu;
	required_device_array<sega8_cart_slot_device, 16> m_slots;
	required_device_array<sega8_card_slot_device, 16> m_cards;

	uint8_t m_store_control;
	uint8_t m_store_cart_selection_data;
};


/*----------- defined in machine/sms.c -----------*/

#define IO_EXPANSION    (0x80)  /* Expansion slot enable (1= disabled, 0= enabled) */
#define IO_CARTRIDGE    (0x40)  /* Cartridge slot enable (1= disabled, 0= enabled) */
#define IO_CARD         (0x20)  /* Card slot disabled (1= disabled, 0= enabled) */
#define IO_WORK_RAM     (0x10)  /* Work RAM disabled (1= disabled, 0= enabled) */
#define IO_BIOS_ROM     (0x08)  /* BIOS ROM disabled (1= disabled, 0= enabled) */
#define IO_CHIP         (0x04)  /* I/O chip disabled (1= disabled, 0= enabled) */

#endif // MAME_INCLUDES_SMS_H
