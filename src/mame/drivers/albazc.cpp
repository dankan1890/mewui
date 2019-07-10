// license:BSD-3-Clause
// copyright-holders:David Haywood, Stephane Humbert
/* Hanaroku - Alba ZC HW */

/*
TODO:
- colour decoding might not be perfect
- Background color should be green, but current handling might be wrong.
- some unknown sprite attributes
- don't know what to do when the jackpot is displayed (missing controls ?)
- according to the board pic, there should be one more 4-switches dip
  switch bank, and probably some NVRAM because there's a battery.
*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/nvram.h"
#include "machine/ticket.h"
#include "sound/ay8910.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"

class albazc_state : public driver_device
{
public:
	albazc_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_spriteram1(*this, "spriteram1"),
		m_spriteram2(*this, "spriteram2"),
		m_spriteram3(*this, "spriteram3"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_hopper(*this, "hopper")
	{ }

	void hanaroku(machine_config &config);

private:
	/* video-related */
	DECLARE_WRITE8_MEMBER(hanaroku_out_0_w);
	DECLARE_WRITE8_MEMBER(hanaroku_out_1_w);
	DECLARE_WRITE8_MEMBER(hanaroku_out_2_w);
	DECLARE_WRITE8_MEMBER(albazc_vregs_w);
	virtual void video_start() override;
	void albazc_palette(palette_device &palette) const;
	uint32_t screen_update_hanaroku(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void hanaroku_map(address_map &map);

	required_shared_ptr<uint8_t> m_spriteram1;
	required_shared_ptr<uint8_t> m_spriteram2;
	required_shared_ptr<uint8_t> m_spriteram3;
	uint8_t m_flip_bit;
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<ticket_dispenser_device> m_hopper;
};



/* video */

void albazc_state::albazc_palette(palette_device &palette) const
{
	uint8_t const *const color_prom(memregion("proms")->base());
	for (int i = 0; i < 0x200; i++)
	{
		int const b = (color_prom[i * 2 + 1] & 0x1f);
		int const g = ((color_prom[i * 2 + 1] & 0xe0) | ((color_prom[i * 2 + 0] & 0x03) <<8)) >> 5;
		int const r = (color_prom[i * 2 + 0] & 0x7c) >> 2;

		palette.set_pen_color(i, pal5bit(r), pal5bit(g), pal5bit(b));
	}
}


void albazc_state::video_start()
{
}

void albazc_state::draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int i;

	for (i = 511; i >= 0; i--)
	{
		int code = m_spriteram1[i] | (m_spriteram2[i] << 8);
		int color = (m_spriteram2[i + 0x200] & 0xf8) >> 3;
		int flipx = 0;
		int flipy = 0;
		int sx = m_spriteram1[i + 0x200] | ((m_spriteram2[i + 0x200] & 0x07) << 8);
		int sy = 242 - m_spriteram3[i];

		if (m_flip_bit)
		{
			sy = 242 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		m_gfxdecode->gfx(0)->transpen(bitmap,cliprect, code, color, flipx, flipy,
			sx, sy, 0);
	}
}

uint32_t albazc_state::screen_update_hanaroku(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0x1f0, cliprect);   // ???
	draw_sprites(bitmap, cliprect);
	return 0;
}

WRITE8_MEMBER(albazc_state::hanaroku_out_0_w)
{
	/*
	    bit     description

	     0      meter1 (coin1)
	     1      meter2 (coin2)
	     2      meter3 (1/2 d-up)
	     3      meter4
	     4      call out (meter)
	     5      lockout (key)
	     6      hopper2 (play)
	     7      meter5 (start)
	*/

	machine().bookkeeping().coin_counter_w(0, data & 0x01);
	machine().bookkeeping().coin_counter_w(1, data & 0x02);
	machine().bookkeeping().coin_counter_w(2, data & 0x04);
	machine().bookkeeping().coin_counter_w(3, data & 0x08);
	machine().bookkeeping().coin_counter_w(4, data & 0x80);
}

WRITE8_MEMBER(albazc_state::hanaroku_out_1_w)
{
	/*
	    bit     description

	     0      hopper1 (data clear)
	     1      dis dat
	     2      dis clk
	     3      pay out
	     4      ext in 1
	     5      ext in 2
	     6      ?
	     7      ?
	*/

	m_hopper->motor_w(BIT(data, 0));
}

WRITE8_MEMBER(albazc_state::hanaroku_out_2_w)
{
	// unused
}

WRITE8_MEMBER(albazc_state::albazc_vregs_w)
{
	#ifdef UNUSED_FUNCTION
	{
		static uint8_t x[5];
		x[offset] = data;
		popmessage("%02x %02x %02x %02x %02x",x[0],x[1],x[2],x[3],x[4]);
	}
	#endif

	if(offset == 0)
	{
		/* core bug with this? */
		//flip_screen_set((data & 0x40) >> 6);
		m_flip_bit = (data & 0x40) >> 6;
	}
}

/* main cpu */

void albazc_state::hanaroku_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x87ff).ram().share("spriteram1");
	map(0x9000, 0x97ff).ram().share("spriteram2");
	map(0xa000, 0xa1ff).ram().share("spriteram3");
	map(0xa200, 0xa2ff).nopw();    // ??? written once during P.O.S.T.
	map(0xa300, 0xa304).w(FUNC(albazc_state::albazc_vregs_w));   // ???
	map(0xb000, 0xb000).nopw();    // ??? always 0x40
	map(0xc000, 0xc3ff).ram();         // main ram
	map(0xc400, 0xc4ff).ram().share("nvram");
	map(0xd000, 0xd000).r("aysnd", FUNC(ay8910_device::data_r));
	map(0xd000, 0xd001).w("aysnd", FUNC(ay8910_device::address_data_w));
	map(0xe000, 0xe000).portr("IN0").w(FUNC(albazc_state::hanaroku_out_0_w));
	map(0xe001, 0xe001).portr("IN1");
	map(0xe002, 0xe002).portr("IN2").w(FUNC(albazc_state::hanaroku_out_1_w));
	map(0xe004, 0xe004).portr("DSW3").w(FUNC(albazc_state::hanaroku_out_2_w));
}


static INPUT_PORTS_START( hanaroku )
	PORT_START("IN0")   /* 0xe000 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )      // adds n credits depending on "Coinage" Dip Switch
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )      // adds 5 credits
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_DOUBLE_UP ) PORT_NAME("1/2 D-Up")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 ) PORT_NAME("Reset")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE2 ) PORT_NAME("Meter")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE3 ) PORT_NAME("Key") PORT_TOGGLE
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 ) PORT_NAME("Play")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 ) PORT_NAME("Start")

	PORT_START("IN1")   /* 0xe001 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_HANAFUDA_A )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_HANAFUDA_B )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_HANAFUDA_C )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_HANAFUDA_D )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_HANAFUDA_E )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_HANAFUDA_F )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_HANAFUDA_YES )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_HANAFUDA_NO )

	PORT_START("IN2")   /* 0xe002 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MEMORY_RESET ) PORT_NAME("Data Clear")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_READ_LINE_DEVICE_MEMBER("hopper", ticket_dispenser_device, line_r) // "Medal In"
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_GAMBLE_PAYOUT )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Ext In 1")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Ext In 2")
	PORT_BIT( 0xc0, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW1")  /* 0xd000 - Port A */
	PORT_BIT(  0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW2")  /* 0xd000 - Port B */
	PORT_BIT(  0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW3")  /* 0xe004 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coinage ) )      // Stored at 0xc028
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x00, "1 Coin/10 Credits" )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Flip_Screen ) )  // Stored at 0xc03a
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )      // Stored at 0xc078
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x20, "Game Mode" )             // Stored at 0xc02e
	PORT_DIPSETTING(    0x30, "Mode 0" )                // Collect OFF
	PORT_DIPSETTING(    0x20, "Mode 1" )                // Collect ON (code at 0x36ea)
	PORT_DIPSETTING(    0x10, "Mode 2" )                // Collect ON (code at 0x3728)
	PORT_DIPSETTING(    0x00, "Mode 3" )                // No credit counter
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )
INPUT_PORTS_END


static const gfx_layout hanaroku_charlayout =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(3,4),RGN_FRAC(2,4),RGN_FRAC(1,4),RGN_FRAC(0,4) },
	{ 0,1,2,3,4,5,6,7,
		64,65,66,67,68,69,70,71},
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		128+0*8,128+1*8,128+2*8,128+3*8,128+4*8,128+5*8,128+6*8,128+7*8 },
	16*16
};



static GFXDECODE_START( gfx_hanaroku )
	GFXDECODE_ENTRY( "gfx1", 0, hanaroku_charlayout,   0, 32  )
GFXDECODE_END


void albazc_state::hanaroku(machine_config &config)
{
	Z80(config, m_maincpu, 6000000);         /* ? MHz */
	m_maincpu->set_addrmap(AS_PROGRAM, &albazc_state::hanaroku_map);
	m_maincpu->set_vblank_int("screen", FUNC(albazc_state::irq0_line_hold));

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);

	TICKET_DISPENSER(config, m_hopper, attotime::from_msec(50), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_HIGH );

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(64*8, 64*8);
	screen.set_visarea(0, 48*8-1, 2*8, 30*8-1);
	screen.set_screen_update(FUNC(albazc_state::screen_update_hanaroku));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_hanaroku);

	PALETTE(config, m_palette, FUNC(albazc_state::albazc_palette), 0x200);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	ay8910_device &aysnd(AY8910(config, "aysnd", 1500000)); /* ? MHz */
	aysnd.port_a_read_callback().set_ioport("DSW1");
	aysnd.port_b_read_callback().set_ioport("DSW2");
	aysnd.add_route(ALL_OUTPUTS, "mono", 0.50);
}


ROM_START( hanaroku )
	ROM_REGION( 0x10000, "maincpu", 0 ) /* z80 code */
	ROM_LOAD( "zc5_1a.u02",  0x00000, 0x08000, CRC(9e3b62ce) SHA1(81aee570b67950c21ab3c8f9235dd383529b34d5) )

	ROM_REGION( 0x20000, "gfx1", 0 ) /* tiles */
	ROM_LOAD( "zc0_002.u14",  0x00000, 0x08000, CRC(76adab7f) SHA1(6efbe52ae4a1d15fe93bd05058546bf146a64154) )
	ROM_LOAD( "zc0_003.u15",  0x08000, 0x08000, CRC(c208e64b) SHA1(0bc226c39331bb2e1d4d8f756199ceec85c28f28) )
	ROM_LOAD( "zc0_004.u16",  0x10000, 0x08000, CRC(e8a46ee4) SHA1(09cac230c1c49cb282f540b1608ad33b1cc1a943) )
	ROM_LOAD( "zc0_005.u17",  0x18000, 0x08000, CRC(7ad160a5) SHA1(c897fbe4a7c2a2f352333131dfd1a76e176f0ed8) )

	ROM_REGION( 0x0400, "proms", 0 ) /* colour */
	ROM_LOAD16_BYTE( "zc0_006.u21",  0x0000, 0x0200, CRC(8e8fbc30) SHA1(7075521bbd790c46c58d9e408b0d7d6a42ed00bc) )
	ROM_LOAD16_BYTE( "zc0_007.u22",  0x0001, 0x0200, CRC(67225de1) SHA1(98322e71d93d247a67fb4e52edad6c6c32a603d8) )
ROM_END


GAME( 1988, hanaroku, 0, hanaroku, hanaroku, albazc_state, empty_init, ROT0, "Alba", "Hanaroku", MACHINE_IMPERFECT_GRAPHICS | MACHINE_IMPERFECT_COLORS | MACHINE_SUPPORTS_SAVE )
