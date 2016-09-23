#include "label.h"
#include "ui/ui.h"

namespace ui {

label::label(render_container &rc, mame_ui_manager &mui)
	: m_rectangle(0.0f, 0.5f, 0.0f, 0.5f)
	, m_container(rc)
	, m_ui(mui)
	, m_transparent(false)
	, m_focused(false)
{}

label::label(render_container &rc, mame_ui_manager &mui, float minx, float miny, float maxx, float maxy)
	: m_rectangle(minx, miny, maxx, maxy)
	, m_container(rc)
	, m_ui(mui)
	, m_transparent(false)
	, m_focused(false)
{}

label::label(render_container &rc, mame_ui_manager &mui, rectangle rect)
	: m_rectangle(rect)
	, m_container(rc)
	, m_ui(mui)
	, m_transparent(false)
	, m_focused(false)
{}

void label::draw_internal()
{
	auto x1 = m_rectangle.left();
	auto x2 = m_rectangle.right();
	auto y1 = m_rectangle.top();
	auto y2 = m_rectangle.bottom();
	auto transp = m_transparent ? rgb_t::transparent : UI_BACKGROUND_COLOR;

	m_container.add_rect(x1, y1, x2, y2, transp, PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA));
	if (!m_text.empty())
	{
			m_ui.draw_text_full(m_container, m_text.c_str(), x1, y1,m_rectangle.width(), ui::text_layout::LEFT, 
								ui::text_layout::TRUNCATE, mame_ui_manager::NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR);
	}
}

} // namespace ui