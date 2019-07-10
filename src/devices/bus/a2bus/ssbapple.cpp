// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    ssbapple.c

    Implementation of the SSB Apple speech card
    Must be in slot 2 for the provided software to work!

*********************************************************************/

#include "emu.h"
#include "ssbapple.h"
#include "sound/tms5220.h"
#include "speaker.h"

/***************************************************************************
    PARAMETERS
***************************************************************************/

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(A2BUS_SSBAPPLE, a2bus_ssb_device, "a2ssbapl", "Multitech Industrial SSB Apple speech card")

#define TMS_TAG         "tms5220"

/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void a2bus_ssb_device::device_add_mconfig(machine_config &config)
{
	SPEAKER(config, "ssbapple").front_center();
	TMS5220(config, m_tms, 640000); // guess - this gives 8 kHz output according to the datasheet
	m_tms->add_route(ALL_OUTPUTS, "ssbapple", 1.0);
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

a2bus_ssb_device::a2bus_ssb_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, type, tag, owner, clock),
	device_a2bus_card_interface(mconfig, *this),
	m_tms(*this, TMS_TAG)
{
}

a2bus_ssb_device::a2bus_ssb_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	a2bus_ssb_device(mconfig, A2BUS_SSBAPPLE, tag, owner, clock)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void a2bus_ssb_device::device_start()
{
	if (slotno() != 2)
		popmessage("SSB Card should be in slot 2!\n");
}

void a2bus_ssb_device::device_reset()
{
}

bool a2bus_ssb_device::take_c800()
{
	return false;
}

uint8_t a2bus_ssb_device::read_cnxx(uint8_t offset)
{
	return 0x1f | m_tms->status_r();
}

void a2bus_ssb_device::write_cnxx(uint8_t offset, uint8_t data)
{
	m_tms->data_w(data);
}
