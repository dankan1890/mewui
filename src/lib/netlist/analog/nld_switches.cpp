// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_legacy.c
 *
 */

#include "nld_switches.h"
#include "nl_setup.h"

#define R_OFF   (1.0 / netlist().gmin())
#define R_ON    0.01

namespace netlist
{
	namespace devices
	{
// ----------------------------------------------------------------------------------------
// SWITCH
// ----------------------------------------------------------------------------------------

NETLIB_RESET(switch1)
{
	m_R.set_R(R_OFF);
}

NETLIB_UPDATE(switch1)
{
	if (!m_POS())
	{
		m_R.set_R(R_OFF);
	}
	else
	{
		m_R.set_R(R_ON);
	}

	m_R.update_dev();
}

NETLIB_UPDATE_PARAM(switch1)
{
	update();
}

// ----------------------------------------------------------------------------------------
// SWITCH2
// ----------------------------------------------------------------------------------------


NETLIB_RESET(switch2)
{
	m_R1.set_R(R_ON);
	m_R2.set_R(R_OFF);
}

NETLIB_UPDATE(switch2)
{
	if (!m_POS())
	{
		m_R1.set_R(R_ON);
		m_R2.set_R(R_OFF);
	}
	else
	{
		m_R1.set_R(R_OFF);
		m_R2.set_R(R_ON);
	}

	m_R1.update_dev();
	m_R2.update_dev();
}

NETLIB_UPDATE_PARAM(switch2)
{
	update();
}

	} //namespace devices
} // namespace netlist
