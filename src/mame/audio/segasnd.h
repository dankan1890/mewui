// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Sega g80 common sound hardware

*************************************************************************/

#include "cpu/mcs48/mcs48.h"

class speech_sound_device : public device_t,
									public device_sound_interface
{
public:
	speech_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	~speech_sound_device() {}

	DECLARE_WRITE8_MEMBER( data_w );
	DECLARE_WRITE8_MEMBER( control_w );

	DECLARE_READ8_MEMBER( t0_r );
	DECLARE_READ8_MEMBER( t1_r );
	DECLARE_READ8_MEMBER( p1_r );
	DECLARE_READ8_MEMBER( rom_r );
	DECLARE_WRITE8_MEMBER( p1_w );
	DECLARE_WRITE8_MEMBER( p2_w );

	DECLARE_WRITE_LINE_MEMBER(drq_w);

protected:
	// device-level overrides
	virtual void device_start() override;

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples) override;

private:
	// internal state
	uint8_t m_drq;
	uint8_t m_latch;
	uint8_t m_t0;
	uint8_t m_p2;
	uint8_t *m_speech;

	TIMER_CALLBACK_MEMBER( delayed_speech_w );
};

extern const device_type SEGASPEECH;

MACHINE_CONFIG_EXTERN( sega_speech_board );




struct g80_filter_state
{
		g80_filter_state():
		capval(0),
		exponent(0) {}

	double              capval;             /* current capacitor value */
	double              exponent;           /* constant exponent */
};


struct timer8253_channel
{
		timer8253_channel():
		holding(0),
		latchmode(0),
		latchtoggle(0),
		clockmode(0),
		bcdmode(0),
		output(0),
		lastgate(0),
		gate(0),
		subcount(0),
		count(0),
		remain(0) {}

	uint8_t               holding;            /* holding until counts written? */
	uint8_t               latchmode;          /* latching mode */
	uint8_t               latchtoggle;        /* latching state */
	uint8_t               clockmode;          /* clocking mode */
	uint8_t               bcdmode;            /* BCD mode? */
	uint8_t               output;             /* current output value */
	uint8_t               lastgate;           /* previous gate value */
	uint8_t               gate;               /* current gate value */
	uint8_t               subcount;           /* subcount (2MHz clocks per input clock) */
	uint16_t              count;              /* initial count */
	uint16_t              remain;             /* current down counter value */
};


struct timer8253
{
		timer8253()
		{
		env[0] = 0;
		env[1] = 0;
		env[2] = 0;
		config = 0;
		}

	timer8253_channel   chan[3];            /* three channels' worth of information */
	double              env[3];             /* envelope value for each channel */
	g80_filter_state        chan_filter[2];     /* filter states for the first two channels */
	g80_filter_state        gate1;              /* first RC filter state */
	g80_filter_state        gate2;              /* second RC filter state */
	uint8_t               config;             /* configuration for this timer */
};


class usb_sound_device : public device_t,
									public device_sound_interface
{
public:
	usb_sound_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);
	usb_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	~usb_sound_device() {}
	required_device<i8035_device> m_ourcpu;                 /* CPU index of the 8035 */

	DECLARE_READ8_MEMBER( status_r );
	DECLARE_WRITE8_MEMBER( data_w );
	DECLARE_READ8_MEMBER( ram_r );
	DECLARE_WRITE8_MEMBER( ram_w );

	DECLARE_READ8_MEMBER( p1_r );
	DECLARE_WRITE8_MEMBER( p1_w );
	DECLARE_WRITE8_MEMBER( p2_w );
	DECLARE_READ8_MEMBER( t1_r );

	DECLARE_READ8_MEMBER( workram_r );
	DECLARE_WRITE8_MEMBER( workram_w );

	TIMER_DEVICE_CALLBACK_MEMBER( increment_t1_clock_timer_cb );

protected:
	// device-level overrides
	virtual machine_config_constructor device_mconfig_additions() const override;
	virtual void device_start() override;
	virtual void device_reset() override;

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples) override;

private:
	// internal state
	sound_stream        *m_stream;              /* output stream */
	device_t            *m_maincpu;
	uint8_t               m_in_latch;             /* input latch */
	uint8_t               m_out_latch;            /* output latch */
	uint8_t               m_last_p2_value;        /* current P2 output value */
	optional_shared_ptr<uint8_t> m_program_ram;          /* pointer to program RAM */
	required_shared_ptr<uint8_t> m_work_ram;             /* pointer to work RAM */
	uint8_t               m_work_ram_bank;        /* currently selected work RAM bank */
	uint8_t               m_t1_clock;             /* T1 clock value */
	uint8_t               m_t1_clock_mask;        /* T1 clock mask (configured via jumpers) */
	timer8253           m_timer_group[3];       /* 3 groups of timers */
	uint8_t               m_timer_mode[3];        /* mode control for each group */
	uint32_t              m_noise_shift;
	uint8_t               m_noise_state;
	uint8_t               m_noise_subcount;
	double              m_gate_rc1_exp[2];
	double              m_gate_rc2_exp[2];
	g80_filter_state        m_final_filter;
	g80_filter_state        m_noise_filters[5];

	TIMER_CALLBACK_MEMBER( delayed_usb_data_w );
	void timer_w(int which, uint8_t offset, uint8_t data);
	void env_w(int which, uint8_t offset, uint8_t data);
};

extern const device_type SEGAUSB;

class usb_rom_sound_device : public usb_sound_device
{
public:
	usb_rom_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	~usb_rom_sound_device() {}

protected:
	// device-level overrides
	virtual machine_config_constructor device_mconfig_additions() const override;
};

extern const device_type SEGAUSBROM;

#define MCFG_SEGAUSB_ADD(_tag) \
	MCFG_SOUND_ADD(_tag, SEGAUSB, 0) \
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "speaker", 1.0)

	#define MCFG_SEGAUSBROM_ADD(_tag) \
	MCFG_SOUND_ADD(_tag, SEGAUSBROM, 0) \
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "speaker", 1.0)
