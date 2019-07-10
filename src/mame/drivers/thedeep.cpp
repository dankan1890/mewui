// license:BSD-3-Clause
// copyright-holders:Luca Elia
/***************************************************************************

                            -= Run Deep / The Deep =-

                    driver by   Luca Elia (l.elia@tin.it)

Main CPU    :   Z80 (LH0080B @ 6MHz) + i8751 (Intel C8751H-88, protection)

Sound CPU   :   65C02 (R65C02P2 @ 2MHz)

Sound Chips :   YM2203C

Video Chips :   L7B0073 DATA EAST MXC 06 8746
                L7A0072 DATA EAST BAC 06 VAE8713

Board       :   DE-0298-1

Notes:

- The MCU handles coins and the bank switching of the roms for the main cpu.
  It additionally provides some z80 code that is copied to ram.

- One ROM (FI-1) is not used.

***************************************************************************/

#include "emu.h"
#include "includes/thedeep.h"

#include "cpu/m6502/m65c02.h"
#include "cpu/z80/z80.h"
#include "sound/2203intf.h"
#include "screen.h"
#include "speaker.h"


/***************************************************************************

                            Main CPU + MCU simulation

***************************************************************************/


WRITE8_MEMBER(thedeep_state::nmi_w)
{
	m_nmi_enable = data;
	if (!m_nmi_enable)
		m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}

void thedeep_state::machine_start()
{
	membank("bank1")->configure_entries(0, 4, memregion("maincpu")->base() + 0x10000, 0x4000);
	save_item(NAME(m_nmi_enable));
	save_item(NAME(m_protection_command));
	save_item(NAME(m_protection_data));
	save_item(NAME(m_protection_index));
	save_item(NAME(m_protection_irq));
	save_item(NAME(m_mcu_p3_reg));
}

void thedeep_state::machine_reset()
{
	membank("bank1")->set_entry(0);
	m_scroll[0] = 0;
	m_scroll[1] = 0;
	m_scroll[2] = 0;
	m_scroll[3] = 0;
	m_protection_command = 0;
	m_protection_index = -1;
	m_protection_irq = 0;
}

WRITE8_MEMBER(thedeep_state::protection_w)
{
	m_protection_command = data;
	switch (m_protection_command)
	{
		case 0x11:
			flip_screen_set(1);
			m_spritegen->set_flip_screen(true);
		break;

		case 0x20:
			flip_screen_set(0);
			m_spritegen->set_flip_screen(false);
		break;

		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
			membank("bank1")->set_entry(m_protection_command & 3);
		break;

		case 0x59:
		{
			if (m_protection_index < 0)
				m_protection_index = 0;

			if ( m_protection_index < 0x19b )
// d000-d00c:   hl += a * b
// d00d-d029:   input a (e.g. $39) output hl (e.g. h=$03 l=$09).
//              Replace trainling 0's with space ($10). 00 -> '  '
// d02a-d039:   input a (e.g. $39) output hl (e.g. h=$03 l=$09).
//              Replace trainling 0's with space ($10). 00 -> ' 0'
// d03a-d046:   input a (e.g. $39) output hl (e.g. h=$03 l=$09). 00 -> '00'
// d047-d086:   a /= e (e can be 0!)
// d087-d0a4:   print ASCII string from HL to IX (sub $30 to every char)
// d0a4-d0be:   print any string from HL to IX
// d0bf-d109:   print ASCII string from HL to IX. Color is in c. (e.g. "game over")
// d10a-d11f:   print 2 digit decimal number in hl to ix, color c. change ix
// d120-d157:   update score: add 3 BCD bytes at ix to those at iy, then clear those at ix
// d158-d165:   print digit: (IX+0) <- H; (IX+1) <-L. ix+=40
// d166-d174:   hl = (hl + 2*a)
// d175-d181:   hl *= e (e must be non zero)
// d182-d19a:   hl /= de
				m_protection_data = memregion("mcu")->base()[0x185+m_protection_index++];
			else
				m_protection_data = 0xc9;

			m_protection_irq  = 1;
		}
		break;

		default:
			logerror( "pc %04x: protection_command %02x\n", m_maincpu->pc(),m_protection_command);
	}
}

READ8_MEMBER(thedeep_state::e004_r)
{
	return m_protection_irq ? 1 : 0;
}

READ8_MEMBER(thedeep_state::protection_r)
{
	m_protection_irq = 0;
	return m_protection_data;
}

WRITE8_MEMBER(thedeep_state::e100_w)
{
	if (data != 1)
		logerror("pc %04x: e100 = %02x\n", m_maincpu->pc(),data);
}

void thedeep_state::main_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0xbfff).bankr("bank1");    // ROM (banked)
	map(0xc000, 0xcfff).ram();
	map(0xd000, 0xdfff).ram();                             // RAM (MCU data copied here)
	map(0xe000, 0xe000).rw(FUNC(thedeep_state::protection_r), FUNC(thedeep_state::protection_w));   // To MCU
	map(0xe004, 0xe004).rw(FUNC(thedeep_state::e004_r), FUNC(thedeep_state::nmi_w));    //
	map(0xe008, 0xe008).portr("e008");           // P1 (Inputs)
	map(0xe009, 0xe009).portr("e009");           // P2
	map(0xe00a, 0xe00a).portr("e00a");           // DSW1
	map(0xe00b, 0xe00b).portr("e00b");           // DSW2
	map(0xe00c, 0xe00c).w(m_soundlatch, FUNC(generic_latch_8_device::write));  // To Sound CPU
	map(0xe100, 0xe100).w(FUNC(thedeep_state::e100_w));   // ?
	map(0xe210, 0xe213).writeonly().share("scroll");    // Scroll
	map(0xe400, 0xe7ff).ram().share("spriteram");   // Sprites
	map(0xe800, 0xefff).ram().w(FUNC(thedeep_state::vram_1_w)).share("vram_1");  // Text Layer
	map(0xf000, 0xf7ff).ram().w(FUNC(thedeep_state::vram_0_w)).share("vram_0");  // Background Layer
	map(0xf800, 0xf83f).ram().share("scroll2"); // Column Scroll
	map(0xf840, 0xffff).ram();
}


/***************************************************************************

                                    Sound CPU

***************************************************************************/

void thedeep_state::audio_map(address_map &map)
{
	map(0x0000, 0x07ff).ram();
	map(0x0800, 0x0801).w("ymsnd", FUNC(ym2203_device::write));  //
	map(0x3000, 0x3000).r(m_soundlatch, FUNC(generic_latch_8_device::read)); // From Main CPU
	map(0x8000, 0xffff).rom();
}


/***************************************************************************

                                    MCU

***************************************************************************/

WRITE8_MEMBER(thedeep_state::p1_w)
{
	flip_screen_set(!BIT(data, 0));
	m_spritegen->set_flip_screen(!BIT(data, 0));
	membank("bank1")->set_entry((data & 6) >> 1);
	logerror("P1 %02x\n",data);
}

READ8_MEMBER(thedeep_state::from_main_r)
{
	static uint8_t res;

	res = 0x11;

	logerror("From Main read = %02x\n",res);
	return 0x20;
}

WRITE8_MEMBER(thedeep_state::to_main_w)
{
	// ...
}

WRITE8_MEMBER(thedeep_state::p3_w)
{
	/* bit 0 0->1 transition IRQ0 to main */
	if((!(m_mcu_p3_reg & 0x01)) && data & 0x01)
		m_maincpu->set_input_line(0, HOLD_LINE);

	/* bit 6 0->1 transition INT1 IRQ ACK */
	if((!(m_mcu_p3_reg & 0x40)) && data & 0x40)
		m_mcu->set_input_line(MCS51_INT1_LINE, CLEAR_LINE);

	/* bit 7 0->1 transition INT0 IRQ ACK */
	if((!(m_mcu_p3_reg & 0x80)) && data & 0x80)
		m_mcu->set_input_line(MCS51_INT0_LINE, CLEAR_LINE);

	m_mcu_p3_reg = data;
	logerror("P3 %02x\n",data);
}

READ8_MEMBER(thedeep_state::p0_r)
{
	uint8_t coin_mux;

	coin_mux = ((ioport("COINS")->read() & 0x0e) == 0x0e); // bit 0 is hard-wired to ALL three coin latches

	return (ioport("COINS")->read() & 0xfe) | (coin_mux & 1);
}


/***************************************************************************

                                Input Ports

***************************************************************************/

static INPUT_PORTS_START( thedeep )
	PORT_START("e008")
	PORT_BIT(  0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    )    // Up / down shown in service mode
	PORT_BIT(  0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  )
	PORT_BIT(  0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  )
	PORT_BIT(  0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT(  0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT(  0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT(  0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x80, IP_ACTIVE_LOW, IPT_START1  )

	PORT_START("e009")
	PORT_BIT(  0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT(  0x80, IP_ACTIVE_LOW, IPT_START2  )

	PORT_START("e00a")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )       PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_B ) )       PORT_DIPLOCATION("SW1:3,4")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x10, 0x10, "Invulnerability (Cheat)")    PORT_DIPLOCATION("SW1:5") /* Listed as "NOT USED - ALWAYS OFF" */
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, "Start Stage" )       PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(    0x20, "1" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Flip_Screen ) )  PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("e00b")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )   PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x02, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x03, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Harder ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) )        PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x04, "5" )
	PORT_DIPSETTING(    0x00, "6" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) )   PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x00, "50k" )
	PORT_DIPSETTING(    0x30, "50k 70k" )
	PORT_DIPSETTING(    0x20, "60k 80k" )
	PORT_DIPSETTING(    0x10, "80k 100k" )
	PORT_DIPUNUSED_DIPLOC( 0x40, 0x40, "SW2:7" ) /* Listed as "Unused" in the manual */
	PORT_SERVICE_DIPLOC(  0x80, IP_ACTIVE_LOW, "SW2:8" )

	PORT_START("MCU")   // Read by the mcu
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_IMPULSE(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) PORT_IMPULSE(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SERVICE1 ) PORT_IMPULSE(1)

	PORT_START("COINS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_CUSTOM ) // mux of bits 1-2-3
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


/***************************************************************************

                                Graphics Layouts

***************************************************************************/

static const gfx_layout layout_8x8x2 =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ 0, 4 },
	{ STEP4(RGN_FRAC(1,2),1), STEP4(RGN_FRAC(0,2),1) },
	{ STEP8(0,8) },
	8*8
};

static const gfx_layout layout_16x16x4 =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(0,4),RGN_FRAC(1,4),RGN_FRAC(2,4),RGN_FRAC(3,4) },
	{ STEP8(8*8*2,1), STEP8(0,1) },
	{ STEP16(0,8) },
	16*16
};

static GFXDECODE_START( gfx_thedeep )
	GFXDECODE_ENTRY( "sprites", 0, layout_16x16x4,  0x080,  8 ) // [0] Sprites
	GFXDECODE_ENTRY( "bg_gfx", 0, layout_16x16x4,   0x100, 16 ) // [1] Background Layer
	GFXDECODE_ENTRY( "text", 0, layout_8x8x2,   0x000, 16 ) // [2] Text Layer
GFXDECODE_END



/***************************************************************************

                                Machine Drivers

***************************************************************************/

TIMER_DEVICE_CALLBACK_MEMBER(thedeep_state::interrupt)
{
	int scanline = param;

	if (scanline == 124) // TODO: clean this
	{
		if (m_protection_command != 0x59)
		{
			int coins = ioport("MCU")->read();
			if      (coins & 1) m_protection_data = 1;
			else if (coins & 2) m_protection_data = 2;
			else if (coins & 4) m_protection_data = 3;
			else                m_protection_data = 0;

			if (m_protection_data)
				m_protection_irq = 1;
		}
		if (m_protection_irq)
			m_maincpu->set_input_line(0, HOLD_LINE);
	}
	else if(scanline == 0)
	{
		if (m_nmi_enable)
			m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
	}
}

INTERRUPT_GEN_MEMBER(thedeep_state::mcu_irq)
{
	m_mcu->set_input_line(MCS51_INT1_LINE, ASSERT_LINE);
}

void thedeep_state::thedeep(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, XTAL(12'000'000)/2); /* verified on pcb */
	m_maincpu->set_addrmap(AS_PROGRAM, &thedeep_state::main_map);

	TIMER(config, "scantimer", 0).configure_scanline(FUNC(thedeep_state::interrupt), "screen", 0, 1);

	M65C02(config, m_audiocpu, XTAL(12'000'000)/8); /* verified on pcb */
	m_audiocpu->set_addrmap(AS_PROGRAM, &thedeep_state::audio_map);
	/* IRQ by YM2203, NMI by when sound latch written by main cpu */

	/* MCU is a i8751 running at 8Mhz (8mhz xtal)*/
	I8751(config, m_mcu, XTAL(8'000'000));
	m_mcu->port_in_cb<0>().set(FUNC(thedeep_state::p0_r));
	m_mcu->port_out_cb<1>().set(FUNC(thedeep_state::p1_w));
	m_mcu->port_in_cb<1>().set(FUNC(thedeep_state::from_main_r));
	m_mcu->port_out_cb<2>().set(FUNC(thedeep_state::to_main_w));
	m_mcu->port_out_cb<3>().set(FUNC(thedeep_state::p3_w));
	m_mcu->set_vblank_int("screen", FUNC(thedeep_state::mcu_irq)); // unknown source, but presumably vblank
	m_mcu->set_disable();

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(0x100, 0xf8);
	screen.set_visarea(0, 0x100-1, 0, 0xf8-1);
	screen.set_screen_update(FUNC(thedeep_state::screen_update));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_thedeep);
	PALETTE(config, m_palette, FUNC(thedeep_state::thedeep_palette), 512);

	DECO_MXC06(config, m_spritegen, 0);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, m_soundlatch);
	m_soundlatch->data_pending_callback().set_inputline(m_audiocpu, INPUT_LINE_NMI);

	ym2203_device &ym(YM2203(config, "ymsnd", XTAL(12'000'000)/4)); /* verified on pcb */
	ym.irq_handler().set_inputline("audiocpu", 0);
	ym.add_route(ALL_OUTPUTS, "mono", 1.0);
}



/***************************************************************************

                                ROMs Loading

***************************************************************************/

/***************************************************************************

Here are the proms for The Deep!
NOTE: This game is Vertical.
I couldn't test this board so I don't know the manufacturer, sorry.
1 Z80
1 R6502
1 YM 2203
1 OSC 12 Mhz
1 OSC 8 Mhz
1 MPU 8751 (which is read-protected)

If you need more info or if this package doesn't
Work, mail me.

..............CaBBe!...................................

***************************************************************************/

ROM_START( thedeep )
	ROM_REGION( 0x20000, "maincpu", 0 )     /* Z80 Code */
	ROM_LOAD( "dp-10.rom", 0x00000, 0x08000, CRC(7480b7a5) SHA1(ac6f121873a70c8077576322c201b7089c7b8a91) )
	ROM_LOAD( "dp-09.rom", 0x10000, 0x10000, CRC(c630aece) SHA1(809916a1ba1c8e0af4c228b0f26ac638e2abf81e) )

	ROM_REGION( 0x10000, "audiocpu", 0 )        /* 65C02 Code */
	ROM_LOAD( "dp-12.rom", 0x8000, 0x8000, CRC(c4e848c4) SHA1(d2dec5c8d7d59703f5485cab9124bf4f835fe728) )

	ROM_REGION( 0x1000, "mcu", 0 )      /* i8751 Code */
	ROM_LOAD( "dp-14", 0x0000, 0x1000, CRC(0b886dad) SHA1(487192764342f8b0a320d20a378bf94f84592da9) )   // 1xxxxxxxxxxx = 0xFF

	ROM_REGION( 0x40000, "sprites", 0 ) /* Sprites */
	ROM_LOAD( "dp-08.rom", 0x00000, 0x10000, CRC(c5624f6b) SHA1(a3c0b13cddae760f30c7344d718cd69cad990054) )
	ROM_LOAD( "dp-07.rom", 0x10000, 0x10000, CRC(c76768c1) SHA1(e41ace1cb06ebe7f676b3b179b7dd01d00cf4d6a) )
	ROM_LOAD( "dp-06.rom", 0x20000, 0x10000, CRC(98adea78) SHA1(6a1af70de995a0a5e42fd395dd9454b7e2d9cb82) )
	ROM_LOAD( "dp-05.rom", 0x30000, 0x10000, CRC(76ea7dd1) SHA1(c29abb44a1182b47da749eeeb2db025ae3f28ea7) )

	ROM_REGION( 0x40000, "bg_gfx", 0 )  /* 16x16x4 Background Layer */
	ROM_LOAD( "dp-03.rom", 0x00000, 0x10000, CRC(6bf5d819) SHA1(74079632d7c88ec22010c1a5bece0e36847fdab9) )
	ROM_LOAD( "dp-01.rom", 0x10000, 0x10000, CRC(e56be2fe) SHA1(25acc0f6d9cb5a727c9bac3e80aeb85a4727ddb0) )
	ROM_LOAD( "dp-04.rom", 0x20000, 0x10000, CRC(4db02c3c) SHA1(6284541372dec1113570cef31ca3c1a202fb4add) )
	ROM_LOAD( "dp-02.rom", 0x30000, 0x10000, CRC(1add423b) SHA1(b565340d719044ba2c428aab74f43f5a7cf7e2a3) )

	ROM_REGION( 0x4000, "text", 0 ) /* 8x8x2 Text Layer */
	ROM_LOAD( "dp-11.rom", 0x0000, 0x4000, CRC(196e23d1) SHA1(ed14e63fccb3e5dce462d9b8155e78749eaf9b3b) )

	ROM_REGION( 0x600, "proms", 0 ) /* Colors */
	ROM_LOAD( "fi-1", 0x000, 0x200, CRC(f31efe09) SHA1(808c90fe02ed7b4000967c331b8773c4168b8a97) )  // FIXED BITS (xxxxxx0xxxxxx0x0)
	ROM_LOAD( "fi-2", 0x200, 0x200, CRC(f305c8d5) SHA1(f82c709dc75a3c681d6f0ebf2702cb90110b1f0c) )  // FIXED BITS (0000xxxx)
	ROM_LOAD( "fi-3", 0x400, 0x200, CRC(f61a9686) SHA1(24082f60b72268d240ceca6999bdf18872625cd2) )
ROM_END

ROM_START( rundeep )
	ROM_REGION( 0x20000, "maincpu", 0 )     /* Z80 Code */
	ROM_LOAD( "3", 0x00000, 0x08000, CRC(c9c9e194) SHA1(e9552c3321585f0902f29b55a7de8e2316885713) )
	ROM_LOAD( "9", 0x10000, 0x10000, CRC(931f4e67) SHA1(f4942c5f0fdbcd6cdb96ddbbf2015f938b56b466) )

	ROM_REGION( 0x10000, "audiocpu", 0 )        /* 65C02 Code */
	ROM_LOAD( "dp-12.rom", 0x8000, 0x8000, CRC(c4e848c4) SHA1(d2dec5c8d7d59703f5485cab9124bf4f835fe728) )

	ROM_REGION( 0x1000, "mcu", 0 )      /* i8751 Code */
	ROM_LOAD( "dp-14", 0x0000, 0x1000, CRC(0b886dad) SHA1(487192764342f8b0a320d20a378bf94f84592da9) )   // 1xxxxxxxxxxx = 0xFF

	ROM_REGION( 0x40000, "sprites", 0 ) /* Sprites */
	ROM_LOAD( "dp-08.rom", 0x00000, 0x10000, CRC(c5624f6b) SHA1(a3c0b13cddae760f30c7344d718cd69cad990054) )
	ROM_LOAD( "dp-07.rom", 0x10000, 0x10000, CRC(c76768c1) SHA1(e41ace1cb06ebe7f676b3b179b7dd01d00cf4d6a) )
	ROM_LOAD( "dp-06.rom", 0x20000, 0x10000, CRC(98adea78) SHA1(6a1af70de995a0a5e42fd395dd9454b7e2d9cb82) )
	ROM_LOAD( "dp-05.rom", 0x30000, 0x10000, CRC(76ea7dd1) SHA1(c29abb44a1182b47da749eeeb2db025ae3f28ea7) )

	ROM_REGION( 0x40000, "bg_gfx", 0 )  /* 16x16x4 Background Layer */
	ROM_LOAD( "dp-03.rom", 0x00000, 0x10000, CRC(6bf5d819) SHA1(74079632d7c88ec22010c1a5bece0e36847fdab9) )
	ROM_LOAD( "dp-01.rom", 0x10000, 0x10000, CRC(e56be2fe) SHA1(25acc0f6d9cb5a727c9bac3e80aeb85a4727ddb0) )
	ROM_LOAD( "dp-04.rom", 0x20000, 0x10000, CRC(4db02c3c) SHA1(6284541372dec1113570cef31ca3c1a202fb4add) )
	ROM_LOAD( "dp-02.rom", 0x30000, 0x10000, CRC(1add423b) SHA1(b565340d719044ba2c428aab74f43f5a7cf7e2a3) )

	ROM_REGION( 0x4000, "text", 0 ) /* 8x8x2 Text Layer */
	ROM_LOAD( "11", 0x0000, 0x4000, CRC(5d29e4b9) SHA1(608345291062e9ce329ebe9a8c1e65d52e358785) )

	ROM_REGION( 0x600, "proms", 0 ) /* Colors */
	ROM_LOAD( "fi-1", 0x000, 0x200, CRC(f31efe09) SHA1(808c90fe02ed7b4000967c331b8773c4168b8a97) )  // FIXED BITS (xxxxxx0xxxxxx0x0)
	ROM_LOAD( "fi-2", 0x200, 0x200, CRC(f305c8d5) SHA1(f82c709dc75a3c681d6f0ebf2702cb90110b1f0c) )  // FIXED BITS (0000xxxx)
	ROM_LOAD( "fi-3", 0x400, 0x200, CRC(f61a9686) SHA1(24082f60b72268d240ceca6999bdf18872625cd2) )
ROM_END

GAME( 1987, thedeep, 0,       thedeep, thedeep, thedeep_state, empty_init, ROT270, "Wood Place Inc.", "The Deep (Japan)", MACHINE_SUPPORTS_SAVE )
GAME( 1988, rundeep, thedeep, thedeep, thedeep, thedeep_state, empty_init, ROT270, "bootleg (Cream)", "Run Deep",         MACHINE_SUPPORTS_SAVE )
