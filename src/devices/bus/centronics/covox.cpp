// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

    Covox Speech Thing

***************************************************************************/

#include "emu.h"
#include "covox.h"
#include "sound/volt_reg.h"

//**************************************************************************
//  COVOX DEVICE
//**************************************************************************

// device type definition
const device_type CENTRONICS_COVOX = &device_creator<centronics_covox_device>;

static MACHINE_CONFIG_FRAGMENT( covox )
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

centronics_covox_device::centronics_covox_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, CENTRONICS_COVOX, "Covox Speech Thing", tag, owner, clock, "covox", __FILE__),
	device_centronics_peripheral_interface( mconfig, *this ),
	m_dac(*this, "dac"),
	m_data(0)
{
}

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor centronics_covox_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( covox );
}

void centronics_covox_device::device_start()
{
	save_item(NAME(m_data));
}

void centronics_covox_device::update_dac()
{
	if (started())
		m_dac->write(m_data);
}

//**************************************************************************
//  COVOX STEREO DEVICE
//**************************************************************************

// device type definition
const device_type CENTRONICS_COVOX_STEREO = &device_creator<centronics_covox_stereo_device>;

static MACHINE_CONFIG_FRAGMENT( covox_stereo )
	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SOUND_ADD("ldac", DAC_8BIT_R2R, 0) MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.5) // unknown DAC
	MCFG_SOUND_ADD("rdac", DAC_8BIT_R2R, 0) MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.5) // unknown DAC
	MCFG_DEVICE_ADD("vref", VOLTAGE_REGULATOR, 0) MCFG_VOLTAGE_REGULATOR_OUTPUT(5.0)
	MCFG_SOUND_ROUTE_EX(0, "ldac", 1.0, DAC_VREF_POS_INPUT) MCFG_SOUND_ROUTE_EX(0, "ldac", -1.0, DAC_VREF_NEG_INPUT)
	MCFG_SOUND_ROUTE_EX(0, "rdac", 1.0, DAC_VREF_POS_INPUT) MCFG_SOUND_ROUTE_EX(0, "rdac", -1.0, DAC_VREF_NEG_INPUT)
MACHINE_CONFIG_END


/***************************************************************************
    IMPLEMENTATION
***************************************************************************/
//-------------------------------------------------
//  centronics_covox_stereo_device - constructor
//-------------------------------------------------

centronics_covox_stereo_device::centronics_covox_stereo_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, CENTRONICS_COVOX_STEREO, "Covox (Stereo-in-1)", tag, owner, clock, "covox_stereo", __FILE__),
	device_centronics_peripheral_interface( mconfig, *this ),
	m_ldac(*this, "ldac"),
	m_rdac(*this, "rdac"),
	m_strobe(0),
	m_data(0),
	m_autofd(0)
{
}

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor centronics_covox_stereo_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( covox_stereo );
}

void centronics_covox_stereo_device::device_start()
{
	save_item(NAME(m_data));
	save_item(NAME(m_strobe));
	save_item(NAME(m_autofd));
}

void centronics_covox_stereo_device::update_dac()
{
	if (started())
	{
		if (m_strobe)
			m_ldac->write(m_data);

		if (m_autofd)
			m_rdac->write(m_data);
	}
}
