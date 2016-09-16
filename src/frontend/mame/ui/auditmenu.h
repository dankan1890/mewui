// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

    ui/auditmenu.h

    Internal UI user interface.

***************************************************************************/

#pragma once

#ifndef MAME_FRONTEND_UI_AUDITMENU_H
#define MAME_FRONTEND_UI_AUDITMENU_H

#include "ui/menu.h"

namespace ui {
//-------------------------------------------------
//  class audit menu
//-------------------------------------------------
class menu_audit : public menu
{
public:
	enum class mode { FAST, FULL };

	menu_audit(mame_ui_manager &mui, render_container &container, vptr_game &availablesorted, vptr_game &unavailablesorted, mode audit_mode);
	virtual ~menu_audit() override;

private:
	virtual void populate() override;
	virtual void handle() override;

	void save_available_machines() const;

	vptr_game &m_availablesorted;
	vptr_game &m_unavailablesorted;

	mode m_audit_mode;
	bool m_first;
};

bool sorted_game_list(const game_driver *x, const game_driver *y);

} // namespace ui

#endif /* MAME_FRONTEND_UI_AUDITMENU_H */
