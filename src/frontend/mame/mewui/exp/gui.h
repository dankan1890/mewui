#pragma once

#ifndef EXP_GUI_H
#define EXP_GUI_H

#include "ui/menu.h"
#include "textbox.h"

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
		if (std::is_same<textbox, T>::value)
		{
			v_textbox.emplace_back(textbox{ m_container, m_ui });
			return &v_textbox.back();
		}
		return nullptr; // TODO: return error
	}

	// internal widgets vectors
	std::vector<textbox> v_textbox;
};

} // namespace ui

#endif