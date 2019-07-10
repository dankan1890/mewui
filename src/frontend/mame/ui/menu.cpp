// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, Aaron Giles, Nathan Woods, Maurizio Petrarota
/*********************************************************************

    ui/menu.cpp

    Internal MAME menus for the user interface.

*********************************************************************/

#include "emu.h"

#include "ui/menu.h"

#include "ui/ui.h"
#include "ui/mainmenu.h"
#include "ui/utils.h"
#include "ui/miscmenu.h"

#include "cheat.h"
#include "mame.h"

#include "drivenum.h"
#include "rendutil.h"
#include "uiinput.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <utility>


namespace ui {

/***************************************************************************
    CONSTANTS
***************************************************************************/

#define UI_MENU_POOL_SIZE  65536

/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

std::mutex menu::s_global_state_guard;
menu::global_state_map menu::s_global_states;

/***************************************************************************
    INLINE FUNCTIONS
***************************************************************************/

menu::global_state_ptr menu::get_global_state(running_machine &machine)
{
	std::lock_guard<std::mutex> guard(s_global_state_guard);
	auto const it(s_global_states.find(&machine));
	return (it != s_global_states.end()) ? it->second : global_state_ptr();
}

//-------------------------------------------------
//  exclusive_input_pressed - return true if the
//  given key is pressed and we haven't already
//  reported a key
//-------------------------------------------------

bool menu::exclusive_input_pressed(int &iptkey, int key, int repeat)
{
	if ((iptkey == IPT_INVALID) && machine().ui_input().pressed_repeat(key, repeat))
	{
		iptkey = key;
		return true;
	}
	else
	{
		return false;
	}
}



/***************************************************************************
    CORE SYSTEM MANAGEMENT
***************************************************************************/

menu::global_state::global_state(running_machine &machine, ui_options const &options)
	: widgets_manager(machine)
	, m_machine(machine)
	, m_cleanup_callbacks()
	, m_bgrnd_bitmap()
	, m_bgrnd_texture(nullptr, machine.render())
	, m_stack()
	, m_free()
{
	render_manager &render(machine.render());

	// create a texture for main menu background
	m_bgrnd_texture.reset(render.texture_alloc(render_texture::hq_scale));
	if (options.use_background_image() && (&machine.system() == &GAME_NAME(___empty)))
	{
		m_bgrnd_bitmap = std::make_unique<bitmap_argb32>(0, 0);
		emu_file backgroundfile(".", OPEN_FLAG_READ);
		render_load_jpeg(*m_bgrnd_bitmap, backgroundfile, nullptr, "background.jpg");

		if (!m_bgrnd_bitmap->valid())
			render_load_png(*m_bgrnd_bitmap, backgroundfile, nullptr, "background.png");

		if (m_bgrnd_bitmap->valid())
			m_bgrnd_texture->set_bitmap(*m_bgrnd_bitmap, m_bgrnd_bitmap->cliprect(), TEXFORMAT_ARGB32);
		else
			m_bgrnd_bitmap->reset();
	}
}


menu::global_state::~global_state()
{
	// it shouldn't really be possible to get here with active menus because of reference loops
	assert(!m_stack);
	assert(!m_free);

	stack_reset();
	clear_free_list();

	for (auto const &callback : m_cleanup_callbacks)
		callback(m_machine);
}


void menu::global_state::add_cleanup_callback(cleanup_callback &&callback)
{
	m_cleanup_callbacks.emplace_back(std::move(callback));
}


void menu::global_state::stack_push(std::unique_ptr<menu> &&menu)
{
	menu->m_parent = std::move(m_stack);
	m_stack = std::move(menu);
	m_stack->reset(reset_options::SELECT_FIRST);
	m_stack->machine().ui_input().reset();
}


void menu::global_state::stack_pop()
{
	if (m_stack)
	{
		std::unique_ptr<menu> menu(std::move(m_stack));
		m_stack = std::move(menu->m_parent);
		menu->m_parent = std::move(m_free);
		m_free = std::move(menu);
		m_machine.ui_input().reset();
	}
}


void menu::global_state::stack_reset()
{
	while (m_stack)
		stack_pop();
}


void menu::global_state::clear_free_list()
{
	// free stack is in reverse order - unwind it properly
	std::unique_ptr<menu> reversed;
	while (m_free)
	{
		std::unique_ptr<menu> menu(std::move(m_free));
		m_free = std::move(menu->m_parent);
		menu->m_parent = std::move(reversed);
		reversed = std::move(menu);
	}
	while (reversed)
		reversed = std::move(reversed->m_parent);
}


bool menu::global_state::stack_has_special_main_menu() const
{
	for (auto menu = m_stack.get(); menu != nullptr; menu = menu->m_parent.get())
	{
		if (menu->is_special_main_menu())
			return true;
	}
	return false;
}




//-------------------------------------------------
//  init - initialize the menu system
//-------------------------------------------------

void menu::init(running_machine &machine, ui_options &mopt)
{
	// initialize the menu stack
	{
		std::lock_guard<std::mutex> guard(s_global_state_guard);
		auto const ins(s_global_states.emplace(&machine, std::make_shared<global_state>(machine, mopt)));
		assert(ins.second); // calling init twice is bad
		if (ins.second)
			machine.add_notifier(MACHINE_NOTIFY_EXIT, machine_notify_delegate(&menu::exit, &machine)); // add an exit callback to free memory
		else
			ins.first->second->stack_reset();
	}
}


//-------------------------------------------------
//  exit - clean up after ourselves
//-------------------------------------------------

void menu::exit(running_machine &machine)
{
	// free menus
	global_state_ptr const state(get_global_state(machine));
	state->stack_reset();
	state->clear_free_list();

	std::lock_guard<std::mutex> guard(s_global_state_guard);
	s_global_states.erase(&machine);
}



/***************************************************************************
    CORE MENU MANAGEMENT
***************************************************************************/

//-------------------------------------------------
//  menu - menu constructor
//-------------------------------------------------

menu::menu(mame_ui_manager &mui, render_container &container)
	: m_visible_lines(0)
	, m_visible_items(0)
	, m_global_state(get_global_state(mui.machine()))
	, m_special_main_menu(false)
	, m_ui(mui)
	, m_container(container)
	, m_parent()
	, m_event()
	, m_pool(nullptr)
	, m_customtop(0.0f)
	, m_custombottom(0.0f)
	, m_resetpos(0)
	, m_resetref(nullptr)
	, m_mouse_hit(false)
	, m_mouse_button(false)
	, m_mouse_x(-1.0f)
	, m_mouse_y(-1.0f)
{
	assert(m_global_state); // not calling init is bad

	reset(reset_options::SELECT_FIRST);

	top_line = 0;
}


//-------------------------------------------------
//  ~menu - menu destructor
//-------------------------------------------------

menu::~menu()
{
	// free the pools
	while (m_pool)
	{
		pool *const ppool = m_pool;
		m_pool = m_pool->next;
		global_free_array(ppool);
	}
}


//-------------------------------------------------
//  reset - free all items in the menu,
//  and all memory allocated from the memory pool
//-------------------------------------------------

void menu::reset(reset_options options)
{
	// based on the reset option, set the reset info
	m_resetpos = 0;
	m_resetref = nullptr;
	if (options == reset_options::REMEMBER_POSITION)
		m_resetpos = m_selected;
	else if (options == reset_options::REMEMBER_REF)
		m_resetref = get_selection_ref();

	// reset all the pools and the item count back to 0
	for (pool *ppool = m_pool; ppool != nullptr; ppool = ppool->next)
		ppool->top = (uint8_t *)(ppool + 1);
	m_items.clear();
	m_visible_items = 0;
	m_selected = 0;

	// add an item to return
	if (!m_parent)
	{
		item_append(_("Return to Machine"), "", 0, nullptr);
	}
	else if (m_parent->is_special_main_menu())
	{
		if (machine().options().ui() == emu_options::UI_SIMPLE)
			item_append(_("Exit"), "", 0, nullptr);
		else
			item_append(_("Exit"), "", FLAG_LEFT_ARROW | FLAG_RIGHT_ARROW, nullptr);
	}
	else
	{
		if (machine().options().ui() != emu_options::UI_SIMPLE && stack_has_special_main_menu())
			item_append(_("Return to Previous Menu"), "", FLAG_LEFT_ARROW | FLAG_RIGHT_ARROW, nullptr);
		else
			item_append(_("Return to Previous Menu"), "", 0, nullptr);
	}

}


//-------------------------------------------------
//  is_special_main_menu - returns whether the
//  menu has special needs
//-------------------------------------------------

bool menu::is_special_main_menu() const
{
	return m_special_main_menu;
}


//-------------------------------------------------
//  set_special_main_menu - set whether the
//  menu has special needs
//-------------------------------------------------

void menu::set_special_main_menu(bool special)
{
	m_special_main_menu = special;
}


//-------------------------------------------------
//  item_append - append a new item to the
//  end of the menu
//-------------------------------------------------

void menu::item_append(menu_item item)
{
	item_append(item.text, item.subtext, item.flags, item.ref, item.type);
}

//-------------------------------------------------
//  item_append - append a new item to the
//  end of the menu
//-------------------------------------------------

void menu::item_append(menu_item_type type, uint32_t flags)
{
	if (type == menu_item_type::SEPARATOR)
		item_append(MENU_SEPARATOR_ITEM, "", flags, nullptr, menu_item_type::SEPARATOR);
}

//-------------------------------------------------
//  item_append - append a new item to the
//  end of the menu
//-------------------------------------------------

void menu::item_append(const std::string &text, const std::string &subtext, uint32_t flags, void *ref, menu_item_type type)
{
	item_append(std::string(text), std::string(subtext), flags, ref, type);
}

//-------------------------------------------------
//  item_append - append a new item to the
//  end of the menu
//-------------------------------------------------

void menu::item_append(std::string &&text, std::string &&subtext, uint32_t flags, void *ref, menu_item_type type)
{
	// only allow multiline as the first item
	if ((flags & FLAG_MULTILINE) != 0)
		assert(m_items.size() == 1);

	// only allow a single multi-line item
	else if (m_items.size() >= 2)
		assert((m_items[0].flags & FLAG_MULTILINE) == 0);

	// allocate a new item and populate it
	menu_item pitem;
	pitem.text = std::move(text);
	pitem.subtext = std::move(subtext);
	pitem.flags = flags;
	pitem.ref = ref;
	pitem.type = type;

	// append to array
	auto index = m_items.size();
	if (!m_items.empty())
	{
		m_items.emplace(m_items.end() - 1, std::move(pitem));
		--index;
	}
	else
		m_items.emplace_back(std::move(pitem));

	// update the selection if we need to
	if (m_resetpos == index || (m_resetref != nullptr && m_resetref == ref))
		m_selected = index;
	if (m_resetpos == (m_items.size() - 1))
		m_selected = m_items.size() - 1;
}


//-------------------------------------------------
//  item_append_on_off - append a new "On"/"Off"
//  item to the end of the menu
//-------------------------------------------------

void menu::item_append_on_off(const std::string &text, bool state, uint32_t flags, void *ref, menu_item_type type)
{
	if (flags & FLAG_DISABLE)
		ref = nullptr;
	else
		flags |= state ? FLAG_LEFT_ARROW : FLAG_RIGHT_ARROW;

	item_append(std::string(text), state ? _("On") : _("Off"), flags, ref, type);
}


//-------------------------------------------------
//  repopulate - repopulate menu items
//-------------------------------------------------

void menu::repopulate(reset_options options)
{
	reset(options);
	populate(m_customtop, m_custombottom);
}


//-------------------------------------------------
//  process - process a menu, drawing it
//  and returning any interesting events
//-------------------------------------------------

const menu::event *menu::process(uint32_t flags, float x0, float y0)
{
	// reset the event
	m_event.iptkey = IPT_INVALID;

	// first make sure our selection is valid
	validate_selection(1);

	// draw the menu
	if (m_items.size() > 1 && (m_items[0].flags & FLAG_MULTILINE) != 0)
		draw_text_box();
	else
		draw(flags);

	// process input
	if (!(flags & PROCESS_NOKEYS) && !(flags & PROCESS_NOINPUT))
	{
		// read events
		handle_events(flags, m_event);

		// handle the keys if we don't already have an event
		if (m_event.iptkey == IPT_INVALID)
			handle_keys(flags, m_event.iptkey);
	}

	// update the selected item in the event
	if ((m_event.iptkey != IPT_INVALID) && selection_valid())
	{
		m_event.itemref = get_selection_ref();
		m_event.type = m_items[m_selected].type;
		return &m_event;
	}
	else
	{
		return nullptr;
	}
}


//-------------------------------------------------
//  m_pool_alloc - allocate temporary memory
//  from the menu's memory pool
//-------------------------------------------------

void *menu::m_pool_alloc(size_t size)
{
	assert(size < UI_MENU_POOL_SIZE);

	// find a pool with enough room
	for (pool *ppool = m_pool; ppool != nullptr; ppool = ppool->next)
	{
		if (ppool->end - ppool->top >= size)
		{
			void *result = ppool->top;
			ppool->top += size;
			return result;
		}
	}

	// allocate a new pool
	pool *ppool = (pool *)global_alloc_array_clear<uint8_t>(sizeof(*ppool) + UI_MENU_POOL_SIZE);

	// wire it up
	ppool->next = m_pool;
	m_pool = ppool;
	ppool->top = (uint8_t *)(ppool + 1);
	ppool->end = ppool->top + UI_MENU_POOL_SIZE;
	return m_pool_alloc(size);
}


//-------------------------------------------------
//  set_selection - changes the index
//  of the currently selected menu item
//-------------------------------------------------

void menu::set_selection(void *selected_itemref)
{
	m_selected = -1;
	for (int itemnum = 0; itemnum < m_items.size(); itemnum++)
	{
		if (m_items[itemnum].ref == selected_itemref)
		{
			m_selected = itemnum;
			break;
		}
	}
}



/***************************************************************************
    INTERNAL MENU PROCESSING
***************************************************************************/

//-------------------------------------------------
//  draw - draw a menu
//-------------------------------------------------

void menu::draw(uint32_t flags)
{
	// first draw the FPS counter
	if (ui().show_fps_counter())
	{
		ui().draw_text_full(container(), machine().video().speed_text().c_str(), 0.0f, 0.0f, 1.0f,
				ui::text_layout::RIGHT, ui::text_layout::WORD, mame_ui_manager::OPAQUE_, rgb_t::white(), rgb_t::black(), nullptr, nullptr);
	}

	bool const customonly = (flags & PROCESS_CUSTOM_ONLY);
	bool const noimage = (flags & PROCESS_NOIMAGE);
	bool const noinput = (flags & PROCESS_NOINPUT);
	float const line_height = ui().get_line_height();
	float const lr_arrow_width = 0.4f * line_height * machine().render().ui_aspect();
	float const ud_arrow_width = line_height * machine().render().ui_aspect();
	float const gutter_width = lr_arrow_width * 1.3f;

	if (&machine().system() == &GAME_NAME(___empty) && !noimage)
		draw_background();

	// compute the width and height of the full menu
	float visible_width = 0;
	float visible_main_menu_height = 0;
	for (auto const &pitem : m_items)
	{
		// compute width of left hand side
		float total_width = gutter_width + ui().get_string_width(pitem.text.c_str()) + gutter_width;

		// add in width of right hand side
		if (!pitem.subtext.empty())
			total_width += 2.0f * gutter_width + ui().get_string_width(pitem.subtext.c_str());

		// track the maximum
		if (total_width > visible_width)
			visible_width = total_width;

		// track the height as well
		visible_main_menu_height += line_height;
	}

	// account for extra space at the top and bottom
	float const visible_extra_menu_height = m_customtop + m_custombottom;

	// add a little bit of slop for rounding
	visible_width += 0.01f;
	visible_main_menu_height += 0.01f;

	// if we are too wide or too tall, clamp it down
	if (visible_width + 2.0f * ui().box_lr_border() > 1.0f)
		visible_width = 1.0f - 2.0f * ui().box_lr_border();

	// if the menu and extra menu won't fit, take away part of the regular menu, it will scroll
	if (visible_main_menu_height + visible_extra_menu_height + 2.0f * ui().box_tb_border() > 1.0f)
		visible_main_menu_height = 1.0f - 2.0f * ui().box_tb_border() - visible_extra_menu_height;

	m_visible_lines = std::min(int(std::floor(visible_main_menu_height / line_height)), int(unsigned(m_items.size())));
	visible_main_menu_height = float(m_visible_lines) * line_height;

	// compute top/left of inner menu area by centering
	float const visible_left = (1.0f - visible_width) * 0.5f;
	float const visible_top = ((1.0f - visible_main_menu_height - visible_extra_menu_height) * 0.5f) + m_customtop;

	// first add us a box
	float const x1 = visible_left - ui().box_lr_border();
	float const y1 = visible_top - ui().box_tb_border();
	float const x2 = visible_left + visible_width + ui().box_lr_border();
	float const y2 = visible_top + visible_main_menu_height + ui().box_tb_border();
	if (!customonly)
		ui().draw_outlined_box(container(), x1, y1, x2, y2, ui().colors().background_color());

	if (top_line < 0 || is_first_selected())
		top_line = 0;
	if (m_selected >= (top_line + m_visible_lines))
		top_line = m_selected - (m_visible_lines / 2);
	if ((top_line > (m_items.size() - m_visible_lines)) || is_last_selected())
		top_line = m_items.size() - m_visible_lines;

	// if scrolling, show arrows
	bool const show_top_arrow((m_items.size() > m_visible_lines) && !first_item_visible());
	bool const show_bottom_arrow((m_items.size() > m_visible_lines) && !last_item_visible());

	// set the number of visible lines, minus 1 for top arrow and 1 for bottom arrow
	m_visible_items = m_visible_lines - (show_top_arrow ? 1 : 0) - (show_bottom_arrow ? 1 : 0);

	// determine effective positions taking into account the hilighting arrows
	float const effective_width = visible_width - 2.0f * gutter_width;
	float const effective_left = visible_left + gutter_width;

	// locate mouse
	if (!customonly && !noinput)
		map_mouse();
	else
		ignore_mouse();

	// loop over visible lines
	m_hover = m_items.size() + 1;
	bool selected_subitem_too_big = false;
	float const line_x0 = x1 + 0.5f * UI_LINE_WIDTH;
	float const line_x1 = x2 - 0.5f * UI_LINE_WIDTH;
	if (!customonly)
	{
		for (int linenum = 0; linenum < m_visible_lines; linenum++)
		{
			auto const itemnum = top_line + linenum;
			menu_item const &pitem = m_items[itemnum];
			char const *const itemtext = pitem.text.c_str();
			rgb_t fgcolor = ui().colors().text_color();
			rgb_t bgcolor = ui().colors().text_bg_color();
			rgb_t fgcolor2 = ui().colors().subitem_color();
			rgb_t fgcolor3 = ui().colors().clone_color();
			float const line_y0 = visible_top + (float)linenum * line_height;
			float const line_y1 = line_y0 + line_height;

			// set the hover if this is our item
			if (mouse_in_rect(line_x0, line_y0, line_x1, line_y1) && is_selectable(pitem))
				m_hover = itemnum;

			// if we're selected, draw with a different background
			if (is_selected(itemnum))
			{
				fgcolor = fgcolor2 = fgcolor3 = ui().colors().selected_color();
				bgcolor = ui().colors().selected_bg_color();
			}

			// else if the mouse is over this item, draw with a different background
			else if (itemnum == m_hover)
			{
				fgcolor = fgcolor2 = fgcolor3 = ui().colors().mouseover_color();
				bgcolor = ui().colors().mouseover_bg_color();
			}

			// if we have some background hilighting to do, add a quad behind everything else
			if (bgcolor != ui().colors().text_bg_color())
				highlight(line_x0, line_y0, line_x1, line_y1, bgcolor);

			if (linenum == 0 && show_top_arrow)
			{
				// if we're on the top line, display the up arrow
				draw_arrow(
							0.5f * (x1 + x2) - 0.5f * ud_arrow_width,
							line_y0 + 0.25f * line_height,
							0.5f * (x1 + x2) + 0.5f * ud_arrow_width,
							line_y0 + 0.75f * line_height,
							fgcolor,
							ROT0);
				if (m_hover == itemnum)
					m_hover = HOVER_ARROW_UP;
			}
			else if (linenum == m_visible_lines - 1 && show_bottom_arrow)
			{
				// if we're on the bottom line, display the down arrow
				draw_arrow(
							0.5f * (x1 + x2) - 0.5f * ud_arrow_width,
							line_y0 + 0.25f * line_height,
							0.5f * (x1 + x2) + 0.5f * ud_arrow_width,
							line_y0 + 0.75f * line_height,
							fgcolor,
							ROT0 ^ ORIENTATION_FLIP_Y);
				if (m_hover == itemnum)
					m_hover = HOVER_ARROW_DOWN;
			}
			else if (pitem.type == menu_item_type::SEPARATOR)
			{
				// if we're just a divider, draw a line
				container().add_line(visible_left, line_y0 + 0.5f * line_height, visible_left + visible_width, line_y0 + 0.5f * line_height, UI_LINE_WIDTH, ui().colors().border_color(), PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA));
			}
			else if (pitem.subtext.empty())
			{
				// if we don't have a subitem, just draw the string centered
				if (pitem.flags & FLAG_UI_HEADING)
				{
					float heading_width = ui().get_string_width(itemtext);
					container().add_line(visible_left, line_y0 + 0.5f * line_height, visible_left + ((visible_width - heading_width) / 2) - ui().box_lr_border(), line_y0 + 0.5f * line_height, UI_LINE_WIDTH, ui().colors().border_color(), PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA));
					container().add_line(visible_left + visible_width - ((visible_width - heading_width) / 2) + ui().box_lr_border(), line_y0 + 0.5f * line_height, visible_left + visible_width, line_y0 + 0.5f * line_height, UI_LINE_WIDTH, ui().colors().border_color(), PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA));
				}
				ui().draw_text_full(container(), itemtext, effective_left, line_y0, effective_width,
					ui::text_layout::CENTER, ui::text_layout::TRUNCATE, mame_ui_manager::NORMAL, fgcolor, bgcolor, nullptr, nullptr);
			}
			else
			{
				// otherwise, draw the item on the left and the subitem text on the right
				bool const subitem_invert(pitem.flags & FLAG_INVERT);
				char const *subitem_text(pitem.subtext.c_str());
				float item_width, subitem_width;

				// draw the left-side text
				ui().draw_text_full(container(), itemtext, effective_left, line_y0, effective_width,
					ui::text_layout::LEFT, ui::text_layout::TRUNCATE, mame_ui_manager::NORMAL, fgcolor, bgcolor, &item_width, nullptr);

				if (pitem.flags & FLAG_COLOR_BOX)
				{
					rgb_t color = rgb_t((uint32_t)strtoul(subitem_text, nullptr, 16));

					// give 2 spaces worth of padding
					subitem_width = ui().get_string_width("FF00FF00");

					ui().draw_outlined_box(container(), effective_left + effective_width - subitem_width, line_y0,
						effective_left + effective_width, line_y1, color);
				}
				else
				{
					// give 2 spaces worth of padding
					item_width += 2.0f * gutter_width;

					// if the subitem doesn't fit here, display dots
					if (ui().get_string_width(subitem_text) > effective_width - item_width)
					{
						subitem_text = "...";
						if (is_selected(itemnum))
							selected_subitem_too_big = true;
					}

					// customize subitem text color
					if (!core_stricmp(subitem_text, _("On")))
						fgcolor2 = rgb_t(0x00,0xff,0x00);

					if (!core_stricmp(subitem_text, _("Off")))
						fgcolor2 = rgb_t(0xff,0x00,0x00);

					if (!core_stricmp(subitem_text, _("Auto")))
						fgcolor2 = rgb_t(0xff,0xff,0x00);

					// draw the subitem right-justified
					ui().draw_text_full(container(), subitem_text, effective_left + item_width, line_y0, effective_width - item_width,
								ui::text_layout::RIGHT, ui::text_layout::TRUNCATE, mame_ui_manager::NORMAL, subitem_invert ? fgcolor3 : fgcolor2, bgcolor, &subitem_width, nullptr);
				}

				// apply arrows
				if (is_selected(itemnum) && (pitem.flags & FLAG_LEFT_ARROW))
				{
					draw_arrow(
								effective_left + effective_width - subitem_width - gutter_width,
								line_y0 + 0.1f * line_height,
								effective_left + effective_width - subitem_width - gutter_width + lr_arrow_width,
								line_y0 + 0.9f * line_height,
								fgcolor,
								ROT90 ^ ORIENTATION_FLIP_X);
				}
				if (is_selected(itemnum) && (pitem.flags & FLAG_RIGHT_ARROW))
				{
					draw_arrow(
								effective_left + effective_width + gutter_width - lr_arrow_width,
								line_y0 + 0.1f * line_height,
								effective_left + effective_width + gutter_width,
								line_y0 + 0.9f * line_height,
								fgcolor,
								ROT90);
				}
			}
		}
	}

	// if the selected subitem is too big, display it in a separate offset box
	if (selected_subitem_too_big)
	{
		menu_item const &pitem = selected_item();
		bool const subitem_invert(pitem.flags & FLAG_INVERT);
		auto const linenum = m_selected - top_line;
		float const line_y = visible_top + (float)linenum * line_height;
		float target_width, target_height;

		// compute the multi-line target width/height
		ui().draw_text_full(container(), pitem.subtext.c_str(), 0, 0, visible_width * 0.75f,
			ui::text_layout::RIGHT, ui::text_layout::WORD, mame_ui_manager::NONE, rgb_t::white(), rgb_t::black(), &target_width, &target_height);

		// determine the target location
		float const target_x = visible_left + visible_width - target_width - ui().box_lr_border();
		float target_y = line_y + line_height + ui().box_tb_border();
		if (target_y + target_height + ui().box_tb_border() > visible_main_menu_height)
			target_y = line_y - target_height - ui().box_tb_border();

		// add a box around that
		ui().draw_outlined_box(container(), target_x - ui().box_lr_border(),
				target_y - ui().box_tb_border(),
				target_x + target_width + ui().box_lr_border(),
				target_y + target_height + ui().box_tb_border(),
				subitem_invert ? ui().colors().selected_bg_color() : ui().colors().background_color());

		ui().draw_text_full(container(), pitem.subtext.c_str(), target_x, target_y, target_width,
				ui::text_layout::RIGHT, ui::text_layout::WORD, mame_ui_manager::NORMAL, ui().colors().selected_color(), ui().colors().selected_bg_color(), nullptr, nullptr);
	}

	// if there is something special to add, do it by calling the virtual method
	custom_render(get_selection_ref(), m_customtop, m_custombottom, x1, y1, x2, y2);
}

void menu::custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2)
{
}

//-------------------------------------------------
//  draw_text_box - draw a multiline
//  word-wrapped text box with a menu item at the
//  bottom
//-------------------------------------------------

void menu::draw_text_box()
{
	const char *text = m_items[0].text.c_str();
	const char *backtext = m_items[1].text.c_str();
	float line_height = ui().get_line_height();
	float lr_arrow_width = 0.4f * line_height * machine().render().ui_aspect();
	float gutter_width = lr_arrow_width;
	float target_width, target_height, prior_width;
	float target_x, target_y;

	// compute the multi-line target width/height
	ui().draw_text_full(container(), text, 0, 0, 1.0f - 2.0f * ui().box_lr_border() - 2.0f * gutter_width,
		ui::text_layout::LEFT, ui::text_layout::WORD, mame_ui_manager::NONE, rgb_t::white(), rgb_t::black(), &target_width, &target_height);
	target_height += 2.0f * line_height;
	if (target_height > 1.0f - 2.0f * ui().box_tb_border())
		target_height = floorf((1.0f - 2.0f * ui().box_tb_border()) / line_height) * line_height;

	// maximum against "return to prior menu" text
	prior_width = ui().get_string_width(backtext) + 2.0f * gutter_width;
	target_width = std::max(target_width, prior_width);

	// determine the target location
	target_x = 0.5f - 0.5f * target_width;
	target_y = 0.5f - 0.5f * target_height;

	// make sure we stay on-screen
	if (target_x < ui().box_lr_border() + gutter_width)
		target_x = ui().box_lr_border() + gutter_width;
	if (target_x + target_width + gutter_width + ui().box_lr_border() > 1.0f)
		target_x = 1.0f - ui().box_lr_border() - gutter_width - target_width;
	if (target_y < ui().box_tb_border())
		target_y = ui().box_tb_border();
	if (target_y + target_height + ui().box_tb_border() > 1.0f)
		target_y = 1.0f - ui().box_tb_border() - target_height;

	// add a box around that
	ui().draw_outlined_box(container(), target_x - ui().box_lr_border() - gutter_width,
							target_y - ui().box_tb_border(),
							target_x + target_width + gutter_width + ui().box_lr_border(),
							target_y + target_height + ui().box_tb_border(),
							(m_items[0].flags & FLAG_REDTEXT) ?  UI_RED_COLOR : ui().colors().background_color());
	ui().draw_text_full(container(), text, target_x, target_y, target_width,
			ui::text_layout::LEFT, ui::text_layout::WORD, mame_ui_manager::NORMAL, ui().colors().text_color(), ui().colors().text_bg_color(), nullptr, nullptr);

	// draw the "return to prior menu" text with a hilight behind it
	highlight(
				target_x + 0.5f * UI_LINE_WIDTH,
				target_y + target_height - line_height,
				target_x + target_width - 0.5f * UI_LINE_WIDTH,
				target_y + target_height,
				ui().colors().selected_bg_color());
	ui().draw_text_full(container(), backtext, target_x, target_y + target_height - line_height, target_width,
		ui::text_layout::CENTER, ui::text_layout::TRUNCATE, mame_ui_manager::NORMAL, ui().colors().selected_color(), ui().colors().selected_bg_color(), nullptr, nullptr);

	// artificially set the hover to the last item so a double-click exits
	m_hover = m_items.size() - 1;
}


//-------------------------------------------------
//  map_mouse - map mouse pointer location to menu
//  coordinates
//-------------------------------------------------

void menu::map_mouse()
{
	ignore_mouse();
	int32_t mouse_target_x, mouse_target_y;
	render_target *const mouse_target = machine().ui_input().find_mouse(&mouse_target_x, &mouse_target_y, &m_mouse_button);
	if (mouse_target)
	{
		if (mouse_target->map_point_container(mouse_target_x, mouse_target_y, container(), m_mouse_x, m_mouse_y))
			m_mouse_hit = true;
	}
}


//-------------------------------------------------
//  ignore_mouse - set members to ignore mouse
//  input
//-------------------------------------------------

void menu::ignore_mouse()
{
	m_mouse_hit = false;
	m_mouse_button = false;
	m_mouse_x = -1.0f;
	m_mouse_y = -1.0f;
}


//-------------------------------------------------
//  handle_events - generically handle
//  input events for a menu
//-------------------------------------------------

void menu::handle_events(uint32_t flags, event &ev)
{
	bool stop = false;
	ui_event local_menu_event;

	// loop while we have interesting events
	while (!stop && machine().ui_input().pop_event(&local_menu_event))
	{
		switch (local_menu_event.event_type)
		{
			// if we are hovering over a valid item, select it with a single click
			case ui_event::MOUSE_DOWN:
				if (custom_mouse_down())
					return;

				if ((flags & PROCESS_ONLYCHAR) == 0)
				{
					if (m_hover >= 0 && m_hover < m_items.size())
						m_selected = m_hover;
					else if (m_hover == HOVER_ARROW_UP)
					{
						if ((flags & FLAG_UI_DATS) != 0)
						{
							top_line -= m_visible_items - (last_item_visible() ? 1 : 0);
							return;
						}
						m_selected -= m_visible_items;
						if (m_selected < 0)
							m_selected = 0;
						top_line -= m_visible_items - (last_item_visible() ? 1 : 0);
					}
					else if (m_hover == HOVER_ARROW_DOWN)
					{
						if ((flags & FLAG_UI_DATS) != 0)
						{
							top_line += m_visible_lines - 2;
							return;
						}
						m_selected += m_visible_lines - 2 + is_first_selected();
						if (m_selected > m_items.size() - 1)
							m_selected = m_items.size() - 1;
						top_line += m_visible_lines - 2;
					}
				}
				break;

			// if we are hovering over a valid item, fake a UI_SELECT with a double-click
			case ui_event::MOUSE_DOUBLE_CLICK:
				if (!(flags & PROCESS_ONLYCHAR) && m_hover >= 0 && m_hover < m_items.size())
				{
					m_selected = m_hover;
					ev.iptkey = IPT_UI_SELECT;
					if (is_last_selected())
					{
						ev.iptkey = IPT_UI_CANCEL;
						stack_pop();
					}
					stop = true;
				}
				break;

			// caught scroll event
			case ui_event::MOUSE_WHEEL:
				if (!(flags & PROCESS_ONLYCHAR))
				{
					if (local_menu_event.zdelta > 0)
					{
						if ((flags & FLAG_UI_DATS) != 0)
						{
							top_line -= local_menu_event.num_lines;
							return;
						}
						if (is_first_selected())
							select_last_item();
						else
						{
							m_selected -= local_menu_event.num_lines;
							validate_selection(-1);
						}
						top_line -= (m_selected <= top_line && top_line != 0);
						if (m_selected <= top_line && m_visible_items != m_visible_lines)
							top_line -= local_menu_event.num_lines;
					}
					else
					{
						if ((flags & FLAG_UI_DATS))
						{
							top_line += local_menu_event.num_lines;
							return;
						}
						if (is_last_selected())
							select_first_item();
						else
						{
							m_selected += local_menu_event.num_lines;
							validate_selection(1);
						}
						top_line += (m_selected >= top_line + m_visible_items + (top_line != 0));
						if (m_selected >= (top_line + m_visible_items + (top_line != 0)))
							top_line += local_menu_event.num_lines;
					}
				}
				break;

			// translate CHAR events into specials
			case ui_event::IME_CHAR:
				ev.iptkey = IPT_SPECIAL;
				ev.unichar = local_menu_event.ch;
				stop = true;
				break;

			// ignore everything else
			default:
				break;
		}
	}
}


//-------------------------------------------------
//  handle_keys - generically handle
//  keys for a menu
//-------------------------------------------------

void menu::handle_keys(uint32_t flags, int &iptkey)
{
	bool ignorepause = stack_has_special_main_menu();
	int code;

	// bail if no items
	if (m_items.empty())
		return;

	// if we hit select, return true or pop the stack, depending on the item
	if (exclusive_input_pressed(iptkey, IPT_UI_SELECT, 0))
	{
		if (is_last_selected())
		{
			iptkey = IPT_UI_CANCEL;
			stack_pop();
		}
		return;
	}

	// bail out
	if ((flags & PROCESS_ONLYCHAR))
		return;

	// hitting cancel also pops the stack
	if (exclusive_input_pressed(iptkey, IPT_UI_CANCEL, 0))
	{
		if (!menu_has_search_active())
			stack_pop();
		return;
	}

	// validate the current selection
	validate_selection(1);

	// swallow left/right keys if they are not appropriate
	bool ignoreleft = ((selected_item().flags & FLAG_LEFT_ARROW) == 0);
	bool ignoreright = ((selected_item().flags & FLAG_RIGHT_ARROW) == 0);

	if ((m_items[0].flags & FLAG_UI_DATS))
		ignoreleft = ignoreright = false;

	// accept left/right keys as-is with repeat
	if (!ignoreleft && exclusive_input_pressed(iptkey, IPT_UI_LEFT, (flags & PROCESS_LR_REPEAT) ? 6 : 0))
		return;
	if (!ignoreright && exclusive_input_pressed(iptkey, IPT_UI_RIGHT, (flags & PROCESS_LR_REPEAT) ? 6 : 0))
		return;

	// up backs up by one item
	if (exclusive_input_pressed(iptkey, IPT_UI_UP, 6))
	{
		if ((m_items[0].flags & FLAG_UI_DATS))
		{
			top_line--;
			return;
		}
		if (is_first_selected())
			select_last_item();
		else
		{
			--m_selected;
			validate_selection(-1);
		}
		top_line -= (m_selected <= top_line && top_line != 0);
		if (m_selected <= top_line && m_visible_items != m_visible_lines)
			top_line--;
	}

	// down advances by one item
	if (exclusive_input_pressed(iptkey, IPT_UI_DOWN, 6))
	{
		if ((m_items[0].flags & FLAG_UI_DATS))
		{
			top_line++;
			return;
		}
		if (is_last_selected())
			select_first_item();
		else
		{
			++m_selected;
			validate_selection(1);
		}
		top_line += (m_selected >= top_line + m_visible_items + (top_line != 0));
		if (m_selected >= (top_line + m_visible_items + (top_line != 0)))
			top_line++;
	}

	// page up backs up by m_visible_items
	if (exclusive_input_pressed(iptkey, IPT_UI_PAGE_UP, 6))
	{
		m_selected -= m_visible_items;
		top_line -= m_visible_items - (last_item_visible() ? 1 : 0);
		if (m_selected < 0)
			m_selected = 0;
		validate_selection(1);
	}

	// page down advances by m_visible_items
	if (exclusive_input_pressed(iptkey, IPT_UI_PAGE_DOWN, 6))
	{
		m_selected += m_visible_lines - 2 + is_first_selected();
		top_line += m_visible_lines - 2;

		if (m_selected > m_items.size() - 1)
			m_selected = m_items.size() - 1;
		validate_selection(-1);
	}

	// home goes to the start
	if (exclusive_input_pressed(iptkey, IPT_UI_HOME, 0))
		select_first_item();

	// end goes to the last
	if (exclusive_input_pressed(iptkey, IPT_UI_END, 0))
		select_last_item();

	// pause enables/disables pause
	if (!ignorepause && exclusive_input_pressed(iptkey, IPT_UI_PAUSE, 0))
	{
		if (machine().paused())
			machine().resume();
		else
			machine().pause();
	}

	// handle a toggle cheats request
	if (machine().ui_input().pressed_repeat(IPT_UI_TOGGLE_CHEAT, 0))
		mame_machine_manager::instance()->cheat().set_enable(!mame_machine_manager::instance()->cheat().enabled());

	// see if any other UI keys are pressed
	if (iptkey == IPT_INVALID)
	{
		for (code = IPT_UI_FIRST + 1; code < IPT_UI_LAST; code++)
		{
			if (code == IPT_UI_CONFIGURE || (code == IPT_UI_LEFT && ignoreleft) || (code == IPT_UI_RIGHT && ignoreright) || (code == IPT_UI_PAUSE && ignorepause))
				continue;
			if (exclusive_input_pressed(iptkey, code, 0))
				break;
		}
	}
}


//-------------------------------------------------
//  select_first_item - select the first item in
//  the menu
//-------------------------------------------------

void menu::select_first_item()
{
	m_selected = top_line = 0;
	validate_selection(1);
}


//-------------------------------------------------
//  select_last_item - select the last item in the
//  menu
//-------------------------------------------------

void menu::select_last_item()
{
	m_selected = top_line = m_items.size() - 1;
	validate_selection(-1);
}


//-------------------------------------------------
//  validate_selection - validate the
//  current selection and ensure it is on a
//  correct item
//-------------------------------------------------

void menu::validate_selection(int scandir)
{
	// clamp to be in range
	if (m_selected < 0)
		m_selected = 0;
	else if (m_selected >= m_items.size())
		m_selected = m_items.size() - 1;

	// skip past unselectable items
	while (!is_selectable(m_items[m_selected]))
		m_selected = (m_selected + m_items.size() + scandir) % m_items.size();
}



/***************************************************************************
    MENU STACK MANAGEMENT
***************************************************************************/

void menu::do_handle()
{
	if (m_items.size() < 2)
		populate(m_customtop, m_custombottom);
	handle();
}


/***************************************************************************
    UI SYSTEM INTERACTION
***************************************************************************/

//-------------------------------------------------
//  ui_menu_ui_handler - displays the current menu
//  and calls the menu handler
//-------------------------------------------------

uint32_t menu::ui_handler(render_container &container, mame_ui_manager &mui)
{
	global_state_ptr const state(get_global_state(mui.machine()));

	// if we have no menus stacked up, start with the main menu
	if (!state->topmost_menu<menu>())
		state->stack_push(std::unique_ptr<menu>(global_alloc_clear<menu_main>(mui, container)));

	// update the menu state
	if (state->topmost_menu<menu>())
		state->topmost_menu<menu>()->do_handle();

	// clear up anything pending to be released
	state->clear_free_list();

	// if the menus are to be hidden, return a cancel here
	if (mui.is_menu_active() && ((mui.machine().ui_input().pressed(IPT_UI_CONFIGURE) && !state->stack_has_special_main_menu()) || !state->topmost_menu<menu>()))
		return UI_HANDLER_CANCEL;

	return 0;
}

/***************************************************************************
    MENU HELPERS
***************************************************************************/

//-------------------------------------------------
//  highlight
//-------------------------------------------------

void menu::highlight(float x0, float y0, float x1, float y1, rgb_t bgcolor)
{
	container().add_quad(x0, y0, x1, y1, bgcolor, m_global_state->hilight_texture(), PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA) | PRIMFLAG_TEXWRAP(1) | PRIMFLAG_PACKABLE);
}


//-------------------------------------------------
//  draw_arrow
//-------------------------------------------------

void menu::draw_arrow(float x0, float y0, float x1, float y1, rgb_t fgcolor, uint32_t orientation)
{
	container().add_quad(x0, y0, x1, y1, fgcolor, m_global_state->arrow_texture(), PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA) | PRIMFLAG_TEXORIENT(orientation) | PRIMFLAG_PACKABLE);
}


//-------------------------------------------------
//  extra_text_draw_box - generically adds header
//  or footer text
//-------------------------------------------------

void menu::extra_text_draw_box(float origx1, float origx2, float origy, float yspan, const char *text, int direction)
{
	// get the size of the text
	auto layout = ui().create_layout(container());
	layout.add_text(text);

	// position this extra text
	float x1, y1, x2, y2;
	extra_text_position(origx1, origx2, origy, yspan, layout, direction, x1, y1, x2, y2);

	// draw a box
	ui().draw_outlined_box(container(), x1, y1, x2, y2, ui().colors().background_color());

	// take off the borders
	x1 += ui().box_lr_border();
	y1 += ui().box_tb_border();

	// draw the text within it
	layout.emit(container(), x1, y1);
}


void menu::draw_background()
{
	// draw background image if available
	if (ui().options().use_background_image() && m_global_state->bgrnd_bitmap() && m_global_state->bgrnd_bitmap()->valid())
		container().add_quad(0.0f, 0.0f, 1.0f, 1.0f, rgb_t::white(), m_global_state->bgrnd_texture(), PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA));
}


//-------------------------------------------------
//  extra_text_position - given extra text that has
//  been put into a layout, position it
//-------------------------------------------------

void menu::extra_text_position(float origx1, float origx2, float origy, float yspan, text_layout &layout,
	int direction, float &x1, float &y1, float &x2, float &y2)
{
	float width = layout.actual_width() + (2 * ui().box_lr_border());
	float maxwidth = std::max(width, origx2 - origx1);

	// compute our bounds
	x1 = 0.5f - 0.5f * maxwidth;
	x2 = x1 + maxwidth;
	y1 = origy + (yspan * direction);
	y2 = origy + (ui().box_tb_border() * direction);

	if (y1 > y2)
		std::swap(y1, y2);
}


//-------------------------------------------------
//  extra_text_render - generically adds header
//  and footer text
//-------------------------------------------------

void menu::extra_text_render(float top, float bottom, float origx1, float origy1, float origx2, float origy2, const char *header, const char *footer)
{
	header = (header && *header) ? header : nullptr;
	footer = (footer && *footer) ? footer : nullptr;

	if (header != nullptr)
		extra_text_draw_box(origx1, origx2, origy1, top, header, -1);
	if (footer != nullptr)
		extra_text_draw_box(origx1, origx2, origy2, bottom, footer, +1);
}

} // namespace ui
