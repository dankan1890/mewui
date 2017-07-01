// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

    nanamame/datfile.h

    NANAMAME DATs manager.

***************************************************************************/

#ifndef NANAMAME_DATFILE_H
#define NANAMAME_DATFILE_H

#pragma once

#include <memory>
#include <unordered_map>

class ui_options;

namespace nanamame
{
class tokenizer
{
public:
	// constructors
	explicit tokenizer(std::string const& searchpath, char sep);

	// main interface
	bool next(std::string& buffer);
	void reset();

private:
	// internal state
	std::string m_searchpath;
	std::string::const_iterator m_current;
	bool m_is_first;
	char m_sep;
};

// class Datafile Manager
class datfile_manager
{
public:
	// construction/destruction
	datfile_manager(running_machine&, ui_options&);

	// getters
	running_machine& machine() const { return m_machine; }

	// actions
	void load_data_info(const game_driver*, std::string&, std::string) const;
	void load_command_info(std::string&, std::string const&) const;
	void load_software_info(std::string const&, std::string&, std::string const&, std::string const&) const;
	void command_sub_menu(const game_driver*, std::vector<std::string>&) const;
	static bool has_software(std::string const&, std::string const&, std::string const&);

private:
	using drvindex = std::unordered_map<std::string, long>;
	using dataindex = std::unordered_map<const game_driver *, long>;
	using swindex = std::unordered_map<std::string, drvindex>;
	using fileptr = std::unique_ptr<FILE, int(*)(FILE*)>;

	// global index
	static dataindex m_histidx, m_mameidx, m_messidx, m_cmdidx, m_sysidx, m_storyidx, m_ginitidx;
	static drvindex m_drvidx, m_messdrvidx, m_menuidx;
	static swindex m_swindex;

	// internal helpers
	void init_history(fileptr&&) const;
	void init_mameinfo(fileptr&&) const;
	void init_messinfo(fileptr&&) const;
	void init_command(fileptr&&) const;
	void init_sysinfo(fileptr&&) const;
	void init_story(fileptr&&) const;
	void init_gameinit(fileptr&&) const;

	fileptr parseopen(char const*) const;

	int index_mame_mess_info(fileptr&&, dataindex&, drvindex&, int&) const;
	int index_datafile(fileptr&&, dataindex&, int&) const;
	void index_menuidx(fileptr&&, game_driver const*, dataindex const&, drvindex&) const;

	static long const* find_software(std::string const&, std::string const&, std::string const&);

	void load_data_text(FILE*, game_driver const*, std::string&, dataindex const&, std::string const&) const;
	static void load_driver_text(FILE*, game_driver const*, std::string&, drvindex const&, std::string const&);

	// internal state
	running_machine& m_machine; // reference to our machine
	ui_options& m_options;
	static bool first_run;
};
} // namespace nanamame

#endif  // NANAMAME_DATFILE_H
