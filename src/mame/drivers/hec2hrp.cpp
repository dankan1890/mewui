// license:BSD-3-Clause
// copyright-holders:JJ Stacino
/***************************************************************************

        Hector 2HR+
        Victor
        Hector 2HR
        Hector HRX
        Hector MX40c
        Hector MX80c
        Hector 1
        Interact

    These machines can load and run cassettes for the interact / hector1.
    hec2hr - press 2 then 1
    hec2hrp - press 2 then 1
    hec2hrx - press 5 then 1
    hec2mx40 - press 5 then 1
    hec2mx80 - not compatible
    victor - press R then L

        12/05/2009 Skeleton driver - Micko : mmicko@gmail.com
        31/06/2009 Video - Robbbert

        29/10/2009 Update skeleton to functional machine
                    by yo_fr            (jj.stac @ aliceadsl.fr)

                => add Keyboard,
                => add color,
                => add cassette,
                => add sn76477 sound and 1bit sound,
                => add joysticks (stick, pot, fire)
                => add BR/HR switching
                => add bank switch for HRX
                => add device MX80c and bank switching for the ROM
    Importante note : Keyboard emulation code obtained from
                    DChector project : http://dchector.free.fr/ made by DanielCoulom
                    (thanks Daniel)
        03/01/2010 Update and cleanup  by yo_fr       (jj.stac@aliceadsl.fr)
                => add the port mapping for keyboard
        20/11/2010 : synchronization between uPD765 and Z80 are now OK, CP/M running! JJStacino
        11/11/2011 : add minidisk (3.5") support  JJStacino

        for more information about these machines, see the DChector project : http://dchector.free.fr/ made by DanielCoulom
        (thanks to Daniel) and Yves site : http://hectorvictor.free.fr/ (thanks too Yves!)

    TODO :  Add cartridge functionality
            Adjust the one-shot and A/D timing (sn76477)

****************************************************************************/
/* Joystick 1 :
 Numpad :
                (UP)5
  (left)1                     (right)3
               (down)2

 Fire <+>
 Pot =>  home/end */

	/* Joystick 0 :
	arrows
	            (UP)^
	(left)<-                     (right)->
	           (down)v

	Fire <?> near <1>
	Pot => INS /SUPPR

	Cassette : wav file (1 way, 16 bits, 44100hz)
	        K7 file  (For data and games)
	        FOR file (for forth screen data)
*/

#include "emu.h"
#include "includes/hec2hrp.h"

#include "cpu/z80/z80.h"
#include "imagedev/cassette.h"
#include "imagedev/printer.h"
#include "sound/discrete.h"  /* for 1 Bit sound*/

#include "screen.h"
#include "speaker.h"

#include "formats/hect_dsk.h"
#include "formats/hect_tap.h"
#include "formats/hector_minidisc.h"


void hec2hrp_state::hecdisc2_mem(address_map &map)
{
	map.unmap_value_high();

	map(0x0000, 0x3fff).bankrw("bank3"); /* ROM at start up, RAM later */
	map(0x4000, 0xffff).ram();
}

void hec2hrp_state::hecdisc2_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	// ROM page handling
	map(0x000, 0x00f).rw(FUNC(hec2hrp_state::disc2_io00_port_r), FUNC(hec2hrp_state::disc2_io00_port_w));
	// RS232 - 8251 comms handling
	map(0x020, 0x02f).rw(FUNC(hec2hrp_state::disc2_io20_port_r), FUNC(hec2hrp_state::disc2_io20_port_w));
	// Hector comms handling
	map(0x030, 0x03f).rw(FUNC(hec2hrp_state::disc2_io30_port_r), FUNC(hec2hrp_state::disc2_io30_port_w));
	map(0x040, 0x04f).rw(FUNC(hec2hrp_state::disc2_io40_port_r), FUNC(hec2hrp_state::disc2_io40_port_w));
	map(0x050, 0x05f).rw(FUNC(hec2hrp_state::disc2_io50_port_r), FUNC(hec2hrp_state::disc2_io50_port_w));
	// uPD765 link
	map(0x060, 0x061).m(m_upd_fdc, FUNC(upd765a_device::map));
	map(0x070, 0x070).mirror(0x00f).rw(m_upd_fdc, FUNC(upd765a_device::dma_r), FUNC(upd765a_device::dma_w));
}

void hec2hrp_state::hec2hrp_mem(address_map &map)
{
	map.unmap_value_high();
	/* Main ROM page*/
	map(0x0000, 0x3fff).rom();

	/* Hardware address mapping */
	map(0x0800, 0x0808).w(FUNC(hec2hrp_state::switch_bank_w));/* Bank handling */
	map(0x1000, 0x1000).w(FUNC(hec2hrp_state::color_a_w));  /* Color c0/c1 */
	map(0x1800, 0x1800).w(FUNC(hec2hrp_state::color_b_w));  /* Color c2/c3 */
	map(0x2000, 0x2003).w(FUNC(hec2hrp_state::sn_2000_w));  /* Sound */
	map(0x2800, 0x2803).w(FUNC(hec2hrp_state::sn_2800_w));  /* Sound */
	map(0x3000, 0x3000).rw(FUNC(hec2hrp_state::cassette_r), FUNC(hec2hrp_state::sn_3000_w));  /* Write necessary */
	map(0x3800, 0x3807).rw(FUNC(hec2hrp_state::keyboard_r), FUNC(hec2hrp_state::keyboard_w)); /* Keyboard */

	/* Video br mapping */
	map(0x4000, 0x49ff).ram().share("videoram");
	/* contiguous RAM */
	map(0x4A00, 0xbfff).ram();
	/* from 0xC000 to 0xFFFF => Bank Ram for video and data */
	map(0xc000, 0xffff).ram().share("hector_videoram");
}

void hec2hrp_state::hec2hrx_mem(address_map &map)
{
	map.unmap_value_high();
	/* Main ROM page*/
	map(0x0000, 0x3fff).bankr("bank2");

	/* Hardware address mapping*/
	map(0x0800, 0x0808).w(FUNC(hec2hrp_state::switch_bank_w));/* Bank handling */
	map(0x1000, 0x1000).w(FUNC(hec2hrp_state::color_a_w));  /* Color c0/c1 */
	map(0x1800, 0x1800).w(FUNC(hec2hrp_state::color_b_w));  /* Color c2/c3 */
	map(0x2000, 0x2003).w(FUNC(hec2hrp_state::sn_2000_w));  /* Sound */
	map(0x2800, 0x2803).w(FUNC(hec2hrp_state::sn_2800_w));  /* Sound */
	map(0x3000, 0x3000).rw(FUNC(hec2hrp_state::cassette_r), FUNC(hec2hrp_state::sn_3000_w));  /* Write necessary */
	map(0x3800, 0x3807).rw(FUNC(hec2hrp_state::keyboard_r), FUNC(hec2hrp_state::keyboard_w)); /* Keyboard */

	/* Video br mapping */
	map(0x4000, 0x49ff).ram().share("videoram");
	/* contiguous RAM */
	map(0x4A00, 0xbfff).ram();
	/* from 0xC000 to 0xFFFF => Bank Ram for video and data */
	map(0xc000, 0xffff).bankrw("bank1").share("hector_videoram");
}

void hec2hrp_state::hec2hrp_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	map(0x000, 0x0ff).rw(FUNC(hec2hrp_state::io_8255_r), FUNC(hec2hrp_state::io_8255_w));
}

void hec2hrp_state::hec2hrx_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	map(0x0f0, 0x0ff).rw(FUNC(hec2hrp_state::io_8255_r), FUNC(hec2hrp_state::io_8255_w));
}

void hec2hrp_state::hec2mdhrx_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);

	// Minidisc commands and changing the rom page */
	map(0x04, 0x07).rw(m_minidisc_fdc, FUNC(fd1793_device::read), FUNC(fd1793_device::write));
	map(0x08, 0x08).w(FUNC(hec2hrp_state::minidisc_control_w));
	map(0x0f0, 0x0ff).rw(FUNC(hec2hrp_state::io_8255_r), FUNC(hec2hrp_state::io_8255_w));
}

void hec2hrp_state::hec2mx40_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	map(0x000, 0x0ef).w(FUNC(hec2hrp_state::mx40_io_port_w));
	map(0x0f0, 0x0f3).rw(FUNC(hec2hrp_state::io_8255_r), FUNC(hec2hrp_state::io_8255_w));
}

void hec2hrp_state::hec2mx80_io(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	map(0x000, 0x0ef).w(FUNC(hec2hrp_state::mx80_io_port_w));
	map(0x0f0, 0x0f3).rw(FUNC(hec2hrp_state::io_8255_r), FUNC(hec2hrp_state::io_8255_w));


}

/* Input ports */
static INPUT_PORTS_START( hec2hrp )
	/* keyboard input */
	PORT_START("KEY.0") /* [0] - port 3000 @ 0 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH)    PORT_CHAR('\\') PORT_CHAR('|')
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Space")          PORT_CODE(KEYCODE_SPACE)
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Return")         PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Tab")            PORT_CODE(KEYCODE_TAB)
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("<--")            PORT_CODE(KEYCODE_BACKSPACE)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Caps Lock")      PORT_CODE(KEYCODE_CAPSLOCK)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl")           PORT_CODE(KEYCODE_LCONTROL)   PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift")          PORT_CODE(KEYCODE_LSHIFT)     PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_START("KEY.1") /* [1] - port 3000 @ 1 */    /* buttons => 2  1  0  /  .  -  ,  +     */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2 \"")           PORT_CODE(KEYCODE_2)    PORT_CHAR('2')
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1 >")            PORT_CODE(KEYCODE_1)    PORT_CHAR('1')
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0 <")            PORT_CODE(KEYCODE_0)    PORT_CHAR('0')
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)     PORT_CHAR('.') PORT_CHAR('>')
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)    PORT_CHAR(',') PORT_CHAR('<')
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)    PORT_CHAR('-') PORT_CHAR('_')
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)   PORT_CHAR('=') PORT_CHAR('+')

	PORT_START("KEY.2") /* [1] - port 3000 @ 2 */     /* buttons => .. 9  8  7  6  5  4  3  */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9 )")            PORT_CODE(KEYCODE_9)    PORT_CHAR('9')
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8 (")            PORT_CODE(KEYCODE_8)    PORT_CHAR('8')
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7 :")            PORT_CODE(KEYCODE_7)    PORT_CHAR('7')
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6 !")            PORT_CODE(KEYCODE_6)    PORT_CHAR('6')
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5 %")            PORT_CODE(KEYCODE_5)    PORT_CHAR('5')
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4 $")            PORT_CODE(KEYCODE_4)    PORT_CHAR('4')
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3 /")            PORT_CODE(KEYCODE_3)    PORT_CHAR('3')
	PORT_START("KEY.3") /* [1] - port 3000 @ 3 */    /* buttons =>  B  A  ..  ? .. =   ..  ;       */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B)     PORT_CHAR('B')
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A")           PORT_CODE(KEYCODE_Q)   PORT_CHAR('Q')
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE)    PORT_CHAR('[') PORT_CHAR('{')
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE)   PORT_CHAR(']') PORT_CHAR('}')
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNUSED)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)       PORT_CHAR('/') PORT_CHAR('?')
	PORT_START("KEY.4") /* [1] - port 3000 @ 4 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J)            PORT_CHAR('J')
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I)            PORT_CHAR('I')
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H)            PORT_CHAR('H')
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G)            PORT_CHAR('G')
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F)            PORT_CHAR('F')
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E)            PORT_CHAR('E')
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C)            PORT_CHAR('C')
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D)            PORT_CHAR('D')

	PORT_START("KEY.5") /* [1] - port 3000 @ 5 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R)            PORT_CHAR('R')
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q")         PORT_CODE(KEYCODE_A)         PORT_CHAR('A')
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P)            PORT_CHAR('P')
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O)            PORT_CHAR('O')
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N)            PORT_CHAR('N')
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)        PORT_CHAR('M')
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L)            PORT_CHAR('L')
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K)            PORT_CHAR('K')

	PORT_START("KEY.6") /* [1] - port 3000 @ 6 */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z")          PORT_CODE(KEYCODE_W)            PORT_CHAR('W')
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)            PORT_CHAR('Y')
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X")          PORT_CODE(KEYCODE_X)            PORT_CHAR('X')
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W")          PORT_CODE(KEYCODE_Z)            PORT_CHAR('Z')
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V)            PORT_CHAR('V')
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U)            PORT_CHAR('U')
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T)            PORT_CHAR('T')
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S)            PORT_CHAR('S')

	PORT_START("KEY.7") /* [1] - port 3000 @ 7  JOYSTICK */
		PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joy(0) LEFT")        PORT_CODE(KEYCODE_LEFT)     PORT_CHAR(UCHAR_MAMEKEY(LEFT))
		PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joy(0) RIGHT")       PORT_CODE(KEYCODE_RIGHT)    PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
		PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joy(0) UP")          PORT_CODE(KEYCODE_UP)       PORT_CHAR(UCHAR_MAMEKEY(UP))
		PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joy(0) DOWN")        PORT_CODE(KEYCODE_DOWN)     PORT_CHAR(UCHAR_MAMEKEY(DOWN))
		PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joy(1) LEFT")        PORT_CODE(KEYCODE_1_PAD)     // Joy(1) on numpad
		PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joy(1) RIGHT")       PORT_CODE(KEYCODE_3_PAD)
		PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joy(1) UP")          PORT_CODE(KEYCODE_5_PAD)
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joy(1) DOWN")        PORT_CODE(KEYCODE_2_PAD)

	PORT_START("KEY.8") /* [1] - port 3000 @ 8  not for the real machine, but to emulate the analog signal of the joystick */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("RESET")             PORT_CODE(KEYCODE_ESC)      PORT_CHAR(27)
		PORT_BIT(0x02, IP_ACTIVE_LOW,  IPT_KEYBOARD) PORT_NAME("Joy(0) FIRE")       PORT_CODE(KEYCODE_TILDE)
		PORT_BIT(0x04, IP_ACTIVE_LOW,  IPT_KEYBOARD) PORT_NAME("Joy(1) FIRE")       PORT_CODE(KEYCODE_PLUS_PAD)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Pot(0)+") PORT_CODE(KEYCODE_INSERT)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Pot(0)-") PORT_CODE(KEYCODE_DEL)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Pot(1)+") PORT_CODE(KEYCODE_HOME) PORT_CHAR(UCHAR_MAMEKEY(HOME))
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Pot(1)-") PORT_CODE(KEYCODE_END) PORT_CHAR(UCHAR_MAMEKEY(END))
		PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

MACHINE_START_MEMBER(hec2hrp_state,hec2hrp)
{
	hector_init();
}

MACHINE_RESET_MEMBER(hec2hrp_state,hec2hrp)
{
	// Machines init
	hector_reset(1, 0);
}

MACHINE_START_MEMBER(hec2hrp_state,hec2hrx)
{
	uint8_t *RAM   = memregion("maincpu")->base();
	//Patch rom possible !
	//RAMD2[0xff6b] = 0x0ff; // force verbose mode

	// Memory install for bank switching
	membank("bank1")->configure_entry(HECTOR_BANK_PROG , &RAM[0xc000]   );
	membank("bank1")->configure_entry(HECTOR_BANK_VIDEO, m_hector_videoram_hrx); // Video RAM

	// Set bank HECTOR_BANK_PROG as basic bank
	membank("bank1")->set_entry(HECTOR_BANK_PROG);

	// MX-specific
	membank("bank2")->configure_entry(HECTORMX_BANK_PAGE0 , &RAM[0x0000]);
	membank("bank2")->configure_entry(HECTORMX_BANK_PAGE1 , memregion("page1")->base() ); // ROM page 1
	membank("bank2")->configure_entry(HECTORMX_BANK_PAGE2 , memregion("page2")->base() ); // ROM page 2
	membank("bank2")->set_entry(HECTORMX_BANK_PAGE0);

	// Disk II-specific
	membank("bank3")->configure_entry(DISCII_BANK_ROM , memregion("rom_disc2")->base() ); // ROM
	membank("bank3")->configure_entry(DISCII_BANK_RAM , memregion("disc2mem" )->base() ); // RAM
	membank("bank3")->set_entry(DISCII_BANK_ROM);

	// As video HR ram is in bank, use external memory
	m_hector_videoram.set_target(m_hector_videoram_hrx,m_hector_videoram.bytes());

	hector_init();
}
/*****************************************************************************/
MACHINE_START_MEMBER(hec2hrp_state,hec2mdhrx)
/*****************************************************************************/
//minidisc
{
	uint8_t *RAM   = memregion("maincpu")->base();

	// Memory install for bank switching
	membank("bank1")->configure_entry(HECTOR_BANK_PROG, &RAM[0xc000]);
	membank("bank1")->configure_entry(HECTOR_BANK_VIDEO, m_hector_videoram_hrx); // Video RAM

	// Set HECTOR_BANK_PROG as basic bank
	membank("bank1")->set_entry(HECTOR_BANK_PROG);
	//Here, bank 5 is not used for the language switch but for the floppy ROM

	// Mini disk-specific
	membank("bank2")->configure_entry(HECTOR_BANK_BASE, &RAM[0x0000]); // ROM base page
	membank("bank2")->configure_entry(HECTOR_BANK_DISC, memregion("page2")->base() ); // ROM mini disc page
	membank("bank2")->set_entry(HECTOR_BANK_BASE);

	// As video HR ram is in bank, use external memory
	m_hector_videoram.set_target(m_hector_videoram_hrx,m_hector_videoram.bytes());

	hector_init();
}

MACHINE_RESET_MEMBER(hec2hrp_state,hec2hrx)
{
	// Hector Memory
	membank("bank1")->set_entry(HECTOR_BANK_PROG);
	membank("bank2")->set_entry(HECTORMX_BANK_PAGE0);

	// DISK II Memory
	membank("bank3")->set_entry(DISCII_BANK_ROM);

	hector_reset(1, 1);
	hector_disc2_reset();
}

// Mini disk
MACHINE_RESET_MEMBER(hec2hrp_state,hec2mdhrx)
{
	// Hector Memory
	membank("bank1")->set_entry(HECTOR_BANK_PROG);
	membank("bank2")->set_entry(HECTORMX_BANK_PAGE0);

	hector_reset(1, 0);
}

// mini disk interface

FLOPPY_FORMATS_MEMBER( hec2hrp_state::minidisc_formats )
	FLOPPY_HMD_FORMAT
FLOPPY_FORMATS_END

static void minidisc_floppies(device_slot_interface &device)
{
	device.option_add("dd", FLOPPY_35_DD);
}


void hec2hrp_state::hec2hr(machine_config &config)
{
	Z80(config, m_maincpu, 5_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hec2hrp_mem);
	m_maincpu->set_addrmap(AS_IO, &hec2hrp_state::hec2hrp_io);
	m_maincpu->set_periodic_int(FUNC(hec2hrp_state::irq0_line_hold), attotime::from_hz(50)); /*  put on the Z80 irq in Hz*/

	MCFG_MACHINE_RESET_OVERRIDE(hec2hrp_state,hec2hrp)
	MCFG_MACHINE_START_OVERRIDE(hec2hrp_state,hec2hrp)

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(400)); /* 2500 not accurate */
	screen.set_size(512, 230);
	screen.set_visarea(0, 243, 0, 227);
	screen.set_screen_update(FUNC(hec2hrp_state::screen_update_hec2hrp));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_entries(16);
	MCFG_VIDEO_START_OVERRIDE(hec2hrp_state,hec2hrp)

	hector_audio(config);

	CASSETTE(config, m_cassette);
	m_cassette->set_formats(hector_cassette_formats);
	m_cassette->set_default_state(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_ENABLED);

	PRINTER(config, m_printer, 0);
}

void hec2hrp_state::hec2hrp(machine_config &config)
{
	Z80(config, m_maincpu, 5_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hec2hrp_mem);
	m_maincpu->set_addrmap(AS_IO, &hec2hrp_state::hec2hrp_io);
	m_maincpu->set_periodic_int(FUNC(hec2hrp_state::irq0_line_hold), attotime::from_hz(50)); /*  put on the Z80 irq in Hz*/

	MCFG_MACHINE_RESET_OVERRIDE(hec2hrp_state,hec2hrp)
	MCFG_MACHINE_START_OVERRIDE(hec2hrp_state,hec2hrp)

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(400)); /* 2500 not accurate */
	screen.set_size(512, 230);
	screen.set_visarea(0, 243, 0, 227);
	screen.set_screen_update(FUNC(hec2hrp_state::screen_update_hec2hrp));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_entries(16);
	MCFG_VIDEO_START_OVERRIDE(hec2hrp_state,hec2hrp)

	hector_audio(config);

	CASSETTE(config, m_cassette);
	m_cassette->set_formats(hector_cassette_formats);
	m_cassette->set_default_state(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_ENABLED);

	PRINTER(config, m_printer, 0);
}

static void hector_floppies(device_slot_interface &device)
{
	device.option_add("525hd", FLOPPY_525_HD);
}

void hec2hrp_state::hec2mx40(machine_config &config)
{
	Z80(config, m_maincpu, 5_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hec2hrx_mem);
	m_maincpu->set_addrmap(AS_IO, &hec2hrp_state::hec2mx40_io);
	m_maincpu->set_periodic_int(FUNC(hec2hrp_state::irq0_line_hold), attotime::from_hz(50)); /*  put on the Z80 irq in Hz*/

	Z80(config, m_disc2cpu, 4_MHz_XTAL);
	m_disc2cpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hecdisc2_mem);
	m_disc2cpu->set_addrmap(AS_IO, &hec2hrp_state::hecdisc2_io);

	UPD765A(config, m_upd_fdc, 8'000'000, false, true);
	m_upd_fdc->intrq_wr_callback().set(FUNC(hec2hrp_state::disc2_fdc_interrupt));
	m_upd_fdc->drq_wr_callback().set(FUNC(hec2hrp_state::disc2_fdc_dma_irq));

	FLOPPY_CONNECTOR(config, m_upd_connector[0], hector_floppies, "525hd", floppy_image_device::default_floppy_formats);
	FLOPPY_CONNECTOR(config, m_upd_connector[1], hector_floppies, "525hd", floppy_image_device::default_floppy_formats);

	MCFG_MACHINE_RESET_OVERRIDE(hec2hrp_state,hec2hrx)
	MCFG_MACHINE_START_OVERRIDE(hec2hrp_state,hec2hrx)

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(400)); /* 2500 not accurate */
	screen.set_size(512, 230);
	screen.set_visarea(0, 243, 0, 227);
	screen.set_screen_update(FUNC(hec2hrp_state::screen_update_hec2hrp));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_entries(16);
	MCFG_VIDEO_START_OVERRIDE(hec2hrp_state,hec2hrp)

	hector_audio(config);

	CASSETTE(config, m_cassette);
	m_cassette->set_formats(hector_cassette_formats);
	m_cassette->set_default_state(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_ENABLED);

	PRINTER(config, m_printer, 0);
}


void hec2hrp_state::hec2hrx(machine_config &config)
{
	Z80(config, m_maincpu, 5_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hec2hrx_mem);
	m_maincpu->set_addrmap(AS_IO, &hec2hrp_state::hec2hrx_io);
	m_maincpu->set_periodic_int(FUNC(hec2hrp_state::irq0_line_hold), attotime::from_hz(50)); /*  put on the Z80 irq in Hz*/

	MCFG_MACHINE_RESET_OVERRIDE(hec2hrp_state,hec2hrx)
	MCFG_MACHINE_START_OVERRIDE(hec2hrp_state,hec2hrx)

	Z80(config, m_disc2cpu, 4_MHz_XTAL);
	m_disc2cpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hecdisc2_mem);
	m_disc2cpu->set_addrmap(AS_IO, &hec2hrp_state::hecdisc2_io);

	UPD765A(config, m_upd_fdc, 8'000'000, false, true);
	m_upd_fdc->intrq_wr_callback().set(FUNC(hec2hrp_state::disc2_fdc_interrupt));
	m_upd_fdc->drq_wr_callback().set(FUNC(hec2hrp_state::disc2_fdc_dma_irq));

	FLOPPY_CONNECTOR(config, m_upd_connector[0], hector_floppies, "525hd", floppy_image_device::default_floppy_formats);
	FLOPPY_CONNECTOR(config, m_upd_connector[1], hector_floppies, "525hd", floppy_image_device::default_floppy_formats);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(400)); /* 2500 not accurate */
	screen.set_size(512, 230);
	screen.set_visarea(0, 243, 0, 227);
	screen.set_screen_update(FUNC(hec2hrp_state::screen_update_hec2hrp));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_entries(16);
	MCFG_VIDEO_START_OVERRIDE(hec2hrp_state,hec2hrp)

	hector_audio(config);

	CASSETTE(config, m_cassette);
	m_cassette->set_formats(hector_cassette_formats);
	m_cassette->set_default_state(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_ENABLED);

	PRINTER(config, m_printer, 0);
}

void hec2hrp_state::hec2mdhrx(machine_config &config)
{
	Z80(config, m_maincpu, 5_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hec2hrx_mem);
	m_maincpu->set_addrmap(AS_IO, &hec2hrp_state::hec2mdhrx_io);
	m_maincpu->set_periodic_int(FUNC(hec2hrp_state::irq0_line_hold), attotime::from_hz(50)); /*  put on the Z80 irq in Hz*/

	MCFG_MACHINE_RESET_OVERRIDE(hec2hrp_state,hec2mdhrx)
	MCFG_MACHINE_START_OVERRIDE(hec2hrp_state,hec2mdhrx)

	/* 3.5" ("mini") disc */
	FD1793(config, m_minidisc_fdc, 1_MHz_XTAL);
	FLOPPY_CONNECTOR(config, "wd179x:0", minidisc_floppies, "dd", hec2hrp_state::minidisc_formats);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(400)); /* 2500 not accurate */
	screen.set_size(512, 230);
	screen.set_visarea(0, 243, 0, 227);
	screen.set_screen_update(FUNC(hec2hrp_state::screen_update_hec2hrp));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_entries(16);
	MCFG_VIDEO_START_OVERRIDE(hec2hrp_state,hec2hrp)

	hector_audio(config);

	CASSETTE(config, m_cassette);
	m_cassette->set_formats(hector_cassette_formats);
	m_cassette->set_default_state(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_ENABLED);

	PRINTER(config, m_printer, 0);
}


void hec2hrp_state::hec2mx80(machine_config &config)
{
	Z80(config, m_maincpu, 5_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hec2hrx_mem);
	m_maincpu->set_addrmap(AS_IO, &hec2hrp_state::hec2mx80_io);
	m_maincpu->set_periodic_int(FUNC(hec2hrp_state::irq0_line_hold), attotime::from_hz(50)); /*  put on the Z80 irq in Hz*/

	MCFG_MACHINE_RESET_OVERRIDE(hec2hrp_state,hec2hrx)
	MCFG_MACHINE_START_OVERRIDE(hec2hrp_state,hec2hrx)

	Z80(config, m_disc2cpu, 4_MHz_XTAL);
	m_disc2cpu->set_addrmap(AS_PROGRAM, &hec2hrp_state::hecdisc2_mem);
	m_disc2cpu->set_addrmap(AS_IO, &hec2hrp_state::hecdisc2_io);

	UPD765A(config, m_upd_fdc, 8'000'000, false, true);
	m_upd_fdc->intrq_wr_callback().set(FUNC(hec2hrp_state::disc2_fdc_interrupt));
	m_upd_fdc->drq_wr_callback().set(FUNC(hec2hrp_state::disc2_fdc_dma_irq));

	FLOPPY_CONNECTOR(config, m_upd_connector[0], hector_floppies, "525hd", floppy_image_device::default_floppy_formats);
	FLOPPY_CONNECTOR(config, m_upd_connector[1], hector_floppies, "525hd", floppy_image_device::default_floppy_formats);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(400)); /* 2500 not accurate */
	screen.set_size(512, 230);
	screen.set_visarea(0, 243, 0, 227);
	screen.set_screen_update(FUNC(hec2hrp_state::screen_update_hec2hrp));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_entries(16);
	MCFG_VIDEO_START_OVERRIDE(hec2hrp_state,hec2hrp)

	hector_audio(config);

	CASSETTE(config, m_cassette);
	m_cassette->set_formats(hector_cassette_formats);
	m_cassette->set_default_state(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_ENABLED);

	PRINTER(config, m_printer, 0);
}

ROM_START( hec2hr )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "2hr.bin", 0x0000, 0x1000, CRC(84b9e672) SHA1(8c8b089166122eee565addaed10f84c5ce6d849b))
	ROM_REGION( 0x4000, "page1", ROMREGION_ERASEFF )
	ROM_REGION( 0x4000, "page2", ROMREGION_ERASEFF )
ROM_END

ROM_START( victor )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "victor.rom",  0x0000, 0x1000, CRC(d1e9508f) SHA1(d0f1bdcd39917fafc8859223ab38eee2a7dc85ff))
	ROM_REGION( 0x4000, "page1", ROMREGION_ERASEFF )
	ROM_REGION( 0x4000, "page2", ROMREGION_ERASEFF )
ROM_END

ROM_START( hec2hrp )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "hector2hrp.rom", 0x0000, 0x4000, CRC(983f52e4) SHA1(71695941d689827356042ee52ffe55ce7e6b8ecd))
	ROM_REGION( 0x4000, "page1", ROMREGION_ERASEFF )
	ROM_REGION( 0x4000, "page2", ROMREGION_ERASEFF )
	ROM_REGION( 0x10000, "disc2cpu", ROMREGION_ERASEFF )
	ROM_REGION( 0x04000, "rom_disc2", ROMREGION_ERASEFF )
ROM_END

ROM_START( hec2hrx )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "hector2hrx.rom", 0x0000, 0x4000, CRC(f047c521) SHA1(744336b2acc76acd7c245b562bdc96dca155b066))
	ROM_REGION( 0x4000, "page1", ROMREGION_ERASEFF )
	ROM_REGION( 0x4000, "page2", ROMREGION_ERASEFF )
	ROM_REGION( 0x10000, "disc2cpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x04000, "rom_disc2", ROMREGION_ERASEFF )
	ROM_LOAD( "d800k.bin" , 0x0000,0x1000, CRC(831bd584) SHA1(9782ee58f570042608d9d568b2c3fc4c6d87d8b9))
	ROM_REGION( 0x10000,  "disc2mem", ROMREGION_ERASE00 )
ROM_END

// minidisc
ROM_START( hec2mdhrx )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "mdic1.bin" , 0x0000,0x2000, CRC(ddda1065) SHA1(e7bba14a72605238d2f8299da029b8320a563254))
	ROM_LOAD( "mdicmb.bin" , 0x2000,0x2000, CRC(d8090747) SHA1(f2925b68002307562e2ea5e36b740e5458f0f0eb))

	ROM_REGION( 0x4000, "page1", ROMREGION_ERASEFF )  // Page 1 = unused page
	ROM_REGION( 0x4000, "page2", ROMREGION_ERASEFF )  // Page 2 = minidisc page
	ROM_LOAD( "mdic3.bin"  , 0x0000,0x2000, CRC(87801816) SHA1(ddf441f40df014b237cdf17430d1989f3a452d04))
	ROM_LOAD( "mdicmb.bin" , 0x2000,0x2000, CRC(d8090747) SHA1(f2925b68002307562e2ea5e36b740e5458f0f0eb))
ROM_END

ROM_START( hec2mx80 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "mx80c_page0.rom" , 0x0000,0x4000, CRC(a75945cf) SHA1(542391e482271be0997b069cf13c8b5dae28feec))
	ROM_REGION( 0x4000, "page1", ROMREGION_ERASEFF )
	ROM_LOAD( "mx80c_page1.rom", 0x0000, 0x4000, CRC(4615f57c) SHA1(5de291bf3ae0320915133b99f1a088cb56c41658))
	ROM_REGION( 0x4000, "page2", ROMREGION_ERASEFF )
	ROM_LOAD( "mx80c_page2.rom" , 0x0000,0x4000, CRC(2d5d975e) SHA1(48307132e0f3fad0262859bb8142d108f694a436))

	ROM_REGION( 0x04000, "rom_disc2", ROMREGION_ERASEFF )
	ROM_LOAD( "d800k.bin" , 0x0000,0x1000, CRC(831bd584) SHA1(9782ee58f570042608d9d568b2c3fc4c6d87d8b9))
	ROM_REGION( 0x10000,  "disc2mem", ROMREGION_ERASE00 )
ROM_END

ROM_START( hec2mx40 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "mx40c_page0.rom" , 0x0000,0x4000, CRC(9bb5566d) SHA1(0c8c2e396ec8eb995d2b621abe06b6968ca5d0aa))
	ROM_REGION( 0x4000, "page1", ROMREGION_ERASEFF )
	ROM_LOAD( "mx40c_page1.rom", 0x0000, 0x4000, CRC(192a76fa) SHA1(062aa6df0b554b85774d4b5edeea8496a4baca35))
	ROM_REGION( 0x4000, "page2", ROMREGION_ERASEFF )
	ROM_LOAD( "mx40c_page2.rom" , 0x0000,0x4000, CRC(ef1b2654) SHA1(66624ea040cb7ede4720ad2eca0738d0d3bad89a))

	ROM_REGION( 0x04000, "rom_disc2", ROMREGION_ERASEFF )
//  ROM_LOAD( "d360k.bin" , 0x0000,0x4000, CRC(2454eacb) SHA1(dc0d5a7d5891a7e422d9d142a2419527bb15dfd5))
	ROM_LOAD( "d800k.bin" , 0x0000,0x1000, CRC(831bd584) SHA1(9782ee58f570042608d9d568b2c3fc4c6d87d8b9))
//  ROM_LOAD( "d200k.bin" , 0x0000,0x4000, CRC(e2801377) SHA1(0926df5b417ecd8013e35c71b76780c5a25c1cbf))
	ROM_REGION( 0x10000,  "disc2mem", ROMREGION_ERASE00 )
ROM_END

/* Driver */

/*  YEAR   NAME       PARENT   COMPAT    MACHINE    INPUT    CLASS          INIT        COMPANY       FULLNAME                  FLAGS */
COMP(1983, hec2hrp,   0,       interact, hec2hrp,   hec2hrp, hec2hrp_state, empty_init, "Micronique", "Hector 2HR+",            MACHINE_IMPERFECT_SOUND)
COMP(1980, victor,    hec2hrp, 0,        hec2hrp,   hec2hrp, hec2hrp_state, empty_init, "Micronique", "Victor",                 MACHINE_IMPERFECT_SOUND)
COMP(1983, hec2hr,    hec2hrp, 0,        hec2hr,    hec2hrp, hec2hrp_state, empty_init, "Micronique", "Hector 2HR",             MACHINE_IMPERFECT_SOUND)
COMP(1984, hec2hrx,   hec2hrp, 0,        hec2hrx,   hec2hrp, hec2hrp_state, empty_init, "Micronique", "Hector HRX + Disc2",     MACHINE_IMPERFECT_SOUND)
COMP(1985, hec2mdhrx, hec2hrp, 0,        hec2mdhrx, hec2hrp, hec2hrp_state, empty_init, "Micronique", "Hector HRX + mini Disc", MACHINE_IMPERFECT_SOUND)
COMP(1985, hec2mx80,  hec2hrp, 0,        hec2mx80,  hec2hrp, hec2hrp_state, empty_init, "Micronique", "Hector MX 80c + Disc2",  MACHINE_IMPERFECT_SOUND)
COMP(1985, hec2mx40,  hec2hrp, 0,        hec2mx40,  hec2hrp, hec2hrp_state, empty_init, "Micronique", "Hector MX 40c + Disc2",  MACHINE_IMPERFECT_SOUND)
