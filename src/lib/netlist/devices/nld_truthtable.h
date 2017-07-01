// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_truthtable.h
 *
 *  Created on: 19 Jun 2014
 *      Author: andre
 */

#ifndef NLD_TRUTHTABLE_H_
#define NLD_TRUTHTABLE_H_

#include <new>
#include <cstdint>

#include "nl_setup.h"
#include "nl_factory.h"
#include "plib/plists.h"

#define NETLIB_TRUTHTABLE(cname, nIN, nOUT)                                     \
	class NETLIB_NAME(cname) : public nld_truthtable_t<nIN, nOUT>               \
	{                                                                           \
	public:                                                                     \
		template <class C>                                                      \
		NETLIB_NAME(cname)(C &owner, const pstring &name)                       \
		: nld_truthtable_t<nIN, nOUT>(owner, name, nullptr, &m_ttbl, m_desc) { }   \
	private:                                                                    \
		static truthtable_t m_ttbl;                                             \
		static const char *m_desc[];                                            \
	}


namespace netlist
{
	namespace devices
	{
	template<unsigned bits>
	struct need_bytes_for_bits
	{
		enum { value =
			bits <= 8       ?  1 :
			bits <= 16      ?  2 :
			bits <= 32      ?  4 :
								8
		};
	};

	template<unsigned bits> struct uint_for_size;
	template<> struct uint_for_size<1> { typedef uint_least8_t type; };
	template<> struct uint_for_size<2> { typedef uint_least16_t type; };
	template<> struct uint_for_size<4> { typedef uint_least32_t type; };
	template<> struct uint_for_size<8> { typedef uint_least64_t type; };

	struct packed_int
	{
		template<typename C>
		packed_int(C *data)
		: m_data(data)
		, m_size(sizeof(C))
		{}

		void set(const size_t pos, const uint_least64_t val)
		{
			switch (m_size)
			{
				case 1: static_cast<uint_least8_t  *>(m_data)[pos] = static_cast<uint_least8_t>(val); break;
				case 2: static_cast<uint_least16_t *>(m_data)[pos] = static_cast<uint_least16_t>(val); break;
				case 4: static_cast<uint_least32_t *>(m_data)[pos] = static_cast<uint_least32_t>(val); break;
				case 8: static_cast<uint_least64_t *>(m_data)[pos] = static_cast<uint_least64_t>(val); break;
				default: { }
			}
		}

		uint_least64_t operator[] (size_t pos) const
		{
			switch (m_size)
			{
				case 1: return static_cast<uint_least8_t  *>(m_data)[pos]; break;
				case 2: return static_cast<uint_least16_t *>(m_data)[pos]; break;
				case 4: return static_cast<uint_least32_t *>(m_data)[pos]; break;
				case 8: return static_cast<uint_least64_t *>(m_data)[pos]; break;
				default:
					return 0; //should never happen
			}
		}

		uint_least64_t adjust(uint_least64_t val) const
		{
			switch (m_size)
			{
				case 1: return static_cast<uint_least8_t >(val); break;
				case 2: return static_cast<uint_least16_t>(val); break;
				case 4: return static_cast<uint_least32_t>(val); break;
				case 8: return static_cast<uint_least64_t>(val); break;
				default:
					return 0; //should never happen
			}
		}
	private:
		void *m_data;
		size_t m_size;
	};

	struct truthtable_desc_t
	{
		truthtable_desc_t(unsigned NO, unsigned NI, bool *initialized,
				packed_int outs, uint_least8_t *timing, netlist_time *timing_nt)
		: m_NO(NO), m_NI(NI),  m_initialized(initialized),
			m_outs(outs), m_timing(timing), m_timing_nt(timing_nt),
			m_num_bits(m_NI),
			m_size(1 << (m_num_bits))
		{
		}

		void setup(const plib::pstring_vector_t &desc, uint_least64_t disabled_ignore);

	private:
		void help(unsigned cur, plib::pstring_vector_t list,
				uint_least64_t state, uint_least64_t val, std::vector<uint_least8_t> &timing_index);
		static unsigned count_bits(uint_least64_t v);
		static uint_least64_t set_bits(uint_least64_t v, uint_least64_t b);
		uint_least64_t get_ignored_simple(uint_least64_t i);
		uint_least64_t get_ignored_extended(uint_least64_t i);

		unsigned m_NO;
		unsigned m_NI;
		bool *m_initialized;
		packed_int m_outs;
		uint_least8_t  *m_timing;
		netlist_time *m_timing_nt;

		/* additional values */

		const std::size_t m_num_bits;
		const std::size_t m_size;

	};

	template<unsigned m_NI, unsigned m_NO>
	NETLIB_OBJECT(truthtable_t)
	{
	private:
		detail::family_setter_t m_fam;
	public:

		static constexpr int m_num_bits = m_NI;
		static constexpr int m_size = (1 << (m_num_bits));

		struct truthtable_t
		{
			truthtable_t()
			: m_initialized(false)
			{}
			bool m_initialized;
			typename uint_for_size<need_bytes_for_bits<m_NO + m_NI>::value>::type m_outs[m_size];
			uint_least8_t m_timing[m_size * m_NO];
			netlist_time m_timing_nt[16];
		};

		template <class C>
		nld_truthtable_t(C &owner, const pstring &name, const logic_family_desc_t *fam,
				truthtable_t *ttp, const char *desc[])
		: device_t(owner, name)
		, m_fam(*this, fam)
		, m_ign(*this, "m_ign", 0)
		, m_active(*this, "m_active", 1)
		, m_ttp(ttp)
		{
			while (*desc != nullptr && **desc != 0 )
				{
					m_desc.push_back(*desc);
					desc++;
				}
			startxx();
		}

		template <class C>
		nld_truthtable_t(C &owner, const pstring &name, const logic_family_desc_t *fam,
				truthtable_t *ttp, const plib::pstring_vector_t &desc)
		: device_t(owner, name)
		, m_fam(*this, fam)
		, m_ign(*this, "m_ign", 0)
		, m_active(*this, "m_active", 1)
		, m_ttp(ttp)
		{
			m_desc = desc;
			startxx();
		}

		void startxx()
		{
			set_hint_deactivate(true);

			pstring header = m_desc[0];

			plib::pstring_vector_t io(header,"|");
			// checks
			nl_assert_always(io.size() == 2, "too many '|'");
			plib::pstring_vector_t inout(io[0], ",");
			nl_assert_always(inout.size() == m_num_bits, "bitcount wrong");
			plib::pstring_vector_t out(io[1], ",");
			nl_assert_always(out.size() == m_NO, "output count wrong");

			for (std::size_t i=0; i < m_NI; i++)
			{
				inout[i] = inout[i].trim();
				m_I.emplace(i, *this, inout[i]);
			}
			for (std::size_t i=0; i < m_NO; i++)
			{
				out[i] = out[i].trim();
				m_Q.emplace(i, *this, out[i]);
			}
			// Connect output "Q" to input "_Q" if this exists
			// This enables timed state without having explicit state ....
			uint_least64_t disabled_ignore = 0;
			for (std::size_t i=0; i < m_NO; i++)
			{
				pstring tmp = "_" + out[i];
				const std::size_t idx = plib::container::indexof(inout, tmp);
				if (idx != plib::container::npos)
				{
					connect_late(m_Q[i], m_I[idx]);
					// disable ignore for this inputs altogether.
					// FIXME: This shouldn't be necessary
					disabled_ignore |= (1<<idx);
				}
			}

			m_ign = 0;

			truthtable_desc_t desc(m_NO, m_NI, &m_ttp->m_initialized,
					packed_int(m_ttp->m_outs),
					m_ttp->m_timing, m_ttp->m_timing_nt);

			desc.setup(m_desc, disabled_ignore * 0);
	#if 0
			printf("%s\n", name().cstr());
			for (int j=0; j < m_size; j++)
				printf("%05x %04x %04x %04x\n", j, m_ttp->m_outs[j] & ((1 << m_NO)-1),
						m_ttp->m_outs[j] >> m_NO, m_ttp->m_timing[j * m_NO + 0]);
			for (int k=0; m_ttp->m_timing_nt[k] != netlist_time::zero(); k++)
				printf("%d %f\n", k, m_ttp->m_timing_nt[k].as_double() * 1000000.0);
	#endif
		}

		NETLIB_RESETI()
		{
			m_active = 0;
			m_ign = 0;
			for (std::size_t i = 0; i < m_NI; i++)
				m_I[i].activate();
			for (std::size_t i=0; i<m_NO;i++)
				if (this->m_Q[i].net().num_cons()>0)
					m_active++;
		}

		NETLIB_UPDATEI()
		{
			process<true>();
		}

	public:
		void inc_active() noexcept override
		{
			if (m_NI > 1)
				if (++m_active == 1)
				{
					process<false>();
				}
		}

		void dec_active() noexcept override
		{
			/* FIXME:
			 * Based on current measurements there is no point to disable
			 * 1 input devices. This should actually be a parameter so that we
			 * can decide for each individual gate whether it is beneficial to
			 * ignore deactivation.
			 */
			if (m_NI > 1)
				if (--m_active == 0)
				{
					for (std::size_t i = 0; i< m_NI; i++)
						m_I[i].inactivate();
					m_ign = (1<<m_NI)-1;
				}
		}

		//logic_input_t m_I[m_NI];
		//logic_output_t m_Q[m_NO];
		plib::uninitialised_array_t<logic_input_t, m_NI> m_I;
		plib::uninitialised_array_t<logic_output_t, m_NO> m_Q;

	protected:

	private:

		template<bool doOUT>
		inline void process()
		{
			netlist_time mt = netlist_time::zero();

			uint_least64_t state = 0;
			if (m_NI > 1)
			{
				auto ign = m_ign;
				if (!doOUT)
					for (std::size_t i = 0; i < m_NI; i++)
					{
						m_I[i].activate();
						state |= (m_I[i]() << i);
						mt = std::max(this->m_I[i].net().time(), mt);
					}
				else
					for (std::size_t i = 0; i < m_NI; ign >>= 1, i++)
					{
						if ((ign & 1))
							m_I[i].activate();
						state |= (m_I[i]() << i);
					}
			}
			else
			{
				if (!doOUT)
					for (std::size_t i = 0; i < m_NI; i++)
					{
						state |= (m_I[i]() << i);
						mt = std::max(this->m_I[i].net().time(), mt);
					}
				else
					for (std::size_t i = 0; i < m_NI; i++)
						state |= (m_I[i]() << i);
			}
			auto nstate = state;

			const auto outstate = m_ttp->m_outs[nstate];
			const auto out = outstate & ((1 << m_NO) - 1);

			m_ign = outstate >> m_NO;

			const auto timebase = nstate * m_NO;

			if (doOUT)
			{
				for (std::size_t i = 0; i < m_NO; i++)
					m_Q[i].push((out >> i) & 1, m_ttp->m_timing_nt[m_ttp->m_timing[timebase + i]]);
			}
			else
				for (std::size_t i = 0; i < m_NO; i++)
					m_Q[i].net().set_Q_time((out >> i) & 1, mt + m_ttp->m_timing_nt[m_ttp->m_timing[timebase + i]]);

			if (m_NI > 1)
			{
				auto ign(m_ign);
				for (std::size_t i = 0; ign != 0; ign >>= 1, i++)
					if (ign & 1)
						m_I[i].inactivate();
			}
		}

		/* FIXME: check width */
		state_var_u32       m_ign;
		state_var_s32       m_active;
		truthtable_t *      m_ttp;
		plib::pstring_vector_t m_desc;
	};

	class netlist_base_factory_truthtable_t : public base_factory_t
	{
		P_PREVENT_COPYING(netlist_base_factory_truthtable_t)
	public:
		netlist_base_factory_truthtable_t(const pstring &name, const pstring &classname,
				const pstring &def_param)
		: base_factory_t(name, classname, def_param), m_family(family_TTL())
		{}

		virtual ~netlist_base_factory_truthtable_t()
		{
		}

		plib::pstring_vector_t m_desc;
		const logic_family_desc_t *m_family;
	};

	void tt_factory_create(setup_t &setup, tt_desc &desc);

	} //namespace devices
} // namespace netlist



#endif /* NLD_TRUTHTABLE_H_ */
