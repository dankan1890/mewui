//-------------------------------------------------
//  force the game select menu to be visible
//  and inescapable
//-------------------------------------------------

#include "gui.h"
#include "ui/miscmenu.h"
#include "textbox.h"
#include "label.h"

namespace ui {
void main_form::force_game_select(mame_ui_manager &mui, render_container &container)
{
	// reset the menu stack
	menu::stack_reset(mui.machine());

	// add the quit entry followed by the game select entry
	menu::stack_push_special_main<menu_quit_game>(mui, container);
	menu::stack_push<main_form>(mui, container);

	// force the menus on
	mui.show_menu();

	// make sure MAME is paused
	mui.machine().pause();
}

//-------------------------------------------------
//  ctor
//-------------------------------------------------

main_form::main_form(mame_ui_manager &mui, render_container &container)
	: menu(mui, container)
	, m_ui(mui)
	, m_container(container)
{}

//-------------------------------------------------
//  dtor
//-------------------------------------------------

main_form::~main_form()
{}

void main_form::populate()
{
	item_append(ui::menu_item_type::SEPARATOR); // Dummy

	auto box = add_widget<textbox>(m_container, m_ui);
	box->focus(true);
	box->text("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
	auto xbox = add_widget<textbox>(m_container, m_ui);
	xbox->rect(rectangle{ 0.6f, 0.8f, 0.6f, 0.8f });
	xbox->transparent(true);
	xbox->text("Test");

	auto lbl = add_widget<label>(m_container, m_ui);
	lbl->rect(rectangle{ 0.0f, 0.5f, 0.6f, 1.0f });
	lbl->text("LABEL");
}

void main_form::handle()
{
	process_widgets();

	process(PROCESS_CUSTOM_ONLY);
}

} // namespace ui
