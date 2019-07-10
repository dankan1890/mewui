// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Victor 9000 / ACT Sirius 1 emulation

**********************************************************************/

/*

    TODO:

    - contrast
    - MC6852
    - codec sound
    - expansion bus
        - Z80 card
        - Winchester DMA card (Xebec S1410 + Tandon TM502/TM603SE)
        - RAM cards
        - clock cards
    - floppy 8048

*/

#include "emu.h"
#include "bus/centronics/ctronics.h"
#include "bus/ieee488/ieee488.h"
#include "bus/rs232/rs232.h"
#include "cpu/i86/i86.h"
#include "formats/victor9k_dsk.h"
#include "imagedev/floppy.h"
#include "machine/6522via.h"
#include "machine/mc6852.h"
#include "machine/pit8253.h"
#include "machine/pic8259.h"
#include "machine/ram.h"
#include "machine/victor9k_kb.h"
#include "machine/victor9k_fdc.h"
#include "machine/z80dart.h"
#include "sound/hc55516.h"
#include "video/mc6845.h"
#include "emupal.h"
#include "screen.h"
#include "softlist.h"
#include "speaker.h"

#define I8088_TAG       "8l"
#define I8253_TAG       "13h"
#define I8259A_TAG      "7l"
#define UPD7201_TAG     "16e"
#define HD46505S_TAG    "11a"
#define MC6852_TAG      "11b"
#define HC55516_TAG     "15c"
#define M6522_1_TAG     "m6522_1"
#define M6522_2_TAG     "m6522_2"
#define M6522_3_TAG     "14l"
#define DAC0808_0_TAG   "5b"
#define DAC0808_1_TAG   "5c"
#define RS232_A_TAG     "rs232a"
#define RS232_B_TAG     "rs232b"
#define SCREEN_TAG      "screen"
#define KB_TAG          "kb"

class victor9k_state : public driver_device
{
public:
	victor9k_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, I8088_TAG),
		m_ieee488(*this, IEEE488_TAG),
		m_pic(*this, I8259A_TAG),
		m_upd7201(*this, UPD7201_TAG),
		m_ssda(*this, MC6852_TAG),
		m_via1(*this, M6522_1_TAG),
		m_via2(*this, M6522_2_TAG),
		m_via3(*this, M6522_3_TAG),
		m_cvsd(*this, HC55516_TAG),
		m_crtc(*this, HD46505S_TAG),
		m_ram(*this, RAM_TAG),
		m_kb(*this, KB_TAG),
		m_fdc(*this, "fdc"),
		m_centronics(*this, "centronics"),
		m_rs232a(*this, RS232_A_TAG),
		m_rs232b(*this, RS232_B_TAG),
		m_palette(*this, "palette"),
		m_rom(*this, I8088_TAG),
		m_video_ram(*this, "video_ram"),
		m_brt(0),
		m_cont(0),
		m_via1_irq(CLEAR_LINE),
		m_via2_irq(CLEAR_LINE),
		m_via3_irq(CLEAR_LINE),
		m_fdc_irq(CLEAR_LINE),
		m_ssda_irq(CLEAR_LINE),
		m_kbrdy(1),
		m_kbackctl(0)
	{ }

	void victor9k(machine_config &config);

private:
	required_device<cpu_device> m_maincpu;
	required_device<ieee488_device> m_ieee488;
	required_device<pic8259_device> m_pic;
	required_device<upd7201_device> m_upd7201;
	required_device<mc6852_device> m_ssda;
	required_device<via6522_device> m_via1;
	required_device<via6522_device> m_via2;
	required_device<via6522_device> m_via3;
	required_device<hc55516_device> m_cvsd;
	required_device<mc6845_device> m_crtc;
	required_device<ram_device> m_ram;
	required_device<victor_9000_keyboard_device> m_kb;
	required_device<victor_9000_fdc_device> m_fdc;
	required_device<centronics_device> m_centronics;
	required_device<rs232_port_device> m_rs232a;
	required_device<rs232_port_device> m_rs232b;
	required_device<palette_device> m_palette;
	required_memory_region m_rom;
	required_shared_ptr<uint8_t> m_video_ram;

	virtual void machine_start() override;
	virtual void machine_reset() override;

	DECLARE_WRITE8_MEMBER( via1_pa_w );
	DECLARE_WRITE_LINE_MEMBER( write_nfrd );
	DECLARE_WRITE_LINE_MEMBER( write_ndac );
	DECLARE_WRITE8_MEMBER( via1_pb_w );
	DECLARE_WRITE_LINE_MEMBER( via1_irq_w );
	DECLARE_WRITE_LINE_MEMBER( codec_vol_w );

	DECLARE_WRITE8_MEMBER( via2_pa_w );
	DECLARE_WRITE8_MEMBER( via2_pb_w );
	DECLARE_WRITE_LINE_MEMBER( write_ria );
	DECLARE_WRITE_LINE_MEMBER( write_rib );
	DECLARE_WRITE_LINE_MEMBER( via2_irq_w );

	DECLARE_WRITE8_MEMBER( via3_pb_w );
	DECLARE_WRITE_LINE_MEMBER( via3_irq_w );

	DECLARE_WRITE_LINE_MEMBER( fdc_irq_w );

	DECLARE_WRITE_LINE_MEMBER( ssda_irq_w );
	DECLARE_WRITE_LINE_MEMBER( ssda_sm_dtr_w );

	DECLARE_WRITE_LINE_MEMBER( kbrdy_w );
	DECLARE_WRITE_LINE_MEMBER( kbdata_w );
	DECLARE_WRITE_LINE_MEMBER( vert_w );

	MC6845_UPDATE_ROW( crtc_update_row );

	DECLARE_WRITE_LINE_MEMBER( mux_serial_b_w );
	DECLARE_WRITE_LINE_MEMBER( mux_serial_a_w );

	void victor9k_palette(palette_device &palette) const;

	// video state
	int m_brt;
	int m_cont;
	int m_hires;

	// interrupts
	int m_via1_irq;
	int m_via2_irq;
	int m_via3_irq;
	int m_fdc_irq;
	int m_ssda_irq;

	// keyboard
	int m_kbrdy;
	int m_kbackctl;

	void update_kback();

	void victor9k_mem(address_map &map);
};



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define LOG 0



//**************************************************************************
//  ADDRESS MAPS
//**************************************************************************

//-------------------------------------------------
//  ADDRESS_MAP( victor9k_mem )
//-------------------------------------------------

void victor9k_state::victor9k_mem(address_map &map)
{
	map(0x00000, 0x1ffff).ram();
	map(0x20000, 0xdffff).noprw();
	map(0xe0000, 0xe0001).mirror(0x7f00).rw(m_pic, FUNC(pic8259_device::read), FUNC(pic8259_device::write));
	map(0xe0020, 0xe0023).mirror(0x7f00).rw(I8253_TAG, FUNC(pit8253_device::read), FUNC(pit8253_device::write));
	map(0xe0040, 0xe0043).mirror(0x7f00).rw(m_upd7201, FUNC(upd7201_device::cd_ba_r), FUNC(upd7201_device::cd_ba_w));
	map(0xe8000, 0xe8000).mirror(0x7f00).rw(m_crtc, FUNC(mc6845_device::status_r), FUNC(mc6845_device::address_w));
	map(0xe8001, 0xe8001).mirror(0x7f00).rw(m_crtc, FUNC(mc6845_device::register_r), FUNC(mc6845_device::register_w));
	map(0xe8020, 0xe802f).mirror(0x7f00).m(m_via1, FUNC(via6522_device::map));
	map(0xe8040, 0xe804f).mirror(0x7f00).m(m_via2, FUNC(via6522_device::map));
	map(0xe8060, 0xe8061).mirror(0x7f00).rw(m_ssda, FUNC(mc6852_device::read), FUNC(mc6852_device::write));
	map(0xe8080, 0xe808f).mirror(0x7f00).m(m_via3, FUNC(via6522_device::map));
	map(0xe80a0, 0xe80af).mirror(0x7f00).rw(m_fdc, FUNC(victor_9000_fdc_device::cs5_r), FUNC(victor_9000_fdc_device::cs5_w));
	map(0xe80c0, 0xe80cf).mirror(0x7f00).rw(m_fdc, FUNC(victor_9000_fdc_device::cs6_r), FUNC(victor_9000_fdc_device::cs6_w));
	map(0xe80e0, 0xe80ef).mirror(0x7f00).rw(m_fdc, FUNC(victor_9000_fdc_device::cs7_r), FUNC(victor_9000_fdc_device::cs7_w));
	map(0xf0000, 0xf0fff).mirror(0x1000).ram().share("video_ram");
	map(0xf8000, 0xf9fff).mirror(0x6000).rom().region(I8088_TAG, 0);
}



//**************************************************************************
//  INPUT PORTS
//**************************************************************************

//-------------------------------------------------
//  INPUT_PORTS( victor9k )
//-------------------------------------------------

static INPUT_PORTS_START( victor9k )
	// defined in machine/victor9kb.c
INPUT_PORTS_END



//**************************************************************************
//  DEVICE CONFIGURATION
//**************************************************************************

//-------------------------------------------------
//  MC6845
//-------------------------------------------------

#define DC_SECRET   0x1000
#define DC_UNDLN    0x2000
#define DC_LOWINT   0x4000
#define DC_RVS      0x8000

MC6845_UPDATE_ROW( victor9k_state::crtc_update_row )
{
	int hires = BIT(ma, 13);
	int dot_addr = BIT(ma, 12);
	int width = hires ? 16 : 10;

	if (m_hires != hires)
	{
		m_hires = hires;
		m_crtc->set_unscaled_clock(XTAL(30'000'000) / width);
		m_crtc->set_hpixels_per_column(width);
	}

	address_space &program = m_maincpu->space(AS_PROGRAM);
	const rgb_t *palette = m_palette->palette()->entry_list_raw();

	int x = hbp;

	offs_t aa = (ma & 0x7ff) << 1;

	for (int sx = 0; sx < x_count; sx++)
	{
		uint16_t dc = (m_video_ram[aa + 1] << 8) | m_video_ram[aa];
		offs_t ab = (dot_addr << 15) | ((dc & 0x7ff) << 4) | (ra & 0x0f);
		uint16_t dd = program.read_word(ab << 1);

		int cursor = (sx == cursor_x) ? 1 : 0;
		int undln = !((dc & DC_UNDLN) && BIT(dd, 15)) ? 2 : 0;
		int rvs = (dc & DC_RVS) ? 4 : 0;
		int secret = (dc & DC_SECRET) ? 1 : 0;
		int lowint = (dc & DC_LOWINT) ? 1 : 0;

		for (int bit = 0; bit < width; bit++)
		{
			int pixel = 0;

			switch (rvs | undln | cursor)
			{
			case 0: case 5:
				pixel = 1;
				break;

			case 1: case 4:
				pixel = 0;
				break;

			case 2: case 7:
				pixel = !(!(BIT(dd, bit) && !secret));
				break;

			case 3: case 6:
				pixel = !(BIT(dd, bit) && !secret);
				break;
			}

			int color = 0;

			if (pixel && de)
			{
				int pen = 1 + m_brt;
				if (!lowint) pen = 9;
				color = palette[pen];
			}

			bitmap.pix32(vbp + y, x++) = color;
		}

		aa += 2;
		aa &= 0xfff;
	}
}

WRITE_LINE_MEMBER(victor9k_state::vert_w)
{
	m_via2->write_pa7(state);
	m_pic->ir7_w(state);
}



WRITE_LINE_MEMBER(victor9k_state::mux_serial_b_w)
{
}

WRITE_LINE_MEMBER(victor9k_state::mux_serial_a_w)
{
}

//-------------------------------------------------
//  PIC8259
//-------------------------------------------------

/*

    pin     signal      description

    IR0     SYN         sync detect
    IR1     COMM        serial communications (7201)
    IR2     TIMER       8253 timer
    IR3     PARALLEL    all 6522 IRQ (including disk)
    IR4     IR4         expansion IR4
    IR5     IR5         expansion IR5
    IR6     KBINT       keyboard data ready
    IR7     VINT        vertical sync or nonspecific interrupt

*/

//-------------------------------------------------
//  MC6852_INTERFACE( ssda_intf )
//-------------------------------------------------

WRITE_LINE_MEMBER( victor9k_state::ssda_irq_w )
{
	m_ssda_irq = state;

	m_pic->ir3_w(m_ssda_irq || m_via1_irq || m_via3_irq || m_fdc_irq);
}


WRITE_LINE_MEMBER( victor9k_state::ssda_sm_dtr_w )
{
	m_ssda->cts_w(state);
	m_ssda->dcd_w(!state);
	//m_cvsd->enc_dec_w(!state);
}


WRITE8_MEMBER( victor9k_state::via1_pa_w )
{
	/*

	    bit     description

	    PA0     DIO1
	    PA1     DIO2
	    PA2     DIO3
	    PA3     DIO4
	    PA4     DIO5
	    PA5     DIO6
	    PA6     DIO7
	    PA7     DIO8

	*/

	// centronics
	m_centronics->write_data0(BIT(data, 0));
	m_centronics->write_data1(BIT(data, 1));
	m_centronics->write_data2(BIT(data, 2));
	m_centronics->write_data3(BIT(data, 3));
	m_centronics->write_data4(BIT(data, 4));
	m_centronics->write_data5(BIT(data, 5));
	m_centronics->write_data6(BIT(data, 6));
	m_centronics->write_data7(BIT(data, 7));

	// IEEE-488
	m_ieee488->write_dio(data);
}

DECLARE_WRITE_LINE_MEMBER( victor9k_state::write_nfrd )
{
	m_via1->write_pb6(state);
	m_via1->write_ca1(state);
}

DECLARE_WRITE_LINE_MEMBER( victor9k_state::write_ndac )
{
	m_via1->write_pb7(state);
	m_via1->write_ca2(state);
}

WRITE8_MEMBER( victor9k_state::via1_pb_w )
{
	/*

	    bit     description

	    PB0     STROBE/DAV
	    PB1     PI/EOI
	    PB2     REN
	    PB3     ATN
	    PB4     IFC
	    PB5     SRQ/BUSY SRQ
	    PB6     NRFD/ACK RFD
	    PB7     SEL/DAC

	*/

	// centronics
	m_centronics->write_strobe(BIT(data, 0));

	// IEEE-488
	m_ieee488->host_dav_w(BIT(data, 0));
	m_ieee488->host_eoi_w(BIT(data, 1));
	m_ieee488->host_ren_w(BIT(data, 2));
	m_ieee488->host_atn_w(BIT(data, 3));
	m_ieee488->host_ifc_w(BIT(data, 4));
	m_ieee488->host_srq_w(BIT(data, 5));
	m_ieee488->host_nrfd_w(BIT(data, 6));
	m_ieee488->host_ndac_w(BIT(data, 7));
}

WRITE_LINE_MEMBER( victor9k_state::codec_vol_w )
{
}

WRITE_LINE_MEMBER( victor9k_state::via1_irq_w )
{
	m_via1_irq = state;

	m_pic->ir3_w(m_ssda_irq || m_via1_irq || m_via3_irq || m_fdc_irq);
}

WRITE8_MEMBER( victor9k_state::via2_pa_w )
{
	/*

	    bit     description

	    PA0     _INT/EXTA
	    PA1     _INT/EXTB
	    PA2
	    PA3
	    PA4
	    PA5
	    PA6
	    PA7

	*/
}

void victor9k_state::update_kback()
{
	int kback = !(!(m_kbrdy && !m_via2_irq) && !(m_kbackctl && m_via2_irq));

	m_kb->kback_w(kback);
}

WRITE8_MEMBER( victor9k_state::via2_pb_w )
{
	/*

	    bit     description

	    PB0     TALK/LISTEN
	    PB1     KBACKCTL
	    PB2     BRT0
	    PB3     BRT1
	    PB4     BRT2
	    PB5     CONT0
	    PB6     CONT1
	    PB7     CONT2

	*/

	// keyboard acknowledge
	m_kbackctl = BIT(data, 1);
	update_kback();

	// brightness
	m_brt = (data >> 2) & 0x07;

	// contrast
	m_cont = data >> 5;

	if (LOG) logerror("BRT %u CONT %u\n", m_brt, m_cont);
}

WRITE_LINE_MEMBER( victor9k_state::via2_irq_w )
{
	m_via2_irq = state;

	m_pic->ir6_w(m_via2_irq);
	update_kback();
}


WRITE_LINE_MEMBER( victor9k_state::write_ria )
{
	m_upd7201->ria_w(state);
	m_via2->write_pa2(state);
}


WRITE_LINE_MEMBER( victor9k_state::write_rib )
{
	m_upd7201->rib_w(state);
	m_via2->write_pa4(state);
}


/*
    bit    description

    PA0    J5-16
    PA1    J5-18
    PA2    J5-20
    PA3    J5-22
    PA4    J5-24
    PA5    J5-26
    PA6    J5-28
    PA7    J5-30
    PB0    J5-32
    PB1    J5-34
    PB2    J5-36
    PB3    J5-38
    PB4    J5-40
    PB5    J5-42
    PB6    J5-44
    PB7    J5-46
    CA1    J5-12
    CB1    J5-48
    CA2    J5-14
    CB2    J5-50
*/

WRITE8_MEMBER( victor9k_state::via3_pb_w )
{
	// codec clock output
	m_ssda->rx_clk_w(!BIT(data, 7));
	m_ssda->tx_clk_w(!BIT(data, 7));
	m_cvsd->clock_w(!BIT(data, 7));
}

WRITE_LINE_MEMBER( victor9k_state::via3_irq_w )
{
	m_via3_irq = state;

	m_pic->ir3_w(m_ssda_irq || m_via1_irq || m_via3_irq || m_fdc_irq);
}


//-------------------------------------------------
//  VICTOR9K_KEYBOARD_INTERFACE( kb_intf )
//-------------------------------------------------

WRITE_LINE_MEMBER( victor9k_state::kbrdy_w )
{
	if (LOG) logerror("KBRDY %u\n", state);

	m_via2->write_cb1(state);

	m_kbrdy = state;
	update_kback();
}

WRITE_LINE_MEMBER( victor9k_state::kbdata_w )
{
	if (LOG) logerror("KBDATA %u\n", state);

	m_via2->write_cb2(state);
	m_via2->write_pa6(state);
}


WRITE_LINE_MEMBER( victor9k_state::fdc_irq_w )
{
	m_fdc_irq = state;

	m_pic->ir3_w(m_ssda_irq || m_via1_irq || m_via3_irq || m_fdc_irq);
}


//**************************************************************************
//  MACHINE INITIALIZATION
//**************************************************************************

void victor9k_state::victor9k_palette(palette_device &palette) const
{
	palette.set_pen_color(0, rgb_t(0x00, 0x00, 0x00));

	// BRT0 82K
	// BRT1 39K
	// BRT2 20K
	// 12V 220K pullup
	palette.set_pen_color(1, rgb_t(0x00, 0x10, 0x00));
	palette.set_pen_color(2, rgb_t(0x00, 0x20, 0x00));
	palette.set_pen_color(3, rgb_t(0x00, 0x40, 0x00));
	palette.set_pen_color(4, rgb_t(0x00, 0x60, 0x00));
	palette.set_pen_color(5, rgb_t(0x00, 0x80, 0x00));
	palette.set_pen_color(6, rgb_t(0x00, 0xa0, 0x00));
	palette.set_pen_color(7, rgb_t(0x00, 0xc0, 0x00));
	palette.set_pen_color(8, rgb_t(0x00, 0xff, 0x00));

	// CONT0 620R
	// CONT1 332R
	// CONT2 162R
	// 12V 110R pullup
	palette.set_pen_color(9, rgb_t(0xff, 0x00, 0x00));
}

void victor9k_state::machine_start()
{
	// state saving
	save_item(NAME(m_brt));
	save_item(NAME(m_cont));
	save_item(NAME(m_via1_irq));
	save_item(NAME(m_via2_irq));
	save_item(NAME(m_via3_irq));
	save_item(NAME(m_fdc_irq));
	save_item(NAME(m_ssda_irq));
	save_item(NAME(m_kbrdy));
	save_item(NAME(m_kbackctl));

#ifndef USE_SCP
	// patch out SCP self test
	m_rom->base()[0x11ab] = 0xc3;

	// patch out ROM checksum error
	m_rom->base()[0x1d51] = 0x90;
	m_rom->base()[0x1d52] = 0x90;
	m_rom->base()[0x1d53] = 0x90;
	m_rom->base()[0x1d54] = 0x90;
#endif
}

void victor9k_state::machine_reset()
{
	m_maincpu->reset();
	m_upd7201->reset();
	m_ssda->reset();
	m_via1->reset();
	m_via2->reset();
	m_via3->reset();
	m_crtc->reset();
	m_fdc->reset();
}



//**************************************************************************
//  MACHINE CONFIGURATION
//**************************************************************************

//-------------------------------------------------
//  machine_config( victor9k )
//-------------------------------------------------

void victor9k_state::victor9k(machine_config &config)
{
	// basic machine hardware
	I8088(config, m_maincpu, XTAL(30'000'000)/6);
	m_maincpu->set_addrmap(AS_PROGRAM, &victor9k_state::victor9k_mem);
	m_maincpu->set_irq_acknowledge_callback(I8259A_TAG, FUNC(pic8259_device::inta_cb));

	// video hardware
	screen_device &screen(SCREEN(config, SCREEN_TAG, SCREEN_TYPE_RASTER));
	screen.set_color(rgb_t::green());
	screen.set_refresh_hz(50);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); // not accurate
	screen.set_screen_update(HD46505S_TAG, FUNC(hd6845s_device::screen_update));
	screen.set_size(640, 480);
	screen.set_visarea(0, 640-1, 0, 480-1);

	PALETTE(config, m_palette, FUNC(victor9k_state::victor9k_palette), 16);

	HD6845S(config, m_crtc, XTAL(30'000'000)/10); // HD6845 == HD46505S
	m_crtc->set_screen(SCREEN_TAG);
	m_crtc->set_show_border_area(true);
	m_crtc->set_char_width(10);
	m_crtc->set_update_row_callback(FUNC(victor9k_state::crtc_update_row), this);
	m_crtc->out_vsync_callback().set(FUNC(victor9k_state::vert_w));

	// sound hardware
	SPEAKER(config, "mono").front_center();
	HC55516(config, m_cvsd, 0);
	//MCFG_HC55516_DIG_OUT_CB(WRITELINE(MC6852_TAG, mc6852_device, rx_w))
	m_cvsd->add_route(ALL_OUTPUTS, "mono", 0.25);

	// devices
	IEEE488(config, m_ieee488, 0);

	m_ieee488->dav_callback().set(M6522_1_TAG, FUNC(via6522_device::write_pb0));
	m_ieee488->eoi_callback().set(M6522_1_TAG, FUNC(via6522_device::write_pb1));
	m_ieee488->ren_callback().set(M6522_1_TAG, FUNC(via6522_device::write_pb2));
	m_ieee488->atn_callback().set(M6522_1_TAG, FUNC(via6522_device::write_pb3));
	m_ieee488->ifc_callback().set(M6522_1_TAG, FUNC(via6522_device::write_pb4));
	m_ieee488->srq_callback().set(M6522_1_TAG, FUNC(via6522_device::write_pb5));
	m_ieee488->nrfd_callback().set(FUNC(victor9k_state::write_nfrd));
	m_ieee488->ndac_callback().set(FUNC(victor9k_state::write_ndac));

	PIC8259(config, m_pic, 0);
	m_pic->out_int_callback().set_inputline(m_maincpu, INPUT_LINE_IRQ0);

	pit8253_device &pit(PIT8253(config, I8253_TAG, 0));
	pit.set_clk<0>(2500000);
	pit.out_handler<0>().set(FUNC(victor9k_state::mux_serial_b_w));
	pit.set_clk<1>(2500000);
	pit.out_handler<1>().set(FUNC(victor9k_state::mux_serial_a_w));
	pit.set_clk<2>(100000);
	pit.out_handler<2>().set(I8259A_TAG, FUNC(pic8259_device::ir2_w));

	UPD7201(config, m_upd7201, XTAL(30'000'000)/30);
	m_upd7201->out_txda_callback().set(RS232_A_TAG, FUNC(rs232_port_device::write_txd));
	m_upd7201->out_dtra_callback().set(RS232_A_TAG, FUNC(rs232_port_device::write_dtr));
	m_upd7201->out_rtsa_callback().set(RS232_A_TAG, FUNC(rs232_port_device::write_rts));
	m_upd7201->out_txdb_callback().set(RS232_B_TAG, FUNC(rs232_port_device::write_txd));
	m_upd7201->out_dtrb_callback().set(RS232_B_TAG, FUNC(rs232_port_device::write_dtr));
	m_upd7201->out_rtsb_callback().set(RS232_B_TAG, FUNC(rs232_port_device::write_rts));
	m_upd7201->out_int_callback().set(I8259A_TAG, FUNC(pic8259_device::ir1_w));

	MC6852(config, m_ssda, XTAL(30'000'000)/30);
	m_ssda->tx_data_callback().set(HC55516_TAG, FUNC(hc55516_device::digit_w));
	m_ssda->sm_dtr_callback().set(FUNC(victor9k_state::ssda_sm_dtr_w));
	m_ssda->irq_callback().set(FUNC(victor9k_state::ssda_irq_w));

	VIA6522(config, m_via1, XTAL(30'000'000)/30);
	m_via1->readpa_handler().set(IEEE488_TAG, FUNC(ieee488_device::dio_r));
	m_via1->writepa_handler().set(FUNC(victor9k_state::via1_pa_w));
	m_via1->writepb_handler().set(FUNC(victor9k_state::via1_pb_w));
	m_via1->cb2_handler().set(FUNC(victor9k_state::codec_vol_w));
	m_via1->irq_handler().set(FUNC(victor9k_state::via1_irq_w));

	VIA6522(config, m_via2, XTAL(30'000'000)/30);
	m_via2->writepa_handler().set(FUNC(victor9k_state::via2_pa_w));
	m_via2->writepb_handler().set(FUNC(victor9k_state::via2_pb_w));
	m_via2->irq_handler().set(FUNC(victor9k_state::via2_irq_w));

	VIA6522(config, m_via3, XTAL(30'000'000)/30);
	m_via3->writepb_handler().set(FUNC(victor9k_state::via3_pb_w));
	m_via3->irq_handler().set(FUNC(victor9k_state::via3_irq_w));

	CENTRONICS(config, m_centronics, centronics_devices, "printer");
	m_centronics->busy_handler().set(M6522_1_TAG, FUNC(via6522_device::write_pb5));
	m_centronics->ack_handler().set(M6522_1_TAG, FUNC(via6522_device::write_pb6));
	m_centronics->select_handler().set(M6522_1_TAG, FUNC(via6522_device::write_pb7));

	RS232_PORT(config, m_rs232a, default_rs232_devices, nullptr);
	m_rs232a->rxd_handler().set(UPD7201_TAG, FUNC(z80dart_device::rxa_w));
	m_rs232a->dcd_handler().set(UPD7201_TAG, FUNC(z80dart_device::dcda_w));
	m_rs232a->ri_handler().set(FUNC(victor9k_state::write_ria));
	m_rs232a->cts_handler().set(UPD7201_TAG, FUNC(z80dart_device::ctsa_w));
	m_rs232a->dsr_handler().set(M6522_2_TAG, FUNC(via6522_device::write_pa3));

	RS232_PORT(config, m_rs232b, default_rs232_devices, nullptr);
	m_rs232b->rxd_handler().set(UPD7201_TAG, FUNC(z80dart_device::rxb_w));
	m_rs232b->dcd_handler().set(UPD7201_TAG, FUNC(z80dart_device::dcdb_w));
	m_rs232b->ri_handler().set(FUNC(victor9k_state::write_ria));
	m_rs232b->cts_handler().set(UPD7201_TAG, FUNC(z80dart_device::ctsb_w));
	m_rs232b->dsr_handler().set(M6522_2_TAG, FUNC(via6522_device::write_pa5));

	VICTOR9K_KEYBOARD(config, m_kb, 0);
	m_kb->kbrdy_handler().set(FUNC(victor9k_state::kbrdy_w));
	m_kb->kbdata_handler().set(FUNC(victor9k_state::kbdata_w));

	VICTOR_9000_FDC(config, m_fdc, 0);
	m_fdc->irq_wr_callback().set(FUNC(victor9k_state::fdc_irq_w));
	m_fdc->syn_wr_callback().set(I8259A_TAG, FUNC(pic8259_device::ir0_w)).invert();
	m_fdc->lbrdy_wr_callback().set_inputline(I8088_TAG, INPUT_LINE_TEST).invert();

	RAM(config, m_ram).set_default_size("128K");

	SOFTWARE_LIST(config, "flop_list").set_type("victor9k_flop", SOFTWARE_LIST_ORIGINAL_SYSTEM);
}



//**************************************************************************
//  ROMS
//**************************************************************************

//-------------------------------------------------
//  ROM( victor9k )
//-------------------------------------------------

ROM_START( victor9k )
	ROM_REGION( 0x2000, I8088_TAG, 0 )
	ROM_DEFAULT_BIOS( "univ" )
	ROM_SYSTEM_BIOS( 0, "old", "Older" )
	ROMX_LOAD( "102320.7j", 0x0000, 0x1000, CRC(3d615fd7) SHA1(b22f7e5d66404185395d8effbf57efded0079a92), ROM_BIOS(0) )
	ROMX_LOAD( "102322.8j", 0x1000, 0x1000, CRC(9209df0e) SHA1(3ee8e0c15186bbd5768b550ecc1fa3b6b1dbb928), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "univ", "Universal" )
	ROMX_LOAD( "v9000 univ. fe f3f7 13db.7j", 0x0000, 0x1000, CRC(25c7a59f) SHA1(8784e9aa7eb9439f81e18b8e223c94714e033911), ROM_BIOS(1) )
	ROMX_LOAD( "v9000 univ. ff f3f7 39fe.8j", 0x1000, 0x1000, CRC(496c7467) SHA1(eccf428f62ef94ab85f4a43ba59ae6a066244a66), ROM_BIOS(1) )
ROM_END



//**************************************************************************
//  SYSTEM DRIVERS
//**************************************************************************

//    YEAR  NAME      PARENT  COMPAT  MACHINE   INPUT     CLASS           INIT        COMPANY                     FULLNAME       FLAGS
COMP( 1982, victor9k, 0,      0,      victor9k, victor9k, victor9k_state, empty_init, "Victor Business Products", "Victor 9000", MACHINE_IMPERFECT_COLORS | MACHINE_IMPERFECT_SOUND | MACHINE_SUPPORTS_SAVE )
