if (_OPTIONS["targetos"] == "windows") then
	defines {
		"MEWUI_WINDOWS",
	}
end

--if (_OPTIONS["targetos"] == "macosx") then
	defines {
		"NO_MEM_TRACKING",
	}
--end

files {
	MAME_DIR .. "src/emu/mewui/auditmenu.cpp",
	MAME_DIR .. "src/emu/mewui/auditmenu.h",
	MAME_DIR .. "src/emu/mewui/cmddata.h",
	MAME_DIR .. "src/emu/mewui/cmdrender.h",
	MAME_DIR .. "src/emu/mewui/ctrlmenu.cpp",
	MAME_DIR .. "src/emu/mewui/ctrlmenu.h",
	MAME_DIR .. "src/emu/mewui/custmenu.cpp",
	MAME_DIR .. "src/emu/mewui/custmenu.h",
	MAME_DIR .. "src/emu/mewui/custui.cpp",
	MAME_DIR .. "src/emu/mewui/custui.h",
	MAME_DIR .. "src/emu/mewui/datfile.cpp",
	MAME_DIR .. "src/emu/mewui/datfile.h",
	MAME_DIR .. "src/emu/mewui/datmenu.cpp",
	MAME_DIR .. "src/emu/mewui/datmenu.h",
	MAME_DIR .. "src/emu/mewui/defimg.h",
	MAME_DIR .. "src/emu/mewui/dirmenu.cpp",
	MAME_DIR .. "src/emu/mewui/dirmenu.h",
	MAME_DIR .. "src/emu/mewui/dsplmenu.cpp",
	MAME_DIR .. "src/emu/mewui/dsplmenu.h",
	MAME_DIR .. "src/emu/mewui/icorender.h",
	MAME_DIR .. "src/emu/mewui/inifile.cpp",
	MAME_DIR .. "src/emu/mewui/inifile.h",
	MAME_DIR .. "src/emu/mewui/miscmenu.cpp",
	MAME_DIR .. "src/emu/mewui/miscmenu.h",
	MAME_DIR .. "src/emu/mewui/moptions.cpp",
	MAME_DIR .. "src/emu/mewui/moptions.h",
	MAME_DIR .. "src/emu/mewui/optsmenu.cpp",
	MAME_DIR .. "src/emu/mewui/optsmenu.h",
	MAME_DIR .. "src/emu/mewui/selector.cpp",
	MAME_DIR .. "src/emu/mewui/selector.h",
	MAME_DIR .. "src/emu/mewui/selgame.cpp",
	MAME_DIR .. "src/emu/mewui/selgame.h",
	MAME_DIR .. "src/emu/mewui/selsoft.cpp",
	MAME_DIR .. "src/emu/mewui/selsoft.h",
	MAME_DIR .. "src/emu/mewui/sndmenu.cpp",
	MAME_DIR .. "src/emu/mewui/sndmenu.h",
	MAME_DIR .. "src/emu/mewui/starimg.h",
	MAME_DIR .. "src/emu/mewui/toolbar.h",
	MAME_DIR .. "src/emu/mewui/utils.cpp",
	MAME_DIR .. "src/emu/mewui/utils.h",
}

dependency {
	{ MAME_DIR .. "src/emu/rendfont.cpp", GEN_DIR .. "emu/mewui/uicmd14.fh" },
}

custombuildtask {
	{ MAME_DIR .. "src/emu/mewui/uicmd14.png"	, GEN_DIR .. "emu/mewui/uicmd14.fh",  {  MAME_DIR.. "scripts/build/png2bdc.py",  MAME_DIR .. "scripts/build/file2str.py" }, {"@echo Converting uicmd14.png...", "python $(1) $(<) temp_cmd.bdc", "python $(2) temp_cmd.bdc $(@) font_uicmd14 UINT8" }},
}
