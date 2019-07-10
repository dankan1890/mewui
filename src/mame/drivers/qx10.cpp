// license:BSD-3-Clause
// copyright-holders:Mariusz Wojcieszek, Angelo Salese
/***************************************************************************

    QX-10

    Preliminary driver by Mariusz Wojcieszek

    Status:
    Driver boots and load CP/M from floppy image. Needs upd7220 for gfx

    Done:
    - preliminary memory map
    - floppy (upd765)
    - DMA
    - Interrupts (pic8295)

    banking:
    - 0x1c = 0
    map(0x0000,0x1fff).rom()
    map(0x2000,0xdfff).noprw()
    map(0xe000,0xffff).ram()
    - 0x1c = 1 0x20 = 1
    map(0x0000,0x7fff).ram() (0x18 selects bank)
    map(0x8000,0x87ff) CMOS
    map(0x8800,0xdfff).noprw() or previous bank?
    map(0xe000,0xffff).ram()
    - 0x1c = 1 0x20 = 0
    map(0x0000,0xdfff).ram() (0x18 selects bank)
    map(0xe000,0xffff).ram()
****************************************************************************/


#include "emu.h"

#include "bus/rs232/rs232.h"
#include "cpu/z80/z80.h"
#include "imagedev/floppy.h"
#include "machine/am9517a.h"
#include "machine/i8255.h"
#include "machine/mc146818.h"
#include "machine/pic8259.h"
#include "machine/pit8253.h"
#include "machine/qx10kbd.h"
#include "machine/ram.h"
#include "machine/upd765.h"
#include "machine/z80dart.h"
#include "video/upd7220.h"
#include "emupal.h"

#include "screen.h"
#include "softlist.h"
#include "imagedev/snapquik.h"


#define MAIN_CLK    15974400

#define RS232_TAG   "rs232"

/*
    Driver data
*/

class qx10_state : public driver_device
{
public:
	qx10_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_pit_1(*this, "pit8253_1"),
		m_pit_2(*this, "pit8253_2"),
		m_pic_m(*this, "pic8259_master"),
		m_pic_s(*this, "pic8259_slave"),
		m_scc(*this, "upd7201"),
		m_ppi(*this, "i8255"),
		m_dma_1(*this, "8237dma_1"),
		m_dma_2(*this, "8237dma_2"),
		m_fdc(*this, "upd765"),
		m_floppy(*this, "upd765:%u", 0U),
		m_hgdc(*this, "upd7220"),
		m_rtc(*this, "rtc"),
		m_kbd(*this, "kbd"),
		m_vram_bank(0),
		m_char_rom(*this, "chargen"),
		m_maincpu(*this, "maincpu"),
		m_screen(*this, "screen"),
		m_ram(*this, RAM_TAG),
		m_palette(*this, "palette")
	{
	}

	void qx10(machine_config &config);

private:
	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	virtual void machine_start() override;
	virtual void machine_reset() override;

	virtual void video_start() override;

	void update_memory_mapping();

	DECLARE_WRITE8_MEMBER( qx10_18_w );
	DECLARE_WRITE8_MEMBER( prom_sel_w );
	DECLARE_WRITE8_MEMBER( cmos_sel_w );
	DECLARE_WRITE_LINE_MEMBER( qx10_upd765_interrupt );
	DECLARE_READ8_MEMBER( fdc_dma_r );
	DECLARE_WRITE8_MEMBER( fdc_dma_w );
	DECLARE_WRITE8_MEMBER( fdd_motor_w );
	DECLARE_READ8_MEMBER( qx10_30_r );
	DECLARE_READ8_MEMBER( gdc_dack_r );
	DECLARE_WRITE8_MEMBER( gdc_dack_w );
	DECLARE_WRITE_LINE_MEMBER( tc_w );
	DECLARE_READ8_MEMBER( mc146818_r );
	DECLARE_WRITE8_MEMBER( mc146818_w );
	DECLARE_READ8_MEMBER( get_slave_ack );
	DECLARE_READ8_MEMBER( vram_bank_r );
	DECLARE_WRITE8_MEMBER( vram_bank_w );
	DECLARE_READ16_MEMBER( vram_r );
	DECLARE_WRITE16_MEMBER( vram_w );
	DECLARE_READ8_MEMBER(memory_read_byte);
	DECLARE_WRITE8_MEMBER(memory_write_byte);
	DECLARE_WRITE_LINE_MEMBER(keyboard_clk);
	DECLARE_WRITE_LINE_MEMBER(keyboard_irq);

	DECLARE_QUICKLOAD_LOAD_MEMBER(quickload_cb);

	void qx10_palette(palette_device &palette) const;
	DECLARE_WRITE_LINE_MEMBER(dma_hrq_changed);

	UPD7220_DISPLAY_PIXELS_MEMBER( hgdc_display_pixels );
	UPD7220_DRAW_TEXT_LINE_MEMBER( hgdc_draw_text );

	void qx10_io(address_map &map);
	void qx10_mem(address_map &map);
	void upd7220_map(address_map &map);

	required_device<pit8253_device> m_pit_1;
	required_device<pit8253_device> m_pit_2;
	required_device<pic8259_device> m_pic_m;
	required_device<pic8259_device> m_pic_s;
	required_device<upd7201_device> m_scc;
	required_device<i8255_device> m_ppi;
	required_device<am9517a_device> m_dma_1;
	required_device<am9517a_device> m_dma_2;
	required_device<upd765a_device> m_fdc;
	required_device_array<floppy_connector, 2> m_floppy;
	required_device<upd7220_device> m_hgdc;
	required_device<mc146818_device> m_rtc;
	required_device<rs232_port_device> m_kbd;
	uint8_t m_vram_bank;
	//required_shared_ptr<uint8_t> m_video_ram;
	std::unique_ptr<uint16_t[]> m_video_ram;
	required_region_ptr<uint8_t> m_char_rom;

	required_device<cpu_device> m_maincpu;
	required_device<screen_device> m_screen;
	required_device<ram_device> m_ram;
	required_device<palette_device> m_palette;

	/* FDD */
	int     m_fdcint;
	int     m_fdcmotor;
	//int     m_fdcready;

	/* memory */
	int     m_membank;
	int     m_memprom;
	int     m_memcmos;
	uint8_t   m_cmosram[0x800];

	uint8_t m_color_mode;
};

UPD7220_DISPLAY_PIXELS_MEMBER( qx10_state::hgdc_display_pixels )
{
	const rgb_t *palette = m_palette->palette()->entry_list_raw();
	int xi,gfx[3];
	uint8_t pen;

	if(m_color_mode)
	{
		gfx[0] = m_video_ram[((address) + 0x00000) >> 1];
		gfx[1] = m_video_ram[((address) + 0x20000) >> 1];
		gfx[2] = m_video_ram[((address) + 0x40000) >> 1];
	}
	else
	{
		gfx[0] = m_video_ram[(address) >> 1];
		gfx[1] = 0;
		gfx[2] = 0;
	}

	for(xi=0;xi<16;xi++)
	{
		pen = ((gfx[0] >> xi) & 1) ? 1 : 0;
		pen|= ((gfx[1] >> xi) & 1) ? 2 : 0;
		pen|= ((gfx[2] >> xi) & 1) ? 4 : 0;

		bitmap.pix32(y, x + xi) = palette[pen];
	}
}

UPD7220_DRAW_TEXT_LINE_MEMBER( qx10_state::hgdc_draw_text )
{
	const rgb_t *palette = m_palette->palette()->entry_list_raw();

	for (int x = 0; x < pitch; x++)
	{
		int tile = m_video_ram[((addr+x)*2) >> 1] & 0xff;
		int attr = m_video_ram[((addr+x)*2) >> 1] >> 8;

		uint8_t color = (m_color_mode) ? 1 : (attr & 4) ? 2 : 1; /* TODO: color mode */

		for (int yi = 0; yi < lr; yi++)
		{
			uint8_t tile_data = (m_char_rom[tile*16+yi]);

			if(attr & 8)
				tile_data^=0xff;

			if(cursor_on && cursor_addr == addr+x) //TODO
				tile_data^=0xff;

			if(attr & 0x80 && m_screen->frame_number() & 0x10) //TODO: check for blinking interval
				tile_data=0;

			for (int xi = 0; xi < 8; xi++)
			{
				int res_x = x * 8 + xi;
				int res_y = y + yi;

				if(!m_screen->visible_area().contains(res_x, res_y))
					continue;

				uint8_t pen;
				if(yi >= 16)
					pen = 0;
				else
					pen = ((tile_data >> xi) & 1) ? color : 0;

				if(pen)
					bitmap.pix32(res_y, res_x) = palette[pen];
			}
		}
	}
}

uint32_t qx10_state::screen_update( screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect )
{
	bitmap.fill(m_palette->black_pen(), cliprect);

	m_hgdc->screen_update(screen, bitmap, cliprect);

	return 0;
}

/*
    Memory
*/
void qx10_state::update_memory_mapping()
{
	int drambank = 0;

	if (m_membank & 1)
	{
		drambank = 0;
	}
	else if (m_membank & 2)
	{
		drambank = 1;
	}
	else if (m_membank & 4)
	{
		drambank = 2;
	}
	else if (m_membank & 8)
	{
		drambank = 3;
	}

	if (!m_memprom)
	{
		membank("bank1")->set_base(memregion("maincpu")->base());
	}
	else
	{
		membank("bank1")->set_base(m_ram->pointer() + drambank*64*1024);
	}
	if (m_memcmos)
	{
		membank("bank2")->set_base(m_cmosram);
	}
	else
	{
		membank("bank2")->set_base(m_ram->pointer() + drambank*64*1024 + 32*1024);
	}
}

READ8_MEMBER( qx10_state::fdc_dma_r )
{
	return m_fdc->dma_r();
}

WRITE8_MEMBER( qx10_state::fdc_dma_w )
{
	m_fdc->dma_w(data);
}


WRITE8_MEMBER( qx10_state::qx10_18_w )
{
	m_membank = (data >> 4) & 0x0f;
	update_memory_mapping();
}

WRITE8_MEMBER( qx10_state::prom_sel_w )
{
	m_memprom = data & 1;
	update_memory_mapping();
}

WRITE8_MEMBER( qx10_state::cmos_sel_w )
{
	m_memcmos = data & 1;
	update_memory_mapping();
}

/***********************************************************

    Quickload

    This loads a .COM file to address 0x100 then jumps
    there. Sometimes .COM has been renamed to .CPM to
    prevent windows going ballistic. These can be loaded
    as well.

************************************************************/

QUICKLOAD_LOAD_MEMBER(qx10_state::quickload_cb)
{
	address_space& prog_space = m_maincpu->space(AS_PROGRAM);

	if (quickload_size >= 0xfd00)
		return image_init_result::FAIL;

	/* The right RAM bank must be active */
	m_membank = 0;
	update_memory_mapping();

	/* Avoid loading a program if CP/M-80 is not in memory */
	if ((prog_space.read_byte(0) != 0xc3) || (prog_space.read_byte(5) != 0xc3))
	{
		machine_reset();
		return image_init_result::FAIL;
	}

	/* Load image to the TPA (Transient Program Area) */
	for (uint16_t i = 0; i < quickload_size; i++)
	{
		uint8_t data;
		if (image.fread( &data, 1) != 1)
			return image_init_result::FAIL;
		prog_space.write_byte(i+0x100, data);
	}

	/* clear out command tail */
	prog_space.write_byte(0x80, 0);   prog_space.write_byte(0x81, 0);

	/* Roughly set SP basing on the BDOS position */
	m_maincpu->set_state_int(Z80_SP, 256 * prog_space.read_byte(7) - 300);
	m_maincpu->set_pc(0x100);       // start program

	return image_init_result::PASS;
}

/*
    FDD
*/

static void qx10_floppies(device_slot_interface &device)
{
	device.option_add("525dd", FLOPPY_525_DD);
}

WRITE_LINE_MEMBER( qx10_state::qx10_upd765_interrupt )
{
	m_fdcint = state;

	//logerror("Interrupt from upd765: %d\n", state);
	// signal interrupt
	m_pic_m->ir6_w(state);
}

WRITE8_MEMBER( qx10_state::fdd_motor_w )
{
	m_fdcmotor = 1;

	m_floppy[0]->get_device()->mon_w(false);
	// motor off controlled by clock
}

READ8_MEMBER( qx10_state::qx10_30_r )
{
	floppy_image_device *floppy1,*floppy2;

	floppy1 = m_floppy[0]->get_device();
	floppy2 = m_floppy[1]->get_device();

	return m_fdcint |
			/*m_fdcmotor*/ 0 << 1 |
			((floppy1 != nullptr) || (floppy2 != nullptr) ? 1 : 0) << 3 |
			m_membank << 4;
}

/*
    DMA8237
*/
WRITE_LINE_MEMBER(qx10_state::dma_hrq_changed)
{
	/* Assert HLDA */
	m_dma_1->hack_w(state);
}

READ8_MEMBER( qx10_state::gdc_dack_r )
{
	logerror("GDC DACK read\n");
	return 0;
}

WRITE8_MEMBER( qx10_state::gdc_dack_w )
{
	logerror("GDC DACK write %02x\n", data);
}

WRITE_LINE_MEMBER( qx10_state::tc_w )
{
	/* floppy terminal count */
	m_fdc->tc_w(!state);
}

/*
    8237 DMA (Master)
    Channel 1: Floppy disk
    Channel 2: GDC
    Channel 3: Option slots
*/
READ8_MEMBER(qx10_state::memory_read_byte)
{
	address_space& prog_space = m_maincpu->space(AS_PROGRAM);
	return prog_space.read_byte(offset);
}

WRITE8_MEMBER(qx10_state::memory_write_byte)
{
	address_space& prog_space = m_maincpu->space(AS_PROGRAM);
	return prog_space.write_byte(offset, data);
}

/*
    8237 DMA (Slave)
    Channel 1: Option slots #1
    Channel 2: Option slots #2
    Channel 3: Option slots #3
    Channel 4: Option slots #4
*/

/*
    MC146818
*/

WRITE8_MEMBER(qx10_state::mc146818_w)
{
	m_rtc->write(!offset, data);
}

READ8_MEMBER(qx10_state::mc146818_r)
{
	return m_rtc->read(!offset);
}

WRITE_LINE_MEMBER(qx10_state::keyboard_irq)
{
	m_scc->m1_r(); // always set
	m_pic_m->ir4_w(state);
}

WRITE_LINE_MEMBER(qx10_state::keyboard_clk)
{
	// clock keyboard too
	m_scc->rxca_w(state);
	m_scc->txca_w(state);
}

/*
    Master PIC8259
    IR0     Power down detection interrupt
    IR1     Software timer #1 interrupt
    IR2     External interrupt INTF1
    IR3     External interrupt INTF2
    IR4     Keyboard/RS232 interrupt
    IR5     CRT/lightpen interrupt
    IR6     Floppy controller interrupt
    IR7     Slave cascade
*/

READ8_MEMBER( qx10_state::get_slave_ack )
{
	if (offset==7) { // IRQ = 7
		return m_pic_s->acknowledge();
	}
	return 0x00;
}


/*
    Slave PIC8259
    IR0     Printer interrupt
    IR1     External interrupt #1
    IR2     Calendar clock interrupt
    IR3     External interrupt #2
    IR4     External interrupt #3
    IR5     Software timer #2 interrupt
    IR6     External interrupt #4
    IR7     External interrupt #5

*/

READ8_MEMBER( qx10_state::vram_bank_r )
{
	return m_vram_bank;
}

WRITE8_MEMBER( qx10_state::vram_bank_w )
{
	if(m_color_mode)
	{
		m_vram_bank = data & 7;
		if(data != 1 && data != 2 && data != 4)
			printf("%02x\n",data);
	}
}

void qx10_state::qx10_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x7fff).bankrw("bank1");
	map(0x8000, 0xdfff).bankrw("bank2");
	map(0xe000, 0xffff).ram();
}

void qx10_state::qx10_io(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x03).rw(m_pit_1, FUNC(pit8253_device::read), FUNC(pit8253_device::write));
	map(0x04, 0x07).rw(m_pit_2, FUNC(pit8253_device::read), FUNC(pit8253_device::write));
	map(0x08, 0x09).rw(m_pic_m, FUNC(pic8259_device::read), FUNC(pic8259_device::write));
	map(0x0c, 0x0d).rw(m_pic_s, FUNC(pic8259_device::read), FUNC(pic8259_device::write));
	map(0x10, 0x13).rw(m_scc, FUNC(z80dart_device::cd_ba_r), FUNC(z80dart_device::cd_ba_w));
	map(0x14, 0x17).rw(m_ppi, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x18, 0x1b).portr("DSW").w(FUNC(qx10_state::qx10_18_w));
	map(0x1c, 0x1f).w(FUNC(qx10_state::prom_sel_w));
	map(0x20, 0x23).w(FUNC(qx10_state::cmos_sel_w));
	map(0x2c, 0x2c).portr("CONFIG");
	map(0x2d, 0x2d).rw(FUNC(qx10_state::vram_bank_r), FUNC(qx10_state::vram_bank_w));
	map(0x30, 0x33).rw(FUNC(qx10_state::qx10_30_r), FUNC(qx10_state::fdd_motor_w));
	map(0x34, 0x35).m(m_fdc, FUNC(upd765a_device::map));
	map(0x38, 0x39).rw(m_hgdc, FUNC(upd7220_device::read), FUNC(upd7220_device::write));
//  map(0x3a, 0x3a) GDC zoom
//  map(0x3b, 0x3b) GDC light pen req
	map(0x3c, 0x3d).rw(FUNC(qx10_state::mc146818_r), FUNC(qx10_state::mc146818_w));
	map(0x40, 0x4f).rw(m_dma_1, FUNC(am9517a_device::read), FUNC(am9517a_device::write));
	map(0x50, 0x5f).rw(m_dma_2, FUNC(am9517a_device::read), FUNC(am9517a_device::write));
//  map(0xfc, 0xfd) Multi-Font comms
}

/* Input ports */
/* TODO: shift break */
/*INPUT_CHANGED_MEMBER(qx10_state::key_stroke)
{
    if(newval && !oldval)
    {
        m_keyb.rx = (uint8_t)(uintptr_t)(param) & 0x7f;
        m_pic_m->ir4_w(1);
    }

    if(oldval && !newval)
        m_keyb.rx = 0;
}*/

static INPUT_PORTS_START( qx10 )
	/* TODO: All of those have unknown meaning */
	PORT_START("DSW")
	PORT_DIPNAME( 0x01, 0x00, "DSW" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unknown ) ) //CMOS related
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("CONFIG")
	PORT_CONFNAME( 0x03, 0x02, "Video Board" )
	PORT_CONFSETTING( 0x02, "Monochrome" )
	PORT_CONFSETTING( 0x01, "Color" )
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END


void qx10_state::machine_start()
{
}

void qx10_state::machine_reset()
{
	m_dma_1->dreq0_w(1);

	m_memprom = 0;
	m_memcmos = 0;
	m_membank = 0;
	update_memory_mapping();

	{
		int i;

		/* TODO: is there a bit that sets this up? */
		m_color_mode = ioport("CONFIG")->read() & 1;

		if(m_color_mode) //color
		{
			for ( i = 0; i < 8; i++ )
				m_palette->set_pen_color(i, pal1bit((i >> 2) & 1), pal1bit((i >> 1) & 1), pal1bit((i >> 0) & 1));
		}
		else //monochrome
		{
			for ( i = 0; i < 8; i++ )
				m_palette->set_pen_color(i, pal1bit(0), pal1bit(0), pal1bit(0));

			m_palette->set_pen_color(1, 0x00, 0x9f, 0x00);
			m_palette->set_pen_color(2, 0x00, 0xff, 0x00);
			m_vram_bank = 0;
		}
	}
}

/* F4 Character Displayer */
static const gfx_layout qx10_charlayout =
{
	8, 16,                  /* 8 x 16 characters */
	RGN_FRAC(1,1),          /* 128 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 7, 6, 5, 4, 3, 2, 1, 0 },
	/* y offsets */
	{  0*8,  1*8,  2*8,  3*8,  4*8,  5*8,  6*8,  7*8, 8*8,  9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	8*16                    /* every char takes 16 bytes */
};

static GFXDECODE_START( gfx_qx10 )
	GFXDECODE_ENTRY( "chargen", 0x0000, qx10_charlayout, 1, 1 )
GFXDECODE_END

void qx10_state::video_start()
{
	// allocate memory
	m_video_ram = make_unique_clear<uint16_t[]>(0x30000);
}

void qx10_state::qx10_palette(palette_device &palette) const
{
	// ...
}

READ16_MEMBER( qx10_state::vram_r )
{
	int bank = 0;

	if (m_vram_bank & 1)     { bank = 0; } // B
	else if(m_vram_bank & 2) { bank = 1; } // G
	else if(m_vram_bank & 4) { bank = 2; } // R

	return m_video_ram[offset + (0x20000 * bank)];
}

WRITE16_MEMBER( qx10_state::vram_w )
{
	int bank = 0;

	if (m_vram_bank & 1)     { bank = 0; } // B
	else if(m_vram_bank & 2) { bank = 1; } // G
	else if(m_vram_bank & 4) { bank = 2; } // R

	COMBINE_DATA(&m_video_ram[offset + (0x20000 * bank)]);
}

void qx10_state::upd7220_map(address_map &map)
{
	map(0x00000, 0x3ffff).rw(FUNC(qx10_state::vram_r), FUNC(qx10_state::vram_w));
}

static void keyboard(device_slot_interface &device)
{
	device.option_add("qx10", QX10_KEYBOARD);
}

void qx10_state::qx10(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, MAIN_CLK / 4);
	m_maincpu->set_addrmap(AS_PROGRAM, &qx10_state::qx10_mem);
	m_maincpu->set_addrmap(AS_IO, &qx10_state::qx10_io);
	m_maincpu->set_irq_acknowledge_callback("pic8259_master", FUNC(pic8259_device::inta_cb));

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(50);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(2500)); // not accurate
	m_screen->set_screen_update(FUNC(qx10_state::screen_update));
	m_screen->set_size(640, 480);
	m_screen->set_visarea(0, 640-1, 0, 480-1);
	GFXDECODE(config, "gfxdecode", m_palette, gfx_qx10);
	PALETTE(config, m_palette, FUNC(qx10_state::qx10_palette), 8);

	/* Devices */

/*
    Timer 0
    Counter CLK                         Gate                    OUT             Operation
    0       Keyboard clock (1200bps)    Memory register D0      Speaker timer   Speaker timer (100ms)
    1       Keyboard clock (1200bps)    +5V                     8259A (10E) IR5 Software timer
    2       Clock 1,9668MHz             Memory register D7      8259 (12E) IR1  Software timer
*/
	PIT8253(config, m_pit_1, 0);
	m_pit_1->set_clk<0>(1200);
	m_pit_1->set_clk<1>(1200);
	m_pit_1->set_clk<2>(MAIN_CLK / 8);

/*
    Timer 1
    Counter CLK                 Gate        OUT                 Operation
    0       Clock 1,9668MHz     +5V         Speaker frequency   1kHz
    1       Clock 1,9668MHz     +5V         Keyboard clock      1200bps (Clock / 1664)
    2       Clock 1,9668MHz     +5V         RS-232C baud rate   9600bps (Clock / 208)
*/
	PIT8253(config, m_pit_2, 0);
	m_pit_2->set_clk<0>(MAIN_CLK / 8);
	m_pit_2->set_clk<1>(MAIN_CLK / 8);
	m_pit_2->out_handler<1>().set(FUNC(qx10_state::keyboard_clk));
	m_pit_2->set_clk<2>(MAIN_CLK / 8);
	m_pit_2->out_handler<2>().set(m_scc, FUNC(z80dart_device::rxtxcb_w));

	PIC8259(config, m_pic_m, 0);
	m_pic_m->out_int_callback().set_inputline(m_maincpu, 0);
	m_pic_m->in_sp_callback().set_constant(1);
	m_pic_m->read_slave_ack_callback().set(FUNC(qx10_state::get_slave_ack));

	PIC8259(config, m_pic_s, 0);
	m_pic_s->out_int_callback().set(m_pic_m, FUNC(pic8259_device::ir7_w));
	m_pic_s->in_sp_callback().set_constant(0);

	UPD7201(config, m_scc, MAIN_CLK/4); // channel b clock set by pit2 channel 2
	// Channel A: Keyboard
	m_scc->out_txda_callback().set(m_kbd, FUNC(rs232_port_device::write_txd));
	// Channel B: RS232
	m_scc->out_txdb_callback().set(RS232_TAG, FUNC(rs232_port_device::write_txd));
	m_scc->out_dtrb_callback().set(RS232_TAG, FUNC(rs232_port_device::write_dtr));
	m_scc->out_rtsb_callback().set(RS232_TAG, FUNC(rs232_port_device::write_rts));
	m_scc->out_int_callback().set(FUNC(qx10_state::keyboard_irq));

	AM9517A(config, m_dma_1, MAIN_CLK/4);
	m_dma_1->out_hreq_callback().set(FUNC(qx10_state::dma_hrq_changed));
	m_dma_1->out_eop_callback().set(FUNC(qx10_state::tc_w));
	m_dma_1->in_memr_callback().set(FUNC(qx10_state::memory_read_byte));
	m_dma_1->out_memw_callback().set(FUNC(qx10_state::memory_write_byte));
	m_dma_1->in_ior_callback<0>().set(FUNC(qx10_state::fdc_dma_r));
	m_dma_1->in_ior_callback<1>().set(FUNC(qx10_state::gdc_dack_r));
	//m_dma_1->in_ior_callback<2>().set(m_hgdc, FUNC(upd7220_device::dack_r));
	m_dma_1->out_iow_callback<0>().set(FUNC(qx10_state::fdc_dma_w));
	m_dma_1->out_iow_callback<1>().set(FUNC(qx10_state::gdc_dack_w));
	//m_dma_1->out_iow_callback<2>().set(m_hgdc, FUNC(upd7220_device::dack_w));
	AM9517A(config, m_dma_2, MAIN_CLK/4);

	I8255(config, m_ppi, 0);

	UPD7220(config, m_hgdc, MAIN_CLK/6); // unk clock
	m_hgdc->set_addrmap(0, &qx10_state::upd7220_map);
	m_hgdc->set_display_pixels(FUNC(qx10_state::hgdc_display_pixels));
	m_hgdc->set_draw_text(FUNC(qx10_state::hgdc_draw_text));
	m_hgdc->set_screen("screen");

	MC146818(config, m_rtc, 32.768_kHz_XTAL);
	m_rtc->irq().set(m_pic_s, FUNC(pic8259_device::ir2_w));

	UPD765A(config, m_fdc, 8'000'000, true, true);
	m_fdc->intrq_wr_callback().set(FUNC(qx10_state::qx10_upd765_interrupt));
	m_fdc->drq_wr_callback().set(m_dma_1, FUNC(am9517a_device::dreq0_w)).invert();
	FLOPPY_CONNECTOR(config, m_floppy[0], qx10_floppies, "525dd", floppy_image_device::default_floppy_formats);
	FLOPPY_CONNECTOR(config, m_floppy[1], qx10_floppies, "525dd", floppy_image_device::default_floppy_formats);

	rs232_port_device &rs232(RS232_PORT(config, RS232_TAG, default_rs232_devices, nullptr));
	rs232.rxd_handler().set(m_scc, FUNC(upd7201_device::rxb_w));

	RS232_PORT(config, m_kbd, keyboard, "qx10");
	m_kbd->rxd_handler().set(m_scc, FUNC(upd7201_device::rxa_w));

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("256K");

	// software lists
	SOFTWARE_LIST(config, "flop_list").set_original("qx10_flop");

	QUICKLOAD(config, "quickload", "com,cpm", attotime::from_seconds(3)).set_load_callback(FUNC(qx10_state::quickload_cb), this);
}

/* ROM definition */
ROM_START( qx10 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_SYSTEM_BIOS(0, "v006", "v0.06")
	ROMX_LOAD( "ipl006.bin", 0x0000, 0x0800, CRC(3155056a) SHA1(67cc0ae5055d472aa42eb40cddff6da69ffc6553), ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "v003", "v0.03")
	ROMX_LOAD( "ipl003.bin", 0x0000, 0x0800, CRC(3cbc4008) SHA1(cc8c7d1aa0cca8f9753d40698b2dc6802fd5f890), ROM_BIOS(1))

	/* This is probably the i8039 program ROM for the Q10MF Multifont card, and the actual font ROMs are missing (6 * HM43128) */
	/* The first part of this rom looks like code for an embedded controller?
	    From 0300 on, is a keyboard lookup table */
	ROM_REGION( 0x0800, "i8039", 0 )
	ROM_LOAD( "m12020a.3e", 0x0000, 0x0800, CRC(fa27f333) SHA1(73d27084ca7b002d5f370220d8da6623a6e82132))

	ROM_REGION( 0x1000, "chargen", 0 )
//  ROM_LOAD( "qge.2e",   0x0000, 0x0800, BAD_DUMP CRC(ed93cb81) SHA1(579e68bde3f4184ded7d89b72c6936824f48d10b))  //this one contains special characters only
	ROM_LOAD( "qge.2e",   0x0000, 0x1000, BAD_DUMP CRC(eb31a2d5) SHA1(6dc581bf2854a07ae93b23b6dfc9c7abd3c0569e))
ROM_END

/* Driver */

/*    YEAR  NAME  PARENT  COMPAT  MACHINE  INPUT  CLASS       INIT        COMPANY  FULLNAME  FLAGS */
COMP( 1983, qx10, 0,      0,      qx10,    qx10,  qx10_state, empty_init, "Epson", "QX-10",  MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
