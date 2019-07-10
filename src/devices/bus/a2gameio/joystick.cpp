// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    Apple II analog joysticks

*********************************************************************/

#include "emu.h"
#include "bus/a2gameio/joystick.h"

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// device type definition
DEFINE_DEVICE_TYPE(APPLE2_JOYSTICK, apple2_joystick_device, "a2joy", "Apple II analog joysticks")

//**************************************************************************
//  PARAMETERS
//**************************************************************************

#define JOYSTICK_DELTA          80
#define JOYSTICK_SENSITIVITY    50
#define JOYSTICK_AUTOCENTER     80

//**************************************************************************
//  INPUT PORTS
//**************************************************************************

static INPUT_PORTS_START( apple2_joystick )
	PORT_START("joystick_1_x")      /* Joystick 1 X Axis */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X) PORT_NAME("P1 Joystick X")
	PORT_SENSITIVITY(JOYSTICK_SENSITIVITY)
	PORT_KEYDELTA(JOYSTICK_DELTA)
	PORT_CENTERDELTA(JOYSTICK_AUTOCENTER)
	PORT_MINMAX(0,0xff) PORT_PLAYER(1)
	PORT_CODE_DEC(KEYCODE_4_PAD)    PORT_CODE_INC(KEYCODE_6_PAD)
	PORT_CODE_DEC(JOYCODE_X_LEFT_SWITCH)    PORT_CODE_INC(JOYCODE_X_RIGHT_SWITCH)

	PORT_START("joystick_1_y")      /* Joystick 1 Y Axis */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y) PORT_NAME("P1 Joystick Y")
	PORT_SENSITIVITY(JOYSTICK_SENSITIVITY)
	PORT_KEYDELTA(JOYSTICK_DELTA)
	PORT_CENTERDELTA(JOYSTICK_AUTOCENTER)
	PORT_MINMAX(0,0xff) PORT_PLAYER(1)
	PORT_CODE_DEC(KEYCODE_8_PAD)    PORT_CODE_INC(KEYCODE_2_PAD)
	PORT_CODE_DEC(JOYCODE_Y_UP_SWITCH)      PORT_CODE_INC(JOYCODE_Y_DOWN_SWITCH)

	PORT_START("joystick_2_x")      /* Joystick 2 X Axis */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X) PORT_NAME("P2 Joystick X")
	PORT_SENSITIVITY(JOYSTICK_SENSITIVITY)
	PORT_KEYDELTA(JOYSTICK_DELTA)
	PORT_CENTERDELTA(JOYSTICK_AUTOCENTER)
	PORT_MINMAX(0,0xff) PORT_PLAYER(2)
	PORT_CODE_DEC(JOYCODE_X_LEFT_SWITCH)    PORT_CODE_INC(JOYCODE_X_RIGHT_SWITCH)

	PORT_START("joystick_2_y")      /* Joystick 2 Y Axis */
	PORT_BIT( 0xff, 0x80, IPT_AD_STICK_Y) PORT_NAME("P2 Joystick Y")
	PORT_SENSITIVITY(JOYSTICK_SENSITIVITY)
	PORT_KEYDELTA(JOYSTICK_DELTA)
	PORT_CENTERDELTA(JOYSTICK_AUTOCENTER)
	PORT_MINMAX(0,0xff) PORT_PLAYER(2)
	PORT_CODE_DEC(JOYCODE_Y_UP_SWITCH)      PORT_CODE_INC(JOYCODE_Y_DOWN_SWITCH)

	PORT_START("joystick_buttons")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1)   PORT_PLAYER(1)            PORT_CODE(KEYCODE_0_PAD)    PORT_CODE(JOYCODE_BUTTON1)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2)   PORT_PLAYER(1)            PORT_CODE(KEYCODE_ENTER_PAD)PORT_CODE(JOYCODE_BUTTON2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1)   PORT_PLAYER(2)            PORT_CODE(JOYCODE_BUTTON1)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON2)   PORT_PLAYER(2) PORT_CODE(JOYCODE_BUTTON2)
INPUT_PORTS_END

//**************************************************************************
//  DEVICE IMPLEMENTATION
//**************************************************************************

apple2_joystick_device::apple2_joystick_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, APPLE2_JOYSTICK, tag, owner, clock)
	, device_a2gameio_interface(mconfig, *this)
	, m_joy_x(*this, "joystick_%u_x", 1U)
	, m_joy_y(*this, "joystick_%u_y", 1U)
	, m_buttons(*this, "joystick_buttons")
{
}

ioport_constructor apple2_joystick_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(apple2_joystick);
}

void apple2_joystick_device::device_start()
{
}

u8 apple2_joystick_device::pdl0_r()
{
	return m_joy_x[0]->read();
}

u8 apple2_joystick_device::pdl1_r()
{
	return m_joy_y[0]->read();
}

u8 apple2_joystick_device::pdl2_r()
{
	return m_joy_x[1]->read();
}

u8 apple2_joystick_device::pdl3_r()
{
	return m_joy_x[1]->read();
}

READ_LINE_MEMBER(apple2_joystick_device::sw0_r)
{
	return BIT(m_buttons->read(), 4);
}

READ_LINE_MEMBER(apple2_joystick_device::sw1_r)
{
	return BIT(m_buttons->read(), 5);
}

READ_LINE_MEMBER(apple2_joystick_device::sw2_r)
{
	return BIT(m_buttons->read(), 6);
}

READ_LINE_MEMBER(apple2_joystick_device::sw3_r)
{
	return BIT(m_buttons->read(), 7);
}
