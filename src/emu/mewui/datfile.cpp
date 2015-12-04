// license:BSD-3-Clause
// copyright-holders:Dankan1890
/***************************************************************************

    mewui/datfile.c

    MEWUI DATs manager.

***************************************************************************/

#include "emu.h"
#include "drivenum.h"
#include "mewui/datfile.h"
#include "mewui/utils.h"

//-------------------------------------------------
//  TAGS
//-------------------------------------------------
static const char *TAG_BIO = "$bio";
static const char *TAG_INFO = "$info";
static const char *TAG_HISTORY_R = "## REVISION:";
static const char *TAG_MAME = "$mame";
static const char *TAG_COMMAND = "$cmd";
static const char *TAG_END = "$end";
static const char *TAG_DRIVER = "$drv";
static const char *TAG_STORY = "$story";
static const char *TAG_MAMEINFO_R = "# MAMEINFO.DAT";
static const char *TAG_MESSINFO_R = "#     MESSINFO.DAT";
static const char *TAG_SYSINFO_R = "# This file was generated on";
static const char *TAG_STORY_R = "# version";
static const char *DATAFILE_TAG = "$";
static const char *TAG_COMMAND_SEPARATOR = "-----------------------------------------------";

//-------------------------------------------------
// ctor
//-------------------------------------------------
datfile_manager::datfile_manager(running_machine &machine) : m_machine(machine)
{
	if (machine.options().enabled_dats())
	{
		if (ParseOpen("mameinfo.dat"))
			init_mameinfo();

		if (ParseOpen("command.dat"))
			init_command();

		if (ParseOpen("story.dat"))
			init_storyinfo();

		if (ParseOpen("messinfo.dat"))
			init_messinfo();

		if (ParseOpen("sysinfo.dat"))
			init_sysinfo();

		if (ParseOpen("history.dat"))
			init_history();
	}
}

//-------------------------------------------------
//  initialize sysinfo.dat index
//-------------------------------------------------

void datfile_manager::init_sysinfo()
{
	int swcount = 0;
	int count = index_datafile(m_sysidx, swcount);
	osd_printf_verbose("Sysinfo.dat games found = %i\n", count);
	osd_printf_verbose("Rev = %s\n", m_sysinfo_rev.c_str());
}

//-------------------------------------------------
//  initialize story.dat index
//-------------------------------------------------

void datfile_manager::init_storyinfo()
{
	int swcount = 0;
	int count = index_datafile(m_storyidx, swcount);
	osd_printf_verbose("Story.dat games found = %i\n", count);
}

//-------------------------------------------------
//  initialize history.dat index
//-------------------------------------------------

void datfile_manager::init_history()
{
	int swcount = 0;
	int count = index_datafile(m_histidx, swcount);
	osd_printf_verbose("History.dat games found = %i\n", count);
	osd_printf_verbose("History.dat softwares found = %i\n", swcount);
	osd_printf_verbose("Rev = %s\n", m_history_rev.c_str());
}

//-------------------------------------------------
//  initialize mameinfo.dat index
//-------------------------------------------------

void datfile_manager::init_mameinfo()
{
	int drvcount = 0;
	int count = index_mame_mess_info(m_mameidx, m_drvidx, drvcount);
	osd_printf_verbose("Mameinfo.dat games found = %i\n", count);
	osd_printf_verbose("Mameinfo.dat drivers found = %d\n", drvcount);
	osd_printf_verbose("Rev = %s\n", m_mame_rev.c_str());
}

//-------------------------------------------------
//  initialize messinfo.dat index
//-------------------------------------------------

void datfile_manager::init_messinfo()
{
	int drvcount = 0;
	int count = index_mame_mess_info(m_messidx, m_messdrvidx, drvcount);
	osd_printf_verbose("Messinfo.dat games found = %i\n", count);
	osd_printf_verbose("Messinfo.dat drivers found = %d\n", drvcount);
	osd_printf_verbose("Rev = %s\n", m_mess_rev.c_str());
}

//-------------------------------------------------
//  initialize command.dat index
//-------------------------------------------------

void datfile_manager::init_command()
{
	int swcount = 0;
	int count = index_datafile(m_cmdidx, swcount);
	osd_printf_verbose("Command.dat games found = %i\n", count);
}

//-------------------------------------------------
//  load software info
//-------------------------------------------------

void datfile_manager::load_software_info(const char *softlist, std::string &buffer, const char *softname)
{
	// Load history text
	if (ParseOpen("history.dat"))
	{
		if (!m_swindex.empty())
		{
			std::string readbuf;
			bool found = false;
			long s_offset = 0;

			// Find software in software list index
			for (int x = 0; !found && x < m_swindex.size(); x++)
				if (m_swindex[x].listname.compare(softlist) == 0)
					for (int y = 0; !found && y < m_swindex[x].items.size(); y++)
						if (m_swindex[x].items[y].name.compare(softname) == 0)
						{
							found = true;
							s_offset = m_swindex[x].items[y].offset;
						}

			if (!found)
				return;

			std::ifstream myfile(m_fullpath.c_str());
			myfile.seekg(s_offset, myfile.beg);
			size_t tend = strlen(TAG_END);
			while (myfile.good())
			{
				// read from datafile
				clean_getline(myfile, readbuf);

				// end entry when a end tag is encountered
				if (!core_strnicmp(TAG_END, readbuf.c_str(), tend))
					break;

				// add this string to the buffer
				buffer.append(readbuf).append("\n");
			}
			myfile.close();
		}
	}
}

//-------------------------------------------------
//  load_data_info
//-------------------------------------------------

void datfile_manager::load_data_info(const game_driver *drv, std::string &buffer, int type)
{
	std::vector<Drvindex> index_idx;
	std::vector<Itemsindex> driver_idx;
	const char *tag = nullptr;
	std::string filename;

	switch (type)
	{
		case MEWUI_HISTORY_LOAD:
			filename.assign("history.dat");
			tag = TAG_BIO;
			index_idx = m_histidx;
			break;
		case MEWUI_MAMEINFO_LOAD:
			filename.assign("mameinfo.dat");
			tag = TAG_MAME;
			index_idx = m_mameidx;
			driver_idx = m_drvidx;
			break;
		case MEWUI_SYSINFO_LOAD:
			filename.assign("sysinfo.dat");
			tag = TAG_BIO;
			index_idx = m_sysidx;
			break;
		case MEWUI_MESSINFO_LOAD:
			filename.assign("messinfo.dat");
			tag = TAG_MAME;
			index_idx = m_messidx;
			driver_idx = m_messdrvidx;
			break;
		case MEWUI_STORY_LOAD:
			filename.assign("story.dat");
			tag = TAG_STORY;
			index_idx = m_storyidx;
			break;
	}

	if (ParseOpen(filename.c_str()))
	{
		load_data_text(drv, buffer, index_idx, tag);

		// load driver info
		if (!driver_idx.empty())
			load_driver_text(drv, buffer, driver_idx, TAG_DRIVER);

		// cleanup mameinfo double line spacing
		if (tag == TAG_MAME)
			strreplace(buffer, "\n\n", "\n");

	}
}

//-------------------------------------------------
//  load a game text into the buffer
//-------------------------------------------------

void datfile_manager::load_data_text(const game_driver *drv, std::string &buffer, std::vector<Drvindex> &idx, const char *tag)
{
	size_t x = 0;
	for (x = 0; x < idx.size() && idx[x].driver != drv; ++x) ;

	if (x == idx.size())
	{
		int cloneof = driver_list::non_bios_clone(*drv);
		if (cloneof == -1)
			return;
		else
		{
			const game_driver *c_drv = &driver_list::driver(cloneof);
			for (x = 0; x < idx.size() && idx[x].driver != c_drv; ++x) ;
			if (x == idx.size())
				return;
		}
	}

	std::string readbuf;
	std::ifstream myfile(m_fullpath.c_str(), std::ifstream::binary);

	myfile.seekg(idx[x].offset, myfile.beg);
	size_t tend = strlen(TAG_END);
	size_t ttag = strlen(tag);
	while (myfile.good())
	{
		// read from datafile
		clean_getline(myfile, readbuf);

		// end entry when a end tag is encountered
		if (!core_strnicmp(TAG_END, readbuf.c_str(), tend))
			break;

		// continue if a specific tag is encountered
		if (!core_strnicmp(tag, readbuf.c_str(), ttag))
			continue;

		// add this string to the buffer
		buffer.append(readbuf).append("\n");
	}
	myfile.close();
}

//-------------------------------------------------
//  load a driver name and offset into an
//  indexed array
//-------------------------------------------------
void datfile_manager::load_driver_text(const game_driver *drv, std::string &buffer, std::vector<Itemsindex> &idx, const char *tag)
{
	std::string s;
	core_filename_extract_base(s, drv->source_file);
	size_t index = 0;
	for (index = 0; index < idx.size() && idx[index].name != s; ++index) ;

	// if driver not found, return
	if (index == idx.size())
		return;

	std::string readbuf;
	std::ifstream myfile(m_fullpath.c_str(), std::ifstream::binary);
	size_t tend = strlen(TAG_END);
	size_t ttag = strlen(tag);
	myfile.seekg(idx[index].offset, myfile.beg);
	buffer.append("\n--- DRIVER INFO ---\n").append("Driver: ").append(s).append("\n\n");
	while (myfile.good())
	{
		// read from datafile
		clean_getline(myfile, readbuf);

		// end entry when a end tag is encountered
		if (!core_strnicmp(TAG_END, readbuf.c_str(), tend))
			break;

		// continue if a specific tag is encountered
		if (!core_strnicmp(tag, readbuf.c_str(), ttag))
			continue;

		// add this string to the buffer
		buffer.append(readbuf).append("\n");
	}
	myfile.close();
}

//-------------------------------------------------
//  load a game name and offset into an
//  indexed array (mameinfo)
//-------------------------------------------------
int datfile_manager::index_mame_mess_info(std::vector<Drvindex> &index, std::vector<Itemsindex> &index_drv, int &drvcount)
{
	int          count = 0;
	std::string  readbuf, name;
	int          t_mame = strlen(TAG_MAMEINFO_R);
	int          t_mess = strlen(TAG_MESSINFO_R);
	int          t_drv = strlen(TAG_DRIVER);
	int          t_tag = strlen(TAG_MAME);
	int          t_info = strlen(TAG_INFO);

	std::ifstream myfile(m_fullpath.c_str(), std::ifstream::binary);
	if (myfile.is_open())
	{
		// loop through datafile
		while (myfile.good())
		{
			clean_getline(myfile, readbuf);

			if (m_mame_rev.empty() && readbuf.compare(0, t_mame, TAG_MAMEINFO_R) == 0)
			{
				size_t found = readbuf.find(" ", t_mame + 1);
				m_mame_rev = readbuf.substr(t_mame + 1, found - t_mame);
			}
			else if (m_mess_rev.empty() && readbuf.compare(0, t_mess, TAG_MESSINFO_R) == 0)
			{
				size_t found = readbuf.find(" ", t_mess + 1);
				m_mess_rev = readbuf.substr(t_mess + 1, found - t_mess);
			}
			// TAG_INFO
			else if (readbuf.compare(0, t_info, TAG_INFO) == 0)
			{
				std::string xid;
				clean_getline(myfile, xid);

				name = readbuf.substr(t_info + 1);

				if (xid.compare(0, t_tag, TAG_MAME) == 0)
				{
					// validate driver
					int game_index = driver_list::find(name.c_str());
					if (game_index != -1)
					{
						Drvindex idx;
						idx.driver = &driver_list::driver(game_index);
						idx.offset = myfile.tellg();
						index.push_back(idx);
						count++;
					}
				}
				else if (xid.compare(0, t_drv, TAG_DRIVER) == 0)
				{
					Itemsindex idx_drv;
					idx_drv.name = name;
					idx_drv.offset = myfile.tellg();
					index_drv.push_back(idx_drv);
					drvcount++;
				}
			}
		}
		myfile.close();
	}

	return count;
}

//-------------------------------------------------
//  load a game name and offset into an
//  indexed array
//-------------------------------------------------
int datfile_manager::index_datafile(std::vector<Drvindex> &index, int &swcount)
{
	int          count = 0;
	std::string  readbuf, name;
	int          t_hist = strlen(TAG_HISTORY_R);
	int          t_story = strlen(TAG_STORY_R);
	int          t_sysinfo = strlen(TAG_SYSINFO_R);
	int          t_info = strlen(TAG_INFO);
	int          t_bio = strlen(TAG_BIO);
	std::string  carriage("\r\n");

	std::ifstream myfile(m_fullpath.c_str(), std::ifstream::binary);
	if (myfile.is_open())
	{
		// loop through datafile
		while (myfile.good())
		{
			clean_getline(myfile, readbuf);

			if (m_history_rev.empty() && readbuf.compare(0, t_hist, TAG_HISTORY_R) == 0)
			{
				size_t found = readbuf.find(" ", t_hist + 1);
				m_history_rev = readbuf.substr(t_hist + 1, found - t_hist);
			}
			else if (m_sysinfo_rev.empty() && readbuf.compare(0, t_sysinfo, TAG_SYSINFO_R) == 0)
			{
				size_t found = readbuf.find(".", t_sysinfo + 1);
				m_sysinfo_rev = readbuf.substr(t_sysinfo + 1, found - t_sysinfo);
			}
			else if (m_story_rev.empty() && readbuf.compare(0, t_story, TAG_STORY_R) == 0)
			{
				size_t found = readbuf.find_first_of(carriage, t_story + 1);
				m_story_rev.assign(readbuf.substr(t_story + 1, found - t_story));
			}
			// TAG_INFO identifies the driver
			else if (readbuf.compare(0, t_info, TAG_INFO) == 0)
			{
				int curpoint = t_info + 1;
				int ends = readbuf.length();
				while (curpoint < ends)
				{
					// search for comma
					size_t found = readbuf.find(",", curpoint);

					// found it
					if (found != std::string::npos)
					{
						// copy data and validate driver
						int len = found - curpoint;
						name.assign(readbuf.substr(curpoint, len));
						strtrimspace(name);

						// validate driver
						int game_index = driver_list::find(name.c_str());

						if (game_index != -1)
						{
							Drvindex idx;
							idx.driver = &driver_list::driver(game_index);
							idx.offset = myfile.tellg();
							index.push_back(idx);
							count++;
						}

						// update current point
						curpoint = found + 1;
					}
					// if comma not found, copy data while until reach the end of string
					else if (curpoint < ends)
					{
						name.assign(readbuf.substr(curpoint));
						size_t found = name.find_last_not_of(carriage);
						if (found != std::string::npos)
							name.erase(found+1);

						int game_index = driver_list::find(name.c_str());
						if (game_index != -1)
						{
							Drvindex idx;
							idx.driver = &driver_list::driver(game_index);
							idx.offset = myfile.tellg();
							index.push_back(idx);
							count++;
						}

						// update current point
						curpoint = ends;
					}
				}
			}
			// search for software info
			else if (!readbuf.empty() && readbuf[0] == DATAFILE_TAG[0])
			{
				std::string readbuf_2;
				clean_getline(myfile, readbuf_2);

				// TAG_BIO identifies software list
				if (readbuf_2.compare(0, t_bio, TAG_BIO) == 0)
				{
					size_t eq_sign = readbuf.find("=");
					std::string s_list(readbuf.substr(1, eq_sign - 1));
					std::string s_roms(readbuf.substr(eq_sign + 1));
					int ends = s_list.length();
					int curpoint = 0;

					while (curpoint < ends)
					{
						size_t found = s_list.find(",", curpoint);

						// found it
						if (found != std::string::npos)
						{
							int len = found - curpoint;
							name.assign(s_list.substr(curpoint, len));
							strtrimspace(name);
							curpoint = found + 1;
						}
						else
						{
							name.assign(s_list);
							strtrimspace(name);
							curpoint = ends;
						}

						// search for a software list in the index, if not found then allocates
						int list_index = find_or_allocate(name);
						int cpoint = 0;
						int cends = s_roms.length();

						while (cpoint < cends)
						{
							// search for comma
							size_t found = s_roms.find(",", cpoint);

							// found it
							if (found != std::string::npos)
							{
								// copy data
								int len = found - cpoint;
								name.assign(s_roms.substr(cpoint, len));
								strtrimspace(name);

								// add a SoftwareItem
								Itemsindex t_temp;
								t_temp.name.assign(name);
								t_temp.offset = myfile.tellg();
								m_swindex[list_index].items.push_back(t_temp);

								// update current point
								cpoint = found + 1;
								swcount++;
							}
							else
							{
								// if reach the end, bail out
								if (s_roms[cpoint] == '\r' || s_roms[cpoint] == '\n')
									break;

								// copy data
								name.assign(s_roms.substr(cpoint));
								size_t found = name.find_last_not_of(carriage);
								if (found != std::string::npos)
									name.erase(found+1);

								// add a SoftwareItem
								Itemsindex t_temp;
								t_temp.name.assign(name);
								t_temp.offset = myfile.tellg();
								m_swindex[list_index].items.push_back(t_temp);

								// update current point
								cpoint = cends;
								swcount++;
							}
						}
					}
				}
			}
		}
		myfile.close();
	}
	return count;
}

//-------------------------------------------------
//  open up file for reading
//-------------------------------------------------

bool datfile_manager::ParseOpen(const char *filename)
{
	// Open file up in binary mode
	emu_file fp(machine().options().history_path(), OPEN_FLAG_READ);

	if (fp.open(filename) == FILERR_NONE)
	{
		m_fullpath.assign(fp.fullpath());
		fp.close();
		return true;
	}
	return false;
}

//-------------------------------------------------
//  create the menu index
//-------------------------------------------------

void datfile_manager::index_menuidx(const game_driver *drv, std::vector<Drvindex> &idx, std::vector<Itemsindex> &index)
{
	std::string readbuf;
	size_t x = 0;

	// find driver in datafile index
	for (x = 0; x < idx.size() && idx[x].driver != drv; ++x) ;

	if (x == idx.size())
	{
		int cl = driver_list::clone(*drv);
		if (cl == -1)
			return;

		const game_driver *gdrv = &driver_list::driver(cl);
		for (x = 0; x < idx.size() && idx[x].driver != gdrv; ++x) ;

		if (x == idx.size())
			return;
	}

	// seek to correct point in datafile
	std::ifstream myfile(m_fullpath.c_str(), std::ifstream::binary);
	myfile.seekg(idx[x].offset, myfile.beg);
	size_t tinfo = strlen(TAG_INFO);
	size_t tcommand = strlen(TAG_COMMAND);
	while (clean_getline(myfile, readbuf))
	{
		if (!core_strnicmp(TAG_INFO, readbuf.c_str(), tinfo))
			break;

		// TAG_COMMAND identifies the driver
		if (!core_strnicmp(TAG_COMMAND, readbuf.c_str(), tcommand))
		{
			clean_getline(myfile, readbuf);
			Itemsindex m_idx;
			m_idx.name = readbuf;
			m_idx.offset = myfile.tellg();
			index.push_back(m_idx);
		}
	}
	myfile.close();
}

//-------------------------------------------------
//  load command text into the buffer
//-------------------------------------------------

void datfile_manager::load_command_info(std::string &buffer, const int sel)
{
	if (ParseOpen("command.dat"))
	{
		std::string readbuf;
		size_t tcs = strlen(TAG_COMMAND_SEPARATOR);
		size_t tend = strlen(TAG_END);

		// open and seek to correct point in datafile
		std::ifstream myfile(m_fullpath.c_str());
		myfile.seekg(m_menuidx[sel].offset, myfile.beg);
		while (myfile.good())
		{
			// read from datafile
			clean_getline(myfile, readbuf);

			// skip separator lines
			if (!core_strnicmp(TAG_COMMAND_SEPARATOR, readbuf.c_str(), tcs))
				continue;

			// end entry when a tag is encountered
			if (!core_strnicmp(TAG_END, readbuf.c_str(), tend))
				break;

			// add this string to the buffer
			buffer.append(readbuf).append("\n");;
		}
		myfile.close();
	}
}

//-------------------------------------------------
//  load submenu item for command.dat
//-------------------------------------------------

void datfile_manager::command_sub_menu(const game_driver *drv, std::vector<std::string> &menuitems)
{
	if (ParseOpen("command.dat"))
	{
		m_menuidx.clear();
		index_menuidx(drv, m_cmdidx, m_menuidx);
		for (auto & elem : m_menuidx)
			menuitems.push_back(elem.name);
	}
}

int datfile_manager::find_or_allocate(std::string &name)
{
	int x = 0;
	for (; x < m_swindex.size(); x++)
		if (m_swindex[x].listname.compare(name) == 0)
			return x;

	if (x == m_swindex.size())
	{
		SoftwareListIndex tmp;
		tmp.listname.assign(name);
		m_swindex.push_back(tmp);
	}
	return x;
}
