// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    Atari Bad Lands hardware

    driver by Aaron Giles

    Games supported:
        * Bad Lands (1989)

    Known bugs:
        * none at this time

****************************************************************************

    Memory map

****************************************************************************

    ========================================================================
    MAIN CPU
    ========================================================================
    000000-03FFFF   R     xxxxxxxx xxxxxxxx   Program ROM
    FC0000          R     -------x --------   Sound command buffer full
    FC0000            W   -------- --------   Sound CPU reset
    FD0000-FD1FFF   R/W   -------- xxxxxxxx   EEPROM
    FE0000            W   -------- --------   Watchdog reset
    FE2000            W   -------- --------   VBLANK IRQ acknowledge
    FE4000          R     -------- xxxx----   Switch inputs
                    R     -------- x-------      (Self test)
                    R     -------- -x------      (VBLANK)
                    R     -------- --x-----      (Player 2 button)
                    R     -------- ---x----      (Player 1 button)
    FE6000          R     -------- xxxxxxxx   Player 1 steering
    FE6002          R     -------- xxxxxxxx   Player 2 steering
    FE6004          R     -------- xxxxxxxx   Player 1 pedal
    FE6006          R     -------- xxxxxxxx   Player 2 pedal
    FE8000            W   xxxxxxxx --------   Sound command write
    FEA000          R     xxxxxxxx --------   Sound response read
    FEC000            W   -------- -------x   Playfield tile bank select
    FEE000            W   -------- --------   EEPROM enable
    FFC000-FFC0FF   R/W   xxxxxxxx xxxxxxxx   Playfield palette RAM (128 entries)
                    R/W   x------- --------      (RGB 1 LSB)
                    R/W   -xxxxx-- --------      (Red 5 MSB)
                    R/W   ------xx xxx-----      (Green 5 MSB)
                    R/W   -------- ---xxxxx      (Blue 5 MSB)
    FFC100-FFC1FF   R/W   xxxxxxxx xxxxxxxx   Motion object palette RAM (128 entries)
    FFC200-FFC3FF   R/W   xxxxxxxx xxxxxxxx   Extra palette RAM (256 entries)
    FFE000-FFEFFF   R/W   xxxxxxxx xxxxxxxx   Playfield RAM (64x32 tiles)
                    R/W   xxx----- --------      (Palette select)
                    R/W   ---x---- --------      (Tile bank select)
                    R/W   ----xxxx xxxxxxxx      (Tile index)
    FFF000-FFFFFF   R/W   xxxxxxxx xxxxxxxx   Motion object RAM (32 entries x 4 words)
                    R/W   ----xxxx xxxxxxxx      (0: Tile index)
                    R/W   xxxxxxxx x-------      (1: Y position)
                    R/W   -------- ----xxxx      (1: Number of Y tiles - 1)
                    R/W   xxxxxxxx x-------      (3: X position)
                    R/W   -------- ----x---      (3: Priority)
                    R/W   -------- -----xxx      (3: Palette select)
    ========================================================================
    Interrupts:
        IRQ1 = VBLANK
        IRQ2 = sound CPU communications
    ========================================================================


    ========================================================================
    SOUND CPU (based on JSA II, but implemented onboard)
    ========================================================================
    0000-1FFF   R/W   xxxxxxxx   Program RAM
    2000-2001   R/W   xxxxxxxx   YM2151 communications
    2802        R     xxxxxxxx   Sound command read
    2804        R     xxxx--xx   Status input
                R     x-------      (Self test)
                R     -x------      (Sound command buffer full)
                R     --x-----      (Sound response buffer full)
                R     ---x----      (Self test)
                R     ------xx      (Coin inputs)
    2806        R/W   --------   IRQ acknowledge
    2A02          W   xxxxxxxx   Sound response write
    2A04          W   xxxx---x   Sound control
                  W   xx------      (ROM bank select)
                  W   --xx----      (Coin counters)
                  W   -------x      (YM2151 reset)
    3000-3FFF   R     xxxxxxxx   Banked ROM
    4000-FFFF   R     xxxxxxxx   Program ROM
    ========================================================================
    Interrupts:
        IRQ = timed interrupt ORed with YM2151 interrupt
        NMI = latch on sound command
    ========================================================================


Badlands
Atari Games, 1989

PCB Layout
----------

A047193-01 BADLANDS
|-------------------------------------------------|
|TDA2030 LM324                ROM.2N ROM.2S ROM.2U|
|VOL     LM324                ROM.4N ROM.4S ROM.4U|
|6264    YM3012                                   |
|      YM2151          6116     PAL3              |
|   ROM.9C   PAL2                 ROM.10R  10T    |
|J    6502                        ROM.14R  ROM.14T|
|A                            MB87316             |
|M                     14.31818MHz    PAL4  VMA   |
|M                                                |
|A    VGC7205      ROM.17F          PAL5          |
|  TEST_SW   68000 ROM.20F      SOS               |
| NE556 PAL1       ROM.24F          PAL6     6264 |
|LED    DIP20      ROM.27F 2816                   |
|-------------------------------------------------|
Notes:
      68000  - clock 7.15909MHz [14.31818/2]
      6502   - clock 1.7897725 [14.31818/8]
      YM2151 - clock 3.579545MHz [14.31818/4]
      VGC7205- VLSI VGC7205A0672 ATARI 137304-2002
      2816   - 2k x8 EEPROM
      DIP20  - socket not populated
      SOS    - Motorola (rebadged) SOS-137550-001 ZKZKAA8946
      VMA    - Motorola (rebadged) VMA-137552-001 ZKZKAB8948
      MB87316- Fujitsu MB87316 137536-001 with clock input 7.15909MHz
               Listed in Jed Margolin's Atari Approved Vendor List
               (http://www.jmargolin.com/schem/atariavl.htm) as.....
               137536-001 IC,Line_Buffer(LB),CMOS,672B_RAM,500_Gate,40pin FUJITSU MB87316

      ROMs   -
               location  label
               ------------------------
               9C        136074-1018 E1FF
               17F       136074-1009 86DF
               20F       136074-1008 87FF
               24F       136074-1007 F2DE
               27F       136074-1006 98FE
               2N        136074-1013 A5A0
               2S        136074-1014 567B
               2U        136074-1017 BB88
               4N        136074-1012 6B70
               4S        136074-1014 4475
               4U        136074-1016 42A5
               10R       136074-1011 041C
               10T       not populated
               14R       136074-1010 5E67
               14T       136074-1019 D4C4

      PALs   -
               PAL1 - Lattice GAL16V8A-25LP labelled '136074-1001'
               PAL2 - Lattice GAL16V8A-25LP labelled '136074-1005'
               PAL3 - Lattice GAL16V8A-25LP labelled '136074-1004'
               PAL4 - Lattice GAL16V8A-25LP labelled '136074-1003'
               PAL5 - Lattice GAL16V8A-25LP labelled '136074-1002'
               PAL6 - Lattice GAL16V8A-25LP labelled '136074-2000'

Measurements -
              X1    - 14.31995MHz
              VSync - 59.9310Hz
              HSync - 15.4611kHz

****************************************************************************/


#include "emu.h"
#include "includes/badlands.h"
#include "emupal.h"


/*************************************
 *
 *  Initialization
 *
 *************************************/

TIMER_DEVICE_CALLBACK_MEMBER(badlands_state::sound_scanline)
{
	int scanline = param;
	//address_space &space = m_audiocpu->space(AS_PROGRAM);

	// 32V
	if ((scanline % 64) == 0 && scanline < 240)
		m_soundcomm->sound_irq_gen(*m_audiocpu);
}


MACHINE_START_MEMBER(badlands_state,badlands)
{
	save_item(NAME(m_pedal_value));
}


MACHINE_RESET_MEMBER(badlands_state,badlands)
{
	m_pedal_value[0] = m_pedal_value[1] = 0x80;

	//scanline_timer_reset(*m_screen, 32);

	membank("soundbank")->set_entry(0);
}



/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

INTERRUPT_GEN_MEMBER(badlands_state::vblank_int)
{
	// TODO: remove this hack
	int pedal_state = ioport("PEDALS")->read();
	int i;

	/* update the pedals once per frame */
	for (i = 0; i < 2; i++)
	{
		m_pedal_value[i]--;
		if (pedal_state & (1 << i))
			m_pedal_value[i]++;
	}

	m_maincpu->set_input_line(M68K_IRQ_1, ASSERT_LINE);
}

WRITE16_MEMBER(badlands_state::video_int_ack_w)
{
	m_maincpu->set_input_line(M68K_IRQ_1, CLEAR_LINE);
}



/*************************************
 *
 *  I/O read dispatch
 *
 *************************************/

READ16_MEMBER(badlands_state::sound_busy_r)
{
	int temp = 0xfeff;
	if (m_soundcomm->main_to_sound_ready()) temp ^= 0x0100;
	return temp;
}


READ16_MEMBER(badlands_state::pedal_0_r)
{
	return m_pedal_value[0];
}


READ16_MEMBER(badlands_state::pedal_1_r)
{
	return m_pedal_value[1];
}


/*************************************
 *
 *  Audio I/O handlers
 *
 *************************************/

READ8_MEMBER(badlands_state::audio_io_r)
{
	int result = 0xff;

	switch (offset & 0x206)
	{
		case 0x000:     /* n/c */
			logerror("audio_io_r: Unknown read at %04X\n", offset & 0x206);
			break;

		case 0x002:     /* /RDP */
			result = m_soundcomm->sound_command_r();
			break;

		case 0x004:     /* /RDIO */
			/*
			    0x80 = self test
			    0x40 = NMI line state (active low)
			    0x20 = sound output full
			    0x10 = self test
			    0x08 = +5V
			    0x04 = +5V
			    0x02 = coin 2
			    0x01 = coin 1
			*/
			result = ioport("AUDIO")->read();
			if (!(ioport("FE4000")->read() & 0x0080)) result ^= 0x90;
			result ^= 0x10;
			break;

		case 0x006:     /* /IRQACK */
			m_soundcomm->sound_irq_ack_r();
			break;

		case 0x200:     /* /VOICE */
		case 0x202:     /* /WRP */
		case 0x204:     /* /WRIO */
		case 0x206:     /* /MIX */
			logerror("audio_io_r: Unknown read at %04X\n", offset & 0x206);
			break;
	}

	return result;
}


WRITE8_MEMBER(badlands_state::audio_io_w)
{
	switch (offset & 0x206)
	{
		case 0x000:     /* n/c */
		case 0x002:     /* /RDP */
		case 0x004:     /* /RDIO */
			logerror("audio_io_w: Unknown write (%02X) at %04X\n", data & 0xff, offset & 0x206);
			break;

		case 0x006:     /* /IRQACK */
			m_soundcomm->sound_irq_ack_w();
			break;

		case 0x200:     /* n/c */
		case 0x206:     /* n/c */
			break;

		case 0x202:     /* /WRP */
			m_soundcomm->sound_response_w(data);
			break;

		case 0x204:     /* WRIO */
			/*
			    0xc0 = bank address
			    0x20 = coin counter 1
			    0x10 = coin counter 2
			    0x08 = n/c
			    0x04 = n/c
			    0x02 = n/c
			    0x01 = YM2151 reset (active low)
			*/

			/* update the bank */
			membank("soundbank")->set_entry((data >> 6) & 3);
			machine().bookkeeping().coin_counter_w(0, data & 0x20);
			machine().bookkeeping().coin_counter_w(1, data & 0x10);
			break;
	}
}



/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

void badlands_state::main_map(address_map &map)
{
	map(0x000000, 0x03ffff).rom();
	map(0xfc0000, 0xfc1fff).r(FUNC(badlands_state::sound_busy_r)).w(m_soundcomm, FUNC(atari_sound_comm_device::sound_reset_w));
	map(0xfd0000, 0xfd1fff).rw("eeprom", FUNC(eeprom_parallel_28xx_device::read), FUNC(eeprom_parallel_28xx_device::write)).umask16(0x00ff);
	map(0xfe0000, 0xfe1fff).w("watchdog", FUNC(watchdog_timer_device::reset16_w));
	map(0xfe2000, 0xfe3fff).w(FUNC(badlands_state::video_int_ack_w));
	map(0xfe4000, 0xfe5fff).portr("FE4000");
	map(0xfe6000, 0xfe6001).portr("FE6000");
	map(0xfe6002, 0xfe6003).portr("FE6002");
	map(0xfe6004, 0xfe6005).r(FUNC(badlands_state::pedal_0_r));
	map(0xfe6006, 0xfe6007).r(FUNC(badlands_state::pedal_1_r));
	map(0xfe8000, 0xfe9fff).w(m_soundcomm, FUNC(atari_sound_comm_device::main_command_w)).umask16(0xff00);
	map(0xfea000, 0xfebfff).r(m_soundcomm, FUNC(atari_sound_comm_device::main_response_r)).umask16(0xff00);
	map(0xfec000, 0xfedfff).w(FUNC(badlands_state::badlands_pf_bank_w));
	map(0xfee000, 0xfeffff).w("eeprom", FUNC(eeprom_parallel_28xx_device::unlock_write16));
	map(0xffc000, 0xffc3ff).rw("palette", FUNC(palette_device::read8), FUNC(palette_device::write8)).umask16(0xff00).share("palette");
	map(0xffe000, 0xffefff).ram().w(m_playfield_tilemap, FUNC(tilemap_device::write16)).share("playfield");
	map(0xfff000, 0xfff1ff).ram().share("mob");
	map(0xfff200, 0xffffff).ram();
}



/*************************************
 *
 *  Sound CPU memory handlers
 *
 *************************************/

void badlands_state::audio_map(address_map &map)
{
	map(0x0000, 0x1fff).ram();
	map(0x2000, 0x2001).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0x2800, 0x2bff).rw(FUNC(badlands_state::audio_io_r), FUNC(badlands_state::audio_io_w));
	map(0x3000, 0x3fff).bankr("soundbank");
	map(0x4000, 0xffff).rom();
}


/*************************************
 *
 *  Graphics definitions
 *
 *************************************/

static const gfx_layout pflayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0, 4, 8, 12, 16, 20, 24, 28 },
	{ 0*8, 4*8, 8*8, 12*8, 16*8, 20*8, 24*8, 28*8 },
	32*8
};

static const gfx_layout badlands_molayout =
{
	16,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60 },
	{ 0*8, 8*8, 16*8, 24*8, 32*8, 40*8, 48*8, 56*8 },
	64*8
};

static GFXDECODE_START( gfx_badlands )
	GFXDECODE_ENTRY( "gfx1", 0, pflayout,           0, 8 )
	GFXDECODE_ENTRY( "gfx2", 0, badlands_molayout,  128, 8 )
GFXDECODE_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

void badlands_state::badlands(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, ATARI_CLOCK_14MHz/2);
	m_maincpu->set_addrmap(AS_PROGRAM, &badlands_state::main_map);
	m_maincpu->set_vblank_int("screen", FUNC(badlands_state::vblank_int));

	M6502(config, m_audiocpu, ATARI_CLOCK_14MHz/8);
	m_audiocpu->set_addrmap(AS_PROGRAM, &badlands_state::audio_map);
	TIMER(config, "scantimer").configure_scanline(FUNC(badlands_state::sound_scanline), "screen", 0, 1);

	MCFG_MACHINE_START_OVERRIDE(badlands_state,badlands)
	MCFG_MACHINE_RESET_OVERRIDE(badlands_state,badlands)

	EEPROM_2816(config, "eeprom").lock_after_write(true);

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	GFXDECODE(config, m_gfxdecode, "palette", gfx_badlands);
	palette_device &palette(PALETTE(config, "palette"));
	palette.set_format(palette_device::IRGB_1555, 256);
	palette.set_membits(8);

	TILEMAP(config, m_playfield_tilemap, m_gfxdecode, 2, 8,8, TILEMAP_SCAN_ROWS, 64,32).set_info_callback(FUNC(badlands_state::get_playfield_tile_info));

	ATARI_MOTION_OBJECTS(config, m_mob, 0, m_screen, badlands_state::s_mob_config);
	m_mob->set_gfxdecode(m_gfxdecode);

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	/* note: these parameters are from published specs, not derived */
	/* the board uses an SOS-2 chip to generate video signals */
	m_screen->set_raw(ATARI_CLOCK_14MHz/2, 456, 0, 336, 262, 0, 240);
	m_screen->set_screen_update(FUNC(badlands_state::screen_update_badlands));
	m_screen->set_palette("palette");

	MCFG_VIDEO_START_OVERRIDE(badlands_state,badlands)

	/* sound hardware */
	ATARI_SOUND_COMM(config, "soundcomm", "audiocpu")
		.int_callback().set_inputline("maincpu", M68K_IRQ_2);
	SPEAKER(config, "mono").front_center();

	YM2151(config, "ymsnd", ATARI_CLOCK_14MHz/4).add_route(0, "mono", 0.30).add_route(1, "mono", 0.30);
}



/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( badlands )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 4*64k for 68000 code */
	ROM_LOAD16_BYTE( "136074-1008.20f",  0x00000, 0x10000, CRC(a3da5774) SHA1(5ab1eb61d25594b2d7c40400cb57e7f47a717598) )
	ROM_LOAD16_BYTE( "136074-1006.27f",  0x00001, 0x10000, CRC(aa03b4f3) SHA1(5eda60c715ffcefd4ad34bdb90579e8671dc384a) )
	ROM_LOAD16_BYTE( "136074-1009.17f",  0x20000, 0x10000, CRC(0e2e807f) SHA1(5b61de066dca12c44335aa68a13c821845657866) )
	ROM_LOAD16_BYTE( "136074-1007.24f",  0x20001, 0x10000, CRC(99a20c2c) SHA1(9b0a5a5dafb8816e72330d302c60339b600b49a8) )

	ROM_REGION( 0x10000, "audiocpu", 0 )    /* 64k for 6502 code */
	ROM_LOAD( "136074-1018.9c", 0x00000, 0x10000, CRC(a05fd146) SHA1(d97abbcf7897ca720cc18ff3a323f41cd3b23c34) )

	ROM_REGION( 0x60000, "gfx1", ROMREGION_INVERT )
	ROM_LOAD( "136074-1012.4n",  0x000000, 0x10000, CRC(5d124c6c) SHA1(afebaaf90b3751f5e873fc4c45f1d5385ef86a6e) )  /* playfield */
	ROM_LOAD( "136074-1013.2n",  0x010000, 0x10000, CRC(b1ec90d6) SHA1(8d4c7db8e1bf9c050f5869eb38fa573867fdc12b) )
	ROM_LOAD( "136074-1014.4s",  0x020000, 0x10000, CRC(248a6845) SHA1(086ef0840b889e790ce3fcd09f98589aae932456) )
	ROM_LOAD( "136074-1015.2s",  0x030000, 0x10000, CRC(792296d8) SHA1(833cdb968064151ca77bb3dbe416ff7127a12de4) )
	ROM_LOAD( "136074-1016.4u",  0x040000, 0x10000, CRC(878f7c66) SHA1(31159bea5d6aac8100fca8f3860220b97d63e72e) )
	ROM_LOAD( "136074-1017.2u",  0x050000, 0x10000, CRC(ad0071a3) SHA1(472b197e5d320b3424d8a8d8c051b1023a07ae08) )

	ROM_REGION( 0x30000, "gfx2", ROMREGION_INVERT )
	ROM_LOAD( "136074-1010.14r", 0x000000, 0x10000, CRC(c15f629e) SHA1(944e3479dce6e420cf9a3f4c1438c5ca66e5cb97) )  /* mo */
	ROM_LOAD( "136074-1011.10r", 0x010000, 0x10000, CRC(fb0b6717) SHA1(694ab0f04d673682831a24027757d4b3c40a4e0e) )
	ROM_LOAD( "136074-1019.14t", 0x020000, 0x10000, CRC(0e26bff6) SHA1(ee018dd37a27c7e7c16a57ea0d32aeb9cdf26bb4) )

	ROM_REGION( 0x0C00, "plds", 0 ) /* GAL16V8A-25LP */
	ROM_LOAD( "136074-1001.26c",  0x0000, 0x0117, CRC(04c3be6a) SHA1(f027834e652f3ff778b09c3754294b303f9ed826) )
	ROM_LOAD( "136074-1002.21r",  0x0200, 0x0117, CRC(f68bf41d) SHA1(72edd6d0f5d55d39c0020f384149de7ac964f273) )
	ROM_LOAD( "136074-1003.16s",  0x0400, 0x0117, CRC(a288bbd0) SHA1(62f5900ac88ffb335257f58d892492f370805498) )
	ROM_LOAD( "136074-1004.9n",   0x0600, 0x0117, CRC(5ffbdaad) SHA1(f7f802dfb7c9b404305a36b8354f91151e61c502) )
	ROM_LOAD( "136074-1005.12e",  0x0800, 0x0117, CRC(9df77c79) SHA1(52c1c190b80db9b9bc43ce6eefd5f37ac16e590c) )
	ROM_LOAD( "136074-2000.26r",  0x0A00, 0x0117, CRC(fb8fb3d0) SHA1(361b8f7984695ff26156afe79eaa2d85a150a978) )
ROM_END



/*************************************
 *
 *  Driver initialization
 *
 *************************************/

void badlands_state::init_badlands()
{
	/* initialize the audio system */
	membank("soundbank")->configure_entries(0, 4, memregion("audiocpu")->base(), 0x01000);
}



/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1989, badlands, 0, badlands, badlands, badlands_state, init_badlands, ROT0, "Atari Games", "Bad Lands", MACHINE_SUPPORTS_SAVE )
