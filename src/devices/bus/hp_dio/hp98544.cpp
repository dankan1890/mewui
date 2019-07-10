// license:BSD-3-Clause
// copyright-holders:R. Belmont, Sven Schnelle
/***************************************************************************

  HP98544 high-resolution monochrome board

  VRAM at 0x200000, ROM and registers at 0x560000

***************************************************************************/

#include "emu.h"
#include "hp98544.h"
#include "screen.h"

#define HP98544_SCREEN_NAME   "98544_screen"
#define HP98544_ROM_REGION    "98544_rom"

//*************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE_NS(HPDIO_98544, bus::hp_dio, dio16_98544_device, "dio98544", "HP98544 high-res monochrome DIO video card")

namespace bus {
	namespace hp_dio {

ROM_START( hp98544 )
	ROM_REGION( 0x2000, HP98544_ROM_REGION, 0 )
	ROM_LOAD( "98544_1818-1999.bin", 0x000000, 0x002000, CRC(8c7d6480) SHA1(d2bcfd39452c38bc652df39f84c7041cfdf6bd51) )
ROM_END

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void dio16_98544_device::device_add_mconfig(machine_config &config)
{
	screen_device &screen(SCREEN(config, HP98544_SCREEN_NAME, SCREEN_TYPE_RASTER));
	screen.set_screen_update(FUNC(dio16_98544_device::screen_update));
	screen.screen_vblank().set(FUNC(dio16_98544_device::vblank_w));
	screen.set_raw(XTAL(64'108'800), 1408, 0, 1024, 795, 0, 768);

	topcat_device &topcat0(TOPCAT(config, "topcat", XTAL(64108800)));
	topcat0.set_fb_width(1024);
	topcat0.set_fb_height(768);
	topcat0.set_planemask(1);
	topcat0.irq_out_cb().set(FUNC(dio16_98544_device::int_w));
}

//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------



const tiny_rom_entry *dio16_98544_device::device_rom_region() const
{
	return ROM_NAME( hp98544 );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  dio16_98544_device - constructor
//-------------------------------------------------

dio16_98544_device::dio16_98544_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	dio16_98544_device(mconfig, HPDIO_98544, tag, owner, clock)
{
}

dio16_98544_device::dio16_98544_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, type, tag, owner, clock),
	device_dio16_card_interface(mconfig, *this),
	device_memory_interface(mconfig, *this),
	m_topcat(*this, "topcat"),
	m_space_config("vram", ENDIANNESS_BIG, 8, 20, 0, address_map_constructor(FUNC(dio16_98544_device::map), this)),
	m_rom(*this, HP98544_ROM_REGION),
	m_vram(*this, "vram")
{
}

void dio16_98544_device::map(address_map& map)
{
	map(0, 0xfffff).ram().share("vram");
}

device_memory_interface::space_config_vector dio16_98544_device::memory_space_config() const
{
	return space_config_vector{ std::make_pair(0, &m_space_config) };
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void dio16_98544_device::device_start()
{
	save_item(NAME(m_intreg));
	dio().install_memory(
			0x200000, 0x2fffff,
			read16_delegate(FUNC(topcat_device::vram_r), static_cast<topcat_device*>(m_topcat)),
			write16_delegate(FUNC(topcat_device::vram_w), static_cast<topcat_device*>(m_topcat)));
	dio().install_memory(
			0x560000, 0x563fff,
			read16_delegate(FUNC(dio16_98544_device::rom_r), this),
			write16_delegate(FUNC(dio16_98544_device::rom_w), this));
	dio().install_memory(
			0x564000, 0x567fff,
			read16_delegate(FUNC(topcat_device::ctrl_r), static_cast<topcat_device*>(m_topcat)),
			write16_delegate(FUNC(topcat_device::ctrl_w), static_cast<topcat_device*>(m_topcat)));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void dio16_98544_device::device_reset()
{
}

READ16_MEMBER(dio16_98544_device::rom_r)
{
	if (offset == 1)
		return m_intreg;

	return 0xff00 | m_rom[offset];
}

// the video chip registers live here, so these writes are valid
WRITE16_MEMBER(dio16_98544_device::rom_w)
{
	if (offset == 1) {
		m_intreg = data;
	}
}

WRITE_LINE_MEMBER(dio16_98544_device::vblank_w)
{
	m_topcat->vblank_w(state);
}

WRITE_LINE_MEMBER(dio16_98544_device::int_w)
{
	int line = (m_intreg >> 3) & 7;

	if (state)
		m_intreg |= 0x40;
	else
		m_intreg &= ~0x40;
	if (!(m_intreg & 0x80))
		state = false;

	irq1_out(line == 1 && state);
	irq2_out(line == 2 && state);
	irq3_out(line == 3 && state);
	irq4_out(line == 4 && state);
	irq5_out(line == 5 && state);
	irq6_out(line == 6 && state);
	irq7_out(line == 7 && state);

}

uint32_t dio16_98544_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	int startx, starty, endx, endy;


	if (!m_topcat->has_changed())
		return UPDATE_HAS_NOT_CHANGED;

	for (int y = 0; y < m_v_pix; y++) {
		uint32_t *scanline = &bitmap.pix32(y);
		for (int x = 0; x < m_h_pix; x++) {
			uint8_t tmp = m_vram[y * m_h_pix + x];
			*scanline++ = tmp ? rgb_t(255,255,255) : rgb_t(0, 0, 0);
		}
	}

	m_topcat->get_cursor_pos(startx, starty, endx, endy);

	for (int y = starty; y <= endy; y++) {
		uint32_t *scanline = &bitmap.pix32(y);
		memset(scanline + startx, 0xff, (endx - startx) << 2);
	}

	return 0;
}

} // namespace bus::hp_dio
} // namespace bus
