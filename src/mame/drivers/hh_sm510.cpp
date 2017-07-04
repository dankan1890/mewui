// license:BSD-3-Clause
// copyright-holders:hap, Sean Riddle
// thanks-to:Igor, ChoccyHobNob, RColtrane
/***************************************************************************

  Sharp SM5xx family handhelds.

  TODO:
  - improve LCD segments in SVGs for: gnw_mc25, gnw_eg26, exospace
  - SVG background/foreground vector graphics where possible. Doesn't apply to eg. the
    Konami games where MAME's SVG renderer needs to add support for embedded images.
  - confirm gnw_mc25/gnw_eg26 rom (dumped from Soviet clone, but pretty confident that it's same)

***************************************************************************/

#include "emu.h"
#include "cpu/sm510/sm510.h"
#include "cpu/sm510/sm500.h"
#include "sound/spkrdev.h"
#include "rendlay.h"
#include "screen.h"
#include "speaker.h"

// internal artwork
#include "gnw_dualv.lh"
#include "gnw_dualh.lh"
//#include "hh_sm510_test.lh" // common test-layout - use external artwork
//#include "hh_sm500_test.lh" // "


class hh_sm510_state : public driver_device
{
public:
	hh_sm510_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_inp_matrix(*this, "IN.%u", 0),
		m_speaker(*this, "speaker"),
		m_inp_lines(0),
		m_display_wait(33)
	{ }

	// devices
	required_device<cpu_device> m_maincpu;
	optional_ioport_array<7> m_inp_matrix; // max 7
	optional_device<speaker_sound_device> m_speaker;

	// misc common
	u16 m_inp_mux;                  // multiplexed inputs mask
	int m_inp_lines;                // number of input mux columns
	
	u8 read_inputs(int columns);

	virtual void update_k_line();
	virtual DECLARE_INPUT_CHANGED_MEMBER(input_changed);
	virtual DECLARE_INPUT_CHANGED_MEMBER(acl_button);
	virtual DECLARE_WRITE16_MEMBER(sm510_lcd_segment_w);
	virtual DECLARE_WRITE8_MEMBER(sm500_lcd_segment_w);
	virtual DECLARE_READ8_MEMBER(input_r);
	virtual DECLARE_WRITE8_MEMBER(input_w);
	virtual DECLARE_WRITE8_MEMBER(piezo_r1_w);
	virtual DECLARE_WRITE8_MEMBER(piezo_r2_w);
	virtual DECLARE_WRITE8_MEMBER(piezo_input_w);

	// display common
	int m_display_wait;             // lcd segment on/off-delay in milliseconds (default 33ms)
	u8 m_display_x_len;             // lcd number of groups
	u8 m_display_y_len;             // lcd number of segments
	u8 m_display_z_len;             // lcd number of commons
	u32 m_display_state[0x20];      // lcd segment data (max. 5-bit offset)
	u8 m_display_decay[0x20][0x20]; // (internal use)
	u8 m_display_cache[0x20][0x20]; // (internal use)

	void set_display_size(u8 x, u8 y, u8 z);
	TIMER_DEVICE_CALLBACK_MEMBER(display_decay_tick);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
};


// machine start/reset

void hh_sm510_state::machine_start()
{
	// zerofill
	m_inp_mux = 0;
	/* m_inp_lines = 0; */ // not here
	m_display_x_len = 0;
	m_display_y_len = 0;
	m_display_z_len = 0;
	memset(m_display_state, 0, sizeof(m_display_state));
	memset(m_display_decay, 0, sizeof(m_display_decay));
	memset(m_display_cache, ~0, sizeof(m_display_cache));

	// register for savestates
	save_item(NAME(m_inp_mux));
	save_item(NAME(m_inp_lines));
	save_item(NAME(m_display_x_len));
	save_item(NAME(m_display_y_len));
	save_item(NAME(m_display_z_len));
	save_item(NAME(m_display_state));
	save_item(NAME(m_display_decay));
	/* save_item(NAME(m_display_cache)); */ // don't save!
}

void hh_sm510_state::machine_reset()
{
}



/***************************************************************************

  Helper Functions

***************************************************************************/

// lcd panel - on lcd handhelds, usually not a generic x/y screen device
// deflicker here, especially needed for SM500/SM5A with the active shift register

TIMER_DEVICE_CALLBACK_MEMBER(hh_sm510_state::display_decay_tick)
{
	u8 z_mask = (1 << m_display_z_len) - 1;
	u8 zx_len = 1 << (m_display_x_len + m_display_z_len);
	
	for (int zx = 0; zx < zx_len; zx++)
	{
		for (int y = 0; y < m_display_y_len; y++)
		{
			// delay lcd segment on/off state
			if (m_display_state[zx] >> y & 1)
			{
				if (m_display_decay[y][zx] < (2 * m_display_wait - 1))
					m_display_decay[y][zx]++;
			}
			else if (m_display_decay[y][zx] > 0)
				m_display_decay[y][zx]--;
			u8 active_state = (m_display_decay[y][zx] < m_display_wait) ? 0 : 1;

			if (active_state != m_display_cache[y][zx])
			{
				// SM510 series: output to x.y.z, where:
				// x = group a/b/bs/c (0/1/2/3)
				// y = segment 1-16 (0-15)
				// z = common H1-H4 (0-3)

				// SM500 series: output to x.y.z, where:
				// x = O group (0-*)
				// y = O segment 1-4 (0-3)
				// z = common H1/H2 (0/1)
				char buf[0x10];
				sprintf(buf, "%d.%d.%d", zx >> m_display_z_len, y, zx & z_mask);
				output().set_value(buf, active_state);
				
				m_display_cache[y][zx] = active_state;
			}
		}
	}
}

void hh_sm510_state::set_display_size(u8 x, u8 y, u8 z)
{
	// x = groups(in bits)
	// y = number of segments per group
	// z = commons(in bits)
	m_display_x_len = x;
	m_display_y_len = y;
	m_display_z_len = z;
}

WRITE16_MEMBER(hh_sm510_state::sm510_lcd_segment_w)
{
	set_display_size(2, 16, 2);
	m_display_state[offset] = data;
}

WRITE8_MEMBER(hh_sm510_state::sm500_lcd_segment_w)
{
	set_display_size(4, 4, 1);
	m_display_state[offset] = data;
}


// generic input handlers - usually S output is input mux, and K input for buttons

u8 hh_sm510_state::read_inputs(int columns)
{
	u8 ret = 0;

	// read selected input rows
	for (int i = 0; i < columns; i++)
		if (m_inp_mux >> i & 1)
			ret |= m_inp_matrix[i]->read();

	return ret;
}

void hh_sm510_state::update_k_line()
{
	// this is necessary because the MCU can wake up on K input activity
	m_maincpu->set_input_line(SM510_INPUT_LINE_K, read_inputs(m_inp_lines) ? ASSERT_LINE : CLEAR_LINE);
}

INPUT_CHANGED_MEMBER(hh_sm510_state::input_changed)
{
	update_k_line();
}

WRITE8_MEMBER(hh_sm510_state::input_w)
{
	m_inp_mux = data;
	update_k_line();
}

READ8_MEMBER(hh_sm510_state::input_r)
{
	return read_inputs(m_inp_lines);
}

INPUT_CHANGED_MEMBER(hh_sm510_state::acl_button)
{
	// ACL button is directly tied to MCU ACL pin
	m_maincpu->set_input_line(SM510_INPUT_LINE_ACL, newval ? ASSERT_LINE : CLEAR_LINE);
}


// other generic output handlers

WRITE8_MEMBER(hh_sm510_state::piezo_r1_w)
{
	// R1 to piezo (SM511 R pin is melody output)
	m_speaker->level_w(data & 1);
}

WRITE8_MEMBER(hh_sm510_state::piezo_r2_w)
{
	// R2 to piezo
	m_speaker->level_w(data >> 1 & 1);
}

WRITE8_MEMBER(hh_sm510_state::piezo_input_w)
{
	// R1 to piezo, other to input mux
	piezo_r1_w(space, 0, data & 1);
	input_w(space, 0, data >> 1);
}





/***************************************************************************

  Minidrivers (subclass, I/O, Inputs, Machine Config)

***************************************************************************/

/***************************************************************************

  Konami Top Gun
  * PCB label BH003
  * Sharp SM510 under epoxy (die label CMS54C, KMS598)

***************************************************************************/

class ktopgun_state : public hh_sm510_state
{
public:
	ktopgun_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 3;
	}
};

// config

static INPUT_PORTS_START( ktopgun )
	PORT_START("IN.0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_VOLUME_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Sound")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Power On/Start")
	PORT_BIT( 0x0e, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("All Clear")
INPUT_PORTS_END

static MACHINE_CONFIG_START( ktopgun )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM510, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1611, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1611-1, 0, 1080-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Konami Contra
  * PCB label BH002
  * Sharp SM511 under epoxy (die label KMS73B, KMS773)

  Contra handheld is titled simply "C" in the USA.

***************************************************************************/

class kcontra_state : public hh_sm510_state
{
public:
	kcontra_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 3;
	}
};

// config

static INPUT_PORTS_START( kcontra )
	PORT_START("IN.0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_VOLUME_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Sound")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Power On/Start")
	PORT_BIT( 0x0e, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("All Clear")
INPUT_PORTS_END

static MACHINE_CONFIG_START( kcontra )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM511, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1501, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1501-1, 0, 1080-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Konami Teenage Mutant Ninja Turtles
  * PCB label BH005
  * Sharp SM511 under epoxy (die label KMS73B, KMS774)

***************************************************************************/

class ktmnt_state : public hh_sm510_state
{
public:
	ktmnt_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 3;
	}
};

// config

static INPUT_PORTS_START( ktmnt )
	PORT_START("IN.0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_VOLUME_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Sound")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Power On/Start")

	PORT_START("IN.2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNUSED ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("All Clear")
INPUT_PORTS_END

static MACHINE_CONFIG_START( ktmnt )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM511, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1380, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1380-1, 0, 1080-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Konami Gradius
  * PCB label BH004
  * Sharp SM511 under epoxy (die label KMS73B, KMS774)

  Known in Japan as Nemesis.

***************************************************************************/

class kgradius_state : public hh_sm510_state
{
public:
	kgradius_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 2;
	}
};

// config

static INPUT_PORTS_START( kgradius )
	PORT_START("IN.0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Power On/Start")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_VOLUME_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Sound")

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("All Clear")
INPUT_PORTS_END

static MACHINE_CONFIG_START( kgradius )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM511, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1435, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1435-1, 0, 1080-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Konami Lone Ranger
  * PCB label BH009
  * Sharp SM511 under epoxy (die label KMS73B, KMS781)

***************************************************************************/

class kloneran_state : public hh_sm510_state
{
public:
	kloneran_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 2;
	}
};

// config

static INPUT_PORTS_START( kloneran )
	PORT_START("IN.0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_VOLUME_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Sound")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Pause")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Power On/Start")

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("All Clear")
INPUT_PORTS_END

static MACHINE_CONFIG_START( kloneran )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM511, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1495, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1495-1, 0, 1080-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Nintendo Game & Watch: Mickey Mouse (model MC-25), Egg (model EG-26)
  * Sharp SM5A label ?

  MC-25 and EG-26 are the same game, it's assumed that the latter was for
  regions where Nintendo wasn't able to license from Disney.

  In 1984, Elektronika(USSR) released a clone: Nu, pogodi! This was followed
  by several other titles that were the same under the hood, only differing
  in graphics. They also made a slightly modified version, adding a new game
  mode (by pressing A+B) where the player/CPU roles are reversed. This version
  is known as Razvedciki kosmosa (export version: Explorers of Space).

***************************************************************************/

class mc25_state : public hh_sm510_state
{
public:
	mc25_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 3;
	}
};

// config

static INPUT_PORTS_START( mc25 )
	PORT_START("IN.0")
	PORT_BIT( 0x0f, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_16WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_16WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_16WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_16WAY

	PORT_START("IN.2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Time")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game B")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game A")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Alarm")

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("ACL")

	PORT_START("BA") // MCU BA(alpha) pin pulled to GND, only works after power-on
	PORT_CONFNAME( 0x01, 0x01, "Infinite Lives (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( exospace )
	PORT_INCLUDE( mc25 )

	PORT_MODIFY("BA")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static MACHINE_CONFIG_START( mc25 )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM5A, XTAL_32_768kHz)
	MCFG_SM500_WRITE_O_CB(WRITE8(hh_sm510_state, sm500_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_input_w))
	MCFG_SM510_READ_BA_CB(IOPORT("BA"))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1711, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1711-1, 0, 1080-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( eg26, mc25 )

	/* video hardware */
	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_SIZE(1694, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1694-1, 0, 1080-1)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( nupogodi, mc25 )

	/* basic machine hardware */
	MCFG_CPU_REPLACE("maincpu", KB1013VK12, XTAL_32_768kHz)
	MCFG_SM500_WRITE_O_CB(WRITE8(hh_sm510_state, sm500_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_input_w))
	MCFG_SM510_READ_BA_CB(IOPORT("BA"))

	/* video hardware */
	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_SIZE(1724, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1724-1, 0, 1080-1)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( exospace, nupogodi )

	/* video hardware */
	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_SIZE(1756, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1756-1, 0, 1080-1)
MACHINE_CONFIG_END





/***************************************************************************

  Nintendo Game & Watch: Mickey & Donald (model DM-53)
  * PCB label DM-53
  * Sharp SM510 label DM-53 52ZC (die label CMS54C, CMS565)

***************************************************************************/

class dm53_state : public hh_sm510_state
{
public:
	dm53_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 2;
	}
};

// config

static INPUT_PORTS_START( dm53 )
	PORT_START("IN.0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Time")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game B")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game A")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Alarm")

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("ACL")

	PORT_START("B") // MCU B(beta) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "Infinite Lives (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static MACHINE_CONFIG_START( dm53 )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM510, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r2_w))
	MCFG_SM510_READ_B_CB(IOPORT("B"))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen_top", "svg_top")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1920/2, 1238/2)
	MCFG_SCREEN_VISIBLE_AREA(0, 1920/2-1, 0, 1238/2-1)

	MCFG_SCREEN_SVG_ADD("screen_bottom", "svg_bottom")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1920/2, 1219/2)
	MCFG_SCREEN_VISIBLE_AREA(0, 1920/2-1, 0, 1219/2-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_gnw_dualv)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Nintendo Game & Watch: Donkey Kong II (model JR-55)
  * PCB label JR-55
  * Sharp SM510 label JR-55 53YC (die label CMS54C, KMS560)

***************************************************************************/

class jr55_state : public hh_sm510_state
{
public:
	jr55_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 3;
	}
};

// config

static INPUT_PORTS_START( jr55 )
	PORT_START("IN.0")
	PORT_BIT( 0x07, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Time")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game B")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game A")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Alarm")

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("ACL")

	PORT_START("B") // MCU B(beta) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "Invincibility (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static MACHINE_CONFIG_START( jr55 )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM510, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))
	MCFG_SM510_READ_B_CB(IOPORT("B"))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen_top", "svg_top")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1920/2, 1229/2)
	MCFG_SCREEN_VISIBLE_AREA(0, 1920/2-1, 0, 1229/2-1)

	MCFG_SCREEN_SVG_ADD("screen_bottom", "svg_bottom")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1920/2, 1238/2)
	MCFG_SCREEN_VISIBLE_AREA(0, 1920/2-1, 0, 1238/2-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_gnw_dualv)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Nintendo Game & Watch: Mario Bros. (model MW-56)
  * PCB label MW-56-M
  * Sharp SM510 label MW-56 533C (no decap)

***************************************************************************/

class mw56_state : public hh_sm510_state
{
public:
	mw56_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 2;
	}
};

// config

static INPUT_PORTS_START( mw56 )
	PORT_START("IN.0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICKRIGHT_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICKLEFT_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Time")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game B")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game A")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Alarm")

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("ACL")

	PORT_START("BA") // MCU BA(alpha) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "Increase Score (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )

	PORT_START("B") // MCU B(beta) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "Infinite Lives (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static MACHINE_CONFIG_START( mw56 )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM510, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))
	MCFG_SM510_READ_BA_CB(IOPORT("BA"))
	MCFG_SM510_READ_B_CB(IOPORT("B"))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen_left", "svg_left")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(2258/2, 1440/2)
	MCFG_SCREEN_VISIBLE_AREA(0, 2258/2-1, 0, 1440/2-1)

	MCFG_SCREEN_SVG_ADD("screen_right", "svg_right")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(2261/2, 1440/2)
	MCFG_SCREEN_VISIBLE_AREA(0, 2261/2-1, 0, 1440/2-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_gnw_dualh)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Nintendo Game & Watch: Donkey Kong Jr. (model DJ-101)
  * Sharp SM510 label DJ-101 52ZA (no decap)

  This is the new wide screen version, there's also a tabletop version that
  plays more like the arcade game.

***************************************************************************/

class dj101_state : public hh_sm510_state
{
public:
	dj101_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 3;
	}
};

// config

static INPUT_PORTS_START( dj101 )
	PORT_START("IN.0")
	PORT_BIT( 0x07, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)

	PORT_START("IN.2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Time")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game B")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game A")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Alarm")

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("ACL")

	PORT_START("BA") // MCU BA(alpha) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "Increase Score (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )

	PORT_START("B") // MCU B(beta) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "Invincibility (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static MACHINE_CONFIG_START( dj101 )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM510, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))
	MCFG_SM510_READ_BA_CB(IOPORT("BA"))
	MCFG_SM510_READ_B_CB(IOPORT("B"))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1665, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1665-1, 0, 1080-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Nintendo Game & Watch: Mario's Cement Factory (model ML-102)
  * Sharp SM510 label ML-102 298D (die label CMS54C, KMS577)

  This is the new wide screen version, there's also a tabletop version.

***************************************************************************/

class ml102_state : public hh_sm510_state
{
public:
	ml102_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 2;
	}
};

// config

static INPUT_PORTS_START( ml102 )
	PORT_START("IN.0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_16WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_16WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Time")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game B")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game A")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Alarm")

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("ACL")

	PORT_START("BA") // MCU BA(alpha) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "Increase Score (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )

	PORT_START("B") // MCU B(beta) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "Infinite Lives (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static MACHINE_CONFIG_START( ml102 )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM510, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))
	MCFG_SM510_READ_BA_CB(IOPORT("BA"))
	MCFG_SM510_READ_B_CB(IOPORT("B"))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1647, 1080)
	MCFG_SCREEN_VISIBLE_AREA(0, 1647-1, 0, 1080-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Nintendo Game & Watch: Boxing (model BX-301)
  * Sharp SM511 label BX-301 287C (die label KMS73B, KMS744)

  Also known as Punch-Out!! in the USA.

***************************************************************************/

class bx301_state : public hh_sm510_state
{
public:
	bx301_state(const machine_config &mconfig, device_type type, const char *tag)
		: hh_sm510_state(mconfig, type, tag)
	{
		m_inp_lines = 7;
	}
};

// config

static INPUT_PORTS_START( bx301 )
	PORT_START("IN.0")
	PORT_BIT( 0x0b, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_PLAYER(2)

	PORT_START("IN.1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x0e, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.2")
	PORT_BIT( 0x03, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_PLAYER(2)

	PORT_START("IN.3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.4")
	PORT_BIT( 0x03, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_PLAYER(2)

	PORT_START("IN.5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr)
	PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SELECT ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Time")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game B")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Game A")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_SERVICE2 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, input_changed, nullptr) PORT_NAME("Alarm")

	PORT_START("ACL")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_CHANGED_MEMBER(DEVICE_SELF, hh_sm510_state, acl_button, nullptr) PORT_NAME("ACL")

	PORT_START("BA") // MCU BA(alpha) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "P2 Decrease Health (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )

	PORT_START("B") // MCU B(beta) pin pulled to GND
	PORT_CONFNAME( 0x01, 0x01, "P1 Infinite Health (Cheat)")
	PORT_CONFSETTING(    0x01, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static MACHINE_CONFIG_START( bx301 )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", SM511, XTAL_32_768kHz)
	MCFG_SM510_WRITE_SEGS_CB(WRITE16(hh_sm510_state, sm510_lcd_segment_w))
	MCFG_SM510_READ_K_CB(READ8(hh_sm510_state, input_r))
	MCFG_SM510_WRITE_S_CB(WRITE8(hh_sm510_state, input_w))
	MCFG_SM510_WRITE_R_CB(WRITE8(hh_sm510_state, piezo_r1_w))
	MCFG_SM510_READ_BA_CB(IOPORT("BA"))
	MCFG_SM510_READ_B_CB(IOPORT("B"))

	/* video hardware */
	MCFG_SCREEN_SVG_ADD("screen", "svg")
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(1920, 529)
	MCFG_SCREEN_VISIBLE_AREA(0, 1920-1, 0, 529-1)

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", hh_sm510_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_svg)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_CONFIG_END





/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( ktopgun ) // except for filler/unused bytes, ROM listing in patent US5137277 "BH003 Top Gun" is same
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "cms54c_kms598", 0x0000, 0x1000, CRC(50870b35) SHA1(cda1260c2e1c180995eced04b7d7ff51616dcef5) )

	ROM_REGION( 423308, "svg", 0)
	ROM_LOAD( "ktopgun.svg", 0, 423308, CRC(1beb2f6c) SHA1(0802dc10b7a28ab1143dde083816e5cea6879a6b) )
ROM_END


ROM_START( kcontra ) // except for filler/unused bytes, ROM listing in patent US5120057 "BH002 C (Contra)" program/melody is same
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "kms73b_kms773.program", 0x0000, 0x1000, CRC(bf834877) SHA1(055dd56ec16d63afba61ab866481fd9c029fb54d) )

	ROM_REGION( 0x100, "maincpu:melody", 0 )
	ROM_LOAD( "kms73b_kms773.melody", 0x000, 0x100, CRC(23d02b99) SHA1(703938e496db0eeacd14fe7605d4b5c39e0a5bc8) )

	ROM_REGION( 710421, "svg", 0)
	ROM_LOAD( "kcontra.svg", 0, 710421, CRC(7926115d) SHA1(5ed98f2a39f43b06e9670b85b7e788466c5b9f36) )
ROM_END


ROM_START( ktmnt ) // except for filler/unused bytes, ROM listing in patent US5150899 "BH005 TMNT" program/melody is same
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "kms73b_kms774.program", 0x0000, 0x1000, CRC(a1064f87) SHA1(92156c35fbbb414007ee6804fe635128a741d5f1) )

	ROM_REGION( 0x100, "maincpu:melody", 0 )
	ROM_LOAD( "kms73b_kms774.melody", 0x000, 0x100, CRC(8270d626) SHA1(bd91ca1d5cd7e2a62eef05c0033b19dcdbe441ca) )

	ROM_REGION( 607415, "svg", 0)
	ROM_LOAD( "ktmnt.svg", 0, 607415, CRC(508cca0d) SHA1(e295abf39a33a5d4d6a41ad8f294d73857262494) )
ROM_END


ROM_START( kgradius )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "kms73b_kms771.program", 0x0000, 0x1000, CRC(830c2afc) SHA1(bb9ebd4e52831cc02cd92dd4b37675f34cf37b8c) )

	ROM_REGION( 0x100, "maincpu:melody", 0 )
	ROM_LOAD( "kms73b_kms771.melody", 0x000, 0x100, CRC(4c586b73) SHA1(14c5ab2898013a577f678970a648c374749cc66d) )

	ROM_REGION( 628686, "svg", 0)
	ROM_LOAD( "kgradius.svg", 0, 628686, CRC(eb0c8778) SHA1(f58e206c443f0514dad988dc04a011f3e4b3ecf5) )
ROM_END


ROM_START( kloneran )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "kms73b_kms781.program", 0x0000, 0x1000, CRC(52b9735f) SHA1(06c5ef6e7e781b1176d4c1f2445f765ccf18b3f7) )

	ROM_REGION( 0x100, "maincpu:melody", 0 )
	ROM_LOAD( "kms73b_kms781.melody", 0x000, 0x100, CRC(a393de36) SHA1(55089f04833ccb318524ab2b584c4817505f4019) )

	ROM_REGION( 630174, "svg", 0)
	ROM_LOAD( "kloneran.svg", 0, 630174, CRC(39254bd1) SHA1(c00605e818e9327cee63d2438be011510ec8ec73) )
ROM_END


ROM_START( gnw_mc25 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "mc-25", 0x0000, 0x0740, BAD_DUMP CRC(cb820c32) SHA1(7e94fc255f32db725d5aa9e196088e490c1a1443) ) // dumped from Soviet clone

	ROM_REGION( 99701, "svg", 0)
	ROM_LOAD( "gnw_mc25.svg", 0, 99701, CRC(390f53fb) SHA1(8eba84b7b084c3a1b26840a93030eeaf7ee89227) )
ROM_END

ROM_START( gnw_eg26 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "eg-26", 0x0000, 0x0740, BAD_DUMP CRC(cb820c32) SHA1(7e94fc255f32db725d5aa9e196088e490c1a1443) ) // dumped from Soviet clone

	ROM_REGION( 100220, "svg", 0)
	ROM_LOAD( "gnw_eg26.svg", 0, 100220, CRC(b53664a3) SHA1(4d45f4f78a9c6d3ee1e6bdac64cbb1fbea5ba4ba) )
ROM_END

ROM_START( nupogodi )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "nupogodi.bin", 0x0000, 0x0740, CRC(cb820c32) SHA1(7e94fc255f32db725d5aa9e196088e490c1a1443) )

	ROM_REGION( 123824, "svg", 0)
	ROM_LOAD( "nupogodi.svg", 0, 123824, CRC(fb227607) SHA1(7b41c83a4b1494d76db18ff649541629de9bdc18) )
ROM_END

ROM_START( exospace )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "exospace.bin", 0x0000, 0x0740, CRC(553e2b09) SHA1(2b74f8437b881fbb62b61f25435a5bfc66872a9a) )

	ROM_REGION( 63138, "svg", 0)
	ROM_LOAD( "exospace.svg", 0, 63138, CRC(a17dad7e) SHA1(714bc200b81c8e6bf0c07c0eabf85442a1791e58) )
ROM_END


ROM_START( gnw_dm53 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "dm-53_cms54c_cms565", 0x0000, 0x1000, CRC(e21fc0f5) SHA1(3b65ccf9f98813319410414e11a3231b787cdee6) )

	ROM_REGION( 124103, "svg_top", 0)
	ROM_LOAD( "gnw_dm53_top.svg", 0, 124103, CRC(53741c86) SHA1(01aa0e97b2ad1bed66a71aeef4426e3f1a42da84) )

	ROM_REGION( 108339, "svg_bottom", 0)
	ROM_LOAD( "gnw_dm53_bottom.svg", 0, 108339, CRC(b88bf17d) SHA1(105c978722c00c14301661e5ef5051fdfc33e878) )
ROM_END


ROM_START( gnw_jr55 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "jr-55_cms54c_kms560", 0x0000, 0x1000, CRC(46aed0ae) SHA1(72f75ccbd84aea094148c872fc7cc1683619a18a) )

	ROM_REGION( 261499, "svg_top", 0)
	ROM_LOAD( "gnw_jr55_top.svg", 0, 261499, CRC(3e48708f) SHA1(fc35bd4022327f2061d054c23316d2d6412b25b7) )

	ROM_REGION( 389658, "svg_bottom", 0)
	ROM_LOAD( "gnw_jr55_bottom.svg", 0, 389658, CRC(0a5eb4b2) SHA1(498aded148401a624a4ed215126e0d0b27d25d44) )
ROM_END


ROM_START( gnw_mw56 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "mw-56", 0x0000, 0x1000, CRC(385e59da) SHA1(2f79281bdf2f2afca2fb5bd7b9a3beeffc9c4eb7) )

	ROM_REGION( 165452, "svg_left", 0)
	ROM_LOAD( "gnw_mw56_left.svg", 0, 165452, CRC(07fda7f3) SHA1(4c884e57963cef4f22263afd7fe3cd72cd1cd756) )

	ROM_REGION( 224225, "svg_right", 0)
	ROM_LOAD( "gnw_mw56_right.svg", 0, 224225, CRC(515e1a7a) SHA1(e63419f6f7784b1a0a75e6e29c806a3abe2d7a42) )
ROM_END


ROM_START( gnw_dj101 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "dj-101", 0x0000, 0x1000, CRC(8dcfb5d1) SHA1(e0ef578e9362eb9a3cab631376df3cf55978f2de) )

	ROM_REGION( 276947, "svg", 0)
	ROM_LOAD( "gnw_dj101.svg", 0, 276947, CRC(9e1eb311) SHA1(f5004626549ea1f60fae87751bcad375580b7423) )
ROM_END


ROM_START( gnw_ml102 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "ml-102_cms54c_kms577", 0x0000, 0x1000, CRC(c1128dea) SHA1(8647e36f43a0e37756a3c7b6a3f08d4c8243f1cc) )

	ROM_REGION( 359508, "svg", 0)
	ROM_LOAD( "gnw_ml102.svg", 0, 359508, CRC(04d42f75) SHA1(06d0dd5a6c65eda581746379ccdcf4bc02e26f83) )
ROM_END


ROM_START( gnw_bx301 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "bx-301_kms73b_kms744.program", 0x0000, 0x1000, CRC(0fdf0303) SHA1(0b791c9d4874e9534d0a9b7a8968ce02fe4bee96) )

	ROM_REGION( 0x100, "maincpu:melody", 0 )
	ROM_LOAD( "bx-301_kms73b_kms744.melody", 0x000, 0x100, CRC(439d943d) SHA1(52880df15ec7513f96482f455ef3d9778aa24750) )

	ROM_REGION( 258505, "svg", 0)
	ROM_LOAD( "gnw_bx301.svg", 0, 258505, CRC(b2416cf9) SHA1(c31789e86baad1e2f6b070865d08d9298cf54f69) )
ROM_END



//    YEAR  NAME       PARENT  COMP MACHINE    INPUT      STATE        INIT  COMPANY, FULLNAME, FLAGS
CONS( 1989, ktopgun,   0,        0, ktopgun,   ktopgun,   ktopgun_state,  0, "Konami", "Top Gun (handheld)", MACHINE_SUPPORTS_SAVE )
CONS( 1989, kcontra,   0,        0, kcontra,   kcontra,   kcontra_state,  0, "Konami", "Contra (handheld)", MACHINE_SUPPORTS_SAVE )
CONS( 1989, ktmnt,     0,        0, ktmnt,     ktmnt,     ktmnt_state,    0, "Konami", "Teenage Mutant Ninja Turtles (handheld)", MACHINE_SUPPORTS_SAVE )
CONS( 1989, kgradius,  0,        0, kgradius,  kgradius,  kgradius_state, 0, "Konami", "Gradius (handheld)", MACHINE_SUPPORTS_SAVE )
CONS( 1989, kloneran,  0,        0, kloneran,  kloneran,  kloneran_state, 0, "Konami", "Lone Ranger (handheld)", MACHINE_SUPPORTS_SAVE )

CONS( 1981, gnw_mc25,  0,        0, mc25,      mc25,      mc25_state,     0, "Nintendo", "Game & Watch: Mickey Mouse", MACHINE_SUPPORTS_SAVE )
CONS( 1981, gnw_eg26,  gnw_mc25, 0, eg26,      mc25,      mc25_state,     0, "Nintendo", "Game & Watch: Egg", MACHINE_SUPPORTS_SAVE )
CONS( 1984, nupogodi,  gnw_mc25, 0, nupogodi,  mc25,      mc25_state,     0, "Elektronika", "Nu, pogodi!", MACHINE_SUPPORTS_SAVE )
CONS( 1989, exospace,  gnw_mc25, 0, exospace,  exospace,  mc25_state,     0, "Elektronika", "Explorers of Space", MACHINE_SUPPORTS_SAVE )

CONS( 1982, gnw_dm53,  0,        0, dm53,      dm53,      dm53_state,     0, "Nintendo", "Game & Watch: Mickey & Donald", MACHINE_SUPPORTS_SAVE )
CONS( 1983, gnw_jr55,  0,        0, jr55,      jr55,      jr55_state,     0, "Nintendo", "Game & Watch: Donkey Kong II", MACHINE_SUPPORTS_SAVE )
CONS( 1983, gnw_mw56,  0,        0, mw56,      mw56,      mw56_state,     0, "Nintendo", "Game & Watch: Mario Bros.", MACHINE_SUPPORTS_SAVE )

CONS( 1982, gnw_dj101, 0,        0, dj101,     dj101,     dj101_state,    0, "Nintendo", "Game & Watch: Donkey Kong Jr. (new wide screen)", MACHINE_SUPPORTS_SAVE )
CONS( 1983, gnw_ml102, 0,        0, ml102,     ml102,     ml102_state,    0, "Nintendo", "Game & Watch: Mario's Cement Factory (new wide screen)", MACHINE_SUPPORTS_SAVE )

CONS( 1984, gnw_bx301, 0,        0, bx301,     bx301,     bx301_state,    0, "Nintendo", "Game & Watch: Boxing", MACHINE_SUPPORTS_SAVE )
