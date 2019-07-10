// license:BSD-3-Clause
// copyright-holders:Bryan McPhail, David Haywood
/*************************************************************************

    Boogie Wings

*************************************************************************/

#include "cpu/h6280/h6280.h"
#include "sound/okim6295.h"
#include "video/deco16ic.h"
#include "video/deco_ace.h"
#include "video/bufsprite.h"
#include "video/decospr.h"
#include "machine/deco104.h"

class boogwing_state : public driver_device
{
public:
	boogwing_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_deco104(*this, "ioprot")
		, m_deco_ace(*this, "deco_ace")
		, m_deco_tilegen(*this, "tilegen%u", 1)
		, m_oki(*this, "oki%u", 1)
		, m_sprgen(*this, "spritegen%u", 1)
		, m_spriteram(*this, "spriteram%u", 1)
		, m_pf_rowscroll(*this, "pf%u_rowscroll", 1)
		, m_decrypted_opcodes(*this, "decrypted_opcodes")
	{ }

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<h6280_device> m_audiocpu;
	required_device<deco104_device> m_deco104;
	required_device<deco_ace_device> m_deco_ace;
	required_device_array<deco16ic_device, 2> m_deco_tilegen;
	required_device_array<okim6295_device, 2> m_oki;
	required_device_array<decospr_device, 2> m_sprgen;
	required_device_array<buffered_spriteram16_device, 2> m_spriteram;
	/* memory pointers */
	required_shared_ptr_array<uint16_t, 4> m_pf_rowscroll;
	required_shared_ptr<uint16_t> m_decrypted_opcodes;

	uint16_t m_priority;

	DECLARE_WRITE8_MEMBER(sound_bankswitch_w);
	DECLARE_WRITE16_MEMBER(priority_w);
	void init_boogwing();
	virtual void machine_reset() override;
	virtual void video_start() override;
	uint32_t screen_update_boogwing(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void mix_boogwing(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	DECLARE_READ16_MEMBER( boogwing_protection_region_0_104_r );
	DECLARE_WRITE16_MEMBER( boogwing_protection_region_0_104_w );

	DECO16IC_BANK_CB_MEMBER(bank_callback);
	DECO16IC_BANK_CB_MEMBER(bank_callback2);
	void boogwing(machine_config &config);
	void audio_map(address_map &map);
	void boogwing_map(address_map &map);
	void decrypted_opcodes_map(address_map &map);
};
