// license:BSD-3-Clause
// copyright-holders:hap
// thanks-to:yoyo_chessboard
/******************************************************************************

Fidelity Dame Sensory Challenger (DSC)

Hardware notes:
- Z80A CPU @ 3.9MHz
- 8KB ROM(MOS 2364), 1KB RAM(2*TMM314APL)
- 4-digit 7seg panel, sensory board with 50 buttons
- PCB label 510-1030A01

It's a checkers game for once instead of chess.

When playing it on MAME with the sensorboard device, use the modifier keys
(eg. hold CTRL to ignore sensor). The game expects the player to press a sensor
only once when doing a multiple capture.

TODO:
- doesn't announce winner/loser when the game ends, or is this normal?

******************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/sensorboard.h"
#include "machine/timer.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "video/pwm.h"
#include "speaker.h"

// internal artwork
#include "fidel_dsc.lh" // clickable


namespace {

class dsc_state : public driver_device
{
public:
	dsc_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_irq_on(*this, "irq_on"),
		m_board(*this, "board"),
		m_display(*this, "display"),
		m_dac(*this, "dac"),
		m_inputs(*this, "IN.%u", 0)
	{ }

	// machine drivers
	void dsc(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	// devices/pointers
	required_device<cpu_device> m_maincpu;
	required_device<timer_device> m_irq_on;
	required_device<sensorboard_device> m_board;
	required_device<pwm_display_device> m_display;
	required_device<dac_bit_interface> m_dac;
	required_ioport_array<2> m_inputs;

	// address maps
	void main_map(address_map &map);

	// periodic interrupts
	template<int Line> TIMER_DEVICE_CALLBACK_MEMBER(irq_on) { m_maincpu->set_input_line(Line, ASSERT_LINE); }
	template<int Line> TIMER_DEVICE_CALLBACK_MEMBER(irq_off) { m_maincpu->set_input_line(Line, CLEAR_LINE); }

	// I/O handlers
	void update_display();
	DECLARE_WRITE8_MEMBER(control_w);
	DECLARE_WRITE8_MEMBER(select_w);
	DECLARE_READ8_MEMBER(input_r);

	void init_board(int state);
	u8 read_board_row(u8 row);

	u8 m_inp_mux;
	u8 m_led_select;
};

void dsc_state::machine_start()
{
	// zerofill
	m_inp_mux = 0;
	m_led_select = 0;

	// register for savestates
	save_item(NAME(m_inp_mux));
	save_item(NAME(m_led_select));
}



/******************************************************************************
    Devices, I/O
******************************************************************************/

// sensorboard handlers

void dsc_state::init_board(int state)
{
	for (int i = 0; i < 20; i++)
	{
		m_board->write_piece(i % 5, i / 5, 1); // white
		m_board->write_piece(i % 5, i / 5 + 6, 3); // black
	}
}

u8 dsc_state::read_board_row(u8 row)
{
	u8 data = 0;

	// inputs to sensorboard translation table
	static const u8 lut_i2sb[64] =
	{
		0x00, 0x50, 0x60, 0x70, 0x40, 0x30, 0x20, 0x10,
		0x01, 0x51, 0x61, 0x71, 0x41, 0x31, 0x21, 0x11,
		0x02, 0x52, 0x62, 0x72, 0x42, 0x32, 0x22, 0x12,
		0x03, 0x83, 0x84, 0x82, 0x91, 0x81, 0x90, 0x80,
		0xff, 0x93, 0x94, 0x92, 0xff, 0xff, 0xff, 0xff,
		0x04, 0x53, 0x63, 0x73, 0x43, 0x33, 0x23, 0x13,
        0xff, 0x54, 0x64, 0x74, 0x44, 0x34, 0x24, 0x14,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};

	for (int i = 0; i < 8; i++)
	{
		u8 pos = lut_i2sb[row * 8 + i];
		data = data << 1 | m_board->read_sensor(pos & 0xf, pos >> 4);
	}

	return data;
}


// TTL

void dsc_state::update_display()
{
	// 4 7seg leds
	m_display->matrix(m_led_select, m_inp_mux);
}

WRITE8_MEMBER(dsc_state::control_w)
{
	// d0-d7: input mux, 7seg data
	m_inp_mux = data;
	update_display();
}

WRITE8_MEMBER(dsc_state::select_w)
{
	// d4: speaker out
	m_dac->write(BIT(~data, 4));

	// d0-d3: digit select
	m_led_select = data & 0xf;
	update_display();
}

READ8_MEMBER(dsc_state::input_r)
{
	u8 data = 0;

	// d0-d7: multiplexed inputs (active low)
	for (int i = 0; i < 8; i++)
		if (BIT(~m_inp_mux, i))
		{
			// read checkerboard
			data |= read_board_row(i);

			// read other buttons
			if (i >= 6)
				data |= m_inputs[i - 6]->read();
		}

	return ~data;
}



/******************************************************************************
    Address Maps
******************************************************************************/

void dsc_state::main_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x1fff).rom();
	map(0x4000, 0x4000).mirror(0x1fff).w(FUNC(dsc_state::control_w));
	map(0x6000, 0x6000).mirror(0x1fff).w(FUNC(dsc_state::select_w));
	map(0x8000, 0x8000).mirror(0x1fff).r(FUNC(dsc_state::input_r));
	map(0xa000, 0xa3ff).mirror(0x1c00).ram();
}



/******************************************************************************
    Input Ports
******************************************************************************/

static INPUT_PORTS_START( dsc )
	PORT_START("IN.0")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Black King")

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Black")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("White King")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("White")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_NAME("RV")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD) PORT_NAME("RE")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD) PORT_NAME("PB")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD) PORT_NAME("LV")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_NAME("CL")
INPUT_PORTS_END



/******************************************************************************
    Machine Drivers
******************************************************************************/

void dsc_state::dsc(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 3.9_MHz_XTAL); // 3.9MHz resonator
	m_maincpu->set_addrmap(AS_PROGRAM, &dsc_state::main_map);

	const attotime irq_period = attotime::from_hz(523); // from 555 timer (22nF, 120K, 2.7K)
	TIMER(config, m_irq_on).configure_periodic(FUNC(dsc_state::irq_on<INPUT_LINE_IRQ0>), irq_period);
	m_irq_on->set_start_delay(irq_period - attotime::from_usec(41)); // active for 41us
	TIMER(config, "irq_off").configure_periodic(FUNC(dsc_state::irq_off<INPUT_LINE_IRQ0>), irq_period);

	SENSORBOARD(config, m_board).set_type(sensorboard_device::BUTTONS);
	m_board->init_cb().set(FUNC(dsc_state::init_board));
	m_board->set_size(5, 10); // 2 columns per x (eg. square 1 & 6 are same x)
	m_board->set_spawnpoints(4);
	m_board->set_delay(attotime::from_msec(100));

	/* video hardware */
	PWM_DISPLAY(config, m_display).set_size(4, 8);
	m_display->set_segmask(0xf, 0x7f);
	config.set_default_layout(layout_fidel_dsc);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac).add_route(ALL_OUTPUTS, "speaker", 0.25);
	VOLTAGE_REGULATOR(config, "vref").add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( damesc ) // model DSC
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "101-1027a01", 0x0000, 0x2000, CRC(d86c985c) SHA1(20f923a24420050fd16e1172f5e889f144d17ac9) ) // MOS 2364
ROM_END

} // anonymous namespace



/******************************************************************************
    Drivers
******************************************************************************/

//    YEAR  NAME    PARENT CMP MACHINE  INPUT  STATE      INIT        COMPANY, FULLNAME, FLAGS
CONS( 1981, damesc, 0,      0, dsc,     dsc,   dsc_state, empty_init, "Fidelity Electronics", "Dame Sensory Challenger", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
