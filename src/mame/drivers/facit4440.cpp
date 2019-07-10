// license:BSD-3-Clause
// copyright-holders:AJR
/****************************************************************************

    Preliminary driver for Facit 4440 Twist terminal.

    The “Twist” sobriquet refers to the adjustable orientation of the
    monitor, whose normally black-on-white display has the dimensions of a
    sheet of A4 paper. It displays 24 lines of 80 characters in landscape
    mode and 72 lines of rather smaller characters in portrait mode.

    No user manual for this terminal has been found. The layout of the
    keyboard is also unclear, not to mention the serial protocol.

    Nonvolatile settings are configured through VT100-style setup screens.
    Several non-ANSI emulation modes are also available.

    An OEM customized version was sold by Norsk Data AS as the ND 319 Twist.

    The timing of the display circuit seems a bit uncertain. Promotional
    information claims that it has a 40 MHz bandwidth, but this may be only
    a nominal maximum rating since the only high-frequency oscillator seen
    on the PCB is 32 MHz. It is unclear how the CRTC parameters and this
    value can produce the documented 65 Hz refresh rate. The actual circuit
    also generates alarming levels of electromagnetic radiation.

****************************************************************************/

#include "emu.h"
//#include "bus/rs232/rs232.h"
#include "cpu/z80/z80.h"
#include "machine/clock.h"
#include "machine/er1400.h"
#include "machine/z80ctc.h"
#include "machine/z80dart.h"
#include "video/mc6845.h"
#include "screen.h"

class facit4440_state : public driver_device
{
public:
	facit4440_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_earom(*this, "earom%u", 0U)
		, m_crtc(*this, "crtc")
		, m_test(*this, "TEST")
		, m_orientation(*this, "ORIENTATION")
		, m_chargen(*this, "chargen")
		, m_scanprom(*this, "scanprom")
		, m_videoram(*this, "videoram")
	{
	}

	void facit4440(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	void earom_latch_w(u8 data);
	void control_2000_w(u8 data);
	u8 misc_status_r();
	void control_6000_w(u8 data);
	void control_a000_w(u8 data);

	DECLARE_WRITE_LINE_MEMBER(vsync_w);

	MC6845_UPDATE_ROW(update_row);

	void mem_map(address_map &map);
	void io_map(address_map &map);

	required_device<z80_device> m_maincpu;
	required_device_array<er1400_device, 2> m_earom;
	required_device<mc6845_device> m_crtc;

	required_ioport m_test;
	required_ioport m_orientation;

	required_region_ptr<u8> m_chargen;
	required_region_ptr<u8> m_scanprom;
	required_shared_ptr<u8> m_videoram;

	u8 m_control_latch[3];
};

void facit4440_state::earom_latch_w(u8 data)
{
	// SN74LS174 latch + SN7406 inverter

	// Prevent outputs from interfering with data reads
	if (!BIT(data, 2))
		data &= 0xfc;

	// FIXME: clock must be written first here due to data/control setup time
	m_earom[0]->clock_w(BIT(data, 5));
	m_earom[1]->clock_w(BIT(data, 5));

	m_earom[0]->data_w(BIT(data, 1));
	m_earom[1]->data_w(BIT(data, 0));

	for (auto &earom : m_earom)
	{
		earom->c2_w(BIT(data, 2));
		earom->c1_w(BIT(data, 3));
		earom->c3_w(BIT(data, 4));
	}
}

void facit4440_state::control_2000_w(u8 data)
{
	m_control_latch[0] = data;

	if (!BIT(m_control_latch[0], 5))
		m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}

void facit4440_state::control_6000_w(u8 data)
{
	m_control_latch[1] = data;
}

void facit4440_state::control_a000_w(u8 data)
{
	m_control_latch[2] = data;
}

u8 facit4440_state::misc_status_r()
{
	u8 status = m_test->read() << 6;

	status |= m_earom[0]->data_r() << 4;
	status |= m_earom[1]->data_r() << 3;

	status |= m_crtc->vsync_r() << 1;
	status |= m_orientation->read();

	return status;
}

WRITE_LINE_MEMBER(facit4440_state::vsync_w)
{
	if (state && BIT(m_control_latch[0], 5))
		m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
}

MC6845_UPDATE_ROW(facit4440_state::update_row)
{
	offs_t base = ma / 5 * 6;
	u32 *px = &bitmap.pix32(y);

	for (int i = 0; i < x_count; i++)
	{
		u8 chr = m_videoram[(base + i) & 0x1fff];
		rgb_t fg = rgb_t::white();
		rgb_t bg = rgb_t::black();

		// TODO: double-width scans based on attributes
		u8 scan = m_scanprom[ra | (BIT(m_control_latch[1], 7) ? 0x20 : 0x00)];
		u8 dots = m_chargen[(u16(chr) << 5) | (scan & 0x1f)];

		for (int n = 8; n > 0; n--, dots <<= 1)
			*px++ = BIT(dots, 7) ? fg : bg;
	}
}

void facit4440_state::mem_map(address_map &map)
{
	map(0x0000, 0x7fff).rom().region("mainprg", 0);
	map(0x0000, 0x0000).w(FUNC(facit4440_state::earom_latch_w));
	map(0x2000, 0x2000).w(FUNC(facit4440_state::control_2000_w));
	map(0x4000, 0x4000).mirror(0x1ffe).w("crtc", FUNC(mc6845_device::address_w));
	map(0x4001, 0x4001).mirror(0x1ffe).w("crtc", FUNC(mc6845_device::register_w));
	map(0x6000, 0x6000).w(FUNC(facit4440_state::control_6000_w));
	map(0x8000, 0x8000).r(FUNC(facit4440_state::misc_status_r));
	map(0x8000, 0x8050).nopw();
	map(0xa000, 0xbfff).rom().region("testprg", 0);
	map(0xa000, 0xa000).w(FUNC(facit4440_state::control_a000_w));
	map(0xc000, 0xdfff).ram().share("videoram"); // 4x TMM2016AP-90
	map(0xe000, 0xffff).ram(); // 4x TMM2016AP-90
}

void facit4440_state::io_map(address_map &map)
{
	map.global_mask(0xff);
	map(0xec, 0xef).rw("kbdart", FUNC(z80dart_device::cd_ba_r), FUNC(z80dart_device::cd_ba_w));
	map(0xf4, 0xf7).rw("iodart", FUNC(z80dart_device::cd_ba_r), FUNC(z80dart_device::cd_ba_w));
	map(0xf8, 0xfb).rw("ctc", FUNC(z80ctc_device::read), FUNC(z80ctc_device::write));
}

void facit4440_state::machine_start()
{
	m_control_latch[0] = m_control_latch[1] = m_control_latch[2] = 0;

	save_item(NAME(m_control_latch));
}

static INPUT_PORTS_START(facit4440)
	PORT_START("TEST")
	PORT_CONFNAME(3, 0, "Test Mode")
	PORT_CONFSETTING(0, DEF_STR(Off))
	PORT_CONFSETTING(3, DEF_STR(On))
	PORT_CONFSETTING(2, "Burn In")

	PORT_START("ORIENTATION")
	PORT_CONFNAME(1, 1, "Orientation")
	PORT_CONFSETTING(0, "Portrait")
	PORT_CONFSETTING(1, "Landscape")
INPUT_PORTS_END

static const z80_daisy_config daisy_chain[] =
{
	{ "ctc" },
	{ "iodart" },
	{ "kbdart" },
	{ nullptr }
};

void facit4440_state::facit4440(machine_config &config)
{
	constexpr u32 CH_WIDTH = 8;
	constexpr u32 FAKE_DOT_CLOCK = 65 * 103 * 621 * CH_WIDTH;

	Z80(config, m_maincpu, 32_MHz_XTAL / 8); // clock unknown
	m_maincpu->set_addrmap(AS_PROGRAM, &facit4440_state::mem_map);
	m_maincpu->set_addrmap(AS_IO, &facit4440_state::io_map);
	m_maincpu->set_daisy_config(daisy_chain);

	ER1400(config, m_earom[0]); // M5G1400P
	ER1400(config, m_earom[1]); // M5G1400P

	z80ctc_device &ctc(Z80CTC(config, "ctc", 32_MHz_XTAL / 8));
	ctc.intr_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
	ctc.set_clk<0>(2.4576_MHz_XTAL / 4);
	ctc.set_clk<1>(2.4576_MHz_XTAL / 4);
	ctc.set_clk<2>(2.4576_MHz_XTAL / 4);
	ctc.zc_callback<0>().set("iodart", FUNC(z80dart_device::txca_w));
	ctc.zc_callback<1>().set("iodart", FUNC(z80dart_device::rxca_w));
	ctc.zc_callback<2>().set("iodart", FUNC(z80dart_device::txcb_w));
	ctc.zc_callback<2>().append("iodart", FUNC(z80dart_device::rxcb_w));

	clock_device &keybclk(CLOCK(config, "keybclk", 2.4576_MHz_XTAL / 32)); // unclear
	keybclk.signal_handler().set("kbdart", FUNC(z80dart_device::txca_w));
	keybclk.signal_handler().append("kbdart", FUNC(z80dart_device::rxca_w));

	z80dart_device &iodart(Z80DART(config, "iodart", 32_MHz_XTAL / 8)); // Z80ADART
	iodart.out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
	iodart.out_txda_callback().set("iodart", FUNC(z80dart_device::rxa_w)); // temporary loopback for testing
	iodart.out_rtsa_callback().set("iodart", FUNC(z80dart_device::ctsa_w)); // temporary loopback for testing
	iodart.out_dtra_callback().set("iodart", FUNC(z80dart_device::dcdb_w)); // temporary loopback for testing
	iodart.out_txdb_callback().set("iodart", FUNC(z80dart_device::rxb_w)); // temporary loopback for testing

	z80dart_device &kbdart(Z80DART(config, "kbdart", 32_MHz_XTAL / 8)); // Z80ADART
	kbdart.out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);
	kbdart.out_txda_callback().set("kbdart", FUNC(z80dart_device::rxa_w)); // FIXME: serial keyboard needed here
	// Channel B is not used

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_raw(FAKE_DOT_CLOCK, 103 * CH_WIDTH, 0, 80 * CH_WIDTH, 621, 0, 500);
	//screen.set_raw(FAKE_DOT_CLOCK, 103 * CH_WIDTH, 0, 80 * CH_WIDTH, 621, 0, 560);
	screen.set_screen_update("crtc", FUNC(mc6845_device::screen_update));

	MC6845(config, m_crtc, FAKE_DOT_CLOCK / CH_WIDTH); // HD46505SP-2
	m_crtc->set_char_width(CH_WIDTH);
	m_crtc->set_show_border_area(false);
	m_crtc->set_update_row_callback(FUNC(facit4440_state::update_row), this);
	m_crtc->out_hsync_callback().set("ctc", FUNC(z80ctc_device::trg3));
	m_crtc->out_vsync_callback().set(FUNC(facit4440_state::vsync_w));
}


ROM_START(facit4440)
	ROM_REGION(0x8000, "mainprg", 0)
	ROM_LOAD("rom7.bin", 0x0000, 0x4000, CRC(a8da2b11) SHA1(4436ef14c29ae299f7bc338748158771c02d02a9))
	ROM_LOAD("rom6.bin", 0x4000, 0x4000, CRC(790b7642) SHA1(688a80cbf011e5c14f501e11fe0e3bf64a85bbd7))

	ROM_REGION(0x2000, "testprg", 0)
	ROM_LOAD("rom5.bin", 0x0000, 0x2000, CRC(715d02b6) SHA1(e304718dbdc8867ac01909fd2d027e5014a8c4f9))

	ROM_REGION(0x3000, "chargen", 0) // order unknown
	ROM_LOAD("rom1.bin", 0x0000, 0x1000, CRC(b503c173) SHA1(209bf59e2e9953179d04c4e768fc41574e039d36))
	ROM_LOAD("rom3.bin", 0x1000, 0x1000, CRC(a55a25d9) SHA1(c0d321e65f214adee01bf5f8c495b2518fa31b7b))
	ROM_LOAD("rom4.bin", 0x2000, 0x1000, CRC(52004ef8) SHA1(50d6e2eb48f60db3a3c9d206fc40d3294b6adc0e))

	ROM_REGION(0x0800, "scanprom", 0)
	ROM_LOAD("rom2.bin", 0x0000, 0x0800, CRC(9e1a190c) SHA1(fb08ee806f1056bcdfb5b08ea85995e1d3d01298))
ROM_END

COMP(1984, facit4440, 0, 0, facit4440, facit4440, facit4440_state, empty_init, "Facit", "4440 Twist (30M-F1)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS | MACHINE_NO_SOUND)
