// license:BSD-3-Clause
// copyright-holders:David Haywood
// thanks to: Ricky2001, ArcadeHacker, IFW

/*
 todo:
  - fix sound emulation (speed needs verifying + sample playback)
  - fix sprite communication / banking
    * bit "output bit 0x02 %d (IC21)" at 0x42 might be important
    * mag_exzi currently requires a gross hack to stop the sprite CPU crashing on startup
    * mag_xain sometimes leaves old sprites on the screen, probably due to a lost clear
      command
  - fix flipscreen
  - verify behavior of unknown / unused ports / interrupt sources etc.
  - verify the disk images, convert to a better format that can natively store protection
    * RAW data also available if required
    * as mentioned, the disks are copy protected, see notes below
  - Use proper floppy drive emulation code that originally came from MESS (tied with above)
  - verify all clocks and screen params (50hz seems to match original videos)
  - work out why we need a protection hack and replace it with proper emulation
    * there are no per-game protection devices, so it's something to do with the base hardware
    * there seem to be 2 checks, one based on a weird sector on the discs, the other based on
      a port read
  - add additional hardware notes from ArcadeHacker
  - layer enables on War Mission? (transitions from title screen etc.)

 notes:
  - high scores will be defaulted if the data in the table is corrupt, the games give no
    option to do this otherwise.  A backup copy of the score table is kept, so you also
    have to enter and exit service mode.

*/


/*

 Magnet System by

 EFO SA (Electrónica Funcional Operativa SA).
 based on Cedar hardware


 http://retrolaser.es/cedar-computer-el-ordenador-profesional-de-efo-sa/
 http://www.recreativas.org/magnet-system-2427-efosa

 A number of original games as well as conversions were advertised for this system, it is however
 believed that EFO went bankrupt before anything hit the market.  The only 3 dumped games are
 conversions and appear to be in incomplete states (it is rather easy to break Time Scanner for
 example, the ball simply gets stuck in some places)  These are not simply bootlegs, they're
 completely original pieces of code more akin to home computer ports.

 The following were advertised
  Original Games
  - A Day in Space **
  - Crazy Driver
  - Jungle Trophy
  - Quadrum
  - War Mission ** *
  - The Burning Cave
  - Scorpio
  - Paris Dakar **
  - Sailing Race
  - Formula

  Ports / Conversions
  - Exzisus *
  - Double Dragon
  - Flying Shark
  - Time Scanner *
  - Xain d'Sleena *
  - Boody Kids (Booby Kids?)

  ** screenshots present on flyer
  * dumps exist



Disk Protection

Sectors are 1024 (0x400) bytes long but marked on the disc as 512 bytes as a copy protection
Sector numbering for each track starts at 200 (0xC8) again, presumably as a protection.
Each track has 6 sectors (200 - 205)
The drive runs at 240 RPM allowing for ~25% extra data. (attempting to dump at other speeds won't work)

 data order / sector marking
 track 0, side 0, sector 200...205 (instead of sector 0...5)
 track 0, side 1, sector 200...205
 track 1, side 0, sector 200...205
 track 1, side 1, sector 200...205

Note, the games store settings / scores to the disk and don't provide any kind of 'factory reset'
option, so if used the data will not be pristine.

PCB Setup

The hardware consists of 5 main PCBs in a cage.
1x Audio PCB (on top)
1x Master PCB
2x Plane PCBs (both identical aside from jumper settings)
1x Sprite PCB

There are small memory sub-boards on the Master PCB and Sprite PCB; due to the awkwardness of
the banking at times (and the fact that even with 4 banks of 256 colours, only one can be active)
I suspect the additional memory was an afterthought.

(put more details hardware notes here)


*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/z80daisy.h"
#include "machine/z80pio.h"
#include "machine/bankdev.h"
#include "machine/z80ctc.h"
#include "sound/ay8910.h"

#include "audio/efo_zsu.h"
#include "machine/cedar_magnet_plane.h"
#include "machine/cedar_magnet_sprite.h"
#include "machine/cedar_magnet_flop.h"

#include "emupal.h"
#include "screen.h"


#define LOG_IC49_PIO_PB 0
#define LOG_IC48_PIO_PB 0
#define LOG_IC48_PIO_PA 0

class cedar_magnet_state : public driver_device
{
public:
	cedar_magnet_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_bank0(*this, "bank0")
		, m_sub_ram_bankdev(*this, "mb_sub_ram")
		, m_sub_pal_bankdev(*this, "mb_sub_pal")
		, m_ram0(*this, "ram0")
		, m_pal_r(*this, "pal_r")
		, m_pal_g(*this, "pal_g")
		, m_pal_b(*this, "pal_b")
		, m_ic48_pio(*this, "z80pio_ic48")
		, m_ic49_pio(*this, "z80pio_ic49")
		, m_io_coin(*this, "COIN%u", 1U)
		, m_ic48_pio_pa_val(0xff)
		, m_ic48_pio_pb_val(0xff)
		, m_ic49_pio_pb_val(0xff)
		, m_address1hack(-1)
		, m_address2hack(-1)
		, m_palette(*this, "palette")
		, m_maincpu(*this, "maincpu")
		, m_cedsound(*this, "cedtop")
		, m_cedplane0(*this, "cedplane0")
		, m_cedplane1(*this, "cedplane1")
		, m_cedsprite(*this, "cedsprite")
	{
	}

	void cedar_magnet(machine_config &config);

private:
	required_device<address_map_bank_device> m_bank0;
	required_device<address_map_bank_device> m_sub_ram_bankdev;
	required_device<address_map_bank_device> m_sub_pal_bankdev;

	required_shared_ptr<u8> m_ram0;
	required_shared_ptr<u8> m_pal_r;
	required_shared_ptr<u8> m_pal_g;
	required_shared_ptr<u8> m_pal_b;

	required_device<z80pio_device> m_ic48_pio;
	required_device<z80pio_device> m_ic49_pio;

	optional_ioport_array<2> m_io_coin;

	u8 ic48_pio_pa_r();
	void ic48_pio_pa_w(u8 data);

	u8 ic48_pio_pb_r();
	void ic48_pio_pb_w(u8 data);

	u8 ic49_pio_pb_r();
	void ic49_pio_pb_w(u8 data);

	// 1x range ports
	void port18_w(u8 data);
	void port19_w(u8 data);
	void port1b_w(u8 data);

	u8 port18_r();
	u8 port19_r();
	u8 port1a_r();

	// 7x range ports
	void rambank_palbank_w(u8 data);
	void palupload_w(u8 data);
	void paladdr_w(u8 data);
	u8 watchdog_r();
	u8 port7c_r();

	// other ports
	u8 other_cpu_r(offs_t offset);
	void other_cpu_w(offs_t offset, u8 data);

	u8 m_paladdr;
	int m_palbank;

	u8 m_ic48_pio_pa_val;
	u8 m_ic48_pio_pb_val;
	u8 m_ic49_pio_pb_val;

	void set_palette(int offset);
	void palette_r_w(offs_t offset, u8 data);
	void palette_g_w(offs_t offset, u8 data);
	void palette_b_w(offs_t offset, u8 data);

	void handle_sub_board_cpu_lines(cedar_magnet_board_interface &dev, int old_data, int data);
	INTERRUPT_GEN_MEMBER(irq);
	void kludge_protection();
	int m_address1hack;
	int m_address2hack;

	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	u32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	required_device<palette_device> m_palette;
	required_device<cpu_device> m_maincpu;

	required_device<cedar_magnet_sound_device> m_cedsound;
	required_device<cedar_magnet_plane_device> m_cedplane0;
	required_device<cedar_magnet_plane_device> m_cedplane1;
	required_device<cedar_magnet_sprite_device> m_cedsprite;

	void cedar_bank0(address_map &map);
	void cedar_magnet_io(address_map &map);
	void cedar_magnet_mainboard_sub_pal_map(address_map &map);
	void cedar_magnet_mainboard_sub_ram_map(address_map &map);
	void cedar_magnet_map(address_map &map);
};

/***********************

  Memory maps

***********************/

void cedar_magnet_state::cedar_magnet_mainboard_sub_pal_map(address_map &map)
{
// these are 3x MOTOROLA MM2114N SRAM 4096 bit RAM (twice the size because we map bytes, but only 4 bits are used)
// these are on the master board memory sub-board
	map(0x2400, 0x27ff).ram().w(FUNC(cedar_magnet_state::palette_r_w)).share("pal_r");
	map(0x2800, 0x2bff).ram().w(FUNC(cedar_magnet_state::palette_g_w)).share("pal_g");
	map(0x3000, 0x33ff).ram().w(FUNC(cedar_magnet_state::palette_b_w)).share("pal_b");
}

void cedar_magnet_state::cedar_magnet_mainboard_sub_ram_map(address_map &map)
{
// these are 8x SIEMENS HYB 41256-15 AA - 262,144 bit DRAM (32kbytes)
// these are on the master board memory sub-board
	map(0x00000, 0x3ffff).ram().share("ram0");
}

void cedar_magnet_state::cedar_magnet_map(address_map &map)
{
	map(0x0000, 0xffff).m(m_bank0, FUNC(address_map_bank_device::amap8));
}

void cedar_magnet_state::cedar_magnet_io(address_map &map)
{
	map.global_mask(0xff);

	map(0x18, 0x18).rw(FUNC(cedar_magnet_state::port18_r), FUNC(cedar_magnet_state::port18_w));
	map(0x19, 0x19).rw(FUNC(cedar_magnet_state::port19_r), FUNC(cedar_magnet_state::port19_w));
	map(0x1a, 0x1a).r(FUNC(cedar_magnet_state::port1a_r));
	map(0x1b, 0x1b).w(FUNC(cedar_magnet_state::port1b_w));

	map(0x20, 0x23).rw(m_ic48_pio, FUNC(z80pio_device::read_alt), FUNC(z80pio_device::write_alt));
	map(0x40, 0x43).rw(m_ic49_pio, FUNC(z80pio_device::read_alt), FUNC(z80pio_device::write_alt));

	map(0x60, 0x63).rw("flop", FUNC(cedar_magnet_flop_device::read), FUNC(cedar_magnet_flop_device::write));

	map(0x64, 0x64).portr("P1_IN");
	map(0x68, 0x68).portr("P2_IN");
	map(0x6c, 0x6c).portr("TEST");

	// banking / access controls to the sub-board memory
	map(0x70, 0x70).w(FUNC(cedar_magnet_state::rambank_palbank_w));
	map(0x74, 0x74).w(FUNC(cedar_magnet_state::palupload_w));
	map(0x78, 0x78).rw(FUNC(cedar_magnet_state::watchdog_r), FUNC(cedar_magnet_state::paladdr_w));
	map(0x7c, 0x7c).r(FUNC(cedar_magnet_state::port7c_r)); // protection??

	map(0xff, 0xff).w(m_cedsound, FUNC(cedar_magnet_sound_device::sound_command_w));
}

void cedar_magnet_state::cedar_bank0(address_map &map)
{
	/* memory configuration 0 */
	map(0x00000, 0x0ffff).m(m_sub_ram_bankdev, FUNC(address_map_bank_device::amap8));

	/* memory configuration  1 */
	map(0x10000, 0x1dfff).m(m_sub_ram_bankdev, FUNC(address_map_bank_device::amap8));
	map(0x1e000, 0x1ffff).rom().region("maincpu", 0x0000);

	/* memory configuration  2*/
	map(0x20000, 0x2bfff).m(m_sub_ram_bankdev, FUNC(address_map_bank_device::amap8));
	map(0x2c000, 0x2ffff).rw(FUNC(cedar_magnet_state::other_cpu_r), FUNC(cedar_magnet_state::other_cpu_w));

	/* memory configuration 3*/
	map(0x30000, 0x31fff).rom().region("maincpu", 0x0000).mirror(0x0e000);
}


/***********************

  7x - ports
  Main board RAM sub-board

***********************/

void cedar_magnet_state::rambank_palbank_w(u8 data)
{
	// ---- --xx
	// xx = program bank
	m_sub_ram_bankdev->set_bank(data & 0x03);

	// yyy? yy-- palette bank
	m_palbank = data;
	int palbank = ((data & 0xc0) >> 6) | (data & 0x3c);
	m_sub_pal_bankdev->set_bank(palbank);
}

void cedar_magnet_state::palupload_w(u8 data)
{
	m_sub_pal_bankdev->write8(m_paladdr, data);
}

void cedar_magnet_state::paladdr_w(u8 data)
{
	m_paladdr = data;
}

u8 cedar_magnet_state::watchdog_r()
{
	// watchdog
	return 0x00;
}


/***********************

  7c - protection??

***********************/

u8 cedar_magnet_state::port7c_r()
{
	//logerror("%s: port7c_r\n", machine().describe_context());
	return 0x01;
}


/***********************

  1x ports
  Unknown, debug? protection?

***********************/

u8 cedar_magnet_state::port18_r()
{
//  logerror("%s: port18_r\n", machine().describe_context());
	return 0x00;
}

void cedar_magnet_state::port18_w(u8 data)
{
//  logerror("%s: port18_w %02x\n", machine().describe_context(), data);
}

u8 cedar_magnet_state::port19_r()
{
	u8 ret = 0x00;
//  logerror("%s: port19_r\n", machine().describe_context());

// 9496 in a,($19)
// 9498 bit 2,a

	ret |= 0x04;

	return ret;
}

u8 cedar_magnet_state::port1a_r()
{
//  logerror("%s: port1a_r\n", machine().describe_context());
	return 0x00;
}


void cedar_magnet_state::port19_w(u8 data)
{
//  logerror("%s: port19_w %02x\n", machine().describe_context(), data);
}

void cedar_magnet_state::port1b_w(u8 data)
{
//  logerror("%s: port1b_w %02x\n", machine().describe_context(), data);
}

/***********************

  Palette / Video

***********************/

void cedar_magnet_state::set_palette(int offset)
{
	m_palette->set_pen_color(offset^0xff, pal4bit(m_pal_r[offset]), pal4bit(m_pal_g[offset]), pal4bit(m_pal_b[offset]));
}

void cedar_magnet_state::palette_r_w(offs_t offset, u8 data)
{
	m_pal_r[offset] = data;
	set_palette(offset);
}

void cedar_magnet_state::palette_g_w(offs_t offset, u8 data)
{
	m_pal_g[offset] = data;
	set_palette(offset);
}

void cedar_magnet_state::palette_b_w(offs_t offset, u8 data)
{
	m_pal_b[offset] = data;
	set_palette(offset);
}

u32 cedar_magnet_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(m_palette->black_pen(), cliprect);

	int pal = (m_palbank >> 6);

	m_cedplane1->draw(screen, bitmap, cliprect,pal);
	m_cedplane0->draw(screen, bitmap, cliprect,pal);
	m_cedsprite->draw(screen, bitmap, cliprect,pal);

	return 0;
}

void cedar_magnet_state::video_start()
{
}

/***********************

  Access to other CPUs

***********************/

u8 cedar_magnet_state::other_cpu_r(offs_t offset)
{
	int bankbit0 = (m_ic48_pio_pa_val & 0x60) >> 5;
	int plane0select = (m_ic48_pio_pa_val & 0x07) >> 0;
	int plane1select = (m_ic48_pio_pb_val & 0x07) >> 0;
	int spriteselect = (m_ic48_pio_pb_val & 0x70) >> 4;
	int soundselect = (m_ic49_pio_pb_val & 0x70) >> 4;
	int windowbank = (m_ic49_pio_pb_val & 0x0c) >> 2;
	int unk2 = (m_ic49_pio_pb_val & 0x03) >> 0;

	int cpus_accessed = 0;
	u8 ret = 0x00;

	int offset2 = offset + windowbank * 0x4000;

	if (spriteselect == 0x1)
	{
		cpus_accessed++;
		ret |= m_cedsprite->read_cpu_bus(offset2);
	}

	if (plane0select == 0x1)
	{
		cpus_accessed++;
		ret |= m_cedplane0->read_cpu_bus(offset2);
	}

	if (plane1select == 0x1)
	{
		cpus_accessed++;
		ret |= m_cedplane1->read_cpu_bus(offset2);
	}

	if (soundselect == 0x1)
	{
		cpus_accessed++;
		ret |= m_cedsound->read_cpu_bus(offset2);
		logerror("%s: reading soundselect! %04x - bank bits %d %d %d %d %d %d %d\n", machine().describe_context(), offset,bankbit0, plane0select, plane1select, spriteselect, soundselect, windowbank, unk2);
	}

	if (cpus_accessed != 1)
		logerror("%s: reading multiple CPUS!!! %04x - bank bits %d %d %d %d %d %d %d\n", machine().describe_context(), offset,bankbit0, plane0select, plane1select, spriteselect, soundselect, windowbank, unk2);

//  if ((offset==0) || (offset2 == 0xe) || (offset2 == 0xf) || (offset2 == 0x68))
//      logerror("%s: reading banked bus area %04x - bank bits %d %d %d %d %d %d %d\n", machine().describe_context(), offset,bankbit0, plane0select, plane1select, spriteselect, soundselect, windowbank, unk2);

	return ret;
}

void cedar_magnet_state::other_cpu_w(offs_t offset, u8 data)
{
	int bankbit0 = (m_ic48_pio_pa_val & 0x60) >> 5;
	int plane0select = (m_ic48_pio_pa_val & 0x07) >> 0;
	int plane1select = (m_ic48_pio_pb_val & 0x07) >> 0;
	int spriteselect = (m_ic48_pio_pb_val & 0x70) >> 4;
	int soundselect = (m_ic49_pio_pb_val & 0x70) >> 4;
	int windowbank = (m_ic49_pio_pb_val & 0x0c) >> 2;
	int unk2 = (m_ic49_pio_pb_val & 0x03) >> 0;

	int cpus_accessed = 0;

	int offset2 = offset + windowbank * 0x4000;

	if (spriteselect == 0x1)
	{
		cpus_accessed++;
		m_cedsprite->write_cpu_bus(offset2, data);
	}

	if (plane0select == 0x1)
	{
		cpus_accessed++;
		m_cedplane0->write_cpu_bus(offset2, data);
	}

	if (plane1select == 0x1)
	{
		cpus_accessed++;
		m_cedplane1->write_cpu_bus(offset2, data);
	}

	if (soundselect == 0x1)
	{
		cpus_accessed++;
		m_cedsound->write_cpu_bus(offset2, data);
	//  logerror("%s: sound cpu write %04x %02x - bank bits %d %d %d %d %d %d %d\n", machine().describe_context(), offset,data, bankbit0, plane0select, plane1select, spriteselect, soundselect, windowbank, unk2);
	}

	if (cpus_accessed != 1)
		logerror("%s: writing multiple CPUS!!! %04x %02x - bank bits %d %d %d %d %d %d %d\n", machine().describe_context(), offset,data, bankbit0, plane0select, plane1select, spriteselect, soundselect, windowbank, unk2);

//  if ((offset==0) || (offset2 == 0xe) || (offset2 == 0xf) || (offset2 == 0x68))
//      logerror("%s: other cpu write %04x %02x - bank bits %d %d %d %d %d %d %d\n", machine().describe_context(), offset,data, bankbit0, plane0select, plane1select, spriteselect, soundselect, windowbank, unk2);
}


void cedar_magnet_state::handle_sub_board_cpu_lines(cedar_magnet_board_interface &dev, int old_data, int data)
{
	if (old_data != data)
	{
		if (data & 0x04)
			dev.reset_assert();
		else
			dev.reset_clear();

		if (data & 0x02)
			dev.halt_clear();
		else
			dev.halt_assert();
	}
}

/***********************

  IC 48 PIO handlers
   (mapped at 0x20 / 0x22)

***********************/

u8 cedar_magnet_state::ic48_pio_pa_r() // 0x20
{
	u8 ret = m_ic48_pio_pa_val & ~0x08;

	ret |= m_io_coin[0]->read()<<3;
	if (!m_cedplane0->is_running()) ret &= ~0x01;

	// interrupt source stuff??
	ret &= ~0x10;

	if (LOG_IC48_PIO_PA) logerror("%s: ic48_pio_pa_r (returning %02x)\n", machine().describe_context(), ret);
	return ret;
}

void cedar_magnet_state::ic48_pio_pa_w(u8 data) // 0x20
{
	int oldplane0select = (m_ic48_pio_pa_val & 0x07) >> 0;

	// bits 19 are set to input?
	m_ic48_pio_pa_val = data;

	// address 0x20 - pio ic48 port a
	if (LOG_IC48_PIO_PA) logerror("%s: ic48_pio_pa_w %02x (memory banking etc.)\n", machine().describe_context(), data);

	if (LOG_IC48_PIO_PA) logerror("output bit 0x80 %d (unused)\n", (data >> 7)&1); // A7 -> 12 J4 unpopulated
	if (LOG_IC48_PIO_PA) logerror("output bit 0x40 %d (bank)\n", (data >> 6)&1); // A6 -> 2 74HC10 3NAND IC19
	if (LOG_IC48_PIO_PA) logerror("output bit 0x20 %d (bank)\n", (data >> 5)&1); // A5 -> 4 74HC10 3NAND IC19
	if (LOG_IC48_PIO_PA) logerror("input  bit 0x10 %d (interrupt source related?)\n", (data >> 4)&1); // 10 in // A4 <- 9 74HC74 IC20 <- input from 18 74LS244 IC61
	if (LOG_IC48_PIO_PA) logerror("input  bit 0x08 %d (COIN1)\n", (data >> 3)&1); // 08 in // A3 <- 4 74HC14P (inverter) IC4 <- EDGE 21 COIN1
	if (LOG_IC48_PIO_PA) logerror("output bit 0x04 %d (plane0 CPU/bus related?)\n", (data >> 2)&1); // A2 -> 45 J6
	if (LOG_IC48_PIO_PA) logerror("output bit 0x02 %d (plane0 CPU/bus related?)\n", (data >> 1)&1); // A1 -> 47 J6
	if (LOG_IC48_PIO_PA) logerror("input  bit 0x01 %d (plane0 CPU/bus related?)\n", (data >> 0)&1); // A0 -> 49 J6

	int bankbit0 = (m_ic48_pio_pa_val & 0x60) >> 5;
	m_bank0->set_bank(bankbit0);

	int plane0select = (m_ic48_pio_pa_val & 0x07) >> 0;

	handle_sub_board_cpu_lines(*m_cedplane0, oldplane0select, plane0select);
}


u8 cedar_magnet_state::ic48_pio_pb_r() // 0x22
{
	u8 ret = m_ic48_pio_pb_val & ~0x80;

	ret |= m_io_coin[1]->read()<<7;

	if (!m_cedsprite->is_running()) ret &= ~0x10;
	if (!m_cedplane1->is_running()) ret &= ~0x01;

	if (LOG_IC48_PIO_PB) logerror("%s: ic48_pio_pb_r (returning %02x)\n", machine().describe_context(), ret);
	return ret;
}

void cedar_magnet_state::ic48_pio_pb_w(u8 data) // 0x22
{
	int oldplane1select = (m_ic48_pio_pb_val & 0x07) >> 0;
	int oldspriteselect = (m_ic48_pio_pb_val & 0x70) >> 4;

	m_ic48_pio_pb_val = data;

	if (LOG_IC48_PIO_PB)  logerror("%s: ic48_pio_pb_w %02x\n", machine().describe_context(), data);

	// address 0x22 - pio ic48 port b
	if (LOG_IC48_PIO_PB) logerror("input  bit 0x80 %d (COIN2)\n", (data >> 7)&1); // B7 <- 2 74HC14P (inverter) IC4 <- EDGE 22 COIN2
	if (LOG_IC48_PIO_PB) logerror("output bit 0x40 (J6) (sprite CPU/bus related?) %d\n", (data >> 6)&1); // B6 -> 41 J6
	if (LOG_IC48_PIO_PB) logerror("output bit 0x20 (J6) (sprite CPU/bus related?) %d\n", (data >> 5)&1); // B5 -> 43 J6
	if (LOG_IC48_PIO_PB) logerror("input  bit 0x10 (J6) (sprite CPU/bus related?) %d\n", (data >> 4)&1); // B4 -> 44 J6
	if (LOG_IC48_PIO_PB) logerror("output bit 0x08 (Q8) %d\n", (data >> 3)&1); // B3 -> Q8 transistor
	if (LOG_IC48_PIO_PB) logerror("output bit 0x04 (J6) (plane1 CPU/bus related?) %d\n", (data >> 2)&1); // B2 -> 46 J6
	if (LOG_IC48_PIO_PB) logerror("output bit 0x02 (J6) (plane1 CPU/bus related?) %d\n", (data >> 1)&1); // B1 -> 48 J6
	if (LOG_IC48_PIO_PB) logerror("input  bit 0x01 (J6) (plane1 CPU/bus related?) %d\n", (data >> 0)&1); // B0 -> 50 J6

	int plane1select = (m_ic48_pio_pb_val & 0x07) >> 0;
	int spriteselect = (m_ic48_pio_pb_val & 0x70) >> 4;

	handle_sub_board_cpu_lines(*m_cedplane1, oldplane1select, plane1select);
	handle_sub_board_cpu_lines(*m_cedsprite, oldspriteselect, spriteselect);
}

/***********************

  IC 49 PIO handlers
     (mapped at 0x42)

***********************/

u8 cedar_magnet_state::ic49_pio_pb_r() // 0x42
{
	u8 ret = m_ic49_pio_pb_val;

	if (!m_cedsound->is_running()) ret &= ~0x10;

	if (LOG_IC49_PIO_PB) logerror("%s: ic49_pio_pb_r (returning %02x)\n", machine().describe_context(), ret);
	return ret;
}

void cedar_magnet_state::ic49_pio_pb_w(u8 data) // 0x42
{
	int oldsoundselect = (m_ic49_pio_pb_val & 0x70) >> 4;

	m_ic49_pio_pb_val = data;

	//logerror("%s: ic49_pio_pb_w %02x\n", machine().describe_context(), data);

	// address 0x42 - pio ic49 port b
	if (LOG_IC49_PIO_PB) logerror("output bit 0x80 %d (Q9)\n", (data >> 7)&1); // B7 -> Q9 transistor
	if (LOG_IC49_PIO_PB) logerror("output bit 0x40 %d (sound CPU bus related) (J3)\n", (data >> 6)&1); // B6 -> 9 J3
	if (LOG_IC49_PIO_PB) logerror("output bit 0x20 %d (sound CPU bus related) (J3)\n", (data >> 5)&1); // B5 -> 8 J3
	if (LOG_IC49_PIO_PB) logerror("input  bit 0x10 %d (sound CPU bus related) (J3)\n", (data >> 4)&1); // B4 -> 7 J3       // input?
	if (LOG_IC49_PIO_PB) logerror("output bit 0x08 %d (J7)\n", (data >> 3)&1); // B3 -> 35 J7  bank bits
	if (LOG_IC49_PIO_PB) logerror("output bit 0x04 %d (J7)\n", (data >> 2)&1); // B2 -> 36 J7  bank bits
	// there is code to mask out both bottom bits here before load operations?
	if (LOG_IC49_PIO_PB) logerror("output bit 0x02 %d (IC21)\n", (data >> 1)&1); // B1 -> 3 74HC04 IC21 (set before some SPRITE cpu operations, possibly halts the blitter?)
	if (LOG_IC49_PIO_PB) logerror("output bit 0x01 (LED) %d\n", (data >> 0)&1); // B0 -> LED LD1



	int soundselect = (m_ic49_pio_pb_val & 0x70) >> 4;

	handle_sub_board_cpu_lines(*m_cedsound, oldsoundselect, soundselect);
}

/***********************

  Init / Inputs / Machine

***********************/

void cedar_magnet_state::machine_start()
{
	save_item(NAME(m_paladdr));
}

void cedar_magnet_state::machine_reset()
{
	m_ic48_pio_pa_val = 0xff;

	int bankbit0 = (m_ic48_pio_pa_val & 0x60) >> 5;
	m_bank0->set_bank(bankbit0);
	m_sub_ram_bankdev->set_bank(3);
	m_sub_pal_bankdev->set_bank(0);
}


static INPUT_PORTS_START( cedar_magnet )
	PORT_START("COIN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )

	PORT_START("COIN2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN2 )

	PORT_START("P1_IN")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("P2_IN")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START("TEST")
	PORT_BIT( 0x7f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE_NO_TOGGLE( 0x80, IP_ACTIVE_LOW )
INPUT_PORTS_END

INTERRUPT_GEN_MEMBER(cedar_magnet_state::irq)
{
	kludge_protection();

	m_maincpu->set_input_line(0, HOLD_LINE);
	m_cedplane0->irq_hold();
	m_cedplane1->irq_hold();
	m_cedsprite->irq_hold();
}

void cedar_magnet_state::cedar_magnet(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 4000000);         /* ? MHz */
	m_maincpu->set_addrmap(AS_PROGRAM, &cedar_magnet_state::cedar_magnet_map);
	m_maincpu->set_addrmap(AS_IO, &cedar_magnet_state::cedar_magnet_io);
	m_maincpu->set_vblank_int("screen", FUNC(cedar_magnet_state::irq));

	ADDRESS_MAP_BANK(config, "bank0").set_map(&cedar_magnet_state::cedar_bank0).set_options(ENDIANNESS_LITTLE, 8, 18, 0x10000);
	ADDRESS_MAP_BANK(config, "mb_sub_ram").set_map(&cedar_magnet_state::cedar_magnet_mainboard_sub_ram_map).set_options(ENDIANNESS_LITTLE, 8, 18, 0x10000);
	ADDRESS_MAP_BANK(config, "mb_sub_pal").set_map(&cedar_magnet_state::cedar_magnet_mainboard_sub_pal_map).set_options(ENDIANNESS_LITTLE, 8, 8+6, 0x100);

	Z80PIO(config, m_ic48_pio, 4000000/2);
//  m_ic48_pio->out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
	m_ic48_pio->in_pa_callback().set(FUNC(cedar_magnet_state::ic48_pio_pa_r));
	m_ic48_pio->out_pa_callback().set(FUNC(cedar_magnet_state::ic48_pio_pa_w));
	m_ic48_pio->in_pb_callback().set(FUNC(cedar_magnet_state::ic48_pio_pb_r));
	m_ic48_pio->out_pb_callback().set(FUNC(cedar_magnet_state::ic48_pio_pb_w));

	Z80PIO(config, m_ic49_pio, 4000000/2);
//  m_ic49_pio->out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
//  m_ic49_pio->in_pa_callback().set(FUNC(cedar_magnet_state::ic49_pio_pa_r)); // NOT USED
//  m_ic49_pio->out_pa_callback().set(FUNC(cedar_magnet_state::ic49_pio_pa_w)); // NOT USED
	m_ic49_pio->in_pb_callback().set(FUNC(cedar_magnet_state::ic49_pio_pb_r));
	m_ic49_pio->out_pb_callback().set(FUNC(cedar_magnet_state::ic49_pio_pb_w));

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(256, 256);
	screen.set_visarea(0, 256-8-1, 0, 192-1);
	screen.set_screen_update(FUNC(cedar_magnet_state::screen_update));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette).set_entries(0x400);

	CEDAR_MAGNET_SOUND(config, m_cedsound, 0);
	CEDAR_MAGNET_PLANE(config, m_cedplane0, 0);
	CEDAR_MAGNET_PLANE(config, m_cedplane1, 0);
	CEDAR_MAGNET_SPRITE(config, m_cedsprite, 0);

	CEDAR_MAGNET_FLOP(config, "flop", 0);

	config.m_perfect_cpu_quantum = subtag("maincpu");
}


#define BIOS_ROM \
	ROM_REGION( 0x10000, "maincpu", 0 ) \
	ROM_LOAD( "magnet-master-vid-e03.bin", 0x00000, 0x02000, CRC(86c4a4f0) SHA1(6db1a006b2e0b2a7cc9748ade881debb098b6757) )



ROM_START( cedmag )
	BIOS_ROM

	ROM_REGION( 0x100000, "flop:disk", ROMREGION_ERASE00 )
	// no disk inserted
ROM_END

ROM_START( mag_time )
	BIOS_ROM

	ROM_REGION( 0x100000, "flop:disk", 0 )
	ROM_LOAD( "timescanner.img", 0x00000, 0xf0000, CRC(214c558c) SHA1(9c71fce35acaf17ac685f77aebb1b0a930060f0b) )
ROM_END

ROM_START( mag_exzi )
	BIOS_ROM

	ROM_REGION( 0x100000, "flop:disk", ROMREGION_ERASE00 )
	ROM_LOAD( "exzisus.img", 0x00000, 0xf0000, CRC(3705e9dc) SHA1(78c8010d224f5deb202a29bd273ea7dc85ddcdb4) )
ROM_END

ROM_START( mag_xain )
	BIOS_ROM

	ROM_REGION( 0x100000, "flop:disk", ROMREGION_ERASE00 )
	ROM_LOAD( "xain.img", 0x00000, 0xf0000, CRC(5647849f) SHA1(edd2f3f6359424583bf526bf4601476dc849e617) )
ROM_END


/*
    Data after 0xd56b0 would not read consistently, however the game only appears to use the first 24 tracks (up to 0x48fff)
    as it loads once on startup, not during gameplay, and all tracks before that gave consistent reads.  There is data after this
    point but it is likely leftovers from another game / whatever was on the disk before, so for our purposes this should be fine.

    Some bullets do seem to spawn from locations where there are no enemies, but I think this is just annoying game design.
*/
ROM_START( mag_war )
	BIOS_ROM

	ROM_REGION( 0x100000, "flop:disk", ROMREGION_ERASE00 )
	ROM_LOAD( "war mission wm 4_6_87.img", 0x00000, 0xf0000, CRC(7c813520) SHA1(2ba5999709a52302aa367fb46199b331421a0d56) )
ROM_END

/*
    Data read 100% consistently with multiple drives
*/
ROM_START( mag_wara )
	BIOS_ROM

	ROM_REGION( 0x100000, "flop:disk", ROMREGION_ERASE00 )
	ROM_LOAD( "war mission wm 9_4_87.img", 0x00000, 0xf0000, CRC(6296ea6f) SHA1(c0aaf51362bfa3362ef39c3fb1e1c848b73fd780) )
ROM_END

/*
    Data read 100% consistently with multiple drives
*/
ROM_START( mag_burn )
	BIOS_ROM

	ROM_REGION( 0x100000, "flop:disk", ROMREGION_ERASE00 ) //
	ROM_LOAD( "theburningcavern 31_3_87.img", 0x00000, 0xf0000, CRC(c95911f8) SHA1(eda3bdbbcc3e00a7da83253209e832855c2968b1) )
ROM_END

/*
    Data read 100% consistently with non-original drive (usually gives worse results)
    later tracks showed differences with original drive on each read (around 0xeef80 onwards, doesn't seem to be game data)

    weirdly there's was a single byte in an earlier track that read consistently, but in a different way for each drive
    0x2480e: 9d (non-original) vs 1d (original drive)
    1d seems to be correct as the same data is also elsewhere on the disc
*/
ROM_START( mag_day )
	BIOS_ROM

	ROM_REGION( 0x100000, "flop:disk", ROMREGION_ERASE00 )
	ROM_LOAD( "adayinspace 31_3_87.img", 0x00000, 0xf0000, CRC(bc65302d) SHA1(6ace68a0b5f7a07a8f5c318c5359011074e7f2ec) )
ROM_END

/*
    protection? (Time Scanner note)

    one part of the code is a weird loop checking values from port 0x7c while doing other nonsensical stuff, a flag gets set to 0xff if it fails

    the other part is after reading the weird extra block on the disk (score / protection data at 0xea400 in the disk image*) and again a flag
    gets set to 0xff in certain conditions there's then a check after inserting a coin, these values can't be 0xff at that point, and there
    doesn't appear to be any code to reset them.

    *0xea400 is/was track 4e, side 00, sector 01 for future reference if the floppy format changes

    all games have the same code in them but at different addresses
*/


void cedar_magnet_state::kludge_protection()
{
	const int max_addr = 0x3ffff;

	if (m_address1hack == -1)
	{
		for (int i = 0; i < max_addr - 4; i++)
		{
			if ((m_ram0[i + 0] == 0x7f) && (m_ram0[i + 1] == 0xc8) && (m_ram0[i + 2] == 0x3e) && (m_ram0[i + 3] == 0xff))
			{
				m_address1hack = i + 2;
				logerror("found patch at %06x\n", i + 2);
				break;
			}
		}
	}
	else
	{
		if ((m_ram0[m_address1hack] == 0x3e) && (m_ram0[m_address1hack + 1] == 0xff)) m_ram0[m_address1hack] = 0xc9;
	}

	if (m_address2hack == -1)
	{
		for (int i = 0; i < max_addr - 4; i++)
		{
			if ((m_ram0[i + 0] == 0x10) && (m_ram0[i + 1] == 0xdd) && (m_ram0[i + 2] == 0x3e) && (m_ram0[i + 3] == 0xff))
			{
				m_address2hack = i + 2;
				logerror("found patch at %06x\n", i + 2);
				break;
			}
		}
	}
	else
	{
		if ((m_ram0[m_address2hack] == 0x3e) && (m_ram0[m_address2hack + 1] == 0xff)) m_ram0[m_address2hack] = 0xc9;
	}
}

GAME( 1987, cedmag,   0,      cedar_magnet, cedar_magnet, cedar_magnet_state, empty_init, ROT0,  "EFO SA / Cedar", "Magnet System",                         MACHINE_IS_BIOS_ROOT )

GAME( 1987, mag_time, cedmag, cedar_magnet, cedar_magnet, cedar_magnet_state, empty_init, ROT90, "EFO SA / Cedar", "Time Scanner (TS 2.0, Magnet System)",  MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // original game was by Sega

GAME( 1987, mag_exzi, cedmag, cedar_magnet, cedar_magnet, cedar_magnet_state, empty_init, ROT0,  "EFO SA / Cedar", "Exzisus (EX 1.0, Magnet System)",       MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // original game was by Taito

GAME( 1987, mag_xain, cedmag, cedar_magnet, cedar_magnet, cedar_magnet_state, empty_init, ROT0,  "EFO SA / Cedar", "Xain'd Sleena (SC 3.0, Magnet System)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // original game was by Technos

GAME( 1987, mag_war,  cedmag, cedar_magnet, cedar_magnet, cedar_magnet_state, empty_init, ROT90, "EFO SA / Cedar", "War Mission (WM 04/06/87)",             MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // date in program
GAME( 1987, mag_wara, mag_war,cedar_magnet, cedar_magnet, cedar_magnet_state, empty_init, ROT90, "EFO SA / Cedar", "War Mission (WM 09/04/87)",             MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // the '9' was handwritten over a printed letter on disk label, date not in program

GAME( 1987, mag_burn, cedmag, cedar_magnet, cedar_magnet, cedar_magnet_state, empty_init, ROT0,  "EFO SA / Cedar", "The Burning Cavern (31/03/87)",         MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // date on label

GAME( 1987, mag_day,  cedmag, cedar_magnet, cedar_magnet, cedar_magnet_state, empty_init, ROT90, "EFO SA / Cedar", "A Day In Space (31/03/87)",             MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_SOUND ) // date on label
