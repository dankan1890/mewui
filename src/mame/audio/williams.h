// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    williams.h

    Functions to emulate general the various Williams/Midway sound cards.

****************************************************************************/

#include "machine/6821pia.h"
#include "cpu/m6809/m6809.h"
#include "sound/ym2151.h"
#include "sound/okim6295.h"
#include "sound/hc55516.h"



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DECLARE_DEVICE_TYPE(WILLIAMS_CVSD_SOUND, williams_cvsd_sound_device)
DECLARE_DEVICE_TYPE(WILLIAMS_NARC_SOUND, williams_narc_sound_device)
DECLARE_DEVICE_TYPE(WILLIAMS_ADPCM_SOUND, williams_adpcm_sound_device)



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> williams_cvsd_sound_device

class williams_cvsd_sound_device :  public device_t,
									public device_mixer_interface
{
public:
	// construction/destruction
	williams_cvsd_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock = 0);

	// read/write
	void write(u16 data);
	DECLARE_WRITE_LINE_MEMBER(reset_write);

	// internal communications
	void bank_select_w(u8 data);
	void cvsd_digit_clock_clear_w(u8 data);
	void cvsd_clock_set_w(u8 data);

	void williams_cvsd_map(address_map &map);

	mc6809e_device *get_cpu() { return m_cpu; }

protected:
	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	// devices
	required_device<mc6809e_device> m_cpu;
	required_device<pia6821_device> m_pia;
	required_device<hc55516_device> m_hc55516;

	required_memory_bank m_rombank;

	// internal state
	u8 m_talkback;

	void talkback_w(u8 data);
};


// ======================> williams_narc_sound_device

class williams_narc_sound_device :  public device_t,
									public device_mixer_interface
{
public:
	// construction/destruction
	williams_narc_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock = 0);

	// read/write
	u16 read();
	void write(u16 data);
	DECLARE_WRITE_LINE_MEMBER(reset_write);

	// internal communications
	void master_bank_select_w(u8 data);
	void slave_bank_select_w(u8 data);
	u8 command_r();
	void command2_w(u8 data);
	u8 command2_r();
	void master_talkback_w(u8 data);
	void master_sync_w(u8 data);
	void slave_talkback_w(u8 data);
	void slave_sync_w(u8 data);
	void cvsd_digit_clock_clear_w(u8 data);
	void cvsd_clock_set_w(u8 data);

	void williams_narc_master_map(address_map &map);
	void williams_narc_slave_map(address_map &map);

	mc6809e_device *get_cpu() { return m_cpu[0]; }

protected:
	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	// timer IDs
	enum
	{
		TID_MASTER_COMMAND,
		TID_SLAVE_COMMAND,
		TID_SYNC_CLEAR
	};

	// devices
	required_device_array<mc6809e_device, 2> m_cpu;
	required_device<hc55516_device> m_hc55516;

	required_memory_bank m_masterbank;
	required_memory_bank m_slavebank;

	// internal state
	u8 m_latch;
	u8 m_latch2;
	u8 m_talkback;
	u8 m_audio_sync;
	u8 m_sound_int_state;
};


// ======================> williams_adpcm_sound_device

class williams_adpcm_sound_device : public device_t,
									public device_mixer_interface
{
public:
	// construction/destruction
	williams_adpcm_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock = 0);

	// read/write
	void write(u16 data);
	DECLARE_WRITE_LINE_MEMBER(reset_write);
	DECLARE_READ_LINE_MEMBER(irq_read);

	// internal communications
	void bank_select_w(u8 data);
	void oki6295_bank_select_w(u8 data);
	u8 command_r();
	void talkback_w(u8 data);

	void williams_adpcm_map(address_map &map);
	void williams_adpcm_oki_map(address_map &map);

	mc6809e_device *get_cpu() { return m_cpu; }

protected:
	// timer IDs
	enum
	{
		TID_COMMAND,
		TID_IRQ_CLEAR
	};

	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	// devices
	required_device<mc6809e_device> m_cpu;

	required_memory_bank m_rombank;
	required_memory_bank m_okibank;

	// internal state
	u8 m_latch;
	u8 m_talkback;
	u8 m_sound_int_state;
};
