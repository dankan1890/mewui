// license:BSD-3-Clause
// copyright-holders:R. Belmont, ElSemi
/*
    Namco System FL
    Driver by R. Belmont and ElSemi

    IMPORTANT (for Manual Calibration, MAME now loads these defaults instead)
    ---------
    To calibrate the steering, do the following:
    1) Delete the .nv file
    2) Start the game.  Speed Racer will guide you through the calibration
       (the "jump button" is mapped to Space).
       For Final Lap R, hold down service (9) and tap test (F2).  If you do
       not get an "initializing" message followed by the input test, keep doing
       it until you do.
    3) Exit MAME and restart the game, it's now calibrated.


PCB Layout
----------

SYSTEM-FL MAIN PCB 8636960100 (8636970100)
       |--------------------------------------------------------------------------------------------|
       |                                                                       |-----------------|  |
       |                                                                       |       4Z IC3 IC1| Z|
       |                21Y 20Y 19Y 18Y     16Y     14Y 13Y 12Y                |       4Y        | Y|
       |                    20X                                                |       4X        | X|
    |--|                                                                       |          IC4 IC2| W|
    |               22U 21U         18U     16U     14U 13U 12U                |    Sub Board    | U|
    |               22T             18T                                        |-----------------| T|
    |                                                                                               |
    |J          23S                                         12S                        4S    2S    S|
    |                                                                                               |
    |A          23R                                     13R                                        R|
    |                                                                                               |
    |M                  21P 20P 19P 18P                             10P    8P 7P    5P             P|
    |                       20N                                                               OSC1 N|
    |M                                                                                    OSC2      |
    |           23L     21L                             OSC3                              3L       L|
    |A                                              14K         11K                                K|
    |                                                                                               |
    |                           19J 18J 17J 16J                                                    J|
    |--|        23F     21F                                                                        F|
       |                                                                                            |
    |--|        23E                                     13E                                        E|
    |N          23D             19D                     13D                               3D       D|
    |A                                                                                              |
    |M          23C                                                                                C|
    |C  25B             21B                                                                        B|
    |O                                                                                              |
    |4              22A 21A     19A 18A     16A 15A 14A 13A 12A 11A 10A 9A 8A 7A 6A 5A    3A       A|
    |4                                                                                              |
    |--|                                                                                            |
       |25  24  23  22  21  20  19  18  17  16  15  14  13  12  11  10  9  8  7  6  5  4  3  2  1   |
       |--------------------------------------------------------------------------------------------|

RAM
---
4Y : CY7C199-25PC
4Z : CY7C199-25PC
5A : TC514256BZ-60
5P : LH528256N-70LL
6A : TC514256BZ-60
7A : TC514256BZ-60
7P : LH528256N-70LL
8A : TC514256BZ-60
8P : LH528256N-70LL
9A : TC514256BZ-60
10A: TC514256BZ-60
10P: LH528256N-70LL
11A: TC514256BZ-60
12A: TC514256BZ-60
12U: TC511632FL-70
12Y: TC511632FL-70
13U: TC511632FL-70
13Y: TC511632FL-70
14U: TC511632FL-70
14Y: TC511632FL-70
16U: TC511632FL-70
16Y: TC511632FL-70
19Y: M5M5178AFP-25
20Y: M5M5178AFP-25
21A: 65256BLFP-10T
21Y: M5M5178AFP-25
22A: 65256BLFP-10T
22T: LH528256N-70LL
22U: LH528256N-70LL
23E: M5M5256BFP-70LL
23F: M5M5256BFP-70LL

CUSTOM
------
4X : NAMCO C355 (sprite chip)
18Y: NAMCO 156
20X: NAMCO C116
21U: NAMCO 145
18T: NAMCO 123
23R: NAMCO C352
3D : NAMCO C380
4S : NAMCO 187
23L: NAMCO 75 (M37702 MCU)
11K: NAMCO 137
21B: NAMCO C345  9348EV  333791  VY06436A  NX25024K JAPAN
21F: NAMCO 195


ROMs - Main Board
-----------------
23S: mask ROM - SE1_VOI.23S (PCB LABEL 'VOICE'), mounted on a small plug-in PCB
     labelled MEMEXT 32M MROM PCB 8635909200 (8635909300). This chip is programmed in BYTE mode.
18U: MB834000 mask ROM - SE1_SSH.18U (PCB LABEL 'SSHAPE')
21P: MB838000 mask ROM - SE1_SCH0.21P (PCB LABEL 'SCHA0')
20P: MB838000 mask ROM - SE1_SCH1.20P (PCB LABEL 'SCHA1')
19P: MB838000 mask ROM - SE1_SCH2.19P (PCB LABEL 'SCHA2')
18P: MB838000 mask ROM - SE1_SCH3.18P (PCB LABEL 'SCHA3')
21L: M27C4002 EPROM - SE1_SPR.21L (PCB LABEL 'SPROG')
14K: MB834000 mask ROM - SE1_RSH.14K (PCB LABEL 'RSHAPE')
19J: MB838000 mask ROM - SE1_RCH0.19J (PCB LABEL 'RCHA0')
18J: MB838000 mask ROM - SE1_RCH1.18J (PCB LABEL 'RCHA1')
17J, 16J: RCH2, RCH3 but sockets not populated
19A: D27C4096 EPROM - SE2MPEA4.19A (PCB LABEL 'PROGE')
18A: D27C4096 EPROM - SE2MPOA4.18A (PCB LABEL 'PROGO')
16A: AM27C040 EPROM - SE1_DAT3.16A (PCB LABEL 'DATA3')
15A: AM27C040 EPROM - SE1_DAT2.15A (PCB LABEL 'DATA2')
14A: AM27C040 EPROM - SE1_DAT1.14A (PCB LABEL 'DATA1')
13A: AM27C040 EPROM - SE1_DAT0.13A (PCB LABEL 'DATA0')


ROMs - Sub Board
----------------
IC1: MB8316200 SOP44 mask ROM - SE1OBJ0L.IC1 (PCB LABEL 'OBJ0L')
IC2: MB8316200 SOP44 mask ROM - SE1OBJ0U.IC2 (PCB LABEL 'OBJ0U')
IC3: MB8316200 SOP44 mask ROM - SE1OBJ1L.IC3 (PCB LABEL 'OBJ1L')
IC4: MB8316200 SOP44 mask ROM - SE1OBJ1U.IC4 (PCB LABEL 'OBJ1U')


PALs
----
2S : PAL16L8BCN (PCB LABEL 'SYSFL-1')
3L : PAL16L8BCN (PCB LABEL 'SYSFL-2')
12S: PALCE16V8H (PCB LABEL 'SYSFL-3')
20N: PAL20L8BCN (PCB LABEL 'SYSFL-4')
19D: PALCE16V8H (PCB LABEL 'SYSFL-5')


OTHER
-----
OSC1: 80.000MHz
OSC2: 27.800MHz
OSC3: 48.384MHz
13D : KEYCUS2 (not populated)
3A  : Intel NG80960KA-20 (i960 CPU)
25B : Sharp PC9D10
23C : IR2C24AN
23D : IR2C24AN
13E : HN58C65 EEPROM

*/

#include "emu.h"
#include "includes/namcofl.h"

#include "cpu/i960/i960.h"
#include "sound/c352.h"
#include "machine/nvram.h"
#include "speaker.h"

#include "namcofl.lh"


READ32_MEMBER(namcofl_state::fl_unk1_r)
{
	return 0xffffffff;
}

READ32_MEMBER(namcofl_state::fl_network_r)
{
	return 0xffffffff;
}

READ32_MEMBER(namcofl_state::namcofl_sysreg_r)
{
	return 0;
}

WRITE32_MEMBER(namcofl_state::namcofl_sysreg_w)
{
	if ((offset == 2) && ACCESSING_BITS_0_7)  // address space configuration
	{
		if (data == 0)  // RAM at 00000000, ROM at 10000000
		{
			membank("bank1")->set_entry(1);
			membank("bank2")->set_entry(1);
		}
		else        // ROM at 00000000, RAM at 10000000
		{
			membank("bank1")->set_entry(0);
			membank("bank2")->set_entry(0);
		}
	}
}

// FIXME: remove this trampoline once the IRQ is moved into the actual device
WRITE8_MEMBER(namcofl_state::namcofl_c116_w)
{
	m_c116->write(space, offset, data);

	if ((offset & 0x180e) == 0x180a)
	{
		uint16_t triggerscanline=m_c116->get_reg(5)-(32+1);
		m_raster_interrupt_timer->adjust(m_screen->time_until_pos(triggerscanline));
	}
}

void namcofl_state::namcofl_mem(address_map &map)
{
	map(0x00000000, 0x000fffff).bankrw("bank1");
	map(0x10000000, 0x100fffff).bankrw("bank2");
	map(0x20000000, 0x201fffff).rom().region("data", 0);
	map(0x30000000, 0x30001fff).ram().share("nvram"); /* nvram */
	map(0x30100000, 0x30100003).w(FUNC(namcofl_state::namcofl_spritebank_w));
	map(0x30284000, 0x3028bfff).ram().share("shareram");
	map(0x30300000, 0x30303fff).ram(); /* COMRAM */
	map(0x30380000, 0x303800ff).r(FUNC(namcofl_state::fl_network_r)); /* network registers */
	map(0x30400000, 0x30407fff).r(m_c116, FUNC(namco_c116_device::read)).w(FUNC(namcofl_state::namcofl_c116_w));
	map(0x30800000, 0x3080ffff).rw(m_c123tmap, FUNC(namco_c123tmap_device::videoram16_r), FUNC(namco_c123tmap_device::videoram16_w));
	map(0x30a00000, 0x30a0003f).rw(m_c123tmap, FUNC(namco_c123tmap_device::control16_r), FUNC(namco_c123tmap_device::control16_w));
	map(0x30c00000, 0x30c1ffff).rw(m_c169roz, FUNC(namco_c169roz_device::videoram_r), FUNC(namco_c169roz_device::videoram_w));
	map(0x30d00000, 0x30d0001f).rw(m_c169roz, FUNC(namco_c169roz_device::control_r), FUNC(namco_c169roz_device::control_w));
	map(0x30e00000, 0x30e1ffff).rw(m_c355spr, FUNC(namco_c355spr_device::spriteram_r), FUNC(namco_c355spr_device::spriteram_w)).share("objram");
	map(0x30f00000, 0x30f0000f).ram(); /* NebulaM2 code says this is int enable at 0000, int request at 0004, but doesn't do much about it */
	map(0x40000000, 0x4000005f).rw(FUNC(namcofl_state::namcofl_sysreg_r), FUNC(namcofl_state::namcofl_sysreg_w));
	map(0xfffffffc, 0xffffffff).r(FUNC(namcofl_state::fl_unk1_r));
}


WRITE16_MEMBER(namcofl_state::mcu_shared_w)
{
	// HACK!  Many games data ROM routines redirect the vector from the sound command read to an RTS.
	// This needs more investigation.  nebulray and vshoot do NOT do this.
	// Timers A2 and A3 are set up in "external input counter" mode, this may be related.
#if 0
	if ((offset == 0x647c/2) && (data != 0))
	{
		data = 0xd2f6;
	}
#endif

	COMBINE_DATA(&m_shareram[offset]);

	// C75 BIOS has a very short window on the CPU sync signal, so immediately let the i960 at it
	if ((offset == 0x6000/2) && (data & 0x80))
	{
		m_mcu->yield();
	}
}


READ8_MEMBER(namcofl_state::port6_r)
{
	return m_mcu_port6;
}

WRITE8_MEMBER(namcofl_state::port6_w)
{
	m_mcu_port6 = data;
}

READ8_MEMBER(namcofl_state::port7_r)
{
	switch (m_mcu_port6 & 0xf0)
	{
		case 0x00:
			return m_in0->read();

		case 0x20:
			return m_misc->read();

		case 0x40:
			return m_in1->read();

		case 0x60:
			return m_in2->read();

		default:
			break;
	}

	return 0xff;
}

READ8_MEMBER(namcofl_state::dac7_r)
{
	return m_accel.read_safe(0xff);
}

READ8_MEMBER(namcofl_state::dac6_r)
{
	return m_brake.read_safe(0xff);
}

READ8_MEMBER(namcofl_state::dac5_r)
{
	return m_wheel.read_safe(0xff);
}

READ8_MEMBER(namcofl_state::dac4_r){ return 0xff; }
READ8_MEMBER(namcofl_state::dac3_r){ return 0xff; }
READ8_MEMBER(namcofl_state::dac2_r){ return 0xff; }
READ8_MEMBER(namcofl_state::dac1_r){ return 0xff; }
READ8_MEMBER(namcofl_state::dac0_r){ return 0xff; }

void namcofl_state::namcoc75_am(address_map &map)
{
	map(0x002000, 0x002fff).rw("c352", FUNC(c352_device::read), FUNC(c352_device::write));
	map(0x004000, 0x00bfff).ram().w(FUNC(namcofl_state::mcu_shared_w)).share("shareram");
	map(0x200000, 0x27ffff).rom().region("c75data", 0);
}


static INPUT_PORTS_START( speedrcr )
	PORT_START("MISC")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE1 ) // enters pages in test menu

	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN ) // no gear button on this
	PORT_DIPNAME( 0x20, 0x20, "Freeze Screen" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN ) // buttons are named because their use in navigating test mode isn't clear
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Weapon 1 / Test Menu Down") PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Weapon 2 / Test Menu Modify") PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Weapon 3 / Test Menu Up") PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Start / Jump") PORT_CODE(KEYCODE_SPACE) // jump / start button

	PORT_START("IN3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("ACCEL")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_START("BRAKE")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED ) // no brake

	PORT_START("WHEEL")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(100) PORT_KEYDELTA(4)
INPUT_PORTS_END


static INPUT_PORTS_START( finalapr )
	PORT_START("MISC")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_SERVICE( 0x40, IP_ACTIVE_LOW )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE1 )

	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Gear Change") PORT_CODE(KEYCODE_LSHIFT) PORT_TOGGLE // gear
	PORT_DIPNAME( 0x20, 0x20, "Freeze Screen" )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN ) // freezes the game

	PORT_START("IN3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("ACCEL")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(6)

	PORT_START("BRAKE")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(6)

	PORT_START("WHEEL")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(100) PORT_KEYDELTA(6)
INPUT_PORTS_END


TIMER_CALLBACK_MEMBER(namcofl_state::network_interrupt_callback)
{
	m_maincpu->set_input_line(I960_IRQ0, ASSERT_LINE);
	m_network_interrupt_timer->adjust(m_screen->frame_period());
}


TIMER_CALLBACK_MEMBER(namcofl_state::vblank_interrupt_callback)
{
	m_maincpu->set_input_line(I960_IRQ2, ASSERT_LINE);
	m_vblank_interrupt_timer->adjust(m_screen->frame_period());
}


TIMER_CALLBACK_MEMBER(namcofl_state::raster_interrupt_callback)
{
	m_screen->update_partial(m_screen->vpos());
	m_maincpu->set_input_line(I960_IRQ1, ASSERT_LINE);
	m_raster_interrupt_timer->adjust(m_screen->frame_period());
}

TIMER_DEVICE_CALLBACK_MEMBER(namcofl_state::mcu_irq0_cb)
{
	m_mcu->set_input_line(M37710_LINE_IRQ0, HOLD_LINE);
}

TIMER_DEVICE_CALLBACK_MEMBER(namcofl_state::mcu_irq2_cb)
{
	m_mcu->set_input_line(M37710_LINE_IRQ2, HOLD_LINE);
}

TIMER_DEVICE_CALLBACK_MEMBER(namcofl_state::mcu_adc_cb)
{
	m_mcu->set_input_line(M37710_LINE_ADC, HOLD_LINE);
}


MACHINE_START_MEMBER(namcofl_state,namcofl)
{
	m_raster_interrupt_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(namcofl_state::raster_interrupt_callback),this));
	m_network_interrupt_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(namcofl_state::network_interrupt_callback),this));
	m_vblank_interrupt_timer =  machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(namcofl_state::vblank_interrupt_callback),this));

	membank("bank1")->configure_entry(0, memregion("maincpu")->base());
	membank("bank1")->configure_entry(1, m_workram.get());
	membank("bank1")->set_entry(0);

	membank("bank2")->configure_entry(0, m_workram.get());
	membank("bank2")->configure_entry(1, memregion("maincpu")->base());
	membank("bank2")->set_entry(0);
}


MACHINE_RESET_MEMBER(namcofl_state,namcofl)
{
	m_network_interrupt_timer->adjust(m_screen->time_until_pos(m_screen->visible_area().max_y + 3));
	m_vblank_interrupt_timer->adjust(m_screen->time_until_pos(m_screen->visible_area().max_y + 1));

	memset(m_workram.get(), 0x00, 0x100000);

	membank("bank1")->set_entry(0);
	membank("bank2")->set_entry(0);
}


void namcofl_state::namcofl(machine_config &config)
{
	I960(config, m_maincpu, 80_MHz_XTAL/4); // i80960KA-20 == 20 MHz part
	m_maincpu->set_addrmap(AS_PROGRAM, &namcofl_state::namcofl_mem);

	NAMCO_C75(config, m_mcu, 48.384_MHz_XTAL/3);
	m_mcu->set_addrmap(AS_PROGRAM, &namcofl_state::namcoc75_am);
	m_mcu->p6_in_cb().set(FUNC(namcofl_state::port6_r));
	m_mcu->p6_out_cb().set(FUNC(namcofl_state::port6_w));
	m_mcu->p7_in_cb().set(FUNC(namcofl_state::port7_r));
	m_mcu->an7_cb().set(FUNC(namcofl_state::dac7_r));
	m_mcu->an6_cb().set(FUNC(namcofl_state::dac6_r));
	m_mcu->an5_cb().set(FUNC(namcofl_state::dac5_r));
	m_mcu->an4_cb().set(FUNC(namcofl_state::dac4_r));
	m_mcu->an3_cb().set(FUNC(namcofl_state::dac3_r));
	m_mcu->an2_cb().set(FUNC(namcofl_state::dac2_r));
	m_mcu->an1_cb().set(FUNC(namcofl_state::dac1_r));
	m_mcu->an0_cb().set(FUNC(namcofl_state::dac0_r));
	/* TODO: irq generation for these */
	TIMER(config, "mcu_irq0").configure_periodic(FUNC(namcofl_state::mcu_irq0_cb), attotime::from_hz(60));
	TIMER(config, "mcu_irq2").configure_periodic(FUNC(namcofl_state::mcu_irq2_cb), attotime::from_hz(60));
	TIMER(config, "mcu_adc").configure_periodic(FUNC(namcofl_state::mcu_adc_cb), attotime::from_hz(60));

	MCFG_MACHINE_START_OVERRIDE(namcofl_state,namcofl)
	MCFG_MACHINE_RESET_OVERRIDE(namcofl_state,namcofl)
	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_1);

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_size(NAMCOFL_HTOTAL, NAMCOFL_VTOTAL);
	m_screen->set_visarea(0, NAMCOFL_HBSTART-1, 0, NAMCOFL_VBSTART-1);
	m_screen->set_screen_update(FUNC(namcofl_state::screen_update_namcofl));
	m_screen->set_palette(m_c116);

	NAMCO_C169ROZ(config, m_c169roz, 0);
	m_c169roz->set_palette(m_c116);
	m_c169roz->set_is_namcofl(true);
	m_c169roz->set_ram_words(0x20000/2);
	m_c169roz->set_tile_callback(namco_c169roz_device::c169_tilemap_delegate(&namcofl_state::RozCB, this));
	m_c169roz->set_color_base(0x1800);

	NAMCO_C355SPR(config, m_c355spr, 0);
	m_c355spr->set_screen(m_screen);
	m_c355spr->set_palette(m_c116);
	m_c355spr->set_scroll_offsets(0, 0);
	m_c355spr->set_tile_callback(namco_c355spr_device::c355_obj_code2tile_delegate(&namcofl_state::FLobjcode2tile, this));
	m_c355spr->set_palxor(0x0);
	m_c355spr->set_color_base(0);

	NAMCO_C123TMAP(config, m_c123tmap, 0);
	m_c123tmap->set_palette(m_c116);
	m_c123tmap->set_tile_callback(namco_c123tmap_device::c123_tilemap_delegate(&namcofl_state::TilemapCB, this));
	m_c123tmap->set_color_base(0x1000);

	NAMCO_C116(config, m_c116, 0);
	m_c116->enable_shadows();

	MCFG_VIDEO_START_OVERRIDE(namcofl_state,namcofl)

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();
	c352_device &c352(C352(config, "c352", 48.384_MHz_XTAL/2, 288));
	c352.add_route(0, "lspeaker", 1.00);
	c352.add_route(1, "rspeaker", 1.00);
	//c352.add_route(2, "lspeaker", 1.00); // Second DAC not present.
	//c352.add_route(3, "rspeaker", 1.00);
}

ROM_START( speedrcr )
	ROM_REGION( 0x200000, "maincpu", 0 ) // i960 program
	ROM_LOAD32_WORD("se2mpea4.19a",   0x000000, 0x080000, CRC(95ab3fd7) SHA1(273a536f8512f3c55260ac1b78533bc35b8390ed) )
	ROM_LOAD32_WORD("se2mpoa4.18a",   0x000002, 0x080000, CRC(5b5ef1eb) SHA1(3e9e4abb1a32269baef772079de825dfe1ea230c) )

	ROM_REGION32_LE( 0x200000, "data", 0 ) // Data
	ROM_LOAD32_BYTE("se1_dat0.13a",   0x000000, 0x080000, CRC(cc5d6ff5) SHA1(6fad40a1fac75bc64d3b7a7562cf7ce2a3abd36a) )
	ROM_LOAD32_BYTE("se1_dat1.14a",   0x000001, 0x080000, CRC(ddc8b306) SHA1(f169d521b800c108deffdef9fc6b0058621ee909) )
	ROM_LOAD32_BYTE("se1_dat2.15a",   0x000002, 0x080000, CRC(2a29abbb) SHA1(945419ed61e9a656a340214a63a01818396fbe98) )
	ROM_LOAD32_BYTE("se1_dat3.16a",   0x000003, 0x080000, CRC(49849aff) SHA1(b7c7eea1d56304e40e996ee998c971313ff03614) )

	ROM_REGION16_LE( 0x80000, "c75data", 0 ) // C75 data
	ROM_LOAD("se1_spr.21l",   0x000000,  0x80000, CRC(850a27ac) SHA1(7d5db840ec67659a1f2e69a62cdb03ce6ee0b47b) )

	ROM_REGION( 0x200000, "c169roz", 0 ) // "RCHAR" (roz characters)
	ROM_LOAD("se1_rch0.19j",   0x000000, 0x100000, CRC(a0827288) SHA1(13691ef4d402a6dc91851de4f82cfbdf96d417cb) )
	ROM_LOAD("se1_rch1.18j",   0x100000, 0x100000, CRC(af7609ad) SHA1(b16041f0eb47d7566011d9d762a3083411dc422e) )

	ROM_REGION( 0x400000, "c123tmap", 0 ) // "SCHAR" (regular BG characters)
	ROM_LOAD("se1_sch0.21p",   0x000000, 0x100000, CRC(7b5cfad0) SHA1(5a0355e37eb191bc0cf8b6b7c3d0274560b9bbd5) )
	ROM_LOAD("se1_sch1.20p",   0x100000, 0x100000, CRC(5086e0d3) SHA1(0aa7d11f4f9a75117e69cc77f1b73a68d9007aef) )
	ROM_LOAD("se1_sch2.19p",   0x200000, 0x100000, CRC(e59a731e) SHA1(3fed72e9bb485d4d689ab51490360c4c6f1dc5cb) )
	ROM_LOAD("se1_sch3.18p",   0x300000, 0x100000, CRC(f817027a) SHA1(71745476f496c60d89c8563b3e46bc85eebc79ce) )

	ROM_REGION( 0x800000, "c355spr", 0 )  // OBJ
	ROM_LOAD32_WORD("se1obj0l.ic1", 0x000000, 0x200000, CRC(17585218) SHA1(3332afa9bd194ac37b8d6f352507c523a0f2e2b3) )
	ROM_LOAD32_WORD("se1obj0u.ic2", 0x000002, 0x200000, CRC(d14b1236) SHA1(e5447732ef3acec88fb7a00e0deca3e71a40ae65) )
	ROM_LOAD32_WORD("se1obj1l.ic3", 0x400000, 0x200000, CRC(c4809fd5) SHA1(e0b80fccc17c83fb9d08f7f1cf2cd2f0f3a510b4) )
	ROM_LOAD32_WORD("se1obj1u.ic4", 0x400002, 0x200000, CRC(0beefa56) SHA1(012fb7b330dbf851ab2217da0a0e7136ddc3d23f) )

	ROM_REGION( 0x100000, "c169roz:mask", 0 ) // "RSHAPE" (roz mask like NB-1?)
	ROM_LOAD("se1_rsh.14k",    0x000000, 0x100000, CRC(7aa5a962) SHA1(ff936dfcfcc4ee1f5f2232df62def76ff99e671e) )

	ROM_REGION( 0x100000, "c123tmap:mask", 0 ) // "SSHAPE" (mask for other tiles?)
	ROM_LOAD("se1_ssh.18u",    0x000000, 0x100000, CRC(7a8e0bda) SHA1(f6a508d90274d0205fec0c46f5f783a2715c0c6e) )

	ROM_REGION( 0x1000000, "c352", 0 ) // Samples
	ROM_LOAD("se1_voi.23s",   0x000000, 0x400000, CRC(b95e2ffb) SHA1(7669232d772caa9afa4c7593d018e8b6e534114a) )

	ROM_REGION( 0x000005, "pals", 0) /* PAL's */
	ROM_LOAD( "sysfl-1.bin",  0x000000, 0x000001, NO_DUMP ) /* PAL16L8BCN at 2S */
	ROM_LOAD( "sysfl-2.bin",  0x000000, 0x000001, NO_DUMP ) /* PAL16L8BCN at 3L */
	ROM_LOAD( "sysfl-3.bin",  0x000000, 0x000001, NO_DUMP ) /* PALCE16V8H-15PC/4 at 12S */
	ROM_LOAD( "sysfl-4.bin",  0x000000, 0x000001, NO_DUMP ) /* PAL20L8BCNS at 20N */
	ROM_LOAD( "sysfl-5.bin",  0x000000, 0x000001, NO_DUMP ) /* PALCE16V8H-15PC/4 at 19D */

	ROM_REGION( 0x2000, "nvram", 0 ) // default settings, including calibration
	ROM_LOAD("speedrcr.nv",   0x000000, 0x2000, CRC(58b41c70) SHA1(c30ea7f4951ce208781deafef8d99bdb4902e5b8) )
ROM_END




ROM_START( finalapr )
	ROM_REGION( 0x200000, "maincpu", 0 ) // i960 program
	ROM_LOAD32_WORD("flr2mpeb.19a",   0x000000, 0x080000, CRC(8bfe615f) SHA1(7b867eb261268a83177f1f873689f77d1b6c47ca) )
	ROM_LOAD32_WORD("flr2mpob.18a",   0x000002, 0x080000, CRC(91c14e4f) SHA1(934a86daaef0e3e2c2b3066f4677ccb3aaab6eaf) )

	ROM_REGION32_LE( 0x200000, "data", ROMREGION_ERASEFF ) // Data

	ROM_REGION16_LE( 0x80000, "c75data", 0 ) // C75 data
	ROM_LOAD("flr1spr.21l",   0x000000,  0x20000, CRC(69bb0f5e) SHA1(6831d618de42a165e508ad37db594d3aa290c530) )

	ROM_REGION( 0x200000, "c169roz", 0 ) // "RCHAR" (roz characters)
	ROM_LOAD("flr1rch0.19j",   0x000000, 0x100000, CRC(f413f50d) SHA1(cdd8073dda4feaea78e3b94520cf20a9799fd04d) )
	ROM_LOAD("flr1rch1.18j",   0x100000, 0x100000, CRC(4654d519) SHA1(f8bb473013cdca48dd98df0de2f78c300c156e91) )

	ROM_REGION( 0x400000, "c123tmap", 0 ) // "SCHAR" (regular BG characters)
	ROM_LOAD("flr1sch0.21p",   0x000000, 0x100000, CRC(7169efca) SHA1(66c7aa1b50b236b4700b07be0dca7aebdabedb8c) )
	ROM_LOAD("flr1sch1.20p",   0x100000, 0x100000, CRC(aa233a02) SHA1(0011329f585658d90f820daf0ba08ce2735bddfc) )
	ROM_LOAD("flr1sch2.19p",   0x200000, 0x100000, CRC(9b6b7abd) SHA1(5cdec70db1b46bc5d0866ca155b520157fef3adf) )
	ROM_LOAD("flr1sch3.18p",   0x300000, 0x100000, CRC(50a14f54) SHA1(ab9c2f2e11f006a9dc7e5aedd5788d7d67166d36) )

	ROM_REGION( 0x800000, "c355spr", 0 )  // OBJ
	ROM_LOAD32_WORD("flr1obj0l.ic1", 0x000000, 0x200000, CRC(364a902c) SHA1(4a1ea48eee86d410e36096cc100b4c9a5a645034) )
	ROM_LOAD32_WORD("flr1obj0u.ic2", 0x000002, 0x200000, CRC(a5c7b80e) SHA1(4e0e863cfdd8c051c3c4594bb21e11fb93c28f0c) )
	ROM_LOAD32_WORD("flr1obj1l.ic3", 0x400000, 0x200000, CRC(51fd8de7) SHA1(b1571c45e8c33d746716fd790c704a3361d02bdc) )
	ROM_LOAD32_WORD("flr1obj1u.ic4", 0x400002, 0x200000, CRC(1737aa3c) SHA1(8eaf0dc5d60a270d2c1626f54f5edbddbb0a59c8) )

	ROM_REGION( 0x80000, "c169roz:mask", 0 ) // "RSHAPE" (roz mask like NB-1?)
	ROM_LOAD("flr1rsh.14k",    0x000000, 0x080000, CRC(037c0983) SHA1(c48574a8ad125cedfaf2538c5ff824e121204629) )

	ROM_REGION( 0x80000, "c123tmap:mask", 0 ) // "SSHAPE" (mask for other tiles?)
	ROM_LOAD("flr1ssh.18u",    0x000000, 0x080000, CRC(f70cb2bf) SHA1(dbddda822287783a43415172b81d0382a8ac43d8) )

	ROM_REGION( 0x1000000, "c352", 0 ) // Samples
	ROM_LOAD("flr1voi.23s",   0x000000, 0x200000, CRC(ff6077cd) SHA1(73c289125ddeae3e43153e4c570549ca04501262) )

	ROM_REGION( 0x2000, "nvram", 0 ) // default settings, including calibration
	ROM_LOAD("finalapr.nv",   0x000000, 0x2000, CRC(d51d65fe) SHA1(8a0a523cb6ba2880951e41ca04db23584f0a108c) )
ROM_END

ROM_START( finalapro )
	ROM_REGION( 0x200000, "maincpu", 0 ) // i960 program
	ROM_LOAD32_WORD("flr2mpe.19a",   0x000000, 0x080000, CRC(cc8961ae) SHA1(08ce4d27a723101370d1c536b26256ce0d8a1b6c) )
	ROM_LOAD32_WORD("flr2mpo.18a",   0x000002, 0x080000, CRC(8118f465) SHA1(c4b79878a82fd36b5707e92aa893f69c2b942d57) )

	ROM_REGION32_LE( 0x200000, "data", ROMREGION_ERASEFF ) // Data

	ROM_REGION16_LE( 0x80000, "c75data", 0 ) // C75 data
	ROM_LOAD("flr1spr.21l",   0x000000,  0x20000, CRC(69bb0f5e) SHA1(6831d618de42a165e508ad37db594d3aa290c530) )

	ROM_REGION( 0x200000, "c169roz", 0 ) // "RCHAR" (roz characters)
	ROM_LOAD("flr1rch0.19j",   0x000000, 0x100000, CRC(f413f50d) SHA1(cdd8073dda4feaea78e3b94520cf20a9799fd04d) )
	ROM_LOAD("flr1rch1.18j",   0x100000, 0x100000, CRC(4654d519) SHA1(f8bb473013cdca48dd98df0de2f78c300c156e91) )

	ROM_REGION( 0x400000, "c123tmap", 0 ) // "SCHAR" (regular BG characters)
	ROM_LOAD("flr1sch0.21p",   0x000000, 0x100000, CRC(7169efca) SHA1(66c7aa1b50b236b4700b07be0dca7aebdabedb8c) )
	ROM_LOAD("flr1sch1.20p",   0x100000, 0x100000, CRC(aa233a02) SHA1(0011329f585658d90f820daf0ba08ce2735bddfc) )
	ROM_LOAD("flr1sch2.19p",   0x200000, 0x100000, CRC(9b6b7abd) SHA1(5cdec70db1b46bc5d0866ca155b520157fef3adf) )
	ROM_LOAD("flr1sch3.18p",   0x300000, 0x100000, CRC(50a14f54) SHA1(ab9c2f2e11f006a9dc7e5aedd5788d7d67166d36) )

	ROM_REGION( 0x800000, "c355spr", 0 )  // OBJ
	ROM_LOAD32_WORD("flr1obj0l.ic1", 0x000000, 0x200000, CRC(364a902c) SHA1(4a1ea48eee86d410e36096cc100b4c9a5a645034) )
	ROM_LOAD32_WORD("flr1obj0u.ic2", 0x000002, 0x200000, CRC(a5c7b80e) SHA1(4e0e863cfdd8c051c3c4594bb21e11fb93c28f0c) )
	ROM_LOAD32_WORD("flr1obj1l.ic3", 0x400000, 0x200000, CRC(51fd8de7) SHA1(b1571c45e8c33d746716fd790c704a3361d02bdc) )
	ROM_LOAD32_WORD("flr1obj1u.ic4", 0x400002, 0x200000, CRC(1737aa3c) SHA1(8eaf0dc5d60a270d2c1626f54f5edbddbb0a59c8) )

	ROM_REGION( 0x80000, "c169roz:mask", 0 ) // "RSHAPE" (roz mask like NB-1?)
	ROM_LOAD("flr1rsh.14k",    0x000000, 0x080000, CRC(037c0983) SHA1(c48574a8ad125cedfaf2538c5ff824e121204629) )

	ROM_REGION( 0x80000, "c123tmap:mask", 0 ) // "SSHAPE" (mask for other tiles?)
	ROM_LOAD("flr1ssh.18u",    0x000000, 0x080000, CRC(f70cb2bf) SHA1(dbddda822287783a43415172b81d0382a8ac43d8) )

	ROM_REGION( 0x1000000, "c352", 0 ) // Samples
	ROM_LOAD("flr1voi.23s",   0x000000, 0x200000, CRC(ff6077cd) SHA1(73c289125ddeae3e43153e4c570549ca04501262) )

	ROM_REGION( 0x2000, "nvram", 0 ) // default settings, including calibration
	ROM_LOAD("finalapr.nv",   0x000000, 0x2000, CRC(d51d65fe) SHA1(8a0a523cb6ba2880951e41ca04db23584f0a108c) )
ROM_END


ROM_START( finalaprj )
	ROM_REGION( 0x200000, "maincpu", 0 ) // i960 program
	ROM_LOAD32_WORD("flr1_mpec.19a", 0x000000, 0x080000, CRC(52735494) SHA1(db9873cb39bcfdd3dbe2e5079249fecac2c46df9) )
	ROM_LOAD32_WORD("flr1_mpoc.18a", 0x000002, 0x080000, CRC(b11fe577) SHA1(70b51a1e66a3bb92f027aad7ba0f358c0e139b3c) )

	ROM_REGION32_LE( 0x200000, "data", ROMREGION_ERASEFF ) // Data

	ROM_REGION16_LE( 0x80000, "c75data", 0 ) // C75 data
	ROM_LOAD("flr1spr.21l",   0x000000,  0x20000, CRC(69bb0f5e) SHA1(6831d618de42a165e508ad37db594d3aa290c530) )

	ROM_REGION( 0x200000, "c169roz", 0 ) // "RCHAR" (roz characters)
	ROM_LOAD("flr1rch0.19j",   0x000000, 0x100000, CRC(f413f50d) SHA1(cdd8073dda4feaea78e3b94520cf20a9799fd04d) )
	ROM_LOAD("flr1rch1.18j",   0x100000, 0x100000, CRC(4654d519) SHA1(f8bb473013cdca48dd98df0de2f78c300c156e91) )

	ROM_REGION( 0x400000, "c123tmap", 0 ) // "SCHAR" (regular BG characters)
	ROM_LOAD("flr1sch0.21p",   0x000000, 0x100000, CRC(7169efca) SHA1(66c7aa1b50b236b4700b07be0dca7aebdabedb8c) )
	ROM_LOAD("flr1sch1.20p",   0x100000, 0x100000, CRC(aa233a02) SHA1(0011329f585658d90f820daf0ba08ce2735bddfc) )
	ROM_LOAD("flr1sch2.19p",   0x200000, 0x100000, CRC(9b6b7abd) SHA1(5cdec70db1b46bc5d0866ca155b520157fef3adf) )
	ROM_LOAD("flr1sch3.18p",   0x300000, 0x100000, CRC(50a14f54) SHA1(ab9c2f2e11f006a9dc7e5aedd5788d7d67166d36) )

	ROM_REGION( 0x800000, "c355spr", 0 )  // OBJ
	ROM_LOAD32_WORD("flr1obj0l.ic1", 0x000000, 0x200000, CRC(364a902c) SHA1(4a1ea48eee86d410e36096cc100b4c9a5a645034) )
	ROM_LOAD32_WORD("flr1obj0u.ic2", 0x000002, 0x200000, CRC(a5c7b80e) SHA1(4e0e863cfdd8c051c3c4594bb21e11fb93c28f0c) )
	ROM_LOAD32_WORD("flr1obj1l.ic3", 0x400000, 0x200000, CRC(51fd8de7) SHA1(b1571c45e8c33d746716fd790c704a3361d02bdc) )
	ROM_LOAD32_WORD("flr1obj1u.ic4", 0x400002, 0x200000, CRC(1737aa3c) SHA1(8eaf0dc5d60a270d2c1626f54f5edbddbb0a59c8) )

	ROM_REGION( 0x80000, "c169roz:mask", 0 ) // "RSHAPE" (roz mask like NB-1?)
	ROM_LOAD("flr1rsh.14k",    0x000000, 0x080000, CRC(037c0983) SHA1(c48574a8ad125cedfaf2538c5ff824e121204629) )

	ROM_REGION( 0x80000, "c123tmap:mask", 0 ) // "SSHAPE" (mask for other tiles?)
	ROM_LOAD("flr1ssh.18u",    0x000000, 0x080000, CRC(f70cb2bf) SHA1(dbddda822287783a43415172b81d0382a8ac43d8) )

	ROM_REGION( 0x1000000, "c352", 0 ) // Samples
	ROM_LOAD("flr1voi.23s",   0x000000, 0x200000, CRC(ff6077cd) SHA1(73c289125ddeae3e43153e4c570549ca04501262) )

	ROM_REGION( 0x2000, "nvram", 0 ) // default settings, including calibration
	ROM_LOAD("finalapr.nv",   0x000000, 0x2000, CRC(d51d65fe) SHA1(8a0a523cb6ba2880951e41ca04db23584f0a108c) )
ROM_END

void namcofl_state::common_init()
{
	m_workram = std::make_unique<uint32_t[]>(0x100000/4);
	save_pointer(NAME(m_workram), 0x100000/4),

	save_item(NAME(m_mcu_port6));
	save_item(NAME(m_sprbank));
}

void namcofl_state::init_speedrcr()
{
	common_init();
}

void namcofl_state::init_finalapr()
{
	common_init();
}

GAME(  1995, speedrcr,         0, namcofl, speedrcr, namcofl_state, init_speedrcr, ROT0, "Namco", "Speed Racer", MACHINE_IMPERFECT_GRAPHICS | MACHINE_NODEVICE_LAN | MACHINE_SUPPORTS_SAVE )
GAMEL( 1995, finalapr,         0, namcofl, finalapr, namcofl_state, init_finalapr, ROT0, "Namco", "Final Lap R (Rev. B)", MACHINE_NODEVICE_LAN | MACHINE_SUPPORTS_SAVE, layout_namcofl )
GAMEL( 1995, finalapro, finalapr, namcofl, finalapr, namcofl_state, init_finalapr, ROT0, "Namco", "Final Lap R", MACHINE_NODEVICE_LAN | MACHINE_SUPPORTS_SAVE, layout_namcofl )
GAMEL( 1995, finalaprj, finalapr, namcofl, finalapr, namcofl_state, init_finalapr, ROT0, "Namco", "Final Lap R (Japan Rev. C)", MACHINE_NODEVICE_LAN | MACHINE_SUPPORTS_SAVE, layout_namcofl )
