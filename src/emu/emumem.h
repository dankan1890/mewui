// license:BSD-3-Clause
// copyright-holders:Aaron Giles,Olivier Galibert
/***************************************************************************

    emumem.h

    Functions which handle device memory accesses.

***************************************************************************/

#pragma once

#ifndef __EMU_H__
#error Dont include this file directly; include emu.h instead.
#endif

#ifndef MAME_EMU_EMUMEM_H
#define MAME_EMU_EMUMEM_H

#include <type_traits>

using s8 = std::int8_t;
using u8 = std::uint8_t;
using s16 = std::int16_t;
using u16 = std::uint16_t;
using s32 = std::int32_t;
using u32 = std::uint32_t;
using s64 = std::int64_t;
using u64 = std::uint64_t;


//**************************************************************************
//  CONSTANTS
//**************************************************************************

// address space names for common use
constexpr int AS_PROGRAM = 0; // program address space
constexpr int AS_DATA    = 1; // data address space
constexpr int AS_IO      = 2; // I/O address space
constexpr int AS_OPCODES = 3; // (decrypted) opcodes, when separate from data accesses

// read or write constants
enum class read_or_write
{
	READ = 1,
	WRITE = 2,
	READWRITE = 3
};



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// offsets and addresses are 32-bit (for now...)
using offs_t = u32;

// address map constructors are delegates that build up an address_map
using address_map_constructor = named_delegate<void (address_map &)>;

// struct with function pointers for accessors; use is generally discouraged unless necessary
struct data_accessors
{
	u8      (*read_byte)(address_space &space, offs_t address);
	u16     (*read_word)(address_space &space, offs_t address);
	u16     (*read_word_masked)(address_space &space, offs_t address, u16 mask);
	u32     (*read_dword)(address_space &space, offs_t address);
	u32     (*read_dword_masked)(address_space &space, offs_t address, u32 mask);
	u64     (*read_qword)(address_space &space, offs_t address);
	u64     (*read_qword_masked)(address_space &space, offs_t address, u64 mask);

	void    (*write_byte)(address_space &space, offs_t address, u8 data);
	void    (*write_word)(address_space &space, offs_t address, u16 data);
	void    (*write_word_masked)(address_space &space, offs_t address, u16 data, u16 mask);
	void    (*write_dword)(address_space &space, offs_t address, u32 data);
	void    (*write_dword_masked)(address_space &space, offs_t address, u32 data, u32 mask);
	void    (*write_qword)(address_space &space, offs_t address, u64 data);
	void    (*write_qword_masked)(address_space &space, offs_t address, u64 data, u64 mask);
};

// a line in the memory structure dump
struct memory_entry {
	offs_t start, end;
	class handler_entry *entry;
};


// ======================> read_delegate

// declare delegates for each width
using read8_delegate  = device_delegate<u8  (address_space &, offs_t, u8 )>;
using read16_delegate = device_delegate<u16 (address_space &, offs_t, u16)>;
using read32_delegate = device_delegate<u32 (address_space &, offs_t, u32)>;
using read64_delegate = device_delegate<u64 (address_space &, offs_t, u64)>;

using read8m_delegate  = device_delegate<u8  (address_space &, offs_t)>;
using read16m_delegate = device_delegate<u16 (address_space &, offs_t)>;
using read32m_delegate = device_delegate<u32 (address_space &, offs_t)>;
using read64m_delegate = device_delegate<u64 (address_space &, offs_t)>;

using read8s_delegate  = device_delegate<u8  (offs_t, u8 )>;
using read16s_delegate = device_delegate<u16 (offs_t, u16)>;
using read32s_delegate = device_delegate<u32 (offs_t, u32)>;
using read64s_delegate = device_delegate<u64 (offs_t, u64)>;

using read8sm_delegate  = device_delegate<u8  (offs_t)>;
using read16sm_delegate = device_delegate<u16 (offs_t)>;
using read32sm_delegate = device_delegate<u32 (offs_t)>;
using read64sm_delegate = device_delegate<u64 (offs_t)>;

using read8mo_delegate  = device_delegate<u8  (address_space &)>;
using read16mo_delegate = device_delegate<u16 (address_space &)>;
using read32mo_delegate = device_delegate<u32 (address_space &)>;
using read64mo_delegate = device_delegate<u64 (address_space &)>;

using read8smo_delegate  = device_delegate<u8  ()>;
using read16smo_delegate = device_delegate<u16 ()>;
using read32smo_delegate = device_delegate<u32 ()>;
using read64smo_delegate = device_delegate<u64 ()>;


// ======================> write_delegate

// declare delegates for each width
using write8_delegate  = device_delegate<void (address_space &, offs_t, u8,  u8 )>;
using write16_delegate = device_delegate<void (address_space &, offs_t, u16, u16)>;
using write32_delegate = device_delegate<void (address_space &, offs_t, u32, u32)>;
using write64_delegate = device_delegate<void (address_space &, offs_t, u64, u64)>;

using write8m_delegate  = device_delegate<void (address_space &, offs_t, u8 )>;
using write16m_delegate = device_delegate<void (address_space &, offs_t, u16)>;
using write32m_delegate = device_delegate<void (address_space &, offs_t, u32)>;
using write64m_delegate = device_delegate<void (address_space &, offs_t, u64)>;

using write8s_delegate  = device_delegate<void (offs_t, u8,  u8 )>;
using write16s_delegate = device_delegate<void (offs_t, u16, u16)>;
using write32s_delegate = device_delegate<void (offs_t, u32, u32)>;
using write64s_delegate = device_delegate<void (offs_t, u64, u64)>;

using write8sm_delegate  = device_delegate<void (offs_t, u8 )>;
using write16sm_delegate = device_delegate<void (offs_t, u16)>;
using write32sm_delegate = device_delegate<void (offs_t, u32)>;
using write64sm_delegate = device_delegate<void (offs_t, u64)>;

using write8mo_delegate  = device_delegate<void (address_space &, u8 )>;
using write16mo_delegate = device_delegate<void (address_space &, u16)>;
using write32mo_delegate = device_delegate<void (address_space &, u32)>;
using write64mo_delegate = device_delegate<void (address_space &, u64)>;

using write8smo_delegate  = device_delegate<void (u8 )>;
using write16smo_delegate = device_delegate<void (u16)>;
using write32smo_delegate = device_delegate<void (u32)>;
using write64smo_delegate = device_delegate<void (u64)>;


namespace emu { namespace detail {

template <typename D, typename T, typename Enable = void> struct rw_device_class  { };

template <typename D, typename T, typename Ret, typename... Params>
struct rw_device_class<D, Ret (T::*)(Params...), std::enable_if_t<std::is_constructible<D, Ret (T::*)(Params...), const char *, const char *, T *>::value> > { using type = T; };
template <typename D, typename T, typename Ret, typename... Params>
struct rw_device_class<D, Ret (T::*)(Params...) const, std::enable_if_t<std::is_constructible<D, Ret (T::*)(Params...) const, const char *, const char *, T *>::value> > { using type = T; };
template <typename D, typename T, typename Ret, typename... Params>
struct rw_device_class<D, Ret (*)(T &, Params...), std::enable_if_t<std::is_constructible<D, Ret (*)(T &, Params...), const char *, const char *, T *>::value> > { using type = T; };

template <typename D, typename T> using rw_device_class_t  = typename rw_device_class <D, T>::type;

template <typename T>
inline read8_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read8_delegate, std::remove_reference_t<T> > *obj)
{ return read8_delegate(func, name, tag, obj); }
template <typename T>
inline read16_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read16_delegate, std::remove_reference_t<T> > *obj)
{ return read16_delegate(func, name, tag, obj); }
template <typename T>
inline read32_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read32_delegate, std::remove_reference_t<T> > *obj)
{ return read32_delegate(func, name, tag, obj); }
template <typename T>
inline read64_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read64_delegate, std::remove_reference_t<T> > *obj)
{ return read64_delegate(func, name, tag, obj); }

template <typename T>
inline write8_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write8_delegate, std::remove_reference_t<T> > *obj)
{ return write8_delegate(func, name, tag, obj); }
template <typename T>
inline write16_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write16_delegate, std::remove_reference_t<T> > *obj)
{ return write16_delegate(func, name, tag, obj); }
template <typename T>
inline write32_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write32_delegate, std::remove_reference_t<T> > *obj)
{ return write32_delegate(func, name, tag, obj); }
template <typename T>
inline write64_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write64_delegate, std::remove_reference_t<T> > *obj)
{ return write64_delegate(func, name, tag, obj); }


template <typename T>
inline read8m_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read8m_delegate, std::remove_reference_t<T> > *obj)
{ return read8m_delegate(func, name, tag, obj); }
template <typename T>
inline read16m_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read16m_delegate, std::remove_reference_t<T> > *obj)
{ return read16m_delegate(func, name, tag, obj); }
template <typename T>
inline read32m_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read32m_delegate, std::remove_reference_t<T> > *obj)
{ return read32m_delegate(func, name, tag, obj); }
template <typename T>
inline read64m_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read64m_delegate, std::remove_reference_t<T> > *obj)
{ return read64m_delegate(func, name, tag, obj); }

template <typename T>
inline write8m_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write8m_delegate, std::remove_reference_t<T> > *obj)
{ return write8m_delegate(func, name, tag, obj); }
template <typename T>
inline write16m_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write16m_delegate, std::remove_reference_t<T> > *obj)
{ return write16m_delegate(func, name, tag, obj); }
template <typename T>
inline write32m_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write32m_delegate, std::remove_reference_t<T> > *obj)
{ return write32m_delegate(func, name, tag, obj); }
template <typename T>
inline write64m_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write64m_delegate, std::remove_reference_t<T> > *obj)
{ return write64m_delegate(func, name, tag, obj); }


template <typename T>
inline read8s_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read8s_delegate, std::remove_reference_t<T> > *obj)
{ return read8s_delegate(func, name, tag, obj); }
template <typename T>
inline read16s_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read16s_delegate, std::remove_reference_t<T> > *obj)
{ return read16s_delegate(func, name, tag, obj); }
template <typename T>
inline read32s_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read32s_delegate, std::remove_reference_t<T> > *obj)
{ return read32s_delegate(func, name, tag, obj); }
template <typename T>
inline read64s_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read64s_delegate, std::remove_reference_t<T> > *obj)
{ return read64s_delegate(func, name, tag, obj); }

template <typename T>
inline write8s_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write8s_delegate, std::remove_reference_t<T> > *obj)
{ return write8s_delegate(func, name, tag, obj); }
template <typename T>
inline write16s_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write16s_delegate, std::remove_reference_t<T> > *obj)
{ return write16s_delegate(func, name, tag, obj); }
template <typename T>
inline write32s_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write32s_delegate, std::remove_reference_t<T> > *obj)
{ return write32s_delegate(func, name, tag, obj); }
template <typename T>
inline write64s_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write64s_delegate, std::remove_reference_t<T> > *obj)
{ return write64s_delegate(func, name, tag, obj); }


template <typename T>
inline read8sm_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read8sm_delegate, std::remove_reference_t<T> > *obj)
{ return read8sm_delegate(func, name, tag, obj); }
template <typename T>
inline read16sm_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read16sm_delegate, std::remove_reference_t<T> > *obj)
{ return read16sm_delegate(func, name, tag, obj); }
template <typename T>
inline read32sm_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read32sm_delegate, std::remove_reference_t<T> > *obj)
{ return read32sm_delegate(func, name, tag, obj); }
template <typename T>
inline read64sm_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read64sm_delegate, std::remove_reference_t<T> > *obj)
{ return read64sm_delegate(func, name, tag, obj); }

template <typename T>
inline write8sm_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write8sm_delegate, std::remove_reference_t<T> > *obj)
{ return write8sm_delegate(func, name, tag, obj); }
template <typename T>
inline write16sm_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write16sm_delegate, std::remove_reference_t<T> > *obj)
{ return write16sm_delegate(func, name, tag, obj); }
template <typename T>
inline write32sm_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write32sm_delegate, std::remove_reference_t<T> > *obj)
{ return write32sm_delegate(func, name, tag, obj); }
template <typename T>
inline write64sm_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write64sm_delegate, std::remove_reference_t<T> > *obj)
{ return write64sm_delegate(func, name, tag, obj); }


template <typename T>
inline read8mo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read8mo_delegate, std::remove_reference_t<T> > *obj)
{ return read8mo_delegate(func, name, tag, obj); }
template <typename T>
inline read16mo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read16mo_delegate, std::remove_reference_t<T> > *obj)
{ return read16mo_delegate(func, name, tag, obj); }
template <typename T>
inline read32mo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read32mo_delegate, std::remove_reference_t<T> > *obj)
{ return read32mo_delegate(func, name, tag, obj); }
template <typename T>
inline read64mo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read64mo_delegate, std::remove_reference_t<T> > *obj)
{ return read64mo_delegate(func, name, tag, obj); }

template <typename T>
inline write8mo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write8mo_delegate, std::remove_reference_t<T> > *obj)
{ return write8mo_delegate(func, name, tag, obj); }
template <typename T>
inline write16mo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write16mo_delegate, std::remove_reference_t<T> > *obj)
{ return write16mo_delegate(func, name, tag, obj); }
template <typename T>
inline write32mo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write32mo_delegate, std::remove_reference_t<T> > *obj)
{ return write32mo_delegate(func, name, tag, obj); }
template <typename T>
inline write64mo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write64mo_delegate, std::remove_reference_t<T> > *obj)
{ return write64mo_delegate(func, name, tag, obj); }


template <typename T>
inline read8smo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read8smo_delegate, std::remove_reference_t<T> > *obj)
{ return read8smo_delegate(func, name, tag, obj); }
template <typename T>
inline read16smo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read16smo_delegate, std::remove_reference_t<T> > *obj)
{ return read16smo_delegate(func, name, tag, obj); }
template <typename T>
inline read32smo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read32smo_delegate, std::remove_reference_t<T> > *obj)
{ return read32smo_delegate(func, name, tag, obj); }
template <typename T>
inline read64smo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<read64smo_delegate, std::remove_reference_t<T> > *obj)
{ return read64smo_delegate(func, name, tag, obj); }

template <typename T>
inline write8smo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write8smo_delegate, std::remove_reference_t<T> > *obj)
{ return write8smo_delegate(func, name, tag, obj); }
template <typename T>
inline write16smo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write16smo_delegate, std::remove_reference_t<T> > *obj)
{ return write16smo_delegate(func, name, tag, obj); }
template <typename T>
inline write32smo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write32smo_delegate, std::remove_reference_t<T> > *obj)
{ return write32smo_delegate(func, name, tag, obj); }
template <typename T>
inline write64smo_delegate make_delegate(T &&func, const char *name, const char *tag, rw_device_class_t<write64smo_delegate, std::remove_reference_t<T> > *obj)
{ return write64smo_delegate(func, name, tag, obj); }



template <typename L>
inline std::enable_if_t<std::is_constructible<read8_delegate, L, const char *>::value, read8_delegate> make_lr8_delegate(L l, const char *name)
{ return read8_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read8m_delegate, L, const char *>::value, read8m_delegate> make_lr8_delegate(L l, const char *name)
{ return read8m_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read8s_delegate, L, const char *>::value, read8s_delegate> make_lr8_delegate(L l, const char *name)
{ return read8s_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read8sm_delegate, L, const char *>::value, read8sm_delegate> make_lr8_delegate(L l, const char *name)
{ return read8sm_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read8mo_delegate, L, const char *>::value, read8mo_delegate> make_lr8_delegate(L l, const char *name)
{ return read8mo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read8smo_delegate, L, const char *>::value, read8smo_delegate> make_lr8_delegate(L l, const char *name)
{ return read8smo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read16_delegate, L, const char *>::value, read16_delegate> make_lr16_delegate(L l, const char *name)
{ return read16_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read16m_delegate, L, const char *>::value, read16m_delegate> make_lr16_delegate(L l, const char *name)
{ return read16m_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read16s_delegate, L, const char *>::value, read16s_delegate> make_lr16_delegate(L l, const char *name)
{ return read16s_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read16sm_delegate, L, const char *>::value, read16sm_delegate> make_lr16_delegate(L l, const char *name)
{ return read16sm_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read16mo_delegate, L, const char *>::value, read16mo_delegate> make_lr16_delegate(L l, const char *name)
{ return read16mo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read16smo_delegate, L, const char *>::value, read16smo_delegate> make_lr16_delegate(L l, const char *name)
{ return read16smo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read32_delegate, L, const char *>::value, read32_delegate> make_lr32_delegate(L l, const char *name)
{ return read32_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read32m_delegate, L, const char *>::value, read32m_delegate> make_lr32_delegate(L l, const char *name)
{ return read32m_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read32s_delegate, L, const char *>::value, read32s_delegate> make_lr32_delegate(L l, const char *name)
{ return read32s_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read32sm_delegate, L, const char *>::value, read32sm_delegate> make_lr32_delegate(L l, const char *name)
{ return read32sm_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read32mo_delegate, L, const char *>::value, read32mo_delegate> make_lr32_delegate(L l, const char *name)
{ return read32mo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read32smo_delegate, L, const char *>::value, read32smo_delegate> make_lr32_delegate(L l, const char *name)
{ return read32smo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read64_delegate, L, const char *>::value, read64_delegate> make_lr64_delegate(L l, const char *name)
{ return read64_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read64m_delegate, L, const char *>::value, read64m_delegate> make_lr64_delegate(L l, const char *name)
{ return read64m_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read64s_delegate, L, const char *>::value, read64s_delegate> make_lr64_delegate(L l, const char *name)
{ return read64s_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read64sm_delegate, L, const char *>::value, read64sm_delegate> make_lr64_delegate(L l, const char *name)
{ return read64sm_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read64mo_delegate, L, const char *>::value, read64mo_delegate> make_lr64_delegate(L l, const char *name)
{ return read64mo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<read64smo_delegate, L, const char *>::value, read64smo_delegate> make_lr64_delegate(L l, const char *name)
{ return read64smo_delegate(l, name); }


template <typename L>
inline std::enable_if_t<std::is_constructible<write8_delegate, L, const char *>::value, write8_delegate> make_lw8_delegate(L l, const char *name)
{ return write8_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write8m_delegate, L, const char *>::value, write8m_delegate> make_lw8_delegate(L l, const char *name)
{ return write8m_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write8s_delegate, L, const char *>::value, write8s_delegate> make_lw8_delegate(L l, const char *name)
{ return write8s_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write8sm_delegate, L, const char *>::value, write8sm_delegate> make_lw8_delegate(L l, const char *name)
{ return write8sm_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write8mo_delegate, L, const char *>::value, write8mo_delegate> make_lw8_delegate(L l, const char *name)
{ return write8mo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write8smo_delegate, L, const char *>::value, write8smo_delegate> make_lw8_delegate(L l, const char *name)
{ return write8smo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write16_delegate, L, const char *>::value, write16_delegate> make_lw16_delegate(L l, const char *name)
{ return write16_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write16m_delegate, L, const char *>::value, write16m_delegate> make_lw16_delegate(L l, const char *name)
{ return write16m_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write16s_delegate, L, const char *>::value, write16s_delegate> make_lw16_delegate(L l, const char *name)
{ return write16s_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write16sm_delegate, L, const char *>::value, write16sm_delegate> make_lw16_delegate(L l, const char *name)
{ return write16sm_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write16mo_delegate, L, const char *>::value, write16mo_delegate> make_lw16_delegate(L l, const char *name)
{ return write16mo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write16smo_delegate, L, const char *>::value, write16smo_delegate> make_lw16_delegate(L l, const char *name)
{ return write16smo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write32_delegate, L, const char *>::value, write32_delegate> make_lw32_delegate(L l, const char *name)
{ return write32_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write32m_delegate, L, const char *>::value, write32m_delegate> make_lw32_delegate(L l, const char *name)
{ return write32m_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write32s_delegate, L, const char *>::value, write32s_delegate> make_lw32_delegate(L l, const char *name)
{ return write32s_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write32sm_delegate, L, const char *>::value, write32sm_delegate> make_lw32_delegate(L l, const char *name)
{ return write32sm_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write32mo_delegate, L, const char *>::value, write32mo_delegate> make_lw32_delegate(L l, const char *name)
{ return write32mo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write32smo_delegate, L, const char *>::value, write32smo_delegate> make_lw32_delegate(L l, const char *name)
{ return write32smo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write64_delegate, L, const char *>::value, write64_delegate> make_lw64_delegate(L l, const char *name)
{ return write64_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write64m_delegate, L, const char *>::value, write64m_delegate> make_lw64_delegate(L l, const char *name)
{ return write64m_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write64s_delegate, L, const char *>::value, write64s_delegate> make_lw64_delegate(L l, const char *name)
{ return write64s_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write64sm_delegate, L, const char *>::value, write64sm_delegate> make_lw64_delegate(L l, const char *name)
{ return write64sm_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write64mo_delegate, L, const char *>::value, write64mo_delegate> make_lw64_delegate(L l, const char *name)
{ return write64mo_delegate(l, name); }

template <typename L>
inline std::enable_if_t<std::is_constructible<write64smo_delegate, L, const char *>::value, write64smo_delegate> make_lw64_delegate(L l, const char *name)
{ return write64smo_delegate(l, name); }



// =====================-> Width -> types

template<int Width> struct handler_entry_size {};
template<> struct handler_entry_size<0> { using uX = u8;  };
template<> struct handler_entry_size<1> { using uX = u16; };
template<> struct handler_entry_size<2> { using uX = u32; };
template<> struct handler_entry_size<3> { using uX = u64; };

// =====================-> Address segmentation for the search tree

constexpr int handler_entry_dispatch_lowbits(int highbits, int width, int ashift)
{
	return (highbits > 48) ? 48 :
		(highbits > 32) ? 32 :
		(highbits > 14) ? 14 :
		width + ashift;
}

} } // namespace emu::detail


// ======================> memory_units_descritor forwards declaration

template<int Width, int AddrShift, int Endian> class memory_units_descriptor;



// =====================-> The root class of all handlers

// Handlers the refcounting as part of the interface

class handler_entry
{
	DISABLE_COPYING(handler_entry);

	template<int Width, int AddrShift, endianness_t Endian> friend class address_space_specific;

public:
	// Typing flags
	static constexpr u32 F_DISPATCH    = 0x00000001; // handler that forwards the access to other handlers
	static constexpr u32 F_UNITS       = 0x00000002; // handler that merges/splits an access among multiple handlers (unitmask support)
	static constexpr u32 F_PASSTHROUGH = 0x00000004; // handler that passes through the request to another handler

	// Start/end of range flags
	static constexpr u8 START = 1;
	static constexpr u8 END   = 2;

	// Intermediary structure for reference count checking
	class reflist {
	public:
		void add(const handler_entry *entry);

		void propagate();
		void check();

	private:
		std::unordered_map<const handler_entry *, u32> refcounts;
		std::unordered_set<const handler_entry *> seen;
		std::unordered_set<const handler_entry *> todo;
	};

	handler_entry(address_space *space, u32 flags) { m_space = space; m_refcount = 1; m_flags = flags; }
	virtual ~handler_entry() {}

	inline void ref(int count = 1) const { m_refcount += count; }
	inline void unref(int count = 1) const { m_refcount -= count; if(!m_refcount) delete this; }
	inline u32 flags() const { return m_flags; }

	inline bool is_dispatch() const { return m_flags & F_DISPATCH; }
	inline bool is_units() const { return m_flags & F_UNITS; }
	inline bool is_passthrough() const { return m_flags & F_PASSTHROUGH; }

	virtual void dump_map(std::vector<memory_entry> &map) const;

	virtual std::string name() const = 0;
	virtual void enumerate_references(handler_entry::reflist &refs) const;
	u32 get_refcount() const { return m_refcount; }

protected:
	// Address range storage
	struct range {
		offs_t start;
		offs_t end;

		inline void set(offs_t _start, offs_t _end) {
			start = _start;
			end = _end;
		}

		inline void intersect(offs_t _start, offs_t _end) {
			if(_start > start)
				start = _start;
			if(_end < end)
				end = _end;
		}
	};

	address_space *m_space;
	mutable u32 m_refcount;
	u32 m_flags;
};


// =====================-> The parent class of all read handlers

// Provides the populate/read/get_ptr/lookup API

template<int Width, int AddrShift, int Endian> class handler_entry_read_passthrough;

template<int Width, int AddrShift, int Endian> class handler_entry_read : public handler_entry
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;

	struct mapping {
		handler_entry_read<Width, AddrShift, Endian> *original;
		handler_entry_read<Width, AddrShift, Endian> *patched;
		u8 ukey;
	};

	handler_entry_read(address_space *space, u32 flags) : handler_entry(space, flags) {}
	~handler_entry_read() {}

	virtual uX read(offs_t offset, uX mem_mask) = 0;
	virtual void *get_ptr(offs_t offset) const;
	virtual void lookup(offs_t address, offs_t &start, offs_t &end, handler_entry_read<Width, AddrShift, Endian> *&handler) const;

	inline void populate(offs_t start, offs_t end, offs_t mirror, handler_entry_read<Width, AddrShift, Endian> *handler) {
		if(mirror)
			populate_mirror(start, end, start, end, mirror, handler);
		else
			populate_nomirror(start, end, start, end, handler);
	}

	virtual void populate_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_read<Width, AddrShift, Endian> *handler);
	virtual void populate_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_read<Width, AddrShift, Endian> *handler);

	inline void populate_mismatched(offs_t start, offs_t end, offs_t mirror, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor) {
		std::vector<mapping> mappings;
		if(mirror)
			populate_mismatched_mirror(start, end, start, end, mirror, descriptor, mappings);
		else
			populate_mismatched_nomirror(start, end, start, end, descriptor, START|END, mappings);
	}

	virtual void populate_mismatched_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, u8 rkey, std::vector<mapping> &mappings);
	virtual void populate_mismatched_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, std::vector<mapping> &mappings);

	inline void populate_passthrough(offs_t start, offs_t end, offs_t mirror, handler_entry_read_passthrough<Width, AddrShift, Endian> *handler) {
		std::vector<mapping> mappings;
		if(mirror)
			populate_passthrough_mirror(start, end, start, end, mirror, handler, mappings);
		else
			populate_passthrough_nomirror(start, end, start, end, handler, mappings);
	}

	virtual void populate_passthrough_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_read_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings);
	virtual void populate_passthrough_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_read_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings);

	// Remove a set of passthrough handlers, leaving the lower handler in their place
	virtual void detach(const std::unordered_set<handler_entry *> &handlers);
};


// =====================-> The parent class of all write handlers

// Provides the populate/write/get_ptr/lookup API

template<int Width, int AddrShift, int Endian> class handler_entry_write_passthrough;

template<int Width, int AddrShift, int Endian> class handler_entry_write : public handler_entry
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;

	struct mapping {
		handler_entry_write<Width, AddrShift, Endian> *original;
		handler_entry_write<Width, AddrShift, Endian> *patched;
		u8 ukey;
	};

	handler_entry_write(address_space *space, u32 flags) : handler_entry(space, flags) {}
	virtual ~handler_entry_write() {}

	virtual void write(offs_t offset, uX data, uX mem_mask) = 0;
	virtual void *get_ptr(offs_t offset) const;
	virtual void lookup(offs_t address, offs_t &start, offs_t &end, handler_entry_write<Width, AddrShift, Endian> *&handler) const;

	inline void populate(offs_t start, offs_t end, offs_t mirror, handler_entry_write<Width, AddrShift, Endian> *handler) {
		if(mirror)
			populate_mirror(start, end, start, end, mirror, handler);
		else
			populate_nomirror(start, end, start, end, handler);
	}

	virtual void populate_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write<Width, AddrShift, Endian> *handler);
	virtual void populate_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write<Width, AddrShift, Endian> *handler);

	inline void populate_mismatched(offs_t start, offs_t end, offs_t mirror, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor) {
		std::vector<mapping> mappings;
		if(mirror)
			populate_mismatched_mirror(start, end, start, end, mirror, descriptor, mappings);
		else
			populate_mismatched_nomirror(start, end, start, end, descriptor, START|END, mappings);
	}

	virtual void populate_mismatched_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, u8 rkey, std::vector<mapping> &mappings);
	virtual void populate_mismatched_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, std::vector<mapping> &mappings);

	inline void populate_passthrough(offs_t start, offs_t end, offs_t mirror, handler_entry_write_passthrough<Width, AddrShift, Endian> *handler) {
		std::vector<mapping> mappings;
		if(mirror)
			populate_passthrough_mirror(start, end, start, end, mirror, handler, mappings);
		else
			populate_passthrough_nomirror(start, end, start, end, handler, mappings);
	}

	virtual void populate_passthrough_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings);
	virtual void populate_passthrough_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings);

	// Remove a set of passthrough handlers, leaving the lower handler in their place
	virtual void detach(const std::unordered_set<handler_entry *> &handlers);
};

// =====================-> Passthrough handler management structure
class memory_passthrough_handler
{
	template<int Width, int AddrShift, int Endian> friend class handler_entry_read_passthrough;
	template<int Width, int AddrShift, int Endian> friend class handler_entry_write_passthrough;

public:
	memory_passthrough_handler(address_space &space) : m_space(space) {}

	inline void remove();

private:
	address_space &m_space;
	std::unordered_set<handler_entry *> m_handlers;

	void add_handler(handler_entry *handler) { m_handlers.insert(handler); }
	void remove_handler(handler_entry *handler) { m_handlers.erase(m_handlers.find(handler)); }
};

// =====================-> Forward declaration for address_space

template<int Width, int AddrShift, int Endian> class handler_entry_read_unmapped;
template<int Width, int AddrShift, int Endian> class handler_entry_write_unmapped;

// ======================> address offset -> byte offset

constexpr offs_t memory_offset_to_byte(offs_t offset, int AddrShift) { return AddrShift < 0 ? offset << iabs(AddrShift) : offset >> iabs(AddrShift); }

// ======================> generic read/write decomposition routines

// generic direct read
template<int Width, int AddrShift, int Endian, int TargetWidth, bool Aligned, typename T> typename emu::detail::handler_entry_size<TargetWidth>::uX  memory_read_generic(T rop, offs_t address, typename emu::detail::handler_entry_size<TargetWidth>::uX mask)
{
	using TargetType = typename emu::detail::handler_entry_size<TargetWidth>::uX;
	using NativeType = typename emu::detail::handler_entry_size<Width>::uX;

	constexpr u32 TARGET_BYTES = 1 << TargetWidth;
	constexpr u32 TARGET_BITS = 8 * TARGET_BYTES;
	constexpr u32 NATIVE_BYTES = 1 << Width;
	constexpr u32 NATIVE_BITS = 8 * NATIVE_BYTES;
	constexpr u32 NATIVE_STEP = AddrShift >= 0 ? NATIVE_BYTES << iabs(AddrShift) : NATIVE_BYTES >> iabs(AddrShift);
	constexpr u32 NATIVE_MASK = Width + AddrShift >= 0 ? (1 << (Width + AddrShift)) - 1 : 0;

	// equal to native size and aligned; simple pass-through to the native reader
	if (NATIVE_BYTES == TARGET_BYTES && (Aligned || (address & NATIVE_MASK) == 0))
		return rop(address & ~NATIVE_MASK, mask);

	// if native size is larger, see if we can do a single masked read (guaranteed if we're aligned)
	if (NATIVE_BYTES > TARGET_BYTES)
	{
		u32 offsbits = 8 * (memory_offset_to_byte(address, AddrShift) & (NATIVE_BYTES - (Aligned ? TARGET_BYTES : 1)));
		if (Aligned || (offsbits + TARGET_BITS <= NATIVE_BITS))
		{
			if (Endian != ENDIANNESS_LITTLE) offsbits = NATIVE_BITS - TARGET_BITS - offsbits;
			return rop(address & ~NATIVE_MASK, (NativeType)mask << offsbits) >> offsbits;
		}
	}

	// determine our alignment against the native boundaries, and mask the address
	u32 offsbits = 8 * (memory_offset_to_byte(address, AddrShift) & (NATIVE_BYTES - 1));
	address &= ~NATIVE_MASK;

	// if we're here, and native size is larger or equal to the target, we need exactly 2 reads
	if (NATIVE_BYTES >= TARGET_BYTES)
	{
		// little-endian case
		if (Endian == ENDIANNESS_LITTLE)
		{
			// read lower bits from lower address
			TargetType result = 0;
			NativeType curmask = (NativeType)mask << offsbits;
			if (curmask != 0) result = rop(address, curmask) >> offsbits;

			// read upper bits from upper address
			offsbits = NATIVE_BITS - offsbits;
			curmask = mask >> offsbits;
			if (curmask != 0) result |= rop(address + NATIVE_STEP, curmask) << offsbits;
			return result;
		}

		// big-endian case
		else
		{
			// left-justify the mask to the target type
			constexpr u32 LEFT_JUSTIFY_TARGET_TO_NATIVE_SHIFT = ((NATIVE_BITS >= TARGET_BITS) ? (NATIVE_BITS - TARGET_BITS) : 0);
			NativeType result = 0;
			NativeType ljmask = (NativeType)mask << LEFT_JUSTIFY_TARGET_TO_NATIVE_SHIFT;
			NativeType curmask = ljmask >> offsbits;

			// read upper bits from lower address
			if (curmask != 0) result = rop(address, curmask) << offsbits;
			offsbits = NATIVE_BITS - offsbits;

			// read lower bits from upper address
			curmask = ljmask << offsbits;
			if (curmask != 0) result |= rop(address + NATIVE_STEP, curmask) >> offsbits;

			// return the un-justified result
			return result >> LEFT_JUSTIFY_TARGET_TO_NATIVE_SHIFT;
		}
	}

	// if we're here, then we have 2 or more reads needed to get our final result
	else
	{
		// compute the maximum number of loops; we do it this way so that there are
		// a fixed number of loops for the compiler to unroll if it desires
		constexpr u32 MAX_SPLITS_MINUS_ONE = TARGET_BYTES / NATIVE_BYTES - 1;
		TargetType result = 0;

		// little-endian case
		if (Endian == ENDIANNESS_LITTLE)
		{
				// read lowest bits from first address
			NativeType curmask = mask << offsbits;
			if (curmask != 0) result = rop(address, curmask) >> offsbits;

			// read middle bits from subsequent addresses
			offsbits = NATIVE_BITS - offsbits;
			for (u32 index = 0; index < MAX_SPLITS_MINUS_ONE; index++)
			{
				address += NATIVE_STEP;
				curmask = mask >> offsbits;
				if (curmask != 0) result |= (TargetType)rop(address, curmask) << offsbits;
				offsbits += NATIVE_BITS;
			}

			// if we're not aligned and we still have bits left, read uppermost bits from last address
			if (!Aligned && offsbits < TARGET_BITS)
			{
				curmask = mask >> offsbits;
				if (curmask != 0) result |= (TargetType)rop(address + NATIVE_STEP, curmask) << offsbits;
			}
		}

		// big-endian case
		else
		{
			// read highest bits from first address
			offsbits = TARGET_BITS - (NATIVE_BITS - offsbits);
			NativeType curmask = mask >> offsbits;
			if (curmask != 0) result = (TargetType)rop(address, curmask) << offsbits;

			// read middle bits from subsequent addresses
			for (u32 index = 0; index < MAX_SPLITS_MINUS_ONE; index++)
			{
				offsbits -= NATIVE_BITS;
				address += NATIVE_STEP;
				curmask = mask >> offsbits;
				if (curmask != 0) result |= (TargetType)rop(address, curmask) << offsbits;
			}

			// if we're not aligned and we still have bits left, read lowermost bits from the last address
			if (!Aligned && offsbits != 0)
			{
				offsbits = NATIVE_BITS - offsbits;
				curmask = mask << offsbits;
				if (curmask != 0) result |= rop(address + NATIVE_STEP, curmask) >> offsbits;
			}
		}
		return result;
	}
}

// generic direct write
template<int Width, int AddrShift, int Endian, int TargetWidth, bool Aligned, typename T> void memory_write_generic(T wop, offs_t address, typename emu::detail::handler_entry_size<TargetWidth>::uX data, typename emu::detail::handler_entry_size<TargetWidth>::uX mask)
{
	using NativeType = typename emu::detail::handler_entry_size<Width>::uX;

	constexpr u32 TARGET_BYTES = 1 << TargetWidth;
	constexpr u32 TARGET_BITS = 8 * TARGET_BYTES;
	constexpr u32 NATIVE_BYTES = 1 << Width;
	constexpr u32 NATIVE_BITS = 8 * NATIVE_BYTES;
	constexpr u32 NATIVE_STEP = AddrShift >= 0 ? NATIVE_BYTES << iabs(AddrShift) : NATIVE_BYTES >> iabs(AddrShift);
	constexpr u32 NATIVE_MASK = Width + AddrShift >= 0 ? (1 << (Width + AddrShift)) - 1 : 0;

	// equal to native size and aligned; simple pass-through to the native writer
	if (NATIVE_BYTES == TARGET_BYTES && (Aligned || (address & NATIVE_MASK) == 0))
		return wop(address & ~NATIVE_MASK, data, mask);

	// if native size is larger, see if we can do a single masked write (guaranteed if we're aligned)
	if (NATIVE_BYTES > TARGET_BYTES)
	{
		u32 offsbits = 8 * (memory_offset_to_byte(address, AddrShift) & (NATIVE_BYTES - (Aligned ? TARGET_BYTES : 1)));
		if (Aligned || (offsbits + TARGET_BITS <= NATIVE_BITS))
		{
			if (Endian != ENDIANNESS_LITTLE) offsbits = NATIVE_BITS - TARGET_BITS - offsbits;
			return wop(address & ~NATIVE_MASK, (NativeType)data << offsbits, (NativeType)mask << offsbits);
		}
	}

	// determine our alignment against the native boundaries, and mask the address
	u32 offsbits = 8 * (memory_offset_to_byte(address, AddrShift) & (NATIVE_BYTES - 1));
	address &= ~NATIVE_MASK;

	// if we're here, and native size is larger or equal to the target, we need exactly 2 writes
	if (NATIVE_BYTES >= TARGET_BYTES)
	{
		// little-endian case
		if (Endian == ENDIANNESS_LITTLE)
		{
			// write lower bits to lower address
			NativeType curmask = (NativeType)mask << offsbits;
			if (curmask != 0) wop(address, (NativeType)data << offsbits, curmask);

			// write upper bits to upper address
			offsbits = NATIVE_BITS - offsbits;
			curmask = mask >> offsbits;
			if (curmask != 0) wop(address + NATIVE_STEP, data >> offsbits, curmask);
		}

		// big-endian case
		else
		{
			// left-justify the mask and data to the target type
			constexpr u32 LEFT_JUSTIFY_TARGET_TO_NATIVE_SHIFT = ((NATIVE_BITS >= TARGET_BITS) ? (NATIVE_BITS - TARGET_BITS) : 0);
			NativeType ljdata = (NativeType)data << LEFT_JUSTIFY_TARGET_TO_NATIVE_SHIFT;
			NativeType ljmask = (NativeType)mask << LEFT_JUSTIFY_TARGET_TO_NATIVE_SHIFT;
				// write upper bits to lower address
			NativeType curmask = ljmask >> offsbits;
			if (curmask != 0) wop(address, ljdata >> offsbits, curmask);
				// write lower bits to upper address
			offsbits = NATIVE_BITS - offsbits;
			curmask = ljmask << offsbits;
			if (curmask != 0) wop(address + NATIVE_STEP, ljdata << offsbits, curmask);
		}
	}

	// if we're here, then we have 2 or more writes needed to get our final result
	else
	{
		// compute the maximum number of loops; we do it this way so that there are
		// a fixed number of loops for the compiler to unroll if it desires
		constexpr u32 MAX_SPLITS_MINUS_ONE = TARGET_BYTES / NATIVE_BYTES - 1;

		// little-endian case
		if (Endian == ENDIANNESS_LITTLE)
		{
			// write lowest bits to first address
			NativeType curmask = mask << offsbits;
			if (curmask != 0) wop(address, data << offsbits, curmask);

			// write middle bits to subsequent addresses
			offsbits = NATIVE_BITS - offsbits;
			for (u32 index = 0; index < MAX_SPLITS_MINUS_ONE; index++)
			{
				address += NATIVE_STEP;
				curmask = mask >> offsbits;
				if (curmask != 0) wop(address, data >> offsbits, curmask);
				offsbits += NATIVE_BITS;
			}

			// if we're not aligned and we still have bits left, write uppermost bits to last address
			if (!Aligned && offsbits < TARGET_BITS)
			{
				curmask = mask >> offsbits;
				if (curmask != 0) wop(address + NATIVE_STEP, data >> offsbits, curmask);
			}
		}

		// big-endian case
		else
		{
			// write highest bits to first address
			offsbits = TARGET_BITS - (NATIVE_BITS - offsbits);
			NativeType curmask = mask >> offsbits;
			if (curmask != 0) wop(address, data >> offsbits, curmask);

			// write middle bits to subsequent addresses
			for (u32 index = 0; index < MAX_SPLITS_MINUS_ONE; index++)
			{
				offsbits -= NATIVE_BITS;
				address += NATIVE_STEP;
				curmask = mask >> offsbits;
				if (curmask != 0) wop(address, data >> offsbits, curmask);
			}

			// if we're not aligned and we still have bits left, write lowermost bits to the last address
			if (!Aligned && offsbits != 0)
			{
				offsbits = NATIVE_BITS - offsbits;
				curmask = mask << offsbits;
				if (curmask != 0) wop(address + NATIVE_STEP, data << offsbits, curmask);
			}
		}
	}
}


// ======================> memory_access_cache

// memory_access_cache contains state data for cached access
template<int Width, int AddrShift, int Endian> class memory_access_cache
{
	using NativeType = typename emu::detail::handler_entry_size<Width>::uX;
	static constexpr u32 NATIVE_BYTES = 1 << Width;
	static constexpr u32 NATIVE_MASK = Width + AddrShift >= 0 ? (1 << (Width + AddrShift)) - 1 : 0;

public:
	// construction/destruction
	memory_access_cache(address_space &space,
						handler_entry_read <Width, AddrShift, Endian> *root_read,
						handler_entry_write<Width, AddrShift, Endian> *root_write);

	~memory_access_cache();

	// getters
	address_space &space() const { return m_space; }

	// see if an address is within bounds, update it if not
	void check_address_r(offs_t address) {
		if(address >= m_addrstart_r && address <= m_addrend_r)
			return;
		m_root_read->lookup(address, m_addrstart_r, m_addrend_r, m_cache_r);
	}

	void check_address_w(offs_t address) {
		if(address >= m_addrstart_w && address <= m_addrend_w)
			return;
		m_root_write->lookup(address, m_addrstart_w, m_addrend_w, m_cache_w);
	}

	// accessor methods

	void *read_ptr(offs_t address) {
		check_address_r(address);
		return m_cache_r->get_ptr(address);
	}

	u8 read_byte(offs_t address) { address &= m_addrmask; return Width == 0 ? read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 0, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xff); }
	u16 read_word(offs_t address) { address &= m_addrmask; return Width == 1 ? read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 1, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffff); }
	u16 read_word(offs_t address, u16 mask) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 1, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u16 read_word_unaligned(offs_t address) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 1, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffff); }
	u16 read_word_unaligned(offs_t address, u16 mask) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 1, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u32 read_dword(offs_t address) { address &= m_addrmask; return Width == 2 ? read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 2, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffffffff); }
	u32 read_dword(offs_t address, u32 mask) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 2, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u32 read_dword_unaligned(offs_t address) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 2, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffffffff); }
	u32 read_dword_unaligned(offs_t address, u32 mask) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 2, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u64 read_qword(offs_t address) { address &= m_addrmask; return Width == 3 ? read_native(address & ~NATIVE_MASK) : memory_read_generic<Width, AddrShift, Endian, 3, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffffffffffffffffU); }
	u64 read_qword(offs_t address, u64 mask) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 3, true>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }
	u64 read_qword_unaligned(offs_t address) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 3, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, 0xffffffffffffffffU); }
	u64 read_qword_unaligned(offs_t address, u64 mask) { address &= m_addrmask; return memory_read_generic<Width, AddrShift, Endian, 3, false>([this](offs_t offset, NativeType mask) -> NativeType { return read_native(offset, mask); }, address, mask); }

	void write_byte(offs_t address, u8 data) { address &= m_addrmask; if (Width == 0) write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 0, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xff); }
	void write_word(offs_t address, u16 data) { address &= m_addrmask; if (Width == 1) write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 1, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffff); }
	void write_word(offs_t address, u16 data, u16 mask) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 1, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_word_unaligned(offs_t address, u16 data) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 1, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffff); }
	void write_word_unaligned(offs_t address, u16 data, u16 mask) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 1, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_dword(offs_t address, u32 data) { address &= m_addrmask; if (Width == 2) write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 2, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffffffff); }
	void write_dword(offs_t address, u32 data, u32 mask) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 2, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_dword_unaligned(offs_t address, u32 data) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 2, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffffffff); }
	void write_dword_unaligned(offs_t address, u32 data, u32 mask) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 2, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_qword(offs_t address, u64 data) { address &= m_addrmask; if (Width == 3) write_native(address & ~NATIVE_MASK, data); else memory_write_generic<Width, AddrShift, Endian, 3, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffffffffffffffffU); }
	void write_qword(offs_t address, u64 data, u64 mask) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 3, true>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }
	void write_qword_unaligned(offs_t address, u64 data) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 3, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, 0xffffffffffffffffU); }
	void write_qword_unaligned(offs_t address, u64 data, u64 mask) { address &= m_addrmask; memory_write_generic<Width, AddrShift, Endian, 3, false>([this](offs_t offset, NativeType data, NativeType mask) { write_native(offset, data, mask); }, address, data, mask); }

private:
	address_space &             m_space;

	int                         m_notifier_id;             // id to remove the notifier on destruction

	offs_t                      m_addrmask;                // address mask
	offs_t                      m_addrstart_r;             // minimum valid address for reading
	offs_t                      m_addrend_r;               // maximum valid address for reading
	offs_t                      m_addrstart_w;             // minimum valid address for writing
	offs_t                      m_addrend_w;               // maximum valid address for writing
	handler_entry_read<Width, AddrShift, Endian> *m_cache_r;   // read cache
	handler_entry_write<Width, AddrShift, Endian> *m_cache_w;  // write cache

	handler_entry_read <Width, AddrShift, Endian> *m_root_read;  // decode tree roots
	handler_entry_write<Width, AddrShift, Endian> *m_root_write;

	NativeType read_native(offs_t address, NativeType mask = ~NativeType(0));
	void write_native(offs_t address, NativeType data, NativeType mask = ~NativeType(0));
};


// ======================> address_space_config

// describes an address space and provides basic functions to map addresses to bytes
class address_space_config
{
	friend class address_map;

public:
	// construction/destruction
	address_space_config();
	address_space_config(const char *name, endianness_t endian, u8 datawidth, u8 addrwidth, s8 addrshift = 0, address_map_constructor internal = address_map_constructor());
	address_space_config(const char *name, endianness_t endian, u8 datawidth, u8 addrwidth, s8 addrshift, u8 logwidth, u8 pageshift, address_map_constructor internal = address_map_constructor());

	// getters
	const char *name() const { return m_name; }
	endianness_t endianness() const { return m_endianness; }
	int data_width() const { return m_data_width; }
	int addr_width() const { return m_addr_width; }
	int addr_shift() const { return m_addr_shift; }
	int logaddr_width() const { return m_logaddr_width; }
	int page_shift() const { return m_page_shift; }
	bool is_octal() const { return m_is_octal; }

	// Actual alignment of the bus addresses
	int alignment() const { int bytes = m_data_width / 8; return m_addr_shift < 0 ? bytes >> -m_addr_shift : bytes << m_addr_shift; }

	// Address delta to byte delta helpers
	inline offs_t addr2byte(offs_t address) const { return (m_addr_shift < 0) ? (address << -m_addr_shift) : (address >> m_addr_shift); }
	inline offs_t byte2addr(offs_t address) const { return (m_addr_shift > 0) ? (address << m_addr_shift) : (address >> -m_addr_shift); }

	// address-to-byte conversion helpers
	inline offs_t addr2byte_end(offs_t address) const { return (m_addr_shift < 0) ? ((address << -m_addr_shift) | ((1 << -m_addr_shift) - 1)) : (address >> m_addr_shift); }
	inline offs_t byte2addr_end(offs_t address) const { return (m_addr_shift > 0) ? ((address << m_addr_shift) | ((1 << m_addr_shift) - 1)) : (address >> -m_addr_shift); }

	// state (TODO: privatize)
	const char *        m_name;
	endianness_t        m_endianness;
	u8                  m_data_width;
	u8                  m_addr_width;
	s8                  m_addr_shift;
	u8                  m_logaddr_width;
	u8                  m_page_shift;
	bool                m_is_octal;                 // to determine if messages/debugger will show octal or hex

	address_map_constructor m_internal_map;
};


// ======================> address_space

// address_space holds live information about an address space
class address_space
{
	friend class memory_bank;
	friend class memory_block;
	template<int Width, int AddrShift, int Endian> friend class handler_entry_read_unmapped;
	template<int Width, int AddrShift, int Endian> friend class handler_entry_write_unmapped;
	template<int Width, int AddrShift, int Endian> friend class memory_access_cache;

	struct notifier_t {
		std::function<void (read_or_write)> m_notifier;
		int m_id;
	};

protected:
	// construction/destruction
	address_space(memory_manager &manager, device_memory_interface &memory, int spacenum);

public:
	virtual ~address_space();

	// getters
	device_t &device() const { return m_device; }
	const char *name() const { return m_name; }
	int spacenum() const { return m_spacenum; }
	address_map *map() const { return m_map.get(); }

	template<int Width, int AddrShift, int Endian> memory_access_cache<Width, AddrShift, Endian> *cache() {
		if(AddrShift != m_config.addr_shift())
			fatalerror("Requesting cache() with address shift %d while the config says %d\n", AddrShift, m_config.addr_shift());
		if(8 << Width != m_config.data_width())
			fatalerror("Requesting cache() with data width %d while the config says %d\n", 8 << Width, m_config.data_width());
		if(Endian != m_config.endianness())
			fatalerror("Requesting cache() with endianness %s while the config says %s\n",
					   endianness_names[Endian], endianness_names[m_config.endianness()]);

		return static_cast<memory_access_cache<Width, AddrShift, Endian> *>(create_cache());
	}

	int add_change_notifier(std::function<void (read_or_write)> n);
	void remove_change_notifier(int id);

	void invalidate_caches(read_or_write mode) {
		if(u32(mode) & ~m_in_notification) {
			u32 old = m_in_notification;
			m_in_notification |= u32(mode);
			for(const auto &n : m_notifiers)
				n.m_notifier(mode);
			m_in_notification = old;
		}
	}

	virtual void validate_reference_counts() const = 0;

	virtual void remove_passthrough(std::unordered_set<handler_entry *> &handlers) = 0;

	int data_width() const { return m_config.data_width(); }
	int addr_width() const { return m_config.addr_width(); }
	int logaddr_width() const { return m_config.logaddr_width(); }
	int alignment() const { return m_config.alignment(); }
	endianness_t endianness() const { return m_config.endianness(); }
	int addr_shift() const { return m_config.addr_shift(); }
	u64 unmap() const { return m_unmap; }
	bool is_octal() const { return m_config.is_octal(); }

	offs_t addrmask() const { return m_addrmask; }
	u8 addrchars() const { return m_addrchars; }
	offs_t logaddrmask() const { return m_logaddrmask; }
	u8 logaddrchars() const { return m_logaddrchars; }

	// debug helpers
	virtual std::string get_handler_string(read_or_write readorwrite, offs_t byteaddress) const = 0;
	virtual void dump_maps(std::vector<memory_entry> &read_map, std::vector<memory_entry> &write_map) const = 0;
	bool log_unmap() const { return m_log_unmap; }
	void set_log_unmap(bool log) { m_log_unmap = log; }

	// general accessors
	virtual void accessors(data_accessors &accessors) const = 0;
	virtual void *get_read_ptr(offs_t address) const = 0;
	virtual void *get_write_ptr(offs_t address) const = 0;

	// read accessors
	virtual u8 read_byte(offs_t address) = 0;
	virtual u16 read_word(offs_t address) = 0;
	virtual u16 read_word(offs_t address, u16 mask) = 0;
	virtual u16 read_word_unaligned(offs_t address) = 0;
	virtual u16 read_word_unaligned(offs_t address, u16 mask) = 0;
	virtual u32 read_dword(offs_t address) = 0;
	virtual u32 read_dword(offs_t address, u32 mask) = 0;
	virtual u32 read_dword_unaligned(offs_t address) = 0;
	virtual u32 read_dword_unaligned(offs_t address, u32 mask) = 0;
	virtual u64 read_qword(offs_t address) = 0;
	virtual u64 read_qword(offs_t address, u64 mask) = 0;
	virtual u64 read_qword_unaligned(offs_t address) = 0;
	virtual u64 read_qword_unaligned(offs_t address, u64 mask) = 0;

	// write accessors
	virtual void write_byte(offs_t address, u8 data) = 0;
	virtual void write_word(offs_t address, u16 data) = 0;
	virtual void write_word(offs_t address, u16 data, u16 mask) = 0;
	virtual void write_word_unaligned(offs_t address, u16 data) = 0;
	virtual void write_word_unaligned(offs_t address, u16 data, u16 mask) = 0;
	virtual void write_dword(offs_t address, u32 data) = 0;
	virtual void write_dword(offs_t address, u32 data, u32 mask) = 0;
	virtual void write_dword_unaligned(offs_t address, u32 data) = 0;
	virtual void write_dword_unaligned(offs_t address, u32 data, u32 mask) = 0;
	virtual void write_qword(offs_t address, u64 data) = 0;
	virtual void write_qword(offs_t address, u64 data, u64 mask) = 0;
	virtual void write_qword_unaligned(offs_t address, u64 data) = 0;
	virtual void write_qword_unaligned(offs_t address, u64 data, u64 mask) = 0;

	// address-to-byte conversion helpers
	offs_t address_to_byte(offs_t address) const { return m_config.addr2byte(address); }
	offs_t address_to_byte_end(offs_t address) const { return m_config.addr2byte_end(address); }
	offs_t byte_to_address(offs_t address) const { return m_config.byte2addr(address); }
	offs_t byte_to_address_end(offs_t address) const { return m_config.byte2addr_end(address); }

	// umap ranges (short form)
	void unmap_read(offs_t addrstart, offs_t addrend, offs_t addrmirror = 0) { unmap_generic(addrstart, addrend, addrmirror, read_or_write::READ, false); }
	void unmap_write(offs_t addrstart, offs_t addrend, offs_t addrmirror = 0) { unmap_generic(addrstart, addrend, addrmirror, read_or_write::WRITE, false); }
	void unmap_readwrite(offs_t addrstart, offs_t addrend, offs_t addrmirror = 0) { unmap_generic(addrstart, addrend, addrmirror, read_or_write::READWRITE, false); }
	void nop_read(offs_t addrstart, offs_t addrend, offs_t addrmirror = 0) { unmap_generic(addrstart, addrend, addrmirror, read_or_write::READ, true); }
	void nop_write(offs_t addrstart, offs_t addrend, offs_t addrmirror = 0) { unmap_generic(addrstart, addrend, addrmirror, read_or_write::WRITE, true); }
	void nop_readwrite(offs_t addrstart, offs_t addrend, offs_t addrmirror = 0) { unmap_generic(addrstart, addrend, addrmirror, read_or_write::READWRITE, true); }

	// install ports, banks, RAM (short form)
	void install_read_port(offs_t addrstart, offs_t addrend, const char *rtag) { install_read_port(addrstart, addrend, 0, rtag); }
	void install_write_port(offs_t addrstart, offs_t addrend, const char *wtag) { install_write_port(addrstart, addrend, 0, wtag); }
	void install_readwrite_port(offs_t addrstart, offs_t addrend, const char *rtag, const char *wtag) { install_readwrite_port(addrstart, addrend, 0, rtag, wtag); }
	void install_read_bank(offs_t addrstart, offs_t addrend, const char *tag) { install_read_bank(addrstart, addrend, 0, tag); }
	void install_write_bank(offs_t addrstart, offs_t addrend, const char *tag) { install_write_bank(addrstart, addrend, 0, tag); }
	void install_readwrite_bank(offs_t addrstart, offs_t addrend, const char *tag) { install_readwrite_bank(addrstart, addrend, 0, tag); }
	void install_read_bank(offs_t addrstart, offs_t addrend, memory_bank *bank) { install_read_bank(addrstart, addrend, 0, bank); }
	void install_write_bank(offs_t addrstart, offs_t addrend, memory_bank *bank) { install_write_bank(addrstart, addrend, 0, bank); }
	void install_readwrite_bank(offs_t addrstart, offs_t addrend, memory_bank *bank) { install_readwrite_bank(addrstart, addrend, 0, bank); }
	void install_rom(offs_t addrstart, offs_t addrend, void *baseptr = nullptr) { install_rom(addrstart, addrend, 0, baseptr); }
	void install_writeonly(offs_t addrstart, offs_t addrend, void *baseptr = nullptr) { install_writeonly(addrstart, addrend, 0, baseptr); }
	void install_ram(offs_t addrstart, offs_t addrend, void *baseptr = nullptr) { install_ram(addrstart, addrend, 0, baseptr); }

	// install ports, banks, RAM (with mirror/mask)
	void install_read_port(offs_t addrstart, offs_t addrend, offs_t addrmirror, const char *rtag) { install_readwrite_port(addrstart, addrend, addrmirror, rtag, ""); }
	void install_write_port(offs_t addrstart, offs_t addrend, offs_t addrmirror, const char *wtag) { install_readwrite_port(addrstart, addrend, addrmirror, "", wtag); }
	virtual void install_readwrite_port(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string rtag, std::string wtag) = 0;
	void install_read_bank(offs_t addrstart, offs_t addrend, offs_t addrmirror, const char *tag) { install_bank_generic(addrstart, addrend, addrmirror, tag, ""); }
	void install_write_bank(offs_t addrstart, offs_t addrend, offs_t addrmirror, const char *tag) { install_bank_generic(addrstart, addrend, addrmirror, "", tag); }
	void install_readwrite_bank(offs_t addrstart, offs_t addrend, offs_t addrmirror, const char *tag)  { install_bank_generic(addrstart, addrend, addrmirror, tag, tag); }
	void install_read_bank(offs_t addrstart, offs_t addrend, offs_t addrmirror, memory_bank *bank) { install_bank_generic(addrstart, addrend, addrmirror, bank, nullptr); }
	void install_write_bank(offs_t addrstart, offs_t addrend, offs_t addrmirror, memory_bank *bank) { install_bank_generic(addrstart, addrend, addrmirror, nullptr, bank); }
	void install_readwrite_bank(offs_t addrstart, offs_t addrend, offs_t addrmirror, memory_bank *bank)  { install_bank_generic(addrstart, addrend, addrmirror, bank, bank); }
	void install_rom(offs_t addrstart, offs_t addrend, offs_t addrmirror, void *baseptr = nullptr) { install_ram_generic(addrstart, addrend, addrmirror, read_or_write::READ, baseptr); }
	void install_writeonly(offs_t addrstart, offs_t addrend, offs_t addrmirror, void *baseptr = nullptr) { install_ram_generic(addrstart, addrend, addrmirror, read_or_write::WRITE, baseptr); }
	void install_ram(offs_t addrstart, offs_t addrend, offs_t addrmirror, void *baseptr = nullptr) { install_ram_generic(addrstart, addrend, addrmirror, read_or_write::READWRITE, baseptr); }

	// install device memory maps
	template <typename T> void install_device(offs_t addrstart, offs_t addrend, T &device, void (T::*map)(address_map &map), u64 unitmask = 0, int cswidth = 0) {
		address_map_constructor delegate(map, "dynamic_device_install", &device);
		install_device_delegate(addrstart, addrend, device, delegate, unitmask, cswidth);
	}

	virtual void install_device_delegate(offs_t addrstart, offs_t addrend, device_t &device, address_map_constructor &map, u64 unitmask = 0, int cswidth = 0) = 0;

	// install taps without mirroring
	memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u8  &data, u8  mem_mask)> tap, memory_passthrough_handler *mph = nullptr) { return install_read_tap(addrstart, addrend, 0, name, tap, mph); }
	memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u16 &data, u16 mem_mask)> tap, memory_passthrough_handler *mph = nullptr) { return install_read_tap(addrstart, addrend, 0, name, tap, mph); }
	memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u32 &data, u32 mem_mask)> tap, memory_passthrough_handler *mph = nullptr) { return install_read_tap(addrstart, addrend, 0, name, tap, mph); }
	memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u64 &data, u64 mem_mask)> tap, memory_passthrough_handler *mph = nullptr) { return install_read_tap(addrstart, addrend, 0, name, tap, mph); }
	memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u8  &data, u8  mem_mask)> tap, memory_passthrough_handler *mph = nullptr) { return install_write_tap(addrstart, addrend, 0, name, tap, mph); }
	memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u16 &data, u16 mem_mask)> tap, memory_passthrough_handler *mph = nullptr) { return install_write_tap(addrstart, addrend, 0, name, tap, mph); }
	memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u32 &data, u32 mem_mask)> tap, memory_passthrough_handler *mph = nullptr) { return install_write_tap(addrstart, addrend, 0, name, tap, mph); }
	memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u64 &data, u64 mem_mask)> tap, memory_passthrough_handler *mph = nullptr) { return install_write_tap(addrstart, addrend, 0, name, tap, mph); }
	memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u8  &data, u8  mem_mask)> tapr, std::function<void (offs_t offset, u8  &data, u8  mem_mask)> tapw, memory_passthrough_handler *mph = nullptr) { return install_readwrite_tap(addrstart, addrend, 0, name, tapr, tapw, mph); }
	memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u16 &data, u16 mem_mask)> tapr, std::function<void (offs_t offset, u16 &data, u16 mem_mask)> tapw, memory_passthrough_handler *mph = nullptr) { return install_readwrite_tap(addrstart, addrend, 0, name, tapr, tapw, mph); }
	memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u32 &data, u32 mem_mask)> tapr, std::function<void (offs_t offset, u32 &data, u32 mem_mask)> tapw, memory_passthrough_handler *mph = nullptr) { return install_readwrite_tap(addrstart, addrend, 0, name, tapr, tapw, mph); }
	memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, std::string name, std::function<void (offs_t offset, u64 &data, u64 mem_mask)> tapr, std::function<void (offs_t offset, u64 &data, u64 mem_mask)> tapw, memory_passthrough_handler *mph = nullptr) { return install_readwrite_tap(addrstart, addrend, 0, name, tapr, tapw, mph); }

	// install taps with mirroring
	virtual memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u8  &data, u8  mem_mask)> tap, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u16 &data, u16 mem_mask)> tap, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u32 &data, u32 mem_mask)> tap, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_read_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u64 &data, u64 mem_mask)> tap, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u8  &data, u8  mem_mask)> tap, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u16 &data, u16 mem_mask)> tap, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u32 &data, u32 mem_mask)> tap, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_write_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u64 &data, u64 mem_mask)> tap, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u8  &data, u8  mem_mask)> tapr, std::function<void (offs_t offset, u8  &data, u8  mem_mask)> tapw, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u16 &data, u16 mem_mask)> tapr, std::function<void (offs_t offset, u16 &data, u16 mem_mask)> tapw, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u32 &data, u32 mem_mask)> tapr, std::function<void (offs_t offset, u32 &data, u32 mem_mask)> tapw, memory_passthrough_handler *mph = nullptr);
	virtual memory_passthrough_handler *install_readwrite_tap(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string name, std::function<void (offs_t offset, u64 &data, u64 mem_mask)> tapr, std::function<void (offs_t offset, u64 &data, u64 mem_mask)> tapw, memory_passthrough_handler *mph = nullptr);


	// install new-style delegate handlers (short form)
	void install_read_handler(offs_t addrstart, offs_t addrend, read8_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write8_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read8_delegate rhandler, write8_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read16_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write16_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read16_delegate rhandler, write16_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read32_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write32_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read32_delegate rhandler, write32_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read64_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write64_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read64_delegate rhandler, write64_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }

	void install_read_handler(offs_t addrstart, offs_t addrend, read8m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write8m_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read8m_delegate rhandler, write8m_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read16m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write16m_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read16m_delegate rhandler, write16m_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read32m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write32m_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read32m_delegate rhandler, write32m_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read64m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write64m_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read64m_delegate rhandler, write64m_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }

	void install_read_handler(offs_t addrstart, offs_t addrend, read8s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write8s_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read8s_delegate rhandler, write8s_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read16s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write16s_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read16s_delegate rhandler, write16s_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read32s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write32s_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read32s_delegate rhandler, write32s_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read64s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write64s_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read64s_delegate rhandler, write64s_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }

	void install_read_handler(offs_t addrstart, offs_t addrend, read8sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write8sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read8sm_delegate rhandler, write8sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read16sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write16sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read16sm_delegate rhandler, write16sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read32sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write32sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read32sm_delegate rhandler, write32sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read64sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write64sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read64sm_delegate rhandler, write64sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }

	void install_read_handler(offs_t addrstart, offs_t addrend, read8mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write8mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read8mo_delegate rhandler, write8mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read16mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write16mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read16mo_delegate rhandler, write16mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read32mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write32mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read32mo_delegate rhandler, write32mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read64mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write64mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read64mo_delegate rhandler, write64mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }

	void install_read_handler(offs_t addrstart, offs_t addrend, read8smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write8smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read8smo_delegate rhandler, write8smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read16smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write16smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read16smo_delegate rhandler, write16smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read32smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write32smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read32smo_delegate rhandler, write32smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { return install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }
	void install_read_handler(offs_t addrstart, offs_t addrend, read64smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) { install_read_handler(addrstart, addrend, 0, 0, 0, rhandler, unitmask, cswidth); }
	void install_write_handler(offs_t addrstart, offs_t addrend, write64smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_write_handler(addrstart, addrend, 0, 0, 0, whandler, unitmask, cswidth); }
	void install_readwrite_handler(offs_t addrstart, offs_t addrend, read64smo_delegate rhandler, write64smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) { install_readwrite_handler(addrstart, addrend, 0, 0, 0, rhandler, whandler, unitmask, cswidth); }

	// install new-style delegate handlers (with mirror/mask)
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8_delegate rhandler, write8_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16_delegate rhandler, write16_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32_delegate rhandler, write32_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64_delegate rhandler, write64_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;

	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8m_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8m_delegate rhandler, write8m_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16m_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16m_delegate rhandler, write16m_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32m_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32m_delegate rhandler, write32m_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64m_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64m_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64m_delegate rhandler, write64m_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;

	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8s_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8s_delegate rhandler, write8s_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16s_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16s_delegate rhandler, write16s_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32s_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32s_delegate rhandler, write32s_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64s_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64s_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64s_delegate rhandler, write64s_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;

	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8sm_delegate rhandler, write8sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16sm_delegate rhandler, write16sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32sm_delegate rhandler, write32sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64sm_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64sm_delegate rhandler, write64sm_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;

	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8mo_delegate rhandler, write8mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16mo_delegate rhandler, write16mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32mo_delegate rhandler, write32mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64mo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64mo_delegate rhandler, write64mo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;

	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write8smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read8smo_delegate rhandler, write8smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write16smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read16smo_delegate rhandler, write16smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write32smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read32smo_delegate rhandler, write32smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_read_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64smo_delegate rhandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_write_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, write64smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;
	virtual void install_readwrite_handler(offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, read64smo_delegate rhandler, write64smo_delegate whandler, u64 unitmask = 0, int cswidth = 0) = 0;

	// setup
	void prepare_map();
	void populate_from_map(address_map *map = nullptr);
	void allocate_memory();
	void locate_memory();

	template<int Width, int AddrShift, int Endian> handler_entry_read_unmapped <Width, AddrShift, Endian> *get_unmap_r() const { return static_cast<handler_entry_read_unmapped <Width, AddrShift, Endian> *>(m_unmap_r); }
	template<int Width, int AddrShift, int Endian> handler_entry_write_unmapped<Width, AddrShift, Endian> *get_unmap_w() const { return static_cast<handler_entry_write_unmapped<Width, AddrShift, Endian> *>(m_unmap_w); }

protected:
	// internal helpers
	virtual void *create_cache() = 0;

	void populate_map_entry(const address_map_entry &entry, read_or_write readorwrite);
	virtual void unmap_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, read_or_write readorwrite, bool quiet) = 0;
	virtual void install_ram_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, read_or_write readorwrite, void *baseptr) = 0;
	virtual void install_bank_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, std::string rtag, std::string wtag) = 0;
	virtual void install_bank_generic(offs_t addrstart, offs_t addrend, offs_t addrmirror, memory_bank *rbank, memory_bank *wbank) = 0;
	void adjust_addresses(offs_t &start, offs_t &end, offs_t &mask, offs_t &mirror);
	void *find_backing_memory(offs_t addrstart, offs_t addrend);
	bool needs_backing_store(const address_map_entry &entry);
	memory_bank &bank_find_or_allocate(const char *tag, offs_t addrstart, offs_t addrend, offs_t addrmirror, read_or_write readorwrite);
	address_map_entry *block_assign_intersecting(offs_t bytestart, offs_t byteend, u8 *base);
	void check_optimize_all(const char *function, int width, offs_t addrstart, offs_t addrend, offs_t addrmask, offs_t addrmirror, offs_t addrselect, u64 unitmask, int cswidth, offs_t &nstart, offs_t &nend, offs_t &nmask, offs_t &nmirror, u64 &nunitmask, int &ncswidth);
	void check_optimize_mirror(const char *function, offs_t addrstart, offs_t addrend, offs_t addrmirror, offs_t &nstart, offs_t &nend, offs_t &nmask, offs_t &nmirror);
	void check_address(const char *function, offs_t addrstart, offs_t addrend);

	// private state
	const address_space_config &m_config;       // configuration of this space
	device_t &              m_device;           // reference to the owning device
	std::unique_ptr<address_map> m_map;         // original memory map
	offs_t                  m_addrmask;         // physical address mask
	offs_t                  m_logaddrmask;      // logical address mask
	u64                     m_unmap;            // unmapped value
	int                     m_spacenum;         // address space index
	bool                    m_log_unmap;        // log unmapped accesses in this space?
	const char *            m_name;             // friendly name of the address space
	u8                      m_addrchars;        // number of characters to use for physical addresses
	u8                      m_logaddrchars;     // number of characters to use for logical addresses

	handler_entry           *m_unmap_r;
	handler_entry           *m_unmap_w;

	handler_entry           *m_nop_r;
	handler_entry           *m_nop_w;

	std::vector<std::unique_ptr<memory_passthrough_handler>> m_mphs;

	std::vector<notifier_t> m_notifiers;        // notifier list for address map change
	int                     m_notifier_id;      // next notifier id
	u32                     m_in_notification;  // notification(s) currently being done
	memory_manager &        m_manager;          // reference to the owning manager
};


// ======================> memory_block

// a memory block is a chunk of RAM associated with a range of memory in a device's address space
class memory_block
{
	DISABLE_COPYING(memory_block);

public:
	// construction/destruction
	memory_block(address_space &space, offs_t start, offs_t end, void *memory = nullptr);
	~memory_block();

	// getters
	running_machine &machine() const { return m_machine; }
	offs_t addrstart() const { return m_addrstart; }
	offs_t addrend() const { return m_addrend; }
	u8 *data() const { return m_data; }

	// is the given range contained by this memory block?
	bool contains(address_space &space, offs_t addrstart, offs_t addrend) const
	{
		return (&space == &m_space && m_addrstart <= addrstart && m_addrend >= addrend);
	}

private:
	// internal state
	running_machine &       m_machine;              // need the machine to free our memory
	address_space &         m_space;                // which address space are we associated with?
	offs_t                  m_addrstart, m_addrend; // start/end for verifying a match
	u8 *                    m_data;                 // pointer to the data for this block
	std::vector<u8>         m_allocated;            // pointer to the actually allocated block
};


// ======================> memory_bank

// a memory bank is a global pointer to memory that can be shared across devices and changed dynamically
class memory_bank
{
	// a bank reference is an entry in a list of address spaces that reference a given bank
	class bank_reference
	{
	public:
		// construction/destruction
		bank_reference(address_space &space, read_or_write readorwrite)
			: m_space(space),
				m_readorwrite(readorwrite) { }

		// getters
		address_space &space() const { return m_space; }

		// does this reference match the space+read/write combination?
		bool matches(const address_space &space, read_or_write readorwrite) const
		{
			return (&space == &m_space && (readorwrite == read_or_write::READWRITE || readorwrite == m_readorwrite));
		}

	private:
		// internal state
		address_space &         m_space;            // address space that references us
		read_or_write           m_readorwrite;      // used for read or write?

	};

public:
	// construction/destruction
	memory_bank(address_space &space, int index, offs_t start, offs_t end, const char *tag = nullptr);
	~memory_bank();

	// getters
	running_machine &machine() const { return m_machine; }
	int entry() const { return m_curentry; }
	bool anonymous() const { return m_anonymous; }
	offs_t addrstart() const { return m_addrstart; }
	void *base() const { return m_entries.empty() ? nullptr : m_entries[m_curentry]; }
	const char *tag() const { return m_tag.c_str(); }
	const char *name() const { return m_name.c_str(); }

	// compare a range against our range
	bool matches_exactly(offs_t addrstart, offs_t addrend) const { return (m_addrstart == addrstart && m_addrend == addrend); }
	bool fully_covers(offs_t addrstart, offs_t addrend) const { return (m_addrstart <= addrstart && m_addrend >= addrend); }
	bool is_covered_by(offs_t addrstart, offs_t addrend) const { return (m_addrstart >= addrstart && m_addrend <= addrend); }
	bool straddles(offs_t addrstart, offs_t addrend) const { return (m_addrstart < addrend && m_addrend > addrstart); }

	// track and verify address space references to this bank
	bool references_space(const address_space &space, read_or_write readorwrite) const;
	void add_reference(address_space &space, read_or_write readorwrite);

	// set the base explicitly
	void set_base(void *base);

	// configure and set entries
	void configure_entry(int entrynum, void *base);
	void configure_entries(int startentry, int numentries, void *base, offs_t stride);
	void set_entry(int entrynum);
	void add_notifier(std::function<void (void *)> cb);

private:
	// internal state
	running_machine &       m_machine;              // need the machine to free our memory
	std::vector<u8 *>       m_entries;              // the entries
	bool                    m_anonymous;            // are we anonymous or explicit?
	offs_t                  m_addrstart;            // start offset
	offs_t                  m_addrend;              // end offset
	int                     m_curentry;             // current entry
	std::string             m_name;                 // friendly name for this bank
	std::string             m_tag;                  // tag for this bank
	std::vector<std::unique_ptr<bank_reference>> m_reflist;     // list of address spaces referencing this bank
	std::vector<std::function<void (void *)>> m_alloc_notifier; // list of notifier targets when allocating
};


// ======================> memory_share

// a memory share contains information about shared memory region
class memory_share
{
public:
	// construction/destruction
	memory_share(u8 width, size_t bytes, endianness_t endianness, void *ptr = nullptr)
		: m_ptr(ptr),
			m_bytes(bytes),
			m_endianness(endianness),
			m_bitwidth(width),
			m_bytewidth(width <= 8 ? 1 : width <= 16 ? 2 : width <= 32 ? 4 : 8)
	{ }

	// getters
	void *ptr() const { return m_ptr; }
	size_t bytes() const { return m_bytes; }
	endianness_t endianness() const { return m_endianness; }
	u8 bitwidth() const { return m_bitwidth; }
	u8 bytewidth() const { return m_bytewidth; }

	// setters
	void set_ptr(void *ptr) { m_ptr = ptr; }

private:
	// internal state
	void *                  m_ptr;                  // pointer to the memory backing the region
	size_t                  m_bytes;                // size of the shared region in bytes
	endianness_t            m_endianness;           // endianness of the memory
	u8                      m_bitwidth;             // width of the shared region in bits
	u8                      m_bytewidth;            // width in bytes, rounded up to a power of 2

};


// ======================> memory_region

// memory region object
class memory_region
{
	DISABLE_COPYING(memory_region);

	friend class memory_manager;
public:
	// construction/destruction
	memory_region(running_machine &machine, const char *name, u32 length, u8 width, endianness_t endian);

	// getters
	running_machine &machine() const { return m_machine; }
	u8 *base() { return (m_buffer.size() > 0) ? &m_buffer[0] : nullptr; }
	u8 *end() { return base() + m_buffer.size(); }
	u32 bytes() const { return m_buffer.size(); }
	const char *name() const { return m_name.c_str(); }

	// flag expansion
	endianness_t endianness() const { return m_endianness; }
	u8 bitwidth() const { return m_bitwidth; }
	u8 bytewidth() const { return m_bytewidth; }

	// data access
	u8 &as_u8(offs_t offset = 0) { return m_buffer[offset]; }
	u16 &as_u16(offs_t offset = 0) { return reinterpret_cast<u16 *>(base())[offset]; }
	u32 &as_u32(offs_t offset = 0) { return reinterpret_cast<u32 *>(base())[offset]; }
	u64 &as_u64(offs_t offset = 0) { return reinterpret_cast<u64 *>(base())[offset]; }

private:
	// internal data
	running_machine &       m_machine;
	std::string             m_name;
	std::vector<u8>         m_buffer;
	endianness_t            m_endianness;
	u8                      m_bitwidth;
	u8                      m_bytewidth;
};



// ======================> memory_manager

// holds internal state for the memory system
class memory_manager
{
	friend class address_space;
	template<int Width, int AddrShift, endianness_t Endian> friend class address_space_specific;
	friend memory_region::memory_region(running_machine &machine, const char *name, u32 length, u8 width, endianness_t endian);
public:
	// construction/destruction
	memory_manager(running_machine &machine);
	void initialize();

	// getters
	running_machine &machine() const { return m_machine; }
	const std::unordered_map<std::string, std::unique_ptr<memory_bank>> &banks() const { return m_banklist; }
	const std::unordered_map<std::string, std::unique_ptr<memory_region>> &regions() const { return m_regionlist; }
	const std::unordered_map<std::string, std::unique_ptr<memory_share>> &shares() const { return m_sharelist; }

	// regions
	memory_region *region_alloc(const char *name, u32 length, u8 width, endianness_t endian);
	void region_free(const char *name);
	memory_region *region_containing(const void *memory, offs_t bytes) const;

	memory_bank *find(const char *tag) const;
	memory_bank *find(address_space &space, offs_t addrstart, offs_t addrend) const;
	memory_bank *allocate(address_space &space, offs_t addrstart, offs_t addrend, const char *tag = nullptr);

private:
	void allocate(device_memory_interface &memory);

	// internal state
	running_machine &           m_machine;              // reference to the machine
	bool                        m_initialized;          // have we completed initialization?

	std::vector<std::unique_ptr<memory_block>>   m_blocklist;            // head of the list of memory blocks

	std::unordered_map<std::string, std::unique_ptr<memory_bank>>    m_banklist;             // data gathered for each bank
	std::unordered_map<std::string, std::unique_ptr<memory_share>>   m_sharelist;            // map for share lookups
	std::unordered_map<std::string, std::unique_ptr<memory_region>>  m_regionlist;           // list of memory regions
};



//**************************************************************************
//  MACROS
//**************************************************************************

// space read/write handler function macros
#define READ8_MEMBER(name)              u8     name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u8 mem_mask)
#define WRITE8_MEMBER(name)             void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u8 data, ATTR_UNUSED u8 mem_mask)
#define READ16_MEMBER(name)             u16    name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u16 mem_mask)
#define WRITE16_MEMBER(name)            void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u16 data, ATTR_UNUSED u16 mem_mask)
#define READ32_MEMBER(name)             u32    name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u32 mem_mask)
#define WRITE32_MEMBER(name)            void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u32 data, ATTR_UNUSED u32 mem_mask)
#define READ64_MEMBER(name)             u64    name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u64 mem_mask)
#define WRITE64_MEMBER(name)            void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u64 data, ATTR_UNUSED u64 mem_mask)

#define DECLARE_READ8_MEMBER(name)      u8     name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u8 mem_mask = 0xff)
#define DECLARE_WRITE8_MEMBER(name)     void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u8 data, ATTR_UNUSED u8 mem_mask = 0xff)
#define DECLARE_READ16_MEMBER(name)     u16    name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u16 mem_mask = 0xffff)
#define DECLARE_WRITE16_MEMBER(name)    void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u16 data, ATTR_UNUSED u16 mem_mask = 0xffff)
#define DECLARE_READ32_MEMBER(name)     u32    name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u32 mem_mask = 0xffffffff)
#define DECLARE_WRITE32_MEMBER(name)    void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u32 data, ATTR_UNUSED u32 mem_mask = 0xffffffff)
#define DECLARE_READ64_MEMBER(name)     u64    name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u64 mem_mask = 0xffffffffffffffffU)
#define DECLARE_WRITE64_MEMBER(name)    void   name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED u64 data, ATTR_UNUSED u64 mem_mask = 0xffffffffffffffffU)

// device delegate macros
#define READ8_DELEGATE(_class, _member)                     read8_delegate(FUNC(_class::_member), this)
#define WRITE8_DELEGATE(_class, _member)                    write8_delegate(FUNC(_class::_member), this)
#define READ16_DELEGATE(_class, _member)                    read16_delegate(FUNC(_class::_member), this)
#define WRITE16_DELEGATE(_class, _member)                   write16_delegate(FUNC(_class::_member), this)
#define READ32_DELEGATE(_class, _member)                    read32_delegate(FUNC(_class::_member), this)
#define WRITE32_DELEGATE(_class, _member)                   write32_delegate(FUNC(_class::_member), this)
#define READ64_DELEGATE(_class, _member)                    read64_delegate(FUNC(_class::_member), this)
#define WRITE64_DELEGATE(_class, _member)                   write64_delegate(FUNC(_class::_member), this)

#define READ8_DEVICE_DELEGATE(_device, _class, _member)     read8_delegate(FUNC(_class::_member), (_class *)_device)
#define WRITE8_DEVICE_DELEGATE(_device, _class, _member)    write8_delegate(FUNC(_class::_member), (_class *)_device)
#define READ16_DEVICE_DELEGATE(_device, _class, _member)    read16_delegate(FUNC(_class::_member), (_class *)_device)
#define WRITE16_DEVICE_DELEGATE(_device, _class, _member)   write16_delegate(FUNC(_class::_member), (_class *)_device)
#define READ32_DEVICE_DELEGATE(_device, _class, _member)    read32_delegate(FUNC(_class::_member), (_class *)_device)
#define WRITE32_DEVICE_DELEGATE(_device, _class, _member)   write32_delegate(FUNC(_class::_member), (_class *)_device)
#define READ64_DEVICE_DELEGATE(_device, _class, _member)    read64_delegate(FUNC(_class::_member), (_class *)_device)
#define WRITE64_DEVICE_DELEGATE(_device, _class, _member)   write64_delegate(FUNC(_class::_member), (_class *)_device)


// helper macro for merging data with the memory mask
#define COMBINE_DATA(varptr)            (*(varptr) = (*(varptr) & ~mem_mask) | (data & mem_mask))

#define ACCESSING_BITS_0_7              ((mem_mask & 0x000000ffU) != 0)
#define ACCESSING_BITS_8_15             ((mem_mask & 0x0000ff00U) != 0)
#define ACCESSING_BITS_16_23            ((mem_mask & 0x00ff0000U) != 0)
#define ACCESSING_BITS_24_31            ((mem_mask & 0xff000000U) != 0)
#define ACCESSING_BITS_32_39            ((mem_mask & 0x000000ff00000000U) != 0)
#define ACCESSING_BITS_40_47            ((mem_mask & 0x0000ff0000000000U) != 0)
#define ACCESSING_BITS_48_55            ((mem_mask & 0x00ff000000000000U) != 0)
#define ACCESSING_BITS_56_63            ((mem_mask & 0xff00000000000000U) != 0)

#define ACCESSING_BITS_0_15             ((mem_mask & 0x0000ffffU) != 0)
#define ACCESSING_BITS_16_31            ((mem_mask & 0xffff0000U) != 0)
#define ACCESSING_BITS_32_47            ((mem_mask & 0x0000ffff00000000U) != 0)
#define ACCESSING_BITS_48_63            ((mem_mask & 0xffff000000000000U) != 0)

#define ACCESSING_BITS_0_31             ((mem_mask & 0xffffffffU) != 0)
#define ACCESSING_BITS_32_63            ((mem_mask & 0xffffffff00000000U) != 0)


// macros for accessing bytes and words within larger chunks

// read/write a byte to a 16-bit space
#define BYTE_XOR_BE(a)                  ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(1,0))
#define BYTE_XOR_LE(a)                  ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(0,1))

// read/write a byte to a 32-bit space
#define BYTE4_XOR_BE(a)                 ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(3,0))
#define BYTE4_XOR_LE(a)                 ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(0,3))

// read/write a word to a 32-bit space
#define WORD_XOR_BE(a)                  ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(2,0))
#define WORD_XOR_LE(a)                  ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(0,2))

// read/write a byte to a 64-bit space
#define BYTE8_XOR_BE(a)                 ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(7,0))
#define BYTE8_XOR_LE(a)                 ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(0,7))

// read/write a word to a 64-bit space
#define WORD2_XOR_BE(a)                 ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(6,0))
#define WORD2_XOR_LE(a)                 ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(0,6))

// read/write a dword to a 64-bit space
#define DWORD_XOR_BE(a)                 ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(4,0))
#define DWORD_XOR_LE(a)                 ((a) ^ NATIVE_ENDIAN_VALUE_LE_BE(0,4))


// helpers for checking address alignment
#define WORD_ALIGNED(a)                 (((a) & 1) == 0)
#define DWORD_ALIGNED(a)                (((a) & 3) == 0)
#define QWORD_ALIGNED(a)                (((a) & 7) == 0)


template<int Width, int AddrShift, int Endian> typename emu::detail::handler_entry_size<Width>::uX memory_access_cache<Width, AddrShift, Endian>::read_native(offs_t address, typename emu::detail::handler_entry_size<Width>::uX mask)
{
	check_address_r(address);
	return m_cache_r->read(address, mask);
}

template<int Width, int AddrShift, int Endian> void memory_access_cache<Width, AddrShift, Endian>::write_native(offs_t address, typename emu::detail::handler_entry_size<Width>::uX data, typename emu::detail::handler_entry_size<Width>::uX mask)
{
	check_address_w(address);
	m_cache_w->write(address, data, mask);
}

void memory_passthrough_handler::remove()
{
	m_space.remove_passthrough(m_handlers);
}

#endif  /* MAME_EMU_EMUMEM_H */
