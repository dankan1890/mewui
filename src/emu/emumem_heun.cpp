// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

#include "emu.h"
#include "emumem_hea.h"
#include "emumem_heun.h"

template<int Width, int AddrShift, int Endian> typename emu::detail::handler_entry_size<Width>::uX handler_entry_read_unmapped<Width, AddrShift, Endian>::read(offs_t offset, uX mem_mask)
{
	if (inh::m_space->log_unmap() && !inh::m_space->m_manager.machine().side_effects_disabled())
		inh::m_space->device().logerror(inh::m_space->is_octal()
										? "%s: unmapped %s memory read from %0*o & %0*o\n"
										: "%s: unmapped %s memory read from %0*X & %0*X\n",
										inh::m_space->m_manager.machine().describe_context(), inh::m_space->name(),
										inh::m_space->addrchars(), offset,
										2 << Width, mem_mask);
	return inh::m_space->unmap();
}

template<int Width, int AddrShift, int Endian> std::string handler_entry_read_unmapped<Width, AddrShift, Endian>::name() const
{
	return "unmapped";
}


template<int Width, int AddrShift, int Endian> void handler_entry_write_unmapped<Width, AddrShift, Endian>::write(offs_t offset, uX data, uX mem_mask)
{
	if (inh::m_space->log_unmap() && !inh::m_space->m_manager.machine().side_effects_disabled())
		inh::m_space->device().logerror(inh::m_space->is_octal()
										? "%s: unmapped %s memory write to %0*o = %0*o & %0*o\n"
										: "%s: unmapped %s memory write to %0*X = %0*X & %0*X\n",
										inh::m_space->m_manager.machine().describe_context(), inh::m_space->name(),
										inh::m_space->addrchars(), offset,
										2 << Width, data,
										2 << Width, mem_mask);
}

template<int Width, int AddrShift, int Endian> std::string handler_entry_write_unmapped<Width, AddrShift, Endian>::name() const
{
	return "unmapped";
}




template<int Width, int AddrShift, int Endian> typename emu::detail::handler_entry_size<Width>::uX handler_entry_read_nop<Width, AddrShift, Endian>::read(offs_t offset, uX mem_mask)
{
	return inh::m_space->unmap();
}

template<int Width, int AddrShift, int Endian> std::string handler_entry_read_nop<Width, AddrShift, Endian>::name() const
{
	return "nop";
}


template<int Width, int AddrShift, int Endian> void handler_entry_write_nop<Width, AddrShift, Endian>::write(offs_t offset, uX data, uX mem_mask)
{
}

template<int Width, int AddrShift, int Endian> std::string handler_entry_write_nop<Width, AddrShift, Endian>::name() const
{
	return "nop";
}


template class handler_entry_read_unmapped<0,  1, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<0,  1, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<0,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<0,  0, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<1,  3, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<1,  3, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<1,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<1,  0, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<1, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<1, -1, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<2,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<2,  0, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<2, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<2, -1, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<2, -2, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<2, -2, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<3,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<3,  0, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<3, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<3, -1, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<3, -2, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<3, -2, ENDIANNESS_BIG>;
template class handler_entry_read_unmapped<3, -3, ENDIANNESS_LITTLE>;
template class handler_entry_read_unmapped<3, -3, ENDIANNESS_BIG>;

template class handler_entry_write_unmapped<0,  1, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<0,  1, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<0,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<0,  0, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<1,  3, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<1,  3, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<1,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<1,  0, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<1, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<1, -1, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<2,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<2,  0, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<2, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<2, -1, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<2, -2, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<2, -2, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<3,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<3,  0, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<3, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<3, -1, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<3, -2, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<3, -2, ENDIANNESS_BIG>;
template class handler_entry_write_unmapped<3, -3, ENDIANNESS_LITTLE>;
template class handler_entry_write_unmapped<3, -3, ENDIANNESS_BIG>;


template class handler_entry_read_nop<0,  1, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<0,  1, ENDIANNESS_BIG>;
template class handler_entry_read_nop<0,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<0,  0, ENDIANNESS_BIG>;
template class handler_entry_read_nop<1,  3, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<1,  3, ENDIANNESS_BIG>;
template class handler_entry_read_nop<1,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<1,  0, ENDIANNESS_BIG>;
template class handler_entry_read_nop<1, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<1, -1, ENDIANNESS_BIG>;
template class handler_entry_read_nop<2,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<2,  0, ENDIANNESS_BIG>;
template class handler_entry_read_nop<2, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<2, -1, ENDIANNESS_BIG>;
template class handler_entry_read_nop<2, -2, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<2, -2, ENDIANNESS_BIG>;
template class handler_entry_read_nop<3,  0, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<3,  0, ENDIANNESS_BIG>;
template class handler_entry_read_nop<3, -1, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<3, -1, ENDIANNESS_BIG>;
template class handler_entry_read_nop<3, -2, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<3, -2, ENDIANNESS_BIG>;
template class handler_entry_read_nop<3, -3, ENDIANNESS_LITTLE>;
template class handler_entry_read_nop<3, -3, ENDIANNESS_BIG>;

template class handler_entry_write_nop<0,  1, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<0,  1, ENDIANNESS_BIG>;
template class handler_entry_write_nop<0,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<0,  0, ENDIANNESS_BIG>;
template class handler_entry_write_nop<1,  3, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<1,  3, ENDIANNESS_BIG>;
template class handler_entry_write_nop<1,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<1,  0, ENDIANNESS_BIG>;
template class handler_entry_write_nop<1, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<1, -1, ENDIANNESS_BIG>;
template class handler_entry_write_nop<2,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<2,  0, ENDIANNESS_BIG>;
template class handler_entry_write_nop<2, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<2, -1, ENDIANNESS_BIG>;
template class handler_entry_write_nop<2, -2, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<2, -2, ENDIANNESS_BIG>;
template class handler_entry_write_nop<3,  0, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<3,  0, ENDIANNESS_BIG>;
template class handler_entry_write_nop<3, -1, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<3, -1, ENDIANNESS_BIG>;
template class handler_entry_write_nop<3, -2, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<3, -2, ENDIANNESS_BIG>;
template class handler_entry_write_nop<3, -3, ENDIANNESS_LITTLE>;
template class handler_entry_write_nop<3, -3, ENDIANNESS_BIG>;
