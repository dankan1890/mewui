// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nanamame/mainform.h

	NANAMAME Main Form.

***************************************************************************/
#pragma once

#ifndef NANAMAME_MAIN_FORM_H
#define NANAMAME_MAIN_FORM_H

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/tabbar.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/notifier.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/threads/pool.hpp>
#include "custom.h"

using namespace nana;
struct game_driver;
class emu_options;
class ui_options;
class mame_ui_manager;

namespace nanamame
{
struct ci_less
{
	bool operator()(const std::string& s1, const std::string& s2) const
	{
		return (core_stricmp(s1.c_str(), s2.c_str()) < 0);
	}
};

class tab_page_picturebox : public panel<true>
{
public:
	explicit tab_page_picturebox(window);
	picture m_picture{ *this };
	combox m_combox{ *this };

private:
	combox_item_render m_cir;
	place m_place{ *this };
};

class tab_page_textbox : public panel<true>
{
public:
	explicit tab_page_textbox(window);
	textbox m_textbox{ *this };
	combox m_combox{ *this };

private:
	combox_item_render m_cir;
	place m_place{ *this };
};

class tab_page_softwarebox : public panel<true>
{
public:
	explicit tab_page_softwarebox(window);
	listbox m_softwarebox{ *this };

private:
	place m_place{ *this };
};

class panel_filter : public panel<true>
{
public:
	explicit panel_filter(window);
	treebox m_treebox{ *this };

private:
	//drawing dw{ *this };
	label m_title{ *this, "   Filters" };
	place m_place{ *this };
};

class statusbar : public panel<true>
{
public:
	explicit statusbar(window);
	void update(int);

private:
	label m_machines{ *this };
	place m_place{ *this };
};

// Main class
class main_form : public form
{
public:
	// ctor / dtor
	main_form(running_machine&, const game_driver**, emu_options&, std::unique_ptr<mame_ui_manager>&, std::string&);

private:
	// Layout management
	listbox m_machinebox{ *this };
	statusbar m_statusbar{ *this };
	menubar m_menubar{ *this };
	textbox m_search{ *this };
	button m_search_button{ *this };
	menu m_context_menu;
	notifier m_notif{ *this };
	paint::font m_font;

	tabbar<std::string> m_tabbar{ *this };
	tabbar<std::string> m_tabsw{ *this };

	tab_page_textbox m_textpage{ *this };
	tab_page_picturebox m_imgpage{ *this };
	tab_page_softwarebox m_swpage{ *this };
	panel_filter m_filters{ *this };

	const game_driver** m_system;
	emu_options& m_options;
	std::string m_latest_machine;
	std::unique_ptr<datfile_manager> m_datfile;
	std::unique_ptr<mame_ui_manager>& m_ui;
	std::map<std::string, std::vector<std::pair<const game_driver *, paint::image>>, ci_less> m_sortedlist;
	std::string m_exename;
	int m_resel = 0;
	threads::pool m_pool;
	color m_bgcolor{ "#232323" }, m_tab_color{ 240, 240, 240 }, m_menubar_color{ "#2C2C2C" };
    float m_layout_version = 1.01f;

	// Init widgets
	void init_tabbar();
	void init_machinebox();
	void init_menubar();
	void init_filters();

	// Init menu
	void init_file_menu();
	void init_view_menu();
	void init_options_menu();
	void init_help_menu();
	void init_context_menu();

	void populate_listbox(const std::string& filter = std::string("All"), const std::string& sub = std::string());

	void update_selection(listbox::item_proxy &item);
	void update_selection();

	void load_image(const game_driver* driver);
	void load_data(const game_driver* driver);
	void load_sw_data(std::string list, std::string name, std::string parent);

	void start_machine(std::string&);
	void start_software();

	void save_options();
	void perform_search();

	void handle_events();
	paint::image load_icon(const game_driver *drv) const;
	void refresh_icons();

};
} // namespace nanamame

#endif /* NANAMAME_MAIN_FORM_H */
