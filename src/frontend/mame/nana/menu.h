// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nana/menu.h

	NANAMANE Menu.

***************************************************************************/
#pragma once

#ifndef __MAME_NANA_MENU_H__
#define __MAME_NANA_MENU_H__

#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/element.hpp>
using namespace nana;

namespace nanamame
{
	class custom_renderer : public menu::renderer_interface
	{
	public:
		using cloneable_renderer = pat::cloneable<renderer_interface>;

		custom_renderer(const cloneable_renderer & rd);

	private:
		void background(graph_reference graph, window wd) override;
		void item(graph_reference graph, const nana::rectangle& r, const attr & at) override;
		void item_image(graph_reference graph, const nana::point& pos, unsigned image_px, const paint::image& img) override
		{
			rdptr_->item_image(graph, pos, image_px, img);
		}

		void item_text(graph_reference graph, const nana::point& pos, const std::string& text, unsigned pixels, const attr& atr) override
		{
			rdptr_->item_text(graph, pos, text, pixels, atr);
		}

		void sub_arrow(graph_reference graph, const nana::point& pos, unsigned pixels, const attr & atr) override
		{
			rdptr_->sub_arrow(graph, pos, pixels, atr);
		}

	private:
		cloneable_renderer rdptr_;
		facade<element::crook> crook_;
	};

} // namespace nanamame

#endif /* __MAME_NANA_MENU_H__ */