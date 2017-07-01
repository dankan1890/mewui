// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Midway Zeus2 Video

**************************************************************************/
#include "zeus2.h"

#define LOG_REGS         1

/*************************************
*  Constructor
*************************************/
zeus2_renderer::zeus2_renderer(zeus2_device *state)
	: poly_manager<float, zeus2_poly_extra_data, 4, 10000>(state->machine())
	, m_state(state)
{
}

const device_type ZEUS2 = &device_creator<zeus2_device>;

zeus2_device::zeus2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, ZEUS2, "Midway Zeus2", tag, owner, clock, "zeus2", __FILE__),
	m_vblank(*this), m_irq(*this), m_atlantis(0)
{
}

/*************************************
*  Display interrupt generation
*************************************/

TIMER_CALLBACK_MEMBER(zeus2_device::display_irq_off)
{
	m_vblank(CLEAR_LINE);

	//attotime vblank_period = m_screen->time_until_pos(m_zeusbase[0x37] & 0xffff);

	///* if zero, adjust to next frame, otherwise we may get stuck in an infinite loop */
	//if (vblank_period == attotime::zero)
	//  vblank_period = m_screen->frame_period();
	//vblank_timer->adjust(vblank_period);
	vblank_timer->adjust(m_screen->time_until_vblank_start());
	//machine().scheduler().timer_set(attotime::from_hz(30000000), timer_expired_delegate(FUNC(zeus2_device::display_irq), this));
}

TIMER_CALLBACK_MEMBER(zeus2_device::display_irq)
{
	m_vblank(ASSERT_LINE);
	/* set a timer for the next off state */
	//machine().scheduler().timer_set(m_screen->time_until_pos(0), timer_expired_delegate(FUNC(zeus2_device::display_irq_off), this), 0, this);
	machine().scheduler().timer_set(m_screen->time_until_vblank_end(), timer_expired_delegate(FUNC(zeus2_device::display_irq_off), this), 0, this);
	//machine().scheduler().timer_set(attotime::from_hz(30000000), timer_expired_delegate(FUNC(zeus2_device::display_irq_off), this));
}

TIMER_CALLBACK_MEMBER(zeus2_device::int_timer_callback)
{
	//m_maincpu->set_input_line(2, ASSERT_LINE);
	m_irq(ASSERT_LINE);
}

/*************************************
 *  Video startup
 *************************************/


void zeus2_device::device_start()
{
	/* allocate memory for "wave" RAM */
	waveram = auto_alloc_array(machine(), uint32_t, WAVERAM0_WIDTH * WAVERAM0_HEIGHT * 8/4);
	m_frameColor = std::make_unique<uint32_t[]>(WAVERAM1_WIDTH * WAVERAM1_HEIGHT * 2);
	m_frameDepth = std::make_unique<uint32_t[]>(WAVERAM1_WIDTH * WAVERAM1_HEIGHT * 2);

	/* initialize polygon engine */
	poly = auto_alloc(machine(), zeus2_renderer(this));

	//m_screen = machine().first_screen();
	m_screen = downcast<screen_device *>(machine().device("screen"));
	m_vblank.resolve_safe();
	m_irq.resolve_safe();

	/* we need to cleanup on exit */
	//machine().add_notifier(MACHINE_NOTIFY_EXIT, machine_notify_delegate(&zeus2_device::exit_handler2, this));

	int_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(zeus2_device::int_timer_callback), this));
	vblank_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(zeus2_device::display_irq), this));

	//printf("%s\n", machine().system().name);
	m_thegrid = strcmp(machine().system().name, "thegrid")==0;

	/* save states */
	save_pointer(NAME(waveram), WAVERAM0_WIDTH * WAVERAM0_HEIGHT * 2);
	save_pointer(NAME(m_frameColor.get()), WAVERAM1_WIDTH * WAVERAM1_HEIGHT * 2);
	save_pointer(NAME(m_frameDepth.get()), WAVERAM1_WIDTH * WAVERAM1_HEIGHT * 2);
	save_pointer(NAME(m_zeusbase), 0x80);
	save_pointer(NAME(m_renderRegs), 0x50);
	save_pointer(NAME(m_pal_table), 0x100);
	save_item(NAME(zeus_fifo));
	save_item(NAME(zeus_fifo_words));
	save_item(NAME(zeus_cliprect.min_x));
	save_item(NAME(zeus_cliprect.max_x));
	save_item(NAME(zeus_cliprect.min_y));
	save_item(NAME(zeus_cliprect.max_y));
	save_item(NAME(zeus_matrix));
	save_item(NAME(zeus_point));
	save_item(NAME(zeus_point2));
	save_item(NAME(zeus_texbase));
	save_item(NAME(zeus_quad_size));
	save_item(NAME(m_fill_color));
	save_item(NAME(m_fill_depth));
	save_item(NAME(m_renderAddr));
	save_item(NAME(m_yScale));
}

void zeus2_device::device_reset()
{
	memset(m_zeusbase, 0, sizeof(m_zeusbase[0]) * 0x80);
	memset(m_renderRegs, 0, sizeof(m_renderRegs[0]) * 0x50);

	m_palSize = 0;
	zbase = 32.0f;
	m_yScale = 0;
	yoffs = 0x1dc000;
	//yoffs = 0x00040000;
	texel_width = 256;
	zeus_fifo_words = 0;
	m_fill_color = 0;
	m_fill_depth = 0;
	m_renderAddr = 0;
}
#if DUMP_WAVE_RAM
#include <iostream>
#include <fstream>
#endif
void zeus2_device::device_stop()
{
#if DUMP_WAVE_RAM
	std::string fileName = "waveram_";
	fileName += machine().system().name;
	fileName += ".bin";
	std::ofstream myfile;
	myfile.open(fileName.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

	if (myfile.is_open())
		myfile.write((char *)waveram, WAVERAM0_WIDTH * WAVERAM0_HEIGHT * 2 * sizeof(uint32_t));
	myfile.close();
#endif

#if TRACK_REG_USAGE
{
	reg_info *info;
	int regnum;

	for (regnum = 0; regnum < 0x80; regnum++)
	{
		printf("Register %02X\n", regnum);
		if (regread_count[regnum] == 0)
			printf("\tNever read\n");
		else
			printf("\tRead %d times\n", regread_count[regnum]);

		if (regwrite_count[regnum] == 0)
			printf("\tNever written\n");
		else
		{
			printf("\tWritten %d times\n", regwrite_count[regnum]);
			for (info = regdata[regnum]; info != nullptr; info = info->next)
				printf("\t%08X\n", info->value);
		}
	}

	for (regnum = 0; regnum < 0x100; regnum++)
		if (subregwrite_count[regnum] != 0)
		{
			printf("Sub-Register %02X (%d writes)\n", regnum, subregwrite_count[regnum]);
			for (info = subregdata[regnum]; info != nullptr; info = info->next)
				printf("\t%08X\n", info->value);
		}
}
#endif

}



/*************************************
 *
 *  Video update
 *
 *************************************/

uint32_t zeus2_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	// Wait until configuration is completed before transfering anything
	if (m_zeusbase[0x30] == 0)
		return 0;

	int x, y;

	poly->wait("SCREEN_UPDATE");

	//if (machine().input().code_pressed(KEYCODE_DOWN)) { zbase += machine().input().code_pressed(KEYCODE_LSHIFT) ? 0x10 : 1; popmessage("Zbase = %f", (double)zbase); }
	//if (machine().input().code_pressed(KEYCODE_UP)) { zbase -= machine().input().code_pressed(KEYCODE_LSHIFT) ? 0x10 : 1; popmessage("Zbase = %f", (double)zbase); }

	/* normal update case */
	//if (!machine().input().code_pressed(KEYCODE_W))
	if (1)
	{
		int xoffs = screen.visible_area().min_x;
		for (y = cliprect.min_y; y <= cliprect.max_y; y++)
		{
			uint32_t *colorptr = &m_frameColor[frame_addr_from_xy(0, y, false)];
			uint32_t *dest = &bitmap.pix32(y);
			for (x = cliprect.min_x; x <= cliprect.max_x; x++) {
				uint32_t bufX = x - xoffs;
				dest[x] = colorptr[bufX];
			}
		}
	}

	/* waveram drawing case */
	else
	{
		const void *base;

		if (machine().input().code_pressed(KEYCODE_DOWN)) yoffs += machine().input().code_pressed(KEYCODE_LSHIFT) ? 0x1000 : 40;
		if (machine().input().code_pressed(KEYCODE_UP)) yoffs -= machine().input().code_pressed(KEYCODE_LSHIFT) ? 0x1000 : 40;
		if (machine().input().code_pressed(KEYCODE_LEFT) && texel_width > 4) { texel_width >>= 1; while (machine().input().code_pressed(KEYCODE_LEFT)) ; }
		if (machine().input().code_pressed(KEYCODE_RIGHT) && texel_width < 512) { texel_width <<= 1; while (machine().input().code_pressed(KEYCODE_RIGHT)) ; }

		if (yoffs < 0) yoffs = 0;
		if (1) {
			//base = waveram0_ptr_from_expanded_addr(yoffs << 8);
			//base = waveram0_ptr_from_expanded_addr(yoffs);
			base = WAVERAM_BLOCK0(yoffs);
		}
		else
			base = (void *)&m_frameColor[yoffs << 6];

		int xoffs = screen.visible_area().min_x;
		for (y = cliprect.min_y; y <= cliprect.max_y; y++)
		{
			uint32_t *dest = &bitmap.pix32(y);
			for (x = cliprect.min_x; x <= cliprect.max_x; x++)
			{
				if (1) {
					uint8_t tex = get_texel_8bit((uint64_t *)base, y, x, texel_width);
					dest[x] = (tex << 16) | (tex << 8) | tex;
				}
				else {
					dest[x] = ((uint32_t *)(base))[((y * WAVERAM1_WIDTH)) + x - xoffs];
				}
			}
		}
		popmessage("offs = %06X base = %08X", yoffs, base);
	}

	return 0;
}



/*************************************
 *
 *  Core read handler
 *
 *************************************/

READ32_MEMBER( zeus2_device::zeus2_r )
{
	int logit = (offset != 0x00 && offset != 0x01 &&
		offset != 0x48 && offset != 0x49 &&
		offset != 0x54 && offset != 0x58 && offset != 0x59 && offset != 0x5a);
	logit &= LOG_REGS;
	uint32_t result = m_zeusbase[offset];
#if TRACK_REG_USAGE
	regread_count[offset]++;
#endif

	switch (offset)
	{
		case 0x00:
			result = 0x20;
			break;

		case 0x01:
			/* bit  $000C0070 are tested in a loop until 0 */
			/* bits $00080000 is tested in a loop until 0 */
			/* bit  $00000004 is tested for toggling; probably VBLANK */
			result = 0x00;
			if (m_screen->vblank())
				result |= 0x04;
			break;

		case 0x07:
			/* this is needed to pass the self-test in thegrid */
			result = 0x10451998;
			break;

		case 0x54:
			/* both upper 16 bits and lower 16 bits seem to be used as vertical counters */
			result = (m_screen->vpos() << 16) | m_screen->vpos();
			break;
	}

	if (logit)
		logerror("%08X:zeus2_r(%02X) = %08X\n", machine().device("maincpu")->safe_pc(), offset, result);

	return result;
}



/*************************************
 *
 *  Core write handler
 *
 *************************************/

WRITE32_MEMBER( zeus2_device::zeus2_w )
{
	int logit = (offset != 0x08 &&
					(offset != 0x20 || data != 0) &&
					offset != 0x40 && offset != 0x41 && offset != 0x48 && offset != 0x49 && offset != 0x4e &&
					offset != 0x50 && offset != 0x51 && offset != 0x57 && offset != 0x58 && offset != 0x59 && offset != 0x5a && offset != 0x5e
		);
	logit &= LOG_REGS;
	if (logit)
		logerror("%08X:zeus2_w", machine().device("maincpu")->safe_pc());
	zeus2_register32_w(offset, data, logit);
}



/*************************************
 *
 *  Handle register writes
 *
 *************************************/

void zeus2_device::zeus2_register32_w(offs_t offset, uint32_t data, int logit)
{
	uint32_t oldval = m_zeusbase[offset];

#if TRACK_REG_USAGE
regwrite_count[offset]++;
if (regdata_count[offset] < 256)
{
	reg_info **tailptr;

	for (tailptr = &regdata[offset]; *tailptr != nullptr; tailptr = &(*tailptr)->next)
		if ((*tailptr)->value == data)
			break;
	if (*tailptr == nullptr)
	{
		*tailptr = alloc_or_die(reg_info);
		(*tailptr)->next = nullptr;
		(*tailptr)->value = data;
		regdata_count[offset]++;
	}
}
#endif

	/* writes to register $CC need to force a partial update */
//  if ((offset & ~1) == 0xcc)
//      m_screen->update_partial(m_screen->vpos());

	/* always write to low word? */
	m_zeusbase[offset] = data;

	/* log appropriately */
	if (logit) {
		logerror("(%02X) = %08X", offset, data);
	}
	/* handle the update */
	zeus2_register_update(offset, oldval, logit);
}



/*************************************
 *
 *  Update state after a register write
 *
 *************************************/

void zeus2_device::zeus2_register_update(offs_t offset, uint32_t oldval, int logit)
{
	/* handle the writes; only trigger on low accesses */
	switch (offset)
	{
		case 0x08:
			zeus_fifo[zeus_fifo_words++] = m_zeusbase[0x08];
			if (zeus2_fifo_process(zeus_fifo, zeus_fifo_words))
				zeus_fifo_words = 0;

			/* set the interrupt signal to indicate we can handle more */
			int_timer->adjust(attotime::from_nsec(500));
			break;

		case 0x20:
			/* toggles between two values based on the page:

			    Page #      m_zeusbase[0x20]      m_zeusbase[0x38]
			    ------      --------------      --------------
			       0          $04000190           $00000000
			       1          $04000000           $01900000
			*/
			zeus2_pointer_write(m_zeusbase[0x20] >> 24, (m_zeusbase[0x20] & 0xffffff), logit);
			break;

		case 0x22:
			if (logit)
				logerror("\tRend Setup 0");
			break;

		case 0x23:
			if (logit)
				logerror("\tRend Setup 1");
			break;

		case 0x24:
			if (logit)
				logerror("\tRend Setup 4");
			break;

		case 0x2a:
			// 0x000000c0 = bilenear off
			if (logit)
				logerror("\tRend Force Off");
			break;

		case 0x2b:
			if (logit)
				logerror("\tRend Force On");
			break;

		case 0x2c:
			if (logit)
				logerror("\tRend AE Flag");
			break;

		case 0x2d:
			if (logit)
				logerror("\tRend AF Flag");
			break;

		case 0x2f:
			if (logit)
				logerror("\tPixel Proc Setup");
			break;


		case 0x30:
			{
				m_yScale = (((m_zeusbase[0x39] >> 16) & 0xfff) < 0x100) ? 0 : 1;
				int hor = ((m_zeusbase[0x34] & 0xffff) - (m_zeusbase[0x33] >> 16)) << m_yScale;
				int ver = ((m_zeusbase[0x35] & 0xffff) + 1) << m_yScale;
				popmessage("reg[30]: %08X Screen: %dH X %dV yScale: %d", m_zeusbase[0x30], hor, ver, m_yScale);
			}
			m_screen->update_partial(m_screen->vpos());
			{
				int vtotal = (m_zeusbase[0x37] & 0xffff) << m_yScale;
				int htotal = (m_zeusbase[0x34] >> 16) << m_yScale;
				rectangle visarea((m_zeusbase[0x33] >> 16) << m_yScale, htotal - 1, 0, (m_zeusbase[0x35] & 0xffff) << m_yScale);
				if (htotal > 0 && vtotal > 0 && visarea.min_x < visarea.max_x && visarea.max_y < vtotal)
				{
					m_screen->configure(htotal, vtotal, visarea, HZ_TO_ATTOSECONDS((double)ZEUS2_VIDEO_CLOCK / 4.0 / (htotal * vtotal)));
					zeus_cliprect = visarea;
					zeus_cliprect.max_x -= zeus_cliprect.min_x;
					zeus_cliprect.min_x = 0;
					// Startup vblank timer
					vblank_timer->adjust(attotime::from_usec(1));
				}
			}
			break;

		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
			break;

		case 0x38:
			{
				uint32_t temp = m_zeusbase[0x38];
				m_zeusbase[0x38] = oldval;
				m_screen->update_partial(m_screen->vpos());
				log_fifo = machine().input().code_pressed(KEYCODE_L);
				//log_fifo = 1;
				m_zeusbase[0x38] = temp;
			}
			break;

		case 0x40:
		{
			int code = (m_zeusbase[0x40] >> 16) & 0xf;
			if (code == 0x2) {
				/* in direct mode it latches values */
				if ((m_zeusbase[0x4e] & 0x20))
				{
					const void *src = waveram0_ptr_from_expanded_addr(m_zeusbase[0x41]);
					m_zeusbase[0x48] = WAVERAM_READ32(src, 0);
					m_zeusbase[0x49] = WAVERAM_READ32(src, 1);

					if (m_zeusbase[0x4e] & 0x40)
					{
						m_zeusbase[0x41]++;
						m_zeusbase[0x41] += (m_zeusbase[0x41] & 0x400) << 6;
						m_zeusbase[0x41] &= ~0xfc00;
					}
				}
				// mwskinsa 0xE085001F -- light table?
				// thegrid 0x0055002C ???
				// crusnexo 0x005500FF ???
				// crusnexo 0x00A20000 ???
			}
			else if (code == 0x5) {
				// Zeus burst from waveram
				if (logit)
					logerror("\t-- ucode load: control: %08X addr: %08X", m_zeusbase[0x40], m_zeusbase[0x41]);
				// Load ucode from waveram
				poly->wait("UCODE_LOAD");
				void *dataPtr = waveram0_ptr_from_expanded_addr(m_zeusbase[0x41]);
				load_ucode(dataPtr, m_zeusbase[0x40], logit);
				if (((m_zeusbase[0x40] >> 24) & 0xff) == 0xc0) {
					// Light table load
					if (logit) logerror("\t-- light table loaded");
				}
				else {
					// Zeus Quad Size
					switch (m_zeusbase[0x40]) {
					case 0x38550083: case 0x2D550083: case 0x3885007B:
						zeus_quad_size = 14;
						break;
					case 0x3855006F: case 0x38550088: case 0x388500A9:
						zeus_quad_size = 12;
						break;
					case 0x38550075:
						if (m_zeusbase[0x41] == 0x00000324)
							zeus_quad_size = 12;
						else
							zeus_quad_size = 10;
						break;
					case 0x38850077:
						zeus_quad_size = 10;
						break;
					default:
						logerror(" default quad size 10\n");
						zeus_quad_size = 10;
						break;
					}
				}
				if (1 && logit) {
					uint32_t *wavePtr = (uint32_t*)waveram0_ptr_from_expanded_addr(m_zeusbase[0x41]);
					uint32_t waveData;
					int size = m_zeusbase[0x40] & 0xff;
					logerror("\n Setup size=%d [40]=%08X [41]=%08X [4e]=%08X\n", zeus_quad_size, m_zeusbase[0x40], m_zeusbase[0x41], m_zeusbase[0x4e]);
					for (int i = 0; i <= size; ++i) {
						waveData = *wavePtr++;
						logerror(" %08X", waveData);
						waveData = *wavePtr++;
						//logerror(" %08X", waveData);
						if (0 && (i + 1) % 16 == 0)
							logerror("\n");
					}
					logerror("\n");
				}
			}
			else if (code == 0x4) {
				// Load pal table from RGB555
				if (logit)
					logerror("\t-- pal table rgb555 load: control: %08X addr: %08X", m_zeusbase[0x40], m_zeusbase[0x41]);
				poly->wait("PAL_TABLE_WRITE");
				void *dataPtr = waveram0_ptr_from_expanded_addr(m_zeusbase[0x41]);
				load_pal_table(dataPtr, m_zeusbase[0x40], 0, logit);
			}
			else {
				if (logit)
					logerror("\t-- unknown burst: control: %08X addr: %08X", m_zeusbase[0x40], m_zeusbase[0x41]);
			}
		}
		break;
		case 0x41:
			/* this is the address, except in read mode, where it latches values */
			if (m_zeusbase[0x4e] & 0x10)
			{
				const void *src = waveram0_ptr_from_expanded_addr(oldval);
				m_zeusbase[0x41] = oldval;
				m_zeusbase[0x48] = WAVERAM_READ32(src, 0);
				m_zeusbase[0x49] = WAVERAM_READ32(src, 1);

				if (m_zeusbase[0x4e] & 0x40)
				{
					m_zeusbase[0x41]++;
					m_zeusbase[0x41] += (m_zeusbase[0x41] & 0x400) << 6;
					m_zeusbase[0x41] &= ~0xfc00;
				}
			} else {
				// mwskinsa (atlantis) writes 0xffffffff and expects 0x1fff03ff to be read back
				m_zeusbase[0x41] &= 0x1fff03ff;
			}
			break;

		case 0x48:
		case 0x49:
			/* if we're in write mode, process it */
			if (m_zeusbase[0x40] == 0x00890000)
			{
				/*
				    m_zeusbase[0x4e]:
				        bit 0-1: which register triggers write through
				        bit 3:   enable write through via these registers
				        bit 4:   seems to be set during reads, when 0x41 is used for latching
				        bit 6:   enable autoincrement on write through
				*/
				if ((m_zeusbase[0x4e] & 0x08) && (offset & 3) == (m_zeusbase[0x4e] & 3))
				{
					void *dest = waveram0_ptr_from_expanded_addr(m_zeusbase[0x41]);
					WAVERAM_WRITE32(dest, 0, m_zeusbase[0x48]);
					WAVERAM_WRITE32(dest, 1, m_zeusbase[0x49]);
					if (logit)
						logerror("\t[41]=%08X [4E]=%08X", m_zeusbase[0x41], m_zeusbase[0x4e]);

					if (m_zeusbase[0x4e] & 0x40)
					{
						m_zeusbase[0x41]++;
						m_zeusbase[0x41] += (m_zeusbase[0x41] & 0x400) << 6;
						m_zeusbase[0x41] &= ~0xfc00;
					}
				}
			}

			/* make sure we log anything else */
			else if (logit)
				logerror("\t[40]=%08X [4E]=%08X\n", m_zeusbase[0x40], m_zeusbase[0x4e]);
			break;

		case 0x50:
			if (m_zeusbase[0x50] == 0x00510000) {
				// SGRAM Special Mode Register Write
				if (m_zeusbase[0x51] == 0x00200000) {
					// SGRAM Mask Register
					if ((m_zeusbase[0x58] & m_zeusbase[0x59] & m_zeusbase[0x5a]) != 0xffffffff)
						logerror("zeus2_register_update: Warning! Mask Register not equal to 0xffffffff\n");
				}
				if (m_zeusbase[0x51] == 0x00400000) {
					// SGRAM Color Register
					m_fill_color = m_zeusbase[0x58];
					m_fill_depth = ((m_zeusbase[0x5a] & 0xffff) << 16) | ((m_zeusbase[0x58]>>24)<<8);
					m_fill_depth >>= 8;  // Sign extend down to 24 bits
					if (m_zeusbase[0x58] != m_zeusbase[0x59])
						logerror("zeus2_register_update: Warning! Different fill colors are set.\n");
				}
			}
			//else if (1 && ((m_zeusbase[0x50] & 0x000f0000)==0x80000) && (m_zeusbase[0x50] & 0xffff)) {
			else if ((m_zeusbase[0x50] &  0x80000) && (m_zeusbase[0x50] & 0xffff)) {
				// Fast fill
				// Unknown what the exact bit fields are, this is a just a guess
				// Atlantis: 0x00983FFF => clear entire frame buffer, 0x00981FFF => clear one frame
				// crusnexo: 0x007831FF => clear one frame
				// thegrid:  0x008831FF => clear one frame
				// thegrid:  0x0079FFFF => clear entire frame buffer at 51=0 then 51=00800000, only seen at initial tests in thegrid
				uint32_t addr = frame_addr_from_phys_addr(m_zeusbase[0x51]);
				uint32_t numBytes = (m_zeusbase[0x50] & 0xffff) + 1;
				numBytes *= 0x40;
				if (m_zeusbase[0x50] & 0x10000) {
					addr = 0x0;
					numBytes = WAVERAM1_WIDTH * WAVERAM1_HEIGHT * 8;
					//printf("Clearing buffer: numBytes: %08X addr: %08X reg50: %08X\n", numBytes, addr, m_zeusbase[0x50]);
				}
				if (logit)
					logerror(" -- Clearing buffer: numBytes: %08X addr: %08X reg51: %08X", numBytes, addr, m_zeusbase[0x51]);
				memset(&m_frameColor[addr], m_fill_color, numBytes);
				memset(&m_frameDepth[addr], m_fill_depth, numBytes);
			}
			else if ((m_zeusbase[0x5e] >> 16) != 0xf208 && !(m_zeusbase[0x5e] & 0xffff)) {
			/* If 0x5e==0xf20a0000 (atlantis) or 0xf20d0000 (the grid) then process the read/write now */
				/*
				m_zeusbase[0x5e]:
				bit 0-1: which register triggers write through
				bit 3:   enable write through via these registers
				bit 4:   seems to be set during reads, when 0x51 is used for latching
				bit 5:   unknown, currently used to specify ordering, but this is suspect
				bit 6:   enable autoincrement on write through
				*/
				//if (m_zeusbase[0x50] == 0x00890000)
				if ((m_zeusbase[0x50] == 0x00890000) || (m_zeusbase[0x50] == 0x00e90000))
				{
					frame_write();
				}
				else if (m_zeusbase[0x50] == 0x00720000) {
					/* Do the read */
					frame_read();
				}
				/* make sure we log anything else */
				//else if (logit || m_zeusbase[0x50] != 0x0)
				//	logerror("\tw[50]=%08X [5E]=%08X\n", m_zeusbase[0x50], m_zeusbase[0x5e]);
			}
			break;
		case 0x51:
			// Set direct rendering location
			m_renderAddr = frame_addr_from_phys_addr(m_zeusbase[0x51]);
			/* in this mode, crusnexo expects the reads to immediately latch */
			//if ((m_zeusbase[0x50] == 0x00a20000) || (m_zeusbase[0x50] == 0x00720000))
			if (m_zeusbase[0x50] == 0x00a20000)
				oldval = m_zeusbase[0x51];

			/* this is the address, except in read mode, where it latches values */
			if ((m_zeusbase[0x5e] & 0x10) || (m_zeusbase[0x50] == 0x00a20000))
			{
				m_zeusbase[0x51] = oldval;
				frame_read();
			}
			break;

		case 0x57:
			/* thegrid uses this to write either left or right halves of pixels */
			//if (m_zeusbase[0x50] == 0x00e90000)
			//{
			//  uint32_t addr = frame_addr_from_reg51();
			//  if (m_zeusbase[0x57] & 1)
			//      m_frameColor[addr] = m_zeusbase[0x58];
			//  if (m_zeusbase[0x57] & 4)
			//      m_frameColor[addr+1] = m_zeusbase[0x59];
			//}

			///* make sure we log anything else */
			//else if (logit)
			//  logerror("\t[50]=%08X [5E]=%08X\n", m_zeusbase[0x50], m_zeusbase[0x5e]);
			break;

		case 0x58:
		case 0x59:
		case 0x5a:
			/* if we're in write mode, process it */
			if (m_zeusbase[0x50] == 0x00890000)
			{
				/*
				    m_zeusbase[0x5e]:
				        bit 0-1: which register triggers write through
				        bit 3:   enable write through via these registers
				        bit 4:   seems to be set during reads, when 0x51 is used for latching
				        bit 5:   unknown, currently used to specify ordering, but this is suspect
				        bit 6:   enable autoincrement on write through
				*/
				if ((m_zeusbase[0x5e] & 0x08) && (offset & 3) == (m_zeusbase[0x5e] & 3))
				{
					frame_write();
				}
			}

			/* make sure we log anything else */
			else if ((m_zeusbase[0x5e] >> 16) == 0xf208)
				if (logit)
					logerror("\t[50]=%08X [5E]=%08X", m_zeusbase[0x50], m_zeusbase[0x5e]);
			break;

		// 0x60, 0x61, 0x62 Translation matrix, set using fifo command

		case 0x63:
			if (logit)
				logerror("\tMAC Trans3 HSR Correction Factor = %8.2f", reinterpret_cast<float&>(m_zeusbase[offset]));
			break;

		case 0x64:
			if (logit)
				logerror("\tMAC Offset");
			break;

		case 0x65:
			if (logit)
				logerror("\tMAC Offset");
			break;

		case 0x66:
			if (logit)
				logerror("\tMultiply Offset");
			break;

		case 0x67:
			if (logit)
				logerror("\tMath MAC Setup");
			break;

		case 0x68:
			if (logit)
				logerror("\tALU Float Offset");
			break;

		case 0x6A:
			if (logit)
				logerror("\tALU RegC X_OFF = %8.2f", reinterpret_cast<float&>(m_zeusbase[offset]));
			break;

		case 0x6B:
			if (logit)
				logerror("\tALU RegD Y_OFF = %8.2f", reinterpret_cast<float&>(m_zeusbase[offset]));
			break;

		case 0x6c:
			if (logit)
				logerror("\tALU Inv Offset");
			break;

		case 0x6f:
			if (logit)
				logerror("\tLight Table Setup");
			break;

		case 0x76:
			if (logit)
				logerror("\tMath Comp Reg0 XClip = %8.2f", reinterpret_cast<float&>(m_zeusbase[offset]));
			break;

		case 0x77:
			if (logit)
				logerror("\tMath Comp Reg1 YClip = %8.2f", reinterpret_cast<float&>(m_zeusbase[offset]));
			break;

		case 0x78:
			if (logit)
				logerror("\tMath Comp Reg2 ZClip = %8.2f", reinterpret_cast<float&>(m_zeusbase[offset]));
			break;

		case 0x79:
			if (logit)
				logerror("\tMath Comp Reg3 YRange = %8.2f", reinterpret_cast<float&>(m_zeusbase[offset]));
			break;

		case 0x7a:
			if (logit)
				logerror("\tMath Conditional Branch Setup");
			break;

		case 0x7c:
			if (logit)
				logerror("\tMath Compare Setup 1");
			break;

		case 0x7D:
			if (logit)
				logerror("\tMath FIFO AF / AE Input FIFO AF / AE");
			break;

		case 0x7f:
			if (logit)
				logerror("\tMath Setup Reg");
			break;

	}
	if (logit)
		logerror("\n");
}

/*************************************
*  Load pal table from waveram
*************************************/
void zeus2_device::load_pal_table(void *wavePtr, uint32_t ctrl, int type, int logit)
{
	int count = ctrl & 0xffff;
	m_palSize = (count + 1) * 4;
	uint32_t addr = (ctrl >> 24) << 1;
	uint32_t *tablePtr = &m_pal_table[addr];
	if (type == 0) {
		// Convert from RGB555
		uint16_t *src = (uint16_t*)wavePtr;
		for (int i = 0; i <= count; ++i) {
			*tablePtr++ = conv_rgb555_to_rgb32(*src++);
			*tablePtr++ = conv_rgb555_to_rgb32(*src++);
			*tablePtr++ = conv_rgb555_to_rgb32(*src++);
			*tablePtr++ = conv_rgb555_to_rgb32(*src++);
		}
	}
	else {
		// Raw Copy
		uint32_t *src = (uint32_t*)wavePtr;
		for (int i = 0; i <= count; ++i) {
			*tablePtr++ = *src++;
			*tablePtr++ = *src++;
		}
	}
	if (logit) {
		logerror("\ntable: ");
		tablePtr = &m_pal_table[addr];
		for (int i = 0; i <= count*4; ++i) {
			logerror(" %08X", *tablePtr++);
			if (0 && (i + 1) % 16 == 0)
				logerror("\n");
		}
		logerror("\n");
	}
}
/*************************************
*  Load microcode from waveram
*************************************/
void zeus2_device::load_ucode(void *wavePtr, uint32_t ctrl, int logit)
{
	int count = ctrl & 0xffff;
	uint32_t addr = (ctrl >> 24) << 1;
	uint32_t *src = (uint32_t*)wavePtr;
	uint32_t *tablePtr = &m_ucode[addr];
	for (int i = 0; i <= count; ++i) {
		*tablePtr++ = *src++;
		*tablePtr++ = *src++;
	}
}

/*************************************
 *
 *  Process the FIFO
 *
 *************************************/

void zeus2_device::zeus2_pointer_write(uint8_t which, uint32_t value, int logit)
{
#if TRACK_REG_USAGE
subregwrite_count[which]++;
if (subregdata_count[which] < 256)
{
	reg_info **tailptr;

	for (tailptr = &subregdata[which]; *tailptr != nullptr; tailptr = &(*tailptr)->next)
		if ((*tailptr)->value == value)
			break;
	if (*tailptr == nullptr)
	{
		*tailptr = alloc_or_die(reg_info);
		(*tailptr)->next = nullptr;
		(*tailptr)->value = value;
		subregdata_count[which]++;
	}
}
#endif
	if (which<0x50)
		m_renderRegs[which] = value;

	switch (which)
	{
		case 0x01:
			// Limit to 16 bits
			m_renderRegs[which] &= 0xffff;
			if (logit)
				logerror("\t(R%02X) = %4i Rend XClip", which & 0xffff, value);
			break;

		case 0x02:
			// Limit to 16 bits
			m_renderRegs[which] &= 0xffff;
			if (logit)
				logerror("\t(R%02X) = %4i Rend YClip", which & 0xffff, value);
			break;

		case 0x03:
			if (logit)
				logerror("\t(R%02X) = %06x Rend XOffset", which, value);
			break;

		case 0x04:
			if (logit)
				logerror("\t(R%02X) = %06x Rend YOffset", which, value);
			break;

		case 0x05:
			zeus_texbase = value % (WAVERAM0_HEIGHT * WAVERAM0_WIDTH);
			if (logit)
				logerror("\t(R%02X)  texbase = %06x", which, zeus_texbase);
			break;

		case 0x07:
			if (logit)
				logerror("\t(R%02X)  texel mask = %06x", which, zeus_texbase);
			break;

		case 0x08:
			{
				//int blockNum = ((m_renderRegs[0x9] >> 16) * 1024 + (m_renderRegs[0x9] & 0xffff));
				int blockNum = m_renderRegs[0x9];
				void *dataPtr = (void *)(&waveram[blockNum * 2]);
				if (logit)
					logerror("\t(R%02X) = %06x PAL Control Load Table Byte Addr = %08X", which, value, blockNum * 8);
				load_pal_table(dataPtr, m_renderRegs[0x8], 0, logit);
			}
			break;

		case 0x09:
			if (logit)
				logerror("\t(R%02X) = %06x PAL Addr", which, value);
			break;

		case 0x0a:
			if (logit)
				logerror("\t(R%02X) = %4i Pixel ALU IntA", which, value);
			break;

		case 0x0b:
			if (logit)
				logerror("\t(R%02X) = %4i Pixel ALU IntB", which, value);
			break;

		case 0x0c:
			if (logit)
				logerror("\t(R%02X) = %4i Pixel ALU IntC (Translucency FG)", which, value);
			break;

		case 0x0d:
			if (logit)
				logerror("\t(R%02X) = %4i Pixel ALU IntD (Translucency BG)", which, value);
			break;

		case 0x11:
			if (logit)
				logerror("\t(R%02X)  Texel Setup = %06x", which, zeus_texbase);
			break;

		case 0x12:
			if (logit)
				logerror("\t(R%02X)  Pixel FIFO Setup = %06x", which, value);
			break;

		case 0x14:
			if (logit)
				logerror("\t(R%02X) = %06x ZBuf Control", which, value);
			break;

		case 0x15:
			m_zbufmin = int32_t((value & 0xffffff) << 8) >> 8;
			if (logit)
				logerror("\t(R%02X) = %d zbufmin", which, m_zbufmin / 4096.0f);
			break;

		case 0x40:
			// 0x004000 no shading
			// 0x024004 gouraud shading
			if (logit)
				logerror("\t(R%02X) = %06x Pixel ALU Control", which, value);
			break;

		case 0xff:
			// Reset???
			if (logit)
				logerror("\tRender Reset");
			break;

		default:
			if (logit)
				logerror("\t(R%02X) = %06x", which, value);
			break;


#if 0
		case 0x0c:
		case 0x0d:
			// These seem to have something to do with blending.
			// There are fairly unique 0x0C,0x0D pairs for various things:
			// Car reflection on initial screen: 0x40, 0x00
			// Additively-blended "flares": 0xFA, 0xFF
			// Car windshields (and drivers, apparently): 0x82, 0x7D
			// Other minor things: 0xA4, 0x100
			break;
#endif
	}
}

/*************************************
 *  Process the FIFO
 *************************************/

bool zeus2_device::zeus2_fifo_process(const uint32_t *data, int numwords)
{
	int dataoffs = 0;

	/* handle logging */
	switch (data[0] >> 24)
	{
		// 0x00: write 32-bit value to low registers
	case 0x00:
		// Ignore the all zeros commmand
		if (((data[0] >> 16) & 0x7f) == 0x0) {
			if (log_fifo && (data[0] & 0xfff) != 0x2c0)
				log_fifo_command(data, numwords, " -- ignored\n");
			return true;
		}
		// Drop through to 0x05 command
	/* 0x05: write 32-bit value to low registers */
	case 0x05:
		if (numwords < 2)
			return false;
		if (log_fifo)
			log_fifo_command(data, numwords, " -- reg32");
		if (((data[0] >> 16) & 0x7f) != 0x08)
			zeus2_register32_w((data[0] >> 16) & 0x7f, data[1], log_fifo);
		break;

		/* 0x08: set matrix and point (thegrid) */
		case 0x08:
			if (numwords < 14)
				return false;
			zeus2_register32_w(0x63, data[1], log_fifo);
			dataoffs = 1;

		/* 0x07: set matrix and point (crusnexo) */
		case 0x07:
			if (numwords < 13)
				return false;

			/* extract the matrix from the raw data */
			zeus_matrix[0][0] = convert_float(data[dataoffs + 1]);
			zeus_matrix[0][1] = convert_float(data[dataoffs + 2]);
			zeus_matrix[0][2] = convert_float(data[dataoffs + 3]);
			zeus_matrix[1][0] = convert_float(data[dataoffs + 4]);
			zeus_matrix[1][1] = convert_float(data[dataoffs + 5]);
			zeus_matrix[1][2] = convert_float(data[dataoffs + 6]);
			zeus_matrix[2][0] = convert_float(data[dataoffs + 7]);
			zeus_matrix[2][1] = convert_float(data[dataoffs + 8]);
			zeus_matrix[2][2] = convert_float(data[dataoffs + 9]);

			/* extract the translation point from the raw data */
			zeus_point[0] = convert_float(data[dataoffs + 10]);
			zeus_point[1] = convert_float(data[dataoffs + 11]);
			zeus_point[2] = convert_float(data[dataoffs + 12]);

			if (log_fifo)
			{
				log_fifo_command(data, numwords, "\n");
				logerror("\t\tmatrix ( %8.2f %8.2f %8.2f ) ( %8.2f %8.2f %8.2f ) ( %8.2f %8.2f %8.2f )\n\t\tvector %8.2f %8.2f %8.5f\n",
						(double) zeus_matrix[0][0], (double) zeus_matrix[0][1], (double) zeus_matrix[0][2],
						(double) zeus_matrix[1][0], (double) zeus_matrix[1][1], (double) zeus_matrix[1][2],
						(double) zeus_matrix[2][0], (double) zeus_matrix[2][1], (double) zeus_matrix[2][2],
						(double) zeus_point[0],
						(double) zeus_point[1],
						(double) zeus_point[2]);
			}
			break;

		// 0x14: ?? atlantis
		/* 0x15: set point only (thegrid) */
		/* 0x16: set point only (crusnexo) */
		case 0x14:
		case 0x15:
		case 0x16:
			if (numwords < 4)
				return false;

			/* extract the translation point from the raw data */
			zeus_point[0] = convert_float(data[1]);
			zeus_point[1] = convert_float(data[2]);
			zeus_point[2] = convert_float(data[3]);

			if (log_fifo)
			{
				log_fifo_command(data, numwords, "\n");
				logerror("\t\tvector %8.2f %8.2f %8.5f\n",
						(double) zeus_point[0],
						(double) zeus_point[1],
						(double) zeus_point[2]);
			}
			break;

		// 0x1c: thegrid (3 words)
		case 0x1c:
			if (m_thegrid) {
				if (numwords < 3)
					return false;
				if (log_fifo)
					log_fifo_command(data, numwords, " -- Init light source\n");
				break;
			}
		// 0x1b: thegrid
		// 0x1c: crusnexo (4 words)
		// 0x10: atlantis???
		case 0x10:
		case 0x1b:
			if (numwords < 4)
				return false;
			if (log_fifo)
			{
				log_fifo_command(data, numwords, " -- unknown control + happens after clear screen\n");
				logerror("\t\tvector2 %8.2f %8.2f %8.5f\n",
						(double) convert_float(data[1]),
						(double) convert_float(data[2]),
						(double) convert_float(data[3]));

				/* extract the translation point from the raw data */
				zeus_point2[0] = convert_float(data[1]);
				zeus_point2[1] = convert_float(data[2]);
				zeus_point2[2] = convert_float(data[3]);
			}
			break;

		// thegrid ???
		case 0x1d:
			if (numwords < 2)
				return false;
			if (log_fifo)
			{
				log_fifo_command(data, numwords, " -- unknown\n");
				logerror("\t\tdata %8.5f\n",
					(double)convert_float(data[1]));
			}
			break;

		/* 0x23: render model in waveram (thegrid) */
		/* 0x24: render model in waveram (crusnexo) */
		// 0x17: ??? (atlantis)
		case 0x17:
		case 0x23:
		case 0x24:
			if (numwords < 2)
				return false;
			if (log_fifo)
				log_fifo_command(data, numwords, "");
			zeus2_draw_model(data[1], data[0] & 0xffff, log_fifo);
			break;

		// 0x2d; set direct render pixels location (atlantis)
		case 0x2d:
			if (numwords < 2)
				return false;
			if (log_fifo)
				log_fifo_command(data, numwords, "\n");
			m_renderAddr = frame_addr_from_phys_addr(data[1]);
			//zeus2_draw_model(data[1], data[0] & 0xff, log_fifo);
			break;

		/* 0x31: sync pipeline? (thegrid) */
		/* 0x32: sync pipeline? (crusnexo) */
		// 0x25 ?? (atlantis)
		case 0x25:
		case 0x31:
		case 0x32:
			if (log_fifo)
				log_fifo_command(data, numwords, " sync? \n");
			break;

		/* 0x38: direct render quad (crusnexo) */
		// 0x38: direct write to frame buffer (atlantis)
		case 0x38:
			if (data[0] == 0x38000000) {
				if (numwords < 3)
					return false;
				// Direct write to frame buffer
				m_frameColor[m_renderAddr++] = conv_rgb555_to_rgb32((uint16_t)data[1]);
				m_frameColor[m_renderAddr++] = conv_rgb555_to_rgb32((uint16_t)(data[1] >> 16));
				m_frameColor[m_renderAddr++] = conv_rgb555_to_rgb32((uint16_t)data[2]);
				m_frameColor[m_renderAddr++] = conv_rgb555_to_rgb32((uint16_t)(data[2] >> 16));
			} else if (numwords < 12)
				return false;
			//print_fifo_command(data, numwords, "\n");
			if (log_fifo)
				log_fifo_command(data, numwords, "\n");
			break;

		default:
			if (1 || data[0] != 0x2c0)
			{
				printf("Unknown command %08X\n", data[0]);
				if (log_fifo)
					log_fifo_command(data, numwords, "\n");
			}
			break;
	}
	return true;
}

/*************************************
 *  Draw a model in waveram
 *************************************/

void zeus2_device::zeus2_draw_model(uint32_t baseaddr, uint16_t count, int logit)
{
	uint32_t databuffer[32];
	int databufcount = 0;
	int model_done = false;
	uint32_t texdata = 0;

	if (logit)
		logerror(" -- model @ %08X, len %04X\n", baseaddr, count);

	if (count > 0xc800)
		fatalerror("Extreme count\n");

	while (baseaddr != 0 && !model_done)
	{
		const void *base = waveram0_ptr_from_expanded_addr(baseaddr);
		int curoffs;

		/* reset the objdata address */
		baseaddr = 0;

		/* loop until we run out of data */
		for (curoffs = 0; curoffs <= count; curoffs++)
		{
			int countneeded = 2;
			uint8_t cmd;
			uint8_t subCmd;

			/* accumulate 2 words of data */
			databuffer[databufcount++] = WAVERAM_READ32(base, curoffs * 2 + 0);
			databuffer[databufcount++] = WAVERAM_READ32(base, curoffs * 2 + 1);

			/* if this is enough, process the command */
			cmd = databuffer[0] >> 24;
			subCmd = (databuffer[1] >> 24) & 0xfc;

			if ((cmd == 0x38 && subCmd != 0x38) || (cmd == 0x2d)) {
				countneeded = zeus_quad_size;
			}
			if (databufcount == countneeded)
			{
				/* handle logging of the command */
				if (logit)
				{
					//if ((cmd == 0x38) || (cmd == 0x2d))
					//  log_render_info(texdata);
					if (cmd != 0x00 || (cmd == 0x00 && curoffs == count)) {
						logerror("\t");
						for (int offs = 0; offs < databufcount; offs++)
							logerror("%08X ", databuffer[offs]);
						logerror("-- ");
					}
				}

				/* handle the command */
				switch (cmd)
				{
					case 0x00: // crusnexo
						if (logit && curoffs == count)
							logerror(" end cmd 00\n");
					case 0x21:  /* thegrid */
					case 0x22:  /* crusnexo */
						if (((databuffer[0] >> 16) & 0xff) == 0x9b)
						{
							texdata = databuffer[1];
							if (logit)
								logerror("texdata\n");
						}
						else if (logit)
							logerror("unknown offset\n");
						break;

					case 0x31:  /* thegrid */
						if (logit)
							logerror("sync?\n");
						break;

					case 0x29:  // atlantis
					case 0x35:  /* thegrid */
					case 0x36:  /* crusnexo */
						if (logit)
							logerror("reg32");
						zeus2_register32_w((databuffer[0] >> 16) & 0x7f, databuffer[1], logit);
						break;

					case 0x2d:  // atlantis
						poly->zeus2_draw_quad(databuffer, texdata, logit);
						break;

					case 0x38:  /* crusnexo/thegrid */
						if (subCmd == 0x38) {
							// Direct commands from waveram buffer
							//uint32_t cmdData[2];
							//for (int subIndex = 0; subIndex < 2; ++subIndex) {
							//  uint32_t offset = (databuffer[subIndex] & 0xff) * 6;
							//  //printf("directRead curoffs: 0x%X\n", curoffs);
							//  for (int cmdIndex = 0; cmdIndex < 3; ++cmdIndex) {
							//      cmdData[0] = m_directCmd[offset + cmdIndex * 2 + 0];
							//      cmdData[1] = m_directCmd[offset + cmdIndex * 2 + 1];
							//      if (curoffs < 0x40)
							//          printf("directRead curoffs: 0x%X cmdData %08X %08X\n", curoffs, cmdData[0], cmdData[1]);
							//      if (cmdData[0] != 0 && cmdData[1] != 0) {
							//          // Error check
							//          if (cmdData[0] != 0x58 && cmdData[0] != 0x5A) {
							//              if (curoffs < 0x20)
							//                  printf("case38 error curoffs: 0x%X cmdData %08X %08X\n", curoffs, cmdData[0], cmdData[1]);
							//          }
							//          else {
							//              zeus2_register32_w(cmdData[0] & 0x7f, cmdData[1], logit);
							//          }
							//      }
							//  }
							//}
							//void *palbase = waveram0_ptr_from_expanded_addr(m_zeusbase[0x41]);
							//uint8_t texel = databuffer[0];
							//uint32_t color = WAVERAM_READ16(palbase, texel);
							//m_frameDepth[m_renderAddr] = 0;
							//m_frameColor[m_renderAddr++] = conv_rgb555_to_rgb32(color);
							//texel = databuffer[1];
							//color = WAVERAM_READ16(palbase, texel);
							//m_frameDepth[m_renderAddr] = 0;
							//m_frameColor[m_renderAddr++] = conv_rgb555_to_rgb32(color);
							//m_frameDepth[m_renderAddr] = 0;
							//m_frameColor[m_renderAddr++] = databuffer[0] & 0x00ffffff;
							//m_frameDepth[m_renderAddr] = 0;
							//m_frameColor[m_renderAddr++] = databuffer[1] & 0x00ffffff;
							//if (logit)
							//  if ((curoffs + 1) % 16 == 0)
							//      logerror("\n");
						}
						else {
							poly->zeus2_draw_quad(databuffer, texdata, logit);
						}
						break;

					default:
						if (logit)
							logerror("unknown model data\n");
						break;
				}

				/* reset the count */
				databufcount = 0;
			}
		}
		// Log unused data
		if (databufcount != 0) {
			if (logit)
			{
				logerror("\t");
				for (int offs = 0; offs < databufcount; offs++)
					logerror("%08X ", databuffer[offs]);
				logerror("-- Unused data\n");
			}
		}
	}
}

/*************************************
 *  Draw a quad
 *************************************/
void zeus2_renderer::zeus2_draw_quad(const uint32_t *databuffer, uint32_t texdata, int logit)
{
	z2_poly_vertex clipvert[8];
	z2_poly_vertex vert[4];
	//  float uscale, vscale;
	float maxy, maxx;
	//  int val1, val2, texwshift;
	int numverts;
	int i;
	//  int16_t normal[3];
	//  int32_t rotnormal[3];

	if (logit)
		m_state->logerror("quad %d\n", m_state->zeus_quad_size);

	//if (machine().input().code_pressed(KEYCODE_Q) && (m_state->m_renderRegs[0x5] != 0x1fdf00)) return;
	//if (machine().input().code_pressed(KEYCODE_E) && (m_state->m_renderRegs[0x5] != 0x07f540)) return;
	//if (machine().input().code_pressed(KEYCODE_R) && (m_state->m_renderRegs[0x5] != 0x081580)) return;
	//if (machine().input().code_pressed(KEYCODE_T) && (m_state->m_renderRegs[0x5] != 0x14db00)) return;
	//if (machine().input().code_pressed(KEYCODE_Y) && (m_state->m_renderRegs[0x5] != 0x14d880)) return;
	//if (machine().input().code_pressed(KEYCODE_Q) && (texdata & 0xffff) == 0x119) return;
	//if (machine().input().code_pressed(KEYCODE_E) && (texdata & 0xffff) == 0x01d) return;
	//if (machine().input().code_pressed(KEYCODE_R) && (texdata & 0xffff) == 0x11d) return;
	//if (machine().input().code_pressed(KEYCODE_T) && (texdata & 0xffff) == 0x05d) return;
	//if (machine().input().code_pressed(KEYCODE_Y) && (texdata & 0xffff) == 0x0dd) return;
	//if (machine().input().code_pressed(KEYCODE_U) && (texdata & 0xffff) == 0x119) return;
	//if (machine().input().code_pressed(KEYCODE_I) && (texdata & 0xffff) == 0x119) return;
	//if (machine().input().code_pressed(KEYCODE_O) && (texdata & 0xffff) == 0x119) return;
	//if (machine().input().code_pressed(KEYCODE_L) && (texdata & 0x100)) return;

	/*
	0   38800000
	1   x2 | x1
	2   v1 | u1
	3   y2 | y1
	4   v2 | u2
	5   z2 | z1
	6   v3 | u3
	7   v4 | u4
	8   ???
	9   x4 | x3
	10  y4 | y3
	11  z4 | z3

	In memory:
	+0 = ???
	+1 = set via $05410000/value
	+2 = x1
	+3 = y1
	+4 = z1
	+5 = x2
	+6 = y2
	+7 = z2
	+8 = x3
	+9 = y3
	+10= z3
	+11= x4
	+12= y4
	+13= z4
	+14= uv1
	+15= uv2
	+16= uv3
	+17= uv4
	+18= set via $05200000/$05000000 | (value << 10) (uvoffset?)
	+19= ???


	38810000 00000000 00C7|FF38 FF5E|FF5E 15400154 11400114 00000000 00000000 FF38|00C7 00A3|00A3 -- quad
	xxxx|xxxx yyyy|yyyy                                     xxxx|xxxx yyyy|yyyy
	*/
	// Altantis rendermode: 0x024004 startup, then 0x020202, then 0x021E0E
	/* extract raw x,y,z */
	if (m_state->m_atlantis) {
			// Atlantis quad 14
		texdata = databuffer[1];
		vert[0].x = (int16_t)databuffer[2];
		vert[0].y = (int16_t)databuffer[3];
		vert[0].p[0] = (int16_t)databuffer[4];
		vert[0].p[1] = (databuffer[5] >> 0) & 0xff;
		vert[0].p[2] = (databuffer[5] >> 8) & 0xff;

		vert[1].x = (int16_t)(databuffer[2] >> 16);
		vert[1].y = (int16_t)(databuffer[3] >> 16);
		vert[1].p[0] = (int16_t)(databuffer[4] >> 16);
		vert[1].p[1] = (databuffer[5] >> 16) & 0xff;
		vert[1].p[2] = (databuffer[5] >> 24) & 0xff;

		vert[2].x = (int16_t)databuffer[6];
		vert[2].y = (int16_t)databuffer[7];
		vert[2].p[0] = (int16_t)databuffer[8];
		vert[2].p[1] = (databuffer[9] >> 0) & 0xff;
		vert[2].p[2] = (databuffer[9] >> 8) & 0xff;

		vert[3].x = (int16_t)(databuffer[6] >> 16);
		vert[3].y = (int16_t)(databuffer[7] >> 16);
		vert[3].p[0] = (int16_t)(databuffer[8] >> 16);
		vert[3].p[1] = (databuffer[9] >> 16) & 0xff;
		vert[3].p[2] = (databuffer[9] >> 24) & 0xff;
	}
	else {
		//printf("R40: %06X\n", m_state->m_renderRegs[0x40]);
		vert[0].x = (int16_t)databuffer[2];
		vert[0].y = (int16_t)databuffer[3];
		vert[0].p[0] = (int16_t)databuffer[6];
		vert[0].p[1] = (databuffer[1] >> 0) & 0x3ff;
		vert[0].p[2] = (databuffer[1] >> 16) & 0x3ff;

		vert[1].x = (int16_t)(databuffer[2] >> 16);
		vert[1].y = (int16_t)(databuffer[3] >> 16);
		vert[1].p[0] = (int16_t)(databuffer[6] >> 16);
		vert[1].p[1] = (databuffer[4] >> 0) & 0x3ff;
		vert[1].p[2] = (databuffer[4] >> 10) & 0x3ff;

		vert[2].x = (int16_t)databuffer[8];
		vert[2].y = (int16_t)databuffer[9];
		vert[2].p[0] = (int16_t)databuffer[7];
		vert[2].p[1] = (databuffer[4] >> 20) & 0x3ff;
		vert[2].p[2] = (databuffer[5] >> 0) & 0x3ff;

		vert[3].x = (int16_t)(databuffer[8] >> 16);
		vert[3].y = (int16_t)(databuffer[9] >> 16);
		vert[3].p[0] = (int16_t)(databuffer[7] >> 16);
		vert[3].p[1] = (databuffer[5] >> 10) & 0x3ff;
		vert[3].p[2] = (databuffer[5] >> 20) & 0x3ff;
	}
	int unknown[8];
	float unknownFloat[4];
	if (m_state->zeus_quad_size == 14) {
		// buffer 10-13 ???? 00000000 1FF7FC00 00000000 1FF7FC00 -- mwskinsa quad 14
		/* 10:13 16 bit coordinates */
		unknown[0] = (int16_t)databuffer[10];
		unknown[1] = (int16_t)(databuffer[10] >> 16);
		unknown[2] = (int16_t)databuffer[11];
		unknown[3] = (int16_t)(databuffer[11] >> 16);
		unknown[4] = (int16_t)databuffer[12];
		unknown[5] = (int16_t)(databuffer[12] >> 16);
		unknown[6] = (int16_t)databuffer[13];
		unknown[7] = (int16_t)(databuffer[13] >> 16);
		unknownFloat[0] = m_state->convert_float(databuffer[10]);
		unknownFloat[1] = m_state->convert_float(databuffer[11]);
		unknownFloat[2] = m_state->convert_float(databuffer[12]);
		unknownFloat[3] = m_state->convert_float(databuffer[13]);
	}
	/*
	vert[0].x = (int16_t)databuffer[1];
	vert[0].y = (int16_t)databuffer[3];
	vert[0].p[0] = (int16_t)databuffer[5];
	vert[0].p[1] = (uint16_t)databuffer[2];
	vert[0].p[2] = (uint16_t)(databuffer[2] >> 16);

	vert[1].x = (int16_t)(databuffer[1] >> 16);
	vert[1].y = (int16_t)(databuffer[3] >> 16);
	vert[1].p[0] = (int16_t)(databuffer[5] >> 16);
	vert[1].p[1] = (uint16_t)databuffer[4];
	vert[1].p[2] = (uint16_t)(databuffer[4] >> 16);

	vert[2].x = (int16_t)databuffer[9];
	vert[2].y = (int16_t)databuffer[10];
	vert[2].p[0] = (int16_t)databuffer[11];
	vert[2].p[1] = (uint16_t)databuffer[6];
	vert[2].p[2] = (uint16_t)(databuffer[6] >> 16);

	vert[3].x = (int16_t)(databuffer[9] >> 16);
	vert[3].y = (int16_t)(databuffer[10] >> 16);
	vert[3].p[0] = (int16_t)(databuffer[11] >> 16);
	vert[3].p[1] = (uint16_t)databuffer[7];
	vert[3].p[2] = (uint16_t)(databuffer[7] >> 16);
	*/

	int logextra = 1;

	int intScale = m_state->m_zeusbase[0x66] - 0x8e;
	float fScale = pow(2.0f, intScale);
	for (i = 0; i < 4; i++)
	{
		float x = vert[i].x;
		float y = vert[i].y;
		float z = vert[i].p[0];
		if (1) {
		  x *= fScale;
		  y *= fScale;
		  z *= fScale;
		}
		vert[i].x = x * m_state->zeus_matrix[0][0] + y * m_state->zeus_matrix[0][1] + z * m_state->zeus_matrix[0][2];
		vert[i].y = x * m_state->zeus_matrix[1][0] + y * m_state->zeus_matrix[1][1] + z * m_state->zeus_matrix[1][2];
		vert[i].p[0] = x * m_state->zeus_matrix[2][0] + y * m_state->zeus_matrix[2][1] + z * m_state->zeus_matrix[2][2];

		if (1 || !(m_state->m_renderRegs[0x14] & 0x000001)) {
			vert[i].x += m_state->zeus_point[0];
			vert[i].y += m_state->zeus_point[1];
			vert[i].p[0] += m_state->zeus_point[2];
		}
		//if (0)
		//	//vert[i].p[0] += m_state->zbase;
		//	vert[i].p[0] += reinterpret_cast<float&>(m_state->m_zeusbase[0x63]);
		//else {
			int shift;
			shift = 1024 >> m_state->m_zeusbase[0x6c];
			vert[i].p[0] += shift;
		//	//float zScale = reinterpret_cast<float&>(m_state->m_zeusbase[0x63]);
		//	//vert[i].p[0] += zScale;
		//}

		vert[i].p[2] += (texdata >> 16) << 2;
		vert[i].p[1] *= 256.0f / 4.0f;
		vert[i].p[2] *= 256.0f / 4.0f;

		// back face cull using polygon normal and first vertex
		if (1 && i == 0)
		{
			int8_t normal[3];
			float rotnormal[3];

			normal[0] = databuffer[0] >> 0;
			normal[1] = databuffer[0] >> 8;
			normal[2] = databuffer[0] >> 16;

			rotnormal[0] = normal[0] * m_state->zeus_matrix[0][0] + normal[1] * m_state->zeus_matrix[0][1] + normal[2] * m_state->zeus_matrix[0][2];
			rotnormal[1] = normal[0] * m_state->zeus_matrix[1][0] + normal[1] * m_state->zeus_matrix[1][1] + normal[2] * m_state->zeus_matrix[1][2];
			rotnormal[2] = normal[0] * m_state->zeus_matrix[2][0] + normal[1] * m_state->zeus_matrix[2][1] + normal[2] * m_state->zeus_matrix[2][2];

			float dot = rotnormal[0] * vert[0].x + rotnormal[1] * vert[0].y + rotnormal[2] * vert[0].p[0];

			if (dot >= 0)
				return;
		}


		if (logextra & logit)
		{
			m_state->logerror("\t\t(%f,%f,%f) (%02X,%02X)\n",
				(double)vert[i].x, (double)vert[i].y, (double)vert[i].p[0],
				(int)(vert[i].p[1] / 256.0f), (int)(vert[i].p[2] / 256.0f));
		}
	}
	if (logextra & logit && m_state->zeus_quad_size == 14) {
		m_state->logerror("unknown: int16: %d %d %d %d %d %d %d %d float: %f %f %f %f\n",
			unknown[0], unknown[1], unknown[2], unknown[3], unknown[4], unknown[5], unknown[6], unknown[7],
			unknownFloat[0], unknownFloat[1], unknownFloat[2], unknownFloat[3]);
	}
	bool enable_perspective = true; // !(m_state->m_renderRegs[0x14] & 0x2);
	float clipVal = enable_perspective ? 1.0f / 512.0f / 4.0f : 0.0f;
	//float clipVal = m_state->m_zbufmin / 4096.0f;
	numverts = this->zclip_if_less(4, &vert[0], &clipvert[0], 4, clipVal);
	if (numverts < 3)
		return;

	float xOrigin = reinterpret_cast<float&>(m_state->m_zeusbase[0x6a]);
	float yOrigin = reinterpret_cast<float&>(m_state->m_zeusbase[0x6b]);

	float oozBase = (m_state->m_atlantis) ? 1024.0f : (m_state->m_thegrid) ? 512.0f : 512.0f;
	//oozBase = 1 << m_state->m_zeusbase[0x6c];
	maxx = maxy = -1000.0f;
	for (i = 0; i < numverts; i++)
	{
		// mwskinsa has R14=0x40a1 for tips box which has z=0
		//if (!(m_state->m_renderRegs[0x14] & 0x1)) {
		if (enable_perspective) {
			// 412.0f here works for crusnexo
			// 1024.0f works for mwskinsa
			 float ooz = oozBase / clipvert[i].p[0];
			//float ooz = 1024.0f / clipvert[i].p[0];
			//float ooz = float(1 << m_state->m_zeusbase[0x6c]) / clipvert[i].p[0];
			 clipvert[i].x *= ooz;
			 clipvert[i].y *= ooz;
		}

		if (1) {
			//clipvert[i].x += 256.5f / 1.0f;
			//clipvert[i].y += 200.5f / 1.0f;
			clipvert[i].x += xOrigin;
			clipvert[i].y += yOrigin;
		}

		clipvert[i].p[0] *= 65536.0f * 16.0f;

		maxx = std::max(maxx, clipvert[i].x);
		maxy = std::max(maxy, clipvert[i].y);
		if (logextra & logit)
			m_state->logerror("\t\t\tTranslated=(%f,%f)\n", (double)clipvert[i].x, (double)clipvert[i].y);
	}
	for (i = 0; i < numverts; i++)
	{
		if (clipvert[i].x == maxx)
			clipvert[i].x += 0.0005f;
		if (clipvert[i].y == maxy)
			clipvert[i].y += 0.0005f;
	}

	zeus2_poly_extra_data& extra = this->object_data_alloc();
	int texmode = texdata & 0xffff;
	// 0x014d == atlantis initial screen and scoreboard background
	//if (texmode != 0x014D) return;
	// Just a guess but seems to work
	//extra.texwidth = 2 << ((texmode >> 2) & 7);
	extra.texwidth = 0x20 << ((texmode >> 2) & 3);
	//extra.texwidth = 0x2 << ((texmode >> 2) & 0xf);

	//switch (texmode)
	//{
	//case 0x14d:     // atlantis
	//case 0x18e:     // atlantis
	//case 0x01d:     /* crusnexo: RHS of score bar */
	//case 0x05d:     /* crusnexo: background, road */
	//case 0x0dd:     /* crusnexo: license plate letters */
	//case 0x11d:     /* crusnexo: LHS of score bar */
	//case 0x15d:     /* crusnexo */
	//case 0x85d:     /* crusnexo */
	//case 0x95d:     /* crusnexo */
	//case 0xc1d:     /* crusnexo */
	//case 0xc5d:     /* crusnexo */
	//  extra.texwidth = 256;
	//  break;

	//case 0x18a:     // atlantis
	//case 0x059:     /* crusnexo */
	//case 0x0d9:     /* crusnexo */
	//case 0x119:     /* crusnexo: license plates */
	//case 0x159:     /* crusnexo */
	//  extra.texwidth = 128;
	//  break;

	//case 0x055:     /* crusnexo */
	//case 0x145:     // atlantis
	//case 0x155:     /* crusnexo */
	//  extra.texwidth = 64;
	//  break;

	//case 0x000:     // thegrid guess
	//case 0x120:     // thegrid guess
	//case 0x140:     // atlantis
	//case 0x141:     // atlantis
	//  extra.texwidth = 32;
	//  break;

	//default:
	//{
	//  static uint8_t hits[0x10000];
	//  if (!hits[(texdata & 0xffff)])
	//  {
	//      hits[(texdata & 0xffff)] = 1;
	//      printf("texMode = %04X\n", (texdata & 0xffff));
	//  }
	//  break;
	//}
	//}

	extra.solidcolor = 0;//m_zeusbase[0x00] & 0x7fff;
	extra.zbufmin = m_state->m_zbufmin;
	extra.alpha = 0;//m_zeusbase[0x4e];
	extra.transcolor = 0x100; // !(texmode & 100) ? 0 : 0x100;
	extra.texbase = WAVERAM_BLOCK0_EXT(m_state->zeus_texbase);
	extra.palbase = m_state->waveram0_ptr_from_expanded_addr(m_state->m_zeusbase[0x41]);
	//extra.depth_test_enable = !(m_state->m_renderRegs[0x40] & 0x020000);
	// crusnexo text is R14=0x4062
	extra.depth_test_enable = !(m_state->m_renderRegs[0x14] & 0x000020);
	extra.depth_min_enable = false; // !(m_state->m_renderRegs[0x14] & 0x000040);
	//extra.depth_test_enable = !(m_state->m_renderRegs[0x40] & 0x000002);
	//extra.depth_test_enable = true; // (texmode & 0x0010);
	extra.depth_write_enable = true;
	extra.get_texel = (m_state->m_palSize==256) ? m_state->get_texel_8bit : m_state->get_texel_4bit;
	// Note: Before being converted to the "poly.h" interface, this used to call the polylgcy function
	//       poly_render_quad_fan.  The behavior seems to be the same as it once was after a few short
	//       tests, but the (numverts == 5) statement below may actually be a quad fan instead of a 5-sided
	//       polygon.
	if (numverts == 3)
		render_triangle(m_state->zeus_cliprect, render_delegate(&zeus2_renderer::render_poly_8bit, this), 4, clipvert[0], clipvert[1], clipvert[2]);
	else if (numverts == 4)
		render_polygon<4>(m_state->zeus_cliprect, render_delegate(&zeus2_renderer::render_poly_8bit, this), 4, clipvert);
	else if (numverts == 5)
		render_polygon<5>(m_state->zeus_cliprect, render_delegate(&zeus2_renderer::render_poly_8bit, this), 4, clipvert);
}



/*************************************
*  Rasterizers
*************************************/

void zeus2_renderer::render_poly_8bit(int32_t scanline, const extent_t& extent, const zeus2_poly_extra_data& object, int threadid)
{
	int32_t curz = extent.param[0].start;
	int32_t curu = extent.param[1].start;
	int32_t curv = extent.param[2].start;
	//  int32_t curi = extent.param[3].start;
	int32_t dzdx = extent.param[0].dpdx;
	int32_t dudx = extent.param[1].dpdx;
	int32_t dvdx = extent.param[2].dpdx;
	//  int32_t didx = extent.param[3].dpdx;
	const void *texbase = object.texbase;
	//const void *palbase = object.palbase;
	uint16_t transcolor = object.transcolor;
	int texwidth = object.texwidth;
	int x;

	uint32_t addr = m_state->frame_addr_from_xy(0, scanline, true);
	uint32_t *depthptr = &m_state->m_frameDepth[addr];
	uint32_t *colorptr = &m_state->m_frameColor[addr];
	for (x = extent.startx; x < extent.stopx; x++)
	{
		bool depth_pass = true;
		if (object.depth_test_enable) {
			if (object.depth_min_enable && curz < object.zbufmin)
				depth_pass = false;
			if (curz < 0 || curz > depthptr[x])
				depth_pass = false;
			else if (object.depth_write_enable)
				depthptr[x] = curz; // Should limit to 24 bits
		}
		if (depth_pass) {
			int u0 = (curu >> 8);// & (texwidth - 1);
			int v0 = (curv >> 8);// & 255;
			int u1 = (u0 + 1);
			int v1 = (v0 + 1);
			uint8_t texel0 = object.get_texel(texbase, v0, u0, texwidth);
			//if (texel0 == object.transcolor)
			//	continue;
			uint8_t texel1 = object.get_texel(texbase, v0, u1, texwidth);
			uint8_t texel2 = object.get_texel(texbase, v1, u0, texwidth);
			uint8_t texel3 = object.get_texel(texbase, v1, u1, texwidth);
			if (texel0 != transcolor)
			{
				uint32_t color0 = m_state->m_pal_table[texel0];
				uint32_t color1 = m_state->m_pal_table[texel1];
				uint32_t color2 = m_state->m_pal_table[texel2];
				uint32_t color3 = m_state->m_pal_table[texel3];
				rgb_t filtered = rgbaint_t::bilinear_filter(color0, color1, color2, color3, curu, curv);
				colorptr[x] = filtered;
			}
		}
		curz += dzdx;
		curu += dudx;
		curv += dvdx;
		//      curi += didx;
	}
}

/*************************************
 *  Debugging tools
 *************************************/

void zeus2_device::log_fifo_command(const uint32_t *data, int numwords, const char *suffix)
{
	int wordnum;
	logerror("Zeus cmd %02X :", data[0] >> 24);
	for (wordnum = 0; wordnum < numwords; wordnum++)
		logerror(" %08X", data[wordnum]);
	logerror("%s", suffix);
}

void zeus2_device::print_fifo_command(const uint32_t *data, int numwords, const char *suffix)
{
	int wordnum;
	printf("Zeus cmd %02X :", data[0] >> 24);
	for (wordnum = 0; wordnum < numwords; wordnum++)
		printf(" %08X", data[wordnum]);
	printf("%s", suffix);
}

void zeus2_device::log_render_info(uint32_t texdata)
{
	logerror("-- RMode0 R40 = %08X texdata = %08X", m_renderRegs[0x40], texdata);
	logerror("\n-- RMode1 ");
	for (int i = 1; i <= 0x9; ++i)
		logerror(" R%02X=%06X", i, m_renderRegs[i]);
	for (int i = 0xa; i <= 0x15; ++i)
		logerror(" R%02X=%06X", i, m_renderRegs[i]);
	logerror("\n-- RMode2 ");
	for (int i = 0x63; i <= 0x6f; ++i)
		logerror(" %02X=%08X", i, m_zeusbase[i]);
	logerror("\n");
}
