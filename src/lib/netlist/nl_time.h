// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nltime.h
 */

#ifndef NLTIME_H_
#define NLTIME_H_

#include <cstdint>

#include "nl_config.h"
#include "plib/pstate.h"

//============================================================
//  MACROS
//============================================================

#define NLTIME_FROM_NS(t)  netlist_time::from_nsec(t)
#define NLTIME_FROM_US(t)  netlist_time::from_usec(t)
#define NLTIME_FROM_MS(t)  netlist_time::from_msec(t)
#define NLTIME_IMMEDIATE   netlist_time::from_nsec(1)

// ----------------------------------------------------------------------------------------
// netlist_time
// ----------------------------------------------------------------------------------------

namespace netlist
{
	template <typename TYPE, TYPE RES>
	struct ptime final
	{
	public:

		using internal_type = TYPE;
		using mult_type = std::uint_fast64_t;
		static constexpr internal_type resolution = RES;

		constexpr ptime() noexcept : m_time(0) {}
		constexpr ptime(const ptime &rhs) noexcept = default;
		constexpr ptime(ptime &&rhs) noexcept = default;

		constexpr explicit ptime(const double t) = delete;
		//: m_time((internal_type) ( t * (double) resolution)) { }
		constexpr explicit ptime(const internal_type nom, const internal_type den) noexcept
		: m_time(nom * (resolution / den)) { }

		ptime &operator=(const ptime rhs) { m_time = rhs.m_time; return *this; }

		ptime &operator+=(const ptime &rhs) { m_time += rhs.m_time; return *this; }
		ptime &operator-=(const ptime &rhs) { m_time -= rhs.m_time; return *this; }

		friend ptime operator-(ptime lhs, const ptime &rhs)
		{
			lhs -= rhs;
			return lhs;
		}

		friend ptime operator+(ptime lhs, const ptime &rhs)
		{
			lhs += rhs;
			return lhs;
		}

		friend ptime operator*(ptime lhs, const mult_type factor)
		{
			lhs.m_time *= static_cast<internal_type>(factor);
			return lhs;
		}

		friend mult_type operator/(const ptime &lhs, const ptime &rhs)
		{
			return static_cast<mult_type>(lhs.m_time / rhs.m_time);
		}

		friend bool operator<(const ptime &lhs, const ptime &rhs)
		{
			return (lhs.m_time < rhs.m_time);
		}

		friend bool operator>(const ptime &lhs, const ptime &rhs)
		{
			return (rhs < lhs);
		}

		friend bool operator<=(const ptime &lhs, const ptime &rhs)
		{
			return !(lhs > rhs);
		}

		friend bool operator>=(const ptime &lhs, const ptime &rhs)
		{
			return !(lhs < rhs);
		}

		friend bool operator==(const ptime &lhs, const ptime &rhs)
		{
			return lhs.m_time == rhs.m_time;
		}

		friend bool operator!=(const ptime &lhs, const ptime &rhs)
		{
			return !(lhs == rhs);
		}

		constexpr internal_type as_raw() const { return m_time; }
		constexpr double as_double() const { return static_cast<double>(m_time)
				/ static_cast<double>(resolution); }

		// for save states ....
		internal_type *get_internaltype_ptr() { return &m_time; }

		static constexpr ptime from_nsec(const internal_type ns) { return ptime(ns, UINT64_C(1000000000)); }
		static constexpr ptime from_usec(const internal_type us) { return ptime(us, UINT64_C(1000000)); }
		static constexpr ptime from_msec(const internal_type ms) { return ptime(ms, UINT64_C(1000)); }
		static constexpr ptime from_hz(const internal_type hz) { return ptime(1 , hz); }
		static constexpr ptime from_raw(const internal_type raw) { return ptime(raw, resolution); }
		static constexpr ptime from_double(const double t) { return ptime(static_cast<internal_type>( t * static_cast<double>(resolution)), resolution); }

		static constexpr ptime zero() { return ptime(0, resolution); }
		static constexpr ptime quantum() { return ptime(1, resolution); }
		static constexpr ptime never() { return ptime(plib::numeric_limits<internal_type>::max(), resolution); }
	private:
		internal_type m_time;
	};

#if (PHAS_INT128)
	using netlist_time = ptime<UINT128, NETLIST_INTERNAL_RES>;
#else
	using netlist_time = ptime<std::uint_fast64_t, NETLIST_INTERNAL_RES>;
#endif
}

namespace plib {
template<> inline void state_manager_t::save_item(const void *owner, netlist::netlist_time &nlt, const pstring &stname)
{
	save_state_ptr(owner, stname, datatype_t(sizeof(netlist::netlist_time::internal_type), false, true, false), 1, nlt.get_internaltype_ptr());
}
}

#endif /* NLTIME_H_ */
