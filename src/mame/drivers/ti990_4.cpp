// license:LGPL-2.1+
// copyright-holders:Michael Zapf
/*
    ti990/4 driver

    We emulate a basic ti990/4 board, with a FD800 disk controller and an 733
    ASR terminal.  A little floppy-based software for this computer is
    available thanks to efforts by Dave Pitts: mostly, Forth and TX990.


    Board setup options:
    8kb of DRAM (onboard option, with optional parity): base 0x0000 or 0x2000
    4 banks of 512 bytes of ROM or SRAM: base 0x0000, 0x0800, 0xF000 or 0xF800
    power-up vector: 0x0000 (level 0) or 0xFFFC (load)
    optional memerr interrupt (level 2)
    optional power fail interrupt (level 1)
    optional real-time clock interrupt (level 5 or 7)


    Setup for the emulated system:
    0x0000: 8kb on-board DRAM + 24kb extension RAM (total 32kb)
    0xF800: 512 bytes SRAM
    0xFA00: 512 bytes SRAM (or empty?)
    0xFC00: 512 bytes self-test ROM
    0xFE00: 512 bytes loader ROM
    power-up vector: 0xFFFC (load)

    Note that only interrupt levels 3-7 are supported by the board (8-15 are not wired).

TODO:
* finish ASR emulation
* programmer panel
* emulate other devices: card reader, printer

    Original implementation: Raphael Nabet

    Rewritten by Michael Zapf 2014
*/

#include "emu.h"
#include "cpu/tms9900/tms9900.h"

#include "video/911_vdt.h"
#include "sound/beep.h"
#include "video/733_asr.h"

#include "bus/ti99x/990_dk.h"


class ti990_4_state : public driver_device
{
public:
	ti990_4_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_fd800(*this, "fd800") { }

	void ti990_4v(machine_config &config);
	void ti990_4(machine_config &config);

	void init_ti990_4();
	void init_ti990_4v();

private:
	DECLARE_READ8_MEMBER( panel_read );
	DECLARE_WRITE8_MEMBER( panel_write );
	DECLARE_WRITE8_MEMBER( external_operation );
	DECLARE_READ8_MEMBER( interrupt_level );
	DECLARE_WRITE_LINE_MEMBER( fd_interrupt );
	DECLARE_WRITE_LINE_MEMBER( asrkey_interrupt );
	DECLARE_WRITE_LINE_MEMBER( vdtkey_interrupt );
	DECLARE_WRITE_LINE_MEMBER( line_interrupt );

	DECLARE_MACHINE_RESET(ti990_4);

	void crumap(address_map &map);
	void crumap_v(address_map &map);
	void memmap(address_map &map);

	void        hold_load();
	void        device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	int         m_intlines;
	int         m_int_level;
	emu_timer*  m_nmi_timer;
	void        reset_int_lines();
	void        set_int_line(int line, int state);

	bool        m_ckon_state;

	// Connected devices
	required_device<tms9900_device>     m_maincpu;
	required_device<fd800_legacy_device> m_fd800;
};

enum
{
	NMI_TIMER_ID = 1
};

void ti990_4_state::hold_load()
{
	m_maincpu->set_input_line(INT_9900_LOAD, ASSERT_LINE);
	logerror("ti990_4: Triggering LOAD interrupt\n");
	m_nmi_timer->adjust(attotime::from_msec(100));
}

/*
    LOAD interrupt trigger callback
*/
void ti990_4_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	m_maincpu->set_input_line(INT_9900_LOAD, CLEAR_LINE);
	logerror("ti990_4: Released LOAD interrupt\n");
}

READ8_MEMBER( ti990_4_state::panel_read )
{
	if (offset == 11 || offset == 14)
		return 1;

	return 0;
}

WRITE8_MEMBER( ti990_4_state::panel_write )
{
	logerror("ti990_4: writing to panel @CRU %04x: %02x\n", offset<<1, data);
}

void ti990_4_state::set_int_line(int line, int state)
{
	if (state)
		m_intlines |= (1 << line);
	else
		m_intlines &= ~(1 << line);

	if (m_intlines != 0)
	{
		m_int_level = 0;
		while ((m_intlines & (1 << m_int_level))==0) m_int_level++;
		logerror("ti990_4: Setting int level to %x\n", m_int_level);
		m_maincpu->set_input_line(INT_9900_INTREQ, ASSERT_LINE);
	}
	else
		m_maincpu->set_input_line(INT_9900_INTREQ, CLEAR_LINE);
}

void ti990_4_state::reset_int_lines()
{
	m_intlines = 0;
}

/*
    Callback from the floppy controller.
*/
WRITE_LINE_MEMBER(ti990_4_state::fd_interrupt)
{
	set_int_line(7, state);
}

/*
    Connection to VDT
*/
WRITE_LINE_MEMBER(ti990_4_state::vdtkey_interrupt)
{
	set_int_line(3, state);
}

WRITE_LINE_MEMBER(ti990_4_state::line_interrupt)
{
	if (m_ckon_state) set_int_line(5, state);
}

/*
    Callback from the terminal.
*/
WRITE_LINE_MEMBER(ti990_4_state::asrkey_interrupt)
{
	set_int_line(6, state);
}

WRITE8_MEMBER( ti990_4_state::external_operation )
{
	static char const *const extop[8] = { "inv1", "inv2", "IDLE", "RSET", "inv3", "CKON", "CKOF", "LREX" };
	switch (offset)
	{
	case IDLE_OP:
		return;
	case RSET_OP:
		m_ckon_state = false;
		// clear controller panel and smi fault LEDs
		break;
	case CKON_OP:
		m_ckon_state = true;
		break;
	case CKOF_OP:
		m_ckon_state = false;
		set_int_line(5, 0);
		break;
	case LREX_OP:
		// TODO: Verify this
		hold_load();
		break;

	default:
		logerror("ti99_4x: External operation %s not implemented on TI-990/4 board\n", extop[offset]);
	}
}

READ8_MEMBER( ti990_4_state::interrupt_level )
{
	return m_int_level;
}

/*
    Memory map - see description above
*/

void ti990_4_state::memmap(address_map &map)
{
	map(0x0000, 0x7fff).ram(); /* dynamic RAM */
	map(0x8000, 0xf7ff).noprw(); /* reserved for expansion */
	map(0xf800, 0xfbff).ram(); /* static RAM? */
	map(0xfc00, 0xffff).rom(); /* LOAD ROM */
}


/*
    CRU map

    0x0000-0x1EFF: user devices
    0x1F00-0x1F3F: CRU interrupt + expansion control
    0x1F40-0x1F5F: TILINE coupler interrupt control
    0x1F60-0x1F9F: reserved
    0x1FA0-0x1FBF: memory mapping and memory protect
    0x1FC0-0x1FDF: internal interrupt control
    0x1FF0-0x1FFF: front panel

    Default user map:
    0x0000-0x001f: 733 ASR (int 6)
    0x0020-0x003f: PROM programmer (wired to int 15, unused)
    0x0040-0x005f: 804 card reader (int 4)
    0x0060-0x007f: line printer (wired to int 14, unused)
    0x0080-0x00bf: FD800 floppy controller (int 7)
    0x00c0-0x00ff: VDT1 (int 3 - wired to int 11, unused)
    0x0100-0x013f: VDT2, or CRU expansion (int ??? - wired to int 10, unused)
    0x0140-0x017f: VDT3 (int ??? - wired to int 9, unused)
*/

void ti990_4_state::crumap(address_map &map)
{
	map(0x0000, 0x001f).rw("asr733", FUNC(asr733_device::cru_r), FUNC(asr733_device::cru_w));
	map(0x0080, 0x00bf).rw(m_fd800, FUNC(fd800_legacy_device::cru_r), FUNC(fd800_legacy_device::cru_w));
	map(0x1fe0, 0x1fff).rw(FUNC(ti990_4_state::panel_read), FUNC(ti990_4_state::panel_write));
}

void ti990_4_state::crumap_v(address_map &map)
{
	map(0x0080, 0x00bf).rw(m_fd800, FUNC(fd800_legacy_device::cru_r), FUNC(fd800_legacy_device::cru_w));
	map(0x0100, 0x011f).rw("vdt911", FUNC(vdt911_device::cru_r), FUNC(vdt911_device::cru_w));
	map(0x1fe0, 0x1fff).rw(FUNC(ti990_4_state::panel_read), FUNC(ti990_4_state::panel_write));
}


/* static const floppy_interface ti990_4_floppy_interface =
{
    FLOPPY_STANDARD_8_DSSD,
    LEGACY_FLOPPY_OPTIONS_NAME(fd800),
    nullptr
}; */

MACHINE_RESET_MEMBER(ti990_4_state,ti990_4)
{
	hold_load();
	reset_int_lines();
	m_ckon_state = false;
	m_maincpu->set_ready(ASSERT_LINE);
}

void ti990_4_state::init_ti990_4()
{
	m_nmi_timer = timer_alloc(NMI_TIMER_ID);
}

void ti990_4_state::ti990_4(machine_config &config)
{
	/* basic machine hardware */
	/* TMS9900 CPU @ 3.0(???) MHz */
	TMS9900(config, m_maincpu, 3000000);
	m_maincpu->set_addrmap(AS_PROGRAM, &ti990_4_state::memmap);
	m_maincpu->set_addrmap(AS_IO, &ti990_4_state::crumap);
	m_maincpu->extop_cb().set(FUNC(ti990_4_state::external_operation));
	m_maincpu->intlevel_cb().set(FUNC(ti990_4_state::interrupt_level));

	MCFG_MACHINE_RESET_OVERRIDE(ti990_4_state, ti990_4 )

	// Terminal
	asr733_device& term(ASR733(config, "asr733", 0));
	term.keyint_cb().set(FUNC(ti990_4_state::asrkey_interrupt));
	term.lineint_cb().set(FUNC(ti990_4_state::line_interrupt));

	// Floppy controller
	TI99X_FD800(config, "fd800", 0).int_cb().set(FUNC(ti990_4_state::fd_interrupt));

	//  TODO: Add floppy drives
}

void ti990_4_state::ti990_4v(machine_config &config)
{
	/* basic machine hardware */
	/* TMS9900 CPU @ 3.0(???) MHz */
	TMS9900(config, m_maincpu, 3000000);
	m_maincpu->set_addrmap(AS_PROGRAM, &ti990_4_state::memmap);
	m_maincpu->set_addrmap(AS_IO, &ti990_4_state::crumap_v);
	m_maincpu->extop_cb().set(FUNC(ti990_4_state::external_operation));
	m_maincpu->intlevel_cb().set(FUNC(ti990_4_state::interrupt_level));

	// VDT 911 terminal
	vdt911_device& term(VDT911(config, "vdt911", 0));
	term.keyint_cb().set(FUNC(ti990_4_state::vdtkey_interrupt));
	term.lineint_cb().set(FUNC(ti990_4_state::line_interrupt));

	// Floppy controller
	TI99X_FD800(config, "fd800", 0).int_cb().set(FUNC(ti990_4_state::fd_interrupt));

	//  TODO: Add floppy drives
}

/*
  ROM loading
*/
ROM_START(ti990_4)
	/*CPU memory space*/
	ROM_REGION16_BE(0x10000, "maincpu",0)

#if 0
	/* ROM set 945121-5: "733 ASR ROM loader with self test (prototyping)"
	(cf 945401-9701 pp. 1-19) */

	/* test ROM */
	ROMX_LOAD("94519209.u39", 0xFC00, 0x100, CRC(0a0b0c42), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1))
	ROMX_LOAD("94519210.u55", 0xFC00, 0x100, CRC(d078af61), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1))
	ROMX_LOAD("94519211.u61", 0xFC01, 0x100, CRC(6cf7d4a0), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1))
	ROMX_LOAD("94519212.u78", 0xFC01, 0x100, CRC(d9522458), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1))

	/* LOAD ROM */
	ROMX_LOAD("94519113.u3", 0xFE00, 0x100, CRC(8719b04e), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1))
	ROMX_LOAD("94519114.u4", 0xFE00, 0x100, CRC(72a040e0), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1))
	ROMX_LOAD("94519115.u6", 0xFE01, 0x100, CRC(9ccf8cca), ROM_NIBBLE | ROM_SHIFT_NIBBLE_HI | ROM_SKIP(1))
	ROMX_LOAD("94519116.u7", 0xFE01, 0x100, CRC(fa387bf3), ROM_NIBBLE | ROM_SHIFT_NIBBLE_LO | ROM_SKIP(1))

#else
	/* ROM set 945121-4(?): "Floppy disc loader with self test" (cf 945401-9701 pp. 1-19) */
	ROM_LOAD16_WORD("ti9904.rom", 0xFC00, 0x400, CRC(691e7d19) SHA1(58d9bed80490fdf71c743bfd3077c70840b7df8c))
#endif

	ROM_REGION(asr733_device::chr_region_len, asr733_chr_region, ROMREGION_ERASEFF)
ROM_END

ROM_START(ti990_4v)
	/*CPU memory space*/
	ROM_REGION16_BE(0x10000, "maincpu",0)
	ROM_LOAD16_WORD("ti9904.rom", 0xFC00, 0x400, CRC(691e7d19) SHA1(58d9bed80490fdf71c743bfd3077c70840b7df8c))

	/* VDT911 character definitions */
	ROM_REGION(vdt911_device::chr_region_len, vdt911_chr_region, ROMREGION_ERASEFF)
ROM_END

//    YEAR  NAME      PARENT   COMPAT  MACHINE   INPUT  CLASS          INIT          COMPANY              FULLNAME                                                           FLAGS
COMP( 1976, ti990_4,  0,       0,      ti990_4,  0,     ti990_4_state, init_ti990_4, "Texas Instruments", "TI Model 990/4 Microcomputer System",                             MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
COMP( 1976, ti990_4v, ti990_4, 0,      ti990_4v, 0,     ti990_4_state, init_ti990_4, "Texas Instruments", "TI Model 990/4 Microcomputer System with Video Display Terminal", MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
