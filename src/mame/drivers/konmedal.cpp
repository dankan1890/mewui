// license:BSD-3-Clause
// copyright-holders:R. Belmont
/***************************************************************************

 Tsukande Toru Chicchi (つかんでとるちっち)
 (c) 1995 Konami
 Driver by R. Belmont

Rundown of PCB:
Main CPU:  Z80
Sound: YMZ280B

Konami Custom chips:
053252 (timing/interrupt controller?)
054156 (tilemaps)
054157 (tilemaps)

Inputs:

 c702 bit 5 = medal 3 error
 c703 bit 5 = medal 4 error

***************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/eepromser.h"
#include "machine/gen_latch.h"
#include "sound/ymz280b.h"
#include "video/k054156_k054157_k056832.h"
#include "video/konami_helper.h"
#include "screen.h"
#include "speaker.h"

class konmedal_state : public driver_device
{
public:
	konmedal_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_k056832(*this, "k056832"),
		m_palette(*this, "palette"),
		m_ymz(*this, "ymz")
	{ }

	required_device<cpu_device> m_maincpu;
	required_device<k056832_device> m_k056832;
	required_device<palette_device> m_palette;
	required_device<ymz280b_device> m_ymz;

	DECLARE_PALETTE_INIT(konmedal);

	READ8_MEMBER(vram_r);
	WRITE8_MEMBER(vram_w);
	READ8_MEMBER(magic_r);
	WRITE8_MEMBER(bankswitch_w);
	WRITE8_MEMBER(control2_w);
	READ8_MEMBER(inputs_r)
	{
		return 0xff;
	}

private:
	u8 m_control, m_control2;

public:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	uint32_t screen_update_konmedal(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(konmedal_interrupt);
	K056832_CB_MEMBER(tile_callback);
};

WRITE8_MEMBER(konmedal_state::control2_w)
{
	//printf("%02x to control2\n", data);
	m_control2 = data;
}

READ8_MEMBER(konmedal_state::vram_r)
{
	if (!(m_control2 & 0x80))
	{
		if (offset & 1)
		{
			return m_k056832->ram_code_hi_r(space, offset>>1);
		}
		else
		{
			return m_k056832->ram_code_lo_r(space, offset>>1);
		}
	}
	else if (m_control == 0)    // ROM readback
	{
		return m_k056832->konmedal_rom_r(space, offset);
	}

	return 0;
}

WRITE8_MEMBER(konmedal_state::vram_w)
{
	// there are (very few) writes above F000 in some screens.
	// bug?  debug?  this?  who knows.

	if (offset & 1)
	{
		m_k056832->ram_code_hi_w(space, offset>>1, data);
		return;
	}

	m_k056832->ram_code_lo_w(space, offset>>1, data);
}

READ8_MEMBER(konmedal_state::magic_r)
{
	return 0xc1;    // checked at 60f before reading a page of the VROM
}

K056832_CB_MEMBER(konmedal_state::tile_callback)
{
	int codebits = *code;
	int bs;
	int bankshifts[4] = { 0, 4, 8, 12 };
	int mode, data, bank;

	m_k056832->read_avac(&mode, &data);

	*color = (codebits >> 12) & 0xf;
	bs = (codebits & 0xc00) >> 10;
	bank = (data >> bankshifts[bs]) & 0xf;
	*code = (codebits & 0x3ff) | (bank << 10);
}

void konmedal_state::video_start()
{
}

uint32_t konmedal_state::screen_update_konmedal(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
//  bitmap.fill(m_back_colorbase, cliprect);
	bitmap.fill(0, cliprect);
	screen.priority().fill(0, cliprect);

	// game only draws on this layer, apparently
	m_k056832->tilemap_draw(screen, bitmap, cliprect, 3, 0, 1);

	return 0;
}

PALETTE_INIT_MEMBER(konmedal_state, konmedal)
{
	int i;
	uint8_t *PROM = memregion("proms")->base();

	for (i = 0; i < 256; i++)
	{
		palette.set_pen_color(i,
			PROM[i]<<4,
			PROM[0x100+i]<<4,
			PROM[0x200+i]<<4);
	}
}

INTERRUPT_GEN_MEMBER(konmedal_state::konmedal_interrupt)
{
	m_maincpu->set_input_line(0, HOLD_LINE);
	m_k056832->mark_plane_dirty(3);
}

WRITE8_MEMBER(konmedal_state::bankswitch_w)
{
	//printf("ROM bank %x (full %02x)\n", data>>4, data);
	membank("bank1")->set_entry(data>>4);
	m_control = data & 0xf;
}

static ADDRESS_MAP_START( medal_main, AS_PROGRAM, 8, konmedal_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM AM_REGION("maincpu", 0)
	AM_RANGE(0x8000, 0x9fff) AM_ROMBANK("bank1")
	AM_RANGE(0xa000, 0xafff) AM_RAM // work RAM?
	AM_RANGE(0xb800, 0xbfff) AM_RAM // stack goes here
	AM_RANGE(0xc000, 0xc03f) AM_DEVWRITE("k056832", k056832_device, write)
	AM_RANGE(0xc100, 0xc100) AM_WRITE(control2_w)
	AM_RANGE(0xc400, 0xc400) AM_WRITE(bankswitch_w)
	AM_RANGE(0xc500, 0xc500) AM_NOP // read to reset watchdog
	AM_RANGE(0xc702, 0xc703) AM_READ(inputs_r)
	AM_RANGE(0xc800, 0xc80f) AM_DEVWRITE("k056832", k056832_device, b_w)
	AM_RANGE(0xc80f, 0xc80f) AM_READ(magic_r)
	AM_RANGE(0xd000, 0xd001) AM_DEVREADWRITE("ymz", ymz280b_device, read, write)
	AM_RANGE(0xe000, 0xffff) AM_READWRITE(vram_r, vram_w)
ADDRESS_MAP_END

static INPUT_PORTS_START( konmedal )

INPUT_PORTS_END

void konmedal_state::machine_start()
{
	membank("bank1")->configure_entries(0, 0x10, memregion("maincpu")->base(), 0x2000);
	membank("bank1")->set_entry(4);
}

void konmedal_state::machine_reset()
{
}

static MACHINE_CONFIG_START( konmedal )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80, XTAL_14_31818MHz/2) // z84c0008pec 8mhz part, 14.31818Mhz xtal verified on PCB, divisor unknown
	MCFG_CPU_PROGRAM_MAP(medal_main)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", konmedal_state, konmedal_interrupt)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(59.62)  /* verified on pcb */
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(80, 400-1, 16, 240-1)
	MCFG_SCREEN_UPDATE_DRIVER(konmedal_state, screen_update_konmedal)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_PALETTE_ADD("palette", 8192)
	MCFG_PALETTE_ENABLE_SHADOWS()
	MCFG_PALETTE_FORMAT(xBBBBBGGGGGRRRRR)
	MCFG_PALETTE_INIT_OWNER(konmedal_state, konmedal)

	MCFG_DEVICE_ADD("k056832", K056832, 0)
	MCFG_K056832_CB(konmedal_state, tile_callback)
	MCFG_K056832_CONFIG("gfx1", K056832_BPP_4, 1, 0, "none")
	MCFG_K056832_PALETTE("palette")

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_DEVICE_ADD("ymz", YMZ280B, XTAL_16_9344MHz) // 16.9344MHz xtal verified on PCB
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_CONFIG_END

ROM_START( tsukande )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* main program */
	ROM_LOAD( "441-d02.4g",   0x000000, 0x020000, CRC(6ed17227) SHA1(4e3f5219cbf6f42c60df38a99f3009fe49f78fc1) )

	ROM_REGION( 0x80000, "gfx1", 0 )   /* tilemaps */
	ROM_LOAD32_BYTE( "441-a03.4l",   0x000002, 0x020000, CRC(8adf3304) SHA1(1c8312c76cd626978ff5b3896fb5a5b34be72988) )
	ROM_LOAD32_BYTE( "441-a04.4m",   0x000003, 0x020000, CRC(038e0c67) SHA1(2b8640bfad7026a2d86fb6498aff4d7a9cb0b700) )
	ROM_LOAD32_BYTE( "441-a05.4p",   0x000000, 0x020000, CRC(937c4740) SHA1(155c869b9321d62df115435d7c855f9be4278e45) )
	ROM_LOAD32_BYTE( "441-a06.4p",   0x000001, 0x020000, CRC(947a8c45) SHA1(16e3dceb304266bbd2bddc2cec832ebff04e4c71) )

	ROM_REGION( 0x400, "proms", 0 )
	ROM_LOAD( "441a07.20k",   0x000000, 0x000100, CRC(7d0c53c2) SHA1(f357e0cb3d53374208ad1670e70be03b399a4c02) )
	ROM_LOAD( "441a08.21k",   0x000100, 0x000100, CRC(e2c3e853) SHA1(36a3008dde714ade53b9a01ac9d94c6cc655c293) )
	ROM_LOAD( "441a09.23k",   0x000200, 0x000100, CRC(3daca33a) SHA1(38644f574beaa593f3348b49eabea9e03d722013) )
	ROM_LOAD( "441a10.21m",   0x000300, 0x000100, CRC(063722ff) SHA1(7ba43acfdccb02e7913dc000c4f9c57c54b1315f) )

	ROM_REGION( 0x100000, "ymz", 0 )
	ROM_LOAD( "441a11.10d",   0x000000, 0x080000, CRC(e60a7495) SHA1(76963324e818974bc5209e7122282ba4d73fda93) )
	ROM_LOAD( "441a12.10e",   0x080000, 0x080000, CRC(dc2dd5bc) SHA1(28ef6c96c360d706a4296a686f3f2a54fce61bfb) )
ROM_END

GAME( 1995, tsukande, 0, konmedal, konmedal,  konmedal_state, 0, ROT0, "Konami", "Tsukande Toru Chicchi", MACHINE_NOT_WORKING)
