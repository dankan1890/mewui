// license:BSD-3-Clause
// copyright-holders:F. Ulivi
/*********************************************************************

    hp_taco.cpp

    HP TApe COntroller (5006-3012)

*********************************************************************/

// This device has been reverse engineered entirely through documents & study of HP software.
// I had no access to the real device to experiment.
// Available documentation on the internal working of TACO chip is close to nothing. The best
// I could find is [1] (see below) where all that's described is a (too) brief summary of registers and little else.
// In other words, no description of the commands that can be issued to TACO chips.
// So, my main source of information was the careful study of HP software, especially the 9845 system test ROM (09845-66520).
// The second half of this ROM holds a comprehensive set of tape drive tests.
// Another source was the "SIF" utility tools which use a lot of peculiar command sequences.
// The main shortcomings of my approach are:
// * I could identify only those TACO commands that are actually used by the software. I managed
//   to identify 17 out of 32 possible commands. The purpose of the rest of commands is anyone's guess.
// * I could only guess the behavior of TACO chips in corner cases (especially behavior in various error/abnormal
//   conditions)
//
// Documentation I used:
// [1]  HP, manual 64940-90905, may 80 rev. - Model 64940A tape control & drive service manual
// [2]  US patent 4,075,679 describing HP9825 system (this system had a discrete implementation of tape controller). The
//      firmware listing was quite useful in identifying sequences of commands (for example how to find a specific sector etc.).
// [3]  http://www.hp9845.net site
// [4]  April 1978 issue of HP Journal. There is a one-page summary of TACO chip on page 20.
// [5]  HP, manual 09845-10201, apr 81 rev. - General Utility Routines. This manual describes the SIF format and related
//      utility tools.

// This is an overview of the TACO/CPU interface.
//
// Reg. | R/W | Content
// =====================
// R4   | R/W | Data register: words read/written to/from tape pass through this register
// R5   | R/W | Command and status register (see below)
// R6   | R/W | Tachometer register. Writing it sets a pulse counter that counts up on either tachometer pulses or IRGs, depending
//      |     | on command. When the counter rolls over from 0xffff to 0 it typically ends the command.
//      |     | Current counter value is returned when reading from this register.
// R7   | R   | Checksum register. Reading it clears it next time the checksum is updated.
// R7   | W   | Timing register. It controls somehow the encoding and decoding of bits. For now I completely ignore it because its
//      |     | content it's totally unknown to me. It seems safe to do so, anyway. I can see that it's always set to 0x661d before
//      |     | writing to tape and to 0x0635 before reading (0x061d is set by SIF utilities).
//
// Format of TACO command/status register (R5)
// Bit    R/W Content
// ===============
// 15     RW  Tape direction (1 = forward)
// 14..10 RW  Command (see the "enum" below)
//  9     RW  Automatic stopping of tape at command completion (when 1)
//  8     RW  Minimum size of gaps (1 = 1.5", 0 = 0.066")
//  7     RW  Speed of tape (1 = 90 ips, 0 = 22 ips)
//  6     RW  Option bit for various commands. Most of them use it to select read threshold (0 = low, 1 = high).
//  5     R   Current track (1 = B)
//  4     R   Gap detected (1)
//  3     R   Write protection (1)
//  2     R   Servo failure (1)
//  1     R   Cartridge out (1)
//  0     R   Hole detected (1)

// Here's a summary of the on-tape format of HP9845 systems.
// * A tape has two independent tracks (A & B).
// * Each track holds 426 sectors.
// * Each sector has an header and 256 bytes of payload (see below)
// * Sectors are separated by gaps of uniform magnetization called IRGs (Inter-Record Gaps)
// * The basic unit of data I/O are 16-bit words
// * Bits are encoded by different distances between magnetic flux reversals
// * The structure of tracks is:
//   - Begin of tape holes
//   - The deadzone: 350x 0xffff words
//   - 1" of IRG
//   - Sector #0 (track A) or #426 (track B)
//   - 1" of IRG (2.5" on track A)
//   - Sector #1 (track A) or #427 (track B)
//   - 1" of IRG
//   - Sector #2 (track A) or #428 (track B)
//   - ...and so on up to sector #425/#851
//   - 6" of final gap
//   - Non-recorded tape
//   - End of tape holes
// * Even though the tape format is not SIF (HP's own Standard Interchange Format), it is
//   clearly based on SIF itself. The whole tape content is stored according to SIF,
//   specifically it is "encapsulated" inside file #1.
// * Sector #0 is not used. It serves as SIF "file identifier record" (i.e. it identifies
//   that the rest of tape is inside file #1 from SIF point of view).
// * Sectors #1 and #2 hold the first copy of tape directory
// * Sectors #3 and #4 hold the second/backup copy of tape directory
// * User data are stored starting from sector #5
// * There is no "fragmentation" map (like file allocation table in FAT filesystem): a file
//   spanning more than 1 sector always occupy a single block of contiguous sectors.
//
// A sector is structured like this (see description of SIF in [5], pg 655 and following):
// Word 0:      Invisible preamble word (always 0). Preamble comes from 9825, don't know if it's
//              actually there in TACO encoding. I assumed it is.
// Word 1:      File word: file identifier bit, empty record indicator and file number
// Word 2:      Record word: sector number and "free field pattern"
// Word 3:      Length word: bytes available and used in sector
// Word 4:      Checksum (sum of words 1..3)
// Words 5..132:        Payload
// Word 133:    Checksum (sum of words 5..132)
//
// Physical encoding of words is borrowed from 9825 as I wasn't able
// to gather any info on the actual encoding of TACO chips.
// This is how 9825 encodes words on tape:
// - the unit of encoding are 16-bit words
// - each word is encoded from MSB to LSB
// - each word has an extra invisible "1" encoded at the end (a kind of "stop" bit)
// - tape is read/written at slow speed only (21.98 ips)
// - a 0 is encoded with a distance between flux reversals of 1/35200 s
//   (giving a maximum density of about 1600 reversals per inch)
// - a 1 is encoded with a distance that's 1.75 times that of a 0
//
// This driver is based on the following model of the actual TACO/tape system:
// * Tape immediately reaches working speed (no spin-up time)
// * Change of speed is immediate as well
// * Time & distance to stop and invert direction of tape are modeled, though. Firmware is upset by
//   a tape with null braking/inversion time/distance.
// * Speed of tape is exceptionally accurate. Real tape was controlled by a closed loop
//   with something like 1% accuracy on speed.
// * Storage is modeled by one "map" data structure per track. Each map maps the tape position
//   to the 16-bit word stored at that position. Gaps are modeled by lack of data in the map.
//   There is no model of the physical encoding of bits (except to compute how long each word
//   is on tape).
// * Read threshold is ignored. Real tapes could be read with either a low or high threshold.
// * "Flag" bit is used as a busy/ready signal in real TACO. Here I assumed the device is
//   always ready, so Flag is always active.
// * I tried to fill the (many) gaps on chip behavior with "sensible" solutions. I could only
//   validate my solutions by running the original firmware in MAME, though (no real hw at hand).
//
// TODOs/issues:
// * Commands 00 & 10 seem to do the same things. My bet is that they differ in some subtle way that
//   is not stimulated by all the tape software I used for R.E. Maybe it's just the length of the
//   no-data timeout they have.
// * Find more info on TACO chips (does anyone with a working 9845 or access to internal HP docs want to
//   help me here, please?)
//
#include "emu.h"
#include "hp_taco.h"
#include "ui/uimain.h"

// Debugging
#define VERBOSE 1
#define LOG(x)  do { if (VERBOSE) logerror x; } while (0)
#define VERBOSE_0 0
#define LOG_0(x)  do { if (VERBOSE_0) logerror x; } while (0)

// Macros to clear/set single bits
#define BIT_MASK(n) (1U << (n))
#define BIT_CLR(w , n)  ((w) &= ~BIT_MASK(n))
#define BIT_SET(w , n)  ((w) |= BIT_MASK(n))

// Timers
enum {
	TAPE_TMR_ID,
	HOLE_TMR_ID,
	TIMEOUT_TMR_ID
};

// Constants
#define CMD_REG_MASK    0xffc0  // Command register mask
#define STATUS_REG_MASK 0x003f  // Status register mask
#define TACH_TICKS_PER_INCH     968     // Tachometer pulses per inch of tape movement
#define TAPE_POS_FRACT  1024    // 10 bits of fractional part in tape_pos_t
#define TACH_FREQ_SLOW  21276   // Tachometer pulse frequency for slow speed (21.98 ips)
#define TACH_FREQ_FAST  87196   // Tachometer pulse frequency for fast speed (90.08 ips)
#define TACH_FREQ_BRAKE_SLOW    11606   // Tachometer pulse frequency when stopping from slow speed (11.99 ips)
#define TACH_FREQ_BRAKE_FAST    44566   // Tachometer pulse frequency when stopping from fast speed (46.04 ips)
#define TAPE_INIT_POS   (80 * hti_format_t::ONE_INCH_POS)     // Initial tape position: 80" from beginning (just past the punched part)
#define QUICK_CMD_USEC  25      // usec for "quick" command execution (totally made up)
#define FAST_BRAKE_DIST 3350450 // Braking distance at fast speed (~3.38 in)
// There are 2 braking distances here: The first one (commented out) is the theoretical value, the second one
// is the value that works. Ideally we would always be using the first value but there is a kind of race
// condition when loading the memory test from the exerciser that freezes the system. So, the second (shorter)
// value is used to avoid this condition.
//#define SLOW_BRAKE_DIST 197883  // Braking distance at slow speed (~0.2 in)
#define SLOW_BRAKE_DIST 71000  // Braking distance at slow speed (~0.07 in)
#define PREAMBLE_WORD   0       // Value of preamble word
#define END_GAP_LENGTH  (6 * hti_format_t::ONE_INCH_POS)      // Length of final gap: 6"
// Minimum gap lengths are probably counted from tacho pulses in real TACO: short gaps could be equal to 64 pulses and long ones
// to 1472 (23 * 64)
#define SHORT_GAP_LENGTH        ((hti_format_t::tape_pos_t)(0.066 * hti_format_t::ONE_INCH_POS))    // Minimum length of short gaps: 0.066" ([1], pg 8-10)
#define LONG_GAP_LENGTH ((hti_format_t::tape_pos_t)(1.5 * hti_format_t::ONE_INCH_POS))      // Minimum length of long gaps: 1.5" ([1], pg 8-10)
#define PREAMBLE_TIMEOUT    ((hti_format_t::tape_pos_t)(2.6 * hti_format_t::ONE_INCH_POS))  // Min. length of gap making preamble search time out (totally made up)
#define DATA_TIMEOUT    ((hti_format_t::tape_pos_t)(0.066 * hti_format_t::ONE_INCH_POS))    // Min. length of gap that will cause data reading to time out (totally made up)

// Parts of command register
#define CMD_CODE(reg) \
		(((reg) >> 10) & 0x1f)
#define DIR_FWD(reg) \
		(BIT(reg , 15))
#define AUTO_STOP(reg) \
		(BIT(reg , 9))
#define LONG_GAP(reg) \
		(BIT(reg , 8))
#define SPEED_FAST(reg) \
		(BIT(reg , 7))
#define CMD_OPT(reg) \
		(BIT(reg , 6))
#define DIR_FWD_MASK    BIT_MASK(15)    // Direction = forward
#define SPEED_FAST_MASK BIT_MASK(7)     // Speed = fast

// Commands
enum {
	CMD_INDTA_INGAP,        // 00: scan for data first then for gap (see also cmd 10)
	CMD_UNK_01,             // 01: unknown
	CMD_FINAL_GAP,          // 02: write final gap
	CMD_INIT_WRITE,         // 03: write words for tape formatting
	CMD_STOP,               // 04: stop
	CMD_UNK_05,             // 05: unknown
	CMD_SET_TRACK,          // 06: set A/B track
	CMD_UNK_07,             // 07: unknown
	CMD_UNK_08,             // 08: unknown
	CMD_UNK_09,             // 09: unknown
	CMD_MOVE,               // 0a: move tape
	CMD_UNK_0b,             // 0b: unknown
	CMD_INGAP_MOVE,         // 0c: scan for gap then move a bit further (used to gain some margin when inverting tape movement)
	CMD_UNK_0d,             // 0d: unknown
	CMD_CLEAR,              // 0e: clear errors/unlatch status bits
	CMD_UNK_0f,             // 0f: unknown
	CMD_NOT_INDTA,          // 10: scan for end of data (at the moment it's the same as cmd 00)
	CMD_UNK_11,             // 11: unknown
	CMD_UNK_12,             // 12: unknown
	CMD_UNK_13,             // 13: unknown
	CMD_UNK_14,             // 14: unknown
	CMD_UNK_15,             // 15: unknown
	CMD_WRITE_IRG,          // 16: write inter-record gap
	CMD_UNK_17,             // 17: unknown
	CMD_SCAN_RECORDS,       // 18: scan records (count IRGs)
	CMD_RECORD_WRITE,       // 19: write record words
	CMD_MOVE_INDTA,         // 1a: move then scan for data
	CMD_UNK_1b,             // 1b: unknown (for now it seems harmless to handle it as NOP)
	CMD_MOVE_INGAP,         // 1c: move tape a given distance then scan for gap (as cmd 0c but in reverse order)
	CMD_START_READ,         // 1d: start record reading
	CMD_DELTA_MOVE_IRG,     // 1e: move tape a given distance, detect gaps in parallel
	CMD_END_READ            // 1f: stop reading
};

// Bits of status register
#define STATUS_HOLE_BIT         0       // Hole detected
#define STATUS_CART_OUT_BIT     1       // Cartridge out
#define STATUS_SFAIL_BIT        2       // Servo failure
#define STATUS_WPR_BIT          3       // Write protection
#define STATUS_GAP_BIT          4       // Gap detected
#define STATUS_TRACKB_BIT       5       // Track B selected
#define STATUS_CART_OUT_MASK    BIT_MASK(STATUS_CART_OUT_BIT)   // Cartridge out
#define STATUS_WPR_MASK         BIT_MASK(STATUS_WPR_BIT)        // Write protection
#define STATUS_ERR_MASK         (STATUS_CART_OUT_MASK)  // Mask of errors in status reg.

// Device type definition
DEFINE_DEVICE_TYPE(HP_TACO, hp_taco_device, "hp_taco", "HP TACO")

// Constructors
hp_taco_device::hp_taco_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
		: device_t(mconfig, type, tag, owner, clock),
			device_image_interface(mconfig , *this),
			m_irq_handler(*this),
			m_flg_handler(*this),
			m_sts_handler(*this),
	m_image(),
			m_image_dirty(false)
{
		clear_state();
}

hp_taco_device::hp_taco_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
		: hp_taco_device(mconfig, HP_TACO, tag, owner, clock)
{
}

WRITE16_MEMBER(hp_taco_device::reg_w)
{
		LOG_0(("wr R%u = %04x\n", 4 + offset , data));

		// Any I/O activity clears IRQ
		irq_w(false);

		switch (offset) {
		case 0:
				// Data register
				m_data_reg = data;
				m_data_reg_full = true;
				break;

		case 1:
				// Command register
				start_cmd_exec(data & CMD_REG_MASK);
				break;

		case 2:
				// Tachometer register
				m_tach_reg = data;
				freeze_tach_reg(true);
				break;

		case 3:
				// Timing register
				m_timing_reg = data;
				break;
		}
}

READ16_MEMBER(hp_taco_device::reg_r)
{
		uint16_t res = 0;

		// Any I/O activity clears IRQ
		irq_w(false);

		switch (offset) {
		case 0:
				// Data register
				res = m_data_reg;
				break;

		case 1:
				// Command & status register
				res = (m_cmd_reg & CMD_REG_MASK) | (m_status_reg & STATUS_REG_MASK);
				break;

		case 2:
				// Tachometer register
				update_tach_reg();
				res = m_tach_reg;
				break;

		case 3:
				// Checksum register: it clears when read
				res = m_checksum_reg;
				m_clear_checksum_reg = true;
				break;
		}

		LOG_0(("rd R%u = %04x\n", 4 + offset , res));

		return res;
}

READ_LINE_MEMBER(hp_taco_device::flg_r)
{
		return m_flg;
}

READ_LINE_MEMBER(hp_taco_device::sts_r)
{
		return m_sts;
}

// device_start
void hp_taco_device::device_start()
{
		LOG(("device_start"));
		m_irq_handler.resolve_safe();
		m_flg_handler.resolve_safe();
		m_sts_handler.resolve_safe();

		save_item(NAME(m_data_reg));
		save_item(NAME(m_data_reg_full));
		save_item(NAME(m_cmd_reg));
		//save_item(NAME(m_cmd_state));
		save_item(NAME(m_status_reg));
		save_item(NAME(m_tach_reg));
		save_item(NAME(m_tach_reg_ref));
		save_item(NAME(m_tach_reg_frozen));
		save_item(NAME(m_checksum_reg));
		save_item(NAME(m_clear_checksum_reg));
		save_item(NAME(m_timing_reg));
		save_item(NAME(m_irq));
		save_item(NAME(m_flg));
		save_item(NAME(m_sts));
		save_item(NAME(m_tape_pos));
		save_item(NAME(m_start_time));
		save_item(NAME(m_tape_fwd));
		save_item(NAME(m_tape_fast));
		save_item(NAME(m_image_dirty));
		save_item(NAME(m_tape_wr));
		save_item(NAME(m_rw_pos));
		save_item(NAME(m_next_word));
		save_item(NAME(m_rd_it_valid));
		save_item(NAME(m_gap_detect_start));

		m_tape_timer = timer_alloc(TAPE_TMR_ID);
		m_hole_timer = timer_alloc(HOLE_TMR_ID);
		m_timeout_timer = timer_alloc(TIMEOUT_TMR_ID);
}

// device_stop
void hp_taco_device::device_stop()
{
}

// device_reset
void hp_taco_device::device_reset()
{
		LOG(("device_reset"));
		clear_state();

		m_irq = false;
		m_flg = true;

		m_irq_handler(false);
		m_flg_handler(true);
		set_error(false);

		m_tape_timer->reset();
		m_hole_timer->reset();
		m_timeout_timer->reset();
}

void hp_taco_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
		update_tape_pos();

		switch (id) {
		case TAPE_TMR_ID:
				LOG_0(("Tape tmr @%g cmd %02x st %d\n" , machine().time().as_double() , CMD_CODE(m_cmd_reg) , m_cmd_state));

				cmd_fsm();
				break;

		case HOLE_TMR_ID:
				LOG_0(("Hole tmr @%g cmd %02x st %d\n" , machine().time().as_double() , CMD_CODE(m_cmd_reg) , m_cmd_state));

				BIT_SET(m_status_reg , STATUS_HOLE_BIT);

				if (m_cmd_state >= CMD_PH0 && m_cmd_state <= CMD_END) {
						switch (CMD_CODE(m_cmd_reg)) {
						case CMD_FINAL_GAP:
						case CMD_WRITE_IRG:
							m_image.write_gap(current_track() , m_rw_pos , m_tape_pos);
								m_rw_pos = m_tape_pos;
								break;

						case CMD_INIT_WRITE:
						case CMD_RECORD_WRITE:
						case CMD_MOVE_INGAP:
								m_hole_timer->adjust(time_to_next_hole());
								// No IRQ at holes
								return;

						case CMD_DELTA_MOVE_IRG:
								// Hit hole before end of programmed pulses
								terminate_cmd_now();
								update_tach_reg();
								freeze_tach_reg(true);
								return;

						case CMD_INDTA_INGAP:
						case CMD_INGAP_MOVE:
						case CMD_NOT_INDTA:
						case CMD_SCAN_RECORDS:
							// Commands are terminated at first hole (and failure is reported)
							terminate_cmd_now();
							set_error(true);
							return;

						case CMD_START_READ:
						case CMD_END_READ:
							// Commands report failure at first hole
							set_error(true);
							break;

						default:
								// Other cmds: default processing (update tape pos, set IRQ, schedule timer for next hole)
								break;
						}

						irq_w(true);
				}
				m_hole_timer->adjust(time_to_next_hole());
				break;

		case TIMEOUT_TMR_ID:
			LOG_0(("T/O tmr @%g cmd %02x st %d\n" , machine().time().as_double() , CMD_CODE(m_cmd_reg) , m_cmd_state));
			switch (CMD_CODE(m_cmd_reg)) {
			case CMD_START_READ:
				if (m_cmd_state == CMD_PH1) {
					irq_w(true);
				}
				break;

			default:
				// Most commands are terminated with failure on data T/O
				terminate_cmd_now();
				break;
			}
			set_error(true);
			break;

		default:
				break;
		}
}

void hp_taco_device::clear_state(void)
{
		m_data_reg = 0;
		m_data_reg_full = false;
		m_cmd_reg = 0;
		m_status_reg = 0;
		m_tach_reg = 0;
		m_tach_reg_ref = m_tape_pos;
		m_tach_reg_frozen = true;
		m_checksum_reg = 0;
		m_clear_checksum_reg = false;
		m_timing_reg = 0;
		m_cmd_state = CMD_IDLE;
		m_tape_pos = TAPE_INIT_POS;
		m_start_time = attotime::never;
		m_tape_fwd = false;
		m_tape_fast = false;
		// m_image_dirty is not touched
		m_tape_wr = false;
		m_rw_pos = 0;
		m_next_word = 0;
		m_rd_it_valid = false;
		m_gap_detect_start = hti_format_t::NULL_TAPE_POS;

		set_tape_present(false);
		set_tape_present(is_loaded());
}

void hp_taco_device::irq_w(bool state)
{
		if (state != m_irq) {
				m_irq = state;
				m_irq_handler(state);
				LOG_0(("IRQ = %d\n" , state));
		}
}

void hp_taco_device::set_error(bool state)
{
		m_sts = !state;
		m_sts_handler(m_sts);
		LOG_0(("error = %d\n" , state));
}

bool hp_taco_device::is_braking(void) const
{
		return m_cmd_state == CMD_INVERTING || m_cmd_state == CMD_STOPPING;
}

unsigned hp_taco_device::speed_to_tick_freq(void) const
{
		return is_braking() ?
				(m_tape_fast ? TACH_FREQ_BRAKE_FAST * TAPE_POS_FRACT : TACH_FREQ_BRAKE_SLOW * TAPE_POS_FRACT) :
				(m_tape_fast ? TACH_FREQ_FAST * TAPE_POS_FRACT : TACH_FREQ_SLOW * TAPE_POS_FRACT);
}

hti_format_t::tape_pos_t hp_taco_device::current_tape_pos(void) const
{
		if (m_start_time.is_never()) {
				// Tape not moving
				return m_tape_pos;
		}

		attotime delta_time(machine().time() - m_start_time);
		LOG_0(("delta_time = %g\n" , delta_time.as_double()));
		// How many tachometer ticks has the tape moved?
		hti_format_t::tape_pos_t delta_tach = (hti_format_t::tape_pos_t)(delta_time.as_ticks(speed_to_tick_freq()));
		LOG_0(("delta_tach = %u\n" , delta_tach));

		auto tape_pos = m_tape_pos;
		if (!hti_format_t::pos_offset(tape_pos , m_tape_fwd , delta_tach)) {
				LOG(("Tape unspooled!\n"));
		}

		return tape_pos;
}

void hp_taco_device::update_tape_pos(void)
{
		if (m_start_time.is_never()) {
				// Tape not moving
				return;
		}

		m_tape_pos = current_tape_pos();
		m_start_time = machine().time();
		LOG_0(("Tape pos = %u\n" , m_tape_pos));

		// Gap detection
		bool gap_detected = false;
		if (m_gap_detect_start != hti_format_t::NULL_TAPE_POS && abs(m_gap_detect_start - m_tape_pos) >= min_gap_size()) {
				auto tmp = m_tape_pos;
				hti_format_t::pos_offset(tmp , m_tape_fwd , -min_gap_size());
				gap_detected = m_image.just_gap(current_track() , tmp , m_tape_pos);
		}
		if (gap_detected) {
				BIT_SET(m_status_reg, STATUS_GAP_BIT);
		} else {
				BIT_CLR(m_status_reg, STATUS_GAP_BIT);
		}
		// Tach register update
		update_tach_reg();
}

void hp_taco_device::update_tach_reg(void)
{
		if (m_tach_reg_frozen) {
				LOG_0(("Tach reg frozen\n"));
				return;
		}

		auto pos = current_tape_pos();
		hti_format_t::tape_pos_t pos_int = pos / TAPE_POS_FRACT;
		hti_format_t::tape_pos_t ref_int = m_tach_reg_ref / TAPE_POS_FRACT;
		uint16_t reg_value = (uint16_t)(abs(pos_int - ref_int) + m_tach_reg);

		LOG_0(("Tach = %04x @ pos = %d, ref_pos = %d\n" , reg_value , pos , m_tach_reg_ref));

		m_tach_reg = reg_value;
		m_tach_reg_ref = pos;
}

void hp_taco_device::freeze_tach_reg(bool freeze)
{
		if (freeze) {
				m_tach_reg_frozen = true;
		} else {
				m_tach_reg_frozen = false;
				m_tach_reg_ref = current_tape_pos();
		}

}

attotime hp_taco_device::time_to_distance(hti_format_t::tape_pos_t distance) const
{
		// +1 for rounding
		return attotime::from_ticks(distance + 1 , speed_to_tick_freq());
}

attotime hp_taco_device::time_to_target(hti_format_t::tape_pos_t target) const
{
		return time_to_distance(abs(target - m_tape_pos));
}

attotime hp_taco_device::time_to_stopping_pos(void) const
{
		return time_to_distance(m_tape_fast ? FAST_BRAKE_DIST : SLOW_BRAKE_DIST);
}

bool hp_taco_device::start_tape_cmd(uint16_t cmd_reg , uint16_t must_be_1 , uint16_t must_be_0)
{
		m_cmd_reg = cmd_reg;

		uint16_t to_be_tested = (m_cmd_reg & CMD_REG_MASK) | (m_status_reg & STATUS_REG_MASK);
		// Bits in STATUS_ERR_MASK must always be 0
		must_be_0 |= STATUS_ERR_MASK;

		// It's not an error if the error state is already set (sts false)
		if (((to_be_tested & (must_be_1 | must_be_0)) ^ must_be_1) != 0) {
				return false;
		} else {
				bool prev_tape_wr = m_tape_wr;
				bool prev_tape_fwd = m_tape_fwd;
				bool prev_tape_fast = m_tape_fast;
				bool prev_tape_braking = is_braking();
				bool not_moving = m_start_time.is_never();

				m_start_time = machine().time();
				m_tape_wr = (must_be_0 & STATUS_WPR_MASK) != 0;
				m_tape_fwd = DIR_FWD(m_cmd_reg);
				m_tape_fast = SPEED_FAST(m_cmd_reg);

				if (m_tape_wr) {
						// Write command: disable gap detector
						m_gap_detect_start = hti_format_t::NULL_TAPE_POS;
						BIT_CLR(m_status_reg, STATUS_GAP_BIT);
						m_image_dirty = true;
				} else if (not_moving || prev_tape_braking || prev_tape_wr != m_tape_wr || prev_tape_fwd != m_tape_fwd || prev_tape_fast != m_tape_fast) {
						// Tape started or re-started right now, switched from writing to reading, direction changed or speed changed: (re)start gap detector
						m_gap_detect_start = m_tape_pos;
						BIT_CLR(m_status_reg, STATUS_GAP_BIT);
				}

				if (!not_moving && prev_tape_fwd != m_tape_fwd) {
						// Tape direction inverted, stop tape before executing command
						m_tape_fwd = prev_tape_fwd;
						m_tape_fast = prev_tape_fast;
						m_cmd_state = CMD_INVERTING;
						LOG_0(("Direction reversed! fwd = %d fast = %d\n" , m_tape_fwd , m_tape_fast));
						if (!prev_tape_braking) {
							m_tape_timer->adjust(time_to_stopping_pos());
						}
				} else {
						// No change in direction, immediate execution
						m_cmd_state = CMD_PH0;
						m_tape_timer->adjust(attotime::zero);
				}

				m_hole_timer->reset();
				m_timeout_timer->reset();
				return true;
		}
}

void hp_taco_device::stop_tape(void)
{
		m_start_time = attotime::never;
		m_gap_detect_start = hti_format_t::NULL_TAPE_POS;
}

unsigned hp_taco_device::current_track(void)
{
		return BIT(m_status_reg , STATUS_TRACKB_BIT);
}

attotime hp_taco_device::fetch_next_wr_word(void)
{
		if (m_data_reg_full) {
				m_next_word = m_data_reg;
				m_data_reg_full = false;
				LOG_0(("next %04x (DR)\n" , m_next_word));
		} else {
				// When data register is empty, write checksum word
				m_next_word = m_checksum_reg;
				LOG_0(("next %04x (CS)\n" , m_next_word));
		}
		if (m_clear_checksum_reg) {
			m_checksum_reg = 0;
			m_clear_checksum_reg = false;
		}
		// Update checksum with new word
		m_checksum_reg += m_next_word;

		return time_to_distance(hti_format_t::word_length(m_next_word));
}

attotime hp_taco_device::time_to_rd_next_word(hti_format_t::tape_pos_t& word_rd_pos)
{
		if (m_rd_it_valid) {
			word_rd_pos = hti_format_t::farthest_end(m_rd_it , m_tape_fwd);
				return time_to_target(word_rd_pos);
		} else {
				return attotime::never;
		}
}

hti_format_t::tape_pos_t hp_taco_device::min_gap_size(void) const
{
		return LONG_GAP(m_cmd_reg) ? LONG_GAP_LENGTH : SHORT_GAP_LENGTH;
}

void hp_taco_device::set_tape_present(bool present)
{
		if (present) {
				if (is_readonly()) {
						BIT_SET(m_status_reg, STATUS_WPR_BIT);
				} else {
						BIT_CLR(m_status_reg, STATUS_WPR_BIT);
				}
				// STATUS_CART_OUT_BIT is reset by CMD_CLEAR
		} else {
				BIT_SET(m_status_reg, STATUS_CART_OUT_BIT);
				BIT_SET(m_status_reg, STATUS_WPR_BIT);
		}
}

attotime hp_taco_device::time_to_next_hole(void) const
{
	auto pos = hti_format_t::next_hole(m_tape_pos , m_tape_fwd);

	if (pos == hti_format_t::NULL_TAPE_POS) {
		return attotime::never;
	} else {
		return time_to_target(pos);
	}
}

attotime hp_taco_device::time_to_tach_pulses(void) const
{
		return time_to_distance((hti_format_t::tape_pos_t)(0x10000U - m_tach_reg) * TAPE_POS_FRACT);
}

void hp_taco_device::terminate_cmd_now(void)
{
		m_cmd_state = CMD_END;
		m_tape_timer->adjust(attotime::zero);
		m_hole_timer->reset();
		m_timeout_timer->reset();
}

void hp_taco_device::set_data_timeout(bool long_timeout)
{
	attotime timeout = time_to_distance(long_timeout ? PREAMBLE_TIMEOUT : DATA_TIMEOUT);
	m_timeout_timer->adjust(timeout , 0 , timeout);
}

void hp_taco_device::cmd_fsm(void)
{
		if (m_cmd_state == CMD_END) {
				// Command ended
				m_cmd_state = CMD_IDLE;
				m_hole_timer->reset();
				m_timeout_timer->reset();
				irq_w(true);
				if (AUTO_STOP(m_cmd_reg)) {
						// Automatic stop after command execution
						LOG_0(("Tape clamped\n"));
						m_cmd_state = CMD_STOPPING;
						m_tape_timer->adjust(time_to_stopping_pos());
				}
		} else if (m_cmd_state == CMD_STOPPING) {
				// Braking phase ended
				m_cmd_state = CMD_IDLE;
				stop_tape();
				if (CMD_CODE(m_cmd_reg) == CMD_STOP) {
						irq_w(true);
				}
		} else {
				attotime cmd_duration = attotime::never;

				if (m_cmd_state == CMD_INVERTING) {
						m_tape_fwd = DIR_FWD(m_cmd_reg);
						m_tape_fast = SPEED_FAST(m_cmd_reg);
						m_cmd_state = CMD_PH0;
				}

				if (m_cmd_state == CMD_PH0) {
						m_hole_timer->adjust(time_to_next_hole());
				}

				switch (CMD_CODE(m_cmd_reg)) {
				case CMD_FINAL_GAP:
						if (m_cmd_state == CMD_PH0) {
								// PH0
								m_rw_pos = m_tape_pos;
								cmd_duration = time_to_distance(END_GAP_LENGTH);
								m_cmd_state = CMD_PH1;
						} else {
								// PH1
							m_image.write_gap(current_track() , m_rw_pos , m_tape_pos);
								cmd_duration = attotime::zero;
								m_cmd_state = CMD_END;
						}
						break;

				case CMD_RECORD_WRITE:
						if (m_cmd_state == CMD_PH0) {
								// PH0
								// Search for preamble first
							m_rd_it_valid = m_image.next_data(current_track() , m_tape_pos , m_tape_fwd , false , m_rd_it);
								cmd_duration = time_to_rd_next_word(m_rw_pos);
								// Set T/O for preamble search
								set_data_timeout(true);
								m_cmd_state = CMD_PH1;
								break;
						} else if (m_cmd_state == CMD_PH1) {
								// PH1
								if (m_rd_it->second == PREAMBLE_WORD) {
										LOG_0(("Got preamble\n"));
										m_cmd_state = CMD_PH2;
										// m_rw_pos already at correct position
										cmd_duration = fetch_next_wr_word();
										m_timeout_timer->reset();
										irq_w(true);
								} else {
									auto res = m_image.adv_it(current_track() , m_tape_fwd , m_rd_it);
										if (res != hti_format_t::ADV_NO_MORE_DATA) {
												cmd_duration = time_to_rd_next_word(m_rw_pos);
										}
										// Set T/O for arrival of data words
										set_data_timeout(false);
								}
								break;
						}
						// Intentional fall-through on PH2

				case CMD_INIT_WRITE:
						if (m_cmd_state == CMD_PH0) {
								// PH0
								m_next_word = PREAMBLE_WORD;
								m_rw_pos = m_tape_pos;
								cmd_duration = time_to_distance(hti_format_t::word_length(m_next_word));
								m_cmd_state = CMD_PH1;
						} else {
								// PH1 & PH2 of CMD_RECORD_WRITE
								hti_format_t::tape_pos_t length;
								m_image.write_word(current_track() , m_rw_pos , m_next_word , length);
								hti_format_t::pos_offset(m_rw_pos , m_tape_fwd , length);
								// Just to be sure..
								m_tape_pos = m_rw_pos;
								cmd_duration = fetch_next_wr_word();
								irq_w(true);
						}
						break;

				case CMD_SET_TRACK:
						// PH0
						// When b9 is 0, set track A/B
						// When b9 is 1, ignore command (in TACO chip it has an unknown purpose)
						if (!AUTO_STOP(m_cmd_reg)) {
								if (CMD_OPT(m_cmd_reg)) {
										BIT_SET(m_status_reg, STATUS_TRACKB_BIT);
								} else {
										BIT_CLR(m_status_reg, STATUS_TRACKB_BIT);
								}
						}
						cmd_duration = attotime::from_usec(QUICK_CMD_USEC);
						m_hole_timer->reset();
						m_cmd_state = CMD_END;
						break;

				case CMD_MOVE:
						// PH0
						// Endless movement: not setting cmd_duration
						m_cmd_state = CMD_END;
						break;

				case CMD_INGAP_MOVE:
						if (m_cmd_state == CMD_PH0) {
								// PH0
							auto target = m_tape_pos;
								if (m_image.next_gap(current_track() , target, m_tape_fwd, min_gap_size())) {
										LOG_0(("IRG @%d\n" , target));
										cmd_duration = time_to_target(target);
								}
								m_cmd_state = CMD_PH1;
						} else {
								// PH1
								cmd_duration = time_to_tach_pulses();
								freeze_tach_reg(false);
								m_cmd_state = CMD_END;
						}
						break;

				case CMD_INDTA_INGAP:
				case CMD_NOT_INDTA:
						if (m_cmd_state == CMD_PH0) {
								// PH0
							if (m_image.next_data(current_track() , m_tape_pos , m_tape_fwd , true , m_rd_it)) {
									cmd_duration = time_to_target(hti_format_t::farthest_end(m_rd_it , m_tape_fwd));
								}
								// Set T/O for data
								set_data_timeout(true);
								m_cmd_state = CMD_PH1;
						} else {
								// PH1
								auto target = m_tape_pos;
								if (m_image.next_gap(current_track() , target, m_tape_fwd, min_gap_size())) {
										LOG_0(("End of data @%d\n" , target));
										cmd_duration = time_to_target(target);
								}
								// Got data, stop T/O
								m_timeout_timer->reset();
								m_cmd_state = CMD_END;
						}
						break;

				case CMD_WRITE_IRG:
						if (m_cmd_state == CMD_PH0) {
								// PH0
								freeze_tach_reg(false);
								m_rw_pos = m_tape_pos;
								cmd_duration = time_to_tach_pulses();
								m_cmd_state = CMD_PH1;
						} else {
								// PH1
							m_image.write_gap(current_track() , m_rw_pos , m_tape_pos);
								cmd_duration = attotime::zero;
								m_cmd_state = CMD_END;
						}
						break;

				case CMD_SCAN_RECORDS:
						if (m_cmd_state == CMD_PH0 || m_cmd_state == CMD_PH2) {
							// PH0 and PH2
							if (m_cmd_state == CMD_PH2) {
								m_tach_reg++;
								if (m_tach_reg == 0) {
									// All gaps found, bail out
									cmd_duration = attotime::zero;
									m_cmd_state = CMD_END;
									break;
								}
							}
							if (m_image.next_data(current_track() , m_tape_pos , m_tape_fwd , true , m_rd_it)) {
								cmd_duration = time_to_target(hti_format_t::farthest_end(m_rd_it , m_tape_fwd));
							}
							// Set T/O for data
							set_data_timeout(true);
							m_cmd_state = CMD_PH1;
						} else if (m_cmd_state == CMD_PH1) {
							// PH1
							auto target = m_tape_pos;
							if (m_image.next_gap(current_track() , target, m_tape_fwd, min_gap_size())) {
								LOG_0(("Gap @%d (%u to go)\n" , target , 0x10000U - m_tach_reg));
								cmd_duration = time_to_target(target);
							}
							m_timeout_timer->reset();
							m_cmd_state = CMD_PH2;
						}
						break;

				case CMD_MOVE_INDTA:
						if (m_cmd_state == CMD_PH0) {
								// PH0
								freeze_tach_reg(false);
								cmd_duration = time_to_tach_pulses();
								m_cmd_state = CMD_PH1;
						} else {
								// PH1
							if (m_image.next_data(current_track() , m_tape_pos , m_tape_fwd , true , m_rd_it)) {
									cmd_duration = time_to_target(hti_format_t::farthest_end(m_rd_it , m_tape_fwd));
								}
								// Apparently this cmd doesn't set no-data T/O
								m_cmd_state = CMD_END;
						}
						break;

				case CMD_UNK_1b:
						// PH0
						// Unknown purpose, but make it a NOP (it's used in "T" test of test ROM)
						cmd_duration = attotime::from_usec(QUICK_CMD_USEC);
						m_cmd_state = CMD_END;
						break;

				case CMD_MOVE_INGAP:
						if (m_cmd_state == CMD_PH0) {
								// PH0
								freeze_tach_reg(false);
								cmd_duration = time_to_tach_pulses();
								m_cmd_state = CMD_PH1;
						} else {
								// PH1
								auto target = m_tape_pos;
								if (m_image.next_gap(current_track() , target, m_tape_fwd, min_gap_size())) {
										LOG_0(("GAP @%d\n" , target));
										cmd_duration = time_to_target(target);
								}
								m_cmd_state = CMD_END;
						}
						break;

				case CMD_START_READ:
						if (m_cmd_state == CMD_PH0) {
								// PH0
								// Should also check if tape position has gone too far to read word @ m_rd_it
								if (!m_rd_it_valid) {
										// Search for preamble first
									m_rd_it_valid = m_image.next_data(current_track() , m_tape_pos , m_tape_fwd , false , m_rd_it);
										// Set T/O for preamble search
										set_data_timeout(true);

										m_cmd_state = CMD_PH1;
								} else {
										// Resume reading from last position, skip preamble search
										m_cmd_state = CMD_PH2;
								}
						} else {
								// Just to be sure..
								m_tape_pos = m_rw_pos;

								if (m_cmd_state == CMD_PH1) {
										// PH1
										// Any word with at least a 0 will do as preamble.
										// But anything that's not the correct preamble word (0) will cause a wrong alignment
										// with word boundaries. TACO will read garbage data in this case (this effect is not simulated here).
										if (m_rd_it->second != 0xffff) {
												m_cmd_state = CMD_PH2;
												LOG_0(("Got preamble %04x\n" , m_rd_it->second));
										}
								} else {
										// PH2
									if (m_irq) {
										LOG(("Data reg overflow!\n"));
									}
										irq_w(true);
										m_data_reg = m_rd_it->second;
										if (m_clear_checksum_reg) {
												m_checksum_reg = 0;
												m_clear_checksum_reg = false;
										}
										m_checksum_reg += m_data_reg;
										LOG_0(("RD %04x\n" , m_data_reg));
								}
								// Set T/O for arrival of data words
								set_data_timeout(false);
								auto res = m_image.adv_it(current_track() , m_tape_fwd , m_rd_it);
								LOG_0(("adv_it %d\n" , res));
								if (res == hti_format_t::ADV_NO_MORE_DATA) {
										m_rd_it_valid = false;
								}
						}
						cmd_duration = time_to_rd_next_word(m_rw_pos);
						break;

				case CMD_DELTA_MOVE_IRG:
						// PH0
						freeze_tach_reg(false);
						cmd_duration = time_to_tach_pulses();
						m_cmd_state = CMD_END;
						break;

				case CMD_END_READ:
						if (m_cmd_state == CMD_PH0) {
								// PH0
								cmd_duration = time_to_rd_next_word(m_rw_pos);
								set_data_timeout(false);
								m_cmd_state = CMD_PH1;
						} else {
								// PH1
								m_tape_pos = m_rw_pos;
								// Note: checksum is not updated
								m_data_reg = m_rd_it->second;
								LOG_0(("Final RD %04x\n" , m_data_reg));
								auto res = m_image.adv_it(current_track() , m_tape_fwd , m_rd_it);
								if (res == hti_format_t::ADV_NO_MORE_DATA) {
										m_rd_it_valid = false;
								}
								cmd_duration = attotime::zero;
								m_cmd_state = CMD_END;
						}
						break;

				default:
						break;
				}

				m_tape_timer->adjust(cmd_duration);
		}
}

void hp_taco_device::start_cmd_exec(uint16_t new_cmd_reg)
{
		LOG_0(("New cmd %02x @ %g cmd %02x st %d\n" , CMD_CODE(new_cmd_reg) , machine().time().as_double() , CMD_CODE(m_cmd_reg) , m_cmd_state));

		update_tape_pos();

		unsigned new_cmd_code = CMD_CODE(new_cmd_reg);

		if (new_cmd_code != CMD_START_READ &&
			new_cmd_code != CMD_END_READ &&
			new_cmd_code != CMD_CLEAR) {
				m_rd_it_valid = false;
		}

		bool started = false;

		switch (new_cmd_code) {
		case CMD_INDTA_INGAP:
				// Errors: CART OUT,FAST SPEED
				started = start_tape_cmd(new_cmd_reg , 0 , SPEED_FAST_MASK);
				break;

		case CMD_FINAL_GAP:
				// Errors: WP,CART OUT
				started = start_tape_cmd(new_cmd_reg , 0 , STATUS_WPR_MASK);
				break;

		case CMD_INIT_WRITE:
				// Errors: WP,CART OUT,fast speed,reverse
				started = start_tape_cmd(new_cmd_reg , DIR_FWD_MASK , STATUS_WPR_MASK | SPEED_FAST_MASK);
				break;

		case CMD_STOP:
				m_cmd_reg = new_cmd_reg;
				freeze_tach_reg(false);
				m_hole_timer->reset();
				m_timeout_timer->reset();
				if (is_braking()) {
						// Already braking
						// m_tape_timer already set
				} else if (m_start_time.is_never()) {
						// Tape is already stopped
						LOG_0(("Already stopped\n"));
						m_tape_timer->adjust(attotime::from_usec(QUICK_CMD_USEC));
				} else {
						// Start braking timer
						m_cmd_state = CMD_STOPPING;
						m_tape_timer->adjust(time_to_stopping_pos());
				}
				m_cmd_state = CMD_STOPPING;
				return;

		case CMD_SET_TRACK:
				// Don't know if this command really starts the tape or not (probably it doesn't)
				started = start_tape_cmd(new_cmd_reg , 0 , 0);
				break;

		case CMD_MOVE:
				started = start_tape_cmd(new_cmd_reg , 0 , 0);
				break;

		case CMD_INGAP_MOVE:
				// Errors: CART OUT,FAST SPEED
				started = start_tape_cmd(new_cmd_reg , 0 , SPEED_FAST_MASK);
				break;

		case CMD_CLEAR:
				set_error(false);
				BIT_CLR(m_status_reg, STATUS_HOLE_BIT);
				BIT_CLR(m_status_reg, STATUS_CART_OUT_BIT);
				BIT_CLR(m_status_reg, STATUS_WPR_BIT);
				set_tape_present(is_loaded());
				// This is a special command: it doesn't raise IRQ at completion and it
				// doesn't replace current command
				return;

		case CMD_NOT_INDTA:
				// Errors: CART OUT
				started = start_tape_cmd(new_cmd_reg , 0 , 0);
				break;

		case CMD_WRITE_IRG:
				// Errors: WP,CART OUT
				started = start_tape_cmd(new_cmd_reg , 0 , STATUS_WPR_MASK);
				break;

		case CMD_SCAN_RECORDS:
				// Errors: CART OUT
				started = start_tape_cmd(new_cmd_reg , 0 , 0);
				break;

		case CMD_RECORD_WRITE:
				// Errors: WP,CART OUT,fast speed,reverse
				started = start_tape_cmd(new_cmd_reg , DIR_FWD_MASK , STATUS_WPR_MASK | SPEED_FAST_MASK);
				break;

		case CMD_MOVE_INDTA:
				// Errors: CART OUT,FAST SPEED
				started = start_tape_cmd(new_cmd_reg , 0 , SPEED_FAST_MASK);
				break;

		case CMD_UNK_1b:
				started = start_tape_cmd(new_cmd_reg , 0 , 0);
				break;

		case CMD_MOVE_INGAP:
				started = start_tape_cmd(new_cmd_reg , 0 , 0);
				break;

		case CMD_START_READ:
				// Yes, you can read tape backwards: test "C" does that!
				// Because of this DIR_FWD_MASK is not in the "must be 1" mask.
				started = start_tape_cmd(new_cmd_reg , 0 , SPEED_FAST_MASK);
				break;

		case CMD_DELTA_MOVE_IRG:
				started = start_tape_cmd(new_cmd_reg , 0 , 0);
				break;

		case CMD_END_READ:
				// This command only makes sense after CMD_START_READ
				if (CMD_CODE(m_cmd_reg) == CMD_START_READ) {
						started = start_tape_cmd(new_cmd_reg , 0 , SPEED_FAST_MASK);
						LOG_0(("END_READ %d\n" , m_rd_it_valid));
				}
				break;

		default:
				LOG(("Unrecognized command\n"));
				started = false;
				break;
		}

		if (!started) {
				set_error(true);
				m_cmd_state = CMD_IDLE;
				m_tape_timer->reset();
				m_hole_timer->reset();
				m_timeout_timer->reset();
		}
}

image_init_result hp_taco_device::internal_load(bool is_create)
{
	device_reset();

	io_generic io;
	io.file = (device_image_interface *)this;
	io.procs = &image_ioprocs;
	io.filler = 0;
	if (is_create) {
		m_image.clear_tape();
		m_image.save_tape(&io);
	} else if (!m_image.load_tape(&io)) {
		seterror(IMAGE_ERROR_INVALIDIMAGE , "Wrong format");
		set_tape_present(false);
		return image_init_result::FAIL;
	}

	m_image_dirty = false;

	set_tape_present(true);
	return image_init_result::PASS;
}

image_init_result hp_taco_device::call_load()
{
	LOG(("call_load\n"));
	return internal_load(false);
}

image_init_result hp_taco_device::call_create(int format_type, util::option_resolution *format_options)
{
	LOG(("call_create\n"));
	return internal_load(true);
}

void hp_taco_device::call_unload()
{
	LOG(("call_unload dirty=%d\n" , m_image_dirty));

	device_reset();

	if (m_image_dirty) {
		io_generic io;
		io.file = (device_image_interface *)this;
		io.procs = &image_ioprocs;
		io.filler = 0;
		m_image.save_tape(&io);
		m_image_dirty = false;
	}

	m_image.clear_tape();
	set_tape_present(false);
}

std::string hp_taco_device::call_display()
{
	std::string buffer;
	// Mostly lifted from cassette_image_device::call_display ;)

	// Do not show anything if image not loaded or tape not moving
	if (!exists() || m_start_time.is_never()) {
		return buffer;
	}

	char track = BIT(m_status_reg , STATUS_TRACKB_BIT) ? 'B' : 'A';
	char r_w = m_tape_wr ? 'W' : 'R';
	char m1;
	char m2;

	if (m_tape_fwd) {
		m1 = '>';
		m2 = m_tape_fast ? '>' : ' ';
	} else {
		m1 = '<';
		m2 = m_tape_fast ? '<' : ' ';
	}

	int pos_in = current_tape_pos() / hti_format_t::ONE_INCH_POS;

	buffer = string_format("%c %c %c%c [%04d/1824]" , track , r_w , m1 , m2 , pos_in);

	// Not correct when there are 2 or more instances of TACO
	return buffer;
}

const char *hp_taco_device::file_extensions() const
{
		return "hti";
}
