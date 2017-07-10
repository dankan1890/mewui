// license:BSD-3-Clause
// copyright-holders:Vas Crabb

#include "emu.h"
#include "intellec4.h"

#include <algorithm>


DEFINE_DEVICE_TYPE_NS(INTELLEC4_UNIV_SLOT, bus::intellec4, univ_slot_device, "intlc4univslot", "INTELLEC 4/MOD 40 Universal Slot")
DEFINE_DEVICE_TYPE_NS(INTELLEC4_UNIV_BUS,  bus::intellec4, univ_bus_device,  "intlc4univbus",  "INTELLEC 4/MOD 40 Universal Bus")


namespace bus { namespace intellec4 {

/***********************************************************************
    SLOT DEVICE
***********************************************************************/

univ_slot_device::univ_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, INTELLEC4_UNIV_SLOT, tag, owner, clock)
	, device_slot_interface(mconfig, *this)
{
}


void univ_slot_device::set_bus_tag(device_t &device, char const *tag)
{
}


void univ_slot_device::device_start()
{
}



/***********************************************************************
    BUS DEVICE
***********************************************************************/

/*----------------------------------
  address space configuration
----------------------------------*/

void univ_bus_device::set_rom_space(device_t &device, char const *tag, int space)
{
	univ_bus_device &bus(downcast<univ_bus_device &>(device));
	bus.m_rom_device.set_tag(tag);
	bus.m_rom_space = space;
}

void univ_bus_device::set_rom_ports_space(device_t &device, char const *tag, int space)
{
	univ_bus_device &bus(downcast<univ_bus_device &>(device));
	bus.m_rom_ports_device.set_tag(tag);
	bus.m_rom_ports_space = space;
}

void univ_bus_device::set_memory_space(device_t &device, char const *tag, int space)
{
	univ_bus_device &bus(downcast<univ_bus_device &>(device));
	bus.m_memory_device.set_tag(tag);
	bus.m_memory_space = space;
}

void univ_bus_device::set_status_space(device_t &device, char const *tag, int space)
{
	univ_bus_device &bus(downcast<univ_bus_device &>(device));
	bus.m_status_device.set_tag(tag);
	bus.m_status_space = space;
}

void univ_bus_device::set_ram_ports_space(device_t &device, char const *tag, int space)
{
	univ_bus_device &bus(downcast<univ_bus_device &>(device));
	bus.m_ram_ports_device.set_tag(tag);
	bus.m_ram_ports_space = space;
}


univ_bus_device::univ_bus_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, INTELLEC4_UNIV_BUS, tag, owner, clock)
	, m_rom_device(*this, finder_base::DUMMY_TAG)
	, m_rom_ports_device(*this, finder_base::DUMMY_TAG)
	, m_memory_device(*this, finder_base::DUMMY_TAG)
	, m_status_device(*this, finder_base::DUMMY_TAG)
	, m_ram_ports_device(*this, finder_base::DUMMY_TAG)
	, m_rom_space(-1)
	, m_rom_ports_space(-1)
	, m_memory_space(-1)
	, m_status_space(-1)
	, m_ram_ports_space(-1)
	, m_stop_out_cb(*this)
	, m_test_out_cb(*this)
	, m_reset_4002_out_cb(*this)
	, m_user_reset_out_cb(*this)
	, m_stop(0U)
	, m_test(0U)
	, m_reset_4002(0U)
	, m_user_reset(0U)
{
	std::fill(std::begin(m_cards), std::end(m_cards), nullptr);
}


/*----------------------------------
  input lines
----------------------------------*/

WRITE_LINE_MEMBER(univ_bus_device::sync_in)
{
	// FIXME: distribute to cards
}

WRITE_LINE_MEMBER(univ_bus_device::test_in)
{
	if (state)
		m_test &= ~u16(1U);
	else
		m_test |= 1U;
	// FIXME: distribute to cards
}

WRITE_LINE_MEMBER(univ_bus_device::stop_in)
{
	if (state)
		m_stop &= ~u16(1U);
	else
		m_stop |= 1U;
	// FIXME: distribute to cards
}

WRITE_LINE_MEMBER(univ_bus_device::stop_acknowledge_in)
{
	// FIXME: distribute to cards
}

WRITE_LINE_MEMBER(univ_bus_device::cpu_reset_in)
{
	// FIXME: distribute to cards
}

WRITE_LINE_MEMBER(univ_bus_device::reset_4002_in)
{
	if (state)
		m_reset_4002 &= ~u16(1U);
	else
		m_reset_4002 |= 1U;
	// FIXME: distribute to cards
}


/*----------------------------------
  device_t implementation
----------------------------------*/

void univ_bus_device::device_validity_check(validity_checker &valid) const
{
	if (m_rom_device && !m_rom_device->space_config(m_rom_space))
		osd_printf_error("ROM space device %s (%s) lacks address space %d config\n", m_rom_device->device().tag(), m_rom_device->device().name(), m_rom_space);
	if (m_rom_ports_device && !m_rom_ports_device->space_config(m_rom_ports_space))
		osd_printf_error("ROM ports space device %s (%s) lacks address space %d config\n", m_rom_ports_device->device().tag(), m_rom_ports_device->device().name(), m_rom_ports_space);
	if (m_memory_device && !m_memory_device->space_config(m_memory_space))
		osd_printf_error("Memory space device %s (%s) lacks address space %d config\n", m_memory_device->device().tag(), m_memory_device->device().name(), m_memory_space);
	if (m_status_device && !m_status_device->space_config(m_status_space))
		osd_printf_error("Status space device %s (%s) lacks address space %d config\n", m_status_device->device().tag(), m_status_device->device().name(), m_status_space);
	if (m_ram_ports_device && !m_ram_ports_device->space_config(m_ram_ports_space))
		osd_printf_error("RAM ports space device %s (%s) lacks address space %d config\n", m_ram_ports_device->device().tag(), m_ram_ports_device->device().name(), m_ram_ports_space);
}

void univ_bus_device::device_start()
{
	m_stop_out_cb.resolve_safe();
	m_test_out_cb.resolve_safe();
	m_reset_4002_out_cb.resolve_safe();
	m_user_reset_out_cb.resolve_safe();
}

} } // namespace bus::intellec4



SLOT_INTERFACE_START(intellec4_univ_cards)
SLOT_INTERFACE_END
