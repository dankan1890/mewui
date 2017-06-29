// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    dimemory.h

    Device memory interfaces.

***************************************************************************/

#pragma once

#ifndef __EMU_H__
#error Dont include this file directly; include emu.h instead.
#endif

#ifndef MAME_EMU_DIMEMORY_H
#define MAME_EMU_DIMEMORY_H


//**************************************************************************
//  CONSTANTS
//**************************************************************************

// Translation intentions
constexpr int TRANSLATE_TYPE_MASK       = 0x03;     // read write or fetch
constexpr int TRANSLATE_USER_MASK       = 0x04;     // user mode or fully privileged
constexpr int TRANSLATE_DEBUG_MASK      = 0x08;     // debug mode (no side effects)

constexpr int TRANSLATE_READ            = 0;        // translate for read
constexpr int TRANSLATE_WRITE           = 1;        // translate for write
constexpr int TRANSLATE_FETCH           = 2;        // translate for instruction fetch
constexpr int TRANSLATE_READ_USER       = (TRANSLATE_READ | TRANSLATE_USER_MASK);
constexpr int TRANSLATE_WRITE_USER      = (TRANSLATE_WRITE | TRANSLATE_USER_MASK);
constexpr int TRANSLATE_FETCH_USER      = (TRANSLATE_FETCH | TRANSLATE_USER_MASK);
constexpr int TRANSLATE_READ_DEBUG      = (TRANSLATE_READ | TRANSLATE_DEBUG_MASK);
constexpr int TRANSLATE_WRITE_DEBUG     = (TRANSLATE_WRITE | TRANSLATE_DEBUG_MASK);
constexpr int TRANSLATE_FETCH_DEBUG     = (TRANSLATE_FETCH | TRANSLATE_DEBUG_MASK);



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_DEVICE_ADDRESS_MAP(_space, _map) \
	device_memory_interface::static_set_addrmap(*device, _space, ADDRESS_MAP_NAME(_map));

#define MCFG_DEVICE_REMOVE_ADDRESS_MAP(_space) \
	device_memory_interface::static_set_addrmap(*device, _space, nullptr);

#define MCFG_DEVICE_PROGRAM_MAP(_map) \
	MCFG_DEVICE_ADDRESS_MAP(AS_PROGRAM, _map)

#define MCFG_DEVICE_DATA_MAP(_map) \
	MCFG_DEVICE_ADDRESS_MAP(AS_DATA, _map)

#define MCFG_DEVICE_IO_MAP(_map) \
	MCFG_DEVICE_ADDRESS_MAP(AS_IO, _map)

#define MCFG_DEVICE_DECRYPTED_OPCODES_MAP(_map) \
	MCFG_DEVICE_ADDRESS_MAP(AS_DECRYPTED_OPCODES, _map)



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> device_memory_interface

class device_memory_interface : public device_interface
{
	friend class device_scheduler;

public:
	// construction/destruction
	device_memory_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_memory_interface();

	// configuration access
	address_map_constructor address_map(address_spacenum spacenum = AS_0) const { return (spacenum < ARRAY_LENGTH(m_address_map)) ? m_address_map[spacenum] : nullptr; }
	const address_space_config *space_config(address_spacenum spacenum = AS_0) const { return memory_space_config(spacenum); }

	// static inline configuration helpers
	static void static_set_addrmap(device_t &device, address_spacenum spacenum, address_map_constructor map);

	// basic information getters
	bool has_space(int index = 0) const { return (m_addrspace[index] != nullptr); }
	bool has_space(address_spacenum index) const { return (m_addrspace[int(index)] != nullptr); }
	bool has_configured_map(int index = 0) const { return (m_address_map[index] != nullptr); }
	bool has_configured_map(address_spacenum index) const { return (m_address_map[int(index)] != nullptr); }
	address_space &space(int index = 0) const { assert(m_addrspace[index] != nullptr); return *m_addrspace[index]; }
	address_space &space(address_spacenum index) const { assert(m_addrspace[int(index)] != nullptr); return *m_addrspace[int(index)]; }

	// address space accessors
	void set_address_space(address_spacenum spacenum, address_space &space);

	// address translation
	bool translate(address_spacenum spacenum, int intention, offs_t &address) { return memory_translate(spacenum, intention, address); }

	// deliberately ambiguous functions; if you have the memory interface
	// just use it
	device_memory_interface &memory() { return *this; }

protected:
	// required overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum) const = 0;

	// optional operation overrides
	virtual bool memory_translate(address_spacenum spacenum, int intention, offs_t &address);

	// interface-level overrides
	virtual void interface_validity_check(validity_checker &valid) const override;

	// configuration
	address_map_constructor m_address_map[ADDRESS_SPACES]; // address maps for each address space

private:
	// internal state
	address_space *     m_addrspace[ADDRESS_SPACES]; // reported address spaces
};

// iterator
typedef device_interface_iterator<device_memory_interface> memory_interface_iterator;



#endif  /* MAME_EMU_DIMEMORY_H */
