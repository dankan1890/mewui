// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

    ui/mewui.h

    ImGui launch menu.

***************************************************************************/

#ifndef PROJECT_MODERN_H
#define PROJECT_MODERN_H

#include "ui/menu.h"

class media_auditor;

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

	std::vector<const game_driver *> m_sortedlist;
	std::vector<const game_driver *> m_displaylist;

	std::string m_search;
	int m_isabios = 0;
	int sub_node_year = -1;
	int sub_node_manuf = -1;
	u16 m_current_filter = 0;

	void init_sorted_list();
	void build_list(const std::string& text = {} );
	bool filters_panel();
	void machines_panel(bool f_reset);
	void menubar();
	bool software_panel(const game_driver * drv);
	std::string make_error_text(bool summary, media_auditor const &auditor, bool software);
	void show_error(std::string &error_text, bool &error);
	void imguihandle_keys(int &current, int max);
	void imguihandle_mouse(s32 &m_mouse_x, s32 &m_mouse_y, s32 &zdelta, bool &m_mouse_button, bool &m_mouse_rbutton);
};

} // namespace ui


#endif //PROJECT_MODERN_H
