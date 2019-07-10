// license:BSD-3-Clause
// copyright-holders:Stefan Jokisch
/***************************************************************************

  Atari Tournament Table driver

  Hardware is identical to the VCS2600 except for an extra 6532 chip.

***************************************************************************/

#include "emu.h"
#include "machine/6532riot.h"
#include "cpu/m6502/m6507.h"
#include "machine/watchdog.h"
#include "sound/tiaintf.h"
#include "video/tia.h"
#include "screen.h"
#include "speaker.h"


class tourtabl_state : public driver_device
{
public:
	tourtabl_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_leds(*this, "led%u", 0U)
	{ }

	void tourtabl(machine_config &config);

private:
	virtual void machine_start() override { m_leds.resolve(); }
	DECLARE_WRITE8_MEMBER(tourtabl_led_w);
	DECLARE_READ16_MEMBER(tourtabl_read_input_port);
	DECLARE_READ8_MEMBER(tourtabl_get_databus_contents);
	void main_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	output_finder<4> m_leds;
};


#define MASTER_CLOCK    XTAL(3'579'545)


WRITE8_MEMBER(tourtabl_state::tourtabl_led_w)
{
	m_leds[0] = BIT(data, 6); /* start 1 */
	m_leds[1] = BIT(data, 5); /* start 2 */
	m_leds[2] = BIT(data, 4); /* start 4 */
	m_leds[3] = BIT(data, 7); /* select game */

	machine().bookkeeping().coin_lockout_global_w(!(data & 0x80));
}


READ16_MEMBER(tourtabl_state::tourtabl_read_input_port)
{
	static const char *const tianames[] = { "PADDLE4", "PADDLE3", "PADDLE2", "PADDLE1", "TIA_IN4", "TIA_IN5" };

	return ioport(tianames[offset])->read();
}

READ8_MEMBER(tourtabl_state::tourtabl_get_databus_contents)
{
	return offset;
}


void tourtabl_state::main_map(address_map &map)
{
	map(0x0000, 0x007f).mirror(0x0100).rw("tia_video", FUNC(tia_video_device::read), FUNC(tia_video_device::write));
	map(0x0080, 0x00ff).mirror(0x0100).ram();
	map(0x0280, 0x029f).rw("riot1", FUNC(riot6532_device::read), FUNC(riot6532_device::write));
	map(0x0400, 0x047f).ram();
	map(0x0500, 0x051f).rw("riot2", FUNC(riot6532_device::read), FUNC(riot6532_device::write));
	map(0x0800, 0x1fff).rom();
}


static INPUT_PORTS_START( tourtabl )

	PORT_START("PADDLE4")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(40) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_REVERSE PORT_PLAYER(4)

	PORT_START("PADDLE3")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(40) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_REVERSE PORT_PLAYER(3)

	PORT_START("PADDLE2")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(40) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_REVERSE PORT_PLAYER(2)

	PORT_START("PADDLE1")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_SENSITIVITY(40) PORT_KEYDELTA(10) PORT_CENTERDELTA(0) PORT_REVERSE PORT_PLAYER(1)

	PORT_START("TIA_IN4")   /* TIA INPT4 */
	PORT_DIPNAME( 0x80, 0x80, "Breakout Replay" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ))
	PORT_DIPSETTING(    0x80, DEF_STR( On ))

	PORT_START("TIA_IN5")   /* TIA INPT5 */
	PORT_DIPNAME( 0x80, 0x80, "Game Length" )
	PORT_DIPSETTING(    0x00, "11 points (3 balls)" )
	PORT_DIPSETTING(    0x80, "15 points (5 balls)" )

	PORT_START("RIOT0_SWA") /* RIOT #0 SWCHA */
	PORT_DIPNAME( 0x0F, 0x0E, "Replay Level" )
	PORT_DIPSETTING(    0x0B, "200 points" )
	PORT_DIPSETTING(    0x0C, "250 points" )
	PORT_DIPSETTING(    0x0D, "300 points" )
	PORT_DIPSETTING(    0x0E, "400 points" )
	PORT_DIPSETTING(    0x0F, "450 points" )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)

	PORT_START("RIOT0_SWB") /* RIOT #0 SWCHB */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Game Select") PORT_CODE(KEYCODE_SPACE)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START("RIOT1_SWA")/* RIOT #1 SWCHA */
	PORT_DIPNAME( 0x0F, 0x07, DEF_STR( Coinage ))
	PORT_DIPSETTING(    0x00, "Mode A" )
	PORT_DIPSETTING(    0x01, "Mode B" )
	PORT_DIPSETTING(    0x02, "Mode C" )
	PORT_DIPSETTING(    0x03, "Mode D" )
	PORT_DIPSETTING(    0x04, "Mode E" )
	PORT_DIPSETTING(    0x05, "Mode F" )
	PORT_DIPSETTING(    0x06, "Mode G" )
	PORT_DIPSETTING(    0x07, "Mode H" )
	PORT_DIPSETTING(    0x08, "Mode I" )
	PORT_DIPSETTING(    0x09, "Mode J" )
	PORT_DIPSETTING(    0x0A, "Mode K" )
	PORT_DIPSETTING(    0x0B, "Mode L" )
	PORT_DIPSETTING(    0x0C, "Mode M" )
	PORT_DIPSETTING(    0x0D, "Mode N" )
	PORT_DIPSETTING(    0x0E, "Mode O" )
	PORT_DIPSETTING(    0x0F, "Mode P" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Language ) )
	PORT_DIPSETTING(    0x00, DEF_STR( English ) )
	PORT_DIPSETTING(    0x10, DEF_STR( French ) )
	PORT_DIPSETTING(    0x20, DEF_STR( German ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Spanish ) )
	PORT_SERVICE( 0x40, IP_ACTIVE_HIGH )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("RIOT1_SWB") /* RIOT #1 SWCHB */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )

INPUT_PORTS_END


void tourtabl_state::tourtabl(machine_config &config)
{
	/* basic machine hardware */
	M6507(config, m_maincpu, MASTER_CLOCK / 3);
	m_maincpu->set_addrmap(AS_PROGRAM, &tourtabl_state::main_map);

	riot6532_device &riot1(RIOT6532(config, "riot1", MASTER_CLOCK / 3));
	riot1.in_pa_callback().set_ioport("RIOT0_SWA");
	riot1.in_pb_callback().set_ioport("RIOT0_SWB");
	riot1.out_pb_callback().set("watchdog", FUNC(watchdog_timer_device::reset_w));

	riot6532_device &riot2(RIOT6532(config, "riot2", MASTER_CLOCK / 3));
	riot2.in_pa_callback().set_ioport("RIOT1_SWA");
	riot2.in_pb_callback().set_ioport("RIOT1_SWB");
	riot2.out_pb_callback().set(FUNC(tourtabl_state::tourtabl_led_w));

	WATCHDOG_TIMER(config, "watchdog");

	/* video hardware */
	tia_ntsc_video_device &tia(TIA_NTSC_VIDEO(config, "tia_video", 0, "tia"));
	tia.read_input_port_callback().set(FUNC(tourtabl_state::tourtabl_read_input_port));
	tia.databus_contents_callback().set(FUNC(tourtabl_state::tourtabl_get_databus_contents));

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_raw(MASTER_CLOCK, 228, 34, 34 + 160, 262, 46, 46 + 200);
	screen.set_screen_update("tia_video", FUNC(tia_video_device::screen_update));
	screen.set_palette("tia_video:palette");

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
	TIA(config, "tia", MASTER_CLOCK/114).add_route(ALL_OUTPUTS, "mono", 1.0);
}


ROM_START( tourtabl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "030751.ab2", 0x0800, 0x0800, CRC(4479a6f7) SHA1(bf3fd859614533a592f831e3539ea0a9d1964c82) )
	ROM_LOAD( "030752.ab3", 0x1000, 0x0800, CRC(c92c49dc) SHA1(cafcf13e1b1087b477a667d1e785f5e2be187b0d) )
	ROM_LOAD( "030753.ab4", 0x1800, 0x0800, CRC(3978b269) SHA1(4fa05c655bb74711eb99428f36df838ec70da699) )
ROM_END


ROM_START( tourtab2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "030929.ab2", 0x0800, 0x0800, CRC(fcdfafa2) SHA1(f35ab83366a334a110fbba0cef09f4db950dbb68) )
	ROM_LOAD( "030752.ab3", 0x1000, 0x0800, CRC(c92c49dc) SHA1(cafcf13e1b1087b477a667d1e785f5e2be187b0d) )
	ROM_LOAD( "030753.ab4", 0x1800, 0x0800, CRC(3978b269) SHA1(4fa05c655bb74711eb99428f36df838ec70da699) )
ROM_END


GAME( 1978, tourtabl, 0,        tourtabl, tourtabl, tourtabl_state, empty_init, ROT0, "Atari", "Tournament Table (set 1)", MACHINE_SUPPORTS_SAVE )
GAME( 1978, tourtab2, tourtabl, tourtabl, tourtabl, tourtabl_state, empty_init, ROT0, "Atari", "Tournament Table (set 2)", MACHINE_SUPPORTS_SAVE )
