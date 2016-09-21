#pragma once
#include "basic_types.h"
#include "gui.h"

namespace ui {

class textbox
{
public:
	textbox(main_form &);
	textbox(main_form &, float minx, float miny, float maxx, float maxy);

private:
	void draw();
	ui::rectangle m_rectangle;
	render_container &m_container;
};

} // namespace ui