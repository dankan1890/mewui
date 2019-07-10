// license:BSD-3-Clause
// copyright-holders:Luca Elia

#include "machine/gen_latch.h"
#include "machine/timer.h"
#include "emupal.h"
#include "screen.h"

class suna16_state : public driver_device
{
public:
	suna16_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this,"maincpu")
		, m_pcm1(*this,"pcm1")
		, m_pcm2(*this,"pcm2")
		, m_gfxdecode(*this, "gfxdecode")
		, m_screen(*this, "screen")
		, m_palette(*this, "palette")
		, m_soundlatch(*this, "soundlatch")
		, m_spriteram(*this, "spriteram")
		, m_spriteram2(*this, "spriteram2")
		, m_bank1(*this, "bank1")
		, m_bank2(*this, "bank2")
		, m_leds(*this, "led%u", 0U)
	{ }

	void uballoon(machine_config &config);
	void sunaq(machine_config &config);
	void bssoccer(machine_config &config);
	void bestbest(machine_config &config);

	void init_uballoon();

private:
	// common
	DECLARE_WRITE16_MEMBER(soundlatch_w);
	DECLARE_READ16_MEMBER(paletteram_r);
	DECLARE_WRITE16_MEMBER(paletteram_w);
	DECLARE_WRITE16_MEMBER(flipscreen_w);

	// bestbest specific
	DECLARE_WRITE16_MEMBER(bestbest_flipscreen_w);
	DECLARE_WRITE16_MEMBER(bestbest_coin_w);
	DECLARE_READ8_MEMBER(bestbest_prot_r);
	DECLARE_WRITE8_MEMBER(bestbest_prot_w);
	DECLARE_WRITE8_MEMBER(bestbest_ay8910_port_a_w);

	// bssoccer specific
	DECLARE_WRITE16_MEMBER(bssoccer_leds_w);
	DECLARE_WRITE8_MEMBER(bssoccer_pcm_1_bankswitch_w);
	DECLARE_WRITE8_MEMBER(bssoccer_pcm_2_bankswitch_w);

	// uballoon specific
	DECLARE_WRITE16_MEMBER(uballoon_leds_w);
	DECLARE_WRITE8_MEMBER(uballoon_pcm_1_bankswitch_w);
	DECLARE_READ8_MEMBER(uballoon_prot_r);
	DECLARE_WRITE8_MEMBER(uballoon_prot_w);

	TIMER_DEVICE_CALLBACK_MEMBER(bssoccer_interrupt);

	virtual void video_start() override;
	DECLARE_MACHINE_START(bestbest);
	DECLARE_MACHINE_START(bssoccer);
	DECLARE_MACHINE_START(uballoon);
	DECLARE_MACHINE_RESET(uballoon);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_bestbest(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, uint16_t *sprites, int gfx);

	void bestbest_map(address_map &map);
	void bestbest_pcm_1_iomap(address_map &map);
	void bestbest_pcm_1_map(address_map &map);
	void bestbest_sound_map(address_map &map);
	void bssoccer_map(address_map &map);
	void bssoccer_pcm_1_io_map(address_map &map);
	void bssoccer_pcm_1_map(address_map &map);
	void bssoccer_pcm_2_io_map(address_map &map);
	void bssoccer_pcm_2_map(address_map &map);
	void bssoccer_sound_map(address_map &map);
	void sunaq_map(address_map &map);
	void sunaq_sound_map(address_map &map);
	void uballoon_map(address_map &map);
	void uballoon_pcm_1_io_map(address_map &map);
	void uballoon_pcm_1_map(address_map &map);
	void uballoon_sound_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_pcm1;
	optional_device<cpu_device> m_pcm2;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device<generic_latch_8_device> m_soundlatch;

	required_shared_ptr<uint16_t> m_spriteram;
	optional_shared_ptr<uint16_t> m_spriteram2;

	optional_memory_bank m_bank1;
	optional_memory_bank m_bank2;

	output_finder<4> m_leds;

	std::unique_ptr<uint16_t[]> m_paletteram;
	int m_color_bank;
	uint8_t m_prot;
};
