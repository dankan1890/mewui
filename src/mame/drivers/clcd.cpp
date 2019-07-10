// license:BSD-3-Clause
// copyright-holders:Curt Coder,smf
/***************************************************************************

        Commodore LCD prototype

        GTE G65SC102PI-2
        GTE G65SC51P-1
        Rockwell R65C22P2 x 2
        AMI S3530X Bell 103/V.21 Single chip modem

****************************************************************************/


#include "emu.h"
#include "bus/centronics/ctronics.h"
#include "bus/rs232/rs232.h"
#include "cpu/m6502/m65c02.h"
#include "machine/6522via.h"
#include "machine/bankdev.h"
#include "machine/input_merger.h"
#include "machine/mos6551.h"
#include "machine/msm58321.h"
#include "machine/ram.h"
#include "machine/nvram.h"
#include "sound/spkrdev.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"

class clcd_state : public driver_device
{
public:
	clcd_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_acia(*this, "acia"),
		m_via0(*this, "via0"),
		m_rtc(*this, "rtc"),
		m_centronics(*this, "centronics"),
		m_ram(*this, "ram"),
		m_nvram(*this, "nvram"),
		m_bankdev(*this, "bank%u", 1U),
		m_lcd_char_rom(*this, "lcd_char_rom"),
		m_lcd_scrollx(0),
		m_lcd_scrolly(0),
		m_lcd_mode(0),
		m_lcd_size(0),
		m_mmu_mode(MMU_MODE_KERN),
		m_mmu_saved_mode(MMU_MODE_KERN),
		m_mmu_offset1(0),
		m_mmu_offset2(0),
		m_mmu_offset3(0),
		m_mmu_offset4(0),
		m_mmu_offset5(0),
		m_key_clk(0),
		m_key_poll(0),
		m_key_column(0),
		m_key_shift(0),
		m_key_force_format(0),
		m_col(*this, "COL%u", 0U),
		m_special(*this, "SPECIAL")
	{
	}

	virtual void driver_start() override
	{
		m_mmu_mode = MMU_MODE_TEST;
		update_mmu_mode(MMU_MODE_KERN);

		save_item(NAME(m_lcd_scrollx));
		save_item(NAME(m_lcd_scrolly));
		save_item(NAME(m_lcd_mode));
		save_item(NAME(m_lcd_size));
		save_item(NAME(m_mmu_mode));
		save_item(NAME(m_mmu_saved_mode));
		save_item(NAME(m_mmu_offset1));
		save_item(NAME(m_mmu_offset2));
		save_item(NAME(m_mmu_offset3));
		save_item(NAME(m_mmu_offset4));
		save_item(NAME(m_mmu_offset5));
		save_item(NAME(m_key_clk));
		save_item(NAME(m_key_poll));
		save_item(NAME(m_key_column));
		save_item(NAME(m_key_shift));

		m_rtc->cs1_w(1);
		m_acia->write_cts(0);
		m_nvram->set_base(ram()->pointer(), ram()->size());
	}

	void clcd_palette(palette_device &palette) const
	{
		palette.set_pen_color(0, rgb_t(36,72,36));
		palette.set_pen_color(1, rgb_t(2,4,2));
	}

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
	{
		if (m_lcd_mode & LCD_MODE_GRAPH)
		{
			for (int y = 0; y < 128; y++)
			{
				int offset = (m_lcd_scrolly * 128) + (y * 64);

				for (int x = 0; x < 60; x++)
				{
					uint8_t bit = m_ram->pointer()[offset++];

					bitmap.pix16(y, (x * 8) + 0) = (bit >> 7) & 1;
					bitmap.pix16(y, (x * 8) + 1) = (bit >> 6) & 1;
					bitmap.pix16(y, (x * 8) + 2) = (bit >> 5) & 1;
					bitmap.pix16(y, (x * 8) + 3) = (bit >> 4) & 1;
					bitmap.pix16(y, (x * 8) + 4) = (bit >> 3) & 1;
					bitmap.pix16(y, (x * 8) + 5) = (bit >> 2) & 1;
					bitmap.pix16(y, (x * 8) + 6) = (bit >> 1) & 1;
					bitmap.pix16(y, (x * 8) + 7) = (bit >> 0) & 1;
				}
			}
		}
		else
		{
			uint8_t *font = m_lcd_char_rom->base();
			if (m_lcd_mode & LCD_MODE_ALT)
			{
				font += 1024;
			}

			int chrw = (m_lcd_size & LCD_SIZE_CHRW) ? 8 : 6;

			for (int y = 0; y < 128; y++)
			{
				int offset = (m_lcd_scrolly * 128) + (m_lcd_scrollx & 0x7f) + ((y / 8) * 128);

				for (int x = 0; x < 480; x++)
				{
					uint8_t ch = m_ram->pointer()[offset + (x / chrw)];
					uint8_t bit = font[((ch & 0x7f) * 8) + (y % 8)];
					if (ch & 0x80)
					{
						bit = ~bit;
					}

					bitmap.pix16(y, x) = (bit >> (7 - (x % chrw))) & 1;
				}
			}
		}

		return 0;
	}

	READ8_MEMBER(ram_r)
	{
		if (offset < m_ram->size())
		{
			return m_ram->pointer()[offset];
		}

		return 0xff;
	}

	WRITE8_MEMBER(ram_w)
	{
		if (offset < m_ram->size())
		{
			m_ram->pointer()[offset] = data;
		}
	}

	enum
	{
		MMU_MODE_KERN,
		MMU_MODE_APPL,
		MMU_MODE_RAM,
		MMU_MODE_TEST
	};

	enum
	{
		LCD_MODE_ALT = 1,
		LCD_MODE_GRAPH = 2
	};

	enum
	{
		LCD_SIZE_CHRW = 4
	};

	void update_mmu_mode(int new_mode)
	{
		if (m_mmu_mode != new_mode)
		{
			m_mmu_mode = new_mode;

			switch (m_mmu_mode)
			{
			case MMU_MODE_KERN:
				m_bankdev[0]->set_bank(0x04 + 0x00);
				update_mmu_offset5();
				m_bankdev[2]->set_bank(0x20 + 0xc0);
				m_bankdev[3]->set_bank(0x30 + 0xc0);
				break;

			case MMU_MODE_APPL:
				update_mmu_offset1();
				update_mmu_offset2();
				update_mmu_offset3();
				update_mmu_offset4();
				break;

			case MMU_MODE_RAM:
				m_bankdev[0]->set_bank(0x04);
				m_bankdev[1]->set_bank(0x10);
				m_bankdev[2]->set_bank(0x20);
				m_bankdev[3]->set_bank(0x30);
				break;

			case MMU_MODE_TEST:
				m_bankdev[0]->set_bank(0x04 + 0x200);
				m_bankdev[1]->set_bank(0x10 + 0x200);
				m_bankdev[2]->set_bank(0x20 + 0x200);
				m_bankdev[3]->set_bank(0x30 + 0x200);
				break;
			}
		}
	}

	void update_mmu_offset1()
	{
		if (m_mmu_mode == MMU_MODE_APPL)
		{
			m_bankdev[0]->set_bank(0x04 + m_mmu_offset1);
		}
	}

	void update_mmu_offset2()
	{
		if (m_mmu_mode == MMU_MODE_APPL)
		{
			m_bankdev[1]->set_bank((0x10 + m_mmu_offset2) & 0xff);
		}
	}

	void update_mmu_offset3()
	{
		if (m_mmu_mode == MMU_MODE_APPL)
		{
			m_bankdev[2]->set_bank((0x20 + m_mmu_offset3) & 0xff);
		}
	}

	void update_mmu_offset4()
	{
		if (m_mmu_mode == MMU_MODE_APPL)
		{
			m_bankdev[3]->set_bank((0x30 + m_mmu_offset4) & 0xff);
		}
	}

	void update_mmu_offset5()
	{
		if (m_mmu_mode == MMU_MODE_KERN)
		{
			m_bankdev[1]->set_bank((0x10 + m_mmu_offset5) & 0xff);
		}
	}

	WRITE8_MEMBER(mmu_mode_kern_w)
	{
		update_mmu_mode(MMU_MODE_KERN);
	}

	WRITE8_MEMBER(mmu_mode_appl_w)
	{
		update_mmu_mode(MMU_MODE_APPL);
	}

	WRITE8_MEMBER(mmu_mode_ram_w)
	{
		update_mmu_mode(MMU_MODE_RAM);
	}

	WRITE8_MEMBER(mmu_mode_recall_w)
	{
		update_mmu_mode(m_mmu_saved_mode);
	}

	WRITE8_MEMBER(mmu_mode_save_w)
	{
		m_mmu_saved_mode = m_mmu_mode;
	}

	WRITE8_MEMBER(mmu_mode_test_w)
	{
		update_mmu_mode(MMU_MODE_TEST);
	}

	WRITE8_MEMBER(mmu_offset1_w)
	{
		if (m_mmu_offset1 != data)
		{
			m_mmu_offset1 = data;
			update_mmu_offset1();
		}
	}

	WRITE8_MEMBER(mmu_offset2_w)
	{
		if (m_mmu_offset2 != data)
		{
			m_mmu_offset2 = data;
			update_mmu_offset2();
		}
	}

	WRITE8_MEMBER(mmu_offset3_w)
	{
		if (m_mmu_offset3 != data)
		{
			m_mmu_offset3 = data;
			update_mmu_offset3();
		}
	}

	WRITE8_MEMBER(mmu_offset4_w)
	{
		if (m_mmu_offset4 != data)
		{
			m_mmu_offset4 = data;
			update_mmu_offset4();
		}
	}

	WRITE8_MEMBER(mmu_offset5_w)
	{
		if (m_mmu_offset5 != data)
		{
			m_mmu_offset5 = data;
			update_mmu_offset5();
		}
	}

	READ8_MEMBER(mmu_offset1_r)
	{
		return m_mmu_offset1;
	}

	READ8_MEMBER(mmu_offset2_r)
	{
		return m_mmu_offset2;
	}

	READ8_MEMBER(mmu_offset3_r)
	{
		return m_mmu_offset3;
	}

	READ8_MEMBER(mmu_offset4_r)
	{
		return m_mmu_offset4;
	}

	READ8_MEMBER(mmu_offset5_r)
	{
		return m_mmu_offset5;
	}

	WRITE8_MEMBER(lcd_scrollx_w)
	{
		m_lcd_scrollx = data;
	}

	WRITE8_MEMBER(lcd_scrolly_w)
	{
		m_lcd_scrolly = data;
	}

	WRITE8_MEMBER(lcd_mode_w)
	{
		m_lcd_mode = data;
	}

	WRITE8_MEMBER(lcd_size_w)
	{
		m_lcd_size = data;
	}

	WRITE8_MEMBER(via0_pa_w)
	{
		m_key_column = data;
	}

	WRITE8_MEMBER(via0_pb_w)
	{
		write_key_poll((data >> 0) & 1);
		m_rtc->cs2_w((data >> 1) & 1);
	}

	WRITE_LINE_MEMBER(write_key_poll)
	{
		if (m_key_poll != state)
		{
			m_key_poll = state;

			if (m_key_poll != 0)
			{
				m_key_shift = m_special->read();

				for (int i = 0; i < 8; i++)
				{
					if ((m_key_column & (128 >> i)) == 0)
					{
						m_key_shift |= m_col[i]->read() << 8;
					}
				}

				if (m_key_force_format)
				{
					m_key_shift = 0x15; // Simulate holding CBM+SHIFT+STOP if you have no NVRAM

					m_key_force_format--;
				}
			}
		}
	}

	WRITE_LINE_MEMBER(via0_cb1_w)
	{
		int newm_key_clk = state & 1;
		if (m_key_clk != newm_key_clk)
		{
			m_key_clk = newm_key_clk;

			if (!m_key_clk)
			{
				m_via0->write_cb2((m_key_shift & 0x8000) != 0);
				m_key_shift <<= 1;
			}
		}
	}

	WRITE8_MEMBER(via1_pa_w)
	{
		m_rtc->d0_w(BIT(data, 0));
		m_centronics->write_data0(BIT(data, 0));

		m_rtc->d1_w(BIT(data, 1));
		m_centronics->write_data1(BIT(data, 1));

		m_rtc->d2_w(BIT(data, 2));
		m_centronics->write_data2(BIT(data, 2));

		m_rtc->d3_w(BIT(data, 3));
		m_centronics->write_data3(BIT(data, 3));

		m_rtc->read_w(BIT(data, 4));
		m_centronics->write_data4(BIT(data, 4));

		m_rtc->write_w(BIT(data, 5));
		m_centronics->write_data5(BIT(data, 5));

		m_rtc->address_write_w(BIT(data, 6));
		m_centronics->write_data6(BIT(data, 6));

		m_centronics->write_data7(BIT(data, 7));
	}

	WRITE8_MEMBER(via1_pb_w)
	{
		//int centronics_unknown = !BIT(data,5);
	}

	ram_device *ram()
	{
		return m_ram;
	}

	void force_format()
	{
		m_key_force_format = 10;
	}

	void nvram_init(nvram_device &nvram, void *data, size_t size);

	void clcd(machine_config &config);
	void clcd_banked_mem(address_map &map);
	void clcd_mem(address_map &map);
private:
	required_device<m65c02_device> m_maincpu;
	required_device<mos6551_device> m_acia;
	required_device<via6522_device> m_via0;
	required_device<msm58321_device> m_rtc;
	required_device<centronics_device> m_centronics;
	required_device<ram_device> m_ram;
	required_device<nvram_device> m_nvram;
	required_device_array<address_map_bank_device, 4> m_bankdev;
	required_memory_region m_lcd_char_rom;
	int m_lcd_scrollx;
	int m_lcd_scrolly;
	int m_lcd_mode;
	int m_lcd_size;
	int m_mmu_mode;
	int m_mmu_saved_mode;
	uint8_t m_mmu_offset1;
	uint8_t m_mmu_offset2;
	uint8_t m_mmu_offset3;
	uint8_t m_mmu_offset4;
	uint8_t m_mmu_offset5;
	int m_key_clk;
	int m_key_poll;
	int m_key_column;
	uint16_t m_key_shift;
	int m_key_force_format;
	required_ioport_array<8> m_col;
	required_ioport m_special;
};

void clcd_state::nvram_init(nvram_device &nvram, void *data, size_t size)
{
	memset(data, 0x00, size);
	force_format();
}


void clcd_state::clcd_banked_mem(address_map &map)
{
	/* KERN/APPL/RAM */
	map(0x00000, 0x1ffff).mirror(0x40000).rw(FUNC(clcd_state::ram_r), FUNC(clcd_state::ram_w));
	map(0x20000, 0x3ffff).mirror(0x40000).rom().region("maincpu", 0);

	/* TEST */
	map(0x81000, 0x83fff).r(FUNC(clcd_state::mmu_offset1_r));
	map(0x84000, 0x87fff).r(FUNC(clcd_state::mmu_offset2_r));
	map(0x88000, 0x8bfff).r(FUNC(clcd_state::mmu_offset3_r));
	map(0x8c000, 0x8dfff).r(FUNC(clcd_state::mmu_offset4_r));
	map(0x8e000, 0x8f7ff).r(FUNC(clcd_state::mmu_offset5_r));
}

void clcd_state::clcd_mem(address_map &map)
{
	map(0x0000, 0x0fff).rw(FUNC(clcd_state::ram_r), FUNC(clcd_state::ram_w));
	map(0x1000, 0x3fff).rw(m_bankdev[0], FUNC(address_map_bank_device::read8), FUNC(address_map_bank_device::write8));
	map(0x4000, 0x7fff).rw(m_bankdev[1], FUNC(address_map_bank_device::read8), FUNC(address_map_bank_device::write8));
	map(0x8000, 0xbfff).rw(m_bankdev[2], FUNC(address_map_bank_device::read8), FUNC(address_map_bank_device::write8));
	map(0xc000, 0xf7ff).rw(m_bankdev[3], FUNC(address_map_bank_device::read8), FUNC(address_map_bank_device::write8));
	map(0xf800, 0xf80f).mirror(0x70).m(m_via0, FUNC(via6522_device::map));
	map(0xf880, 0xf88f).mirror(0x70).m("via1", FUNC(via6522_device::map));
	map(0xf980, 0xf983).mirror(0x7c).rw(m_acia, FUNC(mos6551_device::read), FUNC(mos6551_device::write));
	map(0xfa00, 0xffff).rom().region("maincpu", 0x1fa00);
	map(0xfa00, 0xfa00).mirror(0x7f).w(FUNC(clcd_state::mmu_mode_kern_w));
	map(0xfa80, 0xfa80).mirror(0x7f).w(FUNC(clcd_state::mmu_mode_appl_w));
	map(0xfb00, 0xfb00).mirror(0x7f).w(FUNC(clcd_state::mmu_mode_ram_w));
	map(0xfb80, 0xfb80).mirror(0x7f).w(FUNC(clcd_state::mmu_mode_recall_w));
	map(0xfc00, 0xfc00).mirror(0x7f).w(FUNC(clcd_state::mmu_mode_save_w));
	map(0xfc80, 0xfc80).mirror(0x7f).w(FUNC(clcd_state::mmu_mode_test_w));
	map(0xfd00, 0xfd00).mirror(0x7f).w(FUNC(clcd_state::mmu_offset1_w));
	map(0xfd80, 0xfd80).mirror(0x7f).w(FUNC(clcd_state::mmu_offset2_w));
	map(0xfe00, 0xfe00).mirror(0x7f).w(FUNC(clcd_state::mmu_offset3_w));
	map(0xfe80, 0xfe80).mirror(0x7f).w(FUNC(clcd_state::mmu_offset4_w));
	map(0xff00, 0xff00).mirror(0x7f).w(FUNC(clcd_state::mmu_offset5_w));
	map(0xff80, 0xff80).mirror(0x7c).w(FUNC(clcd_state::lcd_scrollx_w));
	map(0xff81, 0xff81).mirror(0x7c).w(FUNC(clcd_state::lcd_scrolly_w));
	map(0xff82, 0xff82).mirror(0x7c).w(FUNC(clcd_state::lcd_mode_w));
	map(0xff83, 0xff83).mirror(0x7c).w(FUNC(clcd_state::lcd_size_w));
}

/* Input ports */
static INPUT_PORTS_START( clcd )
	PORT_START( "COL0" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F6")              PORT_CODE(KEYCODE_F6)           PORT_CHAR(UCHAR_MAMEKEY(F6))
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("/ ?")             PORT_CODE(KEYCODE_SLASH)        PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME(", <")             PORT_CODE(KEYCODE_COMMA)        PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("N")               PORT_CODE(KEYCODE_N)            PORT_CHAR('N')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("V")               PORT_CODE(KEYCODE_V)            PORT_CHAR('V')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("X")               PORT_CODE(KEYCODE_X)            PORT_CHAR('X')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F4")              PORT_CODE(KEYCODE_F4)           PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("@")               PORT_CODE(KEYCODE_QUOTE)        PORT_CHAR('@')

	PORT_START( "COL1" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Q")               PORT_CODE(KEYCODE_Q)            PORT_CHAR('Q')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("+")               PORT_CODE(KEYCODE_PLUS_PAD)     PORT_CHAR('+')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("-")               PORT_CODE(KEYCODE_MINUS)        PORT_CHAR('-')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("O")               PORT_CODE(KEYCODE_O)            PORT_CHAR('O')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("U")               PORT_CODE(KEYCODE_U)            PORT_CHAR('U')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("T")               PORT_CODE(KEYCODE_T)            PORT_CHAR('T')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("E")               PORT_CODE(KEYCODE_E)            PORT_CHAR('E')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F5")              PORT_CODE(KEYCODE_F5)           PORT_CHAR(UCHAR_MAMEKEY(F5))

	PORT_START( "COL2" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F8")              PORT_CODE(KEYCODE_F8)           PORT_CHAR(UCHAR_MAMEKEY(F8))
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("=")               PORT_CODE(KEYCODE_EQUALS)       PORT_CHAR('=')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME(": [")             PORT_CODE(KEYCODE_OPENBRACE)    PORT_CHAR(':') PORT_CHAR('[')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("K")               PORT_CODE(KEYCODE_K)            PORT_CHAR('K')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("H")               PORT_CODE(KEYCODE_H)            PORT_CHAR('H')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F")               PORT_CODE(KEYCODE_F)            PORT_CHAR('F')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("S")               PORT_CODE(KEYCODE_S)            PORT_CHAR('S')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F3")              PORT_CODE(KEYCODE_F3)           PORT_CHAR(UCHAR_MAMEKEY(F3))

	PORT_START( "COL3" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("SPACE")           PORT_CODE(KEYCODE_SPACE)        PORT_CHAR(' ')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("ESC")             PORT_CODE(KEYCODE_ESC)          PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME(". >")             PORT_CODE(KEYCODE_STOP)         PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("M")               PORT_CODE(KEYCODE_M)            PORT_CHAR('M')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("B")               PORT_CODE(KEYCODE_B)            PORT_CHAR('B')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("C")               PORT_CODE(KEYCODE_C)            PORT_CHAR('C')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Z")               PORT_CODE(KEYCODE_Z)            PORT_CHAR('Z')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F1")              PORT_CODE(KEYCODE_F1)           PORT_CHAR(UCHAR_MAMEKEY(F1))

	PORT_START( "COL4" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("2 \"")            PORT_CODE(KEYCODE_2)            PORT_CHAR('2') PORT_CHAR('\"')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("RIGHT")           PORT_CODE(KEYCODE_RIGHT)        PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("UP")              PORT_CODE(KEYCODE_UP)           PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("0 ^")             PORT_CODE(KEYCODE_0)            PORT_CHAR('0')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("8 (")             PORT_CODE(KEYCODE_8)            PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("6 &")             PORT_CODE(KEYCODE_6)            PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("4 $")             PORT_CODE(KEYCODE_4)            PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F7")              PORT_CODE(KEYCODE_F7)           PORT_CHAR(UCHAR_MAMEKEY(F7))

	PORT_START( "COL5" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("F2")              PORT_CODE(KEYCODE_F2)           PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("; ]")             PORT_CODE(KEYCODE_CLOSEBRACE)   PORT_CHAR(';') PORT_CHAR(']')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("L")               PORT_CODE(KEYCODE_L)            PORT_CHAR('L')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("J")               PORT_CODE(KEYCODE_J)            PORT_CHAR('J')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("G")               PORT_CODE(KEYCODE_G)            PORT_CHAR('G')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("D")               PORT_CODE(KEYCODE_D)            PORT_CHAR('D')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("A")               PORT_CODE(KEYCODE_A)            PORT_CHAR('A')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("TAB")             PORT_CODE(KEYCODE_TAB)          PORT_CHAR('\t')

	PORT_START( "COL6" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("HOME CLR")        PORT_CODE(KEYCODE_HOME)         PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("*")               PORT_CODE(KEYCODE_BACKSLASH)    PORT_CHAR('*')
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("P")               PORT_CODE(KEYCODE_P)            PORT_CHAR('P')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("I")               PORT_CODE(KEYCODE_I)            PORT_CHAR('I')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("Y")               PORT_CODE(KEYCODE_Y)            PORT_CHAR('Y')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("R")               PORT_CODE(KEYCODE_R)            PORT_CHAR('R')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("W")               PORT_CODE(KEYCODE_W)            PORT_CHAR('W')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("RETURN")          PORT_CODE(KEYCODE_ENTER)        PORT_CHAR('\r')

	PORT_START( "COL7" )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("1 !")             PORT_CODE(KEYCODE_1)            PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("LEFT")            PORT_CODE(KEYCODE_LEFT)         PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("DOWN")            PORT_CODE(KEYCODE_DOWN)         PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("9 )")             PORT_CODE(KEYCODE_9)            PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("7 \'")            PORT_CODE(KEYCODE_7)            PORT_CHAR('7') PORT_CHAR('\'')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("5 %")             PORT_CODE(KEYCODE_5)            PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("3 #")             PORT_CODE(KEYCODE_3)            PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("DEL INST")        PORT_CODE(KEYCODE_BACKSPACE)    PORT_CHAR('\b') PORT_CHAR(UCHAR_MAMEKEY(INSERT))

	PORT_START( "SPECIAL" )
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("STOP")            PORT_CODE(KEYCODE_END)          PORT_CHAR(UCHAR_MAMEKEY(END))
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("CAPSLOCK")        PORT_CODE(KEYCODE_CAPSLOCK)     PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("LEFT SHIFT")      PORT_CODE(KEYCODE_LSHIFT)       PORT_CHAR(UCHAR_MAMEKEY(LSHIFT))
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("CONTROL")         PORT_CODE(KEYCODE_LCONTROL)     PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("CBM")             PORT_CODE(KEYCODE_LALT)         PORT_CHAR(UCHAR_MAMEKEY(LALT))
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // clears screen and goes into infinite loop
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // clears screen and goes into infinite loop
INPUT_PORTS_END

void clcd_state::clcd(machine_config &config)
{
	/* basic machine hardware */
	M65C02(config, m_maincpu, 2000000);
	m_maincpu->set_addrmap(AS_PROGRAM, &clcd_state::clcd_mem);

	INPUT_MERGER_ANY_HIGH(config, "mainirq").output_handler().set_inputline("maincpu", m65c02_device::IRQ_LINE);

	via6522_device &via0(VIA6522(config, "via0", 2000000));
	via0.writepa_handler().set(FUNC(clcd_state::via0_pa_w));
	via0.writepb_handler().set(FUNC(clcd_state::via0_pb_w));
	via0.cb1_handler().set(FUNC(clcd_state::via0_cb1_w));
	via0.irq_handler().set("mainirq", FUNC(input_merger_device::in_w<0>));

	via6522_device &via1(VIA6522(config, "via1", 2000000));
	via1.writepa_handler().set(FUNC(clcd_state::via1_pa_w));
	via1.writepb_handler().set(FUNC(clcd_state::via1_pb_w));
	via1.irq_handler().set("mainirq", FUNC(input_merger_device::in_w<1>));
	via1.ca2_handler().set(m_centronics, FUNC(centronics_device::write_strobe)).invert();
	via1.cb2_handler().set("speaker", FUNC(speaker_sound_device::level_w));

	MOS6551(config, m_acia, 2000000);
	m_acia->set_xtal(XTAL(1'843'200));
	m_acia->irq_handler().set("mainirq", FUNC(input_merger_device::in_w<2>));
	m_acia->txd_handler().set("rs232", FUNC(rs232_port_device::write_txd));
	m_acia->rts_handler().set("rs232", FUNC(rs232_port_device::write_rts));
	m_acia->dtr_handler().set("rs232", FUNC(rs232_port_device::write_dtr));

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, nullptr));
	rs232.rxd_handler().set(m_acia, FUNC(mos6551_device::write_rxd));
	rs232.dcd_handler().set(m_acia, FUNC(mos6551_device::write_dcd));
	rs232.dsr_handler().set(m_acia, FUNC(mos6551_device::write_dsr));
	rs232.cts_handler().set("via1", FUNC(via6522_device::write_pb4));

	CENTRONICS(config, m_centronics, centronics_devices, nullptr);
	m_centronics->busy_handler().set("via1", FUNC(via6522_device::write_pb6)).invert();

	for (auto &bankdev : m_bankdev)
	{
		ADDRESS_MAP_BANK(config, bankdev);
		bankdev->set_addrmap(AS_PROGRAM, &clcd_state::clcd_banked_mem);
		bankdev->set_endianness(ENDIANNESS_LITTLE);
		bankdev->set_data_width(8);
		bankdev->set_stride(0x400);
	}

	MSM58321(config, m_rtc, XTAL(32'768));
	m_rtc->d0_handler().set("via1", FUNC(via6522_device::write_pa0));
	m_rtc->d1_handler().set("via1", FUNC(via6522_device::write_pa1));
	m_rtc->d2_handler().set("via1", FUNC(via6522_device::write_pa2));
	m_rtc->d3_handler().set("via1", FUNC(via6522_device::write_pa3));
	m_rtc->busy_handler().set("via1", FUNC(via6522_device::write_pa7));
	m_rtc->set_year0(1984);
	m_rtc->set_default_24h(true);

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_LCD));
	screen.set_refresh_hz(80);
	screen.set_screen_update(FUNC(clcd_state::screen_update));
	screen.set_size(480, 128);
	screen.set_visarea_full();
	screen.set_palette("palette");

	PALETTE(config, "palette", FUNC(clcd_state::clcd_palette), 2);

	// sound hardware
	SPEAKER(config, "mono").front_center();
	SPEAKER_SOUND(config, "speaker").add_route(ALL_OUTPUTS, "mono", 0.25);

	RAM(config, "ram").set_default_size("128K").set_extra_options("32K,64K").set_default_value(0);

	NVRAM(config, "nvram").set_custom_handler(FUNC(clcd_state::nvram_init));
}


ROM_START( clcd )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "ss-calc-13apr.u105", 0x00000, 0x8000, CRC(88a587a7) SHA1(b08f3169b7cd696bb6a9b6e6e87a077345377ac4))
	ROM_LOAD( "sept-m-13apr.u104",  0x08000, 0x8000, CRC(41028c3c) SHA1(fcab6f0bbeef178eb8e5ecf82d9c348d8f318a8f))
	ROM_LOAD( "sizapr.u103",        0x10000, 0x8000, CRC(0aa91d9f) SHA1(f0842f370607f95d0a0ec6afafb81bc063c32745))
	ROM_LOAD( "kizapr.u102",        0x18000, 0x8000, CRC(59103d52) SHA1(e49c20b237a78b54c2cb26b133d5903bb60bd8ef))

	ROM_REGION( 0x800, "lcd_char_rom", 0 )
	ROM_LOAD( "lcd-char-rom.u16",   0x00000, 0x800, CRC(7b6d3867) SHA1(cb594801438849f933ddc3e64b03b56f42f59f09))
	ROM_IGNORE(0x800)
ROM_END


/*    YEAR  NAME  PARENT  COMPAT  MACHINE  INPUT  CLASS       INIT        COMPANY                        FULLNAME           FLAGS */
COMP( 1985, clcd, 0,      0,      clcd,    clcd,  clcd_state, empty_init, "Commodore Business Machines", "LCD (Prototype)", 0 )
