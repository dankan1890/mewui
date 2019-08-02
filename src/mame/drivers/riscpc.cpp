// license:BSD-3-Clause
// copyright-holders:Angelo Salese
/***************************************************************************

    Acorn Archimedes 7000/7000+

    very preliminary driver by Angelo Salese,
    based on work by Tomasz Slanina and Tom Walker

    TODO:
	- Move device implementations into specific files;
	- PS/2 keyboard doesn't work properly;
	- i2c device should actually be a pcf8583 RTC with i2c as slave 
	  (and indeed CMOS settings doesn't work);
	- Fix pendingUnd fatalerror from ARM7 core
	- Fix per-machine configurations;
	
    ???
    bp (0382827C) (second trigger)
    do R13 = SR13

****************************************************************************/

#include "emu.h"
#include "cpu/arm7/arm7.h"
#include "cpu/arm7/arm7core.h"
#include "machine/i2cmem.h"
#include "machine/at_keybc.h"
#include "bus/pc_kbd/pc_kbdc.h"
#include "bus/pc_kbd/keyboards.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"
#include "debugger.h"

class riscpc_state : public driver_device
{
public:
	riscpc_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),
		m_i2cmem(*this, "i2cmem"),
		m_kbdc(*this, "kbdc")
	{ }

	void rpc700(machine_config &config);
	void rpc600(machine_config &config);
	void sarpc(machine_config &config);
	void sarpc_j233(machine_config &config);
	void a7000(machine_config &config);
	void a7000p(machine_config &config);

private:
	void base_config(machine_config &config);

	required_device<cpu_device> m_maincpu;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device<i2cmem_device> m_i2cmem;
	required_device<ps2_keyboard_controller_device> m_kbdc;

	std::unique_ptr<uint32_t[]> m_vram;

	DECLARE_READ32_MEMBER(a7000_iomd_r);
	DECLARE_WRITE32_MEMBER(a7000_iomd_w);
	DECLARE_WRITE32_MEMBER(a7000_vidc20_w);
	DECLARE_WRITE_LINE_MEMBER(keyboard_reset);
	DECLARE_WRITE_LINE_MEMBER(keyboard_interrupt);

	uint8_t m_vidc20_pal_index;
	uint16_t m_vidc20_horz_reg[0x10];
	uint16_t m_vidc20_vert_reg[0x10];
	uint8_t m_vidc20_bpp_mode;
	emu_timer *m_flyback_timer;
	uint16_t m_timer_in[2];
	uint16_t m_timer_out[2];
	int m_timer_counter[2];
	emu_timer *m_IOMD_timer[2];
	uint8_t m_IRQ_status_A, m_IRQ_status_B;
	uint8_t m_IRQ_mask_A, m_IRQ_mask_B;
	uint8_t m_IOMD_IO_ctrl;
	uint8_t m_IOMD_keyb_ctrl;
	uint16_t m_io_id;
	uint8_t m_viddma_status;
	uint32_t m_viddma_addr_init;
	uint32_t m_viddma_addr_start;
	uint32_t m_viddma_addr_end;
	uint8_t m_t0readinc;
	uint8_t m_t1readinc;
	uint8_t m_i2c_clk;
	void fire_iomd_timer(int timer);
	void viddma_transfer_start();
	void vidc20_dynamic_screen_change();
	virtual void video_start() override;
	virtual void machine_reset() override;
	virtual void machine_start() override;

	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(IOMD_timer0_callback);
	TIMER_CALLBACK_MEMBER(IOMD_timer1_callback);
	TIMER_CALLBACK_MEMBER(flyback_timer_callback);
	TIMER_CALLBACK_MEMBER(videodma_timer_callback);

	void a7000_map(address_map &map);
	void riscpc_map(address_map &map);
	void update_irq();
};


/*
 *
 * VIDC20 chip emulation
 *
 */

void riscpc_state::video_start()
{
	const uint32_t vram_size = 0x1000000/4;
	m_vram = std::make_unique<uint32_t[]>(vram_size);
	save_pointer(NAME(m_vram), vram_size);
}


static const char *const vidc20_regnames[] =
{
	"Video Palette",                    // 0
	"Video Palette Address",            // 1
	"RESERVED",                         // 2
	"LCD offset",                       // 3
	"Border Colour",                    // 4
	"Cursor Palette Logical Colour 1",  // 5
	"Cursor Palette Logical Colour 2",  // 6
	"Cursor Palette Logical Colour 3",  // 7
	"Horizontal",                       // 8
	"Vertical",                         // 9
	"Stereo Image",                     // A
	"Sound",                            // B
	"External",                         // C
	"Frequency Synthesis",              // D
	"Control",                          // E
	"Data Control"                      // F
};

#if 0
static const char *const vidc20_horz_regnames[] =
{
	"Horizontal Cycle",                 // 0x80 HCR
	"Horizontal Sync Width",            // 0x81 HSWR
	"Horizontal Border Start",          // 0x82 HBSR
	"Horizontal Display Start",         // 0x83 HDSR
	"Horizontal Display End",           // 0x84 HDER
	"Horizontal Border End",            // 0x85 HBER
	"Horizontal Cursor Start",          // 0x86 HCSR
	"Horizontal Interlace",             // 0x87 HIR
	"Horizontal Counter TEST",          // 0x88
	"Horizontal <UNDEFINED>",           // 0x89
	"Horizontal <UNDEFINED>",           // 0x8a
	"Horizontal <UNDEFINED>",           // 0x8b
	"Horizontal All TEST",              // 0x8c
	"Horizontal <UNDEFINED>",           // 0x8d
	"Horizontal <UNDEFINED>",           // 0x8e
	"Horizontal <UNDEFINED>"            // 0x8f
};
#endif

#define HCR  0
#define HSWR 1
#define HBSR 2
#define HDSR 3
#define HDER 4
#define HBER 5
#define HCSR 6
#define HIR  7

#if 0
static const char *const vidc20_vert_regnames[] =
{
	"Vertical Cycle",                   // 0x90 VCR
	"Vertical Sync Width",              // 0x91 VSWR
	"Vertical Border Start",            // 0x92 VBSR
	"Vertical Display Start",           // 0x93 VDSR
	"Vertical Display End",             // 0x94 VDER
	"Vertical Border End",              // 0x95 VBER
	"Vertical Cursor Start",            // 0x96 VCSR
	"Vertical Cursor End",              // 0x97 VCER
	"Vertical Counter TEST",            // 0x98
	"Horizontal <UNDEFINED>",           // 0x99
	"Vertical Counter Increment TEST",  // 0x9a
	"Horizontal <UNDEFINED>",           // 0x9b
	"Vertical All TEST",                // 0x9c
	"Horizontal <UNDEFINED>",           // 0x9d
	"Horizontal <UNDEFINED>",           // 0x9e
	"Horizontal <UNDEFINED>"            // 0x9f
};
#endif

#define VCR  0
#define VSWR 1
#define VBSR 2
#define VDSR 3
#define VDER 4
#define VBER 5
#define VCSR 6
#define VCER 7

void riscpc_state::vidc20_dynamic_screen_change()
{
	/* sanity checks - first pass */
	/*
	total cycles + border start/end
	*/
	if(m_vidc20_horz_reg[HCR] && m_vidc20_horz_reg[HBSR] && m_vidc20_horz_reg[HBER] &&
		m_vidc20_vert_reg[VCR] && m_vidc20_vert_reg[VBSR] && m_vidc20_vert_reg[VBER])
	{
		/* sanity checks - second pass */
		/*
		total cycles > border end > border start
		*/
		if((m_vidc20_horz_reg[HCR] > m_vidc20_horz_reg[HBER]) &&
			(m_vidc20_horz_reg[HBER] > m_vidc20_horz_reg[HBSR]) &&
			(m_vidc20_vert_reg[VCR] > m_vidc20_vert_reg[VBER]) &&
			(m_vidc20_vert_reg[VBER] > m_vidc20_vert_reg[VBSR]))
		{
			/* finally ready to change the resolution */
			int hblank_period,vblank_period;
			rectangle visarea = m_screen->visible_area();
			hblank_period = (m_vidc20_horz_reg[HCR] & 0x3ffc);
			vblank_period = (m_vidc20_vert_reg[VCR] & 0x3fff);
			/* note that we use the border registers as the visible area */
			visarea.set(
					(m_vidc20_horz_reg[HBSR] & 0x3ffe),
					(m_vidc20_horz_reg[HBER] & 0x3ffe) - 1,
					(m_vidc20_vert_reg[VBSR] & 0x1fff),
					(m_vidc20_vert_reg[VBER] & 0x1fff) - 1);

			m_screen->configure(hblank_period, vblank_period, visarea, m_screen->frame_period().attoseconds() );
			logerror("VIDC20: successfully changed the screen to:\n Display Size = %d x %d\n Border Size %d x %d\n Cycle Period %d x %d\n",
						(m_vidc20_horz_reg[HDER]-m_vidc20_horz_reg[HDSR]),(m_vidc20_vert_reg[VDER]-m_vidc20_vert_reg[VDSR]),
						(m_vidc20_horz_reg[HBER]-m_vidc20_horz_reg[HBSR]),(m_vidc20_vert_reg[VBER]-m_vidc20_vert_reg[VBSR]),
						hblank_period,vblank_period);
		}
	}
}

WRITE32_MEMBER( riscpc_state::a7000_vidc20_w )
{
	int r,g,b,cursor_index,horz_reg,vert_reg,reg = data >> 28;

	switch(reg)
	{
		case 0: // Video Palette
			r = (data & 0x0000ff) >> 0;
			g = (data & 0x00ff00) >> 8;
			b = (data & 0xff0000) >> 16;

			m_palette->set_pen_color(m_vidc20_pal_index & 0xff,r,g,b);

			/* auto-increment & wrap-around */
			m_vidc20_pal_index++;
			m_vidc20_pal_index &= 0xff;
			break;

		case 1: // Video Palette Address

			// according to RPCEmu, these mustn't be set
			if (data & 0x0fffff00)
				return;

			m_vidc20_pal_index = data & 0xff;
			break;

		case 4: // Border Color
			r = (data & 0x0000ff) >> 0;
			g = (data & 0x00ff00) >> 8;
			b = (data & 0xff0000) >> 16;

			m_palette->set_pen_color(0x100,r,g,b);
			break;
		case 5: // Cursor Palette Logical Colour n
		case 6:
		case 7:
			cursor_index = 0x100 + reg - 4; // 0x101,0x102 and 0x103 (plus 0x100 of above, 2bpp)

			r = (data & 0x0000ff) >> 0;
			g = (data & 0x00ff00) >> 8;
			b = (data & 0xff0000) >> 16;

			m_palette->set_pen_color(cursor_index,r,g,b);
			break;
		case 8: // Horizontal
			horz_reg = (data >> 24) & 0xf;
			m_vidc20_horz_reg[horz_reg] = data & 0x3fff;
			if(horz_reg == 0 || horz_reg == 2 || horz_reg == 5)
				vidc20_dynamic_screen_change();

			// logerror("VIDC20: %s Register write = %08x (%d)\n",vidc20_horz_regnames[horz_reg],val,val);
			break;
		case 9: // Vertical
			vert_reg = (data >> 24) & 0xf;
			m_vidc20_vert_reg[vert_reg] = data & 0x1fff;
			if(vert_reg == 0 || vert_reg == 2 || vert_reg == 5)
				vidc20_dynamic_screen_change();

			if(vert_reg == 4)
			{
				if(m_vidc20_vert_reg[VDER] != 0)
					m_flyback_timer->adjust(m_screen->time_until_pos(m_vidc20_vert_reg[VDER]));
				else
					m_flyback_timer->adjust(attotime::never);
			}

			// logerror("VIDC20: %s Register write = %08x (%d)\n",vidc20_vert_regnames[vert_reg],val,val);

			break;
		case 0x0e: // Control
			m_vidc20_bpp_mode = (data & 0xe0) >> 5;
			break;
		default: logerror("VIDC20: %s Register write = %08x\n",vidc20_regnames[reg],data & 0xfffffff);
	}
}

uint32_t riscpc_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	int x_size,y_size,x_start,y_start;
	int x,y,xi;
	uint32_t count;

	bitmap.fill(m_palette->pen(0x100), cliprect);

	x_size = (m_vidc20_horz_reg[HDER]-m_vidc20_horz_reg[HDSR]);
	y_size = (m_vidc20_vert_reg[VDER]-m_vidc20_vert_reg[VDSR]);
	x_start = m_vidc20_horz_reg[HDSR];
	y_start = m_vidc20_vert_reg[VDSR];

	/* check if display is enabled */
	if(x_size <= 0 || y_size <= 0)
		return 0;

//  popmessage("%d",m_vidc20_bpp_mode);

	count = 0;

	switch(m_vidc20_bpp_mode)
	{
		case 0: /* 1 bpp */
		{
			for(y=0;y<y_size;y++)
			{
				for(x=0;x<x_size;x+=32)
				{
					for(xi=0;xi<32;xi++)
						bitmap.pix32(y+y_start, x+xi+x_start) = m_palette->pen((m_vram[count]>>(xi))&1);

					count++;
				}
			}
			break;
		}
		case 2: /* 4 bpp */
		{
			for(y=0;y<y_size;y++)
			{
				for(x=0;x<x_size;x+=8)
				{
					for(xi=0;xi<8;xi++)
						bitmap.pix32(y+y_start, x+xi+x_start) = m_palette->pen((m_vram[count]>>(xi*4))&0xf);

					count++;
				}
			}
			break;
		}
		#if 0
		case 1: /* 2 bpp */
		{
			for(y=0;y<y_size;y++)
			{
				for(x=0;x<x_size;x+=4)
				{
					for(xi=0;xi<4;xi++)
						bitmap.pix32(y+y_start, x+xi+x_start) = m_palette->pen((vram[count]>>(xi*2))&3);

					count++;
				}
			}
		}
		break;

		case 3: /* 8 bpp */
		{
			for(y=0;y<y_size;y++)
			{
				for(x=0;x<x_size;x++)
				{
					bitmap.pix32(y+y_start, x+x_start) = m_palette->pen((vram[count])&0xff);

					count++;
				}
			}
		}
		break;
		case 4: /* 16 bpp */
		{
			for(y=0;y<y_size;y++)
			{
				for(x=0;x<x_size;x++)
				{
					int r,g,b,pen;

					pen = ((vram[count]<<8)|(vram[count+1]))&0xffff;
					r = (pen & 0x000f);
					g = (pen & 0x00f0) >> 4;
					b = (pen & 0x0f00) >> 8;
					r = (r << 4) | (r & 0xf);
					g = (g << 4) | (g & 0xf);
					b = (b << 4) | (b & 0xf);

					bitmap.pix32(y+y_start, x+x_start) =  b | g << 8 | r << 16;

					count+=2;
				}
			}
		}
		break;
		case 6: /* 32 bpp */
		{
			for(y=0;y<y_size;y++)
			{
				for(x=0;x<x_size;x++)
				{
					int r,g,b,pen;

					pen = ((vram[count]<<24)|(vram[count+1]<<16)|(vram[count+2]<<8)|(vram[count+3]<<0));
					r = (pen & 0x0000ff);
					g = (pen & 0x00ff00) >> 8;
					b = (pen & 0xff0000) >> 16;

					bitmap.pix32(y+y_start, x+x_start) =  b | g << 8 | r << 16;

					count+=4;
				}
			}
		}
		break;
		#endif
		default:
			fatalerror("VIDC20 %08x BPP mode not supported\n",m_vidc20_bpp_mode);
			break;
	}

	return 0;
}

/*
*
* IOMD / ARM7500 / ARM7500FE chip emulation
*
*/

/* TODO: some of these registers are actually ARM7500 specific */
static const char *const iomd_regnames[] =
{
	"I/O Control",                      // 0x000 IOCR
	"Keyboard Data",                    // 0x004 KBDDAT
	"Keyboard Control",                 // 0x008 KBDCR
	"General Purpose I/O Lines",        // 0x00c IOLINES
	"IRQA Status",                      // 0x010 IRQSTA
	"IRQA Request/clear",               // 0x014 IRQRQA
	"IRQA Mask",                        // 0x018 IRQMSKA
	"Enter SUSPEND Mode",               // 0x01c SUSMODE
	"IRQB Status",                      // 0x020 IRQSTB
	"IRQB Request/clear",               // 0x024 IRQRQB
	"IRQB Mask",                        // 0x028 IRQMSKB
	"Enter STOP Mode",                  // 0x02c STOPMODE
	"FIQ Status",                       // 0x030 FIQST
	"FIQ Request/clear",                // 0x034 FIQRQ
	"FIQ Mask",                         // 0x038 FIQMSK
	"Clock divider control",            // 0x03c CLKCTL
	"Timer 0 Low Bits",                 // 0x040 T0LOW
	"Timer 0 High Bits",                // 0x044 T0HIGH
	"Timer 0 Go Command",               // 0x048 T0GO
	"Timer 0 Latch Command",            // 0x04c T0LATCH
	"Timer 1 Low Bits",                 // 0x050 T1LOW
	"Timer 1 High Bits",                // 0x054 T1HIGH
	"Timer 1 Go Command",               // 0x058 T1GO
	"Timer 1 Latch Command",            // 0x05c T1LATCH
	"IRQC Status",                      // 0x060 IRQSTC
	"IRQC Request/clear",               // 0x064 IRQRQC
	"IRQC Mask",                        // 0x068 IRQMSKC
	"LCD and IIS Control Bits",         // 0x06c VIDIMUX
	"IRQD Status",                      // 0x070 IRQSTD
	"IRQD Request/clear",               // 0x074 IRQRQD
	"IRQD Mask",                        // 0x078 IRQMSKD
	"<RESERVED>",                       // 0x07c
	"ROM Control Bank 0",               // 0x080 ROMCR0
	"ROM Control Bank 1",               // 0x084 ROMCR1
	"DRAM Control (IOMD)",              // 0x088 DRAMCR
	"VRAM and Refresh Control",         // 0x08c VREFCR
	"Flyback Line Size",                // 0x090 FSIZE
	"Chip ID no. Low Byte",             // 0x094 ID0
	"Chip ID no. High Byte",            // 0x098 ID1
	"Chip Version Number",              // 0x09c VERSION
	"Mouse X Position",                 // 0x0a0 MOUSEX
	"Mouse Y Position",                 // 0x0a4 MOUSEY
	"Mouse Data",                       // 0x0a8 MSEDAT
	"Mouse Control",                    // 0x0ac MSECR
	"<RESERVED>",                       // 0x0b0
	"<RESERVED>",                       // 0x0b4
	"<RESERVED>",                       // 0x0b8
	"<RESERVED>",                       // 0x0bc
	"DACK Timing Control",              // 0x0c0 DMATCR
	"I/O Timing Control",               // 0x0c4 IOTCR
	"Expansion Card Timing",            // 0x0c8 ECTCR
	"DMA External Control",             // 0x0cc DMAEXT (IOMD) / ASTCR (ARM7500)
	"DRAM Width Control",               // 0x0d0 DRAMWID
	"Force CAS/RAS Lines Low",          // 0x0d4 SELFREF
	"<RESERVED>",                       // 0x0d8
	"<RESERVED>",                       // 0x0dc
	"A to D IRQ Control",               // 0x0e0 ATODICR
	"A to D IRQ Status",                // 0x0e4 ATODCC
	"A to D IRQ Converter Control",     // 0x0e8 ATODICR
	"A to D IRQ Counter 1",             // 0x0ec ATODCNT1
	"A to D IRQ Counter 2",             // 0x0f0 ATODCNT2
	"A to D IRQ Counter 3",             // 0x0f4 ATODCNT3
	"A to D IRQ Counter 4",             // 0x0f8 ATODCNT4
	"<RESERVED>",                       // 0x0fc
	"I/O DMA 0 CurA",                   // 0x100 IO0CURA
	"I/O DMA 0 EndA",                   // 0x104 IO0ENDA
	"I/O DMA 0 CurB",                   // 0x108 IO0CURB
	"I/O DMA 0 EndB",                   // 0x10c IO0ENDB
	"I/O DMA 0 Control",                // 0x110 IO0CR
	"I/O DMA 0 Status",                 // 0x114 IO0ST
	"<RESERVED>",                       // 0x118
	"<RESERVED>",                       // 0x11c
	"I/O DMA 1 CurA",                   // 0x120 IO1CURA
	"I/O DMA 1 EndA",                   // 0x124 IO1ENDA
	"I/O DMA 1 CurB",                   // 0x128 IO1CURB
	"I/O DMA 1 EndB",                   // 0x12c IO1ENDB
	"I/O DMA 1 Control",                // 0x130 IO1CR
	"I/O DMA 1 Status",                 // 0x134 IO1ST
	"<RESERVED>",                       // 0x138
	"<RESERVED>",                       // 0x13c
	"I/O DMA 2 CurA",                   // 0x140 IO2CURA
	"I/O DMA 2 EndA",                   // 0x144 IO2ENDA
	"I/O DMA 2 CurB",                   // 0x148 IO2CURB
	"I/O DMA 2 EndB",                   // 0x14c IO2ENDB
	"I/O DMA 2 Control",                // 0x150 IO2CR
	"I/O DMA 2 Status",                 // 0x154 IO2ST
	"<RESERVED>",                       // 0x158
	"<RESERVED>",                       // 0x15c
	"I/O DMA 3 CurA",                   // 0x160 IO3CURA
	"I/O DMA 3 EndA",                   // 0x164 IO3ENDA
	"I/O DMA 3 CurB",                   // 0x168 IO3CURB
	"I/O DMA 3 EndB",                   // 0x16c IO3ENDB
	"I/O DMA 3 Control",                // 0x170 IO3CR
	"I/O DMA 3 Status",                 // 0x174 IO3ST
	"<RESERVED>",                       // 0x178
	"<RESERVED>",                       // 0x17c
	"Sound DMA 0 CurA",                 // 0x180 SD0CURA
	"Sound DMA 0 EndA",                 // 0x184 SD0ENDA
	"Sound DMA 0 CurB",                 // 0x188 SD0CURB
	"Sound DMA 0 EndB",                 // 0x18c SD0ENDB
	"Sound DMA 0 Control",              // 0x190 SD0CR
	"Sound DMA 0 Status",               // 0x194 SD0ST
	"<RESERVED>",                       // 0x198
	"<RESERVED>",                       // 0x19c
	"Sound DMA 1 CurA",                 // 0x1a0 SD1CURA
	"Sound DMA 1 EndA",                 // 0x1a4 SD1ENDA
	"Sound DMA 1 CurB",                 // 0x1a8 SD1CURB
	"Sound DMA 1 EndB",                 // 0x1ac SD1ENDB
	"Sound DMA 1 Control",              // 0x1b0 SD1CR
	"Sound DMA 1 Status",               // 0x1b4 SD1ST
	"<RESERVED>",                       // 0x1b8
	"<RESERVED>",                       // 0x1bc
	"Cursor DMA Current",               // 0x1c0 CURSCUR
	"Cursor DMA Init",                  // 0x1c4 CURSINIT
	"Duplex LCD Current B",             // 0x1c8 VIDCURB
	"<RESERVED>",                       // 0x1cc
	"Video DMA Current",                // 0x1d0 VIDCUR
	"Video DMA End",                    // 0x1d4 VIDEND
	"Video DMA Start",                  // 0x1d8 VIDSTART
	"Video DMA Init",                   // 0x1dc VIDINIT
	"Video DMA Control",                // 0x1e0 VIDCR
	"<RESERVED>",                       // 0x1e4
	"Duplex LCD Init B",                // 0x1e8 VIDINITB
	"<RESERVED>",                       // 0x1ec
	"DMA IRQ Status",                   // 0x1f0 DMAST
	"DMA IRQ Request",                  // 0x1f4 DMARQ
	"DMA IRQ Mask",                     // 0x1f8 DMAMSK
	"<RESERVED>"                        // 0x1fc
};

#define IOMD_IOCR       0x000/4
#define IOMD_KBDDAT     0x004/4
#define IOMD_KBDCR      0x008/4

#define IOMD_IRQSTA     0x010/4
#define IOMD_IRQRQA     0x014/4
#define IOMD_IRQMSKA    0x018/4

#define IOMD_IRQSTB     0x020/4
#define IOMD_IRQRQB     0x024/4
#define IOMD_IRQMSKB    0x028/4

#define IOMD_T0LOW      0x040/4
#define IOMD_T0HIGH     0x044/4
#define IOMD_T0GO       0x048/4
#define IOMD_T0LATCH    0x04c/4

#define IOMD_T1LOW      0x050/4
#define IOMD_T1HIGH     0x054/4
#define IOMD_T1GO       0x058/4
#define IOMD_T1LATCH    0x05c/4

#define IOMD_ID0        0x094/4
#define IOMD_ID1        0x098/4
#define IOMD_VERSION    0x09c/4

#define IOMD_VIDCUR     0x1d0/4
#define IOMD_VIDEND     0x1d4/4
#define IOMD_VIDSTART   0x1d8/4
#define IOMD_VIDINIT    0x1dc/4
#define IOMD_VIDCR      0x1e0/4

#define IOMD_DMARQ      0x1f4/4

void riscpc_state::update_irq()
{
	if (m_IRQ_status_A & m_IRQ_mask_A)
		m_maincpu->pulse_input_line(ARM7_IRQ_LINE, m_maincpu->minimum_quantum_time());
	if (m_IRQ_status_B & m_IRQ_mask_B)
		m_maincpu->pulse_input_line(ARM7_IRQ_LINE, m_maincpu->minimum_quantum_time());
}

void riscpc_state::fire_iomd_timer(int timer)
{
	int timer_count = m_timer_counter[timer];
	int val = timer_count / 2; // correct?

	if(val==0)
		m_IOMD_timer[timer]->adjust(attotime::never);
	else
		m_IOMD_timer[timer]->adjust(attotime::from_usec(val), 0, attotime::from_usec(val));
}

TIMER_CALLBACK_MEMBER(riscpc_state::IOMD_timer0_callback)
{
	m_IRQ_status_A|=0x20;
	update_irq();
}

TIMER_CALLBACK_MEMBER(riscpc_state::IOMD_timer1_callback)
{
	m_IRQ_status_A|=0x40;
	update_irq();
}

TIMER_CALLBACK_MEMBER(riscpc_state::flyback_timer_callback)
{
	m_IRQ_status_A|=0x08;
	update_irq();

	// for convenience lets just implement the video DMA transfer here
	// the actual transfer probably works per scanline once enabled.
	if(m_viddma_status & 0x20)
		viddma_transfer_start();

	m_flyback_timer->adjust(m_screen->time_until_pos(m_vidc20_vert_reg[VDER]));
}

void riscpc_state::viddma_transfer_start()
{
	address_space &mem = m_maincpu->space(AS_PROGRAM);
	uint32_t src = m_viddma_addr_init;
	uint32_t dst = 0;
	uint32_t size = m_viddma_addr_end;
	uint32_t dma_index;

	/* TODO: this should actually be a qword transfer */
	for(dma_index = 0;dma_index < size;dma_index+=4)
	{
		m_vram[dst] = mem.read_dword(src);

		src+=4;
		// TODO: honor boundary
		dst++;
	}
}

READ32_MEMBER( riscpc_state::a7000_iomd_r )
{
//  if(offset != IOMD_KBDCR)
//      logerror("IOMD: %s Register (%04x) read\n",iomd_regnames[offset & (0x1ff >> 2)],offset*4);


	switch(offset)
	{
		case IOMD_IOCR:
		{
			uint8_t flyback;
			int vert_pos;

			vert_pos = m_screen->vpos();
			flyback = (vert_pos <= m_vidc20_vert_reg[VDSR] || vert_pos >= m_vidc20_vert_reg[VDER]) ? 0x80 : 0x00;

			return flyback | (m_IOMD_IO_ctrl & 0x7c) | 0x34 | (m_i2c_clk << 1) | (m_i2cmem->read_sda() ? 1 : 0);
		}
		case IOMD_KBDDAT:   return m_kbdc->data_r();
		case IOMD_KBDCR:    return m_kbdc->status_r();

		/*
			1--- ---- always high (force)
			-x-- ---- Timer 1
			--x- ---- Timer 0
			---x ---- Power On Reset
			---- x--- Flyback
			---- -x-- nINT1
			---- --0- always low
			---- ---x INT2
		*/
		case IOMD_IRQSTA:   return (m_IRQ_status_A & 0x7d) | 0x80;
		case IOMD_IRQRQA:   return (m_IRQ_status_A & m_IRQ_mask_A);
		case IOMD_IRQMSKA:  return m_IRQ_mask_A;

		case IOMD_IRQSTB:   return m_IRQ_status_B;
		case IOMD_IRQRQB:   return (m_IRQ_status_B & m_IRQ_mask_B);
		case IOMD_IRQMSKB:  return m_IRQ_mask_B;

		case IOMD_T0LOW:    return m_timer_out[0] & 0xff;
		case IOMD_T0HIGH:   return (m_timer_out[0] >> 8) & 0xff;

		case IOMD_T1LOW:    return m_timer_out[1] & 0xff;
		case IOMD_T1HIGH:   return (m_timer_out[1] >> 8) & 0xff;

		case IOMD_ID0:      return m_io_id & 0xff; // IOMD ID low
		case IOMD_ID1:      return (m_io_id >> 8) & 0xff; // IOMD ID high
		case IOMD_VERSION:  return 0;

		case IOMD_VIDEND:   return m_viddma_addr_end & 0x00fffff8; //bits 31:24 undefined
		case IOMD_VIDSTART: return m_viddma_addr_start & 0x1ffffff8; //bits 31, 30, 29 undefined
		case IOMD_VIDINIT:  return m_viddma_addr_init & 0x1ffffff8;
		case IOMD_VIDCR:    return (m_viddma_status & 0xa0) | 0x50; //bit 6 = DRAM mode, bit 4 = QWORD transfer

		default:    logerror("IOMD: %s Register (%04x) read\n",iomd_regnames[offset & (0x1ff >> 2)],offset*4); break;
	}

	return 0;
}

WRITE32_MEMBER( riscpc_state::a7000_iomd_w )
{
//  logerror("IOMD: %s Register (%04x) write = %08x\n",iomd_regnames[offset & (0x1ff >> 2)],offset*4,data);

	switch(offset)
	{
		case IOMD_IOCR: 
			m_IOMD_IO_ctrl = data & 0x7c;
			m_i2cmem->write_sda(data & 0x01);
			m_i2c_clk = (data & 2) >> 1;
			m_i2cmem->write_scl(m_i2c_clk);
			break;

		case IOMD_KBDDAT: m_kbdc->data_w(data); break;
		case IOMD_KBDCR:  m_kbdc->command_w(data); 
			//m_IRQ_status_B |= 0x40;
			//machine().debug_break();
			//update_irq();
			break;

		case IOMD_IRQRQA:   
			m_IRQ_status_A &= ~data;
			m_IRQ_status_A |= 0x80;
			update_irq();
			break;
			
		case IOMD_IRQMSKA:  
			m_IRQ_mask_A = data; 
			update_irq();
			break;

		case IOMD_IRQRQB:
			m_IRQ_status_B &= ~data;
			update_irq();
			break;
			
		case IOMD_IRQMSKB:  
			m_IRQ_mask_B = data; 
			update_irq();
			break;

		case IOMD_T0LOW:    m_timer_in[0] = (m_timer_in[0] & 0xff00) | (data & 0xff); break;
		case IOMD_T0HIGH:   m_timer_in[0] = (m_timer_in[0] & 0x00ff) | ((data & 0xff) << 8); break;
		case IOMD_T0GO:
			m_timer_counter[0] = m_timer_in[0];
			fire_iomd_timer(0);
			break;
		case IOMD_T0LATCH:
			{
				m_t0readinc^=1;
				m_timer_out[0] = m_timer_counter[0];
				if(m_t0readinc)
				{
					m_timer_counter[0]--;
					if(m_timer_counter[0] < 0)
						m_timer_counter[0]+= m_timer_in[0];
				}
			}
			break;

		case IOMD_T1LOW:    m_timer_in[1] = (m_timer_in[1] & 0xff00) | (data & 0xff); break;
		case IOMD_T1HIGH:   m_timer_in[1] = (m_timer_in[1] & 0x00ff) | ((data & 0xff) << 8); break;
		case IOMD_T1GO:
			m_timer_counter[1] = m_timer_in[1];
			fire_iomd_timer(1);
			break;
		case IOMD_T1LATCH:
			{
				m_t1readinc^=1;
				m_timer_out[1] = m_timer_counter[1];
				if(m_t1readinc)
				{
					m_timer_counter[1]--;
					if(m_timer_counter[1] < 0)
						m_timer_counter[1]+= m_timer_in[1];
				}
			}
			break;

		case IOMD_VIDEND:   m_viddma_addr_end = data & 0x00fffff8; //bits 31:24 unused
			break;
		case IOMD_VIDSTART: m_viddma_addr_start = data & 0x1ffffff8; //bits 31, 30, 29 unused
			break;
		case IOMD_VIDINIT:  m_viddma_addr_init = data & 0x1ffffff8;
			break;
		case IOMD_VIDCR:
			m_viddma_status = data & 0xa0; if(data & 0x20) { viddma_transfer_start(); }
			break;


		default: logerror("IOMD: %s Register (%04x) write = %08x\n",iomd_regnames[offset & (0x1ff >> 2)],offset*4,data);
	}
}

void riscpc_state::a7000_map(address_map &map)
{
	map(0x00000000, 0x003fffff).mirror(0x00800000).rom().region("user1", 0);
//  AM_RANGE(0x01000000, 0x01ffffff) AM_NOP //expansion ROM
	// 
//	map(0x02000000, 0x027fffff).mirror(0x00800000).ram(); // VRAM, not installed on A7000 models
//  I/O 03000000 - 033fffff
//  AM_RANGE(0x03010000, 0x03011fff) //Super IO
//  AM_RANGE(0x03012000, 0x03029fff) //FDC
//  AM_RANGE(0x0302b000, 0x0302bfff) //Network podule
//  AM_RANGE(0x03040000, 0x0304ffff) //podule space 0,1,2,3
//  AM_RANGE(0x03070000, 0x0307ffff) //podule space 4,5,6,7
	map(0x03200000, 0x032001ff).rw(FUNC(riscpc_state::a7000_iomd_r), FUNC(riscpc_state::a7000_iomd_w)); //IOMD Registers //mirrored at 0x03000000-0x1ff?
	map(0x03310000, 0x03310003).portr("MOUSE");

	map(0x03400000, 0x037fffff).w(FUNC(riscpc_state::a7000_vidc20_w));
//  AM_RANGE(0x08000000, 0x08ffffff) AM_MIRROR(0x07000000) //EASI space
	map(0x10000000, 0x13ffffff).ram(); //SIMM 0 bank 0
	map(0x14000000, 0x17ffffff).ram(); //SIMM 0 bank 1
//  AM_RANGE(0x18000000, 0x18ffffff) AM_MIRROR(0x03000000) AM_RAM //SIMM 1 bank 0
//  AM_RANGE(0x1c000000, 0x1cffffff) AM_MIRROR(0x03000000) AM_RAM //SIMM 1 bank 1
}

void riscpc_state::riscpc_map(address_map &map)
{
	a7000_map(map);
	map(0x02000000, 0x027fffff).mirror(0x00800000).ram(); // VRAM
}


/* Input ports */
static INPUT_PORTS_START( a7000 )
//	PORT_INCLUDE( at_keyboard )

	PORT_START("MOUSE")
	// for debugging we leave video and sound HWs as options, eventually slotify them
	PORT_CONFNAME( 0x01, 0x00, "Monitor Type" )
	PORT_CONFSETTING(    0x00, "VGA" )
	PORT_CONFSETTING(    0x01, "TV Screen" )
	PORT_BIT( 0x0e, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Mouse Right")   PORT_CODE(MOUSECODE_BUTTON3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Mouse Center")  PORT_CODE(MOUSECODE_BUTTON2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Mouse Left")    PORT_CODE(MOUSECODE_BUTTON1)
	// TODO: understand condition where this occurs
	PORT_CONFNAME( 0x80, 0x00, "CMOS Reset bit" )
	PORT_CONFSETTING(    0x00, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x80, DEF_STR( On ) )
	PORT_CONFNAME( 0x100, 0x000, "Sound HW" )
	PORT_CONFSETTING(    0x000, "16-bit" )
	PORT_CONFSETTING(    0x100, "8-bit" )
	PORT_BIT(0xfffffe00, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

void riscpc_state::machine_start()
{
	m_IOMD_timer[0] = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(riscpc_state::IOMD_timer0_callback),this));
	m_IOMD_timer[1] = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(riscpc_state::IOMD_timer1_callback),this));
	m_flyback_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(riscpc_state::flyback_timer_callback),this));

	m_io_id = 0xd4e7;
}

void riscpc_state::machine_reset()
{
	m_IOMD_IO_ctrl = 0x08 | 0x34; //bit 0,1 and 3 set high on reset plus 2,4,5 always high
	// TODO: jumps to EASI space at $0c0016xx with this on!?
//  m_IRQ_status_A = 0x10; // set POR bit ON
	m_IRQ_mask_A = 0x00;

	m_IOMD_keyb_ctrl = 0x00;

	m_IOMD_timer[0]->adjust(attotime::never);
	m_IOMD_timer[1]->adjust(attotime::never);
	m_flyback_timer->adjust(attotime::never);
}

WRITE_LINE_MEMBER(riscpc_state::keyboard_reset)
{
	printf("RST %d\n",state);
}

WRITE_LINE_MEMBER(riscpc_state::keyboard_interrupt)
{
	printf("IRQ %d\n",state);
	if (!state)
		return;
	
//	machine().debug_break();
	m_IRQ_status_B|=0x80;
	update_irq();
}


void riscpc_state::base_config(machine_config &config)
{
	I2C_24C02(config, m_i2cmem);

	pc_kbdc_device &kbd_con(PC_KBDC(config, "kbd_con", 0));
	kbd_con.out_clock_cb().set(m_kbdc, FUNC(ps2_keyboard_controller_device::kbd_clk_w));
	kbd_con.out_data_cb().set(m_kbdc, FUNC(ps2_keyboard_controller_device::kbd_data_w));

	// TODO: verify type
	pc_kbdc_slot_device &kbd(PC_KBDC_SLOT(config, "kbd", pc_at_keyboards, STR_KBD_IBM_PC_AT_101));
	kbd.set_pc_kbdc_slot(&kbd_con);

	// auxiliary connector
//	pc_kbdc_device &aux_con(PC_KBDC(config, "aux_con", 0));
//	aux_con.out_clock_cb().set(m_kbdc, FUNC(ps2_keyboard_controller_device::aux_clk_w));
//	aux_con.out_data_cb().set(m_kbdc, FUNC(ps2_keyboard_controller_device::aux_data_w));

	// auxiliary port
//	pc_kbdc_slot_device &aux(PC_KBDC_SLOT(config, "aux", ps2_mice, STR_HLE_PS2_MOUSE));
//	aux.set_pc_kbdc_slot(&aux_con);

	PS2_KEYBOARD_CONTROLLER(config, m_kbdc, 12_MHz_XTAL);
	m_kbdc->hot_res().set(FUNC(riscpc_state::keyboard_reset));
	m_kbdc->kbd_clk().set(kbd_con, FUNC(pc_kbdc_device::clock_write_from_mb));
	m_kbdc->kbd_data().set(kbd_con, FUNC(pc_kbdc_device::data_write_from_mb));
	m_kbdc->kbd_irq().set(FUNC(riscpc_state::keyboard_interrupt));
//	m_kbdc->aux_clk().set(aux_con, FUNC(pc_kbdc_device::clock_write_from_mb));
//	m_kbdc->aux_data().set(aux_con, FUNC(pc_kbdc_device::data_write_from_mb));
//	m_kbdc->aux_irq().set(FUNC(riscpc_state::keyboard_interrupt));

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	m_screen->set_size(1900, 1080); //max available size
	m_screen->set_visarea(0, 1900-1, 0, 1080-1);
	m_screen->set_screen_update(FUNC(riscpc_state::screen_update));
	PALETTE(config, m_palette).set_entries(0x200);
}

void riscpc_state::rpc600(machine_config &config)
{
	/* Basic machine hardware */
	ARM7(config, m_maincpu, 60_MHz_XTAL/2); // ARM610
	m_maincpu->set_addrmap(AS_PROGRAM, &riscpc_state::riscpc_map);
	base_config(config);
}

void riscpc_state::rpc700(machine_config &config)
{
	/* Basic machine hardware */
	ARM710A(config, m_maincpu, 80_MHz_XTAL/2); // ARM710
	m_maincpu->set_addrmap(AS_PROGRAM, &riscpc_state::riscpc_map);
	base_config(config);
}

void riscpc_state::a7000(machine_config &config)
{
	/* Basic machine hardware */
	ARM7(config, m_maincpu, XTAL(32'000'000)); // ARM7500
	m_maincpu->set_addrmap(AS_PROGRAM, &riscpc_state::a7000_map);
	base_config(config);
}

void riscpc_state::a7000p(machine_config &config)
{
	a7000(config);
	m_maincpu->set_clock(XTAL(48'000'000)); // ARM7500FE
}

void riscpc_state::sarpc(machine_config &config)
{
	/* Basic machine hardware */
	ARM7(config, m_maincpu, 202000000); // StrongARM
	m_maincpu->set_addrmap(AS_PROGRAM, &riscpc_state::riscpc_map);
	base_config(config);
}

void riscpc_state::sarpc_j233(machine_config &config)
{
	/* Basic machine hardware */
	ARM7(config, m_maincpu, 233000000); // StrongARM
	m_maincpu->set_addrmap(AS_PROGRAM, &riscpc_state::riscpc_map);
	base_config(config);
}

ROM_START(rpc600)
	ROM_REGION( 0x800000, "user1", ROMREGION_ERASEFF )
	// Version 3.50
	ROM_SYSTEM_BIOS( 0, "350", "RiscOS 3.50" )
	ROMX_LOAD("0277,521-01.bin", 0x000000, 0x100000, CRC(8ba4444e) SHA1(1b31d7a6e924bef0e0056c3a00a3fed95e55b175), ROM_BIOS(0))
	ROMX_LOAD("0277,522-01.bin", 0x100000, 0x100000, CRC(2bc95c9f) SHA1(f8c6e2a1deb4fda48aac2e9fa21b9e01955331cf), ROM_BIOS(0))
ROM_END

ROM_START(rpc700)
	ROM_REGION( 0x800000, "user1", ROMREGION_ERASEFF )
	// Version 3.60
	ROM_SYSTEM_BIOS( 0, "360", "RiscOS 3.60" )
	ROMX_LOAD("1203,101-01.bin", 0x000000, 0x200000, CRC(2eeded56) SHA1(7217f942cdac55033b9a8eec4a89faa2dd63cd68), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("1203,102-01.bin", 0x000002, 0x200000, CRC(6db87d21) SHA1(428403ed31682041f1e3d114ea02a688d24b7d94), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
ROM_END

ROM_START(a7000)
	ROM_REGION( 0x800000, "user1", ROMREGION_ERASEFF )
	// Version 3.60
	ROM_SYSTEM_BIOS( 0, "360", "RiscOS 3.60" )
	ROMX_LOAD("1203,101-01.bin", 0x000000, 0x200000, CRC(2eeded56) SHA1(7217f942cdac55033b9a8eec4a89faa2dd63cd68), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("1203,102-01.bin", 0x000002, 0x200000, CRC(6db87d21) SHA1(428403ed31682041f1e3d114ea02a688d24b7d94), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
ROM_END

ROM_START(a7000p)
	ROM_REGION( 0x800000, "user1", ROMREGION_ERASEFF )
	// Version 3.71
	ROM_SYSTEM_BIOS( 0, "371", "RiscOS 3.71" )
	ROMX_LOAD("1203,261-01.bin", 0x000000, 0x200000, CRC(8e3c570a) SHA1(ffccb52fa8e165d3f64545caae1c349c604386e9), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("1203,262-01.bin", 0x000002, 0x200000, CRC(cf4615b4) SHA1(c340f29aeda3557ebd34419fcb28559fc9b620f8), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
	// Version 4.02
	ROM_SYSTEM_BIOS( 1, "402", "RiscOS 4.02" )
	ROMX_LOAD("riscos402_1.bin", 0x000000, 0x200000, CRC(4c32f7e2) SHA1(d290e29a4de7be9eb36cbafbb2dc99b1c4ce7f72), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(1))
	ROMX_LOAD("riscos402_2.bin", 0x000002, 0x200000, CRC(7292b790) SHA1(67f999c1ccf5419e0a142b7e07f809e13dfed425), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(1))
	// Version 4.39
	ROM_SYSTEM_BIOS( 2, "439", "RiscOS 4.39" )
	ROMX_LOAD("riscos439_1.bin", 0x000000, 0x200000, CRC(dab94cb8) SHA1(a81fb7f1a8117f85e82764675445092d769aa9af), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(2))
	ROMX_LOAD("riscos439_2.bin", 0x000002, 0x200000, CRC(22e6a5d4) SHA1(b73b73c87824045130840a19ce16fa12e388c039), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(2))
ROM_END

ROM_START(sarpc)
	ROM_REGION( 0x800000, "user1", ROMREGION_ERASEFF )
	// Version 3.70
	ROM_SYSTEM_BIOS( 0, "370", "RiscOS 3.70" )
	ROMX_LOAD("1203,191-01.bin", 0x000000, 0x200000, NO_DUMP, ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("1203,192-01.bin", 0x000002, 0x200000, NO_DUMP, ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
ROM_END

ROM_START(sarpc_j233)
	ROM_REGION( 0x800000, "user1", ROMREGION_ERASEFF )
	// Version 3.71
	ROM_SYSTEM_BIOS( 0, "371", "RiscOS 3.71" )
	ROMX_LOAD("1203,261-01.bin", 0x000000, 0x200000, CRC(8e3c570a) SHA1(ffccb52fa8e165d3f64545caae1c349c604386e9), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("1203,262-01.bin", 0x000002, 0x200000, CRC(cf4615b4) SHA1(c340f29aeda3557ebd34419fcb28559fc9b620f8), ROM_GROUPWORD | ROM_SKIP(2) | ROM_BIOS(0))
ROM_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*    YEAR  NAME        PARENT  COMPAT  MACHINE     INPUT  CLASS         INIT        COMPANY  FULLNAME                  FLAGS */
COMP( 1994, rpc600,     0,      0,      rpc600,     a7000, riscpc_state, empty_init, "Acorn", "Risc PC 600",            MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1994, rpc700,     rpc600, 0,      rpc700,     a7000, riscpc_state, empty_init, "Acorn", "Risc PC 700",            MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1995, a7000,      rpc600, 0,      a7000,      a7000, riscpc_state, empty_init, "Acorn", "Archimedes A7000",       MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1997, a7000p,     rpc600, 0,      a7000p,     a7000, riscpc_state, empty_init, "Acorn", "Archimedes A7000+",      MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1997, sarpc,      rpc600, 0,      sarpc,      a7000, riscpc_state, empty_init, "Acorn", "StrongARM Risc PC",      MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1997, sarpc_j233, rpc600, 0,      sarpc_j233, a7000, riscpc_state, empty_init, "Acorn", "J233 StrongARM Risc PC", MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
