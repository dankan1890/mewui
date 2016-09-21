#pragma once
#include "ui/menu.h"

namespace ui {

class main_form : public menu
{
public:
	main_form(mame_ui_manager &, render_container &, const char *);
	virtual ~main_form();

	// force game select menu
	static void force_game_select(mame_ui_manager &, render_container &);

	render_container &rcontainer() const { return m_container; }
	mame_ui_manager &mui() const { return m_ui; }
protected:
	virtual bool menu_has_search_active() override { return m_search.empty(); }

private:
	std::string m_search;
	virtual void populate() override;
	virtual void handle() override;

	mame_ui_manager &m_ui;
	render_container &m_container;
};

} // namespace ui