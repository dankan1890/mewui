// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	mewui/mewui.cpp

	MEWUI Start Point.

***************************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "drivenum.h"
#include "mewui/datfile.h"
#include "emuopts.h"
#include "mewui/mewui.h"
#include "mewui/mainform.h"

namespace mewui
{
	const game_driver *start_gui(running_machine &machine, emu_options &_options, std::string &exename)
	{
		auto system = &GAME_NAME(___empty);
		auto mui = std::make_unique<mame_ui_manager>(machine);
		mui->load_ui_options();

		// Add main icon
		API::window_icon_default(paint::image{ exename });

		// Execute GUI
		main_form gui{ machine, &system, _options, mui, exename };
		gui.show();
		exec();

		return system;
	}
}