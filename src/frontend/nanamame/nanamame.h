// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nanamame/mewui.h

	NANAMAME Start Point.

***************************************************************************/
#pragma once

#ifndef NANAMAME_H
#define NANAMAME_H

namespace nanamame
{
const game_driver* start_gui(running_machine& machine, emu_options& options, std::string& exename);
} // namespace nanamame

#endif /* NANAMAME_H */
