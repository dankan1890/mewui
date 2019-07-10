// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
/***************************************************************************

    drivers/amstr_pc.c

    Driver file for Amstrad PC1512 and related machines.

    PC-XT memory map

    00000-9FFFF   RAM
    A0000-AFFFF   NOP       or videoram EGA/VGA
    B0000-B7FFF   videoram  MDA, page #0
    B8000-BFFFF   videoram  CGA and/or MDA page #1, T1T mapped RAM
    C0000-C7FFF   NOP       or ROM EGA/VGA
    C8000-C9FFF   ROM       XT HDC #1
    CA000-CBFFF   ROM       XT HDC #2
    D0000-EFFFF   NOP       or 'adapter RAM'
    F0000-FDFFF   NOP       or ROM Basic + other Extensions
    FE000-FFFFF   ROM

Amstrad PC1640
==============

More information can be found at http://www.seasip.info/AmstradXT/1640tech/index.html

***************************************************************************/

#include "emu.h"
#include "cpu/nec/nec.h"
#include "cpu/i86/i86.h"

#include "machine/mc146818.h"
#include "machine/genpc.h"
#include "bus/isa/isa.h"
#include "bus/isa/isa_cards.h"
#include "bus/pc_joy/pc_joy.h"
#include "bus/rs232/rs232.h"

#include "machine/pckeybrd.h"
#include "machine/pc_lpt.h"
#include "machine/ram.h"

class amstrad_pc_state : public driver_device
{
public:
	amstrad_pc_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_ram(*this, RAM_TAG),
		m_mb(*this, "mb"),
		m_keyboard(*this, "pc_keyboard"),
		m_lpt1(*this, "lpt_1"),
		m_lpt2(*this, "lpt_2"),
		m_mouse{ 0, 0 }
	{
	}

	void pc200(machine_config &config);
	void pc2086(machine_config &config);
	void ppc640(machine_config &config);
	void ppc512(machine_config &config);

private:
	required_device<cpu_device> m_maincpu;
	required_device<ram_device> m_ram;
	required_device<pc_noppi_mb_device> m_mb;
	required_device<pc_keyboard_device> m_keyboard;
	required_device<pc_lpt_device> m_lpt1;
	required_device<pc_lpt_device> m_lpt2;

	DECLARE_READ8_MEMBER( pc1640_port60_r );
	DECLARE_WRITE8_MEMBER( pc1640_port60_w );

	DECLARE_READ8_MEMBER( pc1640_mouse_x_r );
	DECLARE_READ8_MEMBER( pc1640_mouse_y_r );
	DECLARE_WRITE8_MEMBER( pc1640_mouse_x_w );
	DECLARE_WRITE8_MEMBER( pc1640_mouse_y_w );

	DECLARE_READ8_MEMBER( pc200_port378_r );
	DECLARE_READ8_MEMBER( pc200_port278_r );
	DECLARE_READ8_MEMBER( pc1640_port378_r );
	DECLARE_READ8_MEMBER( pc1640_port3d0_r );
	DECLARE_READ8_MEMBER( pc1640_port4278_r );
	DECLARE_READ8_MEMBER( pc1640_port278_r );

	struct {
		uint8_t x,y; //byte clipping needed
	} m_mouse;

	// 64 system status register?
	uint8_t m_port60;
	uint8_t m_port61;
	uint8_t m_port62;
	uint8_t m_port65;

	int m_dipstate;
	static void cfg_com(device_t *device);
	static void cfg_fdc(device_t *device);

	void pc200_io(address_map &map);
	void pc2086_map(address_map &map);
	void ppc512_io(address_map &map);
	void ppc640_map(address_map &map);
};

void amstrad_pc_state::ppc640_map(address_map &map)
{
	map(0xf0000, 0xfffff).rom().region("bios", 0);
}

void amstrad_pc_state::pc2086_map(address_map &map)
{
	map(0xc0000, 0xc9fff).rom().region("bios", 0);
	map(0xf0000, 0xfffff).rom().region("bios", 0x10000);
}

void amstrad_pc_state::pc200_io(address_map &map)
{
	map(0x0000, 0x00ff).m(m_mb, FUNC(pc_noppi_mb_device::map));
	map(0x0060, 0x0065).rw(FUNC(amstrad_pc_state::pc1640_port60_r), FUNC(amstrad_pc_state::pc1640_port60_w));
	map(0x0078, 0x0079).rw(FUNC(amstrad_pc_state::pc1640_mouse_x_r), FUNC(amstrad_pc_state::pc1640_mouse_x_w));
	map(0x007a, 0x007b).rw(FUNC(amstrad_pc_state::pc1640_mouse_y_r), FUNC(amstrad_pc_state::pc1640_mouse_y_w));
	map(0x0200, 0x0207).rw("pc_joy", FUNC(pc_joy_device::joy_port_r), FUNC(pc_joy_device::joy_port_w));
	map(0x0278, 0x027b).r(FUNC(amstrad_pc_state::pc200_port278_r));
	map(0x0278, 0x027b).w(m_lpt2, FUNC(pc_lpt_device::write)).umask16(0x00ff);
	map(0x0378, 0x037b).r(FUNC(amstrad_pc_state::pc200_port378_r));
	map(0x0378, 0x037b).w(m_lpt1, FUNC(pc_lpt_device::write)).umask16(0x00ff);
	map(0x03bc, 0x03bf).rw("lpt_0", FUNC(pc_lpt_device::read), FUNC(pc_lpt_device::write)).umask16(0x00ff);
}

void amstrad_pc_state::ppc512_io(address_map &map)
{
	pc200_io(map);
	map(0x0070, 0x0071).rw("rtc", FUNC(mc146818_device::read), FUNC(mc146818_device::write));
}

/* pc20 (v2)
   fc078
   fc102 color/mono selection
   fc166
   fc1b4
   fd841 (output something)
   ff17c (output something, read monitor type inputs)
   fc212
   fc26c
   fc2df
   fc3fe
   fc0f4
   fc432
   fc49f
   fc514
   fc566
   fc5db
   fc622 in 3de

port 0379 read
port 03de write/read
 */

/* pc1512 (v1)
   fc1b5
   fc1f1
   fc264
   fc310
   fc319
   fc385
   fc436
   fc459
   fc4cb
   fc557
   fc591
   fc624
   fc768
   fc818
   fc87d display amstrad ..
    fca17 keyboard check
     fca69
     fd680
      fd7f9
     fca7b !keyboard interrupt routine for this check
 */


/* pc1640 (v3)
   bios power up self test
   important addresses

   fc0c9
   fc0f2
   fc12e
   fc193
   fc20e
   fc2c1
   fc2d4


   fc375
   fc3ba
   fc3e1
   fc412
   fc43c
   fc47d
   fc48f
   fc51f
   fc5a2
   fc5dd mouse

   fc1c0
   fc5fa
    the following when language selection not 0 (test for presence of 0x80000..0x9ffff ram)
    fc60e
    fc667
   fc678
   fc6e5
   fc72e
   fc78f
   fc7cb ; coprocessor related
   fc834
    feda6 no problem with disk inserted

   fca2a

   cmos ram 28 0 amstrad pc1512 integrated cga
   cmos ram 23 dipswitches?
*/

/* test sequence in bios
 write 00 to 65
 write 30 to 61
 read 62 and (0x10)
 write 34 to 61
 read 62 and (0x0f)
 return or of the 2 62 reads

 allows set of the original ibm pc "dipswitches"!!!!

 66 write gives reset?
*/

/* mouse x counter at 0x78 (read- writable)
   mouse y counter at 0x7a (read- writable)

   mouse button 1,2 keys
   joystick (4 directions, 2 buttons) keys
   these get value from cmos ram
   74 15 00 enter
   70 17 00 forward del
   77 1b 00 joystick button 1
   78 19 00 joystick button 2


   79 00 4d right
   7a 00 4b left
   7b 00 50 down
   7c 00 48 up

   7e 00 01 mouse button left
   7d 01 01 mouse button right
*/

WRITE8_MEMBER( amstrad_pc_state::pc1640_port60_w )
{
	switch (offset) {
	case 1:
		m_port61=data;
		if (data==0x30) m_port62=(m_port65&0x10)>>4;
		else if (data==0x34) m_port62=m_port65&0xf;
		m_mb->m_pit8253->write_gate2(BIT(data, 0));
		m_mb->pc_speaker_set_spkrdata( data & 0x02 );
		m_keyboard->enable(data&0x40);
		if(data & 0x80)
			m_mb->m_pic8259->ir1_w(0);

		break;
	case 4:
		if (data&0x80) {
			m_port60=data^0x8d;
		} else {
			m_port60=data;
		}
		break;
	case 5:
		// stores the configuration data for port 62 configuration dipswitch emulation
		m_port65=data;
		break;
	}

	logerror("pc1640 write %.2x %.2x\n",offset,data);
}


READ8_MEMBER( amstrad_pc_state::pc1640_port60_r )
{
	int data=0;
	switch (offset) {
	case 0:
		if (m_port61&0x80)
			data=m_port60;
		else
			data = m_keyboard->read(space, 0);
		break;

	case 1:
		data = m_port61;
		break;

	case 2:
		data = m_port62;
		if (m_mb->pit_out2())
			data |= 0x20;
		break;
	}
	return data;
}

READ8_MEMBER( amstrad_pc_state::pc200_port378_r )
{
	uint8_t data = m_lpt1->read(space, offset);

	if (offset == 1)
		data = (data & ~7) | (ioport("DSW0")->read() & 7);
	if (offset == 2)
		data = (data & ~0xe0) | (ioport("DSW0")->read() & 0xc0);

	return data;
}

READ8_MEMBER( amstrad_pc_state::pc200_port278_r )
{
	uint8_t data = m_lpt2->read(space, offset);

	if (offset == 1)
		data = (data & ~7) | (ioport("DSW0")->read() & 7);
	if (offset == 2)
		data = (data & ~0xe0) | (ioport("DSW0")->read() & 0xc0);

	return data;
}


READ8_MEMBER( amstrad_pc_state::pc1640_port378_r )
{
	uint8_t data = m_lpt1->read(space, offset);

	if (offset == 1)
		data=(data & ~7) | (ioport("DSW0")->read() & 7);
	if (offset == 2)
	{
		switch (m_dipstate)
		{
		case 0:
			data = (data&~0xe0) | (ioport("DSW0")->read() & 0xe0);
			break;
		case 1:
			data = (data&~0xe0) | ((ioport("DSW0")->read() & 0xe000)>>8);
			break;
		case 2:
			data = (data&~0xe0) | ((ioport("DSW0")->read() & 0xe00)>>4);
			break;

		}
	}
	return data;
}

READ8_MEMBER( amstrad_pc_state::pc1640_port3d0_r )
{
	if (offset==0xa) m_dipstate=0;
	return space.read_byte(0x3d0+offset);
}

READ8_MEMBER( amstrad_pc_state::pc1640_port4278_r )
{
	if (offset==2) m_dipstate=1;
	// read parallelport
	return 0;
}

READ8_MEMBER( amstrad_pc_state::pc1640_port278_r )
{
	if ((offset==2)||(offset==0)) m_dipstate=2;
	// read parallelport
	return 0;
}

READ8_MEMBER( amstrad_pc_state::pc1640_mouse_x_r )
{
	return m_mouse.x - ioport("pc_mouse_x")->read();
}

READ8_MEMBER( amstrad_pc_state::pc1640_mouse_y_r )
{
	return m_mouse.y - ioport("pc_mouse_y")->read();
}

WRITE8_MEMBER( amstrad_pc_state::pc1640_mouse_x_w )
{
	m_mouse.x = data + ioport("pc_mouse_x")->read();
}

WRITE8_MEMBER( amstrad_pc_state::pc1640_mouse_y_w )
{
	m_mouse.y = data + ioport("pc_mouse_y")->read();
}

static INPUT_PORTS_START( pc200 )
	PORT_START("DSW0") /* IN1 */
	PORT_DIPNAME( 0x07, 0x07, "Name/Language")
	PORT_DIPSETTING(    0x00, "English/less checks" )
	PORT_DIPSETTING(    0x01, DEF_STR( Italian ) ) //prego attendere
	PORT_DIPSETTING(    0x02, "V.g. v\xC3\xA4nta" )
	PORT_DIPSETTING(    0x03, "Vent et cjeblik" ) // seldom c
	PORT_DIPSETTING(    0x04, DEF_STR( Spanish ) ) //Por favor
	PORT_DIPSETTING(    0x05, DEF_STR( French ) ) //patientez
	PORT_DIPSETTING(    0x06, DEF_STR( German ) ) // bitte warten
	PORT_DIPSETTING(    0x07, DEF_STR( English ) ) // please wait
	PORT_DIPNAME( 0x08, 0x00, "37a 0x40")
	PORT_DIPSETTING(    0x00, "0x00" )
	PORT_DIPSETTING(    0x08, "0x08" )
/* 2008-05 FP: This Dip Switch overlaps the next one.
Since pc200 is anyway NOT_WORKING, I comment out this one */
/*  PORT_DIPNAME( 0x10, 0x00, "37a 0x80")
    PORT_DIPSETTING(    0x00, "0x00" )
    PORT_DIPSETTING(    0x10, "0x10" ) */
	PORT_DIPNAME( 0x30, 0x00, "Integrated Graphics Adapter")
	PORT_DIPSETTING(    0x00, "CGA 1" )
	PORT_DIPSETTING(    0x10, "CGA 2" )
	PORT_DIPSETTING(    0x20, "external" )
	PORT_DIPSETTING(    0x30, "MDA" )
	PORT_DIPNAME( 0xc0, 0x80, "Startup Mode")
	PORT_DIPSETTING(    0x00, "external Color 80 Columns" )
	PORT_DIPSETTING(    0x40, "Color 40 Columns" )
	PORT_DIPSETTING(    0x80, "Color 80 Columns" )
	PORT_DIPSETTING(    0xc0, DEF_STR( Mono ) )

	PORT_START("DSW1") /* IN2 */
	PORT_BIT ( 0x80, 0x80,   IPT_UNUSED ) // com 1 on motherboard
	PORT_DIPNAME( 0x40, 0x40, "COM2: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x20, 0x00, "COM3: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x10, 0x00, "COM4: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x08, "LPT1: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Yes ) )
	PORT_BIT ( 0x04, 0x04,   IPT_UNUSED ) // lpt 1 on motherboard
	PORT_DIPNAME( 0x02, 0x00, "LPT3: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x01, 0x00, "Game port enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Yes ) )

	PORT_START("DSW2") /* IN3 */
	PORT_DIPNAME( 0x08, 0x08, "HDC1 (C800:0 port 320-323)")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x04, 0x04, "HDC2 (CA00:0 port 324-327)")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_BIT( 0x02, 0x02,   IPT_UNUSED ) /* no turbo switch */
	PORT_BIT( 0x01, 0x01,   IPT_UNUSED )

	PORT_INCLUDE( at_keyboard )     /* IN4 - IN11 */
INPUT_PORTS_END

// static const gfx_layout pc200_charlayout =
// {
	// 8, 16,                  /* 8 x 16 characters */
	// 2048,                   /* 2048 characters */
	// 1,                  /* 1 bits per pixel */
	// { 0 },                  /* no bitplanes */
	// /* x offsets */
	// { 0, 1, 2, 3, 4, 5, 6, 7 },
	// /* y offsets */
	// { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	// 8*16                    /* every char takes 16 bytes */
// };

// static GFXDECODE_START( pc200 )
	// GFXDECODE_ENTRY( "gfx1", 0x0000, pc200_charlayout, 3, 1 )
// GFXDECODE_END

void amstrad_pc_state::cfg_com(device_t *device)
{
	/* has its own mouse */
	device->subdevice<rs232_port_device>("serport0")->set_default_option(nullptr);
}

void amstrad_pc_state::cfg_fdc(device_t *device)
{
	/* all machines have an internal 3.5" drive */
	auto fdc0 = dynamic_cast<device_slot_interface *>(device->subdevice("fdc:0"));
	fdc0->set_default_option("35dd");
	fdc0->set_fixed(true);

	auto fdc1 = dynamic_cast<device_slot_interface *>(device->subdevice("fdc:1"));
	fdc1->set_default_option("35dd");
}

void amstrad_pc_state::pc200(machine_config &config)
{
	/* basic machine hardware */
	I8086(config, m_maincpu, 8000000);
	m_maincpu->set_addrmap(AS_PROGRAM, &amstrad_pc_state::ppc640_map);
	m_maincpu->set_addrmap(AS_IO, &amstrad_pc_state::pc200_io);
	m_maincpu->set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	PCNOPPI_MOTHERBOARD(config, "mb", 0).set_cputag(m_maincpu);

	// FIXME: determine ISA bus clock
	ISA8_SLOT(config, "aga", 0, "mb:isa", pc_isa8_cards, "aga_pc200", true);
	ISA8_SLOT(config, "fdc", 0, "mb:isa", pc_isa8_cards, "fdc_xt", true).set_option_machine_config("fdc_xt", cfg_fdc);
	ISA8_SLOT(config, "com", 0, "mb:isa", pc_isa8_cards, "com", true).set_option_machine_config("com", cfg_com);

	ISA8_SLOT(config, "isa1", 0, "mb:isa", pc_isa8_cards, nullptr, false);
	ISA8_SLOT(config, "isa2", 0, "mb:isa", pc_isa8_cards, nullptr, false);

	/* printer */
	pc_lpt_device &lpt0(PC_LPT(config, "lpt_0"));
	lpt0.irq_handler().set("mb:pic8259", FUNC(pic8259_device::ir7_w));

	PC_LPT(config, m_lpt1);
	m_lpt1->irq_handler().set("mb:pic8259", FUNC(pic8259_device::ir7_w));

	PC_LPT(config, m_lpt2);
	m_lpt2->irq_handler().set("mb:pic8259", FUNC(pic8259_device::ir5_w));

	PC_JOY(config, "pc_joy");

	PC_KEYB(config, m_keyboard);
	m_keyboard->keypress().set("mb:pic8259", FUNC(pic8259_device::ir1_w));

	/* internal ram */
	RAM(config, m_ram).set_default_size("640K").set_extra_options("512K");
}

void amstrad_pc_state::pc2086(machine_config &config)
{
	pc200(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &amstrad_pc_state::pc2086_map);
}

void amstrad_pc_state::ppc640(machine_config &config)
{
	pc200(config);
	V30(config.replace(), m_maincpu, 8000000);
	m_maincpu->set_addrmap(AS_PROGRAM, &amstrad_pc_state::ppc640_map);
	m_maincpu->set_addrmap(AS_IO, &amstrad_pc_state::ppc512_io);
	m_maincpu->set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	config.device_remove("isa1");
	config.device_remove("isa2");

	MC146818(config, "rtc", 32.768_kHz_XTAL);
}

void amstrad_pc_state::ppc512(machine_config &config)
{
	ppc640(config);
	m_ram->set_default_size("512K").set_extra_options("640K");
}

/*
Sinclair PC200 ROMs (from a v1.2 PC200):

40109.ic159     -- Character set, the same as in the 1.5 PC200. Label:

            AMSTRAD
            40109
            8827 B

40184.ic132 -- BIOS v1.2.
40185.ic129    Labels are:

            AMSTRAD         AMSTRAD
            40184           40185
            V1.2:5EA8       V1.2:A058
*/
ROM_START( pc200 )
//  ROM_REGION(0x100000,"bios", 0)
	ROM_REGION16_LE(0x10000,"bios", 0)
	// special bios at 0xe0000 !?
	ROM_SYSTEM_BIOS(0, "v15", "v1.5")
	ROMX_LOAD("40185-2.ic129", 0xc001, 0x2000, CRC(41302eb8) SHA1(8b4b2afea543b96b45d6a30365281decc15f2932), ROM_SKIP(1) | ROM_BIOS(0)) // v2
	ROMX_LOAD("40184-2.ic132", 0xc000, 0x2000, CRC(71b84616) SHA1(4135102a491b25fc659d70b957e07649f3eacf24), ROM_SKIP(1) | ROM_BIOS(0)) // v2
	ROM_SYSTEM_BIOS(1, "v13", "v1.3")
	ROMX_LOAD("40185v13.ic129", 0xc001, 0x2000, CRC(f082f08e) SHA1(b332db419033588a7380bfecdf46104974347341), ROM_SKIP(1) | ROM_BIOS(1))
	ROMX_LOAD("40184v13.ic132", 0xc000, 0x2000, CRC(5daf6068) SHA1(93a2ccfb0e29c8f2c98f06c64bb0ea0b3acafb13), ROM_SKIP(1) | ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "v12", "v1.2")
	ROMX_LOAD("40185.ic129", 0xc001, 0x2000, CRC(c2b4eeac) SHA1(f11015fadf0c16d86ce2c5047be3e6a4782044f7), ROM_SKIP(1) | ROM_BIOS(2))
	ROMX_LOAD("40184.ic132", 0xc000, 0x2000, CRC(b22704a6) SHA1(dadd573db6cd34f339f2f0ae55b07537924c024a), ROM_SKIP(1) | ROM_BIOS(2))
	// also mapped to f0000, f4000, f8000
	ROM_REGION( 0x800, "keyboard", 0 )
	ROM_LOAD( "40112.ic801", 0x000, 0x800, CRC(842a954c) SHA1(93ca6badf20e0215025fe109959eddead8c52f38) )
ROM_END


ROM_START( pc20 )
//  ROM_REGION(0x100000,"bios", 0)
	ROM_REGION16_LE(0x10000,"bios", 0)

	// special bios at 0xe0000 !?
	// This is probably referring to a check for the Amstrad RP5-2 diagnostic
	// card, which can be plugged into an Amstrad XT for troubleshooting purposes.
	// - John Elliott
	ROM_LOAD16_BYTE("pc20v2.0", 0xc001, 0x2000, CRC(41302eb8) SHA1(8b4b2afea543b96b45d6a30365281decc15f2932)) // v2
	ROM_LOAD16_BYTE("pc20v2.1", 0xc000, 0x2000, CRC(71b84616) SHA1(4135102a491b25fc659d70b957e07649f3eacf24)) // v2
	// also mapped to f0000, f4000, f8000
ROM_END


ROM_START( ppc512 )
//  ROM_REGION(0x100000,"bios", 0)
	ROM_REGION16_LE(0x10000,"bios", 0)
	// special bios at 0xe0000 !?
	ROM_LOAD16_BYTE("40107.v1", 0xc000, 0x2000, CRC(4e37e769) SHA1(88be3d3375ec3b0a7041dbcea225b197e50d4bfe)) // v1.9
	ROM_LOAD16_BYTE("40108.v1", 0xc001, 0x2000, CRC(4f0302d9) SHA1(e4d69ca98c3b98f3705a2902b16746360043f039)) // v1.9
	// also mapped to f0000, f4000, f8000
	ROM_REGION( 0x800, "keyboard", 0 ) // PPC512 / PPC640 / PC200 102-key keyboard
	ROM_LOAD( "40112.ic801", 0x000, 0x800, CRC(842a954c) SHA1(93ca6badf20e0215025fe109959eddead8c52f38) )
ROM_END


ROM_START( ppc640 )
//  ROM_REGION(0x100000,"bios", 0)
	ROM_REGION16_LE(0x10000,"bios", 0)
	// special bios at 0xe0000 !?
	ROM_LOAD16_BYTE("40107.v2", 0xc000, 0x2000, CRC(0785b63e) SHA1(4dbde6b9e9500298bb6241a8daefd85927f1ad28)) // v2.1
	ROM_LOAD16_BYTE("40108.v2", 0xc001, 0x2000, CRC(5351cf8c) SHA1(b4dbf11b39378ab4afd2107d3fe54a99fffdedeb)) // v2.1
	// also mapped to f0000, f4000, f8000
	ROM_REGION(0x2000,"subcpu", 0)
	ROM_LOAD("40135.ic192", 0x00000, 0x2000, CRC(75d99199) SHA1(a76d39fda3d5140e1fb9ce70fcddbdfb8f891dc6))

	ROM_REGION( 0x800, "keyboard", 0 ) // PPC512 / PPC640 / PC200 102-key keyboard
	ROM_LOAD( "40112.ic801", 0x000, 0x800, CRC(842a954c) SHA1(93ca6badf20e0215025fe109959eddead8c52f38) )
ROM_END


ROM_START( pc2086 )
	ROM_REGION16_LE( 0x20000, "bios", 0 )
	ROM_LOAD(        "40186.ic171", 0x00000, 0x8000, CRC(959f00ba) SHA1(5df1efe4203cd076292a7713bd7ebd1196dca577) )
	ROM_LOAD16_BYTE( "40179.ic129", 0x1c000, 0x2000, CRC(003605e4) SHA1(b882e97ee81b9ba0e7d969c63da3f2052f23b4b9) )
	ROM_LOAD16_BYTE( "40180.ic132", 0x1c001, 0x2000, CRC(28ee5e58) SHA1(93e045609466fcec74e2bb72578bb7405281cf7b) )

	ROM_REGION( 0x800, "keyboard", 0 ) // PC2086 / PC3086 102-key keyboard
	ROM_LOAD( "40178.ic801", 0x000, 0x800, CRC(f72f1c2e) SHA1(34897e78b3d10f96b36d81778e97c4a9a1b8618b) )
ROM_END


ROM_START( pc3086 )
	ROM_REGION16_LE( 0x20000, "bios", 0 )
	ROM_LOAD( "c000.bin", 0x00000, 0x8000, CRC(5a6c38e9) SHA1(382d2028e0dc5515a68843829563ce29018edb08) )
	ROM_LOAD( "c800.bin", 0x08000, 0x2000, CRC(3329c6d5) SHA1(982e852278185d69acde47a4f3942bc09ed76777) )
	ROM_LOAD( "fc00.bin", 0x1c000, 0x4000, CRC(b5630753) SHA1(98c344831cc4dc59ebb39bbb1961964a8d39fe20) )

	ROM_REGION( 0x800, "keyboard", 0 ) // PC2086 / PC3086 102-key keyboard
	ROM_LOAD( "40178.ic801", 0x000, 0x800, CRC(f72f1c2e) SHA1(34897e78b3d10f96b36d81778e97c4a9a1b8618b) )
ROM_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*    YEAR  NAME    PARENT   COMPAT  MACHINE  INPUT  CLASS             INIT        COMPANY         FULLNAME */
COMP( 1987, ppc512, ibm5150, 0,      ppc512,  pc200, amstrad_pc_state, empty_init, "Amstrad plc",  "Amstrad PPC512", MACHINE_NOT_WORKING)
COMP( 1987, ppc640, ibm5150, 0,      ppc640,  pc200, amstrad_pc_state, empty_init, "Amstrad plc",  "Amstrad PPC640", MACHINE_NOT_WORKING)
COMP( 1988, pc20,   ibm5150, 0,      pc200,   pc200, amstrad_pc_state, empty_init, "Amstrad plc",  "Amstrad PC20" , MACHINE_NOT_WORKING)
COMP( 1988, pc200,  ibm5150, 0,      pc200,   pc200, amstrad_pc_state, empty_init, "Sinclair Research Ltd",  "PC200 Professional Series", MACHINE_NOT_WORKING)
COMP( 1988, pc2086, ibm5150, 0,      pc2086,  pc200, amstrad_pc_state, empty_init, "Amstrad plc",  "Amstrad PC2086", MACHINE_NOT_WORKING )
COMP( 1990, pc3086, ibm5150, 0,      pc2086,  pc200, amstrad_pc_state, empty_init, "Amstrad plc",  "Amstrad PC3086", MACHINE_NOT_WORKING )
