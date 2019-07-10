// license: BSD-3-Clause
// copyright-holders: Bryan McPhail, David Haywood, Dirk Best
/***************************************************************************

    Super Burger Time

***************************************************************************/

#ifndef MAME_INCLUDES_SUPBTIME_H
#define MAME_INCLUDES_SUPBTIME_H

#include "cpu/m68000/m68000.h"
#include "cpu/h6280/h6280.h"
#include "machine/decocrpt.h"
#include "machine/gen_latch.h"
#include "video/decospr.h"
#include "video/deco16ic.h"
#include "sound/ym2151.h"
#include "sound/okim6295.h"
#include "screen.h"
#include "speaker.h"

class supbtime_state : public driver_device
{
public:
	supbtime_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_spriteram(*this, "spriteram")
		, m_pf_rowscroll(*this, "pf%u_rowscroll", 1U)
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_deco_tilegen(*this, "tilegen")
		, m_sprgen(*this, "spritegen")
	{ }

	void chinatwn(machine_config &config);
	void supbtime(machine_config &config);
	void tumblep(machine_config &config);

	void init_tumblep();

private:
	DECLARE_WRITE_LINE_MEMBER(vblank_w);
	DECLARE_READ16_MEMBER(vblank_ack_r);
	uint32_t screen_update_common(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, bool use_offsets);
	uint32_t screen_update_chinatwn(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_supbtime(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_tumblep(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void chinatwn_map(address_map &map);
	void sound_map(address_map &map);
	void supbtime_map(address_map &map);
	void tumblep_map(address_map &map);

	required_shared_ptr<uint16_t> m_spriteram;
	required_shared_ptr_array<uint16_t, 2> m_pf_rowscroll;
	required_device<cpu_device> m_maincpu;
	required_device<h6280_device> m_audiocpu;
	required_device<deco16ic_device> m_deco_tilegen;
	required_device<decospr_device> m_sprgen;
};


#endif
