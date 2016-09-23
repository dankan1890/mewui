#pragma once

#ifndef EXP_GUI_H
#define EXP_GUI_H

#include "ui/menu.h"
#include "basic_types.h"

namespace ui {

class main_form : public menu, exp_widget
{
public:
	main_form(mame_ui_manager &mui, render_container &container);
	virtual ~main_form();

	// force game select menu
	static void force_game_select(mame_ui_manager &mui, render_container &container);

	render_container &rcontainer() const { return m_container; }
	mame_ui_manager &mui() const { return m_ui; }
protected:
	virtual bool menu_has_search_active() override { return !m_search.empty(); }

private:

	// reference
	std::string m_search;
	mame_ui_manager &m_ui;
	render_container &m_container;

	// standard functions
	virtual void populate() override;
	virtual void handle() override;

};

} // namespace ui

#endif