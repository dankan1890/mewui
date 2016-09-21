#include "textbox.h"

namespace ui {

textbox::textbox(main_form &mf)
	: m_rectangle(0, 0, 0.5f, 0.5f) 
	, m_container(mf.rcontainer())
{
}

textbox::textbox(main_form &mf, float minx, float miny, float maxx, float maxy)
	: m_rectangle(minx, miny, maxx, maxy)
	, m_container(mf.rcontainer())
{
	
}

void textbox::draw()
{
	
}

} // namespace ui