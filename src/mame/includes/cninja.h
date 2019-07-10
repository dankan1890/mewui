// license:BSD-3-Clause
// copyright-holders:Bryan McPhail
/*************************************************************************

    Caveman Ninja (and other DECO 16bit titles)

*************************************************************************/

#include "cpu/h6280/h6280.h"
#include "sound/okim6295.h"
#include "video/deco16ic.h"
#include "video/bufsprite.h"
#include "video/decospr.h"
#include "machine/deco_irq.h"
#include "machine/deco146.h"
#include "machine/deco104.h"
#include "machine/gen_latch.h"
#include "emupal.h"
#include "screen.h"

class cninja_state : public driver_device
{
public:
	cninja_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_ioprot(*this, "ioprot")
		, m_deco_tilegen(*this, "tilegen%u", 1U)
		, m_oki2(*this, "oki2")
		, m_sprgen(*this, "spritegen%u", 1U)
		, m_gfxdecode(*this, "gfxdecode")
		, m_screen(*this, "screen")
		, m_palette(*this, "palette")
		, m_soundlatch(*this, "soundlatch")
		, m_spriteram(*this, "spriteram%u", 1U)
		, m_pf_rowscroll(*this, "pf%u_rowscroll", 1U)
		, m_okibank(*this, "okibank")
	{ }

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	optional_device<deco_146_base_device> m_ioprot;
	required_device_array<deco16ic_device, 2> m_deco_tilegen;
	optional_device<okim6295_device> m_oki2;
	optional_device_array<decospr_device, 2> m_sprgen;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	optional_device<generic_latch_8_device> m_soundlatch;
	optional_device_array<buffered_spriteram16_device, 2> m_spriteram;

	/* memory pointers */
	required_shared_ptr_array<uint16_t, 4> m_pf_rowscroll;
	optional_memory_bank m_okibank;

	uint16_t m_priority;

	template<int Chip> DECLARE_WRITE16_MEMBER(cninja_pf_control_w);
	DECLARE_WRITE8_MEMBER(sound_bankswitch_w);
	DECLARE_WRITE8_MEMBER(cninjabl2_oki_bank_w);
	void init_mutantf();
	void init_cninjabl2();
	DECLARE_MACHINE_START(robocop2);
	DECLARE_MACHINE_RESET(robocop2);
	DECLARE_VIDEO_START(cninja);
	DECLARE_VIDEO_START(stoneage);
	DECLARE_VIDEO_START(mutantf);
	uint32_t screen_update_cninja(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_cninjabl(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_cninjabl2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_edrandy(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_robocop2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_mutantf(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void cninjabl_draw_sprites( screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect );

	DECO16IC_BANK_CB_MEMBER(cninja_bank_callback);
	DECO16IC_BANK_CB_MEMBER(robocop2_bank_callback);
	DECO16IC_BANK_CB_MEMBER(mutantf_1_bank_callback);
	DECO16IC_BANK_CB_MEMBER(mutantf_2_bank_callback);

	DECOSPR_PRIORITY_CB_MEMBER(pri_callback);

	DECLARE_READ16_MEMBER( edrandy_protection_region_6_146_r );
	DECLARE_WRITE16_MEMBER( edrandy_protection_region_6_146_w );
	DECLARE_READ16_MEMBER( edrandy_protection_region_8_146_r );
	DECLARE_WRITE16_MEMBER( edrandy_protection_region_8_146_w );

	DECLARE_READ16_MEMBER( mutantf_protection_region_0_146_r );
	DECLARE_WRITE16_MEMBER( mutantf_protection_region_0_146_w );
	DECLARE_READ16_MEMBER( cninja_protection_region_0_104_r );
	DECLARE_WRITE16_MEMBER( cninja_protection_region_0_104_w );

	DECLARE_READ16_MEMBER(cninjabl2_sprite_dma_r);
	DECLARE_WRITE16_MEMBER(robocop2_priority_w);
	DECLARE_READ16_MEMBER(mutantf_71_r);
	void cninjabl(machine_config &config);
	void edrandy(machine_config &config);
	void cninja(machine_config &config);
	void robocop2(machine_config &config);
	void stoneage(machine_config &config);
	void cninjabl2(machine_config &config);
	void mutantf(machine_config &config);
	void cninja_map(address_map &map);
	void cninjabl2_oki_map(address_map &map);
	void cninjabl2_s_map(address_map &map);
	void cninjabl_map(address_map &map);
	void cninjabl_sound_map(address_map &map);
	void edrandy_map(address_map &map);
	void mutantf_map(address_map &map);
	void robocop2_map(address_map &map);
	void sound_map(address_map &map);
	void sound_map_mutantf(address_map &map);
	void stoneage_s_map(address_map &map);
};
