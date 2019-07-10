// license:LGPL-2.1+
// copyright-holders:David Haywood, Angelo Salese, Olivier Galibert, Mariusz Wojcieszek, R. Belmont
#ifndef MAME_INCLUDES_STV_H
#define MAME_INCLUDES_STV_H

#pragma once

#include "includes/saturn.h"
#include "audio/rax.h"
#include "machine/eepromser.h"
#include "machine/ticket.h"

class stv_state : public saturn_state
{
public:
	stv_state(const machine_config &mconfig, device_type type, const char *tag)
		: saturn_state(mconfig, type, tag),
		m_cart1(*this, "stv_slot1"),
		m_cart2(*this, "stv_slot2"),
		m_cart3(*this, "stv_slot3"),
		m_cart4(*this, "stv_slot4"),
		m_rax(*this, "rax"),
		m_protbank(*this, "protbank"),
		m_eeprom(*this, "eeprom"),
		m_cryptdevice(*this, "315_5881"),
		m_5838crypt(*this, "315_5838"),
		m_hopper(*this, "hopper")
	{
	}

	void stv_slot(machine_config &config);
	void stv_cartslot(machine_config &config);
	void stv(machine_config &config);
	void hopper(machine_config &config);
	void batmanfr(machine_config &config);
	void stv_5838(machine_config &config);
	void stv_5881(machine_config &config);
	void stvcd(machine_config &config);

	void init_astrass();
	void init_batmanfr();
	void init_finlarch();
	void init_decathlt();
	void init_decathlt_nokey();
	void init_sanjeon();
	void init_puyosun();
	void init_winterht();
	void init_gaxeduel();
	void init_rsgun();
	void init_groovef();
	void init_sandor();
	void init_cottonbm();
	void init_smleague();
	void init_nameclv3();
	void init_danchiq();
	void init_hanagumi();
	void init_cotton2();
	void init_seabass();
	void init_stv();
	void init_thunt();
	void init_critcrsh();
	void init_stvmp();
	void init_sasissu();
	void init_dnmtdeka();
	void init_ffreveng();
	void init_fhboxers();
	void init_pblbeach();
	void init_sss();
	void init_diehard();
	void init_danchih();
	void init_shienryu();
	void init_elandore();
	void init_prikura();
	void init_maruchan();
	void init_colmns97();
	void init_grdforce();
	void init_suikoenb();
	void init_magzun();
	void init_shanhigw();
	void init_sokyugrt();
	void init_vfremix();
	void init_twcup98();
	void init_znpwfv();
	void init_othellos();
	void init_mausuke();
	void init_hopper();

private:
	DECLARE_READ8_MEMBER(stv_ioga_r);
	DECLARE_WRITE8_MEMBER(stv_ioga_w);
	DECLARE_READ8_MEMBER(critcrsh_ioga_r);
	DECLARE_READ8_MEMBER(magzun_ioga_r);
	DECLARE_WRITE8_MEMBER(magzun_ioga_w);
	DECLARE_READ8_MEMBER(stvmp_ioga_r);
	DECLARE_WRITE8_MEMBER(stvmp_ioga_w);
	DECLARE_READ32_MEMBER(stv_ioga_r32);
	DECLARE_WRITE32_MEMBER(stv_ioga_w32);
	DECLARE_READ32_MEMBER(critcrsh_ioga_r32);
	DECLARE_READ32_MEMBER(stvmp_ioga_r32);
	DECLARE_WRITE32_MEMBER(stvmp_ioga_w32);
	DECLARE_READ32_MEMBER(magzun_ioga_r32);
	DECLARE_WRITE32_MEMBER(magzun_ioga_w32);
	DECLARE_READ32_MEMBER(magzun_hef_hack_r);
	DECLARE_READ32_MEMBER(magzun_rx_hack_r);
	DECLARE_WRITE8_MEMBER(hop_ioga_w);
	DECLARE_WRITE32_MEMBER(hop_ioga_w32);

	image_init_result load_cart(device_image_interface &image, generic_slot_device *slot);
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER( stv_cart1 ) { return load_cart(image, m_cart1); }
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER( stv_cart2 ) { return load_cart(image, m_cart2); }
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER( stv_cart3 ) { return load_cart(image, m_cart3); }
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER( stv_cart4 ) { return load_cart(image, m_cart4); }
	optional_device<generic_slot_device> m_cart1;
	optional_device<generic_slot_device> m_cart2;
	optional_device<generic_slot_device> m_cart3;
	optional_device<generic_slot_device> m_cart4;

	void install_stvbios_speedups( void );

	DECLARE_MACHINE_START(stv);
	DECLARE_MACHINE_RESET(stv);

	DECLARE_MACHINE_RESET(batmanfr);
	DECLARE_WRITE32_MEMBER(batmanfr_sound_comms_w);
	optional_device<acclaim_rax_device> m_rax;

	uint8_t     m_port_sel,m_mux_data;
	uint8_t     m_system_output;
	uint8_t     m_ioga_mode;
	uint8_t     m_ioga_portg;
	uint16_t    m_serial_tx;

	// protection specific variables and functions (see machine/stvprot.c)
	uint32_t m_abus_protenable;
	uint32_t m_abus_protkey;

	READ32_MEMBER(decathlt_prot_r);
	void sega5838_map(address_map &map);
	optional_memory_bank m_protbank;
	bool m_newprotection_element; // debug helper only, doesn't need saving
	int m_protbankval; // debug helper only, doesn't need saving
	WRITE32_MEMBER(decathlt_prot_srcaddr_w);

	uint32_t m_a_bus[4];

	DECLARE_READ32_MEMBER( common_prot_r );
	DECLARE_WRITE32_MEMBER( common_prot_w );

	void install_common_protection();
	void stv_register_protection_savestates();

	required_device<eeprom_serial_93cxx_device> m_eeprom;
	optional_device<sega_315_5881_crypt_device> m_cryptdevice;
	optional_device<sega_315_5838_comp_device> m_5838crypt;
	optional_device<ticket_dispenser_device> m_hopper;
	uint16_t crypt_read_callback(uint32_t addr);

	DECLARE_READ8_MEMBER(pdr1_input_r);
	DECLARE_READ8_MEMBER(pdr2_input_r);
	DECLARE_WRITE8_MEMBER(pdr1_output_w);
	DECLARE_WRITE8_MEMBER(pdr2_output_w);
	void stv_select_game(int gameno);
	uint8_t     m_prev_gamebank_select;

	void sound_mem(address_map &map);
	void scsp_mem(address_map &map);
	void stv_mem(address_map &map);
	void stvcd_mem(address_map &map);
};

class stvpc_state : public stv_state
{
public:
	using stv_state::stv_state;
	static constexpr feature_type unemulated_features() { return feature::CAMERA | feature::PRINTER; }
};

//#define MASTER_CLOCK_352 57272720
//#define MASTER_CLOCK_320 53693174
#define CEF_1   m_vdp1_regs[0x010/2]|=0x0002
#define CEF_0   m_vdp1_regs[0x010/2]&=~0x0002
#define BEF_1   m_vdp1_regs[0x010/2]|=0x0001
#define BEF_0   m_vdp1_regs[0x010/2]&=~0x0001
#define STV_VDP1_TVMR ((m_vdp1_regs[0x000/2])&0xffff)
#define STV_VDP1_VBE  ((STV_VDP1_TVMR & 0x0008) >> 3)
#define STV_VDP1_TVM  ((STV_VDP1_TVMR & 0x0007) >> 0)

extern gfx_decode_entry const gfx_stv[];

#endif // MAME_INCLUDES_STV_H
