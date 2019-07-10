// license:BSD-3-Clause
// copyright-holders:hap
// thanks-to:yoyo_chessboard
/******************************************************************************

Fidelity Sensory Chess Challenger 8

Hardware notes:
- Z80A CPU @ 3.9MHz
- 4KB ROM(MOS 2732), 256 bytes RAM(35391CP)
- chessboard buttons, 8*8+1 leds
- PCB label 510-1011 REV.2

******************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/sensorboard.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "video/pwm.h"
#include "speaker.h"

// internal artwork
#include "fidel_sc8.lh" // clickable


namespace {

class scc_state : public driver_device
{
public:
	scc_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_board(*this, "board"),
		m_display(*this, "display"),
		m_dac(*this, "dac"),
		m_inputs(*this, "IN.0")
	{ }

	// machine drivers
	void scc(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	// devices/pointers
	required_device<cpu_device> m_maincpu;
	required_device<sensorboard_device> m_board;
	required_device<pwm_display_device> m_display;
	required_device<dac_bit_interface> m_dac;
	required_ioport m_inputs;

	// address maps
	void main_map(address_map &map);
	void main_io(address_map &map);

	// I/O handlers
	DECLARE_READ8_MEMBER(input_r);
	DECLARE_WRITE8_MEMBER(control_w);

	u8 m_inp_mux;
	u8 m_led_data;
};

void scc_state::machine_start()
{
	// zerofill
	m_inp_mux = 0;
	m_led_data = 0;

	// register for savestates
	save_item(NAME(m_inp_mux));
	save_item(NAME(m_led_data));
}



/******************************************************************************
    Devices, I/O
******************************************************************************/

// TTL

WRITE8_MEMBER(scc_state::control_w)
{
	// a0-a2,d7: led data
	u8 mask = 1 << (offset & 7);
	m_led_data = (m_led_data & ~mask) | ((data & 0x80) ? mask : 0);

	// d0-d3: led select, input mux (row 9 is speaker out)
	// d4: corner led(direct)
	m_inp_mux = data & 0xf;
	u16 sel = 1 << m_inp_mux;
	m_dac->write(BIT(sel, 9));
	m_display->matrix((sel & 0xff) | (data << 4 & 0x100), m_led_data);
}

READ8_MEMBER(scc_state::input_r)
{
	u8 data = 0;

	// d0-d7: multiplexed inputs (active low)
	// read chessboard sensors
	if (m_inp_mux < 8)
		data = m_board->read_file(m_inp_mux);

	// read button panel
	else if (m_inp_mux == 8)
		data = m_inputs->read();

	return ~data;
}



/******************************************************************************
    Address Maps
******************************************************************************/

void scc_state::main_map(address_map &map)
{
	map(0x0000, 0x0fff).rom();
	map(0x5000, 0x50ff).ram();
}

void scc_state::main_io(address_map &map)
{
	map.global_mask(0x07);
	map(0x00, 0x07).rw(FUNC(scc_state::input_r), FUNC(scc_state::control_w));
}



/******************************************************************************
    Input Ports
******************************************************************************/

static INPUT_PORTS_START( scc )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Pawn")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Rook")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("Knight")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("Bishop")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_NAME("Queen")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_NAME("King")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_DEL) PORT_NAME("CL")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_R) PORT_NAME("RE")
INPUT_PORTS_END



/******************************************************************************
    Machine Drivers
******************************************************************************/

void scc_state::scc(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 3.9_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &scc_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &scc_state::main_io);

	SENSORBOARD(config, m_board).set_type(sensorboard_device::BUTTONS);
	m_board->init_cb().set(m_board, FUNC(sensorboard_device::preset_chess));
	m_board->set_delay(attotime::from_msec(100));

	/* video hardware */
	PWM_DISPLAY(config, m_display).set_size(9, 8);
	config.set_default_layout(layout_fidel_sc8);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac).add_route(ALL_OUTPUTS, "speaker", 0.25);
	VOLTAGE_REGULATOR(config, "vref").add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( fscc8 ) // model SCC
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "101-32017", 0x0000, 0x1000, CRC(5340820d) SHA1(e3494c7624b3cacbbb9a0a8cc9e1ed3e00326dfd) ) // 2732
ROM_END

} // anonymous namespace



/******************************************************************************
    Drivers
******************************************************************************/

//    YEAR  NAME   PARENT CMP MACHINE  INPUT  STATE      INIT        COMPANY, FULLNAME, FLAGS
CONS( 1980, fscc8, 0,      0, scc,     scc,   scc_state, empty_init, "Fidelity Electronics", "Sensory Chess Challenger 8", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
