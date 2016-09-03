// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nana/menu.cpp

	NANAMAME Menu.

***************************************************************************/

#include "nana/menu.h"
namespace nanamame
{
	custom_renderer::custom_renderer(const cloneable_renderer & rd)
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

	void custom_renderer::item(graph_reference graph, const nana::rectangle& r, const attr & at)
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
} // namespace nanamame