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
#include <nana/gui/element.hpp>
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

class custom_placer
	: public treebox::compset_placer_interface
{
	using cloneable_placer = pat::cloneable<treebox::compset_placer_interface>;
public:
	explicit custom_placer(const cloneable_placer& r)
		: placer_(r) 
	{}

private:
	void enable(component_t comp, bool enabled) override
	{
		return placer_->enable(comp, enabled);
	}

	bool enabled(component_t comp) const override
	{
		return placer_->enabled(comp);
	}

	unsigned item_height(graph_reference graph) const override
	{
		return placer_->item_height(graph);
	}

	unsigned item_width(graph_reference graph, const item_attribute_t& attr) const override
	{
		return graph.width();
	}

	bool locate(component_t comp, const item_attribute_t& attr, nana::rectangle * r) const override
	{
		return placer_->locate(comp, attr, r);
	}

	cloneable_placer placer_;
};

} // namespace mewui

#endif /* MEWUI_MENU_H */
