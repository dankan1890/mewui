-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   test.lua
--
--   Small driver-specific example makefile
--   Use make SUBTARGET=tiny to build
--
---------------------------------------------------------------------------


--------------------------------------------------
-- Specify all the CPU cores necessary for the
-- drivers referenced in tiny.lst.
--------------------------------------------------

CPUS["Z80"] = true
CPUS["M6502"] = true
CPUS["MCS48"] = true
CPUS["MCS51"] = true
CPUS["M6800"] = true
CPUS["M6809"] = true
CPUS["M680X0"] = true
CPUS["TMS9900"] = true
CPUS["COP400"] = true

--------------------------------------------------
-- Specify all the sound cores necessary for the
-- drivers referenced in tiny.lst.
--------------------------------------------------

SOUNDS["SAMPLES"] = true
SOUNDS["DAC"] = true
SOUNDS["DISCRETE"] = true
SOUNDS["AY8910"] = true
SOUNDS["YM2151"] = true
SOUNDS["ASTROCADE"] = true
SOUNDS["TMS5220"] = true
SOUNDS["OKIM6295"] = true
SOUNDS["HC55516"] = true
SOUNDS["YM3812"] = true
SOUNDS["CEM3394"] = true
SOUNDS["VOTRAX"] = true
SOUNDS["WAVE"] = true
SOUNDS["SN76477"] = true

--------------------------------------------------
-- specify available video cores
--------------------------------------------------

VIDEOS["FIXFREQ"] = true

--------------------------------------------------
-- specify available machine cores
--------------------------------------------------

MACHINES["NETLIST"] = true
MACHINES["6821PIA"] = true
MACHINES["TTL74148"] = true
MACHINES["TTL74153"] = true
MACHINES["TTL7474"] = true
MACHINES["RIOT6532"] = true
MACHINES["PIT8253"] = true
MACHINES["Z80CTC"] = true
MACHINES["68681"] = true
MACHINES["BANKDEV"] = true
MACHINES["Z80PIO"] = true
MACHINES["Z80DART"] = true
MACHINES["Z80DMA"] = true
MACHINES["WD_FDC"] = true

--------------------------------------------------
-- specify available bus cores
--------------------------------------------------

BUSES["CENTRONICS"] = true
BUSES["RS232"] = true
BUSES["ABCBUS"] = true
BUSES["SCSI"] = true

--------------------------------------------------
-- This is the list of files that are necessary
-- for building all of the drivers referenced
-- in tiny.lst
--------------------------------------------------

function createProjects_mewui_test(_target, _subtarget)
	project ("mewui_test")
	targetsubdir(_target .."_" .. _subtarget)
	kind (LIBTYPE)
	uuid (os.uuid("drv-mewui-test"))

	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/devices",
		MAME_DIR .. "src/lib/netlist",
		MAME_DIR .. "src/mame",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "3rdparty",
		GEN_DIR  .. _target .. "/layout",
	}

	files{
		MAME_DIR .. "src/mame/machine/ticket.cpp",
		MAME_DIR .. "src/mame/drivers/carpolo.cpp",
		MAME_DIR .. "src/mame/machine/carpolo.cpp",
		MAME_DIR .. "src/mame/video/carpolo.cpp",
		MAME_DIR .. "src/mame/drivers/circus.cpp",
		MAME_DIR .. "src/mame/audio/circus.cpp",
		MAME_DIR .. "src/mame/video/circus.cpp",
		MAME_DIR .. "src/mame/drivers/exidy.cpp",
		MAME_DIR .. "src/mame/audio/exidy.cpp",
		MAME_DIR .. "src/mame/video/exidy.cpp",
		MAME_DIR .. "src/mame/audio/exidy440.cpp",
		MAME_DIR .. "src/mame/drivers/starfire.cpp",
		MAME_DIR .. "src/mame/video/starfire.cpp",
		MAME_DIR .. "src/mame/drivers/vertigo.cpp",
		MAME_DIR .. "src/mame/machine/vertigo.cpp",
		MAME_DIR .. "src/mame/video/vertigo.cpp",
		MAME_DIR .. "src/mame/drivers/victory.cpp",
		MAME_DIR .. "src/mame/video/victory.cpp",
		MAME_DIR .. "src/mame/audio/targ.cpp",
		MAME_DIR .. "src/mame/drivers/astrocde.cpp",
		MAME_DIR .. "src/mame/video/astrocde.cpp",
		MAME_DIR .. "src/mame/drivers/gridlee.cpp",
		MAME_DIR .. "src/mame/audio/gridlee.cpp",
		MAME_DIR .. "src/mame/video/gridlee.cpp",
		MAME_DIR .. "src/mame/drivers/williams.cpp",
		MAME_DIR .. "src/mame/machine/williams.cpp",
		MAME_DIR .. "src/mame/audio/williams.cpp",
		MAME_DIR .. "src/mame/video/williams.cpp",
		MAME_DIR .. "src/mame/audio/gorf.cpp",
		MAME_DIR .. "src/mame/audio/wow.cpp",
		MAME_DIR .. "src/mame/drivers/gaelco.cpp",
		MAME_DIR .. "src/mame/video/gaelco.cpp",
		MAME_DIR .. "src/mame/machine/gaelcrpt.cpp",
		MAME_DIR .. "src/mame/drivers/wrally.cpp",
		MAME_DIR .. "src/mame/machine/wrally.cpp",
		MAME_DIR .. "src/mame/video/wrally.cpp",
		MAME_DIR .. "src/mame/drivers/looping.cpp",
		MAME_DIR .. "src/mame/drivers/supertnk.cpp",
		MAME_DIR .. "src/mame/drivers/pong.cpp",
		MAME_DIR .. "src/mame/drivers/nl_pong.cpp",
		MAME_DIR .. "src/mame/drivers/nl_pongd.cpp",
		MAME_DIR .. "src/mame/drivers/nl_breakout.cpp",
		MAME_DIR .. "src/mame/drivers/abc80.cpp",
		MAME_DIR .. "src/mame/machine/abc80kb.cpp",
		MAME_DIR .. "src/mame/video/abc80.cpp",
	}
end

function linkProjects_mewui_test(_target, _subtarget)
	links {
		"mewui_test",
	}
end
