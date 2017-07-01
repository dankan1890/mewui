// license:BSD-3-Clause
// copyright-holders:Barry Rodewald
/*
 * digiblst.c
 *
 *  Created on: 23/08/2014
 */

#include "emu.h"
#include "digiblst.h"
#include "sound/volt_reg.h"

//**************************************************************************
//  COVOX DEVICE
//**************************************************************************

// device type definition
const device_type CENTRONICS_DIGIBLASTER = &device_creator<centronics_digiblaster_device>;

static MACHINE_CONFIG_FRAGMENT( digiblst )
	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("speaker")
	MCFG_SOUND_ADD("dac", DAC_8BIT_R2R, 0) MCFG_SOUND_ROUTE(ALL_OUTPUTS, "speaker", 0.5) // unknown DAC
	MCFG_DEVICE_ADD("vref", VOLTAGE_REGULATOR, 0) MCFG_VOLTAGE_REGULATOR_OUTPUT(5.0)
	MCFG_SOUND_ROUTE_EX(0, "dac", 1.0, DAC_VREF_POS_INPUT) MCFG_SOUND_ROUTE_EX(0, "dac", -1.0, DAC_VREF_NEG_INPUT)
MACHINE_CONFIG_END


/***************************************************************************
    IMPLEMENTATION
***************************************************************************/
//-------------------------------------------------
//  centronics_covox_device - constructor
//-------------------------------------------------

centronics_digiblaster_device::centronics_digiblaster_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, CENTRONICS_DIGIBLASTER, "Digiblaster (DIY)", tag, owner, clock, "digiblst", __FILE__),
	device_centronics_peripheral_interface( mconfig, *this ),
	m_dac(*this, "dac"),
	m_data(0)
{
}

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor centronics_digiblaster_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( digiblst );
}

void centronics_digiblaster_device::device_start()
{
	save_item(NAME(m_data));
}

void centronics_digiblaster_device::update_dac()
{
	if (started())
		m_dac->write(m_data);
}
