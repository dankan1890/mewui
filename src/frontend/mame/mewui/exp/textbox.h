#pragma once

#ifndef EXP_TEXTBOX_H
#define EXP_TEXTBOX_H

#include "basic_types.h"
#include <string>

class mame_ui_manager;
class render_container;

namespace ui {

class textbox : public exp_widget
{
public:
	textbox(render_container &container, mame_ui_manager &mui);
	textbox(render_container &container, mame_ui_manager &mui, float minx, float maxx, float miny, float maxy);
	textbox(render_container &container, mame_ui_manager &mui, rectangle rect);

	virtual ~textbox() {}

	virtual void draw_internal() override;
	void text(std::string text) { m_text = text; }
	void focus(bool focus) { m_focused = focus; }
	void transparent(bool tr) { m_transparent = tr; }
	bool focus() const { return m_focused; }
	bool transparent() const { return m_transparent; }
	void rect(rectangle rc) { m_rectangle = rc; }
	rectangle rect() const { return m_rectangle; }

private:
	ui::rectangle		m_rectangle;
	render_container	&m_container;
	mame_ui_manager		&m_ui;
	std::string			m_text;
	bool				m_transparent;
	bool				m_focused;
};

} // namespace ui
#endif
