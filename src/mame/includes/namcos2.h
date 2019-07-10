// license:BSD-3-Clause
// copyright-holders:K.Wilkins
/***************************************************************************

  namcos2.h

  Common functions & declarations for the Namco System 2 driver

***************************************************************************/
#ifndef MAME_INCLUDES_NAMCOS2_H
#define MAME_INCLUDES_NAMCOS2_H

#pragma once

#include "machine/namco_c139.h"
#include "machine/namco_c148.h"
#include "machine/timer.h"
#include "sound/c140.h"
#include "video/c45.h"
#include "video/namco_c116.h"
#include "machine/namco65.h"
#include "machine/namco68.h"
#include "video/namco_c169roz.h"
#include "video/namco_c355spr.h"
#include "video/namco_c123tmap.h"
#include "video/namcos2_sprite.h"
#include "video/namcos2_roz.h"
#include "screen.h"

/*********************************************/
/* IF GAME SPECIFIC HACKS ARE REQUIRED THEN  */
/* USE THE m_gametype MEMBER TO FIND */
/* OUT WHAT GAME IS RUNNING                  */
/*********************************************/

class namcos2_state : public driver_device
{
public:
	namcos2_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_gametype(0),
		m_maincpu(*this, "maincpu"),
		m_slave(*this, "slave"),
		m_audiocpu(*this, "audiocpu"),
		m_c65(*this, "c65mcu"),
		m_c68(*this, "c68mcu"),
		m_c140(*this, "c140"),
		m_c116(*this, "c116"),
		m_c123tmap(*this, "c123tmap"),
		m_master_intc(*this, "master_intc"),
		m_slave_intc(*this, "slave_intc"),
		m_sci(*this, "sci"),
		m_c169roz(*this, "c169roz"),
		m_c355spr(*this, "c355spr"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_audiobank(*this, "audiobank"),
		m_dpram(*this, "dpram"),
		m_spriteram(*this, "spriteram"),
		m_c45_road(*this, "c45_road"),
		m_ns2sprite(*this, "s2sprite"),
		m_ns2roz(*this, "s2roz")
	{ }

	void configure_c116_standard(machine_config &config);
	void configure_c148_standard(machine_config &config);
	void configure_c65_standard(machine_config &config);
	void configure_c68_standard(machine_config &config);
	void configure_c123tmap_standard(machine_config &config);
	void metlhawk(machine_config &config);
	void gollygho(machine_config &config);
	void assaultp(machine_config &config);
	void sgunner2(machine_config &config);
	void base2(machine_config &config);
	void finallap_noio(machine_config &config);
	void finallap(machine_config &config);
	void finallap_c68(machine_config &config);
	void finalap2(machine_config &config);
	void finalap3(machine_config &config);
	void suzuka8h(machine_config &config);
	void luckywld(machine_config &config);
	void base3(machine_config &config);
	void sgunner(machine_config &config);
	void base_noio(machine_config &config);
	void base(machine_config &config);
	void base_c68(machine_config &config);

	void init_cosmogng();
	void init_sgunner2();
	void init_kyukaidk();
	void init_bubbletr();
	void init_suzuk8h2();
	void init_burnforc();
	void init_gollygho();
	void init_rthun2j();
	void init_sws();
	void init_finehour();
	void init_finallap();
	void init_dirtfoxj();
	void init_marvlanj();
	void init_sws92();
	void init_dsaber();
	void init_assault();
	void init_mirninja();
	void init_finalap2();
	void init_valkyrie();
	void init_fourtrax();
	void init_finalap3();
	void init_luckywld();
	void init_assaultj();
	void init_dsaberj();
	void init_suzuka8h();
	void init_phelios();
	void init_sws93();
	void init_metlhawk();
	void init_sws92g();
	void init_assaultp();
	void init_ordyne();
	void init_marvland();
	void init_rthun2();

private:

enum
	{
		/* Namco System 2 */
		NAMCOS2_ASSAULT = 0x1000,
		NAMCOS2_ASSAULT_JP,
		NAMCOS2_ASSAULT_PLUS,
		NAMCOS2_BUBBLE_TROUBLE,
		NAMCOS2_BURNING_FORCE,
		NAMCOS2_COSMO_GANG,
		NAMCOS2_COSMO_GANG_US,
		NAMCOS2_DIRT_FOX,
		NAMCOS2_DIRT_FOX_JP,
		NAMCOS2_DRAGON_SABER,
		NAMCOS2_FINAL_LAP,
		NAMCOS2_FINAL_LAP_2,
		NAMCOS2_FINAL_LAP_3,
		NAMCOS2_FINEST_HOUR,
		NAMCOS2_FOUR_TRAX,
		NAMCOS2_GOLLY_GHOST,
		NAMCOS2_LUCKY_AND_WILD,
		NAMCOS2_MARVEL_LAND,
		NAMCOS2_METAL_HAWK,
		NAMCOS2_MIRAI_NINJA,
		NAMCOS2_ORDYNE,
		NAMCOS2_PHELIOS,
		NAMCOS2_ROLLING_THUNDER_2,
		NAMCOS2_STEEL_GUNNER,
		NAMCOS2_STEEL_GUNNER_2,
		NAMCOS2_SUPER_WSTADIUM,
		NAMCOS2_SUPER_WSTADIUM_92,
		NAMCOS2_SUPER_WSTADIUM_92T,
		NAMCOS2_SUPER_WSTADIUM_93,
		NAMCOS2_SUZUKA_8_HOURS,
		NAMCOS2_SUZUKA_8_HOURS_2,
		NAMCOS2_VALKYRIE,
		NAMCOS2_KYUUKAI_DOUCHUUKI,
	};

	int m_gametype;
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_slave;
	required_device<cpu_device> m_audiocpu;
	optional_device<namcoc65_device> m_c65;
	optional_device<namcoc68_device> m_c68;
	required_device<c140_device> m_c140;
	required_device<namco_c116_device> m_c116;
	required_device<namco_c123tmap_device> m_c123tmap;
	required_device<namco_c148_device> m_master_intc;
	required_device<namco_c148_device> m_slave_intc;
	required_device<namco_c139_device> m_sci;
	optional_device<namco_c169roz_device> m_c169roz;
	optional_device<namco_c355spr_device> m_c355spr;
	optional_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_memory_bank m_audiobank;

	std::unique_ptr<uint8_t[]> m_eeprom;

	DECLARE_READ16_MEMBER(dpram_word_r);
	DECLARE_WRITE16_MEMBER(dpram_word_w);
	DECLARE_READ8_MEMBER(dpram_byte_r);
	DECLARE_WRITE8_MEMBER(dpram_byte_w);

	DECLARE_WRITE8_MEMBER(eeprom_w);
	DECLARE_READ8_MEMBER(eeprom_r);

	DECLARE_WRITE8_MEMBER(sound_bankselect_w);

	DECLARE_WRITE8_MEMBER(sound_reset_w);
	DECLARE_WRITE8_MEMBER(system_reset_w);
	void reset_all_subcpus(int state);

	virtual void video_start() override;
	void video_start_luckywld();
	void video_start_metlhawk();
	void video_start_sgunner();

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_finallap(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_luckywld(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_metlhawk(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_sgunner(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	DECLARE_READ8_MEMBER( c116_r );

	DECLARE_READ16_MEMBER( gfx_ctrl_r );
	DECLARE_WRITE16_MEMBER( gfx_ctrl_w );

	virtual void machine_start() override;
	virtual void machine_reset() override;

	void create_shadow_table();
	void apply_clip( rectangle &clip, const rectangle &cliprect );

	int get_pos_irq_scanline() { return (m_c116->get_reg(5) - 32) & 0xff; }
	TIMER_DEVICE_CALLBACK_MEMBER(screen_scanline);

	required_shared_ptr<uint8_t> m_dpram; /* 2Kx8 */
	optional_shared_ptr<uint16_t> m_spriteram;
	uint16_t m_gfx_ctrl;
	uint16_t m_serial_comms_ctrl[0x8];
	unsigned m_finallap_prot_count;
	int m_sendval;

	optional_device<namco_c45_road_device> m_c45_road;
	optional_device<namcos2_sprite_device> m_ns2sprite;
	optional_device<namcos2_roz_device> m_ns2roz;

	DECLARE_READ16_MEMBER( namcos2_68k_key_r );
	DECLARE_WRITE16_MEMBER( namcos2_68k_key_w );
	DECLARE_READ16_MEMBER( namcos2_finallap_prot_r );
	void GollyGhostUpdateLED_c4( int data );
	void GollyGhostUpdateLED_c6( int data );
	void GollyGhostUpdateLED_c8( int data );
	void GollyGhostUpdateLED_ca( int data );
	void GollyGhostUpdateDiorama_c0( int data );
	void TilemapCB(uint16_t code, int *tile, int *mask);
	void TilemapCB_finalap2(uint16_t code, int *tile, int *mask);
	void RozCB_luckywld(uint16_t code, int *tile, int *mask, int which);
	void RozCB_metlhawk(uint16_t code, int *tile, int *mask, int which);

	void common_default_am(address_map &map);
	void common_finallap_am(address_map &map);
	void common_suzuka8h_am(address_map &map);
	void common_metlhawk_am(address_map &map);
	void common_sgunner_am(address_map &map);
	void master_default_am(address_map &map);
	void master_finallap_am(address_map &map);
	void master_suzuka8h_am(address_map &map);
	void master_luckywld_am(address_map &map);
	void master_metlhawk_am(address_map &map);
	void master_sgunner_am(address_map &map);

	void namcos2_68k_default_cpu_board_am(address_map &map);
	void slave_default_am(address_map &map);
	void slave_finallap_am(address_map &map);
	void slave_suzuka8h_am(address_map &map);
	void slave_luckywld_am(address_map &map);
	void slave_metlhawk_am(address_map &map);
	void slave_sgunner_am(address_map &map);
	void sound_default_am(address_map &map);
};

/**************************************************************/
/* Non-shared memory custom IO device - IRQ/Inputs/Outputs   */
/**************************************************************/

#define NAMCOS2_C148_0          0       /* 0x1c0000 */
#define NAMCOS2_C148_1          1       /* 0x1c2000 */
#define NAMCOS2_C148_2          2       /* 0x1c4000 */
#define NAMCOS2_C148_CPUIRQ     3       /* 0x1c6000 */
#define NAMCOS2_C148_EXIRQ      4       /* 0x1c8000 */
#define NAMCOS2_C148_POSIRQ     5       /* 0x1ca000 */
#define NAMCOS2_C148_SERIRQ     6       /* 0x1cc000 */
#define NAMCOS2_C148_VBLANKIRQ  7       /* 0x1ce000 */

#endif // MAME_INCLUDES_NAMCOS2_H
