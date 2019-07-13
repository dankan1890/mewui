// license:BSD-3-Clause
// copyright-holders:AJR, Roberto Fresca
/*

VTech Genius Junior series

CPU is 68HC05 derived?

*/

/***************************************************************************************

    Product name:    Pitagorin Junior.
    Brand:           VTech.
    Type:            First steps (4-6 years old) laptop.
    Language:        Spanish.
    Description:     23 didactic games with voice and sounds for 1 or 2 players.
                     (simple maths operations, spell, hangman, letters, numbers, etc)

    Docs by Roberto Fresca.

  ***************************************************************************************

  Games / Activities ...

  ORDER  TITLE                       TRANSLATION
  -----+---------------------------+----------------------
   01  - La letra perdida.           The missing letter.
   02  - Deletrear.                  Spell.
   03  - Plurales.                   Plurals.
   04  - Verbos.                     Verbs.
   05  - El Ahorcado.                Hangman.
   06  - Revoltijo de letras.        Messed letters.
   07  - La palabra escondida.       The hidden word.
   08  - Trueque de letras.          Swapped letters.
   09  - La letra intrusa.           The intruder letter.
   10  - Matematicas (+,-,x,/).      Mathematics (+,-,x,/).
   11  - Aprendiendo los numeros.    Learning the numbers.
   12  - Redondeando cifras.         Rounding numbers.
   13  - Encuentra el signo.         Find the sign.
   14  - Calculadora.                Calculator.
   15  - Tres en raya.               Three in a row.
   16  - El juego de los puntos.     The dot's game.
   17  - El juego del squash.        The squash game.
   18  - El juego del arquero.       The archer game.
   19  - Dibujos animados.           Animated cartoons.
   20  - El compositor.              The composer.

  ***************************************************************************************

  What's inside....

  PCB silkscreened '9817'
      etched on copper '35-19122-2' & '703013-C'

  1x Unknown CPU inside an epoxy blob (more than 100 connections) @ U? (covered with the blob).
  1x VTech LH532HJT mask ROM (originary from Sharp) also silkscreened '9811D' @ U3.
  1x Texas Instruments 84C91HT (CSM10150AN) speech synth with 8-bit microprocessor @ U2.
  1x SN74HC00N @ U5.
  1x SN74HC244N @ U4.

  1x Unknown oscillator (XTAL1).
  1x Unknown trimpot on an r/c oscillator (XTAL2).

  1x 32 contacts (single side) expansion port.
  1x 3 contacts (unknown) connector.
  1x 17 contacts Keyboard (KEY1) connector.
  1x 3 contacts (CONT) connector.


  PCB layout:
                         .......CONNECTORS........
  .------------------------------------------------------------------------------------.
  | .-----------------.  ooo ooooooooooooooooo ooo                         9817        |.---.
  | |   THIS SECTOR   |  unk        KEY1       CONT                      .-------.     /   =|
  | |                 |                                                  |       |    /    =|
  | |  IS POPULATED   |                                                  | VTECH |   | E   =|
  | |                 |            .-----------.                         |       |   | X   =|
  | |   WITH A LOT    |            | SN74HC00N |      (84C91)            | LH532 |   | P P =|
  | |                 |            '-----------'    .----------.         |  HJT  |   | A O =|
  | |      OF...      |                 U5          |CSM10150AN|         |       |   | N R =|
  | |                 |                             '----------'.---.    | 9811D |   | S T =|
  | |   RESISTORS,    |                                  U2     | / |    |       |   | I   =|
  | |                 |   U4                                    '---'    |       |   | O   =|
  | |   CAPACITORS,   |  .--.                                   XTAL 2   '-------'   | N   =|
  | |                 |  |SN|                          ____                 U3        \    =|
  | |      AND        |  |74|                         /    \                           \   =|
  | |                 |  |HC|         35-19122-2     | BLOB |                          |'---'
  | |  TRANSISTORS    |  |24|          703013-C       \____/              .----.       |
  | |                 |  |4N|                           U?                '----'       |
  | '-----------------'  '--'                                             XTAL 1       |
  '------------------------------------------------------------------------------------'


  Expansion Port:

  CONNECTOR                     CONNECTOR
  ---------                     ---------
  01 ----> Vcc                  17 ----> LH532HJT (pin 09)
  02 ----> Vcc                  18 ----> LH532HJT (pin 25)
  03 ----> GND                  19 ----> LH532HJT (pin 10)
  04 ----> ???                  20 ----> LH532HJT (pin 23)
  05 ----> LH532HJT (pin 03)    21 ----> LH532HJT (pin 11)
  06 ----> LH532HJT (pin 02)    22 ----> LH532HJT (pin 21)
  07 ----> LH532HJT (pin 04)    23 ----> LH532HJT (pin 12)
  08 ----> LH532HJT (pin 30)    24 ----> LH532HJT (pin 20)
  09 ----> LH532HJT (pin 05)    25 ----> LH532HJT (pin 13)
  10 ----> LH532HJT (pin 29)    26 ----> LH532HJT (pin 19)
  11 ----> LH532HJT (pin 06)    27 ----> LH532HJT (pin 14)
  12 ----> LH532HJT (pin 28)    28 ----> LH532HJT (pin 18)
  13 ----> LH532HJT (pin 07)    29 ----> LH532HJT (pin 15)
  14 ----> LH532HJT (pin 27)    30 ----> LH532HJT (pin 17)
  15 ----> LH532HJT (pin 08)    31 ----> GND
  16 ----> LH532HJT (pin 26)    32 ----> GND


  U3 - VTech LH532HJT (9811D) 2Mb mask ROM.
       Seems to be 27C020 pin compatible.

                .----v----.
          VCC --|01     32|-- VCC
              --|02     31|--
              --|03     30|--
              --|04     29|--
              --|05     28|--
              --|06     27|--
              --|07     26|--
              --|08     25|--
              --|09     24|--
              --|10     23|--
              --|11     22|--
              --|12     21|--
              --|13     20|--
              --|14     19|--
              --|15     18|--
          GND --|16     17|--
                '---------'


  U2 - Texas Instruments 84C91HT (CSM10150AN).

       Speech Generator with 8-bit microprocessor, 8K ROM, 112 bytes RAM.
       Maximum Clock Frequency = 9.6 MHz.
       Package = DIP16
       Technology = CMOS

                 .---v---.
               --|01   16|--
               --|02   15|--
               --|03   14|--
               --|04   13|--
           GND --|05   12|-- VCC
               --|06   11|--
               --|07   10|-- GND
               --|08   09|--
                 '-------'

  ***************************************************************************************/

#include "emu.h"
#include "cpu/m6805/m68hc05.h"
#include "softlist_dev.h"


class geniusjr_state : public driver_device
{
public:
	geniusjr_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_rombank(*this, "rombank")
	{
	}

	void gj4000(machine_config &config);
	void gj5000(machine_config &config);
	void gjrstar(machine_config &config);
	void gjmovie(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	void gj4000_map(address_map &map);
	void gj5000_map(address_map &map);
	void gjrstar_map(address_map &map);

	required_device<m68hc05_device> m_maincpu;
	required_memory_bank m_rombank;

	u16 m_bank_size;
};

void geniusjr_state::gj4000_map(address_map &map)
{
	map(0x8000, 0xffff).bankr("rombank");
}

void geniusjr_state::gj5000_map(address_map &map)
{
	map(0x4000, 0x7fff).bankr("rombank");
}

void geniusjr_state::gjrstar_map(address_map &map)
{
	map(0x2000, 0x3fff).bankr("rombank");
}


INPUT_PORTS_START( geniusjr )
INPUT_PORTS_END


void geniusjr_state::machine_start()
{
	memory_region *extrom = memregion("extrom");

	m_rombank->configure_entries(0, extrom->bytes() / m_bank_size, extrom->base(), m_bank_size);
	m_rombank->set_entry(0);
}

void geniusjr_state::gj4000(machine_config &config)
{
	M68HC05L9(config, m_maincpu, 8'000'000); // unknown clock
	m_maincpu->set_addrmap(AS_PROGRAM, &geniusjr_state::gj4000_map);

	m_bank_size = 0x8000;
}

void geniusjr_state::gj5000(machine_config &config)
{
	M68HC05L9(config, m_maincpu, 8'000'000); // unknown clock (type also uncertain)
	m_maincpu->set_addrmap(AS_PROGRAM, &geniusjr_state::gj5000_map);

	m_bank_size = 0x4000;
}

void geniusjr_state::gjrstar(machine_config &config)
{
	M68HC05L9(config, m_maincpu, 8'000'000); // unknown clock (type also uncertain, could be L7 instead of L9)
	m_maincpu->set_addrmap(AS_PROGRAM, &geniusjr_state::gjrstar_map);

	m_bank_size = 0x2000;
}

void geniusjr_state::gjmovie(machine_config &config)
{
	gjrstar(config);

	SOFTWARE_LIST(config, "cart_list").set_original("gjmovie");
}


ROM_START( gj4000 )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "hc05_internal.bin", 0x0000, 0x1fff, NO_DUMP )

	ROM_REGION( 0x40000, "extrom", 0 )
	ROM_LOAD( "27-05886-000-000.u4", 0x000000, 0x40000, CRC(5f6db95b) SHA1(fe683154e33a82ea38696096616d11e850e0c7a3))
ROM_END

ROM_START( gj5000 )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "hc05_internal.bin", 0x0000, 0x1fff, NO_DUMP )

	ROM_REGION( 0x80000, "extrom", 0 )
	ROM_LOAD( "27-6019-01.u2", 0x000000, 0x80000, CRC(946e5b7d) SHA1(80963d6ad80d49e54c8996bfc77ac135c4935be5))
ROM_END

ROM_START( gjmovie )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "hc05_internal.bin", 0x0000, 0x1fff, NO_DUMP )

	ROM_REGION( 0x40000, "extrom", 0 )
	ROM_LOAD( "lh532hlk.bin", 0x000000, 0x40000, CRC(2e64c296) SHA1(604034f902e20851cb9af60964031a508ceef83e))
ROM_END

ROM_START( gjrstar )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "hc05_internal.bin", 0x0000, 0x1fff, NO_DUMP )

	ROM_REGION( 0x40000, "extrom", 0 )
	ROM_LOAD( "27-5740-00.u1", 0x000000, 0x40000, CRC(ff3dc3bb) SHA1(bc16dfc1e12b0008456c700c431c8df6263b671f))
ROM_END

ROM_START( gjrstar2 )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "hc05_internal.bin", 0x0000, 0x1fff, NO_DUMP )

	ROM_REGION( 0x40000, "extrom", 0 )
	ROM_LOAD( "27-5740-00.u1", 0x000000, 0x40000, CRC(ff3dc3bb) SHA1(bc16dfc1e12b0008456c700c431c8df6263b671f))     // identical to 'Genius Junior Redstar'
ROM_END

ROM_START( gjrstar3 )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "hc05_internal.bin", 0x0000, 0x1fff, NO_DUMP )

	ROM_REGION( 0x40000, "extrom", 0 )
	ROM_LOAD( "54-06056-000-000.u3", 0x000000, 0x040000, CRC(72522179) SHA1(ede9491713ad018012cf925a519bcafe126f1ad3))
ROM_END

ROM_START( pitagjr )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD( "hc05_internal.bin", 0x0000, 0x1fff, NO_DUMP )

	ROM_REGION( 0x40000, "extrom", 0 )
	ROM_LOAD( "lh532hjt_9811d.u3", 0x00000, 0x40000, CRC(23878b45) SHA1(8f3c41c10cfde9d76763c3a8701ec6616db4ab40) )
ROM_END


//    YEAR  NAME      PARENT   COMPAT  MACHINE   INPUT     CLASS           INIT        COMPANY   FULLNAME                             FLAGS
COMP( 1996, gj4000,   0,       0,      gj4000,   geniusjr, geniusjr_state, empty_init, "VTech",  "Genius Junior 4000 (Germany)",      MACHINE_IS_SKELETON )
COMP( 1993, gjmovie,  0,       0,      gjmovie,  geniusjr, geniusjr_state, empty_init, "VTech",  "Genius Junior Movie (Germany)",     MACHINE_IS_SKELETON )
COMP( 1996, gjrstar,  0,       0,      gjrstar,  geniusjr, geniusjr_state, empty_init, "VTech",  "Genius Junior Redstar (Germany)",   MACHINE_IS_SKELETON )
COMP( 1996, gjrstar2, gjrstar, 0,      gjrstar,  geniusjr, geniusjr_state, empty_init, "VTech",  "Genius Junior Redstar 2 (Germany)", MACHINE_IS_SKELETON )
COMP( 1998, gjrstar3, 0,       0,      gjrstar,  geniusjr, geniusjr_state, empty_init, "VTech",  "Genius Junior Redstar 3 (Germany)", MACHINE_IS_SKELETON )
COMP( 1998, gj5000,   0,       0,      gj5000,   geniusjr, geniusjr_state, empty_init, "VTech",  "Genius Junior 5000 (Germany)",      MACHINE_IS_SKELETON )
COMP( 199?, pitagjr,  0,       0,      gjrstar,  geniusjr, geniusjr_state, empty_init, "VTech",  "Pitagorin Junior",                  MACHINE_IS_SKELETON )
