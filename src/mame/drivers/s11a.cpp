// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic, Robbbert
/****************************************************************************************

    Pinball
    Williams System 11A

    Status of games:


ToDo:
- Doesn't react to the Advance button very well
- Some LEDs flicker

Note: To start a game, certain switches need to be activated.  You must first press and
      hold one of the trough switches (usually the left) and the ball shooter switch for
      about 1 second.  Then you are able to start a game.
      For Pinbot, you must hold L and V for a second, then press start.
      For Millionaire, you must hold [ and ] for a second, then start.

*****************************************************************************************/

#include "emu.h"
#include "includes/s11a.h"

#include "cpu/m6800/m6800.h"
#include "cpu/m6809/m6809.h"
#include "sound/volt_reg.h"
#include "speaker.h"

#include "s11a.lh"


void s11a_state::s11a_main_map(address_map &map)
{
	map(0x0000, 0x0fff).ram().share("nvram");
	map(0x2100, 0x2103).mirror(0x00fc).rw(m_pia21, FUNC(pia6821_device::read), FUNC(pia6821_device::write)); // sound+solenoids
	map(0x2200, 0x2200).mirror(0x01ff).w(FUNC(s11a_state::sol3_w)); // solenoids
	map(0x2400, 0x2403).mirror(0x03fc).rw(m_pia24, FUNC(pia6821_device::read), FUNC(pia6821_device::write)); // lamps
	map(0x2800, 0x2803).mirror(0x03fc).rw(m_pia28, FUNC(pia6821_device::read), FUNC(pia6821_device::write)); // display
	map(0x2c00, 0x2c03).mirror(0x03fc).rw(m_pia2c, FUNC(pia6821_device::read), FUNC(pia6821_device::write)); // alphanumeric display
	map(0x3000, 0x3003).mirror(0x03fc).rw(m_pia30, FUNC(pia6821_device::read), FUNC(pia6821_device::write)); // inputs
	map(0x3400, 0x3403).mirror(0x0bfc).rw(m_pia34, FUNC(pia6821_device::read), FUNC(pia6821_device::write)); // widget
	map(0x4000, 0xffff).rom();
}

void s11a_state::s11a_audio_map(address_map &map)
{
	map(0x0000, 0x07ff).mirror(0x0800).ram();
	map(0x1000, 0x1fff).w(FUNC(s11a_state::bank_w));
	map(0x2000, 0x2003).mirror(0x0ffc).rw(m_pias, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	map(0x8000, 0xbfff).bankr("bank0");
	map(0xc000, 0xffff).bankr("bank1");
}

void s11a_state::s11a_bg_map(address_map &map)
{
	map(0x0000, 0x07ff).mirror(0x1800).ram();
	map(0x2000, 0x2001).mirror(0x1ffe).rw(m_ym, FUNC(ym2151_device::read), FUNC(ym2151_device::write));
	map(0x4000, 0x4003).mirror(0x1ffc).rw(m_pia40, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	map(0x7800, 0x7fff).w(FUNC(s11a_state::bgbank_w));
	map(0x8000, 0xffff).bankr("bgbank");
}

static INPUT_PORTS_START( s11a )
	PORT_START("X0")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("X1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER )

	PORT_START("X2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_F)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_G)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_H)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_J)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_K)

	PORT_START("X4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_L)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_N)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_M)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_COMMA)

	PORT_START("X8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_STOP)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_SLASH)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_COLON)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_MINUS)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_EQUALS)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSPACE)

	PORT_START("X10")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSLASH)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_ENTER)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_LEFT)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_UP)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_DOWN)

	PORT_START("X20")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Y)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_I)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_O)

	PORT_START("X40")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("X80")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DIAGS")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Audio Diag") PORT_CODE(KEYCODE_1_PAD) PORT_CHANGED_MEMBER(DEVICE_SELF, s11a_state, audio_nmi, 1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Main Diag") PORT_CODE(KEYCODE_4_PAD) PORT_CHANGED_MEMBER(DEVICE_SELF, s11a_state, main_nmi, 1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Advance") PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Up/Down") PORT_CODE(KEYCODE_6_PAD) PORT_TOGGLE
	PORT_CONFNAME( 0x10, 0x10, "Language" )
	PORT_CONFSETTING( 0x00, "German" )
	PORT_CONFSETTING( 0x10, "English" )
INPUT_PORTS_END


MACHINE_RESET_MEMBER( s11a_state, s11a )
{
	MACHINE_RESET_CALL_MEMBER(s11);
	membank("bgbank")->set_entry(0);
}

WRITE8_MEMBER( s11a_state::dig0_w )
{
	data &= 0x7f;
	set_strobe(data & 15);
	set_diag((data & 0x70) >> 4);
	m_digits[60] = 0;  // +5VDC (always on)
	m_digits[61] = get_diag() & 0x01;  // connected to PA4
	m_digits[62] = 0;  // Blanking (pretty much always on)
	set_segment1(0);
	set_segment2(0);
}

WRITE8_MEMBER( s11a_state::bgbank_w )
{
	membank("bgbank")->set_entry(data & 0x03);
}

void s11a_state::init_s11a()
{
	uint8_t *BGROM = memregion("bgcpu")->base();
	membank("bgbank")->configure_entries(0, 4, &BGROM[0x10000], 0x8000);
	membank("bgbank")->set_entry(0);
	s11_state::init_s11();
}

void s11a_state::s11a(machine_config &config)
{
	/* basic machine hardware */
	M6808(config, m_maincpu, XTAL(4'000'000));
	m_maincpu->set_addrmap(AS_PROGRAM, &s11a_state::s11a_main_map);
	MCFG_MACHINE_RESET_OVERRIDE(s11a_state, s11a)

	/* Video */
	config.set_default_layout(layout_s11a);

	/* Sound */
	genpin_audio(config);

	/* Devices */
	PIA6821(config, m_pia21, 0);
	m_pia21->readpa_handler().set(FUNC(s11_state::sound_r));
	m_pia21->writepa_handler().set(FUNC(s11_state::sound_w));
	m_pia21->writepb_handler().set(FUNC(s11_state::sol2_w));
	m_pia21->ca2_handler().set(FUNC(s11_state::pia21_ca2_w));
	m_pia21->cb2_handler().set(FUNC(s11_state::pia21_cb2_w));
	m_pia21->irqa_handler().set(FUNC(s11_state::pia_irq));
	m_pia21->irqb_handler().set(FUNC(s11_state::pia_irq));

	PIA6821(config, m_pia24, 0);
	m_pia24->writepa_handler().set(FUNC(s11_state::lamp0_w));
	m_pia24->writepb_handler().set(FUNC(s11_state::lamp1_w));
	m_pia24->cb2_handler().set(FUNC(s11_state::pia24_cb2_w));
	m_pia24->irqa_handler().set(FUNC(s11_state::pia_irq));
	m_pia24->irqb_handler().set(FUNC(s11_state::pia_irq));

	PIA6821(config, m_pia28, 0);
	m_pia28->readpa_handler().set(FUNC(s11_state::pia28_w7_r));
	m_pia28->writepa_handler().set(FUNC(s11a_state::dig0_w));
	m_pia28->writepb_handler().set(FUNC(s11_state::dig1_w));
	m_pia28->ca2_handler().set(FUNC(s11_state::pia28_ca2_w));
	m_pia28->cb2_handler().set(FUNC(s11_state::pia28_cb2_w));
	m_pia28->irqa_handler().set(FUNC(s11_state::pia_irq));
	m_pia28->irqb_handler().set(FUNC(s11_state::pia_irq));

	PIA6821(config, m_pia2c, 0);
	m_pia2c->writepa_handler().set(FUNC(s11_state::pia2c_pa_w));
	m_pia2c->writepb_handler().set(FUNC(s11_state::pia2c_pb_w));
	m_pia2c->irqa_handler().set(FUNC(s11_state::pia_irq));
	m_pia2c->irqb_handler().set(FUNC(s11_state::pia_irq));

	PIA6821(config, m_pia30, 0);
	m_pia30->readpa_handler().set(FUNC(s11_state::switch_r));
	m_pia30->writepb_handler().set(FUNC(s11_state::switch_w));
	m_pia30->cb2_handler().set(FUNC(s11_state::pia30_cb2_w));
	m_pia30->irqa_handler().set(FUNC(s11_state::pia_irq));
	m_pia30->irqb_handler().set(FUNC(s11_state::pia_irq));

	PIA6821(config, m_pia34, 0);
	m_pia34->writepa_handler().set(FUNC(s11_state::pia34_pa_w));
	m_pia34->writepb_handler().set(FUNC(s11_state::pia34_pb_w));
	m_pia34->cb2_handler().set(FUNC(s11_state::pia34_cb2_w));
	m_pia34->irqa_handler().set(FUNC(s11_state::pia_irq));
	m_pia34->irqb_handler().set(FUNC(s11_state::pia_irq));

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_1);

	/* Add the soundcard */
	M6802(config, m_audiocpu, XTAL(4'000'000));
	m_audiocpu->set_addrmap(AS_PROGRAM, &s11a_state::s11a_audio_map);

	SPEAKER(config, "speaker").front_center();
	MC1408(config, "dac", 0).add_route(ALL_OUTPUTS, "speaker", 0.25);
	voltage_regulator_device &vref(VOLTAGE_REGULATOR(config, "vref"));
	vref.add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac", -1.0, DAC_VREF_NEG_INPUT);
	vref.add_route(0, "dac1", 1.0, DAC_VREF_POS_INPUT); vref.add_route(0, "dac1", -1.0, DAC_VREF_NEG_INPUT);

	SPEAKER(config, "speech").front_center();
	HC55516(config, m_hc55516, 0).add_route(ALL_OUTPUTS, "speech", 0.50);

	PIA6821(config, m_pias, 0);
	m_pias->readpa_handler().set(FUNC(s11_state::sound_r));
	m_pias->writepa_handler().set(FUNC(s11_state::sound_w));
	m_pias->writepb_handler().set("dac", FUNC(dac_byte_interface::data_w));
	m_pias->cb2_handler().set(FUNC(s11_state::pia40_cb2_w));
	m_pias->irqa_handler().set_inputline(m_audiocpu, M6802_IRQ_LINE);
	m_pias->irqa_handler().set_inputline(m_audiocpu, M6802_IRQ_LINE);

	/* Add the background music card */
	MC6809E(config, m_bgcpu, XTAL(8'000'000) / 4); // MC68B09E
	m_bgcpu->set_addrmap(AS_PROGRAM, &s11a_state::s11a_bg_map);

	SPEAKER(config, "bg").front_center();
	YM2151(config, m_ym, XTAL(3'579'545));
	m_ym->irq_handler().set(FUNC(s11a_state::ym2151_irq_w));
	m_ym->add_route(ALL_OUTPUTS, "bg", 0.50);

	MC1408(config, "dac1", 0).add_route(ALL_OUTPUTS, "bg", 0.25);

	PIA6821(config, m_pia40, 0);
	m_pia40->writepa_handler().set("dac1", FUNC(dac_byte_interface::data_w));
	m_pia40->writepb_handler().set(FUNC(s11_state::pia40_pb_w));
	m_pia40->ca2_handler().set(FUNC(s11_state::pias_ca2_w));
	m_pia40->cb2_handler().set(FUNC(s11_state::pias_cb2_w));
	m_pia40->irqa_handler().set_inputline(m_bgcpu, M6809_FIRQ_LINE);
	m_pia40->irqb_handler().set_inputline(m_bgcpu, INPUT_LINE_NMI);
}

/*------------------------
/ F14 Tomcat 5/87 (#554)
/-------------------------*/

ROM_START(f14_p3)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("f14_l3.u26", 0x4000, 0x4000, CRC(cd607556) SHA1(2ec95085784370a071cbf5df7ae5c6b4749605e2))
	ROM_LOAD("f14_l3.u27", 0x8000, 0x8000, CRC(72951fd1) SHA1(b5f3fe1859e0abf9ab558b4b4f6754134d528c23))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("f14_u21.l1", 0x18000, 0x8000, CRC(e412300c) SHA1(382d0cfa47abea295f0c7501bc0a010473e9d73b))
	ROM_LOAD("f14_u22.l1", 0x10000, 0x8000, CRC(c9dd7496) SHA1(de3cb855d87033274cc912578b02d1593d2d69f9))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("f14_u4.l1", 0x10000, 0x8000, CRC(43ecaabf) SHA1(64b50dbff03cd556130d0cff47b951fdf37d397d))
	ROM_LOAD("f14_u19.l1", 0x18000, 0x8000, CRC(d0de4a7c) SHA1(46ecd5786653add47751cc56b38d9db7c4622377))
ROM_END

ROM_START(f14_p4)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("u26_l4.128", 0x4000, 0x4000, CRC(7b39706a) SHA1(0dc0b1a1dfd12bc73e6fd8b825fe72ddc8fc1497))
	ROM_LOAD("u27_l4.256", 0x8000, 0x8000, CRC(189f9488) SHA1(7536d56cb83bf29f8d8b03b226a5f60200776095))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("f14_u21.l1", 0x18000, 0x8000, CRC(e412300c) SHA1(382d0cfa47abea295f0c7501bc0a010473e9d73b))
	ROM_LOAD("f14_u22.l1", 0x10000, 0x8000, CRC(c9dd7496) SHA1(de3cb855d87033274cc912578b02d1593d2d69f9))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("f14_u4.l1", 0x10000, 0x8000, CRC(43ecaabf) SHA1(64b50dbff03cd556130d0cff47b951fdf37d397d))
	ROM_LOAD("f14_u19.l1", 0x18000, 0x8000, CRC(d0de4a7c) SHA1(46ecd5786653add47751cc56b38d9db7c4622377))
ROM_END

ROM_START(f14_p5)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("f14_u26.p5", 0x4000, 0x4000, CRC(f5d9b132) SHA1(b6a5edf8f015ae86513cd28ce2436f3c07199d47))
	ROM_LOAD("f14_u27.p5", 0x8000, 0x8000, CRC(45de7e15) SHA1(a3160cbc0d3a5eb4cdd301251c40806e7c1d3ee8))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("f14_u21.l1", 0x18000, 0x8000, CRC(e412300c) SHA1(382d0cfa47abea295f0c7501bc0a010473e9d73b))
	ROM_LOAD("f14_u22.l1", 0x10000, 0x8000, CRC(c9dd7496) SHA1(de3cb855d87033274cc912578b02d1593d2d69f9))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("f14_u4.l1", 0x10000, 0x8000, CRC(43ecaabf) SHA1(64b50dbff03cd556130d0cff47b951fdf37d397d))
	ROM_LOAD("f14_u19.l1", 0x18000, 0x8000, CRC(d0de4a7c) SHA1(46ecd5786653add47751cc56b38d9db7c4622377))
ROM_END

ROM_START(f14_l1)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("f14_u26.l1", 0x4000, 0x4000, CRC(62c2e615) SHA1(456ce0d1f74fa5e619c272880ba8ac6819848ddc))
	ROM_LOAD("f14_u27.l1", 0x8000, 0x8000, CRC(da1740f7) SHA1(1395a4f3891a043cfedc5426ec88af35eab8d4ea))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("f14_u21.l1", 0x18000, 0x8000, CRC(e412300c) SHA1(382d0cfa47abea295f0c7501bc0a010473e9d73b))
	ROM_LOAD("f14_u22.l1", 0x10000, 0x8000, CRC(c9dd7496) SHA1(de3cb855d87033274cc912578b02d1593d2d69f9))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("f14_u4.l1", 0x10000, 0x8000, CRC(43ecaabf) SHA1(64b50dbff03cd556130d0cff47b951fdf37d397d))
	ROM_LOAD("f14_u19.l1", 0x18000, 0x8000, CRC(d0de4a7c) SHA1(46ecd5786653add47751cc56b38d9db7c4622377))
ROM_END

/*--------------------
/ Fire! 8/87 (#556)
/--------------------*/
ROM_START(fire_l3)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("fire_u26.l3", 0x4000, 0x4000, CRC(48abae33) SHA1(00ce24316aa007eec090ae74818003e11a141214))
	ROM_LOAD("fire_u27.l3", 0x8000, 0x8000, CRC(4ebf4888) SHA1(45dc0231404ed70be2ab5d599a673aac6271550e))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("fire_u21.l2", 0x18000, 0x8000, CRC(2edde0a4) SHA1(de292a340a3a06b0b996fc69fee73eb7bbfbbe64))
	ROM_LOAD("fire_u22.l2", 0x10000, 0x8000, CRC(16145c97) SHA1(523e99df3907a2c843c6e27df4d16799c4136a46))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("fire_u4.l1", 0x10000, 0x8000, CRC(0e058918) SHA1(4d6bf2290141119174787f8dd653c47ea4c73693))
ROM_END

ROM_START(fire_l2)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("fire_u26.l2", 0x4000, 0x4000, CRC(05434ea7) SHA1(462808954de18fed25e6df8f4cc66acdd05a3d85))
	ROM_LOAD("fire_u27.l2", 0x8000, 0x8000, CRC(517d0367) SHA1(b31e591cc24fe937124d8c1da69ab654c12bbb65))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("fire_u21.l2", 0x18000, 0x8000, CRC(2edde0a4) SHA1(de292a340a3a06b0b996fc69fee73eb7bbfbbe64))
	ROM_LOAD("fire_u22.l2", 0x10000, 0x8000, CRC(16145c97) SHA1(523e99df3907a2c843c6e27df4d16799c4136a46))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("fire_u4.l1", 0x10000, 0x8000, CRC(0e058918) SHA1(4d6bf2290141119174787f8dd653c47ea4c73693))
ROM_END

/*--------------------------------------
/ Fire! Champagne Edition 9/87 (#556SE)
/---------------------------------------*/

/*-------------------------
/ Millionaire 1/87 (#555)
/--------------------------*/
ROM_START(milln_l3)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("mill_u26.l3", 0x4000, 0x4000, CRC(07bc9fff) SHA1(b16082fb51df3e4d2fb786cb8894b1c232521ef3))
	ROM_LOAD("mill_u27.l3", 0x8000, 0x8000, CRC(ba789c43) SHA1(c066a304882bea4cba1e215642416fcb22585aa4))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("mill_u21.l1", 0x18000, 0x8000, CRC(4cd1ee90) SHA1(4e24b96138ced16eff9036303ca6347e3423dbfc))
	ROM_LOAD("mill_u22.l1", 0x10000, 0x8000, CRC(73735cfc) SHA1(f74c873a20990263e0d6b35609fc51c08c9f8e31))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("mill_u4.l1", 0x10000, 0x8000, CRC(cf766506) SHA1(a6e4df19a513102abbce2653d4f72245f54407b1))
	ROM_LOAD("mill_u19.l1", 0x18000, 0x8000, CRC(e073245a) SHA1(cbaddde6bb19292ace574a8329e18c97c2ee9763))
ROM_END

/*--------------------
/ Pinbot 10/86 (#549)
/--------------------*/
ROM_START(pb_l5)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("pbot_u26.l5", 0x4000, 0x4000, CRC(daa0c8e4) SHA1(47289b350eb0d84aa0d37e53383e18625451bbe8))
	ROM_LOAD("pbot_u27.l5", 0x8000, 0x8000, CRC(e625d6ce) SHA1(1858dc2183954342b8e2e5eb9a14edcaa8dad5ae))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u21.l1", 0x18000, 0x8000, CRC(3eab88d9) SHA1(667e3b675e2ae8fec6a6faddb9b0dd5531d64f8f))
	ROM_LOAD("pbot_u22.l1", 0x10000, 0x8000, CRC(a2d2c9cb) SHA1(46437dc54538f1626caf41a2818ddcf8000c44e4))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u4.l1", 0x10000, 0x8000, CRC(de5926bd) SHA1(3d111e27c5f0c8c0afc5fe5cc45bf77c12b69228))
	ROM_LOAD("pbot_u19.l1", 0x18000, 0x8000, CRC(40eb4e9f) SHA1(07b0557b35599a2dd5aa66a306fbbe8f50eed998))
ROM_END

ROM_START(pb_l1)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("u26-l1.rom", 0x4000, 0x4000, CRC(e3b94ca4) SHA1(1db2acb025941cc165cc7ec70a160e07ab1eeb2e))
	ROM_LOAD("u27-l1.rom", 0x8000, 0x8000, CRC(fa0be640) SHA1(723dd96bbcc9b3043c91e0215050fb626dd6ced3))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u21.l1", 0x18000, 0x8000, CRC(3eab88d9) SHA1(667e3b675e2ae8fec6a6faddb9b0dd5531d64f8f))
	ROM_LOAD("pbot_u22.l1", 0x10000, 0x8000, CRC(a2d2c9cb) SHA1(46437dc54538f1626caf41a2818ddcf8000c44e4))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u4.l1", 0x10000, 0x8000, CRC(de5926bd) SHA1(3d111e27c5f0c8c0afc5fe5cc45bf77c12b69228))
	ROM_LOAD("pbot_u19.l1", 0x18000, 0x8000, CRC(40eb4e9f) SHA1(07b0557b35599a2dd5aa66a306fbbe8f50eed998))
ROM_END

ROM_START(pb_l2)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("u26-l1.rom", 0x4000, 0x4000, CRC(e3b94ca4) SHA1(1db2acb025941cc165cc7ec70a160e07ab1eeb2e))
	ROM_LOAD("u27-l2.rom", 0x8000, 0x8000, CRC(0a334fc5) SHA1(d08afe6ddc141e37f97ea588d184a316ff7f6db7))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u21.l1", 0x18000, 0x8000, CRC(3eab88d9) SHA1(667e3b675e2ae8fec6a6faddb9b0dd5531d64f8f))
	ROM_LOAD("pbot_u22.l1", 0x10000, 0x8000, CRC(a2d2c9cb) SHA1(46437dc54538f1626caf41a2818ddcf8000c44e4))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u4.l1", 0x10000, 0x8000, CRC(de5926bd) SHA1(3d111e27c5f0c8c0afc5fe5cc45bf77c12b69228))
	ROM_LOAD("pbot_u19.l1", 0x18000, 0x8000, CRC(40eb4e9f) SHA1(07b0557b35599a2dd5aa66a306fbbe8f50eed998))
ROM_END

ROM_START(pb_l3)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("u26-l1.rom", 0x4000, 0x4000, CRC(e3b94ca4) SHA1(1db2acb025941cc165cc7ec70a160e07ab1eeb2e))
	ROM_LOAD("u27-l3.rom", 0x8000, 0x8000, CRC(6f40ee84) SHA1(85453137e3fdb1e422e3903dd053e04c9f2b9607))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u21.l1", 0x18000, 0x8000, CRC(3eab88d9) SHA1(667e3b675e2ae8fec6a6faddb9b0dd5531d64f8f))
	ROM_LOAD("pbot_u22.l1", 0x10000, 0x8000, CRC(a2d2c9cb) SHA1(46437dc54538f1626caf41a2818ddcf8000c44e4))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u4.l1", 0x10000, 0x8000, CRC(de5926bd) SHA1(3d111e27c5f0c8c0afc5fe5cc45bf77c12b69228))
	ROM_LOAD("pbot_u19.l1", 0x18000, 0x8000, CRC(40eb4e9f) SHA1(07b0557b35599a2dd5aa66a306fbbe8f50eed998))
ROM_END

ROM_START(pb_p4)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("u26-l2.rom", 0x4000, 0x4000, CRC(e3b94ca4) SHA1(1db2acb025941cc165cc7ec70a160e07ab1eeb2e))
	ROM_LOAD("u27_p4.bin", 0x8000, 0x8000, CRC(fbe2c466) SHA1(ac6c8f953b00e0ec7626cd1ccf4e16851ab905d0))

	ROM_REGION(0x20000, "audiocpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u21.l1", 0x18000, 0x8000, CRC(3eab88d9) SHA1(667e3b675e2ae8fec6a6faddb9b0dd5531d64f8f))
	ROM_LOAD("pbot_u22.l1", 0x10000, 0x8000, CRC(a2d2c9cb) SHA1(46437dc54538f1626caf41a2818ddcf8000c44e4))

	ROM_REGION(0x30000, "bgcpu", ROMREGION_ERASEFF)
	ROM_LOAD("pbot_u4.l1", 0x10000, 0x8000, CRC(de5926bd) SHA1(3d111e27c5f0c8c0afc5fe5cc45bf77c12b69228))
	ROM_LOAD("pbot_u19.l1", 0x18000, 0x8000, CRC(40eb4e9f) SHA1(07b0557b35599a2dd5aa66a306fbbe8f50eed998))
ROM_END

GAME(1987, f14_l1,   0,       s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "F-14 Tomcat (L-1)", MACHINE_IS_SKELETON_MECHANICAL)
GAME(1987, f14_p3,   f14_l1,  s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "F-14 Tomcat (P-3)", MACHINE_IS_SKELETON_MECHANICAL)
GAME(1987, f14_p4,   f14_l1,  s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "F-14 Tomcat (P-4)", MACHINE_IS_SKELETON_MECHANICAL)
GAME(1987, f14_p5,   f14_l1,  s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "F-14 Tomcat (P-5)", MACHINE_IS_SKELETON_MECHANICAL)
GAME(1987, fire_l3,  0,       s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "Fire! (L-3)",       MACHINE_IS_SKELETON_MECHANICAL)
GAME(1987, fire_l2,  fire_l3, s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "Fire! (L-2)",       MACHINE_IS_SKELETON_MECHANICAL)
GAME(1987, milln_l3, 0,       s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "Millionaire (L-3)", MACHINE_IS_SKELETON_MECHANICAL)
GAME(1986, pb_l5,    0,       s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "Pin-Bot (L-5)",     MACHINE_IS_SKELETON_MECHANICAL)
GAME(1986, pb_l1,    pb_l5,   s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "Pin-Bot (L-1)",     MACHINE_IS_SKELETON_MECHANICAL)
GAME(1986, pb_l2,    pb_l5,   s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "Pin-Bot (L-2)",     MACHINE_IS_SKELETON_MECHANICAL)
GAME(1986, pb_l3,    pb_l5,   s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "Pin-Bot (L-3)",     MACHINE_IS_SKELETON_MECHANICAL)
GAME(1986, pb_p4,    pb_l5,   s11a, s11a, s11a_state, init_s11a, ROT0, "Williams", "Pin-Bot (P-4)",     MACHINE_IS_SKELETON_MECHANICAL)
