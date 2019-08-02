// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    Sega System 16B hardware

***************************************************************************/
#ifndef MAME_INCLUDES_SEGAS16B_H
#define MAME_INCLUDES_SEGAS16B_H

#pragma once

#include "cpu/m68000/m68000.h"
#include "cpu/mcs51/mcs51.h"
#include "cpu/z80/z80.h"
#include "machine/cxd1095.h"
#include "machine/gen_latch.h"
#include "machine/nvram.h"
#include "machine/segaic16.h"
#include "machine/upd4701.h"
#include "sound/ym2151.h"
#include "sound/ym2413.h"
#include "sound/upd7759.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "video/segaic16.h"
#include "video/sega16sp.h"


// ======================> segas16b_state

class segas16b_state : public sega_16bit_common_base
{
public:
	// construction/destruction
	segas16b_state(const machine_config &mconfig, device_type type, const char *tag)
		: sega_16bit_common_base(mconfig, type, tag)
		, m_mapper(*this, "mapper")
		, m_maincpu(*this, "maincpu")
		, m_soundcpu(*this, "soundcpu")
		, m_mcu(*this, "mcu")
		, m_ym2151(*this, "ym2151")
		, m_ym2413(*this, "ym2413")
		, m_upd7759(*this, "upd")
		, m_dac(*this, "dac")
		, m_multiplier(*this, "multiplier")
		, m_cmptimer_1(*this, "cmptimer_1")
		, m_cmptimer_2(*this, "cmptimer_2")
		, m_nvram(*this, "nvram")
		, m_sprites(*this, "sprites")
		, m_segaic16vid(*this, "segaic16vid")
		, m_soundlatch(*this, "soundlatch")
		, m_cxdio(*this, "cxdio")
		, m_upd4701a(*this, {"upd4701a1", "upd4701a2"})
		, m_workram(*this, "workram")
		, m_romboard(ROM_BOARD_INVALID)
		, m_tilemap_type(segaic16_video_device::TILEMAP_16B)
		, m_disable_screen_blanking(false)
		, m_i8751_initial_config(nullptr)
		, m_atomicp_sound_divisor(0)
		, m_atomicp_sound_count(0)
		, m_hwc_input_value(0)
		, m_hwc_monitor(*this, "MONITOR")
		, m_hwc_left(*this, "LEFT")
		, m_hwc_right(*this, "RIGHT")
		, m_mj_input_num(0)
		, m_mj_last_val(0)
		, m_mj_inputs(*this, {"MJ0", "MJ1", "MJ2", "MJ3", "MJ4", "MJ5"})
		, m_spritepalbase(0x400)
		, m_gfxdecode(*this, "gfxdecode")
		, m_sound_decrypted_opcodes(*this, "sound_decrypted_opcodes")
		, m_decrypted_opcodes(*this, "decrypted_opcodes")
		, m_bootleg_scroll(*this, "bootleg_scroll")
		, m_bootleg_page(*this, "bootleg_page")
		, m_lamps(*this, "lamp%u", 0U)
	{ }

	void rom_5797_fragment(machine_config &config);
	void system16b_fd1094_5797(machine_config &config);
	void fpointbla(machine_config &config);
	void atomicp(machine_config &config);
	void aceattacb_fd1094(machine_config &config);
	void system16b_i8751(machine_config &config);
	void system16c(machine_config &config);
	void system16b_mc8123(machine_config &config);
	void system16b_i8751_5797(machine_config &config);
	void system16b_fd1089a(machine_config &config);
	void system16b_5797(machine_config &config);
	void system16b_split(machine_config &config);
	void system16b_fd1089b(machine_config &config);
	void system16b(machine_config &config);
	void system16b_fd1094(machine_config &config);
	void fpointbl(machine_config &config);
	void lockonph(machine_config &config);
	void dfjail(machine_config &config);

	// ROM board-specific driver init
	void init_generic_5521();
	void init_generic_5358();
	void init_generic_5704();
	void init_generic_5358_small();
	void init_generic_5797();
	void init_generic_korean();
	void init_generic_bootleg();
	void init_lockonph();
	// game-specific driver init
	void init_isgsm();
	void init_tturf_5704();
	void init_wb3_5704();
	void init_hwchamp_5521();
	void init_altbeas5_5521();
	void init_sdi_5358_small();
	void init_fpointbla();
	void init_altbeasj_5521();
	void init_ddux_5704();
	void init_snapper();
	void init_shinobi4_5521();
	void init_defense_5358_small();
	void init_sjryuko_5358_small();
	void init_exctleag_5358();
	void init_tetrbx();
	void init_aceattac_5358();
	void init_passshtj_5358();
	void init_cencourt_5358();
	void init_shinfz();
	void init_dunkshot_5358_small();
	void init_timescan_5358_small();
	void init_shinobi3_5358();
	void init_altbeas4_5521();
	void init_aliensyn7_5358_small();

protected:
	// memory mapping
	void memory_mapper(sega_315_5195_mapper_device &mapper, uint8_t index);

	// main CPU read/write handlers
	DECLARE_WRITE16_MEMBER( rom_5704_bank_w );
	DECLARE_READ16_MEMBER( rom_5797_bank_math_r );
	DECLARE_WRITE16_MEMBER( rom_5797_bank_math_w );
	DECLARE_READ16_MEMBER( unknown_rgn2_r );
	DECLARE_WRITE16_MEMBER( unknown_rgn2_w );
	DECLARE_READ16_MEMBER( standard_io_r );
	DECLARE_WRITE16_MEMBER( standard_io_w );
	DECLARE_WRITE16_MEMBER( atomicp_sound_w );

	DECLARE_READ16_MEMBER( bootleg_custom_io_r );
	DECLARE_WRITE16_MEMBER( bootleg_custom_io_w );

	// sound CPU read/write handlers
	DECLARE_WRITE8_MEMBER( upd7759_control_w );
	DECLARE_READ8_MEMBER( upd7759_status_r );
	DECLARE_WRITE16_MEMBER( sound_w16 );

	// dfjail
	DECLARE_WRITE8_MEMBER( dfjail_sound_control_w );
	DECLARE_WRITE8_MEMBER( dfjail_dac_data_w );
	INTERRUPT_GEN_MEMBER( dfjail_soundirq_cb );
	bool m_dfjail_nmi_enable;
	uint16_t m_dfjail_dac_data;

	// other callbacks
	DECLARE_WRITE_LINE_MEMBER(upd7759_generate_nmi);
	INTERRUPT_GEN_MEMBER( i8751_main_cpu_vblank );
	DECLARE_WRITE8_MEMBER(spin_68k_w);

	// video updates
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	DECLARE_WRITE16_MEMBER( tileram_w ) { m_segaic16vid->tileram_w(space,offset,data,mem_mask); };
	DECLARE_WRITE16_MEMBER( textram_w ) { m_segaic16vid->textram_w(space,offset,data,mem_mask); };

	// bootleg stuff
	void tilemap_16b_fpointbl_fill_latch(int i, uint16_t* latched_pageselect, uint16_t* latched_yscroll, uint16_t* latched_xscroll, uint16_t* textram);

	void decrypted_opcodes_map(address_map &map);
	void decrypted_opcodes_map_fpointbla(address_map &map);
	void decrypted_opcodes_map_x(address_map &map);
	void fpointbl_map(address_map &map);
	void fpointbl_sound_map(address_map &map);
	void lockonph_map(address_map &map);
	void dfjail_map(address_map &map);
	void lockonph_sound_iomap(address_map &map);
	void lockonph_sound_map(address_map &map);
	void map_fpointbla(address_map &map);
	void mcu_io_map(address_map &map);
	void sound_decrypted_opcodes_map(address_map &map);
	void sound_map(address_map &map);
	void sound_portmap(address_map &map);
	void bootleg_sound_map(address_map &map);
	void bootleg_sound_portmap(address_map &map);
	void dfjail_sound_iomap(address_map &map);
	void system16b_bootleg_map(address_map &map);
	void system16b_map(address_map &map);
	void system16c_map(address_map &map);

	// internal types
	typedef delegate<void ()> i8751_sim_delegate;

	// timer IDs
	enum
	{
		TID_INIT_I8751,
		TID_ATOMICP_SOUND_IRQ
	};

	// rom board types
	enum segas16b_rom_board
	{
		ROM_BOARD_INVALID,
		ROM_BOARD_171_5358_SMALL,       // 171-5358 with smaller ROMs
		ROM_BOARD_171_5358,             // 171-5358
		ROM_BOARD_171_5521,             // 171-5521
		ROM_BOARD_171_5704,             // 171-5704 - don't know any diff between this and 171-5521
		ROM_BOARD_171_5797,             // 171-5797
		ROM_BOARD_KOREAN                // (custom Korean)
	};

	// device overrides
	virtual void video_start() override;
	virtual void machine_start() override { m_lamps.resolve(); }
	virtual void machine_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// internal helpers
	void init_generic(segas16b_rom_board rom_board);

	// i8751 simulations
	void altbeast_common_i8751_sim(offs_t soundoffs, offs_t inputoffs, int alt_bank);
	void altbeasj_i8751_sim();
	void altbeas5_i8751_sim();
	void ddux_i8751_sim();
	void tturf_i8751_sim();
	void wb3_i8751_sim();

	// custom I/O handlers
	DECLARE_READ16_MEMBER( aceattac_custom_io_r );
	DECLARE_WRITE16_MEMBER( aceattac_custom_io_w );
	DECLARE_READ16_MEMBER( dunkshot_custom_io_r );
	DECLARE_READ16_MEMBER( hwchamp_custom_io_r );
	DECLARE_WRITE16_MEMBER( hwchamp_custom_io_w );
	DECLARE_READ16_MEMBER( passshtj_custom_io_r );
	DECLARE_READ16_MEMBER( sdi_custom_io_r );
	DECLARE_READ16_MEMBER( sjryuko_custom_io_r );
	DECLARE_WRITE16_MEMBER( sjryuko_custom_io_w );

	// devices
	optional_device<sega_315_5195_mapper_device> m_mapper;
	required_device<m68000_device> m_maincpu;
	optional_device<z80_device> m_soundcpu;
	optional_device<i8751_device> m_mcu;
	optional_device<ym2151_device> m_ym2151;
	optional_device<ym2413_device> m_ym2413;
	optional_device<upd7759_device> m_upd7759;
	optional_device<dac_word_interface> m_dac; // dfjail
	optional_device<sega_315_5248_multiplier_device> m_multiplier;
	optional_device<sega_315_5250_compare_timer_device> m_cmptimer_1;
	optional_device<sega_315_5250_compare_timer_device> m_cmptimer_2;
	required_device<nvram_device> m_nvram;
	optional_device<sega_sys16b_sprite_device> m_sprites;
	required_device<segaic16_video_device> m_segaic16vid;
	optional_device<generic_latch_8_device> m_soundlatch; // not for atomicp
	optional_device<cxd1095_device> m_cxdio; // for aceattac
	optional_device_array<upd4701_device, 2> m_upd4701a; // for aceattac

	// memory pointers
	required_shared_ptr<uint16_t> m_workram;

	// configuration
	segas16b_rom_board  m_romboard;
	int                 m_tilemap_type;
	read16_delegate     m_custom_io_r;
	write16_delegate    m_custom_io_w;
	bool                m_disable_screen_blanking;
	const uint8_t *       m_i8751_initial_config;
	i8751_sim_delegate  m_i8751_vblank_hook;
	uint8_t               m_atomicp_sound_divisor;

	// game-specific state
	uint8_t               m_atomicp_sound_count;
	uint8_t               m_hwc_input_value;
	optional_ioport     m_hwc_monitor;
	optional_ioport     m_hwc_left;
	optional_ioport     m_hwc_right;
	uint8_t               m_mj_input_num;
	uint8_t               m_mj_last_val;
	optional_ioport_array<6> m_mj_inputs;
	int                 m_spritepalbase;

	required_device<gfxdecode_device> m_gfxdecode;
	optional_shared_ptr<uint8_t> m_sound_decrypted_opcodes;
	optional_shared_ptr<uint16_t> m_decrypted_opcodes;
	optional_shared_ptr<uint16_t> m_bootleg_scroll;
	optional_shared_ptr<uint16_t> m_bootleg_page;
	output_finder<2> m_lamps;
};

class afighter_16b_analog_state : public segas16b_state
{
public:
	// construction/destruction
	afighter_16b_analog_state(const machine_config &mconfig, device_type type, const char *tag)
		: segas16b_state(mconfig, type, tag),
			m_accel(*this, "ACCEL"),
			m_steer(*this, "STEER")
	{ }

	DECLARE_CUSTOM_INPUT_MEMBER(afighter_accel_r);
	DECLARE_CUSTOM_INPUT_MEMBER(afighter_handl_left_r);
	DECLARE_CUSTOM_INPUT_MEMBER(afighter_handl_right_r);

private:
	required_ioport     m_accel;
	required_ioport     m_steer;
};


// ======================> isgsm_state

class isgsm_state : public segas16b_state
{
public:
	// construction/destruction
	isgsm_state(const machine_config &mconfig, device_type type, const char *tag)
		: segas16b_state(mconfig, type, tag),
			m_read_xor(0),
			m_cart_addrlatch(0),
			m_cart_addr(0),
			m_data_type(0),
			m_data_addr(0),
			m_data_mode(0),
			m_addr_latch(0),
			m_security_value(0),
			m_security_latch(0),
			m_rle_control_position(8),
			m_rle_control_byte(0),
			m_rle_latched(false),
			m_rle_byte(0)
	{ }

	void isgsm(machine_config &config);

	// driver init
	void init_isgsm();
	void init_shinfz();
	void init_tetrbx();

private:
	// read/write handlers
	DECLARE_WRITE16_MEMBER( cart_addr_high_w );
	DECLARE_WRITE16_MEMBER( cart_addr_low_w );
	DECLARE_READ16_MEMBER( cart_data_r );
	DECLARE_WRITE16_MEMBER( data_w );
	DECLARE_WRITE16_MEMBER( datatype_w );
	DECLARE_WRITE16_MEMBER( addr_high_w );
	DECLARE_WRITE16_MEMBER( addr_low_w );
	DECLARE_WRITE16_MEMBER( cart_security_high_w );
	DECLARE_WRITE16_MEMBER( cart_security_low_w );
	DECLARE_READ16_MEMBER( cart_security_low_r );
	DECLARE_READ16_MEMBER( cart_security_high_r );
	DECLARE_WRITE16_MEMBER( sound_reset_w );
	DECLARE_WRITE16_MEMBER( main_bank_change_w );

	// security callbacks
	uint32_t shinfz_security(uint32_t input);
	uint32_t tetrbx_security(uint32_t input);

	// driver overrides
	virtual void machine_reset() override;

	// configuration
	uint8_t           m_read_xor;
	typedef delegate<uint32_t (uint32_t)> security_callback_delegate;
	security_callback_delegate m_security_callback;

	// internal state
	uint16_t          m_cart_addrlatch;
	uint32_t          m_cart_addr;
	uint8_t           m_data_type;
	uint32_t          m_data_addr;
	uint8_t           m_data_mode;
	uint16_t          m_addr_latch;
	uint32_t          m_security_value;
	uint16_t          m_security_latch;
	uint8_t           m_rle_control_position;
	uint8_t           m_rle_control_byte;
	bool            m_rle_latched;
	uint8_t           m_rle_byte;
	void isgsm_map(address_map &map);
};

#endif // MAME_INCLUDES_SEGAS16B_H
