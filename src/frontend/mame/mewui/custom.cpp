// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	mewui/menu.cpp

	MEWUI Menu.

***************************************************************************/

#include "mewui/custom.h"

namespace mewui
{

custom_renderer::custom_renderer(const cloneable_renderer& rd)
	: rdptr_(rd)
	, crook_("menu_crook")
{
	crook_.check(facade<element::crook>::state::checked);
}

void custom_renderer::background(graph_reference graph, window)
{
	auto sz = graph.size();
	sz.width -= 30;
	sz.height -= 2;
	graph.rectangle(false, colors::gray_border);
	graph.rectangle({ 1, 1, 28, sz.height }, true, static_cast<color_rgb>(0xf2f4fe));
	graph.rectangle({ 29, 1, sz.width, sz.height }, true, static_cast<color_rgb>(0xeaf0ff));
}

void custom_renderer::item(graph_reference graph, const nana::rectangle& r, const attr& at)
{
	if (!at.enabled)
		return;

	if (at.item_state == state::active)
	{
		graph.rectangle(r, false, static_cast<color_rgb>(0xa8d8eb));
		graph.palette(false, static_cast<color_rgb>(0xc0ddfc));
		paint::draw(graph).corner(r, 1);

		if (at.enabled)
			graph.gradual_rectangle(nana::rectangle(r).pare_off(1), static_cast<color_rgb>(0xfdf4bf), static_cast<color_rgb>(0xfdf4bf), true);
	}

	if (at.checked && (menu::checks::none != at.check_style))
	{
		color clr(0xE6, 0xEF, 0xF4);
		auto crook_r = r;
		crook_r.width = 16;
		crook_.radio(at.check_style == menu::checks::option);
		crook_.draw(graph, clr, colors::black, crook_r, element_state::normal);
	}
}

bool buttom_custom::draw(graph_reference graph, const nana::color& bgcolor, const nana::color& fgcolor, const nana::rectangle& r, element_state state)
{
	auto rc = r;
	if (state == element_state::hovered)
	{
		graph.rectangle(rc, false, colors::blue);
		graph.rectangle(rc.pare_off(1), true, color(229, 241, 251));
	}
	else
	{
		graph.rectangle(rc, false, colors::gray_border);
		graph.rectangle(rc.pare_off(1), true, color(225, 225, 225));
	}
	return true;
}

void treebox_custom_renderer::set_color(const nana::color& bgcolor, const nana::color& fgcolor)
{
	bgcolor_ = bgcolor;
	fgcolor_ = fgcolor;
	renderer_->set_color(bgcolor, fgcolor);
}

void treebox_custom_renderer::bground(graph_reference graph, const compset_interface * compset) const
{
	comp_attribute_t attr;
	if (compset->comp_attribute(component::bground, attr))
	{
		const color color_table[][2] = {
			{ bgcolor_, bgcolor_ }, //highlighted
			{ { 51, 153, 255 },{ 51, 153, 255 } }  //Selected
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

void treebox_custom_renderer::expander(graph_reference graph, const compset_interface * compset) const
{
	comp_attribute_t attr;
	if (compset->comp_attribute(component::expender, attr))
	{
		auto ccolor = colors::black;
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

		arrow.draw(graph, bgcolor_, ccolor, r, element_state::normal);
	}
	//renderer_->expander(graph, compset);
}

void treebox_custom_renderer::crook(graph_reference graph, const compset_interface * compset) const
{
	comp_attribute_t attr;
	if (compset->comp_attribute(component::crook, attr))
	{
		attr.area.y += (attr.area.height - 16) / 2;
		crook_.check(compset->item_attribute().checked);
		crook_.draw(graph, bgcolor_, fgcolor_, attr.area, attr.mouse_pointed ? element_state::hovered : element_state::normal);
	}

	//		renderer_->crook(graph, compset);
}

void treebox_custom_renderer::text(graph_reference graph, const compset_interface * compset) const
{
	comp_attribute_t attr;
	if (compset->comp_attribute(component::text, attr))
	{
		if (compset->item_attribute().selected)
			fgcolor_ = colors::white;

		graph.string(point{ attr.area.x, attr.area.y + 3 }, compset->item_attribute().text, fgcolor_);
	}

	//		renderer_->text(graph, compset);
}

} // namespace mewui
