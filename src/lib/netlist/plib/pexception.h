// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * palloc.h
 *
 */

#ifndef PEXCEPTION_H_
#define PEXCEPTION_H_

#include "pstring.h"
#include "ptypes.h"

#include <exception>

namespace plib {

	//============================================================
	// terminate
	//============================================================

	/*! Terminate the program
	 *
	 * \note could be enhanced by setting a termination handler
	 */
	[[noreturn]] void terminate(const pstring &msg) noexcept;

	//============================================================
	//  exception base
	//============================================================

	class pexception : public std::exception
	{
	public:
		explicit pexception(const pstring &text);

		const pstring &text() { return m_text; }
		const char* what() const noexcept override { return m_text.c_str(); }

	private:
		pstring m_text;
	};

	class file_e : public plib::pexception
	{
	public:
		file_e(const pstring &fmt, const pstring &filename);
	};

	class file_open_e : public file_e
	{
	public:
		explicit file_open_e(const pstring &filename);
	};

	class file_read_e : public file_e
	{
	public:
		explicit file_read_e(const pstring &filename);
	};

	class file_write_e : public file_e
	{
	public:
		explicit file_write_e(const pstring &filename);
	};

	class null_argument_e : public plib::pexception
	{
	public:
		explicit null_argument_e(const pstring &argument);
	};

	class out_of_mem_e : public plib::pexception
	{
	public:
		explicit out_of_mem_e(const pstring &location);
	};

	/* FIXME: currently only a stub for later use. More use could be added by
	 * using “-fnon-call-exceptions" and sigaction to enable c++ exception supported.
	 */

	class fpexception_e : public pexception
	{
	public:
		explicit fpexception_e(const pstring &text);
	};

	static constexpr unsigned FP_INEXACT = 0x0001;
	static constexpr unsigned FP_DIVBYZERO = 0x0002;
	static constexpr unsigned FP_UNDERFLOW = 0x0004;
	static constexpr unsigned FP_OVERFLOW = 0x0008;
	static constexpr unsigned FP_INVALID = 0x00010;
	static constexpr unsigned FP_ALL = 0x0001f;

	/*
	 * Catch SIGFPE on linux for debugging purposes.
	 */

	class fpsignalenabler
	{
	public:
		explicit fpsignalenabler(unsigned fpexceptions);

		COPYASSIGNMOVE(fpsignalenabler, delete)

		~fpsignalenabler();

		/* is the functionality supported ? */
		static bool supported();
		/* returns last global enable state */
		static bool global_enable(bool enable);

	private:
		int m_last_enabled;

		static bool m_enable;
	};


} // namespace plib

#endif /* PEXCEPTION_H_ */
