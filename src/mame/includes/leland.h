// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Cinemat/Leland driver

*************************************************************************/

#include "machine/eepromser.h"
#include "sound/ym2151.h"
#include "sound/dac.h"
#include "sound/ay8910.h"
#include "machine/pit8253.h"
#include "cpu/i86/i186.h"

#define LELAND_BATTERY_RAM_SIZE 0x4000
#define ATAXX_EXTRA_TRAM_SIZE 0x800


struct vram_state_data
{
	uint16_t  m_addr;
	uint8_t   m_latch[2];
};

class leland_80186_sound_device;

class leland_state : public driver_device
{
public:
	leland_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_master(*this, "master"),
		m_slave(*this, "slave"),
		m_mainram(*this, "mainram"),
		m_eeprom(*this, "eeprom"),
		m_sound(*this, "custom"),
		m_dac0(*this, "dac0"),
		m_dac1(*this, "dac1"),
		m_ay8910(*this, "ay8910"),
		m_ay8912(*this, "ay8912"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette")  { }

	required_device<cpu_device> m_master;
	required_device<cpu_device> m_slave;
	required_shared_ptr<uint8_t> m_mainram;
	required_device<eeprom_serial_93cxx_device> m_eeprom;
	optional_device<leland_80186_sound_device> m_sound;
	optional_device<dac_byte_interface> m_dac0;
	optional_device<dac_byte_interface> m_dac1;
	optional_device<ay8910_device> m_ay8910;
	optional_device<ay8912_device> m_ay8912;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

	uint8_t m_dac_control;
	uint8_t *m_alleymas_kludge_mem;
	std::unique_ptr<uint8_t[]> m_ataxx_qram;
	uint8_t m_gfx_control;
	uint8_t m_wcol_enable;
	emu_timer *m_master_int_timer;
	uint8_t *m_master_base;
	uint8_t *m_slave_base;
	uint8_t *m_xrom_base;
	uint32_t m_master_length;
	uint32_t m_slave_length;
	int m_dangerz_x;
	int m_dangerz_y;
	uint8_t m_analog_result;
	uint8_t m_dial_last_input[4];
	uint8_t m_dial_last_result[4];
	uint8_t m_keycard_shift;
	uint8_t m_keycard_bit;
	uint8_t m_keycard_state;
	uint8_t m_keycard_clock;
	uint8_t m_keycard_command[3];
	uint8_t m_top_board_bank;
	uint8_t m_sound_port_bank;
	uint8_t m_alternate_bank;
	uint8_t m_master_bank;
	void (leland_state::*m_update_master_bank)();
	uint32_t m_xrom1_addr;
	uint32_t m_xrom2_addr;
	uint8_t m_battery_ram_enable;
	uint8_t *m_battery_ram;
	std::unique_ptr<uint8_t[]> m_extra_tram;
	std::unique_ptr<uint8_t[]> m_video_ram;
	struct vram_state_data m_vram_state[2];
	uint16_t m_xscroll;
	uint16_t m_yscroll;
	uint8_t m_gfxbank;
	uint16_t m_last_scanline;
	emu_timer *m_scanline_timer;

	DECLARE_READ8_MEMBER(cerberus_dial_1_r);
	DECLARE_READ8_MEMBER(cerberus_dial_2_r);
	DECLARE_WRITE8_MEMBER(alleymas_joystick_kludge);
	DECLARE_READ8_MEMBER(dangerz_input_y_r);
	DECLARE_READ8_MEMBER(dangerz_input_x_r);
	DECLARE_READ8_MEMBER(dangerz_input_upper_r);
	DECLARE_READ8_MEMBER(redline_pedal_1_r);
	DECLARE_READ8_MEMBER(redline_pedal_2_r);
	DECLARE_READ8_MEMBER(redline_wheel_1_r);
	DECLARE_READ8_MEMBER(redline_wheel_2_r);
	DECLARE_READ8_MEMBER(offroad_wheel_1_r);
	DECLARE_READ8_MEMBER(offroad_wheel_2_r);
	DECLARE_READ8_MEMBER(offroad_wheel_3_r);
	DECLARE_READ8_MEMBER(ataxx_trackball_r);
	DECLARE_READ8_MEMBER(indyheat_wheel_r);
	DECLARE_READ8_MEMBER(indyheat_analog_r);
	DECLARE_WRITE8_MEMBER(indyheat_analog_w);
	DECLARE_WRITE8_MEMBER(leland_master_alt_bankswitch_w);
	DECLARE_WRITE8_MEMBER(leland_battery_ram_w);
	DECLARE_WRITE8_MEMBER(ataxx_battery_ram_w);
	DECLARE_READ8_MEMBER(leland_master_analog_key_r);
	DECLARE_WRITE8_MEMBER(leland_master_analog_key_w);
	DECLARE_READ8_MEMBER(leland_master_input_r);
	DECLARE_WRITE8_MEMBER(leland_master_output_w);
	DECLARE_READ8_MEMBER(ataxx_master_input_r);
	DECLARE_WRITE8_MEMBER(ataxx_master_output_w);
	DECLARE_WRITE8_MEMBER(leland_gated_paletteram_w);
	DECLARE_READ8_MEMBER(leland_gated_paletteram_r);
	DECLARE_WRITE8_MEMBER(ataxx_paletteram_and_misc_w);
	DECLARE_READ8_MEMBER(ataxx_paletteram_and_misc_r);
	DECLARE_WRITE8_MEMBER(leland_slave_small_banksw_w);
	DECLARE_WRITE8_MEMBER(leland_slave_large_banksw_w);
	DECLARE_WRITE8_MEMBER(ataxx_slave_banksw_w);
	DECLARE_READ8_MEMBER(leland_raster_r);
	DECLARE_WRITE8_MEMBER(leland_scroll_w);
	DECLARE_WRITE8_MEMBER(leland_master_video_addr_w);
	DECLARE_WRITE8_MEMBER(leland_mvram_port_w);
	DECLARE_READ8_MEMBER(leland_mvram_port_r);
	DECLARE_WRITE8_MEMBER(leland_slave_video_addr_w);
	DECLARE_WRITE8_MEMBER(leland_svram_port_w);
	DECLARE_READ8_MEMBER(leland_svram_port_r);
	DECLARE_WRITE8_MEMBER(ataxx_mvram_port_w);
	DECLARE_WRITE8_MEMBER(ataxx_svram_port_w);
	DECLARE_READ8_MEMBER(ataxx_mvram_port_r);
	DECLARE_READ8_MEMBER(ataxx_svram_port_r);
	DECLARE_READ8_MEMBER(ataxx_eeprom_r);
	DECLARE_WRITE8_MEMBER(ataxx_eeprom_w);
	DECLARE_READ8_MEMBER(leland_sound_port_r);
	DECLARE_WRITE8_MEMBER(leland_sound_port_w);
	DECLARE_WRITE8_MEMBER(leland_gfx_port_w);

	DECLARE_DRIVER_INIT(dblplay);
	DECLARE_DRIVER_INIT(viper);
	DECLARE_DRIVER_INIT(quarterb);
	DECLARE_DRIVER_INIT(aafb);
	DECLARE_DRIVER_INIT(redlin2p);
	DECLARE_DRIVER_INIT(aafbb);
	DECLARE_DRIVER_INIT(dangerz);
	DECLARE_DRIVER_INIT(mayhem);
	DECLARE_DRIVER_INIT(offroad);
	DECLARE_DRIVER_INIT(pigout);
	DECLARE_DRIVER_INIT(alleymas);
	DECLARE_DRIVER_INIT(offroadt);
	DECLARE_DRIVER_INIT(teamqb);
	DECLARE_DRIVER_INIT(strkzone);
	DECLARE_DRIVER_INIT(wseries);
	DECLARE_DRIVER_INIT(powrplay);
	DECLARE_DRIVER_INIT(basebal2);
	DECLARE_DRIVER_INIT(upyoural);
	DECLARE_DRIVER_INIT(cerberus);
	DECLARE_DRIVER_INIT(aafbd2p);
	DECLARE_DRIVER_INIT(ataxx);
	DECLARE_DRIVER_INIT(ataxxj);
	DECLARE_DRIVER_INIT(wsf);
	DECLARE_DRIVER_INIT(indyheat);
	DECLARE_DRIVER_INIT(brutforc);
	DECLARE_DRIVER_INIT(asylum);
	DECLARE_MACHINE_START(ataxx);
	DECLARE_MACHINE_RESET(ataxx);
	DECLARE_MACHINE_START(leland);
	DECLARE_MACHINE_RESET(leland);
	DECLARE_VIDEO_START(leland);
	DECLARE_VIDEO_START(leland2);
	DECLARE_VIDEO_START(ataxx);

	uint32_t screen_update_leland(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_ataxx(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(leland_master_interrupt);
	TIMER_CALLBACK_MEMBER(leland_interrupt_callback);
	TIMER_CALLBACK_MEMBER(ataxx_interrupt_callback);
	TIMER_CALLBACK_MEMBER(scanline_callback);
	TIMER_CALLBACK_MEMBER(leland_delayed_mvram_w);

	void leland_video_addr_w(address_space &space, int offset, int data, int num);
	int leland_vram_port_r(address_space &space, int offset, int num);
	void leland_vram_port_w(address_space &space, int offset, int data, int num);
	int dial_compute_value(int new_val, int indx);

	void update_dangerz_xy();
	void cerberus_bankswitch();
	void mayhem_bankswitch();
	void dangerz_bankswitch();
	void basebal2_bankswitch();
	void redline_bankswitch();
	void viper_bankswitch();
	void offroad_bankswitch();
	void ataxx_bankswitch();
	void leland_init_eeprom(uint8_t default_val, const uint16_t *data, uint8_t serial_offset, uint8_t serial_type);
	void ataxx_init_eeprom(const uint16_t *data);
	int keycard_r();
	void keycard_w(int data);
	void leland_rotate_memory(const char *cpuname);
	void init_master_ports(uint8_t mvram_base, uint8_t io_base);
};


#define SERIAL_TYPE_NONE        0
#define SERIAL_TYPE_ADD         1
#define SERIAL_TYPE_ADD_XOR     2
#define SERIAL_TYPE_ENCRYPT     3
#define SERIAL_TYPE_ENCRYPT_XOR 4


/*----------- defined in audio/leland.c -----------*/

class leland_80186_sound_device : public device_t
{
public:
	leland_80186_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	leland_80186_sound_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);
	virtual machine_config_constructor device_mconfig_additions() const override;

	DECLARE_WRITE16_MEMBER(peripheral_ctrl);
	DECLARE_WRITE8_MEMBER(leland_80186_control_w);
	DECLARE_WRITE8_MEMBER(ataxx_80186_control_w);
	DECLARE_READ16_MEMBER(peripheral_r);
	DECLARE_WRITE16_MEMBER(peripheral_w);
	DECLARE_WRITE8_MEMBER(leland_80186_command_lo_w);
	DECLARE_WRITE8_MEMBER(leland_80186_command_hi_w);
	DECLARE_READ8_MEMBER(leland_80186_response_r);
	DECLARE_WRITE16_MEMBER(dac_w);
	DECLARE_WRITE16_MEMBER(ataxx_dac_control);
	DECLARE_WRITE_LINE_MEMBER(pit0_2_w);
	DECLARE_WRITE_LINE_MEMBER(pit1_0_w);
	DECLARE_WRITE_LINE_MEMBER(pit1_1_w);
	DECLARE_WRITE_LINE_MEMBER(pit1_2_w);
	DECLARE_WRITE_LINE_MEMBER(i80186_tmr0_w);
	DECLARE_WRITE_LINE_MEMBER(i80186_tmr1_w);
protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	int m_type;

	enum {
		TYPE_LELAND,
		TYPE_REDLINE,
		TYPE_ATAXX,
		TYPE_WSF
	};

	required_device<dac_byte_interface> m_dac1;
	required_device<dac_byte_interface> m_dac2;
	required_device<dac_byte_interface> m_dac3;
	required_device<dac_byte_interface> m_dac4;
	optional_device<dac_byte_interface> m_dac5;
	optional_device<dac_byte_interface> m_dac6;
	optional_device<dac_byte_interface> m_dac7;
	optional_device<dac_byte_interface> m_dac8;
	optional_device<dac_word_interface> m_dac9;
	required_device<dac_byte_interface> m_dac1vol;
	required_device<dac_byte_interface> m_dac2vol;
	required_device<dac_byte_interface> m_dac3vol;
	required_device<dac_byte_interface> m_dac4vol;
	optional_device<dac_byte_interface> m_dac5vol;
	optional_device<dac_byte_interface> m_dac6vol;
	optional_device<dac_byte_interface> m_dac7vol;
	optional_device<dac_byte_interface> m_dac8vol;

private:
	void command_lo_sync(void *ptr, int param);
	void delayed_response_r(void *ptr, int param);
	void set_clock_line(int which, int state) { m_clock_active = state ? (m_clock_active | (1<<which)) : (m_clock_active & ~(1<<which)); }

	// internal state
	i80186_cpu_device *m_audiocpu;
	uint16_t m_peripheral;
	uint8_t m_last_control;
	uint8_t m_clock_active;
	uint8_t m_clock_tick;
	uint16_t m_sound_command;
	uint16_t m_sound_response;
	uint32_t m_ext_start;
	uint32_t m_ext_stop;
	uint8_t m_ext_active;
	uint8_t* m_ext_base;

	required_device<pit8254_device> m_pit0;
	optional_device<pit8254_device> m_pit1;
	optional_device<pit8254_device> m_pit2;
	optional_device<ym2151_device> m_ymsnd;
};

extern const device_type LELAND_80186;

class redline_80186_sound_device : public leland_80186_sound_device
{
public:
	redline_80186_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	DECLARE_WRITE16_MEMBER(redline_dac_w);
	virtual machine_config_constructor device_mconfig_additions() const override;
};

extern const device_type REDLINE_80186;

class ataxx_80186_sound_device : public leland_80186_sound_device
{
public:
	ataxx_80186_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual machine_config_constructor device_mconfig_additions() const override;
};

extern const device_type ATAXX_80186;

class wsf_80186_sound_device : public leland_80186_sound_device
{
public:
	wsf_80186_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual machine_config_constructor device_mconfig_additions() const override;
};

extern const device_type WSF_80186;

ADDRESS_MAP_EXTERN(leland_80186_map_program, 16);
ADDRESS_MAP_EXTERN(leland_80186_map_io, 16);
ADDRESS_MAP_EXTERN(redline_80186_map_io, 16);
ADDRESS_MAP_EXTERN(ataxx_80186_map_io, 16);


/*----------- defined in video/leland.c -----------*/

MACHINE_CONFIG_EXTERN( leland_video );
MACHINE_CONFIG_EXTERN( ataxx_video );
