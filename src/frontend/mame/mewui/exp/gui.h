#pragma once

#ifndef EXP_GUI_H
#define EXP_GUI_H

#include "ui/menu.h"
#include "textbox.h"

namespace ui {

class main_form : public menu
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
	using w_container = std::vector<std::shared_ptr<exp_widget>>;

	// reference
	std::string m_search;
	mame_ui_manager &m_ui;
	render_container &m_container;

	// standard functions
	virtual void populate() override;
	virtual void handle() override;

	// widgets processing
	void process_widgets();

	template<typename T>
	T *add_widget()
	{
		v_widgets.push_back(std::make_shared<T>(m_container, m_ui));
		return static_cast<T*>(v_widgets.back().get());
	}

	// internal widgets vectors
	w_container v_widgets;
};

} // namespace ui

#endif