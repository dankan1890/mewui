// license:BSD-3-Clause
// copyright-holders:
/****************************************************************************************

Unknown Color Poker Game
------------------------------

8080A CPU

Four 2716 eproms

Six 2102 Rams

Two 2112 Rams

Two 5101 Rams (Low Power Versions, one connected to Battery)

Four position DIP Switch - DIP 1 changes on screen text from
normal to high lighted as seen pics. Other DIPS unknown.

Sound?

Date of manufacture unknown. Latest date codes on logic chips is 1980.

Chaneman 3/20/2019


TODO: everything

*******************************************************************************************/

#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "machine/nvram.h"

#include "screen.h"
#include "emupal.h"
#include "speaker.h"

class unkpoker_state : public driver_device
{
public:
	unkpoker_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this,"maincpu"),
		m_videoram(*this, "videoram"),
		m_chargen(*this, "gfx1")
	{ }

	void unkpoker(machine_config &config);

private:
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void mem_map(address_map &map);
	void io_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	required_shared_ptr<uint8_t> m_videoram;
	required_region_ptr<u8> m_chargen;
};


uint32_t unkpoker_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect) // taken from video21.cpp
{
	uint16_t sy = 0, ma = 0;

	for (uint8_t y = 0; y < 28; y++)
	{
		for (uint8_t ra = 0; ra < 8; ra++)
		{
			uint16_t *p = &bitmap.pix16(sy++);

			for (uint16_t x = 0; x < 32; x++)
			{
				uint8_t chr = m_videoram[x + ma] & 0x7f;
				uint8_t gfx = m_chargen[(chr << 3) | ra];

				/* Display a scanline of a character */
				*p++ = BIT(gfx, 7);
				*p++ = BIT(gfx, 6);
				*p++ = BIT(gfx, 5);
				*p++ = BIT(gfx, 4);
				*p++ = BIT(gfx, 3);
				*p++ = BIT(gfx, 2);
				*p++ = BIT(gfx, 1);
				*p++ = BIT(gfx, 0);
			}
		}
		ma += 32;
	}
	return 0;
}


void unkpoker_state::mem_map(address_map &map) {
	map(0x0000,0x0fff).rom();
	map(0x1000,0x1fff).ram();
	map(0x8000,0x83ff).ram().share("videoram");
}

void unkpoker_state::io_map(address_map &map) {
	map(0x01,0x01).portr("IN");
	map(0x02,0x02).portr("DSW");
}


static INPUT_PORTS_START( unkpoker )
	PORT_START("IN")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPUNKNOWN_DIPLOC(0x01, 0x01, "DSW:1")
	PORT_DIPUNKNOWN_DIPLOC(0x02, 0x02, "DSW:2")
	PORT_DIPUNKNOWN_DIPLOC(0x04, 0x04, "DSW:3")
	PORT_DIPUNKNOWN_DIPLOC(0x08, 0x08, "DSW:4")
INPUT_PORTS_END


static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,1),
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};


static GFXDECODE_START( gfx_unkpoker )
	GFXDECODE_ENTRY( "gfx1", 0, tiles8x8_layout, 0, 1 )
GFXDECODE_END


void unkpoker_state::unkpoker(machine_config &config)
{
	/* basic machine hardware */
	I8080A(config, m_maincpu, 2000000); // guessed
	m_maincpu->set_addrmap(AS_PROGRAM, &unkpoker_state::mem_map);
	m_maincpu->set_addrmap(AS_IO, &unkpoker_state::io_map);

	//NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(256, 256);
	screen.set_visarea(0, 255, 0, 255);
	screen.set_screen_update(FUNC(unkpoker_state::screen_update));
	screen.set_palette("palette");

	GFXDECODE(config, "gfxdecode", "palette", gfx_unkpoker);
	PALETTE(config, "palette", palette_device::RGB_3BIT);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
}


ROM_START( unkpoker )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "cdpat.13b",  0x0000, 0x0800, CRC(c3f3040f) SHA1(ed1916bcd9e1e80502fcff5ddd599c101a226e7c) )
	ROM_LOAD( "cdpat.11b",  0x0800, 0x0800, CRC(16a97398) SHA1(737192dd0e1b3083f1facd327a83d98a0b7f4d66) )

	ROM_REGION( 0x1000, "gfx1", 0 )
	ROM_LOAD( "cd.3b", 0x0000, 0x0800, CRC(e3997d7d) SHA1(6c595c70afedc7aef024215d153fe31b418adc25) )
	ROM_LOAD( "cd.3c", 0x0800, 0x0800, CRC(b61adb76) SHA1(9805593fc6d9b01e4a63bfc35e5442c4c547c103) )
ROM_END

GAME(1980?, unkpoker, 0, unkpoker, unkpoker, unkpoker_state, empty_init, ROT0, "<unknown>", "unknown 1980 poker game", MACHINE_NOT_WORKING | MACHINE_NO_SOUND | MACHINE_SUPPORTS_SAVE)
