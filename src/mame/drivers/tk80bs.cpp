// license:BSD-3-Clause
// copyright-holders:Angelo Salese, Robbbert
/***************************************************************************


NEC TK80BS
**********
TK-80BS (c) 1980 NEC

Preliminary driver by Angelo Salese
Various additions by Robbbert

The TK80BS (Basic Station) has a plugin keyboard, BASIC in rom,
and connected to a tv.

TODO:
    - (try to) dump proper roms, the whole driver is based off fake roms;
    - bios 0 BASIC doesn't seem to work properly; (It does if you type NEW first)
    - bios 1 does not boot up because it runs off into the weeds
    - bios 2 also does that, somehow it starts up anyway, but no commands work


****************************************************************************/

#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "machine/i8255.h"
#include "machine/keyboard.h"
#include "emupal.h"
#include "screen.h"

class tk80bs_state : public driver_device
{
public:
	tk80bs_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_p_videoram(*this, "videoram"),
		m_maincpu(*this, "maincpu"),
		m_ppi(*this, "ppi"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette")
	{
	}

	void tk80bs(machine_config &config);

private:
	DECLARE_READ8_MEMBER(ppi_custom_r);
	DECLARE_WRITE8_MEMBER(ppi_custom_w);
	void kbd_put(u8 data);
	DECLARE_READ8_MEMBER(port_a_r);
	DECLARE_READ8_MEMBER(port_b_r);
	uint32_t screen_update_tk80bs(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	required_shared_ptr<uint8_t> m_p_videoram;
	void tk80bs_mem(address_map &map);

	uint8_t m_term_data;
	required_device<cpu_device> m_maincpu;
	required_device<i8255_device> m_ppi;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
};


uint32_t tk80bs_state::screen_update_tk80bs(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int x,y;
	int count;

	count = 0;

	for(y=0;y<16;y++)
	{
		for(x=0;x<32;x++)
		{
			int tile = m_p_videoram[count++];

			m_gfxdecode->gfx(0)->opaque(bitmap,cliprect, tile, 0, 0, 0, x*8, y*8);
		}
	}

	return 0;
}

/* A0 and A1 are swapped at the 8255 chip */
READ8_MEMBER( tk80bs_state::ppi_custom_r )
{
	switch(offset)
	{
		case 1:
			return m_ppi->read(2);
		case 2:
			return m_ppi->read(1);
		default:
			return m_ppi->read(offset);
	}
}

WRITE8_MEMBER( tk80bs_state::ppi_custom_w )
{
	switch(offset)
	{
		case 1:
			m_ppi->write(2, data);
			break;
		case 2:
			m_ppi->write(1, data);
			break;
		default:
			m_ppi->write(offset, data);
	}
}

void tk80bs_state::tk80bs_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x07ff).rom();
//  AM_RANGE(0x0c00, 0x7bff) AM_ROM // ext
	map(0x7df8, 0x7df9).noprw(); // i8251 sio
	map(0x7dfc, 0x7dff).rw(FUNC(tk80bs_state::ppi_custom_r), FUNC(tk80bs_state::ppi_custom_w));
	map(0x7e00, 0x7fff).ram().share("videoram"); // video ram
	map(0x8000, 0xcfff).ram(); // RAM
	map(0xd000, 0xefff).rom(); // BASIC
	map(0xf000, 0xffff).rom(); // BSMON
}


/* Input ports */
static INPUT_PORTS_START( tk80bs )
INPUT_PORTS_END

READ8_MEMBER( tk80bs_state::port_a_r )
{
	uint8_t ret = m_term_data;
	m_term_data = 0;
	return ret;
}


READ8_MEMBER( tk80bs_state::port_b_r )
{
	if (m_term_data)
	{
		m_ppi->pc4_w(0); // send a strobe pulse
		return 0x20;
	}
	else
		return 0;
}

void tk80bs_state::kbd_put(u8 data)
{
	data &= 0x7f;
	if (data > 0x5f) data-=0x20;
	m_term_data = data;
}

/* F4 Character Displayer */
static const gfx_layout tk80bs_charlayout =
{
	8, 8,
	512,
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static GFXDECODE_START( gfx_tk80bs )
	GFXDECODE_ENTRY( "chargen", 0x0000, tk80bs_charlayout, 0, 1 )
GFXDECODE_END


void tk80bs_state::tk80bs(machine_config &config)
{
	/* basic machine hardware */
	I8080(config, m_maincpu, XTAL(1'000'000)); //unknown clock
	m_maincpu->set_addrmap(AS_PROGRAM, &tk80bs_state::tk80bs_mem);

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	screen.set_size(256, 128);
	screen.set_visarea(0, 256-1, 0, 128-1);
	screen.set_screen_update(FUNC(tk80bs_state::screen_update_tk80bs));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette, palette_device::MONOCHROME);
	GFXDECODE(config, m_gfxdecode, m_palette, gfx_tk80bs);

	/* Devices */
	I8255(config, m_ppi);
	m_ppi->in_pa_callback().set(FUNC(tk80bs_state::port_a_r));
	m_ppi->in_pb_callback().set(FUNC(tk80bs_state::port_b_r));

	generic_keyboard_device &keyboard(GENERIC_KEYBOARD(config, "keyboard", 0));
	keyboard.set_keyboard_callback(FUNC(tk80bs_state::kbd_put));
}


ROM_START( tk80bs )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	/* all of these aren't taken from an original machine*/
	ROM_SYSTEM_BIOS(0, "psedo", "Pseudo LEVEL 1")
	ROMX_LOAD( "tk80.dummy", 0x0000, 0x0800, BAD_DUMP CRC(553b25ca) SHA1(939350d7fa56ce567ddf393c9f4b9db6ebc18a2c), ROM_BIOS(0))
	ROMX_LOAD( "ext.l1",     0x0c00, 0x6e46, BAD_DUMP CRC(d05ed3ff) SHA1(8544aa2cb58df9edf221f5be2cdafa248dd33828), ROM_BIOS(0))
	ROMX_LOAD( "lv1basic.l1",0xe000, 0x09a2, BAD_DUMP CRC(3ff67a71) SHA1(528c9331740637e853c099e1739ecdea6dd200bc), ROM_BIOS(0))
	ROMX_LOAD( "bsmon.l1",   0xf000, 0x0db0, BAD_DUMP CRC(5daa599b) SHA1(7e6ec5bfb3eea114f7ee9ef589a89246b8533b2f), ROM_BIOS(0))

	ROM_SYSTEM_BIOS(1, "psedo10", "Pseudo LEVEL 2 1.0")
	ROMX_LOAD( "tk80.dummy", 0x0000, 0x0800, BAD_DUMP CRC(553b25ca) SHA1(939350d7fa56ce567ddf393c9f4b9db6ebc18a2c), ROM_BIOS(1))
	ROMX_LOAD( "ext.10",     0x0c00, 0x3dc2, BAD_DUMP CRC(3c64d488) SHA1(919180d5b34b981ab3dd8b2885d3c0933203f355), ROM_BIOS(1))
	ROMX_LOAD( "lv2basic.10",0xd000, 0x2000, BAD_DUMP CRC(594fe70e) SHA1(5854c1be5fa78c1bfee365379495f14bc23e15e7), ROM_BIOS(1))
	ROMX_LOAD( "bsmon.10",   0xf000, 0x0daf, BAD_DUMP CRC(d0047983) SHA1(79e2b5dc47b574b55375cbafffff144744093ec1), ROM_BIOS(1))

	ROM_SYSTEM_BIOS(2, "psedo11", "Pseudo LEVEL 2 1.1")
	ROMX_LOAD( "tk80.dummy", 0x0000, 0x0800, BAD_DUMP CRC(553b25ca) SHA1(939350d7fa56ce567ddf393c9f4b9db6ebc18a2c), ROM_BIOS(2))
	ROMX_LOAD( "ext.11",     0x0c00, 0x3dd4, BAD_DUMP CRC(bd5c5169) SHA1(2ad70828348372328b76bac0fa93d3f6f17ade34), ROM_BIOS(2))
	ROMX_LOAD( "lv2basic.11",0xd000, 0x2000, BAD_DUMP CRC(3df9a3bd) SHA1(9539409c876bce27d630fe47d07a4316d2ce09cb), ROM_BIOS(2))
	ROMX_LOAD( "bsmon.11",   0xf000, 0x0ff6, BAD_DUMP CRC(fca7a609) SHA1(7c7eb5e5e4cf1e0021383bdfc192b88262aba6f5), ROM_BIOS(2))

	ROM_REGION( 0x1000, "chargen", ROMREGION_ERASEFF )
	ROM_LOAD( "font.rom",    0x0000, 0x1000, BAD_DUMP CRC(94d95199) SHA1(9fe741eab866b0c520d4108bccc6277172fa190c))
ROM_END

/* Driver */

//    YEAR  NAME    PARENT  COMPAT  MACHINE  INPUT   CLASS         INIT        COMPANY  FULLNAME   FLAGS
COMP( 1980, tk80bs, tk80,   0,      tk80bs,  tk80bs, tk80bs_state, empty_init, "NEC",   "TK-80BS", MACHINE_NOT_WORKING | MACHINE_NO_SOUND_HW)
