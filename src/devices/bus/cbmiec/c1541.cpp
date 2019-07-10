// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Commodore 1540/1541/1541C/1541-II Single Disk Drive emulation

**********************************************************************/

/*

    TODO:

    - c1540 fails to load the directory intermittently

    - hardware extensions
        - Dolphin-DOS 2.0
        - Dolphin-DOS 3.0
        - Professional-DOS
        - Prologic-DOS

*/

/*

    1540/1541/1541A/SX-64 Parts

    Location       Part Number    Description
                                  2016 2K x 8 bit Static RAM (short board)
    UA2-UB3                       2114 (4) 1K x 4 bit Static RAM (long board)
                   325572-01      64H105 40 pin Gate Array (short board)
                   325302-01      2364-130 ROM DOS 2.6 C000-DFFF
                   325303-01      2364-131 ROM DOS 2.6 (1540) E000-FFFF
                   901229-01      2364-173 ROM DOS 2.6 rev. 0 E000-FFFF
                   901229-03      2364-197 ROM DOS 2.6 rev. 1 E000-FFFF
                   901229-05      8K ROM DOS 2.6 rev. 2 E000-FFFF
                                  6502 CPU
                                  6522 (2) VIA
    drive                         Alps DFB111M25A
    drive                         Alps FDM2111-B2
    drive                         Newtronics D500

    1541B/1541C Parts

    Location       Part Number    Description
    UA3                           2016 2K x 8 bit Static RAM
    UC2                           6502 CPU
    UC1, UC3                      6522 (2) CIA
    UC4            251828-02      64H156 42 pin Gate Array
    UC5            251829-01      64H157 20 pin Gate Array
    UD1          * 251853-01      R/W Hybrid
    UD1          * 251853-02      R/W Hybrid
    UA2            251968-01      27128 EPROM DOS 2.6 rev. 3 C000-FFFF
    drive                         Newtronics D500
      * Not interchangeable.

    1541-II Parts

    Location       Part Number    Description
    U5                            2016-15 2K x 8 bit Static RAM
    U12                           SONY CX20185 R/W Amp.
    U3                            6502A CPU
    U6, U8                        6522 (2) CIA
    U10            251828-01      64H156 40 pin Gate Array
    U4             251968-03      16K ROM DOS 2.6 rev. 4 C000-FFFF
    drive                         Chinon FZ-501M REV A
    drive                         Digital System DS-50F
    drive                         Newtronics D500
    drive                         Safronic DS-50F

    ...

    PCB Assy # 1540008-01
    Schematic # 1540001
    Original "Long" Board
    Has 4 discreet 2114 RAMs
    ALPS Drive only

    PCB Assy # 1540048
    Schematic # 1540049
    Referred to as the CR board
    Changed to 2048 x 8 bit RAM pkg.
    A 40 pin Gate Array is used
    Alps Drive (-01)
    Newtronics Drive (-03)

    PCB Assy # 250442-01
    Schematic # 251748
    Termed the 1541 A
    Just one jumper change to accommodate both types of drive

    PCB Assy # 250446-01
    Schematic # 251748 (See Notes)
    Termed the 1541 A-2
    Just one jumper change to accommodate both types of drive

    ...

    VIC1541 1540001-01   Very early version, long board.
            1540001-03   As above, only the ROMs are different.
            1540008-01

    1541    1540048-01   Shorter board with a 40 pin gate array, Alps mech.
            1540048-03   As above, but Newtronics mech.
            1540049      Similar to above
            1540050      Similar to above, Alps mechanism.

    SX64    250410-01    Design most similar to 1540048-01, Alps mechanism.

    1541    251777       Function of bridge rects. reversed, Newtronics mech.
            251830       Same as above

    1541A   250442-01    Alps or Newtronics drive selected by a jumper.
    1541A2  250446-01    A 74LS123 replaces the 9602 at UD4.
    1541B   250448-01    Same as the 1541C, but in a case like the 1541.
    1541C   250448-01    Short board, new 40/42 pin gate array, 20 pin gate
                         array and a R/W hybrid chip replace many components.
                         Uses a Newtronics drive with optical trk 0 sensor.
    1541C   251854       As above, single DOS ROM IC, trk 0 sensor, 30 pin
                         IC for R/W ampl & stepper motor control (like 1541).

    1541-II              A complete redesign using the 40 pin gate array
                         from the 1451C and a Sony R/W hybrid, but not the
                         20 pin gate array, single DOS ROM IC.

    NOTE: These system boards are the 60 Hz versions.
          The -02 and -04 boards are probably the 50 Hz versions.

    The ROMs appear to be completely interchangeable. For instance, the
    first version of ROM for the 1541-II contained the same code as the
    last version of the 1541. I copy the last version of the 1541-II ROM
    into two 68764 EPROMs and use them in my original 1541 (long board).
    Not only do they work, but they work better than the originals.


    http://www.amiga-stuff.com/hardware/cbmboards.html

*/

#include "emu.h"
#include "c1541.h"
#include "bus/centronics/ctronics.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define M6502_TAG       "ucd5"
#define M6522_0_TAG     "uab1"
#define M6522_1_TAG     "ucd4"
#define C64H156_TAG     "uc4"
#define C64H157_TAG     "uc5"

#define MC6821_TAG      "pia"
#define CENTRONICS_TAG  "centronics"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(C1540,                      c1540_t,                      "c1540",    "Commodore 1540 Disk Drive")
DEFINE_DEVICE_TYPE(C1541,                      c1541_t,                      "c1541",    "Commodore 1541 Disk Drive")
DEFINE_DEVICE_TYPE(C1541C,                     c1541c_t,                     "c1541c",   "Commodore 1541C Disk Drive")
DEFINE_DEVICE_TYPE(C1541II,                    c1541ii_t,                    "c1541ii",  "Commodore 1541-II Disk Drive")
DEFINE_DEVICE_TYPE(SX1541,                     sx1541_t,                     "sx1541",   "SX1541 Disk Drive")
DEFINE_DEVICE_TYPE(FSD1,                       fsd1_t,                       "fsd1",     "FSD-1 Disk Drive")
DEFINE_DEVICE_TYPE(FSD2,                       fsd2_t,                       "fsd2",     "FSD-2 Disk Drive")
DEFINE_DEVICE_TYPE(CSD1,                       csd1_t,                       "csd1",     "CSD-1 Disk Drive")
DEFINE_DEVICE_TYPE(C1541_DOLPHIN_DOS,          c1541_dolphin_dos_t,          "c1541dd",  "Commodore 1541 Dolphin-DOS 2.0 Disk Drive")
DEFINE_DEVICE_TYPE(C1541_PROFESSIONAL_DOS_V1,  c1541_professional_dos_v1_t,  "c1541pd",  "Commodore 1541 Professional-DOS v1 Disk Drive")
DEFINE_DEVICE_TYPE(C1541_PROLOGIC_DOS_CLASSIC, c1541_prologic_dos_classic_t, "c1541pdc", "Commodore 1541 ProLogic-DOS Classic Disk Drive")
DEFINE_DEVICE_TYPE(INDUS_GT,                   indus_gt_t,                   "indusgt",  "Indus GT Disk Drive")
DEFINE_DEVICE_TYPE(TECHNICA,                   technica_t,                   "technica", "Westfalia Technica Disk Drive")
DEFINE_DEVICE_TYPE(BLUE_CHIP,                  blue_chip_t,                  "bluechip", "Amtech Blue Chip Disk Drive")
DEFINE_DEVICE_TYPE(COMMANDER_C2,               commander_c2_t,               "cmdrc2",   "Commander C-II Disk Drive")
DEFINE_DEVICE_TYPE(ENHANCER_2000,              enhancer_2000_t,              "enh2000",  "Enhancer 2000 Disk Drive")
DEFINE_DEVICE_TYPE(FD148,                      fd148_t,                      "fd148",    "Rapid Access FD-148 Disk Drive")
DEFINE_DEVICE_TYPE(MSD_SD1,                    msd_sd1_t,                    "msdsd1",   "MSD SD-1 Disk Drive")
DEFINE_DEVICE_TYPE(MSD_SD2,                    msd_sd2_t,                    "msdsd2",   "MSD SD-2 Disk Drive")


//-------------------------------------------------
//  ROM( c1540 )
//-------------------------------------------------

ROM_START( c1540 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "325302-01.uab4", 0x0000, 0x2000, CRC(29ae9752) SHA1(8e0547430135ba462525c224e76356bd3d430f11) )
	ROM_LOAD( "325303-01.uab5", 0x2000, 0x2000, CRC(10b39158) SHA1(56dfe79b26f50af4e83fd9604857756d196516b9) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *c1540_t::device_rom_region() const
{
	return ROM_NAME( c1540 );
}


//-------------------------------------------------
//  ROM( c1541 )
//-------------------------------------------------

ROM_START( c1541 )
	ROM_REGION( 0x8000, M6502_TAG, 0 )
	ROM_LOAD( "325302-01.uab4", 0x0000, 0x2000, CRC(29ae9752) SHA1(8e0547430135ba462525c224e76356bd3d430f11) )

	ROM_DEFAULT_BIOS("r6")
	ROM_SYSTEM_BIOS( 0, "r1", "Revision 1" )
	ROMX_LOAD( "901229-01.uab5", 0x2000, 0x2000, CRC(9a48d3f0) SHA1(7a1054c6156b51c25410caec0f609efb079d3a77), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "r2", "Revision 2" )
	ROMX_LOAD( "901229-02.uab5", 0x2000, 0x2000, CRC(b29bab75) SHA1(91321142e226168b1139c30c83896933f317d000), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 2, "r3", "Revision 3" )
	ROMX_LOAD( "901229-03.uab5", 0x2000, 0x2000, CRC(9126e74a) SHA1(03d17bd745066f1ead801c5183ac1d3af7809744), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 3, "r4", "Revision 4" )
	ROMX_LOAD( "901229-04.uab5", 0x2000, 0x2000, NO_DUMP, ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 4, "r5", "Revision 5" )
	ROMX_LOAD( "901229-05 ae.uab5", 0x2000, 0x2000, CRC(361c9f37) SHA1(f5d60777440829e46dc91285e662ba072acd2d8b), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 5, "r6", "Revision 6" )
	ROMX_LOAD( "901229-06 aa.uab5", 0x2000, 0x2000, CRC(3a235039) SHA1(c7f94f4f51d6de4cdc21ecbb7e57bb209f0530c0), ROM_BIOS(5) )
	ROM_SYSTEM_BIOS( 6, "jiffydos", "JiffyDOS v6.01" )
	ROMX_LOAD( "jiffydos 1541.uab5", 0x2000, 0x2000, CRC(bc7e4aeb) SHA1(db6cfaa6d9b78d58746c811de29f3b1f44d99ddf), ROM_BIOS(6) )
	ROM_SYSTEM_BIOS( 7, "speeddos", "SpeedDOS-Plus+" )
	ROMX_LOAD( "speed-dosplus.uab5", 0x0000, 0x4000, CRC(f9db1eac) SHA1(95407e59a9c1d26a0e4bcf2c244cfe8942576e2c), ROM_BIOS(7) )
	ROM_SYSTEM_BIOS( 8, "rolo27", "Rolo DOS v2.7" )
	ROMX_LOAD( "rolo27.uab5", 0x0000, 0x2000, CRC(171c7962) SHA1(04c892c4b3d7c74750576521fa081f07d8ca8557), ROM_BIOS(8) )
	ROM_SYSTEM_BIOS( 9, "tt34", "TurboTrans v3.4" )
	ROMX_LOAD( "ttd34.uab5", 0x0000, 0x8000, CRC(518d34a1) SHA1(4d6ffdce6ab122e9627b0a839861687bcd4e03ec), ROM_BIOS(9) )
	ROM_SYSTEM_BIOS( 10, "digidos", "DigiDOS" )
	ROMX_LOAD( "digidos.uab5", 0x0000, 0x2000, CRC(b3f05ea3) SHA1(99d3d848344c68410b686cda812f3788b41fead3), ROM_BIOS(10) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *c1541_t::device_rom_region() const
{
	return ROM_NAME( c1541 );
}


//-------------------------------------------------
//  ROM( c1541c )
//-------------------------------------------------

ROM_START( c1541c )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_DEFAULT_BIOS("r2")
	ROM_SYSTEM_BIOS( 0, "r1", "Revision 1" )
	ROMX_LOAD( "251968-01.ua2", 0x0000, 0x4000, CRC(1b3ca08d) SHA1(8e893932de8cce244117fcea4c46b7c39c6a7765), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "r2", "Revision 2" )
	ROMX_LOAD( "251968-02.ua2", 0x0000, 0x4000, CRC(2d862d20) SHA1(38a7a489c7bbc8661cf63476bf1eb07b38b1c704), ROM_BIOS(1) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *c1541c_t::device_rom_region() const
{
	return ROM_NAME( c1541c );
}


//-------------------------------------------------
//  ROM( c1541ii )
//-------------------------------------------------

ROM_START( c1541ii )
	ROM_REGION( 0x8000, M6502_TAG, 0 )
	ROM_LOAD( "251968-03.u4", 0x0000, 0x4000, CRC(899fa3c5) SHA1(d3b78c3dbac55f5199f33f3fe0036439811f7fb3) )

	ROM_DEFAULT_BIOS("r1")
	ROM_SYSTEM_BIOS( 0, "r1", "Revision 1" )
	ROMX_LOAD( "355640-01.u4", 0x0000, 0x4000, CRC(57224cde) SHA1(ab16f56989b27d89babe5f89c5a8cb3da71a82f0), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "jiffydos", "JiffyDOS v6.01" )
	ROMX_LOAD( "jiffydos 1541-ii.u4", 0x0000, 0x4000, CRC(dd409902) SHA1(b1a5b826304d3df2a27d7163c6a81a532e040d32), ROM_BIOS(1) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *c1541ii_t::device_rom_region() const
{
	return ROM_NAME( c1541ii );
}


//-------------------------------------------------
//  ROM( sx1541 )
//-------------------------------------------------

ROM_START( sx1541 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "325302-01.uab4",    0x0000, 0x2000, CRC(29ae9752) SHA1(8e0547430135ba462525c224e76356bd3d430f11) )

	ROM_DEFAULT_BIOS("r5")
	ROM_SYSTEM_BIOS( 0, "r5", "Revision 5" )
	ROMX_LOAD( "901229-05 ae.uab5", 0x2000, 0x2000, CRC(361c9f37) SHA1(f5d60777440829e46dc91285e662ba072acd2d8b), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "jiffydos", "JiffyDOS v6.01" )
	ROMX_LOAD( "jiffydos sx1541",   0x0000, 0x4000, CRC(783575f6) SHA1(36ccb9ff60328c4460b68522443ecdb7f002a234), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 2, "flash", "1541 FLASH!" )
	ROMX_LOAD( "1541 flash.uab5",   0x2000, 0x2000, CRC(22f7757e) SHA1(86a1e43d3d22b35677064cca400a6bd06767a3dc), ROM_BIOS(2) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *sx1541_t::device_rom_region() const
{
	return ROM_NAME( sx1541 );
}


//-------------------------------------------------
//  ROM( fsd1 )
//-------------------------------------------------

ROM_START( fsd1 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "fsd1.bin", 0x0000, 0x4000, CRC(57224cde) SHA1(ab16f56989b27d89babe5f89c5a8cb3da71a82f0) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *fsd1_t::device_rom_region() const
{
	return ROM_NAME( fsd1 );
}


//-------------------------------------------------
//  ROM( fsd2 )
//-------------------------------------------------

ROM_START( fsd2 )
	ROM_REGION( 0x4000, M6502_TAG, 0 ) // data lines D3 and D4 are swapped
	ROM_DEFAULT_BIOS("rb")
	ROM_SYSTEM_BIOS( 0, "ra", "Revision A" )
	ROMX_LOAD( "fsd2a.u3", 0x0000, 0x4000, CRC(edf18265) SHA1(47a7c4bdcc20ecc5e59d694b151f493229becaea), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "rb", "Revision B" )
	ROMX_LOAD( "fsd2b.u3", 0x0000, 0x4000, CRC(b39e4600) SHA1(991132fcc6e70e9a428062ae76055a150f2f7ac6), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 2, "jiffydos", "JiffyDOS v5.0" )
	ROMX_LOAD( "jiffydos v5.0.u3", 0x0000, 0x4000, CRC(46c3302c) SHA1(e3623658cb7af30c9d3bce2ba3b6ad5ee89ac1b8), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 3, "rexdos", "REX-DOS" )
	ROMX_LOAD( "rdos.bin", 0x0000, 0x4000, CRC(8ad6dba1) SHA1(f279d327d5e16ea1b62fb18514fb679d0b442941), ROM_BIOS(3) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *fsd2_t::device_rom_region() const
{
	return ROM_NAME( fsd2 );
}


//-------------------------------------------------
//  ROM( csd1 )
//-------------------------------------------------

ROM_START( csd1 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "ic14", 0x0000, 0x2000, CRC(adb6980e) SHA1(13051587dfe43b04ce1bf354b89438ddf6d8d76b) )
	ROM_LOAD( "ic15", 0x2000, 0x2000, CRC(b0cecfa1) SHA1(c67e79a7ffefc9e9eafc238cb6ff6bb718f19afb) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *csd1_t::device_rom_region() const
{
	return ROM_NAME( csd1 );
}


//-------------------------------------------------
//  ROM( c1541dd )
//-------------------------------------------------

ROM_START( c1541dd )
	ROM_REGION( 0x8000, M6502_TAG, 0 )
	ROM_LOAD( "dd20.bin", 0x0000, 0x8000, CRC(94c7fe19) SHA1(e4d5b9ad6b719dd988276214aa4536d3525d313c) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *c1541_dolphin_dos_t::device_rom_region() const
{
	return ROM_NAME( c1541dd );
}


//-------------------------------------------------
//  ROM( c1541pd )
//-------------------------------------------------

ROM_START( c1541pd )
	ROM_REGION( 0x6000, M6502_TAG, 0 )
	ROM_LOAD( "325302-01.uab4", 0x0000, 0x2000, CRC(29ae9752) SHA1(8e0547430135ba462525c224e76356bd3d430f11) )
	ROM_LOAD( "professionaldos-v1-floppy-expansion-eprom-27128.bin", 0x2000, 0x4000, CRC(c9abf072) SHA1(2b26adc1f4192b6ca1514754f73c929087b24426) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *c1541_professional_dos_v1_t::device_rom_region() const
{
	return ROM_NAME( c1541pd );
}


//-------------------------------------------------
//  ROM( c1541pdc )
//-------------------------------------------------

ROM_START( c1541pdc )
	ROM_REGION( 0x8000, M6502_TAG, 0 )
	ROM_LOAD( "325302-01.uab4", 0x0000, 0x2000, CRC(29ae9752) SHA1(8e0547430135ba462525c224e76356bd3d430f11) )
	ROM_LOAD( "901229-06 aa.uab5", 0x2000, 0x2000, CRC(3a235039) SHA1(c7f94f4f51d6de4cdc21ecbb7e57bb209f0530c0) )
	ROM_LOAD( "kernal.bin", 0x4000, 0x4000, CRC(79032ed5) SHA1(0ca4d5ef41c7e3d18d8945476d1481573af3e27c) )

	ROM_REGION( 0x2000, "mmu", 0 )
	ROM_LOAD( "mmu.bin", 0x0000, 0x2000, CRC(4c41392c) SHA1(78846af2ee6a56fceee44f9246659685ab2cbb7e) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *c1541_prologic_dos_classic_t::device_rom_region() const
{
	return ROM_NAME( c1541pdc );
}


//-------------------------------------------------
//  ROM( indusgt )
//-------------------------------------------------

ROM_START( indusgt )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "u18 v1.1.u18", 0x0000, 0x2000, CRC(e401ce56) SHA1(9878053bdff7a036f57285c2c4974459df2602d8) )
	ROM_LOAD( "u17 v1.1.u17", 0x2000, 0x2000, CRC(575ad906) SHA1(f48837b024add84f888acd83a9cf9eb7d2379172) )

	ROM_REGION( 0x2000, "romdisk", 0 )
	ROM_LOAD( "u19 v1.1.u19", 0x0000, 0x2000, CRC(8f83e7a5) SHA1(5bceaad520dac9d0527723b3b454e8ec99748e5b) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *indus_gt_t::device_rom_region() const
{
	return ROM_NAME( indusgt );
}


//-------------------------------------------------
//  ROM( technica )
//-------------------------------------------------

ROM_START( technica )
	ROM_REGION( 0x4000, M6502_TAG, 0 ) // data lines should be scrambled
	ROM_LOAD( "technica dos plus.bin", 0x0000, 0x4000, BAD_DUMP CRC(6a1ef3ff) SHA1(1aaa52ed4a3f120ec8664bcefec890c7f9aaecf2) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *technica_t::device_rom_region() const
{
	return ROM_NAME( technica );
}


//-------------------------------------------------
//  ROM( bluechip )
//-------------------------------------------------

ROM_START( bluechip )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_SYSTEM_BIOS( 0, "1", "1" )
	ROMX_LOAD( "bluechip_fd_stockrom.bin", 0x0000, 0x4000, CRC(d4293619) SHA1(18b3dc4c2f919ac8f288d0199e29993a0b53a9bd), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "2", "2" )
	ROMX_LOAD( "amtech_bluechip_rom.bin", 0x0000, 0x4000, CRC(3733ccea) SHA1(c11317cb9370e722950579a610a3effda313aeee), ROM_BIOS(1) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *blue_chip_t::device_rom_region() const
{
	return ROM_NAME( bluechip );
}


//-------------------------------------------------
//  ROM( cmdrc2 )
//-------------------------------------------------

ROM_START( cmdrc2 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "commander_c-ii_8k_rom1.bin", 0x0000, 0x2000, CRC(cb19daf3) SHA1(9fab414451af54d0bed9d4c9fd5fab1b8720c269) )
	ROM_LOAD( "commander_c-ii_8k_rom2.bin", 0x2000, 0x2000, CRC(ed85a390) SHA1(eecf92fb8cc20a6c86e30f897d09d427509dd3d3) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *commander_c2_t::device_rom_region() const
{
	return ROM_NAME( cmdrc2 );
}


//-------------------------------------------------
//  ROM( enh2000 )
//-------------------------------------------------

ROM_START( enh2000 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "enhancer 2000 comtel 2.6.bin", 0x0000, 0x4000, CRC(20353d3b) SHA1(473dd2e06037799e6f562c443165d9b2b9f4a368) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *enhancer_2000_t::device_rom_region() const
{
	return ROM_NAME( enh2000 );
}


//-------------------------------------------------
//  ROM( fd148 )
//-------------------------------------------------

ROM_START( fd148 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "rapid access fd148.bin", 0x0000, 0x4000, CRC(3733ccea) SHA1(c11317cb9370e722950579a610a3effda313aeee) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *fd148_t::device_rom_region() const
{
	return ROM_NAME( fd148 );
}


//-------------------------------------------------
//  ROM( msdsd1 )
//-------------------------------------------------

ROM_START( msdsd1 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "sd-1-1.3-c000.bin", 0x0000, 0x2000, CRC(f399778d) SHA1(c0d939c354d84018038c60a231fc43fb9279d8a4) )
	ROM_LOAD( "sd-1-1.3-e000.bin", 0x2000, 0x2000, CRC(7ac80da4) SHA1(99dd15c6d97938eba73880b18986a037e90742ab) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *msd_sd1_t::device_rom_region() const
{
	return ROM_NAME( msdsd1 );
}


//-------------------------------------------------
//  ROM( msdsd2 )
//-------------------------------------------------

ROM_START( msdsd2 )
	ROM_REGION( 0x4000, M6502_TAG, 0 )
	ROM_LOAD( "sd-2-2.3-c000.bin", 0x0000, 0x2000, CRC(2207560e) SHA1(471e9b4a4ac09ceee9acc1774534510396f98b9a) )
	ROM_LOAD( "sd-2-2.3-e000.bin", 0x2000, 0x2000, CRC(4efd87a2) SHA1(4beec0b7ce2349add3b0a5bceee60826637df8d9) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *msd_sd2_t::device_rom_region() const
{
	return ROM_NAME( msdsd2 );
}


//-------------------------------------------------
//  read -
//-------------------------------------------------

READ8_MEMBER( c1541_prologic_dos_classic_t::read )
{
	return 0;
}


//-------------------------------------------------
//  write -
//-------------------------------------------------

WRITE8_MEMBER( c1541_prologic_dos_classic_t::write )
{
}


//-------------------------------------------------
//  ADDRESS_MAP( c1541_mem )
//-------------------------------------------------

void c1541_base_t::c1541_mem(address_map &map)
{
	map(0x0000, 0x07ff).mirror(0x6000).ram();
	map(0x1800, 0x180f).mirror(0x63f0).m(M6522_0_TAG, FUNC(via6522_device::map));
	map(0x1c00, 0x1c0f).mirror(0x63f0).m(M6522_1_TAG, FUNC(via6522_device::map));
	map(0x8000, 0xbfff).mirror(0x4000).rom().region(M6502_TAG, 0);
}


//-------------------------------------------------
//  ADDRESS_MAP( c1541dd_mem )
//-------------------------------------------------

void c1541_base_t::c1541dd_mem(address_map &map)
{
	map(0x0000, 0x07ff).mirror(0x6000).ram();
	map(0x1800, 0x180f).mirror(0x63f0).m(M6522_0_TAG, FUNC(via6522_device::map));
	map(0x1c00, 0x1c0f).mirror(0x63f0).m(M6522_1_TAG, FUNC(via6522_device::map));
	map(0x8000, 0x9fff).ram();
	map(0xa000, 0xffff).rom().region(M6502_TAG, 0x2000);
}


//-------------------------------------------------
//  ADDRESS_MAP( c1541pd_mem )
//-------------------------------------------------

void c1541_base_t::c1541pd_mem(address_map &map)
{
	map(0x0000, 0x07ff).mirror(0x6000).ram();
	map(0x1800, 0x180f).mirror(0x63f0).m(M6522_0_TAG, FUNC(via6522_device::map));
	map(0x1c00, 0x1c0f).mirror(0x63f0).m(M6522_1_TAG, FUNC(via6522_device::map));
	map(0x8000, 0x9fff).rom().region(M6502_TAG, 0x4000);
	map(0xa000, 0xbfff).ram();
	map(0xc000, 0xffff).rom().region(M6502_TAG, 0x0000);
}


//-------------------------------------------------
//  ADDRESS_MAP( c1541pdc_mem )
//-------------------------------------------------

void c1541_prologic_dos_classic_t::c1541pdc_mem(address_map &map)
{
	map(0x0000, 0xffff).rw(FUNC(c1541_prologic_dos_classic_t::read), FUNC(c1541_prologic_dos_classic_t::write));
/*  AM_RANGE(0x0000, 0x07ff) AM_MIRROR(0x6000) AM_RAM AM_SHARE("share1")
    AM_RANGE(0x1800, 0x180f) AM_MIRROR(0x63f0) AM_DEVREADWRITE(M6522_0_TAG, via6522_device, read, write)
    AM_RANGE(0x1c00, 0x1c0f) AM_MIRROR(0x63f0) AM_DEVREADWRITE(M6522_1_TAG, via6522_device, read, write)
    AM_RANGE(0x8000, 0x87ff) AM_RAM AM_SHARE("share1")
    AM_RANGE(0x8800, 0x9fff) AM_RAM
    AM_RANGE(0xa000, 0xb7ff) AM_ROM AM_REGION(M6502_TAG, 0x0000)
    AM_RANGE(0xb800, 0xb80f) AM_READWRITE(pia_r, pia_w)
    AM_RANGE(0xf000, 0xffff) AM_ROM AM_REGION(M6502_TAG, 0x2000)*/
}


WRITE_LINE_MEMBER( c1541_base_t::via0_irq_w )
{
	m_via0_irq = state;

	m_maincpu->set_input_line(INPUT_LINE_IRQ0, (m_via0_irq || m_via1_irq) ? ASSERT_LINE : CLEAR_LINE);
}

READ8_MEMBER( c1541_base_t::via0_pa_r )
{
	// dummy read to acknowledge ATN IN interrupt
	return m_parallel_data;
}

WRITE8_MEMBER( c1541_base_t::via0_pa_w )
{
	if (m_other != nullptr)
	{
		m_other->parallel_data_w(data);
	}
}

READ8_MEMBER( c1541_base_t::via0_pb_r )
{
	/*

	    bit     description

	    PB0     DATA IN
	    PB1
	    PB2     CLK IN
	    PB3
	    PB4
	    PB5     J1
	    PB6     J2
	    PB7     ATN IN

	*/

	uint8_t data;

	// data in
	data = !m_bus->data_r() && !m_ga->atn_r();

	// clock in
	data |= !m_bus->clk_r() << 2;

	// serial bus address
	data |= ((m_slot->get_address() - 8) & 0x03) << 5;

	// attention in
	data |= !m_bus->atn_r() << 7;

	return data;
}

WRITE8_MEMBER( c1541_base_t::via0_pb_w )
{
	/*

	    bit     description

	    PB0
	    PB1     DATA OUT
	    PB2
	    PB3     CLK OUT
	    PB4     ATNA
	    PB5
	    PB6
	    PB7

	*/

	// data out
	m_data_out = BIT(data, 1);

	// attention acknowledge
	m_ga->atna_w(BIT(data, 4));

	// clock out
	m_bus->clk_w(this, !BIT(data, 3));
}

WRITE_LINE_MEMBER( c1541_base_t::via0_ca2_w )
{
	if (m_other != nullptr)
	{
		m_other->parallel_strobe_w(state);
	}
}

READ8_MEMBER( c1541c_t::via0_pa_r )
{
	/*

	    bit     description

	    PA0     TR00 SENCE
	    PA1
	    PA2
	    PA3
	    PA4
	    PA5
	    PA6
	    PA7

	*/

	return !m_floppy->trk00_r();
}


WRITE_LINE_MEMBER( c1541_base_t::via1_irq_w )
{
	m_via1_irq = state;

	m_maincpu->set_input_line(INPUT_LINE_IRQ0, (m_via0_irq || m_via1_irq) ? ASSERT_LINE : CLEAR_LINE);
}

READ8_MEMBER( c1541_base_t::via1_pb_r )
{
	/*

	    bit     signal      description

	    PB0
	    PB1
	    PB2
	    PB3
	    PB4     WPS         write protect sense
	    PB5
	    PB6
	    PB7     SYNC        SYNC detect line

	*/

	uint8_t data = 0;

	// write protect sense
	data |= !m_floppy->wpt_r() << 4;

	// SYNC detect line
	data |= m_ga->sync_r() << 7;

	return data;
}

WRITE8_MEMBER( c1541_base_t::via1_pb_w )
{
	/*

	    bit     signal      description

	    PB0     STP0        stepping motor bit 0
	    PB1     STP1        stepping motor bit 1
	    PB2     MTR         motor ON/OFF
	    PB3     ACT         drive 0 LED
	    PB4
	    PB5     DS0         density select 0
	    PB6     DS1         density select 1
	    PB7     SYNC        SYNC detect line

	*/

	// spindle motor
	m_ga->mtr_w(BIT(data, 2));

	// stepper motor
	m_ga->stp_w(data & 0x03);

	// activity LED
	m_leds[LED_ACT] = BIT(data, 3);

	// density select
	m_ga->ds_w((data >> 5) & 0x03);
}


//-------------------------------------------------
//  C64H156_INTERFACE( ga_intf )
//-------------------------------------------------

WRITE_LINE_MEMBER( c1541_base_t::atn_w )
{
	set_iec_data();
}

WRITE_LINE_MEMBER( c1541_base_t::byte_w )
{
	m_maincpu->set_input_line(M6502_SET_OVERFLOW, state);

	m_via1->write_ca1(state);
}


//-------------------------------------------------
//  FLOPPY_FORMATS( floppy_formats )
//-------------------------------------------------

FLOPPY_FORMATS_MEMBER( c1541_base_t::floppy_formats )
	FLOPPY_D64_FORMAT,
	FLOPPY_G64_FORMAT
FLOPPY_FORMATS_END


READ8_MEMBER( c1541_prologic_dos_classic_t::pia_r )
{
	return m_pia->read((offset >> 2) & 0x03);
}

WRITE8_MEMBER( c1541_prologic_dos_classic_t::pia_w )
{
	m_pia->write((offset >> 2) & 0x03, data);
}

WRITE8_MEMBER( c1541_prologic_dos_classic_t::pia_pa_w )
{
	/*

	    bit     description

	    0       1/2 MHz
	    1
	    2
	    3       35/40 tracks
	    4
	    5
	    6
	    7       Hi

	*/
}

READ8_MEMBER( c1541_prologic_dos_classic_t::pia_pb_r )
{
	return m_parallel_data;
}

WRITE8_MEMBER( c1541_prologic_dos_classic_t::pia_pb_w )
{
	m_parallel_data = data;

	m_cent_data_out->write(data);
}


//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void c1541_base_t::device_add_mconfig(machine_config &config)
{
	M6502(config, m_maincpu, XTAL(16'000'000)/16);
	m_maincpu->set_addrmap(AS_PROGRAM, &c1541_base_t::c1541_mem);
	config.m_perfect_cpu_quantum = subtag(M6502_TAG);

	VIA6522(config, m_via0, XTAL(16'000'000)/16);
	m_via0->readpa_handler().set(FUNC(c1541_base_t::via0_pa_r));
	m_via0->readpb_handler().set(FUNC(c1541_base_t::via0_pb_r));
	m_via0->writepa_handler().set(FUNC(c1541_base_t::via0_pa_w));
	m_via0->writepb_handler().set(FUNC(c1541_base_t::via0_pb_w));
	m_via0->cb2_handler().set(FUNC(c1541_base_t::via0_ca2_w));
	m_via0->irq_handler().set(FUNC(c1541_base_t::via0_irq_w));

	VIA6522(config, m_via1, XTAL(16'000'000)/16);
	m_via1->readpa_handler().set(C64H156_TAG, FUNC(c64h156_device::yb_r));
	m_via1->readpb_handler().set(FUNC(c1541_base_t::via1_pb_r));
	m_via1->writepa_handler().set(C64H156_TAG, FUNC(c64h156_device::yb_w));
	m_via1->writepb_handler().set(FUNC(c1541_base_t::via1_pb_w));
	m_via1->ca2_handler().set(C64H156_TAG, FUNC(c64h156_device::soe_w));
	m_via1->cb2_handler().set(C64H156_TAG, FUNC(c64h156_device::oe_w));
	m_via1->irq_handler().set(FUNC(c1541_base_t::via1_irq_w));

	C64H156(config, m_ga, XTAL(16'000'000));
	m_ga->atn_callback().set(FUNC(c1541_base_t::atn_w));
	m_ga->byte_callback().set(FUNC(c1541_base_t::byte_w));

	floppy_connector &connector(FLOPPY_CONNECTOR(config, C64H156_TAG":0", 0));
	connector.option_add("525ssqd", ALPS_3255190X);
	connector.set_default_option("525ssqd");
	connector.set_fixed(true);
	connector.set_formats(c1541_base_t::floppy_formats);
}


void c1541c_t::device_add_mconfig(machine_config &config)
{
	c1541_base_t::device_add_mconfig(config);
}


void c1541_dolphin_dos_t::device_add_mconfig(machine_config &config)
{
	c1541_base_t::device_add_mconfig(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &c1541_dolphin_dos_t::c1541dd_mem);
}


void c1541_professional_dos_v1_t::device_add_mconfig(machine_config &config)
{
	c1541_base_t::device_add_mconfig(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &c1541_professional_dos_v1_t::c1541pd_mem);
}


void c1541_prologic_dos_classic_t::device_add_mconfig(machine_config &config)
{
	c1541_base_t::device_add_mconfig(config);

	m_maincpu->set_addrmap(AS_PROGRAM, &c1541_prologic_dos_classic_t::c1541pdc_mem);

	PIA6821(config, m_pia, 0);
	m_pia->readpb_handler().set(FUNC(c1541_prologic_dos_classic_t::pia_pb_r));
	m_pia->writepa_handler().set(FUNC(c1541_prologic_dos_classic_t::pia_pa_w));
	m_pia->writepb_handler().set(FUNC(c1541_prologic_dos_classic_t::pia_pb_w));
	m_pia->ca2_handler().set(CENTRONICS_TAG, FUNC(centronics_device::write_strobe));

	centronics_device &centronics(CENTRONICS(config, CENTRONICS_TAG, centronics_devices, "printer"));
	centronics.ack_handler().set(MC6821_TAG, FUNC(pia6821_device::ca1_w));

	output_latch_device &cent_data_out(OUTPUT_LATCH(config, "cent_data_out", 0));
	centronics.set_output_latch(cent_data_out);
}


//-------------------------------------------------
//  INPUT_PORTS( c1541 )
//-------------------------------------------------

static INPUT_PORTS_START( c1541 )
	PORT_START("ADDRESS")
	PORT_DIPNAME( 0x03, 0x00, "Device Address" )
	PORT_DIPSETTING(    0x00, "8" )
	PORT_DIPSETTING(    0x01, "9" )
	PORT_DIPSETTING(    0x02, "10" )
	PORT_DIPSETTING(    0x03, "11" )
INPUT_PORTS_END


//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor c1541_base_t::device_input_ports() const
{
	return INPUT_PORTS_NAME( c1541 );
}



//**************************************************************************
//  INLINE HELPERS
//**************************************************************************

//-------------------------------------------------
//  set_iec_data -
//-------------------------------------------------

inline void c1541_base_t::set_iec_data()
{
	int data = !m_data_out && !m_ga->atn_r();

	m_bus->data_w(this, data);
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  c1541_base_t - constructor
//-------------------------------------------------

c1541_base_t::c1541_base_t(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, type, tag, owner, clock),
	device_cbm_iec_interface(mconfig, *this),
	device_c64_floppy_parallel_interface(mconfig, *this),
	m_floppy(*this, C64H156_TAG":0:525ssqd"),
	m_maincpu(*this, M6502_TAG),
	m_via0(*this, M6522_0_TAG),
	m_via1(*this, M6522_1_TAG),
	m_ga(*this, C64H156_TAG),
	m_address(*this, "ADDRESS"),
	m_leds(*this, "led%u", 0U),
	m_data_out(1),
	m_via0_irq(CLEAR_LINE),
	m_via1_irq(CLEAR_LINE)
{
}


//-------------------------------------------------
//  c1540_t - constructor
//-------------------------------------------------

c1540_t::c1540_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, C1540, tag, owner, clock) { }


//-------------------------------------------------
//  c1541_t - constructor
//-------------------------------------------------

c1541_t::c1541_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, C1541, tag, owner, clock) { }


//-------------------------------------------------
//  c1541c_t - constructor
//-------------------------------------------------

c1541c_t::c1541c_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, C1541C, tag, owner, clock) {  }


//-------------------------------------------------
//  c1541ii_t - constructor
//-------------------------------------------------

c1541ii_t::c1541ii_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, C1541II, tag, owner, clock) {  }


//-------------------------------------------------
//  sx1541_t - constructor
//-------------------------------------------------

sx1541_t::sx1541_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, SX1541, tag, owner, clock) { }


//-------------------------------------------------
//  fsd1_t - constructor
//-------------------------------------------------

fsd1_t::fsd1_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, FSD1, tag, owner, clock) { }


//-------------------------------------------------
//  fsd2_t - constructor
//-------------------------------------------------

fsd2_t::fsd2_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, FSD2, tag, owner, clock) { }


//-------------------------------------------------
//  csd1_t - constructor
//-------------------------------------------------

csd1_t::csd1_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, CSD1, tag, owner, clock) { }


//-------------------------------------------------
//  c1541_dolphin_dos_t - constructor
//-------------------------------------------------

c1541_dolphin_dos_t::c1541_dolphin_dos_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, C1541_DOLPHIN_DOS, tag, owner, clock) {  }


//-------------------------------------------------
//  c1541_professional_dos_v1_t - constructor
//-------------------------------------------------

c1541_professional_dos_v1_t::c1541_professional_dos_v1_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, C1541_PROFESSIONAL_DOS_V1, tag, owner, clock) {  }


//-------------------------------------------------
//  c1541_prologic_dos_classic_t - constructor
//-------------------------------------------------

c1541_prologic_dos_classic_t::c1541_prologic_dos_classic_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, C1541_PROLOGIC_DOS_CLASSIC, tag, owner, clock),
		m_pia(*this, MC6821_TAG),
		m_cent_data_out(*this, "cent_data_out"),
		m_mmu_rom(*this, "mmu")
{
}


//-------------------------------------------------
//  indus_gt_t - constructor
//-------------------------------------------------

indus_gt_t::indus_gt_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, INDUS_GT, tag, owner, clock) { }


//-------------------------------------------------
//  technica_t - constructor
//-------------------------------------------------

technica_t::technica_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, TECHNICA, tag, owner, clock) { }


//-------------------------------------------------
//  blue_chip_t - constructor
//-------------------------------------------------

blue_chip_t::blue_chip_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, BLUE_CHIP, tag, owner, clock) { }


//-------------------------------------------------
//  commander_c2_t - constructor
//-------------------------------------------------

commander_c2_t::commander_c2_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, COMMANDER_C2, tag, owner, clock) { }


//-------------------------------------------------
//  enhancer_2000_t - constructor
//-------------------------------------------------

enhancer_2000_t::enhancer_2000_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, ENHANCER_2000, tag, owner, clock) { }


//-------------------------------------------------
//  fd148_t - constructor
//-------------------------------------------------

fd148_t::fd148_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, FD148, tag, owner, clock) { }


//-------------------------------------------------
//  msd_sd1_t - constructor
//-------------------------------------------------

msd_sd1_t::msd_sd1_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, MSD_SD1, tag, owner, clock) { }


//-------------------------------------------------
//  msd_sd2_t - constructor
//-------------------------------------------------

msd_sd2_t::msd_sd2_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: c1541_base_t(mconfig, MSD_SD2, tag, owner, clock) { }


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void c1541_base_t::device_start()
{
	m_leds.resolve();

	// install image callbacks
	m_ga->set_floppy(m_floppy);

	// register for state saving
	save_item(NAME(m_data_out));
	save_item(NAME(m_via0_irq));
	save_item(NAME(m_via1_irq));
}

void fsd2_t::device_start()
{
	c1541_base_t::device_start();

	// decrypt ROM
	uint8_t *rom = memregion(M6502_TAG)->base();

	for (offs_t offset = 0; offset < 0x4000; offset++)
	{
		uint8_t data = bitswap<8>(rom[offset], 7, 6, 5, 3, 4, 2, 1, 0);

		rom[offset] = data;
	}
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void c1541_base_t::device_reset()
{
	m_maincpu->reset();

	m_via0->reset();
	m_via1->reset();

	// initialize gate array
	m_ga->accl_w(0);
	m_ga->ted_w(1);
}


//-------------------------------------------------
//  iec_atn_w -
//-------------------------------------------------

void c1541_base_t::cbm_iec_atn(int state)
{
	m_via0->write_ca1(!state);
	m_ga->atni_w(!state);

	set_iec_data();
}


//-------------------------------------------------
//  iec_reset_w -
//-------------------------------------------------

void c1541_base_t::cbm_iec_reset(int state)
{
	if (!state)
	{
		device_reset();
	}
}


//-------------------------------------------------
//  parallel_data_w -
//-------------------------------------------------

void c1541_base_t::parallel_data_w(uint8_t data)
{
	m_parallel_data = data;
}


//-------------------------------------------------
//  parallel_strobe_w -
//-------------------------------------------------

void c1541_base_t::parallel_strobe_w(int state)
{
	m_via0->write_cb1(state);
}
