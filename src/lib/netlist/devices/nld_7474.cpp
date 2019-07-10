
// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7474.c
 *
 */

#include "nld_7474.h"
#include "netlist/nl_base.h"
#include "nlid_system.h"

#include <array>

namespace netlist
{
	namespace devices
	{

	NETLIB_OBJECT(7474)
	{
		NETLIB_CONSTRUCTOR(7474)
		, m_D(*this, "D")
		, m_CLRQ(*this, "CLRQ")
		, m_PREQ(*this, "PREQ")
		, m_CLK(*this, "CLK", NETLIB_DELEGATE(7474, clk))
		, m_Q(*this, "Q")
		, m_QQ(*this, "QQ")
		, m_nextD(*this, "m_nextD", 0)
		, m_power_pins(*this)
		{
		}

	private:
		NETLIB_RESETI();
		NETLIB_UPDATEI();
		NETLIB_HANDLERI(clk);

		logic_input_t m_D;
		logic_input_t m_CLRQ;
		logic_input_t m_PREQ;
		logic_input_t m_CLK;
		logic_output_t m_Q;
		logic_output_t m_QQ;

		state_var<netlist_sig_t> m_nextD;

		nld_power_pins m_power_pins;

		void newstate(const netlist_sig_t stateQ, const netlist_sig_t stateQQ)
		{
			// 0: High-to-low 40 ns, 1: Low-to-high 25 ns
			static constexpr const std::array<netlist_time, 2> delay = { NLTIME_FROM_NS(40), NLTIME_FROM_NS(25) };
			m_Q.push(stateQ, delay[stateQ]);
			m_QQ.push(stateQQ, delay[stateQQ]);
		}
	};

	NETLIB_OBJECT(7474_dip)
	{
		NETLIB_CONSTRUCTOR(7474_dip)
		, m_A(*this, "A")
		, m_B(*this, "B")
		{
			register_subalias("1", "A.CLRQ");
			register_subalias("2", "A.D");
			register_subalias("3", "A.CLK");
			register_subalias("4", "A.PREQ");
			register_subalias("5", "A.Q");
			register_subalias("6", "A.QQ");
			register_subalias("7", "A.GND");

			register_subalias("8", "B.QQ");
			register_subalias("9", "B.Q");
			register_subalias("10", "B.PREQ");
			register_subalias("11", "B.CLK");
			register_subalias("12", "B.D");
			register_subalias("13", "B.CLRQ");
			register_subalias("14", "A.VCC");

			connect("A.GND", "B.GND");
			connect("A.VCC", "B.VCC");
		}
		NETLIB_UPDATEI();
		NETLIB_RESETI();

	private:
		NETLIB_SUB(7474) m_A;
		NETLIB_SUB(7474) m_B;
	};

	NETLIB_HANDLER(7474, clk)
	{
		newstate(m_nextD, !m_nextD);
		m_CLK.inactivate();
	}

	NETLIB_UPDATE(7474)
	{
		const auto preq(m_PREQ());
		const auto clrq(m_CLRQ());
		if (preq & clrq)
		{
			m_D.activate();
			m_nextD = m_D();
			m_CLK.activate_lh();
		}
		else
		{
			newstate(preq ^ 1, clrq ^ 1);
			m_CLK.inactivate();
			m_D.inactivate();
		}
	}

	NETLIB_RESET(7474)
	{
		m_CLK.set_state(logic_t::STATE_INP_LH);
		m_D.set_state(logic_t::STATE_INP_ACTIVE);
		m_nextD = 0;
	}

	NETLIB_RESET(7474_dip)
	{
	}

	NETLIB_UPDATE(7474_dip)
	{
	}

	NETLIB_DEVICE_IMPL(7474, "TTL_7474", "+CLK,+D,+CLRQ,+PREQ,@VCC,@GND")
	NETLIB_DEVICE_IMPL(7474_dip, "TTL_7474_DIP", "")

	} //namespace devices
} // namespace netlist
