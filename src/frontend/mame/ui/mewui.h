// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

    ui/mewui.h

    ImGui launch menu.

***************************************************************************/

#ifndef PROJECT_MODERN_H
#define PROJECT_MODERN_H

#include "ui/menu.h"

namespace ui {
class modern_launcher : public menu
{
public:
	modern_launcher(mame_ui_manager &mui, render_container &container);
	virtual ~modern_launcher();

	// force game select menu
	static void force_game_select(mame_ui_manager &mui, render_container &container);

protected:
	virtual bool menu_has_search_active() override { return !m_search.empty(); }
	virtual void custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2) override;


private:
	virtual void populate(float &customtop, float &custombottom) override;
	virtual void handle() override;

	static std::vector<const game_driver *> m_sortedlist;
	std::vector<const game_driver *> m_displaylist;

	std::string m_search;
	static int m_isabios;
	int sub_node_year = -1;
	int sub_node_manuf = -1;
	void init_sorted_list();
	void build_list(const std::string& text = {} );
	void filters_panel();
	void machines_panel();
	void menubar();

};

} // namespace ui


#endif //PROJECT_MODERN_H
