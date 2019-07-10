// license:BSD-3-Clause
// copyright-holders:R. Belmont, Olivier Galibert
/*************************************************************************************

    Yamaha MU-80 and MU-100 : 32-voice polyphonic/multitimbral General MIDI/GS/XG tone modules
    Preliminary driver by R. Belmont and O. Galibert

    MU80 CPU: Hitachi H8/3002 (HD6413D02F16), strapped for mode 4, with a 12 MHz oscillator
    Sound ASICs: 2x Yamaha YMM275-F/SWP20 + 2x YMM279-F/SWD wave decoders + HD62908 "MEG" effects processor

    I/O ports from service manual:

    Port B (MU80)
        0 - LCD data, SW data, LED 1
        1 - LCD data, SW data, LED 2
        2 - LCD data, SW data, LED 3
        3 - LCD data, SW data, LED 4
        4 - LCD data, SW data, LED 5
        5 - LCD data, SW strobe data
        6 - LCD data, SW strobe data
        7 - LCD data, SW data, LED 6

    Port 2:
        0 - (out) LCD control RS
        1 - (out) LCD control R/W
        2 - (out) LCD control E
        3 - (out) LCD contrast A
        4 - (out) LCD contrast B
        5 - (out) LCD contrast C
        6 - (out) 1 MHz clock for serial
        7 - NC

    Port 3:
        4 - (out) A/D gain control 1
        5 - (out) A/D gain control 2

    Port 5:
        3 - (out) Reset signal for rotary encoder

    Port 6:
        1 - NC
        2 - (out) PB select (SW1)
        3 - (out) PB select (SW2)
        4 - (out) reset PB
        5 - (out) reset SWP30 (sound chip)
        6 - NC
        7 - (in) Plug detection for A/D input

    Port A (MU80):
        0 -
        1 - LCD control RS
        2 -
        3 - (same as sws on MU100) LED,SW Strobe data latch
        4 - (same as swd on MU100) SW data read control
        5 - LCD control E
        6 - LCD control RW
        7 -

    Port F:
        0 - (out) (sws) LED,SW Strobe data latch
        1 - (out) (swd) SW data read control
        2 - (out) PB select (SW4)

    Port G:
        0 - (out) PB select (SW3)

    Analog input channels:
        0 - level input R
        2 - level output L
        4 - host SW type switch position
        6 - battery voltage
        7 - model check (0 for MU100, 0.5 for OEM, 1 for MU100R)

    Switch map at the connector (17=ground)
        09 8 play
        10 8 edit
        11 8 mute/solo
        12 8 part -
        13 8 part +
        14 8 util
        15 8 effect
        16 8 enter
        12 7 select <
        13 7 select >
        16 7 mode
        15 7 eq
        14 7 exit
        10 7 value -
        11 7 value +
           2 led play
           3 led edit
           4 led util
           5 led effect
           6 led mode
           1 led eq

     IC32:
        1 p10 c.2
        2 p11 c.3
        3 p12 c.4
        4 p13 c.5
        5 p14 c.6
        6 p15 c.7
        7 p16 c.8
        8 p17 c.1
        g sws

     IC33
        1 p17 c.09
        2 p16 c.10
        3 p15 c.11
        4 p14 c.12
        5 p13 c.13
        6 p12 c.14
        7 p11 c.15
        8 p10 c.16
        g swd

**************************************************************************************/

#include "emu.h"

#include "bus/midi/midiinport.h"
#include "bus/midi/midioutport.h"
#include "cpu/h8/h83002.h"
#include "machine/mulcd.h"
#include "sound/swp20.h"
#include "sound/meg.h"

#include "debugger.h"
#include "speaker.h"


static INPUT_PORTS_START( mu80 )
	PORT_START("P7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Enter")     PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Effect")    PORT_CODE(KEYCODE_F)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Util")      PORT_CODE(KEYCODE_U)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Part +")    PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Part -")    PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Mute/Solo") PORT_CODE(KEYCODE_S)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Edit")      PORT_CODE(KEYCODE_E)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Play")      PORT_CODE(KEYCODE_A)

	PORT_START("P8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Mode")      PORT_CODE(KEYCODE_M)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Eq")        PORT_CODE(KEYCODE_Q)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Exit")      PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Select >")  PORT_CODE(KEYCODE_STOP)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Select <")  PORT_CODE(KEYCODE_COMMA)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Value +")   PORT_CODE(KEYCODE_EQUALS)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Value -")   PORT_CODE(KEYCODE_MINUS)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

class mu80_state : public driver_device
{
public:
	mu80_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_mu80cpu(*this, "mu80cpu")
		, m_swp20_0(*this, "swp20_0")
		, m_swp20_1(*this, "swp20_1")
		, m_meg(*this, "meg")
		, m_lcd(*this, "lcd")
		, m_ioport_p7(*this, "P7")
		, m_ioport_p8(*this, "P8")
	{ }

	void mu80(machine_config &config);

private:
	enum {
		P2_LCD_RS     = 0x01,
		P2_LCD_RW     = 0x02,
		P2_LCD_ENABLE = 0x04
	};

	enum {
		P6_LCD_RS     = 0x04,
		P6_LCD_RW     = 0x02,
		P6_LCD_ENABLE = 0x01
	};

	enum {
		PA_LCD_RS     = 0x02,
		PA_LCD_ENABLE = 0x20,
		PA_LCD_RW     = 0x40
	};

	required_device<h83002_device> m_mu80cpu;
	required_device<swp20_device> m_swp20_0;
	required_device<swp20_device> m_swp20_1;
	required_device<meg_device> m_meg;
	required_device<mulcd_device> m_lcd;
	required_ioport m_ioport_p7;
	required_ioport m_ioport_p8;

	u8 cur_p6, cur_pa, cur_pb, cur_ic32;

	u16 adc_zero_r();
	u16 adc_ar_r();
	u16 adc_al_r();
	u16 adc_midisw_r();
	u16 adc_battery_r();

	void p6_w(u16 data);
	u16 p6_r();
	void pa_w(u16 data);
	u16 pa_r();
	void pb_w(u16 data);
	u16 pb_r();

	virtual void machine_start() override;
	void mu80_iomap(address_map &map);
	void mu80_map(address_map &map);
};

void mu80_state::machine_start()
{
	cur_p6 = cur_pa = cur_pb = cur_ic32 = 0xff;
}

void mu80_state::mu80_map(address_map &map)
{
	map(0x000000, 0x07ffff).rom().region("mu80cpu", 0);
	map(0x200000, 0x20ffff).ram(); // 64K work RAM
	map(0x400000, 0x40003f).m(m_swp20_0, FUNC(swp20_device::map));
	map(0x440000, 0x44001f).m(m_meg, FUNC(meg_device::map));
	map(0x460000, 0x46003f).m(m_swp20_1, FUNC(swp20_device::map));
}

// Grounded adc input
u16 mu80_state::adc_zero_r()
{
	return 0;
}

// Analog input right (also sent to the swp)
u16 mu80_state::adc_ar_r()
{
	return 0;
}

// Analog input left (also sent to the swp)
u16 mu80_state::adc_al_r()
{
	return 0;
}

// Put the host switch to pure midi
u16 mu80_state::adc_midisw_r()
{
	return 0;
}

// Battery level
u16 mu80_state::adc_battery_r()
{
	return 0x200;
}

void mu80_state::pb_w(u16 data)
{
	cur_pb = data;
}

u16 mu80_state::pb_r()
{
	if((cur_pa & PA_LCD_ENABLE)) {
		if(cur_pa & PA_LCD_RW) {
			if(cur_pa & PA_LCD_RS)
				return m_lcd->data_read();
			else
				return m_lcd->control_read();
		} else {
			if(!(cur_pa & 0x10)) {
				u8 val = 0xff;
				if(!(cur_ic32 & 0x20))
					val &= m_ioport_p7->read();
				if(!(cur_ic32 & 0x40))
					val &= m_ioport_p8->read();
				return val;
			}

			return 0x00;
		}
	}

	return cur_pb;
}

void mu80_state::p6_w(u16 data)
{
	cur_p6 = data;
}

u16 mu80_state::p6_r()
{
	return cur_p6;
}

void mu80_state::pa_w(u16 data)
{
	data ^= PA_LCD_ENABLE;
	if(!(cur_pa & PA_LCD_ENABLE) && (data & PA_LCD_ENABLE)) {
		if(!(cur_pa & PA_LCD_RW)) {
			if(cur_pa & PA_LCD_RS)
				m_lcd->data_write(cur_pb);
			else
				m_lcd->control_write(cur_pb);
		}
	}

	if(!(cur_pa & 0x08) && (data & 0x08))
		cur_ic32 = cur_pb;

	cur_pa = data;
}

u16 mu80_state::pa_r()
{
	return cur_pa;
}

void mu80_state::mu80_iomap(address_map &map)
{
	map(h8_device::PORT_6, h8_device::PORT_6).rw(FUNC(mu80_state::p6_r), FUNC(mu80_state::p6_w));
	map(h8_device::PORT_A, h8_device::PORT_A).rw(FUNC(mu80_state::pa_r), FUNC(mu80_state::pa_w));
	map(h8_device::PORT_B, h8_device::PORT_B).rw(FUNC(mu80_state::pb_r), FUNC(mu80_state::pb_w));
	map(h8_device::ADC_0, h8_device::ADC_0).r(FUNC(mu80_state::adc_ar_r));
	map(h8_device::ADC_1, h8_device::ADC_1).r(FUNC(mu80_state::adc_zero_r));
	map(h8_device::ADC_2, h8_device::ADC_2).r(FUNC(mu80_state::adc_al_r));
	map(h8_device::ADC_3, h8_device::ADC_3).r(FUNC(mu80_state::adc_zero_r));
	map(h8_device::ADC_4, h8_device::ADC_4).r(FUNC(mu80_state::adc_midisw_r));
	map(h8_device::ADC_5, h8_device::ADC_6).r(FUNC(mu80_state::adc_zero_r));
	map(h8_device::ADC_6, h8_device::ADC_6).r(FUNC(mu80_state::adc_battery_r));
	map(h8_device::ADC_7, h8_device::ADC_7).r(FUNC(mu80_state::adc_zero_r)); // inputmod from the gate array
}

void mu80_state::mu80(machine_config &config)
{
	H83002(config, m_mu80cpu, 12_MHz_XTAL);
	m_mu80cpu->set_addrmap(AS_PROGRAM, &mu80_state::mu80_map);
	m_mu80cpu->set_addrmap(AS_IO, &mu80_state::mu80_iomap);

	MULCD(config, m_lcd);

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	SWP20(config, m_swp20_0);
	m_swp20_0->set_device_rom_tag("swp20");

	SWP20(config, m_swp20_1);
	m_swp20_1->set_device_rom_tag("swp20");

	MEG(config, m_meg);

	auto &mdin_a(MIDI_PORT(config, "mdin_a"));
	midiin_slot(mdin_a);
	mdin_a.rxd_handler().set("mu80cpu:sci1", FUNC(h8_sci_device::rx_w));

	auto &mdin_b(MIDI_PORT(config, "mdin_b"));
	midiin_slot(mdin_b);
	mdin_b.rxd_handler().set("mu80cpu:sci0", FUNC(h8_sci_device::rx_w));

	auto &mdout(MIDI_PORT(config, "mdout"));
	midiout_slot(mdout);
	m_mu80cpu->subdevice<h8_sci_device>("sci0")->tx_handler().set(mdout, FUNC(midi_port_device::write_txd));
}

ROM_START( mu80 )
	ROM_REGION( 0x80000, "mu80cpu", 0 )
	ROM_LOAD16_WORD_SWAP( "yamaha_mu80.bin", 0x000000, 0x080000, CRC(c31074c0) SHA1(a11bd4523cd8ff1e1744078c3b4c18112b73c61e) )

	ROM_REGION( 0x800000, "swp20", ROMREGION_ERASE00 )
ROM_END

CONS( 1994, mu80, 0, 0, mu80, mu80, mu80_state, empty_init, "Yamaha", "MU80", MACHINE_NOT_WORKING )
