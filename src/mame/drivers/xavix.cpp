// license:BSD-3-Clause
// copyright-holders:David Haywood, Angelo Salese
/***************************************************************************

    Preliminary driver for XaviX TV PNP console and childs (Let's! Play TV Classic)

    CPU is an M6502 derivative with added opcodes for far-call handling

    Notes from http://www.videogameconsolelibrary.com/pg00-xavix.htm#page=reviews (thanks Guru!)
    (** this isn't entirely accurate, XaviX Tennis appears to be Super Xavix, see other notes in driver)

    XaviXPORT arrived on the scene with 3 game titles (XaviX Tennis, XaviX Bowling and XaviX Baseball) using their
    original XaviX Multiprocessor.  This proprietary chip is reported to contain an 8-bit high speed central processing
    unit (6502) at 21 MHz, picture processor, sound processor, DMA controller, 1K bytes high speed RAM, universal timer,
    AD/Converter and I/O device control.  Each cartridge comes with a wireless peripheral to be used with the game (Baseball Bat,
    Tennis Racquet, etc.) that requires "AA" batteries.  The XaviXPORT system retailed for $79.99 USD with the cartridges
    retailing for $49.99 USD.

    The following year at CES 2005, SSD COMPANY LIMITED introduced two new XaviXPORT titles (XaviX Golf and XaviX Bass Fishing) each
    containing the upgraded "Super XaviX".  This new chip is said to sport a 16-bit high central processing unit (65816) at 43 MHz.
    SSD COMPANY LIMITED is already working on their next chip called "XaviX II" that is said to be a 32-bit RISC processor
    with 3D capabilities.

    Notes:

    To access service mode in Monster Truck hold Horn and Nitro on startup

    There are multiple revisions of the CPU hardware, the SSD 2000 / SSD 2002 chips definitely add more opcodes
    (thanks to Sean Riddle for this table)

    preliminary list of XaviX software based on various sources (some likely still missing)

     year       name                                                                                            PCB ID      ROM width       TSOP pads   ROM size        SEEPROM             die markings            extra components / notes

    2011        anpan-man kazoku de ikunou mat DX/JoyPalette/Japan                                              -           -               -           -               -                   -                       -
    2009        anpan-man pyon-pyon ikunou mat/JoyPalette/Japan                                                 -           -               -           -               -                   -                       -
    2008        kyuukyoku! kinniku grand slam! SASUKE kanzen seiha/EPOCH/Japan                                  -           -               -           -               -                   -                       -
    2007        Tokyo Friend park? perfect! mezase! grand slam!/EPOCH/Japan                                     -           -               -           -               -                   -                       -
    2006    1   Let's TV Play series "Kamen Rider Kabuto" /EPOCH/Japan                                          -           -               -           -               -                   -                       -
            2   Let's TV Play series "Bo-kenger" /EPOCH/Japan                                                   -           -               -           -               -                   -                       -
            3   Challenge Ai-chan! Exciting Ping-pong /TAKARATOMY/Japan                                         -           -               -           -               -                   -                       -
            4   Sasuke & Sportsman Tournament /BANDAI/Japan                                                     -           -               -           -               -                   -                       -
            5   Hyper resque, I am a resque team /BANDAI/Japan                                                  -           -               -           -               -                   -                       -
            6   Let's TV Play series "Ultraman" /BANDAI/Japan                                                   -           -               -           -               -                   -                       -
            7   Let's TV Play Classic series "Namco Nostalgia 1" /BANDAI/Japan                                  CGSJ        x8              48          1M              24LC04                                      dumped non destructively
            8   Let's TV Play Classic series "Namco Nostalgia 2" /BANDAI/Japan                                  CGSJ        x8              48          1M              24LC04              SSD 98 PL7351-181       dumped
            9   Let's TV Play Classic series "Taito Nostalgia 1" /BANDAI/Japan                                  CGSJ        x8              48          2M              24LC04                                      flash, dumped non destructively
            10  Let's TV Play Classic series "Taito Nostalgia 2" /BANDAI/Japan                                  CGSJ        x8              48          2M              24LC04                                      flash, dumped non destructively
            11  Let's play and study! Doraemon Hiragana book /BANDAI/Japan                                      -           -               -           -               -                   -                       -
            12  Scan card! Exciting Stage Soccer. /EPOCH/Japan                                                  -           -               -           -               -                   -                       -
            13  Hello Kitty Super TV computer /EPOCH/Japan                                                      -           -               -           -               -                   -                       -
            14  Doraemon Super TV computer /EPOCH/Japan                                                         -           -               -           -               -                   -                       -
    2005    1   Let's TV Play series "Dragon Ball Z" /BANDAI/Japan                                              -           -               -           -               -                   -                       -
            2   Let's TV Play series "Purikyua" /BANDAI/Japan                                                   -           -               -           -               -                   -                       -
            3   Idaten Jump /TOMY/Japan                                                                         -           -               -           -               -                   -                       -
            4   Tokyo Friend Park 2 Special /EPOCH/Japan                                                        -           -               -           -               -                   -                       -
            5   Masked Rider HIBIKI /BANDAI/Japan                                                               -           -               -           -               -                   -                       -
            6   Magic Ranger Battle /BANDAI/Japan                                                               -           -               -           -               -                   -                       -
            7   Accessory cartridge for Super TV computer "ECC Junior"/EPOCH/Japan                              -           -               -           -               -                   -                       -
            8   Wild Adventure Mini Golf Game /Hasbro/USA                                                       MGFA        x8              48          4M              24C04               SSD 98 PL7351-181       dumped
            9   MX DIRT REBEL Game /Hasbro/USA                                                                  MTXA        x8              48          8M              24C04               SSD 2000 NEC 85605-621  dumped
            10  Dokodemo Doraemon Japan Travel Game DX /EPOCH/Japan                                             -           -               -           -               -                   -                       -
            11  Tomas Plarail /TOMY/Japan                                                                       -           -               -           -               -                   -                       -
            12  Thomas TV Personal Computer /EPOCH/Japan                                                        -           -               -           -               -                   -                       -
            13  STAR WARS Light Saber Battle /TOMY/Japan                                                        -           -               -           -               -                   -                       -
            14  Jala Jaland /atlus/Japan                                                                        -           -               -           -               -                   -                       -        (is this the arcade release of Jara-Ja Land /TAKARA/Japan below?, Atlas are an arcade distributor and arcade version is confirmed)
            15  Star Wars Lightsaber Battle Game /Hasbro/USA                                                    SWSA        x8              48          8M              24C02               SSD 2000 NEC 85605-621  dumped
            16  Gururin World /EPOCH/Japan                                                                      -           x8              -           -               -                   SSD 98 PL7351-181       dumped
            17  Toinohgi Onmyo-daisenki /BANDAI/Japan                                                           -           -               -           -               -                   -                       -
    2004    1   Accessory cartridge for Super TV computer "Double mouse party"/EPOCH/Japan                      -           -               -           -               -                   -                       -
            2   Printer for TV computer /EPOCH/Japan                                                            -           -               -           -               -                   -                       -
            3   Virtual punching battle of "One Piece" /BANDAI/Japan                                            -           -               -           -               -                   -                       -
            4   Accessory cartridge for Super TV computer "Doraemon"/EPOCH/Japan                                -           -               -           -               -                   -                       -
            5   Accessory cartridge for Super TV computer "Hamutaro"/EPOCH/Japan                                -           -               -           -               -                   -                       -
            6   Super TV computer /EPOCH/Japan                                                                  -           -               -           -               -                   -                       -
            7   Super Dash ball /EPOCH/Japan                                                                    -           x8              -           -               -                   SSD 2000 NEC 85605-621  dumped
            8   Exciting sports Tennis X Fitness /EPOCH/Japan                                                   -           -               -           -               -                   -                       -
            9   Accessory memory mascot for TV mail Pc mail cot 2 characters (Putchi, Petchi) /EPOCH/Japan      -           -               -           -               -                   -                       -
            10  Accessory memory mascot for TV mail Pc mail cot 2 characters (Charuru, Kurau) /EPOCH/Japan      -           -               -           -               -                   -                       -
            11  The Lord of the Rings Warrior of Middle Earth /Hasbro/USA                                       LORA        x8              48          8M              24C02               SSD 2000 NEC 85605-621  dumped
            12  Beyblade Arcade Challenge 5-in-1 /Hasbro/USA                                                    -           -               -           -               -                   -                       have
            13  All star Festival Quize /EPOCH/Japan                                                            -           -               -           -               -                   -                       -
            14  e-kara mix /TAKARA/Japan                                                                        -           -               -           -               -                   -                       -
            15  Jumping Popira /TAKARA/Japan                                                                    -           -               -           -               -                   -                       -
            16  Tour around Japan. I'm a Prarail motorman /TOMY/Japan                                           -           -               -           -               -                   -                       -
            17  TV mail PC "Mercot /EPOCH/Japan                                                                 -           -               -           -               -                   -                       -
            18  Play TV Monster Truck /RADICA/USA                                                               74026       x8              48          4M              none                SSD 98 PL7351-181       dumped
            19  Play TV Madden Football /RADICA/USA                                                             74021       x8              48          4M              none                SSD 98 PL7351-181       dumped
            20  Play TV SSX Snowboarder (and Snowboarder white?) /RADICA/USA                                    74023                       none                                                                    dumped
            21  Disney Princess "Kira-Kira magical lesson" /TOMY/Japan                                          -           -               -           -               -                   -                       -
            22  Mermaid Melody "pichi-pichi Pitch" e-pitch microcomputer pure starter set /TAKARA/Japan         -           -               -           -               -                   -                       -
            23  Hello Kitty TV computer /EPOCH/Japan                                                            -           -               -           -               -                   -                       -
            24  Gan-Gan Revoultion /TAKARA/Japan                                                                -           -               -           -               -                   -                       -
    2003    1   Tokyo Friend Park II /EPOCH/Japan                                                               -           -               -           -               -                   -                       -
            2   TV mah-jongg /EPOCH/Japan                                                                       -           -               -           -               -                   -                       -
            3   e-kara Web /TAKARA/Japan                                                                       (e-kara web unit is a PC attachment, e-kara web carts can have songs flashed into them)
            4   Doraemon TV computer /EPOCH/Japan                                                               -           -               -           -               -                   -                       -
            5   Exciting stadium DX, Hansin Tigers version /EPOCH/Japan                                         -           -               -           -               -                   -                       -
            6   Dragon Quest /SQUARE ENIX/Japan                                                                 -           -               -           8M              -                   SSD 2000?               dumped
            7   Croquette! Win a medal! /EPOCH/Japan                                                            -           -               -           -               -                   -                       -
            8   Taiko Popira /TAKARA/Japan                                                                      -           -               -           -               -                   -                       dumped
            9   Together Minimoni, Dancing' Stage! plus /EPOCH/Japan                                            -           -               -           -               -                   -                       -
            10  Evio /TOMY/Japan                                                                                -           -               -           -               -                   -                       -
            11  Together Minimoni,Jumping Party! /EPOCH/Japan                                                   -           -               -           -               -                   -                       -
            12  Hamutaro TV computer /EPOCH/Japan                                                               -           -               -           -               -                   -                       -
            13  Jara-Ja Land /TAKARA/Japan                                                                      -           -               -           -               -                   -                       -       (is this the home release of Jala Jaland /atlus/Japan above?)
            14  Tomika, Draiving by Car navigation system /TOMY/Japan                                           -           -               -           -               -                   -                       -
            15  PLAY TV Rescue Heroes /RADICA/USA                                                               73036       x8              48          2M              none                SSD 98 PL7351-181       dumped
            16  PLAY TV Huntin' 2 /RADICA/USA                                                                   73030       x8              none                        none                SSD 98 PL7351-181       dumped
            17  Let's play Ping-pong. Exciting pingpong2 /EPOCH/Japan                                           -           -               -           -               -                   -                       -
            18  Cartridge for Slot machine TV "King of wild animal" /TAKARA/Japan                               -           -               -           -               -                   -                       -
            19  ChyoroQ "Burning up Racer /TAKARA/Japan                                                         -           -               -           -               -                   -                       -
            20  Super shot! Exciting golf /EPOCH/Japan                                                          -           -               -           -               -                   -                       -
            21  PichiPichi Pitchi /TAKARA/Japan                                                                 -           -               -           -               -                   -                       -
            22  Dual Station /TAKARA/Japan                                                                      -           -               -           -               -                   -                       -
            23  Gei-Geki GoGo! Shooting /TAKARA/Japan                                                           -           -               -           -               -                   -                       -
            24  Let's fish a big one. Exciting fishing! /EPOCH/Japan                                            -           -               -           -               -                   -                       -
            25  Champion Pinball /TOMY/Japan                                                                    -           -               -           -               -                   -                       -
            26  Excite Fishing DX                                                                               EF2J        x8              48          4M              24C08               SSD 98 PL7351-181       dumped
    2002    1   Accessory cartridge for Slot machine "Gin-gin maru TV" /TAKARA/Japan                            -           -               -           -               -                   -                       -
            2   Wildest computer robot "Daigander" (Korean version) /TAKARA/Korea                               -           -               -           -               -                   -                       -
            3   Hamutaro's circus /EPOCH/Japan                                                                  -           -               -           -               -                   -                       -
            4   Doraemon Wakuwaku Kuukihou /EPOCH/Japan                                                         -           x8              -           -               -                   -                       dumped
            5   Strike! Exciting bowling /EPOCH/Japan                                                           -           -               -           -               -                   -                       -
            6   e-kara /Hasbro/Spain                                                                            -           -               -           -               -                   -                       -
            7   Starter set for e-kara H.S," Morning sisters" /TAKARA/Japan                                     -           -               -           -               -                   -                       -
            8   e-kara H.S.(headphones set) /TAKARA/Japan                                                       -           -               -           -               -                   -                       -
            9   Accessory cartridge for Slot machine TV," Aladdin TV" /TAKARA/Japan                             -           -               -           -               -                   -                       -
            10  Accessory cartridge for Slot machine TV "Businessman Kintaro/TAKARA/Japan                       -           -               -           -               -                   -                       -
            11  Poko-poko Hammers /TAKARA/Japan                                                                 -           -               -           -               -                   -                       -
            12  e-kara N Angel blue special set /TAKARA/Japan                                                   -           -               -           -               -                   -                       -
            13  Together Minimoni,Dancing Stage! /EPOCH/Japan                                                   -           -               -           -               -                   -                       -
            14  King of shooting /TOMY/Japan                                                                    -           -               -           -               -                   -                       -
            15  Knock them out! Exciting boxing /EPOCH/Japan                                                    -           -               -           -               -                   -                       -
            16  Popira2 /TAKARA/Japan                                                                           -           -               -           -               -                   -                       -
            17  Zuba-Zuba Blade /TAKARA/Japan                                                                   -           -               -           -               -                   -                       -
            18  Starter set for e-kara N "Morning sisters" /TAKARA/Japan                                        -           -               -           -               -                   -                       -
            19  e-kara /Hasbro/England                                                                          -           -               -           -               -                   -                       dumped? (if same as one of the 'US' sets?)
            20  e-kara /Takara USA/USA                                                                          -           -               -           -               -                   -                       dumped? (or #10 below, this might be ekaraa set with improved text)
            21  PLAY TV Soccer /RADICA/USA                                                                      76088500    x8              none                        none                SSD 98 PA7351-107       have
            22  PLAY TV Jr. Construction /RADICA/USA                                                            -           -               -           -               -                   -                       -
            23  PLAY TV Boxing /RADICA/Japan                                                                    72039       x8              48          2M              none                SSD 98 PA7351-107       dumped
            24  PLAY TV Baseball 2 /RADICA/USA                                                                  72042       x8              48          2M              none                SSD 98 PL7351-181       dumped
            25  Barbie Dance Party /RADICA/USA,EU                                                               -           x8              none        -               -                   -                       dumped
            26  Compete! Exciting stadium DX /EPOCH/Japan                                                       -           -               -           -               -                   -                       -
            27  e-kara N /EPOCH/Japan                                                                           -           -               -           -               -                   -                       -
            28  Who's the ace? Excite Tennis /EPOCH/Japan                                                       -           -               -           -               -                   -                       -
            29  Wildest computer robot, "Daigander" /TAKARA/Japan                                               -           -               -           -               -                   -                       -
            30  Cartridge for Slot machine TV "King of wild animal Jr." /TAKARA/Japan                           -           -               -           -               -                   -                       -
            31  Gachinko Contest! Slot machine TV /DCT/Japan                                                    -           -               -           -               -                   -                       -
            32  Beyblade Ultimate shooter /TAKARA/Japan                                                         -           -               -           -               -                   -                       -
    2001    1   Ping-pong(Chinese version) /Tenpon/China                                                        -           -               -           -               -                   -                       -
            2   TV hockey /TOMY/Japan                                                                           -           -               -           -               -                   -                       -
            3   e-kara Morning sisters /TAKARA/Japan                                                            -           -               -           -               -                   -                       -
            4   e-kara Duet microphone plus /TAKARA/Japan                                                       -           -               -           -               -                   -                       -
            5   e-kara plus /TAKARA/Japan                                                                       -           -               -           -               -                   -                       -
            6   Hamutaro, Dancing', Running /EPOCH/Japan                                                        -           -               -           -               -                   -                       -
            7   Gin-gin Snowboarders /TAKARA/Japan                                                              -           -               -           -               -                   -                       -
            8   Shoot! Exciting striker /EPOCH/Japan                                                            -           -               -           -               -                   -                       -
            9   e-kara US version /TAKARA USA, Hasbro/USA,EU                                                    71076       x8              none        1M                                  SSD 98 PA7351-107       this one or #20 above?  dumped
            10  Popira Korea version /SONOKONG/Korea                                                            -           -               -           -               -                   -                       -
            11  I singer: e-kara Korean version /SONOKONG/Korea                                                 -           -               -           -               -                   -                       -
            12  Ms.Comett, Lovely baton /TAKARA/Japan<                                                          -           -               -           -               -                   -                       -
            13  Dance Dance revolution family mat /KONAMI,KONAMI Sports/Japan                                   -           -               -           -               -                   -                       dumped
            14  PLAY TV Card Night /RADICA/USA                                                                  71063       x8              40          1M              none                SSD 98 PA7351-107       dumped
            15  PLAY TV Bass Fishin' /RADICA/USA                                                                71008       x8              40          1M              none                SSD 98 PA7351-107       dumped
            16  PLAY TV Snowboarder (blue) /RADICA/USA                                                          71023       x8              40          1M              none                SSD 98 PL7351-181       dumped
            17  Bistro Kids /SEGA Toys/Japan                                                                    -           -               -           -               -                   -                       -
            18  Let's construct the town! /TAKARA/Japan                                                         -           -               -           -               -                   -                       -
            19  Let's fish black bass! Exciting Fishing /EPOCH/Japan                                            -           -               -           -               -                   -                       -
            20  Baseball Korean version /SONOKONG/Korea                                                         -           -               -           -               -                   -                       -
            21  Ping-pong Korean version /SONOKONG/Korea                                                        -           -               -           -               -                   -                       -
            22  e-kara Hello Kitty /TAKARA/Japan                                                                -           -               -           -               -                   -                       -
            23  Special box "Morning sisters" /TAKARA/Japan                                                     -           -               -           -               -                   -                       -
            24  Gan-Gan Adventure /TAKARA/Japan                                                                 -           -               -           -               -                   -                       -
            25  e-kara wireless unit /TAKARA/Japan                                                              -           -               -           -               -                   -                       -
            26  Webdiver Gradion /TAKARA/Japan                                                                  -           -               -           -               -                   -                       -
            27  black bass tsurouze! Excite Fishing/EPOCH/Japan                                                 -           -               -           -               -                   -                       -
            28  Hamu-chan's Daishuugou dance surunoda! hasirunoda!/EPOCH/Japan                                  -           -               -           -               -                   -                       -
    2000    1   Popira /TAKARA/Japan                                                                            -           -               -           -               -                   -                       dumped
            2   e-kara Duet microphone /TAKARA/Japan                                                            -           -               -           -               -                   -                       -
            3   e-kara /TAKARA/Japan                                                                            -           -               -           -               -                   -                       dumped
            4   Let's play ping-pong. Exciting ping-pong /EPOCH/Japan                                           -           -               -           -               -                   -                       dumped
            5   PLAY TV Huntin' Buckmasters /RADICA/USA                                                         8074        x8              none                        none                SSD 98 PA7351-107       dumped
            6   PLAY TV Ping Pong /RADICA/USA,HK,EU                                                             8028        x8              48          1M              none                SSD 97 PA7270-107       dumped
            7   PLAY TV OPUS /RADICA/USA,EU                                                                     -           -               -           -               -                   -                       dumped (US version, PAL version appears to use different ROM)
            8   PLAY TV Baseball 2 /EPOCH/Japan, HK                                                             -           -               -           -               -                   -                       -
            9   Let's hit a homerun! Exciting baseball /EPOCH/Japan                                             -           -               -           -               -                                           -
                Play TV Baseball /RADICA/USA,EU                                                                 8017        x8              none                        none                SSD 98 PA7351-107       dumped
    1999    1   ABC Jungle Fun Hippo /Vteck/HK, USA, France                                                     -           -               -           -               -                   -                       -
    Unknown 1   PLAY TV Football /RADICA/USA                                                                    74021       x8              48          4M              none                SSD 98 PL7351-181       dumped
                XaviXTennis                                                                                     SGM6446     x16             48          8M              24C08               SSD 2002 NEC 85054-611  dumped
                XaviXBowling                                                                                    SGM644C     x16             48                                                                      dumped


    TODO: put into above table (XaviXPORT cartridges)

    XaviX Tennis and XaviX Baseball are the simplest: just the CPU, x16 ROM and 24C08 SEEPROM.  Bowling and Boxing also have 4 IR LEDs and a 32x32 sensor.
    XaviX Fishing has ROM, 24C08 and a 24-pin daughterboard with a Nordic nRF24E1 2.4GHz 8051-based SoC.
    All the rest of the carts include an S35390 I2C clock chip with crystal and battery backup, and have two x16 ROM chips, using a 5-pin single-gate inverter to create complementary /OE signals.
    J-MAT, Fitness Exercise, Fitness Challenge and Bike Concept have a 24CS64 SEEPROM.  Bike Concept also has two 74HC14s.
    Fitness Dance has an Atmel H93864C (maybe SEEPROM?) a Microchip DSPIC 33FJ12GP202 and two JRC 2740 dual op amps.
    Music and Circuit has a 24CS64, two UTC324 quad op amps, a 74HC14, a 74HCT04, and an 8-pin SOIC labeled 61545, which is likely an M61545 dual electronic volume control.

    It looks like the sensors (cameras) are from ETOMS
    They are all 32x32 arrays except Fitness Play, which is 64x64.  Most of the PCBs are marked CIS.
    Bowling and Boxing look identical.
    LOTR and SW look identical
    DQ looks similar to LOTR
    Real Swing Golf (different driver) and Fitness Play look different from all the others.

    The sensor dies for Bowling, Boxing, Star Wars and DQ are labeled CU5501A.
    LOTR is CU5501
    Real Swing Golf is CU5502
    Fitness Play is S-5300A SLH2039H and does not have ETOMS on it.
    The Fitness Play main PCB has an extra glob next to the ribbon cable to the camera.
    Most of the camera PCBs connect to the main PCB with an 18-20 wire ribbon cable.

    Real Swing Golf just has 6 wires, Its camera PCB is the only one with a ceramic resonator
    Maybe the CU5502 chip offloads some processing from the CPU?



    NOTES:

    Play TV Monster Truck runs off an entirely different codebase to everything else, presumably coded by the developer from scratch rather than using code supplied by SSD Company LTD
    Play TV Rescue Heroes fails to display any kind of XaviX logo or SSD Copyright, it is the only XaviX based game so far to not show these details anywhere in the game.



***************************************************************************/

#include "emu.h"
#include "includes/xavix.h"

// NTSC clock for regular XaviX?
#define MAIN_CLOCK XTAL(21'477'272)
// some games (eg Radica Opus) run off a 3.579545MHz XTAL ( same as the above /6 ) so presumably there is a divider / multiplier circuit on some PCBs?
// TODO: what's the PAL clock?

/* rad_madf has callf #$8f3f21 in various places, and expects to jump to code in ROM, it is unclear how things map in this case, as presumably
   the CPU 0 page memory and stack are still at 0 but ROM must be in the 3xxx range (game hasn't got far enough to call this yet to help either)

   the maximum romsize appears to be 0x800000 so presumably the high bit being set has some additional meaning

   for now treat it as a swapped arrangement vs. the reads from the lower range, except where page 0 ram would map, it's also possible that
   vram etc. is completely unavailable if executing from these addresses, there isn't much evidence at the moment

   note, many DMA operations and tile bank redirects etc. have the high bit set too, so that could be significant if it defines how it accesses
   memory in those cases too

   this can't be correct, it breaks monster truck which expects ROM at 8000 even in the >0x800000 region, maybe code + data mappings need
   to be kept separate, with >0x800000 showing both ROM banks for code, but still having the zero page area etc. for data?

   The code at 00EA84 in Ping Pong stores 8e to the data bank, reads 16 bit pointer from from 0000 + y (expecting it to come from zero page ram - value 3081)
   then reads data from 3081, pushes it to stack (which is expected to work) then sets data bank back to 00 (writes to ff, expecting it to work) then pulls
   the value written and puts it in RAM.  Is stack actually still memory mapped at this point, or do stack operations always go to stack regardless?
   Do reads return databank/codebank/stack, or only zero page? is zero page visibility maybe even conditional on how it gets used?

   in namcons1 the code at 00F3F2 has data bank set to 0x84 and expects to read from ROM using lda ($0a), y where the content of 0x0a is 0000
   this means that 0a and 0b must be read as zero page, but the actual pointer read from is ROM, bypassing zero page entirely, I currently have a separate
   address space called by that single opcode to handle this, it might need expanding to any other opcodes that aren't using zero page directly tho
   possible stack is similar?


*/

// this is only used for opcode / oprand reads, data memory addressing is handled in core, doing the opcode / oprand addressing in core causes disassembly issues when running from lowbus space (ram, interrupts on most games)
void xavix_state::xavix_map(address_map &map)
{
	map(0x000000, 0x7fffff).r(FUNC(xavix_state::opcodes_000000_r));
	map(0x800000, 0xffffff).r(FUNC(xavix_state::opcodes_800000_r));

	map(0x000000, 0x003fff).w(FUNC(xavix_state::debug_mem_w));

}

// this is used by data reads / writes after some processing in the core to decide if data reads can see lowbus, zeropage, stack, bank registers etc. and only falls through to here on a true external bus access
void xavix_state::xavix_extbus_map(address_map &map)
{
	map(0x000000, 0xffffff).rw(FUNC(xavix_state::extbus_r), FUNC(xavix_state::extbus_w));
}

void xavix_state::xavix_lowbus_map(address_map &map)
{
	map(0x0000, 0x3fff).ram().share("mainram");

	// Memory Emulator / Text Array
	map(0x4000, 0x4fff).rw(FUNC(xavix_state::xavix_memoryemu_txarray_r), FUNC(xavix_state::xavix_memoryemu_txarray_w));

	// Sprite RAM (aka Fragment RAM)
	map(0x6000, 0x67ff).ram().w(FUNC(xavix_state::spriteram_w)).share("fragment_sprite");

	// Palette RAM
	map(0x6800, 0x68ff).ram().w(FUNC(xavix_state::palram_sh_w)).share("palram_sh");
	map(0x6900, 0x69ff).ram().w(FUNC(xavix_state::palram_l_w)).share("palram_l");

	// Segment RAM
	map(0x6a00, 0x6a1f).ram().share("segment_regs"); // test mode, pass flag 0x20

	// Tilemap 1 Registers
	map(0x6fc8, 0x6fcf).rw(FUNC(xavix_state::tmap1_regs_r), FUNC(xavix_state::tmap1_regs_w));

	// Tilemap 2 Registers
	map(0x6fd0, 0x6fd7).rw(FUNC(xavix_state::tmap2_regs_r), FUNC(xavix_state::tmap2_regs_w));

	// Sprite Registers
	map(0x6fd8, 0x6fd8).w(FUNC(xavix_state::spriteregs_w));

	// Sprite DMA
	map(0x6fe0, 0x6fe0).rw(FUNC(xavix_state::spritefragment_dma_status_r), FUNC(xavix_state::spritefragment_dma_trg_w)); // after writing to 6fe1/6fe2 and 6fe5/6fe6 rad_mtrk writes 0x43/0x44 here then polls on 0x40   (see function call at c273) write values are hardcoded, similar code at 18401
	map(0x6fe1, 0x6fe2).w(FUNC(xavix_state::spritefragment_dma_params_1_w));
	map(0x6fe5, 0x6fe6).w(FUNC(xavix_state::spritefragment_dma_params_2_w));

	// Arena Registers (controls visible window + more?)
	map(0x6fe8, 0x6fe8).rw(FUNC(xavix_state::arena_start_r), FUNC(xavix_state::arena_start_w)); // r/w tested
	map(0x6fe9, 0x6fe9).rw(FUNC(xavix_state::arena_end_r), FUNC(xavix_state::arena_end_w)); // r/w tested
	map(0x6fea, 0x6fea).rw(FUNC(xavix_state::arena_control_r), FUNC(xavix_state::arena_control_w));

	// Colour Mixing / Enabling Registers
	map(0x6ff0, 0x6ff0).ram().w(FUNC(xavix_state::colmix_sh_w)).share("colmix_sh"); // effect colour?
	map(0x6ff1, 0x6ff1).ram().w(FUNC(xavix_state::colmix_l_w)).share("colmix_l");
	map(0x6ff2, 0x6ff2).ram().w(FUNC(xavix_state::colmix_6ff2_w)).share("colmix_ctrl"); // set to 07 after clearing above things in interrupt 0

	// Display Control Register / Status Flags
	map(0x6ff8, 0x6ff8).rw(FUNC(xavix_state::dispctrl_6ff8_r), FUNC(xavix_state::dispctrl_6ff8_w)); // always seems to be a read/store or read/modify/store
	map(0x6ff9, 0x6ff9).r(FUNC(xavix_state::pal_ntsc_r));
	map(0x6ffa, 0x6ffa).ram().w(FUNC(xavix_state::dispctrl_posirq_x_w)).share("posirq_x");
	map(0x6ffb, 0x6ffb).ram().w(FUNC(xavix_state::dispctrl_posirq_y_w)).share("posirq_y"); // increases / decreases when you jump in snowboard (snowboard, used to blank ground)

	// Lightgun / pen 1 control
	// map(0x6ffc, 0x6fff)

	// Sound RAM
	// map(0x7400, 0x757f)

	// Sound Control
	map(0x75f0, 0x75f1).rw(FUNC(xavix_state::sound_startstop_r), FUNC(xavix_state::sound_startstop_w)); // r/w tested read/written 8 times in a row
	map(0x75f2, 0x75f3).rw(FUNC(xavix_state::sound_updateenv_r), FUNC(xavix_state::sound_updateenv_w));
	map(0x75f4, 0x75f5).r(FUNC(xavix_state::sound_sta16_r)); // related to 75f0 / 75f1 (read after writing there - rad_mtrk)
	map(0x75f6, 0x75f6).rw(FUNC(xavix_state::sound_volume_r), FUNC(xavix_state::sound_volume_w)); // r/w tested
	map(0x75f7, 0x75f7).w(FUNC(xavix_state::sound_regbase_w));
	map(0x75f8, 0x75f8).rw(FUNC(xavix_state::sound_75f8_r), FUNC(xavix_state::sound_75f8_w)); // r/w tested
	map(0x75f9, 0x75f9).rw(FUNC(xavix_state::sound_75f9_r), FUNC(xavix_state::sound_75f9_w));
	map(0x75fa, 0x75fa).rw(FUNC(xavix_state::sound_timer0_r), FUNC(xavix_state::sound_timer0_w)); // r/w tested
	map(0x75fb, 0x75fb).rw(FUNC(xavix_state::sound_timer1_r), FUNC(xavix_state::sound_timer1_w)); // r/w tested
	map(0x75fc, 0x75fc).rw(FUNC(xavix_state::sound_timer2_r), FUNC(xavix_state::sound_timer2_w)); // r/w tested
	map(0x75fd, 0x75fd).rw(FUNC(xavix_state::sound_timer3_r), FUNC(xavix_state::sound_timer3_w)); // r/w tested
	map(0x75fe, 0x75fe).rw(FUNC(xavix_state::sound_irqstatus_r), FUNC(xavix_state::sound_irqstatus_w));
	map(0x75ff, 0x75ff).w(FUNC(xavix_state::sound_75ff_w));

	// Slot Registers
	map(0x7810, 0x7810).w(FUNC(xavix_state::slotreg_7810_w)); // startup

	// External Bus Interface control
	map(0x7900, 0x7902).rw(FUNC(xavix_state::extintrf_790x_r), FUNC(xavix_state::extintrf_790x_w));

	// DMA Controller
	map(0x7980, 0x7980).rw(FUNC(xavix_state::rom_dmastat_r), FUNC(xavix_state::rom_dmatrg_w));
	map(0x7981, 0x7983).ram().w(FUNC(xavix_state::rom_dmasrc_w)).share("rom_dma_src");
	map(0x7984, 0x7985).ram().w(FUNC(xavix_state::rom_dmadst_w)).share("rom_dma_dst");
	map(0x7986, 0x7987).ram().w(FUNC(xavix_state::rom_dmalen_w)).share("rom_dma_len");

	// IO Ports
	map(0x7a00, 0x7a00).rw(FUNC(xavix_state::io0_data_r), FUNC(xavix_state::io0_data_w));
	map(0x7a01, 0x7a01).rw(FUNC(xavix_state::io1_data_r), FUNC(xavix_state::io1_data_w));
	map(0x7a02, 0x7a02).rw(FUNC(xavix_state::io0_direction_r), FUNC(xavix_state::io0_direction_w));
	map(0x7a03, 0x7a03).rw(FUNC(xavix_state::io1_direction_r), FUNC(xavix_state::io1_direction_w));

	// IO Event Interrupt control
	map(0x7a80, 0x7a80).rw(FUNC(xavix_state::ioevent_enable_r), FUNC(xavix_state::ioevent_enable_w));
	map(0x7a81, 0x7a81).rw(FUNC(xavix_state::ioevent_irqstate_r), FUNC(xavix_state::ioevent_irqack_w));

	// Mouse / Trackball?
	map(0x7b00, 0x7b00).rw("anport", FUNC(xavix_anport_device::mouse_7b00_r), FUNC(xavix_anport_device::mouse_7b00_w));
	map(0x7b01, 0x7b01).rw("anport", FUNC(xavix_anport_device::mouse_7b01_r), FUNC(xavix_anport_device::mouse_7b01_w));
	map(0x7b10, 0x7b10).rw("anport", FUNC(xavix_anport_device::mouse_7b10_r), FUNC(xavix_anport_device::mouse_7b10_w));
	map(0x7b11, 0x7b11).rw("anport", FUNC(xavix_anport_device::mouse_7b11_r), FUNC(xavix_anport_device::mouse_7b11_w));

	// Lightgun / pen 2 control
	//map(0x7b18, 0x7b1b)

	// ADC registers
	map(0x7b80, 0x7b80).rw("adc", FUNC(xavix_adc_device::adc_7b80_r), FUNC(xavix_adc_device::adc_7b80_w)); // rad_snow (not often)
	map(0x7b81, 0x7b81).rw("adc", FUNC(xavix_adc_device::adc_7b81_r), FUNC(xavix_adc_device::adc_7b81_w)); // written (often, m_trck, analog related?)

	// Sleep control
	//map(0x7b82, 0x7b83)

	// Timer control
	map(0x7c00, 0x7c00).rw(FUNC(xavix_state::timer_status_r), FUNC(xavix_state::timer_control_w));
	map(0x7c01, 0x7c01).rw(FUNC(xavix_state::timer_baseval_r), FUNC(xavix_state::timer_baseval_w)); // r/w tested
	map(0x7c02, 0x7c02).rw(FUNC(xavix_state::timer_freq_r), FUNC(xavix_state::timer_freq_w));
	map(0x7c03, 0x7c03).r(FUNC(xavix_state::timer_curval_r));

	// Barrel Shifter registers
	map(0x7ff0, 0x7ff1).rw("math", FUNC(xavix_math_device::barrel_r), FUNC(xavix_math_device::barrel_w));

	// Multiply / Divide registers
	map(0x7ff2, 0x7ff4).rw("math", FUNC(xavix_math_device::mult_param_r), FUNC(xavix_math_device::mult_param_w));
	map(0x7ff5, 0x7ff6).rw("math", FUNC(xavix_math_device::mult_r), FUNC(xavix_math_device::mult_w));

	// CPU Vector registers
	map(0x7ff9, 0x7ff9).w(FUNC(xavix_state::vector_enable_w)); // enables / disables the custom vectors
	map(0x7ffa, 0x7ffa).rw(FUNC(xavix_state::nmi_vector_lo_r), FUNC(xavix_state::nmi_vector_lo_w)); // an IRQ vector (nmi?) - popira needs to read it back if you pause on one of the seeprom carts
	map(0x7ffb, 0x7ffb).rw(FUNC(xavix_state::nmi_vector_hi_r), FUNC(xavix_state::nmi_vector_hi_w));
	map(0x7ffc, 0x7ffc).rw(FUNC(xavix_state::irq_source_r), FUNC(xavix_state::irq_source_w));
	// map(0x7ffd, 0x7ffd) some of the Nostalgia games read here, why?
	map(0x7ffe, 0x7ffe).rw(FUNC(xavix_state::irq_vector_lo_r),FUNC(xavix_state::irq_vector_lo_w)); // an IRQ vector (irq?) - rad_opus needs to read this back!
	map(0x7fff, 0x7fff).rw(FUNC(xavix_state::irq_vector_hi_r),FUNC(xavix_state::irq_vector_hi_w));
}

void xavix_state::superxavix_lowbus_map(address_map &map)
{
	xavix_lowbus_map(map);

	// bitmap layer palette
	map(0x6c00, 0x6cff).ram().w(FUNC(xavix_state::bmp_palram_sh_w)).share("bmp_palram_sh");
	map(0x6d00, 0x6dff).ram().w(FUNC(xavix_state::bmp_palram_l_w)).share("bmp_palram_l");

	// extended external bus stuff (possible banking control?)
	map(0x7909, 0x7909).w(FUNC(xavix_state::extended_extbus_reg0_w));
	map(0x790b, 0x790b).w(FUNC(xavix_state::extended_extbus_reg1_w));
	map(0x790d, 0x790d).w(FUNC(xavix_state::extended_extbus_reg2_w));

	map(0x7a10, 0x7a12).rw("xavix2002io", FUNC(xavix2002_io_device::pio_dir_r), FUNC(xavix2002_io_device::pio_dir_w));
	map(0x7a20, 0x7a22).rw("xavix2002io", FUNC(xavix2002_io_device::pio_out_r), FUNC(xavix2002_io_device::pio_out_w));
	map(0x7a30, 0x7a32).r("xavix2002io", FUNC(xavix2002_io_device::pio_in_r));

	map(0x6fb0, 0x6fc7).ram().share("bmp_base");

	map(0x7ffd, 0x7ffd).nopw(); // looks like a watchdog?
}



static INPUT_PORTS_START( xavix )
	PORT_START("IN0")
	PORT_DIPNAME( 0x01, 0x00, "IN0" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x00, "IN1" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START("AN0")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("AN1")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("AN2")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("AN3")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("AN4")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("AN5")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("AN6")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("AN7")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("MOUSE0X")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("MOUSE0Y")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("MOUSE1X")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("MOUSE1Y")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("REGION") // PAL/NTSC flag
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_CUSTOM )
INPUT_PORTS_END

static INPUT_PORTS_START( xavix_an )
	PORT_INCLUDE(xavix)

	// test inputs, not real!
	PORT_MODIFY("AN0") // 00
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
	PORT_MODIFY("AN1") // 01
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
	PORT_MODIFY("AN2") // 02
	PORT_BIT( 0xff, 0x00, IPT_PEDAL3 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
	PORT_MODIFY("AN3") // 03
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
	PORT_MODIFY("AN4") // 10
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
	PORT_MODIFY("AN5") // 11
	PORT_BIT( 0xff, 0x00, IPT_PEDAL3 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
	PORT_MODIFY("AN6") // 12
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
	PORT_MODIFY("AN7") // 13
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)
INPUT_PORTS_END

static INPUT_PORTS_START( xavix_i2c )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_state,i2c_r, nullptr)
INPUT_PORTS_END

static INPUT_PORTS_START( ttv_lotr )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_lotr_state,camera_r, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_lotr_state,camera_r, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_state,i2c_r, nullptr)
INPUT_PORTS_END

static INPUT_PORTS_START( xavix_bowl )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_bowl_state,camera_r, nullptr)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_bowl_state,camera_r, nullptr)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_state,i2c_r, nullptr)
INPUT_PORTS_END

static INPUT_PORTS_START( epo_sdb )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("MOUSE0X")
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_X ) PORT_SENSITIVITY(25) PORT_KEYDELTA(32) PORT_REVERSE PORT_PLAYER(1)
	PORT_MODIFY("MOUSE0Y")
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_SENSITIVITY(25) PORT_KEYDELTA(32) PORT_PLAYER(1)
	PORT_MODIFY("MOUSE1X")
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_X ) PORT_SENSITIVITY(25) PORT_KEYDELTA(32) PORT_REVERSE PORT_PLAYER(2)
	PORT_MODIFY("MOUSE1Y")
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_SENSITIVITY(25) PORT_KEYDELTA(32) PORT_PLAYER(2)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
INPUT_PORTS_END

static INPUT_PORTS_START( ltv_tam )
	PORT_INCLUDE(xavix_i2c)

	PORT_MODIFY("MOUSE0X")
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_X ) PORT_SENSITIVITY(25) PORT_KEYDELTA(32) PORT_REVERSE PORT_PLAYER(1)
	PORT_MODIFY("MOUSE0Y")
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_SENSITIVITY(25) PORT_KEYDELTA(32) PORT_PLAYER(1)
	PORT_MODIFY("MOUSE1X")
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_X ) PORT_SENSITIVITY(25) PORT_KEYDELTA(32) PORT_REVERSE PORT_PLAYER(2)
	PORT_MODIFY("MOUSE1Y")
	PORT_BIT( 0xff, 0x00, IPT_AD_STICK_Y ) PORT_SENSITIVITY(25) PORT_KEYDELTA(32) PORT_PLAYER(2)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
INPUT_PORTS_END


// left + right drums together = select / forward (needed on initial screen).  left drum = left in menus   right drum  = right in menus
// analog reading depends heavily on timers, they're too fast right now so drum hits are too hard and register multiple times
static INPUT_PORTS_START( taikodp )
	PORT_INCLUDE(xavix_an)

	PORT_MODIFY("AN0") // 00  (read by one type of function, handled in timer interrupt at 0x1d92 in RAM)
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_MODIFY("AN1") // 01  (read by identical function to 00 but with different addresses, handled in timer interrupt at 0x1dbd in RAM)
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_SENSITIVITY(100) PORT_KEYDELTA(20)

	PORT_MODIFY("AN5") // 11  (read by different function, handled in timer interrupt at 0x1de8) (battery status related?)
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("AN7") // 13  (read by identical function to 11 but with different addresses, handled in timer interrupt at 0x1e09 in RAM) (battery status related?)
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	// not read
	PORT_MODIFY("AN2") // 02
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_MODIFY("AN3") // 03
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_MODIFY("AN4") // 10
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_MODIFY("AN6") // 12
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_cart_state,i2c_r, nullptr)
INPUT_PORTS_END

static INPUT_PORTS_START( jpopira )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Pad 1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Pad 2")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Pad 3")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Pad 4")

	PORT_MODIFY("IN1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_i2c_cart_state,i2c_r, nullptr)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END



static INPUT_PORTS_START( xavixp )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("REGION") // PAL/NTSC flag
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_CUSTOM )
INPUT_PORTS_END


/* Test mode lists the following

  LED (on power button?)
  Throttle Low
  Throttle High
  Reverse
  NO2
  Steering Left (4 positions)
  Steering Right (4 positions)
  Horn

*/

CUSTOM_INPUT_MEMBER( xavix_mtrk_state::mtrk_wheel_r )
{
	return m_wheel->read_direction();
}


static INPUT_PORTS_START( rad_mtrk )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Nitro")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Throttle High")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Throttle Low")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Reverse / Back")

	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_CUSTOM )  PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_mtrk_state,mtrk_wheel_r, (void *)0)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Horn")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

static INPUT_PORTS_START( rad_mtrkp )
	PORT_INCLUDE(rad_mtrk)

	PORT_MODIFY("REGION") // PAL/NTSC flag
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_CUSTOM )
INPUT_PORTS_END

static INPUT_PORTS_START( rad_opus )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0") // comments show how inputs labeled on PCB
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Pad 1 <") // PI00
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Pad 2")   // PI01
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Pad 3")   // PI02
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Pad 4 >") // PI03

	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Pause") // PI05
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Menu Select") // PI06
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 ) PORT_NAME("Start / Bomb") // PI07
INPUT_PORTS_END

static INPUT_PORTS_START( rad_hnt )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Menu Next")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Menu Previous")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Menu Select")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Pause / Menu")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Fire Gun")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Safety") PORT_TOGGLE

	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

static INPUT_PORTS_START( rad_crdn )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON2 )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
INPUT_PORTS_END

static INPUT_PORTS_START( rad_crdnp )
	PORT_INCLUDE(rad_crdn)

	PORT_MODIFY("REGION") // PAL/NTSC flag
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_CUSTOM )
INPUT_PORTS_END

static INPUT_PORTS_START( rad_box )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	// 6 types of punch and some navigation controls?
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Left Jan")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Left Hook")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Left Uppercut")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Left Jab")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Left Hook")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Left Uppercut")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON7 )  PORT_NAME("Block")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN ) // needs to be high to pass warning screen?
INPUT_PORTS_END

static INPUT_PORTS_START( rad_boxp )
	PORT_INCLUDE(rad_box)

	PORT_MODIFY("REGION") // PAL/NTSC flag
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_CUSTOM )
INPUT_PORTS_END

static INPUT_PORTS_START( rad_bass ) // also an analog reel
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Menu") // pressing this ingame currently crashes it if read from sound_irqstatus_r is incorrect?
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Select")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Forward") // used to navigate menus
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Back")

	PORT_MODIFY("IN1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) // definitely the dpad, see map screen
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.

INPUT_PORTS_END

static INPUT_PORTS_START( ekara )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_ekara_state,ekara_multi0_r, nullptr)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_ekara_state,ekara_multi1_r, nullptr)

	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.

	PORT_START("EXTRA0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Echo")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Effects")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_NAME("Key Down")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_NAME("Key Up")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("BGM Down")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("BGM Up")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_NAME("Tempo Down")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON8 ) PORT_NAME("Tempo Up")

	PORT_START("EXTRA1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Select")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Cancel")
	PORT_BIT( 0x3c, IP_ACTIVE_HIGH, IPT_UNUSED )
	// no 40/80 due to multiplexer code
INPUT_PORTS_END

static INPUT_PORTS_START( ddrfammt )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("P1 Up Arrow") PORT_PLAYER(1) // these can't be treated as a joystick as opposing directions are often required
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("P1 Down Arrow") PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("P1 Left Arrow") PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("P1 Right Arrow") PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("P2 Up Arrow") PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("P2 Down Arrow") PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("P2 Left Arrow") PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("P2 Right Arrow") PORT_PLAYER(2)

	PORT_MODIFY("IN1") // once connected the game won't see it as disconnected until you turn off.  I'm guessing this is a signal, not a button?
	PORT_CONFNAME( 0x02,  0x00, "2nd Mat Connected" )
	PORT_CONFSETTING(     0x00, DEF_STR( No ) )
	PORT_CONFSETTING(     0x02, DEF_STR( Yes ) )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

static INPUT_PORTS_START( popira )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Pad 1") PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Pad 2") PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Pad 3") PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Pad 4") PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Select Previous") PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Select Next") PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 ) PORT_NAME("Start") PORT_PLAYER(1)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNUSED ) // halts execution and enables a memory viewer if used with cartridge gc0001 (debug leftover?)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( popira2 ) // player 2 buttons have heavy latency, probably due to incorrect timer emulation, hence NOT WORKING flag right now
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("P1 Pad 1") PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("P1 Pad 2") PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("P1 Pad 3") PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("P1 Pad 4") PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Select Previous") PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Select Next") PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 ) PORT_NAME("Start") PORT_PLAYER(1)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x07, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, xavix_popira2_cart_state,i2c_r, nullptr)
	PORT_BIT( 0x30, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	// main input processing code is at 028059, which ends with setting a timer (028079) to read analog ports and get these buttons that way.  main timer handler is at 00eb77, which reads ports via the ADC.  $c3 where ports are stored is also checked at 00e6f4
	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("P2 Pad 1") PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("P2 Pad 2") PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("P2 Pad 3") PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("P2 Pad 4") PORT_PLAYER(2)
INPUT_PORTS_END



static INPUT_PORTS_START( rad_bassp )
	PORT_INCLUDE(rad_bass)

	PORT_MODIFY("REGION") // PAL/NTSC flag
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_CUSTOM )
INPUT_PORTS_END


static INPUT_PORTS_START( rad_snow )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_NAME("Foward / Go")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_NAME("Enter / Select")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

static INPUT_PORTS_START( rad_snowp )
	PORT_INCLUDE(rad_snow)

	PORT_MODIFY("REGION") // PAL/NTSC flag
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_CUSTOM )
INPUT_PORTS_END

static INPUT_PORTS_START( rad_bdp )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Purple / Up")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Red / Down")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Blue / Back")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Pink / Select")

	PORT_MODIFY("IN1")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

static INPUT_PORTS_START( rad_ping )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Select?")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Pause?")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SERVICE ) // resets? not a real button?

	PORT_MODIFY("IN1") // are these for the 2nd player?
	//PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Pause?")
	//PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	//PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	//PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_SERVICE ) // resets? not a real button?
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

static INPUT_PORTS_START( rad_pingp )
	PORT_INCLUDE(rad_ping)

	PORT_MODIFY("REGION") // PAL/NTSC flag
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_CUSTOM )
INPUT_PORTS_END

static INPUT_PORTS_START( nostalgia )
	PORT_INCLUDE(xavix_i2c)

	PORT_MODIFY("IN0") // mappings based on Dragon Buster button list
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON4 ) // Fire4
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON3 ) // Fire3
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // not a button? (but can be used to pass prompts?)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON5 ) // Pause / Add Coins, marked 'Credit' (but not a coin slot)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON2 ) // Fire2
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // Fire1

INPUT_PORTS_END

static INPUT_PORTS_START( tak_geig )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(1) // pause

	PORT_MODIFY("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(2) // pause

INPUT_PORTS_END


static INPUT_PORTS_START( rad_bb )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Menu Select")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Reset?") // might not be a real button
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Pause")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Menu Previous")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Menu Next")

	PORT_MODIFY("IN1")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

// to access hidden test mode reset while holding Button1 and Button2 (works every other reset)
// to cycle through modes use Button 1 until you get to the input test
static INPUT_PORTS_START( rad_bb2 )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("X")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("O")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

static INPUT_PORTS_START( ttv_mx )
	PORT_INCLUDE(xavix_i2c)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // Accel
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) // Brake
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Pause")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_NAME("Motion Up") // you tilt the device, but actual inputs are digital
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_NAME("Motion Down")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_NAME("Motion Left")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_NAME("Motion Right")
INPUT_PORTS_END


static INPUT_PORTS_START( rad_fb )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END



static INPUT_PORTS_START( rad_rh )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0") // hold Right+Button 3 when resetting for a version number
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Skip") // skips level, cheat or real input?
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Forward")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("Back")

	PORT_MODIFY("IN1")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

static INPUT_PORTS_START( epo_epp )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // select
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) // back
	// 04/08 not used for buttons?
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
INPUT_PORTS_END

// there is also a rumble output, likely mapped to one of the port bits
static INPUT_PORTS_START( epo_guru )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 )

	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) // used in the 'from behind' game at least
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )

	PORT_MODIFY("MOUSE1X")
//  PORT_BIT( 0xff, 0x80, IPT_AD_STICK_X ) PORT_SENSITIVITY(6) PORT_KEYDELTA(16) PORT_PLAYER(1) PORT_MINMAX(0x44,0xbc)
	PORT_BIT( 0x1f, 0x10, IPT_AD_STICK_X ) PORT_SENSITIVITY(6) PORT_KEYDELTA(16) PORT_PLAYER(1) // PORT_MINMAX(0x44,0xbc)

	/*
	 (0x20 is subtracted from value returned in read handler)

	 main game
	 00 still
	 01 - 3c right
	 3d - 78 left  - 78 is a nice slow left but invalid for the sub game
	 79 - 7f right
	 80 - 87 left
	 88 - c3 right
	 c4 - ff left

	 sub game (break-out)
	 00 still
	 01 - 3f right
	 40 - 7f left
	 80 still
	 81 - bf right
	 c0 - ff left

	 so valid range seems to be c4-ff (left), 00 (still), 01-3c (right) even if this means the slowest speed going left is faster than the slowest speed going right
	 maybe actual range is 5 bits either way?
	 4 bits either way seems to work best in practice

	 */
INPUT_PORTS_END

static INPUT_PORTS_START( epo_efdx )
	PORT_INCLUDE(xavix_i2c)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // select
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) // back
	// 04/08 not used for buttons?
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
INPUT_PORTS_END

static INPUT_PORTS_START( has_wamg )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN1")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN ) // waits for this after fading out title, what is it?
INPUT_PORTS_END

static INPUT_PORTS_START( evio )
	PORT_INCLUDE(xavix)

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON2 )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_POWER_OFF ) PORT_NAME("Power Switch") // pressing this will turn the game off.
INPUT_PORTS_END

/* correct, 4bpp gfxs */
static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,1) },
	{ 1*4,0*4,3*4,2*4,5*4,4*4,7*4,6*4 },
	{ STEP8(0,4*8) },
	8*8*4
};

static const gfx_layout char16layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,1) },
	{ 1*4,0*4,3*4,2*4,5*4,4*4,7*4,6*4, 9*4,8*4,11*4,10*4,13*4,12*4,15*4,14*4 },
	{ STEP16(0,4*16) },
	16*16*4
};

static const gfx_layout charlayout8bpp =
{
	8,8,
	RGN_FRAC(1,1),
	8,
	{ STEP8(0,1) },
	{ STEP8(0,8) },
	{ STEP8(0,8*8) },
	8*8*8
};

static const gfx_layout char16layout8bpp =
{
	16,16,
	RGN_FRAC(1,1),
	8,
	{ STEP8(0,1) },
	{ STEP16(0,8) },
	{ STEP16(0,16*8) },
	16*16*8
};

static GFXDECODE_START( gfx_xavix )
	GFXDECODE_ENTRY( "bios", 0, charlayout, 0, 16 )
	GFXDECODE_ENTRY( "bios", 0, char16layout, 0, 16 )
	GFXDECODE_ENTRY( "bios", 0, charlayout8bpp, 0, 1 )
	GFXDECODE_ENTRY( "bios", 0, char16layout8bpp, 0, 1 )
GFXDECODE_END


void xavix_state::xavix(machine_config &config)
{
	/* basic machine hardware */
	XAVIX(config, m_maincpu, MAIN_CLOCK);
	m_maincpu->set_addrmap(AS_PROGRAM, &xavix_state::xavix_map);
	m_maincpu->set_addrmap(5, &xavix_state::xavix_lowbus_map);
	m_maincpu->set_addrmap(6, &xavix_state::xavix_extbus_map);
	m_maincpu->disable_cache();
	m_maincpu->set_vblank_int("screen", FUNC(xavix_state::interrupt));
	m_maincpu->set_vector_callback(FUNC(xavix_state::get_vectors));

	TIMER(config, "scantimer").configure_scanline(FUNC(xavix_state::scanline_cb), "screen", 0, 1);

	ADDRESS_MAP_BANK(config, "lowbus").set_map(&xavix_state::xavix_lowbus_map).set_options(ENDIANNESS_LITTLE, 8, 24, 0x8000);

	XAVIX_ADC(config, m_adc, 0);
	m_adc->read_0_callback().set(FUNC(xavix_state::adc0_r));
	m_adc->read_1_callback().set(FUNC(xavix_state::adc1_r));
	m_adc->read_2_callback().set(FUNC(xavix_state::adc2_r));
	m_adc->read_3_callback().set(FUNC(xavix_state::adc3_r));
	m_adc->read_4_callback().set(FUNC(xavix_state::adc4_r));
	m_adc->read_5_callback().set(FUNC(xavix_state::adc5_r));
	m_adc->read_6_callback().set(FUNC(xavix_state::adc6_r));
	m_adc->read_7_callback().set(FUNC(xavix_state::adc7_r));

	XAVIX_ANPORT(config, m_anport, 0);
	m_anport->read_0_callback().set(FUNC(xavix_state::anport0_r));
	m_anport->read_1_callback().set(FUNC(xavix_state::anport1_r));
	m_anport->read_2_callback().set(FUNC(xavix_state::anport2_r));
	m_anport->read_3_callback().set(FUNC(xavix_state::anport3_r));

	XAVIX_MATH(config, m_math, 0);

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(2500));
	m_screen->set_screen_update(FUNC(xavix_state::screen_update));
	m_screen->set_size(32*8, 32*8);
	m_screen->set_visarea(0*8, 32*8-1, 2*8, 30*8-1);
	m_screen->set_palette(m_palette);
	//m_screen->set_video_attributes(VIDEO_UPDATE_SCANLINE);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_xavix);

	PALETTE(config, m_palette, palette_device::BLACK, 256);

	/* sound hardware */

	//SPEAKER(config, "mono").front_center();
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	XAVIX_SOUND(config, m_sound, MAIN_CLOCK);
	m_sound->read_regs_callback().set(FUNC(xavix_state::sound_regram_read_cb));
	m_sound->read_samples_callback().set(FUNC(xavix_state::sample_read));
	//m_sound->add_route(ALL_OUTPUTS, "mono", 1.0);
	m_sound->add_route(0, "lspeaker", 1.0);
	m_sound->add_route(1, "rspeaker", 1.0);
}

void xavix_guru_state::xavix_guru(machine_config &config)
{
	xavix_nv(config);

	m_anport->read_2_callback().set(FUNC(xavix_guru_state::guru_anport2_r));
}


void xavix_i2c_state::xavix_i2c_24lc02(machine_config &config)
{
	xavix(config);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x100); // 24LC02 (taiko)
}

void xavix_i2c_state::xavix_i2c_24c02(machine_config &config)
{
	xavix(config);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x100); // 24C02
}

void xavix_i2c_state::xavix_i2c_24lc04(machine_config &config)
{
	xavix(config);

	// according to http://ww1.microchip.com/downloads/en/devicedoc/21708k.pdf 'the master transmits up to 16 data bytes' however this breaks the Nostalgia games
	// of note Galplus Phalanx on Namco Nostalgia 2, which will hang between stages unable to properly access the device, but with no page support it doesn't hang and scores save
	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x200); // 24LC04 on Nostalgia games, 24C04 on others
}

void xavix_i2c_ltv_tam_state::xavix_i2c_24lc04_tam(machine_config &config)
{
	xavix_i2c_24lc04(config);

	m_anport->read_0_callback().set(FUNC(xavix_i2c_ltv_tam_state::tam_anport0_r));
	m_anport->read_1_callback().set(FUNC(xavix_i2c_ltv_tam_state::tam_anport1_r));
	m_anport->read_2_callback().set(FUNC(xavix_i2c_ltv_tam_state::tam_anport2_r));
	m_anport->read_3_callback().set(FUNC(xavix_i2c_ltv_tam_state::tam_anport3_r));
}

void xavix_i2c_state::xavix_i2c_24c08(machine_config &config)
{
	xavix(config);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x400); // 24C08 (Excite Fishing DX)
}

void xavix_state::xavixp(machine_config &config)
{
	// other clocks should change too!

	xavix(config);
	m_screen->set_refresh_hz(50);
}

void xavix_state::xavix_nv(machine_config &config)
{
	xavix(config);
	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_1);
}

void xavix_state::xavixp_nv(machine_config &config)
{
	xavixp(config);
	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_1);
}

void xavix_state::xavix2000(machine_config &config)
{
	xavix(config);

	XAVIX2000(config.replace(), m_maincpu, MAIN_CLOCK);
	m_maincpu->set_addrmap(AS_PROGRAM, &xavix_state::xavix_map);
	m_maincpu->set_addrmap(5, &xavix_state::xavix_lowbus_map);
	m_maincpu->set_addrmap(6, &xavix_state::xavix_extbus_map);
	m_maincpu->disable_cache();
	m_maincpu->set_vblank_int("screen", FUNC(xavix_state::interrupt));
	m_maincpu->set_vector_callback(FUNC(xavix_state::get_vectors));

	m_palette->set_entries(512);
}

void xavix_state::xavix2002(machine_config &config)
{
	xavix(config);

	XAVIX2002(config.replace(), m_maincpu, MAIN_CLOCK);
	m_maincpu->set_addrmap(AS_PROGRAM, &xavix_state::xavix_map);
	m_maincpu->set_addrmap(5, &xavix_state::superxavix_lowbus_map); // has extra video, io etc.
	m_maincpu->set_addrmap(6, &xavix_state::xavix_extbus_map);
	m_maincpu->disable_cache();
	m_maincpu->set_vblank_int("screen", FUNC(xavix_state::interrupt));
	m_maincpu->set_vector_callback(FUNC(xavix_state::get_vectors));

	m_palette->set_entries(512);

	XAVIX2002IO(config, m_xavix2002io, 0);
}



void xavix_i2c_jmat_state::xavix2002_i2c_jmat(machine_config &config)
{
	xavix2002(config);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x200); // ?

	m_xavix2002io->read_0_callback().set(FUNC(xavix_i2c_jmat_state::read_extended_io0));
	m_xavix2002io->write_0_callback().set(FUNC(xavix_i2c_jmat_state::write_extended_io0));
	m_xavix2002io->read_1_callback().set(FUNC(xavix_i2c_jmat_state::read_extended_io1));
	m_xavix2002io->write_1_callback().set(FUNC(xavix_i2c_jmat_state::write_extended_io1));
	m_xavix2002io->read_2_callback().set(FUNC(xavix_i2c_jmat_state::read_extended_io2));
	m_xavix2002io->write_2_callback().set(FUNC(xavix_i2c_jmat_state::write_extended_io2));
}


void xavix_state::xavix2000_nv(machine_config &config)
{
	xavix2000(config);

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_1);
}

void xavix_2000_nv_sdb_state::xavix2000_nv_sdb(machine_config &config)
{
	xavix2000_nv(config);

	m_anport->read_0_callback().set(FUNC(xavix_2000_nv_sdb_state::sdb_anport0_r));
	m_anport->read_1_callback().set(FUNC(xavix_2000_nv_sdb_state::sdb_anport1_r));
	m_anport->read_2_callback().set(FUNC(xavix_2000_nv_sdb_state::sdb_anport2_r));
	m_anport->read_3_callback().set(FUNC(xavix_2000_nv_sdb_state::sdb_anport3_r));
}

void xavix_i2c_state::xavix2000_i2c_24c04(machine_config &config)
{
	xavix2000(config);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x200); // 24C04
}

void xavix_i2c_state::xavix2000_i2c_24c02(machine_config &config)
{
	xavix2000(config);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x100); // 24C02
}

void xavix_mtrk_state::xavix_mtrk(machine_config &config)
{
	xavix(config);

	XAVIX_MTRK_WHEEL(config, m_wheel, 0);
	m_wheel->event_out_cb().set(FUNC(xavix_state::ioevent_trg08));
}

void xavix_mtrk_state::xavix_mtrkp(machine_config &config)
{
	xavix_mtrk(config);

	m_screen->set_refresh_hz(50);
}

void xavix_madfb_state::xavix_madfb(machine_config &config)
{
	xavix(config);

	XAVIX_MADFB_BALL(config, m_ball, 0);
	m_ball->event_out_cb().set(FUNC(xavix_state::ioevent_trg01));
}


void xavix_cart_state::xavix_cart(machine_config &config)
{
	xavix(config);

	EKARA_CART_SLOT(config, m_cartslot, 0, ekara_cart, nullptr);
}

void xavix_i2c_cart_state::xavix_i2c_taiko(machine_config &config)
{
	xavix_cart(config);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x100); // 24LC02

	SOFTWARE_LIST(config, "cart_list_japan_d").set_original("ekara_japan_d");
	SOFTWARE_LIST(config, "cart_list_japan_sp").set_original("ekara_japan_sp");
}

void xavix_i2c_cart_state::xavix_i2c_jpopira(machine_config &config)
{
	xavix_cart(config);

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x100); // 24LC02

	SOFTWARE_LIST(config, "cart_list_jpopira_jp").set_original("jpopira_jp"); // NOTE, these are for Jumping Popira only, they don't work with the karaoke or regular popira units
	SOFTWARE_LIST(config, "cart_list_japan_sp").set_original("ekara_japan_sp");
}

void xavix_cart_state::xavix_cart_ekara(machine_config &config)
{
	xavix_cart(config);

	/* software lists */
	SOFTWARE_LIST(config, "cart_list_us").set_original("ekara_us");
	SOFTWARE_LIST(config, "cart_list_pal").set_original("ekara_pal");
	SOFTWARE_LIST(config, "cart_list_japan").set_original("ekara_japan");
	SOFTWARE_LIST(config, "cart_list_japan_g").set_original("ekara_japan_g");
	SOFTWARE_LIST(config, "cart_list_japan_p").set_original("ekara_japan_p");
	SOFTWARE_LIST(config, "cart_list_japan_s").set_original("ekara_japan_s");
	SOFTWARE_LIST(config, "cart_list_japan_m").set_original("ekara_japan_m");
	SOFTWARE_LIST(config, "cart_list_japan_d").set_original("ekara_japan_d");
	SOFTWARE_LIST(config, "cart_list_japan_en").set_original("ekara_japan_en");
	SOFTWARE_LIST(config, "cart_list_japan_sp").set_original("ekara_japan_sp");
	SOFTWARE_LIST(config, "cart_list_japan_web").set_original("ekara_japan_web");
	SOFTWARE_LIST(config, "cart_list_japan_a").set_original("ekara_japan_a");
	SOFTWARE_LIST(config, "cart_list_japan_gk").set_original("ekara_japan_gk");
	SOFTWARE_LIST(config, "cart_list_japan_bh").set_original("ekara_japan_bh");
	SOFTWARE_LIST(config, "cart_list_japan_ac").set_original("ekara_japan_ac");
}

void xavix_cart_state::xavix_cart_popira(machine_config &config)
{
	xavix_cart(config);

	// is a battery / power source required to store NVRAM in the CPU?  Popira definitely needs NVRAM storing on power-of
	// XaviX Tennis won't boot if you do (but that could be an unrelated SEEPROM issue?) & DDR Family Mat gets stuck in 2 Player mode with no obvious way of changing back
	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_1);

	/* software lists */
	SOFTWARE_LIST(config, "cart_list_japan_g").set_original("ekara_japan_g");
	SOFTWARE_LIST(config, "cart_list_japan_p").set_original("ekara_japan_p");
	SOFTWARE_LIST(config, "cart_list_japan_d").set_original("ekara_japan_d");
	SOFTWARE_LIST(config, "cart_list_japan_sp").set_original("ekara_japan_sp");
}

// see code at 028060, using table from 00eb6d for conversion
READ8_MEMBER(xavix_popira2_cart_state::popira2_adc0_r)
{
	uint8_t p2 = m_p2->read() & 0x03;
	switch (p2)
	{
	case 0x00: return 0xa0;
	case 0x01: return 0x60;
	case 0x02: return 0x10;
	case 0x03: return 0x00;
	}

	return 0x00;
}

READ8_MEMBER(xavix_popira2_cart_state::popira2_adc1_r)
{
	uint8_t p2 = (m_p2->read() >> 2) & 0x03;
	switch (p2)
	{
	case 0x00: return 0xa0;
	case 0x01: return 0x60;
	case 0x02: return 0x10;
	case 0x03: return 0x00;
	}

	return 0x00;
}

void xavix_popira2_cart_state::xavix_cart_popira2(machine_config &config)
{
	xavix_cart_popira(config);

	m_adc->read_0_callback().set(FUNC(xavix_popira2_cart_state::popira2_adc0_r));
	m_adc->read_1_callback().set(FUNC(xavix_popira2_cart_state::popira2_adc1_r));
}

void xavix_cart_state::xavix_cart_ddrfammt(machine_config &config)
{
	xavix_cart(config);

	/* software lists */
	SOFTWARE_LIST(config, "cart_list_japan_p").set_original("ekara_japan_p");
	//SOFTWARE_LIST(config, "cart_list_japan_sp").set_original("ekara_japan_sp"); // not for this system, but unlike other carts, actually tells you this if inserted rather than crashing the system
}


void xavix_i2c_state::xavix2002_i2c_24c04(machine_config &config)
{
	xavix2002(config);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(0x200); // 24C04
}

void xavix_state::init_xavix()
{
	m_rgnlen = memregion("bios")->bytes();
	m_rgn = memregion("bios")->base();
}

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( taitons1 )
	ROM_REGION( 0x200000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "taitonostalgia1.u3", 0x000000, 0x200000, CRC(25bd8c67) SHA1(a109cd2da6aa4596e3ca3abd1afce2d0001a473f) )
ROM_END

ROM_START( taitons2 )
	ROM_REGION( 0x200000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "taitonostalgia2.bin", 0x000000, 0x200000, CRC(d7dbd93d) SHA1(ad96f80d317e7fd64682a1fe406c5ee9dd5eabf9) )
ROM_END

ROM_START( namcons1 )
	ROM_REGION( 0x100000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "namconostalgia1.bin", 0x000000, 0x100000, CRC(9bcccccd) SHA1(cf8fe6de76fbd23974f999299db6f558f79c8f22) )
ROM_END

ROM_START( namcons2 )
	ROM_REGION( 0x100000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "nostalgia.bin", 0x000000, 0x100000, CRC(03f7f755) SHA1(bdf1b10ab0104ed580951b0c428c4e93e7373afe) )
ROM_END

ROM_START( rad_box )
	ROM_REGION(0x200000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("boxing.bin", 0x000000, 0x200000, CRC(5cd40714) SHA1(165260228c029a9502ca0598c84c24fd9bdeaebe) )
ROM_END

ROM_START( rad_boxp )
	ROM_REGION(0x200000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("boxing.bin", 0x000000, 0x200000, CRC(5cd40714) SHA1(165260228c029a9502ca0598c84c24fd9bdeaebe) )
ROM_END

ROM_START( rad_bass )
	ROM_REGION(0x100000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("bassfishin.bin", 0x000000, 0x100000, CRC(b54eb1c5) SHA1(084faa9349369f2b8846950765f9c8f758db3e9e) )
ROM_END

ROM_START( rad_bassp )
	ROM_REGION(0x100000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("bassfishin.bin", 0x000000, 0x100000, CRC(b54eb1c5) SHA1(084faa9349369f2b8846950765f9c8f758db3e9e) )
ROM_END

ROM_START( rad_opus )
	ROM_REGION(0x100000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("opus.bin", 0x000000, 0x100000, CRC(509df402) SHA1(c5b863670eac8498ddda9dfde91387634cf7aa9f) )
ROM_END

ROM_START( rad_hnt )
	ROM_REGION(0x100000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("huntin1.bin", 0x000000, 0x100000, CRC(e51e250f) SHA1(d72199096d466cd344bb243ef1228e0df9501d00) )
ROM_END

ROM_START( rad_hnt2 )
	ROM_REGION(0x200000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("huntin2.bin", 0x000000, 0x200000, CRC(fb6846df) SHA1(267632790ed42eba7ef1517b86b024799a78839d) )
ROM_END


ROM_START( rad_snow )
	ROM_REGION(0x100000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("snoblu.bin", 0x000000, 0x100000, CRC(593e40b3) SHA1(03483ac39eddd7746470fb60018e704382b0da59) )
ROM_END

ROM_START( rad_snowp )
	ROM_REGION(0x100000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("snoblu.bin", 0x000000, 0x100000, CRC(593e40b3) SHA1(03483ac39eddd7746470fb60018e704382b0da59) )
ROM_END


ROM_START( rad_ping )
	ROM_REGION( 0x100000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "pingpong.bin", 0x000000, 0x100000, CRC(629f7f47) SHA1(2bb19fd202f1e6c319d2f7d18adbfed8a7669235) )
ROM_END

ROM_START( rad_pingp )
	ROM_REGION( 0x100000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "pingpong.bin", 0x000000, 0x100000, CRC(629f7f47) SHA1(2bb19fd202f1e6c319d2f7d18adbfed8a7669235) )
ROM_END

ROM_START( rad_crdn )
	ROM_REGION( 0x100000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "cardnight.bin", 0x000000, 0x100000, CRC(d19eba08) SHA1(cedb9fe785f2a559f518a1d8ecf80d500ddc63c7) )
ROM_END

ROM_START( rad_crdnp )
	ROM_REGION( 0x100000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "cardnight.bin", 0x000000, 0x100000, CRC(d19eba08) SHA1(cedb9fe785f2a559f518a1d8ecf80d500ddc63c7) )
ROM_END

ROM_START( rad_bb )
	ROM_REGION( 0x100000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "radicabaseball.bin", 0x000000, 0x100000, CRC(3fa6f490) SHA1(0772156a67a22d06f5ffd6d1a77f6dc867d0a6d2) )
ROM_END

ROM_START( rad_bb2 )
	ROM_REGION( 0x200000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "baseball2.bin", 0x000000, 0x200000, CRC(bdbf6202) SHA1(18d5cc2d77cbb734629a7a5b6e0f419d21beedbd) )
ROM_END

ROM_START( rad_mtrk )
	ROM_REGION( 0x400000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "monstertruck.bin", 0x000000, 0x400000, CRC(dccda0a7) SHA1(7953cf29643672f8367639555b797c20bb533eab) )
ROM_END

ROM_START( rad_mtrkp ) // rom was dumped from NTSC unit, assuming to be the same
	ROM_REGION( 0x400000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "monstertruck.bin", 0x000000, 0x400000, CRC(dccda0a7) SHA1(7953cf29643672f8367639555b797c20bb533eab) )
ROM_END

ROM_START( rad_ssx )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("snowssx.bin", 0x000000, 0x400000, CRC(108e19a6) SHA1(3dfb18efb6331b96a53138a5ba29dae9cd966e90) )
ROM_END

ROM_START( rad_ssxp )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("snowssx.bin", 0x000000, 0x400000, CRC(108e19a6) SHA1(3dfb18efb6331b96a53138a5ba29dae9cd966e90) )
ROM_END

ROM_START( rad_sbw )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("snowbwhite.bin", 0x000000, 0x400000, CRC(640c1473) SHA1(d37d1484a5b14735b35afbca305dad7d178b08a2) )
ROM_END

ROM_START( rad_bdp )
	ROM_REGION(0x800000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("barbiepad.bin", 0x000000, 0x200000, CRC(48731512) SHA1(377d4e1c98cafcd9d5e1ee27943289d250a6e7a9) )
ROM_END

ROM_START( rad_madf )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("madden.bin", 0x000000, 0x400000, CRC(e972fdcf) SHA1(52001316254880755da959c3441d232fd2c72c7a) )
ROM_END

ROM_START( rad_fb )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("rfootball.bin", 0x000000, 0x400000, CRC(025e0cb4) SHA1(60ce363de236d5119d078e346ad5d2ae50dbc7e1) )
ROM_END

ROM_START( epo_efdx )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("excitefishing.bin", 0x000000, 0x400000, CRC(9c85b261) SHA1(6a363faed2ec89c5176e46554a98ca1e20132579) )
ROM_END

ROM_START( epo_epp )
	ROM_REGION(0x100000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("excitepingpong.bin", 0x000000, 0x100000, CRC(1fdb9cbd) SHA1(8ed0c1f6d2708ab6e79f0b9553e587c6446e8338) )
ROM_END

ROM_START( epo_epp3 )
	ROM_REGION(0x200000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("excitepingpong3.bin", 0x000000, 0x200000, CRC(a2ee8bff) SHA1(6e16dbaac9680e1f311c08e3f573d0cf8708b446))
ROM_END

ROM_START( rad_socr )
	ROM_REGION(0x800000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("soccer.u5", 0x000000, 0x100000, CRC(0564a766) SHA1(bded4f2e0d059731164c35de7df62ef411402901) )
	ROM_LOAD("soccer.u4", 0x400000, 0x200000, CRC(e0e2c488) SHA1(907511d945dec51782af268a94b372dfeb7517c5) )
ROM_END

ROM_START( epo_guru )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("gururinworld.bin", 0x000000, 0x400000, CRC(e5ae4523) SHA1(0e39ef8f94203d34e49422081667805f50a339a1) )
ROM_END

ROM_START( epo_dmon ) // Doraemon has a phototransistor, microphone and 24LC02, PCB says XaviX 2002
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("doraemon.bin", 0x000000, 0x400000, CRC(d8f5412f) SHA1(11a4c017ed400f7aa585be744d9693efe734c192) )
ROM_END

ROM_START( rad_rh )
	ROM_REGION(0x200000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("rescueheroes.bin", 0x000000, 0x200000, CRC(38c391a7) SHA1(120334d4ce89d98438c2a35bf7e53af5096cc878) )
ROM_END

ROM_START( has_wamg )
	ROM_REGION( 0x400000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "minigolf.bin", 0x000000, 0x400000, CRC(35cee2ad) SHA1(c7344e8ba336bc329638485ea571cd731ebf7649) )
ROM_END

ROM_START( ltv_tam )
	ROM_REGION( 0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("letstvtamagotchi.bin", 0x000000, 0x400000, CRC(e3723272) SHA1(e72e128a7a24afb96baafe5f13e13a0d74db4033) )
ROM_END


ROM_START( tak_geig )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("geigeki.bin", 0x000000, 0x400000, CRC(bd0c3576) SHA1(06f614dbec0225ce4ed866b98450912986d72faf) )
ROM_END

/*
    The e-kara cartridges require the BIOS rom to map into 2nd external bus space as they fetch palette data from
    it etc.
*/

ROM_START( ekara )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "ekara.bin", 0x600000, 0x100000, CRC(9b27c4a2) SHA1(d75dda7434933135d2f7e353840a9384e9a0d586) )
	ROM_RELOAD(0x000000, 0x100000)
ROM_END

ROM_START( ekaraa )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "ekara2.bin", 0x600000, 0x100000, CRC(3c92d48d) SHA1(450fbe53826cdb87ec797f84b9757987afcc1ec5) )
	ROM_RELOAD(0x000000, 0x100000)
ROM_END

ROM_START( ekaraj )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "ekarajapan.bin", 0x600000, 0x100000, CRC(e459e43b) SHA1(58b7f36a81571a2df5e812c118fdf68812a05abc) )
	ROM_RELOAD(0x000000, 0x100000)
ROM_END

ROM_START( ekaraphs )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "ekaraheadset.bin", 0x600000, 0x200000, CRC(dd9b3cd7) SHA1(baaf35d56fa45b6f995b8466331bb30f0035f734) )
	ROM_RELOAD(0x000000, 0x200000)
ROM_END

ROM_START( ekarag )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "ekaragermany.bin", 0x600000, 0x200000, CRC(644f06b0) SHA1(433e65a6ea029b67ee62b4b3de52ff3504cfdf16) )
	ROM_RELOAD(0x000000, 0x200000)
ROM_END


ROM_START( epitch )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "ekarajapan.bin", 0x600000, 0x100000, CRC(e459e43b) SHA1(58b7f36a81571a2df5e812c118fdf68812a05abc) )
	ROM_RELOAD(0x000000, 0x100000)
ROM_END

ROM_START( ekaramix )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	// also has a MX87L100C 28-pin chip in the unit, possibly the USB controller.  Part of the ROM contains a FAT filesystem that could possibly appear as a USB drive on the PC?
	ROM_LOAD( "ekaramix.bin", 0x600000, 0x200000, CRC(ee71576e) SHA1(26f8c9edcbbed77e86a1cb5a0b91c92a16fef433) )
	ROM_RELOAD(0x000000, 0x200000)
ROM_END

ROM_START( ddrfammt )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "ekara_ddr_ha010_81947.bin", 0x600000, 0x200000, CRC(737d5d1a) SHA1(a1043047056dd27bca69767ee2044461ec549465) )
	ROM_RELOAD(0x000000, 0x200000)
ROM_END

ROM_START( popira )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "popira.bin", 0x600000, 0x100000, CRC(71d20afc) SHA1(fb2ae023fc8a188c79b2d6550f737e06c05e12da) )
	ROM_RELOAD(0x000000, 0x100000)
ROM_END

ROM_START( popira2 )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "popira2.bin", 0x600000, 0x200000, CRC(6acd8be0) SHA1(8b6c812d37e2dc2f2531b0e47d84ad8485729dc5) )
	ROM_RELOAD(0x000000, 0x200000)
ROM_END

ROM_START( taikodp )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "taikodepopira.bin", 0x600000, 0x200000, CRC(037a8472) SHA1(03cae465965935fc084fb906f8f5de7679f42dd1) )
	ROM_RELOAD(0x000000, 0x200000)
ROM_END

ROM_START( jpopira )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "jumpingpopira.bin", 0x600000, 0x200000, CRC(a7bedbd2) SHA1(d62d4ca660c8df14891217fb7b7a2b4a931ff35f) )
	ROM_RELOAD(0x000000, 0x200000)

	ROM_REGION( 0x100, "i2cmem", ROMREGION_ERASE00 ) // maybe we can remove this eventually, but for now it won't init without a reset between
	ROM_LOAD( "i2cmem.bin", 0x000, 0x100, CRC(70a05af1) SHA1(e8f4ab51445777fe459f9ff09333f548c4e3507c) )
ROM_END

ROM_START( evio )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "evio.bin", 0x600000, 0x200000, CRC(ee22c764) SHA1(f2b7e213eb78065a63ef484a619bcfc61299e30e))
	ROM_RELOAD(0x000000, 0x200000)
ROM_END




/* XaviX hardware titles (1st Generation)

    These use
    SSD 98 PL7351-181
    SSD 98 PA7351-107
    SSD 97 PA7270-107
    type CPUS

    only new opcodes are callf and retf?

*/

CONS( 2006, taitons1,  0,          0,  xavix_i2c_24lc04, nostalgia,xavix_i2c_state,      init_xavix,    "Bandai / SSD Company LTD / Taito",             "Let's! TV Play Classic - Taito Nostalgia 1 (Japan)", MACHINE_IMPERFECT_SOUND )

CONS( 2006, taitons2,  0,          0,  xavix_i2c_24lc04, nostalgia,xavix_i2c_state,      init_xavix,    "Bandai / SSD Company LTD / Taito",             "Let's! TV Play Classic - Taito Nostalgia 2 (Japan)", MACHINE_IMPERFECT_SOUND )

CONS( 2006, namcons1,  0,          0,  xavix_i2c_24lc04, nostalgia,xavix_i2c_state,      init_xavix,    "Bandai / SSD Company LTD / Namco",             "Let's! TV Play Classic - Namco Nostalgia 1 (Japan)", MACHINE_IMPERFECT_SOUND )

CONS( 2006, namcons2,  0,          0,  xavix_i2c_24lc04, nostalgia,xavix_i2c_state,      init_xavix,    "Bandai / SSD Company LTD / Namco",             "Let's! TV Play Classic - Namco Nostalgia 2 (Japan)", MACHINE_IMPERFECT_SOUND )

CONS( 2000, rad_ping,  0,          0,  xavix,            rad_ping, xavix_state,          init_xavix,    "Radica / SSD Company LTD / Simmer Technology", "Play TV Ping Pong (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND ) // "Simmer Technology" is also known as "Hummer Technology Co., Ltd"
CONS( 2000, rad_pingp, rad_ping,   0,  xavixp,           rad_pingp,xavix_state,          init_xavix,    "Radica / SSD Company LTD / Simmer Technology", "ConnecTV Table Tennis (PAL)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

CONS( 2000, rad_opus,  0,          0,  xavix_nv,         rad_opus, xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Opus (NTSC)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // there is a missing 'TV Static' noise effect when menus appear (box shows 'Play TV' ingame just shows 'Radica:Plug & Play')
// ConnecTV Opus is a different ROM in this case? Game doesn't read from PAL/NTSC flag? (verify it isn't detecting PAL in some other way) ( https://www.youtube.com/watch?v=Ub9tstLEj_0 )

CONS( 2000, rad_hnt,   0,          0,  xavix_nv,         rad_hnt,  xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Buckmasters Huntin' (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // need to map gun (box shows 'Play TV' ingame just shows 'Plug & Play')

CONS( 2003, rad_hnt2,  0,          0,  xavix_nv,         rad_hnt,  xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Buckmasters Huntin' 2 (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // need to map gun, crashes on pause

CONS( 2003, rad_mtrk,  0,          0,  xavix_mtrk,       rad_mtrk, xavix_mtrk_state,     init_xavix,    "Radica / SSD Company LTD",                     "Play TV Monster Truck (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
CONS( 2003, rad_mtrkp, rad_mtrk,   0,  xavix_mtrkp,      rad_mtrkp,xavix_mtrk_state,     init_xavix,    "Radica / SSD Company LTD",                     "ConnecTV Monster Truck (PAL)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

CONS( 200?, rad_box,   0,          0,  xavix,            rad_box,  xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Boxin' (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
CONS( 200?, rad_boxp,  rad_box,    0,  xavixp,           rad_boxp, xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "ConnecTV Boxin' (PAL)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

CONS( 200?, rad_crdn,  0,          0,  xavix,            rad_crdn, xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Card Night (NTSC)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )
CONS( 200?, rad_crdnp, rad_crdn,   0,  xavixp,           rad_crdnp,xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "ConnecTV Card Night (PAL)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )

CONS( 2000, rad_bb,    0,          0,  xavix,            rad_bb,   xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Baseball (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // Play TV branding used on box, not ingame

CONS( 2002, rad_bb2,   0,          0,  xavix,            rad_bb2,  xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Baseball 2 (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // contains string "Radica RBB2 V1.0"

CONS( 2001, rad_bass,  0,          0,  xavix,            rad_bass, xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Bass Fishin' (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
CONS( 2001, rad_bassp, rad_bass,   0,  xavixp,           rad_bassp,xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "ConnecTV Bass Fishin' (PAL)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

// there is another 'Snowboarder' with a white coloured board, it appears to be a newer game closer to 'SSX Snowboarder' but without the SSX license.
CONS( 2001, rad_snow,  0,          0,  xavix_nv,         rad_snow, xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Snowboarder (Blue) (NTSC)", MACHINE_IMPERFECT_SOUND )
CONS( 2001, rad_snowp, rad_snow,   0,  xavixp_nv,        rad_snowp,xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "ConnecTV Snowboarder (Blue) (PAL)", MACHINE_IMPERFECT_SOUND )

CONS( 2003, rad_madf,  0,          0,  xavix_madfb,      rad_fb,   xavix_madfb_state,    init_xavix,    "Radica / Electronics Arts / SSD Company LTD",  "EA Sports Madden Football (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND ) // no Play TV branding, USA only release?

CONS( 200?, rad_fb,    0,          0,  xavix_madfb,      rad_fb,   xavix_madfb_state,    init_xavix,    "Radica / SSD Company LTD",                     "Play TV Football (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND ) // USA only release? doesn't change logo for PAL.

CONS( 200?, rad_rh,    0,          0,  xavix,            rad_rh,   xavix_state,          init_xavix,    "Radica / Fisher-Price / SSD Company LTD",      "Play TV Rescue Heroes (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

CONS( 2004, rad_ssx,   0,          0,  xavix,            rad_snow, xavix_state,          init_xavix,    "Radica / Electronics Arts / SSD Company LTD",  "Play TV SSX Snowboarder (NTSC)", MACHINE_IMPERFECT_SOUND )
CONS( 2004, rad_ssxp,  rad_ssx,    0,  xavixp,           rad_snowp,xavix_state,          init_xavix,    "Radica / Electronics Arts / SSD Company LTD",  "ConnecTV SSX Snowboarder (PAL)", MACHINE_IMPERFECT_SOUND )

// basically a reissue of SSX but without the license
CONS( 2006, rad_sbw,   0,          0,  xavix,            rad_snow, xavix_state,          init_xavix,    "Radica / SSD Company LTD",                     "Play TV Snowboarder (White) (NTSC)", MACHINE_IMPERFECT_SOUND )
// doesn't exist with ConnecTV branding?

CONS( 2002, rad_bdp,   0,          0,  xavix,            rad_bdp,  xavix_state,          init_xavix,    "Radica / Mattel / SSD Company LTD",            "Barbie Dance Party", MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )

CONS( 2002, rad_socr,  0,          0,  xavix,            epo_epp,  xavix_state,          init_xavix,    "Radica / Epoch / SSD Company LTD",             "Play TV Soccer", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

CONS( 2000, epo_epp,   0,          0,  xavix,            epo_epp,  xavix_state,          init_xavix,    "Epoch / SSD Company LTD",                      "Excite Ping Pong (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
// Excite Ping Pong 2 is from 2003, and there's a 3rd game from 2006 also

CONS( 2006, epo_epp3,   0,          0,  xavix,            epo_epp,  xavix_state,          init_xavix,    "Epoch / SSD Company LTD",                     "Challenge Ai-chan! Excite Ping Pong (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

CONS( 200?, epo_efdx,  0,          0,  xavix_i2c_24c08,  epo_efdx, xavix_i2c_state,      init_epo_efdx, "Epoch / SSD Company LTD",                      "Excite Fishing DX (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

CONS( 2005, epo_guru,  0,          0,  xavix_guru,       epo_guru,    xavix_guru_state,          init_xavix,    "Epoch / SSD Company LTD",                      "Gururin World (Japan)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )

CONS( 2002, epo_dmon, 0,           0,  xavix_i2c_24c02,  xavix_i2c,xavix_i2c_state,      init_xavix,    "Epoch / SSD Company LTD",                      "Doraemon Wakuwaku Kuukihou (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND ) // full / proper title?

CONS( 200?, has_wamg,  0,          0,  xavix,            has_wamg, xavix_state,          init_xavix,    "Hasbro / Milton Bradley / SSD Company LTD",    "TV Wild Adventure Mini Golf (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

CONS( 2002, tak_geig,  0,          0,  xavix_nv,         tak_geig, xavix_state,          init_xavix,    "Takara / SSD Company LTD",                     "Geigeki Go Go Shooting (Japan)", MACHINE_IMPERFECT_SOUND )

/* Music titles: Emulation note:
   SEEPROM write appears to work (save NVRAM file looks valid) but game fails to read it back properly, fails backup data checksum, and blanks it again.
   Timers might not be 100%, PAL stuff uses different ways to do timing.
*/
CONS( 2000, ekara,    0,           0,  xavix_cart_ekara, ekara,    xavix_ekara_state,    init_xavix,    "Takara / SSD Company LTD / Hasbro",            "e-kara (US?, NTSC, set 1)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*| MACHINE_IS_BIOS_ROOT*/ ) // shows "Please insert a cartridge before turn it on" without cart
CONS( 2000, ekaraa,   ekara,       0,  xavix_cart_ekara, ekara,    xavix_ekara_state,    init_xavix,    "Takara / SSD Company LTD / Hasbro",            "e-kara (US?, NTSC, set 2)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*| MACHINE_IS_BIOS_ROOT*/ ) // shows "Please insert a cartridge before turning on e-kara" without cart
CONS( 2000, ekaraj,   ekara,       0,  xavix_cart_ekara, ekara,    xavix_ekara_state,    init_xavix,    "Takara / SSD Company LTD",                     "e-kara (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*| MACHINE_IS_BIOS_ROOT*/ ) // shows Japanese message without cart
CONS( 2002, ekarag,   ekara,       0,  xavix_cart_ekara, ekara,    xavix_ekara_state,    init_xavix,    "Takara / SSD Company LTD",                     "e-kara (Germany?, includes 3 songs)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*| MACHINE_IS_BIOS_ROOT*/ ) // found in Germany, could just be a Europe-wide model tho?, NOT a headset model, but still has 3 songs.
// the 'e-kara pro headset' has 3 songs built in for the US release.  The Japanese release of this appears to be called 'e-kara H.S.' and it is unclear if it also has built in songs.  The Canadian box says 'cartridge contains' instead of 'songs included' but is likely a printing error.
CONS( 2002, ekaraphs, ekara,       0,  xavix_cart_ekara, ekara,    xavix_ekara_state,    init_xavix,    "Takara / SSD Company LTD",                     "e-kara Pro Headset (US, includes 3 songs)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*| MACHINE_IS_BIOS_ROOT*/ )

// epitch (at least the pichi pichi pitch mermaid starter pack) uses the same internal rom as the Japanese ekara, but has less buttons, so some features aren't available (some games also seem to expect to read it with a different layout eg 'a7' cart, but 'a5' cart doesn't, so must be a way to enable that mode, or bug in code?)
CONS( 2003, epitch,   0,           0,  xavix_cart_ekara, ekara,    xavix_ekara_state,    init_xavix,    "Takara / SSD Company LTD",                     "e-pitch (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*| MACHINE_IS_BIOS_ROOT*/ ) // shows Japanese message without cart

// e-kara mix was another unit that allowed you to connect to a PC, unlike e-kara web it also functions as a regular device
CONS( 200?, ekaramix, 0,           0,  xavix_cart_ekara, ekara,    xavix_ekara_state,    init_xavix,    "Takara / SSD Company LTD",                     "e-kara Mix (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*| MACHINE_IS_BIOS_ROOT*/ )


CONS( 2001, ddrfammt, 0,           0,  xavix_cart_ddrfammt,ddrfammt, xavix_cart_state,   init_xavix,    "Takara / Konami / SSD Company LTD",            "Dance Dance Revolution Family Mat (Japan)", MACHINE_IMPERFECT_SOUND/*|MACHINE_IS_BIOS_ROOT*/ )

CONS( 2000, popira,   0,           0,  xavix_cart_popira,popira,   xavix_cart_state,     init_xavix,    "Takara / SSD Company LTD",                     "Popira (Japan)", MACHINE_IMPERFECT_SOUND/*|MACHINE_IS_BIOS_ROOT*/ ) // The original Popira is a single yellow unit

CONS( 2002, popira2,  0,           0,  xavix_cart_popira2,popira2,  xavix_popira2_cart_state, init_xavix,    "Takara / SSD Company LTD",                 "Popira 2 (Japan)", MACHINE_IMPERFECT_SOUND/*|MACHINE_IS_BIOS_ROOT*/ ) // Popira 2 is a set of 2 blue & green linked units (2nd unit is just a controller, no CPU or TV out)

CONS( 2003, taikodp,  0,           0,  xavix_i2c_taiko,  taikodp,  xavix_i2c_cart_state, init_xavix,    "Takara / SSD Company LTD",                     "Taiko De Popira (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*|MACHINE_IS_BIOS_ROOT*/ ) // inputs? are the drums analog?

CONS( 2004, jpopira,  0,           0,  xavix_i2c_jpopira,jpopira,  xavix_i2c_cart_state, init_xavix,    "Takara / SSD Company LTD",                     "Jumping Popira (Japan)", MACHINE_IMPERFECT_SOUND /*|MACHINE_IS_BIOS_ROOT*/ )


CONS( 2003, evio,     0,           0,  xavix_nv,         evio,     xavix_state,          init_xavix,    "Tomy / SSD Company LTD",                       "Evio (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND /*|MACHINE_IS_BIOS_ROOT*/ ) // inputs? it's a violin controller


// Let’s!TVプレイ 超にんきスポット!ころがしほーだい たまごっちりぞーと   (Let's! TV Play Chou Ninki Spot! Korogashi-Houdai Tamagotchi Resort) (only on the Japanese list? http://test.shinsedai.co.jp/english/products/Applied/list.html )   This also allows you to use an IR reciever to import a Tamagotchi from compatible games
CONS( 2006, ltv_tam,  0,           0,  xavix_i2c_24lc04_tam,  ltv_tam,xavix_i2c_ltv_tam_state,      init_xavix,    "Bandai / SSD Company LTD",                      "Let's! TV Play Chou Ninki Spot! Korogashi-Houdai Tamagotchi Resort (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )


/* SuperXaviX(?) (XaviX 2000 type CPU) hardware titles (2nd XaviX generation?)

   these use the SSD 2000 NEC 85605-621 type CPU

   XavixPort Golf is "SSD 2003 SuperXaviX MXIC 2003 3009" (not dumped yet, but actually marked as SuperXaviX unlike the others!)

   This CPU type adds extra opcodes that don't appear to be present in the 97/98 types
   It does not appear to support the bitmap modes or 16-bit ROMs found in the 2002 type
*/

ROM_START( epo_sdb )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("superdashball.bin", 0x000000, 0x400000, CRC(a004a764) SHA1(47a96822d4d7d6a0f6be5cd729c3747dbab65979) )
ROM_END

ROM_START( epo_ebox )
	ROM_REGION(0x400000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("exciteboxing.bin", 0x000000, 0x400000, CRC(e25ae4f5) SHA1(7f7b613f0ab8f43f5cad0d13de538921e77cae9c) )
ROM_END


ROM_START( ttv_sw )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "jedi.bin", 0x000000, 0x800000, CRC(51cae5fd) SHA1(1ed8d556f31b4182259ca8c766d60c824d8d9744) )
ROM_END

ROM_START( ttv_lotr )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "lotr.bin", 0x000000, 0x800000, CRC(a034ecd5) SHA1(264a9d4327af0a075841ad6129db67d82cf741f1) )
ROM_END

ROM_START( ttv_mx )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "mxdirtrebel.bin", 0x000000, 0x800000, CRC(e64bf1a1) SHA1(137f97d7d857697a13e0c8984509994dc7bc5fc5) )
ROM_END

ROM_START( drgqst )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "dragonquest.bin", 0x000000, 0x800000, CRC(3d24413f) SHA1(1677e81cedcf349de7bf091a232dc82c6424efba) )
ROM_END

ROM_START( ban_onep )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00)
	ROM_LOAD("onepiece.bin", 0x000000, 0x800000, CRC(c5cb5a5f) SHA1(db85f6cc48d77c5a4967b9b8e2999167e3dfc8c8) )
ROM_END

CONS( 2002, epo_ebox, 0, 0, xavix2000_nv,        epo_epp,     xavix_state,             init_xavix, "Epoch / SSD Company LTD",       "Excite Boxing (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND ) // doesn't use XaviX2000 extra opcodes, but had that type of CPU
CONS( 2004, epo_sdb,  0, 0, xavix2000_nv_sdb,    epo_sdb,     xavix_2000_nv_sdb_state, init_xavix, "Epoch / SSD Company LTD",       "Super Dash Ball (Japan)",  MACHINE_IMPERFECT_SOUND )

CONS( 2005, ttv_sw,   0, 0, xavix2000_i2c_24c02, ttv_lotr,    xavix_i2c_lotr_state,    init_xavix, "Tiger / SSD Company LTD",       "Star Wars Saga Edition - Lightsaber Battle Game", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
CONS( 2005, ttv_lotr, 0, 0, xavix2000_i2c_24c02, ttv_lotr,    xavix_i2c_lotr_state,    init_xavix, "Tiger / SSD Company LTD",       "Lord Of The Rings - Warrior of Middle-Earth", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )
CONS( 2005, ttv_mx,   0, 0, xavix2000_i2c_24c04, ttv_mx,      xavix_i2c_state,         init_xavix, "Tiger / SSD Company LTD",       "MX Dirt Rebel", MACHINE_IMPERFECT_SOUND )
CONS( 2003, drgqst,   0, 0, xavix2000_i2c_24c02, ttv_lotr,    xavix_i2c_lotr_state,    init_xavix, "Square Enix / SSD Company LTD", "Kenshin Dragon Quest: Yomigaerishi Densetsu no Ken", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

// hangs after starting a game, or after quite a long time in attract mode (first problem could be bad save data read with the eeprom code, 2nd problem might just be how it is, ends up in a dead loop, not executing invalid code, idle timeout / battery saver)
CONS( 2004, ban_onep, 0, 0, xavix2000_i2c_24c04, ttv_lotr,    xavix_i2c_lotr_state, init_xavix, "Bandai / SSD Company LTD",         "One Piece Punch Battle (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND )

/* SuperXaviX (XaviX 2002 type CPU) hardware titles (3rd XaviX generation?)

  these use the SSD 2002 NEC 85054-611 type CPU
  differences include support for 16-bit ROMs, various bitmap modes, interlace screen modes etc.
  possibly higher horizontal resolution for bitmap layers with others scaled to fit?

*/

/* The 'XaviXPORT' isn't a real console, more of a TV adapter, all the actual hardware (CPU including video hw, sound hw) is in the cartridges and controllers
   and can vary between games, see notes at top of driver.

   The 'Domyos Interactive System (DiS)' released in France by Decathlon appears to be identical to XaviXPORT (but for PAL regions, and with an entirely different software range)
*/

ROM_START( tmy_thom )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "thomastank.bin", 0x000000, 0x800000, CRC(a52a23be) SHA1(e5b3500239d9e56eb5405f7585982959e5a162da) )
ROM_END

// XaviXPORT
ROM_START( xavtenni )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xavixtennis.bin", 0x000000, 0x800000, CRC(23a1d918) SHA1(2241c59e8ea8328013e55952ebf9060ea0a4675b) )
ROM_END

ROM_START( xavbaseb )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpbaseball.bin", 0x000000, 0x800000, CRC(e9ed692d) SHA1(537e390e972156dc7da66ee127ae4c8052038ee5) )
ROM_END

ROM_START( xavbowl )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpbowling.bin", 0x000000, 0x800000, CRC(2873460b) SHA1(ea8e2392f5a12961a23eb66dca8e07dec81ce8c8) )
ROM_END

ROM_START( xavbassf )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpbassfishing.bin", 0x000000, 0x800000, CRC(09ab2f29) SHA1(616254176315d0947002e9ae5a6371a3ffa2e8eb) )

	// code for the nRF24E1s, stored in SEEPROMs.  One in the cartridge, one in the rod/reel
	ROM_REGION( 0x1001, "reel_io", ROMREGION_ERASE00 )
	ROM_LOAD( "xpbassfishingnrf24e1reel.bin", 0x0000, 0x1001, CRC(cfbb19ae) SHA1(32464e4e4be33fdbc7768311f93ce437a316c616) )

	ROM_REGION( 0x800000, "base_io", ROMREGION_ERASE00 )
	ROM_LOAD( "xpbassfishingnrf24e1cart.bin", 0x0000, 0x1001, CRC(62f6303e) SHA1(126b2663e252fb80948f53153e4046e63dd8be32) )
ROM_END

ROM_START( xavbox )
	ROM_REGION( 0x800000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpboxing.bin", 0x000000, 0x800000, CRC(b61e7717) SHA1(162b9c53ac8c9d7b6972db44f7bc1cb0a7837b70) )
ROM_END


ROM_START( xavjmat )
	ROM_REGION( 0x1000000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpjmat.bin", 0x000000, 0x1000000, CRC(71a51eef) SHA1(41fd2c3013d1c86756046ec9174e94400f8fa06d) )
ROM_END

// currently copies the wrong code into RAM to execute (due to extended ROM size, and possible banking)
// [:] ':maincpu' (00E074): rom_dmatrg_w (do DMA?) 01
// [:]   (possible DMA op SRC 00ebe2d3 DST 358a LEN 0398)
//         needs to come from 006be2d3 (so still from lower 8MB, not upper 8MB)

ROM_START( xavmusic )
	ROM_REGION( 0x1000000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpmusicandcircuit.bin", 0x000000, 0x1000000, CRC(e06129d2) SHA1(d074d0dd85ce870f435da3c066a7f52b50999665) )
ROM_END


// Domyos DiS (XaviX 2002 based titles)
ROM_START( domfitex )
	ROM_REGION( 0x1000000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpfitnessexercise.bin", 0x000000, 0x1000000, CRC(f1089229) SHA1(803df8ba0a05cb004a4238c6c71ea1ffa4428990) )
ROM_END

ROM_START( domfitch )
	ROM_REGION( 0x1000000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpfitnesschallenge.bin", 0x000000, 0x1000000, CRC(e0a4093d) SHA1(2692ac03f8be4f86a4777ad0c365cbab7b469e3b) )
ROM_END

ROM_START( domdance )
	ROM_REGION( 0x1000000, "bios", ROMREGION_ERASE00 )
	ROM_LOAD( "xpfitnessdance.bin", 0x000000, 0x1000000, CRC(3170dd41) SHA1(9c9b4f1d8e7c5097271bb8712463ad93c8d55d97) )
ROM_END

// TODO: does it have an SEEPROM? why does it hang? full title?
CONS( 2005, tmy_thom, 0, 0, xavix2002_i2c_24c04, xavix_i2c,  xavix_i2c_state, init_xavix, "Tomy / SSD Company LTD",  "Thomas and Friends (Tomy)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )

CONS( 2004, xavtenni, 0, 0, xavix2002_i2c_24c04, xavix_i2c,  xavix_i2c_state,      init_xavix, "SSD Company LTD",         "XaviX Tennis (XaviXPORT)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )
CONS( 2004, xavbaseb, 0, 0, xavix2002_i2c_24c04, xavix_i2c,  xavix_i2c_state,      init_xavix, "SSD Company LTD",         "XaviX Baseball (XaviXPORT)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )
CONS( 2004, xavbowl,  0, 0, xavix2002_i2c_24c04, xavix_bowl, xavix_i2c_bowl_state, init_xavix, "SSD Company LTD",         "XaviX Bowling (XaviXPORT)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // has IR 'Camera'
CONS( 2004, xavbox,   0, 0, xavix2002_i2c_jmat, xavix,  xavix_i2c_jmat_state,      init_xavix, "SSD Company LTD",         "XaviX Boxing (XaviXPORT)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // has IR 'Camera'
// Bass Fishing PCB is just like Tennis except with an RF daughterboard.
CONS( 2004, xavbassf, 0, 0, xavix2002_i2c_24c04, xavix_i2c,  xavix_i2c_state,      init_xavix, "SSD Company LTD",         "XaviX Bass Fishing (XaviXPORT)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )

// TODO: check SEEPROM type and hookup, banking!
CONS( 2005, xavjmat,  0, 0, xavix2002_i2c_jmat,  xavix,      xavix_i2c_jmat_state, init_xavix, "SSD Company LTD",         "Jackie Chan J-Mat Fitness (XaviXPORT)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )
CONS( 2007, xavmusic, 0, 0, xavix2002_i2c_jmat,  xavix,      xavix_i2c_jmat_state, init_xavix, "SSD Company LTD",         "XaviX Music & Circuit (XaviXPORT)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )

// https://arnaudmeyer.wordpress.com/domyos-interactive-system/
// Domyos Fitness Adventure
// Domyos Fitness Challenge
// Domyos Fitness Exercises
// Domyos Fit Race
// Domyos Soft Fitness
// Domyos Fitness Dance
// Domyos Fitness Play
// Domyos Fitness Training

// Domyos Bike Concept (not listed on site above)

// Has SEEPROM and an RTC.  Exercise has some leftover PC buffer stuff.  (TODO, check SEEPROM type, RTC type, banking) (both Exercises and Challenge are identical PCBs)
CONS( 2008, domfitex, 0, 0, xavix2002_i2c_jmat, xavixp, xavix_i2c_jmat_state, init_xavix, "Decathlon / SSD Company LTD", "Domyos Fitness Exercises (Domyos Interactive System)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )
CONS( 2008, domfitch, 0, 0, xavix2002_i2c_jmat, xavixp, xavix_i2c_jmat_state, init_xavix, "Decathlon / SSD Company LTD", "Domyos Fitness Challenge (Domyos Interactive System)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )
CONS( 2007, domdance, 0, 0, xavix2002_i2c_jmat, xavixp, xavix_i2c_jmat_state, init_xavix, "Decathlon / SSD Company LTD", "Domyos Fitness Dance (Domyos Interactive System)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND )

// some DIS games run on XaviX 2 instead, see xavix2.cpp for Domyos Fitness Adventure and Domyos Bike Concept
