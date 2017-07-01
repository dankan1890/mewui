// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nlsetup.c
 *
 */

#include <cstdio>

#include "solver/nld_solver.h"

#include "plib/palloc.h"
#include "plib/putil.h"
#include "nl_base.h"
#include "nl_setup.h"
#include "nl_parser.h"
#include "nl_factory.h"
#include "devices/net_lib.h"
#include "devices/nld_truthtable.h"
#include "devices/nlid_system.h"
#include "analog/nld_twoterm.h"
#include "solver/nld_solver.h"

static NETLIST_START(base)
	TTL_INPUT(ttlhigh, 1)
	TTL_INPUT(ttllow, 0)
	NET_REGISTER_DEV(GND, GND)
	NET_REGISTER_DEV(PARAMETER, NETLIST)

	LOCAL_SOURCE(diode_models)
	LOCAL_SOURCE(bjt_models)
	LOCAL_SOURCE(family_models)
	LOCAL_SOURCE(TTL74XX_lib)
	LOCAL_SOURCE(CD4XXX_lib)
	LOCAL_SOURCE(OPAMP_lib)
	LOCAL_SOURCE(otheric_lib)

	INCLUDE(diode_models);
	INCLUDE(bjt_models);
	INCLUDE(family_models);
	INCLUDE(TTL74XX_lib);
	INCLUDE(CD4XXX_lib);
	INCLUDE(OPAMP_lib);
	INCLUDE(otheric_lib);

NETLIST_END()


// ----------------------------------------------------------------------------------------
// setup_t
// ----------------------------------------------------------------------------------------

namespace netlist
{
setup_t::setup_t(netlist_t &netlist)
	: m_netlist(netlist)
	, m_factory(*this)
	, m_proxy_cnt(0)
	, m_frontier_cnt(0)
{
	netlist.set_setup(this);
	initialize_factory(m_factory);
	NETLIST_NAME(base)(*this);
}

setup_t::~setup_t()
{
	m_links.clear();
	m_alias.clear();
	m_params.clear();
	m_terminals.clear();
	m_param_values.clear();

	netlist().set_setup(nullptr);
	m_sources.clear();

	pstring::resetmem();
}

pstring setup_t::build_fqn(const pstring &obj_name) const
{
	if (m_namespace_stack.empty())
		//return netlist().name() + "." + obj_name;
		return obj_name;
	else
		return m_namespace_stack.top() + "." + obj_name;
}

void setup_t::namespace_push(const pstring &aname)
{
	if (m_namespace_stack.empty())
		//m_namespace_stack.push(netlist().name() + "." + aname);
		m_namespace_stack.push(aname);
	else
		m_namespace_stack.push(m_namespace_stack.top() + "." + aname);
}

void setup_t::namespace_pop()
{
	m_namespace_stack.pop();
}

void setup_t::register_lib_entry(const pstring &name)
{
	if (plib::container::contains(m_lib, name))
		log().warning("Lib entry collection already contains {1}. IGNORED", name);
	else
		m_lib.push_back(name);
}

void setup_t::register_dev(const pstring &classname, const pstring &name)
{
	if (plib::container::contains(m_lib, classname))
	{
		namespace_push(name);
		include(classname);
		namespace_pop();
	}
	else
	{
		auto f = factory().factory_by_name(classname);
		if (f == nullptr)
			log().fatal("Class {1} not found!\n", classname);
		m_device_factory.push_back(std::pair<pstring, base_factory_t *>(build_fqn(name), f));
	}
}

bool setup_t::device_exists(const pstring name) const
{
	for (auto e : m_device_factory)
	{
		if (e.first == name)
			return true;
	}
	return false;
}


void setup_t::register_model(const pstring &model_in)
{
	auto pos = model_in.find(" ");
	if (pos == model_in.end())
		log().fatal("Unable to parse model: {1}", model_in);
	pstring model = model_in.left(pos).trim().ucase();
	pstring def = model_in.substr(pos + 1).trim();
	if (!m_models.insert({model, def}).second)
		log().fatal("Model already exists: {1}", model_in);
}

void setup_t::register_alias_nofqn(const pstring &alias, const pstring &out)
{
	if (!m_alias.insert({alias, out}).second)
		log().fatal("Error adding alias {1} to alias list\n", alias);
}

void setup_t::register_alias(const pstring &alias, const pstring &out)
{
	pstring alias_fqn = build_fqn(alias);
	pstring out_fqn = build_fqn(out);
	register_alias_nofqn(alias_fqn, out_fqn);
}

void setup_t::register_dippins_arr(const pstring &terms)
{
	plib::pstring_vector_t list(terms,", ");
	if (list.size() == 0 || (list.size() % 2) == 1)
		log().fatal("You must pass an equal number of pins to DIPPINS");
	std::size_t n = list.size();
	for (std::size_t i = 0; i < n / 2; i++)
	{
		register_alias(plib::pfmt("{1}")(i+1), list[i * 2]);
		register_alias(plib::pfmt("{1}")(n-i), list[i * 2 + 1]);
	}
}

pstring setup_t::objtype_as_str(detail::device_object_t &in) const
{
	switch (in.type())
	{
		case terminal_t::TERMINAL:
			return "TERMINAL";
		case terminal_t::INPUT:
			return "INPUT";
		case terminal_t::OUTPUT:
			return "OUTPUT";
		case terminal_t::PARAM:
			return "PARAM";
	}
	// FIXME: noreturn
	log().fatal("Unknown object type {1}\n", static_cast<unsigned>(in.type()));
	return "Error";
}

void setup_t::register_and_set_param(pstring name, param_t &param)
{
	auto i = m_param_values.find(name);
	if (i != m_param_values.end())
	{
		const pstring val = i->second;
		log().debug("Found parameter ... {1} : {1}\n", name, val);
		switch (param.param_type())
		{
			case param_t::DOUBLE:
			{
				double vald = 0;
				if (sscanf(val.cstr(), "%lf", &vald) != 1)
					log().fatal("Invalid number conversion {1} : {2}\n", name, val);
				static_cast<param_double_t &>(param).initial(vald);
			}
			break;
			case param_t::INTEGER:
			case param_t::LOGIC:
			{
				double vald = 0;
				if (sscanf(val.cstr(), "%lf", &vald) != 1)
					log().fatal("Invalid number conversion {1} : {2}\n", name, val);
				static_cast<param_int_t &>(param).initial(static_cast<int>(vald));
			}
			break;
			case param_t::STRING:
			case param_t::MODEL:
			{
				static_cast<param_str_t &>(param).initial(val);
			}
			break;
			//default:
			//  log().fatal("Parameter is not supported {1} : {2}\n", name, val);
		}
	}
	if (!m_params.insert({param.name(), param_ref_t(param.name(), param.device(), param)}).second)
		log().fatal("Error adding parameter {1} to parameter list\n", name);
}

void setup_t::register_term(detail::core_terminal_t &term)
{
	if (!m_terminals.insert({term.name(), &term}).second)
		log().fatal("Error adding {1} {2} to terminal list\n", objtype_as_str(term), term.name());
	log().debug("{1} {2}\n", objtype_as_str(term), term.name());
}

void setup_t::register_link_arr(const pstring &terms)
{
	plib::pstring_vector_t list(terms,", ");
	if (list.size() < 2)
		log().fatal("You must pass at least 2 terminals to NET_C");
	for (std::size_t i = 1; i < list.size(); i++)
	{
		register_link(list[0], list[i]);
	}
}


void setup_t::register_link_fqn(const pstring &sin, const pstring &sout)
{
	link_t temp = link_t(sin, sout);
	log().debug("link {1} <== {2}\n", sin, sout);
	m_links.push_back(temp);
}

void setup_t::register_link(const pstring &sin, const pstring &sout)
{
	register_link_fqn(build_fqn(sin), build_fqn(sout));
}

void setup_t::remove_connections(const pstring pin)
{
	pstring pinfn = build_fqn(pin);
	bool found = false;

	for (auto link = m_links.begin(); link != m_links.end(); )
	{
		if ((link->first == pinfn) || (link->second == pinfn))
		{
			log().verbose("removing connection: {1} <==> {2}\n", link->first, link->second);
			link = m_links.erase(link);
			found = true;
		}
		else
			link++;
	}
	if (!found)
		log().fatal("remove_connections: found no occurrence of {1}\n", pin);
}


void setup_t::register_frontier(const pstring attach, const double r_IN, const double r_OUT)
{
	pstring frontier_name = plib::pfmt("frontier_{1}")(m_frontier_cnt);
	m_frontier_cnt++;
	register_dev("FRONTIER_DEV", frontier_name);
	register_param(frontier_name + ".RIN", r_IN);
	register_param(frontier_name + ".ROUT", r_OUT);
	register_link(frontier_name + ".G", "GND");
	pstring attfn = build_fqn(attach);
	pstring front_fqn = build_fqn(frontier_name);
	bool found = false;
	for (auto & link  : m_links)
	{
		if (link.first == attfn)
		{
			link.first = front_fqn + ".I";
			found = true;
		}
		else if (link.second == attfn)
		{
			link.second = front_fqn + ".I";
			found = true;
		}
	}
	if (!found)
		log().fatal("Frontier setup: found no occurrence of {1}\n", attach);
	register_link(attach, frontier_name + ".Q");
}


void setup_t::register_param(const pstring &param, const double value)
{
	register_param(param, plib::pfmt("{1}").e(value,".9"));
}

void setup_t::register_param(const pstring &param, const pstring &value)
{
	pstring fqn = build_fqn(param);

	auto idx = m_param_values.find(fqn);
	if (idx == m_param_values.end())
	{
		if (!m_param_values.insert({fqn, value}).second)
			log().fatal("Unexpected error adding parameter {1} to parameter list\n", param);
	}
	else
	{
		log().warning("Overwriting {1} old <{2}> new <{3}>\n", fqn, idx->second, value);
		m_param_values[fqn] = value;
	}
}

const pstring setup_t::resolve_alias(const pstring &name) const
{
	pstring temp = name;
	pstring ret;

	/* FIXME: Detect endless loop */
	do {
		ret = temp;
		auto p = m_alias.find(ret);
		temp = (p != m_alias.end() ? p->second : "");
	} while (temp != "");

	log().debug("{1}==>{2}\n", name, ret);
	return ret;
}

detail::core_terminal_t *setup_t::find_terminal(const pstring &terminal_in, bool required)
{
	const pstring &tname = resolve_alias(terminal_in);
	auto ret = m_terminals.find(tname);
	/* look for default */
	if (ret == m_terminals.end())
	{
		/* look for ".Q" std output */
		ret = m_terminals.find(tname + ".Q");
	}

	detail::core_terminal_t *term = (ret == m_terminals.end() ? nullptr : ret->second);

	if (term == nullptr && required)
		log().fatal("terminal {1}({2}) not found!\n", terminal_in, tname);
	if (term != nullptr)
		log().debug("Found input {1}\n", tname);
	return term;
}

detail::core_terminal_t *setup_t::find_terminal(const pstring &terminal_in,
		detail::device_object_t::type_t atype, bool required)
{
	const pstring &tname = resolve_alias(terminal_in);
	auto ret = m_terminals.find(tname);
	/* look for default */
	if (ret == m_terminals.end() && atype == detail::device_object_t::OUTPUT)
	{
		/* look for ".Q" std output */
		ret = m_terminals.find(tname + ".Q");
	}
	if (ret == m_terminals.end() && required)
		log().fatal("terminal {1}({2}) not found!\n", terminal_in, tname);

	detail::core_terminal_t *term = (ret == m_terminals.end() ? nullptr : ret->second);

	if (term != nullptr && term->type() != atype)
	{
		if (required)
			log().fatal("object {1}({2}) found but wrong type\n", terminal_in, tname);
		else
			term = nullptr;
	}
	if (term != nullptr)
		log().debug("Found input {1}\n", tname);

	return term;
}

param_t *setup_t::find_param(const pstring &param_in, bool required) const
{
	const pstring param_in_fqn = build_fqn(param_in);

	const pstring &outname = resolve_alias(param_in_fqn);
	auto ret = m_params.find(outname);
	if (ret == m_params.end() && required)
		log().fatal("parameter {1}({2}) not found!\n", param_in_fqn, outname);
	if (ret != m_params.end())
		log().debug("Found parameter {1}\n", outname);
	return (ret == m_params.end() ? nullptr : &ret->second.m_param);
}

// FIXME avoid dynamic cast here
devices::nld_base_proxy *setup_t::get_d_a_proxy(detail::core_terminal_t &out)
{
	nl_assert(out.is_logic());

	logic_output_t &out_cast = dynamic_cast<logic_output_t &>(out);
	devices::nld_base_proxy *proxy = out_cast.get_proxy();

	if (proxy == nullptr)
	{
		// create a new one ...
		pstring x = plib::pfmt("proxy_da_{1}_{2}")(out.name())(m_proxy_cnt);
		auto new_proxy =
				out_cast.logic_family()->create_d_a_proxy(netlist(), x, &out_cast);
		m_proxy_cnt++;

		//new_proxy->start_dev();

		/* connect all existing terminals to new net */

		for (auto & p : out.net().m_core_terms)
		{
			p->clear_net(); // de-link from all nets ...
			if (!connect(new_proxy->proxy_term(), *p))
				log().fatal("Error connecting {1} to {2}\n", new_proxy->proxy_term().name(), (*p).name());
		}
		out.net().m_core_terms.clear(); // clear the list

		out.net().register_con(new_proxy->in());
		out_cast.set_proxy(proxy);

		proxy = new_proxy.get();

		netlist().register_dev(std::move(new_proxy));
	}
	return proxy;
}

void setup_t::merge_nets(detail::net_t &thisnet, detail::net_t &othernet)
{
	netlist().log().debug("merging nets ...\n");
	if (&othernet == &thisnet)
	{
		netlist().log().warning("Connecting {1} to itself. This may be right, though\n", thisnet.name());
		return; // Nothing to do
	}

	if (thisnet.isRailNet() && othernet.isRailNet())
		netlist().log().fatal("Trying to merge two rail nets: {1} and {2}\n", thisnet.name(), othernet.name());

	if (othernet.isRailNet())
	{
		netlist().log().debug("othernet is railnet\n");
		merge_nets(othernet, thisnet);
	}
	else
	{
		othernet.move_connections(thisnet);
	}
}



void setup_t::connect_input_output(detail::core_terminal_t &in, detail::core_terminal_t &out)
{
	if (out.is_analog() && in.is_logic())
	{
		logic_input_t &incast = dynamic_cast<logic_input_t &>(in);
		pstring x = plib::pfmt("proxy_ad_{1}_{2}")(in.name())( m_proxy_cnt);
		auto proxy = plib::owned_ptr<devices::nld_a_to_d_proxy>::Create(netlist(), x, &incast);
		incast.set_proxy(proxy.get());
		m_proxy_cnt++;

		proxy->m_Q.net().register_con(in);
		out.net().register_con(proxy->m_I);

		netlist().register_dev(std::move(proxy));

	}
	else if (out.is_logic() && in.is_analog())
	{
		devices::nld_base_proxy *proxy = get_d_a_proxy(out);

		connect_terminals(proxy->proxy_term(), in);
		//proxy->out().net().register_con(in);
	}
	else
	{
		if (in.has_net())
			merge_nets(out.net(), in.net());
		else
			out.net().register_con(in);
	}
}


void setup_t::connect_terminal_input(terminal_t &term, detail::core_terminal_t &inp)
{
	if (inp.is_analog())
	{
		connect_terminals(inp, term);
	}
	else if (inp.is_logic())
	{
		logic_input_t &incast = dynamic_cast<logic_input_t &>(inp);
		log().debug("connect_terminal_input: connecting proxy\n");
		pstring x = plib::pfmt("proxy_ad_{1}_{2}")(inp.name())(m_proxy_cnt);
		auto proxy = plib::owned_ptr<devices::nld_a_to_d_proxy>::Create(netlist(), x, &incast);
		incast.set_proxy(proxy.get());
		m_proxy_cnt++;

		connect_terminals(term, proxy->m_I);

		if (inp.has_net())
			//fatalerror("logic inputs can only belong to one net!\n");
			merge_nets(proxy->m_Q.net(), inp.net());
		else
			proxy->m_Q.net().register_con(inp);

		netlist().register_dev(std::move(proxy));
	}
	else
	{
		log().fatal("Netlist: Severe Error");
	}
}

void setup_t::connect_terminal_output(terminal_t &in, detail::core_terminal_t &out)
{
	if (out.is_analog())
	{
		log().debug("connect_terminal_output: {1} {2}\n", in.name(), out.name());
		/* no proxy needed, just merge existing terminal net */
		if (in.has_net())
			merge_nets(out.net(), in.net());
		else
			out.net().register_con(in);
	}
	else if (out.is_logic())
	{
		log().debug("connect_terminal_output: connecting proxy\n");
		devices::nld_base_proxy *proxy = get_d_a_proxy(out);

		connect_terminals(proxy->proxy_term(), in);
	}
	else
	{
		log().fatal("Netlist: Severe Error");
	}
}

void setup_t::connect_terminals(detail::core_terminal_t &t1, detail::core_terminal_t &t2)
{
	if (t1.has_net() && t2.has_net())
	{
		log().debug("T2 and T1 have net\n");
		merge_nets(t1.net(), t2.net());
	}
	else if (t2.has_net())
	{
		log().debug("T2 has net\n");
		t2.net().register_con(t1);
	}
	else if (t1.has_net())
	{
		log().debug("T1 has net\n");
		t1.net().register_con(t2);
	}
	else
	{
		log().debug("adding analog net ...\n");
		// FIXME: Nets should have a unique name
		auto anet = plib::palloc<analog_net_t>(netlist(),"net." + t1.name());
		t1.set_net(anet);
		anet->register_con(t2);
		anet->register_con(t1);
	}
}

static detail::core_terminal_t &resolve_proxy(detail::core_terminal_t &term)
{
	if (term.is_logic())
	{
		logic_t &out = dynamic_cast<logic_t &>(term);
		if (out.has_proxy())
			return out.get_proxy()->proxy_term();
	}
	return term;
}

bool setup_t::connect_input_input(detail::core_terminal_t &t1, detail::core_terminal_t &t2)
{
	bool ret = false;
	if (t1.has_net())
	{
		if (t1.net().isRailNet())
			ret = connect(t2, t1.net().railterminal());
		if (!ret)
		{
			for (auto & t : t1.net().m_core_terms)
			{
				if (t->is_type(detail::core_terminal_t::TERMINAL))
					ret = connect(t2, *t);
				if (ret)
					break;
			}
		}
	}
	if (!ret && t2.has_net())
	{
		if (t2.net().isRailNet())
			ret = connect(t1, t2.net().railterminal());
		if (!ret)
		{
			for (auto & t : t2.net().m_core_terms)
			{
				if (t->is_type(detail::core_terminal_t::TERMINAL))
					ret = connect(t1, *t);
				if (ret)
					break;
			}
		}
	}
	return ret;
}



bool setup_t::connect(detail::core_terminal_t &t1_in, detail::core_terminal_t &t2_in)
{
	log().debug("Connecting {1} to {2}\n", t1_in.name(), t2_in.name());
	detail::core_terminal_t &t1 = resolve_proxy(t1_in);
	detail::core_terminal_t &t2 = resolve_proxy(t2_in);
	bool ret = true;

	if (t1.is_type(detail::core_terminal_t::OUTPUT) && t2.is_type(detail::core_terminal_t::INPUT))
	{
		if (t2.has_net() && t2.net().isRailNet())
			log().fatal("Input {1} already connected\n", t2.name());
		connect_input_output(t2, t1);
	}
	else if (t1.is_type(detail::core_terminal_t::INPUT) && t2.is_type(detail::core_terminal_t::OUTPUT))
	{
		if (t1.has_net()  && t1.net().isRailNet())
			log().fatal("Input {1} already connected\n", t1.name());
		connect_input_output(t1, t2);
	}
	else if (t1.is_type(detail::core_terminal_t::OUTPUT) && t2.is_type(detail::core_terminal_t::TERMINAL))
	{
		connect_terminal_output(dynamic_cast<terminal_t &>(t2), t1);
	}
	else if (t1.is_type(detail::core_terminal_t::TERMINAL) && t2.is_type(detail::core_terminal_t::OUTPUT))
	{
		connect_terminal_output(dynamic_cast<terminal_t &>(t1), t2);
	}
	else if (t1.is_type(detail::core_terminal_t::INPUT) && t2.is_type(detail::core_terminal_t::TERMINAL))
	{
		connect_terminal_input(dynamic_cast<terminal_t &>(t2), t1);
	}
	else if (t1.is_type(detail::core_terminal_t::TERMINAL) && t2.is_type(detail::core_terminal_t::INPUT))
	{
		connect_terminal_input(dynamic_cast<terminal_t &>(t1), t2);
	}
	else if (t1.is_type(detail::core_terminal_t::TERMINAL) && t2.is_type(detail::core_terminal_t::TERMINAL))
	{
		connect_terminals(dynamic_cast<terminal_t &>(t1), dynamic_cast<terminal_t &>(t2));
	}
	else if (t1.is_type(detail::core_terminal_t::INPUT) && t2.is_type(detail::core_terminal_t::INPUT))
	{
		ret = connect_input_input(t1, t2);
	}
	else
		ret = false;
		//netlist().error("Connecting {1} to {2} not supported!\n", t1.name(), t2.name());
	return ret;
}

void setup_t::resolve_inputs()
{
	bool has_twoterms = false;

	log().verbose("Resolving inputs ...");

	/* Netlist can directly connect input to input.
	 * We therefore first park connecting inputs and retry
	 * after all other terminals were connected.
	 */
	int tries = 100;
	while (m_links.size() > 0 && tries >  0) // FIXME: convert into constant
	{

		for (auto li = m_links.begin(); li != m_links.end(); )
		{
			const pstring t1s = li->first;
			const pstring t2s = li->second;
			detail::core_terminal_t *t1 = find_terminal(t1s);
			detail::core_terminal_t *t2 = find_terminal(t2s);

			if (connect(*t1, *t2))
				li = m_links.erase(li);
			else
				li++;
		}
		tries--;
	}
	if (tries == 0)
	{
		for (auto & link : m_links)
			log().warning("Error connecting {1} to {2}\n", link.first, link.second);

		log().fatal("Error connecting -- bailing out\n");
	}

	log().verbose("deleting empty nets ...");

	// delete empty nets

	netlist().m_nets.erase(
			std::remove_if(netlist().m_nets.begin(), netlist().m_nets.end(),
					[](plib::owned_ptr<detail::net_t> &x)
					{
						if (x->num_cons() == 0)
						{
							x->netlist().log().verbose("Deleting net {1} ...", x->name());
							return true;
						}
						else
							return false;
					}), netlist().m_nets.end());

	pstring errstr("");

	log().verbose("looking for terminals not connected ...");
	for (auto & i : m_terminals)
	{
		detail::core_terminal_t *term = i.second;
		if (!term->has_net() && dynamic_cast< devices::NETLIB_NAME(dummy_input) *>(&term->device()) != nullptr)
			log().warning("Found dummy terminal {1} without connections", term->name());
		else if (!term->has_net())
			errstr += plib::pfmt("Found terminal {1} without a net\n")(term->name());
		else if (term->net().num_cons() == 0)
			log().warning("Found terminal {1} without connections", term->name());
	}
	if (errstr != "")
		log().fatal("{1}", errstr);


	log().verbose("looking for two terms connected to rail nets ...\n");
	for (auto & t : netlist().get_device_list<devices::NETLIB_NAME(twoterm)>())
	{
		has_twoterms = true;
		if (t->m_N.net().isRailNet() && t->m_P.net().isRailNet())
			log().warning("Found device {1} connected only to railterminals {2}/{3}\n",
				t->name(), t->m_N.net().name(), t->m_P.net().name());
	}

	log().verbose("initialize solver ...\n");

	if (netlist().solver() == nullptr)
	{
		if (has_twoterms)
			log().fatal("No solver found for this net although analog elements are present\n");
	}
	else
		netlist().solver()->post_start();

	/* finally, set the pointers */

	log().debug("Initializing devices ...\n");
	for (auto &dev : netlist().m_devices)
		dev->set_delegate_pointer();

}

void setup_t::start_devices()
{
	pstring env = plib::util::environment("NL_LOGS");

	if (env != "")
	{
		log().debug("Creating dynamic logs ...\n");
		plib::pstring_vector_t loglist(env, ":");
		for (pstring ll : loglist)
		{
			pstring name = "log_" + ll;
			auto nc = factory().factory_by_name("LOG")->Create(netlist(), name);
			register_link(name + ".I", ll);
			log().debug("    dynamic link {1}: <{2}>\n",ll, name);
			netlist().register_dev(std::move(nc));
		}
	}

	netlist().start();
}

plib::plog_base<NL_DEBUG> &setup_t::log()
{
	return netlist().log();
}
const plib::plog_base<NL_DEBUG> &setup_t::log() const
{
	return netlist().log();
}


// ----------------------------------------------------------------------------------------
// Model / family
// ----------------------------------------------------------------------------------------

class logic_family_std_proxy_t : public logic_family_desc_t
{
public:
	logic_family_std_proxy_t() { }
	virtual plib::owned_ptr<devices::nld_base_d_to_a_proxy> create_d_a_proxy(netlist_t &anetlist,
			const pstring &name, logic_output_t *proxied) const override
	{
		return plib::owned_ptr<devices::nld_base_d_to_a_proxy>::Create<devices::nld_d_to_a_proxy>(anetlist, name, proxied);
	}
};

const logic_family_desc_t *setup_t::family_from_model(const pstring &model)
{
	model_map_t map;
	model_parse(model, map);

	if (setup_t::model_value_str(map, "TYPE") == "TTL")
		return family_TTL();
	if (setup_t::model_value_str(map, "TYPE") == "CD4XXX")
		return family_CD4XXX();

	for (auto & e : netlist().m_family_cache)
		if (e.first == model)
			return e.second.get();

	auto ret = plib::make_unique_base<logic_family_desc_t, logic_family_std_proxy_t>();

	ret->m_low_thresh_V = setup_t::model_value(map, "IVL");
	ret->m_high_thresh_V = setup_t::model_value(map, "IVH");
	ret->m_low_V = setup_t::model_value(map, "OVL");
	ret->m_high_V = setup_t::model_value(map, "OVH");
	ret->m_R_low = setup_t::model_value(map, "ORL");
	ret->m_R_high = setup_t::model_value(map, "ORH");

	auto retp = ret.get();

	netlist().m_family_cache.emplace_back(model, std::move(ret));

	return retp;
}

static pstring model_string(model_map_t &map)
{
	pstring ret = map["COREMODEL"] + "(";
	for (auto & i : map)
		ret = ret + i.first + "=" + i.second + " ";

	return ret + ")";
}

void setup_t::model_parse(const pstring &model_in, model_map_t &map)
{
	pstring model = model_in;
	pstring::iterator pos(nullptr);
	pstring key;

	while (true)
	{
		pos = model.find("(");
		if (pos != model.end()) break;

		key = model.ucase();
		auto i = m_models.find(key);
		if (i == m_models.end())
			log().fatal("Model {1} not found\n", model);
		model = i->second;
	}
	pstring xmodel = model.left(pos);

	if (xmodel.equals("_"))
		map["COREMODEL"] = key;
	else
	{
		auto i = m_models.find(xmodel);
		if (i != m_models.end())
			model_parse(xmodel, map);
		else
			log().fatal("Model doesn't exist: <{1}>\n", model_in);
	}

	pstring remainder=model.substr(pos+1).trim();
	if (!remainder.endsWith(")"))
		log().fatal("Model error {1}\n", model);
	// FIMXE: Not optimal
	remainder = remainder.left(remainder.begin() + (remainder.len() - 1));

	plib::pstring_vector_t pairs(remainder," ", true);
	for (pstring &pe : pairs)
	{
		auto pose = pe.find("=");
		if (pose == pe.end())
			log().fatal("Model error on pair {1}\n", model);
		map[pe.left(pose).ucase()] = pe.substr(pose+1);
	}
}

const pstring setup_t::model_value_str(model_map_t &map, const pstring &entity)
{
	pstring ret;

	if (entity != entity.ucase())
		log().fatal("model parameters should be uppercase:{1} {2}\n", entity, model_string(map));
	if (map.find(entity) == map.end())
		log().fatal("Entity {1} not found in model {2}\n", entity, model_string(map));
	else
		ret = map[entity];

	return ret;
}

nl_double setup_t::model_value(model_map_t &map, const pstring &entity)
{
	pstring tmp = model_value_str(map, entity);

	nl_double factor = NL_FCONST(1.0);
	auto p = tmp.begin() + (tmp.len() - 1);
	switch (*p)
	{
		case 'M': factor = 1e6; break;
		case 'k': factor = 1e3; break;
		case 'm': factor = 1e-3; break;
		case 'u': factor = 1e-6; break;
		case 'n': factor = 1e-9; break;
		case 'p': factor = 1e-12; break;
		case 'f': factor = 1e-15; break;
		case 'a': factor = 1e-18; break;
		default:
			if (*p < '0' || *p > '9')
				nl_exception(plib::pfmt("Unknown number factor in: {1}")(entity));
	}
	if (factor != NL_FCONST(1.0))
		tmp = tmp.left(tmp.begin() + (tmp.len() - 1));
	return tmp.as_double() * factor;
}

void setup_t::tt_factory_create(tt_desc &desc)
{
	devices::tt_factory_create(*this, desc);
}


// ----------------------------------------------------------------------------------------
// Sources
// ----------------------------------------------------------------------------------------

void setup_t::include(const pstring &netlist_name)
{
	for (auto &source : m_sources)
	{
		if (source->parse(*this, netlist_name))
			return;
	}
	log().fatal("unable to find {1} in source collection", netlist_name);
}

bool setup_t::parse_stream(plib::pistream &istrm, const pstring &name)
{
	plib::pomemstream ostrm;

	plib::pimemstream istrm2(plib::ppreprocessor(&m_defines).process(istrm, ostrm));
	return parser_t(istrm2, *this).parse(name);
}

void setup_t::register_define(pstring defstr)
{
	auto p = defstr.find("=");
	if (p != defstr.end())
		register_define(defstr.left(p), defstr.substr(p+1));
	else
		register_define(defstr, "1");
}

// ----------------------------------------------------------------------------------------
// base sources
// ----------------------------------------------------------------------------------------

bool source_string_t::parse(setup_t &setup, const pstring &name)
{
	plib::pimemstream istrm(m_str.cstr(), m_str.len());
	return setup.parse_stream(istrm, name);
}

bool source_mem_t::parse(setup_t &setup, const pstring &name)
{
	plib::pimemstream istrm(m_str.cstr(), m_str.len());
	return setup.parse_stream(istrm, name);
}

bool source_file_t::parse(setup_t &setup, const pstring &name)
{
	plib::pifilestream istrm(m_filename);
	return setup.parse_stream(istrm, name);
}

}
