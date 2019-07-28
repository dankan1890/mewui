// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nanamame/mewui.cpp

	NANAMAME Start Point.

***************************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "drivenum.h"
#include "datfile.h"
#include "emuopts.h"
#include "nanamame.h"
#include "mainform.h"

namespace nanamame
{
const game_driver* start_gui(running_machine& machine, emu_options& _options, std::string& exename)
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
} // namespace nanamame
