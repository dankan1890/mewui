// license:BSD-3-Clause
// copyright-holders:Dankan1890
/***************************************************************************

    mewui/datfile.h

    MEWUI DATs manager.

***************************************************************************/

#pragma once

#ifndef __MEWUI_DATFILE_H__
#define __MEWUI_DATFILE_H__

//-------------------------------------------------
//  STRUCTURES
//-------------------------------------------------
struct tDatafileIndex
{
	long              offset;
	const game_driver *driver;
};

struct sDataDrvIndex
{
	long        offset;
	std::string name;
};

struct tMenuIndex
{
	UINT64      offset;
	std::string menuitem;
};

struct SoftwareItem
{
	std::string softname;
	long        offset;
};

struct SoftwareListIndex
{
	std::string               listname;
	std::vector<SoftwareItem> items;
};

//-------------------------------------------------
//  Datafile Manager
//-------------------------------------------------
class datfile_manager
{
public:

	// construction/destruction
	datfile_manager(running_machine &machine);

	// getters
	running_machine &machine() const { return m_machine; }

	// actions
	void load_data_info(const game_driver *drv, std::string &buffer, int type);
	void load_command_info(std::string &buffer, const int sel);
	void load_software_info(const char *softlist, std::string &buffer, const char *softname);
	void command_sub_menu(const game_driver *drv, std::vector<std::string> &menuitems);

	std::string rev_history() const { return m_history_rev; }
	std::string rev_mameinfo() const { return m_mame_rev; }
	std::string rev_messinfo() const { return m_mess_rev; }
	std::string rev_sysinfo() const { return m_sysinfo_rev; }
	std::string rev_storyinfo() const { return m_story_rev; }

private:

	// global index
	std::vector<tDatafileIndex> m_histidx, m_mameidx, m_messidx, m_cmdidx, m_sysidx, m_storyidx;
	std::vector<sDataDrvIndex> m_drvidx, m_messdrvidx;
	std::vector<tMenuIndex> m_menuidx;
	std::vector<SoftwareListIndex> m_swindex;

	// internal helpers
	void init_history();
	void init_mameinfo();
	void init_messinfo();
	void init_command();
	void init_sysinfo();
	void init_storyinfo();

	bool ParseOpen(const char *filename);

	int index_mame_mess_info(std::vector<tDatafileIndex> &index, std::vector<sDataDrvIndex> &index_drv, int &drvcount);
	int index_datafile(std::vector<tDatafileIndex> &index, int &swcount);
	void index_menuidx(const game_driver *drv, std::vector<tDatafileIndex> &idx, std::vector<tMenuIndex> &index);

	void load_data_text(const game_driver *drv, std::string &buffer, std::vector<tDatafileIndex> &idx, const char *tag);
	void load_driver_text(const game_driver *drv, std::string &buffer, std::vector<sDataDrvIndex> &idx, const char *tag);

	int find_or_allocate(std::string name);

	// internal state
	running_machine     &m_machine;             // reference to our machine
	std::string         m_fullpath;
	std::string         m_history_rev, m_mame_rev, m_mess_rev, m_sysinfo_rev, m_story_rev;
};


#endif  /* __MEWUI_DATFILE_H__ */
