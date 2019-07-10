// license:BSD-3-Clause
// copyright-holders:Angelo Salese
/***************************************************************************

    NEC PC-9801-26 sound card

    Legacy sound card for PC-98xx family, composed by a single YM2203

    TODO:
    - verify sound irq;

***************************************************************************/

#include "emu.h"
#include "bus/cbus/pc9801_26.h"

#include "sound/2203intf.h"
#include "speaker.h"


//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// device type definition
DEFINE_DEVICE_TYPE(PC9801_26, pc9801_26_device, "pc9801_26", "pc9801_26")

WRITE_LINE_MEMBER(pc9801_26_device::sound_irq)
{
	/* TODO: seems to die very often */
	m_bus->int_w<5>(state);
}


//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void pc9801_26_device::device_add_mconfig(machine_config &config)
{
	SPEAKER(config, "mono").front_center();
	YM2203(config, m_opn, 15.9744_MHz_XTAL / 4); // divider not verified
	m_opn->irq_handler().set(FUNC(pc9801_26_device::sound_irq));
	m_opn->port_a_read_callback().set(FUNC(pc9801_26_device::opn_porta_r));
	//m_opn->port_b_read_callback().set(FUNC(pc8801_state::opn_portb_r));
	//m_opn->port_a_write_callback().set(FUNC(pc8801_state::opn_porta_w));
	m_opn->port_b_write_callback().set(FUNC(pc9801_26_device::opn_portb_w));
	m_opn->add_route(ALL_OUTPUTS, "mono", 1.00);
}

// to load a different bios for slots:
// -cbus0 pc9801_26,bios=N
ROM_START( pc9801_26 )
	ROM_REGION( 0x4000, "sound_bios", ROMREGION_ERASEFF )
	// PC9801_26k is a minor change that applies to 286+ CPUs
	ROM_SYSTEM_BIOS( 0,  "26k",     "nec26k" )
	ROMX_LOAD( "26k_wyka01_00.bin", 0x0000, 0x2000, CRC(f071bf69) SHA1(f3cdef94e9fee116cf4a9b54881e77c6cd903815), ROM_SKIP(1) | ROM_BIOS(0) )
	ROMX_LOAD( "26k_wyka02_00.bin", 0x0001, 0x2000, CRC(eaa01052) SHA1(5d47edae49aad591f139d5599fe04b61aefd5ecd), ROM_SKIP(1) | ROM_BIOS(0) )
	// regular BIOS, for V30 and downward CPUs
	ROM_SYSTEM_BIOS( 1,  "26",      "nec26" )
	ROMX_LOAD( "sound.rom",       0x0000, 0x4000, CRC(80eabfde) SHA1(e09c54152c8093e1724842c711aed6417169db23), ROM_BIOS(1) )
	// following rom is unchecked and of dubious quality
	// we also currently mark it based off where they originally belonged to, lacking a better info
	ROM_SYSTEM_BIOS( 2,  "26_9821", "nec26_9821" )
	ROMX_LOAD( "sound_9821.rom",  0x0000, 0x4000, BAD_DUMP CRC(a21ef796) SHA1(34137c287c39c44300b04ee97c1e6459bb826b60), ROM_BIOS(2) )
ROM_END

const tiny_rom_entry *pc9801_26_device::device_rom_region() const
{
	return ROM_NAME( pc9801_26 );
}

//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

static INPUT_PORTS_START( pc9801_26 )
	PORT_INCLUDE( pc9801_joy_port )

	PORT_START("OPN_DSW")
	PORT_CONFNAME( 0x01, 0x01, "PC-9801-26: Port Base" )
	PORT_CONFSETTING(    0x00, "0x088" )
	PORT_CONFSETTING(    0x01, "0x188" )
INPUT_PORTS_END

ioport_constructor pc9801_26_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( pc9801_26 );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  pc9801_26_device - constructor
//-------------------------------------------------

pc9801_26_device::pc9801_26_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: pc9801_snd_device(mconfig, PC9801_26, tag, owner, clock),
		m_bus(*this, DEVICE_SELF_OWNER),
		m_opn(*this, "opn")
{
}


//-------------------------------------------------
//  device_validity_check - perform validity checks
//  on this device
//-------------------------------------------------

void pc9801_26_device::device_validity_check(validity_checker &valid) const
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------


void pc9801_26_device::device_start()
{
	m_bus->program_space().install_rom(0xcc000,0xcffff,memregion(this->subtag("sound_bios").c_str())->base());
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void pc9801_26_device::device_reset()
{
	uint16_t port_base = (ioport("OPN_DSW")->read() & 1) << 8;

	m_bus->io_space().unmap_readwrite(0x0088, 0x008b, 0x100);
	m_bus->install_io(port_base + 0x0088, port_base + 0x008b, read8_delegate(FUNC(pc9801_26_device::opn_r), this), write8_delegate(FUNC(pc9801_26_device::opn_w), this) );
}


//**************************************************************************
//  READ/WRITE HANDLERS
//**************************************************************************

// TODO: leftover mirrors? Doesn't match to what installs above
READ8_MEMBER(pc9801_26_device::opn_r)
{
	if((offset & 1) == 0)
	{
		return offset & 4 ? 0xff : m_opn->read(offset >> 1);
	}
	else // odd
	{
		printf("Read to undefined port [%02x]\n",offset+0x188);
		return 0xff;
	}
}


WRITE8_MEMBER(pc9801_26_device::opn_w)
{
	if((offset & 5) == 0)
		m_opn->write(offset >> 1, data);
	else // odd
		printf("PC9801-26: Write to undefined port [%02x] %02x\n",offset+0x188,data);
}
