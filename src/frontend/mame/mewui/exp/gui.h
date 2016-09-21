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

protected:
	virtual bool menu_has_search_active() override { return m_search.empty(); }

private:
	std::string m_search;
	virtual void populate() override;
	virtual void handle() override;

	mame_ui_manager &m_mui;
	render_container &m_container;
};

} // namespace ui