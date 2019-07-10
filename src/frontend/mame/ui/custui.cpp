// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/*********************************************************************

    ui/custui.cpp

    Internal UI user interface.

*********************************************************************/

#include "emu.h"

#include "ui/custui.h"

#include "ui/ui.h"
#include "ui/selector.h"
#include "ui/utils.h"

#include "drivenum.h"
#include "emuopts.h"
#include "osdepend.h"
#include "uiinput.h"

#include <algorithm>
#include <utility>


namespace ui {

const char *const menu_custom_ui::HIDE_STATUS[] = {
		__("Show All"),
		__("Hide Filters"),
		__("Hide Info/Image"),
		__("Hide Both") };

//-------------------------------------------------
//  ctor
//-------------------------------------------------

menu_custom_ui::menu_custom_ui(mame_ui_manager &mui, render_container &container) : menu(mui, container)
{
	// load languages
	file_enumerator path(mui.machine().options().language_path());
	auto lang = mui.machine().options().language();
	const osd::directory::entry *dirent;
	std::size_t cnt = 0;
	while ((dirent = path.next()))
	{
		if (dirent->type == osd::directory::entry::entry_type::DIR && strcmp(dirent->name, ".") != 0 && strcmp(dirent->name, "..") != 0)
		{
			auto name = std::string(dirent->name);
			auto i = strreplace(name, "_", " (");
			if (i > 0) name = name.append(")");
			m_lang.push_back(name);
			if (strcmp(name.c_str(), lang) == 0)
				m_currlang = cnt;
			++cnt;
		}
	}
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

menu_custom_ui::~menu_custom_ui()
{
	ui().options().set_value(OPTION_HIDE_PANELS, ui_globals::panels_status, OPTION_PRIORITY_CMDLINE);
	if (!m_lang.empty())
	{
		machine().options().set_value(OPTION_LANGUAGE, m_lang[m_currlang].c_str(), OPTION_PRIORITY_CMDLINE);
		load_translation(machine().options());
	}
	ui_globals::reset = true;
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void menu_custom_ui::handle()
{
	bool changed = false;

	// process the menu
	const event *menu_event = process(0);

	if (menu_event != nullptr && menu_event->itemref != nullptr)
	{
		switch ((uintptr_t)menu_event->itemref)
		{
		case FONT_MENU:
			if (menu_event->iptkey == IPT_UI_SELECT)
				menu::stack_push<menu_font_ui>(ui(), container());
			break;
		case COLORS_MENU:
			if (menu_event->iptkey == IPT_UI_SELECT)
				menu::stack_push<menu_colors_ui>(ui(), container());
			break;
		case HIDE_MENU:
			if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT)
			{
				changed = true;
				(menu_event->iptkey == IPT_UI_RIGHT) ? ui_globals::panels_status++ : ui_globals::panels_status--;
			}
			else if (menu_event->iptkey == IPT_UI_SELECT)
			{
				std::vector<std::string> s_sel(ARRAY_LENGTH(HIDE_STATUS));
				std::transform(std::begin(HIDE_STATUS), std::end(HIDE_STATUS), s_sel.begin(), [](auto &s) { return _(s); });
				menu::stack_push<menu_selector>(
						ui(), container(), std::move(s_sel), ui_globals::panels_status,
						[this] (int selection)
						{
							ui_globals::panels_status = selection;
							reset(reset_options::REMEMBER_REF);
						});
			}
			break;
		case LANGUAGE_MENU:
			if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT)
			{
				changed = true;
				(menu_event->iptkey == IPT_UI_RIGHT) ? m_currlang++ : m_currlang--;
			}
			else if (menu_event->iptkey == IPT_UI_SELECT)
			{
				// copying list of language names - expensive
				menu::stack_push<menu_selector>(
						ui(), container(), std::vector<std::string>(m_lang), m_currlang,
						[this] (int selection)
						{
							m_currlang = selection;
							reset(reset_options::REMEMBER_REF);
						});
			}
			break;
		}
	}

	if (changed)
		reset(reset_options::REMEMBER_REF);
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void menu_custom_ui::populate(float &customtop, float &custombottom)
{
	uint32_t arrow_flags;
	item_append(_("Fonts"), "", 0, (void *)(uintptr_t)FONT_MENU);
	item_append(_("Colors"), "", 0, (void *)(uintptr_t)COLORS_MENU);

	if (!m_lang.empty())
	{
		arrow_flags = get_arrow_flags<std::uint16_t>(0, m_lang.size() - 1, m_currlang);
		item_append(_("Language"), m_lang[m_currlang].c_str(), arrow_flags, (void *)(uintptr_t)LANGUAGE_MENU);
	}

	arrow_flags = get_arrow_flags<uint16_t>(0, HIDE_BOTH, ui_globals::panels_status);
	item_append(_("Show side panels"), _(HIDE_STATUS[ui_globals::panels_status]), arrow_flags, (void *)(uintptr_t)HIDE_MENU);

	item_append(menu_item_type::SEPARATOR);
	customtop = ui().get_line_height() + 3.0f * ui().box_tb_border();
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void menu_custom_ui::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	char const *const text[] = { _("Custom UI Settings") };
	draw_text_box(
			std::begin(text), std::end(text),
			origx1, origx2, origy1 - top, origy1 - ui().box_tb_border(),
			ui::text_layout::CENTER, ui::text_layout::TRUNCATE, false,
			ui().colors().text_color(), UI_GREEN_COLOR, 1.0f);
}

//-------------------------------------------------
//  ctor
//-------------------------------------------------

menu_font_ui::menu_font_ui(mame_ui_manager &mui, render_container &container) : menu(mui, container)
{
	ui_options &moptions = mui.options();
	std::string name(mui.machine().options().ui_font());
	list();

#ifdef UI_WINDOWS
	m_bold = (strreplace(name, "[B]", "") + strreplace(name, "[b]", "") > 0);
	m_italic = (strreplace(name, "[I]", "") + strreplace(name, "[i]", "") > 0);
#endif
	m_actual = 0;

	for (std::size_t index = 0; index < m_fonts.size(); index++)
	{
		if (m_fonts[index].first == name)
		{
			m_actual = index;
			break;
		}
	}

	m_info_size = moptions.infos_size();
	m_font_size = moptions.font_rows();
	m_info_max = atof(moptions.get_entry(OPTION_INFOS_SIZE)->maximum());
	m_info_min = atof(moptions.get_entry(OPTION_INFOS_SIZE)->minimum());
	m_font_max = atof(moptions.get_entry(OPTION_FONT_ROWS)->maximum());
	m_font_min = atof(moptions.get_entry(OPTION_FONT_ROWS)->minimum());
}

//-------------------------------------------------
//  create fonts list
//-------------------------------------------------

void menu_font_ui::list()
{
	machine().osd().get_font_families(machine().options().font_path(), m_fonts);

	// add default string to the top of array
	m_fonts.emplace(m_fonts.begin(), std::string("default"), std::string(_("default")));
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

menu_font_ui::~menu_font_ui()
{
	std::string error_string;
	ui_options &moptions = ui().options();

	std::string name(m_fonts[m_actual].first);
#ifdef UI_WINDOWS
	if (name != "default")
	{
		if (m_italic)
			name.insert(0, "[I]");
		if (m_bold)
			name.insert(0, "[B]");
	}
#endif
	machine().options().set_value(OPTION_UI_FONT, name, OPTION_PRIORITY_CMDLINE);
	moptions.set_value(OPTION_INFOS_SIZE, m_info_size, OPTION_PRIORITY_CMDLINE);
	moptions.set_value(OPTION_FONT_ROWS, m_font_size, OPTION_PRIORITY_CMDLINE);

	// OPTION_FONT_ROWS was changed; update the font info
	ui().update_target_font_height();
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void menu_font_ui::handle()
{
	bool changed = false;

	// process the menu
	const event *menu_event = process(PROCESS_LR_REPEAT);

	if (menu_event != nullptr && menu_event->itemref != nullptr)
		switch ((uintptr_t)menu_event->itemref)
		{
			case INFOS_SIZE:
				if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT)
				{
					(menu_event->iptkey == IPT_UI_RIGHT) ? m_info_size += 0.05f : m_info_size -= 0.05f;
					changed = true;
				}
				break;

			case FONT_SIZE:
				if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT)
				{
					(menu_event->iptkey == IPT_UI_RIGHT) ? m_font_size++ : m_font_size--;
					changed = true;
				}
				break;


			case MUI_FNT:
				if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT)
				{
					(menu_event->iptkey == IPT_UI_RIGHT) ? m_actual++ : m_actual--;
					changed = true;
				}
				else if (menu_event->iptkey == IPT_UI_SELECT)
				{
					std::vector<std::string> display_names;
					display_names.reserve(m_fonts.size());
					for (auto const &font : m_fonts)
						display_names.emplace_back(font.second);
					menu::stack_push<menu_selector>(
							ui(), container(), std::move(display_names), m_actual,
							[this] (int selection)
							{
								m_actual = selection;
								reset(reset_options::REMEMBER_REF);
							});
					changed = true;
				}
				break;

#ifdef UI_WINDOWS
			case MUI_BOLD:
			case MUI_ITALIC:
				if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT || menu_event->iptkey == IPT_UI_SELECT)
				{
					((uintptr_t)menu_event->itemref == MUI_BOLD) ? m_bold = !m_bold : m_italic = !m_italic;
					changed = true;
				}
				break;
#endif
		}

	if (changed)
		reset(reset_options::REMEMBER_REF);
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void menu_font_ui::populate(float &customtop, float &custombottom)
{
	// set filter arrow
	uint32_t arrow_flags;

	// add fonts option
	arrow_flags = get_arrow_flags<std::uint16_t>(0, m_fonts.size() - 1, m_actual);
	item_append(_("UI Font"), m_fonts[m_actual].second, arrow_flags, (void *)(uintptr_t)MUI_FNT);

#ifdef UI_WINDOWS
	if (m_fonts[m_actual].first != "default")
	{
		item_append_on_off(_("Bold"), m_bold, 0, (void *)(uintptr_t)MUI_BOLD);
		item_append_on_off(_("Italic"), m_italic, 0, (void *)(uintptr_t)MUI_ITALIC);
	}
#endif

	arrow_flags = get_arrow_flags(m_font_min, m_font_max, m_font_size);
	item_append(_("Lines"), string_format("%2d", m_font_size), arrow_flags, (void *)(uintptr_t)FONT_SIZE);

	item_append(menu_item_type::SEPARATOR);

	// add item
	arrow_flags = get_arrow_flags(m_info_min, m_info_max, m_info_size);
	item_append(_("Infos text size"), string_format("%3.2f", m_info_size), arrow_flags, (void *)(uintptr_t)INFOS_SIZE);

	item_append(menu_item_type::SEPARATOR);

	custombottom = customtop = ui().get_line_height() + 3.0f * ui().box_tb_border();
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void menu_font_ui::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	// top text
	char const *const toptext[] = { _("UI Fonts Settings") };
	draw_text_box(
			std::begin(toptext), std::end(toptext),
			origx1, origx2, origy1 - top, origy1 - ui().box_tb_border(),
			ui::text_layout::CENTER, ui::text_layout::TRUNCATE, false,
			ui().colors().text_color(), UI_GREEN_COLOR, 1.0f);

	if (uintptr_t(selectedref) == INFOS_SIZE)
	{
		char const *const bottomtext[] = { _("Sample text - Lorem ipsum dolor sit amet, consectetur adipiscing elit.") };
		draw_text_box(
				std::begin(bottomtext), std::end(bottomtext),
				origx1, origx2, origy2 + ui().box_tb_border(), origy2 + bottom,
				ui::text_layout::LEFT, ui::text_layout::NEVER, false,
				ui().colors().text_color(), UI_GREEN_COLOR, m_info_size);
	}
}

//-------------------------------------------------
//  ctor
//-------------------------------------------------
#define SET_COLOR_UI(var, opt) var[M##opt].color = mui.options().rgb_value(OPTION_##opt); var[M##opt].option = OPTION_##opt

menu_colors_ui::menu_colors_ui(mame_ui_manager &mui, render_container &container) : menu(mui, container)
{
	SET_COLOR_UI(m_color_table, UI_BACKGROUND_COLOR);
	SET_COLOR_UI(m_color_table, UI_BORDER_COLOR);
	SET_COLOR_UI(m_color_table, UI_CLONE_COLOR);
	SET_COLOR_UI(m_color_table, UI_DIPSW_COLOR);
	SET_COLOR_UI(m_color_table, UI_GFXVIEWER_BG_COLOR);
	SET_COLOR_UI(m_color_table, UI_MOUSEDOWN_BG_COLOR);
	SET_COLOR_UI(m_color_table, UI_MOUSEDOWN_COLOR);
	SET_COLOR_UI(m_color_table, UI_MOUSEOVER_BG_COLOR);
	SET_COLOR_UI(m_color_table, UI_MOUSEOVER_COLOR);
	SET_COLOR_UI(m_color_table, UI_SELECTED_BG_COLOR);
	SET_COLOR_UI(m_color_table, UI_SELECTED_COLOR);
	SET_COLOR_UI(m_color_table, UI_SLIDER_COLOR);
	SET_COLOR_UI(m_color_table, UI_SUBITEM_COLOR);
	SET_COLOR_UI(m_color_table, UI_TEXT_BG_COLOR);
	SET_COLOR_UI(m_color_table, UI_TEXT_COLOR);
	SET_COLOR_UI(m_color_table, UI_UNAVAILABLE_COLOR);
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

menu_colors_ui::~menu_colors_ui()
{
	std::string dec_color;
	for (int index = 1; index < MUI_RESTORE; index++)
	{
		dec_color = string_format("%x", (uint32_t)m_color_table[index].color);
		ui().options().set_value(m_color_table[index].option, dec_color.c_str(), OPTION_PRIORITY_CMDLINE);
	}

	// refresh our cached colors
	ui().colors().refresh(ui().options());
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void menu_colors_ui::handle()
{
	bool changed = false;

	// process the menu
	const event *menu_event = process(0);

	if (menu_event != nullptr && menu_event->itemref != nullptr && menu_event->iptkey == IPT_UI_SELECT)
	{
		if ((uintptr_t)menu_event->itemref != MUI_RESTORE)
			menu::stack_push<menu_rgb_ui>(ui(), container(), &m_color_table[(uintptr_t)menu_event->itemref].color, selected_item().text);
		else
		{
			changed = true;
			restore_colors();
		}
	}

	if (changed)
		reset(reset_options::REMEMBER_REF);
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void menu_colors_ui::populate(float &customtop, float &custombottom)
{
	item_append(_("Normal text"), "", 0, (void *)(uintptr_t)MUI_TEXT_COLOR);
	item_append(_("Selected color"), "", 0, (void *)(uintptr_t)MUI_SELECTED_COLOR);
	item_append(_("Normal text background"), "", 0, (void *)(uintptr_t)MUI_TEXT_BG_COLOR);
	item_append(_("Selected background color"), "", 0, (void *)(uintptr_t)MUI_SELECTED_BG_COLOR);
	item_append(_("Subitem color"), "", 0, (void *)(uintptr_t)MUI_SUBITEM_COLOR);
	item_append(_("Clone"), "", 0, (void *)(uintptr_t)MUI_CLONE_COLOR);
	item_append(_("Border"), "", 0, (void *)(uintptr_t)MUI_BORDER_COLOR);
	item_append(_("Background"), "", 0, (void *)(uintptr_t)MUI_BACKGROUND_COLOR);
	item_append(_("Dipswitch"), "", 0, (void *)(uintptr_t)MUI_DIPSW_COLOR);
	item_append(_("Unavailable color"), "", 0, (void *)(uintptr_t)MUI_UNAVAILABLE_COLOR);
	item_append(_("Slider color"), "", 0, (void *)(uintptr_t)MUI_SLIDER_COLOR);
	item_append(_("Gfx viewer background"), "", 0, (void *)(uintptr_t)MUI_GFXVIEWER_BG_COLOR);
	item_append(_("Mouse over color"), "", 0, (void *)(uintptr_t)MUI_MOUSEOVER_COLOR);
	item_append(_("Mouse over background color"), "", 0, (void *)(uintptr_t)MUI_MOUSEOVER_BG_COLOR);
	item_append(_("Mouse down color"), "", 0, (void *)(uintptr_t)MUI_MOUSEDOWN_COLOR);
	item_append(_("Mouse down background color"), "", 0, (void *)(uintptr_t)MUI_MOUSEDOWN_BG_COLOR);

	item_append(menu_item_type::SEPARATOR);
	item_append(_("Restore originals colors"), "", 0, (void *)(uintptr_t)MUI_RESTORE);

	custombottom = customtop = ui().get_line_height() + 3.0f * ui().box_tb_border();
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void menu_colors_ui::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	// top text
	char const *const toptext[] = { _("UI Colors Settings") };
	draw_text_box(
			std::begin(toptext), std::end(toptext),
			origx1, origx2, origy1 - top, origy1 - ui().box_tb_border(),
			ui::text_layout::CENTER, ui::text_layout::TRUNCATE, false,
			ui().colors().text_color(), UI_GREEN_COLOR, 1.0f);

	// bottom text
	// get the text for 'UI Select'
	std::string const bottomtext[] = { util::string_format(_("Double click or press %1$s to change the color value"), machine().input().seq_name(machine().ioport().type_seq(IPT_UI_SELECT, 0, SEQ_TYPE_STANDARD))) };
	draw_text_box(
			std::begin(bottomtext), std::end(bottomtext),
			origx1, origx2, origy2 + ui().box_tb_border(), origy2 + bottom,
			ui::text_layout::CENTER, ui::text_layout::TRUNCATE, false,
			ui().colors().text_color(), UI_RED_COLOR, 1.0f);

	// compute maxwidth
	char const *const topbuf = _("Menu Preview");

	float width;
	ui().draw_text_full(container(), topbuf, 0.0f, 0.0f, 1.0f, ui::text_layout::CENTER, ui::text_layout::NEVER,
									mame_ui_manager::NONE, rgb_t::white(), rgb_t::black(), &width, nullptr);
	float maxwidth = width + 2.0f * ui().box_lr_border();

	std::string sampletxt[5];

	sampletxt[0] = _("Normal");
	sampletxt[1] = _("Subitem");
	sampletxt[2] = _("Selected");
	sampletxt[3] = _("Mouse Over");
	sampletxt[4] = _("Clone");

	for (auto & elem: sampletxt)
	{
		ui().draw_text_full(container(), elem.c_str(), 0.0f, 0.0f, 1.0f, ui::text_layout::CENTER, ui::text_layout::NEVER,
										mame_ui_manager::NONE, rgb_t::white(), rgb_t::black(), &width, nullptr);
		width += 2 * ui().box_lr_border();
		maxwidth = std::max(maxwidth, width);
	}

	// compute our bounds for header
	float x1 = origx2 + 2.0f * ui().box_lr_border();
	float x2 = x1 + maxwidth;
	float y1 = origy1;
	float y2 = y1 + bottom - ui().box_tb_border();

	// draw a box
	ui().draw_outlined_box(container(), x1, y1, x2, y2, UI_GREEN_COLOR);

	// take off the borders
	x1 += ui().box_lr_border();
	x2 -= ui().box_lr_border();
	y1 += ui().box_tb_border();
	y2 -= ui().box_tb_border();

	// draw the text within it
	ui().draw_text_full(container(), topbuf, x1, y1, x2 - x1, ui::text_layout::CENTER, ui::text_layout::NEVER,
									mame_ui_manager::NORMAL, ui().colors().text_color(), ui().colors().text_bg_color(), nullptr, nullptr);

	// compute our bounds for menu preview
	float line_height = ui().get_line_height();
	x1 -= ui().box_lr_border();
	x2 += ui().box_lr_border();
	y1 = y2 + 2.0f * ui().box_tb_border();
	y2 = y1 + 5.0f * line_height + 2.0f * ui().box_tb_border();

	// draw a box
	ui().draw_outlined_box(container(), x1, y1, x2, y2, m_color_table[MUI_BACKGROUND_COLOR].color);

	// take off the borders
	x1 += ui().box_lr_border();
	x2 -= ui().box_lr_border();
	y1 += ui().box_tb_border();

	// draw normal text
	ui().draw_text_full(container(), sampletxt[0].c_str(), x1, y1, x2 - x1, ui::text_layout::CENTER, ui::text_layout::NEVER,
									mame_ui_manager::NORMAL, m_color_table[MUI_TEXT_COLOR].color, m_color_table[MUI_TEXT_BG_COLOR].color, nullptr, nullptr);
	y1 += line_height;

	// draw subitem text
	ui().draw_text_full(container(), sampletxt[1].c_str(), x1, y1, x2 - x1, ui::text_layout::CENTER, ui::text_layout::NEVER,
									mame_ui_manager::NORMAL, m_color_table[MUI_SUBITEM_COLOR].color, m_color_table[MUI_TEXT_BG_COLOR].color, nullptr, nullptr);
	y1 += line_height;

	// draw selected text
	highlight(x1, y1, x2, y1 + line_height, m_color_table[MUI_SELECTED_BG_COLOR].color);
	ui().draw_text_full(container(), sampletxt[2].c_str(), x1, y1, x2 - x1, ui::text_layout::CENTER, ui::text_layout::NEVER,
									mame_ui_manager::NORMAL, m_color_table[MUI_SELECTED_COLOR].color, m_color_table[MUI_SELECTED_BG_COLOR].color, nullptr, nullptr);
	y1 += line_height;

	// draw mouse over text
	highlight(x1, y1, x2, y1 + line_height, m_color_table[MUI_MOUSEOVER_BG_COLOR].color);
	ui().draw_text_full(container(), sampletxt[3].c_str(), x1, y1, x2 - x1, ui::text_layout::CENTER, ui::text_layout::NEVER,
									mame_ui_manager::NORMAL, m_color_table[MUI_MOUSEOVER_COLOR].color, m_color_table[MUI_MOUSEOVER_BG_COLOR].color, nullptr, nullptr);
	y1 += line_height;

	// draw clone text
	ui().draw_text_full(container(), sampletxt[4].c_str(), x1, y1, x2 - x1, ui::text_layout::CENTER, ui::text_layout::NEVER,
									mame_ui_manager::NORMAL, m_color_table[MUI_CLONE_COLOR].color, m_color_table[MUI_TEXT_BG_COLOR].color, nullptr, nullptr);

}

//-------------------------------------------------
//  restore original colors
//-------------------------------------------------

void menu_colors_ui::restore_colors()
{
	ui_options options;
	for (int index = 1; index < MUI_RESTORE; index++)
		m_color_table[index].color = rgb_t((uint32_t)strtoul(options.value(m_color_table[index].option), nullptr, 16));
}

//-------------------------------------------------
//  ctor
//-------------------------------------------------

menu_rgb_ui::menu_rgb_ui(mame_ui_manager &mui, render_container &container, rgb_t *_color, std::string _title)
	: menu(mui, container),
		m_color(_color),
		m_search(),
		m_key_active(false),
		m_lock_ref(0),
		m_title(_title)
{
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

menu_rgb_ui::~menu_rgb_ui()
{
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void menu_rgb_ui::handle()
{
	bool changed = false;

	// process the menu
	const event *menu_event;

	if (!m_key_active)
		menu_event = process(PROCESS_LR_REPEAT);
	else
		menu_event = process(PROCESS_ONLYCHAR);

	if (menu_event != nullptr && menu_event->itemref != nullptr)
	{
		switch ((uintptr_t)menu_event->itemref)
		{
			case RGB_ALPHA:
				if (menu_event->iptkey == IPT_UI_LEFT && m_color->a() > 1)
				{
					m_color->set_a(m_color->a() - 1);
					changed = true;
				}

				else if (menu_event->iptkey == IPT_UI_RIGHT && m_color->a() < 255)
				{
					m_color->set_a(m_color->a() + 1);
					changed = true;
				}

				else if (menu_event->iptkey == IPT_UI_SELECT || menu_event->iptkey == IPT_SPECIAL)
				{
					inkey_special(menu_event);
					changed = true;
				}

				break;

			case RGB_RED:
				if (menu_event->iptkey == IPT_UI_LEFT && m_color->r() > 1)
				{
					m_color->set_r(m_color->r() - 1);
					changed = true;
				}

				else if (menu_event->iptkey == IPT_UI_RIGHT && m_color->r() < 255)
				{
					m_color->set_r(m_color->r() + 1);
					changed = true;
				}

				else if (menu_event->iptkey == IPT_UI_SELECT || menu_event->iptkey == IPT_SPECIAL)
				{
					inkey_special(menu_event);
					changed = true;
				}

				break;

			case RGB_GREEN:
				if (menu_event->iptkey == IPT_UI_LEFT && m_color->g() > 1)
				{
					m_color->set_g(m_color->g() - 1);
					changed = true;
				}

				else if (menu_event->iptkey == IPT_UI_RIGHT && m_color->g() < 255)
				{
					m_color->set_g(m_color->g() + 1);
					changed = true;
				}

				else if (menu_event->iptkey == IPT_UI_SELECT || menu_event->iptkey == IPT_SPECIAL)
				{
					inkey_special(menu_event);
					changed = true;
				}

				break;

			case RGB_BLUE:
				if (menu_event->iptkey == IPT_UI_LEFT && m_color->b() > 1)
				{
					m_color->set_b(m_color->b() - 1);
					changed = true;
				}

				else if (menu_event->iptkey == IPT_UI_RIGHT && m_color->b() < 255)
				{
					m_color->set_b(m_color->b() + 1);
					changed = true;
				}

				else if (menu_event->iptkey == IPT_UI_SELECT || menu_event->iptkey == IPT_SPECIAL)
				{
					inkey_special(menu_event);
					changed = true;
				}

				break;

			case PALETTE_CHOOSE:
				if (menu_event->iptkey == IPT_UI_SELECT)
					menu::stack_push<menu_palette_sel>(ui(), container(), *m_color);
				break;
		}
	}

	if (changed)
		reset(reset_options::REMEMBER_REF);
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void menu_rgb_ui::populate(float &customtop, float &custombottom)
{
	// set filter arrow
	uint32_t arrow_flags = FLAG_LEFT_ARROW | FLAG_RIGHT_ARROW;
	std::string s_text = std::string(m_search).append("_");
	item_append(_("ARGB Settings"), "", FLAG_DISABLE | FLAG_UI_HEADING, nullptr);

	if (m_lock_ref != RGB_ALPHA)
	{
		arrow_flags = get_arrow_flags<uint8_t>(0, 255, m_color->a());
		item_append(_("Alpha"), string_format("%3u", m_color->a()), arrow_flags, (void *)(uintptr_t)RGB_ALPHA);
	}
	else
		item_append(_("Alpha"), s_text, 0, (void *)(uintptr_t)RGB_ALPHA);

	if (m_lock_ref != RGB_RED)
	{
		arrow_flags = get_arrow_flags<uint8_t>(0, 255, m_color->r());
		item_append(_("Red"), string_format("%3u", m_color->r()), arrow_flags, (void *)(uintptr_t)RGB_RED);
	}
	else
		item_append(_("Red"), s_text, 0, (void *)(uintptr_t)RGB_RED);

	if (m_lock_ref != RGB_GREEN)
	{
		arrow_flags = get_arrow_flags<uint8_t>(0, 255, m_color->g());
		item_append(_("Green"), string_format("%3u", m_color->g()), arrow_flags, (void *)(uintptr_t)RGB_GREEN);
	}
	else
		item_append(_("Green"), s_text, 0, (void *)(uintptr_t)RGB_GREEN);

	if (m_lock_ref != RGB_BLUE)
	{
		arrow_flags = get_arrow_flags<uint8_t>(0, 255, m_color->b());
		item_append(_("Blue"), string_format("%3u", m_color->b()), arrow_flags, (void *)(uintptr_t)RGB_BLUE);
	}
	else
		item_append(_("Blue"), s_text, 0, (void *)(uintptr_t)RGB_BLUE);

	item_append(menu_item_type::SEPARATOR);
	item_append(_("Choose from palette"), "", 0, (void *)(uintptr_t)PALETTE_CHOOSE);
	item_append(menu_item_type::SEPARATOR);

	custombottom = customtop = ui().get_line_height() + 3.0f * ui().box_tb_border();
}

//-------------------------------------------------
//  perform our special rendering
//-------------------------------------------------

void menu_rgb_ui::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	float width, maxwidth = origx2 - origx1;

	// top text
	ui().draw_text_full(container(), m_title.c_str(), 0.0f, 0.0f, 1.0f, ui::text_layout::CENTER, ui::text_layout::NEVER,
						mame_ui_manager::NONE, rgb_t::white(), rgb_t::black(), &width);
	width += 2 * ui().box_lr_border();
	maxwidth = std::max(maxwidth, width);

	// compute our bounds
	float x1 = 0.5f - 0.5f * maxwidth;
	float x2 = x1 + maxwidth;
	float y1 = origy1 - top;
	float y2 = origy1 - ui().box_tb_border();

	// draw a box
	ui().draw_outlined_box(container(), x1, y1, x2, y2, UI_GREEN_COLOR);

	// take off the borders
	x1 += ui().box_lr_border();
	x2 -= ui().box_lr_border();
	y1 += ui().box_tb_border();

	// draw the text within it
	ui().draw_text_full(container(), m_title.c_str(), x1, y1, x2 - x1, ui::text_layout::CENTER, ui::text_layout::NEVER,
						mame_ui_manager::NORMAL, ui().colors().text_color(), ui().colors().text_bg_color());

	std::string sampletxt(_("Color preview ="));
	ui().draw_text_full(container(), sampletxt.c_str(), 0.0f, 0.0f, 1.0f, ui::text_layout::CENTER, ui::text_layout::NEVER,
						mame_ui_manager::NONE, rgb_t::white(), rgb_t::black(), &width);
	width += 2 * ui().box_lr_border();
	maxwidth = std::max(origx2 - origx1, width);

	// compute our bounds
	x1 = 0.5f - 0.5f * maxwidth;
	x2 = x1 + maxwidth;
	y1 = origy2 + ui().box_tb_border();
	y2 = origy2 + bottom;

	// draw a box
	ui().draw_outlined_box(container(), x1, y1, x1 + width, y2, UI_RED_COLOR);

	// take off the borders
	x1 += ui().box_lr_border();
	y1 += ui().box_tb_border();

	// draw the normal text
	ui().draw_text_full(container(), sampletxt.c_str(), x1, y1, width - ui().box_lr_border(), ui::text_layout::CENTER, ui::text_layout::NEVER,
						mame_ui_manager::NORMAL, rgb_t::white(), rgb_t::black());

	x1 += width + ui().box_lr_border();
	y1 -= ui().box_tb_border();

	// draw color box
	ui().draw_outlined_box(container(), x1, y1, x2, y2, *m_color);
}

//-------------------------------------------------
//  handle special key event
//-------------------------------------------------

void menu_rgb_ui::inkey_special(const event *menu_event)
{
	if (menu_event->iptkey == IPT_UI_SELECT)
	{
		m_key_active = !m_key_active;
		m_lock_ref = (uintptr_t)menu_event->itemref;

		if (!m_key_active)
		{
			int val = atoi(m_search.data());
			val = m_color->clamp(val);

			switch ((uintptr_t)menu_event->itemref)
			{
			case RGB_ALPHA:
				m_color->set_a(val);
				break;

			case RGB_RED:
				m_color->set_r(val);
				break;

			case RGB_GREEN:
				m_color->set_g(val);
				break;

			case RGB_BLUE:
				m_color->set_b(val);
				break;
			}

			m_search.erase();
			m_lock_ref = 0;
			return;
		}
	}

	if (!m_key_active)
	{
		m_search.erase();
		return;
	}

	input_character(m_search, 3, menu_event->unichar, uchar_is_digit);
}

std::pair<const char *, const char *> const menu_palette_sel::s_palette[] = {
	{ __("White"),  "FFFFFFFF" },
	{ __("Silver"), "FFC0C0C0" },
	{ __("Gray"),   "FF808080" },
	{ __("Black"),  "FF000000" },
	{ __("Red"),    "FFFF0000" },
	{ __("Orange"), "FFFFA500" },
	{ __("Yellow"), "FFFFFF00" },
	{ __("Green"),  "FF00FF00" },
	{ __("Blue"),   "FF0000FF" },
	{ __("Violet"), "FF8F00FF" }
};

//-------------------------------------------------
//  ctor
//-------------------------------------------------

menu_palette_sel::menu_palette_sel(mame_ui_manager &mui, render_container &container, rgb_t &_color)
	: menu(mui, container), m_original(_color)
{
}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

menu_palette_sel::~menu_palette_sel()
{
}

//-------------------------------------------------
//  handle
//-------------------------------------------------

void menu_palette_sel::handle()
{
	// process the menu
	const event *menu_event = process(0);
	if (menu_event != nullptr && menu_event->itemref != nullptr)
	{
		if (menu_event->iptkey == IPT_UI_SELECT)
		{
			m_original = rgb_t(uint32_t(strtoul(selected_item().subtext.c_str(), nullptr, 16)));
			reset_parent(reset_options::SELECT_FIRST);
			stack_pop();
		}
	}
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void menu_palette_sel::populate(float &customtop, float &custombottom)
{
	for (unsigned x = 0; x < ARRAY_LENGTH(s_palette); ++x)
		item_append(_(s_palette[x].first), s_palette[x].second, FLAG_COLOR_BOX, (void *)(uintptr_t)(x + 1));

	item_append(menu_item_type::SEPARATOR);
}

} // namespace ui
