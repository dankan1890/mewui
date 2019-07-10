// license:BSD-3-Clause
// copyright-holders:Robbbert
/******************************************************************************
*
*    EA Car Computer
*        by Robbbert, March 2011.
*
*    Described in Electronics Australia magazine during 1982.
*    Construction and usage: http://messui.the-chronicles.org/comp/eacc.pdf
*
*    The only RAM is the 128 bytes that comes inside the CPU.
*
*    This computer is mounted in a car, and various sensors (fuel flow, etc)
*    are connected up. By pressing the appropriate buttons various statistics
*    may be obtained.
*
*    Memory Map
*    0000-007F internal ram
*    4000-7FFF ROM
*    8000-BFFF 6821
*    C000-FFFF ROM (mirror)
*
*    The ROM was typed in twice from the dump in the magazine article, and the
*    results compared. Only one byte was different, so I can be confident that
*    it has been typed in properly.
*
*    Setting up: You need to enter the number of expected pulses from the fuel
*    and distance sensors. Paste this: 5 6M123N 7M400N  (start, set litres cal to
*    123 pulses. set km cal to 400 pulses). Then paste this: 1950M0N 1845M0N (set
*    petrol tank capacity to 50 litres, set current amount of petrol to 45).
*    Now enter: 28M100N (the journey is 100km). Press 5 to start the journey.
*    All settings are saved in nvram.
*
*    Stats you can see while travelling:
*    0  - time elapsed
*    08 - time remaining
*    1  - fuel used
*    18 - fuel left
*    2  - km travelled
*    28 - km remaining
*    29 - km that could be travelled with the fuel you have left
*    3  - speed now
*    39 - average speed
*    4  - fuel consumption now (litres per 100km)
*    49 - fuel average consumption
*
******************************************************************************/

/* Core includes */
#include "emu.h"
#include "cpu/m6800/m6800.h"
#include "eacc.lh"
#include "machine/6821pia.h"
#include "machine/nvram.h"
#include "machine/timer.h"


class eacc_state : public driver_device
{
public:
	eacc_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_pia(*this, "pia")
		, m_p_nvram(*this, "nvram")
		, m_digits(*this, "digit%u", 0U)
	{ }

	void eacc(machine_config &config);

private:
	DECLARE_READ_LINE_MEMBER( eacc_cb1_r );
	DECLARE_READ_LINE_MEMBER( eacc_distance_r );
	DECLARE_READ_LINE_MEMBER( eacc_fuel_sensor_r );
	DECLARE_READ8_MEMBER( eacc_keyboard_r );
	DECLARE_WRITE_LINE_MEMBER( eacc_cb2_w );
	DECLARE_WRITE8_MEMBER( eacc_digit_w );
	DECLARE_WRITE8_MEMBER( eacc_segment_w );
	TIMER_DEVICE_CALLBACK_MEMBER(eacc_cb1);
	TIMER_DEVICE_CALLBACK_MEMBER(eacc_nmi);
	void eacc_mem(address_map &map);
	uint8_t m_digit;
	bool m_cb1;
	bool m_cb2;
	bool m_nmi;
	virtual void machine_reset() override;
	virtual void machine_start() override { m_digits.resolve(); }
	required_device<cpu_device> m_maincpu;
	required_device<pia6821_device> m_pia;
	required_shared_ptr<uint8_t> m_p_nvram;
	output_finder<7> m_digits;
};




/******************************************************************************
 Address Maps
******************************************************************************/

void eacc_state::eacc_mem(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xc7ff); // A11,A12,A13 not connected
	map(0x0000, 0x001f).ram().share("nvram"); // inside cpu, battery-backed
	map(0x0020, 0x007f).ram(); // inside cpu
	map(0x4000, 0x47ff).rom().mirror(0x8000);
	map(0x8000, 0x8003).mirror(0x7fc).rw(m_pia, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
}


/******************************************************************************
 Input Ports
******************************************************************************/

static INPUT_PORTS_START(eacc)
	PORT_START("X0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("6 Litres Cal") PORT_CODE(KEYCODE_6_PAD) PORT_CHAR('6')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("ENTER") PORT_CODE(KEYCODE_ENTER_PAD) PORT_CHAR('M')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("2 km") PORT_CODE(KEYCODE_2_PAD) PORT_CHAR('2')
	PORT_BIT( 0xf8, 0, IPT_UNUSED )

	PORT_START("X1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("5 START") PORT_CODE(KEYCODE_5_PAD) PORT_CHAR('5')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("END") PORT_CODE(KEYCODE_PLUS_PAD) PORT_CHAR('N')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("3 km/h") PORT_CODE(KEYCODE_3_PAD) PORT_CHAR('3')
	PORT_BIT( 0xf8, 0, IPT_UNUSED )

	PORT_START("X2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("7 Km Cal") PORT_CODE(KEYCODE_7_PAD) PORT_CHAR('7')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("0 hour.min") PORT_CODE(KEYCODE_0_PAD) PORT_CHAR('0')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("4 l/100km") PORT_CODE(KEYCODE_4_PAD) PORT_CHAR('4')
	PORT_BIT( 0xf8, 0, IPT_UNUSED )

	PORT_START("X3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("8 REM") PORT_CODE(KEYCODE_8_PAD) PORT_CHAR('8')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("1 litres") PORT_CODE(KEYCODE_1_PAD) PORT_CHAR('1')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("9 AV") PORT_CODE(KEYCODE_9_PAD) PORT_CHAR('9')
	PORT_BIT( 0xf8, 0, IPT_UNUSED )
INPUT_PORTS_END

void eacc_state::machine_reset()
{
	m_cb2 = 0;
}

TIMER_DEVICE_CALLBACK_MEMBER(eacc_state::eacc_cb1)
{
	m_cb1 ^= 1; // 15hz
	if (m_cb2)
		m_maincpu->set_input_line(M6802_IRQ_LINE, ASSERT_LINE);
}

TIMER_DEVICE_CALLBACK_MEMBER(eacc_state::eacc_nmi)
{
	if (m_cb2)
	{
		m_nmi = true;
		m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
	}
}

READ_LINE_MEMBER( eacc_state::eacc_cb1_r )
{
	return (m_cb2) ? m_cb1 : 1;
}

READ_LINE_MEMBER( eacc_state::eacc_distance_r )
{
	return machine().rand() & 1; // needs random pulses to simulate movement
}

READ_LINE_MEMBER( eacc_state::eacc_fuel_sensor_r )
{
	return machine().rand() & 1; // needs random pulses to simulate fuel usage
}

WRITE_LINE_MEMBER( eacc_state::eacc_cb2_w )
{
	m_cb2 = state;
}

READ8_MEMBER( eacc_state::eacc_keyboard_r )
{
	uint8_t data = m_digit;

	if (BIT(m_digit, 3))
		data |= ioport("X0")->read();
	if (BIT(m_digit, 4))
		data |= ioport("X1")->read();
	if (BIT(m_digit, 5))
		data |= ioport("X2")->read();
	if (BIT(m_digit, 6))
		data |= ioport("X3")->read();

	return data;
}

WRITE8_MEMBER( eacc_state::eacc_segment_w )
{
	//d7 segment dot
	//d6 segment c
	//d5 segment d
	//d4 segment e
	//d3 segment a
	//d2 segment b
	//d1 segment f
	//d0 segment g

	if (!m_nmi)
	{
		uint8_t i;
		if (BIT(m_digit, 7))
		{
			char lednum[6];
			data ^= 0xff;

			for (i = 0; i < 8; i++)
			{
				sprintf(lednum,"led%d",i);
				output().set_value(lednum, BIT(data, i));
			}
		}
		else
		{
			for (i = 3; i < 7; i++)
				if (BIT(m_digit, i))
					m_digits[i] = bitswap<8>(data, 7, 0, 1, 4, 5, 6, 2, 3);
		}
	}
}

WRITE8_MEMBER( eacc_state::eacc_digit_w )
{
	if (m_nmi)
	{
		m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
		m_nmi = false;
	}
	m_digit = data & 0xf8;
}


/******************************************************************************
 Machine Drivers
******************************************************************************/

void eacc_state::eacc(machine_config &config)
{
	/* basic machine hardware */
	M6802(config, m_maincpu, XTAL(3'579'545));  /* Divided by 4 inside the m6802*/
	m_maincpu->set_addrmap(AS_PROGRAM, &eacc_state::eacc_mem);

	config.set_default_layout(layout_eacc);

	PIA6821(config, m_pia, 0);
	m_pia->readpb_handler().set(FUNC(eacc_state::eacc_keyboard_r));
	m_pia->readca1_handler().set(FUNC(eacc_state::eacc_distance_r));
	m_pia->readcb1_handler().set(FUNC(eacc_state::eacc_cb1_r));
	m_pia->readca2_handler().set(FUNC(eacc_state::eacc_fuel_sensor_r));
	m_pia->writepa_handler().set(FUNC(eacc_state::eacc_segment_w));
	m_pia->writepb_handler().set(FUNC(eacc_state::eacc_digit_w));
	m_pia->cb2_handler().set(FUNC(eacc_state::eacc_cb2_w));
	m_pia->irqa_handler().set_inputline("maincpu", M6802_IRQ_LINE);
	m_pia->irqb_handler().set_inputline("maincpu", M6802_IRQ_LINE);

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);
	TIMER(config, "eacc_nmi").configure_periodic(FUNC(eacc_state::eacc_nmi), attotime::from_hz(600));
	TIMER(config, "eacc_cb1").configure_periodic(FUNC(eacc_state::eacc_cb1), attotime::from_hz(30));
}



/******************************************************************************
 ROM Definitions
******************************************************************************/

ROM_START(eacc)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("eacc.bin", 0x4000, 0x0800, CRC(287a63c0) SHA1(f61b397d33ea40e5742e34d5f5468572125e8b39) )
ROM_END


/******************************************************************************
 Drivers
******************************************************************************/

//    YEAR  NAME  PARENT  COMPAT  MACHINE  INPUT  CLASS       INIT        COMPANY                  FULLNAME           FLAGS
COMP( 1982, eacc, 0,      0,      eacc,    eacc,  eacc_state, empty_init, "Electronics Australia", "EA Car Computer", MACHINE_NO_SOUND_HW)
