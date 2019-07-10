// license:BSD-3-Clause
// copyright-holders:Luca Elia
/***************************************************************************

Electro-mechanical bubble hockey games:

- Chexx (1983 version) by ICE
  http://www.pinrepair.com/arcade/chexx.htm

- Face-Off, an illegal? copy of Chexx
  http://valker.us/gameroom/SegaFaceOff.htm
  https://casetext.com/case/innovative-concepts-in-ent-v-entertainment-enter

(Some sources indicate these may have been copied from a earlier Sega game called Face-Off)

***************************************************************************/

#include "emu.h"
#include "cpu/m6502/m6502.h"
#include "machine/6522via.h"
#include "machine/timer.h"
#include "sound/ay8910.h"
#include "sound/digitalk.h"
#include "speaker.h"

#include "chexx.lh"


#define MAIN_CLOCK XTAL(4'000'000)

class chexx_state : public driver_device
{
public:
	chexx_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_via(*this, "via6522")
		, m_digitalker(*this, "digitalker")
		, m_digits(*this, "digit%u", 0U)
		, m_leds(*this, "led%u", 0U)
		, m_lamps(*this, "lamp%u", 0U)
		, m_dsw(*this, "DSW")
		, m_input(*this, "INPUT")
		, m_coin(*this, "COIN")
	{
	}

	// handlers
	DECLARE_READ8_MEMBER(via_a_in);
	DECLARE_READ8_MEMBER(via_b_in);

	DECLARE_WRITE8_MEMBER(via_a_out);
	DECLARE_WRITE8_MEMBER(via_b_out);

	DECLARE_WRITE_LINE_MEMBER(via_ca2_out);
	DECLARE_WRITE_LINE_MEMBER(via_cb1_out);
	DECLARE_WRITE_LINE_MEMBER(via_cb2_out);
	DECLARE_WRITE_LINE_MEMBER(via_irq_out);

	DECLARE_READ8_MEMBER(input_r);

	DECLARE_WRITE8_MEMBER(lamp_w);

	void chexx(machine_config &config);
	void mem(address_map &map);

protected:
	enum
	{
		TIMER_UPDATE
	};

	void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	void update();

	// digitalker
	void digitalker_set_bank(uint8_t bank);

	// driver_device overrides
	virtual void machine_start() override;
	virtual void machine_reset() override;

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<via6522_device> m_via;
	required_device<digitalker_device> m_digitalker;
	output_finder<4> m_digits;
	output_finder<3> m_leds;
	output_finder<2> m_lamps;

	required_ioport m_dsw;
	required_ioport m_input;
	required_ioport m_coin;

	// vars
	emu_timer *m_update_timer;
	uint8_t  m_port_a;
	uint8_t  m_port_b;
	uint8_t  m_bank;
	uint32_t m_shift;
	uint8_t  m_lamp;
};

class faceoffh_state : public chexx_state
{
public:
	faceoffh_state(const machine_config &mconfig, device_type type, const char *tag)
		: chexx_state(mconfig, type, tag)
		, m_aysnd(*this, "aysnd")
	{
	}

	void faceoffh(machine_config &config);

protected:
	DECLARE_WRITE8_MEMBER(ay_w);

	void mem(address_map &map);

	required_device<ay8910_device> m_aysnd; // only faceoffh
	uint8_t m_ay_cmd;
	uint8_t m_ay_data;
};


// VIA

READ8_MEMBER(chexx_state::via_a_in)
{
	uint8_t ret = 0;
	logerror("%s: VIA read A: %02X\n", machine().describe_context(), ret);
	return ret;
}

READ8_MEMBER(chexx_state::via_b_in)
{
	uint8_t ret = 0;
	logerror("%s: VIA read B: %02X\n", machine().describe_context(), ret);
	return ret;
}

WRITE8_MEMBER(chexx_state::via_a_out)
{
	m_port_a = data;    // multiplexer
	m_digitalker->digitalker_data_w(space, 0, data, 0);
//  logerror("%s: VIA write A = %02X\n", machine().describe_context(), data);
}

WRITE8_MEMBER(chexx_state::via_b_out)
{
	m_port_b = data;

	digitalker_set_bank(data & 3);
	m_digitalker->set_output_gain(0, BIT(data,2) ? 1.0f : 0.0f); // bit 2 controls the Digitalker output
	machine().bookkeeping().coin_counter_w(0, BIT(~data,3));
	// bit 4 is EJECT
	// bit 7 is related to speaker out

//  logerror("%s: VIA write B = %02X\n", machine().describe_context(), data);
}

WRITE_LINE_MEMBER(chexx_state::via_ca2_out)
{
	m_digitalker->digitalker_0_cms_w(CLEAR_LINE);
	m_digitalker->digitalker_0_cs_w(CLEAR_LINE);
	m_digitalker->digitalker_0_wr_w(state ? ASSERT_LINE : CLEAR_LINE);

//  logerror("%s: VIA write CA2 = %02X\n", machine().describe_context(), state);
}

WRITE_LINE_MEMBER(chexx_state::via_cb1_out)
{
//  logerror("%s: VIA write CB1 = %02X\n", machine().describe_context(), state);
}

WRITE_LINE_MEMBER(chexx_state::via_cb2_out)
{
	m_shift = ((m_shift << 1) & 0xffffff) | state;

	// 7segs (score)
	constexpr uint8_t patterns[16] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67, 0, 0, 0, 0, 0, 0 }; // 4511

	m_digits[0] = patterns[(m_shift >> (16+4)) & 0xf];
	m_digits[1] = patterns[(m_shift >> (16+0)) & 0xf];
	m_digits[2] = patterns[(m_shift >>  (8+4)) & 0xf];
	m_digits[3] = patterns[(m_shift >>  (8+0)) & 0xf];

	// Leds (period being played)
	m_leds[0] = BIT(m_shift,2);
	m_leds[1] = BIT(m_shift,1);
	m_leds[2] = BIT(m_shift,0);

//  logerror("%s: VIA write CB2 = %02X\n", machine().describe_context(), state);
}

WRITE_LINE_MEMBER(chexx_state::via_irq_out)
{
	m_maincpu->set_input_line(INPUT_LINE_IRQ0, state ? ASSERT_LINE : CLEAR_LINE);
//  logerror("%s: VIA write IRQ = %02X\n", machine().describe_context(), state);
}

READ8_MEMBER(chexx_state::input_r)
{
	uint8_t ret = m_dsw->read();          // bits 0-3
	uint8_t inp = m_input->read();        // bit 7 (multiplexed)

	for (int i = 0; i < 8; ++i)
		if (BIT(~m_port_a, i) && BIT(~inp, i))
			ret &= 0x7f;

	return ret;
}

// Chexx Memory Map

void chexx_state::mem(address_map &map)
{
	map(0x0000, 0x007f).ram().mirror(0x100); // 6810 - 128 x 8 static RAM
	map(0x4000, 0x400f).m(m_via, FUNC(via6522_device::map));
	map(0x8000, 0x8000).r(FUNC(chexx_state::input_r));
	map(0xf800, 0xffff).rom().region("maincpu", 0);
}

void chexx_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_UPDATE:
		update();
		break;
	}
}

WRITE8_MEMBER(chexx_state::lamp_w)
{
	m_lamp = data;
	m_lamps[0] = BIT(m_lamp,0);
	m_lamps[1] = BIT(m_lamp,1);
}

// Face-Off Memory Map

void faceoffh_state::mem(address_map &map)
{
	map(0x0000, 0x007f).ram().mirror(0x100); // M58725P - 2KB
	map(0x4000, 0x400f).m(m_via, FUNC(via6522_device::map));
	map(0x8000, 0x8000).r(FUNC(faceoffh_state::input_r));
	map(0xa000, 0xa001).w(FUNC(faceoffh_state::ay_w));
	map(0xc000, 0xc000).w(FUNC(faceoffh_state::lamp_w));
	map(0xf000, 0xffff).rom().region("maincpu", 0);
}

WRITE8_MEMBER(faceoffh_state::ay_w)
{
	if (offset)
	{
		m_ay_data = data;
		return;
	}

	if (m_ay_cmd == 0x00 && data == 0x03)
	{
		m_aysnd->address_w(m_ay_data);
//      logerror("%s: AY addr = %02X\n", machine().describe_context(), m_ay_data);
	}
	else if (m_ay_cmd == 0x00 && data == 0x02)
	{
		m_aysnd->data_w(m_ay_data);
//      logerror("%s: AY data = %02X\n", machine().describe_context(), m_ay_data);
	}
	m_ay_cmd = data;
}

// Inputs

static INPUT_PORTS_START( chexx83 )
	PORT_START("COIN")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1   ) PORT_IMPULSE(1) // play anthem
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2   ) PORT_IMPULSE(1) // play anthem

	PORT_START("INPUT")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1  ) PORT_NAME("P1 Goal Sensor")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2  ) PORT_NAME("P2 Goal Sensor")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START3  ) PORT_NAME("Puck Near Goal Sensors") // play "ohh" sample
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Boo Button")  // stop anthem, play "boo" sample, eject puck
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Boo Button")  // stop anthem, play "boo" sample, eject puck
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Puck Eject Ready Sensor")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(    0x03, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x0c, 0x00, "Game Duration (mins)" ) PORT_DIPLOCATION("SW1:3,4")
	PORT_DIPSETTING(    0x00, "2" ) // 40
	PORT_DIPSETTING(    0x04, "3" ) // 60
	PORT_DIPSETTING(    0x08, "4" ) // 80
	PORT_DIPSETTING(    0x0c, "5" ) // 100
	PORT_BIT( 0x70, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_CUSTOM ) // multiplexed inputs
INPUT_PORTS_END

// Machine

void chexx_state::machine_start()
{
	m_digits.resolve();
	m_leds.resolve();
	m_lamps.resolve();

	m_update_timer = timer_alloc(TIMER_UPDATE);
}

void chexx_state::digitalker_set_bank(uint8_t bank)
{
	if (m_bank != bank)
	{
		uint8_t *src = memregion("samples")->base();
		uint8_t *dst = memregion("digitalker")->base();

		memcpy(dst, src + bank * 0x4000, 0x4000);

		m_bank = bank;
	}
}

void chexx_state::machine_reset()
{
	m_bank = -1;
	digitalker_set_bank(0);
	m_update_timer->adjust(attotime::from_hz(60), 0, attotime::from_hz(60));
}

void chexx_state::update()
{
	// NMI on coin-in
	uint8_t coin = (~m_coin->read()) & 0x03;
	m_maincpu->set_input_line(INPUT_LINE_NMI, coin ? ASSERT_LINE : CLEAR_LINE);

	// VIA CA1 connected to Digitalker INTR line
	m_via->write_ca1(m_digitalker->digitalker_0_intr_r());

#if 0
	// Play the digitalker samples (it's not hooked up correctly yet)
	static uint8_t sample = 0, bank = 0;

	if (machine().input().code_pressed_once(KEYCODE_Q))
		--bank;
	if (machine().input().code_pressed_once(KEYCODE_W))
		++bank;
	bank %= 3;
	digitalker_set_bank(bank);

	if (machine().input().code_pressed_once(KEYCODE_A))
		--sample;
	if (machine().input().code_pressed_once(KEYCODE_S))
		++sample;

	if (machine().input().code_pressed_once(KEYCODE_Z))
	{
		m_digitalker->digitalker_0_cms_w(CLEAR_LINE);
		m_digitalker->digitalker_0_cs_w(CLEAR_LINE);

		address_space &space = m_maincpu->space(AS_PROGRAM);
		m_digitalker->digitalker_data_w(space, 0, sample, 0);

		m_digitalker->digitalker_0_wr_w(ASSERT_LINE);
		m_digitalker->digitalker_0_wr_w(CLEAR_LINE);
		m_digitalker->digitalker_0_wr_w(ASSERT_LINE);
	}
#endif
}

void chexx_state::chexx(machine_config &config)
{
	M6502(config, m_maincpu, MAIN_CLOCK/2);
	m_maincpu->set_addrmap(AS_PROGRAM, &chexx_state::mem);

	// via
	VIA6522(config, m_via, MAIN_CLOCK/4);

	m_via->readpa_handler().set(FUNC(chexx_state::via_a_in));
	m_via->readpb_handler().set(FUNC(chexx_state::via_b_in));

	m_via->writepa_handler().set(FUNC(chexx_state::via_a_out));
	m_via->writepb_handler().set(FUNC(chexx_state::via_b_out));

	m_via->ca2_handler().set(FUNC(chexx_state::via_ca2_out));
	m_via->cb1_handler().set(FUNC(chexx_state::via_cb1_out));
	m_via->cb2_handler().set(FUNC(chexx_state::via_cb2_out));
	m_via->irq_handler().set(FUNC(chexx_state::via_irq_out));

	// Layout
	config.set_default_layout(layout_chexx);

	// sound hardware
	SPEAKER(config, "mono").front_center();
	DIGITALKER(config, m_digitalker, MAIN_CLOCK);
	m_digitalker->add_route(ALL_OUTPUTS, "mono", 0.16);
}

void faceoffh_state::faceoffh(machine_config &config)
{
	chexx(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &faceoffh_state::mem);

	AY8910(config, m_aysnd, MAIN_CLOCK/2);
	m_aysnd->add_route(ALL_OUTPUTS, "mono", 0.30);
}

// ROMs

/***************************************************************************

Chexx Hockey (1983 version 1.1)

The "long and skinny" Moog CPU board used a 6502 for the processor,
a 6522 for the PIA, a 6810 static RAM, eight 52164 64k bit sound ROM chips,
a 40 pin 54104 sound chip, and a single 2716 CPU EPROM

***************************************************************************/

ROM_START( chexx83 )
	ROM_REGION( 0x0800, "maincpu", 0 )
	ROM_LOAD( "chexx83.u4", 0x0000, 0x0800, CRC(a34abac1) SHA1(75a31670eb6d1b62ba984f0bac7c6e6067f6ae87) )

	ROM_REGION( 0x4000, "digitalker", ROMREGION_ERASE00 )
	// bank switched (from samples region)

	ROM_REGION( 0x10000, "samples", ROMREGION_ERASE00 )
	ROM_LOAD( "chexx83.u12", 0x0000, 0x2000, NO_DUMP )
	ROM_LOAD( "chexx83.u13", 0x2000, 0x2000, NO_DUMP )
	ROM_LOAD( "chexx83.u14", 0x4000, 0x2000, NO_DUMP )
	ROM_LOAD( "chexx83.u15", 0x6000, 0x2000, NO_DUMP )
	ROM_LOAD( "chexx83.u16", 0x8000, 0x2000, NO_DUMP )
	ROM_LOAD( "chexx83.u17", 0xa000, 0x2000, NO_DUMP )
	ROM_LOAD( "chexx83.u18", 0xc000, 0x2000, NO_DUMP )
	ROM_LOAD( "chexx83.u19", 0xe000, 0x2000, NO_DUMP )
ROM_END

/***************************************************************************

Face-Off PCB?

Entertainment Enterprises Ltd. 1983 (sticker)
Serial No. 025402 (sticker)
MADE IN JAPAN (etched)

CPU:     R6502P
RAM:     M58725P (2KB)
I/O:     R6522P (VIA)
Samples: Digitalker (MM54104)
Music:   AY-3-8910
Misc:    XTAL 4MHz, DSW4, 42-pin connector

***************************************************************************/

ROM_START( faceoffh )
	ROM_REGION( 0x1000, "maincpu", 0 )
	// "Copyright (c) 1983  SoftLogic JAPAN"
	ROM_LOAD( "1.5d", 0x0000, 0x1000, CRC(6ab050be) SHA1(ebecae855e22e9c3c46bdee51f84fd5352bf191a) )

	ROM_REGION( 0x4000, "digitalker", ROMREGION_ERASE00 )
	// bank switched (from samples region)

	ROM_REGION( 0x10000, "samples", 0 )
	ROM_LOAD( "9.2a", 0x0000, 0x2000, CRC(059b3725) SHA1(5837bee1ef34ce19a3101b851ca55029776e4b3e) )    // digitalker header
	ROM_LOAD( "8.2b", 0x2000, 0x2000, CRC(679da4e1) SHA1(01a5b9dd132c1b0de97c153d7de226f5bf357338) )

	ROM_LOAD( "7.2c", 0x4000, 0x2000, CRC(f8461b33) SHA1(717a8842e0ce9ba94dd59504a324bede4844e389) )    // digitalker header
	ROM_LOAD( "6.2d", 0x6000, 0x2000, CRC(156c91e0) SHA1(6017d4b5609b214a6e66dcd76493a7d1442c04d4) )

	ROM_LOAD( "5.3a", 0x8000, 0x2000, CRC(19904604) SHA1(633c211a9a822cdf597a6f3c221ae9c8d6482e82) )    // digitalker header
	ROM_LOAD( "4.3b", 0xa000, 0x2000, CRC(c3386d51) SHA1(7882e88db55ba914be81075e4b2d76e246c34d3b) )

	ROM_FILL(         0xc000, 0x2000, 0xff ) // unpopulated
	ROM_FILL(         0xe000, 0x2000, 0xff ) // unpopulated
ROM_END

GAME( 1983, chexx83,  0,       chexx,    chexx83, chexx_state,    empty_init, ROT270, "ICE",                                                 "Chexx (EM Bubble Hockey, 1983 1.1)", MACHINE_NOT_WORKING | MACHINE_MECHANICAL | MACHINE_NO_SOUND )
GAME( 1983, faceoffh, chexx83, faceoffh, chexx83, faceoffh_state, empty_init, ROT270, "SoftLogic (Entertainment Enterprises, Ltd. license)", "Face-Off (EM Bubble Hockey)",        MACHINE_NOT_WORKING | MACHINE_MECHANICAL | MACHINE_IMPERFECT_SOUND )
