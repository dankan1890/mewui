// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nlbase.c
 *
 */

#include <cstring>

#include "solver/nld_matrix_solver.h"
#include "solver/nld_solver.h"

#include "plib/putil.h"
#include "plib/palloc.h"

#include "nl_base.h"
#include "devices/nlid_system.h"

namespace netlist
{
namespace detail
{
#if (USE_MEMPOOL)
static plib::mempool pool(65536, 8);

void * object_t::operator new (size_t size)
{
	return pool.alloc(size);
}

void object_t::operator delete (void * mem)
{
	if (mem)
		pool.free(mem);
}
#else
void * object_t::operator new (size_t size)
{
	return ::operator new(size);
}

void object_t::operator delete (void * mem)
{
	if (mem)
		::operator delete(mem);
}
#endif

}

// ----------------------------------------------------------------------------------------
// logic_family_ttl_t
// ----------------------------------------------------------------------------------------

class logic_family_ttl_t : public logic_family_desc_t
{
public:
	logic_family_ttl_t() : logic_family_desc_t()
	{
		m_low_thresh_V = 0.8;
		m_high_thresh_V = 2.0;
		// m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
		m_low_V = 0.1;
		m_high_V = 4.0;
		m_R_low = 1.0;
		m_R_high = 130.0;
	}
	virtual plib::owned_ptr<devices::nld_base_d_to_a_proxy> create_d_a_proxy(netlist_t &anetlist, const pstring &name, logic_output_t *proxied) const override
	{
		return plib::owned_ptr<devices::nld_base_d_to_a_proxy>::Create<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
	}
};

class logic_family_cd4xxx_t : public logic_family_desc_t
{
public:
	logic_family_cd4xxx_t() : logic_family_desc_t()
	{
		m_low_thresh_V = 0.8;
		m_high_thresh_V = 2.0;
		// m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
		m_low_V = 0.05;
		m_high_V = 4.95;
		m_R_low = 10.0;
		m_R_high = 10.0;
	}
	virtual plib::owned_ptr<devices::nld_base_d_to_a_proxy> create_d_a_proxy(netlist_t &anetlist, const pstring &name, logic_output_t *proxied) const override
	{
		return plib::owned_ptr<devices::nld_base_d_to_a_proxy>::Create<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
	}
};

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

detail::queue_t::queue_t(netlist_t &nl)
	: timed_queue<net_t *, netlist_time>(512)
	, object_t("QUEUE")
	, netlist_ref(nl)
	, plib::state_manager_t::callback_t()
	, m_qsize(0)
	, m_times(512)
	, m_names(512)
{
}

void detail::queue_t::register_state(plib::state_manager_t &manager, const pstring &module)
{
	netlist().log().debug("register_state\n");
	manager.save_item(this, m_qsize, module + "." + "qsize");
	manager.save_item(this, &m_times[0], module + "." + "times", m_times.size());
	manager.save_item(this, &(m_names[0].m_buf[0]), module + "." + "names", m_names.size() * sizeof(names_t));
}

void detail::queue_t::on_pre_save()
{
	netlist().log().debug("on_pre_save\n");
	m_qsize = this->size();
	netlist().log().debug("current time {1} qsize {2}\n", netlist().time().as_double(), m_qsize);
	for (std::size_t i = 0; i < m_qsize; i++ )
	{
		m_times[i] =  this->listptr()[i].m_exec_time.as_raw();
		pstring p = this->listptr()[i].m_object->name();
		std::size_t n = p.len();
		if (n > 63) n = 63;
		std::strncpy(m_names[i].m_buf, p.cstr(), n);
		m_names[i].m_buf[n] = 0;
	}
}


void detail::queue_t::on_post_load()
{
	this->clear();
	netlist().log().debug("current time {1} qsize {2}\n", netlist().time().as_double(), m_qsize);
	for (std::size_t i = 0; i < m_qsize; i++ )
	{
		detail::net_t *n = netlist().find_net(m_names[i].m_buf);
		//log().debug("Got {1} ==> {2}\n", qtemp[i].m_name, n));
		//log().debug("schedule time {1} ({2})\n", n->time().as_double(),  netlist_time::from_raw(m_times[i]).as_double()));
		this->push(netlist_time::from_raw(m_times[i]), n);
	}
}

// ----------------------------------------------------------------------------------------
// object_t
// ----------------------------------------------------------------------------------------

detail::object_t::object_t(const pstring &aname)
	: m_name(aname)
{
}

detail::object_t::~object_t()
{
}

const pstring &detail::object_t::name() const
{
	return m_name;
}

// ----------------------------------------------------------------------------------------
// device_object_t
// ----------------------------------------------------------------------------------------

detail::device_object_t::device_object_t(core_device_t &dev, const pstring &aname, const type_t atype)
: object_t(aname)
, m_device(dev)
, m_type(atype)
{
}


// ----------------------------------------------------------------------------------------
// netlist_t
// ----------------------------------------------------------------------------------------

netlist_t::netlist_t(const pstring &aname)
	: m_state()
	, m_time(netlist_time::zero())
	, m_queue(*this)
	, m_mainclock(nullptr)
	, m_solver(nullptr)
	, m_gnd(nullptr)
	, m_params(nullptr)
	, m_name(aname)
	, m_setup(nullptr)
	, m_log(this)
	, m_lib(nullptr)
{
	state().save_item(this, static_cast<plib::state_manager_t::callback_t &>(m_queue), "m_queue");
	state().save_item(this, m_time, "m_time");
}

netlist_t::~netlist_t()
{
	m_nets.clear();
	m_devices.clear();

	pfree(m_lib);
	pstring::resetmem();
}

nl_double netlist_t::gmin() const
{
	return solver()->gmin();
}

void netlist_t::register_dev(plib::owned_ptr<device_t> dev)
{
	for (auto & d : m_devices)
		if (d->name() == dev->name())
			log().fatal("Error adding {1} to device list. Duplicate name \n", d->name());
	m_devices.push_back(std::move(dev));
}

void netlist_t::start()
{
	/* load the library ... */

	/* make sure the solver and parameters are started first! */

	for (auto & e : setup().m_device_factory)
	{
		if ( setup().factory().is_class<devices::NETLIB_NAME(mainclock)>(e.second)
				|| setup().factory().is_class<devices::NETLIB_NAME(solver)>(e.second)
				|| setup().factory().is_class<devices::NETLIB_NAME(gnd)>(e.second)
				|| setup().factory().is_class<devices::NETLIB_NAME(netlistparams)>(e.second))
		{
			auto dev = plib::owned_ptr<device_t>(e.second->Create(*this, e.first));
			register_dev(std::move(dev));
		}
	}

	log().debug("Searching for mainclock and solver ...\n");

	m_mainclock = get_single_device<devices::NETLIB_NAME(mainclock)>("mainclock");
	m_solver = get_single_device<devices::NETLIB_NAME(solver)>("solver");
	m_gnd = get_single_device<devices::NETLIB_NAME(gnd)>("gnd");
	m_params = get_single_device<devices::NETLIB_NAME(netlistparams)>("parameter");

	/* create devices */

	for (auto & e : setup().m_device_factory)
	{
		if ( !setup().factory().is_class<devices::NETLIB_NAME(mainclock)>(e.second)
				&& !setup().factory().is_class<devices::NETLIB_NAME(solver)>(e.second)
				&& !setup().factory().is_class<devices::NETLIB_NAME(gnd)>(e.second)
				&& !setup().factory().is_class<devices::NETLIB_NAME(netlistparams)>(e.second))
		{
			auto dev = plib::owned_ptr<device_t>(e.second->Create(*this, e.first));
			register_dev(std::move(dev));
		}
	}

	bool use_deactivate = (m_params->m_use_deactivate() ? true : false);

	for (auto &d : m_devices)
	{
		if (use_deactivate)
		{
			auto p = setup().m_param_values.find(d->name() + ".HINT_NO_DEACTIVATE");
			if (p != setup().m_param_values.end())
			{
				//FIXME: Error checking
				auto v = p->second.as_long();
				d->set_hint_deactivate(!v);
			}
		}
		else
			d->set_hint_deactivate(false);
	}


	pstring libpath = plib::util::environment("NL_BOOSTLIB", plib::util::buildpath({".", "nlboost.so"}));

	m_lib = plib::palloc<plib::dynlib>(libpath);


}

void netlist_t::stop()
{
	/* find the main clock and solver ... */

	log().debug("Stopping all devices ...\n");
	for (auto & dev : m_devices)
		dev->stop_dev();
}

detail::net_t *netlist_t::find_net(const pstring &name)
{
	for (auto & net : m_nets)
		if (net->name() == name)
			return net.get();

	return nullptr;
}

void netlist_t::rebuild_lists()
{
	for (auto & net : m_nets)
		net->rebuild_list();
}


void netlist_t::reset()
{
	m_time = netlist_time::zero();
	m_queue.clear();
	if (m_mainclock != nullptr)
		m_mainclock->m_Q.net().set_time(netlist_time::zero());
	//if (m_solver != nullptr)
	//  m_solver->do_reset();

	// Reset all nets once !
	for (auto & n : m_nets)
		n->reset();

	// Reset all devices once !
	for (auto & dev : m_devices)
		dev->do_reset();

	// Make sure everything depending on parameters is set
	for (auto & dev : m_devices)
		dev->update_param();

	// Step all devices once !
#if 0
	for (std::size_t i = 0; i < m_devices.size(); i++)
	{
		m_devices[i]->update_dev();
	}
#else
	/* FIXME: this makes breakout attract mode working again.
	 * It is however not acceptable that this depends on the startup order.
	 * Best would be, if reset would call update_dev for devices which need it.
	 */
	std::size_t i = m_devices.size();
	while (i>0)
		m_devices[--i]->update_dev();
#endif
}


void netlist_t::process_queue(const netlist_time &delta)
{
	netlist_time stop(m_time + delta);

	m_queue.push(stop, nullptr);

	m_stat_mainloop.start();

	if (m_mainclock == nullptr)
	{
		detail::queue_t::entry_t e(m_queue.pop());
		m_time = e.m_exec_time;
		while (e.m_object != nullptr)
		{
			e.m_object->update_devs();
			m_perf_out_processed.inc();
			e = m_queue.pop();
			m_time = e.m_exec_time;
		}
	}
	else
	{
		logic_net_t &mc_net = m_mainclock->m_Q.net();
		const netlist_time inc = m_mainclock->m_inc;
		netlist_time mc_time(mc_net.time());

		while (1)
		{
			while (m_queue.top().m_exec_time > mc_time)
			{
				m_time = mc_time;
				mc_time += inc;
				mc_net.toggle_new_Q();
				mc_net.update_devs();
			}

			const detail::queue_t::entry_t e(m_queue.pop());
			m_time = e.m_exec_time;
			if (e.m_object == nullptr)
				break;
			e.m_object->update_devs();
			m_perf_out_processed.inc();
		}
		mc_net.set_time(mc_time);
	}
	m_stat_mainloop.stop();
}

void netlist_t::print_stats() const
{
	if (nperftime_t::enabled)
	{
		std::vector<size_t> index;
		for (size_t i=0; i<m_devices.size(); i++)
			index.push_back(i);

		std::sort(index.begin(), index.end(),
				[&](size_t i1, size_t i2) { return m_devices[i1]->m_stat_total_time.total() < m_devices[i2]->m_stat_total_time.total(); });

		nperftime_t::type total_time(0);
		uint_least64_t total_count(0);

		for (auto & j : index)
		{
			auto entry = m_devices[j].get();
			log().verbose("Device {1:20} : {2:12} {3:12} {4:15} {5:12}", entry->name(),
					entry->m_stat_call_count(), entry->m_stat_total_time.count(),
					entry->m_stat_total_time.total(), entry->m_stat_inc_active());
			total_time += entry->m_stat_total_time.total();
			total_count += entry->m_stat_total_time.count();
		}

		nperftime_t overhead;
		nperftime_t test;
		overhead.start();
		for (int j=0; j<100000;j++)
		{
			test.start();
			test.stop();
		}
		overhead.stop();

		nperftime_t::type total_overhead = overhead()
				* static_cast<nperftime_t::type>(total_count)
				/ static_cast<nperftime_t::type>(200000);

		log().verbose("Queue Pushes   {1:15}", queue().m_prof_call());
		log().verbose("Queue Moves    {1:15}", queue().m_prof_sortmove());

		log().verbose("Total loop     {1:15}", m_stat_mainloop());
		/* Only one serialization should be counted in total time */
		/* But two are contained in m_stat_mainloop */
		log().verbose("Total devices  {1:15}", total_time);
		log().verbose("");
		log().verbose("Take the next lines with a grain of salt. They depend on the measurement implementation.");
		log().verbose("Total overhead {1:15}", total_overhead);
		nperftime_t::type overhead_per_pop = (m_stat_mainloop()-2*total_overhead - (total_time - total_overhead))
				/ static_cast<nperftime_t::type>(queue().m_prof_call());
		log().verbose("Overhead per pop  {1:11}", overhead_per_pop );
		log().verbose("");
		for (auto &entry : m_devices)
		{
			if (entry->m_stat_inc_active() > 3 * entry->m_stat_total_time.count())
				log().verbose("HINT({}, NO_DEACTIVATE)", entry->name());
		}
	}
}

// ----------------------------------------------------------------------------------------
// Parameters ...
// ----------------------------------------------------------------------------------------

template <typename C, param_t::param_type_t T>
param_template_t<C, T>::param_template_t(device_t &device, const pstring name, const C val)
: param_t(T, device, device.name() + "." + name)
, m_param(val)
{
	/* pstrings not yet supported, these need special logic */
	if (T != param_t::STRING && T != param_t::MODEL)
		netlist().save(*this, m_param, "m_param");
	device.setup().register_and_set_param(device.name() + "." + name, *this);
}

template class param_template_t<double, param_t::DOUBLE>;
template class param_template_t<int, param_t::INTEGER>;
template class param_template_t<bool, param_t::LOGIC>;
template class param_template_t<pstring, param_t::STRING>;
template class param_template_t<pstring, param_t::MODEL>;

// ----------------------------------------------------------------------------------------
// core_device_t
// ----------------------------------------------------------------------------------------

core_device_t::core_device_t(netlist_t &owner, const pstring &name)
	: object_t(name)
	, logic_family_t()
	, netlist_ref(owner)
	, m_hint_deactivate(false)
#if (NL_PMF_TYPE > NL_PMF_TYPE_VIRTUAL)
	, m_static_update()
#endif
{
	if (logic_family() == nullptr)
		set_logic_family(family_TTL());
}

core_device_t::core_device_t(core_device_t &owner, const pstring &name)
	: object_t(owner.name() + "." + name)
	, logic_family_t()
	, netlist_ref(owner.netlist())
	, m_hint_deactivate(false)
#if (NL_PMF_TYPE > NL_PMF_TYPE_VIRTUAL)
	, m_static_update()
#endif
{
	set_logic_family(owner.logic_family());
	if (logic_family() == nullptr)
		set_logic_family(family_TTL());
	owner.netlist().m_devices.push_back(plib::owned_ptr<core_device_t>(this, false));
}

core_device_t::~core_device_t()
{
}

void core_device_t::set_delegate_pointer()
{
#if (NL_PMF_TYPE == NL_PMF_TYPE_GNUC_PMF)
	void (core_device_t::* pFunc)() = &core_device_t::update;
	m_static_update = pFunc;
#elif (NL_PMF_TYPE == NL_PMF_TYPE_GNUC_PMF_CONV)
	void (core_device_t::* pFunc)() = &core_device_t::update;
	m_static_update = reinterpret_cast<net_update_delegate>((this->*pFunc));
#elif (NL_PMF_TYPE == NL_PMF_TYPE_INTERNAL)
	m_static_update = plib::mfp::get_mfp<net_update_delegate>(&core_device_t::update, this);
#endif
}

void core_device_t::stop_dev()
{
	//NOTE: stop_dev is not removed. It remains so it can be reactivated in case
	//      we run into a situation were RAII and noexcept dtors force us to
	//      to have a device stop() routine which may throw.
	//stop();
}

// ----------------------------------------------------------------------------------------
// device_t
// ----------------------------------------------------------------------------------------

device_t::~device_t()
{
	//log().debug("~net_device_t\n");
}

setup_t &device_t::setup()
{
	return netlist().setup();
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

void device_t::connect_late(detail::core_terminal_t &t1, detail::core_terminal_t &t2)
{
	setup().register_link_fqn(t1.name(), t2.name());
}

void device_t::connect_late(const pstring &t1, const pstring &t2)
{
	setup().register_link_fqn(name() + "." + t1, name() + "." + t2);
}

/* FIXME: this is only used by solver code since matrix solvers are started in
 *        post_start.
 */
void device_t::connect_post_start(detail::core_terminal_t &t1, detail::core_terminal_t &t2)
{
	if (!setup().connect(t1, t2))
		netlist().log().fatal("Error connecting {1} to {2}\n", t1.name(), t2.name());
}


// -----------------------------------------------------------------------------
// family_setter_t
// -----------------------------------------------------------------------------

detail::family_setter_t::family_setter_t(core_device_t &dev, const char *desc)
{
	dev.set_logic_family(dev.netlist().setup().family_from_model(desc));
}

detail::family_setter_t::family_setter_t(core_device_t &dev, const logic_family_desc_t *desc)
{
	dev.set_logic_family(desc);
}

// ----------------------------------------------------------------------------------------
// net_t
// ----------------------------------------------------------------------------------------

// FIXME: move somewhere central

struct do_nothing_deleter{
	template<typename T> void operator()(T*){}
};


detail::net_t::net_t(netlist_t &nl, const pstring &aname, core_terminal_t *mr)
	: object_t(aname)
	, netlist_ref(nl)
	, m_new_Q(*this, "m_new_Q", 0)
	, m_cur_Q (*this, "m_cur_Q", 0)
	, m_time(*this, "m_time", netlist_time::zero())
	, m_active(*this, "m_active", 0)
	, m_in_queue(*this, "m_in_queue", 2)
	, m_railterminal(nullptr)
	, m_cur_Analog(*this, "m_cur_Analog", 0.0)
{
	m_railterminal = mr;
	if (mr != nullptr)
		nl.m_nets.push_back(plib::owned_ptr<net_t>(this, false));
	else
		nl.m_nets.push_back(plib::owned_ptr<net_t>(this, true));
}

detail::net_t::~net_t()
{
	netlist().state().remove_save_items(this);
}

void detail::net_t::inc_active(core_terminal_t &term) NL_NOEXCEPT
{
	m_active++;
	m_list_active.push_front(&term);
	nl_assert(m_active <= static_cast<int>(num_cons()));
	if (m_active == 1)
	{
		railterminal().device().do_inc_active();
		if (m_in_queue == 0)
		{
			if (m_time > netlist().time())
			{
				m_in_queue = 1;     /* pending */
				netlist().push_to_queue(*this, m_time);
			}
			else
			{
				m_cur_Q = m_new_Q;
				m_in_queue = 2;
			}
		}
	}
}

void detail::net_t::dec_active(core_terminal_t &term) NL_NOEXCEPT
{
	--m_active;
	nl_assert(m_active >= 0);
	m_list_active.remove(&term);
	if (m_active == 0)
		railterminal().device().do_dec_active();
}

void detail::net_t::rebuild_list()
{
	/* rebuild m_list */

	unsigned cnt = 0;
	m_list_active.clear();
	for (auto & term : m_core_terms)
		if (term->state() != logic_t::STATE_INP_PASSIVE)
		{
			m_list_active.push_back(term);
			cnt++;
		}
	m_active = cnt;
}

void detail::net_t::update_devs() NL_NOEXCEPT
{
	nl_assert(this->isRailNet());

	static const unsigned masks[4] =
	{
		0,
		core_terminal_t::STATE_INP_LH | core_terminal_t::STATE_INP_ACTIVE,
		core_terminal_t::STATE_INP_HL | core_terminal_t::STATE_INP_ACTIVE,
		0
	};

	const unsigned mask = masks[ m_cur_Q  * 2 + m_new_Q ];

	m_in_queue = 2; /* mark as taken ... */
	m_cur_Q = m_new_Q;

	for (auto & p : m_list_active)
	{
		p.device().m_stat_call_count.inc();
		if ((p.state() & mask) != 0)
			p.device().update_dev();
	}
}

void detail::net_t::reset()
{
	m_time = netlist_time::zero();
	m_active = 0;
	m_in_queue = 2;

	m_new_Q = 0;
	m_cur_Q = 0;
	m_cur_Analog = 0.0;

	/* rebuild m_list */

	m_list_active.clear();
	for (core_terminal_t *ct : m_core_terms)
		m_list_active.push_back(ct);

	for (core_terminal_t *ct : m_core_terms)
		ct->reset();

	for (core_terminal_t *ct : m_core_terms)
		if (ct->state() != logic_t::STATE_INP_PASSIVE)
			m_active++;
}

void detail::net_t::register_con(detail::core_terminal_t &terminal)
{
	terminal.set_net(this);

	m_core_terms.push_back(&terminal);

	if (terminal.state() != logic_t::STATE_INP_PASSIVE)
		m_active++;
}

void detail::net_t::move_connections(detail::net_t &dest_net)
{
	for (auto &ct : m_core_terms)
		dest_net.register_con(*ct);
	m_core_terms.clear();
	m_active = 0;
}

// ----------------------------------------------------------------------------------------
// logic_net_t
// ----------------------------------------------------------------------------------------

logic_net_t::logic_net_t(netlist_t &nl, const pstring &aname, detail::core_terminal_t *mr)
	: net_t(nl, aname, mr)
{
}


// ----------------------------------------------------------------------------------------
// analog_net_t
// ----------------------------------------------------------------------------------------

analog_net_t::analog_net_t(netlist_t &nl, const pstring &aname, detail::core_terminal_t *mr)
	: net_t(nl, aname, mr)
	, m_solver(nullptr)
{
}

// ----------------------------------------------------------------------------------------
// core_terminal_t
// ----------------------------------------------------------------------------------------

detail::core_terminal_t::core_terminal_t(core_device_t &dev, const pstring &aname,
		const type_t type, const state_e state)
: device_object_t(dev, dev.name() + "." + aname, type)
, plib::linkedlist_t<core_terminal_t>::element_t()
, m_net(nullptr)
, m_state(*this, "m_state", state)
{
}

void detail::core_terminal_t::reset()
{
	if (is_type(OUTPUT))
		set_state(STATE_OUT);
	else
		set_state(STATE_INP_ACTIVE);
}

void detail::core_terminal_t::set_net(net_t *anet)
{
	m_net = anet;
}

	void detail::core_terminal_t::clear_net()
{
	m_net = nullptr;
}


// ----------------------------------------------------------------------------------------
// terminal_t
// ----------------------------------------------------------------------------------------

terminal_t::terminal_t(core_device_t &dev, const pstring &aname)
: analog_t(dev, aname, TERMINAL, STATE_BIDIR)
, m_otherterm(nullptr)
, m_Idr1(*this, "m_Idr1", nullptr)
, m_go1(*this, "m_go1", nullptr)
, m_gt1(*this, "m_gt1", nullptr)
{
	netlist().setup().register_term(*this);
}


void terminal_t::schedule_solve()
{
	// FIXME: Remove this after we found a way to remove *ALL* twoterms connected to railnets only.
	if (net().solver() != nullptr)
		net().solver()->update_forced();
}

void terminal_t::schedule_after(const netlist_time &after)
{
	// FIXME: Remove this after we found a way to remove *ALL* twoterms connected to railnets only.
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
	: logic_t(dev, aname, OUTPUT, STATE_OUT)
	, m_my_net(dev.netlist(), name() + ".net", this)
{
	this->set_net(&m_my_net);
	set_logic_family(dev.logic_family());
	netlist().setup().register_term(*this);
}

void logic_output_t::initial(const netlist_sig_t val)
{
	net().initial(val);
}

// ----------------------------------------------------------------------------------------
// analog_input_t
// ----------------------------------------------------------------------------------------

analog_input_t::analog_input_t(core_device_t &dev, const pstring &aname)
: analog_t(dev, aname, INPUT, STATE_INP_ACTIVE)
{
	netlist().setup().register_term(*this);
}

// ----------------------------------------------------------------------------------------
// analog_output_t
// ----------------------------------------------------------------------------------------

analog_output_t::analog_output_t(core_device_t &dev, const pstring &aname)
	: analog_t(dev, aname, OUTPUT, STATE_OUT)
	, m_my_net(dev.netlist(), name() + ".net", this)
{
	this->set_net(&m_my_net);

	net().m_cur_Analog = NL_FCONST(0.0);
	netlist().setup().register_term(*this);
}

void analog_output_t::initial(const nl_double val)
{
	net().m_cur_Analog = val;
}

// -----------------------------------------------------------------------------
// logic_input_t
// -----------------------------------------------------------------------------

logic_input_t::logic_input_t(core_device_t &dev, const pstring &aname)
		: logic_t(dev, aname, INPUT, STATE_INP_ACTIVE)
{
	set_logic_family(dev.logic_family());
	netlist().setup().register_term(*this);
}

// ----------------------------------------------------------------------------------------
// param_t & friends
// ----------------------------------------------------------------------------------------

param_t::param_t(const param_type_t atype, device_t &device, const pstring &name)
	: device_object_t(device, name, PARAM)
	, m_param_type(atype)
{
}

const pstring param_model_t::model_type()
{
	if (m_map.size() == 0)
		netlist().setup().model_parse(this->Value(), m_map);
	return m_map["COREMODEL"];
}


const pstring param_model_t::model_value_str(const pstring &entity)
{
	if (m_map.size() == 0)
		netlist().setup().model_parse(this->Value(), m_map);
	return netlist().setup().model_value_str(m_map, entity);
}

nl_double param_model_t::model_value(const pstring &entity)
{
	if (m_map.size() == 0)
		netlist().setup().model_parse(this->Value(), m_map);
	return netlist().setup().model_value(m_map, entity);
}


	namespace devices
	{
	// ----------------------------------------------------------------------------------------
	// mainclock
	// ----------------------------------------------------------------------------------------

	void NETLIB_NAME(mainclock)::mc_update(logic_net_t &net)
	{
		net.toggle_new_Q();
		net.update_devs();
	}


	} //namespace devices
} // namespace netlist
