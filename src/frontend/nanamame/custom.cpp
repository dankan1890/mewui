// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nanamame/custom.cpp

	NANAMAME customization.

***************************************************************************/

#include "custom.h"
#include <algorithm>
#include <nana/paint/text_renderer.hpp>
#include <nana/gui/drawing.hpp>
#include <nana/gui/detail/effects_renderer.hpp>

namespace nanamame
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

// Status inline widget

void status_widget::create(window wd)
{
	pic_.create(wd, { 0, 0, 16, 16 });
	pic_.events().click([this] {
		indicator_->selected(pos_);
	});

	pic_.events().mouse_move([this] {
		indicator_->hovered(pos_);
	});
}

void status_widget::activate(inline_indicator& ind, index_type pos)
{
	indicator_ = &ind;
	pos_ = pos;
	pic_.transparent(true);
}

void status_widget::resize(const size& dimension)
{
	auto sz = dimension;
	auto half = sz.width / 2;
	auto hhalf = sz.height / 2;
	nana::rectangle r(half - 8, hhalf - 8, 16, 16);
	pic_.move(r);
}

void status_widget::set(const value_type& value)
{
	if (txt_ != value)
	{
		drawing dd{ pic_ };
		status_ = (value == "Working") ? colors::green : colors::red;
		dd.draw([&](paint::graphics& graph) {
			graph.rectangle(true, status_);
			graph.rectangle(false, colors::light_gray);
		});
		txt_ = value;
	}
}

// Combox item_render
void combox_item_render::image(bool enb, unsigned px)
{
	image_enabled_ = enb;
	image_pixels_ = px;
}

void combox_item_render::render(widget_reference, graph_reference graph, const nana::rectangle& r, const float_listbox::item_interface* item, state_t state)
{
	if (state == StateHighlighted)
	{
		graph.rectangle(r, true, color("#3296AA"));

	}
	else
		graph.rectangle(r, true, color("#232323"));

	int x = r.x + 2;
	if (image_enabled_)
	{
		unsigned vpix = (r.height - 4);
		if (item->image())
		{
			auto imgsz = fit_zoom(item->image().size(), { image_pixels_, vpix });

			point to_pos(x, r.y + 2);
			to_pos.x += (image_pixels_ - imgsz.width) / 2;
			to_pos.y += (vpix - imgsz.height) / 2;
			item->image().stretch(nana::rectangle{ item->image().size() }, graph, nana::rectangle(to_pos, imgsz));
		}
		x += (image_pixels_ + 2);
	}

	graph.string({ x, r.y + 2 }, item->text(), colors::white);
}

unsigned combox_item_render::item_pixels(graph_reference graph) const
{
	unsigned ascent, descent, ileading;
	graph.text_metrics(ascent, descent, ileading);
	return ascent + descent + 4;
}

} // namespace nanamame
