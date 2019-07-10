// license:BSD-3-Clause
// copyright-holders:hap, Angelo Salese, Roberto Fresca
/***************************************************************************

KUNG-FU ROUSHI
(c)1987 NAMCO

6809         (4MHz?)
RAM 2016x1
MSM5205 x 2  (2 SPEAKERS)
8255    x 2
custom chips : none

KR1.BIN : PROGRAM
KR2-KR6 : VOICE

50pin cable to 7seg&lamp control board.

Information:
http://www.wshin.com/games/review/ka/kung-fu-roushi.htm
http://www.youtube.com/watch?v=ssEfw-RbSjs
http://www.youtube.com/watch?v=1YacVjpUG8g

---------------------------------------------------------------------------

Game Panel:

tokuten                                       honjitsu yuuryoukiroku
(score)                                       (today's best scores)
XX                                            #1 XX points
                                              #2 XX points
dankai                                        #3 XX points
(level)
1 2 3 4 5

hannoujikan        heikinhannoujikan          shuuryou
(reaction time)    (average reaction time)    (finished, complete)
X.XX seconds       X.XX seconds


Control Panel:

washi no kakegoe no toori ni botan wo osunojazoi!
(hit the buttons as I call them out)

over 8, or 10 points --> level 1
"   16, or 20 "      --> level 2
"   28, or 35 "      --> level 3
"   42, or 52 "      --> level 4
"   52, or 73(?) "   --> level 5
menkyokaiden(full certification)

The 4 buttons are labeled:
mae(forward), migi(right), ushiro(back), hidari(left)


***************************************************************************/

#include "emu.h"
#include "cpu/m6809/m6809.h"
#include "machine/i8255.h"
#include "sound/msm5205.h"
#include "speaker.h"

#include "kungfur.lh"


class kungfur_state : public driver_device
{
public:
	kungfur_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_adpcm1(*this, "adpcm1"),
		m_adpcm2(*this, "adpcm2"),
		m_digits(*this, "digit%u", 0U),
		m_lamps(*this, "lamp%u", 0U)
	{ }

	void kungfur(machine_config &config);

private:
	DECLARE_WRITE8_MEMBER(kungfur_output_w);
	DECLARE_WRITE8_MEMBER(kungfur_latch1_w);
	DECLARE_WRITE8_MEMBER(kungfur_latch2_w);
	DECLARE_WRITE8_MEMBER(kungfur_latch3_w);
	DECLARE_WRITE8_MEMBER(kungfur_control_w);
	DECLARE_WRITE8_MEMBER(kungfur_adpcm1_w);
	DECLARE_WRITE8_MEMBER(kungfur_adpcm2_w);
	INTERRUPT_GEN_MEMBER(kungfur_irq);
	DECLARE_WRITE_LINE_MEMBER(kfr_adpcm1_int);
	DECLARE_WRITE_LINE_MEMBER(kfr_adpcm2_int);
	void kungfur_map(address_map &map);

	uint8_t m_latch[3];
	uint8_t m_control;
	uint32_t m_adpcm_pos[2];
	uint8_t m_adpcm_data[2];
	uint8_t m_adpcm_sel[2];
	virtual void machine_start() override;
	virtual void machine_reset() override;
	required_device<cpu_device> m_maincpu;
	required_device<msm5205_device> m_adpcm1;
	required_device<msm5205_device> m_adpcm2;
	output_finder<14> m_digits;
	output_finder<8> m_lamps;
};


INTERRUPT_GEN_MEMBER(kungfur_state::kungfur_irq)
{
	if (m_control & 0x10)
		device.execute().set_input_line(M6809_IRQ_LINE, ASSERT_LINE);
}


/***************************************************************************

  I/O

***************************************************************************/

WRITE8_MEMBER(kungfur_state::kungfur_output_w)
{
	// d0-d2: output led7seg
	static const u8 lut_digits[24] =
	{
		0, 2, 4, 6, 9, 12,14,0,
		0, 1, 3, 5, 8, 11,13,0,
		0, 0, 0, 0, 7, 10,0, 0
	};
	for (u8 i = 0; i < 3; i++)
	{
		u8 offs = i << 3 | (data & 7);
		if (lut_digits[offs])
			m_digits[lut_digits[offs] - 1] = m_latch[i];
	}

	// 2.6 goes to level lamps
	if ((data & 7) == 6)
	{
		for (u8 i = 0; i < 5; i++)
			m_lamps[i] = BIT(m_latch[2], i);
	}

	// d7: game-over lamp, d3-d4: marquee lamps
	m_lamps[5] = BIT(data, 7);
	m_lamps[6] = BIT(data, 3);
	m_lamps[7] = BIT(data, 4);

	// d5: N/C?
	// d6: coincounter
	machine().bookkeeping().coin_counter_w(0, data & 0x40);
}


// lamp output latches
WRITE8_MEMBER(kungfur_state::kungfur_latch1_w)
{
	m_latch[0] = data;
}

WRITE8_MEMBER(kungfur_state::kungfur_latch2_w)
{
	m_latch[1] = data;
}

WRITE8_MEMBER(kungfur_state::kungfur_latch3_w)
{
	m_latch[2] = data;
}


WRITE8_MEMBER(kungfur_state::kungfur_control_w)
{
	// d0-d3: N/C
	// d4: irq ack
	if (~data & 0x10)
		m_maincpu->set_input_line(M6809_IRQ_LINE, CLEAR_LINE);

	// d5: ?
	// d6-d7: sound trigger (edge)
	if ((data ^ m_control) & 0x40)
	{
		m_adpcm1->reset_w(BIT(data, 6));
		m_adpcm_pos[0] = m_adpcm_data[0] * 0x400;
		m_adpcm_sel[0] = 0;
	}
	if ((data ^ m_control) & 0x80)
	{
		m_adpcm2->reset_w(BIT(data, 7));
		m_adpcm_pos[1] = m_adpcm_data[1] * 0x400;
		m_adpcm_sel[1] = 0;
	}

	m_control = data;
}

// adpcm latches
WRITE8_MEMBER(kungfur_state::kungfur_adpcm1_w)
{
	m_adpcm_data[0] = data;
}

WRITE8_MEMBER(kungfur_state::kungfur_adpcm2_w)
{
	m_adpcm_data[1] = data;
}

// adpcm callbacks
WRITE_LINE_MEMBER(kungfur_state::kfr_adpcm1_int)
{
	uint8_t *ROM = memregion("adpcm1")->base();
	uint8_t data = ROM[m_adpcm_pos[0] & 0x1ffff];

	m_adpcm1->write_data(m_adpcm_sel[0] ? data & 0xf : data >> 4 & 0xf);
	m_adpcm_pos[0] += m_adpcm_sel[0];
	m_adpcm_sel[0] ^= 1;
}

WRITE_LINE_MEMBER(kungfur_state::kfr_adpcm2_int)
{
	uint8_t *ROM = memregion("adpcm2")->base();
	uint8_t data = ROM[m_adpcm_pos[1] & 0x3ffff];

	m_adpcm2->write_data(m_adpcm_sel[1] ? data & 0xf : data >> 4 & 0xf);
	m_adpcm_pos[1] += m_adpcm_sel[1];
	m_adpcm_sel[1] ^= 1;
}


void kungfur_state::kungfur_map(address_map &map)
{
	map(0x0000, 0x07ff).ram();
	map(0x4000, 0x4000).w(FUNC(kungfur_state::kungfur_adpcm1_w));
	map(0x4004, 0x4004).w(FUNC(kungfur_state::kungfur_adpcm2_w));
	map(0x4008, 0x400b).rw("ppi8255_0", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x400c, 0x400f).rw("ppi8255_1", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0xc000, 0xffff).rom();
}


/***************************************************************************

  Inputs

***************************************************************************/

static INPUT_PORTS_START( kungfur )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_16WAY PORT_NAME("Migi (Right)")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  ) PORT_16WAY PORT_NAME("Hidari (Left)")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  ) PORT_16WAY PORT_NAME("Ushiro (Back)")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    ) PORT_16WAY PORT_NAME("Mae (Front)")

	PORT_START("IN1")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING( 0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING( 0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING( 0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING( 0x03, DEF_STR( 1C_2C ) )
//  PORT_DIPSETTING( 0x01, DEF_STR( 1C_2C ) ) // dupe
	PORT_DIPSETTING( 0x02, DEF_STR( 1C_3C ) )
//  PORT_DIPSETTING( 0x00, DEF_STR( 1C_3C ) ) // dupe
//  PORT_DIPSETTING( 0x04, DEF_STR( 0C_0C ) ) // invalid
	PORT_SERVICE( 0x08, IP_ACTIVE_LOW )
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


/***************************************************************************

  Machine Config

***************************************************************************/

void kungfur_state::machine_start()
{
	m_digits.resolve();
	m_lamps.resolve();

	save_item(NAME(m_control));
	save_item(NAME(m_latch));

	save_item(NAME(m_adpcm_pos));
	save_item(NAME(m_adpcm_data));
	save_item(NAME(m_adpcm_sel));
}

void kungfur_state::machine_reset()
{
	m_control = 0;
}

void kungfur_state::kungfur(machine_config &config)
{
	/* basic machine hardware */
	M6809(config, m_maincpu, 8000000/2);    // 4MHz?
	m_maincpu->set_addrmap(AS_PROGRAM, &kungfur_state::kungfur_map);
	m_maincpu->set_periodic_int(FUNC(kungfur_state::kungfur_irq), attotime::from_hz(975));  // close approximation

	i8255_device &ppi0(I8255A(config, "ppi8255_0"));
	// $4008 - always $83 (PPI mode 0, ports B & lower C as input)
	ppi0.out_pa_callback().set(FUNC(kungfur_state::kungfur_output_w));
	ppi0.in_pb_callback().set_ioport("IN0");
	ppi0.in_pc_callback().set_ioport("IN1");
	ppi0.out_pc_callback().set(FUNC(kungfur_state::kungfur_control_w));

	i8255_device &ppi1(I8255A(config, "ppi8255_1"));
	// $400c - always $80 (PPI mode 0, all ports as output)
	ppi1.out_pa_callback().set(FUNC(kungfur_state::kungfur_latch1_w));
	ppi1.out_pb_callback().set(FUNC(kungfur_state::kungfur_latch2_w));
	ppi1.out_pc_callback().set(FUNC(kungfur_state::kungfur_latch3_w));

	/* no video! */

	/* sound hardware */
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();
	MSM5205(config, m_adpcm1, XTAL(384'000));   // clock verified with recording
	m_adpcm1->vck_legacy_callback().set(FUNC(kungfur_state::kfr_adpcm1_int));
	m_adpcm1->set_prescaler_selector(msm5205_device::S48_4B);
	m_adpcm1->add_route(ALL_OUTPUTS, "lspeaker", 1.0);

	MSM5205(config, m_adpcm2, XTAL(384'000));   // clock verified with recording
	m_adpcm2->vck_legacy_callback().set(FUNC(kungfur_state::kfr_adpcm2_int));
	m_adpcm2->set_prescaler_selector(msm5205_device::S48_4B);
	m_adpcm2->add_route(ALL_OUTPUTS, "rspeaker", 1.0);
}


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( kungfur )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "kr1.bin",   0x0c000, 0x04000, CRC(f5b93cc7) SHA1(ed962915aeafea823a6562e6f284a88422f09a08) )

	ROM_REGION( 0x20000, "adpcm1", 0 )
	ROM_LOAD( "kr2.bin",   0x00000, 0x10000, CRC(13f5eba8) SHA1(a3ae2d54ec60d48bfff6192e61033ec583e3603f) )
	ROM_LOAD( "kr3.bin",   0x10000, 0x10000, CRC(05fd1301) SHA1(6871d872315ffb025fea7d2ccd9a203863dc142d) )

	ROM_REGION( 0x40000, "adpcm2", 0 )
	ROM_LOAD( "kr4.bin",   0x00000, 0x10000, CRC(58929279) SHA1(d90f68dd8cf2ddc5e73ed40eb31ebbb0be7e35a4) )
	ROM_LOAD( "kr5.bin",   0x10000, 0x10000, CRC(31ed39c8) SHA1(8da50b2183a287fe3a41ec13078aff7fb40c43a3) )
	ROM_LOAD( "kr6.bin",   0x20000, 0x10000, CRC(9ea75d4a) SHA1(57445ccb961acb11a25cdac81f2e543d92bcb7f9) )
ROM_END

GAMEL( 1987, kungfur, 0, kungfur,  kungfur, kungfur_state, empty_init, ROT0, "Namco", "Kung-Fu Roushi", MACHINE_SUPPORTS_SAVE, layout_kungfur )
