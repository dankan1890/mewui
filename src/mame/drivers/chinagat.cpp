// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria,Paul Hampson, Quench
/*
China Gate.
By Paul Hampson from First Principles
(IE: Roms + a description of their contents and a list of CPUs on board.)

Based on ddragon.c:
"Double Dragon, Double Dragon (bootleg) & Double Dragon II"
"By Carlos A. Lozano & Rob Rosenbrock et. al."

NOTES:
A couple of things unaccounted for:

No backgrounds ROMs from the original board...
- This may be related to the SubCPU. I don't think it's contributing
  much right now, but I could be wrong. And it would explain that vast
  expanse of bankswitch ROM on a slave CPU....
- Just had a look at the sprites, and they seem like kosher sprites all
  the way up.... So it must be hidden in the sub-cpu somewhere?
- Got two bootleg sets with background gfx roms. Using those on the
  original games for now.

OBVIOUS SPEED PROBLEMS...
- Timers are too fast and/or too slow, and the whole thing's moving too fast

Port 0x2800 on the Sub CPU.
- All those I/O looking ports on the main CPU (0x3exx and 0x3fxx)
- One's scroll control. Probably other video control as well.
- Location 0x1a2ec in cgate51.bin (The main CPU's ROM) is 88. This is
  copied to videoram, and causes that minor visual discrepancy on
  the title screen. But the CPU tests that part of the ROM and passes
  it OK. Since it's just a simple summing of words, another word
  somewhere (or others in total) has lost 0x8000. Or the original
  game had this problem. (Not on the screenshot I got)
- The Japanese ones have a different title screen so I can't check.

ADPCM in the bootlegs is not quite right.... Misusing the data?
- They're nibble-swapped versions of the original roms...
- There's an Intel i8748 CPU on the bootlegs (bootleg 1 lists D8749 but
  the microcode dump's the same). This in conjunction with the different
  ADPCM chip (msm5205) are used to 'fake' a M6295.
- Bootleg 1 ADPCM is now wired up, but still not working :-(
  Definitely sync problems between the i8049 and the m5205 which need
  further looking at.


There's also a few small dumps from the boards.


MAJOR DIFFERENCES FROM DOUBLE DRAGON:
Sound system is like Double Dragon II (In fact for MAME's
purposes it's identical. I think DD3 and one or two others
also use this. Was it an addon on the original?
The dual-CPU setup looked similar to DD at first, but
the second CPU doesn't talk to the sprite RAM at all, but
just through the shared memory (which DD1 doesn't have,
except for the sprite RAM.)
Also the 2nd CPU in China Gate has just as much code as
the first CPU, and bankswitches similarly, where DD1 and DD2 have
different Sprite CPUs but only a small bank of code each.
More characters and colours of characters than DD1 or 2.
More sprites than DD1, less than DD2.
But the formats are the same (allowing for extra chars and colours)
Video hardware's like DD1 (thank god)
Input is unique but has a few similarities to DD2 (the coin inputs)

2008-07
Dip locations and factory settings verified with China Gate US manual.
*/

#include "emu.h"
#include "includes/ddragon.h"

#include "cpu/m6809/hd6309.h"
#include "cpu/m6809/m6809.h"
#include "cpu/mcs48/mcs48.h"
#include "cpu/z80/z80.h"
#include "machine/timer.h"
#include "sound/2203intf.h"
#include "sound/okim6295.h"
#include "sound/ym2151.h"
#include "speaker.h"


#define MAIN_CLOCK      XTAL(12'000'000)
#define PIXEL_CLOCK     MAIN_CLOCK / 2

class chinagat_state : public ddragon_state
{
public:
	chinagat_state(const machine_config &mconfig, device_type type, const char *tag)
		: ddragon_state(mconfig, type, tag)
		, m_adpcm(*this, "adpcm")
	{ }

	void chinagat(machine_config &config);
	void saiyugoub1(machine_config &config);
	void saiyugoub2(machine_config &config);

	void init_chinagat();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

private:
	TIMER_DEVICE_CALLBACK_MEMBER(chinagat_scanline);
	DECLARE_WRITE8_MEMBER(interrupt_w);
	DECLARE_WRITE8_MEMBER(video_ctrl_w);
	DECLARE_WRITE8_MEMBER(bankswitch_w);
	DECLARE_WRITE8_MEMBER(sub_bankswitch_w);
	DECLARE_WRITE8_MEMBER(sub_irq_ack_w);
	DECLARE_READ8_MEMBER(saiyugoub1_mcu_command_r);
	DECLARE_WRITE8_MEMBER(saiyugoub1_mcu_command_w);
	DECLARE_WRITE8_MEMBER(saiyugoub1_adpcm_rom_addr_w);
	DECLARE_WRITE8_MEMBER(saiyugoub1_adpcm_control_w);
	DECLARE_WRITE8_MEMBER(saiyugoub1_m5205_clk_w);
	DECLARE_READ_LINE_MEMBER(saiyugoub1_m5205_irq_r);
	DECLARE_WRITE_LINE_MEMBER(saiyugoub1_m5205_irq_w);

	void i8748_map(address_map &map);
	void main_map(address_map &map);
	void saiyugoub1_sound_map(address_map &map);
	void sound_map(address_map &map);
	void sub_map(address_map &map);
	void ym2203c_sound_map(address_map &map);

	optional_device<msm5205_device> m_adpcm;
};


void chinagat_state::video_start()
{
	ddragon_state::video_start();

	m_bg_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(chinagat_state::get_bg_tile_info),this),tilemap_mapper_delegate(FUNC(chinagat_state::background_scan),this), 16, 16, 32, 32);
	m_fg_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(FUNC(chinagat_state::get_fg_16color_tile_info),this),TILEMAP_SCAN_ROWS, 8, 8, 32, 32);

	m_fg_tilemap->set_transparent_pen(0);
	m_fg_tilemap->set_scrolldy(-8, -8);
	m_bg_tilemap->set_scrolldy(-8, -8);
}
/*
    Based on the Solar Warrior schematics, vertical timing counts as follows:

        08,09,0A,0B,...,FC,FD,FE,FF,E8,E9,EA,EB,...,FC,FD,FE,FF,
        08,09,....

    Thus, it counts from 08 to FF, then resets to E8 and counts to FF again.
    This gives (256 - 8) + (256 - 232) = 248 + 24 = 272 total scanlines.

    VBLK is signalled starting when the counter hits F8, and continues through
    the reset to E8 and through until the next reset to 08 again.

    Since MAME's video timing is 0-based, we need to convert this.
*/

TIMER_DEVICE_CALLBACK_MEMBER(chinagat_state::chinagat_scanline)
{
	int scanline = param;
	int screen_height = m_screen->height();
	int vcount_old = scanline_to_vcount((scanline == 0) ? screen_height - 1 : scanline - 1);
	int vcount = scanline_to_vcount(scanline);

	/* update to the current point */
	if (scanline > 0)
		m_screen->update_partial(scanline - 1);

	/* on the rising edge of VBLK (vcount == F8), signal an NMI */
	if (vcount == 0xf8)
		m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);

	/* set 1ms signal on rising edge of vcount & 8 */
	if (!(vcount_old & 8) && (vcount & 8))
		m_maincpu->set_input_line(M6809_FIRQ_LINE, ASSERT_LINE);

	/* adjust for next scanline */
	if (++scanline >= screen_height)
		scanline = 0;
}

WRITE8_MEMBER(chinagat_state::interrupt_w)
{
	switch (offset)
	{
		case 0: /* 3e00 - SND irq */
			m_soundlatch->write(data);
			break;

		case 1: /* 3e01 - NMI ack */
			m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
			break;

		case 2: /* 3e02 - FIRQ ack */
			m_maincpu->set_input_line(M6809_FIRQ_LINE, CLEAR_LINE);
			break;

		case 3: /* 3e03 - IRQ ack */
			m_maincpu->set_input_line(M6809_IRQ_LINE, CLEAR_LINE);
			break;

		case 4: /* 3e04 - sub CPU IRQ ack */
			m_subcpu->set_input_line(m_sprite_irq, ASSERT_LINE);
			break;
	}
}

WRITE8_MEMBER(chinagat_state::video_ctrl_w)
{
	/***************************
	---- ---x   X Scroll MSB
	---- --x-   Y Scroll MSB
	---- -x--   Flip screen
	--x- ----   Enable video ???
	****************************/
	m_scrolly_hi = ((data & 0x02) >> 1);
	m_scrollx_hi = data & 0x01;

	flip_screen_set(~data & 0x04);
}

WRITE8_MEMBER(chinagat_state::bankswitch_w)
{
	membank("bank1")->set_entry(data & 0x07); // shall we check (data & 7) < 6 (# of banks)?
}

WRITE8_MEMBER(chinagat_state::sub_bankswitch_w)
{
	membank("bank4")->set_entry(data & 0x07); // shall we check (data & 7) < 6 (# of banks)?
}

WRITE8_MEMBER(chinagat_state::sub_irq_ack_w)
{
	m_subcpu->set_input_line(m_sprite_irq, CLEAR_LINE);
}

READ8_MEMBER(chinagat_state::saiyugoub1_mcu_command_r )
{
#if 0
	if (m_mcu_command == 0x78)
	{
		m_mcu->suspend(SUSPEND_REASON_HALT, 1); /* Suspend (speed up) */
	}
#endif
	return m_mcu_command;
}

WRITE8_MEMBER(chinagat_state::saiyugoub1_mcu_command_w )
{
	m_mcu_command = data;
#if 0
	if (data != 0x78)
	{
		m_mcu->resume(SUSPEND_REASON_HALT); /* Wake up */
	}
#endif
}

WRITE8_MEMBER(chinagat_state::saiyugoub1_adpcm_rom_addr_w )
{
	/* i8748 Port 1 write */
	m_i8748_P1 = data;
}

WRITE8_MEMBER(chinagat_state::saiyugoub1_adpcm_control_w )
{
	/* i8748 Port 2 write */
	uint8_t *saiyugoub1_adpcm_rom = memregion("adpcm")->base();

	if (data & 0x80)    /* Reset m5205 and disable ADPCM ROM outputs */
	{
		logerror("ADPCM output disabled\n");
		m_pcm_nibble = 0x0f;
		m_adpcm->reset_w(1);
	}
	else
	{
		if ((m_i8748_P2 & 0xc) != (data & 0xc))
		{
			if ((m_i8748_P2 & 0xc) == 0) /* Latch MSB Address */
			{
///             logerror("Latching MSB\n");
				m_adpcm_addr = (m_adpcm_addr & 0x3807f) | (m_i8748_P1 << 7);
			}
			if ((m_i8748_P2 & 0xc) == 4) /* Latch LSB Address */
			{
///             logerror("Latching LSB\n");
				m_adpcm_addr = (m_adpcm_addr & 0x3ff80) | (m_i8748_P1 >> 1);
				m_pcm_shift = (m_i8748_P1 & 1) * 4;
			}
		}

		m_adpcm_addr = ((m_adpcm_addr & 0x07fff) | (data & 0x70 << 11));

		m_pcm_nibble = saiyugoub1_adpcm_rom[m_adpcm_addr & 0x3ffff];

		m_pcm_nibble = (m_pcm_nibble >> m_pcm_shift) & 0x0f;

///     logerror("Writing %02x to m5205. $ROM=%08x  P1=%02x  P2=%02x  Prev_P2=%02x  Nibble=%08x\n", m_pcm_nibble, m_adpcm_addr, m_i8748_P1, data, m_i8748_P2, m_pcm_shift);

		if (((m_i8748_P2 & 0xc) >= 8) && ((data & 0xc) == 4))
		{
			m_adpcm->write_data(m_pcm_nibble);
			logerror("Writing %02x to m5205\n", m_pcm_nibble);
		}
		logerror("$ROM=%08x  P1=%02x  P2=%02x  Prev_P2=%02x  Nibble=%1x  PCM_data=%02x\n", m_adpcm_addr, m_i8748_P1, data, m_i8748_P2, m_pcm_shift, m_pcm_nibble);
	}
	m_i8748_P2 = data;
}

WRITE8_MEMBER(chinagat_state::saiyugoub1_m5205_clk_w )
{
	/* i8748 T0 output clk mode */
	/* This signal goes through a divide by 8 counter */
	/* to the xtal pins of the MSM5205 */

	/* Actually, T0 output clk mode is not supported by the i8048 core */
#if 0
	m_m5205_clk++;
	if (m_m5205_clk == 8)
	{
		m_adpcm->vclk_w(1);      /* ??? */
		m_m5205_clk = 0;
	}
	else
		m_adpcm->vclk_w(0);      /* ??? */
#endif
}

READ_LINE_MEMBER(chinagat_state::saiyugoub1_m5205_irq_r )
{
	if (m_adpcm_sound_irq)
	{
		m_adpcm_sound_irq = 0;
		return 1;
	}
	return 0;
}

WRITE_LINE_MEMBER(chinagat_state::saiyugoub1_m5205_irq_w)
{
	m_adpcm_sound_irq = 1;
}

void chinagat_state::main_map(address_map &map)
{
	map(0x0000, 0x1fff).ram().share("share1");
	map(0x2000, 0x27ff).ram().w(FUNC(chinagat_state::ddragon_fgvideoram_w)).share("fgvideoram");
	map(0x2800, 0x2fff).ram().w(FUNC(chinagat_state::ddragon_bgvideoram_w)).share("bgvideoram");
	map(0x3000, 0x317f).w(m_palette, FUNC(palette_device::write8)).share("palette");
	map(0x3400, 0x357f).w(m_palette, FUNC(palette_device::write8_ext)).share("palette_ext");
	map(0x3800, 0x397f).bankw("bank3").share("spriteram");
	map(0x3e00, 0x3e04).w(FUNC(chinagat_state::interrupt_w));
	map(0x3e06, 0x3e06).writeonly().share("scrolly_lo");
	map(0x3e07, 0x3e07).writeonly().share("scrollx_lo");
	map(0x3f00, 0x3f00).w(FUNC(chinagat_state::video_ctrl_w));
	map(0x3f01, 0x3f01).w(FUNC(chinagat_state::bankswitch_w));
	map(0x3f00, 0x3f00).portr("SYSTEM");
	map(0x3f01, 0x3f01).portr("DSW1");
	map(0x3f02, 0x3f02).portr("DSW2");
	map(0x3f03, 0x3f03).portr("P1");
	map(0x3f04, 0x3f04).portr("P2");
	map(0x4000, 0x7fff).bankr("bank1");
	map(0x8000, 0xffff).rom();
}

void chinagat_state::sub_map(address_map &map)
{
	map(0x0000, 0x1fff).ram().share("share1");
	map(0x2000, 0x2000).w(FUNC(chinagat_state::sub_bankswitch_w));
	map(0x2800, 0x2800).w(FUNC(chinagat_state::sub_irq_ack_w)); /* Called on CPU start and after return from jump table */
//  AM_RANGE(0x2a2b, 0x2a2b) AM_READNOP /* What lives here? */
//  AM_RANGE(0x2a30, 0x2a30) AM_READNOP /* What lives here? */
	map(0x4000, 0x7fff).bankr("bank4");
	map(0x8000, 0xffff).rom();
}

void chinagat_state::sound_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x87ff).ram();
	map(0x8800, 0x8801).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0x9800, 0x9800).rw("oki", FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0xA000, 0xA000).r(m_soundlatch, FUNC(generic_latch_8_device::read));
}

void chinagat_state::ym2203c_sound_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x87ff).ram();
// 8804 and/or 8805 make a gong sound when the coin goes in
// but only on the title screen....

	map(0x8800, 0x8801).rw("ym1", FUNC(ym2203_device::read), FUNC(ym2203_device::write));
//  AM_RANGE(0x8802, 0x8802) AM_DEVREADWRITE("oki", okim6295_device, read, write)
//  AM_RANGE(0x8803, 0x8803) AM_DEVWRITE("oki", okim6295_device, write)
	map(0x8804, 0x8805).rw("ym2", FUNC(ym2203_device::read), FUNC(ym2203_device::write));
//  AM_RANGE(0x8804, 0x8804) AM_WRITEONLY
//  AM_RANGE(0x8805, 0x8805) AM_WRITEONLY

//  AM_RANGE(0x8800, 0x8801) AM_DEVREADWRITE("ymsnd", ym2151_device, read, write)
//  AM_RANGE(0x9800, 0x9800) AM_DEVREADWRITE("oki", okim6295_device, read, write)
	map(0xA000, 0xA000).r(m_soundlatch, FUNC(generic_latch_8_device::read));
}

void chinagat_state::saiyugoub1_sound_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x87ff).ram();
	map(0x8800, 0x8801).rw("ymsnd", FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0x9800, 0x9800).w(FUNC(chinagat_state::saiyugoub1_mcu_command_w));
	map(0xA000, 0xA000).r(m_soundlatch, FUNC(generic_latch_8_device::read));
}

void chinagat_state::i8748_map(address_map &map)
{
	map(0x0000, 0x03ff).rom();
	map(0x0400, 0x07ff).rom();     /* i8749 version */
}



static INPUT_PORTS_START( chinagat )
	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_VBLANK("screen")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:4,5,6")
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	/*PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )*/
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x01, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x03, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )   // "SW2:4" - Left empty in the manual
	PORT_DIPNAME( 0x30, 0x20, "Timer" ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x00, "50" )
	PORT_DIPSETTING(    0x20, "55" )
	PORT_DIPSETTING(    0x30, "60" )
	PORT_DIPSETTING(    0x10, "70" )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:7,8")
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0xc0, "2" )
	PORT_DIPSETTING(    0x80, "3" )
	PORT_DIPSETTING(    0x40, "4" )

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,            /* 8*8 chars */
	RGN_FRAC(1,1),  /* num of characters */
	4,              /* 4 bits per pixel */
	{ 0, 2, 4, 6 },     /* plane offset */
	{ 1, 0, 65, 64, 129, 128, 193, 192 },
	{ STEP8(0,8) },         /* { 0*8, 1*8 ... 6*8, 7*8 }, */
	32*8 /* every char takes 32 consecutive bytes */
};

static const gfx_layout tilelayout =
{
	16,16,          /* 16x16 chars */
	RGN_FRAC(1,2),  /* num of Tiles/Sprites */
	4,              /* 4 bits per pixel */
	{ RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4, 0,4 }, /* plane offset */
	{ 3, 2, 1, 0, 16*8+3, 16*8+2, 16*8+1, 16*8+0,
		32*8+3,32*8+2 ,32*8+1 ,32*8+0 ,48*8+3 ,48*8+2 ,48*8+1 ,48*8+0 },
	{ STEP16(0,8) },        /* { 0*8, 1*8 ... 14*8, 15*8 }, */
	64*8 /* every char takes 64 consecutive bytes */
};

static GFXDECODE_START( gfx_chinagat )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,   0,16 )    /*  8x8  chars */
	GFXDECODE_ENTRY( "gfx2", 0, tilelayout, 128, 8 )    /* 16x16 sprites */
	GFXDECODE_ENTRY( "gfx3", 0, tilelayout, 256, 8 )    /* 16x16 background tiles */
GFXDECODE_END


void chinagat_state::machine_start()
{
	ddragon_state::machine_start();

	/* configure banks */
	membank("bank1")->configure_entries(0, 8, memregion("maincpu")->base() + 0x10000, 0x4000);

	/* register for save states */
	save_item(NAME(m_scrollx_hi));
	save_item(NAME(m_scrolly_hi));
	save_item(NAME(m_adpcm_sound_irq));
	save_item(NAME(m_adpcm_addr));
	save_item(NAME(m_pcm_shift));
	save_item(NAME(m_pcm_nibble));
	save_item(NAME(m_i8748_P1));
	save_item(NAME(m_i8748_P2));
	save_item(NAME(m_mcu_command));
#if 0
	save_item(NAME(m_m5205_clk));
#endif
}


void chinagat_state::machine_reset()
{
	ddragon_state::machine_reset();

	m_scrollx_hi = 0;
	m_scrolly_hi = 0;
	m_adpcm_sound_irq = 0;
	m_adpcm_addr = 0;
	m_pcm_shift = 0;
	m_pcm_nibble = 0;
	m_i8748_P1 = 0;
	m_i8748_P2 = 0;
	m_mcu_command = 0;
#if 0
	m_m5205_clk = 0;
#endif
}


void chinagat_state::chinagat(machine_config &config)
{
	/* basic machine hardware */
	HD6309(config, m_maincpu, MAIN_CLOCK / 2);      /* 1.5 MHz (12MHz oscillator / 4 internally) */
	m_maincpu->set_addrmap(AS_PROGRAM, &chinagat_state::main_map);
	TIMER(config, "scantimer").configure_scanline(FUNC(chinagat_state::chinagat_scanline), "screen", 0, 1);

	HD6309(config, m_subcpu, MAIN_CLOCK / 2);       /* 1.5 MHz (12MHz oscillator / 4 internally) */
	m_subcpu->set_addrmap(AS_PROGRAM, &chinagat_state::sub_map);

	Z80(config, m_soundcpu, XTAL(3'579'545));     /* 3.579545 MHz */
	m_soundcpu->set_addrmap(AS_PROGRAM, &chinagat_state::sound_map);

	config.m_minimum_quantum = attotime::from_hz(6000); /* heavy interleaving to sync up sprite<->main cpu's */

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_raw(PIXEL_CLOCK, 384, 0, 256, 272, 0, 240);   /* based on ddragon driver */
	m_screen->set_screen_update(FUNC(chinagat_state::screen_update_ddragon));
	m_screen->set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_chinagat);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_444, 384);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, m_soundlatch);
	m_soundlatch->data_pending_callback().set_inputline(m_soundcpu, INPUT_LINE_NMI);

	ym2151_device &ymsnd(YM2151(config, "ymsnd", 3579545));
	ymsnd.irq_handler().set_inputline(m_soundcpu, 0);
	ymsnd.add_route(0, "mono", 0.80);
	ymsnd.add_route(1, "mono", 0.80);

	okim6295_device &oki(OKIM6295(config, "oki", 1065000, okim6295_device::PIN7_HIGH)); // pin 7 not verified, clock frequency estimated with recording
	oki.add_route(ALL_OUTPUTS, "mono", 0.80);
}

void chinagat_state::saiyugoub1(machine_config &config)
{
	/* basic machine hardware */
	MC6809E(config, m_maincpu, MAIN_CLOCK / 8);     /* 68B09EP 1.5 MHz (12MHz oscillator) */
	m_maincpu->set_addrmap(AS_PROGRAM, &chinagat_state::main_map);
	TIMER(config, "scantimer").configure_scanline(FUNC(chinagat_state::chinagat_scanline), "screen", 0, 1);

	MC6809E(config, m_subcpu, MAIN_CLOCK / 8);      /* 68B09EP 1.5 MHz (12MHz oscillator) */
	m_subcpu->set_addrmap(AS_PROGRAM, &chinagat_state::sub_map);

	Z80(config, m_soundcpu, XTAL(3'579'545));       /* 3.579545 MHz oscillator */
	m_soundcpu->set_addrmap(AS_PROGRAM, &chinagat_state::saiyugoub1_sound_map);

	i8748_device &mcu(I8748(config, "mcu", 9263750));     /* 9.263750 MHz oscillator, divided by 3*5 internally */
	mcu.set_addrmap(AS_PROGRAM, &chinagat_state::i8748_map);
	mcu.bus_in_cb().set(FUNC(chinagat_state::saiyugoub1_mcu_command_r));
	//MCFG_MCS48_PORT_T0_CLK_CUSTOM(chinagat_state, saiyugoub1_m5205_clk_w)      /* Drives the clock on the m5205 at 1/8 of this frequency */
	mcu.t1_in_cb().set(FUNC(chinagat_state::saiyugoub1_m5205_irq_r));
	mcu.p1_out_cb().set(FUNC(chinagat_state::saiyugoub1_adpcm_rom_addr_w));
	mcu.p2_out_cb().set(FUNC(chinagat_state::saiyugoub1_adpcm_control_w));

	config.m_minimum_quantum = attotime::from_hz(6000);  /* heavy interleaving to sync up sprite<->main cpu's */

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_raw(PIXEL_CLOCK, 384, 0, 256, 272, 0, 240);   /* based on ddragon driver */
	m_screen->set_screen_update(FUNC(chinagat_state::screen_update_ddragon));
	m_screen->set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_chinagat);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_444, 384);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, m_soundlatch);
	m_soundlatch->data_pending_callback().set_inputline(m_soundcpu, INPUT_LINE_NMI);

	ym2151_device &ymsnd(YM2151(config, "ymsnd", 3579545));
	ymsnd.irq_handler().set_inputline(m_soundcpu, 0);
	ymsnd.add_route(0, "mono", 0.80);
	ymsnd.add_route(1, "mono", 0.80);

	MSM5205(config, m_adpcm, 9263750 / 24);
	m_adpcm->vck_legacy_callback().set(FUNC(chinagat_state::saiyugoub1_m5205_irq_w)); /* Interrupt function */
	m_adpcm->set_prescaler_selector(msm5205_device::S64_4B);    /* vclk input mode (6030Hz, 4-bit) */
	m_adpcm->add_route(ALL_OUTPUTS, "mono", 0.60);
}

void chinagat_state::saiyugoub2(machine_config &config)
{
	/* basic machine hardware */
	MC6809E(config, m_maincpu, MAIN_CLOCK / 8);     /* 1.5 MHz (12MHz oscillator) */
	m_maincpu->set_addrmap(AS_PROGRAM, &chinagat_state::main_map);
	TIMER(config, "scantimer").configure_scanline(FUNC(chinagat_state::chinagat_scanline), "screen", 0, 1);

	MC6809E(config, m_subcpu, MAIN_CLOCK / 8);      /* 1.5 MHz (12MHz oscillator) */
	m_subcpu->set_addrmap(AS_PROGRAM, &chinagat_state::sub_map);

	Z80(config, m_soundcpu, XTAL(3'579'545));       /* 3.579545 MHz oscillator */
	m_soundcpu->set_addrmap(AS_PROGRAM, &chinagat_state::ym2203c_sound_map);

	config.m_minimum_quantum = attotime::from_hz(6000); /* heavy interleaving to sync up sprite<->main cpu's */

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_raw(PIXEL_CLOCK, 384, 0, 256, 272, 0, 240);   /* based on ddragon driver */
	m_screen->set_screen_update(FUNC(chinagat_state::screen_update_ddragon));
	m_screen->set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_chinagat);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_444, 384);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, m_soundlatch);
	m_soundlatch->data_pending_callback().set_inputline(m_soundcpu, INPUT_LINE_NMI);

	ym2203_device &ym1(YM2203(config, "ym1", 3579545));
	ym1.irq_handler().set_inputline(m_soundcpu, 0);
	ym1.add_route(0, "mono", 0.50);
	ym1.add_route(1, "mono", 0.50);
	ym1.add_route(2, "mono", 0.50);
	ym1.add_route(3, "mono", 0.80);

	ym2203_device &ym2(YM2203(config, "ym2", 3579545));
	ym2.add_route(0, "mono", 0.50);
	ym2.add_route(1, "mono", 0.50);
	ym2.add_route(2, "mono", 0.50);
	ym2.add_route(3, "mono", 0.80);
}


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( chinagat )
	ROM_REGION( 0x28000, "maincpu", 0 ) /* Main CPU: 128KB for code (bankswitched using $3F01) */
	ROM_LOAD( "cgate51.bin", 0x10000, 0x18000, CRC(439a3b19) SHA1(01393b4302ac7a66390270b01e2757582240f6b8) )   /* Banks 0x4000 long @ 0x4000 */
	ROM_CONTINUE(            0x08000, 0x08000 )             /* Static code */

	ROM_REGION( 0x28000, "sub", 0 ) /* Slave CPU: 128KB for code (bankswitched using $2000) */
	ROM_LOAD( "23j4-0.48",   0x10000, 0x18000, CRC(2914af38) SHA1(3d690fa50b7d36a22de82c026d59a16126a7b73c) ) /* Banks 0x4000 long @ 0x4000 */
	ROM_CONTINUE(            0x08000, 0x08000 )             /* Static code */

	ROM_REGION( 0x10000, "soundcpu", 0 )    /* Music CPU, 64KB */
	ROM_LOAD( "23j0-0.40",   0x00000, 0x08000, CRC(9ffcadb6) SHA1(606dbdd73aee3cabb2142200ac6f8c96169e4b19) )

	ROM_REGION(0x20000, "gfx1", 0 ) /* Text */
	ROM_LOAD( "cgate18.bin", 0x00000, 0x20000, CRC(8d88d64d) SHA1(57265138ebb0c6419542cce5953aee7335bfa2bd) )   /* 0,1,2,3 */

	ROM_REGION(0x80000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "23j7-0.103",  0x00000, 0x20000, CRC(2f445030) SHA1(3fcf32097e655e963d952d01a30396dc195269ca) )   /* 2,3 */
	ROM_LOAD( "23j8-0.102",  0x20000, 0x20000, CRC(237f725a) SHA1(47bebe5b9878ca10fe6efd4f353717e53a372416) )   /* 2,3 */
	ROM_LOAD( "23j9-0.101",  0x40000, 0x20000, CRC(8caf6097) SHA1(50ad192f831b055586a4a9974f8c6c2f2063ede5) )   /* 0,1 */
	ROM_LOAD( "23ja-0.100",  0x60000, 0x20000, CRC(f678594f) SHA1(4bdcf9407543925f4630a8c7f1f48b85f76343a9) )   /* 0,1 */

	ROM_REGION(0x40000, "gfx3", 0 ) /* Background */
	ROM_LOAD( "chinagat_a-13", 0x00000, 0x10000, BAD_DUMP CRC(b745cac4) SHA1(759767ca7c5123b03b9e1a42bb105d194cb76400) ) // not dumped yet, these were taken from the bootleg set instead
	ROM_LOAD( "chinagat_a-12", 0x10000, 0x10000, BAD_DUMP CRC(3c864299) SHA1(cb12616e4d6c53a82beb4cd51510a632894b359c) ) // Where are these on the real board?
	ROM_LOAD( "chinagat_a-15", 0x20000, 0x10000, BAD_DUMP CRC(2f268f37) SHA1(f82cfe3b2001d5ed2a709ca9c51febcf624bb627) )
	ROM_LOAD( "chinagat_a-14", 0x30000, 0x10000, BAD_DUMP CRC(aef814c8) SHA1(f6b9229ca7beb9a0e47d1f6a1083c6102fdd20c8) )

	ROM_REGION(0x40000, "oki", 0 )  /* ADPCM */
	ROM_LOAD( "23j1-0.53", 0x00000, 0x20000, CRC(f91f1001) SHA1(378402a3c966cabd61e9662ae5decd66672a228b) )
	ROM_LOAD( "23j2-0.52", 0x20000, 0x20000, CRC(8b6f26e9) SHA1(7da26ae846814b3957b19c38b6bf7e83617dc6cc) )

	ROM_REGION(0x300, "user1", 0 )  /* Unknown Bipolar PROMs */
	ROM_LOAD( "23jb-0.16", 0x000, 0x200, CRC(46339529) SHA1(64f4c42a826d67b7cbaa8a23a45ebc4eb6248891) ) /* 82S131 on video board */
	ROM_LOAD( "23j5-0.45", 0x200, 0x100, CRC(fdb130a9) SHA1(4c4f214229b9fab2b5d69c745ec5428787b89e1f) ) /* 82S129 on main board */
ROM_END


ROM_START( saiyugou )
	ROM_REGION( 0x28000, "maincpu", 0 ) /* Main CPU: 128KB for code (bankswitched using $3F01) */
	ROM_LOAD( "23j3-0.51",  0x10000, 0x18000, CRC(aa8132a2) SHA1(87c3bd447767f263113c4865afc905a0e484a625) )    /* Banks 0x4000 long @ 0x4000 */
	ROM_CONTINUE(           0x08000, 0x08000)               /* Static code */

	ROM_REGION( 0x28000, "sub", 0 ) /* Slave CPU: 128KB for code (bankswitched using $2000) */
	ROM_LOAD( "23j4-0.48",  0x10000, 0x18000, CRC(2914af38) SHA1(3d690fa50b7d36a22de82c026d59a16126a7b73c) )    /* Banks 0x4000 long @ 0x4000 */
	ROM_CONTINUE(           0x08000, 0x08000)               /* Static code */

	ROM_REGION( 0x10000, "soundcpu", 0 )    /* Music CPU, 64KB */
	ROM_LOAD( "23j0-0.40",  0x00000, 0x8000, CRC(9ffcadb6) SHA1(606dbdd73aee3cabb2142200ac6f8c96169e4b19) )

	ROM_REGION(0x20000, "gfx1", 0 ) /* Text */
	ROM_LOAD( "23j6-0.18",  0x00000, 0x20000, CRC(86d33df0) SHA1(3419959c28703c5177de9c11b61e1dba9e76aca5) )    /* 0,1,2,3 */

	ROM_REGION(0x80000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "23j7-0.103", 0x00000, 0x20000, CRC(2f445030) SHA1(3fcf32097e655e963d952d01a30396dc195269ca) )    /* 2,3 */
	ROM_LOAD( "23j8-0.102", 0x20000, 0x20000, CRC(237f725a) SHA1(47bebe5b9878ca10fe6efd4f353717e53a372416) )    /* 2,3 */
	ROM_LOAD( "23j9-0.101", 0x40000, 0x20000, CRC(8caf6097) SHA1(50ad192f831b055586a4a9974f8c6c2f2063ede5) )    /* 0,1 */
	ROM_LOAD( "23ja-0.100", 0x60000, 0x20000, CRC(f678594f) SHA1(4bdcf9407543925f4630a8c7f1f48b85f76343a9) )    /* 0,1 */

	ROM_REGION(0x40000, "gfx3", 0 ) /* Background */
	ROM_LOAD( "saiyugou_a-13", 0x00000, 0x10000, BAD_DUMP CRC(b745cac4) SHA1(759767ca7c5123b03b9e1a42bb105d194cb76400) ) // not dumped yet, these were taken from the bootleg set instead
	ROM_LOAD( "saiyugou_a-12", 0x10000, 0x10000, BAD_DUMP CRC(3c864299) SHA1(cb12616e4d6c53a82beb4cd51510a632894b359c) ) // Where are these on the real board?
	ROM_LOAD( "saiyugou_a-15", 0x20000, 0x10000, BAD_DUMP CRC(2f268f37) SHA1(f82cfe3b2001d5ed2a709ca9c51febcf624bb627) )
	ROM_LOAD( "saiyugou_a-14", 0x30000, 0x10000, BAD_DUMP CRC(aef814c8) SHA1(f6b9229ca7beb9a0e47d1f6a1083c6102fdd20c8) )

	ROM_REGION(0x40000, "oki", 0 )  /* ADPCM */
	ROM_LOAD( "23j1-0.53", 0x00000, 0x20000, CRC(f91f1001) SHA1(378402a3c966cabd61e9662ae5decd66672a228b) )
	ROM_LOAD( "23j2-0.52", 0x20000, 0x20000, CRC(8b6f26e9) SHA1(7da26ae846814b3957b19c38b6bf7e83617dc6cc) )

	ROM_REGION(0x300, "user1", 0 )  /* Unknown Bipolar PROMs */
	ROM_LOAD( "23jb-0.16", 0x000, 0x200, CRC(46339529) SHA1(64f4c42a826d67b7cbaa8a23a45ebc4eb6248891) ) /* 82S131 on video board */
	ROM_LOAD( "23j5-0.45", 0x200, 0x100, CRC(fdb130a9) SHA1(4c4f214229b9fab2b5d69c745ec5428787b89e1f) ) /* 82S129 on main board */
ROM_END

ROM_START( saiyugoub1 )
	ROM_REGION( 0x28000, "maincpu", 0 ) /* Main CPU: 128KB for code (bankswitched using $3F01) */
	ROM_LOAD( "23j3-0.51",  0x10000, 0x18000, CRC(aa8132a2) SHA1(87c3bd447767f263113c4865afc905a0e484a625) )    /* Banks 0x4000 long @ 0x4000 */
	/* Orientation of bootleg ROMs which are split, but otherwise the same.
	   ROM_LOAD( "a-5.bin", 0x10000, 0x10000, CRC(39795aa5) )      Banks 0x4000 long @ 0x4000
	   ROM_LOAD( "a-9.bin", 0x20000, 0x08000, CRC(051ebe92) )      Banks 0x4000 long @ 0x4000
	*/
	ROM_CONTINUE(           0x08000, 0x08000 )              /* Static code */

	ROM_REGION( 0x28000, "sub", 0 ) /* Slave CPU: 128KB for code (bankswitched using $2000) */
	ROM_LOAD( "23j4-0.48",  0x10000, 0x18000, CRC(2914af38) SHA1(3d690fa50b7d36a22de82c026d59a16126a7b73c) )    /* Banks 0x4000 long @ 0x4000 */
	/* Orientation of bootleg ROMs which are split, but otherwise the same.
	   ROM_LOAD( "a-4.bin", 0x10000, 0x10000, CRC(9effddc1) )      Banks 0x4000 long @ 0x4000
	   ROM_LOAD( "a-8.bin", 0x20000, 0x08000, CRC(a436edb8) )      Banks 0x4000 long @ 0x4000
	*/
	ROM_CONTINUE(           0x08000, 0x08000 )              /* Static code */

	ROM_REGION( 0x10000, "soundcpu", 0 )    /* Music CPU, 64KB */
	ROM_LOAD( "a-1.bin",  0x00000, 0x8000,  CRC(46e5a6d4) SHA1(965ed7bdb727ab32ce3322ca49f1a4e3786e8051) )

	ROM_REGION( 0x800, "mcu", 0 )       /* ADPCM CPU, 1KB */
	ROM_LOAD( "mcu8748.bin", 0x000, 0x400, CRC(6d28d6c5) SHA1(20582c62a72545e68c2e155b063ee7e95e1228ce) )

	ROM_REGION(0x20000, "gfx1", 0 ) /* Text */
	ROM_LOAD( "23j6-0.18",  0x00000, 0x20000, CRC(86d33df0) SHA1(3419959c28703c5177de9c11b61e1dba9e76aca5) )    /* 0,1,2,3 */
	/* Orientation of bootleg ROMs which are split, but otherwise the same.
	   ROM_LOAD( "a-2.bin", 0x00000, 0x10000, CRC(baa5a3b9) )      0,1
	   ROM_LOAD( "a-3.bin", 0x10000, 0x10000, CRC(532d59be) )      2,3
	*/

	ROM_REGION(0x80000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "23j7-0.103",  0x00000, 0x20000, CRC(2f445030) SHA1(3fcf32097e655e963d952d01a30396dc195269ca) )   /* 2,3 */
	ROM_LOAD( "23j8-0.102",  0x20000, 0x20000, CRC(237f725a) SHA1(47bebe5b9878ca10fe6efd4f353717e53a372416) )   /* 2,3 */
	ROM_LOAD( "23j9-0.101",  0x40000, 0x20000, CRC(8caf6097) SHA1(50ad192f831b055586a4a9974f8c6c2f2063ede5) )   /* 0,1 */
	ROM_LOAD( "23ja-0.100",  0x60000, 0x20000, CRC(f678594f) SHA1(4bdcf9407543925f4630a8c7f1f48b85f76343a9) )   /* 0,1 */
	/* Orientation of bootleg ROMs which are split, but otherwise the same
	   ROM_LOAD( "a-23.bin", 0x00000, 0x10000, CRC(12b56225) )     2,3
	   ROM_LOAD( "a-22.bin", 0x10000, 0x10000, CRC(b592aa9b) )     2,3
	   ROM_LOAD( "a-21.bin", 0x20000, 0x10000, CRC(a331ba3d) )     2,3
	   ROM_LOAD( "a-20.bin", 0x30000, 0x10000, CRC(2515d742) )     2,3
	   ROM_LOAD( "a-19.bin", 0x40000, 0x10000, CRC(d796f2e4) )     0,1
	   ROM_LOAD( "a-18.bin", 0x50000, 0x10000, CRC(c9e1c2f9) )     0,1
	   ROM_LOAD( "a-17.bin", 0x60000, 0x10000, CRC(00b6db0a) )     0,1
	   ROM_LOAD( "a-16.bin", 0x70000, 0x10000, CRC(f196818b) )     0,1
	*/

	ROM_REGION(0x40000, "gfx3", 0 ) /* Background */
	ROM_LOAD( "a-13", 0x00000, 0x10000, CRC(b745cac4) SHA1(759767ca7c5123b03b9e1a42bb105d194cb76400) )
	ROM_LOAD( "a-12", 0x10000, 0x10000, CRC(3c864299) SHA1(cb12616e4d6c53a82beb4cd51510a632894b359c) )
	ROM_LOAD( "a-15", 0x20000, 0x10000, CRC(2f268f37) SHA1(f82cfe3b2001d5ed2a709ca9c51febcf624bb627) )
	ROM_LOAD( "a-14", 0x30000, 0x10000, CRC(aef814c8) SHA1(f6b9229ca7beb9a0e47d1f6a1083c6102fdd20c8) )

	/* Some bootlegs have incorrectly halved the ADPCM data ! */
	/* These are same as the 128k sample except nibble-swapped */
	ROM_REGION(0x40000, "adpcm", 0 )    /* ADPCM */     /* Bootleggers wrong data */
	ROM_LOAD ( "a-6.bin",   0x00000, 0x10000, CRC(4da4e935) SHA1(235a1589165a23cfad29e07cf66d7c3a777fc904) )    /* 0x8000, 0x7cd47f01 */
	ROM_LOAD ( "a-7.bin",   0x10000, 0x10000, CRC(6284c254) SHA1(e01be1bd4768ae0ccb1cec65b3a6bc80ed7a4b00) )    /* 0x8000, 0x7091959c */
	ROM_LOAD ( "a-10.bin",  0x20000, 0x10000, CRC(b728ec6e) SHA1(433b5f907e4918e89b79bd927e2993ad3030017b) )    /* 0x8000, 0x78349cb6 */
	ROM_LOAD ( "a-11.bin",  0x30000, 0x10000, CRC(a50d1895) SHA1(0c2c1f8a2e945d6c53ce43413f0e63ced45bae17) )    /* 0x8000, 0xaa5b6834 */

	ROM_REGION(0x300, "user1", 0 )  /* Unknown Bipolar PROMs */
	ROM_LOAD( "23jb-0.16", 0x000, 0x200, CRC(46339529) SHA1(64f4c42a826d67b7cbaa8a23a45ebc4eb6248891) ) /* 82S131 on video board */
	ROM_LOAD( "23j5-0.45", 0x200, 0x100, CRC(fdb130a9) SHA1(4c4f214229b9fab2b5d69c745ec5428787b89e1f) ) /* 82S129 on main board */
ROM_END

ROM_START( saiyugoub2 )
	ROM_REGION( 0x28000, "maincpu", 0 ) /* Main CPU: 128KB for code (bankswitched using $3F01) */
	ROM_LOAD( "23j3-0.51",   0x10000, 0x18000, CRC(aa8132a2) SHA1(87c3bd447767f263113c4865afc905a0e484a625) )   /* Banks 0x4000 long @ 0x4000 */
	/* Orientation of bootleg ROMs which are split, but otherwise the same.
	   ROM_LOAD( "sai5.bin", 0x10000, 0x10000, CRC(39795aa5) )     Banks 0x4000 long @ 0x4000
	   ROM_LOAD( "sai9.bin", 0x20000, 0x08000, CRC(051ebe92) )     Banks 0x4000 long @ 0x4000
	*/
	ROM_CONTINUE(            0x08000, 0x08000 )             /* Static code */

	ROM_REGION( 0x28000, "sub", 0 ) /* Slave CPU: 128KB for code (bankswitched using $2000) */
	ROM_LOAD( "23j4-0.48", 0x10000, 0x18000, CRC(2914af38) SHA1(3d690fa50b7d36a22de82c026d59a16126a7b73c) ) /* Banks 0x4000 long @ 0x4000 */
	/* Orientation of bootleg ROMs which are split, but otherwise the same.
	   ROM_LOAD( "sai4.bin", 0x10000, 0x10000, CRC(9effddc1) )     Banks 0x4000 long @ 0x4000
	   ROM_LOAD( "sai8.bin", 0x20000, 0x08000, CRC(a436edb8) )     Banks 0x4000 long @ 0x4000
	*/
	ROM_CONTINUE(         0x08000, 0x08000 )                /* Static code */

	ROM_REGION( 0x10000, "soundcpu", 0 )    /* Music CPU, 64KB */
	ROM_LOAD( "sai-alt1.bin", 0x00000, 0x8000, CRC(8d397a8d) SHA1(52599521c3dbcecc1ae56bb80dc855e76d700134) )

//  ROM_REGION( 0x800, "cpu3", 0 )     /* ADPCM CPU, 1KB */
//  ROM_LOAD( "sgr-8749.bin", 0x000, 0x800, CRC(9237e8c5) ) /* same as above but padded with 00 for different mcu */

	ROM_REGION(0x20000, "gfx1", 0 ) /* Text */
	ROM_LOAD( "23j6-0.18", 0x00000, 0x20000, CRC(86d33df0) SHA1(3419959c28703c5177de9c11b61e1dba9e76aca5) ) /* 0,1,2,3 */
	/* Orientation of bootleg ROMs which are split, but otherwise the same.
	   ROM_LOAD( "sai2.bin", 0x00000, 0x10000, CRC(baa5a3b9) )     0,1
	   ROM_LOAD( "sai3.bin", 0x10000, 0x10000, CRC(532d59be) )     2,3
	*/

	ROM_REGION(0x80000, "gfx2", 0 ) /* Sprites */
	ROM_LOAD( "23j7-0.103",   0x00000, 0x20000, CRC(2f445030) SHA1(3fcf32097e655e963d952d01a30396dc195269ca) )  /* 2,3 */
	ROM_LOAD( "23j8-0.102",   0x20000, 0x20000, CRC(237f725a) SHA1(47bebe5b9878ca10fe6efd4f353717e53a372416) )  /* 2,3 */
	ROM_LOAD( "23j9-0.101",   0x40000, 0x20000, CRC(8caf6097) SHA1(50ad192f831b055586a4a9974f8c6c2f2063ede5) )  /* 0,1 */
	ROM_LOAD( "23ja-0.100",   0x60000, 0x20000, CRC(f678594f) SHA1(4bdcf9407543925f4630a8c7f1f48b85f76343a9) )  /* 0,1 */
	/* Orientation of bootleg ROMs which are split, but otherwise the same
	   ROM_LOAD( "sai23.bin", 0x00000, 0x10000, CRC(12b56225) )    2,3
	   ROM_LOAD( "sai22.bin", 0x10000, 0x10000, CRC(b592aa9b) )    2,3
	   ROM_LOAD( "sai21.bin", 0x20000, 0x10000, CRC(a331ba3d) )    2,3
	   ROM_LOAD( "sai20.bin", 0x30000, 0x10000, CRC(2515d742) )    2,3
	   ROM_LOAD( "sai19.bin", 0x40000, 0x10000, CRC(d796f2e4) )    0,1
	   ROM_LOAD( "sai18.bin", 0x50000, 0x10000, CRC(c9e1c2f9) )    0,1
	   ROM_LOAD( "roku17.bin",0x60000, 0x10000, CRC(00b6db0a) )    0,1
	   ROM_LOAD( "sai16.bin", 0x70000, 0x10000, CRC(f196818b) )    0,1
	*/

	ROM_REGION(0x40000, "gfx3", 0 ) /* Background */
	ROM_LOAD( "a-13", 0x00000, 0x10000, CRC(b745cac4) SHA1(759767ca7c5123b03b9e1a42bb105d194cb76400) )
	ROM_LOAD( "a-12", 0x10000, 0x10000, CRC(3c864299) SHA1(cb12616e4d6c53a82beb4cd51510a632894b359c) )
	ROM_LOAD( "a-15", 0x20000, 0x10000, CRC(2f268f37) SHA1(f82cfe3b2001d5ed2a709ca9c51febcf624bb627) )
	ROM_LOAD( "a-14", 0x30000, 0x10000, CRC(aef814c8) SHA1(f6b9229ca7beb9a0e47d1f6a1083c6102fdd20c8) )

	ROM_REGION(0x40000, "adpcm", 0 )    /* ADPCM */
	/* These are same as the 128k sample except nibble-swapped */
	/* Some bootlegs have incorrectly halved the ADPCM data !  Bootleggers wrong data */
	ROM_LOAD ( "a-6.bin",   0x00000, 0x10000, CRC(4da4e935) SHA1(235a1589165a23cfad29e07cf66d7c3a777fc904) )    /* 0x8000, 0x7cd47f01 */
	ROM_LOAD ( "a-7.bin",   0x10000, 0x10000, CRC(6284c254) SHA1(e01be1bd4768ae0ccb1cec65b3a6bc80ed7a4b00) )    /* 0x8000, 0x7091959c */
	ROM_LOAD ( "a-10.bin",  0x20000, 0x10000, CRC(b728ec6e) SHA1(433b5f907e4918e89b79bd927e2993ad3030017b) )    /* 0x8000, 0x78349cb6 */
	ROM_LOAD ( "a-11.bin",  0x30000, 0x10000, CRC(a50d1895) SHA1(0c2c1f8a2e945d6c53ce43413f0e63ced45bae17) )    /* 0x8000, 0xaa5b6834 */

	ROM_REGION(0x300, "user1", 0 )  /* Unknown Bipolar PROMs */
	ROM_LOAD( "23jb-0.16", 0x000, 0x200, CRC(46339529) SHA1(64f4c42a826d67b7cbaa8a23a45ebc4eb6248891) ) /* 82S131 on video board */
	ROM_LOAD( "23j5-0.45", 0x200, 0x100, CRC(fdb130a9) SHA1(4c4f214229b9fab2b5d69c745ec5428787b89e1f) ) /* 82S129 on main board */
ROM_END


void chinagat_state::init_chinagat()
{
	uint8_t *MAIN = memregion("maincpu")->base();
	uint8_t *SUB = memregion("sub")->base();

	m_technos_video_hw = 1;
	m_sprite_irq = M6809_IRQ_LINE;

	membank("bank1")->configure_entries(0, 6, &MAIN[0x10000], 0x4000);
	membank("bank4")->configure_entries(0, 6, &SUB[0x10000], 0x4000);
}


//  ( YEAR  NAME        PARENT    MACHINE     INPUT     STATE           INIT           MONITOR COMPANY    FULLNAME     FLAGS ) */
GAME( 1988, chinagat,   0,        chinagat,   chinagat, chinagat_state, init_chinagat, ROT0,   "Technos Japan (Taito / Romstar license)", "China Gate (US)", MACHINE_SUPPORTS_SAVE )
GAME( 1988, saiyugou,   chinagat, chinagat,   chinagat, chinagat_state, init_chinagat, ROT0,   "Technos Japan", "Sai Yu Gou Ma Roku (Japan)", MACHINE_SUPPORTS_SAVE )
GAME( 1988, saiyugoub1, chinagat, saiyugoub1, chinagat, chinagat_state, init_chinagat, ROT0,   "bootleg", "Sai Yu Gou Ma Roku (Japan bootleg 1)", MACHINE_IMPERFECT_SOUND | MACHINE_SUPPORTS_SAVE )
GAME( 1988, saiyugoub2, chinagat, saiyugoub2, chinagat, chinagat_state, init_chinagat, ROT0,   "bootleg", "Sai Yu Gou Ma Roku (Japan bootleg 2)", MACHINE_SUPPORTS_SAVE )
