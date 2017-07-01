// license:BSD-3-Clause
// copyright-holders:Chris Moore, Nicola Salmoria

#include "machine/gen_latch.h"

class bublbobl_state : public driver_device
{
public:
	enum
	{
		TIMER_NMI,
		TIMER_M68705_IRQ_ACK
	};

	bublbobl_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_objectram(*this, "objectram"),
		m_mcu_sharedram(*this, "mcu_sharedram"),
		m_maincpu(*this, "maincpu"),
		m_mcu(*this, "mcu"),
		m_audiocpu(*this, "audiocpu"),
		m_slave(*this, "slave"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_soundlatch(*this, "soundlatch") { }

	/* memory pointers */
	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_objectram;
	optional_shared_ptr<uint8_t> m_mcu_sharedram;

	/* video-related */
	int      m_video_enable;

	/* sound-related */
	int      m_sound_nmi_enable;
	int      m_pending_nmi;
	int      m_sound_status;

	/* mcu-related */
	/* Tokio*/
	int      m_tokio_prot_count;
	/* Bubble Bobble MCU */
	uint8_t    m_ddr1;
	uint8_t    m_ddr2;
	uint8_t    m_ddr3;
	uint8_t    m_ddr4;
	uint8_t    m_port1_in;
	uint8_t    m_port2_in;
	uint8_t    m_port3_in;
	uint8_t    m_port4_in;
	uint8_t    m_port1_out;
	uint8_t    m_port2_out;
	uint8_t    m_port3_out;
	uint8_t    m_port4_out;
	/* Bubble Bobble 68705 */
	uint8_t    m_port_a_in;
	uint8_t    m_port_a_out;
	uint8_t    m_ddr_a;
	uint8_t    m_port_b_in;
	uint8_t    m_port_b_out;
	uint8_t    m_ddr_b;
	int      m_address;
	int      m_latch;
	/* Bobble Bobble */
	int      m_ic43_a;
	int      m_ic43_b;

	/* devices */
	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_mcu;
	required_device<cpu_device> m_audiocpu;
	required_device<cpu_device> m_slave;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<generic_latch_8_device> m_soundlatch;

	DECLARE_WRITE8_MEMBER(bublbobl_bankswitch_w);
	DECLARE_WRITE8_MEMBER(tokio_bankswitch_w);
	DECLARE_WRITE8_MEMBER(tokio_videoctrl_w);
	DECLARE_WRITE8_MEMBER(bublbobl_nmitrigger_w);
	DECLARE_READ8_MEMBER(tokio_mcu_r);
	DECLARE_READ8_MEMBER(tokiob_mcu_r);
	DECLARE_WRITE8_MEMBER(bublbobl_sound_command_w);
	DECLARE_WRITE8_MEMBER(bublbobl_sh_nmi_disable_w);
	DECLARE_WRITE8_MEMBER(bublbobl_sh_nmi_enable_w);
	DECLARE_WRITE8_MEMBER(bublbobl_soundcpu_reset_w);
	DECLARE_READ8_MEMBER(bublbobl_sound_status_r);
	DECLARE_WRITE8_MEMBER(bublbobl_sound_status_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_ddr1_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_ddr1_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_ddr2_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_ddr2_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_ddr3_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_ddr3_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_ddr4_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_ddr4_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_port1_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_port1_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_port2_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_port2_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_port3_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_port3_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_port4_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_port4_w);
	DECLARE_READ8_MEMBER(boblbobl_ic43_a_r);
	DECLARE_WRITE8_MEMBER(boblbobl_ic43_a_w);
	DECLARE_WRITE8_MEMBER(boblbobl_ic43_b_w);
	DECLARE_READ8_MEMBER(boblbobl_ic43_b_r);
	DECLARE_READ8_MEMBER(bublbobl_68705_port_a_r);
	DECLARE_WRITE8_MEMBER(bublbobl_68705_port_a_w);
	DECLARE_WRITE8_MEMBER(bublbobl_68705_ddr_a_w);
	DECLARE_READ8_MEMBER(bublbobl_68705_port_b_r);
	DECLARE_WRITE8_MEMBER(bublbobl_68705_port_b_w);
	DECLARE_WRITE8_MEMBER(bublbobl_68705_ddr_b_w);
	DECLARE_DRIVER_INIT(tokiob);
	DECLARE_DRIVER_INIT(tokio);
	DECLARE_DRIVER_INIT(dland);
	DECLARE_DRIVER_INIT(bublbobl);
	DECLARE_MACHINE_START(tokio);
	DECLARE_MACHINE_RESET(tokio);
	DECLARE_MACHINE_START(bublbobl);
	DECLARE_MACHINE_RESET(bublbobl);
	DECLARE_MACHINE_START(boblbobl);
	DECLARE_MACHINE_RESET(boblbobl);
	DECLARE_MACHINE_START(bub68705);
	DECLARE_MACHINE_RESET(bub68705);
	DECLARE_MACHINE_START(common);
	DECLARE_MACHINE_RESET(common);
	uint32_t screen_update_bublbobl(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(bublbobl_m68705_interrupt);
	void configure_banks(  );

protected:
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
};
