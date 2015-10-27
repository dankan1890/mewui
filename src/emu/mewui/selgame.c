// license:BSD-3-Clause
// copyright-holders:Dankan1890
/*********************************************************************

    mewui/selgame.c

    Main MEWUI menu.

*********************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "mewui/datfile.h"
#include "mewui/inifile.h"
#include "mewui/selgame.h"
#include "drivenum.h"
#include "rendfont.h"
#include "uiinput.h"
#include "audit.h"
#include "ui/miscmenu.h"
#include "mewui/datmenu.h"
#include "mewui/dirmenu.h"
#include "mewui/optsmenu.h"
#include "mewui/selector.h"
#include "mewui/selsoft.h"
#include "sound/samples.h"
#include "unzip.h"
#include "mewui/custmenu.h"
#include "info.h"
#include "mewui/utils.h"
#include "mewui/auditmenu.h"
#include "rendutil.h"

static bool first_start = true;

extern const char *dats_info[];
const char *dats_info[] = { "General Info", "History", "Mameinfo", "Sysinfo", "Messinfo", "Command", "Mamescore" };

//-------------------------------------------------
//  sort
//-------------------------------------------------

inline int c_stricmp(const char *s1, const char *s2)
{
	for (;;)
	{
		int c1 = tolower((UINT8)*s1++);
		int c2 = tolower((UINT8)*s2++);
		if (c1 == 0 || c1 != c2)
			return c1 - c2;
	}
}

bool sort_game_list(const game_driver *x, const game_driver *y)
{
	bool clonex = strcmp(x->parent, "0");
	bool cloney = strcmp(y->parent, "0");

	if (!clonex && !cloney)
		return (c_stricmp(x->description, y->description) < 0);

	int cx = -1, cy = -1;
	if (clonex)
	{
		cx = driver_list::find(x->parent);
		if (cx == -1 || (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0)))
			clonex = false;
	}

	if (cloney)
	{
		cy = driver_list::find(y->parent);
		if (cy == -1 || (cy != -1 && ((driver_list::driver(cy).flags & MACHINE_IS_BIOS_ROOT) != 0)))
			cloney = false;
	}

	if (!clonex && !cloney)
		return (c_stricmp(x->description, y->description) < 0);

	else if (clonex && cloney)
	{
		if (!c_stricmp(x->parent, y->parent))
			return (c_stricmp(x->description, y->description) < 0);
		else
			return (c_stricmp(driver_list::driver(cx).description, driver_list::driver(cy).description) < 0);
	}
	else if (!clonex && cloney)
	{
		if (!c_stricmp(x->name, y->parent))
			return true;
		else
			return (c_stricmp(x->description, driver_list::driver(cy).description) < 0);
	}
	else
	{
		if (!c_stricmp(x->parent, y->name))
			return false;
		else
			return (c_stricmp(driver_list::driver(cx).description, y->description) < 0);
	}
}

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_mewui_select_game::ui_mewui_select_game(running_machine &machine, render_container *container, const char *gamename) : ui_menu(machine, container)
{
	std::string error_string, last_filter, sub_filter;

	// load drivers cache
	load_cache_info();

	// build drivers list
	if (!load_available_machines())
		build_available_list();

	// load custom filter
	load_custom_filters();

	if (first_start)
	{
		reselect_last::driver.assign(machine.options().last_used_machine());
		std::string tmp(machine.options().last_used_filter());
		std::size_t found = tmp.find_first_of(",");
		if (found == std::string::npos)
			last_filter = tmp;
		else
		{
			last_filter = tmp.substr(0, found);
			sub_filter = tmp.substr(found + 1);
		}

		for (size_t ind = 0; ind < main_filters::length; ++ind)
			if (last_filter == main_filters::text[ind])
			{
				main_filters::actual = ind;
				break;
			}

		if (main_filters::actual == FILTER_CATEGORY)
			main_filters::actual = FILTER_ALL;
		else if (main_filters::actual == FILTER_MANUFACTURER)
		{
			for (size_t id = 0; id < c_mnfct::ui.size(); ++id)
				if (sub_filter == c_mnfct::ui[id])
					c_mnfct::actual = id;
		}
		else if (main_filters::actual == FILTER_YEAR)
		{
			for (size_t id = 0; id < c_year::ui.size(); ++id)
				if (sub_filter == c_year::ui[id])
					c_year::actual = id;
		}
		else if (main_filters::actual == FILTER_SCREEN)
		{
			for (size_t id = 0; id < c_screen::length; ++id)
				if (sub_filter == c_screen::text[id])
					c_screen::actual = id;
		}
		first_start = false;
	}

	if (!machine.options().remember_last())
		reselect_last::reset();

	machine.options().set_value(OPTION_SNAPNAME, "%g/%i", OPTION_PRIORITY_CMDLINE, error_string);
	machine.options().set_value(OPTION_SOFTWARENAME, "", OPTION_PRIORITY_CMDLINE, error_string);

	mewui_globals::curimage_view = FIRST_VIEW;
	mewui_globals::curdats_view = MEWUI_FIRST_LOAD;
	mewui_globals::switch_image = false;
	mewui_globals::default_image = true;
	ume_filters::actual = machine.options().start_filter();
	mewui_globals::panels_status = machine.options().hide_panels();
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_mewui_select_game::~ui_mewui_select_game()
{
	std::string error_string, last_driver;
	const game_driver *driver = (selected >= 0 && selected < item.size()) ? (const game_driver *)item[selected].ref : NULL;
	if ((FPTR)driver > 2)
		last_driver.assign(driver->name);

	std::string filter(main_filters::text[main_filters::actual]);
	if (main_filters::actual == FILTER_MANUFACTURER)
		filter.append(",").append(c_mnfct::ui[c_mnfct::actual]);
	if (main_filters::actual == FILTER_YEAR)
		filter.append(",").append(c_year::ui[c_year::actual]);
	if (main_filters::actual == FILTER_SCREEN)
		filter.append(",").append(c_screen::text[c_screen::actual]);

	machine().options().set_value(OPTION_START_FILTER, ume_filters::actual, OPTION_PRIORITY_CMDLINE, error_string);
	machine().options().set_value(OPTION_LAST_USED_FILTER, filter.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
	machine().options().set_value(OPTION_LAST_USED_MACHINE, last_driver.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
	machine().options().set_value(OPTION_HIDE_PANELS, mewui_globals::panels_status, OPTION_PRIORITY_CMDLINE, error_string);
	save_game_options(machine());
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void ui_mewui_select_game::handle()
{
	bool check_filter = false;

	// if i have to load datfile, performe an hard reset
	if (mewui_globals::reset)
	{
		mewui_globals::reset = false;
		machine().schedule_hard_reset();
		ui_menu::stack_reset(machine());
		return;
	}

	// if i have to reselect a software, force software list submenu
	if (reselect_last::get())
	{
		const game_driver *driver = (const game_driver *)item[selected].ref;
		ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_select_software(machine(), container, driver)));
		return;
	}

	// ignore pause keys by swallowing them before we process the menu
	ui_input_pressed(machine(), IPT_UI_PAUSE);

	// process the menu
	const ui_menu_event *menu_event = process(UI_MENU_PROCESS_LR_REPEAT);
	if (menu_event != NULL && menu_event->itemref != NULL)
	{
		// reset the error on any future menu_event
		if (ui_error)
			ui_error = false;

		// handle selections
		else if (menu_event->iptkey == IPT_UI_SELECT)
		{
			if (main_filters::actual != FILTER_FAVORITE_GAME)
				inkey_select(menu_event);
			else
				inkey_select_favorite(menu_event);
		}

		// handle UI_LEFT
		else if (menu_event->iptkey == IPT_UI_LEFT)
		{
			// Images
			if (mewui_globals::rpanel == RP_IMAGES && mewui_globals::curimage_view > FIRST_VIEW)
			{
				mewui_globals::curimage_view--;
				mewui_globals::switch_image = true;
				mewui_globals::default_image = false;
			}

			// Infos
			else if (mewui_globals::rpanel == RP_INFOS && mewui_globals::curdats_view > MEWUI_FIRST_LOAD)
			{
				mewui_globals::curdats_view--;
				topline_datsview = 0;
			}
		}

		// handle UI_RIGHT
		else if (menu_event->iptkey == IPT_UI_RIGHT)
		{
			// Images
			if (mewui_globals::rpanel == RP_IMAGES && mewui_globals::curimage_view < LAST_VIEW)
			{
				mewui_globals::curimage_view++;
				mewui_globals::switch_image = true;
				mewui_globals::default_image = false;
			}

			// Infos
			else if (mewui_globals::rpanel == RP_INFOS && mewui_globals::curdats_view < MEWUI_LAST_LOAD)
			{
				mewui_globals::curdats_view++;
				topline_datsview = 0;
			}
		}

		// handle UI_UP_FILTER
		else if (menu_event->iptkey == IPT_UI_UP_FILTER && main_filters::actual > FILTER_FIRST)
		{
			l_hover = main_filters::actual - 1;
			check_filter = true;
		}

		// handle UI_DOWN_FILTER
		else if (menu_event->iptkey == IPT_UI_DOWN_FILTER && main_filters::actual < FILTER_LAST)
		{
			l_hover = main_filters::actual + 1;
			check_filter = true;
		}

		// handle UI_LEFT_PANEL
		else if (menu_event->iptkey == IPT_UI_LEFT_PANEL)
			mewui_globals::rpanel = RP_IMAGES;

		// handle UI_RIGHT_PANEL
		else if (menu_event->iptkey == IPT_UI_RIGHT_PANEL)
			mewui_globals::rpanel = RP_INFOS;

		// escape pressed with non-empty text clears the text
		else if (menu_event->iptkey == IPT_UI_CANCEL && m_search[0] != 0)
		{
			m_search[0] = '\0';
			reset(UI_MENU_RESET_SELECT_FIRST);
		}

		// handle UI_HISTORY
		else if (menu_event->iptkey == IPT_UI_HISTORY && machine().options().enabled_dats())
		{
			if (main_filters::actual != FILTER_FAVORITE_GAME)
			{
				const game_driver *driver = (const game_driver *)menu_event->itemref;
				if ((FPTR)driver > 2)
					ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_HISTORY_LOAD, driver)));
			}
			else
			{
				ui_software_info *swinfo  = (ui_software_info *)menu_event->itemref;
				if ((FPTR)swinfo > 2)
				{
					if (swinfo->startempty == 1)
						ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_HISTORY_LOAD, swinfo->driver)));
					else
						ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_history_sw(machine(), container, swinfo)));
				}
			}
		}

		// handle UI_MAMEINFO
		else if (menu_event->iptkey == IPT_UI_MAMEINFO && machine().options().enabled_dats())
		{
			if (main_filters::actual != FILTER_FAVORITE_GAME)
			{
				const game_driver *driver = (const game_driver *)menu_event->itemref;
				if ((FPTR)driver > 2)
				{
					if ((driver->flags & MACHINE_TYPE_ARCADE) != 0)
						ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_MAMEINFO_LOAD, driver)));
					else
						ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_MESSINFO_LOAD, driver)));
				}
			}
			else
			{
				ui_software_info *swinfo  = (ui_software_info *)menu_event->itemref;
				if ((FPTR)swinfo > 2 && swinfo->startempty == 1)
				{
					if ((swinfo->driver->flags & MACHINE_TYPE_ARCADE) != 0)
						ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_MAMEINFO_LOAD, swinfo->driver)));
					else
						ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_MESSINFO_LOAD, swinfo->driver)));
				}
			}
		}

		// handle UI_STORY
		else if (menu_event->iptkey == IPT_UI_STORY && machine().options().enabled_dats())
		{
			if (main_filters::actual != FILTER_FAVORITE_GAME)
			{
				const game_driver *driver = (const game_driver *)menu_event->itemref;
				if ((FPTR)driver > 2)
					ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_STORY_LOAD, driver)));
			}
			else
			{
				ui_software_info *swinfo  = (ui_software_info *)menu_event->itemref;
				if ((FPTR)swinfo > 2 && swinfo->startempty == 1)
					ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_STORY_LOAD, swinfo->driver)));
			}
		}

		// handle UI_SYSINFO
		else if (menu_event->iptkey == IPT_UI_SYSINFO && machine().options().enabled_dats())
		{
			if (main_filters::actual != FILTER_FAVORITE_GAME)
			{
				const game_driver *driver = (const game_driver *)menu_event->itemref;
				if ((FPTR)driver > 2)
					ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_SYSINFO_LOAD, driver)));
			}
			else
			{
				ui_software_info *swinfo  = (ui_software_info *)menu_event->itemref;
				if ((FPTR)swinfo > 2 && swinfo->startempty == 1)
					ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_dats(machine(), container, MEWUI_SYSINFO_LOAD, swinfo->driver)));
			}
		}

		// handle UI_COMMAND
		else if (menu_event->iptkey == IPT_UI_COMMAND && machine().options().enabled_dats())
		{
			if (main_filters::actual != FILTER_FAVORITE_GAME)
			{
				const game_driver *driver = (const game_driver *)menu_event->itemref;
				if ((FPTR)driver > 2)
					ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_command(machine(), container, driver)));
			}
			else
			{
				ui_software_info *swinfo  = (ui_software_info *)menu_event->itemref;
				if ((FPTR)swinfo > 2 && swinfo->startempty == 1)
					ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_command(machine(), container, swinfo->driver)));
			}
		}

		// handle UI_FAVORITES
		else if (menu_event->iptkey == IPT_UI_FAVORITES)
		{
			if (main_filters::actual != FILTER_FAVORITE_GAME)
			{
				const game_driver *driver = (const game_driver *)menu_event->itemref;
				if ((FPTR)driver > 2)
				{
					if (!machine().favorite().isgame_favorite(driver))
					{
						machine().favorite().add_favorite_game(driver);
						popmessage("%s\n added to favorites list.", driver->description);
					}

					else
					{
						machine().favorite().remove_favorite_game();
						popmessage("%s\n removed from favorites list.", driver->description);
					}
				}
			}
			else
			{
				ui_software_info *swinfo = (ui_software_info *)menu_event->itemref;
				if ((FPTR)swinfo > 2)
				{
					popmessage("%s\n removed from favorites list.", swinfo->longname.c_str());
					machine().favorite().remove_favorite_game(*swinfo);
					reset(UI_MENU_RESET_SELECT_FIRST);
				}
			}
		}

		// handle UI_EXPORT
		else if (menu_event->iptkey == IPT_UI_EXPORT)
			inkey_export();

		// handle UI_AUDIT_FAST
		else if (menu_event->iptkey == IPT_UI_AUDIT_FAST && !m_unavailablelist.empty())
			ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_audit(machine(), container, m_availablelist, m_unavailablelist, m_availsortedlist, m_unavailsortedlist, 1)));

		// handle UI_AUDIT_ALL
		else if (menu_event->iptkey == IPT_UI_AUDIT_ALL)
			ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_audit(machine(), container, m_availablelist, m_unavailablelist, m_availsortedlist, m_unavailsortedlist, 2)));

		// typed characters append to the buffer
		else if (menu_event->iptkey == IPT_SPECIAL)
			inkey_special(menu_event);

		else if (menu_event->iptkey == IPT_OTHER)
			check_filter = true;
	}

	if (menu_event != NULL && menu_event->itemref == NULL)
	{
		if (menu_event->iptkey == IPT_SPECIAL && menu_event->unichar == 0x09)
			selected = m_prev_selected;

		// handle UI_UP_FILTER
		else if (menu_event->iptkey == IPT_UI_UP_FILTER && main_filters::actual > FILTER_FIRST)
		{
			l_hover = main_filters::actual - 1;
			check_filter = true;
		}

		// handle UI_DOWN_FILTER
		else if (menu_event->iptkey == IPT_UI_DOWN_FILTER && main_filters::actual < FILTER_LAST)
		{
			l_hover = main_filters::actual + 1;
			check_filter = true;
		}
		else if (menu_event->iptkey == IPT_OTHER)
			check_filter = true;
	}

	// if we're in an error state, overlay an error message
	if (ui_error)
		machine().ui().draw_text_box(container,
		                             "The selected game is missing one or more required ROM or CHD images. "
		                             "Please select a different game.\n\nPress any key (except ESC) to continue.",
		                             JUSTIFY_CENTER, 0.5f, 0.5f, UI_RED_COLOR);

	// handle filters selection from key shortcuts
	if (check_filter)
	{
		m_search[0] = '\0';

		if (l_hover == FILTER_CATEGORY)
		{
			main_filters::actual = l_hover;
			ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_game_options(machine(), container)));
		}

		else if (l_hover == FILTER_CUSTOM)
		{
			main_filters::actual = l_hover;
			ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_custom_filter(machine(), container, true)));
		}

		else if (l_hover == FILTER_MANUFACTURER)
			ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_selector(machine(), container, c_mnfct::ui,
			                                     &c_mnfct::actual, SELECTOR_GAME, l_hover)));
		else if (l_hover == FILTER_YEAR)
			ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_selector(machine(), container, c_year::ui,
			                                     &c_year::actual, SELECTOR_GAME, l_hover)));
		else if (l_hover == FILTER_SCREEN)
		{
			std::vector<std::string> text(c_screen::length);
			for (int x = 0; x < c_screen::length; ++x)
				text[x].assign(c_screen::text[x]);

			ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_selector(machine(), container, text,
			                                     &c_screen::actual, SELECTOR_GAME, l_hover)));
		}
		else
		{
			if (l_hover >= FILTER_ALL)
				main_filters::actual = l_hover;
			reset(UI_MENU_RESET_SELECT_FIRST);
		}
	}
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_mewui_select_game::populate()
{
	mewui_globals::redraw_icon = true;
	mewui_globals::switch_image = true;
	int old_item_selected = -1;
	UINT32 flags_mewui = MENU_FLAG_MEWUI | MENU_FLAG_LEFT_ARROW | MENU_FLAG_RIGHT_ARROW;

	if (main_filters::actual != FILTER_FAVORITE_GAME)
	{
		// if search is not empty, find approximate matches
		if (m_search[0] != 0 && !no_active_search())
			populate_search();
		else
		{
			// reset search string
			m_search[0] = '\0';
			m_displaylist.clear();
			m_tmp.clear();

			// if filter is set on category, build category list
			switch (main_filters::actual)
			{
				case FILTER_CATEGORY:
					build_category();
					break;

				case FILTER_MANUFACTURER:
					build_list(m_tmp, c_mnfct::ui[c_mnfct::actual].c_str());
					break;

				case FILTER_YEAR:
					build_list(m_tmp, c_year::ui[c_year::actual].c_str());
					break;

				case FILTER_SCREEN:
				case FILTER_STEREO:
				case FILTER_SAMPLES:
				case FILTER_NOSAMPLES:
				case FILTER_CHD:
				case FILTER_NOCHD:
					build_from_cache(m_tmp, c_screen::actual);
					break;

				case FILTER_CUSTOM:
					build_custom();
					break;

				default:
					build_list(m_tmp);
					break;
			}

			// iterate over entries
			for (size_t curitem = 0; curitem < m_displaylist.size(); curitem++)
			{
				if (!reselect_last::driver.empty() && !(core_stricmp(m_displaylist[curitem]->name, reselect_last::driver.c_str())))
					old_item_selected = curitem;

				bool cloneof = strcmp(m_displaylist[curitem]->parent, "0");
				if (cloneof)
				{
					int cx = driver_list::find(m_displaylist[curitem]->parent);
					if (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0))
						cloneof = false;
				}

				item_append(m_displaylist[curitem]->description, NULL, (!cloneof) ? flags_mewui : (MENU_FLAG_INVERT | flags_mewui),
				            (void *)m_displaylist[curitem]);
			}
		}
	}

	// populate favorites list
	else
	{
		// reset search string
		m_search[0] = '\0';

		flags_mewui |= MENU_FLAG_MEWUI_FAVORITE;

		// iterate over entries
		for (size_t x = 0; x < machine().favorite().m_favorite_list.size(); x++)
		{
			if (machine().favorite().m_favorite_list[x].startempty == 1)
			{
				bool cloneof = strcmp(machine().favorite().m_favorite_list[x].driver->parent, "0");
				if (cloneof)
				{
					int cx = driver_list::find(machine().favorite().m_favorite_list[x].driver->parent);
					if (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0))
						cloneof = false;
				}

				item_append(machine().favorite().m_favorite_list[x].longname.c_str(), NULL,
				            (cloneof) ? (MENU_FLAG_INVERT | flags_mewui) : flags_mewui,
				            (void *)&machine().favorite().m_favorite_list[x]);
			}
			else
				item_append(machine().favorite().m_favorite_list[x].longname.c_str(),
				            machine().favorite().m_favorite_list[x].devicetype.c_str(),
				            machine().favorite().m_favorite_list[x].parentname.empty() ? flags_mewui : (MENU_FLAG_INVERT | flags_mewui),
				            (void *)&machine().favorite().m_favorite_list[x]);
		}
	}

	// add special items
	item_append(MENU_SEPARATOR_ITEM, NULL, MENU_FLAG_MEWUI, NULL);
	item_append("Configure Options", NULL, MENU_FLAG_MEWUI, (void *)1);
	item_append("Configure Directories", NULL, MENU_FLAG_MEWUI, (void *)2);

	// configure the custom rendering
	float y_pixel = 1.0f / container->manager().ui_target().height();
	customtop = 2.0f * machine().ui().get_line_height() + 5.0f * UI_BOX_TB_BORDER + 32 * y_pixel;
	custombottom = 5.0f * machine().ui().get_line_height() + 3.0f * UI_BOX_TB_BORDER;

	// reselect prior game launched, if any
	if (old_item_selected != -1)
	{
		selected = old_item_selected;
		if (mewui_globals::visible_main_lines == 0)
			top_line = (selected != 0) ? selected - 1 : 0;
		else
			top_line = selected - (mewui_globals::visible_main_lines / 2);
		if (reselect_last::software.empty())
			reselect_last::reset();
	}
	else
		reselect_last::reset();
}

//-------------------------------------------------
//  build a list of available drivers
//-------------------------------------------------

void ui_mewui_select_game::build_available_list()
{
	int m_total = driver_list::total();
	std::vector<bool> m_included(m_total, false);

	// open a path to the ROMs and find them in the array
	file_enumerator path(machine().options().media_path());
	const osd_directory_entry *dir;

	// iterate while we get new objects
	while ((dir = path.next()) != NULL)
	{
		char drivername[50];
		char *dst = drivername;
		const char *src;

		// build a name for it
		for (src = dir->name; *src != 0 && *src != '.' && dst < &drivername[ARRAY_LENGTH(drivername) - 1]; src++)
			*dst++ = tolower((UINT8) * src);

		*dst = 0;

		int drivnum = driver_list::find(drivername);

		if (drivnum != -1 && !m_included[drivnum])
		{
			m_availablelist.push_back(&driver_list::driver(drivnum));
			m_included[drivnum] = true;
		}
	}

	// now check and include NONE_NEEDED
	for (int x = 0; x < m_total; ++x)
		if (!m_included[x])
		{
			if (!strcmp("___empty", driver_list::driver(x).name))
				continue;

			const rom_entry *rom = driver_list::driver(x).rom;
			if (ROMENTRY_ISREGION(rom) && ROMENTRY_ISEND(++rom))
			{
				m_availablelist.push_back(&driver_list::driver(x));
				m_included[x] = true;
			}
		}

	// sort
	m_availsortedlist = m_availablelist;
	std::stable_sort(m_availsortedlist.begin(), m_availsortedlist.end(), sort_game_list);

	// now build the unavailable list
	for (int x = 0; x < m_total; ++x)
		if (!m_included[x] && strcmp("___empty", driver_list::driver(x).name))
			m_unavailablelist.push_back(&driver_list::driver(x));

	// sort
	m_unavailsortedlist = m_unavailablelist;
	std::stable_sort(m_unavailsortedlist.begin(), m_unavailsortedlist.end(), sort_game_list);
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void ui_mewui_select_game::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	float tbarspace = (1.0f / container->manager().ui_target().height()) * 32;
	const game_driver *driver = NULL;
	ui_software_info *swinfo = NULL;
	float width, maxwidth = origx2 - origx1;
	std::string tempbuf[5];
	rgb_t color = UI_BACKGROUND_COLOR;
	bool isstar = false;

	if (ume_filters::actual == MEWUI_MAME)
		strprintf(tempbuf[0], "MEWUI %s ( %d / %d machines (%d BIOS) )", mewui_version, visible_items, (driver_list::total() - 1), m_isabios + m_issbios);
	else if (ume_filters::actual == MEWUI_ARCADES)
		strprintf(tempbuf[0], "MEWUI %s ( %d / %d arcades (%d BIOS) )", mewui_version, visible_items, m_isarcades, m_isabios);
	else if (ume_filters::actual == MEWUI_SYSTEMS)
		strprintf(tempbuf[0], "MEWUI %s ( %d / %d systems (%d BIOS) )", mewui_version, visible_items, m_issystems, m_issbios);

	std::string filtered;

	if (main_filters::actual == FILTER_CATEGORY && !machine().inifile().ini_index.empty())
	{
		int c_file = machine().inifile().current_file;
		int c_cat = machine().inifile().current_category;
		std::string s_file = machine().inifile().ini_index[c_file].name;
		std::string s_category = machine().inifile().ini_index[c_file].category[c_cat].name;
		filtered.assign(main_filters::text[main_filters::actual]).append(" (").append(s_file).append(" - ").append(s_category).append(") -");
	}

	else if (main_filters::actual == FILTER_MANUFACTURER)
		filtered.assign(main_filters::text[main_filters::actual]).append(" (").append(c_mnfct::ui[c_mnfct::actual]).append(") -");

	else if (main_filters::actual == FILTER_YEAR)
		filtered.assign(main_filters::text[main_filters::actual]).append(" (").append(c_year::ui[c_year::actual]).append(") -");

	else if (main_filters::actual == FILTER_SCREEN)
		filtered.assign(main_filters::text[main_filters::actual]).append(" (").append(c_screen::text[c_screen::actual]).append(") -");

	// display the current typeahead
	if (no_active_search())
		tempbuf[1].clear();
	else
		tempbuf[1].assign(filtered.c_str()).append(" Search: ").append(m_search).append("_");

	// get the size of the text
	for (int line = 0; line < 2; line++)
	{
		machine().ui().draw_text_full(container, tempbuf[line].c_str(), 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_NEVER,
		                              DRAW_NONE, ARGB_WHITE, ARGB_BLACK, &width, NULL);
		width += 2 * UI_BOX_LR_BORDER;
		maxwidth = MAX(width, maxwidth);
	}

	// compute our bounds
	float x1 = 0.5f - 0.5f * maxwidth;
	float x2 = x1 + maxwidth;
	float y1 = origy1 - top;
	float y2 = origy1 - 3.0f * UI_BOX_TB_BORDER - tbarspace;

	// draw a box
	machine().ui().draw_outlined_box(container, x1, y1, x2, y2, UI_BACKGROUND_COLOR);

	// take off the borders
	x1 += UI_BOX_LR_BORDER;
	x2 -= UI_BOX_LR_BORDER;
	y1 += UI_BOX_TB_BORDER;

	// draw the text within it
	for (int line = 0; line < 2; line++)
	{
		machine().ui().draw_text_full(container, tempbuf[line].c_str(), x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_NEVER,
		                              DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL);
		y1 += machine().ui().get_line_height();
	}

	// draw ume box
	x1 -= UI_BOX_LR_BORDER;
	y1 = origy1 - top;
	draw_ume_box(x1, y1, x2, y2);

	// determine the text to render below
	if (main_filters::actual != FILTER_FAVORITE_GAME)
		driver = ((FPTR)selectedref > 2) ? (const game_driver *)selectedref : NULL;
	else
	{
		swinfo = ((FPTR)selectedref > 2) ? (ui_software_info *)selectedref : NULL;
		if (swinfo && swinfo->startempty == 1)
			driver = swinfo->driver;
	}

	if ((FPTR)driver > 2)
	{
		isstar = machine().favorite().isgame_favorite(driver);

		// first line is game name
		strprintf(tempbuf[0], "Romset: %-.100s", driver->name);

		// next line is year, manufacturer
		strprintf(tempbuf[1], "%s, %-.100s", driver->year, driver->manufacturer);

		// next line is clone/parent status
		int cloneof = driver_list::non_bios_clone(*driver);

		if (cloneof != -1)
			strprintf(tempbuf[2], "Driver is clone of: %-.100s", driver_list::driver(cloneof).description);
		else
			tempbuf[2].assign("Driver is parent");

		// next line is overall driver status
		if (driver->flags & MACHINE_NOT_WORKING)
			tempbuf[3].assign("Overall: NOT WORKING");
		else if (driver->flags & MACHINE_UNEMULATED_PROTECTION)
			tempbuf[3].assign("Overall: Unemulated Protection");
		else
			tempbuf[3].assign("Overall: Working");

		// next line is graphics, sound status
		if (driver->flags & (MACHINE_IMPERFECT_GRAPHICS | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_COLORS))
			tempbuf[4].assign("Graphics: Imperfect, ");
		else
			tempbuf[4].assign("Graphics: OK, ");

		if (driver->flags & MACHINE_NO_SOUND)
			tempbuf[4].append("Sound: Unimplemented");
		else if (driver->flags & MACHINE_IMPERFECT_SOUND)
			tempbuf[4].append("Sound: Imperfect");
		else
			tempbuf[4].append("Sound: OK");

		color = UI_GREEN_COLOR;

		if ((driver->flags & (MACHINE_IMPERFECT_GRAPHICS | MACHINE_WRONG_COLORS | MACHINE_IMPERFECT_COLORS
		    | MACHINE_NO_SOUND | MACHINE_IMPERFECT_SOUND)) != 0)
			color = UI_YELLOW_COLOR;

		if ((driver->flags & (MACHINE_NOT_WORKING | MACHINE_UNEMULATED_PROTECTION)) != 0)
			color = UI_RED_COLOR;
	}

	else if ((FPTR)swinfo > 2)
	{
		isstar = machine().favorite().isgame_favorite(*swinfo);

		// first line is system
		strprintf(tempbuf[0], "System: %-.100s", swinfo->driver->description);

		// next line is year, publisher
		strprintf(tempbuf[1], "%s, %-.100s", swinfo->year.c_str(), swinfo->publisher.c_str());

		// next line is parent/clone
		if (!swinfo->parentname.empty())
			strprintf(tempbuf[2], "Software is clone of: %-.100s", !swinfo->parentlongname.empty() ? swinfo->parentlongname.c_str() : swinfo->parentname.c_str());
		else
			tempbuf[2].assign("Software is parent");

		// next line is supported status
		if (swinfo->supported == SOFTWARE_SUPPORTED_NO)
		{
			tempbuf[3].assign("Supported: No");
			color = UI_RED_COLOR;
		}
		else if (swinfo->supported == SOFTWARE_SUPPORTED_PARTIAL)
		{
			tempbuf[3].assign("Supported: Partial");
			color = UI_YELLOW_COLOR;
		}
		else
		{
			tempbuf[3].assign("Supported: Yes");
			color = UI_GREEN_COLOR;
		}

		// last line is romset name
		strprintf(tempbuf[4], "romset: %-.100s", swinfo->shortname.c_str());
	}

	else
	{
		std::string copyright(emulator_info::get_copyright());
		size_t found = copyright.find("\n");

		tempbuf[0].assign(emulator_info::get_applongname()).append(" ").append(build_version);
		tempbuf[1].assign(copyright.substr(0, found));
		tempbuf[2].assign(copyright.substr(found + 1));
		tempbuf[3].clear();
		tempbuf[4].assign("MEWUI by dankan1890 http://dankan1890.github.io/mewui/");
	}

	// compute our bounds
	x1 = 0.5f - 0.5f * maxwidth;
	x2 = x1 + maxwidth;
	y1 = y2;
	y2 = origy1 - UI_BOX_TB_BORDER;

	// draw toolbar
	draw_toolbar(container, x1, y1, x2, y2);

	// get the size of the text
	maxwidth = origx2 - origx1;

	for (int line = 0; line < 5; line++)
	{
		machine().ui().draw_text_full(container, tempbuf[line].c_str(), 0.0f, 0.0f, 1.0f, JUSTIFY_CENTER, WRAP_NEVER,
		                              DRAW_NONE, ARGB_WHITE, ARGB_BLACK, &width, NULL);
		width += 2 * UI_BOX_LR_BORDER;
		maxwidth = MAX(maxwidth, width);
	}

	// compute our bounds
	x1 = 0.5f - 0.5f * maxwidth;
	x2 = x1 + maxwidth;
	y1 = origy2 + UI_BOX_TB_BORDER;
	y2 = origy2 + bottom;

	// draw a box
	machine().ui().draw_outlined_box(container, x1, y1, x2, y2, color);

	// take off the borders
	x1 += UI_BOX_LR_BORDER;
	x2 -= UI_BOX_LR_BORDER;
	y1 += UI_BOX_TB_BORDER;

	// is favorite? draw the star
	if (isstar)
		draw_star(container, x1, y1);

	// draw all lines
	for (int line = 0; line < 5; line++)
	{
		machine().ui().draw_text_full(container, tempbuf[line].c_str(), x1, y1, x2 - x1, JUSTIFY_CENTER, WRAP_NEVER,
		                              DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL);
		y1 += machine().ui().get_line_height();
	}
}

//-------------------------------------------------
//  force the game select menu to be visible
//  and inescapable
//-------------------------------------------------

void ui_mewui_select_game::force_game_select(running_machine &machine, render_container *container)
{
	// reset the menu stack
	ui_menu::stack_reset(machine);

	// add the quit entry followed by the game select entry
	ui_menu *quit = auto_alloc_clear(machine, ui_menu_quit_game(machine, container));
	quit->set_special_main_menu(true);
	ui_menu::stack_push(quit);
	ui_menu::stack_push(auto_alloc_clear(machine, ui_mewui_select_game(machine, container, NULL)));

	// force the menus on
	machine.ui().show_menu();

	// make sure MAME is paused
	machine.pause();
}

//-------------------------------------------------
//  handle select key event
//-------------------------------------------------

void ui_mewui_select_game::inkey_select(const ui_menu_event *menu_event)
{
	const game_driver *driver = (const game_driver *)menu_event->itemref;

	// special case for configure options
	if ((FPTR)driver == 1)
		ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_game_options(machine(), container)));
	// special case for configure directory
	else if ((FPTR)driver == 2)
		ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_directory(machine(), container)));
	// anything else is a driver
	else
	{
		// audit the game first to see if we're going to work
		driver_enumerator enumerator(machine().options(), *driver);
		enumerator.next();
		media_auditor auditor(enumerator);
		media_auditor::summary summary = auditor.audit_media(AUDIT_VALIDATE_FAST);

		// if everything looks good, schedule the new driver
		if (summary == media_auditor::CORRECT || summary == media_auditor::BEST_AVAILABLE || summary == media_auditor::NONE_NEEDED)
		{
			bool has_swlist = false;
			if ((driver->flags & MACHINE_TYPE_ARCADE) == 0)
			{
				software_list_device_iterator iter(enumerator.config().root_device());
				for (software_list_device *swlistdev = iter.first(); swlistdev != NULL; swlistdev = iter.next())
					if (swlistdev->first_software_info() != NULL)
					{
						ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_select_software(machine(), container, driver)));
						has_swlist = true;
						break;
					}

			}
			if ((driver->flags & MACHINE_TYPE_ARCADE) != 0 || !has_swlist)
			{
				std::vector<s_bios> biosname;
				if (get_bios_count(driver, biosname) > 1 && !machine().options().skip_bios_menu())
					ui_menu::stack_push(auto_alloc_clear(machine(), ui_mewui_bios_selection(machine(), container, biosname, (void *)driver, false, false)));
				else
				{
					reselect_last::driver.assign(driver->name);
					reselect_last::software.clear();
					reselect_last::swlist.clear();
					machine().manager().schedule_new_driver(*driver);
					machine().schedule_hard_reset();
					ui_menu::stack_reset(machine());
				}
			}
		}
		// otherwise, display an error
		else
		{
			reset(UI_MENU_RESET_REMEMBER_REF);
			ui_error = true;
		}
	}
}

//-------------------------------------------------
//  handle select key event for favorites menu
//-------------------------------------------------

void ui_mewui_select_game::inkey_select_favorite(const ui_menu_event *menu_event)
{
	ui_software_info *ui_swinfo = (ui_software_info *)menu_event->itemref;

	// special case for configure options
	if ((FPTR)ui_swinfo == 1)
		ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_game_options(machine(), container)));

	// special case for configure directory
	else if ((FPTR)ui_swinfo == 2)
		ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_directory(machine(), container)));

	else if (ui_swinfo->startempty == 1)
	{
		// audit the game first to see if we're going to work
		driver_enumerator enumerator(machine().options(), *ui_swinfo->driver);
		enumerator.next();
		media_auditor auditor(enumerator);
		media_auditor::summary summary = auditor.audit_media(AUDIT_VALIDATE_FAST);

		// if everything looks good, schedule the new driver
		if (summary == media_auditor::CORRECT || summary == media_auditor::BEST_AVAILABLE || summary == media_auditor::NONE_NEEDED)
		{
			std::vector<s_bios> biosname;
			if (get_bios_count(ui_swinfo->driver, biosname) > 1 && !machine().options().skip_bios_menu())
				ui_menu::stack_push(auto_alloc_clear(machine(), ui_mewui_bios_selection(machine(), container, biosname, (void *)ui_swinfo->driver, false, false)));
			else
			{
				reselect_last::driver.assign(ui_swinfo->driver->name);
				reselect_last::software.clear();
				reselect_last::swlist.clear();
				reselect_last::set(true);
				machine().manager().schedule_new_driver(*ui_swinfo->driver);
				machine().schedule_hard_reset();
				ui_menu::stack_reset(machine());
			}
		}

		// otherwise, display an error
		else
		{
			reset(UI_MENU_RESET_REMEMBER_REF);
			ui_error = true;
		}
	}
	else
	{
		// first validate
		driver_enumerator drv(machine().options(), *ui_swinfo->driver);
		media_auditor auditor(drv);
		drv.next();
		software_list_device *swlist = software_list_device::find_by_name(drv.config(), ui_swinfo->listname.c_str());
		software_info *swinfo = swlist->find(ui_swinfo->shortname.c_str());
		media_auditor::summary summary = auditor.audit_software(swlist->list_name(), swinfo, AUDIT_VALIDATE_FAST);

		if (summary == media_auditor::CORRECT || summary == media_auditor::BEST_AVAILABLE || summary == media_auditor::NONE_NEEDED)
		{
			std::vector<s_bios> biosname;
			if (get_bios_count(ui_swinfo->driver, biosname) > 1 && !machine().options().skip_bios_menu())
			{
				ui_menu::stack_push(auto_alloc_clear(machine(), ui_mewui_bios_selection(machine(), container, biosname, (void *)ui_swinfo, true, false)));
				return;
			}
			else if (swinfo->has_multiple_parts(ui_swinfo->interface.c_str()) && !machine().options().skip_parts_menu())
			{
				std::vector<std::string> partname, partdesc;
				for (const software_part *swpart = swinfo->first_part(); swpart != NULL; swpart = swpart->next())
				{
					if (swpart->matches_interface(ui_swinfo->interface.c_str()))
					{
						partname.push_back(swpart->name());
						std::string menu_part_name(swpart->name());
						if (swpart->feature("part_id") != NULL)
							menu_part_name.assign("(").append(swpart->feature("part_id")).append(")");
						partdesc.push_back(menu_part_name);
					}
				}
				ui_menu::stack_push(auto_alloc_clear(machine(), ui_mewui_software_parts(machine(), container, partname, partdesc, ui_swinfo)));
				return;
			}

			std::string error_string;
			std::string string_list = std::string(ui_swinfo->listname).append(":").append(ui_swinfo->shortname).append(":").append(ui_swinfo->part).append(":").append(ui_swinfo->instance);
			machine().options().set_value(OPTION_SOFTWARENAME, string_list.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
			std::string snap_list = std::string(ui_swinfo->listname).append(PATH_SEPARATOR).append(ui_swinfo->shortname);
			machine().options().set_value(OPTION_SNAPNAME, snap_list.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
			reselect_last::driver.assign(drv.driver().name);
			reselect_last::software.assign(ui_swinfo->shortname);
			reselect_last::swlist.assign(ui_swinfo->listname);
			machine().manager().schedule_new_driver(drv.driver());
			machine().schedule_hard_reset();
			ui_menu::stack_reset(machine());
		}
		// otherwise, display an error
		else
		{
			reset(UI_MENU_RESET_REMEMBER_POSITION);
			ui_error = true;
		}
	}
}

//-------------------------------------------------
//  returns if the search can be activated
//-------------------------------------------------

bool ui_mewui_select_game::no_active_search()
{
	return (main_filters::actual == FILTER_FAVORITE_GAME);
}

//-------------------------------------------------
//  handle special key event
//-------------------------------------------------

void ui_mewui_select_game::inkey_special(const ui_menu_event *menu_event)
{
	int buflen = strlen(m_search);

	// if it's a backspace and we can handle it, do so
	if (((menu_event->unichar == 8 || menu_event->unichar == 0x7f) && buflen > 0) && !no_active_search())
	{
		*(char *)utf8_previous_char(&m_search[buflen]) = 0;
		reset(UI_MENU_RESET_SELECT_FIRST);
	}

	// if it's any other key and we're not maxed out, update
	else if ((menu_event->unichar >= ' ' && menu_event->unichar < 0x7f) && !no_active_search())
	{
		buflen += utf8_from_uchar(&m_search[buflen], ARRAY_LENGTH(m_search) - buflen, menu_event->unichar);
		m_search[buflen] = 0;
		reset(UI_MENU_RESET_SELECT_FIRST);
	}

	// Tab key
	else if (menu_event->unichar == 0x09)
	{
		// if the selection is in the main screen, save and go to submenu
		if (selected <= visible_items)
		{
			m_prev_selected = selected;
			selected = visible_items + 1;
		}

		// otherwise, retrieve the previous position
		else
			selected = m_prev_selected;
	}
}

//-------------------------------------------------
//  build list
//-------------------------------------------------

void ui_mewui_select_game::build_list(std::vector<const game_driver *> &s_drivers, const char *filter_text, int filter, bool bioscheck)
{
	int cx = 0;
	bool cloneof = false;

	if (s_drivers.empty())
	{
		filter = main_filters::actual;

		if (filter == FILTER_AVAILABLE)
			s_drivers = m_availsortedlist;
		else if (filter == FILTER_UNAVAILABLE)
			s_drivers = m_unavailsortedlist;
		else
			s_drivers = m_sortedlist;
	}

	for (size_t index = 0; index < s_drivers.size(); index++)
	{
		if (!bioscheck && filter != FILTER_BIOS && (s_drivers[index]->flags & MACHINE_IS_BIOS_ROOT) != 0)
			continue;

		if ((s_drivers[index]->flags & MACHINE_TYPE_ARCADE) && ume_filters::actual == MEWUI_SYSTEMS)
			continue;

		if (!(s_drivers[index]->flags & MACHINE_TYPE_ARCADE) && ume_filters::actual == MEWUI_ARCADES)
			continue;

		switch (filter)
		{
			case FILTER_ALL:
			case FILTER_AVAILABLE:
			case FILTER_UNAVAILABLE:
				m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_WORKING:
				if (!(s_drivers[index]->flags & MACHINE_NOT_WORKING))
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_NOT_MECHANICAL:
				if (!(s_drivers[index]->flags & MACHINE_MECHANICAL))
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_BIOS:
				if (s_drivers[index]->flags & MACHINE_IS_BIOS_ROOT)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_PARENT:
			case FILTER_CLONES:
				cloneof = strcmp(s_drivers[index]->parent, "0");
				if (cloneof)
				{
					cx = driver_list::find(s_drivers[index]->parent);
					if (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0))
						cloneof = false;
				}

				if (filter == FILTER_CLONES && cloneof)
					m_displaylist.push_back(s_drivers[index]);
				else if (filter == FILTER_PARENT && !cloneof)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_NOT_WORKING:
				if (s_drivers[index]->flags & MACHINE_NOT_WORKING)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_MECHANICAL:
				if (s_drivers[index]->flags & MACHINE_MECHANICAL)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_SAVE:
				if (s_drivers[index]->flags & MACHINE_SUPPORTS_SAVE)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_NOSAVE:
				if (!(s_drivers[index]->flags & MACHINE_SUPPORTS_SAVE))
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_YEAR:
				if (!core_stricmp(filter_text, s_drivers[index]->year))
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_VERTICAL:
				if (s_drivers[index]->flags & ORIENTATION_SWAP_XY)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_HORIZONTAL:
				if (!(s_drivers[index]->flags & ORIENTATION_SWAP_XY))
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_MANUFACTURER:
			{
				std::string name = c_mnfct::getname(s_drivers[index]->manufacturer);

				if (!core_stricmp(filter_text, name.c_str()))
					m_displaylist.push_back(s_drivers[index]);
				break;
			}
		}
	}
}

//-------------------------------------------------
//  build custom display list
//-------------------------------------------------

void ui_mewui_select_game::build_custom()
{
	std::vector<const game_driver *> s_drivers;
	bool bioscheck = false;

	if (custfltr::main == FILTER_AVAILABLE)
		s_drivers = m_availsortedlist;
	else if (custfltr::main == FILTER_UNAVAILABLE)
		s_drivers = m_unavailsortedlist;
	else
		s_drivers = m_sortedlist;

	for (size_t index = 0; index < s_drivers.size(); ++index)
	{
		if ((s_drivers[index]->flags & MACHINE_TYPE_ARCADE) && ume_filters::actual == MEWUI_SYSTEMS)
			continue;

		if (!(s_drivers[index]->flags & MACHINE_TYPE_ARCADE) && ume_filters::actual == MEWUI_ARCADES)
			continue;

		m_displaylist.push_back(s_drivers[index]);
	}

	for (int count = 1; count <= custfltr::numother; count++)
	{
		int filter = custfltr::other[count];
		if (filter == FILTER_BIOS)
			bioscheck = true;
	}

	for (int count = 1; count <= custfltr::numother; count++)
	{
		int filter = custfltr::other[count];
		s_drivers = m_displaylist;
		m_displaylist.clear();

		switch (filter)
		{
			case FILTER_YEAR:
				build_list(s_drivers, c_year::ui[custfltr::year[count]].c_str(), filter, bioscheck);
				break;
			case FILTER_MANUFACTURER:
				build_list(s_drivers, c_mnfct::ui[custfltr::mnfct[count]].c_str(), filter, bioscheck);
				break;
			case FILTER_SCREEN:
				build_from_cache(s_drivers, custfltr::screen[count], filter, bioscheck);
				break;
			case FILTER_CHD:
			case FILTER_NOCHD:
			case FILTER_SAMPLES:
			case FILTER_NOSAMPLES:
			case FILTER_STEREO:
				build_from_cache(s_drivers, 0, filter, bioscheck);
				break;
			default:
				build_list(s_drivers, NULL, filter, bioscheck);
				break;
		}
	}
}

//-------------------------------------------------
//  build category list
//-------------------------------------------------

void ui_mewui_select_game::build_category()
{
	std::vector<int> temp_filter;
	machine().inifile().load_ini_category(temp_filter);

	for (size_t index = 0; index < temp_filter.size(); ++index)
	{
		int actual = temp_filter[index];
		m_tmp.push_back(&driver_list::driver(actual));
	}
	std::stable_sort(m_tmp.begin(), m_tmp.end(), sort_game_list);
	m_displaylist = m_tmp;
}

//-------------------------------------------------
//  build list from cache
//-------------------------------------------------

void ui_mewui_select_game::build_from_cache(std::vector<const game_driver *> &s_drivers, int screens, int filter, bool bioscheck)
{
	if (s_drivers.empty())
	{
		s_drivers = m_sortedlist;
		filter = main_filters::actual;
	}

	for (size_t index = 0; index < s_drivers.size(); ++index)
	{
		if (!bioscheck && filter != FILTER_BIOS && (s_drivers[index]->flags & MACHINE_IS_BIOS_ROOT) != 0)
			continue;

		if ((s_drivers[index]->flags & MACHINE_TYPE_ARCADE) && ume_filters::actual == MEWUI_SYSTEMS)
			continue;

		if (!(s_drivers[index]->flags & MACHINE_TYPE_ARCADE) && ume_filters::actual == MEWUI_ARCADES)
			continue;

		int idx = driver_list::find(s_drivers[index]->name);

		switch (filter)
		{
			case FILTER_SCREEN:
				if (driver_cache[idx].b_screen == screens)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_SAMPLES:
				if (driver_cache[idx].b_samples)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_NOSAMPLES:
				if (!driver_cache[idx].b_samples)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_STEREO:
				if (driver_cache[idx].b_stereo)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_CHD:
				if (driver_cache[idx].b_chd)
					m_displaylist.push_back(s_drivers[index]);
				break;

			case FILTER_NOCHD:
				if (!driver_cache[idx].b_chd)
					m_displaylist.push_back(s_drivers[index]);
				break;
		}
	}
}

//-------------------------------------------------
//  populate search list
//-------------------------------------------------

void ui_mewui_select_game::populate_search()
{
	// allocate memory to track the penalty value
	std::vector<int> penalty(VISIBLE_GAMES_IN_SEARCH, 9999);
	int index = 0;
	for (; index < m_displaylist.size(); ++index)
	{
		// pick the best match between driver name and description
		int curpenalty = fuzzy_substring2(m_search, m_displaylist[index]->description);
		int tmp = fuzzy_substring2(m_search, m_displaylist[index]->name);
		curpenalty = MIN(curpenalty, tmp);

		// insert into the sorted table of matches
		for (int matchnum = VISIBLE_GAMES_IN_SEARCH - 1; matchnum >= 0; matchnum--)
		{
			// stop if we're worse than the current entry
			if (curpenalty >= penalty[matchnum])
				break;

			// as long as this isn't the last entry, bump this one down
			if (matchnum < VISIBLE_GAMES_IN_SEARCH - 1)
			{
				penalty[matchnum + 1] = penalty[matchnum];
				m_searchlist[matchnum + 1] = m_searchlist[matchnum];
			}

			m_searchlist[matchnum] = m_displaylist[index];
			penalty[matchnum] = curpenalty;
		}
	}

	(index < VISIBLE_GAMES_IN_SEARCH) ? m_searchlist[index] = NULL : m_searchlist[VISIBLE_GAMES_IN_SEARCH] = NULL;
	UINT32 flags_mewui = MENU_FLAG_MEWUI | MENU_FLAG_LEFT_ARROW | MENU_FLAG_RIGHT_ARROW;
	for (int curitem = 0; m_searchlist[curitem]; curitem++)
	{
		bool cloneof = strcmp(m_searchlist[curitem]->parent, "0");
		if (cloneof)
		{
			int cx = driver_list::find(m_searchlist[curitem]->parent);
			if (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0))
				cloneof = false;
		}
		item_append(m_searchlist[curitem]->description, NULL, (!cloneof) ? flags_mewui : (MENU_FLAG_INVERT | flags_mewui),
		            (void *)m_searchlist[curitem]);
	}
}

//-------------------------------------------------
//  generate general info
//-------------------------------------------------

void ui_mewui_select_game::general_info(const game_driver *driver, std::string &buffer)
{
	strprintf(buffer, "Romset: %-.100s\n", driver->name);
	buffer.append("Year: ").append(driver->year).append("\n");
	strcatprintf(buffer, "Manufacturer: %-.100s\n", driver->manufacturer);

	int cloneof = driver_list::non_bios_clone(*driver);
	if (cloneof != -1)
		strcatprintf(buffer, "Driver is Clone of: %-.100s\n", driver_list::driver(cloneof).description);
	else
		buffer.append("Driver is Parent\n");

	if (driver->flags & MACHINE_NOT_WORKING)
		buffer.append("Overall: NOT WORKING\n");
	else if (driver->flags & MACHINE_UNEMULATED_PROTECTION)
		buffer.append("Overall: Unemulated Protection\n");
	else
		buffer.append("Overall: Working\n");

	if (driver->flags & MACHINE_IMPERFECT_COLORS)
		buffer.append("Graphics: Imperfect Colors\n");
	else if (driver->flags & MACHINE_WRONG_COLORS)
		buffer.append("Graphics: Wrong Colors\n");
	else if (driver->flags & MACHINE_IMPERFECT_GRAPHICS)
		buffer.append("Graphics: Imperfect\n");
	else
		buffer.append("Graphics: OK\n");

	if (driver->flags & MACHINE_NO_SOUND)
		buffer.append("Sound: Unimplemented\n");
	else if (driver->flags & MACHINE_IMPERFECT_SOUND)
		buffer.append("Sound: Imperfect\n");
	else
		buffer.append("Sound: OK\n");

	strcatprintf(buffer, "Driver is Skeleton: %s\n", ((driver->flags & MACHINE_IS_SKELETON) ? "Yes" : "No"));
	strcatprintf(buffer, "Game is Mechanical: %s\n", ((driver->flags & MACHINE_MECHANICAL) ? "Yes" : "No"));
	strcatprintf(buffer, "Requires Artwork: %s\n", ((driver->flags & MACHINE_REQUIRES_ARTWORK) ? "Yes" : "No"));
	strcatprintf(buffer, "Requires Clickable Artwork: %s\n", ((driver->flags & MACHINE_CLICKABLE_ARTWORK) ? "Yes" : "No"));
	strcatprintf(buffer, "Support Cocktail: %s\n", ((driver->flags & MACHINE_NO_COCKTAIL) ? "Yes" : "No"));
	strcatprintf(buffer, "Driver is Bios: %s\n", ((driver->flags & MACHINE_IS_BIOS_ROOT) ? "Yes" : "No"));
	strcatprintf(buffer, "Support Save: %s\n", ((driver->flags & MACHINE_SUPPORTS_SAVE) ? "Yes" : "No"));

	int idx = driver_list::find(driver->name);
	strcatprintf(buffer, "Screen Type: %s\n", c_screen::text[driver_cache[idx].b_screen]);
	strcatprintf(buffer, "Screen Orentation: %s\n", ((driver->flags & ORIENTATION_SWAP_XY) ? "Vertical" : "Horizontal"));
	strcatprintf(buffer, "Requires Samples: %s\n", (driver_cache[idx].b_samples ? "Yes" : "No"));
	strcatprintf(buffer, "Sound Channel: %s\n", (driver_cache[idx].b_stereo ? "Stereo" : "Mono"));
	strcatprintf(buffer, "Requires CHD: %s\n", (driver_cache[idx].b_chd ? "Yes" : "No"));

	// audit the game first to see if we're going to work
	driver_enumerator enumerator(machine().options(), *driver);
	enumerator.next();
	media_auditor auditor(enumerator);
	media_auditor::summary summary = auditor.audit_media(AUDIT_VALIDATE_FAST);
	media_auditor::summary summary_samples = auditor.audit_samples();

	// if everything looks good, schedule the new driver
	if (summary == media_auditor::CORRECT || summary == media_auditor::BEST_AVAILABLE || summary == media_auditor::NONE_NEEDED)
		buffer.append("Roms Audit Pass: OK\n");
	else
		buffer.append("Roms Audit Pass: BAD\n");

	if (summary_samples == media_auditor::NONE_NEEDED)
		buffer.append("Samples Audit Pass: None Needed\n");
	else if (summary_samples == media_auditor::CORRECT || summary_samples == media_auditor::BEST_AVAILABLE)
		buffer.append("Samples Audit Pass: OK\n");
	else
		buffer.append("Samples Audit Pass: BAD\n");
}

void ui_mewui_select_game::inkey_export()
{
	std::string filename("exported");
	emu_file infile(machine().options().mewui_path(), OPEN_FLAG_READ);
	if (infile.open(filename.c_str(), ".xml") == FILERR_NONE)
	{
		for (int seq = 0; ; seq++)
		{
			std::string seqtext;
			strprintf(seqtext, "%s_%04d", filename.c_str(), seq);
			file_error filerr = infile.open(seqtext.c_str(), ".xml");
			if (filerr != FILERR_NONE)
			{
				filename = seqtext;
				break;
			}
		}
	}

	// attempt to open the output file
	emu_file file(machine().options().mewui_path(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);

	if (file.open(filename.c_str(), ".xml") == FILERR_NONE)
	{
		FILE *pfile;
		std::string fullpath(file.fullpath());
		file.close();
		pfile = fopen(fullpath.c_str() , "w");
		driver_enumerator drivlist(machine().options());
		drivlist.exclude_all();

		if (m_search[0] != 0)
		{
			for (int curitem = 0; m_searchlist[curitem]; curitem++)
			{
				int f = driver_list::find(m_searchlist[curitem]->name);
				drivlist.include(f);
			}
		}
		else
		{
			// iterate over entries
			for (size_t curitem = 0; curitem < m_displaylist.size(); curitem++)
			{
				int f = driver_list::find(m_displaylist[curitem]->name);
				drivlist.include(f);
			}
		}

		// create the XML and save to file
		info_xml_creator creator(drivlist);
		creator.output(pfile);
		fclose(pfile);
		popmessage("%s.xml saved under mewui folder.", filename.c_str());
	}
}

//-------------------------------------------------
//  save drivers infos to file
//-------------------------------------------------

void ui_mewui_select_game::save_cache_info()
{
	// attempt to open the output file
	emu_file file(machine().options().mewui_path(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);

	if (file.open("info_", emulator_info::get_configname(), ".ini") == FILERR_NONE)
	{
		std::string filename(file.fullpath());
		file.close();
		std::ofstream myfile(filename.c_str());

		// generate header
		std::string buffer = std::string("#\n").append(MEWUI_VERSION_TAG).append(mewui_version).append("\n#\n\n");
		myfile << buffer;

		// generate full list
		for (int x = 0; x < driver_list::total(); ++x)
		{
			const game_driver *driver = &driver_list::driver(x);
			if (!strcmp("___empty", driver->name))
				continue;

			m_fulllist.push_back(driver);
			c_mnfct::set(driver->manufacturer);
			c_year::set(driver->year);
		}
		m_sortedlist = m_fulllist;

		// sort manufacturers - years and driver
		std::stable_sort(c_mnfct::ui.begin(), c_mnfct::ui.end());
		std::stable_sort(c_year::ui.begin(), c_year::ui.end());
		std::stable_sort(m_sortedlist.begin(), m_sortedlist.end(), sort_game_list);

		int index = 0;
		m_isabios = 0;
		m_issbios = 0;
		m_isarcades = 0;
		m_issystems = 0;
		for (int x = 0; x < driver_list::total(); ++x)
		{
			const game_driver *driver = &driver_list::driver(x);
			if (!strcmp("___empty", driver->name))
				continue;

			if (driver->flags & MACHINE_TYPE_ARCADE)
			{
				if (driver->flags & MACHINE_IS_BIOS_ROOT)
					m_isabios++;
				m_isarcades++;
			}
			else
			{
				if (driver->flags & MACHINE_IS_BIOS_ROOT)
					m_issbios++;
				m_issystems++;
			}
			cache_info infos;
			machine_config config(*driver, machine().options());

			samples_device_iterator iter(config.root_device());
			infos.b_samples = (iter.first() != NULL) ? 1 : 0;

			const screen_device *screen = config.first_screen();
			infos.b_screen = (screen != NULL) ? screen->screen_type() : 0;

			speaker_device_iterator siter(config.root_device());
			sound_interface_iterator snditer(config.root_device());
			infos.b_stereo = (snditer.first() != NULL && siter.count() > 1) ? 1 : 0;
			infos.b_chd = 0;
			for (const rom_entry *rom = driver->rom; !ROMENTRY_ISEND(rom); ++rom)
				if (ROMENTRY_ISREGION(rom) && ROMREGION_ISDISKDATA(rom))
				{
					infos.b_chd = 1;
					break;
				}
			driver_cache[x].b_screen = infos.b_screen;
			myfile << infos.b_screen;
			driver_cache[x].b_samples = infos.b_samples;
			myfile << infos.b_samples;
			driver_cache[x].b_stereo = infos.b_stereo;
			myfile << infos.b_stereo;
			driver_cache[x].b_chd = infos.b_chd;
			myfile << infos.b_chd;
			int find = driver_list::find(m_sortedlist[index++]->name);
			myfile << find;
		}

		UINT8 space = 0;
		myfile << space << m_isabios;
		myfile << space << m_issbios;
		myfile << space << m_isarcades;
		myfile << space << m_issystems;
		myfile.close();
	}
}

//-------------------------------------------------
//  load drivers infos from file
//-------------------------------------------------

void ui_mewui_select_game::load_cache_info()
{
	driver_cache.resize(driver_list::total() + 1);

	// try to load driver cache
	emu_file efile(machine().options().mewui_path(), OPEN_FLAG_READ);
	file_error filerr = efile.open("info_", emulator_info::get_configname(), ".ini");

	// file not exist ? save and exit
	if (filerr != FILERR_NONE)
	{
		save_cache_info();
		return;
	}

	std::string filename(efile.fullpath());
	efile.close();

	std::ifstream myfile(filename.c_str());
	std::string readbuf;
	std::getline(myfile, readbuf);
	std::getline(myfile, readbuf);
	std::string a_rev = std::string(MEWUI_VERSION_TAG).append(mewui_version);

	// version not matching ? save and exit
	if (a_rev != readbuf)
	{
		myfile.close();
		save_cache_info();
		return;
	}

	std::getline(myfile, readbuf);
	std::getline(myfile, readbuf);

	for (int x = 0; x < driver_list::total(); ++x)
	{
		const game_driver *driver = &driver_list::driver(x);
		if (!strcmp("___empty", driver->name))
			continue;

		m_fulllist.push_back(driver);
		c_mnfct::set(driver->manufacturer);
		c_year::set(driver->year);
		myfile >> driver_cache[x].b_screen;
		myfile >> driver_cache[x].b_samples;
		myfile >> driver_cache[x].b_stereo;
		myfile >> driver_cache[x].b_chd;
		int find;
		myfile >> find;
		m_sortedlist.push_back(&driver_list::driver(find));
	}
	UINT8 space = 0;
	myfile >> space >> m_isabios;
	myfile >> space >> m_issbios;
	myfile >> space >> m_isarcades;
	myfile >> space >> m_issystems;
	myfile.close();
	std::stable_sort(c_mnfct::ui.begin(), c_mnfct::ui.end());
	std::stable_sort(c_year::ui.begin(), c_year::ui.end());
}

//-------------------------------------------------
//  save drivers infos to file
//-------------------------------------------------

void ui_mewui_select_game::save_available_machines()
{
	// attempt to open the output file
	emu_file file(machine().options().mewui_path(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);
	if (file.open(emulator_info::get_configname(), "_avail.ini") == FILERR_NONE)
	{
		std::string filename(file.fullpath());
		file.close();
		std::ofstream myfile(filename.c_str());
		UINT8 space = 0;

		// generate header
		std::string buffer = std::string("#\n").append(MEWUI_VERSION_TAG).append(mewui_version).append("\n#\n\n");
		myfile << buffer;
		myfile << (int)m_availablelist.size() << space;
		myfile << (int)m_unavailablelist.size() << space;
		int find = 0;

		// generate available list
		for (size_t x = 0; x < m_availablelist.size(); ++x)
		{
			find = driver_list::find(m_availablelist[x]->name);
			myfile << find << space;
			find = driver_list::find(m_availsortedlist[x]->name);
			myfile << find << space;
		}

		// generate unavailable list
		for (size_t x = 0; x < m_unavailablelist.size(); ++x)
		{
			find = driver_list::find(m_unavailablelist[x]->name);
			myfile << find << space;
			find = driver_list::find(m_unavailsortedlist[x]->name);
			myfile << find << space;
		}
		myfile.close();
	}
}

//-------------------------------------------------
//  load drivers infos from file
//-------------------------------------------------

bool ui_mewui_select_game::load_available_machines()
{
	// try to load available drivers from file
	emu_file efile(machine().options().mewui_path(), OPEN_FLAG_READ);
	file_error filerr = efile.open(emulator_info::get_configname(), "_avail.ini");

	// file not exist ? exit
	if (filerr != FILERR_NONE)
		return false;

	std::string filename(efile.fullpath());
	efile.close();

	std::ifstream myfile(filename.c_str());
	std::string readbuf;
	std::getline(myfile, readbuf);
	std::getline(myfile, readbuf);
	std::string a_rev = std::string(MEWUI_VERSION_TAG).append(mewui_version);

	// version not matching ? exit
	if (a_rev != readbuf)
	{
		myfile.close();
		return false;
	}

	std::getline(myfile, readbuf);
	std::getline(myfile, readbuf);

	UINT8 space = 0;
	int avsize, unavsize;
	myfile >> avsize >> space >> unavsize >> space;
	int find = 0;

	// load available list
	for (int x = 0; x < avsize; ++x)
	{
		myfile >> find >> space;
		m_availablelist.push_back(&driver_list::driver(find));
		myfile >> find >> space;
		m_availsortedlist.push_back(&driver_list::driver(find));
	}

	// load unavailable list
	for (int x = 0; x < unavsize; ++x)
	{
		myfile >> find >> space;
		m_unavailablelist.push_back(&driver_list::driver(find));
		myfile >> find >> space;
		m_unavailsortedlist.push_back(&driver_list::driver(find));
	}
	myfile.close();
	return true;
}

//-------------------------------------------------
//  load custom filters info from file
//-------------------------------------------------

void ui_mewui_select_game::load_custom_filters()
{
	// attempt to open the output file
	emu_file file(machine().options().mewui_path(), OPEN_FLAG_READ);
	if (file.open("custom_", emulator_info::get_configname(), "_filter.ini") == FILERR_NONE)
	{
		char buffer[MAX_CHAR_INFO];

		// get number of filters
		file.gets(buffer, MAX_CHAR_INFO);
		char *pb = strchr(buffer, '=');
		custfltr::numother = atoi(++pb) - 1;

		// get main filter
		file.gets(buffer, MAX_CHAR_INFO);
		pb = strchr(buffer, '=') + 2;

		for (int y = 0; y < main_filters::length; y++)
			if (!strncmp(pb, main_filters::text[y], strlen(main_filters::text[y])))
			{
				custfltr::main = y;
				break;
			}

		for (int x = 1; x <= custfltr::numother; x++)
		{
			file.gets(buffer, MAX_CHAR_INFO);
			char *cb = strchr(buffer, '=') + 2;
			for (int y = 0; y < main_filters::length; y++)
				if (!strncmp(cb, main_filters::text[y], strlen(main_filters::text[y])))
				{
					custfltr::other[x] = y;
					if (y == FILTER_MANUFACTURER)
					{
						file.gets(buffer, MAX_CHAR_INFO);
						char *ab = strchr(buffer, '=') + 2;
						for (size_t z = 0; z < c_mnfct::ui.size(); z++)
							if (!strncmp(ab, c_mnfct::ui[z].c_str(), c_mnfct::ui[z].length()))
								custfltr::mnfct[x] = z;
					}
					else if (y == FILTER_YEAR)
					{
						file.gets(buffer, MAX_CHAR_INFO);
						char *db = strchr(buffer, '=') + 2;
						for (size_t z = 0; z < c_year::ui.size(); z++)
							if (!strncmp(db, c_year::ui[z].c_str(), c_year::ui[z].length()))
								custfltr::year[x] = z;
					}
					else if (y == FILTER_SCREEN)
					{
						file.gets(buffer, MAX_CHAR_INFO);
						char *db = strchr(buffer, '=') + 2;
						for (size_t z = 0; z < c_screen::length; z++)
							if (!strncmp(db, c_screen::text[z], strlen(c_screen::text[z])))
								custfltr::screen[x] = z;
					}
				}
		}
		file.close();
	}

}


//-------------------------------------------------
//  draw left box
//-------------------------------------------------

float ui_mewui_select_game::draw_left_panel(float x1, float y1, float x2, float y2)
{
	if (mewui_globals::panels_status == SHOW_PANELS || mewui_globals::panels_status == HIDE_RIGHT_PANEL)
	{
		float origy1 = y1;
		float origy2 = y2;
		float text_size = 0.75f;
		float line_height = machine().ui().get_line_height() * text_size;
		float left_width = 0.0f;
		int text_lenght = main_filters::length;
		int afilter = main_filters::actual;
		int phover = HOVER_FILTER_FIRST;
		const char **text = main_filters::text;
		float sc = y2 - y1 - (2.0f * UI_BOX_TB_BORDER);

		if ((text_lenght * line_height) > sc)
		{
			float lm = sc / (text_lenght);
			text_size = lm / machine().ui().get_line_height();
			line_height = machine().ui().get_line_height() * text_size;
		}

		float text_sign = machine().ui().get_string_width_ex("_# ", text_size);
		for (int x = 0; x < text_lenght; x++)
		{
			float total_width;

			// compute width of left hand side
			total_width = machine().ui().get_string_width_ex(text[x], text_size);
			total_width += text_sign;

			// track the maximum
			if (total_width > left_width)
				left_width = total_width;
		}

		x2 = x1 + left_width + 2.0f * UI_BOX_LR_BORDER;
		//machine().ui().draw_outlined_box(container, x1, y1, x2, y2, rgb_t(0xEF, 0x12, 0x47, 0x7B));
		machine().ui().draw_outlined_box(container, x1, y1, x2, y2, UI_BACKGROUND_COLOR);

		// take off the borders
		x1 += UI_BOX_LR_BORDER;
		x2 -= UI_BOX_LR_BORDER;
		y1 += UI_BOX_TB_BORDER;
		y2 -= UI_BOX_TB_BORDER;

		for (int filter = 0; filter < text_lenght; filter++)
		{
			std::string str(text[filter]);
			rgb_t bgcolor = UI_TEXT_BG_COLOR;
			rgb_t fgcolor = UI_TEXT_COLOR;

			if (mouse_hit && x1 <= mouse_x && x2 > mouse_x && y1 <= mouse_y && y1 + line_height > mouse_y)
			{
				bgcolor = UI_MOUSEOVER_BG_COLOR;
				fgcolor = UI_MOUSEOVER_COLOR;
				hover = phover + filter;
			}

			if (afilter == filter)
			{
				bgcolor = UI_SELECTED_BG_COLOR;
				fgcolor = UI_SELECTED_COLOR;
			}

			if (bgcolor != UI_TEXT_BG_COLOR)
				container->add_rect(x1, y1, x2, y1 + line_height, bgcolor, PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA) | PRIMFLAG_TEXWRAP(TRUE));

			float x1t = x1 + text_sign;
			if (afilter == FILTER_CUSTOM)
			{
				if (filter == custfltr::main)
				{
					str.assign("@custom1 ").append(text[filter]);
					x1t -= text_sign;
				}
				else
				{
					for (int count = 1; count <= custfltr::numother; count++)
					{
						int cfilter = custfltr::other[count];
						if (cfilter == filter)
						{
							strprintf(str, "@custom%d %s", count + 1, text[filter]);
							x1t -= text_sign;
							break;
						}
					}
				}
				convert_command_glyph(str);
			}

			machine().ui().draw_text_full(container, str.c_str(), x1t, y1, x2 - x1, JUSTIFY_LEFT, WRAP_NEVER,
			                              DRAW_NORMAL, fgcolor, bgcolor, NULL, NULL, text_size);
			y1 += line_height;
		}

		x1 = x2 + UI_BOX_LR_BORDER;
		x2 = x1 + 2.0f * UI_BOX_LR_BORDER;
		y1 = origy1;
		y2 = origy2;
		line_height = machine().ui().get_line_height();
		float lr_arrow_width = 0.4f * line_height * machine().render().ui_aspect();
		rgb_t fgcolor = UI_TEXT_COLOR;

		// set left-right arrows dimension
		float ar_x0 = 0.5f * (x2 + x1) - 0.5f * lr_arrow_width;
		float ar_y0 = 0.5f * (y2 + y1) + 0.1f * line_height;
		float ar_x1 = ar_x0 + lr_arrow_width;
		float ar_y1 = 0.5f * (y2 + y1) + 0.9f * line_height;

		//machine().ui().draw_outlined_box(container, x1, y1, x2, y2, UI_BACKGROUND_COLOR);
		machine().ui().draw_outlined_box(container, x1, y1, x2, y2, rgb_t(0xEF, 0x12, 0x47, 0x7B));

		if (mouse_hit && x1 <= mouse_x && x2 > mouse_x && y1 <= mouse_y && y2 > mouse_y)
		{
			fgcolor = UI_MOUSEOVER_COLOR;
			hover = HOVER_LPANEL_ARROW;
		}

		draw_arrow(container, ar_x0, ar_y0, ar_x1, ar_y1, fgcolor, ROT90 ^ ORIENTATION_FLIP_X);
		return x2 + UI_BOX_LR_BORDER;
	}
	else
	{
		float line_height = machine().ui().get_line_height();
		float lr_arrow_width = 0.4f * line_height * machine().render().ui_aspect();
		rgb_t fgcolor = UI_TEXT_COLOR;

		// set left-right arrows dimension
		float ar_x0 = 0.5f * (x2 + x1) - 0.5f * lr_arrow_width;
		float ar_y0 = 0.5f * (y2 + y1) + 0.1f * line_height;
		float ar_x1 = ar_x0 + lr_arrow_width;
		float ar_y1 = 0.5f * (y2 + y1) + 0.9f * line_height;

		//machine().ui().draw_outlined_box(container, x1, y1, x2, y2, UI_BACKGROUND_COLOR);
		machine().ui().draw_outlined_box(container, x1, y1, x2, y2, rgb_t(0xEF, 0x12, 0x47, 0x7B));

		if (mouse_hit && x1 <= mouse_x && x2 > mouse_x && y1 <= mouse_y && y2 > mouse_y)
		{
			fgcolor = UI_MOUSEOVER_COLOR;
			hover = HOVER_LPANEL_ARROW;
		}

		draw_arrow(container, ar_x0, ar_y0, ar_x1, ar_y1, fgcolor, ROT90);
		return x2 + UI_BOX_LR_BORDER;
	}
}

//-------------------------------------------------
//  draw infos
//-------------------------------------------------

void ui_mewui_select_game::infos_render(void *selectedref, float origx1, float origy1, float origx2, float origy2)
{
	if (mewui_globals::panels_status == HIDE_RIGHT_PANEL || mewui_globals::panels_status == HIDE_BOTH)
	{
		float line_height = machine().ui().get_line_height();
		float lr_arrow_width = 0.4f * line_height * machine().render().ui_aspect();
		rgb_t fgcolor = UI_TEXT_COLOR;

		// set left-right arrows dimension
		float ar_x0 = 0.5f * (origx2 + origx1) - 0.5f * lr_arrow_width;
		float ar_y0 = 0.5f * (origy2 + origy1) + 0.1f * line_height;
		float ar_x1 = ar_x0 + lr_arrow_width;
		float ar_y1 = 0.5f * (origy2 + origy1) + 0.9f * line_height;

		//machine().ui().draw_outlined_box(container, origx1, origy1, origx2, origy2, UI_BACKGROUND_COLOR);
		machine().ui().draw_outlined_box(container, origx1, origy1, origx2, origy2, rgb_t(0xEF, 0x12, 0x47, 0x7B));

		if (mouse_hit && origx1 <= mouse_x && origx2 > mouse_x && origy1 <= mouse_y && origy2 > mouse_y)
		{
			fgcolor = UI_MOUSEOVER_COLOR;
			hover = HOVER_RPANEL_ARROW;
		}

		draw_arrow(container, ar_x0, ar_y0, ar_x1, ar_y1, fgcolor, ROT90 ^ ORIENTATION_FLIP_X);
		return;
	}
	else
	{
		float line_height = machine().ui().get_line_height();
		float lr_arrow_width = 0.4f * line_height * machine().render().ui_aspect();
		rgb_t fgcolor = UI_TEXT_COLOR;

		float x2 = origx1 + 2.0f * UI_BOX_LR_BORDER;
		float ar_x0 = 0.5f * (x2 + origx1) - 0.5f * lr_arrow_width;
		float ar_y0 = 0.5f * (origy2 + origy1) + 0.1f * line_height;
		float ar_x1 = ar_x0 + lr_arrow_width;
		float ar_y1 = 0.5f * (origy2 + origy1) + 0.9f * line_height;

		//machine().ui().draw_outlined_box(container, origx1, origy1, x2, origy2, UI_BACKGROUND_COLOR);
		machine().ui().draw_outlined_box(container, origx1, origy1, origx2, origy2, rgb_t(0xEF, 0x12, 0x47, 0x7B));

		if (mouse_hit && origx1 <= mouse_x && x2 > mouse_x && origy1 <= mouse_y && origy2 > mouse_y)
		{
			fgcolor = UI_MOUSEOVER_COLOR;
			hover = HOVER_RPANEL_ARROW;
		}

		draw_arrow(container, ar_x0, ar_y0, ar_x1, ar_y1, fgcolor, ROT90);
		origx1 = x2;
	}

	origy1 = draw_right_box_title(origx1, origy1, origx2, origy2);

	static std::string buffer;
	std::vector<int> xstart;
	std::vector<int> xend;

	float text_size = machine().options().infos_size();
	const game_driver *driver = NULL;
	ui_software_info *soft = NULL;
	bool is_favorites = ((item[0].flags & MENU_FLAG_MEWUI_FAVORITE) != 0);
	static ui_software_info *oldsoft = NULL;
	static const game_driver *olddriver = NULL;
	static int oldview = -1;
	static int old_sw_view = -1;

	if (is_favorites)
	{
		soft = ((FPTR)selectedref > 2) ? (ui_software_info *)selectedref : NULL;
		if (soft->startempty == 1)
		{
			driver = soft->driver;
			oldsoft = NULL;
		}
		else
			olddriver = NULL;
	}
	else
	{
		driver = ((FPTR)selectedref > 2) ? (const game_driver *)selectedref : NULL;
		oldsoft = NULL;
	}

	if (driver)
	{
		float line_height = machine().ui().get_line_height();
		float gutter_width = 0.4f * line_height * machine().render().ui_aspect() * 1.3f;
		float ud_arrow_width = line_height * machine().render().ui_aspect();
		float oy1 = origy1 + line_height;

		// MAMESCORE? Full size text
		if (mewui_globals::curdats_view == MEWUI_STORY_LOAD)
			text_size = 1.0f;

		std::string snaptext(dats_info[mewui_globals::curdats_view]);

		// apply title to right panel
		float title_size = 0.0f;
		float txt_lenght = 0.0f;

		for (int x = MEWUI_FIRST_LOAD; x < MEWUI_LAST_LOAD; x++)
		{
			machine().ui().draw_text_full(container, dats_info[x], origx1, origy1, origx2 - origx1, JUSTIFY_CENTER,
			                              WRAP_TRUNCATE, DRAW_NONE, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, &txt_lenght, NULL);
			txt_lenght += 0.01f;
			title_size = MAX(txt_lenght, title_size);
		}

		machine().ui().draw_text_full(container, snaptext.c_str(), origx1, origy1, origx2 - origx1, JUSTIFY_CENTER,
		                              WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL);

		draw_common_arrow(origx1, origy1, origx2, origy2, mewui_globals::curdats_view, MEWUI_FIRST_LOAD, MEWUI_LAST_LOAD, title_size);

		if (driver != olddriver || mewui_globals::curdats_view != oldview)
		{
			buffer.clear();
			olddriver = driver;
			oldview = mewui_globals::curdats_view;
			topline_datsview = 0;
			totallines = 0;
			std::vector<std::string> m_item;

			if (mewui_globals::curdats_view == MEWUI_GENERAL_LOAD)
				general_info(driver, buffer);
			else if (mewui_globals::curdats_view != MEWUI_COMMAND_LOAD)
				machine().datfile().load_data_info(driver, buffer, mewui_globals::curdats_view);
			else
				machine().datfile().command_sub_menu(driver, m_item);

			if (!m_item.empty() && mewui_globals::curdats_view == MEWUI_COMMAND_LOAD)
			{
				for (size_t x = 0; x < m_item.size(); x++)
				{
					std::string t_buffer;
					machine().datfile().load_command_info(t_buffer, x);
					buffer.append(m_item[x]).append("\n");
					if (!t_buffer.empty())
						buffer.append(t_buffer).append("\n");
				}
				convert_command_glyph(buffer);
			}
		}

		if (buffer.empty())
		{
			machine().ui().draw_text_full(container, "No Infos Available", origx1, (origy2 + origy1) * 0.5f, origx2 - origx1, JUSTIFY_CENTER,
			                              WRAP_WORD, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL);
			return;
		}
		else if (mewui_globals::curdats_view != MEWUI_STORY_LOAD && mewui_globals::curdats_view != MEWUI_COMMAND_LOAD)
			machine().ui().wrap_text(container, buffer.c_str(), origx1, origy1, origx2 - origx1 - (2.0f * gutter_width), totallines,
			                         xstart, xend, text_size);
		else
			machine().ui().wrap_text(container, buffer.c_str(), 0.0f, 0.0f, 1.0f - (2.0f * gutter_width), totallines, xstart, xend, text_size);

		int r_visible_lines = floor((origy2 - oy1) / (line_height * text_size));
		if (totallines < r_visible_lines)
			r_visible_lines = totallines;
		if (topline_datsview < 0)
			topline_datsview = 0;
		if (topline_datsview + r_visible_lines >= totallines)
			topline_datsview = totallines - r_visible_lines;

		for (int r = 0; r < r_visible_lines; r++)
		{
			int itemline = r + topline_datsview;
			std::string tempbuf;
			tempbuf.assign(buffer.substr(xstart[itemline], xend[itemline] - xstart[itemline]));

			// up arrow
			if (r == 0 && topline_datsview != 0)
				info_arrow(0, origx1, origx2, oy1, line_height, text_size, ud_arrow_width);
			// bottom arrow
			else if (r == r_visible_lines - 1 && itemline != totallines - 1)
				info_arrow(1, origx1, origx2, oy1, line_height, text_size, ud_arrow_width);
			// special case for mamescore
			else if (mewui_globals::curdats_view == MEWUI_STORY_LOAD)
			{
				int last_underscore = tempbuf.find_last_of('_');
				if (last_underscore == -1)
					machine().ui().draw_text_full(container, tempbuf.c_str(), origx1, oy1, origx2 - origx1, JUSTIFY_CENTER,
					                              WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL,
					                              text_size);
				else
				{
					float effective_width = origx2 - origx1 - gutter_width;
					float effective_left = origx1 + gutter_width;
					std::string last_part(tempbuf.substr(last_underscore + 1));
					int primary = tempbuf.find("___");
					std::string first_part(tempbuf.substr(0, primary));
					float item_width;

					machine().ui().draw_text_full(container, first_part.c_str(), effective_left, oy1, effective_width,
					                              JUSTIFY_LEFT, WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR,
					                              &item_width, NULL, text_size);

					machine().ui().draw_text_full(container, last_part.c_str(), effective_left + item_width, oy1,
					                              origx2 - origx1 - 2.0f * gutter_width - item_width, JUSTIFY_RIGHT,
					                              WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR,
					                              NULL, NULL, text_size);
					}
			}

			// special case for command
			else if (mewui_globals::curdats_view == MEWUI_COMMAND_LOAD || mewui_globals::curdats_view == MEWUI_GENERAL_LOAD)
			{
				int first_dspace = (mewui_globals::curdats_view == MEWUI_COMMAND_LOAD) ? tempbuf.find("  ") : tempbuf.find(":");
				if (first_dspace > 0)
				{
					float effective_width = origx2 - origx1 - gutter_width;
					float effective_left = origx1 + gutter_width;
					std::string first_part(tempbuf.substr(0, first_dspace));
					std::string last_part(tempbuf.substr(first_dspace + 1));
					strtrimspace(last_part);
					machine().ui().draw_text_full(container, first_part.c_str(), effective_left, oy1, effective_width,
					                              JUSTIFY_LEFT, WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR,
					                              NULL, NULL, text_size);

					machine().ui().draw_text_full(container, last_part.c_str(), effective_left, oy1, origx2 - origx1 - 2.0f * gutter_width,
					                              JUSTIFY_RIGHT, WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR,
					                              NULL, NULL, text_size);
				}
				else
					machine().ui().draw_text_full(container, tempbuf.c_str(), origx1 + gutter_width, oy1, origx2 - origx1, JUSTIFY_LEFT,
					                              WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL, text_size);
			}
			else
				machine().ui().draw_text_full(container, tempbuf.c_str(), origx1 + gutter_width, oy1, origx2 - origx1, JUSTIFY_LEFT,
				                              WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL, text_size);

			oy1 += (line_height * text_size);
		}

		// return the number of visible lines, minus 1 for top arrow and 1 for bottom arrow
		right_visible_lines = r_visible_lines - (topline_datsview != 0) - (topline_datsview + r_visible_lines != totallines);
	}
	else if (soft)
	{
		float line_height = machine().ui().get_line_height();
		float gutter_width = 0.4f * line_height * machine().render().ui_aspect() * 1.3f;
		float ud_arrow_width = line_height * machine().render().ui_aspect();
		float oy1 = origy1 + line_height;

		// apply title to right panel
		if (soft->usage.empty())
		{
			machine().ui().draw_text_full(container, "History", origx1, origy1, origx2 - origx1, JUSTIFY_CENTER, WRAP_TRUNCATE,
			                              DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL);
			mewui_globals::cur_sw_dats_view = 0;
		}
		else
		{
			float title_size = 0.0f;
			float txt_lenght = 0.0f;
			std::string t_text[2];
			t_text[0].assign("History");
			t_text[1].assign("Usage");

			for (int x = 0; x < 2; x++)
			{
				machine().ui().draw_text_full(container, t_text[x].c_str(), origx1, origy1, origx2 - origx1, JUSTIFY_CENTER, WRAP_TRUNCATE,
				                              DRAW_NONE, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, &txt_lenght, NULL);
				txt_lenght += 0.01f;
				title_size = MAX(txt_lenght, title_size);
			}

			machine().ui().draw_text_full(container, t_text[mewui_globals::cur_sw_dats_view].c_str(), origx1, origy1, origx2 - origx1,
			                              JUSTIFY_CENTER, WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR,
			                              NULL, NULL);

			draw_common_arrow(origx1, origy1, origx2, origy2, mewui_globals::cur_sw_dats_view, 0, 1, title_size);
		}

		if (oldsoft != soft || old_sw_view != mewui_globals::cur_sw_dats_view)
		{
			if (mewui_globals::cur_sw_dats_view == 0)
			{
				buffer.clear();
				old_sw_view = mewui_globals::cur_sw_dats_view;
				oldsoft = soft;
				if (soft->startempty == 1)
					machine().datfile().load_data_info(soft->driver, buffer, MEWUI_HISTORY_LOAD);
				else
					machine().datfile().load_software_info(soft->listname.c_str(), buffer, soft->shortname.c_str());
			}
			else
			{
				old_sw_view = mewui_globals::cur_sw_dats_view;
				oldsoft = soft;
				buffer.assign(soft->usage);
			}
		}

		if (buffer.empty())
		{
			machine().ui().draw_text_full(container, "No Infos Available", origx1, (origy2 + origy1) * 0.5f, origx2 - origx1, JUSTIFY_CENTER,
			                              WRAP_WORD, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, NULL, NULL);
			return;
		}
		else
			machine().ui().wrap_text(container, buffer.c_str(), origx1, origy1, origx2 - origx1 - (2.0f * gutter_width), totallines,
			                         xstart, xend, text_size);

		int r_visible_lines = floor((origy2 - oy1) / (line_height * text_size));
		if (totallines < r_visible_lines)
			r_visible_lines = totallines;
		if (topline_datsview < 0)
				topline_datsview = 0;
		if (topline_datsview + r_visible_lines >= totallines)
				topline_datsview = totallines - r_visible_lines;

		for (int r = 0; r < r_visible_lines; r++)
		{
			int itemline = r + topline_datsview;
			std::string tempbuf;
			tempbuf.assign(buffer.substr(xstart[itemline], xend[itemline] - xstart[itemline]));

			// up arrow
			if (r == 0 && topline_datsview != 0)
				info_arrow(0, origx1, origx2, oy1, line_height, text_size, ud_arrow_width);
			// bottom arrow
			else if (r == r_visible_lines - 1 && itemline != totallines - 1)
				info_arrow(1, origx1, origx2, oy1, line_height, text_size, ud_arrow_width);
			else
				machine().ui().draw_text_full(container, tempbuf.c_str(), origx1 + gutter_width, oy1, origx2 - origx1,
				                              JUSTIFY_LEFT, WRAP_TRUNCATE, DRAW_NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR,
				                              NULL, NULL, text_size);
			oy1 += (line_height * text_size);
		}

		// return the number of visible lines, minus 1 for top arrow and 1 for bottom arrow
		right_visible_lines = r_visible_lines - (topline_datsview != 0) - (topline_datsview + r_visible_lines != totallines);
	}
}

void ui_mewui_select_game::draw_right_panel(void *selectedref, float x1, float y1, float x2, float y2)
{
	if (mewui_globals::rpanel == RP_IMAGES)
		arts_render(selectedref, x1, y1, x2, y2);
	else
		infos_render(selectedref, x1, y1, x2, y2);
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void ui_mewui_select_game::arts_render(void *selectedref, float origx1, float origy1, float origx2, float origy2)
{
	if (mewui_globals::panels_status == HIDE_RIGHT_PANEL || mewui_globals::panels_status == HIDE_BOTH)
	{
		float line_height = machine().ui().get_line_height();
		float lr_arrow_width = 0.4f * line_height * machine().render().ui_aspect();
		rgb_t fgcolor = UI_TEXT_COLOR;

		// set left-right arrows dimension
		float ar_x0 = 0.5f * (origx2 + origx1) - 0.5f * lr_arrow_width;
		float ar_y0 = 0.5f * (origy2 + origy1) + 0.1f * line_height;
		float ar_x1 = ar_x0 + lr_arrow_width;
		float ar_y1 = 0.5f * (origy2 + origy1) + 0.9f * line_height;

		//machine().ui().draw_outlined_box(container, origx1, origy1, origx2, origy2, UI_BACKGROUND_COLOR);
		machine().ui().draw_outlined_box(container, origx1, origy1, origx2, origy2, rgb_t(0xEF, 0x12, 0x47, 0x7B));

		if (mouse_hit && origx1 <= mouse_x && origx2 > mouse_x && origy1 <= mouse_y && origy2 > mouse_y)
		{
			fgcolor = UI_MOUSEOVER_COLOR;
			hover = HOVER_RPANEL_ARROW;
		}

		draw_arrow(container, ar_x0, ar_y0, ar_x1, ar_y1, fgcolor, ROT90 ^ ORIENTATION_FLIP_X);
		return;
	}
	else
	{
		float line_height = machine().ui().get_line_height();
		float lr_arrow_width = 0.4f * line_height * machine().render().ui_aspect();
		rgb_t fgcolor = UI_TEXT_COLOR;

		float x2 = origx1 + 2.0f * UI_BOX_LR_BORDER;
		// set left-right arrows dimension
		float ar_x0 = 0.5f * (x2 + origx1) - 0.5f * lr_arrow_width;
		float ar_y0 = 0.5f * (origy2 + origy1) + 0.1f * line_height;
		float ar_x1 = ar_x0 + lr_arrow_width;
		float ar_y1 = 0.5f * (origy2 + origy1) + 0.9f * line_height;

		//machine().ui().draw_outlined_box(container, origx1, origy1, x2, origy2, UI_BACKGROUND_COLOR);
		machine().ui().draw_outlined_box(container, origx1, origy1, origx2, origy2, rgb_t(0xEF, 0x12, 0x47, 0x7B));

		if (mouse_hit && origx1 <= mouse_x && x2 > mouse_x && origy1 <= mouse_y && origy2 > mouse_y)
		{
			fgcolor = UI_MOUSEOVER_COLOR;
			hover = HOVER_RPANEL_ARROW;
		}

		draw_arrow(container, ar_x0, ar_y0, ar_x1, ar_y1, fgcolor, ROT90);
		origx1 = x2;
	}

	origy1 = draw_right_box_title(origx1, origy1, origx2, origy2);

	bool is_favorites = ((item[0].flags & MENU_FLAG_MEWUI_FAVORITE) != 0);
	static ui_software_info *oldsoft = NULL;
	static const game_driver *olddriver = NULL;
	const game_driver *driver = NULL;
	ui_software_info *soft = NULL;

	if (is_favorites)
	{
		soft = ((FPTR)selectedref > 2) ? (ui_software_info *)selectedref : NULL;
		if (soft && soft->startempty == 1)
		{
			driver = soft->driver;
			oldsoft = NULL;
		}
		else
			olddriver = NULL;
	}
	else
	{
		driver = ((FPTR)selectedref > 2) ? (const game_driver *)selectedref : NULL;
		oldsoft = NULL;
	}

	if (driver)
	{
		float line_height = machine().ui().get_line_height();
		if (mewui_globals::default_image)
			((driver->flags & MACHINE_TYPE_ARCADE) == 0) ? mewui_globals::curimage_view = CABINETS_VIEW : mewui_globals::curimage_view = SNAPSHOT_VIEW;

		std::string searchstr;
		searchstr = arts_render_common(origx1, origy1, origx2, origy2);

		// loads the image if necessary
		if (driver != olddriver || !snapx_bitmap->valid() || mewui_globals::switch_image)
		{
			emu_file snapfile(searchstr.c_str(), OPEN_FLAG_READ);
			bitmap_argb32 *tmp_bitmap;
			tmp_bitmap = auto_alloc(machine(), bitmap_argb32);

			// try to load snapshot first from saved "0000.png" file
			std::string fullname(driver->name);
			render_load_png(*tmp_bitmap, snapfile, fullname.c_str(), "0000.png");

			if (!tmp_bitmap->valid())
				render_load_jpeg(*tmp_bitmap, snapfile, fullname.c_str(), "0000.jpg");

			// if fail, attemp to load from standard file
			if (!tmp_bitmap->valid())
			{
				fullname.assign(driver->name).append(".png");
				render_load_png(*tmp_bitmap, snapfile, NULL, fullname.c_str());

				if (!tmp_bitmap->valid())
				{
					fullname.assign(driver->name).append(".jpg");
					render_load_jpeg(*tmp_bitmap, snapfile, NULL, fullname.c_str());
				}
			}

			// if fail again, attemp to load from parent file
			if (!tmp_bitmap->valid())
			{
				// set clone status
				bool cloneof = strcmp(driver->parent, "0");
				if (cloneof)
				{
					int cx = driver_list::find(driver->parent);
					if (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0))
						cloneof = false;
				}

				if (cloneof)
				{
					fullname.assign(driver->parent).append(".png");
					render_load_png(*tmp_bitmap, snapfile, NULL, fullname.c_str());

					if (!tmp_bitmap->valid())
					{
						fullname.assign(driver->parent).append(".jpg");
						render_load_jpeg(*tmp_bitmap, snapfile, NULL, fullname.c_str());
					}
				}
			}

			olddriver = driver;
			mewui_globals::switch_image = false;
			arts_render_images(tmp_bitmap, origx1, origy1, origx2, origy2, false);
			auto_free(machine(), tmp_bitmap);
		}

		// if the image is available, loaded and valid, display it
		if (snapx_bitmap->valid())
		{
			float x1 = origx1 + 0.01f;
			float x2 = origx2 - 0.01f;
			float y1 = origy1 + UI_BOX_TB_BORDER + line_height;
			float y2 = origy2 - UI_BOX_TB_BORDER - line_height;

			// apply texture
			container->add_quad( x1, y1, x2, y2, ARGB_WHITE, snapx_texture, PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA));
		}
	}
	else if (soft)
	{
		float line_height = machine().ui().get_line_height();
		std::string fullname, pathname;

		if (mewui_globals::default_image)
			(soft->startempty == 0) ? mewui_globals::curimage_view = SNAPSHOT_VIEW : mewui_globals::curimage_view = CABINETS_VIEW;

		// arts title and searchpath
		std::string searchstr;
		searchstr = arts_render_common(origx1, origy1, origx2, origy2);

		// loads the image if necessary
		if (soft != oldsoft || !snapx_bitmap->valid() || mewui_globals::switch_image)
		{
			emu_file snapfile(searchstr.c_str(), OPEN_FLAG_READ);
			bitmap_argb32 *tmp_bitmap;
			tmp_bitmap = auto_alloc(machine(), bitmap_argb32);

			if (soft->startempty == 1)
			{
				// Load driver snapshot
				fullname.assign(soft->driver->name).append(".png");
				render_load_png(*tmp_bitmap, snapfile, NULL, fullname.c_str());

				if (!tmp_bitmap->valid())
				{
					fullname.assign(soft->driver->name).append(".jpg");
					render_load_jpeg(*tmp_bitmap, snapfile, NULL, fullname.c_str());
				}
			}
			else if (mewui_globals::curimage_view == TITLES_VIEW)
			{
				// First attempt from name list
				pathname.assign(soft->listname.c_str()).append("_titles");
				fullname.assign(soft->shortname.c_str()).append(".png");
				render_load_png(*tmp_bitmap, snapfile, pathname.c_str(), fullname.c_str());

				if (!tmp_bitmap->valid())
				{
					fullname.assign(soft->shortname.c_str()).append(".jpg");
					render_load_jpeg(*tmp_bitmap, snapfile, pathname.c_str(), fullname.c_str());
				}
			}
			else
			{
				// First attempt from name list
				pathname.assign(soft->listname.c_str());
				fullname.assign(soft->shortname.c_str()).append(".png");
				render_load_png(*tmp_bitmap, snapfile, pathname.c_str(), fullname.c_str());

				if (!tmp_bitmap->valid())
				{
					fullname.assign(soft->shortname.c_str()).append(".jpg");
					render_load_jpeg(*tmp_bitmap, snapfile, pathname.c_str(), fullname.c_str());
				}

				if (!tmp_bitmap->valid())
				{
					// Second attempt from driver name + part name
					pathname.assign(soft->driver->name).append(soft->part.c_str());
					fullname.assign(soft->shortname.c_str()).append(".png");
					render_load_png(*tmp_bitmap, snapfile, pathname.c_str(), fullname.c_str());

					if (!tmp_bitmap->valid())
					{
						fullname.assign(soft->shortname.c_str()).append(".jpg");
						render_load_jpeg(*tmp_bitmap, snapfile, pathname.c_str(), fullname.c_str());
					}
				}
			}

			oldsoft = soft;
			mewui_globals::switch_image = false;
			arts_render_images(tmp_bitmap, origx1, origy1, origx2, origy2, true);
			auto_free(machine(), tmp_bitmap);
		}

		// if the image is available, loaded and valid, display it
		if (snapx_bitmap->valid())
		{
			float x1 = origx1 + 0.01f;
			float x2 = origx2 - 0.01f;
			float y1 = origy1 + UI_BOX_TB_BORDER + line_height;
			float y2 = origy2 - UI_BOX_TB_BORDER - line_height;

			// apply texture
			container->add_quad(x1, y1, x2, y2, ARGB_WHITE, snapx_texture, PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA));
		}
	}
}
