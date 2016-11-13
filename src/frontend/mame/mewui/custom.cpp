// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	mewui/custom.cpp

	MEWUI customization.

***************************************************************************/

#include "mewui/custom.h"
#include <algorithm>
#include <nana/paint/text_renderer.hpp>

namespace mewui
{

// Menu
menu_renderer::menu_renderer(const cloneable_renderer& rd)
	: rdptr_(rd)
	, crook_("menu_crook")
{
	crook_.check(facade<element::crook>::state::checked);
}

void menu_renderer::background(graph_reference graph, window)
{
	auto sz = graph.size();
	sz.width -= 30;
	sz.height -= 2;
	graph.rectangle(false, colors::black);
	graph.rectangle({ 1, 1, 28, sz.height }, true, color("#2C2C2C"));
	graph.rectangle({ 29, 1, sz.width, sz.height }, true, color("#2C2C2C"));
}

void menu_renderer::item(graph_reference graph, const nana::rectangle& r, const attr& at)
{
	if (!at.enabled)
		return;

	if (at.item_state == state::active)
		graph.rectangle(r, true, color("#3296AA"));

	if (at.checked && (menu::checks::none != at.check_style))
	{
		color clr(0xE6, 0xEF, 0xF4);
		auto crook_r = r;
		crook_r.width = 16;
		crook_.radio(at.check_style == menu::checks::option);
		crook_.draw(graph, clr, colors::white, crook_r, element_state::normal);
	}
}

void menu_renderer::item_text(graph_reference graph, const nana::point& pos, const std::string& text, unsigned pixels, const attr& atr)
{
	graph.palette(true, atr.enabled ? colors::white : colors::gray_border);
	nana::paint::text_renderer tr(graph);

	auto wstr = to_wstring(text);
	tr.render(pos, wstr.c_str(), wstr.length(), pixels, true);
}


// Button
bool button_renderer::draw(graph_reference graph, const nana::color& bgcolor, const nana::color& fgcolor, const nana::rectangle& r, element_state state)
{
	auto rc = r;
	if (state == element_state::hovered)
	{
		graph.rectangle(rc, false, colors::black);
		graph.rectangle(rc.pare_off(1), true, color("#3296AA"));
	}
	else
	{
		graph.rectangle(rc, false, colors::black);
		graph.rectangle(rc.pare_off(1), true, color("#2C2C2C"));
	}
	return true;
}

// Treebox
void treebox_renderer::set_color(const nana::color& bgcolor, const nana::color& fgcolor)
{
	fgcolor_ = colors::white;
	bgcolor_ = color("#232323");
	renderer_->set_color(bgcolor_, fgcolor_);
}

void treebox_renderer::bground(graph_reference graph, const compset_interface * compset) const
{
	comp_attribute_t attr;
	if (compset->comp_attribute(component::bground, attr))
	{
		const color color_table[][2] = {
			{ bgcolor_, bgcolor_ }, //highlighted
			{ color("#3296AA"), color("#3296AA") }  //Selected
		};

		const color *clrptr;
		if (compset->item_attribute().selected)
			clrptr = color_table[1];
		else
			clrptr = color_table[0];

		if (clrptr)
		{
			auto rc = attr.area;
			rc.width = std::max(attr.area.width, graph.width());
			rc.x = 0;
			graph.rectangle(rc, false, clrptr[1]);
			graph.rectangle(rc.pare_off(1), true, *clrptr);
		}
	}
}

void treebox_renderer::expander(graph_reference graph, const compset_interface * compset) const
{
	comp_attribute_t attr;
	if (compset->comp_attribute(component::expender, attr))
	{
		auto ccolor = colors::white;
		facade<element::arrow> arrow("solid_triangle");
		arrow.direction(direction::southeast);
		if (!compset->item_attribute().expended)
		{
			arrow.switch_to("hollow_triangle");
			arrow.direction(direction::east);
		}
		auto r = attr.area;
		r.y += (attr.area.height - 16) / 2;
		r.width = r.height = 16;
		if (compset->item_attribute().selected)
		{
			ccolor = colors::white;
			if (attr.mouse_pointed)
			{
				if (!compset->item_attribute().expended)
					arrow.switch_to("solid_triangle");
				else
					arrow.switch_to("hollow_triangle");
			}
		}
		else
		{
			if (attr.mouse_pointed)
				ccolor = colors::deep_sky_blue;
		}

		arrow.draw(graph, bgcolor_, ccolor, r, element_state::normal);
	}
}

void treebox_renderer::crook(graph_reference graph, const compset_interface * compset) const
{
	comp_attribute_t attr;
	if (compset->comp_attribute(component::crook, attr))
	{
		attr.area.y += (attr.area.height - 16) / 2;
		crook_.check(compset->item_attribute().checked);
		crook_.draw(graph, bgcolor_, fgcolor_, attr.area, attr.mouse_pointed ? element_state::hovered : element_state::normal);
	}
}

void treebox_renderer::text(graph_reference graph, const compset_interface * compset) const
{
	comp_attribute_t attr;
	if (compset->comp_attribute(component::text, attr))
	{
		if (compset->item_attribute().selected)
			fgcolor_ = colors::white;

		graph.string(point{ attr.area.x, attr.area.y + 3 }, compset->item_attribute().text, fgcolor_);
	}
}

// Tabbar
void tabbar_renderer::background(graph_reference graph, const nana::rectangle& r, const ::nana::color& bgcolor)
{
	if (bgcolor_ != bgcolor)
	{
		bgcolor_ = bgcolor;

		dark_bgcolor_ = bgcolor.blend(colors::black, 0.9);
		blcolor_ = bgcolor.blend(colors::black, 0.5);
		ilcolor_ = bgcolor.blend(colors::white, 0.9);
	}

	graph.rectangle(true, bgcolor);
}

void tabbar_renderer::item(graph_reference graph, const item_t& m, bool active, state_t sta)
{
	const auto& r = m.r;
	color bgcolor;
	color blcolor;
	color dark_bgcolor;

	if (m.bgcolor.invisible())
	{
		bgcolor = bgcolor_;
		blcolor = blcolor_;
		dark_bgcolor = dark_bgcolor_;
	}
	else
	{
		bgcolor = m.bgcolor;
		blcolor = m.bgcolor.blend(colors::black, 0.5);
		dark_bgcolor = m.bgcolor.blend(colors::black, 0.9);
	}

	auto round_r = r;
	round_r.height += 2;
	graph.rectangle(round_r, true, blcolor);

	auto beg = bgcolor;
	auto end = dark_bgcolor;

	if (active)
	{
		if (m.bgcolor.invisible())
			beg = ilcolor_;
		else
			beg = m.bgcolor.blend(colors::white, 0.5);
		end = bgcolor;
	}

	if (sta == item_renderer::highlight)
		beg = beg.blend(colors::white, 0.5);

	graph.gradual_rectangle(round_r.pare_off(1), beg, end, true);
}


} // namespace mewui
