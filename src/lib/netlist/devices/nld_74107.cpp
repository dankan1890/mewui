// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_74107.c
 *
 */

#include "nld_74107.h"

namespace netlist
{
	namespace devices
	{
	NETLIB_OBJECT(74107Asub)
	{
		NETLIB_CONSTRUCTOR(74107Asub)
		, m_clk(*this, "CLK")
		, m_Q(*this, "Q")
		, m_QQ(*this, "QQ")
		, m_Q1(*this, "m_Q1", 0)
		, m_Q2(*this, "m_Q2", 0)
		, m_F(*this, "m_F", 0)
		{
		}

		NETLIB_RESETI();
		NETLIB_UPDATEI();

	public:
		logic_input_t m_clk;

		logic_output_t m_Q;
		logic_output_t m_QQ;

		state_var<netlist_sig_t> m_Q1;
		state_var<netlist_sig_t> m_Q2;
		state_var<netlist_sig_t> m_F;

		void newstate(const netlist_sig_t state);

	};

	NETLIB_OBJECT(74107A)
	{
		NETLIB_CONSTRUCTOR(74107A)
		, m_sub(*this, "sub")
		, m_J(*this, "J")
		, m_K(*this, "K")
		, m_clrQ(*this, "CLRQ")
		{
			register_subalias("CLK", m_sub.m_clk);
			register_subalias("Q", m_sub.m_Q);
			register_subalias("QQ", m_sub.m_QQ);
		}

		//NETLIB_RESETI();
		NETLIB_UPDATEI();
	public:
		NETLIB_SUB(74107Asub) m_sub;

		logic_input_t m_J;
		logic_input_t m_K;
		logic_input_t m_clrQ;

	};

	NETLIB_OBJECT_DERIVED(74107, 74107A)
	{
	public:
		NETLIB_CONSTRUCTOR_DERIVED(74107, 74107A) { }

	};

	NETLIB_OBJECT(74107_dip)
	{
		NETLIB_CONSTRUCTOR(74107_dip)
		, m_1(*this, "1")
		, m_2(*this, "2")
		{
			register_subalias("1", m_1.m_J);
			register_subalias("2", m_1.m_sub.m_QQ);
			register_subalias("3", m_1.m_sub.m_Q);

			register_subalias("4", m_1.m_K);
			register_subalias("5", m_2.m_sub.m_Q);
			register_subalias("6", m_2.m_sub.m_QQ);

			// register_subalias("7", ); ==> GND

			register_subalias("8", m_2.m_J);
			register_subalias("9", m_2.m_sub.m_clk);
			register_subalias("10", m_2.m_clrQ);

			register_subalias("11", m_2.m_K);
			register_subalias("12", m_1.m_sub.m_clk);
			register_subalias("13", m_1.m_clrQ);

			// register_subalias("14", ); ==> VCC

		}
		//NETLIB_RESETI();
		//NETLIB_UPDATEI();

	private:
		NETLIB_SUB(74107) m_1;
		NETLIB_SUB(74107) m_2;
	};

	NETLIB_RESET(74107Asub)
	{
		m_clk.set_state(logic_t::STATE_INP_HL);
		//m_Q.initial(0);
		//m_QQ.initial(1);

		m_Q1 = 0;
		m_Q2 = 0;
		m_F = 0;
	}

	inline void NETLIB_NAME(74107Asub)::newstate(const netlist_sig_t state)
	{
		const netlist_time delay[2] = { NLTIME_FROM_NS(25), NLTIME_FROM_NS(40) };

		m_Q.push(state, delay[state]);
		m_QQ.push(state ^ 1, delay[state ^ 1]);
	}

	NETLIB_UPDATE(74107Asub)
	{
		const netlist_sig_t t = m_Q.net().Q();
		newstate(((t ^ 1) & m_Q1) | (t & m_Q2) | m_F);
		if (m_Q1 ^ 1)
			m_clk.inactivate();
	}

	NETLIB_UPDATE(74107A)
	{
		const auto JK = (m_J() << 1) | m_K();

		switch (JK)
		{
			case 0:
				m_sub.m_Q1 = 0;
				m_sub.m_Q2 = 1;
				m_sub.m_F  = 0;
				m_sub.m_clk.inactivate();
				break;
			case 1:             // (!m_J) & m_K))
				m_sub.m_Q1 = 0;
				m_sub.m_Q2 = 0;
				m_sub.m_F  = 0;
				break;
			case 2:             // (m_J) & !m_K))
				m_sub.m_Q1 = 0;
				m_sub.m_Q2 = 0;
				m_sub.m_F  = 1;
				break;
			case 3:             // (m_J) & m_K))
				m_sub.m_Q1 = 1;
				m_sub.m_Q2 = 0;
				m_sub.m_F  = 0;
				break;
			default:
				break;
		}

		if (!m_clrQ())
		{
			m_sub.m_clk.inactivate();
			m_sub.newstate(0);
		}
		else if (!m_sub.m_Q2)
			m_sub.m_clk.activate_hl();
	}

	NETLIB_DEVICE_IMPL(74107)
	NETLIB_DEVICE_IMPL(74107A)
	NETLIB_DEVICE_IMPL(74107_dip)

	} //namespace devices
} // namespace netlist
