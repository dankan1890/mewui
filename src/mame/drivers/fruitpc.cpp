// license:BSD-3-Clause
// copyright-holders:R. Belmont, Carl
/************************************************************************************

  "Fruit Land" (c) ????
  hack of an open source game by MesSoft with coin handling added

  preliminary driver by R. Belmont and Carl

  Hardware:
  - ST STPCD0166BTC3 486/66 + PC + VGA all on one chip
  - 4x AS4LC1M16E5-60TC 1M x 16 EDO DRAM

=====================================================================================*/

#include "emu.h"
#include "cpu/i386/i386.h"
#include "machine/lpci.h"
#include "machine/pckeybrd.h"
#include "machine/idectrl.h"
#include "video/pc_vga.h"
#include "machine/pcshare.h"
#include "bus/isa/isa.h"
#include "bus/isa/sblaster.h"

class fruitpc_state : public pcat_base_state
{
public:
	fruitpc_state(const machine_config &mconfig, device_type type, const char *tag)
		: pcat_base_state(mconfig, type, tag)
		, m_isabus(*this, "isa")
		, m_inp(*this, "INP%u", 1U)
	{ }

	void fruitpc(machine_config &config);

private:
	required_device<isa8_device> m_isabus;
	required_ioport_array<4> m_inp;

	DECLARE_READ8_MEMBER(fruit_inp_r);
	DECLARE_WRITE8_MEMBER(dma8237_1_dack_w);
	static void fruitpc_sb_conf(device_t *device);
	void fruitpc_io(address_map &map);
	void fruitpc_map(address_map &map);
};

READ8_MEMBER(fruitpc_state::fruit_inp_r)
{
	return m_inp[offset & 0x03]->read();
}

void fruitpc_state::fruitpc_map(address_map &map)
{
	map(0x00000000, 0x0009ffff).ram();
	map(0x000a0000, 0x000bffff).rw("vga", FUNC(vga_device::mem_r), FUNC(vga_device::mem_w)); // VGA VRAM
	map(0x000c0000, 0x000dffff).rom().region("bios", 0);
	map(0x000e0000, 0x000fffff).ram().region("bios", 0);
	map(0x00100000, 0x008fffff).ram();  // 8MB RAM
	map(0x02000000, 0x28ffffff).noprw();
	map(0xfffe0000, 0xffffffff).rom().region("bios", 0);    /* System BIOS */
}

void fruitpc_state::fruitpc_io(address_map &map)
{
	pcat32_io_common(map);
	map(0x01f0, 0x01f7).rw("ide", FUNC(ide_controller_device::cs0_r), FUNC(ide_controller_device::cs0_w));
	map(0x0310, 0x0313).r(FUNC(fruitpc_state::fruit_inp_r));
	map(0x03b0, 0x03bf).rw("vga", FUNC(vga_device::port_03b0_r), FUNC(vga_device::port_03b0_w));
	map(0x03c0, 0x03cf).rw("vga", FUNC(vga_device::port_03c0_r), FUNC(vga_device::port_03c0_w));
	map(0x03d0, 0x03df).rw("vga", FUNC(vga_device::port_03d0_r), FUNC(vga_device::port_03d0_w));
	map(0x03f0, 0x03f7).rw("ide", FUNC(ide_controller_device::cs1_r), FUNC(ide_controller_device::cs1_w));
}

static INPUT_PORTS_START( fruitpc )
	PORT_START("INP1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x00fe, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_START("INP2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x00c0, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_START("INP3")
	PORT_BIT( 0x0003, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_DIPNAME( 0x0004, 0x0004, "CONFIGURATION" )
	PORT_DIPSETTING( 0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, "STATISTICHE" )
	PORT_DIPSETTING( 0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING( 0x0000, DEF_STR( On ) )
	PORT_BIT( 0x00f0, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_START("INP4")
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

//TODO: use atmb device
WRITE8_MEMBER( fruitpc_state::dma8237_1_dack_w ){ m_isabus->dack_w(1, data); }

static void fruitpc_isa8_cards(device_slot_interface &device)
{
	device.option_add("sb15",  ISA8_SOUND_BLASTER_1_5);
}

static DEVICE_INPUT_DEFAULTS_START( fruitpc_sb_def )
	DEVICE_INPUT_DEFAULTS("CONFIG", 0x03, 0x01)
DEVICE_INPUT_DEFAULTS_END

void fruitpc_state::fruitpc_sb_conf(device_t *device)
{
	device->subdevice<pc_joy_device>("pc_joy")->set_default_option(nullptr); // remove joystick
}

void fruitpc_state::fruitpc(machine_config &config)
{
	I486(config, m_maincpu, 66000000); // ST STPCD0166BTC3 66 MHz 486 CPU
	m_maincpu->set_addrmap(AS_PROGRAM, &fruitpc_state::fruitpc_map);
	m_maincpu->set_addrmap(AS_IO, &fruitpc_state::fruitpc_io);
	m_maincpu->set_irq_acknowledge_callback("pic8259_1", FUNC(pic8259_device::inta_cb));

	pcat_common(config);

	ide_controller_device &ide(IDE_CONTROLLER(config, "ide").options(ata_devices, "hdd", nullptr, true));
	ide.irq_handler().set("pic8259_2", FUNC(pic8259_device::ir6_w));

	/* video hardware */
	pcvideo_vga(config);

	m_dma8237_1->out_iow_callback<1>().set(FUNC(fruitpc_state::dma8237_1_dack_w));

	ISA8(config, m_isabus, 0);
	m_isabus->set_memspace("maincpu", AS_PROGRAM);
	m_isabus->set_iospace("maincpu", AS_IO);
	m_isabus->irq2_callback().set("pic8259_2", FUNC(pic8259_device::ir2_w));
	m_isabus->irq3_callback().set("pic8259_1", FUNC(pic8259_device::ir3_w));
	m_isabus->irq4_callback().set("pic8259_1", FUNC(pic8259_device::ir4_w));
	m_isabus->irq5_callback().set("pic8259_1", FUNC(pic8259_device::ir5_w));
	m_isabus->irq6_callback().set("pic8259_1", FUNC(pic8259_device::ir6_w));
	m_isabus->irq7_callback().set("pic8259_1", FUNC(pic8259_device::ir7_w));
	m_isabus->drq1_callback().set("dma8237_1", FUNC(am9517a_device::dreq1_w));
	m_isabus->drq2_callback().set("dma8237_1", FUNC(am9517a_device::dreq2_w));
	m_isabus->drq3_callback().set("dma8237_1", FUNC(am9517a_device::dreq3_w));

	// FIXME: determine ISA bus clock
	isa8_slot_device &isa1(ISA8_SLOT(config, "isa1", 0, "isa", fruitpc_isa8_cards, "sb15", true));
	isa1.set_option_device_input_defaults("sb15", DEVICE_INPUT_DEFAULTS_NAME(fruitpc_sb_def));
	isa1.set_option_machine_config("sb15", fruitpc_sb_conf);
}

ROM_START( fruitpc )
	ROM_REGION( 0x20000, "bios", 0 )
	ROM_LOAD( "at-gs001.bin", 0x000000, 0x020000, CRC(7dec34d0) SHA1(81d194d67fef9f6531bd3cd1ee0baacb5c2558bf) )

	DISK_REGION( "ide:0:hdd:image" )    // 8 MB Compact Flash card
	DISK_IMAGE( "fruit", 0,SHA1(df250ff06a97fa141a4144034f7035ac2947c53c) )
ROM_END

GAME( 2006, fruitpc, 0, fruitpc, fruitpc, fruitpc_state, empty_init, ROT0, "<unknown>", "Fruit Land", MACHINE_IMPERFECT_GRAPHICS )

// this doesn't really belong here, but is some kind of x86 pc-like hardware, exact CPU type etc. unknown
// hardware ia by Paokai, motherboard has logos, large chip with logo too, http://www.paokai.com.tw/
ROM_START( gogostrk )
	ROM_REGION( 0x40000, "bios", 0 )
	ROM_LOAD( "39sf020a.rom1", 0x000000, 0x040000, CRC(236d4d95) SHA1(50579acddc93c05d5f8e17ad3669a29d2dc49965) )

	DISK_REGION( "ide:0:hdd:image" )    // 128 MB CF Card
	DISK_IMAGE( "ggs-5-2-07", 0,SHA1(f214fd39ec8ac02f008823f4b179ea6c6835e1b8) )
ROM_END

GAME( 2007, gogostrk, 0, fruitpc, fruitpc, fruitpc_state, empty_init, ROT0, "American Alpha / Paokai", "Go Go Strike", MACHINE_NOT_WORKING ) // motherboard is dated 2006, if the CF card string is a date it's 2007
