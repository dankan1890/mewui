// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    Sega System Hang On hardware

***************************************************************************/

#include "cpu/m68000/m68000.h"
#include "cpu/mcs51/mcs51.h"
#include "cpu/z80/z80.h"
#include "machine/i8255.h"
#include "machine/gen_latch.h"
#include "machine/segaic16.h"
#include "video/segaic16.h"
#include "video/segaic16_road.h"
#include "video/sega16sp.h"


// ======================> segahang_state

class segahang_state : public sega_16bit_common_base
{
public:
	// construction/destruction
	segahang_state(const machine_config &mconfig, device_type type, const char *tag)
		: sega_16bit_common_base(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_subcpu(*this, "subcpu")
		, m_soundcpu(*this, "soundcpu")
		, m_mcu(*this, "mcu")
		, m_i8255_1(*this, "i8255_1")
		, m_i8255_2(*this, "i8255_2")
		, m_sprites(*this, "sprites")
		, m_segaic16vid(*this, "segaic16vid")
		, m_segaic16road(*this, "segaic16road")
		, m_soundlatch(*this, "soundlatch")
		, m_workram(*this, "workram")
		, m_sharrier_video(false)
		, m_adc_select(0)
		, m_adc_ports(*this, {"ADC0", "ADC1", "ADC2", "ADC3"})
		, m_decrypted_opcodes(*this, "decrypted_opcodes")
		, m_lamps(*this, "lamp%u", 0U)
	{ }

	void sound_board_2203(machine_config &config);
	void sound_board_2203x2(machine_config &config);
	void sound_board_2151(machine_config &config);
	void shared_base(machine_config &config);
	void hangon_base(machine_config &config);
	void sharrier_base(machine_config &config);
	void enduror_base(machine_config &config);
	void endurord_base(machine_config &config);
	void endurob2(machine_config &config);
	void shangupb(machine_config &config);
	void enduror(machine_config &config);
	void shangonro(machine_config &config);
	void enduror1d(machine_config &config);
	void endurord(machine_config &config);
	void sharrier(machine_config &config);
	void endurobl(machine_config &config);
	void enduror1(machine_config &config);
	void hangon(machine_config &config);

	// game-specific driver init
	void init_generic();
	void init_sharrier();
	void init_enduror();
	void init_endurobl();
	void init_endurob2();

private:
	// PPI read/write callbacks
	DECLARE_WRITE8_MEMBER( video_lamps_w );
	DECLARE_WRITE8_MEMBER( tilemap_sound_w );
	DECLARE_WRITE8_MEMBER( sub_control_adc_w );
	DECLARE_READ8_MEMBER( adc_status_r );

	// main CPU read/write handlers
	DECLARE_READ16_MEMBER( hangon_io_r );
	DECLARE_WRITE16_MEMBER( hangon_io_w );
	DECLARE_READ16_MEMBER( sharrier_io_r );
	DECLARE_WRITE16_MEMBER( sharrier_io_w );

	// Z80 sound CPU read/write handlers
	DECLARE_READ8_MEMBER( sound_data_r );

	// I8751-related VBLANK interrupt handlers
	INTERRUPT_GEN_MEMBER( i8751_main_cpu_vblank );

	// video updates
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void decrypted_opcodes_map(address_map &map);
	void fd1094_decrypted_opcodes_map(address_map &map);
	void hangon_map(address_map &map);
	void mcu_io_map(address_map &map);
	void sharrier_map(address_map &map);
	void sound_map_2151(address_map &map);
	void sound_map_2203(address_map &map);
	void sound_portmap_2151(address_map &map);
	void sound_portmap_2203(address_map &map);
	void sound_portmap_2203x2(address_map &map);
	void sub_map(address_map &map);

	// internal types
	typedef delegate<void ()> i8751_sim_delegate;

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
	void sharrier_i8751_sim();

	// devices
	required_device<m68000_device> m_maincpu;
	required_device<m68000_device> m_subcpu;
	required_device<z80_device> m_soundcpu;
	optional_device<i8751_device> m_mcu;
	required_device<i8255_device> m_i8255_1;
	required_device<i8255_device> m_i8255_2;
	required_device<sega_16bit_sprite_device> m_sprites;
	required_device<segaic16_video_device> m_segaic16vid;
	required_device<segaic16_road_device> m_segaic16road;
	required_device<generic_latch_8_device> m_soundlatch;

	// memory pointers
	required_shared_ptr<uint16_t> m_workram;

	// configuration
	bool                    m_sharrier_video;
	i8751_sim_delegate      m_i8751_vblank_hook;

	// internal state
	uint8_t                   m_adc_select;
	optional_ioport_array<4> m_adc_ports;
	bool                    m_shadow;
	optional_shared_ptr<uint16_t> m_decrypted_opcodes;
	output_finder<2> m_lamps;
};
