-- license:BSD-3-Clause
-- copyright-holders:Dankan1890

---------------------------------------------------------------------------
--
--   test.lua
--
--   MEWUI TEST target makefile
--
---------------------------------------------------------------------------


--------------------------------------------------
-- specify available BUSES cores
--------------------------------------------------

BUSES["C64"] = true
BUSES["CBM2"] = true
BUSES["CBMIEC"] = true
BUSES["CENTRONICS"] = true
BUSES["CPC"] = true
BUSES["GAMEBOY"] = true
BUSES["GBA"] = true
BUSES["GENERIC"] = true
BUSES["IEEE488"] = true
BUSES["ISA"] = true
BUSES["MIDI"] = true
BUSES["NEOGEO"] = true
BUSES["NES"] = true
BUSES["NES_CTRL"] = true
BUSES["PC_KBD"] = true
BUSES["PET"] = true
BUSES["PLUS4"] = true
BUSES["RS232"] = true
BUSES["SCSI"] = true
BUSES["SNES"] = true
BUSES["SNES_CTRL"] = true
BUSES["VCS"] = true
BUSES["VCS_CTRL"] = true
BUSES["VIC10"] = true
BUSES["VIC20"] = true
BUSES["ZORRO"] = true

--------------------------------------------------
-- specify available CPUS cores
--------------------------------------------------

CPUS["ARM7"] = true
CPUS["G65816"] = true
CPUS["I8085"] = true
CPUS["I86"] = true
CPUS["LR35902"] = true
CPUS["M6502"] = true
CPUS["M6800"] = true
CPUS["M6809"] = true
CPUS["M6805"] = true
CPUS["M680X0"] = true
CPUS["MCS48"] = true
CPUS["MCS51"] = true
CPUS["MINX"] = true
CPUS["MIPS"] = true
CPUS["NEC"] = true
CPUS["S2650"] = true
CPUS["SPC700"] = true
CPUS["SUPERFX"] = true
CPUS["RSP"] = true
CPUS["UPD7725"] = true
CPUS["UPD7810"] = true
CPUS["Z80"] = true

--------------------------------------------------
-- specify available MACHINES cores
--------------------------------------------------

MACHINES["6522VIA"] = true
MACHINES["6821PIA"] = true
MACHINES["6840PTM"] = true
MACHINES["ACIA6850"] = true
MACHINES["AKIKO"] = true
MACHINES["AM9517A"] = true
MACHINES["AMIGAFDC"] = true
MACHINES["AT_KEYBC"] = true
MACHINES["AUTOCONFIG"] = true
MACHINES["BANKDEV"] = true
MACHINES["CMOS40105"] = true
MACHINES["COM8116"] = true
MACHINES["CORVUSHD"] = true
MACHINES["CR511B"] = true
MACHINES["DMAC"] = true
MACHINES["DS1302"] = true
MACHINES["DS1315"] = true
MACHINES["DS75160A"] = true
MACHINES["DS75161A"] = true
MACHINES["E05A30"] = true
MACHINES["E05A03"] = true
MACHINES["EEPROMDEV"] = true
MACHINES["GAYLE"] = true
MACHINES["I8255"] = true
MACHINES["I8257"] = true
MACHINES["I8251"] = true
MACHINES["I2CMEM"] = true
MACHINES["IDE"] = true
MACHINES["INS8250"] = true
MACHINES["INTELFLASH"] = true
MACHINES["LATCH8"] = true
MACHINES["MC146818"] = true
MACHINES["MC6852"] = true
MACHINES["MIOT6530"] = true
MACHINES["MOS6526"] = true
MACHINES["MOS6529"] = true
MACHINES["MOS6551"] = true
MACHINES["MOS6702"] = true
MACHINES["MOS8706"] = true
MACHINES["MOS8722"] = true
MACHINES["MOS8726"] = true
MACHINES["MSM58321"] = true
MACHINES["MSM6242"] = true
MACHINES["NETLIST"] = true
MACHINES["PC_LPT"] = true
MACHINES["PCKEYBRD"] = true
MACHINES["PIC8259"] = true
MACHINES["PIT8253"] = true
MACHINES["PLA"] = true
MACHINES["R64H156"] = true
MACHINES["RP5C01"] = true
MACHINES["RP5H01"] = true
MACHINES["S2636"] = true
MACHINES["STEPPERS"] = true
MACHINES["TIMEKPR"] = true
MACHINES["TMS6100"] = true
MACHINES["TPI6525"] = true
MACHINES["TTL74145"] = true
MACHINES["UPD1990A"] = true
MACHINES["UPD765"] = true
MACHINES["V3021"] = true
MACHINES["WD_FDC"] = true
MACHINES["WD33C93"] = true
MACHINES["Z80DMA"] = true
MACHINES["Z80PIO"] = true
MACHINES["Z80DART"] = true
MACHINES["Z80CTC"] = true

--------------------------------------------------
-- specify available VIDEOS cores
--------------------------------------------------

VIDEOS["BUFSPRITE"] = true
VIDEOS["DL1416"] = true
VIDEOS["MC6845"] = true
VIDEOS["MOS6566"] = true
VIDEOS["SNES_PPU"] = true

--------------------------------------------------
-- specify available SOUNDS cores
--------------------------------------------------

SOUNDS["AY8910"] = true
SOUNDS["AMIGA"] = true
SOUNDS["BEEP"] = true
SOUNDS["CDDA"] = true
SOUNDS["DAC"] = true
SOUNDS["DISCRETE"] = true
SOUNDS["DMADAC"] = true
SOUNDS["ICS2115"] = true
SOUNDS["MOS7360"] = true
SOUNDS["MOS656X"] = true
SOUNDS["M58817"] = true
SOUNDS["MSM5205"] = true
SOUNDS["NES_APU"] = true
SOUNDS["OKIM6295"] = true
SOUNDS["QSOUND"] = true
SOUNDS["SID6581"] = true
SOUNDS["SID8580"] = true
SOUNDS["SN76477"] = true
SOUNDS["SN76496"] = true
SOUNDS["SP0256"] = true
SOUNDS["SPEAKER"] = true
SOUNDS["T6721A"] = true
SOUNDS["TMS5110"] = true
SOUNDS["VLM5030"] = true
SOUNDS["VRC6"] = true
SOUNDS["WAVE"] = true
SOUNDS["YM2151"] = true
SOUNDS["YM2203"] = true
SOUNDS["YM2413"] = true
SOUNDS["YM2608"] = true
SOUNDS["YM2610"] = true
SOUNDS["YM3526"] = true

dofile("../mame/mess.lua")

function createProjects_mewui_test(_target, _subtarget)
	createProjects_mame_mess(_target, _subtarget)
	project ("mewui_test")
	targetsubdir(_target .."_" .. _subtarget)
	kind "StaticLib"
	uuid (os.uuid("drv-mewui_test"))

	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/devices",
		MAME_DIR .. "src/mame",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "src/lib/netlist",
		MAME_DIR .. "3rdparty",
		GEN_DIR  .. _target .. "/layout",
	}

	files{
		-- Capcom
		MAME_DIR .. "src/mame/drivers/1942.cpp",
		MAME_DIR .. "src/mame/video/1942.cpp",
		MAME_DIR .. "src/mame/drivers/1943.cpp",
		MAME_DIR .. "src/mame/video/1943.cpp",
		MAME_DIR .. "src/mame/drivers/alien.cpp",
		MAME_DIR .. "src/mame/drivers/bionicc.cpp",
		MAME_DIR .. "src/mame/video/bionicc.cpp",
		MAME_DIR .. "src/mame/drivers/supduck.cpp",
		MAME_DIR .. "src/mame/video/tigeroad_spr.cpp",
		MAME_DIR .. "src/mame/drivers/blktiger.cpp",
		MAME_DIR .. "src/mame/video/blktiger.cpp",
		MAME_DIR .. "src/mame/drivers/cbasebal.cpp",
		MAME_DIR .. "src/mame/video/cbasebal.cpp",
		MAME_DIR .. "src/mame/drivers/commando.cpp",
		MAME_DIR .. "src/mame/video/commando.cpp",
		MAME_DIR .. "src/mame/drivers/cps1.cpp",
		MAME_DIR .. "src/mame/video/cps1.cpp",
		MAME_DIR .. "src/mame/drivers/kenseim.cpp",
		MAME_DIR .. "src/mame/drivers/cps2.cpp",
		MAME_DIR .. "src/mame/machine/cps2crpt.cpp",
		MAME_DIR .. "src/mame/drivers/cps3.cpp",
		MAME_DIR .. "src/mame/audio/cps3.cpp",
		MAME_DIR .. "src/mame/drivers/egghunt.cpp",
		MAME_DIR .. "src/mame/drivers/exedexes.cpp",
		MAME_DIR .. "src/mame/video/exedexes.cpp",
		MAME_DIR .. "src/mame/drivers/fcrash.cpp",
		MAME_DIR .. "src/mame/drivers/gng.cpp",
		MAME_DIR .. "src/mame/video/gng.cpp",
		MAME_DIR .. "src/mame/drivers/gunsmoke.cpp",
		MAME_DIR .. "src/mame/video/gunsmoke.cpp",
		MAME_DIR .. "src/mame/drivers/higemaru.cpp",
		MAME_DIR .. "src/mame/video/higemaru.cpp",
		MAME_DIR .. "src/mame/drivers/lastduel.cpp",
		MAME_DIR .. "src/mame/video/lastduel.cpp",
		MAME_DIR .. "src/mame/drivers/lwings.cpp",
		MAME_DIR .. "src/mame/video/lwings.cpp",
		MAME_DIR .. "src/mame/drivers/mitchell.cpp",
		MAME_DIR .. "src/mame/video/mitchell.cpp",
		MAME_DIR .. "src/mame/drivers/sf.cpp",
		MAME_DIR .. "src/mame/video/sf.cpp",
		MAME_DIR .. "src/mame/drivers/sidearms.cpp",
		MAME_DIR .. "src/mame/video/sidearms.cpp",
		MAME_DIR .. "src/mame/drivers/sonson.cpp",
		MAME_DIR .. "src/mame/video/sonson.cpp",
		MAME_DIR .. "src/mame/drivers/srumbler.cpp",
		MAME_DIR .. "src/mame/video/srumbler.cpp",
		MAME_DIR .. "src/mame/drivers/tigeroad.cpp",
		MAME_DIR .. "src/mame/video/tigeroad.cpp",
		MAME_DIR .. "src/mame/machine/tigeroad.cpp",
		MAME_DIR .. "src/mame/drivers/vulgus.cpp",
		MAME_DIR .. "src/mame/video/vulgus.cpp",
		MAME_DIR .. "src/mame/machine/kabuki.cpp",

		-- IGS
		MAME_DIR .. "src/mame/drivers/cabaret.cpp",
		MAME_DIR .. "src/mame/drivers/ddz.cpp",
		MAME_DIR .. "src/mame/drivers/dunhuang.cpp",
		MAME_DIR .. "src/mame/drivers/goldstar.cpp",
		MAME_DIR .. "src/mame/video/goldstar.cpp",
		MAME_DIR .. "src/mame/drivers/jackie.cpp",
		MAME_DIR .. "src/mame/drivers/igspoker.cpp",
		MAME_DIR .. "src/mame/drivers/igs009.cpp",
		MAME_DIR .. "src/mame/drivers/igs011.cpp",
		MAME_DIR .. "src/mame/drivers/igs017.cpp",
		MAME_DIR .. "src/mame/video/igs017_igs031.cpp",
		MAME_DIR .. "src/mame/drivers/igs_fear.cpp",
		MAME_DIR .. "src/mame/drivers/igs_m027.cpp",
		MAME_DIR .. "src/mame/drivers/igs_m036.cpp",
		MAME_DIR .. "src/mame/drivers/iqblock.cpp",
		MAME_DIR .. "src/mame/video/iqblock.cpp",
		MAME_DIR .. "src/mame/drivers/lordgun.cpp",
		MAME_DIR .. "src/mame/video/lordgun.cpp",
		MAME_DIR .. "src/mame/drivers/pgm.cpp",
		MAME_DIR .. "src/mame/video/pgm.cpp",
		MAME_DIR .. "src/mame/machine/pgmprot_igs027a_type1.cpp",
		MAME_DIR .. "src/mame/machine/pgmprot_igs027a_type2.cpp",
		MAME_DIR .. "src/mame/machine/pgmprot_igs027a_type3.cpp",
		MAME_DIR .. "src/mame/machine/pgmprot_igs025_igs012.cpp",
		MAME_DIR .. "src/mame/machine/pgmprot_igs025_igs022.cpp",
		MAME_DIR .. "src/mame/machine/pgmprot_igs025_igs028.cpp",
		MAME_DIR .. "src/mame/machine/pgmprot_orlegend.cpp",
		MAME_DIR .. "src/mame/drivers/pgm2.cpp",
		MAME_DIR .. "src/mame/drivers/spoker.cpp",
		MAME_DIR .. "src/mame/machine/igs036crypt.cpp",
		MAME_DIR .. "src/mame/machine/pgmcrypt.cpp",
		MAME_DIR .. "src/mame/machine/igs025.cpp",
		MAME_DIR .. "src/mame/machine/igs022.cpp",
		MAME_DIR .. "src/mame/machine/igs028.cpp",

		-- Nintendo
		MAME_DIR .. "src/mame/drivers/cham24.cpp",
		MAME_DIR .. "src/mame/drivers/dkong.cpp",
		MAME_DIR .. "src/mame/audio/dkong.cpp",
		MAME_DIR .. "src/mame/video/dkong.cpp",
		MAME_DIR .. "src/mame/drivers/mario.cpp",
		MAME_DIR .. "src/mame/audio/mario.cpp",
		MAME_DIR .. "src/mame/video/mario.cpp",
		MAME_DIR .. "src/mame/drivers/mmagic.cpp",
		MAME_DIR .. "src/mame/drivers/multigam.cpp",
		MAME_DIR .. "src/mame/drivers/n8080.cpp",
		MAME_DIR .. "src/mame/audio/n8080.cpp",
		MAME_DIR .. "src/mame/video/n8080.cpp",
		MAME_DIR .. "src/mame/drivers/nss.cpp",
		MAME_DIR .. "src/mame/machine/snes.cpp",
		MAME_DIR .. "src/mame/audio/snes_snd.cpp",
		MAME_DIR .. "src/mame/drivers/playch10.cpp",
		MAME_DIR .. "src/mame/machine/playch10.cpp",
		MAME_DIR .. "src/mame/video/playch10.cpp",
		MAME_DIR .. "src/mame/drivers/popeye.cpp",
		MAME_DIR .. "src/mame/video/popeye.cpp",
		MAME_DIR .. "src/mame/drivers/punchout.cpp",
		MAME_DIR .. "src/mame/video/punchout.cpp",
		MAME_DIR .. "src/mame/drivers/famibox.cpp",
		MAME_DIR .. "src/mame/drivers/sfcbox.cpp",
		MAME_DIR .. "src/mame/drivers/snesb.cpp",
		MAME_DIR .. "src/mame/drivers/spacefb.cpp",
		MAME_DIR .. "src/mame/audio/spacefb.cpp",
		MAME_DIR .. "src/mame/video/spacefb.cpp",
		MAME_DIR .. "src/mame/drivers/vsnes.cpp",
		MAME_DIR .. "src/mame/machine/vsnes.cpp",
		MAME_DIR .. "src/mame/video/vsnes.cpp",
		MAME_DIR .. "src/mame/video/ppu2c0x.cpp",

		-- Neogeo
		MAME_DIR .. "src/mame/drivers/neogeo.cpp",
		MAME_DIR .. "src/mame/video/neogeo.cpp",
		MAME_DIR .. "src/mame/drivers/neogeo_noslot.cpp",
		MAME_DIR .. "src/mame/video/neogeo_spr.cpp",
		MAME_DIR .. "src/mame/machine/neocrypt.cpp",
		MAME_DIR .. "src/mame/machine/ng_memcard.cpp",

		-- CVS
		MAME_DIR .. "src/mame/drivers/cvs.cpp",
		MAME_DIR .. "src/mame/video/cvs.cpp",
		MAME_DIR .. "src/mame/drivers/galaxia.cpp",
		MAME_DIR .. "src/mame/video/galaxia.cpp",
		MAME_DIR .. "src/mame/drivers/quasar.cpp",
		MAME_DIR .. "src/mame/video/quasar.cpp",
	}
end

function linkProjects_mewui_test(_target, _subtarget)
	linkProjects_mame_mess(_target, _subtarget)
	links {
		"mewui_test",
	}
end
