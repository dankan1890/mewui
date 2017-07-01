// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

    nanamame/datfile.cpp

    NANAMAME DATs manager.

***************************************************************************/

#include "emu.h"
#include "datfile.h"
#include "drivenum.h"
#include "ui/moptions.h"
#include "ui/utils.h"
#include <utility>

namespace nanamame
{
namespace
{
//-------------------------------------------------
//  TAGS
//-------------------------------------------------
std::string const DATAFILE_TAG("$");
std::string const TAG_BIO("$bio");
std::string const TAG_INFO("$info");
std::string const TAG_MAME("$mame");
std::string const TAG_COMMAND("$cmd");
std::string const TAG_END("$end");
std::string const TAG_DRIVER("$drv");
std::string const TAG_STORY("$story");
std::string const TAG_HISTORY_R("## REVISION:");
std::string const TAG_MAMEINFO_R("# MAMEINFO.DAT");
std::string const TAG_MESSINFO_R("#     MESSINFO.DAT");
std::string const TAG_SYSINFO_R("# This file was generated on");
std::string const TAG_STORY_R("# version");
std::string const TAG_COMMAND_SEPARATOR("-----------------------------------------------");
std::string const TAG_GAMEINIT_R("# GAMEINIT.DAT");
} // anonymous namespace

//-------------------------------------------------
//  Statics
//-------------------------------------------------
datfile_manager::dataindex datfile_manager::m_histidx;
datfile_manager::dataindex datfile_manager::m_mameidx;
datfile_manager::dataindex datfile_manager::m_messidx;
datfile_manager::dataindex datfile_manager::m_cmdidx;
datfile_manager::dataindex datfile_manager::m_sysidx;
datfile_manager::dataindex datfile_manager::m_storyidx;
datfile_manager::dataindex datfile_manager::m_ginitidx;
datfile_manager::drvindex datfile_manager::m_drvidx;
datfile_manager::drvindex datfile_manager::m_messdrvidx;
datfile_manager::drvindex datfile_manager::m_menuidx;
datfile_manager::swindex datfile_manager::m_swindex;
bool datfile_manager::first_run = true;

#define opendatsfile(f) do { fileptr datfile = parseopen(#f".dat"); if (datfile) init_##f(std::move(datfile)); } while (false)

//-------------------------------------------------
// ctor
//-------------------------------------------------
datfile_manager::datfile_manager(running_machine& machine, ui_options& moptions)
	: m_machine(machine)
	, m_options(moptions)
{
	if (first_run)
	{
		first_run = false;
		opendatsfile(mameinfo);
		opendatsfile(command);
		opendatsfile(story);
		opendatsfile(messinfo);
		opendatsfile(sysinfo);
		opendatsfile(history);
		opendatsfile(gameinit);
	}
}

//-------------------------------------------------
//  initialize sysinfo.dat index
//-------------------------------------------------
void datfile_manager::init_sysinfo(fileptr&& fp) const
{
	auto swcount = 0;
	auto count = index_datafile(std::move(fp), m_sysidx, swcount);
	osd_printf_verbose("Sysinfo.dat games found = %i\n", count);
}

//-------------------------------------------------
//  initialize story.dat index
//-------------------------------------------------
void datfile_manager::init_story(fileptr&& fp) const
{
	auto swcount = 0;
	auto count = index_datafile(std::move(fp), m_storyidx, swcount);
	osd_printf_verbose("Story.dat games found = %i\n", count);
}

//-------------------------------------------------
//  initialize history.dat index
//-------------------------------------------------
void datfile_manager::init_history(fileptr&& fp) const
{
	auto swcount = 0;
	auto count = index_datafile(std::move(fp), m_histidx, swcount);
	osd_printf_verbose("History.dat systems found = %i\n", count);
	osd_printf_verbose("History.dat software packages found = %i\n", swcount);
}

//-------------------------------------------------
//  initialize gameinit.dat index
//-------------------------------------------------
void datfile_manager::init_gameinit(fileptr&& fp) const
{
	auto swcount = 0;
	drvindex tmp;
	auto count = index_mame_mess_info(std::move(fp), m_ginitidx, tmp, swcount);
	osd_printf_verbose("Gameinit.dat games found = %i\n", count);
}

//-------------------------------------------------
//  initialize mameinfo.dat index
//-------------------------------------------------
void datfile_manager::init_mameinfo(fileptr&& fp) const
{
	auto drvcount = 0;
	auto count = index_mame_mess_info(std::move(fp), m_mameidx, m_drvidx, drvcount);
	osd_printf_verbose("Mameinfo.dat games found = %i\n", count);
	osd_printf_verbose("Mameinfo.dat drivers found = %d\n", drvcount);
}

//-------------------------------------------------
//  initialize messinfo.dat index
//-------------------------------------------------
void datfile_manager::init_messinfo(fileptr&& fp) const
{
	auto drvcount = 0;
	auto count = index_mame_mess_info(std::move(fp), m_messidx, m_messdrvidx, drvcount);
	osd_printf_verbose("Messinfo.dat games found = %i\n", count);
	osd_printf_verbose("Messinfo.dat drivers found = %d\n", drvcount);
}

//-------------------------------------------------
//  initialize command.dat index
//-------------------------------------------------
void datfile_manager::init_command(fileptr&& fp) const
{
	auto swcount = 0;
	auto count = index_datafile(std::move(fp), m_cmdidx, swcount);
	osd_printf_verbose("Command.dat games found = %i\n", count);
}

bool datfile_manager::has_software(std::string const& softlist, std::string const& softname, std::string const& parentname)
{
	return bool(find_software(softlist, softname, parentname));
}

long const* datfile_manager::find_software(std::string const& softlist, std::string const& softname, std::string const& parentname)
{
	// Find software in software list index
	auto const software(m_swindex.find(softlist));
	if (software == m_swindex.end())
		return nullptr;

	auto itemsiter = software->second.find(softname);
	if ((itemsiter == software->second.end()) && !parentname.empty())
		itemsiter = software->second.find(parentname);

	return (itemsiter != software->second.end()) ? &itemsiter->second : nullptr;
}

//-------------------------------------------------
//  load software info
//-------------------------------------------------
void datfile_manager::load_software_info(std::string const& softlist, std::string& buffer, std::string const& softname, std::string const& parentname) const
{
	if (m_swindex.empty())
		return;

	// Load history text
	auto const datfile = parseopen("history.dat");
	if (datfile)
	{
		// Find software in software list index
		auto const s_offset = find_software(softlist, softname, parentname);
		if (!s_offset)
			return;

		char rbuf[64 * 1024];
		std::fseek(datfile.get(), *s_offset, SEEK_SET);
		std::string readbuf;
		while (std::fgets(rbuf, 64 * 1024, datfile.get()) != nullptr)
		{
			readbuf = chartrimcarriage(rbuf);

			// end entry when a end tag is encountered
			if (readbuf == TAG_END)
				break;

			// add this string to the buffer
			buffer.append(readbuf).append("\n");
		}
	}
}

//-------------------------------------------------
//  load_data_info
//-------------------------------------------------
void datfile_manager::load_data_info(const game_driver* drv, std::string& buffer, std::string type) const
{
	dataindex const* index_idx = nullptr;
	drvindex const* driver_idx = nullptr;
	std::string const* tag = nullptr;
	std::string filename;

	if (type == "History")
	{
		filename = "history.dat";
		tag = &TAG_BIO;
		index_idx = &m_histidx;
	}
	if (type == "MameInfo")
	{
		filename = "mameinfo.dat";
		tag = &TAG_MAME;
		index_idx = &m_mameidx;
		driver_idx = &m_drvidx;
	}

	if (type == "SysInfo")
	{
		filename = "sysinfo.dat";
		tag = &TAG_BIO;
		index_idx = &m_sysidx;
	}
	if (type == "MessInfo")
	{
		filename = "messinfo.dat";
		tag = &TAG_MAME;
		index_idx = &m_messidx;
		driver_idx = &m_messdrvidx;
	}
	if (type == "MameScore")
	{
		filename = "story.dat";
		tag = &TAG_STORY;
		index_idx = &m_storyidx;
	}
	if (type == "Machine Init")
	{
		filename = "gameinit.dat";
		tag = &TAG_MAME;
		index_idx = &m_ginitidx;
	}

	auto const datfile = parseopen(filename.c_str());
	if (datfile)
	{
		load_data_text(datfile.get(), drv, buffer, *index_idx, *tag);

		// load driver info
		if (driver_idx && !driver_idx->empty())
			load_driver_text(datfile.get(), drv, buffer, *driver_idx, TAG_DRIVER);

		// cleanup mameinfo and sysinfo double line spacing
		if ((*tag == TAG_MAME && type != "Machine Init") || type == "SysInfo")
			strreplace(buffer, "\n\n", "\n");
	}
}

//-------------------------------------------------
//  load a game text into the buffer
//-------------------------------------------------
void datfile_manager::load_data_text(FILE* fp, game_driver const* drv, std::string& buffer, dataindex const& idx, std::string const& tag) const
{
	auto itemsiter = idx.find(drv);
	if (itemsiter == idx.end())
	{
		auto cloneof = driver_list::non_bios_clone(*drv);
		if (cloneof == -1)
			return;
		else
		{
			auto c_drv = &driver_list::driver(cloneof);
			itemsiter = idx.find(c_drv);
			if (itemsiter == idx.end())
				return;
		}
	}

	auto s_offset = itemsiter->second;
	std::fseek(fp, s_offset, SEEK_SET);
	char rbuf[64 * 1024];
	std::string readbuf;
	while (std::fgets(rbuf, 64 * 1024, fp) != nullptr)
	{
		readbuf = chartrimcarriage(rbuf);

		// end entry when a end tag is encountered
		if (readbuf == TAG_END)
			break;

		// continue if a specific tag is encountered
		if (readbuf == tag)
			continue;

		// add this string to the buffer
		buffer.append(readbuf).append("\n");
	}
}

//-------------------------------------------------
//  load a driver name and offset into an
//  indexed array
//-------------------------------------------------
void datfile_manager::load_driver_text(FILE* fp, game_driver const* drv, std::string& buffer, drvindex const& idx, std::string const& tag)
{
	auto s(core_filename_extract_base(drv->source_file));
	auto index = idx.find(s);

	// if driver not found, return
	if (index == idx.end())
		return;

	buffer.append("\n--- DRIVER INFO ---\n").append("Driver: ").append(s).append("\n\n");
	auto s_offset = index->second;
	std::fseek(fp, s_offset, SEEK_SET);
	char rbuf[64 * 1024];
	std::string readbuf;
	while (std::fgets(rbuf, 64 * 1024, fp) != nullptr)
	{
		readbuf = chartrimcarriage(rbuf);

		// end entry when a end tag is encountered
		if (readbuf == TAG_END)
			break;

		// continue if a specific tag is encountered
		if (readbuf == tag)
			continue;

		// add this string to the buffer
		buffer.append(readbuf).append("\n");
	}
}

//-------------------------------------------------
//  load a game name and offset into an
//  indexed array (mameinfo)
//-------------------------------------------------
int datfile_manager::index_mame_mess_info(fileptr&& fp, dataindex& index, drvindex& index_drv, int& drvcount) const
{
	auto t_info = TAG_INFO.size();
	char rbuf[64 * 1024];
	std::string readbuf, xid, name;

	while (std::fgets(rbuf, 64 * 1024, fp.get()) != nullptr)
	{
		readbuf = chartrimcarriage(rbuf);
		if (readbuf.compare(0, t_info, TAG_INFO) == 0)
		{
			// TAG_INFO
			std::fgets(rbuf, 64 * 1024, fp.get());
			xid = chartrimcarriage(rbuf);
			name = readbuf.substr(t_info + 1);
			if (xid == TAG_MAME)
			{
				// validate driver
				auto game_index = driver_list::find(name.c_str());
				if (game_index != -1)
					index.emplace(&driver_list::driver(game_index), std::ftell(fp.get()));
			}
			else if (xid == TAG_DRIVER)
			{
				index_drv.emplace(name, std::ftell(fp.get()));
				drvcount++;
			}
		}
	}
	return index.size();
}

//-------------------------------------------------
//  load a game name and offset into an
//  indexed array
//-------------------------------------------------
int datfile_manager::index_datafile(fileptr&& fp, dataindex& index, int& swcount) const
{
	std::string readbuf;
	auto const t_info = TAG_INFO.size();
	auto const t_bio = TAG_BIO.size();
	char rbuf[64 * 1024];
	while (std::fgets(rbuf, 64 * 1024, fp.get()) != nullptr)
	{
		readbuf = chartrimcarriage(rbuf);
		if (readbuf.compare(0, t_info, TAG_INFO) == 0)
		{
			// search for game info
			auto rd = readbuf.substr(t_info + 1);
			tokenizer gamelist{ rd, ',' };
			std::string e;
			while (gamelist.next(e))
			{
				auto game_index = driver_list::find(e.c_str());
				if (game_index != -1)
					index.emplace(&driver_list::driver(game_index), std::ftell(fp.get()));
			}
		}
		else if (!readbuf.empty() && readbuf[0] == DATAFILE_TAG[0])
		{
			// search for software info
			std::fgets(rbuf, 64 * 1024, fp.get());
			std::string readbuf_2(chartrimcarriage(rbuf));
			if (readbuf_2.compare(0, t_bio, TAG_BIO) == 0)
			{
				auto eq_sign = readbuf.find('=');
				auto s_list(readbuf.substr(1, eq_sign - 1));
				auto s_roms(readbuf.substr(eq_sign + 1));
				tokenizer t_lists{ s_list, ',' };
				tokenizer t_roms{ s_roms, ',' };
				std::string li;
				while (t_lists.next(li))
				{
					std::string ro;
					while (t_roms.next(ro))
						m_swindex[li].emplace(ro, std::ftell(fp.get()));
				}
				swcount++;
			}
		}
	}
	return index.size();
}

//---------------------------------------------------------
//  parseopen - Open up file for reading
//---------------------------------------------------------
datfile_manager::fileptr datfile_manager::parseopen(const char* filename) const
{
	emu_file file(m_options.history_path(), OPEN_FLAG_READ);
	if (file.open(filename) != osd_file::error::NONE)
		return fileptr(nullptr, &std::fclose);

	std::string const fullpath = file.fullpath();
	file.close();
	fileptr result(std::fopen(fullpath.c_str(), "rb"), &std::fclose);

	fgetc(result.get());
	fseek(result.get(), 0, SEEK_SET);
	return result;
}

//-------------------------------------------------
//  create the menu index
//-------------------------------------------------
void datfile_manager::index_menuidx(fileptr&& fp, const game_driver* drv, dataindex const& idx, drvindex& index) const
{
	auto itemsiter = idx.find(drv);
	if (itemsiter == idx.end())
	{
		auto const cloneof = driver_list::non_bios_clone(*drv);
		if (cloneof == -1)
			return;

		auto const c_drv = &driver_list::driver(cloneof);
		if ((itemsiter = idx.find(c_drv)) == idx.end())
			return;
	}

	// seek to correct point in datafile
	auto const s_offset = itemsiter->second;
	std::fseek(fp.get(), s_offset, SEEK_SET);
	auto const tinfo = TAG_INFO.size();
	char rbuf[64 * 1024];
	std::string readbuf;
	while (std::fgets(rbuf, 64 * 1024, fp.get()) != nullptr)
	{
		readbuf = chartrimcarriage(rbuf);

		if (!core_strnicmp(TAG_INFO.c_str(), readbuf.c_str(), tinfo))
			break;

		// TAG_COMMAND identifies the driver
		if (readbuf == TAG_COMMAND)
		{
			std::fgets(rbuf, 64 * 1024, fp.get());
			chartrimcarriage(rbuf);
			index.emplace(rbuf, std::ftell(fp.get()));
		}
	}
}

//-------------------------------------------------
//  load command text into the buffer
//-------------------------------------------------
void datfile_manager::load_command_info(std::string& buffer, std::string const& sel) const
{
	auto const datfile = parseopen("command.dat");
	if (datfile)
	{
		// open and seek to correct point in datafile
		auto const offset = m_menuidx.at(sel);
		std::fseek(datfile.get(), offset, SEEK_SET);
		char rbuf[64 * 1024];
		std::string readbuf;
		while (std::fgets(rbuf, 64 * 1024, datfile.get()) != nullptr)
		{
			readbuf = chartrimcarriage(rbuf);

			// skip separator lines
			if (readbuf == TAG_COMMAND_SEPARATOR)
				continue;

			// end entry when a tag is encountered
			if (readbuf == TAG_END)
				break;

			// add this string to the buffer
			buffer.append(readbuf).append("\n");;
		}
	}
}

//-------------------------------------------------
//  load submenu item for command.dat
//-------------------------------------------------
void datfile_manager::command_sub_menu(const game_driver* drv, std::vector<std::string>& menuitems) const
{
	auto datfile = parseopen("command.dat");
	if (datfile)
	{
		m_menuidx.clear();
		index_menuidx(std::move(datfile), drv, m_cmdidx, m_menuidx);
		menuitems.reserve(m_menuidx.size());
		for (auto const& elem : m_menuidx)
			menuitems.push_back(elem.first);
	}
}

// class tokenizer
tokenizer::tokenizer(std::string const& searchpath, char sep)
	: m_searchpath(searchpath)
	, m_current(m_searchpath.cbegin())
	, m_is_first(true)
	, m_sep(sep) { }

bool tokenizer::next(std::string& buffer)
{
	// if none left, return FALSE to indicate we are done
	if (!m_is_first && (m_searchpath.cend() == m_current))
		return false;

	// copy up to the next separator
	auto const sep(std::find(m_current, m_searchpath.cend(), m_sep));
	buffer.assign(m_current, sep);
	m_current = sep;
	if (m_searchpath.cend() != m_current)
		++m_current;

	// bump the index and return TRUE
	m_is_first = false;
	return true;
}

void tokenizer::reset()
{
	m_current = m_searchpath.cbegin();
	m_is_first = true;
}
} // namespace nanamame
