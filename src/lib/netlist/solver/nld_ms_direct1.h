// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_ms_direct1.h
 *
 */

#ifndef NLD_MS_DIRECT1_H_
#define NLD_MS_DIRECT1_H_

#include "solver/nld_ms_direct.h"
#include "solver/nld_solver.h"

namespace netlist
{
	namespace devices
	{
class matrix_solver_direct1_t: public matrix_solver_direct_t<1,1>
{
public:

	matrix_solver_direct1_t(netlist_t &anetlist, const pstring &name, const solver_parameters_t *params)
		: matrix_solver_direct_t<1, 1>(anetlist, name, params, 1)
		{}
	virtual unsigned vsolve_non_dynamic(const bool newton_raphson) override;

};

// ----------------------------------------------------------------------------------------
// matrix_solver - Direct1
// ----------------------------------------------------------------------------------------

inline unsigned matrix_solver_direct1_t::vsolve_non_dynamic(ATTR_UNUSED const bool newton_raphson)
{
	build_LE_A<matrix_solver_direct1_t>();
	build_LE_RHS<matrix_solver_direct1_t>();
	//NL_VERBOSE_OUT(("{1} {2}\n", new_val, m_RHS[0] / m_A[0][0]);

	nl_double new_val[1] = { RHS(0) / A(0,0) };

	if (has_dynamic_devices())
	{
		nl_double err = this->delta(new_val);
		store(new_val);
		if (err > m_params.m_accuracy )
			return 2;
		else
			return 1;
	}
	store(new_val);
	return 1;
}

	} //namespace devices
} // namespace netlist


#endif /* NLD_MS_DIRECT1_H_ */
