// license:BSD-3-Clause
// copyright-holders:Robbbert
/********************************************************************************************

    PINBALL
    Stern MP-100 MPU
    (almost identical to Bally MPU-17)


ToDo:
- Dips, Inputs, Solenoids vary per game
- Mechanical
- Sound board - an enormous mass of discrete circuitry

*********************************************************************************************/


#include "emu.h"
#include "machine/genpin.h"
#include "cpu/m6800/m6800.h"
#include "machine/6821pia.h"
#include "machine/timer.h"
#include "st_mp100.lh"


class st_mp100_state : public genpin_class
{
public:
	st_mp100_state(const machine_config &mconfig, device_type type, const char *tag)
		: genpin_class(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_pia_u10(*this, "pia_u10")
		, m_pia_u11(*this, "pia_u11")
		, m_io_test(*this, "TEST")
		, m_io_dsw0(*this, "DSW0")
		, m_io_dsw1(*this, "DSW1")
		, m_io_dsw2(*this, "DSW2")
		, m_io_dsw3(*this, "DSW3")
		, m_io_x0(*this, "X0")
		, m_io_x1(*this, "X1")
		, m_io_x2(*this, "X2")
		, m_io_x3(*this, "X3")
		, m_io_x4(*this, "X4")
		, m_digits(*this, "digit%u", 0U)
	{ }

	void st_mp100(machine_config &config);

	DECLARE_INPUT_CHANGED_MEMBER(activity_test);
	DECLARE_INPUT_CHANGED_MEMBER(self_test);

private:
	DECLARE_READ8_MEMBER(u10_a_r);
	DECLARE_WRITE8_MEMBER(u10_a_w);
	DECLARE_READ8_MEMBER(u10_b_r);
	DECLARE_WRITE8_MEMBER(u10_b_w);
	DECLARE_READ8_MEMBER(u11_a_r);
	DECLARE_WRITE8_MEMBER(u11_a_w);
	DECLARE_WRITE8_MEMBER(u11_b_w);
	DECLARE_WRITE_LINE_MEMBER(u10_ca2_w);
	DECLARE_WRITE_LINE_MEMBER(u10_cb2_w);
	DECLARE_WRITE_LINE_MEMBER(u11_ca2_w);
	DECLARE_WRITE_LINE_MEMBER(u11_cb2_w);
	TIMER_DEVICE_CALLBACK_MEMBER(timer_x);
	TIMER_DEVICE_CALLBACK_MEMBER(u11_timer);
	void st_mp100_map(address_map &map);

	uint8_t m_u10a;
	uint8_t m_u10b;
	uint8_t m_u11a;
	uint8_t m_u11b;
	bool m_u10_ca2;
	bool m_u10_cb2;
	bool m_u11_cb2;
	bool m_timer_x;
	bool m_u11_timer;
	uint8_t m_digit;
	uint8_t m_counter;
	uint8_t m_segment[5];
	virtual void machine_reset() override;
	virtual void machine_start() override { m_digits.resolve(); }
	required_device<m6800_cpu_device> m_maincpu;
	required_device<pia6821_device> m_pia_u10;
	required_device<pia6821_device> m_pia_u11;
	required_ioport m_io_test;
	required_ioport m_io_dsw0;
	required_ioport m_io_dsw1;
	required_ioport m_io_dsw2;
	required_ioport m_io_dsw3;
	required_ioport m_io_x0;
	required_ioport m_io_x1;
	required_ioport m_io_x2;
	required_ioport m_io_x3;
	required_ioport m_io_x4;
	output_finder<48> m_digits;
};


void st_mp100_state::st_mp100_map(address_map &map)
{
	map.global_mask(0x1fff);
	map(0x0000, 0x007f).ram(); // internal to the cpu
	map(0x0088, 0x008b).rw(m_pia_u10, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	map(0x0090, 0x0093).rw(m_pia_u11, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	map(0x00a0, 0x00a7).nopw(); // to sound board
	map(0x00c0, 0x00c7); // to sound board
	map(0x0200, 0x02ff).ram().share("nvram");
	map(0x1000, 0x1fff).rom().region("roms", 0);
}

static INPUT_PORTS_START( mp100 )
	PORT_START("TEST")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE1 ) PORT_NAME("Self Test") PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, st_mp100_state, self_test, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE2 ) PORT_NAME("Activity") PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, st_mp100_state, activity_test, 0)

	PORT_START("DSW0")
	PORT_DIPNAME( 0x1f, 0x02, "Coin Slot 2")
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C )) // same as 01
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_2C ))
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ))
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_4C ))
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_5C ))
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ))
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_7C ))
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_7C ))
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_8C ))
	PORT_DIPSETTING(    0x11, DEF_STR( 2C_8C ))
	PORT_DIPSETTING(    0x12, DEF_STR( 1C_9C ))
	PORT_DIPSETTING(    0x13, "2 coins 9 credits")
	PORT_DIPSETTING(    0x14, "1 coin 10 credits")
	PORT_DIPSETTING(    0x15, "2 coins 10 credits")
	PORT_DIPSETTING(    0x16, "1 coin 11 credits")
	PORT_DIPSETTING(    0x17, "2 coins 11 credits")
	PORT_DIPSETTING(    0x18, "1 coin 12 credits")
	PORT_DIPSETTING(    0x19, "2 coins 12 credits")
	PORT_DIPSETTING(    0x1a, "1 coin 13 credits")
	PORT_DIPSETTING(    0x1b, "2 coins 13 credits")
	PORT_DIPSETTING(    0x1c, "1 coin 14 credits")
	PORT_DIPSETTING(    0x1d, "2 coins 14 credits")
	PORT_DIPSETTING(    0x1e, "1 coin 15 credits")
	PORT_DIPSETTING(    0x1f, "2 coins 15 credits")
	PORT_DIPNAME( 0x20, 0x20, "Award")
	PORT_DIPSETTING(    0x00, "Extra Ball")
	PORT_DIPSETTING(    0x20, "Free Game")
	PORT_DIPNAME( 0x40, 0x00, "Balls")
	PORT_DIPSETTING(    0x00, "3")
	PORT_DIPSETTING(    0x40, "5")
	PORT_DIPNAME( 0x80, 0x00, "Play melody always")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x80, DEF_STR( On ))

	PORT_START("DSW1")
	PORT_DIPNAME( 0x1f, 0x02, "Coin Slot 3")
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C )) // same as 01
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_2C ))
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ))
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_4C ))
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_5C ))
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ))
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_7C ))
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_7C ))
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_8C ))
	PORT_DIPSETTING(    0x11, DEF_STR( 2C_8C ))
	PORT_DIPSETTING(    0x12, DEF_STR( 1C_9C ))
	PORT_DIPSETTING(    0x13, "2 coins 9 credits")
	PORT_DIPSETTING(    0x14, "1 coin 10 credits")
	PORT_DIPSETTING(    0x15, "2 coins 10 credits")
	PORT_DIPSETTING(    0x16, "1 coin 11 credits")
	PORT_DIPSETTING(    0x17, "2 coins 11 credits")
	PORT_DIPSETTING(    0x18, "1 coin 12 credits")
	PORT_DIPSETTING(    0x19, "2 coins 12 credits")
	PORT_DIPSETTING(    0x1a, "1 coin 13 credits")
	PORT_DIPSETTING(    0x1b, "2 coins 13 credits")
	PORT_DIPSETTING(    0x1c, "1 coin 14 credits")
	PORT_DIPSETTING(    0x1d, "2 coins 14 credits")
	PORT_DIPSETTING(    0x1e, "1 coin 15 credits")
	PORT_DIPSETTING(    0x1f, "2 coins 15 credits")
	PORT_DIPNAME( 0x20, 0x00, "S14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x20, DEF_STR( On ))
	PORT_DIPNAME( 0x40, 0x00, "Award for beating high score")
	PORT_DIPSETTING(    0x00, "Novelty")
	PORT_DIPSETTING(    0x40, "3 Free Games")
	PORT_DIPNAME( 0x80, 0x00, "Rollover lights")
	PORT_DIPSETTING(    0x00, "Always on")
	PORT_DIPSETTING(    0x80, "Alternate")

	PORT_START("DSW2")
	PORT_DIPNAME( 0x07, 0x02, "Maximum Credits")
	PORT_DIPSETTING(    0x00, "5")
	PORT_DIPSETTING(    0x01, "10")
	PORT_DIPSETTING(    0x02, "15")
	PORT_DIPSETTING(    0x00, "20")
	PORT_DIPSETTING(    0x00, "25")
	PORT_DIPSETTING(    0x00, "30")
	PORT_DIPSETTING(    0x00, "35")
	PORT_DIPSETTING(    0x00, "40")
	PORT_DIPNAME( 0x08, 0x08, "Credits displayed")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x08, DEF_STR( On ))
	PORT_DIPNAME( 0x10, 0x10, "Match")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x10, DEF_STR( On ))
	PORT_DIPNAME( 0x20, 0x00, "S22")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x20, DEF_STR( On ))
	PORT_DIPNAME( 0x40, 0x00, "S23")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x40, DEF_STR( On ))
	PORT_DIPNAME( 0x80, 0x00, "Award a free game for hitting all targets 2nd time")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x80, DEF_STR( On ))

	PORT_START("DSW3")
	PORT_DIPNAME( 0x01, 0x00, "S25")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x01, DEF_STR( On ))
	PORT_DIPNAME( 0x02, 0x00, "S26")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x02, DEF_STR( On ))
	PORT_DIPNAME( 0x04, 0x04, "S27")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x04, DEF_STR( On ))
	PORT_DIPNAME( 0x08, 0x00, "S28")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x08, DEF_STR( On ))
	PORT_DIPNAME( 0x10, 0x00, "S29")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x10, DEF_STR( On ))
	PORT_DIPNAME( 0x20, 0x00, "S30")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x20, DEF_STR( On ))
	PORT_DIPNAME( 0xc0, 0x80, "Award for Special")
	PORT_DIPSETTING(    0x00, "100000 points")
	PORT_DIPSETTING(    0x40, "Extra Ball")
	PORT_DIPSETTING(    0x80, "Free Game")
	PORT_DIPSETTING(    0xc0, "Extra Ball and Free Game")

	PORT_START("X0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_SLASH)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_COLON)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSLASH)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("Outhole") PORT_CODE(KEYCODE_X)

	PORT_START("X1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN3 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_L)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_ENTER)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_TILT1 ) PORT_NAME("Slam Tilt")

	PORT_START("X2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_F)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_G)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_H)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_J)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_K)

	PORT_START("X3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Y)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_I)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_O)

	PORT_START("X4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_N)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_M)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_COMMA)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_STOP)
INPUT_PORTS_END

static INPUT_PORTS_START( mp200 )
	PORT_START("TEST")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE1 ) PORT_NAME("Self Test") PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, st_mp100_state, self_test, 0)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE2 ) PORT_NAME("Activity") PORT_IMPULSE(1) PORT_CHANGED_MEMBER(DEVICE_SELF, st_mp100_state, activity_test, 0)

	PORT_START("DSW0")
	PORT_DIPNAME( 0x1f, 0x02, "Coin Slot 2")
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C )) // same as 01
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_2C ))
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ))
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_4C ))
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_5C ))
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ))
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_7C ))
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_7C ))
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_8C ))
	PORT_DIPSETTING(    0x11, DEF_STR( 2C_8C ))
	PORT_DIPSETTING(    0x12, DEF_STR( 1C_9C ))
	PORT_DIPSETTING(    0x13, "2 coins 9 credits")
	PORT_DIPSETTING(    0x14, "1 coin 10 credits")
	PORT_DIPSETTING(    0x15, "2 coins 10 credits")
	PORT_DIPSETTING(    0x16, "1 coin 11 credits")
	PORT_DIPSETTING(    0x17, "2 coins 11 credits")
	PORT_DIPSETTING(    0x18, "1 coin 12 credits")
	PORT_DIPSETTING(    0x19, "2 coins 12 credits")
	PORT_DIPSETTING(    0x1a, "1 coin 13 credits")
	PORT_DIPSETTING(    0x1b, "2 coins 13 credits")
	PORT_DIPSETTING(    0x1c, "1 coin 14 credits")
	PORT_DIPSETTING(    0x1d, "2 coins 14 credits")
	PORT_DIPSETTING(    0x1e, "1 coin 15 credits")
	PORT_DIPSETTING(    0x1f, "2 coins 15 credits")
	PORT_DIPNAME( 0x20, 0x20, "Award")
	PORT_DIPSETTING(    0x00, "Extra Ball")
	PORT_DIPSETTING(    0x20, "Free Game")
	PORT_DIPNAME( 0x40, 0x00, "Balls")
	PORT_DIPSETTING(    0x00, "3")
	PORT_DIPSETTING(    0x40, "5")
	PORT_DIPNAME( 0x80, 0x00, "Play melody always")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x80, DEF_STR( On ))

	PORT_START("DSW1")
	PORT_DIPNAME( 0x1f, 0x02, "Coin Slot 3")
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C )) // same as 01
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_1C ))
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ))
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ))
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_2C ))
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_3C ))
	PORT_DIPSETTING(    0x07, DEF_STR( 2C_3C ))
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_4C ))
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_4C ))
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_5C ))
	PORT_DIPSETTING(    0x0b, DEF_STR( 2C_5C ))
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_6C ))
	PORT_DIPSETTING(    0x0d, DEF_STR( 2C_6C ))
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_7C ))
	PORT_DIPSETTING(    0x0f, DEF_STR( 2C_7C ))
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_8C ))
	PORT_DIPSETTING(    0x11, DEF_STR( 2C_8C ))
	PORT_DIPSETTING(    0x12, DEF_STR( 1C_9C ))
	PORT_DIPSETTING(    0x13, "2 coins 9 credits")
	PORT_DIPSETTING(    0x14, "1 coin 10 credits")
	PORT_DIPSETTING(    0x15, "2 coins 10 credits")
	PORT_DIPSETTING(    0x16, "1 coin 11 credits")
	PORT_DIPSETTING(    0x17, "2 coins 11 credits")
	PORT_DIPSETTING(    0x18, "1 coin 12 credits")
	PORT_DIPSETTING(    0x19, "2 coins 12 credits")
	PORT_DIPSETTING(    0x1a, "1 coin 13 credits")
	PORT_DIPSETTING(    0x1b, "2 coins 13 credits")
	PORT_DIPSETTING(    0x1c, "1 coin 14 credits")
	PORT_DIPSETTING(    0x1d, "2 coins 14 credits")
	PORT_DIPSETTING(    0x1e, "1 coin 15 credits")
	PORT_DIPSETTING(    0x1f, "2 coins 15 credits")
	PORT_DIPNAME( 0x20, 0x00, "S14")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x20, DEF_STR( On ))
	PORT_DIPNAME( 0x40, 0x00, "Award for beating high score")
	PORT_DIPSETTING(    0x00, "Novelty")
	PORT_DIPSETTING(    0x40, "3 Free Games")
	PORT_DIPNAME( 0x80, 0x00, "Rollover lights")
	PORT_DIPSETTING(    0x00, "Always on")
	PORT_DIPSETTING(    0x80, "Alternate")

	PORT_START("DSW2")
	PORT_DIPNAME( 0x07, 0x02, "Maximum Credits")
	PORT_DIPSETTING(    0x00, "5")
	PORT_DIPSETTING(    0x01, "10")
	PORT_DIPSETTING(    0x02, "15")
	PORT_DIPSETTING(    0x00, "20")
	PORT_DIPSETTING(    0x00, "25")
	PORT_DIPSETTING(    0x00, "30")
	PORT_DIPSETTING(    0x00, "35")
	PORT_DIPSETTING(    0x00, "40")
	PORT_DIPNAME( 0x08, 0x08, "Credits displayed")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x08, DEF_STR( On ))
	PORT_DIPNAME( 0x10, 0x10, "Match")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x10, DEF_STR( On ))
	PORT_DIPNAME( 0x20, 0x00, "S22")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x20, DEF_STR( On ))
	PORT_DIPNAME( 0x40, 0x00, "S23")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x40, DEF_STR( On ))
	PORT_DIPNAME( 0x80, 0x00, "Award a free game for hitting all targets 2nd time")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x80, DEF_STR( On ))

	PORT_START("DSW3")
	PORT_DIPNAME( 0x01, 0x00, "S25")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x01, DEF_STR( On ))
	PORT_DIPNAME( 0x02, 0x00, "S26")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x02, DEF_STR( On ))
	PORT_DIPNAME( 0x04, 0x04, "S27")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x04, DEF_STR( On ))
	PORT_DIPNAME( 0x08, 0x00, "S28")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x08, DEF_STR( On ))
	PORT_DIPNAME( 0x10, 0x00, "S29")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x10, DEF_STR( On ))
	PORT_DIPNAME( 0x20, 0x00, "S30")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x20, DEF_STR( On ))
	PORT_DIPNAME( 0xc0, 0x80, "Award for Special")
	PORT_DIPSETTING(    0x00, "100000 points")
	PORT_DIPSETTING(    0x40, "Extra Ball")
	PORT_DIPSETTING(    0x80, "Free Game")
	PORT_DIPSETTING(    0xc0, "Extra Ball and Free Game")

	PORT_START("X0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN3 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSLASH)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_TILT1 ) PORT_NAME("Slam Tilt")

	PORT_START("X1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_SLASH)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_COLON)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_L)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_ENTER)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Z)

	PORT_START("X2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_F)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_G)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_H)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_J)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_K)

	PORT_START("X3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_Y)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_I)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_O)

	PORT_START("X4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME("Outhole") PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_N)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_M)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_COMMA)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_STOP)
INPUT_PORTS_END

INPUT_CHANGED_MEMBER( st_mp100_state::activity_test )
{
	if(newval)
		m_maincpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
}

INPUT_CHANGED_MEMBER( st_mp100_state::self_test )
{
	m_pia_u10->ca1_w(newval);
}

WRITE_LINE_MEMBER( st_mp100_state::u10_ca2_w )
{
	m_u10_ca2 = state;
	if (!state)
		m_counter = 0;
}

WRITE_LINE_MEMBER( st_mp100_state::u10_cb2_w )
{
}

WRITE_LINE_MEMBER( st_mp100_state::u11_ca2_w )
{
	output().set_value("led0", !state);
}

WRITE_LINE_MEMBER( st_mp100_state::u11_cb2_w )
{
	m_u11_cb2 = state;
}

READ8_MEMBER( st_mp100_state::u10_a_r )
{
	return m_u10a;
}

WRITE8_MEMBER( st_mp100_state::u10_a_w )
{
	m_u10a = data;

	if (!m_u10_ca2)
	{
		m_counter++;

		if (m_counter==1)
			m_segment[0] = data>>4;
		else
		if (m_counter==3)
			m_segment[1] = data>>4;
		else
		if (m_counter==5)
			m_segment[2] = data>>4;
		else
		if (m_counter==7)
			m_segment[3] = data>>4;
		else
		if (m_counter==9)
			m_segment[4] = data>>4;
	}
}

READ8_MEMBER( st_mp100_state::u10_b_r )
{
	uint8_t data = 0;

	if (BIT(m_u10a, 0))
		data |= m_io_x0->read();

	if (BIT(m_u10a, 1))
		data |= m_io_x1->read();

	if (BIT(m_u10a, 2))
		data |= m_io_x2->read();

	if (BIT(m_u10a, 3))
		data |= m_io_x3->read();

	if (BIT(m_u10a, 4))
		data |= m_io_x4->read();

	if (BIT(m_u10a, 5))
		data |= m_io_dsw0->read();

	if (BIT(m_u10a, 6))
		data |= m_io_dsw1->read();

	if (BIT(m_u10a, 7))
		data |= m_io_dsw2->read();

	if (m_u10_cb2)
		data |= m_io_dsw3->read();

	return data;
}

WRITE8_MEMBER( st_mp100_state::u10_b_w )
{
	m_u10b = data;
}

READ8_MEMBER( st_mp100_state::u11_a_r )
{
	return m_u11a;
}

WRITE8_MEMBER( st_mp100_state::u11_a_w )
{
	m_u11a = data;

	if (!m_u10_ca2)
	{
		if (BIT(data, 2))
			m_digit = 5;
		else if (BIT(data, 3))
			m_digit = 4;
		else if (BIT(data, 4))
			m_digit = 3;
		else if (BIT(data, 5))
			m_digit = 2;
		else if (BIT(data, 6))
			m_digit = 1;
		else if (BIT(data, 7))
			m_digit = 0;

		if (BIT(data, 0) && (m_counter > 8))
		{
			static const uint8_t patterns[16] = { 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0,0,0,0,0,0 }; // MC14543
			m_digits[m_digit] = patterns[m_segment[0]];
			m_digits[10+m_digit] = patterns[m_segment[1]];
			m_digits[20+m_digit] = patterns[m_segment[2]];
			m_digits[30+m_digit] = patterns[m_segment[3]];
			m_digits[40+m_digit] = patterns[m_segment[4]];
		}
	}
}

WRITE8_MEMBER( st_mp100_state::u11_b_w )
{
	m_u11b = data;
	if (!m_u11_cb2)
	{
		switch (data & 15)
		{
			case 0x0: // chime 10
				m_samples->start(1, 1);
				break;
			case 0x1: // chime 100
				m_samples->start(2, 2);
				break;
			case 0x2: // chime 1000
				m_samples->start(3, 3);
				break;
			case 0x3: // chime 10000
				m_samples->start(0, 4);
				break;
			case 0x4: // chime 10000
				m_samples->start(0, 4);
				break;
			case 0x5: // knocker
				m_samples->start(0, 6);
				break;
			case 0x6: // outhole
				m_samples->start(0, 5);
				break;
			// from here, vary per game
			case 0x7:
			case 0x8:
			case 0x9:
				//m_samples->start(0, 5);
				break;
			case 0xa:
				m_samples->start(0, 0);
				break;
			case 0xb:
				m_samples->start(0, 0);
				break;
			case 0xc:
				m_samples->start(0, 0);
				break;
			case 0xd:
				//m_samples->start(0, 0);
				break;
			case 0xe:
				//m_samples->start(0, 5);
				break;
			case 0xf: // not used
				break;
		}
	}
}

void st_mp100_state::machine_reset()
{
	m_u10a = 0;
	m_u10b = 0;
	m_u10_cb2 = 0;
	m_u11a = 0;
	m_u11b = 0;
}

// zero-cross detection
TIMER_DEVICE_CALLBACK_MEMBER( st_mp100_state::timer_x )
{
	m_timer_x ^= 1;
	m_pia_u10->cb1_w(m_timer_x);
}

// 555 timer for display refresh
TIMER_DEVICE_CALLBACK_MEMBER( st_mp100_state::u11_timer )
{
	m_u11_timer ^= 1;
	m_pia_u11->ca1_w(m_u11_timer);
}

void st_mp100_state::st_mp100(machine_config &config)
{
	/* basic machine hardware */
	M6800(config, m_maincpu, 1000000); // no xtal, just 2 chips forming a random oscillator
	m_maincpu->set_addrmap(AS_PROGRAM, &st_mp100_state::st_mp100_map);

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);

	/* Video */
	config.set_default_layout(layout_st_mp100);

	/* Sound */
	genpin_audio(config);

	/* Devices */
	PIA6821(config, m_pia_u10, 0);
	m_pia_u10->readpa_handler().set(FUNC(st_mp100_state::u10_a_r));
	m_pia_u10->writepa_handler().set(FUNC(st_mp100_state::u10_a_w));
	m_pia_u10->readpb_handler().set(FUNC(st_mp100_state::u10_b_r));
	m_pia_u10->writepb_handler().set(FUNC(st_mp100_state::u10_b_w));
	m_pia_u10->ca2_handler().set(FUNC(st_mp100_state::u10_ca2_w));
	m_pia_u10->cb2_handler().set(FUNC(st_mp100_state::u10_cb2_w));
	m_pia_u10->irqa_handler().set_inputline("maincpu", M6800_IRQ_LINE);
	m_pia_u10->irqb_handler().set_inputline("maincpu", M6800_IRQ_LINE);
	TIMER(config, "timer_x").configure_periodic(FUNC(st_mp100_state::timer_x), attotime::from_hz(120)); // mains freq*2

	PIA6821(config, m_pia_u11, 0);
	m_pia_u11->readpa_handler().set(FUNC(st_mp100_state::u11_a_r));
	m_pia_u11->writepa_handler().set(FUNC(st_mp100_state::u11_a_w));
	m_pia_u11->writepb_handler().set(FUNC(st_mp100_state::u11_b_w));
	m_pia_u11->ca2_handler().set(FUNC(st_mp100_state::u11_ca2_w));
	m_pia_u11->cb2_handler().set(FUNC(st_mp100_state::u11_cb2_w));
	m_pia_u11->irqa_handler().set_inputline("maincpu", M6800_IRQ_LINE);
	m_pia_u11->irqb_handler().set_inputline("maincpu", M6800_IRQ_LINE);
	TIMER(config, "timer_d").configure_periodic(FUNC(st_mp100_state::u11_timer), attotime::from_hz(634)); // 555 timer*2
}


/*--------------------------------
/ Pinball #101
/-------------------------------*/
ROM_START(pinball)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(1db32a33) SHA1(2f0a3ca36968b81f29373e4f2cf7ee28a4071882))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(432e9b9e) SHA1(292e509f50bc841f6e469c198fc82c2a9095f008))
ROM_END

/*------------------------------------
/ Stingray #102 - same roms as Pinball
/-------------------------------------*/
ROM_START(stingray)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(1db32a33) SHA1(2f0a3ca36968b81f29373e4f2cf7ee28a4071882))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(432e9b9e) SHA1(292e509f50bc841f6e469c198fc82c2a9095f008))
ROM_END

/*--------------------------------
/ Stars #103
/-------------------------------*/
ROM_START(stars)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(630d05df) SHA1(2baa16265d524297332fa951d9eab3e0e8d26078))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(57e63d42) SHA1(619ef955553654893c3071d8b70855fee8a5e6a7))
ROM_END

/*--------------------------------
/ Memory Lane #104
/-------------------------------*/
ROM_START(memlane)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(aff1859d) SHA1(5a9801d139bf2477b6d351a2654ae07516be144a))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(3e236e3c) SHA1(7f631a5fac8a1b1af3b5332ba38d52553f13531a))
ROM_END

/*--------------------------------
/ Lectronamo #105
/-------------------------------*/
ROM_START(lectrono)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(79e918ff) SHA1(a728eb26d941a9c7484be593a216905237d32551))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(7c6e5fb5) SHA1(3aa4e0c1f377ba024e6b34bd431a188ff02d4eaa))
ROM_END

/*--------------------------------
/ Wildfyre #106
/-------------------------------*/
ROM_START(wildfyre)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(063f8b5e) SHA1(80434de549102bff829b474603d6736b839b8999))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(00336fbc) SHA1(d2c360b8a80b209ecf4ec02ee19a5234c0364504))
ROM_END

/*-----------------------------------
/ Nugent #108 - same roms as lectrono
/------------------------------------*/
ROM_START(nugent)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(79e918ff) SHA1(a728eb26d941a9c7484be593a216905237d32551))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(7c6e5fb5) SHA1(3aa4e0c1f377ba024e6b34bd431a188ff02d4eaa))
ROM_END

/*------------------------------------
/ Dracula #109 - same roms as wildfyre
/-------------------------------------*/
ROM_START(dracula)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(063f8b5e) SHA1(80434de549102bff829b474603d6736b839b8999))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(00336fbc) SHA1(d2c360b8a80b209ecf4ec02ee19a5234c0364504))
ROM_END

/*--------------------------------
/ Trident #110
/-------------------------------*/
ROM_START(trident)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "25arom_p11a.u2",  0x0000, 0x0800, CRC(6dcd6ad3) SHA1(f748acc8628c5013b630a5c7b25a1bf72e36b16d))   // 9316A-2920
	ROM_LOAD( "25arom_p12au.u6", 0x0800, 0x0800, CRC(fb955a6f) SHA1(387080d5af318463475797fecff026d6db776a0c))   // 9316A-2921
ROM_END

ROM_START(tridento)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "25arom_p11.u2",  0x0000, 0x0800, CRC(934e49dd) SHA1(cbf6ca2759166f522f651825da0c75cf7248d3da))
	ROM_LOAD( "25arom_p12u.u6", 0x0800, 0x0800, CRC(540bce56) SHA1(0b21385501b83e448403e0216371487ed54026b7))
ROM_END

/*-------------------------------------
/ Cosmic Princess #111 - same ROMs as Magic
/-------------------------------------*/
ROM_START(princess)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(8838091f) SHA1(d2702b5e15076793b4560c77b78eed6c1da571b6))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(fb955a6f) SHA1(387080d5af318463475797fecff026d6db776a0c))
ROM_END

/*--------------------------------
/ Hot Hand #112
/-------------------------------*/
ROM_START(hothand)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(5e79ea2e) SHA1(9b45c59b2076fcb3a35de1dd3ba2444ea852f149))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(fb955a6f) SHA1(387080d5af318463475797fecff026d6db776a0c))
ROM_END

/*------------------------------------
/ Magic #115 - 2nd rom same as hothand
/-------------------------------------*/
ROM_START(magic)
	ROM_REGION(0x1000, "roms", 0)
	ROM_LOAD( "cpu_u2.716", 0x0000, 0x0800, CRC(8838091f) SHA1(d2702b5e15076793b4560c77b78eed6c1da571b6))
	ROM_LOAD( "cpu_u6.716", 0x0800, 0x0800, CRC(fb955a6f) SHA1(387080d5af318463475797fecff026d6db776a0c))
ROM_END

// chimes
GAME( 1977,  pinball,    0,      st_mp100,   mp100, st_mp100_state, empty_init, ROT0, "Stern", "Pinball",           MACHINE_MECHANICAL | MACHINE_NOT_WORKING )
GAME( 1977,  stingray,   0,      st_mp100,   mp100, st_mp100_state, empty_init, ROT0, "Stern", "Stingray",          MACHINE_MECHANICAL | MACHINE_NOT_WORKING )
GAME( 1978,  stars,      0,      st_mp100,   mp100, st_mp100_state, empty_init, ROT0, "Stern", "Stars",             MACHINE_MECHANICAL | MACHINE_NOT_WORKING )
GAME( 1978,  memlane,    0,      st_mp100,   mp100, st_mp100_state, empty_init, ROT0, "Stern", "Memory Lane",       MACHINE_MECHANICAL | MACHINE_NOT_WORKING )

// sound unit B-521
GAME( 1978,  lectrono,   0,      st_mp100,   mp100, st_mp100_state, empty_init, ROT0, "Stern", "Lectronamo",        MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
GAME( 1978,  wildfyre,   0,      st_mp100,   mp100, st_mp100_state, empty_init, ROT0, "Stern", "Wildfyre",          MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
GAME( 1978,  nugent,     0,      st_mp100,   mp100, st_mp100_state, empty_init, ROT0, "Stern", "Nugent",            MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
GAME( 1979,  dracula,    0,      st_mp100,   mp100, st_mp100_state, empty_init, ROT0, "Stern", "Dracula (Pinball)", MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )

// different inputs
GAME( 1979,  trident,    0,       st_mp100,  mp200, st_mp100_state, empty_init, ROT0, "Stern", "Trident",             MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
GAME( 1979,  tridento,   trident, st_mp100,  mp200, st_mp100_state, empty_init, ROT0, "Stern", "Trident (Older set)", MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
GAME( 1979,  hothand,    0,       st_mp100,  mp200, st_mp100_state, empty_init, ROT0, "Stern", "Hot Hand",            MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
GAME( 1979,  princess,   0,       st_mp100,  mp200, st_mp100_state, empty_init, ROT0, "Stern", "Cosmic Princess",     MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
GAME( 1979,  magic,      0,       st_mp100,  mp200, st_mp100_state, empty_init, ROT0, "Stern", "Magic",               MACHINE_MECHANICAL | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
