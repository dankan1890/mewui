// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    Sega pre-System 16 & System 16A hardware

***************************************************************************/

#include "cpu/m68000/m68000.h"
#include "cpu/mcs48/mcs48.h"
#include "cpu/mcs51/mcs51.h"
#include "cpu/z80/z80.h"
#include "machine/cxd1095.h"
#include "machine/gen_latch.h"
#include "machine/i8255.h"
#include "machine/i8243.h"
#include "machine/nvram.h"
#include "machine/segaic16.h"
#include "machine/watchdog.h"
#include "sound/ym2151.h"
#include "video/segaic16.h"
#include "video/sega16sp.h"


// ======================> segas16a_state

class segas16a_state : public sega_16bit_common_base
{
public:
	// construction/destruction
	segas16a_state(const machine_config &mconfig, device_type type, const char *tag)
		: sega_16bit_common_base(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_soundcpu(*this, "soundcpu")
		, m_mcu(*this, "mcu")
		, m_i8255(*this, "i8255")
		, m_ymsnd(*this, "ymsnd")
		, m_n7751(*this, "n7751")
		, m_n7751_i8243(*this, "n7751_8243")
		, m_nvram(*this, "nvram")
		, m_watchdog(*this, "watchdog")
		, m_segaic16vid(*this, "segaic16vid")
		, m_soundlatch(*this, "soundlatch")
		, m_sprites(*this, "sprites")
		, m_cxdio(*this, "cxdio")
		, m_workram(*this, "nvram")
		, m_sound_decrypted_opcodes(*this, "sound_decrypted_opcodes")
		, m_video_control(0)
		, m_mcu_control(0)
		, m_n7751_command(0)
		, m_n7751_rom_address(0)
		, m_last_buttons1(0)
		, m_last_buttons2(0)
		, m_read_port(0)
		, m_mj_input_num(0)
		, m_mj_inputs(*this, {"MJ0", "MJ1", "MJ2", "MJ3", "MJ4", "MJ5"})
		, m_lamps(*this, "lamp%u", 0U)
	{ }

	void system16a_no7751(machine_config &config);
	void system16a(machine_config &config);
	void system16a_fd1089a_no7751(machine_config &config);
	void system16a_fd1089b_no7751(machine_config &config);
	void system16a_fd1089a(machine_config &config);
	void system16a_fd1094(machine_config &config);
	void system16a_no7751p(machine_config &config);
	void system16a_fd1094_no7751(machine_config &config);
	void system16a_i8751(machine_config &config);
	void system16a_fd1089b(machine_config &config);
	void aceattaca_fd1094(machine_config &config);

	// game-specific driver init
	void init_generic();
	void init_dumpmtmt();
	void init_quartet();
	void init_fantzonep();
	void init_sjryukoa();
	void init_aceattaca();
	void init_passsht16a();
	void init_mjleague();
	void init_sdi();

private:
	// PPI read/write callbacks
	DECLARE_WRITE8_MEMBER( misc_control_w );
	DECLARE_WRITE8_MEMBER( tilemap_sound_w );

	// main CPU read/write handlers
	DECLARE_READ16_MEMBER( standard_io_r );
	DECLARE_WRITE16_MEMBER( standard_io_w );
	DECLARE_READ16_MEMBER( misc_io_r );
	DECLARE_WRITE16_MEMBER( misc_io_w );

	// Z80 sound CPU read/write handlers
	DECLARE_READ8_MEMBER( sound_data_r );
	DECLARE_WRITE8_MEMBER( n7751_command_w );
	DECLARE_WRITE8_MEMBER( n7751_control_w );
	template<int Shift> void n7751_rom_offset_w(uint8_t data);

	// N7751 sound generator CPU read/write handlers
	DECLARE_READ8_MEMBER( n7751_rom_r );
	DECLARE_READ8_MEMBER( n7751_p2_r );
	DECLARE_WRITE8_MEMBER( n7751_p2_w );

	// I8751 MCU read/write handlers
	DECLARE_WRITE8_MEMBER( mcu_control_w );
	DECLARE_WRITE8_MEMBER( mcu_io_w );
	DECLARE_READ8_MEMBER( mcu_io_r );

	// I8751-related VBLANK interrupt handlers
	DECLARE_WRITE_LINE_MEMBER(i8751_main_cpu_vblank_w);

	// video updates
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void decrypted_opcodes_map(address_map &map);
	void mcu_io_map(address_map &map);
	void sound_decrypted_opcodes_map(address_map &map);
	void sound_map(address_map &map);
	void sound_no7751_portmap(address_map &map);
	void sound_portmap(address_map &map);
	void system16a_map(address_map &map);

	// internal types
	typedef delegate<void ()> i8751_sim_delegate;
	typedef delegate<void (uint8_t, uint8_t)> lamp_changed_delegate;

	// timer IDs
	enum
	{
		TID_INIT_I8751,
		TID_PPI_WRITE
	};

	// driver overrides
	virtual void video_start() override;
	virtual void machine_start() override { m_lamps.resolve(); }
	virtual void machine_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// I8751 simulations
	void dumpmtmt_i8751_sim();
	void quartet_i8751_sim();

	// custom I/O handlers
	DECLARE_READ16_MEMBER( aceattaca_custom_io_r );
	DECLARE_WRITE16_MEMBER( aceattaca_custom_io_w );
	DECLARE_READ16_MEMBER( mjleague_custom_io_r );
	DECLARE_READ16_MEMBER( passsht16a_custom_io_r );
	DECLARE_READ16_MEMBER( sdi_custom_io_r );
	DECLARE_READ16_MEMBER( sjryuko_custom_io_r );
	void sjryuko_lamp_changed_w(uint8_t changed, uint8_t newval);

	// devices
	required_device<m68000_device> m_maincpu;
	required_device<z80_device> m_soundcpu;
	optional_device<i8751_device> m_mcu;
	required_device<i8255_device> m_i8255;
	required_device<ym2151_device> m_ymsnd;
	optional_device<n7751_device> m_n7751;
	optional_device<i8243_device> m_n7751_i8243;
	required_device<nvram_device> m_nvram;
	required_device<watchdog_timer_device> m_watchdog;
	required_device<segaic16_video_device> m_segaic16vid;
	required_device<generic_latch_8_device> m_soundlatch;
	required_device<sega_sys16a_sprite_device> m_sprites;
	optional_device<cxd1095_device> m_cxdio;

	// memory pointers
	required_shared_ptr<uint16_t> m_workram;
	optional_shared_ptr<uint8_t> m_sound_decrypted_opcodes;

	// configuration
	read16_delegate         m_custom_io_r;
	write16_delegate        m_custom_io_w;
	i8751_sim_delegate      m_i8751_vblank_hook;
	lamp_changed_delegate   m_lamp_changed_w;

	// internal state
	uint8_t                   m_video_control;
	uint8_t                   m_mcu_control;
	uint8_t                   m_n7751_command;
	uint32_t                  m_n7751_rom_address;
	uint8_t                   m_last_buttons1;
	uint8_t                   m_last_buttons2;
	uint8_t                   m_read_port;
	uint8_t                   m_mj_input_num;
	optional_ioport_array<6> m_mj_inputs;
	output_finder<2> m_lamps;
};

class afighter_16a_analog_state : public segas16a_state
{
public:
	// construction/destruction
	afighter_16a_analog_state(const machine_config &mconfig, device_type type, const char *tag)
		: segas16a_state(mconfig, type, tag),
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
