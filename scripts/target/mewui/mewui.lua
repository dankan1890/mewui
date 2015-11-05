-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   mame.lua
--
--   MAME target makefile
--
---------------------------------------------------------------------------

dofile("../mame/arcade.lua")
dofile("../mame/mess.lua")

function createProjects_mewui_mewui(_target, _subtarget)
	createProjects_mame_arcade(_target, _subtarget)
	createProjects_mame_mess(_target, _subtarget)
end

function linkProjects_mewui_mewui(_target, _subtarget)
	linkProjects_mame_arcade(_target, _subtarget)
	linkProjects_mame_mess(_target, _subtarget)
end
