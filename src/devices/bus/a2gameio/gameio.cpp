// license:BSD-3-Clause
// copyright-holders:AJR
/*********************************************************************

    Apple II Game I/O Connector

    This 16-pin DIP socket is described in the Apple II Reference
    Manual (January 1978) as "a means of connecting paddle controls,
    lights and switches to the APPLE II for use in controlling video
    games, etc." The connector provides for four analog "paddle"
    input signals (0-150KΩ resistance) which are converted to
    digital pulses by a NE558 quad timer on the main board. The
    connector also provides several digital switch inputs and
    "annunciator" outputs, all LS/TTL compatible. Apple joysticks
    provide active high switches (though at least one third-party
    product treats them as active low) and Apple main boards have no
    pullups on these inputs, which thus read 0 if disconnected.

    While pins 9 and 16 are unconnected on the Apple II, they provide
    additional digital output and input pins respectively on the Sanyo
    MBC-550/555 (which uses 74LS123 monostables instead of a NE558).
    The Apple IIgs also recognizes a switch input 3, though this is
    placed on pin 9 of the internal connector rather than 16.

    The Apple IIe, IIc and IIgs also have an external DE-9 connector
    that carries a subset of the signals, excluding the annunciator
    outputs and utility strobe (which the IIc and IIgs do not have).

**********************************************************************
                            ____________
                   +5V   1 |*           | 16  (SW3)
                   SW0   2 |            | 15  AN0
                   SW1   3 |            | 14  AN1
                   SW2   4 |            | 13  AN2
                  /STB   5 |  GAME I/O  | 12  AN3
                  PDL0   6 |            | 11  PDL3
                  PDL2   7 |            | 10  PDL1
                   GND   8 |            |  9  (AN4/SW3)
                            ------------

                  ---------------------------------
                  \  PDL0  PDL2  GND   +5V   SW1  /
                   \ (5)   (4)   (3)   (2)   (1) /
                    \   (9)   (8)   (7)   (6)   /
                     \  PDL3  PDL1  SW0   SW2  /
                      \_______________________/

*********************************************************************/

#include "emu.h"
#include "bus/a2gameio/gameio.h"
#include "bus/a2gameio/joystick.h"
#include "bus/a2gameio/joyport.h"
#include "bus/a2gameio/computereyes.h"


//**************************************************************************
//  CONNECTOR DEVICE IMPLEMENTATION
//**************************************************************************

// device type definition
DEFINE_DEVICE_TYPE(APPLE2_GAMEIO, apple2_gameio_device, "a2gameio", "Apple II Game I/O Connector")

apple2_gameio_device::apple2_gameio_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, APPLE2_GAMEIO, tag, owner, clock)
	, device_slot_interface(mconfig, *this)
	, m_intf(nullptr)
{
}

void apple2_gameio_device::iiandplus_options(device_slot_interface &slot)
{
	slot.option_add("joy", APPLE2_JOYSTICK);
	slot.option_add("joyport", APPLE2_JOYPORT);
	slot.option_add("compeyes", APPLE2_COMPUTEREYES);
}

void apple2_gameio_device::default_options(device_slot_interface &slot)
{
	slot.option_add("joy", APPLE2_JOYSTICK);
	slot.option_add("compeyes", APPLE2_COMPUTEREYES);
}

void apple2_gameio_device::device_config_complete()
{
	m_intf = dynamic_cast<device_a2gameio_interface *>(get_card_device());
}

void apple2_gameio_device::device_resolve_objects()
{
	if (m_intf != nullptr)
		m_intf->m_connector = this;
}

void apple2_gameio_device::device_start()
{
}


//**************************************************************************
//  PASSTHROUGH HANDLERS
//**************************************************************************

u8 apple2_gameio_device::pdl0_r()
{
	if (m_intf != nullptr)
		return m_intf->pdl0_r();

	return 0;
}

u8 apple2_gameio_device::pdl1_r()
{
	if (m_intf != nullptr)
		return m_intf->pdl1_r();

	return 0;
}

u8 apple2_gameio_device::pdl2_r()
{
	if (m_intf != nullptr)
		return m_intf->pdl2_r();

	return 0;
}

u8 apple2_gameio_device::pdl3_r()
{
	if (m_intf != nullptr)
		return m_intf->pdl3_r();

	return 0;
}

READ_LINE_MEMBER(apple2_gameio_device::sw0_r)
{
	if (m_intf != nullptr)
		return m_intf->sw0_r();

	return m_sw_pullups ? 1 : 0;
}

READ_LINE_MEMBER(apple2_gameio_device::sw1_r)
{
	if (m_intf != nullptr)
		return m_intf->sw1_r();

	return m_sw_pullups ? 1 : 0;
}

READ_LINE_MEMBER(apple2_gameio_device::sw2_r)
{
	if (m_intf != nullptr)
		return m_intf->sw2_r();

	return m_sw_pullups ? 1 : 0;
}

READ_LINE_MEMBER(apple2_gameio_device::sw3_r)
{
	if (m_intf != nullptr)
		return m_intf->sw3_r();

	return m_sw_pullups ? 1 : 0;
}

WRITE_LINE_MEMBER(apple2_gameio_device::an0_w)
{
	if (m_intf != nullptr)
		m_intf->an0_w(state);
}

WRITE_LINE_MEMBER(apple2_gameio_device::an1_w)
{
	if (m_intf != nullptr)
		m_intf->an1_w(state);
}

WRITE_LINE_MEMBER(apple2_gameio_device::an2_w)
{
	if (m_intf != nullptr)
		m_intf->an2_w(state);
}

WRITE_LINE_MEMBER(apple2_gameio_device::an3_w)
{
	if (m_intf != nullptr)
		m_intf->an3_w(state);
}

WRITE_LINE_MEMBER(apple2_gameio_device::an4_w)
{
	if (m_intf != nullptr)
		m_intf->an4_w(state);
}

WRITE_LINE_MEMBER(apple2_gameio_device::strobe_w)
{
	if (m_intf != nullptr)
		m_intf->strobe_w(state);
}


//**************************************************************************
//  GAME I/O DEVICE INTERFACE
//**************************************************************************

device_a2gameio_interface::device_a2gameio_interface(const machine_config &mconfig, device_t &device)
	: device_slot_card_interface(mconfig, device)
	, m_connector(nullptr)
{
}

device_a2gameio_interface::~device_a2gameio_interface()
{
}
