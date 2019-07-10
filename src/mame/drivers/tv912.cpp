// license:BSD-3-Clause
// copyright-holders:AJR
/*******************************************************************************

    TeleVideo TVI-912/TVI-920 terminals

    This was the first series of terminals from TeleVideo. The models differed
    from each other in the number and pattern of keys on the nondetachable
    keyboard. Those with a B suffix had a TTY-style keyboard, while the C
    suffix indicated a typewriter-style keyboard. The TVI-920 added a row of
    function keys but was otherwise mostly identical to the TVI-912. The
    TVI-912C keyboard matrix and ribbon connector pinout are almost the same
    as in the TVI-910. The system self-test can be triggered by shorting two
    wires on the keyboard unit near the connector (TODO: figure out how to do
    this on the emulated system).

    Settings for these terminals are controlled by DIP switches, which are not
    read by the CPU except for those of S4 (which is usually left as a block of
    bare jumpers) and the half-duplex and 50/60 Hz switches of S2. Four to five
    of the S2 switches directly manipulate UART pins to select the number and
    framing of data bits. Position 8 (NB2) is deliberately disconnected on
    later revisions to restrict communications to 7 or 8 data bits, likely
    because of known incompatibilities between UART models in 5-bit data mode.
    (For reasons that are less clear, the 19,200 baud option is also commonly
    omitted from documentation.)

    The baud rate switches (S1/S3) are mounted vertically at the rear of the
    unit. As these are connected directly to the frequency outputs of the baud
    rate generation circuit (which consists mainly of the LS163s at A73, A70,
    A71 and A72), only one switch may be down (closed) at a time. (The nominal
    "75 baud" output of this circuit doubles as the beep sound frequency.)
    Originally the UART was connected to receive and transmit data at the
    printer rate when the printer was selected, even though it did not receive
    any data through the printer connector. Later board revisions fixed this
    by tying the signal selected by S1 to the UART's RCP; S3 could be similarly
    tied to TCP by modifying a couple of jumpers.

    The hardware appears to have been originally designed to access two 2316E
    program ROMs (alternatively 8316E or 2716), with the lower 2K at A50 and
    upper 2K at A49. These were ultimately merged into a single 2332 or 8332A
    at A49; several jumpers in the area could be inserted or removed to
    accommodate these different ROM types. At least the later A49B1 and A49C1
    mask ROMs (both MM52132 types) were apparently interchangeable between
    TVI-912B/C and TVI-920B/C. Part or all of the program ROM could also be
    made internal to the CPU by replacing the 8035 with a 8048, 8748 or 8049
    and grounding the EA pin by inserting a jumper at W1.

    Each displayed character nominally consists of 6 x 8 dots within a 7 x 10
    cell. However, the lowest two bits of each row of character data may be
    set to shift either the first three dots, the last three dots or both
    forward by half a dot, thus effectively doubling horizontal resolution.

*******************************************************************************/

#include "emu.h"
#include "cpu/mcs48/mcs48.h"
#include "bus/rs232/rs232.h"
#include "machine/ay31015.h"
#include "machine/bankdev.h"
#include "sound/beep.h"
#include "video/tms9927.h"
#include "screen.h"
#include "speaker.h"

#define TV912_CH_WIDTH 14
#define CHARSET_TEST 0

class tv912_state : public driver_device
{
public:
	tv912_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_crtc(*this, "crtc")
		, m_uart(*this, "uart")
		, m_rs232(*this, "rs232")
		, m_bankdev(*this, "bankdev")
		, m_beep(*this, "beep")
		, m_dispram_bank(*this, "dispram")
		, m_p_chargen(*this, "chargen")
		, m_keys(*this, "KEY%u", 0)
		, m_modem_baud(*this, "MODEMBAUD")
		, m_printer_baud(*this, "PRINTBAUD")
		, m_uart_control(*this, "UARTCTRL")
		, m_video_control(*this, "VIDEOCTRL")
		, m_modifiers(*this, "MODIFIERS")
		, m_half_duplex(*this, "HALFDUP")
		, m_jumpers(*this, "JUMPERS")
		, m_option(*this, "OPTION")
		, m_dtr(*this, "DTR")
		, m_baudgen_timer(nullptr)
	{ }

	void tv912(machine_config &config);

	DECLARE_INPUT_CHANGED_MEMBER(uart_settings_changed);

private:
	DECLARE_WRITE8_MEMBER(p1_w);
	DECLARE_READ8_MEMBER(p2_r);
	DECLARE_WRITE8_MEMBER(p2_w);
	DECLARE_READ8_MEMBER(crtc_r);
	DECLARE_WRITE8_MEMBER(crtc_w);
	DECLARE_READ8_MEMBER(uart_status_r);
	DECLARE_READ8_MEMBER(keyboard_r);
	DECLARE_WRITE8_MEMBER(output_40c);

	u32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	void bank_map(address_map &map);
	void io_map(address_map &map);
	void prog_map(address_map &map);

	enum
	{
		TIMER_BAUDGEN
	};

	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	required_device<cpu_device> m_maincpu;
	required_device<tms9927_device> m_crtc;
	required_device<ay51013_device> m_uart;
	required_device<rs232_port_device> m_rs232;
	required_device<address_map_bank_device> m_bankdev;
	required_device<beep_device> m_beep;
	required_memory_bank m_dispram_bank;
	required_region_ptr<u8> m_p_chargen;
	required_ioport_array<32> m_keys;
	required_ioport m_modem_baud;
	required_ioport m_printer_baud;
	required_ioport m_uart_control;
	required_ioport m_video_control;
	required_ioport m_modifiers;
	required_ioport m_half_duplex;
	required_ioport m_jumpers;
	required_ioport m_option;
	required_ioport m_dtr;

	emu_timer *m_baudgen_timer;

	bool m_force_blank;
	bool m_4hz_flasher;
	bool m_lpt_select;
	u8 m_keyboard_scan;
	std::unique_ptr<u8[]> m_dispram;
};

WRITE8_MEMBER(tv912_state::p1_w)
{
	m_keyboard_scan = data;
}

READ8_MEMBER(tv912_state::p2_r)
{
	ioport_value dup = m_half_duplex->read();

	// P27: -HALF DUPLEX
	u8 result = BIT(dup, 0) << 7;

	// P26: -PTR RDY

	// P25: -DCR
	if (!BIT(dup, 1))
		result |= m_rs232->dsr_r() << 5;
	if (!BIT(dup, 2))
		result |= m_rs232->dcd_r() << 5;

	return result | 0x5f;
}

WRITE8_MEMBER(tv912_state::p2_w)
{
	// P20-P23: Address Signals (4MSBS)
	m_bankdev->set_bank(data & 0x0f);

	// P24: +4Hz Flasher
	m_4hz_flasher = BIT(data, 4);
}

READ8_MEMBER(tv912_state::crtc_r)
{
	return m_crtc->read(space, bitswap<4>(offset, 5, 4, 1, 0));
}

WRITE8_MEMBER(tv912_state::crtc_w)
{
	m_crtc->write(space, bitswap<4>(offset, 5, 4, 1, 0), data);
}

READ8_MEMBER(tv912_state::keyboard_r)
{
	u8 result = m_modifiers->read();

	for (int b = 0; b < 8; b++)
		if (!BIT(m_keyboard_scan, b))
			result &= m_keys[b * 4 + offset]->read();

	return result;
}

READ8_MEMBER(tv912_state::uart_status_r)
{
	m_uart->write_swe(0);
	u8 status = m_uart->dav_r() << 0;
	status |= m_uart->tbmt_r() << 1;
	status |= m_uart->pe_r() << 2;
	status |= m_uart->fe_r() << 3;
	status |= BIT(m_jumpers->read(), offset) << 4;
	status |= m_option->read();
	m_uart->write_swe(1);
	return status;
}

WRITE8_MEMBER(tv912_state::output_40c)
{
	// DB6: -PRTOL

	// DB5: +FORCE BLANK
	m_force_blank = BIT(data, 5);

	// DB4: +SEL LPT
	m_lpt_select = BIT(data, 4);

	// DB3: -BREAK

	// DB2: -RQS
	ioport_value dtr = m_dtr->read();
	m_rs232->write_rts(BIT(data, 2));
	if (!BIT(dtr, 0))
		m_rs232->write_dtr(BIT(data, 2));
	if (!BIT(dtr, 1))
		m_rs232->write_dtr(0);

	// DB1: +BEEP
	m_beep->set_state(BIT(data, 1));

	// DB0: +PG SEL
	m_dispram_bank->set_entry(BIT(data, 0));
}

void tv912_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_BAUDGEN:
		m_uart->write_rcp(param);
		m_uart->write_tcp(param);

		ioport_value sel = (m_lpt_select ? m_printer_baud : m_modem_baud)->read();
		for (int b = 0; b < 10; b++)
		{
			if (!BIT(sel, b))
			{
				unsigned divisor = 11 * (b < 9 ? 1 << b : 176);
				m_baudgen_timer->adjust(attotime::from_hz(23.814_MHz_XTAL / 3.5 / divisor), !param);
				break;
			}
		}

		break;
	}
}

u32 tv912_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	if (m_crtc->screen_reset() || m_force_blank)
	{
		bitmap.fill(rgb_t::black(), cliprect);
		return 0;
	}

	const u8 *dispram = static_cast<u8 *>(m_dispram_bank->base());
	ioport_value videoctrl = m_video_control->read();

	rectangle curs;
	m_crtc->cursor_bounds(curs);

	int scroll = m_crtc->upscroll_offset();

	for (int y = cliprect.top(); y <= cliprect.bottom(); y++)
	{
		int row = ((y / 10) + scroll) % 24;
		int ra = y % 10;
		int x = 0;
		u8 *charbase = &m_p_chargen[(ra & 7) | BIT(videoctrl, 1) << 10];

		for (int pos = 0; pos < 80; pos++)
		{
			u8 ch = (pos < 64)
					? dispram[(row << 6) | pos]
					: dispram[0x600 | ((row & 0x07) << 6) | ((row & 0x18) << 1) | (pos & 0x0f)];

			if (CHARSET_TEST && pos >= 32 && pos < 64)
				ch = (pos & 0x1f) | (row & 7) << 5;

			u8 data = (ra > 0 && ra < 9) ? charbase[(ch & 0x7f) << 3] : 0;
			u8 dots = (data & 0xfc) >> 1;
			bool adv = BIT(data, 1);

			if (x == curs.left() && y >= curs.top() && y <= curs.bottom())
			{
				if (BIT(videoctrl, 0) || !m_4hz_flasher)
					dots ^= 0xff;
			}

			for (int d = 0; d < TV912_CH_WIDTH / 2; d++)
			{
				if (x >= cliprect.left() && x <= cliprect.right())
					bitmap.pix(y, x) = BIT(dots, 7) ? rgb_t::white() : rgb_t::black();
				x++;
				if (adv)
					dots <<= 1;
				if (x >= cliprect.left() && x <= cliprect.right())
					bitmap.pix(y, x) = BIT(dots, 7) ? rgb_t::white() : rgb_t::black();
				x++;
				if (!adv)
					dots <<= 1;

				if (d == 2)
					adv = BIT(data, 0);
			}
		}
	}

	return 0;
}

void tv912_state::machine_start()
{
	m_dispram = make_unique_clear<u8[]>(0x1000);
	m_dispram_bank->configure_entries(0, 2, m_dispram.get(), 0x800);

	m_baudgen_timer = timer_alloc(TIMER_BAUDGEN);
	m_baudgen_timer->adjust(attotime::zero, 0);

	save_item(NAME(m_force_blank));
	save_item(NAME(m_lpt_select));
	save_item(NAME(m_4hz_flasher));
	save_item(NAME(m_keyboard_scan));
	save_pointer(NAME(m_dispram), 0x1000);
}

void tv912_state::machine_reset()
{
	ioport_value uart_ctrl = m_uart_control->read();
	m_uart->write_np(BIT(uart_ctrl, 4));
	m_uart->write_tsb(BIT(uart_ctrl, 3));
	m_uart->write_nb1(BIT(uart_ctrl, 2));
	m_uart->write_nb2(BIT(uart_ctrl, 1));
	m_uart->write_eps(BIT(uart_ctrl, 0));
	m_uart->write_cs(1);
}

void tv912_state::prog_map(address_map &map)
{
	map(0x000, 0xfff).rom().region("maincpu", 0);
}

void tv912_state::io_map(address_map &map)
{
	map(0x00, 0xff).m(m_bankdev, FUNC(address_map_bank_device::amap8));
}

void tv912_state::bank_map(address_map &map)
{
	map(0x000, 0x0ff).mirror(0x300).ram();
	map(0x400, 0x403).mirror(0x3c0).select(0x030).rw(FUNC(tv912_state::crtc_r), FUNC(tv912_state::crtc_w));
	map(0x404, 0x404).mirror(0x3f3).r(m_uart, FUNC(ay51013_device::receive));
	map(0x408, 0x40b).mirror(0x3f0).r(FUNC(tv912_state::uart_status_r));
	map(0x408, 0x408).mirror(0x3f3).w(m_uart, FUNC(ay51013_device::transmit));
	map(0x40c, 0x40f).mirror(0x3f0).r(FUNC(tv912_state::keyboard_r));
	map(0x40c, 0x40c).mirror(0x3f3).w(FUNC(tv912_state::output_40c));
	map(0x800, 0xfff).bankrw("dispram");
}

INPUT_CHANGED_MEMBER(tv912_state::uart_settings_changed)
{
	ioport_value uart_ctrl = m_uart_control->read();
	m_uart->write_np(BIT(uart_ctrl, 4));
	m_uart->write_tsb(BIT(uart_ctrl, 3));
	m_uart->write_nb1(BIT(uart_ctrl, 2));
	m_uart->write_nb2(BIT(uart_ctrl, 1));
	m_uart->write_eps(BIT(uart_ctrl, 0));
}

static INPUT_PORTS_START( switches )
	PORT_START("MODEMBAUD")
	PORT_DIPNAME(0x3ff, 0x3fd, "Modem Port Baud Rate") PORT_DIPLOCATION("S1:1,2,3,4,5,6,7,8,9,10")
	PORT_DIPSETTING(0x2ff, "75") // actual rate: 1,208 ÷ 16 ≈ 75.5
	PORT_DIPSETTING(0x1ff, "110") // actual rate: 1,757 ÷ 16 ≈ 109.8
	PORT_DIPSETTING(0x37f, "150") // actual rate: 2,416 ÷ 16 ≈ 151
	PORT_DIPSETTING(0x3bf, "300") // actual rate: 4,832 ÷ 16 ≈ 302
	PORT_DIPSETTING(0x3df, "600") // actual rate: 9,665 ÷ 16 ≈ 604
	PORT_DIPSETTING(0x3ef, "1200") // actual rate: 19,330 ÷ 16 ≈ 1,208
	PORT_DIPSETTING(0x3f7, "2400") // actual rate: 38,659 ÷ 16 ≈ 2,416
	PORT_DIPSETTING(0x3fb, "4800") // actual rate: 77,318 ÷ 16 ≈ 4,832
	PORT_DIPSETTING(0x3fd, "9600") // actual rate: 154,636 ÷ 16 ≈ 9,665
	PORT_DIPSETTING(0x3fe, "19200") // actual rate: 309,273 ÷ 16 ≈ 19,330

	PORT_START("PRINTBAUD")
	PORT_DIPNAME(0x3ff, 0x3fd, "Printer Port Baud Rate") PORT_DIPLOCATION("S3:1,2,3,4,5,6,7,8,9,10")
	PORT_DIPSETTING(0x2ff, "75")
	PORT_DIPSETTING(0x1ff, "110")
	PORT_DIPSETTING(0x37f, "150")
	PORT_DIPSETTING(0x3bf, "300")
	PORT_DIPSETTING(0x3df, "600")
	PORT_DIPSETTING(0x3ef, "1200")
	PORT_DIPSETTING(0x3f7, "2400")
	PORT_DIPSETTING(0x3fb, "4800")
	PORT_DIPSETTING(0x3fd, "9600")
	PORT_DIPSETTING(0x3fe, "19200")

	PORT_START("VIDEOCTRL")
	PORT_DIPUNUSED_DIPLOC(0x04, 0x04, "S2:1") // disables TTL video output on earlier revisions
	PORT_DIPNAME(0x02, 0x00, "Character Set") PORT_DIPLOCATION("S2:2")
	PORT_DIPSETTING(0x00, "Standard")
	PORT_DIPSETTING(0x02, "Alternate")
	PORT_DIPNAME(0x01, 0x00, "Cursor Flash") PORT_DIPLOCATION("S2:10") // originally jumper W25
	PORT_DIPSETTING(0x01, DEF_STR(Off))
	PORT_DIPSETTING(0x00, DEF_STR(On))
	// S2:10 was previously used to short out 270 ohm resistor in video section

	PORT_START("UARTCTRL")
	PORT_DIPNAME(0x11, 0x11, "Parity Select") PORT_DIPLOCATION("S2:9,5") PORT_CHANGED_MEMBER(DEVICE_SELF, tv912_state, uart_settings_changed, nullptr)
	PORT_DIPSETTING(0x11, "None")
	PORT_DIPSETTING(0x01, "Even")
	PORT_DIPSETTING(0x00, "Odd")
	PORT_DIPNAME(0x08, 0x00, "Stop Bits") PORT_DIPLOCATION("S2:6") PORT_CHANGED_MEMBER(DEVICE_SELF, tv912_state, uart_settings_changed, nullptr)
	PORT_DIPSETTING(0x00, "1")
	PORT_DIPSETTING(0x08, "2")
	PORT_DIPNAME(0x06, 0x06, "Data Bits") PORT_DIPLOCATION("S2:8,7") PORT_CHANGED_MEMBER(DEVICE_SELF, tv912_state, uart_settings_changed, nullptr)
	PORT_DIPSETTING(0x00, "5")
	PORT_DIPSETTING(0x04, "6")
	PORT_DIPSETTING(0x02, "7")
	PORT_DIPSETTING(0x06, "8")

	PORT_START("MODIFIERS")
	PORT_DIPNAME(0x80, 0x80, "Refresh Rate") PORT_DIPLOCATION("S2:4")
	PORT_DIPSETTING(0x00, "50 Hz")
	PORT_DIPSETTING(0x80, "60 Hz")
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Alpha Lock") PORT_CODE(KEYCODE_CAPSLOCK)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CHAR(UCHAR_SHIFT_1) PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl") PORT_CHAR(UCHAR_SHIFT_2) PORT_CODE(KEYCODE_LCONTROL)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Funct") PORT_CODE(KEYCODE_LALT)
	PORT_BIT(0x23, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("HALFDUP")
	PORT_DIPNAME(0x01, 0x01, "Conversation Mode") PORT_DIPLOCATION("S2:3")
	PORT_DIPSETTING(0x00, "Half Duplex")
	PORT_DIPSETTING(0x01, "Full Duplex")
	PORT_DIPNAME(0x06, 0x04, "DCR (RS232)") PORT_DIPLOCATION("S5:1,2")
	PORT_DIPSETTING(0x04, "DSR") // at P3-6
	PORT_DIPSETTING(0x02, "DCD") // at P3-8

	PORT_START("JUMPERS")
	PORT_DIPNAME(0x08, 0x00, "Automatic CRLF") PORT_DIPLOCATION("S4:1") // or jumper W31
	PORT_DIPSETTING(0x08, DEF_STR(Off))
	PORT_DIPSETTING(0x00, DEF_STR(On))
	PORT_DIPNAME(0x04, 0x04, "End of Send Character") PORT_DIPLOCATION("S4:2") // or jumper W32
	PORT_DIPSETTING(0x04, "CR")
	PORT_DIPSETTING(0x00, "EOT")
	PORT_DIPNAME(0x02, 0x02, "Column 80 CRLF") PORT_DIPLOCATION("S4:3") // or jumper W33
	PORT_DIPSETTING(0x00, DEF_STR(Off))
	PORT_DIPSETTING(0x02, DEF_STR(On))
	PORT_DIPNAME(0x01, 0x01, "Terminal Mode") PORT_DIPLOCATION("S4:4") // or jumper W34
	PORT_DIPSETTING(0x01, "Extension")
	PORT_DIPSETTING(0x00, "Page Print")

	PORT_START("OPTION")
	PORT_DIPUNKNOWN_DIPLOC(0x20, 0x20, "S4:5")
	PORT_DIPUNKNOWN_DIPLOC(0x40, 0x40, "S4:6")
	PORT_DIPUNKNOWN_DIPLOC(0x80, 0x80, "S4:7")

	PORT_START("DTR")
	PORT_DIPNAME(0x03, 0x02, "DTR (RS232)") PORT_DIPLOCATION("S5:3,4")
	PORT_DIPSETTING(0x02, "Tied to RTS")
	PORT_DIPSETTING(0x01, "Pulled to +12V")
INPUT_PORTS_END

static INPUT_PORTS_START( tv912b )
	PORT_INCLUDE(switches)

	PORT_START("KEY0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('n') PORT_CHAR('N') PORT_CHAR(0x0e) PORT_CODE(KEYCODE_N)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('x') PORT_CHAR('X') PORT_CHAR(0x18) PORT_CODE(KEYCODE_X)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // some non-printing character
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('m') PORT_CHAR('M') PORT_CODE(KEYCODE_M)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('c') PORT_CHAR('C') PORT_CHAR(0x03) PORT_CODE(KEYCODE_C)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(',') PORT_CHAR('<') PORT_CODE(KEYCODE_COMMA) PORT_CODE(KEYCODE_COMMA_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('v') PORT_CHAR('V') PORT_CHAR(0x16) PORT_CODE(KEYCODE_V)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('.') PORT_CHAR('>') PORT_CODE(KEYCODE_STOP) PORT_CODE(KEYCODE_DEL_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('b') PORT_CHAR('B') PORT_CHAR(0x02) PORT_CODE(KEYCODE_B)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('4') PORT_CHAR('$') PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(0x09) PORT_CODE(KEYCODE_ESC) PORT_CODE(KEYCODE_TAB_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // some non-printing character
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('5') PORT_CHAR('%') PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('1') PORT_CHAR('!') PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('6') PORT_CHAR('&') PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('2') PORT_CHAR('\"') PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('7') PORT_CHAR('\'') PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('3') PORT_CHAR('#') PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('r') PORT_CHAR('R') PORT_CHAR(0x12) PORT_CODE(KEYCODE_R)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(0x1b) PORT_CODE(KEYCODE_TAB)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // some non-printing character
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY9")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('t') PORT_CHAR('T') PORT_CHAR(0x14) PORT_CODE(KEYCODE_T)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('q') PORT_CHAR('Q') PORT_CHAR(0x11) PORT_CODE(KEYCODE_Q)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY10")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('y') PORT_CHAR('Y') PORT_CHAR(0x19) PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('w') PORT_CHAR('W') PORT_CHAR(0x17) PORT_CODE(KEYCODE_W)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY11")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('u') PORT_CHAR('U') PORT_CHAR(0x15) PORT_CODE(KEYCODE_U)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('e') PORT_CHAR('E') PORT_CHAR(0x05) PORT_CODE(KEYCODE_E)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY12")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('g') PORT_CHAR('G') PORT_CHAR(0x07) PORT_CODE(KEYCODE_G)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('a') PORT_CHAR('A') PORT_CHAR(0x01) PORT_CODE(KEYCODE_A)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xc2
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY13")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('h') PORT_CHAR('H') PORT_CODE(KEYCODE_H)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('s') PORT_CHAR('S') PORT_CHAR(0x13) PORT_CODE(KEYCODE_S)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY14")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('j') PORT_CHAR('J') PORT_CODE(KEYCODE_J)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('d') PORT_CHAR('D') PORT_CHAR(0x04) PORT_CODE(KEYCODE_D)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY15")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('k') PORT_CHAR('K') PORT_CODE(KEYCODE_K)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('f') PORT_CHAR('F') PORT_CHAR(0x06) PORT_CODE(KEYCODE_F)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY16")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Return") PORT_CHAR(0x0d) PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('i') PORT_CHAR('I') PORT_CODE(KEYCODE_I)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xc9
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY17")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xd0 (PRINT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('o') PORT_CHAR('O') PORT_CHAR(0x0f) PORT_CODE(KEYCODE_O)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xd1
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY18")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rub Out") PORT_CHAR(0x08) PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('p') PORT_CHAR('P') PORT_CHAR(0x10) PORT_CODE(KEYCODE_P)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY19")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0x9a (BREAK)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Line Feed") PORT_CHAR(0x0a) PORT_CODE(KEYCODE_INSERT)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY20")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0x1f (PAGE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('l') PORT_CHAR('L') PORT_CODE(KEYCODE_L)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xb5
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY21")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(LEFT)) PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(';') PORT_CHAR('+') PORT_CODE(KEYCODE_COLON)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY22")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('/') PORT_CHAR('?') PORT_CODE(KEYCODE_SLASH)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('@') PORT_CHAR('`') PORT_CODE(KEYCODE_TILDE)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY23")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(' ') PORT_CODE(KEYCODE_SPACE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('\\') PORT_CHAR('|') PORT_CHAR(0x1c) PORT_CODE(KEYCODE_BACKSLASH)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY24")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(']') PORT_CHAR('}') PORT_CHAR(0x1d) PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(':') PORT_CHAR('*') PORT_CODE(KEYCODE_MINUS)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xb4
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY25")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('8') PORT_CHAR('(') PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('-') PORT_CHAR('=') PORT_CODE(KEYCODE_EQUALS)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY26")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('9') PORT_CHAR(')') PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('0') PORT_CHAR('_') PORT_CODE(KEYCODE_0) PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY27")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('^') PORT_CHAR('~') PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('[') PORT_CHAR('{') PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY28")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(RIGHT)) PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('z') PORT_CHAR('Z') PORT_CHAR(0x1a) PORT_CODE(KEYCODE_Z)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xd9
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY29")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(UP)) PORT_CODE(KEYCODE_UP)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xbb (CLEAR)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY30")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(DOWN)) PORT_CODE(KEYCODE_DOWN)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character (CLR TAB)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY31")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(HOME)) PORT_CODE(KEYCODE_RALT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character (PROT MODE)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

static INPUT_PORTS_START( tv912c )
	PORT_INCLUDE(switches)

	PORT_START("KEY0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('n') PORT_CHAR('N') PORT_CHAR(0x0e) PORT_CODE(KEYCODE_N)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('x') PORT_CHAR('X') PORT_CHAR(0x18) PORT_CODE(KEYCODE_X)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('m') PORT_CHAR('M') PORT_CODE(KEYCODE_M)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('c') PORT_CHAR('C') PORT_CHAR(0x03) PORT_CODE(KEYCODE_C)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(',') PORT_CHAR('<') PORT_CODE(KEYCODE_COMMA)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('v') PORT_CHAR('V') PORT_CHAR(0x16) PORT_CODE(KEYCODE_V)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('.') PORT_CHAR('>') PORT_CODE(KEYCODE_STOP) PORT_CODE(KEYCODE_DEL_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('b') PORT_CHAR('B') PORT_CHAR(0x02) PORT_CODE(KEYCODE_B)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('4') PORT_CHAR('$') PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(0x1b) PORT_CODE(KEYCODE_ESC)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('5') PORT_CHAR('%') PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('1') PORT_CHAR('!') PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('6') PORT_CHAR('^') PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('2') PORT_CHAR('@') PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('7') PORT_CHAR('&') PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('3') PORT_CHAR('#') PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('r') PORT_CHAR('R') PORT_CHAR(0x12) PORT_CODE(KEYCODE_R)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(0x09) PORT_CODE(KEYCODE_TAB)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY9")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('t') PORT_CHAR('T') PORT_CHAR(0x14) PORT_CODE(KEYCODE_T)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('q') PORT_CHAR('Q') PORT_CHAR(0x11) PORT_CODE(KEYCODE_Q)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY10")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('y') PORT_CHAR('Y') PORT_CHAR(0x19) PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('w') PORT_CHAR('W') PORT_CHAR(0x17) PORT_CODE(KEYCODE_W)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY11")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('u') PORT_CHAR('U') PORT_CHAR(0x15) PORT_CODE(KEYCODE_U)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('e') PORT_CHAR('E') PORT_CHAR(0x05) PORT_CODE(KEYCODE_E)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY12")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('g') PORT_CHAR('G') PORT_CHAR(0x07) PORT_CODE(KEYCODE_G)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('a') PORT_CHAR('A') PORT_CHAR(0x01) PORT_CODE(KEYCODE_A)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY13")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('h') PORT_CHAR('H') PORT_CODE(KEYCODE_H)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('s') PORT_CHAR('S') PORT_CHAR(0x13) PORT_CODE(KEYCODE_S)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY14")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('j') PORT_CHAR('J') PORT_CODE(KEYCODE_J)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('d') PORT_CHAR('D') PORT_CHAR(0x04) PORT_CODE(KEYCODE_D)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY15")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('k') PORT_CHAR('K') PORT_CODE(KEYCODE_K)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('f') PORT_CHAR('F') PORT_CHAR(0x06) PORT_CODE(KEYCODE_F)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY16")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Line Feed") PORT_CHAR(0x0a) PORT_CODE(KEYCODE_INSERT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('i') PORT_CHAR('I') PORT_CODE(KEYCODE_I)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY17")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('o') PORT_CHAR('O') PORT_CHAR(0x0f) PORT_CODE(KEYCODE_O)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY18")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('\'') PORT_CHAR('\"') PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('p') PORT_CHAR('P') PORT_CHAR(0x10) PORT_CODE(KEYCODE_P)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY19")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0x9a (BREAK)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('[') PORT_CHAR(']') PORT_CHAR(0x1d) PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY20")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNKNOWN) // (BLOCK CONV)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('l') PORT_CHAR('L') PORT_CODE(KEYCODE_L)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY21")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(LEFT)) PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(';') PORT_CHAR(':') PORT_CODE(KEYCODE_COLON)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY22")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('/') PORT_CHAR('?') PORT_CODE(KEYCODE_SLASH)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(ENTER_PAD)) PORT_CODE(KEYCODE_ENTER_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY23")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(' ') PORT_CODE(KEYCODE_SPACE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Return") PORT_CHAR(0x0d) PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY24")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('\\') PORT_CHAR('|') PORT_CHAR(0x1c) PORT_CODE(KEYCODE_BACKSLASH)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('-') PORT_CHAR('_') PORT_CODE(KEYCODE_MINUS) PORT_CODE(KEYCODE_TAB_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY25")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('8') PORT_CHAR('*') PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('=') PORT_CHAR('+') PORT_CODE(KEYCODE_EQUALS)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY26")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('9') PORT_CHAR('(') PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('0') PORT_CHAR(')') PORT_CODE(KEYCODE_0) PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY27")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Back Space") PORT_CHAR(0x08) PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('`') PORT_CHAR('~') PORT_CODE(KEYCODE_TILDE)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY28")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(RIGHT)) PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('z') PORT_CHAR('Z') PORT_CHAR(0x1a) PORT_CODE(KEYCODE_Z)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY29")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(UP)) PORT_CODE(KEYCODE_UP)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN) // control character 0xbb (CLEAR)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY30")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(DOWN)) PORT_CODE(KEYCODE_DOWN)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNKNOWN) // (DEL)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("KEY31")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR(UCHAR_MAMEKEY(HOME)) PORT_CODE(KEYCODE_RALT)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CHAR('{') PORT_CHAR('}') PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNKNOWN)
	PORT_BIT(0xdc, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

void tv912_state::tv912(machine_config &config)
{
	i8035_device &maincpu(I8035(config, m_maincpu, 23.814_MHz_XTAL / 4)); // nominally +6MHz, actually 5.9535 MHz
	maincpu.set_addrmap(AS_PROGRAM, &tv912_state::prog_map);
	maincpu.set_addrmap(AS_IO, &tv912_state::io_map);
	maincpu.p1_out_cb().set(FUNC(tv912_state::p1_w));
	maincpu.p2_in_cb().set(FUNC(tv912_state::p2_r));
	maincpu.p2_out_cb().set(FUNC(tv912_state::p2_w));
	maincpu.t0_in_cb().set(m_rs232, FUNC(rs232_port_device::cts_r));
	maincpu.t1_in_cb().set(m_crtc, FUNC(tms9927_device::bl_r)).invert();
	maincpu.prog_out_cb().set(m_uart, FUNC(ay51013_device::write_xr)).invert();

	ADDRESS_MAP_BANK(config, m_bankdev);
	m_bankdev->set_addrmap(0, &tv912_state::bank_map);
	m_bankdev->set_data_width(8);
	m_bankdev->set_addr_width(12);
	m_bankdev->set_stride(0x100);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_raw(23.814_MHz_XTAL, 105 * TV912_CH_WIDTH, 0, 80 * TV912_CH_WIDTH, 270, 0, 240);
	screen.set_screen_update(FUNC(tv912_state::screen_update));

	TMS9927(config, m_crtc, 23.814_MHz_XTAL / TV912_CH_WIDTH);
	m_crtc->set_char_width(TV912_CH_WIDTH);
	m_crtc->vsyn_callback().set_inputline(m_maincpu, MCS48_INPUT_IRQ);
	m_crtc->set_screen("screen");

	AY51013(config, m_uart);
	m_uart->read_si_callback().set(m_rs232, FUNC(rs232_port_device::rxd_r));
	m_uart->write_so_callback().set(m_rs232, FUNC(rs232_port_device::write_txd));
	m_uart->set_auto_rdav(true);

	RS232_PORT(config, m_rs232, default_rs232_devices, "loopback");

	SPEAKER(config, "mono").front_center();
	BEEP(config, m_beep, 23.814_MHz_XTAL / 7 / 11 / 256); // nominally 1200 Hz
	m_beep->add_route(ALL_OUTPUTS, "mono", 0.50);
}

/**************************************************************************************************************

Televideo TVI-912C.
Chips: i8035, TMS9927NL, AY5-1013A (COM2502)
Crystals: 23.814 (divide by 4 for CPU clock)
Other: 1x 8-sw DIP, 1x 10-sw DIP (internal), 2x 10-sw DIP (available to user at the back)

***************************************************************************************************************/

ROM_START( tv912c )
	ROM_REGION(0x1000, "maincpu", 0)
	ROM_LOAD( "a49c1.bin",    0x0000, 0x1000, CRC(40068371) SHA1(44c32f8c3980acebe28fa48f98479910af2eb4ae) ) // MM52132

	ROM_REGION(0x0800, "chargen", 0)
	ROM_LOAD( "a3-2.bin",     0x0000, 0x0800, CRC(bb9a7fbd) SHA1(5f1c4d41b25bd3ca4dbc336873362935daf283da) ) // EA8316
ROM_END

ROM_START( tv912b )
	ROM_REGION(0x1000, "maincpu", 0)
	ROM_LOAD( "televideo912b_rom_a49.bin", 0x0000, 0x1000, CRC(2c95e995) SHA1(77cda383d68b0bbbb783026d8fde679f10f9eded) ) // MM52132 (TVI A49B1)

	ROM_REGION(0x0800, "chargen", 0)
	ROM_LOAD( "televideo912b_rom_a3.bin", 0x0000, 0x0800, CRC(bb9a7fbd) SHA1(5f1c4d41b25bd3ca4dbc336873362935daf283da) ) // AMI 8110QV (A3-2)
ROM_END

COMP( 1978, tv912c, 0,      0, tv912, tv912c, tv912_state, empty_init, "TeleVideo Systems", "TVI-912C", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS ) // attributes not emulated
COMP( 1978, tv912b, tv912c, 0, tv912, tv912b, tv912_state, empty_init, "TeleVideo Systems", "TVI-912B", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS ) // attributes not emulated
