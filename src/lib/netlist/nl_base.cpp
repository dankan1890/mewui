// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nlbase.c
 *
 */

#include "solver/nld_matrix_solver.h"
#include "solver/nld_solver.h"

#include "plib/palloc.h"
#include "plib/pmempool.h"
#include "plib/putil.h"

#include "devices/nlid_proxy.h"
#include "devices/nlid_system.h"
#include "macro/nlm_base.h"
#include "nl_base.h"

#include "nl_errstr.h"

#include <cmath>
#include <cstring>
#include <limits>

namespace netlist
{

// ----------------------------------------------------------------------------------------
// logic_family_ttl_t
// ----------------------------------------------------------------------------------------

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, modernize-use-equals-default)
logic_family_desc_t::logic_family_desc_t()
{
}

class logic_family_ttl_t : public logic_family_desc_t
{
public:
	logic_family_ttl_t() : logic_family_desc_t()
	{
		m_fixed_V = 5.0;
		m_low_thresh_PCNT = 0.8 / 5.0;
		m_high_thresh_PCNT = 2.0 / 5.0;
		// m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
		m_low_VO = 0.1;
		m_high_VO = 1.0; // 4.0
		m_R_low = 1.0;
		m_R_high = 130.0;
	}
	pool_owned_ptr<devices::nld_base_d_to_a_proxy> create_d_a_proxy(netlist_state_t &anetlist, const pstring &name, logic_output_t *proxied) const override;
	pool_owned_ptr<devices::nld_base_a_to_d_proxy> create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, logic_input_t *proxied) const override;
};

pool_owned_ptr<devices::nld_base_d_to_a_proxy> logic_family_ttl_t::create_d_a_proxy(netlist_state_t &anetlist, const pstring &name, logic_output_t *proxied) const
{
	return pool().make_poolptr<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
}
pool_owned_ptr<devices::nld_base_a_to_d_proxy> logic_family_ttl_t::create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, logic_input_t *proxied) const
{
	return pool().make_poolptr<devices::nld_a_to_d_proxy>(anetlist, name, proxied);
}

class logic_family_cd4xxx_t : public logic_family_desc_t
{
public:
	logic_family_cd4xxx_t() : logic_family_desc_t()
	{
		m_fixed_V = 0.0;
		m_low_thresh_PCNT = 1.5 / 5.0;
		m_high_thresh_PCNT = 3.5 / 5.0;
		// m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
		m_low_VO = 0.05;
		m_high_VO = 0.05; // 4.95
		m_R_low = 10.0;
		m_R_high = 10.0;
	}
	pool_owned_ptr<devices::nld_base_d_to_a_proxy> create_d_a_proxy(netlist_state_t &anetlist, const pstring &name, logic_output_t *proxied) const override;
	pool_owned_ptr<devices::nld_base_a_to_d_proxy> create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, logic_input_t *proxied) const override;
};

pool_owned_ptr<devices::nld_base_d_to_a_proxy> logic_family_cd4xxx_t::create_d_a_proxy(netlist_state_t &anetlist, const pstring &name, logic_output_t *proxied) const
{
	return pool().make_poolptr<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
}

pool_owned_ptr<devices::nld_base_a_to_d_proxy> logic_family_cd4xxx_t::create_a_d_proxy(netlist_state_t &anetlist, const pstring &name, logic_input_t *proxied) const
{
	return pool().make_poolptr<devices::nld_a_to_d_proxy>(anetlist, name, proxied);
}

const logic_family_desc_t *family_TTL()
{
	static logic_family_ttl_t obj;
	return &obj;
}
const logic_family_desc_t *family_CD4XXX()
{
	static logic_family_cd4xxx_t obj;
	return &obj;
}


// ----------------------------------------------------------------------------------------
// queue_t
// ----------------------------------------------------------------------------------------

detail::queue_t::queue_t(netlist_state_t &nl)
	: timed_queue<pqentry_t<net_t *, netlist_time>, false, true>(512)
	, netlist_ref(nl)
	, m_qsize(0)
	, m_times(512)
	, m_net_ids(512)
{
}

void detail::queue_t::register_state(plib::state_manager_t &manager, const pstring &module)
{
	//state().log().debug("register_state\n");
	manager.save_item(this, m_qsize, module + "." + "qsize");
	manager.save_item(this, &m_times[0], module + "." + "times", m_times.size());
	manager.save_item(this, &m_net_ids[0], module + "." + "names", m_net_ids.size());
}

void detail::queue_t::on_pre_save(plib::state_manager_t &manager)
{
	plib::unused_var(manager);
	m_qsize = this->size();
	for (std::size_t i = 0; i < m_qsize; i++ )
	{
		m_times[i] =  this->listptr()[i].m_exec_time.as_raw();
		m_net_ids[i] = state().find_net_id(this->listptr()[i].m_object);
	}
}


void detail::queue_t::on_post_load(plib::state_manager_t &manager)
{
	plib::unused_var(manager);
	this->clear();
	for (std::size_t i = 0; i < m_qsize; i++ )
	{
		detail::net_t *n = state().nets()[m_net_ids[i]].get();
		this->push(queue_t::entry_t(netlist_time::from_raw(m_times[i]),n));
	}
}

// ----------------------------------------------------------------------------------------
// netlist_ref_t
// ----------------------------------------------------------------------------------------

detail::netlist_ref::netlist_ref(netlist_state_t &nl)
: m_netlist(nl.exec()) { }

// ----------------------------------------------------------------------------------------
// device_object_t
// ----------------------------------------------------------------------------------------

detail::device_object_t::device_object_t(core_device_t &dev, const pstring &aname)
: object_t(aname)
, m_device(dev)
{
}

detail::terminal_type detail::core_terminal_t::type() const
{
	if (dynamic_cast<const terminal_t *>(this) != nullptr)
		return terminal_type::TERMINAL;
	else if (dynamic_cast<const logic_input_t *>(this) != nullptr)
		return terminal_type::INPUT;
	else if (dynamic_cast<const logic_output_t *>(this) != nullptr)
		return terminal_type::OUTPUT;
	else if (dynamic_cast<const analog_input_t *>(this) != nullptr)
		return terminal_type::INPUT;
	else if (dynamic_cast<const analog_output_t *>(this) != nullptr)
		return terminal_type::OUTPUT;
	else
	{
		state().log().fatal(MF_UNKNOWN_TYPE_FOR_OBJECT(name()));
		return terminal_type::TERMINAL; // please compiler
	}
}

// ----------------------------------------------------------------------------------------
// netlist_t
// ----------------------------------------------------------------------------------------

netlist_t::netlist_t(const pstring &aname, plib::unique_ptr<callbacks_t> callbacks)
	: m_state(plib::make_unique<netlist_state_t>(aname, *this, std::move(callbacks)))
	, m_solver(nullptr)
	, m_time(netlist_time::zero())
	, m_mainclock(nullptr)
	, m_queue(*m_state)
	, m_stats(false)
{
	devices::initialize_factory(nlstate().setup().factory());
	NETLIST_NAME(base)(nlstate().setup());
	run_state_manager().save_item(this, static_cast<plib::state_manager_t::callback_t &>(m_queue), "m_queue");
	run_state_manager().save_item(this, m_time, "m_time");
}

// ----------------------------------------------------------------------------------------
// netlist_t
// ----------------------------------------------------------------------------------------

netlist_state_t::netlist_state_t(const pstring &aname,
	netlist_t & anetlist,
	plib::unique_ptr<callbacks_t> &&callbacks)
: m_name(aname)
, m_netlist(anetlist)
, m_state()
, m_callbacks(std::move(callbacks)) // Order is important here
, m_log(*m_callbacks)
, m_setup(plib::make_unique<setup_t>(*this))
{
	pstring libpath = plib::util::environment("NL_BOOSTLIB", plib::util::buildpath({".", "nlboost.so"}));
	m_lib = plib::make_unique<plib::dynlib>(libpath);
}


void netlist_t::stop()
{
	log().debug("Printing statistics ...\n");
	print_stats();
	log().debug("Stopping solver device ...\n");
	if (m_solver != nullptr)
		m_solver->stop();
}

detail::net_t *netlist_state_t::find_net(const pstring &name) const
{
	for (auto & net : m_nets)
		if (net->name() == name)
			return net.get();

	return nullptr;
}

std::size_t netlist_state_t::find_net_id(const detail::net_t *net) const
{
	for (std::size_t i = 0; i < m_nets.size(); i++)
		if (m_nets[i].get() == net)
			return i;
	return std::numeric_limits<std::size_t>::max();
}



void netlist_state_t::rebuild_lists()
{
	for (auto & net : m_nets)
		net->rebuild_list();
}


void netlist_state_t::compile_defines(std::vector<std::pair<pstring, pstring>> &defs)
{
#define ENTRY(x) { #x, PSTRINGIFY(x) }
	defs.push_back(ENTRY(PHAS_RDTSCP));
	defs.push_back(ENTRY(PUSE_ACCURATE_STATS));
	defs.push_back(ENTRY(PHAS_INT128));
	defs.push_back(ENTRY(USE_ALIGNED_OPTIMIZATIONS));
	defs.push_back(ENTRY(NVCCBUILD));
	defs.push_back(ENTRY(USE_MEMPOOL));
	defs.push_back(ENTRY(USE_QUEUE_STATS));
	defs.push_back(ENTRY(USE_COPY_INSTEAD_OF_REFERENCE));
	defs.push_back(ENTRY(USE_TRUTHTABLE_7448));
	defs.push_back(ENTRY(NL_DEBUG));
	defs.push_back(ENTRY(HAS_OPENMP));
	defs.push_back(ENTRY(USE_OPENMP));

	defs.push_back(ENTRY(PPMF_TYPE));
	defs.push_back(ENTRY(PHAS_PMF_INTERNAL));

#undef ENTRY
}

void netlist_t::reset()
{
	log().debug("Searching for mainclock\n");
	m_mainclock = m_state->get_single_device<devices::NETLIB_NAME(mainclock)>("mainclock");

	log().debug("Searching for solver\n");
	m_solver = m_state->get_single_device<devices::NETLIB_NAME(solver)>("solver");

	m_time = netlist_time::zero();
	m_queue.clear();
	if (m_mainclock != nullptr)
		m_mainclock->m_Q.net().set_next_scheduled_time(netlist_time::zero());
	//if (m_solver != nullptr)
	//  m_solver->reset();

	m_state->reset();
}

void netlist_state_t::reset()
{
	//FIXME: never used ???
	std::unordered_map<core_device_t *, bool> m;

	// Reset all nets once !
	log().verbose("Call reset on all nets:");
	for (auto & n : nets())
		n->reset();

	// Reset all devices once !
	log().verbose("Call reset on all devices:");
	for (auto & dev : m_devices)
		dev.second->reset();

	// Make sure everything depending on parameters is set
	// Currently analog input and logic input also
	// push their outputs to queue.

	log().verbose("Call update_param on all devices:");
	for (auto & dev : m_devices)
		dev.second->update_param();

	// Step all devices once !
	/*
	 * INFO: The order here affects power up of e.g. breakout. However, such
	 * variations are explicitly stated in the breakout manual.
	 */

	auto *netlist_params = get_single_device<devices::NETLIB_NAME(netlistparams)>("parameter");

	switch (netlist_params->m_startup_strategy())
	{
		case 0:
		{
			std::vector<core_device_t *> d;
			std::vector<nldelegate *> t;
			log().verbose("Using default startup strategy");
			for (auto &n : m_nets)
				for (auto & term : n->core_terms())
					if (term->m_delegate.has_object())
					{
						if (!plib::container::contains(t, &term->m_delegate))
						{
							t.push_back(&term->m_delegate);
							term->m_delegate();
						}
						auto *dev = reinterpret_cast<core_device_t *>(term->m_delegate.object());
						if (!plib::container::contains(d, dev))
							d.push_back(dev);
					}
			log().verbose("Devices not yet updated:");
			for (auto &dev : m_devices)
				if (!plib::container::contains(d, dev.second.get()))
				{
					log().verbose("\t ...{1}", dev.second->name());
					dev.second->update();
				}
		}
		break;
		case 1:     // brute force backward
		{
			log().verbose("Using brute force backward startup strategy");

			for (auto &n : m_nets)  // only used if USE_COPY_INSTEAD_OF_REFERENCE == 1
				n->update_inputs();

			std::size_t i = m_devices.size();
			while (i>0)
				m_devices[--i].second->update();

			for (auto &n : m_nets)  // only used if USE_COPY_INSTEAD_OF_REFERENCE == 1
				n->update_inputs();

		}
		break;
		case 2:     // brute force forward
		{
			log().verbose("Using brute force forward startup strategy");
			for (auto &d : m_devices)
				d.second->update();
		}
		break;
	}

#if 1
	/* the above may screw up m_active and the list */
	rebuild_lists();
#endif
}

void netlist_t::process_queue(const netlist_time delta) NL_NOEXCEPT
{
	if (!m_stats)
		process_queue_stats<false>(delta);
	else
	{
		auto sm_guard(m_stat_mainloop.guard());
		process_queue_stats<true>(delta);
	}
}

template <bool KEEP_STATS>
void netlist_t::process_queue_stats(const netlist_time delta) NL_NOEXCEPT
{
	netlist_time stop(m_time + delta);

	qpush(detail::queue_t::entry_t(stop, nullptr));

	if (m_mainclock == nullptr)
	{
		detail::queue_t::entry_t e(m_queue.pop());
		m_time = e.m_exec_time;
		while (e.m_object != nullptr)
		{
			e.m_object->template update_devs<KEEP_STATS>();
			if (KEEP_STATS) m_perf_out_processed.inc();
			e = m_queue.pop();
			m_time = e.m_exec_time;
		}
	}
	else
	{
		logic_net_t &mc_net(m_mainclock->m_Q.net());
		const netlist_time inc(m_mainclock->m_inc);
		netlist_time mc_time(mc_net.next_scheduled_time());

		do
		{
			while (m_queue.top().m_exec_time > mc_time)
			{
				m_time = mc_time;
				mc_net.toggle_new_Q();
				mc_net.update_devs<KEEP_STATS>();
				mc_time += inc;
			}

			detail::queue_t::entry_t e(m_queue.pop());
			m_time = e.m_exec_time;
			if (e.m_object != nullptr)
			{
				e.m_object->template update_devs<KEEP_STATS>();
				if (KEEP_STATS) m_perf_out_processed.inc();
			}
			else
				break;
		} while (true); //while (e.m_object != nullptr);
		mc_net.set_next_scheduled_time(mc_time);
	}
}

void netlist_t::print_stats() const
{
	if (m_stats)
	{
		std::vector<size_t> index;
		for (size_t i=0; i < m_state->m_devices.size(); i++)
			index.push_back(i);

		std::sort(index.begin(), index.end(),
				[&](size_t i1, size_t i2) { return m_state->m_devices[i1].second->m_stats->m_stat_total_time.total() < m_state->m_devices[i2].second->m_stats->m_stat_total_time.total(); });

		nperftime_t<true>::type total_time(0);
		nperftime_t<true>::ctype total_count(0);

		for (auto & j : index)
		{
			auto entry = m_state->m_devices[j].second.get();
			auto stats = m_state->m_devices[j].second.get()->m_stats.get();
			log().verbose("Device {1:20} : {2:12} {3:12} {4:15} {5:12}", entry->name(),
					stats->m_stat_call_count(), stats->m_stat_total_time.count(),
					stats->m_stat_total_time.total(), stats->m_stat_inc_active());
			total_time += stats->m_stat_total_time.total();
			total_count += stats->m_stat_total_time.count();
		}

		log().verbose("Total calls : {1:12} {2:12} {3:12}", total_count,
			total_time, total_time / static_cast<decltype(total_time)>(total_count));

		log().verbose("Total loop     {1:15}", m_stat_mainloop());
		log().verbose("Total time     {1:15}", total_time);

		// FIXME: clang complains about unreachable code without
		const auto clang_workaround_unreachable_code = USE_QUEUE_STATS;
		if (clang_workaround_unreachable_code)
		{
			/* Only one serialization should be counted in total time */
			/* But two are contained in m_stat_mainloop */
			nperftime_t<true> overhead;
			nperftime_t<true> test;
			{
				auto overhead_guard(overhead.guard());
				for (int j=0; j<100000;j++)
				{
					auto test_guard(test.guard());
				}
			}

			nperftime_t<true>::type total_overhead = overhead()
					* static_cast<nperftime_t<true>::type>(total_count)
					/ static_cast<nperftime_t<true>::type>(200000);

			log().verbose("Queue Pushes   {1:15}", m_queue.m_prof_call());
			log().verbose("Queue Moves    {1:15}", m_queue.m_prof_sortmove());
			log().verbose("Queue Removes  {1:15}", m_queue.m_prof_remove());
			log().verbose("Queue Retimes  {1:15}", m_queue.m_prof_retime());
			log().verbose("");

			log().verbose("Take the next lines with a grain of salt. They depend on the measurement implementation.");
			log().verbose("Total overhead {1:15}", total_overhead);
			nperftime_t<true>::type overhead_per_pop = (m_stat_mainloop()-2*total_overhead - (total_time - total_overhead))
					/ static_cast<nperftime_t<true>::type>(m_queue.m_prof_call());
			log().verbose("Overhead per pop  {1:11}", overhead_per_pop );
			log().verbose("");
		}

		auto trigger = total_count * 200 / 1000000; // 200 ppm
		for (auto &entry : m_state->m_devices)
		{
			auto ep = entry.second.get();
			auto stats = ep->m_stats.get();
			// Factor of 3 offers best performace increase
			if (stats->m_stat_inc_active() > 3 * stats->m_stat_total_time.count()
				&& stats->m_stat_inc_active() > trigger)
				log().verbose("HINT({}, NO_DEACTIVATE) // {} {} {}", ep->name(),
					static_cast<double>(stats->m_stat_inc_active()) / static_cast<double>(stats->m_stat_total_time.count()),
					stats->m_stat_inc_active(), stats->m_stat_total_time.count());
		}
	}
}

core_device_t *netlist_state_t::get_single_device(const pstring &classname, bool (*cc)(core_device_t *)) const
{
	core_device_t *ret = nullptr;
	for (auto &d : m_devices)
	{
		if (cc(d.second.get()))
		{
			if (ret != nullptr)
				m_log.fatal(MF_MORE_THAN_ONE_1_DEVICE_FOUND(classname));
			else
				ret = d.second.get();
		}
	}
	return ret;
}


// ----------------------------------------------------------------------------------------
// core_device_t
// ----------------------------------------------------------------------------------------

core_device_t::core_device_t(netlist_state_t &owner, const pstring &name)
	: object_t(name)
	, logic_family_t()
	, netlist_ref(owner)
	, m_hint_deactivate(false)
	, m_active_outputs(*this, "m_active_outputs", 1)
{
	if (logic_family() == nullptr)
		set_logic_family(family_TTL());
	if (exec().stats_enabled())
		m_stats = plib::make_unique<stats_t>();
}

core_device_t::core_device_t(core_device_t &owner, const pstring &name)
	: object_t(owner.name() + "." + name)
	, logic_family_t()
	, netlist_ref(owner.state())
	, m_hint_deactivate(false)
	, m_active_outputs(*this, "m_active_outputs", 1)
{
	set_logic_family(owner.logic_family());
	if (logic_family() == nullptr)
		set_logic_family(family_TTL());
	state().add_dev(this->name(), pool_owned_ptr<core_device_t>(this, false));
	if (exec().stats_enabled())
		m_stats = plib::make_unique<stats_t>();
}

void core_device_t::set_default_delegate(detail::core_terminal_t &term)
{
	if (!term.m_delegate.is_set())
		term.m_delegate.set(&core_device_t::update, this);
}

log_type & core_device_t::log()
{
	return state().log();
}

// ----------------------------------------------------------------------------------------
// device_t
// ----------------------------------------------------------------------------------------

device_t::device_t(netlist_state_t &owner, const pstring &name)
: core_device_t(owner, name)
{
}

device_t::device_t(core_device_t &owner, const pstring &name)
: core_device_t(owner, name)
{
}

setup_t &device_t::setup()
{
	return state().setup();
}

const setup_t &device_t::setup() const
{
	return state().setup();
}

void device_t::register_subalias(const pstring &name, detail::core_terminal_t &term)
{
	pstring alias = this->name() + "." + name;

	// everything already fully qualified
	setup().register_alias_nofqn(alias, term.name());
}

void device_t::register_subalias(const pstring &name, const pstring &aliased)
{
	pstring alias = this->name() + "." + name;
	pstring aliased_fqn = this->name() + "." + aliased;

	// everything already fully qualified
	setup().register_alias_nofqn(alias, aliased_fqn);
}

void device_t::connect(detail::core_terminal_t &t1, detail::core_terminal_t &t2)
{
	setup().register_link_fqn(t1.name(), t2.name());
}

void device_t::connect(const pstring &t1, const pstring &t2)
{
	setup().register_link_fqn(name() + "." + t1, name() + "." + t2);
}

/* FIXME: this is only used by solver code since matrix solvers are started in
 *        post_start.
 */
void device_t::connect_post_start(detail::core_terminal_t &t1, detail::core_terminal_t &t2)
{
	if (!setup().connect(t1, t2))
		log().fatal(MF_ERROR_CONNECTING_1_TO_2(t1.name(), t2.name()));
}


// -----------------------------------------------------------------------------
// family_setter_t
// -----------------------------------------------------------------------------

// NOLINTNEXTLINE(modernize-use-equals-default)
detail::family_setter_t::family_setter_t()
{
}

detail::family_setter_t::family_setter_t(core_device_t &dev, const pstring &desc)
{
	dev.set_logic_family(dev.setup().family_from_model(desc));
}

detail::family_setter_t::family_setter_t(core_device_t &dev, const logic_family_desc_t *desc)
{
	dev.set_logic_family(desc);
}

// ----------------------------------------------------------------------------------------
// net_t
// ----------------------------------------------------------------------------------------

detail::net_t::net_t(netlist_state_t &nl, const pstring &aname, core_terminal_t *mr)
	: object_t(aname)
	, netlist_ref(nl)
	, m_new_Q(*this, "m_new_Q", 0)
	, m_cur_Q (*this, "m_cur_Q", 0)
	, m_in_queue(*this, "m_in_queue", queue_status::DELIVERED)
	, m_next_scheduled_time(*this, "m_time", netlist_time::zero())
	, m_railterminal(mr)
{
}

void detail::net_t::rebuild_list()
{
	/* rebuild m_list */

	m_list_active.clear();
	for (auto & term : m_core_terms)
		if (term->terminal_state() != logic_t::STATE_INP_PASSIVE)
		{
			m_list_active.push_back(term);
			term->set_copied_input(m_cur_Q);
		}
}

template <bool KEEP_STATS, typename T>
void detail::net_t::process(const T mask, netlist_sig_t sig)
{
	m_cur_Q = sig;

	if (KEEP_STATS)
	{
		for (auto & p : m_list_active)
		{
			p.set_copied_input(sig);
			auto *stats = p.device().m_stats.get();
			stats->m_stat_call_count.inc();
			if ((p.terminal_state() & mask))
			{
				auto g(stats->m_stat_total_time.guard());
				p.m_delegate();
			}
		}
	}
	else
	{
		for (auto & p : m_list_active)
		{
			p.set_copied_input(sig);
			if ((p.terminal_state() & mask))
				p.m_delegate();
		}
	}
}

template <bool KEEP_STATS>
void detail::net_t::update_devs() NL_NOEXCEPT
{
	nl_assert(this->isRailNet());

	const unsigned int new_Q(m_new_Q);

	const unsigned int mask((new_Q << core_terminal_t::INP_LH_SHIFT)
			| (m_cur_Q << core_terminal_t::INP_HL_SHIFT));

	m_in_queue = queue_status::DELIVERED; /* mark as taken ... */
	switch (mask)
	{
		case core_terminal_t::STATE_INP_HL:
		case core_terminal_t::STATE_INP_LH:
			process<KEEP_STATS>(mask | core_terminal_t::STATE_INP_ACTIVE, new_Q);
			break;
		default:
			/* do nothing */
			break;
	}
}

void detail::net_t::reset()
{
	m_next_scheduled_time = netlist_time::zero();
	m_in_queue = queue_status::DELIVERED;

	m_new_Q = 0;
	m_cur_Q = 0;

	auto *p = dynamic_cast<analog_net_t *>(this);

	if (p != nullptr)
		p->m_cur_Analog = 0.0;

	/* rebuild m_list and reset terminals to active or analog out state */

	m_list_active.clear();
	for (core_terminal_t *ct : m_core_terms)
	{
		ct->reset();
		if (ct->terminal_state() != logic_t::STATE_INP_PASSIVE)
			m_list_active.push_back(ct);
		ct->set_copied_input(m_cur_Q);
	}
}

void detail::net_t::add_terminal(detail::core_terminal_t &terminal)
{
	for (auto &t : m_core_terms)
		if (t == &terminal)
			state().log().fatal(MF_NET_1_DUPLICATE_TERMINAL_2(name(), t->name()));

	terminal.set_net(this);

	m_core_terms.push_back(&terminal);
}

void detail::net_t::remove_terminal(detail::core_terminal_t &terminal)
{
	if (plib::container::contains(m_core_terms, &terminal))
	{
		terminal.set_net(nullptr);
		plib::container::remove(m_core_terms, &terminal);
	}
	else
		state().log().fatal(MF_REMOVE_TERMINAL_1_FROM_NET_2(terminal.name(), this->name()));
}

void detail::net_t::move_connections(detail::net_t &dest_net)
{
	for (auto &ct : m_core_terms)
		dest_net.add_terminal(*ct);
	m_core_terms.clear();
}

// ----------------------------------------------------------------------------------------
// logic_net_t
// ----------------------------------------------------------------------------------------

logic_net_t::logic_net_t(netlist_state_t &nl, const pstring &aname, detail::core_terminal_t *mr)
	: net_t(nl, aname, mr)
{
}

// ----------------------------------------------------------------------------------------
// analog_net_t
// ----------------------------------------------------------------------------------------

analog_net_t::analog_net_t(netlist_state_t &nl, const pstring &aname, detail::core_terminal_t *mr)
	: net_t(nl, aname, mr)
	, m_cur_Analog(*this, "m_cur_Analog", 0.0)
	, m_solver(nullptr)
{
}

// ----------------------------------------------------------------------------------------
// core_terminal_t
// ----------------------------------------------------------------------------------------

detail::core_terminal_t::core_terminal_t(core_device_t &dev, const pstring &aname,
		const state_e state, nldelegate delegate)
: device_object_t(dev, dev.name() + "." + aname)
, plib::linkedlist_t<core_terminal_t>::element_t()
, m_delegate(delegate)
#if USE_COPY_INSTEAD_OF_REFERENCE
, m_Q(*this, "m_Q", 0)
#endif
, m_net(nullptr)
, m_state(*this, "m_state", state)
{
}

analog_t::analog_t(core_device_t &dev, const pstring &aname, const state_e state,
	nldelegate delegate)
: core_terminal_t(dev, aname, state, delegate)
{
}

logic_t::logic_t(core_device_t &dev, const pstring &aname, const state_e state,
		nldelegate delegate)
	: core_terminal_t(dev, aname, state, delegate)
	, logic_family_t()
	, m_proxy(nullptr)
{
}

// ----------------------------------------------------------------------------------------
// terminal_t
// ----------------------------------------------------------------------------------------

terminal_t::terminal_t(core_device_t &dev, const pstring &aname, terminal_t *otherterm)
: analog_t(dev, aname, STATE_BIDIR)
, m_Idr1(nullptr)
, m_go1(nullptr)
, m_gt1(nullptr)
, m_connected_terminal(otherterm)
{
	state().setup().register_term(*this);
}

void terminal_t::solve_now()
{
	// Nets may belong to railnets which do not have a solver attached
	if (this->has_net())
		if (net().solver() != nullptr)
			net().solver()->update_forced();
}

void terminal_t::schedule_solve_after(const netlist_time after)
{
	// Nets may belong to railnets which do not have a solver attached
	if (this->has_net())
		if (net().solver() != nullptr)
			net().solver()->update_after(after);
}

// ----------------------------------------------------------------------------------------
// net_input_t
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// net_output_t
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// logic_output_t
// ----------------------------------------------------------------------------------------

logic_output_t::logic_output_t(core_device_t &dev, const pstring &aname)
	: logic_t(dev, aname, STATE_OUT)
	, m_my_net(dev.state(), name() + ".net", this)
{
	this->set_net(&m_my_net);
	state().register_net(pool_owned_ptr<logic_net_t>(&m_my_net, false));
	set_logic_family(dev.logic_family());
	state().setup().register_term(*this);
}

void logic_output_t::initial(const netlist_sig_t val)
{
	if (has_net())
		net().initial(val);
}

// ----------------------------------------------------------------------------------------
// analog_input_t
// ----------------------------------------------------------------------------------------

analog_input_t::analog_input_t(core_device_t &dev, const pstring &aname,
	nldelegate delegate)
: analog_t(dev, aname, STATE_INP_ACTIVE, delegate)
{
	state().setup().register_term(*this);
}

// ----------------------------------------------------------------------------------------
// analog_output_t
// ----------------------------------------------------------------------------------------

analog_output_t::analog_output_t(core_device_t &dev, const pstring &aname)
	: analog_t(dev, aname, STATE_OUT)
	, m_my_net(dev.state(), name() + ".net", this)
{
	state().register_net(pool_owned_ptr<analog_net_t>(&m_my_net, false));
	this->set_net(&m_my_net);

	//net().m_cur_Analog = NL_FCONST(0.0);
	state().setup().register_term(*this);
}

void analog_output_t::initial(const nl_double val)
{
	net().set_Q_Analog(val);
}

// -----------------------------------------------------------------------------
// logic_input_t
// -----------------------------------------------------------------------------

logic_input_t::logic_input_t(core_device_t &dev, const pstring &aname,
		nldelegate delegate)
		: logic_t(dev, aname, STATE_INP_ACTIVE, delegate)
{
	set_logic_family(dev.logic_family());
	state().setup().register_term(*this);
}

// ----------------------------------------------------------------------------------------
// Parameters ...
// ----------------------------------------------------------------------------------------

param_t::param_t(device_t &device, const pstring &name)
	: device_object_t(device, device.name() + "." + name)
{
	device.setup().register_param_t(this->name(), *this);
}

param_t::param_type_t param_t::param_type() const
{
	if (dynamic_cast<const param_str_t *>(this) != nullptr)
		return STRING;
	else if (dynamic_cast<const param_double_t *>(this) != nullptr)
		return DOUBLE;
	else if (dynamic_cast<const param_int_t *>(this) != nullptr)
		return INTEGER;
	else if (dynamic_cast<const param_logic_t *>(this) != nullptr)
		return LOGIC;
	else if (dynamic_cast<const param_ptr_t *>(this) != nullptr)
		return POINTER;
	else
	{
		state().log().fatal(MF_UNKNOWN_PARAM_TYPE(name()));
		return POINTER; /* Please compiler */
	}
}


void param_t::update_param()
{
	device().update_param();
}

pstring param_t::get_initial(const device_t &dev, bool *found)
{
	pstring res = dev.setup().get_initial_param_val(this->name(), "");
	*found = (res != "");
	return res;
}

const pstring param_model_t::type()
{
	return state().setup().models().type(str());
}

param_str_t::param_str_t(device_t &device, const pstring &name, const pstring &val)
: param_t(device, name)
{
	m_param = device.setup().get_initial_param_val(this->name(),val);
}

void param_str_t::changed()
{
}

param_ptr_t::param_ptr_t(device_t &device, const pstring &name, uint8_t * val)
: param_t(device, name)
{
	m_param = val; //device.setup().get_initial_param_val(this->name(),val);
	//netlist().save(*this, m_param, "m_param");
}

void param_model_t::changed()
{
	state().log().fatal(MF_MODEL_1_CAN_NOT_BE_CHANGED_AT_RUNTIME(name()));
}

const pstring param_model_t::value_str(const pstring &entity)
{
	return state().setup().models().value_str(str(), entity);
}

nl_double param_model_t::value(const pstring &entity)
{
	return state().setup().models().value(str(), entity);
}


plib::unique_ptr<plib::pistream> param_data_t::stream()
{
	return device().setup().get_data_stream(str());
}

	bool detail::core_terminal_t::is_logic() const NL_NOEXCEPT
	{
		return dynamic_cast<const logic_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_logic_input() const NL_NOEXCEPT
	{
		return dynamic_cast<const logic_input_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_logic_output() const NL_NOEXCEPT
	{
		return dynamic_cast<const logic_output_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_analog() const NL_NOEXCEPT
	{
		return dynamic_cast<const analog_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_analog_input() const NL_NOEXCEPT
	{
		return dynamic_cast<const analog_input_t *>(this) != nullptr;
	}

	bool detail::core_terminal_t::is_analog_output() const NL_NOEXCEPT
	{
		return dynamic_cast<const analog_output_t *>(this) != nullptr;
	}


	bool detail::net_t::is_logic() const NL_NOEXCEPT
	{
		return dynamic_cast<const logic_net_t *>(this) != nullptr;
	}

	bool detail::net_t::is_analog() const NL_NOEXCEPT
	{
		return dynamic_cast<const analog_net_t *>(this) != nullptr;
	}


} // namespace netlist
