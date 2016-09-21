//-------------------------------------------------
//  force the game select menu to be visible
//  and inescapable
//-------------------------------------------------

#include "gui.h"
#include "ui/miscmenu.h"

namespace ui {
void main_form::force_game_select(mame_ui_manager &mui, render_container &container)
{
	// reset the menu stack
	menu::stack_reset(mui.machine());

	// add the quit entry followed by the game select entry
	menu::stack_push_special_main<ui::menu_quit_game>(mui, container);
	menu::stack_push<main_form>(mui, container, nullptr);

	// force the menus on
	mui.show_menu();

	// make sure MAME is paused
	mui.machine().pause();
}

//-------------------------------------------------
//  ctor
//-------------------------------------------------

main_form::main_form(mame_ui_manager &mui, render_container &container, const char *gamename)
	: menu(mui, container)
	, m_mui(mui)
	, m_container(container)
{}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

main_form::~main_form()
{}

void main_form::populate()
{
	item_append(ui::menu_item_type::SEPARATOR);
}

void main_form::handle()
{
	process(0);
}

} // namespace ui
