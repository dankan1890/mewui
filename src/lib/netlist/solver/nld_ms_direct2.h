// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_ms_direct1.h
 *
 */

#ifndef NLD_MS_DIRECT2_H_
#define NLD_MS_DIRECT2_H_

#include "solver/nld_ms_direct.h"
#include "solver/nld_solver.h"

namespace netlist
{
	namespace devices
	{
class matrix_solver_direct2_t: public matrix_solver_direct_t<2,2>
{
public:

	matrix_solver_direct2_t(netlist_t &anetlist, const pstring &name, const solver_parameters_t *params)
		: matrix_solver_direct_t<2, 2>(anetlist, name, params, 2)
		{}
	virtual unsigned vsolve_non_dynamic(const bool newton_raphson) override;

};

// ----------------------------------------------------------------------------------------
// matrix_solver - Direct2
// ----------------------------------------------------------------------------------------

inline unsigned matrix_solver_direct2_t::vsolve_non_dynamic(ATTR_UNUSED const bool newton_raphson)
{
	build_LE_A<matrix_solver_direct2_t>();
	build_LE_RHS<matrix_solver_direct2_t>();

	const nl_double a = A(0,0);
	const nl_double b = A(0,1);
	const nl_double c = A(1,0);
	const nl_double d = A(1,1);

	nl_double new_val[2];
	new_val[1] = (a * RHS(1) - c * RHS(0)) / (a * d - b * c);
	new_val[0] = (RHS(0) - b * new_val[1]) / a;

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

#endif /* NLD_MS_DIRECT2_H_ */
