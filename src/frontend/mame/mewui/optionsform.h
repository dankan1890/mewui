// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	mewui/options.h

	MEWUI options forms.

***************************************************************************/
#pragma once

#ifndef MEWUI_OPTIONS_FORMS_H
#define MEWUI_OPTIONS_FORMS_H

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/label.hpp>

using namespace nana;
class emu_options;
class mame_ui_manager;

namespace mewui 
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

class dir_form : public form
{
public:
	explicit dir_form(window, emu_options&, std::unique_ptr<mame_ui_manager>&);

private:
	class panel_dir : public panel<true>
	{
	public:
		explicit panel_dir(window);
		button m_ok{ *this, "OK" }, m_cancel{ *this, "Cancel" };

	private:
		place m_place{ *this };
	};

	listbox m_listbox{ *this };
	combox m_combox{ *this };
	panel_dir m_panel{ *this };
	std::unique_ptr<mame_ui_manager>& m_ui;
	emu_options& m_options;
};

class folderform : public form
{
public:
	explicit folderform(window);

private:
	treebox m_tb{ *this };
	label m_lbl{ *this, "Select Directory" };
	button m_ok{ *this, "OK" }, m_cancel{ *this, "Cancel" };
};

} // namespace mewui
#endif