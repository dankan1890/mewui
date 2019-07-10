// license:BSD-3-Clause
// copyright-holders:Sergey Svishchev
/***************************************************************************

    Eltec Eurocom II V7 single-board computer.  Used in PPG Waveterm A.

    to do:
    - Eurocom board: timer, interrupts
    - Waveterm: PTM, ADC, DAC, 'end' button
    - autoboot Waveterm software without -debug being necessary
    - support more disk image formats (.dsk, flexemu .flx)

    manuals:
    - http://seib.synth.net/documents/EurocomIIHW.pdf
    - http://seib.synth.net/documents/wt_b_serv.pdf
    - http://seib.synth.net/documents/wt_a_usrd.pdf
    - http://seib.synth.net/documents/wavetermbroc.pdf

    useful links:
    - http://www.ppg.synth.net/waveterm/
    - http://www.hermannseib.com/english/synths/ppg/waveterm.htm
    - http://www.synthmuseum.com/ppg/ppgwterm01.html
    - http://www.theppgs.com/waveterma.html
    - http://machines.hyperreal.org/manufacturers/PPG/info/ppg.waveterm.revisions.txt
    - http://www.flexusergroup.com/flexusergroup/default.htm
    - http://web.archive.org/web/20091026234737/http://geocities.com/flexemu/
    - http://oldcomputer.info/8bit/microtrol/index.htm

****************************************************************************/

#include "emu.h"

#include "bus/rs232/rs232.h"
#include "cpu/m6809/m6809.h"
#include "formats/ppg_dsk.h"
#include "imagedev/floppy.h"
#include "machine/6821pia.h"
#include "machine/6840ptm.h"
#include "machine/6850acia.h"
#include "machine/keyboard.h"
#include "machine/wd_fdc.h"

#include "emupal.h"
#include "screen.h"


#define VC_TOTAL_HORZ 678
#define VC_DISP_HORZ  512

#define VC_TOTAL_VERT 312
#define VC_DISP_VERT  256


//#define LOG_GENERAL (1U <<  0) //defined in logmacro.h already
#define LOG_KEYBOARD  (1U <<  1)
#define LOG_DEBUG     (1U <<  2)

//#define VERBOSE (LOG_DEBUG)
//#define LOG_OUTPUT_FUNC printf
#include "logmacro.h"

#define LOGKBD(...) LOGMASKED(LOG_KEYBOARD, __VA_ARGS__)
#define LOGDBG(...) LOGMASKED(LOG_DEBUG, __VA_ARGS__)


class eurocom2_state : public driver_device
{
public:
	eurocom2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_pia1(*this, "pia1")
		, m_pia2(*this, "pia2")
		, m_acia(*this, "acia")
		, m_fdc(*this, "fdc")
		, m_p_videoram(*this, "videoram")
		, m_screen(*this, "screen")
	{
	}

	void eurocom2(machine_config &config);
	void microtrol(machine_config &config);
protected:
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	DECLARE_READ8_MEMBER(fdc_aux_r);
	DECLARE_WRITE8_MEMBER(fdc_aux_w);
	DECLARE_FLOPPY_FORMATS(floppy_formats);

	DECLARE_WRITE8_MEMBER(vico_w);

	DECLARE_READ8_MEMBER(kbd_get);
	void kbd_put(u8 data);

	DECLARE_READ_LINE_MEMBER(pia1_ca1_r);
	DECLARE_READ_LINE_MEMBER(pia1_ca2_r);
	DECLARE_READ_LINE_MEMBER(pia1_cb1_r);
	DECLARE_WRITE_LINE_MEMBER(pia1_cb2_w);

	void eurocom2_map(address_map &map);

	// driver_device overrides
	virtual void machine_reset() override;
	virtual void machine_start() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	emu_timer *m_sst;

	floppy_image_device *m_floppy;
	bool m_sst_state, m_kbd_ready;
	bitmap_ind16 m_tmpbmp;

	uint8_t m_vico[2];
	uint8_t m_kbd_data;

	required_device<cpu_device> m_maincpu;
	required_device<pia6821_device> m_pia1;
	required_device<pia6821_device> m_pia2;
	required_device<acia6850_device> m_acia;
	required_device<fd1793_device> m_fdc;
	required_shared_ptr<uint8_t> m_p_videoram;
	required_device<screen_device> m_screen;
};

class waveterm_state : public eurocom2_state
{
public:
	waveterm_state(const machine_config &mconfig, device_type type, const char *tag)
		: eurocom2_state(mconfig, type, tag)
		, m_pia3(*this, "pia3")
		, m_ptm(*this, "ptm")
	{ }

	DECLARE_READ8_MEMBER(waveterm_kb_r);
	DECLARE_WRITE8_MEMBER(waveterm_kb_w);
	DECLARE_WRITE_LINE_MEMBER(waveterm_kbh_w);

	DECLARE_READ8_MEMBER(pia3_pa_r);
	DECLARE_WRITE8_MEMBER(pia3_pa_w);
	DECLARE_WRITE8_MEMBER(pia3_pb_w);
	DECLARE_READ_LINE_MEMBER(pia3_ca1_r);
	DECLARE_READ_LINE_MEMBER(pia3_ca2_r);
	DECLARE_WRITE_LINE_MEMBER(pia3_cb2_w);

	DECLARE_READ8_MEMBER(waveterm_adc);
	DECLARE_WRITE8_MEMBER(waveterm_dac);

	void waveterm(machine_config &config);
	void waveterm_map(address_map &map);
protected:
	bool m_driveh;
	uint8_t m_drive;

	required_device<pia6821_device> m_pia3;
	required_device<ptm6840_device> m_ptm;
};


/*
 * b0 -- timer output
 * b1 -- 1 == two-sided diskette in drive
 * b2..b5 nc
 * b6 -- irq
 * b7 -- drq
 */
READ8_MEMBER(eurocom2_state::fdc_aux_r)
{
	uint8_t data = 0;

	data |= (m_floppy ? m_floppy->twosid_r() : 1) << 1;
	data |= (m_fdc->intrq_r() << 6);
	data |= (m_fdc->drq_r() << 7);

	LOGDBG("Floppy %d == %02x\n", offset, data);

	return data;
}

/*
 * b0 -- 1 = select 0
 * ..
 * b3 -- 1 = select 3
 * b4 -- 1 = top head
 * b5 -- 1 = single density
 * b6 -- nc
 * b7 -- 1 = enable timer interrupt
 */
WRITE8_MEMBER(eurocom2_state::fdc_aux_w)
{
	floppy_image_device *floppy0 = m_fdc->subdevice<floppy_connector>("0")->get_device();
	floppy_image_device *floppy1 = m_fdc->subdevice<floppy_connector>("1")->get_device();

	if (BIT(data, 0))
		m_floppy = floppy0;
	else if (BIT(data, 1))
		m_floppy = floppy1;
	else
		m_floppy = nullptr;

	if (m_floppy)
	{
		m_fdc->set_floppy(m_floppy);
		m_floppy->ss_w(BIT(data, 4));
		m_floppy->mon_w(0);
	}
	else
	{
		floppy0->mon_w(1);
		floppy1->mon_w(1);
	}

	m_fdc->dden_w(BIT(data, 5));

	LOGDBG("Floppy %d <- %02x\n", offset, data);
}

WRITE8_MEMBER(eurocom2_state::vico_w)
{
	LOG("VICO %d <- %02x\n", offset, data);

	m_vico[offset & 1] = data;
}


READ_LINE_MEMBER(eurocom2_state::pia1_ca2_r)
{
	LOGDBG("PIA1 CA2 == %d (SST Q14)\n", m_sst_state);

	return m_sst_state;
}

READ_LINE_MEMBER(eurocom2_state::pia1_cb1_r)
{
	LOGDBG("PIA1 CB1 == %d (SST Q6)\n", m_sst_state);

	return m_sst_state;
}

WRITE_LINE_MEMBER(eurocom2_state::pia1_cb2_w)
{
	LOG("PIA1 CB2 <- %d (SST reset)\n", state);
	// reset single-step timer
}

void eurocom2_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	m_sst_state = !m_sst_state;
	m_pia1->ca2_w(m_sst_state);
}


READ_LINE_MEMBER(eurocom2_state::pia1_ca1_r)
{
	return m_kbd_ready;
}

/* bit 7 may be connected to something else -- see section 6.2 of Eurocom manual */
READ8_MEMBER(eurocom2_state::kbd_get)
{
	return m_kbd_data;
}

void eurocom2_state::kbd_put(u8 data)
{
	m_kbd_ready = true;
	m_kbd_data = data;
	m_pia1->ca1_w(false);
	m_pia1->ca1_w(true);
}


READ8_MEMBER(waveterm_state::waveterm_kb_r)
{
	uint8_t data = 0xff;

	if (BIT(m_drive, 0)) data &= ioport("ROW.0")->read();
	if (BIT(m_drive, 1)) data &= ioport("ROW.1")->read();
	if (BIT(m_drive, 2)) data &= ioport("ROW.2")->read();
	if (BIT(m_drive, 3)) data &= ioport("ROW.3")->read();
	if (m_driveh)        data &= ioport("ROW.4")->read();

	return data;
}

WRITE8_MEMBER(waveterm_state::waveterm_kb_w)
{
	m_drive = (~data) >> 4;
}

WRITE_LINE_MEMBER(waveterm_state::waveterm_kbh_w)
{
	m_driveh = !state;
}

WRITE8_MEMBER(waveterm_state::pia3_pb_w)
{
}

READ8_MEMBER(waveterm_state::waveterm_adc)
{
	return m_screen->frame_number() % 255; // XXX
}


uint32_t eurocom2_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	int x, y, offset, page;
	uint16_t gfx, *p;

	page = (m_vico[0] & 3) << 14;

	for (y = 0; y < VC_DISP_VERT; y++)
	{
		offset = (VC_DISP_HORZ / 8) * ((m_vico[1] + y) % VC_DISP_VERT);
		p = &m_tmpbmp.pix16(y);

		for (x = offset; x < offset + VC_DISP_HORZ / 8; x++)
		{
			gfx = m_p_videoram[page + x];

			for (int i = 7; i >= 0; i--)
			{
				*p++ = BIT(gfx, i);
			}
		}
	}

	copybitmap(bitmap, m_tmpbmp, 0, 0, 0, 0, cliprect);

	return 0;
}

void eurocom2_state::eurocom2_map(address_map &map)
{
	map(0x0000, 0xefff).ram().share("videoram");
	map(0xf000, 0xfcef).rom().region("maincpu", 0);
	map(0xfcf0, 0xfcf3).rw(m_pia1, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	map(0xfcf4, 0xfcf5).rw(m_acia, FUNC(acia6850_device::read), FUNC(acia6850_device::write));
	map(0xfcf6, 0xfcf7).w(FUNC(eurocom2_state::vico_w));
	map(0xfcf8, 0xfcfb).rw(m_pia2, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	map(0xfd30, 0xfd37).rw(m_fdc, FUNC(fd1793_device::read), FUNC(fd1793_device::write));
	map(0xfd38, 0xfd38).rw(FUNC(eurocom2_state::fdc_aux_r), FUNC(eurocom2_state::fdc_aux_w));
	map(0xfd40, 0xffff).rom().region("maincpu", 0xd40).nopw();
}

void waveterm_state::waveterm_map(address_map &map)
{
	eurocom2_map(map);
	map(0xfd00, 0xfd03).rw(m_pia3, FUNC(pia6821_device::read), FUNC(pia6821_device::write));
	map(0xfd08, 0xfd0f).rw(m_ptm, FUNC(ptm6840_device::read), FUNC(ptm6840_device::write));
	map(0xfd10, 0xfd17).unmaprw();
	map(0xfd18, 0xfd18).r(FUNC(waveterm_state::waveterm_adc));  //  AD558 ADC
//  AM_RANGE(0xfd20, 0xfd20) AM_READ(waveterm_dac)  //  ZN432 DAC ??
}

static INPUT_PORTS_START(eurocom2)
	PORT_START("S1")
	PORT_DIPNAME(0x0f, 0x01, "Serial baud rate")
	PORT_DIPSETTING(0x01, "9600")

	PORT_DIPNAME(0x40, 0x40, "7 or 8-bit keyboard")
	PORT_DIPSETTING(0x40, "8-bit")
	PORT_DIPSETTING(0x00, "7-bit")

	PORT_DIPNAME(0x80, 0x80, "Periodic timer")
	PORT_DIPSETTING(0x80, DEF_STR(Yes))
	PORT_DIPSETTING(0x00, DEF_STR(No))
INPUT_PORTS_END

static INPUT_PORTS_START(waveterm)
	PORT_INCLUDE(eurocom2)

	PORT_START("FP")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("End") PORT_CODE(KEYCODE_PRTSCR) PORT_CHAR(UCHAR_MAMEKEY(PRTSCR))

	PORT_START("ROW.0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F1") PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F2") PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F3") PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F4") PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F4))

	PORT_START("ROW.1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F5") PORT_CODE(KEYCODE_F5) PORT_CHAR(UCHAR_MAMEKEY(F5))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F6") PORT_CODE(KEYCODE_F6) PORT_CHAR(UCHAR_MAMEKEY(F6))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F7") PORT_CODE(KEYCODE_F7) PORT_CHAR(UCHAR_MAMEKEY(F7))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F8") PORT_CODE(KEYCODE_F8) PORT_CHAR(UCHAR_MAMEKEY(F8))

	PORT_START("ROW.2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F9") PORT_CODE(KEYCODE_F9) PORT_CHAR(UCHAR_MAMEKEY(F9))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("F0") PORT_CODE(KEYCODE_F10) PORT_CHAR(UCHAR_MAMEKEY(F10))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 1") PORT_CODE(KEYCODE_1_PAD) PORT_CHAR(UCHAR_MAMEKEY(1_PAD))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 2") PORT_CODE(KEYCODE_2_PAD) PORT_CHAR(UCHAR_MAMEKEY(2_PAD))

	PORT_START("ROW.3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 3") PORT_CODE(KEYCODE_3_PAD) PORT_CHAR(UCHAR_MAMEKEY(3_PAD))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 4") PORT_CODE(KEYCODE_4_PAD) PORT_CHAR(UCHAR_MAMEKEY(4_PAD))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 5") PORT_CODE(KEYCODE_5_PAD) PORT_CHAR(UCHAR_MAMEKEY(5_PAD))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 6") PORT_CODE(KEYCODE_6_PAD) PORT_CHAR(UCHAR_MAMEKEY(6_PAD))

	PORT_START("ROW.4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 7") PORT_CODE(KEYCODE_7_PAD) PORT_CHAR(UCHAR_MAMEKEY(7_PAD))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 8") PORT_CODE(KEYCODE_8_PAD) PORT_CHAR(UCHAR_MAMEKEY(8_PAD))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 9") PORT_CODE(KEYCODE_9_PAD) PORT_CHAR(UCHAR_MAMEKEY(9_PAD))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Num 0") PORT_CODE(KEYCODE_0_PAD) PORT_CHAR(UCHAR_MAMEKEY(0_PAD))
INPUT_PORTS_END


void eurocom2_state::machine_reset()
{
	m_kbd_ready = false;
	m_floppy = nullptr;

	if (ioport("S1")->read() & 0x80)
		m_sst->adjust(attotime::from_usec(12200), 0, attotime::from_usec(12200));
	else
		m_sst->adjust(attotime::never, 0, attotime::never);
}

void eurocom2_state::machine_start()
{
	m_sst = timer_alloc(0);
	m_tmpbmp.allocate(VC_DISP_HORZ, VC_DISP_VERT);
}


FLOPPY_FORMATS_MEMBER( eurocom2_state::floppy_formats )
	FLOPPY_PPG_FORMAT
FLOPPY_FORMATS_END

static void eurocom_floppies(device_slot_interface &device)
{
	device.option_add("525qd", FLOPPY_525_QD);
	device.option_add("8dsdd", FLOPPY_8_DSDD);
}

void eurocom2_state::eurocom2(machine_config &config)
{
	MC6809(config, m_maincpu, 10.7172_MHz_XTAL / 2); // EXTAL = CLK/2 = 5.3586 MHz; Q = E = 1.33965 MHz
	m_maincpu->set_addrmap(AS_PROGRAM, &eurocom2_state::eurocom2_map);

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER, rgb_t::green());
	m_screen->set_raw(10.7172_MHz_XTAL, VC_TOTAL_HORZ, 0, VC_DISP_HORZ, VC_TOTAL_VERT, 0, VC_DISP_VERT);
	m_screen->set_screen_update(FUNC(eurocom2_state::screen_update));
	m_screen->set_palette("palette");

	PALETTE(config, "palette", palette_device::MONOCHROME);

	generic_keyboard_device &keyboard(GENERIC_KEYBOARD(config, "keyboard", 0));
	keyboard.set_keyboard_callback(FUNC(eurocom2_state::kbd_put));

	PIA6821(config, m_pia1, 0);
	m_pia1->readca1_handler().set(FUNC(eurocom2_state::pia1_ca1_r));  // keyboard strobe
	m_pia1->readca2_handler().set(FUNC(eurocom2_state::pia1_ca2_r));  // SST output Q14
	m_pia1->readcb1_handler().set(FUNC(eurocom2_state::pia1_cb1_r));  // SST output Q6
	m_pia1->cb2_handler().set(FUNC(eurocom2_state::pia1_cb2_w)); // SST reset input
	m_pia1->readpa_handler().set(FUNC(eurocom2_state::kbd_get));
//  m_pia1->readpb_handler().set(FUNC(eurocom2_state::kbd_get));
//  m_pia1->irqa_handler().set_inputline("maincpu", M6809_IRQ_LINE);
//  m_pia1->irqb_handler().set_inputline("maincpu", M6809_IRQ_LINE);

	PIA6821(config, m_pia2, 0);
//  m_pia2->irqa_handler().set_inputline("maincpu", M6809_FIRQ_LINE);
//  m_pia2->irqb_handler().set_inputline("maincpu", M6809_FIRQ_LINE);

	ACIA6850(config, m_acia, 0);
	m_acia->txd_handler().set("rs232", FUNC(rs232_port_device::write_txd));
	m_acia->rts_handler().set("rs232", FUNC(rs232_port_device::write_rts));
	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, nullptr));
	rs232.rxd_handler().set(m_acia, FUNC(acia6850_device::write_rxd));
	rs232.cts_handler().set(m_acia, FUNC(acia6850_device::write_cts));

	FD1793(config, m_fdc, 2_MHz_XTAL / 2);
//  m_fdc->intrq_wr_callback().set_inputline(m_maincpu, M6809_IRQ_LINE);
	FLOPPY_CONNECTOR(config, "fdc:0", eurocom_floppies, "525qd", eurocom2_state::floppy_formats);// enable_sound(true);
	FLOPPY_CONNECTOR(config, "fdc:1", eurocom_floppies, "525qd", eurocom2_state::floppy_formats);// enable_sound(true);
}

void waveterm_state::waveterm(machine_config &config)
{
	eurocom2(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &waveterm_state::waveterm_map);

	m_pia2->cb2_handler().set(FUNC(waveterm_state::waveterm_kbh_w));
	m_pia2->writepb_handler().set(FUNC(waveterm_state::waveterm_kb_w));
	m_pia2->readpb_handler().set(FUNC(waveterm_state::waveterm_kb_r));

	// ports A(in/out), B(out), CA1(in), CA2(in), and CB2(out) = interface to PPG bus via DIL socket on WTI board
	// CB1 -- front panel "End" button
	PIA6821(config, m_pia3, 0);
//  m_pia3->readpa_handler().set(FUNC(waveterm_state::pia3_pa_r));
//  m_pia3->writepa_handler().set(FUNC(waveterm_state::pia3_pa_w));
	m_pia3->writepb_handler().set(FUNC(waveterm_state::pia3_pb_w));
//  m_pia3->readca1_handler().set(FUNC(waveterm_state::pia3_ca1_r));
//  m_pia3->readca2_handler().set(FUNC(waveterm_state::pia3_ca2_r));
	m_pia3->readcb1_handler().set_ioport("FP");
//  m_pia3->cb2_handler().set(FUNC(waveterm_state::pia3_cb2_w));

	PTM6840(config, m_ptm, 0);

	SOFTWARE_LIST(config, "disk_list").set_original("waveterm");
}

void eurocom2_state::microtrol(machine_config &config)
{
	eurocom2(config);

	// TODO: Second board has WD2793A FDC and what looks like a RAM disk
}


ROM_START(eurocom2)
	ROM_REGION(0x1000, "maincpu", 0)

	ROM_DEFAULT_BIOS("mon54")
	ROM_SYSTEM_BIOS(0, "mon24", "Eurocom Control V2.4")
	ROMX_LOAD("mon24.bin", 0x0000, 0x1000, CRC(abf5e115) SHA1(d056705779e109bb56c82f906e2e5a52efe77ec1), ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "mon53", "Eurocom Control V5.3")
	ROMX_LOAD("mon53.bin", 0x0000, 0x1000, CRC(fb39c2ad) SHA1(8ce07c349c56f92503f11bb63e32e32c139c003a), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "mon54", "Eurocom Control V5.4")
	ROMX_LOAD("mon54.bin", 0x0000, 0x1000, CRC(2c5a4ad2) SHA1(67b9deec5a6a71d768e35ac97c16cb8992ae159f), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(3, "monu546", "Eurocom Control U5.4")
	ROMX_LOAD("monu54-6.bin", 0x0000, 0x1000, CRC(80c82fa8) SHA1(7255bc2dd536d3dd08cca3ea46992e5ca59323b1), ROM_BIOS(3))
	ROM_SYSTEM_BIOS(4, "neumon54", "New Monitor 5.4")
	ROMX_LOAD("neumon54.bin", 0x0000, 0x1000, CRC(2b60ca41) SHA1(c7252d2e9b267b046f4f3ea6cd77e40d4744a33e), ROM_BIOS(4))
ROM_END

ROM_START(waveterm)
	ROM_REGION(0x1000, "maincpu", 0)
	ROM_LOAD("rom.bin", 0x0000, 0x1000, CRC(add3c20f) SHA1(4d47d99231bff2209634e6aac5710e782ee2f6da))
ROM_END

ROM_START(microtrol)
	ROM_REGION(0x1000, "maincpu", 0)
	ROM_LOAD("mon1.bin", 0x0000, 0x0800, CRC(4e82af0f) SHA1(a708f0c8a4d7ab216bc065e82a4ad42009cc3696)) // "microtrol Control V5.1"
	ROM_LOAD("mon2.bin", 0x0800, 0x0800, CRC(577a2b4c) SHA1(e7097a96417fa249a62c967039f039e637079cb6))
ROM_END


//    YEAR  NAME       PARENT    COMPAT  MACHINE    INPUT     CLASS           INIT        COMPANY      FULLNAME                     FLAGS
COMP( 1981, eurocom2,  0,        0,      eurocom2,  eurocom2, eurocom2_state, empty_init, "Eltec",     "Eurocom II V7",             MACHINE_IS_SKELETON )
COMP( 1982, waveterm,  eurocom2, 0,      waveterm,  waveterm, waveterm_state, empty_init, "PPG",       "Waveterm A",                MACHINE_IS_SKELETON )
COMP( 1985, microtrol, eurocom2, 0,      microtrol, eurocom2, eurocom2_state, empty_init, "Microtrol", "unknown Microtrol portable computer", MACHINE_IS_SKELETON )
