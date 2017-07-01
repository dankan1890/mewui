// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_twoterm.c
 *
 */

#include <solver/nld_solver.h>
#include <algorithm>

#include "nld_twoterm.h"

namespace netlist
{
	namespace devices
	{
// ----------------------------------------------------------------------------------------
// generic_diode
// ----------------------------------------------------------------------------------------

generic_diode::generic_diode(device_t &dev, pstring name)
	: m_Vd(dev, name + ".m_Vd", 0.7)
	, m_Id(dev, name + ".m_Id", 0.0)
	, m_G(dev,  name + ".m_G", 1e-15)
	, m_Vt(0.0)
	, m_Is(0.0)
	, m_n(0.0)
	, m_gmin(1e-15)
	, m_VtInv(0.0)
	, m_Vcrit(0.0)
{
	set_param(1e-15, 1, 1e-15);
}

void generic_diode::set_param(const nl_double Is, const nl_double n, nl_double gmin)
{
	static const double csqrt2 = std::sqrt(2.0);
	m_Is = Is;
	m_n = n;
	m_gmin = gmin;

	m_Vt = 0.0258 * m_n;

	m_Vcrit = m_Vt * std::log(m_Vt / m_Is / csqrt2);
	m_VtInv = 1.0 / m_Vt;
}

// ----------------------------------------------------------------------------------------
// nld_twoterm
// ----------------------------------------------------------------------------------------

NETLIB_UPDATE(twoterm)
{
	/* only called if connected to a rail net ==> notify the solver to recalculate */
	/* we only need to call the non-rail terminal */
	if (m_P.has_net() && !m_P.net().isRailNet())
		m_P.schedule_solve();
	else if (m_N.has_net() && !m_N.net().isRailNet())
		m_N.schedule_solve();
}

// ----------------------------------------------------------------------------------------
// nld_POT
// ----------------------------------------------------------------------------------------

NETLIB_UPDATE_PARAM(POT)
{
	nl_double v = m_Dial();
	if (m_DialIsLog())
		v = (std::exp(v) - 1.0) / (std::exp(1.0) - 1.0);

	m_R1.update_dev();
	m_R2.update_dev();

	m_R1.set_R(std::max(m_R() * v, netlist().gmin()));
	m_R2.set_R(std::max(m_R() * (NL_FCONST(1.0) - v), netlist().gmin()));

}

// ----------------------------------------------------------------------------------------
// nld_POT2
// ----------------------------------------------------------------------------------------

NETLIB_UPDATE_PARAM(POT2)
{
	nl_double v = m_Dial();

	if (m_DialIsLog())
		v = (std::exp(v) - 1.0) / (std::exp(1.0) - 1.0);
	if (m_Reverse())
		v = 1.0 - v;

	m_R1.update_dev();

	m_R1.set_R(std::max(m_R() * v, netlist().gmin()));
}

// ----------------------------------------------------------------------------------------
// nld_C
// ----------------------------------------------------------------------------------------

NETLIB_RESET(C)
{
	set(netlist().gmin(), 0.0, -5.0 / netlist().gmin());
	//set(1.0/NETLIST_GMIN, 0.0, -5.0 * NETLIST_GMIN);
}

NETLIB_UPDATE_PARAM(C)
{
	//step_time(1.0/48000.0);
	m_GParallel = netlist().gmin() * m_C();
}

NETLIB_UPDATE(C)
{
	NETLIB_NAME(twoterm)::update();
}

// ----------------------------------------------------------------------------------------
// nld_D
// ----------------------------------------------------------------------------------------

NETLIB_UPDATE_PARAM(D)
{
	nl_double Is = m_model.model_value("IS");
	nl_double n = m_model.model_value("N");

	m_D.set_param(Is, n, netlist().gmin());
}

NETLIB_UPDATE(D)
{
	NETLIB_NAME(twoterm)::update();
}

NETLIB_UPDATE_TERMINALS(D)
{
	m_D.update_diode(deltaV());
	set(m_D.G(), 0.0, m_D.Ieq());
}

// ----------------------------------------------------------------------------------------
// nld_VS
// ----------------------------------------------------------------------------------------

NETLIB_RESET(VS)
{
	NETLIB_NAME(twoterm)::reset();
	this->set(1.0 / m_R(), m_V(), 0.0);
}

NETLIB_UPDATE(VS)
{
	NETLIB_NAME(twoterm)::update();
}

// ----------------------------------------------------------------------------------------
// nld_CS
// ----------------------------------------------------------------------------------------

NETLIB_RESET(CS)
{
	NETLIB_NAME(twoterm)::reset();
	this->set(0.0, 0.0, m_I());
}

NETLIB_UPDATE(CS)
{
	NETLIB_NAME(twoterm)::update();
}

	} //namespace devices
} // namespace netlist
