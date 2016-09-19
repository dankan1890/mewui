// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	mewui/mainform.h

	MEWUI Main Form.

***************************************************************************/
#pragma once

#ifndef MEWUI_MAIN_FORM_H
#define MEWUI_MAIN_FORM_H

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
#include <set>

using namespace nana;
struct game_driver;
class emu_options;
class ui_options;
class mame_ui_manager;

namespace mewui
{
	struct ci_less
	{
		bool operator() (const std::string &s1, const std::string &s2) const
		{
			return (core_stricmp(s1.c_str(), s2.c_str()) < 0);
		}
	};

	class tab_page_picturebox : public panel<true>
	{
	public:
		explicit tab_page_picturebox(window);
		picture m_picture{ *this };
		combox  m_combox{ *this };

	private:
		drawing dw{ *this };
		place m_place{ *this };
	};

	class tab_page_textbox : public panel<true>
	{
	public:
		explicit tab_page_textbox(window);
		textbox	m_textbox{ *this };
		combox  m_combox{ *this };

	private:
		drawing dw{ *this };
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

	class panel_filters : public panel<true>
	{
	public:
		explicit panel_filters(window);

		combox m_filters{ *this }, m_subfilters{ *this };
		void populate_subfilter(int);

	private:
		label m_filter_label{ *this }, m_subfilter_label{ *this };
		place m_place{ *this };
		std::set<std::string> m_array[3];
	};

	class statusbar : public panel<true>
	{
	public:
		explicit statusbar(window);
		void update(int, std::string);

	private:
		label m_machines{ *this }, m_working{ *this };
		place m_place{ *this };
	};

	class dir_form : public form
	{
	public:
		explicit dir_form(window, emu_options &, std::unique_ptr<mame_ui_manager> &);

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
		std::unique_ptr<mame_ui_manager> &m_ui;
		emu_options &m_options;
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


	// Main class
	class main_form : public form
	{
	public:
		// ctor / dtor
		main_form(running_machine &, const game_driver **, emu_options &, std::unique_ptr<mame_ui_manager> &);

	private:
		// Layout management
		listbox		m_machinebox{ *this };
		statusbar	m_statusbar{ *this };
		menubar		m_menubar{ *this };
		textbox		m_search{ *this };
		button		m_search_button{ *this, "S" };
		menu		m_context_menu;
		notifier	m_notif{ *this };
		paint::font m_font;

		tabbar<std::string> m_tabbar{ *this };
		tabbar<std::string> m_tabsw{ *this };

		panel_filters		 m_filters{ *this };
		tab_page_textbox     m_textpage{ *this };
		tab_page_picturebox	 m_imgpage{ *this };
		tab_page_softwarebox m_swpage{ *this };

		int										m_prev_subfilter = -1;
		const game_driver						**m_system;
		emu_options								&m_options;
		std::string								m_latest_machine;
		std::unique_ptr<datfile_manager>		m_datfile;
		std::unique_ptr<mame_ui_manager>		&m_ui;
		std::map<std::string, std::vector<const game_driver *>, ci_less> m_sortedlist;
		std::string m_exename;

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

		void populate_listbox(const std::string &filter = std::string("All"), const std::string &sub = std::string());

		void update_selection();

		void load_image(const game_driver *);
		void load_data(const game_driver *);

		void start_machine(std::string&);
		void start_software();

		void save_options();
		void perform_search();

		void handle_events();
		void resize_machinebox();
	};

} // namespace mewui

#endif /* MEWUI_MAIN_FORM_H */