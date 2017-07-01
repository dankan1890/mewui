// license:BSD-3-Clause
// copyright-holders:Curt Coder, F. Ulivi
/*

    HP 9845

    http://www.hp9845.net/

*/
// **************************
// Driver for HP 9845B system
// **************************
//
// What's in:
// - Emulation of both 5061-3001 CPUs
// - LPU & PPU ROMs
// - LPU & PPU RAMs
// - Text mode screen
// - Graphic screen
// - Keyboard
// - T15 tape drive
// - Software list to load optional ROMs
// - Beeper
// - Correct character generator ROMs (a huge "thank you" to Ansgar Kueckes for the dumps!)
// What's not yet in:
// - Better naming of tape drive image (it's now "magt", should be "t15")
// - Better documentation of this file
// What's wrong:
// - Speed, as usual

#include "emu.h"
#include "includes/hp9845.h"
#include "softlist.h"
#include "bus/hp_optroms/hp_optrom.h"

#define BIT_MASK(n) (1U << (n))

// Macros to clear/set single bits
#define BIT_CLR(w , n)  ((w) &= ~BIT_MASK(n))
#define BIT_SET(w , n)  ((w) |= BIT_MASK(n))

// Base address of video buffer
#define VIDEO_BUFFER_BASE       0x17000

// For test "B" of alpha video to succeed this must be < 234
// Basically "B" test is designed to intentionally prevent line buffer to be filled so that display is blanked
// from 2nd row on. This in turn prevents "BAD" text to be visible on screen.
#define MAX_WORD_PER_ROW        220

// Constants of alpha video
#define VIDEO_PIXEL_CLOCK       20849400
#define VIDEO_CHAR_WIDTH        9
#define VIDEO_CHAR_HEIGHT       15
#define VIDEO_CHAR_COLUMNS      80
#define VIDEO_CHAR_TOTAL        99
#define VIDEO_CHAR_ROWS         25
#define VIDEO_ROWS_TOTAL        26
#define VIDEO_HBSTART           (VIDEO_CHAR_WIDTH * VIDEO_CHAR_COLUMNS)
#define VIDEO_HTOTAL            (VIDEO_CHAR_WIDTH * VIDEO_CHAR_TOTAL)
#define VIDEO_VTOTAL            (VIDEO_CHAR_HEIGHT * VIDEO_ROWS_TOTAL)
#define VIDEO_ACTIVE_SCANLINES  (VIDEO_CHAR_HEIGHT * VIDEO_CHAR_ROWS)

// Constants of graphic video
// Pixel clock is 20.8494 MHz (the same as alpha video)
// Horizontal counter counts in [1..727] range
// Vertical counter counts in [34..511] range
#define GVIDEO_HTOTAL           727
#define GVIDEO_HCNT_OFF         1       // Actual start value of h counter
#define GVIDEO_HBEND            (69 - GVIDEO_HCNT_OFF)
#define GVIDEO_HPIXELS          560
#define GVIDEO_HBSTART          (GVIDEO_HBEND + GVIDEO_HPIXELS)
#define GVIDEO_VTOTAL           478
#define GVIDEO_VCNT_OFF         34      // Actual start value of v counter
#define GVIDEO_VBEND            (50 - GVIDEO_VCNT_OFF)
#define GVIDEO_VPIXELS          455
#define GVIDEO_VBSTART          (GVIDEO_VBEND + GVIDEO_VPIXELS)
#define GVIDEO_MEM_SIZE         16384
#define GVIDEO_ADDR_MASK        (GVIDEO_MEM_SIZE - 1)
#define GVIDEO_PA               13

#define T15_PA                  15

#define KEY_SCAN_OSCILLATOR     327680

class hp9845_state : public driver_device
{
public:
	hp9845_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag)
	{ }

	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
};

static INPUT_PORTS_START( hp9845 )
INPUT_PORTS_END

uint32_t hp9845_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	return 0;
}

static INPUT_PORTS_START(hp9845b)
		// Keyboard is arranged in a 8 x 16 matrix. Of the 128 possible positions, 118 are used.
	// Keys are mapped on bit b of KEYn
	// where b = (row & 1) << 4 + column, n = row >> 1
	// column = [0..15]
	// row = [0..7]
	PORT_START("KEY0")
	PORT_BIT(BIT_MASK(0)  , IP_ACTIVE_HIGH , IPT_UNUSED)    // N/U
	PORT_BIT(BIT_MASK(1)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("Print All")  // Print All
	PORT_BIT(BIT_MASK(2)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP+")        // KP +
	PORT_BIT(BIT_MASK(3)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP,")        // KP ,
	PORT_BIT(BIT_MASK(4)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP.")        // KP .
		PORT_BIT(BIT_MASK(5)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP0")        // KP 0
	PORT_BIT(BIT_MASK(6)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_F12)  PORT_NAME("Execute")    // Execute
	PORT_BIT(BIT_MASK(7)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_F11)  PORT_NAME("Cont")       // Cont
	PORT_BIT(BIT_MASK(8)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))        // Right
	PORT_BIT(BIT_MASK(9)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')  // Space
	PORT_BIT(BIT_MASK(10)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/') PORT_CHAR('?')  // /
	PORT_BIT(BIT_MASK(11)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',') PORT_CHAR('<')  // <
	PORT_BIT(BIT_MASK(12)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_N)     PORT_CHAR('n') PORT_CHAR('N')  // N
	PORT_BIT(BIT_MASK(13)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_V)     PORT_CHAR('v') PORT_CHAR('V')  // V
	PORT_BIT(BIT_MASK(14)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_X)     PORT_CHAR('x') PORT_CHAR('X')  // X
	PORT_BIT(BIT_MASK(15)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_LSHIFT)    PORT_CHAR(UCHAR_SHIFT_1)   // Shift
	PORT_BIT(BIT_MASK(16)  , IP_ACTIVE_HIGH , IPT_UNUSED)   // N/U
	PORT_BIT(BIT_MASK(17)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("Auto start") // Auto Start
	PORT_BIT(BIT_MASK(18)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("KP-")        // KP -
	PORT_BIT(BIT_MASK(19)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("KP3")        // KP 3
	PORT_BIT(BIT_MASK(20)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("KP2")        // KP 2
	PORT_BIT(BIT_MASK(21)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("KP1")        // KP 1
	PORT_BIT(BIT_MASK(22)  , IP_ACTIVE_HIGH , IPT_UNUSED)   // N/U
	PORT_BIT(BIT_MASK(23)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))  // Left
	PORT_BIT(BIT_MASK(24)  , IP_ACTIVE_HIGH , IPT_UNUSED)   // Repeat
	PORT_BIT(BIT_MASK(25)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))  // Down
	PORT_BIT(BIT_MASK(26)  , IP_ACTIVE_HIGH , IPT_UNUSED)   // N/U
	PORT_BIT(BIT_MASK(27)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP) PORT_CHAR('.') PORT_CHAR('>')   // >
	PORT_BIT(BIT_MASK(28)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_M)     PORT_CHAR('m') PORT_CHAR('M')  // M
	PORT_BIT(BIT_MASK(29)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_B)     PORT_CHAR('b') PORT_CHAR('B')  // B
	PORT_BIT(BIT_MASK(30)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_C)     PORT_CHAR('c') PORT_CHAR('C')  // C
	PORT_BIT(BIT_MASK(31)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_Z)     PORT_CHAR('z') PORT_CHAR('Z')  // Z

	PORT_START("KEY1")
	PORT_BIT(BIT_MASK(0)  , IP_ACTIVE_HIGH , IPT_UNUSED)    // N/U
	PORT_BIT(BIT_MASK(1)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_INSERT)       PORT_NAME("INSCHAR")    // Ins Char
	PORT_BIT(BIT_MASK(2)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP*")        // KP *
	PORT_BIT(BIT_MASK(3)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP6")        // KP 6
	PORT_BIT(BIT_MASK(4)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP5")        // KP 5
	PORT_BIT(BIT_MASK(5)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP4")        // KP 4
	PORT_BIT(BIT_MASK(6)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP=")        // KP =
	PORT_BIT(BIT_MASK(7)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_F10)  PORT_NAME("Pause")      // Pause
	PORT_BIT(BIT_MASK(8)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_UP)   PORT_CHAR(UCHAR_MAMEKEY(UP))    // Up
	PORT_BIT(BIT_MASK(9)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_ENTER)        PORT_CHAR(13)   // Store
	PORT_BIT(BIT_MASK(10)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)     PORT_CHAR(';') PORT_CHAR(':')      // :
	PORT_BIT(BIT_MASK(11)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_K)     PORT_CHAR('k') PORT_CHAR('K')  // K
	PORT_BIT(BIT_MASK(12)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_H)     PORT_CHAR('h') PORT_CHAR('H')  // H
	PORT_BIT(BIT_MASK(13)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F)     PORT_CHAR('f') PORT_CHAR('F')  // F
	PORT_BIT(BIT_MASK(14)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_S)     PORT_CHAR('s') PORT_CHAR('S')  // S
	PORT_BIT(BIT_MASK(15)  , IP_ACTIVE_HIGH , IPT_UNUSED)   // N/U
	PORT_BIT(BIT_MASK(16)  , IP_ACTIVE_HIGH , IPT_UNUSED)   // N/U
	PORT_BIT(BIT_MASK(17)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("INSLN")      // Ins Ln
	PORT_BIT(BIT_MASK(18)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("KP/")        // KP /
	PORT_BIT(BIT_MASK(19)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("KP9")        // KP 9
	PORT_BIT(BIT_MASK(20)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("KP8")        // KP 8
	PORT_BIT(BIT_MASK(21)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("KP7")        // KP 7
	PORT_BIT(BIT_MASK(22)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("Result")     // Result
	PORT_BIT(BIT_MASK(23)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F9)   PORT_NAME("Run")        // Run
	PORT_BIT(BIT_MASK(24)  , IP_ACTIVE_HIGH , IPT_UNUSED)   // N/U
	PORT_BIT(BIT_MASK(25)  , IP_ACTIVE_HIGH , IPT_UNUSED)   // N/U
	PORT_BIT(BIT_MASK(26)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR('\'') PORT_CHAR('"') // "
	PORT_BIT(BIT_MASK(27)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_L)     PORT_CHAR('l') PORT_CHAR('L')  // L
	PORT_BIT(BIT_MASK(28)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_J)     PORT_CHAR('j') PORT_CHAR('J')  // J
	PORT_BIT(BIT_MASK(29)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_G)     PORT_CHAR('g') PORT_CHAR('G')  // G
	PORT_BIT(BIT_MASK(30)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_D)     PORT_CHAR('d') PORT_CHAR('D')  // D
	PORT_BIT(BIT_MASK(31)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_A)     PORT_CHAR('a') PORT_CHAR('A')  // A

	PORT_START("KEY2")
	PORT_BIT(BIT_MASK(0)  , IP_ACTIVE_HIGH , IPT_UNUSED)    // N/U
	PORT_BIT(BIT_MASK(1)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("DELLN")      // Del Ln
	PORT_BIT(BIT_MASK(2)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP^")        // KP ^
	PORT_BIT(BIT_MASK(3)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP)")        // KP )
	PORT_BIT(BIT_MASK(4)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KP(")        // KP (
	PORT_BIT(BIT_MASK(5)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("KPE")        // KP E
	PORT_BIT(BIT_MASK(6)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("Clear line") // Clear Line
	PORT_BIT(BIT_MASK(7)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_F8)   PORT_NAME("Stop")       // Stop
	PORT_BIT(BIT_MASK(8)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\') PORT_CHAR('|')      // |
	PORT_BIT(BIT_MASK(9)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_CODE(KEYCODE_CLOSEBRACE)   PORT_CHAR(']') PORT_CHAR('}')   // ]
	PORT_BIT(BIT_MASK(10)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_P)     PORT_CHAR('p') PORT_CHAR('P')  // P
	PORT_BIT(BIT_MASK(11)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_I)     PORT_CHAR('i') PORT_CHAR('I')  // I
	PORT_BIT(BIT_MASK(12)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)     PORT_CHAR('y') PORT_CHAR('Y')  // Y
	PORT_BIT(BIT_MASK(13)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_R)     PORT_CHAR('r') PORT_CHAR('R')  // R
	PORT_BIT(BIT_MASK(14)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_W)     PORT_CHAR('w') PORT_CHAR('W')  // W
	PORT_BIT(BIT_MASK(15)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_LCONTROL)  PORT_CHAR(UCHAR_SHIFT_2)   // Control
	PORT_BIT(BIT_MASK(16)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("Typwtr")     // Typwtr
	PORT_BIT(BIT_MASK(17)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_DEL)  PORT_NAME("DELCHAR")    // Del Char
	PORT_BIT(BIT_MASK(18)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_PGDN) PORT_NAME("ROLLDOWN")   // Roll down
	PORT_BIT(BIT_MASK(19)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_PGUP) PORT_NAME("ROLLUP")     // Roll up
	PORT_BIT(BIT_MASK(20)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_HOME) PORT_NAME("HOME")       // Home
	PORT_BIT(BIT_MASK(21)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("Clr to end") // Clr to end
	PORT_BIT(BIT_MASK(22)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("Clear")      // Clear
	PORT_BIT(BIT_MASK(23)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_TILDE)     PORT_CHAR('`') PORT_CHAR('~')      // ~
	PORT_BIT(BIT_MASK(24)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)       // BS
	PORT_BIT(BIT_MASK(25)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)    PORT_CHAR('=') PORT_CHAR('+')      // +
	PORT_BIT(BIT_MASK(26)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('[') PORT_CHAR('{')      // [
	PORT_BIT(BIT_MASK(27)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_O)     PORT_CHAR('o') PORT_CHAR('O')  // O
	PORT_BIT(BIT_MASK(28)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_U)     PORT_CHAR('u') PORT_CHAR('U')  // U
	PORT_BIT(BIT_MASK(29)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_T)     PORT_CHAR('t') PORT_CHAR('T')  // T
	PORT_BIT(BIT_MASK(30)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_E)     PORT_CHAR('e') PORT_CHAR('E')  // E
	PORT_BIT(BIT_MASK(31)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_Q)     PORT_CHAR('q') PORT_CHAR('Q')  // Q

	PORT_START("KEY3")
	PORT_BIT(BIT_MASK(0)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("Tab set")    // Tab set
	PORT_BIT(BIT_MASK(1)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("Recall")     // Recall
	PORT_BIT(BIT_MASK(2)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("K15")        // K15
	PORT_BIT(BIT_MASK(3)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("K14")        // K14
	PORT_BIT(BIT_MASK(4)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("K13")        // K13
	PORT_BIT(BIT_MASK(5)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("K12")        // K12
	PORT_BIT(BIT_MASK(6)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("K11")        // K11
	PORT_BIT(BIT_MASK(7)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("K10")        // K10
		PORT_BIT(BIT_MASK(8)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("K9")         // K9
	PORT_BIT(BIT_MASK(9)  , IP_ACTIVE_HIGH , IPT_KEYBOARD)  PORT_NAME("K8")         // K8
	PORT_BIT(BIT_MASK(10)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_0)     PORT_CHAR('0') // 0
	PORT_BIT(BIT_MASK(11)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_8)     PORT_CHAR('8') PORT_CHAR('(')  // 8
	PORT_BIT(BIT_MASK(12)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_6)     PORT_CHAR('6') PORT_CHAR('&')  // 6
	PORT_BIT(BIT_MASK(13)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_4)     PORT_CHAR('4') PORT_CHAR('$')  // 4
	PORT_BIT(BIT_MASK(14)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_2)     PORT_CHAR('2') PORT_CHAR('"')  // 2
	PORT_BIT(BIT_MASK(15)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_TAB)   PORT_CHAR('\t')        // Tab
	PORT_BIT(BIT_MASK(16)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("Tab clr")    // Tab clr
		PORT_BIT(BIT_MASK(17)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_NAME("Step")  // Step
	PORT_BIT(BIT_MASK(18)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F7)   PORT_NAME("K7") // K7
	PORT_BIT(BIT_MASK(19)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F6)   PORT_NAME("K6") // K6
	PORT_BIT(BIT_MASK(20)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F5)   PORT_NAME("K5") // K5
		PORT_BIT(BIT_MASK(21)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F4)   PORT_NAME("K4") // K4
		PORT_BIT(BIT_MASK(22)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F3)   PORT_NAME("K3") // K3
	PORT_BIT(BIT_MASK(23)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F2)   PORT_NAME("K2") // K2
	PORT_BIT(BIT_MASK(24)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_F1)   PORT_NAME("K1") // K1
	PORT_BIT(BIT_MASK(25)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC)  PORT_NAME("K0") // K0
	PORT_BIT(BIT_MASK(26)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)     PORT_CHAR('-') PORT_CHAR('_')      // _
	PORT_BIT(BIT_MASK(27)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_9)     PORT_CHAR('9') PORT_CHAR(')')  // 9
	PORT_BIT(BIT_MASK(28)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_7)     PORT_CHAR('7') PORT_CHAR('\'') // 7
	PORT_BIT(BIT_MASK(29)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_5)     PORT_CHAR('5') PORT_CHAR('%')  // 5
	PORT_BIT(BIT_MASK(30)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_3)     PORT_CHAR('3') PORT_CHAR('#')  // 3
	PORT_BIT(BIT_MASK(31)  , IP_ACTIVE_HIGH , IPT_KEYBOARD) PORT_CODE(KEYCODE_1)     PORT_CHAR('1') PORT_CHAR('!')  // 1

INPUT_PORTS_END

hp9845b_state::hp9845b_state(const machine_config &mconfig, device_type type, const char *tag) :
			  driver_device(mconfig, type, tag),
			  m_lpu(*this , "lpu"),
			  m_ppu(*this , "ppu"),
			  m_screen(*this , "screen"),
			  m_palette(*this , "palette"),
			  m_gv_timer(*this , "gv_timer"),
			  m_io_key0(*this , "KEY0"),
			  m_io_key1(*this , "KEY1"),
			  m_io_key2(*this , "KEY2"),
			  m_io_key3(*this , "KEY3"),
			  m_t15(*this , "t15"),
			  m_beeper(*this , "beeper"),
			  m_beep_timer(*this , "beep_timer"),
			  m_io_slot0(*this , "slot0")
{
}

uint32_t hp9845b_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
		if (m_graphic_sel) {
				copybitmap(bitmap, m_bitmap, 0, 0, GVIDEO_HBEND, GVIDEO_VBEND, cliprect);
		} else {
				copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);
		}

	return 0;
}

void hp9845b_state::machine_start()
{
	machine().first_screen()->register_screen_bitmap(m_bitmap);

		m_chargen = memregion("chargen")->base();

	m_optional_chargen = memregion("optional_chargen")->base();

				m_graphic_mem.resize(GVIDEO_MEM_SIZE);
}

void hp9845b_state::device_reset()
{
	// FLG & STS are to be reset before sub-devices,
	// because the latter may set the former in their own reset functions
	m_flg_status = 0;
	m_sts_status = 0;
}

void hp9845b_state::machine_reset()
{
	m_lpu->halt_w(1);
	m_ppu->halt_w(0);

	// Some sensible defaults
	m_video_mar = VIDEO_BUFFER_BASE;
	m_video_load_mar = false;
	m_video_first_mar = false;
	m_video_byte_idx = false;
	m_video_attr = 0;
	m_video_buff_idx = false;
	m_video_blanked = false;
	m_graphic_sel = false;
	m_gv_fsm_state = GV_STAT_RESET;
	m_gv_int_en = false;
	m_gv_dma_en = false;

	m_irl_pending = 0;
	m_irh_pending = 0;
	m_pa = 0;

	sts_w(GVIDEO_PA , true);
	update_graphic_bits();

	memset(&m_kb_state[ 0 ] , 0 , sizeof(m_kb_state));
	m_kb_scancode = 0x7f;
	m_kb_status = 0;

	m_beeper->set_state(0);

	logerror("STS=%04x FLG=%04x\n" , m_sts_status , m_flg_status);
}

void hp9845b_state::set_video_mar(uint16_t mar)
{
		m_video_mar = (mar & 0xfff) | VIDEO_BUFFER_BASE;
}

void hp9845b_state::video_fill_buff(bool buff_idx)
{
		unsigned char_idx = 0;
		unsigned iters = 0;
		uint8_t byte;
		address_space& prog_space = m_ppu->space(AS_PROGRAM);

		m_video_buff[ buff_idx ].full = false;

		while (1) {
				if (!m_video_byte_idx) {
						if (iters++ >= MAX_WORD_PER_ROW) {
								// Limit on accesses per row reached
								break;
						}
						m_video_word = prog_space.read_word(m_video_mar << 1);
						if (m_video_load_mar) {
								// Load new address into MAR after start of a new frame or NWA instruction
														if (m_video_first_mar) {
																set_graphic_mode(!BIT(m_video_word , 15));
																m_video_first_mar = false;
														}
								set_video_mar(~m_video_word);
								m_video_load_mar = false;
								continue;
						} else {
								// Read normal word from frame buffer, start parsing at MSB
								set_video_mar(m_video_mar + 1);
								byte = (uint8_t)(m_video_word >> 8);
								m_video_byte_idx = true;
						}
				} else {
						// Parse LSB
						byte = (uint8_t)(m_video_word & 0xff);
						m_video_byte_idx = false;
				}
				if ((byte & 0xc0) == 0x80) {
						// Attribute command
						m_video_attr = byte & 0x1f;
				} else if ((byte & 0xc1) == 0xc0) {
						// New Word Address (NWA)
						m_video_load_mar = true;
						m_video_byte_idx = false;
				} else if ((byte & 0xc1) == 0xc1) {
						// End of line (EOL)
						// Fill rest of buffer with spaces
						memset(&m_video_buff[ buff_idx ].chars[ char_idx ] , 0x20 , 80 - char_idx);
						memset(&m_video_buff[ buff_idx ].attrs[ char_idx ] , m_video_attr , 80 - char_idx);
						m_video_buff[ buff_idx ].full = true;
						break;
				} else {
						// Normal character
						m_video_buff[ buff_idx ].chars[ char_idx ] = byte;
						m_video_buff[ buff_idx ].attrs[ char_idx ] = m_video_attr;
						char_idx++;
						if (char_idx == 80) {
								m_video_buff[ buff_idx ].full = true;
								break;
						}
				}
		}
}

void hp9845b_state::video_render_buff(unsigned video_scanline , unsigned line_in_row, bool buff_idx)
{
		if (!m_video_buff[ buff_idx ].full) {
				m_video_blanked = true;
		}

				const pen_t *pen = m_palette->pens();

		if (m_video_blanked) {
						// Blank scanline
						for (unsigned i = 0; i < VIDEO_HBSTART; i++) {
								m_bitmap.pix32(video_scanline , i) = pen[ 0 ];
						}
		} else {
				bool cursor_line = line_in_row == 12;
				bool ul_line = line_in_row == 14;
								unsigned video_frame = (unsigned)m_screen->frame_number();
				bool cursor_blink = BIT(video_frame , 3);
				bool char_blink = BIT(video_frame , 4);

				for (unsigned i = 0; i < 80; i++) {
						uint8_t charcode = m_video_buff[ buff_idx ].chars[ i ];
						uint8_t attrs = m_video_buff[ buff_idx ].attrs[ i ];
						uint16_t chrgen_addr = ((uint16_t)(charcode ^ 0x7f) << 4) | line_in_row;
						uint16_t pixels;

						if ((ul_line && BIT(attrs , 3)) ||
							(cursor_line && cursor_blink && BIT(attrs , 0))) {
								pixels = ~0;
						} else if (char_blink && BIT(attrs , 2)) {
								pixels = 0;
						} else if (BIT(attrs , 4)) {
							pixels = (uint16_t)(m_optional_chargen[ chrgen_addr ] & 0x7f) << 1;
						} else {
							pixels = (uint16_t)(m_chargen[ chrgen_addr ] & 0x7f) << 1;
						}

						if (BIT(attrs , 1)) {
								pixels = ~pixels;
						}

						for (unsigned j = 0; j < 9; j++) {
								bool pixel = (pixels & (1U << j)) != 0;

								m_bitmap.pix32(video_scanline , i * 9 + j) = pen[ pixel ? 1 : 0 ];
						}
				}
		}
}

TIMER_DEVICE_CALLBACK_MEMBER(hp9845b_state::scanline_timer)
{
		unsigned video_scanline = param;

		if (m_graphic_sel) {
				if (video_scanline >= GVIDEO_VBEND && video_scanline < GVIDEO_VBSTART) {
						graphic_video_render(video_scanline);
				}
		} else if (video_scanline < VIDEO_ACTIVE_SCANLINES) {
				unsigned row = video_scanline / VIDEO_CHAR_HEIGHT;
				unsigned line_in_row = video_scanline - row * VIDEO_CHAR_HEIGHT;

				if (line_in_row == 0) {
						// Start of new row, swap buffers
						m_video_buff_idx = !m_video_buff_idx;
						video_fill_buff(!m_video_buff_idx);
				}

				video_render_buff(video_scanline , line_in_row , m_video_buff_idx);
		}
}

TIMER_DEVICE_CALLBACK_MEMBER(hp9845b_state::gv_timer)
{
		advance_gv_fsm(false , false);
}

void hp9845b_state::vblank_w(screen_device &screen, bool state)
{
				// VBlank signal is fed into HALT flag of PPU
				m_ppu->halt_w(state);

				if (state) {
								// Start of V blank
								set_video_mar(0);
								m_video_load_mar = true;
								m_video_first_mar = true;
								m_video_byte_idx = false;
								m_video_blanked = false;
								m_video_buff_idx = !m_video_buff_idx;
								video_fill_buff(!m_video_buff_idx);
				}
}

void hp9845b_state::set_graphic_mode(bool graphic)
{
		if (graphic != m_graphic_sel) {
				m_graphic_sel = graphic;
				logerror("GS=%d\n" , graphic);
				if (m_graphic_sel) {
						m_screen->configure(GVIDEO_HTOTAL , GVIDEO_VTOTAL , rectangle(GVIDEO_HBEND , GVIDEO_HBSTART - 1 , GVIDEO_VBEND , GVIDEO_VBSTART - 1) , HZ_TO_ATTOSECONDS(VIDEO_PIXEL_CLOCK) * GVIDEO_HTOTAL * GVIDEO_VTOTAL);
				} else {
						m_screen->configure(VIDEO_HTOTAL , VIDEO_VTOTAL , rectangle(0 , VIDEO_HBSTART - 1 , 0 , VIDEO_ACTIVE_SCANLINES - 1) , HZ_TO_ATTOSECONDS(VIDEO_PIXEL_CLOCK) * VIDEO_HTOTAL * VIDEO_VTOTAL);
				}
		}
}

READ16_MEMBER(hp9845b_state::graphic_r)
{
		uint16_t res = 0;

		switch (offset) {
		case 0:
				// R4: data register
				res = m_gv_data_r;
				advance_gv_fsm(true , false);
				break;

		case 1:
				// R5: status register
				if (m_gv_int_en) {
						BIT_SET(res, 7);
				}
				if (m_gv_dma_en) {
						BIT_SET(res, 6);
				}
				BIT_SET(res, 5);
				break;

		case 2:
				// R6: data register with DMA TC
				m_gv_dma_en = false;
				res = m_gv_data_r;
				advance_gv_fsm(true , false);
				break;

		case 3:
				// R7: not mapped
				break;
		}

		//logerror("rd gv R%u = %04x\n", 4 + offset , res);

		return res;
}

WRITE16_MEMBER(hp9845b_state::graphic_w)
{
		//logerror("wr gv R%u = %04x\n", 4 + offset , data);

		switch (offset) {
		case 0:
				// R4: data register
				m_gv_data_w = data;
				m_gv_cursor_w = data;
				advance_gv_fsm(true , false);
				break;

		case 1:
				// R5: command register
				m_gv_cmd = (uint8_t)(data & 0xf);
				if (BIT(data , 5)) {
						m_gv_fsm_state = GV_STAT_RESET;
				}
				m_gv_dma_en = BIT(data , 6) != 0;
				m_gv_int_en = BIT(data , 7) != 0;
				advance_gv_fsm(false , false);
				break;

		case 2:
				// R6: data register with DMA TC
				m_gv_dma_en = false;
				m_gv_data_w = data;
				m_gv_cursor_w = data;
				advance_gv_fsm(true , false);
				break;

		case 3:
				// R7: trigger
				advance_gv_fsm(false , true);
				break;
		}
}

attotime hp9845b_state::time_to_gv_mem_availability(void) const
{
		if (m_graphic_sel) {
				int hpos = m_screen->hpos();
				if (hpos < (34 - GVIDEO_HCNT_OFF) || hpos >= (628 - GVIDEO_HCNT_OFF)) {
						// Access to graphic memory available now
						return attotime::zero;
				} else {
						// Wait until start of hblank
						return m_screen->time_until_pos(m_screen->vpos() , 628);
				}
		} else {
				// TODO:
				return attotime::zero;
		}
}

void hp9845b_state::advance_gv_fsm(bool ds , bool trigger)
{
		bool get_out = false;

		attotime time_mem_av;

		do {
				bool act_trig = trigger || m_gv_dma_en || !BIT(m_gv_cmd , 2);

				switch (m_gv_fsm_state) {
				case GV_STAT_WAIT_DS_0:
						if ((m_gv_cmd & 0xc) == 0xc) {
								// Read command (11xx)
								m_gv_fsm_state = GV_STAT_WAIT_MEM_0;
						} else if (ds) {
								// Wait for data strobe (r/w on r4 or r6)
								m_gv_fsm_state = GV_STAT_WAIT_TRIG_0;
						} else {
								get_out = true;
						}
						break;

				case GV_STAT_WAIT_TRIG_0:
						// Wait for trigger
						if (act_trig) {
								if (BIT(m_gv_cmd , 3)) {
										// Not a cursor command
										// Load memory address
										m_gv_io_counter = ~m_gv_data_w & GVIDEO_ADDR_MASK;
										// Write commands (10xx)
										m_gv_fsm_state = GV_STAT_WAIT_DS_2;
								} else {
										// Cursor command (0xxx)
										if (BIT(m_gv_cmd , 2)) {
												// Write X cursor position (01xx)
												m_gv_cursor_x = (~m_gv_cursor_w >> 6) & 0x3ff;
												//logerror("gv x curs pos = %u\n" , m_gv_cursor_x);
										} else {
												// Write Y cursor position and type (00xx)
												m_gv_cursor_y = (~m_gv_cursor_w >> 6) & 0x1ff;
												m_gv_cursor_gc = BIT(m_gv_cmd , 1) == 0;
												m_gv_cursor_fs = BIT(m_gv_cmd , 0) != 0;
												//logerror("gv y curs pos = %u gc = %d fs = %d\n" , m_gv_cursor_y , m_gv_cursor_gc , m_gv_cursor_fs);
										}
										m_gv_fsm_state = GV_STAT_WAIT_DS_0;
								}
						} else {
								get_out = true;
						}
						break;

				case GV_STAT_WAIT_MEM_0:
						time_mem_av = time_to_gv_mem_availability();
						if (time_mem_av.is_zero()) {
								// Read a word from graphic memory
								m_gv_data_r = m_graphic_mem[ m_gv_io_counter ];
								//logerror("rd gv mem @%04x = %04x\n" , m_gv_io_counter , m_gv_data_r);
								m_gv_io_counter = (m_gv_io_counter + 1) & GVIDEO_ADDR_MASK;
								m_gv_fsm_state = GV_STAT_WAIT_DS_1;
						} else {
								m_gv_timer->adjust(time_mem_av);
								get_out = true;
						}
						break;

				case GV_STAT_WAIT_DS_1:
						if (ds) {
								m_gv_fsm_state = GV_STAT_WAIT_MEM_0;
						} else {
								get_out = true;
						}
						break;

				case GV_STAT_WAIT_DS_2:
						// Wait for data word to be written
						if (ds) {
								m_gv_fsm_state = GV_STAT_WAIT_TRIG_1;
						} else {
								get_out = true;
						}
						break;

				case GV_STAT_WAIT_TRIG_1:
						// Wait for trigger
						if (act_trig) {
								if (BIT(m_gv_cmd , 1)) {
										// Clear words (101x)
										m_gv_data_w = 0;
										m_gv_fsm_state = GV_STAT_WAIT_MEM_1;
								} else if (BIT(m_gv_cmd , 0)) {
										// Write a single pixel (1001)
										m_gv_fsm_state = GV_STAT_WAIT_MEM_2;
								} else {
										// Write words (1000)
										m_gv_fsm_state = GV_STAT_WAIT_MEM_1;
								}
						} else {
								get_out = true;
						}
						break;

				case GV_STAT_WAIT_MEM_1:
						time_mem_av = time_to_gv_mem_availability();
						if (time_mem_av.is_zero()) {
								// Write a full word to graphic memory
								//logerror("wr gv mem @%04x = %04x\n" , m_gv_io_counter , m_gv_data_w);
								m_graphic_mem[ m_gv_io_counter ] = m_gv_data_w;
								m_gv_io_counter = (m_gv_io_counter + 1) & GVIDEO_ADDR_MASK;
								m_gv_fsm_state = GV_STAT_WAIT_DS_2;
						} else {
								m_gv_timer->adjust(time_mem_av);
								get_out = true;
						}
						break;

				case GV_STAT_WAIT_MEM_2:
						time_mem_av = time_to_gv_mem_availability();
						if (time_mem_av.is_zero()) {
								// Write a single pixel to graphic memory
								//logerror("wr gv pixel @%04x:%x = %d\n" , m_gv_io_counter , m_gv_data_w & 0xf , BIT(m_gv_data_w , 15));
								uint16_t mask = 0x8000 >> (m_gv_data_w & 0xf);
								if (BIT(m_gv_data_w , 15)) {
										// Set pixel
										m_graphic_mem[ m_gv_io_counter ] |= mask;
								} else {
										// Clear pixel
										m_graphic_mem[ m_gv_io_counter ] &= ~mask;
								}
								// Not really needed
								m_gv_io_counter = (m_gv_io_counter + 1) & GVIDEO_ADDR_MASK;
								m_gv_fsm_state = GV_STAT_WAIT_DS_0;
						} else {
								m_gv_timer->adjust(time_mem_av);
								get_out = true;
						}
						break;

				default:
						logerror("Invalid state reached %d\n" , m_gv_fsm_state);
						m_gv_fsm_state = GV_STAT_RESET;
				}

				ds = false;
				trigger = false;
		} while (!get_out);

		update_graphic_bits();
}

void hp9845b_state::update_graphic_bits(void)
{
		bool gv_ready = m_gv_fsm_state == GV_STAT_WAIT_DS_0 ||
			m_gv_fsm_state == GV_STAT_WAIT_DS_1 ||
			m_gv_fsm_state == GV_STAT_WAIT_DS_2;

		flg_w(GVIDEO_PA , gv_ready);

		bool irq = m_gv_int_en && !m_gv_dma_en && gv_ready;

		irq_w(GVIDEO_PA , irq);

		bool dmar = gv_ready && m_gv_dma_en;

		m_ppu->dmar_w(dmar);
}

void hp9845b_state::graphic_video_render(unsigned video_scanline)
{
		const pen_t *pen = m_palette->pens();
		bool yc = (video_scanline + GVIDEO_VCNT_OFF) == (m_gv_cursor_y + 6);
		bool yw;
		bool blink;

		if (m_gv_cursor_fs) {
				yw = true;
				// Steady cursor
				blink = true;
		} else {
				yw = (video_scanline + GVIDEO_VCNT_OFF) >= (m_gv_cursor_y + 2) &&
						(video_scanline + GVIDEO_VCNT_OFF) <= (m_gv_cursor_y + 10);
				// Blinking cursor (frame freq. / 16)
				blink = BIT(m_screen->frame_number() , 3) != 0;
		}

		unsigned mem_idx = 36 * (video_scanline - GVIDEO_VBEND);
		for (unsigned i = 0; i < GVIDEO_HPIXELS; i += 16) {
				uint16_t word = m_graphic_mem[ mem_idx++ ];
				unsigned x = i;
				for (uint16_t mask = 0x8000; mask != 0; mask >>= 1) {
						unsigned cnt_h = x + GVIDEO_HBEND + GVIDEO_HCNT_OFF;
						bool xc = cnt_h == (m_gv_cursor_x + 6);
						bool xw = m_gv_cursor_fs || (cnt_h >= (m_gv_cursor_x + 2) && cnt_h <= (m_gv_cursor_x + 10));
						unsigned pixel;
						if (blink && ((xw && yc) || (yw && xc && m_gv_cursor_gc))) {
								// Cursor
								pixel = 2;
						} else {
								// Normal pixel
								pixel = (word & mask) != 0;
						}
						m_bitmap.pix32(video_scanline - GVIDEO_VBEND , x++) = pen[ pixel ];
				}
		}
}

IRQ_CALLBACK_MEMBER(hp9845b_state::irq_callback)
{
		if (irqline == HPHYBRID_IRL) {
			//logerror("irq ack L %02x\n" , m_irl_pending);
				return m_irl_pending;
		} else {
			//logerror("irq ack H %02x\n" , m_irh_pending);
				return m_irh_pending;
		}
}

void hp9845b_state::update_irq(void)
{
		m_ppu->set_input_line(HPHYBRID_IRL , m_irl_pending != 0);
		m_ppu->set_input_line(HPHYBRID_IRH , m_irh_pending != 0);
}

void hp9845b_state::irq_w(uint8_t sc , int state)
{
	unsigned bit_n = sc % 8;

	if (sc < 8) {
		if (state) {
			BIT_SET(m_irl_pending, bit_n);
		} else {
			BIT_CLR(m_irl_pending, bit_n);
		}
	} else {
		if (state) {
			BIT_SET(m_irh_pending, bit_n);
		} else {
			BIT_CLR(m_irh_pending, bit_n);
		}
	}
	update_irq();
}

void hp9845b_state::update_flg_sts(void)
{
	bool sts = BIT(m_sts_status , m_pa);
	bool flg = BIT(m_flg_status , m_pa);
	m_ppu->status_w(sts);
	m_ppu->flag_w(flg);
}

void hp9845b_state::sts_w(uint8_t sc , int state)
{
	if (state) {
		BIT_SET(m_sts_status, sc);
	} else {
		BIT_CLR(m_sts_status, sc);
	}
	if (sc == m_pa) {
		update_flg_sts();
	}
}

void hp9845b_state::flg_w(uint8_t sc , int state)
{
	if (state) {
		BIT_SET(m_flg_status, sc);
	} else {
		BIT_CLR(m_flg_status, sc);
	}
	if (sc == m_pa) {
		update_flg_sts();
	}
}

void hp9845b_state::install_readwrite_handler(uint8_t sc , read16_delegate rhandler, write16_delegate whandler)
{
	// Install r/w handlers to cover all I/O addresses of PPU belonging to "sc" select code
	m_ppu->space(AS_IO).install_readwrite_handler(sc * 4 , sc * 4 + 3 , rhandler , whandler);
}

TIMER_DEVICE_CALLBACK_MEMBER(hp9845b_state::kb_scan)
{
		ioport_value input[ 4 ];
		input[ 0 ] = m_io_key0->read();
		input[ 1 ] = m_io_key1->read();
		input[ 2 ] = m_io_key2->read();
		input[ 3 ] = m_io_key3->read();

		// Set status bits for "shift", "control", "auto start" & "print all" keys
		// ** Print all **
		// (R,C) = (0,1)
		// Bit 12 in kb status
		if (BIT(input[ 0 ] , 1)) {
				BIT_SET(m_kb_status , 12);
				BIT_CLR(input[ 0 ] , 1);
		} else {
				BIT_CLR(m_kb_status, 12);
		}
		// ** Auto start **
		// (R,C) = (1,1)
		// Bit 13 in kb status
		if (BIT(input[ 0 ] , 17)) {
				BIT_SET(m_kb_status , 13);
				BIT_CLR(input[ 0 ] , 17);
		} else {
				BIT_CLR(m_kb_status, 13);
		}
		// ** Control **
		// (R,C) = (4,15)
		// Bit 14 in kb status
		if (BIT(input[ 2 ] , 15)) {
				BIT_SET(m_kb_status , 14);
				BIT_CLR(input[ 2 ] , 15);
		} else {
				BIT_CLR(m_kb_status, 14);
		}
		// ** Shift **
		// (R,C) = (0,15)
		// Bit 15 in kb status
		if (BIT(input[ 0 ] , 15)) {
				BIT_SET(m_kb_status , 15);
				BIT_CLR(input[ 0 ] , 15);
		} else {
				BIT_CLR(m_kb_status, 15);
		}

		// TODO: handle repeat key
		// TODO: handle ctrl+stop

		for (unsigned i = 0; i < 128; i++) {
				ioport_value mask = BIT_MASK(i & 0x1f);
				unsigned idx = i >> 5;

				if ((input[ idx ] & ~m_kb_state[ idx ]) & mask) {
						// Key pressed, store scancode & generate IRL
						m_kb_scancode = i;
						irq_w(0 , 1);
						BIT_SET(m_kb_status, 0);

						// Special case: pressing stop key sets LPU "status" flag
						if (i == 0x47) {
								m_lpu->status_w(1);
						}
				}
		}

		memcpy(&m_kb_state[ 0 ] , &input[ 0 ] , sizeof(m_kb_state));
}

READ16_MEMBER(hp9845b_state::kb_scancode_r)
{
		return ~m_kb_scancode & 0x7f;
}

READ16_MEMBER(hp9845b_state::kb_status_r)
{
		return m_kb_status;
}

WRITE16_MEMBER(hp9845b_state::kb_irq_clear_w)
{
		irq_w(0 , 0);
		BIT_CLR(m_kb_status, 0);
		m_lpu->status_w(0);

		if (BIT(data , 15)) {
			// Start beeper
			m_beep_timer->adjust(attotime::from_ticks(64, KEY_SCAN_OSCILLATOR / 512));
			m_beeper->set_state(1);
		}
}

TIMER_DEVICE_CALLBACK_MEMBER(hp9845b_state::beeper_off)
{
	m_beeper->set_state(0);
}

WRITE8_MEMBER(hp9845b_state::pa_w)
{
	if (data != m_pa) {
		m_pa = data;
		update_flg_sts();
	}
}

WRITE_LINE_MEMBER(hp9845b_state::t15_irq_w)
{
	irq_w(T15_PA , state);
}

WRITE_LINE_MEMBER(hp9845b_state::t15_flg_w)
{
	flg_w(T15_PA , state);
}

WRITE_LINE_MEMBER(hp9845b_state::t15_sts_w)
{
	sts_w(T15_PA , state);
}

static MACHINE_CONFIG_START( hp9845a, hp9845_state )
	//MCFG_CPU_ADD("lpu", HP_5061_3010, XTAL_11_4MHz)
	//MCFG_CPU_ADD("ppu", HP_5061_3011, XTAL_11_4MHz)

	// video hardware
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_UPDATE_DRIVER(hp9845_state, screen_update)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_SIZE(560, 455)
	MCFG_SCREEN_VISIBLE_AREA(0, 560-1, 0, 455-1)

	MCFG_SOFTWARE_LIST_ADD("optrom_list", "hp9845a_rom")
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( hp9835a, hp9845_state )
	//MCFG_CPU_ADD("lpu", HP_5061_3001, XTAL_11_4MHz)
	//MCFG_CPU_ADD("ppu", HP_5061_3001, XTAL_11_4MHz)

	// video hardware
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_UPDATE_DRIVER(hp9845_state, screen_update)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_SIZE(560, 455)
	MCFG_SCREEN_VISIBLE_AREA(0, 560-1, 0, 455-1)

	MCFG_SOFTWARE_LIST_ADD("optrom_list", "hp9835a_rom")
MACHINE_CONFIG_END

static ADDRESS_MAP_START(global_mem_map , AS_PROGRAM , 16 , hp9845b_state)
	ADDRESS_MAP_GLOBAL_MASK(0x3f7fff)
	ADDRESS_MAP_UNMAP_LOW
	AM_RANGE(0x000000 , 0x007fff) AM_RAM AM_SHARE("lpu_ram")
	AM_RANGE(0x014000 , 0x017fff) AM_RAM AM_SHARE("ppu_ram")
	AM_RANGE(0x020000 , 0x027fff) AM_RAM AM_SHARE("lpu_02_ram")
	AM_RANGE(0x030000 , 0x037fff) AM_ROM AM_REGION("lpu" , 0)
	AM_RANGE(0x040000 , 0x047fff) AM_RAM AM_SHARE("lpu_04_ram")
	AM_RANGE(0x050000 , 0x057fff) AM_ROM AM_REGION("ppu" , 0)
ADDRESS_MAP_END

static ADDRESS_MAP_START(ppu_io_map , AS_IO , 16 , hp9845b_state)
		ADDRESS_MAP_UNMAP_LOW
		// PA = 0, IC = 2
		// Keyboard scancode input
		AM_RANGE(HP_MAKE_IOADDR(0 , 2) , HP_MAKE_IOADDR(0 , 2)) AM_READ(kb_scancode_r)
		// PA = 0, IC = 3
		// Keyboard status input & keyboard interrupt clear
		AM_RANGE(HP_MAKE_IOADDR(0 , 3) , HP_MAKE_IOADDR(0 , 3)) AM_READWRITE(kb_status_r , kb_irq_clear_w)
		// PA = 13, IC = 0..3
		// Graphic video
		AM_RANGE(HP_MAKE_IOADDR(GVIDEO_PA , 0) , HP_MAKE_IOADDR(GVIDEO_PA , 3)) AM_READWRITE(graphic_r , graphic_w)
		// PA = 15, IC = 0..3
		// Right-hand side tape drive (T15)
		AM_RANGE(HP_MAKE_IOADDR(T15_PA , 0) , HP_MAKE_IOADDR(T15_PA , 3))        AM_DEVREADWRITE("t15" , hp_taco_device , reg_r , reg_w)
ADDRESS_MAP_END

static MACHINE_CONFIG_START( hp9845b, hp9845b_state )
	MCFG_CPU_ADD("lpu", HP_5061_3001, 5700000)
		MCFG_CPU_PROGRAM_MAP(global_mem_map)
		MCFG_HPHYBRID_SET_9845_BOOT(true)
	MCFG_CPU_ADD("ppu", HP_5061_3001, 5700000)
		MCFG_CPU_PROGRAM_MAP(global_mem_map)
	MCFG_CPU_IO_MAP(ppu_io_map)
		MCFG_HPHYBRID_SET_9845_BOOT(true)
	MCFG_CPU_IRQ_ACKNOWLEDGE_DRIVER(hp9845b_state , irq_callback)
		MCFG_HPHYBRID_PA_CHANGED(WRITE8(hp9845b_state , pa_w))

	// video hardware
	MCFG_SCREEN_ADD_MONOCHROME("screen", RASTER, rgb_t::green())
	MCFG_SCREEN_UPDATE_DRIVER(hp9845b_state, screen_update)
		// These parameters are for alpha video
	MCFG_SCREEN_RAW_PARAMS(VIDEO_PIXEL_CLOCK , VIDEO_HTOTAL , 0 , VIDEO_HBSTART , VIDEO_VTOTAL , 0 , VIDEO_ACTIVE_SCANLINES)
		MCFG_SCREEN_VBLANK_DRIVER(hp9845b_state, vblank_w)
	MCFG_PALETTE_ADD_MONOCHROME_HIGHLIGHT("palette")

	MCFG_TIMER_DRIVER_ADD_SCANLINE("scantimer", hp9845b_state, scanline_timer, "screen", 0, 1)
		MCFG_TIMER_DRIVER_ADD("gv_timer", hp9845b_state, gv_timer)

	// Actual keyboard refresh rate should be KEY_SCAN_OSCILLATOR / 128 (2560 Hz)
	MCFG_TIMER_DRIVER_ADD_PERIODIC("kb_timer" , hp9845b_state , kb_scan , attotime::from_hz(100))

	// Beeper
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("beeper" , BEEP , KEY_SCAN_OSCILLATOR / 512)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS , "mono" , 1.00)

	MCFG_TIMER_DRIVER_ADD("beep_timer" , hp9845b_state , beeper_off);

		// Tape controller
		MCFG_DEVICE_ADD("t15" , HP_TACO , 4000000)
		MCFG_TACO_IRQ_HANDLER(WRITELINE(hp9845b_state , t15_irq_w))
		MCFG_TACO_FLG_HANDLER(WRITELINE(hp9845b_state , t15_flg_w))
		MCFG_TACO_STS_HANDLER(WRITELINE(hp9845b_state , t15_sts_w))

	// In real machine there were 8 slots for LPU ROMs and 8 slots for PPU ROMs in
	// right-hand side and left-hand side drawers, respectively.
	// Here we do away with the distinction between LPU & PPU ROMs: in the end they
	// are visible to both CPUs at the same addresses.
	MCFG_DEVICE_ADD("drawer1", HP_OPTROM_SLOT, 0)
	MCFG_DEVICE_SLOT_INTERFACE(hp_optrom_slot_device, NULL, false)
	MCFG_DEVICE_ADD("drawer2", HP_OPTROM_SLOT, 0)
	MCFG_DEVICE_SLOT_INTERFACE(hp_optrom_slot_device, NULL, false)
	MCFG_DEVICE_ADD("drawer3", HP_OPTROM_SLOT, 0)
	MCFG_DEVICE_SLOT_INTERFACE(hp_optrom_slot_device, NULL, false)
	MCFG_DEVICE_ADD("drawer4", HP_OPTROM_SLOT, 0)
	MCFG_DEVICE_SLOT_INTERFACE(hp_optrom_slot_device, NULL, false)
	MCFG_DEVICE_ADD("drawer5", HP_OPTROM_SLOT, 0)
	MCFG_DEVICE_SLOT_INTERFACE(hp_optrom_slot_device, NULL, false)
	MCFG_DEVICE_ADD("drawer6", HP_OPTROM_SLOT, 0)
	MCFG_DEVICE_SLOT_INTERFACE(hp_optrom_slot_device, NULL, false)
	MCFG_DEVICE_ADD("drawer7", HP_OPTROM_SLOT, 0)
	MCFG_DEVICE_SLOT_INTERFACE(hp_optrom_slot_device, NULL, false)
	MCFG_DEVICE_ADD("drawer8", HP_OPTROM_SLOT, 0)
	MCFG_DEVICE_SLOT_INTERFACE(hp_optrom_slot_device, NULL, false)

	MCFG_SOFTWARE_LIST_ADD("optrom_list", "hp9845b_rom")

	MCFG_HP9845_IO_SLOT_ADD("slot0")
MACHINE_CONFIG_END

ROM_START( hp9845a )
	ROM_REGION( 0200000, "lpu", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "09845-65544-65547-03-system_lpu.bin", 0000000, 0200000, CRC(47beb87f) SHA1(456caefacafcf19435e1e7e68b1c1e4010841664) )

	ROM_REGION( 0200000, "ppu", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "09845-65540-65543-01-system_ppu.bin", 0000000, 0160000, CRC(bc0a34cc) SHA1(9ff215f4ba32ad85f144845d15f762a71e35588b) )
ROM_END

#define rom_hp9845s rom_hp9845a

ROM_START( hp9835a )
	ROM_REGION( 0200000, "lpu", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "1818-2800-03_00-system-lpu.bin", 0000000, 020000, CRC(e0b0977a) SHA1(5afdc6c725abff70b674e46688d8ab38ccf8f3c1) )
	ROM_LOAD( "1818-2801-03_10-system-lpu.bin", 0020000, 020000, CRC(c51c1e3a) SHA1(798964fa2e7a1fc149ce4400b694630049293119) )
	ROM_LOAD( "1818-2802-03_20-system-lpu.bin", 0040000, 020000, CRC(bba70a7e) SHA1(2d488594493f8dfcd753e462414cc51c24596a2c) )
	ROM_LOAD( "1818-2803-03_30-system-lpu.bin", 0060000, 020000, CRC(65e9eba6) SHA1(a11f5d37e8ed14a428335c43e785d635b02d1129) )
	ROM_LOAD( "1818-2804-03_40-system-lpu.bin", 0100000, 020000, CRC(ef83b695) SHA1(8ca2914609ece2c9c59ebba6ece3fcbc8929aeaf) )
	ROM_LOAD( "1818-2805-03_50-system-lpu.bin", 0120000, 020000, CRC(401d539f) SHA1(00bda59f71632c4d4fc3268c04262bb81ef0eeba) )
	ROM_LOAD( "1818-2806-03_60-system-lpu.bin", 0140000, 020000, CRC(fe353db5) SHA1(0fb52d82d3743008cdebebb20c488e34ce2fca4b) )
	ROM_LOAD( "1818-2807-03_70-system-lpu.bin", 0160000, 020000, CRC(45a3cc5e) SHA1(35c9959331acf7c98ab6a880915b03e3e783a656) )

	ROM_REGION( 0200000, "ppu", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "1818-2808-05_00-system-ppu.bin", 0000000, 020000, CRC(d0c96276) SHA1(cc578d586c4eda81469f29eb7cab7f667e0d5977) )
	ROM_LOAD( "1818-2809-05_30-system-ppu.bin", 0060000, 020000, CRC(ccdb7171) SHA1(1d24596bc1219983e7cb81f6987af094f2ca7d81) )
	ROM_LOAD( "1818-2810-05_40-system-ppu.bin", 0100000, 020000, CRC(97487d24) SHA1(823cd16671de8e6ff2c245060c99778acb6ff79c) )
	ROM_LOAD( "1818-2811-05_50-system-ppu.bin", 0120000, 020000, CRC(18aee6fd) SHA1(388d3b2a063ea2cfdfe9fb9f864fa5f08af817b0) )
	ROM_LOAD( "1818-2812-05_60-system-ppu.bin", 0140000, 020000, CRC(c0beeeae) SHA1(a5db36a7f7bad84c1013bd3ec4813c355f72427d) )
	ROM_LOAD( "1818-2813-05_70-system-ppu.bin", 0160000, 020000, CRC(75361bbf) SHA1(40f499c597da5c8c9a55a2a891976d946a54926b) )
ROM_END

#define rom_hp9835b rom_hp9835a

ROM_START( hp9845b )
	ROM_REGION(0x800 , "chargen" , 0)
	ROM_LOAD("chrgen.bin" , 0 , 0x800 , CRC(fe9e844f) SHA1(0c45ae00766ceba94a19bd5e154bd6d23e208cca))

	ROM_REGION(0x800 , "optional_chargen" , 0)
	ROM_LOAD("optional_chrgen.bin" , 0 , 0x800 , CRC(0ecfa63b) SHA1(c295e6393d1503d903c1d2ce576fa597df9746bf))

		ROM_REGION(0x10000, "lpu", ROMREGION_16BIT | ROMREGION_BE)
		ROM_LOAD("9845-LPU-Standard-Processor.bin", 0, 0x10000, CRC(dc266c1b) SHA1(1cf3267f13872fbbfc035b70f8b4ec6b5923f182))

		ROM_REGION(0x10000, "ppu", ROMREGION_16BIT | ROMREGION_BE)
		ROM_LOAD("9845-PPU-Standard-Graphics.bin", 0, 0x10000, CRC(f866510f) SHA1(3e22cd2072e3a5f3603a1eb8477b6b4a198d184d))

#if 0
	ROM_REGION( 0200000, "lpu", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "1818-0823-0827-03_00-revb-system_lpu.bin", 0000000, 020000, CRC(7e49c781) SHA1(866c9ebd98d94bb6f99692e29d2d83f55b38c4b6) )
	ROM_LOAD( "1818-0823-0827-03_10-revb-system_lpu.bin", 0020000, 020000, CRC(2f819e3d) SHA1(250886378c3ce2253229997007c7bf0be80a8d1d) )
	ROM_LOAD( "1818-0824-0828-03_20-reva-system_lpu.bin", 0040000, 020000, CRC(834f7063) SHA1(5c390ed74671e4663cc80d899d07b69fd1fb4be6) )
	ROM_LOAD( "1818-0824-0828-03_20-revb-system_lpu.bin", 0040000, 020000, CRC(aa221deb) SHA1(7878643405ee45405dc5269c3b6dc9459f39437b) )
	ROM_LOAD( "1818-0824-0828-03_30-reva-system_lpu.bin", 0060000, 020000, CRC(0ebafdb2) SHA1(80733bfb7026d39a294841221d80ec40eafffe34) )
	ROM_LOAD( "1818-0824-0828-03_30-revb-system_lpu.bin", 0060000, 020000, CRC(0ebafdb2) SHA1(80733bfb7026d39a294841221d80ec40eafffe34) )
	ROM_LOAD( "1818-0825-0829-03_40-revb-system_lpu.bin", 0100000, 020000, CRC(beb09a57) SHA1(b832b995fa21c219673f0c7cf215dee70698f4f1) )
	ROM_LOAD( "1818-0825-0829-03_50-revb-system_lpu.bin", 0120000, 020000, CRC(bbb06222) SHA1(b0bfe1b48fac61eb955e27e0ddfbea020e09e0eb) )
	ROM_LOAD( "1818-0826-0830-03_60-revc-system_lpu.bin", 0140000, 020000, CRC(5c1c3abe) SHA1(fa9f99bf7c8a6df5c71e9fd8c807f0a2ff06640d) )
	ROM_LOAD( "1818-0826-0830-03_70-revc-system_lpu.bin", 0160000, 020000, CRC(0c61a266) SHA1(0cfbf482e7f8e99c87b97c77cf178682cd7af7d6) )

	ROM_REGION( 0200000, "lpu_fast", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "1818-1506-1502-03_00-reva-system_fast_lpu.bin", 0000000, 020000, CRC(b77194d8) SHA1(6feec8605331783e6f5a2ab6d6cbd9285036e863) )
	ROM_LOAD( "1818-1506-1502-03_10-reva-system_fast_lpu.bin", 0020000, 020000, CRC(bc5557a5) SHA1(282237e561c3f2304cdeb45efa2432748581af45) )
	ROM_LOAD( "1818-1507-1503-03_20-reva-system_fast_lpu.bin", 0040000, 020000, CRC(2ebc71e2) SHA1(a2d39fb24d565465304833dfd0ff87dd5ef26fb3) )
	ROM_LOAD( "1818-1507-1503-03_30-reva-system_fast_lpu.bin", 0060000, 020000, CRC(82e56bc4) SHA1(36201f343382e533c248ddd123507a2e195cca39) )
	ROM_LOAD( "1818-1508-1504-03_40-reva-system_fast_lpu.bin", 0100000, 020000, CRC(70b0fcb0) SHA1(3f7ce60cad0ffec8344f33d584869492c7f73026) )
	ROM_LOAD( "1818-1508-1504-03_50-reva-system_fast_lpu.bin", 0120000, 020000, CRC(935fab96) SHA1(ecb1da2a0bd46e8c0da2875a1af8cf71d8f4bb56) )
	ROM_LOAD( "1818-1509-1505-03_60-reva-system_fast_lpu.bin", 0140000, 020000, CRC(f4119af7) SHA1(72a3e8b8d7d306e55f8adf0e23225bb81bc2b4ba) )
	ROM_LOAD( "1818-1509-1505-03_70-reva-system_fast_lpu.bin", 0160000, 020000, CRC(22fb0864) SHA1(4e1dce32e84ba216dbbd4116f3b22ca7f254f529) )

	ROM_REGION( 0200000, "ppu", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "1818-0833-0837-05_40-revc-system_ppu.bin", 0100000, 020000, CRC(d790795c) SHA1(7ba1e245a98379a34833a780898a784049e33b86) )
	ROM_LOAD( "1818-0833-0837-05_40-revd-system_ppu.bin", 0100000, 020000, CRC(49897e40) SHA1(780a9973ff26d40f470e2004fccceb1019f8ba7f) )
	ROM_LOAD( "1818-0833-0837-05_50-revc-system_ppu.bin", 0120000, 020000, CRC(ef8acde4) SHA1(e68648543aac2b841b08d7758949ba1339a83701) )
	ROM_LOAD( "1818-0833-0837-05_50-revd-system_ppu.bin", 0120000, 020000, CRC(54f61d07) SHA1(f807fb8a59cd9cd221f63907e6a86948a0bf7c1d) )
	ROM_LOAD( "1818-0834-0838-05_60-revc-system_ppu.bin", 0140000, 020000, CRC(20f2100a) SHA1(9304f0b069de9233d697588328f9657dbeabc254) )
	ROM_LOAD( "1818-0834-0838-05_60-revd-system_ppu.bin", 0140000, 020000, CRC(454af601) SHA1(54b56e67e855fd2d699a0dbef0b4d2e8c150c39b) )
	ROM_LOAD( "1818-0834-0838-05_70-revc-system_ppu.bin", 0160000, 020000, CRC(43f62491) SHA1(a9489b37b3fa8768ca6e503f346bd023833ae3ac) )
	ROM_LOAD( "1818-0834-0838-05_70-revd-system_ppu.bin", 0160000, 020000, CRC(43f62491) SHA1(a9489b37b3fa8768ca6e503f346bd023833ae3ac) )
	ROM_LOAD( "1818-1899-1898-05_60-reva-system_ppu.bin", 0140000, 020000, CRC(454af601) SHA1(54b56e67e855fd2d699a0dbef0b4d2e8c150c39b) )
	ROM_LOAD( "1818-1899-1898-05_70-reva-system_ppu.bin", 0160000, 020000, CRC(049604f2) SHA1(89bfd8e086bc9365f156966b0a62c3ac720fc627) )

	ROM_REGION( 0200000, "ppu_tops", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "1818-0831-0835-05_00-reva-tops_ppu.bin", 0000000, 020000, CRC(7ddce706) SHA1(746e34d3de52a17372af9a9eb1ed4974a4eae656) )
	ROM_LOAD( "1818-0831-0835-05_10-reva-tops_ppu.bin", 0020000, 020000, CRC(d7fc3d47) SHA1(a3d723fe62f047cb0c17d405d07bb0b08d08e830) )
	ROM_LOAD( "1818-1209-1208-05_00-revb-tops_ppu.bin", 0000000, 020000, CRC(0dc90614) SHA1(94c07553a62b2c86414bc95314601f90eb4e4022) )
	ROM_LOAD( "1818-1209-1208-05_10-revb-tops_ppu.bin", 0020000, 020000, CRC(4e362657) SHA1(b09098c0acd56b11ec3b72ff3e8b5a1e14ef3ae8) )
	ROM_LOAD( "1818-1592-1591-05_00-revb-tops_ppu.bin", 0000000, 020000, CRC(8cfe29a8) SHA1(f1007b6b1d3f2b603653880c44cec48b23701263) )
	ROM_LOAD( "1818-1592-1591-05_10-revb-tops_ppu.bin", 0020000, 020000, CRC(95048264) SHA1(36cfddef9d1289fdaf69596e10d95f88a520feae) )

	ROM_REGION( 0200000, "ppu_kbd_us", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "1818-0832-0836-05_20-revc-keyboard_us.bin", 0040000, 020000, CRC(3bf6268a) SHA1(65d7dfeaf34c74dbc86ebe5d3bb65c6bd10163cb) )
	ROM_LOAD( "1818-0832-0836-05_30-revc-keyboard_us.bin", 0060000, 020000, CRC(2dfc619c) SHA1(5c54ff502d1344907817210bfdfcab7f8d6b61bd) )

	ROM_REGION( 0200000, "ppu_kbd_de", ROMREGION_16BIT | ROMREGION_BE )
	ROM_LOAD( "1818-0841-0846-05_20-revc-keyboard_german.bin", 0040000, 020000, CRC(76667eca) SHA1(ac63e5d584d1f2da5668d8a9560f927f48e25e03) )
	ROM_LOAD( "1818-0841-0846-05_20-revd-keyboard_german.bin", 0060000, 020000, CRC(3bf6268a) SHA1(65d7dfeaf34c74dbc86ebe5d3bb65c6bd10163cb) )
	ROM_LOAD( "1818-0841-0846-05_30-revc-keyboard_german.bin", 0040000, 020000, CRC(2b83db22) SHA1(6eda714ce05d2d75f4c041e36b6b6df40697d94a) )
	ROM_LOAD( "1818-0841-0846-05_30-revd-keyboard_german.bin", 0060000, 020000, CRC(b4006959) SHA1(584a85f746a3b0c262fdf9e4be8e696c80cfd429) )
#endif
ROM_END

#define rom_hp9845t rom_hp9845b
#define rom_hp9845c rom_hp9845b

COMP( 1978, hp9845a,   0,       0,      hp9845a,       hp9845, driver_device, 0,      "Hewlett-Packard",  "9845A",  MACHINE_IS_SKELETON | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1978, hp9845s,   hp9845a, 0,      hp9845a,       hp9845, driver_device, 0,      "Hewlett-Packard",  "9845S",  MACHINE_IS_SKELETON | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1979, hp9835a,   0,       0,      hp9835a,       hp9845, driver_device, 0,      "Hewlett-Packard",  "9835A",  MACHINE_IS_SKELETON | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1979, hp9835b,   hp9835a, 0,      hp9835a,       hp9845, driver_device, 0,      "Hewlett-Packard",  "9835B",  MACHINE_IS_SKELETON | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1980, hp9845b,   0,       0,      hp9845b,       hp9845b,driver_device, 0,      "Hewlett-Packard",  "9845B",  0 )
COMP( 1980, hp9845t,   hp9845b, 0,      hp9845b,       hp9845b,driver_device, 0,      "Hewlett-Packard",  "9845T",  MACHINE_IS_SKELETON | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1981, hp9845c,   hp9845b, 0,      hp9845b,       hp9845b,driver_device, 0,      "Hewlett-Packard",  "9845C",  MACHINE_IS_SKELETON | MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
