// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

#include "emu.h"
#include "emumem_hep.h"
#include "emumem_het.h"

template<int Width, int AddrShift, int Endian> typename emu::detail::handler_entry_size<Width>::uX handler_entry_read_tap<Width, AddrShift, Endian>::read(offs_t offset, uX mem_mask)
{
	uX data = inh::m_next->read(offset, mem_mask);
	m_tap(offset, data, mem_mask);
	return data;
}

template<int Width, int AddrShift, int Endian> std::string handler_entry_read_tap<Width, AddrShift, Endian>::name() const
{
	return '(' + m_name + ") " + inh::m_next->name();
}

template<int Width, int AddrShift, int Endian> handler_entry_read_tap<Width, AddrShift, Endian> *handler_entry_read_tap<Width, AddrShift, Endian>::instantiate(handler_entry_read<Width, AddrShift, Endian> *next) const
{
	return new handler_entry_read_tap<Width, AddrShift, Endian>(inh::m_space, inh::m_mph, next, m_name, m_tap);
}


template<int Width, int AddrShift, int Endian> void handler_entry_write_tap<Width, AddrShift, Endian>::write(offs_t offset, uX data, uX mem_mask)
{
	m_tap(offset, data, mem_mask);
	inh::m_next->write(offset, data, mem_mask);
}

template<int Width, int AddrShift, int Endian> std::string handler_entry_write_tap<Width, AddrShift, Endian>::name() const
{
	return '(' + m_name + ") " + inh::m_next->name();
}


template<int Width, int AddrShift, int Endian> handler_entry_write_tap<Width, AddrShift, Endian> *handler_entry_write_tap<Width, AddrShift, Endian>::instantiate(handler_entry_write<Width, AddrShift, Endian> *next) const
{
	return new handler_entry_write_tap<Width, AddrShift, Endian>(inh::m_space, inh::m_mph, next, m_name, m_tap);
}



template class handler_entry_read_tap<0,  1, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<0,  1, ENDIANNESS_BIG>;
template class handler_entry_read_tap<0,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<0,  0, ENDIANNESS_BIG>;
template class handler_entry_read_tap<1,  3, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<1,  3, ENDIANNESS_BIG>;
template class handler_entry_read_tap<1,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<1,  0, ENDIANNESS_BIG>;
template class handler_entry_read_tap<1, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<1, -1, ENDIANNESS_BIG>;
template class handler_entry_read_tap<2,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<2,  0, ENDIANNESS_BIG>;
template class handler_entry_read_tap<2, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<2, -1, ENDIANNESS_BIG>;
template class handler_entry_read_tap<2, -2, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<2, -2, ENDIANNESS_BIG>;
template class handler_entry_read_tap<3,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<3,  0, ENDIANNESS_BIG>;
template class handler_entry_read_tap<3, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<3, -1, ENDIANNESS_BIG>;
template class handler_entry_read_tap<3, -2, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<3, -2, ENDIANNESS_BIG>;
template class handler_entry_read_tap<3, -3, ENDIANNESS_LITTLE>;
template class handler_entry_read_tap<3, -3, ENDIANNESS_BIG>;

template class handler_entry_write_tap<0,  1, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<0,  1, ENDIANNESS_BIG>;
template class handler_entry_write_tap<0,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<0,  0, ENDIANNESS_BIG>;
template class handler_entry_write_tap<1,  3, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<1,  3, ENDIANNESS_BIG>;
template class handler_entry_write_tap<1,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<1,  0, ENDIANNESS_BIG>;
template class handler_entry_write_tap<1, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<1, -1, ENDIANNESS_BIG>;
template class handler_entry_write_tap<2,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<2,  0, ENDIANNESS_BIG>;
template class handler_entry_write_tap<2, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<2, -1, ENDIANNESS_BIG>;
template class handler_entry_write_tap<2, -2, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<2, -2, ENDIANNESS_BIG>;
template class handler_entry_write_tap<3,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<3,  0, ENDIANNESS_BIG>;
template class handler_entry_write_tap<3, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<3, -1, ENDIANNESS_BIG>;
template class handler_entry_write_tap<3, -2, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<3, -2, ENDIANNESS_BIG>;
template class handler_entry_write_tap<3, -3, ENDIANNESS_LITTLE>;
template class handler_entry_write_tap<3, -3, ENDIANNESS_BIG>;
