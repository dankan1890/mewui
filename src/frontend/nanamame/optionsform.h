// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nanamame/optionsform.h

	NANAMAME options forms.

***************************************************************************/
#pragma once

#ifndef NANAMAME_OPTIONS_FORMS_H
#define NANAMAME_OPTIONS_FORMS_H

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include "custom.h"

using namespace nana;
class emu_options;
class mame_ui_manager;

namespace nanamame 
{

static const std::pair<std::string, std::string> arts_info[] = 
{
	{ "Snapshots", OPTION_SNAPSHOT_DIRECTORY },
	{ "Cabinets", OPTION_CABINETS_PATH },
	{ "Control Panels", OPTION_CPANELS_PATH },
	{ "PCBs", OPTION_PCBS_PATH },
	{ "Flyers", OPTION_FLYERS_PATH },
	{ "Titles", OPTION_TITLES_PATH },
	{ "Ends", OPTION_ENDS_PATH },
	{ "Artwork Preview", OPTION_ARTPREV_PATH },
	{ "Bosses", OPTION_BOSSES_PATH },
	{ "Logos", OPTION_LOGOS_PATH },
	{ "Versus", OPTION_VERSUS_PATH },
	{ "Game Over", OPTION_GAMEOVER_PATH },
	{ "HowTo", OPTION_HOWTO_PATH },
	{ "Scores", OPTION_SCORES_PATH },
	{ "Select", OPTION_SELECT_PATH },
	{ "Marquees", OPTION_MARQUEES_PATH },
	{ "Covers", OPTION_COVER_PATH }
};

static const std::pair<std::string, std::string> extra_path[] =
{
	{ "Roms", OPTION_MEDIAPATH },
	{ "Software", OPTION_SWPATH },
	{ "INIs", OPTION_INIPATH },
	{ "Languages", OPTION_LANGUAGEPATH },
	{ "Plugins", OPTION_PLUGINSPATH },
	{ "Fonts", OPTION_FONTPATH },
	{ "Cheats", OPTION_CHEATPATH },
	{ "Controls", OPTION_CTRLRPATH },
	{ "Arts", OPTION_ARTPATH },
	{ "Samples", OPTION_SAMPLEPATH },
	{ "Hashs", OPTION_HASHPATH },
	{ "Icons", OPTION_ICONS_PATH },
	{ "UI", OPTION_UI_PATH }
};

class okcancel_panel : public panel<true>
{
public:
	explicit okcancel_panel(window);
	button m_ok{ *this, "OK" }, m_cancel{ *this, "Cancel" };

private:
	place m_place{ *this };
};


class dir_form : public form
{
public:
	explicit dir_form(window, emu_options&, std::unique_ptr<mame_ui_manager>&);
private:
	listbox m_listbox{ *this };
	combox m_combox{ *this };
	okcancel_panel m_panel{ *this };
	button m_b_add{ *this, "Add" }, m_b_change{ *this, "Edit" }, m_b_del{ *this, "Delete" };
	std::unique_ptr<mame_ui_manager>& m_ui;
	emu_options& m_options;
	std::map<std::string, std::string> m_list;
	combox_item_render m_cir;

	void handle();
};

class folderbox
{
public:
	explicit folderbox(window wd) : wd_(wd) {}

	class folderform : public form
	{
	public:
		explicit folderform(window);
		std::string pick() const { return pick_; };

	private:

		treebox m_tb{ *this };
		label m_lbl{ *this, "Select a Folder" };
		okcancel_panel m_panel{ *this };
		std::string pick_;
		void _m_click(const arg_click& arg);
	};

	std::string show() const;
	std::string operator()() const { return show(); }

private:
	window wd_;

};

class plugin_form : public form
{
public:
	explicit plugin_form(window, emu_options&);

private:
	
	group m_group{ *this };
	okcancel_panel m_panel{ *this };
	textbox tb{ *this };
	emu_options& m_options;
};

} // namespace nanamame
#endif