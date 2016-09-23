#include "textbox.h"
#include "ui/ui.h"

namespace ui {

textbox::textbox(render_container &rc, mame_ui_manager &mui)
	: m_rectangle(0.0f, 0.5f, 0.0f, 0.5f)
	, m_container(rc)
	, m_ui(mui)
	, m_transparent(false)
	, m_focused(false)
{
}

textbox::textbox(render_container &rc, mame_ui_manager &mui, float minx, float miny, float maxx, float maxy)
	: m_rectangle(minx, miny, maxx, maxy)
	, m_container(rc)
	, m_ui(mui)
	, m_transparent(false)
	, m_focused(false)
{
}

textbox::textbox(render_container &rc, mame_ui_manager &mui, rectangle rect)
	: m_rectangle(rect)
	, m_container(rc)
	, m_ui(mui)
	, m_transparent(false)
	, m_focused(false)
{
}

void textbox::draw_internal()
{
	auto x1 = m_rectangle.left();
	auto x2 = m_rectangle.right();
	auto y1 = m_rectangle.top();
	auto y2 = m_rectangle.bottom();
	auto transp = m_transparent ? rgb_t::transparent : UI_BACKGROUND_COLOR;
	auto focus = m_focused ? UI_SELECTED_COLOR : UI_BORDER_COLOR;

	m_ui.draw_outlined_box(m_container, x1, y1, x2, y2, focus, transp);

	if (!m_text.empty())
	{
		auto visible_height = m_rectangle.height() - 2.0f * UI_BOX_TB_BORDER;
		float text_heigth;
		m_ui.draw_text_full(m_container, m_text.c_str(), x1 + UI_BOX_LR_BORDER, y1 + UI_BOX_TB_BORDER,
							m_rectangle.width() - 3.0f * UI_BOX_LR_BORDER, ui::text_layout::LEFT, ui::text_layout::WORD,
							mame_ui_manager::NONE, UI_TEXT_COLOR, UI_TEXT_BG_COLOR, nullptr, &text_heigth);
		if (visible_height >= text_heigth)
		{
			m_ui.draw_text_full(m_container, m_text.c_str(), x1 + UI_BOX_LR_BORDER, y1 + UI_BOX_TB_BORDER,
								m_rectangle.width() - 3.0f * UI_BOX_LR_BORDER, ui::text_layout::LEFT, ui::text_layout::WORD,
								mame_ui_manager::NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR);
		}
		else
		{
			m_container.add_line(x2 - UI_BOX_LR_BORDER, y1, x2 - UI_BOX_LR_BORDER, y2, UI_LINE_WIDTH, focus, PRIMFLAG_BLENDMODE(BLENDMODE_ALPHA));
			int lines = std::floor(visible_height / m_ui.get_line_height());
			std::vector<int> start, finish;
			m_ui.wrap_text(m_container, m_text.c_str(), x1 + UI_BOX_LR_BORDER, y1 + UI_BOX_TB_BORDER, m_rectangle.width() - 3.0f * UI_BOX_LR_BORDER, start, finish);
			y1 += UI_BOX_TB_BORDER;
			for (int x = 0; x < lines; ++x)
			{
				m_ui.draw_text_full(m_container, m_text.substr(start[x], finish[x] - start[x]).c_str(), x1 + UI_BOX_LR_BORDER, y1,
									m_rectangle.width() - 3.0f * UI_BOX_LR_BORDER, ui::text_layout::LEFT, ui::text_layout::NEVER,
									mame_ui_manager::NORMAL, UI_TEXT_COLOR, UI_TEXT_BG_COLOR);
				y1 += m_ui.get_line_height();
			}
		}
	}
}

} // namespace ui