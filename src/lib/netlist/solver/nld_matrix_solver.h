// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_matrix_solver.h
 *
 */

#ifndef NLD_MATRIX_SOLVER_H_
#define NLD_MATRIX_SOLVER_H_

#include <type_traits>

//#include "solver/nld_solver.h"
#include "nl_base.h"
#include "plib/pstream.h"

namespace netlist
{
	namespace devices
	{
	/* FIXME: these should become proper devices */

	struct solver_parameters_t
	{
		int m_pivot;
		nl_double m_accuracy;
		nl_double m_lte;
		nl_double m_min_timestep;
		nl_double m_max_timestep;
		nl_double m_sor;
		bool m_dynamic;
		unsigned m_gs_loops;
		unsigned m_nr_loops;
		netlist_time m_nt_sync_delay;
		bool m_log_stats;
	};


class terms_t
{
	P_PREVENT_COPYING(terms_t)

public:
	terms_t()
	: m_railstart(0)
	, m_last_V(0.0)
	, m_DD_n_m_1(0.0)
	, m_h_n_m_1(1e-6)
	{}

	void clear()
	{
		m_term.clear();
		m_net_other.clear();
		m_gt.clear();
		m_go.clear();
		m_Idr.clear();
		m_other_curanalog.clear();
	}

	void add(terminal_t *term, int net_other, bool sorted);

	inline std::size_t count() { return m_term.size(); }

	inline terminal_t **terms() { return m_term.data(); }
	inline int *net_other() { return m_net_other.data(); }
	inline nl_double *gt() { return m_gt.data(); }
	inline nl_double *go() { return m_go.data(); }
	inline nl_double *Idr() { return m_Idr.data(); }
	inline nl_double **other_curanalog() { return m_other_curanalog.data(); }

	void set_pointers();

	std::size_t m_railstart;

	std::vector<unsigned> m_nz;   /* all non zero for multiplication */
	std::vector<unsigned> m_nzrd; /* non zero right of the diagonal for elimination, may include RHS element */
	std::vector<unsigned> m_nzbd; /* non zero below of the diagonal for elimination */

	/* state */
	nl_double m_last_V;
	nl_double m_DD_n_m_1;
	nl_double m_h_n_m_1;

private:
	std::vector<int> m_net_other;
	std::vector<nl_double> m_go;
	std::vector<nl_double> m_gt;
	std::vector<nl_double> m_Idr;
	std::vector<nl_double *> m_other_curanalog;
	std::vector<terminal_t *> m_term;

};

class proxied_analog_output_t : public analog_output_t
{
public:

	proxied_analog_output_t(core_device_t &dev, const pstring &aname)
	: analog_output_t(dev, aname)
	, m_proxied_net(nullptr)
	{ }

	analog_net_t *m_proxied_net; // only for proxy nets in analog input logic
};


class matrix_solver_t : public device_t
{
public:
	using list_t = std::vector<matrix_solver_t *>;

	enum eSortType
	{
		NOSORT,
		ASCENDING,
		DESCENDING
	};

	matrix_solver_t(netlist_t &anetlist, const pstring &name,
			const eSortType sort, const solver_parameters_t *params)
	: device_t(anetlist, name)
	, m_params(*params)
	, m_stat_calculations(*this, "m_stat_calculations", 0)
	, m_stat_newton_raphson(*this, "m_stat_newton_raphson", 0)
	, m_stat_vsolver_calls(*this, "m_stat_vsolver_calls", 0)
	, m_iterative_fail(*this, "m_iterative_fail", 0)
	, m_iterative_total(*this, "m_iterative_total", 0)
	, m_last_step(*this, "m_last_step", netlist_time::zero())
	, m_fb_sync(*this, "FB_sync")
	, m_Q_sync(*this, "Q_sync")
	, m_sort(sort)
	{
		connect_post_start(m_fb_sync, m_Q_sync);
	}

	virtual ~matrix_solver_t();

	void setup(analog_net_t::list_t &nets) { vsetup(nets); }

	void solve_base();

	const netlist_time solve();

	inline bool has_dynamic_devices() const { return m_dynamic_devices.size() > 0; }
	inline bool has_timestep_devices() const { return m_step_devices.size() > 0; }

	void update_forced();
	void update_after(const netlist_time &after)
	{
		m_Q_sync.net().force_queue_execution();
		m_Q_sync.net().reschedule_in_queue(after);
	}

	/* netdevice functions */
	NETLIB_UPDATEI();
	NETLIB_RESETI();

public:
	int get_net_idx(detail::net_t *net);

	plib::plog_base<NL_DEBUG> &log() { return netlist().log(); }

	virtual void log_stats();

	virtual void create_solver_code(plib::postream &strm)
	{
		strm.writeline(plib::pfmt("/* {1} doesn't support static compile */"));
	}

protected:

	void setup_base(analog_net_t::list_t &nets);
	void update_dynamic();

	virtual void vsetup(analog_net_t::list_t &nets) = 0;
	virtual unsigned vsolve_non_dynamic(const bool newton_raphson) = 0;

	netlist_time compute_next_timestep(const double cur_ts);
	/* virtual */ void  add_term(std::size_t net_idx, terminal_t *term);

	template <typename T>
	void store(const T * RESTRICT V);
	template <typename T>
	T delta(const T * RESTRICT V);

	template <typename T>
	void build_LE_A();
	template <typename T>
	void build_LE_RHS();

	std::vector<terms_t *> m_terms;
	std::vector<analog_net_t *> m_nets;
	std::vector<std::unique_ptr<proxied_analog_output_t>> m_inps;

	std::vector<terms_t *> m_rails_temp;

	const solver_parameters_t &m_params;

	state_var<int> m_stat_calculations;
	state_var<int> m_stat_newton_raphson;
	state_var<int> m_stat_vsolver_calls;
	state_var<int> m_iterative_fail;
	state_var<int> m_iterative_total;

private:

	state_var<netlist_time> m_last_step;
	std::vector<core_device_t *> m_step_devices;
	std::vector<core_device_t *> m_dynamic_devices;

	logic_input_t m_fb_sync;
	logic_output_t m_Q_sync;

	/* calculate matrix */
	void setup_matrix();

	void step(const netlist_time &delta);

	void update_inputs();

	const eSortType m_sort;
};

template <typename T>
T matrix_solver_t::delta(const T * RESTRICT V)
{
	/* FIXME: Ideally we should also include currents (RHS) here. This would
	 * need a reevaluation of the right hand side after voltages have been updated
	 * and thus belong into a different calculation. This applies to all solvers.
	 */

	std::size_t iN = this->m_terms.size();
	T cerr = 0;
	for (unsigned i = 0; i < iN; i++)
		cerr = std::max(cerr, std::abs(V[i] - static_cast<T>(this->m_nets[i]->m_cur_Analog)));
	return cerr;
}

template <typename T>
void matrix_solver_t::store(const T * RESTRICT V)
{
	for (std::size_t i = 0, iN=m_terms.size(); i < iN; i++)
		this->m_nets[i]->m_cur_Analog = V[i];
}

template <typename T>
void matrix_solver_t::build_LE_A()
{
	static_assert(std::is_base_of<matrix_solver_t, T>::value, "T must derive from matrix_solver_t");

	T &child = static_cast<T &>(*this);

	const unsigned iN = child.N();
	for (unsigned k = 0; k < iN; k++)
	{
		for (unsigned i=0; i < iN; i++)
			child.A(k,i) = 0.0;

		const std::size_t terms_count = m_terms[k]->count();
		const std::size_t railstart =  m_terms[k]->m_railstart;
		const nl_double * RESTRICT gt = m_terms[k]->gt();

		{
			nl_double akk  = 0.0;
			for (unsigned i = 0; i < terms_count; i++)
				akk += gt[i];

			child.A(k,k) = akk;
		}

		const nl_double * RESTRICT go = m_terms[k]->go();
		const int * RESTRICT net_other = m_terms[k]->net_other();

		for (std::size_t i = 0; i < railstart; i++)
			child.A(k,net_other[i]) -= go[i];
	}
}

template <typename T>
void matrix_solver_t::build_LE_RHS()
{
	static_assert(std::is_base_of<matrix_solver_t, T>::value, "T must derive from matrix_solver_t");
	T &child = static_cast<T &>(*this);

	const unsigned iN = child.N();
	for (unsigned k = 0; k < iN; k++)
	{
		nl_double rhsk_a = 0.0;
		nl_double rhsk_b = 0.0;

		const std::size_t terms_count = m_terms[k]->count();
		const nl_double * RESTRICT go = m_terms[k]->go();
		const nl_double * RESTRICT Idr = m_terms[k]->Idr();
		const nl_double * const * RESTRICT other_cur_analog = m_terms[k]->other_curanalog();

		for (std::size_t i = 0; i < terms_count; i++)
			rhsk_a = rhsk_a + Idr[i];

		for (std::size_t i = m_terms[k]->m_railstart; i < terms_count; i++)
			//rhsk = rhsk + go[i] * terms[i]->m_otherterm->net().as_analog().Q_Analog();
			rhsk_b = rhsk_b + go[i] * *other_cur_analog[i];

		child.RHS(k) = rhsk_a + rhsk_b;
	}
}

	} //namespace devices
} // namespace netlist

#endif /* NLD_MS_DIRECT_H_ */
