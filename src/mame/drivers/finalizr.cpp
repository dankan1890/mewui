// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/***************************************************************************

    Finalizer (GX523) (c) 1985 Konami

    driver by Nicola Salmoria

    2009-03:
    Added dsw locations and verified factory setting based on Guru's notes
    (actually for finalizr only, finalizrb locations assumed to be the same)

***************************************************************************/

#include "emu.h"
#include "includes/finalizr.h"
#include "includes/konamipt.h"

#include "cpu/m6809/m6809.h"
#include "machine/gen_latch.h"
#include "machine/konami1.h"
#include "machine/watchdog.h"
#include "sound/dac.h"
#include "sound/sn76496.h"
#include "sound/volt_reg.h"

#include "screen.h"
#include "speaker.h"


TIMER_DEVICE_CALLBACK_MEMBER(finalizr_state::finalizr_scanline)
{
	int scanline = param;

	if (scanline == 240 && m_irq_enable) // vblank irq
		m_maincpu->set_input_line(M6809_IRQ_LINE, HOLD_LINE);
	else if (((scanline % 32) == 0) && m_nmi_enable) // timer irq
		m_maincpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
}


WRITE8_MEMBER(finalizr_state::finalizr_videoctrl_w)
{
	m_charbank = data & 3;
	m_spriterambank = data & 8;
	/* other bits unknown */
}

WRITE8_MEMBER(finalizr_state::finalizr_coin_w)
{
	machine().bookkeeping().coin_counter_w(0, data & 0x01);
	machine().bookkeeping().coin_counter_w(1, data & 0x02);
}

WRITE8_MEMBER(finalizr_state::finalizr_flipscreen_w)
{
	m_nmi_enable = data & 0x01;
	m_irq_enable = data & 0x02;

	flip_screen_set(~data & 0x08);
}

WRITE8_MEMBER(finalizr_state::finalizr_i8039_irq_w)
{
	m_audiocpu->set_input_line(0, ASSERT_LINE);
}

WRITE8_MEMBER(finalizr_state::i8039_irqen_w)
{
	/*  bit 0x80 goes active low, indicating that the
	    external IRQ being serviced is complete
	    bit 0x40 goes active high to enable the DAC ?
	*/

	if ((data & 0x80) == 0)
		m_audiocpu->set_input_line(0, CLEAR_LINE);
}

READ_LINE_MEMBER(finalizr_state::i8039_t1_r)
{
	/*  I suspect the clock-out from the I8039 T0 line should be connected
	    here (See the i8039_t0_w handler below).
	    The frequency of this clock cannot be greater than I8039 CLKIN / 45
	    Accounting for the I8039 input clock, and internal/external divisors
	    the frequency here should be 192KHz (I8039 CLKIN / 48)

	    Here we apply a positive edge every 3.2 reads, to simulate 192KHz
	    based on the I8039 main xtal clock input frequency of 9.216MHz
	*/

	m_T1_line++;
	m_T1_line %= 16;
	return (!(m_T1_line % 3) && (m_T1_line > 0));
}

WRITE8_MEMBER(finalizr_state::i8039_t0_w)
{
	/*  This becomes a clock output at a frequency of 3.072MHz (derived
	    by internally dividing the main xtal clock input by a factor of 3).
	    This output is divided by a factor of 16, then used as a 192KHz
	    input clock to the T1 input line.
	    The I8039 core currently doesn't support clock out on this pin.
	*/
}

void finalizr_state::main_map(address_map &map)
{
	map(0x0001, 0x0001).writeonly().share("scroll");
	map(0x0003, 0x0003).w(FUNC(finalizr_state::finalizr_videoctrl_w));
	map(0x0004, 0x0004).w(FUNC(finalizr_state::finalizr_flipscreen_w));
//  AM_RANGE(0x0020, 0x003f) AM_WRITEONLY AM_SHARE("scroll")
	map(0x0800, 0x0800).portr("DSW3");
	map(0x0808, 0x0808).portr("DSW2");
	map(0x0810, 0x0810).portr("SYSTEM");
	map(0x0811, 0x0811).portr("P1");
	map(0x0812, 0x0812).portr("P2");
	map(0x0813, 0x0813).portr("DSW1");
	map(0x0818, 0x0818).w("watchdog", FUNC(watchdog_timer_device::reset_w));
	map(0x0819, 0x0819).w(FUNC(finalizr_state::finalizr_coin_w));
	map(0x081a, 0x081a).w("snsnd", FUNC(sn76489a_device::write));   /* This address triggers the SN chip to read the data port. */
	map(0x081b, 0x081b).nopw();        /* Loads the snd command into the snd latch */
	map(0x081c, 0x081c).w(FUNC(finalizr_state::finalizr_i8039_irq_w)); /* custom sound chip */
	map(0x081d, 0x081d).w("soundlatch", FUNC(generic_latch_8_device::write)); /* custom sound chip */
	map(0x2000, 0x23ff).ram().share("colorram");
	map(0x2400, 0x27ff).ram().share("videoram");
	map(0x2800, 0x2bff).ram().share("colorram2");
	map(0x2c00, 0x2fff).ram().share("videoram2");
	map(0x3000, 0x31ff).ram().share("spriteram");
	map(0x3200, 0x37ff).ram();
	map(0x3800, 0x39ff).ram().share("spriteram_2");
	map(0x3a00, 0x3fff).ram();
	map(0x4000, 0xffff).rom();
}

void finalizr_state::sound_map(address_map &map)
{
	map(0x0000, 0x0fff).rom();
}

void finalizr_state::sound_io_map(address_map &map)
{
	map(0x00, 0xff).r("soundlatch", FUNC(generic_latch_8_device::read));
}


static INPUT_PORTS_START( finalizr )
	PORT_START("SYSTEM")
	KONAMI8_SYSTEM_10
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_VBLANK("screen")

	PORT_START("P1")
	KONAMI8_MONO_B12_UNK

	PORT_START("P2")
	KONAMI8_COCKTAIL_B12_UNK

	PORT_START("DSW1")
	KONAMI_COINAGE_LOC(DEF_STR( Free_Play ), "No Coin B", SW1)
	/* "No Coin B" = coins produce sound, but no effect on coin counter */

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )            PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x03, "2" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Cabinet ) )          PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(    0x18, "30000 150000" )
	PORT_DIPSETTING(    0x10, "50000 300000" )
	PORT_DIPSETTING(    0x08, "30000" )
	PORT_DIPSETTING(    0x00, "50000" )
	PORT_DIPNAME( 0x60, 0x40, DEF_STR( Difficulty ) )       PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(    0x60, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Very_Hard ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )      PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW3")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Flip_Screen ) )      PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Controls ) )         PORT_DIPLOCATION("SW3:2")
	PORT_DIPSETTING(    0x02, DEF_STR( Single ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Dual ) )
	PORT_SERVICE_DIPLOC( 0x04, IP_ACTIVE_LOW, "SW3:3" )
	PORT_DIPUNUSED_DIPLOC( 0x08, 0x08, "SW3:4" )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( finalizra )
	PORT_INCLUDE( finalizr )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Bonus_Life ) )       PORT_DIPLOCATION("SW2:4,5")
	PORT_DIPSETTING(    0x18, "20000 100000" )
	PORT_DIPSETTING(    0x10, "30000 150000" )
	PORT_DIPSETTING(    0x08, "20000" )
	PORT_DIPSETTING(    0x00, "30000" )
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "SW2:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x40, 0x40, "SW2:7" )

	PORT_MODIFY("DSW3")
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW3:4" )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4,
			32*8+0*4, 32*8+1*4, 32*8+2*4, 32*8+3*4, 32*8+4*4, 32*8+5*4, 32*8+6*4, 32*8+7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32 },
	32*32
};

static GFXDECODE_START( gfx_finalizr )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,        0, 16 )
	GFXDECODE_ENTRY( "gfx1", 0, spritelayout,  16*16, 16 )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,    16*16, 16 )  /* to handle 8x8 sprites */
GFXDECODE_END


void finalizr_state::machine_start()
{
	save_item(NAME(m_spriterambank));
	save_item(NAME(m_charbank));
	save_item(NAME(m_T1_line));
	save_item(NAME(m_nmi_enable));
	save_item(NAME(m_irq_enable));
}

void finalizr_state::machine_reset()
{
	m_spriterambank = 0;
	m_charbank = 0;
	m_T1_line = 0;
	m_nmi_enable = 0;
	m_irq_enable = 0;
}

void finalizr_state::finalizr(machine_config &config)
{
	/* basic machine hardware */
	KONAMI1(config, m_maincpu, XTAL(18'432'000)/6); /* ??? */
	m_maincpu->set_addrmap(AS_PROGRAM, &finalizr_state::main_map);
	TIMER(config, "scantimer").configure_scanline(FUNC(finalizr_state::finalizr_scanline), "screen", 0, 1);

	I8039(config, m_audiocpu, XTAL(18'432'000)/2); /* 9.216MHz clkin ?? */
	m_audiocpu->set_addrmap(AS_PROGRAM, &finalizr_state::sound_map);
	m_audiocpu->set_addrmap(AS_IO, &finalizr_state::sound_io_map);
	m_audiocpu->p1_out_cb().set("dac", FUNC(dac_byte_interface::data_w));
	m_audiocpu->p2_out_cb().set(FUNC(finalizr_state::i8039_irqen_w));
	m_audiocpu->t1_in_cb().set(FUNC(finalizr_state::i8039_t1_r));

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500) /* not accurate */);
	screen.set_size(36*8, 32*8);
	screen.set_visarea(1*8, 35*8-1, 2*8, 30*8-1);
	screen.set_screen_update(FUNC(finalizr_state::screen_update_finalizr));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_finalizr);
	PALETTE(config, m_palette, FUNC(finalizr_state::finalizr_palette), 2*16*16, 32);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();

	GENERIC_LATCH_8(config, "soundlatch");

	SN76489A(config, "snsnd", XTAL(18'432'000)/12).add_route(ALL_OUTPUTS, "speaker", 0.75);

	DAC_8BIT_R2R(config, "dac", 0).add_route(ALL_OUTPUTS, "speaker", 0.325); // unknown DAC
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref"));
	vref.add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
	vref.add_route(0, "dac", -1.0, DAC_VREF_NEG_INPUT);
}



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( finalizr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "523k01.9c",    0x4000, 0x4000, CRC(716633cb) SHA1(9c21e608b6a73967688fa6aeb5995c20c1b48c74) )
	ROM_LOAD( "523k02.12c",   0x8000, 0x4000, CRC(1bccc696) SHA1(3c29f4a030e76660b5a25347e042e344b0653343) )
	ROM_LOAD( "523k03.13c",   0xc000, 0x4000, CRC(c48927c6) SHA1(9cf6b285034670370ba0246c33e1fe0a057457e7) )

	ROM_REGION( 0x1000, "audiocpu", 0 ) /* 8039 */
	ROM_LOAD( "d8749hd.bin",  0x0000, 0x0800, BAD_DUMP CRC(978dfc33) SHA1(13d24ce577b88bf6ec2e970d36dc67a7ec691c55) )   /* this comes from the bootleg, the original has a custom IC */

	ROM_REGION( 0x20000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "523h04.5e",    0x00000, 0x4000, CRC(c056d710) SHA1(3fe0ab7ef3bce7298c2a073d0985c33f9dc40062) )
	ROM_LOAD16_BYTE( "523h07.5f",    0x00001, 0x4000, CRC(50e512ba) SHA1(f916afb9df1872f9de571d20b9045b20d9172eaa) )
	ROM_LOAD16_BYTE( "523h05.6e",    0x08000, 0x4000, CRC(ae0d0f76) SHA1(6dd0119e4ba7ebb32ba1ca6395f80d18f1617ce8) )
	ROM_LOAD16_BYTE( "523h08.6f",    0x08001, 0x4000, CRC(79f44e17) SHA1(cb32edc4df9f2209f13fc258fec4e67ee91badef) )
	ROM_LOAD16_BYTE( "523h06.7e",    0x10000, 0x4000, CRC(d2db9689) SHA1(ceb5913716b4da2ddff2e837ddaa04d91e52f9e1) )
	ROM_LOAD16_BYTE( "523h09.7f",    0x10001, 0x4000, CRC(8896dc85) SHA1(91493c6b69655de482f0c2a0cb3662fc0d1b6e45) )
	/* 18000-1ffff empty */

	ROM_REGION( 0x0240, "proms", 0 ) /* PROMs at 2F & 3F are MMI 63S081N (or compatibles), PROMs at 10F & 11F are MMI 6301-1N (or compatibles) */
	ROM_LOAD( "523h10.2f",    0x0000, 0x0020, CRC(ec15dd15) SHA1(710384b154a9363fdc88edffda252f1d60e000dc) ) /* palette */
	ROM_LOAD( "523h11.3f",    0x0020, 0x0020, CRC(54be2e83) SHA1(3200abc7f2238d62d7204ef57a6daa2df150538d) ) /* palette */
	ROM_LOAD( "523h13.11f",   0x0040, 0x0100, CRC(4e0647a0) SHA1(fb87f878456b8b76bb2c028cb890d2a5c1c3e388) ) /* characters */
	ROM_LOAD( "523h12.10f",   0x0140, 0x0100, CRC(53166a2a) SHA1(6cdde206036df7176679711f7888d72acee27c8f) ) /* sprites */
ROM_END

ROM_START( finalizra )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "1.9c",    0x4000, 0x4000, CRC(7d464e5c) SHA1(45b1591a6be713dfc58ab657e61531ea2b9263c1) )
	ROM_LOAD( "2.12c",   0x8000, 0x4000, CRC(383dc94e) SHA1(f192e16e83ae34cc97af07072a4dc68e7c4c362c) )
	ROM_LOAD( "3.13c",   0xc000, 0x4000, CRC(ce177f6e) SHA1(034cbe0c1e2baf9577741b3c222a8b4a8ac8c919) )

	ROM_REGION( 0x1000, "audiocpu", 0 ) /* 8039 */
	ROM_LOAD( "d8749hd.bin",  0x0000, 0x0800, BAD_DUMP CRC(978dfc33) SHA1(13d24ce577b88bf6ec2e970d36dc67a7ec691c55) )   /* this comes from the bootleg, the original has a custom IC */

	ROM_REGION( 0x20000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "523h04.5e",    0x00000, 0x4000, CRC(c056d710) SHA1(3fe0ab7ef3bce7298c2a073d0985c33f9dc40062) )
	ROM_LOAD16_BYTE( "523h07.5f",    0x00001, 0x4000, CRC(50e512ba) SHA1(f916afb9df1872f9de571d20b9045b20d9172eaa) )
	ROM_LOAD16_BYTE( "523h05.6e",    0x08000, 0x4000, CRC(ae0d0f76) SHA1(6dd0119e4ba7ebb32ba1ca6395f80d18f1617ce8) )
	ROM_LOAD16_BYTE( "523h08.6f",    0x08001, 0x4000, CRC(79f44e17) SHA1(cb32edc4df9f2209f13fc258fec4e67ee91badef) )
	ROM_LOAD16_BYTE( "523h06.7e",    0x10000, 0x4000, CRC(d2db9689) SHA1(ceb5913716b4da2ddff2e837ddaa04d91e52f9e1) )
	ROM_LOAD16_BYTE( "523h09.7f",    0x10001, 0x4000, CRC(8896dc85) SHA1(91493c6b69655de482f0c2a0cb3662fc0d1b6e45) )
	/* 18000-1ffff empty */

	ROM_REGION( 0x0240, "proms", 0 ) /* PROMs at 2F & 3F are MMI 63S081N (or compatibles), PROMs at 10F & 11F are MMI 6301-1N (or compatibles) */
	ROM_LOAD( "523h10.2f",    0x0000, 0x0020, CRC(ec15dd15) SHA1(710384b154a9363fdc88edffda252f1d60e000dc) ) /* palette */
	ROM_LOAD( "523h11.3f",    0x0020, 0x0020, CRC(54be2e83) SHA1(3200abc7f2238d62d7204ef57a6daa2df150538d) ) /* palette */
	ROM_LOAD( "523h13.11f",   0x0040, 0x0100, CRC(4e0647a0) SHA1(fb87f878456b8b76bb2c028cb890d2a5c1c3e388) ) /* characters */
	ROM_LOAD( "523h12.10f",   0x0140, 0x0100, CRC(53166a2a) SHA1(6cdde206036df7176679711f7888d72acee27c8f) ) /* sprites */
ROM_END

ROM_START( finalizrb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "finalizr.5",   0x4000, 0x8000, CRC(a55e3f14) SHA1(47f6da214b36cc56be547fa4313afcc5572508a2) )
	ROM_LOAD( "finalizr.6",   0xc000, 0x4000, CRC(ce177f6e) SHA1(034cbe0c1e2baf9577741b3c222a8b4a8ac8c919) )

	ROM_REGION( 0x1000, "audiocpu", 0 ) /* 8039 */
	ROM_LOAD( "d8749hd.bin",  0x0000, 0x0800, CRC(978dfc33) SHA1(13d24ce577b88bf6ec2e970d36dc67a7ec691c55) )

	ROM_REGION( 0x20000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "523h04.5e",    0x00000, 0x4000, CRC(c056d710) SHA1(3fe0ab7ef3bce7298c2a073d0985c33f9dc40062) )
	ROM_LOAD16_BYTE( "523h07.5f",    0x00001, 0x4000, CRC(50e512ba) SHA1(f916afb9df1872f9de571d20b9045b20d9172eaa) )
	ROM_LOAD16_BYTE( "523h05.6e",    0x08000, 0x4000, CRC(ae0d0f76) SHA1(6dd0119e4ba7ebb32ba1ca6395f80d18f1617ce8) )
	ROM_LOAD16_BYTE( "523h08.6f",    0x08001, 0x4000, CRC(79f44e17) SHA1(cb32edc4df9f2209f13fc258fec4e67ee91badef) )
	ROM_LOAD16_BYTE( "523h06.7e",    0x10000, 0x4000, CRC(d2db9689) SHA1(ceb5913716b4da2ddff2e837ddaa04d91e52f9e1) )
	ROM_LOAD16_BYTE( "523h09.7f",    0x10001, 0x4000, CRC(8896dc85) SHA1(91493c6b69655de482f0c2a0cb3662fc0d1b6e45) )
	/* 18000-1ffff empty */

	ROM_REGION( 0x0240, "proms", 0 ) /* PROMs at 2F & 3F are MMI 63S081N (or compatibles), PROMs at 10F & 11F are MMI 6301-1N (or compatibles) */
	ROM_LOAD( "523h10.2f",    0x0000, 0x0020, CRC(ec15dd15) SHA1(710384b154a9363fdc88edffda252f1d60e000dc) ) /* palette */
	ROM_LOAD( "523h11.3f",    0x0020, 0x0020, CRC(54be2e83) SHA1(3200abc7f2238d62d7204ef57a6daa2df150538d) ) /* palette */
	ROM_LOAD( "523h13.11f",   0x0040, 0x0100, CRC(4e0647a0) SHA1(fb87f878456b8b76bb2c028cb890d2a5c1c3e388) ) /* characters */
	ROM_LOAD( "523h12.10f",   0x0140, 0x0100, CRC(53166a2a) SHA1(6cdde206036df7176679711f7888d72acee27c8f) ) /* sprites */
ROM_END



GAME( 1985, finalizr,  0,        finalizr, finalizr,  finalizr_state, empty_init, ROT90, "Konami",  "Finalizer - Super Transformation (set 1)",   MACHINE_IMPERFECT_SOUND | MACHINE_SUPPORTS_SAVE )
GAME( 1985, finalizra, finalizr, finalizr, finalizra, finalizr_state, empty_init, ROT90, "Konami",  "Finalizer - Super Transformation (set 2)",   MACHINE_IMPERFECT_SOUND | MACHINE_SUPPORTS_SAVE )
GAME( 1985, finalizrb, finalizr, finalizr, finalizra, finalizr_state, empty_init, ROT90, "bootleg", "Finalizer - Super Transformation (bootleg)", MACHINE_IMPERFECT_SOUND | MACHINE_SUPPORTS_SAVE )
