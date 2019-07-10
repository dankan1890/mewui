// license:BSD-3-Clause
// copyright-holders:Robbbert
/************************************************************************************************

        Unior

    2009-05-12 Skeleton driver.
    2013-10-09 Added DMA and CRTC
    2013-10-11 Added PPI, PIT, UART, sound

    Some info obtained from EMU-80.
    The schematic is difficult to read, and some code is guesswork.

The monitor will only allow certain characters to be typed, thus the
modifier keys appear to do nothing. There is no need to use the enter
key; using spacebar and the correct parameters is enough.

If you press Shift, indicators for numlock and capslock will appear.

Monitor commands:
C - ?
D - hex dump
E - save
F - fill memory
G - go
H - set register
I - load
J - modify memory
K - ?
L - list registers
M - ?

ToDo:
- Cassette (coded but unable to test as the i8251 device needs work to support syndet)
- Colour

**********************************************************************************************/

#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "machine/i8251.h"
#include "machine/pit8253.h"
#include "machine/i8255.h"
#include "machine/i8257.h"
#include "video/i8275.h"
#include "imagedev/cassette.h"
#include "sound/spkrdev.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"


class unior_state : public driver_device
{
public:
	unior_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_pit(*this, "pit")
		, m_dma(*this, "dma")
		, m_uart(*this, "uart")
		, m_cass(*this, "cassette")
		, m_palette(*this, "palette")
		, m_p_chargen(*this, "chargen")
		, m_p_vram(*this, "vram")
		, m_io_keyboard(*this, "X%d", 0)
	{ }

	void unior(machine_config &config);

private:
	DECLARE_WRITE8_MEMBER(vram_w);
	DECLARE_WRITE8_MEMBER(scroll_w);
	DECLARE_READ8_MEMBER(ppi0_b_r);
	DECLARE_WRITE8_MEMBER(ppi0_b_w);
	DECLARE_READ8_MEMBER(ppi1_a_r);
	DECLARE_READ8_MEMBER(ppi1_b_r);
	DECLARE_READ8_MEMBER(ppi1_c_r);
	DECLARE_WRITE8_MEMBER(ppi1_a_w);
	DECLARE_WRITE8_MEMBER(ppi1_c_w);
	DECLARE_WRITE_LINE_MEMBER(hrq_w);
	DECLARE_WRITE_LINE_MEMBER(ctc_z1_w);
	void unior_palette(palette_device &palette) const;
	DECLARE_READ8_MEMBER(dma_r);
	I8275_DRAW_CHARACTER_MEMBER(display_pixels);

	void io_map(address_map &map);
	void mem_map(address_map &map);

	uint8_t m_4c;
	uint8_t m_4e;
	bool m_txe, m_txd;
	virtual void machine_reset() override;
	required_device<cpu_device> m_maincpu;
	required_device<pit8253_device> m_pit;
	required_device<i8257_device> m_dma;
	required_device<i8251_device> m_uart;
	required_device<cassette_image_device> m_cass;
	required_device<palette_device> m_palette;
	required_region_ptr<u8> m_p_chargen;
	required_region_ptr<u8> m_p_vram;
	required_ioport_array<11> m_io_keyboard;
};

void unior_state::mem_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0xf7ff).ram();
	map(0xf800, 0xffff).rom().w(FUNC(unior_state::vram_w)); // main video
}

void unior_state::io_map(address_map &map)
{
	map.unmap_value_high();
	map.global_mask(0xff);
	map(0x30, 0x38).rw(m_dma, FUNC(i8257_device::read), FUNC(i8257_device::write)); // dma data
	map(0x3c, 0x3f).rw("ppi0", FUNC(i8255_device::read), FUNC(i8255_device::write)); // cassette player control
	map(0x4c, 0x4f).rw("ppi1", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x50, 0x50).w(FUNC(unior_state::scroll_w));
	map(0x60, 0x61).rw("crtc", FUNC(i8275_device::read), FUNC(i8275_device::write));
	map(0xdc, 0xdf).rw(m_pit, FUNC(pit8253_device::read), FUNC(pit8253_device::write));
	map(0xec, 0xed).rw(m_uart, FUNC(i8251_device::read), FUNC(i8251_device::write));
}

/* Input ports */
static INPUT_PORTS_START( unior )
	PORT_START("X0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNUSED) // nothing
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNUSED) // nothing
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6_PAD)

	PORT_START("X1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC) PORT_CHAR(27) // cancel input
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TAB) PORT_CHAR(9) // Russian little M
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rus/Lat") PORT_CODE(KEYCODE_LALT) PORT_CODE(KEYCODE_RALT)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("NUMLK") PORT_CODE(KEYCODE_NUMLOCK) // switches 1st indicator between N and B - numlock? rctrl
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_PLUS_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ASTERISK)

	PORT_START("X2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON) PORT_CHAR(';') // ;
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CAPS") PORT_CODE(KEYCODE_CAPSLOCK) // switches 2nd indicator between U and B - capslock? rshift
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_DEL_PAD)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER_PAD)

	PORT_START("X3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F1) // A
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR('^') // ^ (')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3_PAD)

	PORT_START("X4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F2) // B
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F7) // H
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F8) // I

	PORT_START("X5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F3) // C
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("BSPACE") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F6) // G
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("LF") PORT_CODE(KEYCODE_HOME) PORT_CHAR(10) // line feed?

	PORT_START("X6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR(0xA4)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P) PORT_CHAR('P')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I) PORT_CHAR('I') // I then hangs
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_RIGHT) // Russian A
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH) // ? (/)

	PORT_START("X7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F4) // D
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN) // Russian bl
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_UP) // Russian signpost
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED) // } (<>)

	PORT_START("X8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F5) // E
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O) PORT_CHAR('O')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(". >") PORT_CODE(KEYCODE_STOP) PORT_CHAR('>') // >
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("= :") PORT_CODE(KEYCODE_EQUALS) PORT_CHAR(':') // :
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("- =") PORT_CODE(KEYCODE_MINUS) PORT_CHAR('=') // =

	PORT_START("X9")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('[')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT) // Russian U
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0) PORT_CHAR('0')

	PORT_START("X10")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(']')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TILDE) PORT_CHAR('@') // (`)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA) PORT_CHAR('<') // <
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR(')')
INPUT_PORTS_END


/*************************************************

    Video

*************************************************/

/* F4 Character Displayer */
static const gfx_layout unior_charlayout =
{
	8, 8,                   /* 8 x 8 characters */
	256,                    /* 256 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{  0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8                 /* every char takes 8 bytes */
};

static GFXDECODE_START( gfx_unior )
	GFXDECODE_ENTRY( "chargen", 0x0000, unior_charlayout, 0, 1 )
GFXDECODE_END

WRITE8_MEMBER( unior_state::vram_w )
{
	m_p_vram[offset] = data;
}

// pulses a 1 to scroll
WRITE8_MEMBER( unior_state::scroll_w )
{
	if (data)
		memmove(m_p_vram, m_p_vram+80, 24*80);
}

I8275_DRAW_CHARACTER_MEMBER(unior_state::display_pixels)
{
	const rgb_t *palette = m_palette->palette()->entry_list_raw();
	uint8_t gfx = m_p_chargen[(linecount & 7) | (charcode << 3)];

	if (vsp)
		gfx = 0;

	if (lten)
		gfx = 0xff;

	if (rvv)
		gfx ^= 0xff;

	for(uint8_t i=0;i<6;i++)
		bitmap.pix32(y, x + i) = palette[BIT(gfx, 5-i) ? (hlgt ? 2 : 1) : 0];
}

static constexpr rgb_t unior_pens[3] =
{
	{ 0x00, 0x00, 0x00 }, // black
	{ 0xa0, 0xa0, 0xa0 }, // white
	{ 0xff, 0xff, 0xff }  // highlight
};

void unior_state::unior_palette(palette_device &palette) const
{
	palette.set_pen_colors(0, unior_pens);
}


/*************************************************

    i8255

*************************************************/
WRITE_LINE_MEMBER(unior_state::ctc_z1_w)
{
	// write
	if (m_txe)
		m_cass->output(1);
	else
		m_cass->output((m_txd ^ state) ? 1 : 0);

	m_uart->write_txc(state);

	// read - untested
	m_uart->write_rxd((m_cass->input() > 0.04) ? 1 : 0);
	m_uart->write_rxc(state);
}


READ8_MEMBER( unior_state::ppi0_b_r )
{
	return 0;
}

WRITE8_MEMBER( unior_state::ppi0_b_w )
{
}

READ8_MEMBER( unior_state::ppi1_a_r )
{
	return m_4c;
}

READ8_MEMBER( unior_state::ppi1_b_r )
{
	u8 t = m_4c & 15;
	if (t < 11)
		return m_io_keyboard[t]->read();
	else
		return 0xff;
}

READ8_MEMBER( unior_state::ppi1_c_r )
{
	return m_4e;
}

WRITE8_MEMBER( unior_state::ppi1_a_w )
{
	m_4c = data;
}

/*
d0,1,2 = connect to what might be a 74LS138, then to an external slot
d4 = speaker gate
d5 = unknown
d6 = connect to A7 of the palette prom
d7 = not used
*/
WRITE8_MEMBER( unior_state::ppi1_c_w )
{
	m_4e = data;
	m_pit->write_gate2(BIT(data, 4));
}

/*************************************************

    i8257

*************************************************/

READ8_MEMBER(unior_state::dma_r)
{
	if (offset < 0xf800)
		return m_maincpu->space(AS_PROGRAM).read_byte(offset);
	else
		return m_p_vram[offset & 0x7ff];
}

WRITE_LINE_MEMBER( unior_state::hrq_w )
{
	m_maincpu->set_input_line(INPUT_LINE_HALT, state);
	m_dma->hlda_w(state);
}


/*************************************************

    Machine

*************************************************/

void unior_state::machine_reset()
{
	m_maincpu->set_state_int(i8080_cpu_device::I8085_PC, 0xF800);
	m_uart->write_cts(0);
}

void unior_state::unior(machine_config &config)
{
	/* basic machine hardware */
	I8080(config, m_maincpu, XTAL(20'000'000) / 9);
	m_maincpu->set_addrmap(AS_PROGRAM, &unior_state::mem_map);
	m_maincpu->set_addrmap(AS_IO, &unior_state::io_map);

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(50);
	screen.set_size(640, 200);
	screen.set_visarea(0, 640-1, 0, 200-1);
	screen.set_screen_update("crtc", FUNC(i8275_device::screen_update));
	GFXDECODE(config, "gfxdecode", m_palette, gfx_unior);
	PALETTE(config, m_palette, FUNC(unior_state::unior_palette), 3);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
	SPEAKER_SOUND(config, "speaker").add_route(ALL_OUTPUTS, "mono", 0.50);

	CASSETTE(config, m_cass);
	m_cass->set_default_state(CASSETTE_STOPPED | CASSETTE_MOTOR_ENABLED | CASSETTE_SPEAKER_ENABLED);
	m_cass->add_route(ALL_OUTPUTS, "mono", 0.15);

	/* Devices */
	I8251(config, m_uart, 20_MHz_XTAL / 9);
	m_uart->txd_handler().set([this] (bool state) { m_txd = state; });
	m_uart->txempty_handler().set([this] (bool state) { m_txe = state; });

	PIT8253(config, m_pit, 0);
	m_pit->set_clk<0>(20_MHz_XTAL / 12);
	m_pit->set_clk<1>(20_MHz_XTAL / 9);
	m_pit->out_handler<1>().set(FUNC(unior_state::ctc_z1_w));
	m_pit->set_clk<2>(16_MHz_XTAL / 9 / 64); // unknown frequency
	m_pit->out_handler<2>().set("speaker", FUNC(speaker_sound_device::level_w));

	i8255_device &ppi0(I8255(config, "ppi0"));
	// ports a & c connect to an external slot
	ppi0.in_pb_callback().set(FUNC(unior_state::ppi0_b_r));
	ppi0.out_pb_callback().set(FUNC(unior_state::ppi0_b_w));

	i8255_device &ppi1(I8255(config, "ppi1"));
	// ports a & b are for the keyboard
	// port c operates various control lines for mostly unknown purposes
	ppi1.in_pa_callback().set(FUNC(unior_state::ppi1_a_r));
	ppi1.in_pb_callback().set(FUNC(unior_state::ppi1_b_r));
	ppi1.in_pc_callback().set(FUNC(unior_state::ppi1_c_r));
	ppi1.out_pa_callback().set(FUNC(unior_state::ppi1_a_w));
	ppi1.out_pc_callback().set(FUNC(unior_state::ppi1_c_w));

	I8257(config, m_dma, XTAL(20'000'000) / 9);
	m_dma->out_hrq_cb().set(FUNC(unior_state::hrq_w));
	m_dma->in_memr_cb().set(FUNC(unior_state::dma_r));
	m_dma->out_iow_cb<2>().set("crtc", FUNC(i8275_device::dack_w));

	i8275_device &crtc(I8275(config, "crtc", XTAL(20'000'000) / 12));
	crtc.set_character_width(6);
	crtc.set_display_callback(FUNC(unior_state::display_pixels), this);
	crtc.drq_wr_callback().set(m_dma, FUNC(i8257_device::dreq2_w));
	crtc.set_screen("screen");
}

/* ROM definition */
ROM_START( unior )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "unior.rom", 0xf800, 0x0800, CRC(23a347e8) SHA1(2ef3134e2f4a696c3b52a145fa5a2d4c3487194b))

	ROM_REGION( 0x0840, "chargen", ROMREGION_ERASEFF )
	ROM_LOAD( "unior.fnt",   0x0000, 0x0800, CRC(4f654828) SHA1(8c0ac11ea9679a439587952e4908940b67c4105e))
	// according to schematic this should be 256 bytes
	ROM_LOAD( "palette.rom", 0x0800, 0x0040, CRC(b4574ceb) SHA1(f7a82c61ab137de8f6a99b0c5acf3ac79291f26a))

	ROM_REGION( 0x0800, "vram", ROMREGION_ERASEFF )
ROM_END

/* Driver */

/*    YEAR  NAME   PARENT   COMPAT  MACHINE  INPUT  CLASS        INIT        COMPANY      FULLNAME  FLAGS */
COMP( 19??, unior, radio86, 0,      unior,   unior, unior_state, empty_init, "<unknown>", "Unior",  MACHINE_NOT_WORKING )
