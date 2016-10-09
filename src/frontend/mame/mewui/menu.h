// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	mewui/menu.h

	MEWUI Menu.

***************************************************************************/
#pragma once

#ifndef MEWUI_MENU_H
#define MEWUI_MENU_H

#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/tabbar.hpp>
#include <nana/gui/element.hpp>
#include <algorithm>
using namespace nana;

namespace mewui
{
class custom_renderer : public menu::renderer_interface
{
public:
	using cloneable_renderer = pat::cloneable<renderer_interface>;

	custom_renderer(const cloneable_renderer& rd);

private:
	void background(graph_reference graph, window wd) override;
	void item(graph_reference graph, const nana::rectangle& r, const attr& at) override;

	void item_image(graph_reference graph, const nana::point& pos, unsigned image_px, const paint::image& img) override
	{
		rdptr_->item_image(graph, pos, image_px, img);
	}

	void item_text(graph_reference graph, const nana::point& pos, const std::string& text, unsigned pixels, const attr& atr) override
	{
		rdptr_->item_text(graph, pos, text, pixels, atr);
	}

	void sub_arrow(graph_reference graph, const nana::point& pos, unsigned pixels, const attr& atr) override
	{
		rdptr_->sub_arrow(graph, pos, pixels, atr);
	}

	cloneable_renderer rdptr_;
	facade<element::crook> crook_;
};

class treebox_custom_renderer
	: public treebox::renderer_interface
{
	using cloneable_renderer = pat::cloneable<treebox::renderer_interface>;
public:
	explicit treebox_custom_renderer(const cloneable_renderer & rd)
		: renderer_(rd)
	{}
private:
	void set_color(const nana::color& bgcolor, const nana::color& fgcolor) override
	{
		renderer_->set_color(bgcolor, fgcolor);
		bgcolor_ = bgcolor;
	}

	void bground(graph_reference graph, const compset_interface * compset) const override
	{
		comp_attribute_t attr;
		if (compset->comp_attribute(component::bground, attr))
		{
			const color color_table[][2] = { 
				{ bgcolor_, bgcolor_ }, //highlighted
				{ { 0xD5, 0xEF, 0xFC },{ 0x99, 0xDE, 0xFD } }  //Selected
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

	void expander(graph_reference graph, const compset_interface * compset) const override
	{
		renderer_->expander(graph, compset);
	}

	void crook(graph_reference graph, const compset_interface * compset) const override
	{
		renderer_->crook(graph, compset);
	}

	void icon(graph_reference graph, const compset_interface * compset) const override
	{
		renderer_->icon(graph, compset);
	}

	void text(graph_reference graph, const compset_interface * compset) const override
	{
		renderer_->text(graph, compset);
	}

	mutable color bgcolor_;
	cloneable_renderer renderer_;
};

class tabbar_custom_renderer : public tabbar<std::string>::item_renderer
{
public:
	using cloneable_renderer = pat::cloneable<tabbar<std::string>::item_renderer>;

	tabbar_custom_renderer(const cloneable_renderer& rd) 
		: renderer_(rd)
	{}

private:
	void background(graph_reference graph, const nana::rectangle& r, const ::nana::color& bgcolor) override
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

	void item(graph_reference graph, const item_t& m, bool active, state_t sta) override
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

	void close_fly(graph_reference graph, const nana::rectangle& r, bool active, state_t state) override
	{
		renderer_->close_fly(graph, r, active, state);
	}

	void add(graph_reference graph, const nana::rectangle& r, state_t state) override
	{
		renderer_->add(graph, r, state);
	}

	void close(graph_reference graph, const nana::rectangle& r, state_t state) override
	{
		renderer_->close(graph, r, state);
	}

	void back(graph_reference graph, const nana::rectangle& r, state_t state) override
	{
		renderer_->back(graph, r, state);
	}

	void next(graph_reference graph, const nana::rectangle& r, state_t state) override
	{
		renderer_->next(graph, r, state);
	}

	void list(graph_reference graph, const nana::rectangle& r, state_t state) override
	{
		renderer_->list(graph, r, state);
	}

	color bgcolor_;
	color dark_bgcolor_;
	color blcolor_;
	color ilcolor_;
	cloneable_renderer renderer_;
};
} // namespace mewui

#endif /* MEWUI_MENU_H */
