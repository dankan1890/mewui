// license:BSD-3-Clause
// copyright-holders:

/*
The CRT-300 is an extension of CRT-250/CRT-260 boards found in meritm.cpp.

Merit - Multi-Action 6710-13 Touchscreen game

MERIT CRT-300 REV A:
+------------------------------------------------------------+
|       U45#                HY6264ALP-10      10.000000MHz   |
|                                                            |
|       U46                 HY6264ALP-10                     |
|                                                            |
|       U47                                   HD46505SP-2  +-|
|                                                          | |
|       U48                                   ADV476KN35   | |
|                                                          | |
|            PAL16l8ACN.U14                                |J|
|                                                          |2|
|U7**        PAL20L10NC.U8                                 | |
|                                                          | |
|DS1225Y.U6  PAL20L10NC.U1  PC16550DN                      | |
|                                                          | |
|U5*     U20*               DSW  1.84MHz                   +-|
|                                                          +-|
|                           YM2149F                        | |
|                                                          | |
|       Z8400BB1-Z80B       D8255AC-2                      | |
|                                                          |J|
|10.000MHz       DS1231-50  D8255AC-2                      |1|
|                                                          | |
|                                                          | |
|                                                          | |
|VOLUME   MB3731                                           | |
+----------------------------------------------------------+-+

  CPU: Z80B 6MHz part Clocked @ 5MHz (10MHz/2)
Video: HD46505SP-2 CRT controller (enhanced) 2MHz AKA HD68B45SP compatible with MC68B45P
       ADV476KN35 CMOS Monolithic 256x18 Color Palette RAM-DAC PS/2 & VGA compatible
Sound: AY8930
       MB3731 18-Watt BTL Power Amplifier
  RAM: 6264 8K High Speed CMOS Static RAM x 2
  OSC: 10.00MHz x 2, 1.85MHz
  DSW: 8 switch dipswitch block labeled S1
Other: PC16550DN UART with FIFO clocked @ 1.84MHz
       D8255AC Programmable Peripheral Interface chip x 2
       DS1225Y-200 Dallas 8Kx8 NVRAM @ U6
       DS1231 Power Monitor Chip

*  U5 is a 28pin female socket, U20 is 28pin male socket
** U7 is a stacked DS1216 Dallas 2Kx8 SmartWatch RTC + BENCHMARQ bq4010YMA-150 8Kx8 NVRAM

Connectors:
  J1 80-pin connector to backplane & wire harness
  J2 80-pin connector to backplane & wire harness

# Denotes unpopulated

ROMs on CRT-300 mainboard:

U-46
DC-350

U-47
DC-350

U-48
DC-350


CRT-307 rev A
+----------------+
| 28pinM  28pinF |
| U1    74LS541N |
|       SW1      |
| U2    74LS00N  |
+----------------+

Other: 8 switch dipswitch block labeled SW1
       28pinM 28pin male socket to plug into U5
       28pinF 28pin female socket to receive U20

ROMs on CRT-307 daughter board

U-1
DC-350
Ticket

U-2
DC-350
Ticket

Snooping around the U1 & U2 roms with a hex editor shows the game uses a Printer & Modem.
Game can be played in English or French
Games look to be basic Poker games, Blackjack & Super 8 Slots
Copyright is 1989

---------------------------------------------------------------------------------------------

The CRT-350 is an extension of CRT-300 that allows for memory (ROM) expansion.

Merit MULTI-ACTION 7551-21-R2P - Touchscreen game

MERIT CRT-350 REV C (and REV B):
+------------------------------------------------------------+
|       U45*                HY6264ALP-10      10.000000MHz   |
|                                                            |
|       U46                 HY6264ALP-10                     |
|                                                            |
|       U47                                   HD46505SP-2  +-|
|                                                          | |
|       U48                                   IMSG176P-40  | |
|                                                          | |
|U7*    PAL16l8ACN.U14                                     |J|
|                                                          |2|
|U6*    PAL20L10NC.U8                                      | |
|                                                          | |
|U5*    PAL20L10NC.U4A      PC16550DN                      | |
|                                                          | |
||===========J14==========| DSW  1.84MHz                   +-|
|                                                          +-|
|                           YM2149F                        | |
|                                                          | |
|       Z0840006PSC-Z80B    D8255AC-2                      | |
|                                                          |J|
|10.000MHz       DS1231-50  D8255AC-2                      |1|
|                                                          | |
|                                                          | |
|                                                          | |
|VOLUME   LM383T                                           | |
+----------------------------------------------------------+-+

  CPU: Z80B 6MHz part Clocked @ 5MHz (10MHz/2)
Video: HD46505SP-2 CRT controller (enhanced) 2MHz AKA HD68B45SP compatible with MC68B45P
       inmos IMS G176 High performance CMOS color look-up table
Sound: Yamaha YM2149F or AY-3-8910A
       LM383T 7-Watt Audio High Power Amplifier (rev C PCB only)
       MB3731 18-Watt BTL Power Amplifier (rev B PCB only)
  RAM: 6264 8K High Speed CMOS Static RAM x 2
  OSC: 10.00MHz x 2, 1.85MHz
  DSW: 8 switch dipswitch block labeled S1
Other: PC16550DN UART with FIFO clocked @ 1.84MHz
       D8255AC Programmable Peripheral Interface chip x 2
       DS1231 Power Monitor Chip

Connectors:
  J1 80-pin connector to CRT-351 backplane & wire harness
  J2 80-pin connector to CRT-351 backplane & wire harness
  J14 64-pin connector for CRT-352 Expansion board (96 pins, but middle row pins removed)

* Denotes unpopulated


MEMORY EXPANSION BOARD CRT-352 rev A
+--------------------------+
|     U11*         U15     |
|                          |
|     U10          U14     |
|                          |
|     U9           U13     |
|                          |
|     U8           U12     |
|                          |
| 74HC245      INS8250N    |
|                          |
| DS1225Y.U7   PAL 1.84MHz |
|                          |
| DS1216.U18   GAL20XV10B  |
|                          |
| DS1230Y.U17              |
|                          |
||===========J1===========||
|                          |
|       74HC541N    DSW    |
+--------------------------+

Other: DS1225Y-200 Dallas 8Kx8 NVRAM
       DS1230Y-200 Dallas 32Kx8 NVRAM
       DS1216 Dallas 2Kx8 SmartWatch RTC
       PC16550DN UART with FIFO clocked @ 1.84MHz
       8 switch dipswitch block labeled SW1 (enable/disable games)

Connectors:
  J1 96-pin female receiver to connect to CRT-350 main board  (64 pins used, middle row pins not connected)

* Denotes unpopulated


CRT-351
+----------------------------------------------------------------------------+
|      |=J5==| |---------------------------J3-------------------------------||
| |===J6===|                                                   |=====J4=====||
|                                                    JPR3              |-J7-||
|              |=============J2=============||=============J1===============||
+----------------------------------------------------------------------------+

J1 80-pin connector to J1 connector on the CRT-350 mainboard
J2 80-pin connector to J2 connector on the CRT-350 mainboard
J3 65-pin single row connector for wire harness
J4 40-pin dual row connector for printer
J5 16-pin dual row connector for unknown
J6 17-pin single row connector for unknown (some kind of jumper block)
J7 6-pin single row connector for hopper
JPR3 is a 3 pin jumper: Pins 1&2 = Printer, pins 2&3= Hopper



Main PCB graphics roms (on main board):

U46
NC $

U47
NC $

U48
NC $



Program ROMs on Expansion board:

U11 *Empty       U15
                 7551-21-R2P

U10              U14
7551-21-R2P      7551-21-R2P

U9               U13
7551-21-R2P      7551-21-R2P

U8               U12
7551-21-R2P      7551-21-R2P


According to U14:
 INVALID DIPSWITCH SETTING
 ENABLE AT LEAST TWO GAMES
  CS1-1 ON =JOKER POKER
  CS1-2 ON =DEUCES
  CS1-3 ON =FEVER POKER
  CS1-4 ON =JACKS POKER
  CS1-5 ON =BLACKJACK
  CS1-6 ON =KENO WILD
  CS1-7 ON =TBALL KENO
  CS1-8 ON =BINGO
 CSW1-1 ON =DOLR JACKS
 CSW1-2 ON =DOLR DEUCE
 CSW1-3 ON =5# KENO
 CSW1-4 ON =ADDEM

Dipswitch on CRT-350 main is labeled S1
Dipswitch on CRT-352 MEM is labeled SW1

-------------------------------------------------------------

Merit MULTI-ACTION 7556-00-R2 - Touchscreen game

MERIT CRT-350 REV C (and REV B):
+------------------------------------------------------------+
|       U45*                HY6264ALP-10      10.000000MHz   |
|                                                            |
|       U46                 HY6264ALP-10                     |
|                                                            |
|       U47                                   HD46505SP-2  +-|
|                                                          | |
|       U48                                   IMSG176P-40  | |
|                                                          | |
|U7*    PAL16l8ACN.U14                                     |J|
|                                                          |2|
|U6*    PAL20L10NC.U8                                      | |
|                                                          | |
|U5*    PAL20L10NC.U4A      PC16550DN                      | |
|                                                          | |
||===========J14==========| DSW  1.84MHz                   +-|
|                                                          +-|
|                           YM2149F                        | |
|                                                          | |
|       Z0840006PSC-Z80B    D8255AC-2                      | |
|                                                          |J|
|10.000MHz       DS1231-50  D8255AC-2                      |1|
|                                                          | |
|                                                          | |
|                                                          | |
|VOLUME   LM383T                                           | |
+----------------------------------------------------------+-+

  CPU: Z80B 6MHz part Clocked @ 5MHz (10MHz/2)
Video: HD46505SP-2 CRT controller (enhanced) 2MHz AKA HD68B45SP compatible with MC68B45P
       inmos IMS G176 High performance CMOS color look-up table compatible to
         ADV476KN35 CMOS Monolithic 256x18 Color Palette RAM-DAC
Sound: Yamaha YM2149F or AY-3-8910A
       LM383T 7-Watt Audio High Power Amplifier (rev C PCB only)
       MB3731 18-Watt BTL Power Amplifier (rev B PCB only)
  RAM: 6264 8K High Speed CMOS Static RAM x 2
  OSC: 10.00MHz x 2, 1.85MHz
  DSW: 8 switch dipswitch block labeled S1
Other: PC16550DN UART with FIFO clocked @ 1.84MHz
       D8255AC Programmable Peripheral Interface chip x 2
       DS1231 Power Monitor Chip

Connectors:
  J1 80-pin connector to CRT-351 backplane & wire harness
  J2 80-pin connector to CRT-351 backplane & wire harness
  J14 64-pin connector for CRT-352 Expansion board (96 pins, but middle row pins removed)

* Denotes unpopulated


MEMORY EXPANSION BOARD CRT-352 rev A
+--------------------------+
|     U11*         U15     |
|                          |
|     U10          U14     |
|                          |
|     U9           U13     |
|                          |
|     U8           U12     |
|                          |
| 74HC245      INS8250N    |
|                          |
| DS1225Y.U7   PAL 1.84MHz |
|                          |
| DS1216.U18   GAL20XV10B  |
|                          |
| DS1230Y.U17              |
|                          |
||===========J1===========||
|                          |
|       74HC541N    DSW    |
+--------------------------+

Other: DS1225Y-200 Dallas 8Kx8 NVRAM
       DS1230Y-200 Dallas 32Kx8 NVRAM
       DS1216 Dallas 2Kx8 SmartWatch RTC
       PC16550DN UART with FIFO clocked @ 1.84MHz
       8 switch dipswitch block labeled SW1 (enable/disable games)
 NOTE: on this PCB pin28 on the DS1225Y was bent up so data was not correctly saved from PCB
       on this PCB pin28 on the DS1130Y was broken so data was not correctly saved from PCB

Connectors:
  J1 96-pin female receiver to connect to CRT-350 main board  (64 pins used, middle row pins not connected)

* Denotes unpopulated


CRT-351
+----------------------------------------------------------------------------+
|      |=J5==| |---------------------------J3-------------------------------||
| |===J6===|                                                   |=====J4=====||
|                                                    JPR3              |-J7-||
|              |=============J2=============||=============J1===============||
+----------------------------------------------------------------------------+

J1 80-pin connector to J1 connector on the CRT-350 mainboard
J2 80-pin connector to J2 connector on the CRT-350 mainboard
J3 65-pin single row connector for wire harness
J4 40-pin dual row connector for printer
J5 16-pin dual row connector for unknown
J6 17-pin single row connector for unknown (some kind of jumper block)
J7 6-pin single row connector for hopper
JPR3 is a 3 pin jumper: Pins 1&2 = Printer, pins 2&3= Hopper


Main PCB graphics roms (on main board):

U46
MLT8
ck:8bbe

U47
MLT8
ck:0262

U48
MLT8
ck:9daa



Program ROMs on Expansion board:

U11 *Empty       U15
                 7556-01-r0
                 add3

U10              U14
7556-01-r0       7556-01-r0
7c21             dff2

U9               U13
7556-01-r0       7556-01-r0
ef1e             7c21

U8               U12
7556-01-r0       7556-00-r2
23c6


According to U14:
 INVALID DIPSWITCH SETTING
 ENABLE AT LEAST TWO GAMES
  CS1-1 ON =JOKER POKER
  CS1-2 ON =DEUCES
  CS1-3 ON =FEVER POKER
  CS1-4 ON =JACKS POKER
  CS1-5 ON =BLACKJACK
  CS1-6 ON =KENO WILD
  CS1-7 ON =TBALL KENO
  CS1-8 ON =BINGO
 CSW1-1 ON =DOLR JACKS
 CSW1-2 ON =DOLR DEUCE
 CSW1-3 ON =5# KENO
 CSW1-4 ON =TREASURE

Dipswitch on CRT-350 main is labeled S1
Dipswitch on CRT-352 MEM is labeled SW1
*/

#include "emu.h"
#include "screen.h"
#include "cpu/z80/z80.h"
#include "machine/ds1204.h"
#include "machine/i8255.h"
#include "machine/ins8250.h"
#include "machine/microtch.h"
#include "machine/nvram.h"
#include "sound/ay8910.h"
#include "video/mc6845.h"

class merit3xx_state : public driver_device
{
public:
	merit3xx_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu")
	{ }

	void merit3xx(machine_config &config);

private:
	required_device<cpu_device> m_maincpu;

	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void main_map(address_map &map);
};


uint32_t merit3xx_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	return 0;
}

void merit3xx_state::main_map(address_map &map)
{
}

static INPUT_PORTS_START( merit3xx )
INPUT_PORTS_END


void merit3xx_state::merit3xx(machine_config &config)
{
	Z80(config, m_maincpu, 10_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &merit3xx_state::main_map);
}



ROM_START( ma6710 )
	ROM_REGION(0x20000, "maincpu", 0)
	ROM_LOAD( "u-1_dc-350_ticket.u1", 0x00000, 0x10000, CRC(33aa53ce) SHA1(828d6f4828d5d90777c573a6870d800ae6a51425) )
	ROM_LOAD( "u-2_dc-350_ticket.u2", 0x10000, 0x10000, CRC(fcac2391) SHA1(df9a1834441569fef876594aaef7d364831dbae6) )

	ROM_REGION( 0x30000, "gfx1", 0 )
	ROM_LOAD( "u-46_dc-350.u46", 0x00000, 0x10000, CRC(3765a026) SHA1(cdb47d4b3775bec4b3ab16636d795ad737344166) )
	ROM_LOAD( "u-47_dc-350.u47", 0x10000, 0x10000, CRC(bbcf8280) SHA1(83c6fd84bdd09dd82506d81be1cbae797fd59347) )
	ROM_LOAD( "u-48_dc-350.u48", 0x20000, 0x10000, CRC(b93a0481) SHA1(df60d81fb68bd868ce94f8b313896d6d31e54ad4) )

	ROM_REGION( 0x4000, "nvram", 0 )
	ROM_LOAD( "ds1225y.u6", 0x0000, 0x2000, CRC(78fd0284) SHA1(37aa7deaafc6faad7505cd56a442913b35f54166) )
	ROM_LOAD( "bq4010.u5",  0x2000, 0x2000, CRC(003ea272) SHA1(3f464a0189af49470b33825a00905df6b156913f) )
ROM_END


ROM_START( ma7551 )
	ROM_REGION(0x40000, "maincpu", 0)
	ROM_LOAD( "u8_7551-21-r2p.u8",   0x00000, 0x08000, CRC(a2ae7a03) SHA1(2d923cf068fd1b9bd5f48a110f5155b876b9ba37) )
	ROM_LOAD( "u9_7551-21-r2p.u9",   0x08000, 0x08000, CRC(2e669bc9) SHA1(376e808a62e92169a5ae34b9ef808fe4eda6c13c) )
	ROM_LOAD( "u10_7551-21-r2p.u10", 0x10000, 0x08000, CRC(e9425269) SHA1(030a3d9beafd08c5a571672fb6987525c8d9a0f5) )
	// u11 not populated
	ROM_LOAD( "u12_7551-21-r2p.u12", 0x20000, 0x08000, CRC(8ca19c9c) SHA1(a694a9be8b6d2beea8ee171dcfb2fa64eb6af14c) )
	ROM_LOAD( "u13_7551-21-r2p.u13", 0x28000, 0x08000, CRC(9194d993) SHA1(52d094f55c329a7f0b4bf1dd02a7784e9a9faa12) )
	ROM_LOAD( "u14_7551-21-r2p.u14", 0x30000, 0x08000, CRC(fe993b57) SHA1(4c872b3dff278298558493f6fd9a64be63613956) )
	ROM_LOAD( "u15_7551-21-r2p.u15", 0x38000, 0x08000, CRC(31283190) SHA1(153601d5df7fbbc116f876399ce194797175be2f) )

	ROM_REGION( 0x30000, "gfx1", 0 )
	ROM_LOAD( "u46_nc+.u46", 0x00000, 0x10000, CRC(5140ca67) SHA1(0f5f7062cd874529630fd6f58e640c11f0692786) )
	ROM_LOAD( "u47_nc+.u47", 0x10000, 0x10000, CRC(5f1d8ffa) SHA1(c8fe36f91ddd634e6d66434342b8dafdc1ffa332) )
	ROM_LOAD( "u48_nc+.u48", 0x20000, 0x10000, CRC(1ef22a70) SHA1(f33db37dc6e2ded3a39907eb5f5ea6306fd6f8b0) )

	ROM_REGION( 0xa000, "nvram", 0 )
	ROM_LOAD( "dallas_ds1225y-150.u7", 0x0000, 0x2000, CRC(2526c25c) SHA1(fe7d54e65dc7bd93576f496160f63b3c8e8c128b) )
	ROM_LOAD( "dallas_ds1230y-120.u7", 0x2000, 0x8000, CRC(54099035) SHA1(2a8854a862bc24ff72470660e60e9e4228158b42) )
ROM_END


ROM_START( ma7556 )
	ROM_REGION(0x40000, "maincpu", 0)
	ROM_LOAD( "u8_7556-01-r0_23c6.u8",   0x00000, 0x08000, CRC(4dfca3d2) SHA1(2d8cc59edad12368dbc267b763af46e095599bc0) )
	ROM_LOAD( "u9_7556-01-r0_ef1e.u9",   0x08000, 0x08000, CRC(142370d6) SHA1(cb32f204b7bf78874990ef438fd5115cc3ed140e) )
	ROM_LOAD( "u10_7556-01-r0_ef8f.u10", 0x10000, 0x08000, CRC(f2dfb326) SHA1(b50a234ad649d41fb50c6eec345fa9414de6cec9) )
	// u11 not populated
	ROM_LOAD( "u12_7556-00-r2.u12",      0x20000, 0x08000, CRC(34357c5d) SHA1(f71db3cd5ced70a709ecb8de1328c12666abc047) )
	ROM_LOAD( "u13_7556-01-r0_7c21.u13", 0x28000, 0x08000, CRC(5288eecc) SHA1(efd569beb22b8a9354520e7755bd797724593a0a) )
	ROM_LOAD( "u14_7556-01-r0_dff2.u14", 0x30000, 0x08000, CRC(9e5518c1) SHA1(37ed33118d87f0699845f84c820569666ac8c533) )
	ROM_LOAD( "u15_7556-01-r0_add3.u15", 0x38000, 0x08000, CRC(83e5f4cd) SHA1(15b999169b28fb267ec8a265c915c1d366e57655) )

	ROM_REGION( 0x30000, "gfx1", 0 )
	ROM_LOAD( "u46_mlt8_ck-8bbe.u46", 0x00000, 0x10000, CRC(32c11634) SHA1(26f3c5c220b45e8eedad940ff94dc5ef6f89e3fa) )
	ROM_LOAD( "u47_mlt8_ck-0262.u47", 0x10000, 0x10000, CRC(5781bdd7) SHA1(e3f920dd1c247f92044100e28fc39d48b02b6a4b) )
	ROM_LOAD( "u48_mlt8_ck-9daa.u48", 0x20000, 0x10000, CRC(52ac8411) SHA1(9941388b90b6b91c1dab9286db588f0032620ea4) )

	ROM_REGION( 0xa000, "nvram", 0 )
	ROM_LOAD( "dallas_ds1225y-200.u7",  0x0000, 0x2000, BAD_DUMP CRC(5b635a95) SHA1(dd347258ba9e000963da75af5ac383c09b60be0b) )
	ROM_LOAD( "dallas_ds1230y-200.u17", 0x2000, 0x8000, BAD_DUMP CRC(e0c07037) SHA1(c6674a79a51f5aacca4a9e9bd19a2ce475c98b47) )
ROM_END

// CRT-300 games
GAME( 1989, ma6710, 0, merit3xx, merit3xx, merit3xx_state, empty_init, ROT0, "Merit", "Multi-Action 6710-13", MACHINE_IS_SKELETON )

// CRT-350 games
GAME( 199?, ma7551, 0, merit3xx, merit3xx, merit3xx_state, empty_init, ROT0, "Merit", "Multi-Action 7551", MACHINE_IS_SKELETON )
GAME( 199?, ma7556, 0, merit3xx, merit3xx, merit3xx_state, empty_init, ROT0, "Merit", "Multi-Action 7556", MACHINE_IS_SKELETON )
