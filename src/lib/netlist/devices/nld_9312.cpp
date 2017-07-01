// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_9312.c
 *
 */

/*
 *          +---+---+---+---++---+---+
 *          | C | B | A | G || Y | YQ|
 *          +===+===+===+===++===+===+
 *          | X | X | X | 1 ||  0| 1 |
 *          | 0 | 0 | 0 | 0 || D0|D0Q|
 *          | 0 | 0 | 1 | 0 || D1|D1Q|
 *          | 0 | 1 | 0 | 0 || D2|D2Q|
 *          | 0 | 1 | 1 | 0 || D3|D3Q|
 *          | 1 | 0 | 0 | 0 || D4|D4Q|
 *          | 1 | 0 | 1 | 0 || D5|D5Q|
 *          | 1 | 1 | 0 | 0 || D6|D6Q|
 *          | 1 | 1 | 1 | 0 || D7|D7Q|
 *          +---+---+---+---++---+---+
*/
#include "nld_9312.h"
#include "nld_truthtable.h"

namespace netlist
{
	namespace devices
	{
	#if (USE_TRUTHTABLE)
	/* The truthtable implementation is a lot faster than
	 * the carefully crafted code :-(
	 */
	NETLIB_TRUTHTABLE(9312, 12, 2);
	#else

	NETLIB_OBJECT(9312)
	{
		NETLIB_CONSTRUCTOR(9312)
		, m_A(*this, "A")
		, m_B(*this, "B")
		, m_C(*this, "C")
		, m_G(*this, "G")
		, m_D(*this, {{"D0","D1","D2","D3","D4","D5","D6","D7"}})
		, m_Y(*this, "Y")
		, m_YQ(*this, "YQ")
		, m_last_chan(*this, "m_last_chan", 0)
		, m_last_G(*this, "m_last_G", 0)
		{
		}

	NETLIB_UPDATEI();
	NETLIB_RESETI();
	public:
		logic_input_t m_A;
		logic_input_t m_B;
		logic_input_t m_C;
		logic_input_t m_G;
		object_array_t<logic_input_t, 8> m_D;
		logic_output_t m_Y;
		logic_output_t m_YQ;

		state_var_u8 m_last_chan;
		state_var_u8 m_last_G;
	};

	#endif

	NETLIB_OBJECT(9312_dip)
	{
		NETLIB_CONSTRUCTOR(9312_dip)
		, m_sub(*this, "1")
		{
		#if (1 && USE_TRUTHTABLE)

			register_subalias("13", m_sub.m_I[0]);
			register_subalias("12", m_sub.m_I[1]);
			register_subalias("11", m_sub.m_I[2]);
			register_subalias("10", m_sub.m_I[3]);

			register_subalias("1", m_sub.m_I[4]);
			register_subalias("2", m_sub.m_I[5]);
			register_subalias("3", m_sub.m_I[6]);
			register_subalias("4", m_sub.m_I[7]);
			register_subalias("5", m_sub.m_I[8]);
			register_subalias("6", m_sub.m_I[9]);
			register_subalias("7", m_sub.m_I[10]);
			register_subalias("9", m_sub.m_I[11]);

			register_subalias("15", m_sub.m_Q[0]); // Y
			register_subalias("14", m_sub.m_Q[1]); // YQ

		#else

			register_subalias("13", m_sub.m_C);
			register_subalias("12", m_sub.m_B);
			register_subalias("11", m_sub.m_A);
			register_subalias("10", m_sub.m_G);

			register_subalias("1", m_sub.m_D[0]);
			register_subalias("2", m_sub.m_D[1]);
			register_subalias("3", m_sub.m_D[2]);
			register_subalias("4", m_sub.m_D[3]);
			register_subalias("5", m_sub.m_D[4]);
			register_subalias("6", m_sub.m_D[5]);
			register_subalias("7", m_sub.m_D[6]);
			register_subalias("9", m_sub.m_D[7]);

			register_subalias("15", m_sub.m_Y); // Y
			register_subalias("14", m_sub.m_YQ); // YQ

		#endif

		}

		//NETLIB_RESETI();
		//NETLIB_UPDATEI();

	protected:
		NETLIB_SUB(9312) m_sub;
	};

	#if (USE_TRUTHTABLE)
	nld_9312::truthtable_t nld_9312::m_ttbl;

	/* FIXME: Data changes are propagating faster than changing selects A,B,C
	 *        Please refer to data sheet.
	 *        This would require a state machine, thus we do not
	 *        do this right now.
	 */

	const char *nld_9312::m_desc[] = {
			" C, B, A, G,D0,D1,D2,D3,D4,D5,D6,D7| Y,YQ",
			" X, X, X, 1, X, X, X, X, X, X, X, X| 0, 1|33,19",
			" 0, 0, 0, 0, 0, X, X, X, X, X, X, X| 0, 1|33,28",
			" 0, 0, 0, 0, 1, X, X, X, X, X, X, X| 1, 0|33,28",
			" 0, 0, 1, 0, X, 0, X, X, X, X, X, X| 0, 1|33,28",
			" 0, 0, 1, 0, X, 1, X, X, X, X, X, X| 1, 0|33,28",
			" 0, 1, 0, 0, X, X, 0, X, X, X, X, X| 0, 1|33,28",
			" 0, 1, 0, 0, X, X, 1, X, X, X, X, X| 1, 0|33,28",
			" 0, 1, 1, 0, X, X, X, 0, X, X, X, X| 0, 1|33,28",
			" 0, 1, 1, 0, X, X, X, 1, X, X, X, X| 1, 0|33,28",
			" 1, 0, 0, 0, X, X, X, X, 0, X, X, X| 0, 1|33,28",
			" 1, 0, 0, 0, X, X, X, X, 1, X, X, X| 1, 0|33,28",
			" 1, 0, 1, 0, X, X, X, X, X, 0, X, X| 0, 1|33,28",
			" 1, 0, 1, 0, X, X, X, X, X, 1, X, X| 1, 0|33,28",
			" 1, 1, 0, 0, X, X, X, X, X, X, 0, X| 0, 1|33,28",
			" 1, 1, 0, 0, X, X, X, X, X, X, 1, X| 1, 0|33,28",
			" 1, 1, 1, 0, X, X, X, X, X, X, X, 0| 0, 1|33,28",
			" 1, 1, 1, 0, X, X, X, X, X, X, X, 1| 1, 0|33,28",
			""
	};
	#else

	NETLIB_UPDATE(9312)
	{
		const NLUINT8 G = INPLOGIC(m_G);
		if (G)
		{
			const netlist_time delay[2] = { NLTIME_FROM_NS(33), NLTIME_FROM_NS(19) };
			OUTLOGIC(m_Y, 0, delay[0]);
			OUTLOGIC(m_YQ, 1, delay[1]);

			m_A.inactivate();
			m_B.inactivate();
			m_C.inactivate();
			m_last_G = G;
		}
		else
		{
			if (m_last_G)
			{
				m_last_G = G;
				m_A.activate();
				m_B.activate();
				m_C.activate();
			}
			constexpr netlist_time delay[2] = { NLTIME_FROM_NS(33), NLTIME_FROM_NS(28) };
			const NLUINT8 chan = INPLOGIC(m_A) | (INPLOGIC(m_B)<<1) | (INPLOGIC(m_C)<<2);
			if (m_last_chan != chan)
			{
				m_D[m_last_chan].inactivate();
				m_D[chan].activate();
			}
			const auto val = INPLOGIC(m_D[chan]);
			OUTLOGIC(m_Y, val, delay[val]);
			OUTLOGIC(m_YQ, !val, delay[!val]);
			m_last_chan = chan;
		}
	}

	NETLIB_RESET(9312)
	{
	}
	#endif

	NETLIB_DEVICE_IMPL(9312)
	NETLIB_DEVICE_IMPL(9312_dip)

	} //namespace devices
} // namespace netlist
