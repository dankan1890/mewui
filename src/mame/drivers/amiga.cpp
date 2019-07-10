// license:GPL-2.0+
// copyright-holders:Dirk Best
/***************************************************************************

    Commodore Amiga

***************************************************************************/

#include "emu.h"
#include "includes/amiga.h"

#include "bus/amiga/keyboard/keyboard.h"
#include "bus/amiga/zorro/zorro.h"
#include "cpu/m68000/m68000.h"
#include "cpu/m6502/m6502.h"
#include "machine/bankdev.h"
#include "machine/6525tpi.h"
#include "machine/mos6526.h"
#include "machine/gayle.h"
#include "machine/ataintf.h"
#include "machine/dmac.h"
#include "machine/nvram.h"
#include "machine/i2cmem.h"
#include "machine/amigafdc.h"
#include "machine/cr511b.h"
#include "machine/rp5c01.h"
#include "softlist.h"
#include "speaker.h"


//**************************************************************************
//  CONSTANTS / MACROS
//**************************************************************************

#define EXP_SLOT_TAG "exp"
#define ZORROBUS_TAG "zorrobus"


//**************************************************************************
//  PRIVATE DEVICES
//**************************************************************************

/*
The keyboard reset/power-on reset circuit for the Amiga 2000 is built
around the LM339 at U805 (top left on sheet 3 of the schematic).  To
simplify things, we assume all components are ideal.

In stead idle state:
* /KBCLK is high
* U805 pin 1 is driven low, holding C813 discharged
* U805 pin 2 is not driven, allowing C814 to remain charged
* U805 pin 13 is driven low setting thresholds to 2.0V and 1.0V
* U805 pin 14 is not driven, leaving /KBRST deasserted

When /KBCLK is asserted, C814 is allowed to charge.  A short pulse will
not give it sufficient time to charge past the 2.0V threshold on pin 5
and the circuit will remain in idle.

If /KBCLK is asserted for over 112ms:
* U805 pin 1 is not driven, allowing C813 to charge past 2.0V
* U805 pin 2 is driven low, discharging C814
* U805 pin 13 is not driven, raising thresholds to 2.86V and 3.57V
* U805 pin 14 is driven low, asserting /KBRST

The thresholds changing will cause U805 pin 2 to float, allowing C814 to
begin charging.  If /KBCLK is asserted for a further 74 milliseconds,
U805 pin 2 will be driven low keeping C814 discharged until /KBCLK is
deasserted.

C814 (22µF) will charge via R805 (47kΩ) until it reaches the 3.57V
threshold, ensuring the minimum length of a reset pulse is 1.294s.

The power-on reset signal is also allowed to discharge C814, but we
ignore this for simplicity.  Earlier boards use a 1N4148 signal diode,
while later boards replace it with a PST518B at XU1.

The equivalent circuit in the Amiga 1000 works similarly, but has
different components:
* 10µF/22kΩ (C104/R62) for timing /KBCLK pulse
* 10µF/100kΩ (C105/R63) for minimum /KBRESET pulse duration
* Thresholds of 2.499/2.501V and 2.474V/2.526V
* BAS32L diode for power-on reset
* This gives delays of 152ms, 176µs, and 704ms

The equivalent circuit in the Amiga CDTV has the same thresholds as the
Amiga 2000, but uses 1kΩ resistors for timing.  This gives delays of
11.2ms, 7.43ms, and 27.5ms.

*/

DECLARE_DEVICE_TYPE(A1000_KBRESET, a1000_kbreset_device)

class a1000_kbreset_device : public device_t
{
public:
	a1000_kbreset_device(machine_config const &config, char const *tag, device_t *owner, u32 clock = 0U) :
		device_t(config, A1000_KBRESET, tag, owner, clock),
		m_kbrst_cb(*this)
	{
	}

	auto kbrst_cb() { return m_kbrst_cb.bind(); }

	a1000_kbreset_device &set_delays(attotime detect, attotime stray, attotime output)
	{
		m_detect_time = detect;
		m_stray_time = stray;
		m_output_time = output;
		return *this;
	}

	DECLARE_WRITE_LINE_MEMBER(kbclk_w)
	{
		if (bool(state) != bool(m_kbclk))
		{
			m_kbclk = state ? 1U : 0U;
			if (state)
			{
				// U805 pin 1 driven low - discharges C813
				m_c813_level = 0U;
				m_c813_timer->reset();

				// U805 pin 2 floating - allows C814 to charge
				if (!m_c814_charging)
				{
					m_c814_charging = 1U;
					m_c814_timer->adjust(m_output_time); // 0V to 3.57V
				}
			}
			else
			{
				// U805 pin 1 floating - allows C813 to charge
				assert(0U == m_c813_level);
				m_c813_timer->adjust(m_detect_time); // 0V to 2V
			}
		}
	}

protected:
	virtual void device_resolve_objects() override
	{
		m_kbrst_cb.resolve_safe();
	}

	virtual void device_start() override
	{
		// allocate resources
		m_c813_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(a1000_kbreset_device::c813_charged), this));
		m_c814_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(a1000_kbreset_device::c814_charged), this));

		// start in idle state
		m_kbclk = 1U;
		m_kbrst = 1U;
		m_c813_level = 0U;
		m_c814_charging = 1U;

		// always better to save state
		save_item(NAME(m_kbclk));
		save_item(NAME(m_kbrst));
		save_item(NAME(m_c813_level));
		save_item(NAME(m_c814_charging));
	}

private:
	TIMER_CALLBACK_MEMBER(c813_charged)
	{
		assert(2U > m_c813_level);
		if (2U > ++m_c813_level)
			m_c813_timer->adjust(m_stray_time); // 2V to 2.86V

		if ((m_kbrst ? 0U : 1U) < m_c813_level)
		{
			// U805 pin 2 driven low - discharges C814
			if (2U > m_c813_level)
			{
				assert(m_c814_charging);
				m_c814_timer->adjust(m_output_time); // 0V to 3.57V
			}
			else
			{
				m_c814_charging = 0U;
				m_c814_timer->reset();
			}
			if (m_kbrst)
				m_kbrst_cb(m_kbrst = 0U);
		}
	}

	TIMER_CALLBACK_MEMBER(c814_charged)
	{
		// C814 above high threshold - /KBRST deasserted
		assert(m_c814_charging);
		assert(!m_kbrst);
		m_kbrst_cb(m_kbrst = 1U);

		// if C813 is between 2.0V and 2.86V, lowering the threshold will discharge C814
		assert(2U > m_c813_level);
		if (0U < m_c813_level)
		{
			// threshold is bumped back up
			m_kbrst_cb(m_kbrst = 0U);
			m_c814_timer->adjust(m_output_time); // 0V to 3.57V
		}
	}

	attotime m_detect_time = attotime::from_msec(112);
	attotime m_stray_time = attotime::from_msec(74);
	attotime m_output_time = attotime::from_msec(1294);

	devcb_write_line m_kbrst_cb;

	emu_timer *m_c813_timer = nullptr; // C813 = 22µF, R802 = 10kΩ
	emu_timer *m_c814_timer = nullptr; // C814 = 22µF, R805 = 47kΩ

	u8 m_kbclk = 1U; // /KBCLK input
	u8 m_kbrst = 1U; // /KBRST output
	u8 m_c813_level = 0U; // 0 = 0V-2V, 1 = 2V - 2.86V, 2 = 2.86V - 5V
	u8 m_c814_charging = 1U; // U805 pin 2
};

DEFINE_DEVICE_TYPE(A1000_KBRESET, a1000_kbreset_device, "a1000kbrst", "Amiga 1000/2000/CDTV keyboard reset circuit")


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a1000_state : public amiga_state
{
public:
	a1000_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_bootrom(*this, "bootrom"),
		m_wom(*this, "wom")
	{ }

	void init_pal();
	void init_ntsc();

	DECLARE_WRITE16_MEMBER( write_protect_w );

	void a1000(machine_config &config);
	void a1000n(machine_config &config);
	void a1000_bootrom_map(address_map &map);
	void a1000_mem(address_map &map);
	void a1000_overlay_map(address_map &map);
protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	required_device<address_map_bank_device> m_bootrom;
	required_memory_bank m_wom;
	std::vector<u16> m_wom_ram;
};

class a2000_state : public amiga_state
{
public:
	a2000_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_rtc(*this, "u65"),
		m_zorro(*this, ZORROBUS_TAG),
		m_zorro2_int2(0),
		m_zorro2_int6(0)
	{ }

	void init_pal();
	void init_ntsc();

	DECLARE_WRITE_LINE_MEMBER( zorro2_int2_w );
	DECLARE_WRITE_LINE_MEMBER( zorro2_int6_w );

	DECLARE_READ16_MEMBER( clock_r );
	DECLARE_WRITE16_MEMBER( clock_w );

	void a2000(machine_config &config);
	void a2000n(machine_config &config);
	void a2000_mem(address_map &map);
protected:
	virtual void machine_reset() override;

	// amiga_state overrides
	virtual bool int2_pending() override;
	virtual bool int6_pending() override;

private:
	// devices
	required_device<msm6242_device> m_rtc;
	required_device<zorro2_device> m_zorro;

	// internal state
	int m_zorro2_int2;
	int m_zorro2_int6;
};

class a500_state : public amiga_state
{
public:
	a500_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_side(*this, EXP_SLOT_TAG),
		m_side_int2(0),
		m_side_int6(0)
	{ }

	void init_pal();
	void init_ntsc();

	DECLARE_WRITE_LINE_MEMBER( side_int2_w );
	DECLARE_WRITE_LINE_MEMBER( side_int6_w );

	void a500n(machine_config &config);
	void a500(machine_config &config);
	void a500_mem(address_map &map);
protected:
	virtual void machine_reset() override;

	// amiga_state overrides
	virtual bool int2_pending() override;
	virtual bool int6_pending() override;

private:
	// devices
	required_device<exp_slot_device> m_side;

	// internal state
	int m_side_int2;
	int m_side_int6;
};

class cdtv_state : public amiga_state
{
public:
	cdtv_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_rtc(*this, "u61"),
		m_dmac(*this, "u36"),
		m_tpi(*this, "u32"),
		m_cdrom(*this, "cdrom"),
		m_dmac_irq(0),
		m_tpi_irq(0)
	{ }

	void init_pal();
	void init_ntsc();

	DECLARE_READ16_MEMBER( clock_r );
	DECLARE_WRITE16_MEMBER( clock_w );

	DECLARE_READ8_MEMBER( dmac_scsi_data_read );
	DECLARE_WRITE8_MEMBER( dmac_scsi_data_write );
	DECLARE_READ8_MEMBER( dmac_io_read );
	DECLARE_WRITE8_MEMBER( dmac_io_write );
	DECLARE_WRITE_LINE_MEMBER( dmac_int_w );

	DECLARE_WRITE8_MEMBER( tpi_port_b_write );
	DECLARE_WRITE_LINE_MEMBER( tpi_int_w );

	void cdtv(machine_config &config);
	void cdtvn(machine_config &config);
	void cdtv_mem(address_map &map);
	void cdtv_rc_mem(address_map &map);
protected:
	// driver_device overrides
	virtual void machine_start() override;

	// amiga_state overrides
	virtual bool int2_pending() override;
	virtual bool int6_pending() override;

private:
	// devices
	required_device<msm6242_device> m_rtc;
	required_device<amiga_dmac_device> m_dmac;
	required_device<tpi6525_device> m_tpi;
	required_device<cr511b_device> m_cdrom;

	// internal state
	int m_dmac_irq;
	int m_tpi_irq;
};

class a3000_state : public amiga_state
{
public:
	a3000_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag)
	{ }

	DECLARE_READ32_MEMBER( scsi_r );
	DECLARE_WRITE32_MEMBER( scsi_w );
	DECLARE_READ32_MEMBER( motherboard_r );
	DECLARE_WRITE32_MEMBER( motherboard_w );

	void init_pal();
	void init_ntsc();

	void a3000(machine_config &config);
	void a3000n(machine_config &config);
	void a3000_mem(address_map &map);
protected:

private:
};

class a500p_state : public amiga_state
{
public:
	a500p_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_rtc(*this, "u9"),
		m_side(*this, EXP_SLOT_TAG),
		m_side_int2(0),
		m_side_int6(0)
	{ }

	DECLARE_READ16_MEMBER( clock_r );
	DECLARE_WRITE16_MEMBER( clock_w );

	void init_pal();
	void init_ntsc();

	void a500pn(machine_config &config);
	void a500p(machine_config &config);
	void a500p_mem(address_map &map);
protected:
	virtual void machine_reset() override;

	// amiga_state overrides
	virtual bool int2_pending() override;
	virtual bool int6_pending() override;

private:
	// devices
	required_device<msm6242_device> m_rtc;
	required_device<exp_slot_device> m_side;

	// internal state
	int m_side_int2;
	int m_side_int6;
};

class a600_state : public amiga_state
{
public:
	a600_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_gayle_int2(0)
	{ }

	DECLARE_WRITE_LINE_MEMBER( gayle_int2_w );

	void init_pal();
	void init_ntsc();

	static const u8 GAYLE_ID = 0xd0;

	void a600n(machine_config &config);
	void a600(machine_config &config);
	void a600_mem(address_map &map);
protected:
	virtual bool int2_pending() override;

private:
	int m_gayle_int2;
};

class a1200_state : public amiga_state
{
public:
	a1200_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_gayle_int2(0)
	{ }

	DECLARE_WRITE_LINE_MEMBER( gayle_int2_w );

	void init_pal();
	void init_ntsc();

	static const u8 GAYLE_ID = 0xd1;

	void a1200(machine_config &config);
	void a1200n(machine_config &config);
	void a1200_mem(address_map &map);
protected:
	virtual bool int2_pending() override;

private:
	int m_gayle_int2;
};

class a4000_state : public amiga_state
{
public:
	a4000_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_ata(*this, "ata"),
		m_ramsey_config(0),
		m_gary_coldboot(1),
		m_gary_timeout(0),
		m_gary_toenb(0),
		m_ide_interrupt(0)
	{ }

	DECLARE_READ32_MEMBER( scsi_r );
	DECLARE_WRITE32_MEMBER( scsi_w );
	DECLARE_READ16_MEMBER( ide_r );
	DECLARE_WRITE16_MEMBER( ide_w );
	DECLARE_WRITE_LINE_MEMBER( ide_interrupt_w );
	DECLARE_READ32_MEMBER( motherboard_r );
	DECLARE_WRITE32_MEMBER( motherboard_w );

	void init_pal();
	void init_ntsc();

	void a400030n(machine_config &config);
	void a4000tn(machine_config &config);
	void a4000t(machine_config &config);
	void a4000n(machine_config &config);
	void a4000(machine_config &config);
	void a400030(machine_config &config);
	void a400030_mem(address_map &map);
	void a4000_mem(address_map &map);
	void a4000t_mem(address_map &map);
protected:

private:
	required_device<ata_interface_device> m_ata;

	int m_ramsey_config;
	int m_gary_coldboot;
	int m_gary_timeout;
	int m_gary_toenb;
	int m_ide_interrupt;
};

class cd32_state : public amiga_state
{
public:
	cd32_state(const machine_config &mconfig, device_type type, const char *tag) :
		amiga_state(mconfig, type, tag),
		m_player_ports(*this, {"p1_cd32_buttons", "p2_cd32_buttons"}),
		m_cdda(*this, "cdda")
	{ }

	DECLARE_WRITE_LINE_MEMBER( akiko_int_w );
	DECLARE_WRITE8_MEMBER( akiko_cia_0_port_a_write );

	void handle_joystick_cia(u8 pra, u8 dra);
	u16 handle_joystick_potgor(u16 potgor);

	DECLARE_CUSTOM_INPUT_MEMBER( cd32_input );
	DECLARE_CUSTOM_INPUT_MEMBER( cd32_sel_mirror_input );

	void init_pal();
	void init_ntsc();

	required_ioport_array<2> m_player_ports;

	int m_oldstate[2];
	int m_cd32_shifter[2];
	u16 m_potgo_value;

	void cd32n(machine_config &config);
	void cd32(machine_config &config);
	void cd32_mem(address_map &map);
protected:
	// amiga_state overrides
	virtual void potgo_w(u16 data) override;

private:
	required_device<cdda_device> m_cdda;
};


//**************************************************************************
//  REAL TIME CLOCK
//**************************************************************************

READ16_MEMBER( cdtv_state::clock_r )
{
	return m_rtc->read(space, offset / 2);
}

WRITE16_MEMBER( cdtv_state::clock_w )
{
	m_rtc->write(space, offset / 2, data);
}

READ16_MEMBER( a2000_state::clock_r )
{
	return m_rtc->read(space, offset / 2);
}

WRITE16_MEMBER( a2000_state::clock_w )
{
	m_rtc->write(space, offset / 2, data);
}

READ16_MEMBER( a500p_state::clock_r )
{
	return m_rtc->read(space, offset / 2);
}

WRITE16_MEMBER( a500p_state::clock_w )
{
	m_rtc->write(space, offset / 2, data);
}


//**************************************************************************
//  CD-ROM CONTROLLER
//**************************************************************************

READ8_MEMBER( cdtv_state::dmac_scsi_data_read )
{
	if (offset >= 0xb0 && offset <= 0xbf)
		return m_tpi->read(offset);

	return 0xff;
}

WRITE8_MEMBER( cdtv_state::dmac_scsi_data_write )
{
	if (offset >= 0xb0 && offset <= 0xbf)
		m_tpi->write(offset, data);
}

READ8_MEMBER( cdtv_state::dmac_io_read )
{
	return m_cdrom->read(space, 0);
}

WRITE8_MEMBER( cdtv_state::dmac_io_write )
{
	m_cdrom->write(space, 0, data);
}

WRITE_LINE_MEMBER( cdtv_state::dmac_int_w )
{
	m_dmac_irq = state;
	update_int2();
}

WRITE8_MEMBER( cdtv_state::tpi_port_b_write )
{
	m_cdrom->cmd_w(BIT(data, 0));
	m_cdrom->enable_w(BIT(data, 1));
}

WRITE_LINE_MEMBER( cdtv_state::tpi_int_w )
{
	m_tpi_irq = state;
	update_int2();
}


//**************************************************************************
//  DRIVER INIT
//**************************************************************************

// ocs chipset (agnus with support for 512k or 1mb chip ram, denise)
void a1000_state::init_pal()
{
	m_agnus_id = AGNUS_PAL;     // 8367
	m_denise_id = DENISE;       // 8362
}

void a1000_state::init_ntsc()
{
	m_agnus_id = AGNUS_NTSC;    // 8361
	m_denise_id = DENISE;       // 8362
}

void a2000_state::init_pal()
{
	m_agnus_id = AGNUS_PAL;     // 8371 (later versions 8372A)
	m_denise_id = DENISE;       // 8362
}

void a2000_state::init_ntsc()
{
	m_agnus_id = AGNUS_NTSC;    // 8370 (later versions 8372A)
	m_denise_id = DENISE;       // 8362
}

void a500_state::init_pal()
{
	m_agnus_id = AGNUS_PAL;     // 8371 (later versions 8372A)
	m_denise_id = DENISE;       // 8362
}

void a500_state::init_ntsc()
{
	m_agnus_id = AGNUS_NTSC;    // 8370 (later versions 8372A)
	m_denise_id = DENISE;       // 8362
}

void cdtv_state::init_pal()
{
	m_agnus_id = AGNUS_HR_PAL;  // 8372A
	m_denise_id = DENISE;       // 8362
}

void cdtv_state::init_ntsc()
{
	m_agnus_id = AGNUS_HR_NTSC; // 8372A
	m_denise_id = DENISE;       // 8362
}

// ecs chipset (agnus with support for 2mb chip ram, super denise)
void a3000_state::init_pal()
{
	m_agnus_id = AGNUS_HR_PAL_NEW;  // 8372B (early versions: 8372AB)
	m_denise_id = DENISE_HR;        // 8373
}

void a3000_state::init_ntsc()
{
	m_agnus_id = AGNUS_HR_NTSC_NEW; // 8372B (early versions: 8372AB)
	m_denise_id = DENISE_HR;        // 8373
}

void a500p_state::init_pal()
{
	m_agnus_id = AGNUS_HR_PAL;  // 8375 (390544-01)
	m_denise_id = DENISE_HR;    // 8373
}

void a500p_state::init_ntsc()
{
	m_agnus_id = AGNUS_HR_NTSC; // 8375 (390544-02)
	m_denise_id = DENISE_HR;    // 8373
}

void a600_state::init_pal()
{
	m_agnus_id = AGNUS_HR_PAL;  // 8375 (390544-01)
	m_denise_id = DENISE_HR;    // 8373
}

void a600_state::init_ntsc()
{
	m_agnus_id = AGNUS_HR_NTSC; // 8375 (390544-02)
	m_denise_id = DENISE_HR;    // 8373
}

// aga chipset (alice and lisa)
void a1200_state::init_pal()
{
	m_agnus_id = ALICE_PAL_NEW;
	m_denise_id = LISA;
}

void a1200_state::init_ntsc()
{
	m_agnus_id = ALICE_NTSC_NEW;
	m_denise_id = LISA;
}

void a4000_state::init_pal()
{
	m_agnus_id = ALICE_PAL_NEW;
	m_denise_id = LISA;
}

void a4000_state::init_ntsc()
{
	m_agnus_id = ALICE_NTSC_NEW;
	m_denise_id = LISA;
}

void cd32_state::init_pal()
{
	m_agnus_id = ALICE_PAL_NEW;
	m_denise_id = LISA;
}

void cd32_state::init_ntsc()
{
	m_agnus_id = ALICE_NTSC_NEW;
	m_denise_id = LISA;
}


//**************************************************************************
//  MACHINE EMULATION
//**************************************************************************

void a1000_state::machine_start()
{
	// start base machine
	amiga_state::machine_start();

	// allocate 256kb for wom
	m_wom_ram.resize(256 * 1024 / 2);
	m_wom->set_base(&m_wom_ram[0]);
}

void a1000_state::machine_reset()
{
	// base reset
	amiga_state::machine_reset();

	// bootrom visible, wom writable
	m_bootrom->set_bank(0);
	m_maincpu->space(AS_PROGRAM).install_write_bank(0xfc0000, 0xffffff, "wom");
}

// any write to this area will write protect the wom and disable the bootrom
WRITE16_MEMBER( a1000_state::write_protect_w )
{
	m_bootrom->set_bank(1);
	m_maincpu->space(AS_PROGRAM).nop_write(0xfc0000, 0xffffff);
}

void a2000_state::machine_reset()
{
	// base reset
	amiga_state::machine_reset();

	// reset zorro devices
	m_zorro->reset();
}

WRITE_LINE_MEMBER( a2000_state::zorro2_int2_w )
{
	m_zorro2_int2 = state;
	update_int2();
}

WRITE_LINE_MEMBER( a2000_state::zorro2_int6_w )
{
	m_zorro2_int6 = state;
	update_int6();
}

bool a2000_state::int2_pending()
{
	return m_cia_0_irq || m_zorro2_int2;
}

bool a2000_state::int6_pending()
{
	return m_cia_1_irq || m_zorro2_int6;
}

void a500_state::machine_reset()
{
	// base reset
	amiga_state::machine_reset();

	// reset side expansion slot device
	m_side->reset();
}

WRITE_LINE_MEMBER( a500_state::side_int2_w )
{
	m_side_int2 = state;
	update_int2();
}

WRITE_LINE_MEMBER( a500_state::side_int6_w )
{
	m_side_int6 = state;
	update_int6();
}

bool a500_state::int2_pending()
{
	return m_cia_0_irq || m_side_int2;
}

bool a500_state::int6_pending()
{
	return m_cia_1_irq || m_side_int6;
}

void cdtv_state::machine_start()
{
	// start base machine
	amiga_state::machine_start();

	// setup dmac
	m_dmac->set_address_space(&m_maincpu->space(AS_PROGRAM));
	m_dmac->ramsz_w(0);
}

bool cdtv_state::int2_pending()
{
	return m_cia_0_irq || m_dmac_irq || m_tpi_irq;
}

bool cdtv_state::int6_pending()
{
	return m_cia_1_irq;
}

READ32_MEMBER( a3000_state::scsi_r )
{
	u32 data = 0xffffffff;
	logerror("scsi_r(%06x): %08x & %08x\n", offset, data, mem_mask);
	return data;
}

WRITE32_MEMBER( a3000_state::scsi_w )
{
	logerror("scsi_w(%06x): %08x & %08x\n", offset, data, mem_mask);
}

READ32_MEMBER( a3000_state::motherboard_r )
{
	u32 data = 0xffffffff;
	logerror("motherboard_r(%06x): %08x & %08x\n", offset, data, mem_mask);
	return data;
}

WRITE32_MEMBER( a3000_state::motherboard_w )
{
	logerror("motherboard_w(%06x): %08x & %08x\n", offset, data, mem_mask);
}

void a500p_state::machine_reset()
{
	// base reset
	amiga_state::machine_reset();

	// reset side expansion slot device
	m_side->reset();
}

bool a500p_state::int2_pending()
{
	return m_cia_0_irq || m_side_int2;
}

bool a500p_state::int6_pending()
{
	return m_cia_1_irq || m_side_int6;
}

bool a600_state::int2_pending()
{
	return m_cia_0_irq || m_gayle_int2;
}

WRITE_LINE_MEMBER( a600_state::gayle_int2_w )
{
	m_gayle_int2 = state;
	update_int2();
}

bool a1200_state::int2_pending()
{
	return m_cia_0_irq || m_gayle_int2;
}

WRITE_LINE_MEMBER( a1200_state::gayle_int2_w )
{
	m_gayle_int2 = state;
	update_int2();
}

READ32_MEMBER( a4000_state::scsi_r )
{
	u16 data = 0xffff;
	logerror("scsi_r(%06x): %08x & %08x\n", offset, data, mem_mask);
	return data;
}

WRITE32_MEMBER( a4000_state::scsi_w )
{
	logerror("scsi_w(%06x): %08x & %08x\n", offset, data, mem_mask);
}

READ16_MEMBER( a4000_state::ide_r )
{
	u16 data = 0xffff;

	// ide interrupt register
	if (offset == 0x1010)
		return m_ide_interrupt << 15;

	// swap
	mem_mask = (mem_mask << 8) | (mem_mask >> 8);

	// this very likely doesn't respond to all the addresses, figure out which ones
	if (BIT(offset, 12))
		data = m_ata->read_cs1((offset >> 1) & 0x07, mem_mask);
	else
		data = m_ata->read_cs0((offset >> 1) & 0x07, mem_mask);

	// swap
	data = (data << 8) | (data >> 8);

	return data;
}

WRITE16_MEMBER( a4000_state::ide_w )
{
	// ide interrupt register, read only
	if (offset == 0x1010)
		return;

	// swap
	mem_mask = (mem_mask << 8) | (mem_mask >> 8);
	data = (data << 8) | (data >> 8);

	// this very likely doesn't respond to all the addresses, figure out which ones
	if (BIT(offset, 12))
		m_ata->write_cs1((offset >> 1) & 0x07, data, mem_mask);
	else
		m_ata->write_cs0((offset >> 1) & 0x07, data, mem_mask);
}

WRITE_LINE_MEMBER( a4000_state::ide_interrupt_w )
{
	m_ide_interrupt = state;
}

READ32_MEMBER( a4000_state::motherboard_r )
{
	u32 data = 0;

	if (offset == 0)
	{
		if (ACCESSING_BITS_0_7)
			data |= m_ramsey_config & 0xff;
		if (ACCESSING_BITS_8_15)
			data |= (m_gary_coldboot << 7 | 0x7f) << 8;
		if (ACCESSING_BITS_16_23)
			data |= (m_gary_toenb << 7 | 0x7f) << 16;
		if (ACCESSING_BITS_24_31)
			data |= (m_gary_timeout << 7 | 0x7f) << 24;
	}
	else
		data = 0xffffffff;

	logerror("motherboard_r(%06x): %08x & %08x\n", offset, data, mem_mask);

	return data;
}

WRITE32_MEMBER( a4000_state::motherboard_w )
{
	if (offset == 0)
	{
		if (ACCESSING_BITS_0_7)
			m_ramsey_config = data & 0xff;
		if (ACCESSING_BITS_8_15)
			m_gary_coldboot = BIT(data, 7);
		if (ACCESSING_BITS_16_23)
			m_gary_toenb = BIT(data, 7);
		if (ACCESSING_BITS_24_31)
			m_gary_timeout = BIT(data, 7);
	}

	logerror("motherboard_w(%06x): %08x & %08x\n", offset, data, mem_mask);
}

WRITE_LINE_MEMBER(cd32_state::akiko_int_w)
{
	set_interrupt(INTENA_SETCLR | INTENA_PORTS);
}

void cd32_state::potgo_w(u16 data)
{
	int i;

	m_potgo_value = m_potgo_value & 0x5500;
	m_potgo_value |= data & 0xaa00;

	for (i = 0; i < 8; i += 2)
	{
		u16 dir = 0x0200 << i;
		if (data & dir)
		{
			u16 d = 0x0100 << i;
			m_potgo_value &= ~d;
			m_potgo_value |= data & d;
		}
	}
	for (i = 0; i < 2; i++)
	{
		u16 p5dir = 0x0200 << (i * 4); /* output enable P5 */
		u16 p5dat = 0x0100 << (i * 4); /* data P5 */
		if ((m_potgo_value & p5dir) && (m_potgo_value & p5dat))
			m_cd32_shifter[i] = 8;
	}
}

void cd32_state::handle_joystick_cia(u8 pra, u8 dra)
{
	for (int i = 0; i < 2; i++)
	{
		u8 but = 0x40 << i;
		u16 p5dir = 0x0200 << (i * 4); /* output enable P5 */
		u16 p5dat = 0x0100 << (i * 4); /* data P5 */

		if (!(m_potgo_value & p5dir) || !(m_potgo_value & p5dat))
		{
			if ((dra & but) && (pra & but) != m_oldstate[i])
			{
				if (!(pra & but))
				{
					m_cd32_shifter[i]--;
					if (m_cd32_shifter[i] < 0)
						m_cd32_shifter[i] = 0;
				}
			}
		}
		m_oldstate[i] = pra & but;
	}
}

u16 cd32_state::handle_joystick_potgor(u16 potgor)
{
	for (int i = 0; i < 2; i++)
	{
		u16 p9dir = 0x0800 << (i * 4); /* output enable P9 */
		u16 p9dat = 0x0400 << (i * 4); /* data P9 */
		u16 p5dir = 0x0200 << (i * 4); /* output enable P5 */
		u16 p5dat = 0x0100 << (i * 4); /* data P5 */

		/* p5 is floating in input-mode */
		potgor &= ~p5dat;
		potgor |= m_potgo_value & p5dat;
		if (!(m_potgo_value & p9dir))
			potgor |= p9dat;
		/* P5 output and 1 -> shift register is kept reset (Blue button) */
		if ((m_potgo_value & p5dir) && (m_potgo_value & p5dat))
			m_cd32_shifter[i] = 8;
		/* shift at 1 == return one, >1 = return button states */
		if (m_cd32_shifter[i] == 0)
			potgor &= ~p9dat; /* shift at zero == return zero */
		if (m_cd32_shifter[i] >= 2 && ((m_player_ports[i])->read() & (1 << (m_cd32_shifter[i] - 2))))
			potgor &= ~p9dat;
	}
	return potgor;
}

CUSTOM_INPUT_MEMBER( cd32_state::cd32_input )
{
	return handle_joystick_potgor(m_potgo_value) >> 8;
}

CUSTOM_INPUT_MEMBER( cd32_state::cd32_sel_mirror_input )
{
	u8 bits = m_player_ports[(int)(uintptr_t)param]->read();
	return (bits & 0x20)>>5;
}

WRITE8_MEMBER( cd32_state::akiko_cia_0_port_a_write )
{
	// bit 0, cd audio mute
	m_cdda->set_output_gain(0, BIT(data, 0) ? 0.0 : 1.0);

	// bit 1, power led
	m_power_led = BIT(~data, 1);

	handle_joystick_cia(data, m_cia_0->read(2));
}


//**************************************************************************
//  ADDRESS MAPS
//**************************************************************************

// The first Amiga systems used a PAL to decode chip selects, later systems
// switched to the "Gary" chip, the A3000 and A4000 used the "Super Gary"
// chip. The A600 and A1200 use the Gayle chip, while the CD32 uses its
// Akiko custom chip.

#if 0
void a1000_state::a1000_overlay_map(address_map &map)
{
	map(0x000000, 0x03ffff).mirror(0x1c0000).ram().share("chip_ram");
	map(0x200000, 0x20ffff).mirror(0x030000).rom().region("bootrom", 0);
	map(0x280000, 0x2bffff).mirror(0x040000).ram().share("chip_ram");
	map(0x300000, 0x33ffff).mirror(0x040000).ram().share("chip_ram");
	map(0x380000, 0x38ffff).mirror(0x030000).rom().region("bootrom", 0);
}
#endif

void a1000_state::a1000_overlay_map(address_map &map)
{
	map(0x000000, 0x07ffff).mirror(0x180000).ram().share("chip_ram");
	map(0x200000, 0x20ffff).mirror(0x030000).rom().region("bootrom", 0);
	map(0x280000, 0x2fffff).ram().share("chip_ram");
	map(0x300000, 0x37ffff).ram().share("chip_ram");
	map(0x380000, 0x38ffff).mirror(0x030000).rom().region("bootrom", 0);
}

void a1000_state::a1000_bootrom_map(address_map &map)
{
	map(0x000000, 0x00ffff).mirror(0x30000).rom().region("bootrom", 0).w(FUNC(a1000_state::write_protect_w));
	map(0x040000, 0x07ffff).bankr("wom");
}

void a1000_state::a1000_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap16));
	map(0xa00000, 0xbfffff).rw(FUNC(a1000_state::cia_r), FUNC(a1000_state::cia_w));
	map(0xc00000, 0xdfffff).rw(FUNC(a1000_state::custom_chip_r), FUNC(a1000_state::custom_chip_w));
	map(0xe00000, 0xe7ffff).nopw().r(FUNC(a1000_state::rom_mirror_r));
	map(0xe80000, 0xefffff).noprw(); // autoconfig space (installed by devices)
	map(0xf80000, 0xfbffff).m(m_bootrom, FUNC(address_map_bank_device::amap16));
	map(0xfc0000, 0xffffff).bankrw("wom");
}

// Gary/Super Gary/Gayle with 512KB chip RAM
void amiga_state::overlay_512kb_map(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x07ffff).mirror(0x180000).ram().share("chip_ram");
	map(0x200000, 0x27ffff).rom().region("kickstart", 0);
}

// Gary/Super Gary/Gayle with 1MB chip RAM
void amiga_state::overlay_1mb_map(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x0fffff).mirror(0x100000).ram().share("chip_ram");
	map(0x200000, 0x27ffff).rom().region("kickstart", 0);
}

// Gary/Super Gary/Gayle with 1MB chip RAM (32 bit system)
void amiga_state::overlay_1mb_map32(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x0fffff).mirror(0x100000).ram().share("chip_ram");
	map(0x200000, 0x27ffff).rom().region("kickstart", 0);
}

// Gary/Super Gary/Gayle with 2MB chip RAM (32 bit system)
void amiga_state::overlay_2mb_map16(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).ram().share("chip_ram");
	map(0x200000, 0x27ffff).rom().region("kickstart", 0);
}

// Gary/Super Gary/Gayle with 2MB chip RAM (32 bit system)
void amiga_state::overlay_2mb_map32(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).ram().share("chip_ram");
	map(0x200000, 0x27ffff).rom().region("kickstart", 0);
}

// 512KB chip RAM, 512KB slow RAM, RTC
void a2000_state::a2000_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap16));
	map(0xa00000, 0xbfffff).rw(FUNC(a2000_state::cia_r), FUNC(a2000_state::cia_w));
	map(0xc00000, 0xc7ffff).ram();
	map(0xc80000, 0xd7ffff).rw(FUNC(a2000_state::custom_chip_r), FUNC(a2000_state::custom_chip_w));
	map(0xd80000, 0xdbffff).noprw();
	map(0xdc0000, 0xdc7fff).rw(FUNC(a2000_state::clock_r), FUNC(a2000_state::clock_w));
	map(0xdc8000, 0xddffff).noprw();
	map(0xde0000, 0xdeffff).rw(FUNC(a2000_state::custom_chip_r), FUNC(a2000_state::custom_chip_w));
	map(0xdf0000, 0xdfffff).rw(FUNC(a2000_state::custom_chip_r), FUNC(a2000_state::custom_chip_w));
	map(0xe00000, 0xe7ffff).nopw().r(FUNC(a2000_state::rom_mirror_r));
	map(0xe80000, 0xefffff).noprw(); // autoconfig space (installed by devices)
	map(0xf00000, 0xf7ffff).noprw(); // cartridge space
	map(0xf80000, 0xffffff).rom().region("kickstart", 0);
}

// 512KB chip RAM and no clock
void a500_state::a500_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap16));
	map(0xa00000, 0xbfffff).rw(FUNC(a500_state::cia_r), FUNC(a500_state::cia_w));
	map(0xc00000, 0xd7ffff).rw(FUNC(a500_state::custom_chip_r), FUNC(a500_state::custom_chip_w));
	map(0xd80000, 0xddffff).noprw();
	map(0xde0000, 0xdeffff).rw(FUNC(a500_state::custom_chip_r), FUNC(a500_state::custom_chip_w));
	map(0xdf0000, 0xdfffff).rw(FUNC(a500_state::custom_chip_r), FUNC(a500_state::custom_chip_w));
	map(0xe00000, 0xe7ffff).nopw().r(FUNC(a500_state::rom_mirror_r));
	map(0xe80000, 0xefffff).noprw(); // autoconfig space (installed by devices)
	map(0xf00000, 0xf7ffff).noprw(); // cartridge space
	map(0xf80000, 0xffffff).rom().region("kickstart", 0);
}

// 1MB chip RAM, RTC and CDTV specific hardware
void cdtv_state::cdtv_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap16));
	map(0xa00000, 0xbfffff).rw(FUNC(cdtv_state::cia_r), FUNC(cdtv_state::cia_w));
	map(0xc00000, 0xd7ffff).rw(FUNC(cdtv_state::custom_chip_r), FUNC(cdtv_state::custom_chip_w));
	map(0xd80000, 0xdbffff).noprw();
	map(0xdc0000, 0xdc7fff).rw(FUNC(cdtv_state::clock_r), FUNC(cdtv_state::clock_w));
	map(0xdc8000, 0xdc87ff).mirror(0x7800).ram().share("sram");
	map(0xdd0000, 0xddffff).noprw();
	map(0xde0000, 0xdeffff).rw(FUNC(cdtv_state::custom_chip_r), FUNC(cdtv_state::custom_chip_w));
	map(0xdf0000, 0xdfffff).rw(FUNC(cdtv_state::custom_chip_r), FUNC(cdtv_state::custom_chip_w));
	map(0xe00000, 0xe3ffff).mirror(0x40000).ram().share("memcard");
	map(0xe80000, 0xefffff).noprw(); // autoconfig space (installed by devices)
	map(0xf00000, 0xf3ffff).mirror(0x40000).rom().region("cdrom", 0);
	map(0xf80000, 0xffffff).rom().region("kickstart", 0);
}

void cdtv_state::cdtv_rc_mem(address_map &map)
{
	map(0x0800, 0x0fff).rom().region("rcmcu", 0);
}

void a3000_state::a3000_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x00000000, 0x001fffff).m(m_overlay, FUNC(address_map_bank_device::amap32));
	map(0x00b80000, 0x00bfffff).rw(FUNC(a3000_state::cia_r), FUNC(a3000_state::cia_w));
	map(0x00c00000, 0x00cfffff).rw(FUNC(a3000_state::custom_chip_r), FUNC(a3000_state::custom_chip_w));
	map(0x00d00000, 0x00dbffff).noprw();
	map(0x00dc0000, 0x00dcffff).rw("rtc", FUNC(rp5c01_device::read), FUNC(rp5c01_device::write)).umask32(0x000000ff);
	map(0x00dd0000, 0x00ddffff).rw(FUNC(a3000_state::scsi_r), FUNC(a3000_state::scsi_w));
	map(0x00de0000, 0x00deffff).rw(FUNC(a3000_state::motherboard_r), FUNC(a3000_state::motherboard_w));
	map(0x00df0000, 0x00dfffff).rw(FUNC(a3000_state::custom_chip_r), FUNC(a3000_state::custom_chip_w));
	map(0x00e80000, 0x00efffff).noprw(); // autoconfig space (installed by devices)
	map(0x00f00000, 0x00f7ffff).noprw(); // cartridge space
	map(0x00f80000, 0x00ffffff).rom().region("kickstart", 0);
	map(0x07f00000, 0x07ffffff).ram(); // motherboard ram (up to 16mb), grows downward
	map(0xfff80000, 0xffffffff).rom().region("kickstart", 0);
}

// 1MB chip RAM and RTC
void a500p_state::a500p_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap16));
	map(0xa00000, 0xbfffff).rw(FUNC(a500p_state::cia_r), FUNC(a500p_state::cia_w));
	map(0xc00000, 0xc7ffff).ram();
	map(0xc80000, 0xd7ffff).rw(FUNC(a500p_state::custom_chip_r), FUNC(a500p_state::custom_chip_w));
	map(0xd80000, 0xdbffff).noprw();
	map(0xdc0000, 0xdc7fff).rw(FUNC(a500p_state::clock_r), FUNC(a500p_state::clock_w));
	map(0xdc8000, 0xddffff).noprw();
	map(0xde0000, 0xdeffff).rw(FUNC(a500p_state::custom_chip_r), FUNC(a500p_state::custom_chip_w));
	map(0xdf0000, 0xdfffff).rw(FUNC(a500p_state::custom_chip_r), FUNC(a500p_state::custom_chip_w));
	map(0xe00000, 0xe7ffff).nopw().r(FUNC(a500p_state::rom_mirror_r));
	map(0xe80000, 0xefffff).noprw(); // autoconfig space (installed by devices)
	map(0xf80000, 0xffffff).rom().region("kickstart", 0);
}

// 1MB chip RAM, IDE and PCMCIA
void a600_state::a600_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap16));
	map(0x200000, 0xa7ffff).noprw();
	map(0xa80000, 0xafffff).nopw().r(FUNC(a600_state::rom_mirror_r));
	map(0xb00000, 0xb7ffff).nopw().r(FUNC(a600_state::rom_mirror_r));
	map(0xb80000, 0xbeffff).noprw(); // reserved (cdtv)
	map(0xbf0000, 0xbfffff).rw(FUNC(a600_state::cia_r), FUNC(a600_state::gayle_cia_w));
	map(0xc00000, 0xd7ffff).noprw(); // slow mem
	map(0xd80000, 0xd8ffff).noprw(); // spare chip select
	map(0xd90000, 0xd9ffff).noprw(); // arcnet chip select
	map(0xda0000, 0xdaffff).rw("gayle", FUNC(gayle_device::gayle_r), FUNC(gayle_device::gayle_w));
	map(0xdb0000, 0xdbffff).noprw(); // reserved (external ide)
	map(0xdc0000, 0xdcffff).noprw(); // rtc
	map(0xdd0000, 0xddffff).noprw(); // reserved (dma controller)
	map(0xde0000, 0xdeffff).rw("gayle", FUNC(gayle_device::gayle_id_r), FUNC(gayle_device::gayle_id_w));
	map(0xdf0000, 0xdfffff).rw(FUNC(a600_state::custom_chip_r), FUNC(a600_state::custom_chip_w));
	map(0xe00000, 0xe7ffff).nopw().r(FUNC(a600_state::rom_mirror_r));
	map(0xe80000, 0xefffff).noprw(); // autoconfig space (installed by devices)
	map(0xf00000, 0xf7ffff).noprw(); // cartridge space
	map(0xf80000, 0xffffff).rom().region("kickstart", 0);
}

// 2MB chip RAM, IDE and PCMCIA
void a1200_state::a1200_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap32));
	map(0x200000, 0xa7ffff).noprw();
	map(0xa80000, 0xafffff).nopw().r(FUNC(a1200_state::rom_mirror32_r));
	map(0xb00000, 0xb7ffff).nopw().r(FUNC(a1200_state::rom_mirror32_r));
	map(0xb80000, 0xbeffff).noprw(); // reserved (cdtv)
	map(0xbf0000, 0xbfffff).rw(FUNC(a1200_state::cia_r), FUNC(a1200_state::gayle_cia_w));
	map(0xc00000, 0xd7ffff).noprw(); // slow mem
	map(0xd80000, 0xd8ffff).noprw(); // spare chip select
	map(0xd90000, 0xd9ffff).noprw(); // arcnet chip select
	map(0xda0000, 0xdaffff).rw("gayle", FUNC(gayle_device::gayle_r), FUNC(gayle_device::gayle_w));
	map(0xdb0000, 0xdbffff).noprw(); // reserved (external ide)
	map(0xdc0000, 0xdcffff).noprw(); // rtc
	map(0xdd0000, 0xddffff).noprw(); // reserved (dma controller)
	map(0xde0000, 0xdeffff).rw("gayle", FUNC(gayle_device::gayle_id_r), FUNC(gayle_device::gayle_id_w));
	map(0xdf0000, 0xdfffff).rw(FUNC(a1200_state::custom_chip_r), FUNC(a1200_state::custom_chip_w));
	map(0xe00000, 0xe7ffff).nopw().r(FUNC(a1200_state::rom_mirror32_r));
	map(0xe80000, 0xefffff).noprw(); // autoconfig space (installed by devices)
	map(0xf00000, 0xf7ffff).noprw(); // cartridge space
	map(0xf80000, 0xffffff).rom().region("kickstart", 0);
}

// 2MB chip RAM, 4 MB fast RAM, RTC and IDE
void a4000_state::a4000_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x00000000, 0x001fffff).m(m_overlay, FUNC(address_map_bank_device::amap32));
	map(0x00200000, 0x009fffff).noprw(); // zorro2 expansion
	map(0x00a00000, 0x00b7ffff).noprw();
	map(0x00b80000, 0x00beffff).noprw();
	map(0x00bf0000, 0x00bfffff).rw(FUNC(a4000_state::cia_r), FUNC(a4000_state::cia_w));
	map(0x00c00000, 0x00cfffff).rw(FUNC(a4000_state::custom_chip_r), FUNC(a4000_state::custom_chip_w));
	map(0x00d00000, 0x00d9ffff).noprw();
	map(0x00da0000, 0x00dbffff).noprw();
	map(0x00dc0000, 0x00dcffff).rw("rtc", FUNC(rp5c01_device::read), FUNC(rp5c01_device::write)).umask32(0x000000ff);
	map(0x00dd0000, 0x00dd0fff).noprw();
	map(0x00dd1000, 0x00dd3fff).rw(FUNC(a4000_state::ide_r), FUNC(a4000_state::ide_w));
	map(0x00dd4000, 0x00ddffff).noprw();
	map(0x00de0000, 0x00deffff).rw(FUNC(a4000_state::motherboard_r), FUNC(a4000_state::motherboard_w));
	map(0x00df0000, 0x00dfffff).rw(FUNC(a4000_state::custom_chip_r), FUNC(a4000_state::custom_chip_w));
	map(0x00e00000, 0x00e7ffff).nopw().r(FUNC(a4000_state::rom_mirror32_r));
	map(0x00e80000, 0x00efffff).noprw(); // zorro2 autoconfig space (installed by devices)
	map(0x00f00000, 0x00f7ffff).noprw(); // cartridge space
	map(0x00f80000, 0x00ffffff).rom().region("kickstart", 0);
	map(0x01000000, 0x017fffff).noprw(); // reserved (8 mb chip ram)
	map(0x01800000, 0x06ffffff).noprw(); // reserved (motherboard fast ram expansion)
	map(0x07000000, 0x07bfffff).noprw(); // motherboard ram
	map(0x07c00000, 0x07ffffff).ram(); // motherboard ram (up to 16mb), grows downward
	map(0xfff80000, 0xffffffff).rom().region("kickstart", 0);
}

// 2MB chip RAM, 2 MB fast RAM, RTC and IDE
void a4000_state::a400030_mem(address_map &map)
{
	map.unmap_value_high();
	a4000_mem(map);
	map(0x07000000, 0x07dfffff).noprw(); // Drop the first 2Mb
}

// 2MB chip RAM and CD-ROM
void cd32_state::cd32_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x000000, 0x1fffff).m(m_overlay, FUNC(address_map_bank_device::amap32));
	map(0xb80000, 0xb8003f).rw("akiko", FUNC(akiko_device::read), FUNC(akiko_device::write));
	map(0xbf0000, 0xbfffff).rw(FUNC(cd32_state::cia_r), FUNC(cd32_state::gayle_cia_w));
	map(0xc00000, 0xdfffff).rw(FUNC(cd32_state::custom_chip_r), FUNC(cd32_state::custom_chip_w));
	map(0xe00000, 0xe7ffff).rom().region("kickstart", 0x80000);
	map(0xe80000, 0xf7ffff).noprw();
	map(0xf80000, 0xffffff).rom().region("kickstart", 0);
}

// 2 MB chip RAM, IDE, RTC and SCSI
void a4000_state::a4000t_mem(address_map &map)
{
	map.unmap_value_high();
	a4000_mem(map);
	map(0x00dd0000, 0x00dd0fff).rw(FUNC(a4000_state::scsi_r), FUNC(a4000_state::scsi_w));
}


//**************************************************************************
//  INPUTS
//**************************************************************************

static INPUT_PORTS_START( amiga )
	PORT_START("input")
	PORT_CONFNAME(0x10, 0x00, "Game Port 0 Device")
	PORT_CONFSETTING(0x00, "Mouse")
	PORT_CONFSETTING(0x10, DEF_STR(Joystick))
	PORT_CONFNAME(0x20, 0x20, "Game Port 1 Device")
	PORT_CONFSETTING(0x00, "Mouse")
	PORT_CONFSETTING(0x20, DEF_STR(Joystick) )

	PORT_START("cia_0_port_a")
	PORT_BIT(0x3f, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_CUSTOM_MEMBER(DEVICE_SELF, amiga_state, floppy_drive_status, (void *)0)
	PORT_BIT(0x40, IP_ACTIVE_LOW,  IPT_BUTTON1) PORT_PLAYER(1)
	PORT_BIT(0x80, IP_ACTIVE_LOW,  IPT_BUTTON1) PORT_PLAYER(2)

	PORT_START("joy_0_dat")
	PORT_BIT(0x0303, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_CUSTOM_MEMBER(DEVICE_SELF, amiga_state, amiga_joystick_convert, (void *)0)
	PORT_BIT(0xfcfc, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("joy_1_dat")
	PORT_BIT(0x0303, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_CUSTOM_MEMBER(DEVICE_SELF, amiga_state, amiga_joystick_convert, (void *)1)
	PORT_BIT(0xfcfc, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("potgo")
	PORT_BIT(0x0100, IP_ACTIVE_LOW, IPT_BUTTON3) PORT_PLAYER(1)
	PORT_BIT(0x0400, IP_ACTIVE_LOW, IPT_BUTTON2) PORT_PLAYER(1)
	PORT_BIT(0x1000, IP_ACTIVE_LOW, IPT_BUTTON3) PORT_PLAYER(2)
	PORT_BIT(0x4000, IP_ACTIVE_LOW, IPT_BUTTON2) PORT_PLAYER(2)
	PORT_BIT(0xaaff, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("p1_joy")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP)    PORT_PLAYER(1)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN)  PORT_PLAYER(1)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT)  PORT_PLAYER(1)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT) PORT_PLAYER(1)

	PORT_START("p2_joy")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP)    PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN)  PORT_PLAYER(2)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT)  PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT) PORT_PLAYER(2)

	PORT_START("p1_mouse_x")
	PORT_BIT(0xff, 0x00, IPT_MOUSE_X) PORT_SENSITIVITY(100) PORT_KEYDELTA(5) PORT_MINMAX(0, 255) PORT_PLAYER(1)

	PORT_START("p1_mouse_y")
	PORT_BIT(0xff, 0x00, IPT_MOUSE_Y) PORT_SENSITIVITY(100) PORT_KEYDELTA(5) PORT_MINMAX(0, 255) PORT_PLAYER(1)

	PORT_START("p2_mouse_x")
	PORT_BIT(0xff, 0x00, IPT_MOUSE_X) PORT_SENSITIVITY(100) PORT_KEYDELTA(5) PORT_MINMAX(0, 255) PORT_PLAYER(2)

	PORT_START("p2_mouse_y")
	PORT_BIT(0xff, 0x00, IPT_MOUSE_Y) PORT_SENSITIVITY(100) PORT_KEYDELTA(5) PORT_MINMAX(0, 255) PORT_PLAYER(2)
INPUT_PORTS_END

INPUT_PORTS_START( cd32 )
	PORT_INCLUDE(amiga)

	PORT_MODIFY("cia_0_port_a")
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_CUSTOM )
	// this is the regular port for reading a single button joystick on the Amiga, many CD32 games require this to mirror the pad start button!
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, cd32_state, cd32_sel_mirror_input, (void *)0)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, cd32_state, cd32_sel_mirror_input, (void *)1)

	PORT_MODIFY("joy_0_dat")
	PORT_BIT( 0x0303, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, amiga_state, amiga_joystick_convert, (void *)0)
	PORT_BIT( 0xfcfc, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("joy_1_dat")
	PORT_BIT( 0x0303, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, amiga_state, amiga_joystick_convert, (void *)1)
	PORT_BIT( 0xfcfc, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_MODIFY("potgo")
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(DEVICE_SELF, cd32_state, cd32_input, (void *)0)
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNUSED )

	// CD32 '11' button pad (4 dpad directions + 7 buttons), not read directly
	PORT_START("p1_cd32_buttons")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("P1 Play/Pause")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("P1 Left Trigger/Rewind")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_NAME("P1 Right Trigger/Forward")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_PLAYER(1) PORT_NAME("P1 Green/Stop")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_PLAYER(1) PORT_NAME("P1 Yellow/Shuffle")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_PLAYER(1) PORT_NAME("P1 Red/Select")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_PLAYER(1) PORT_NAME("P1 Blue/Loop")

	// CD32 '11' button pad (4 dpad directions + 7 buttons), not read directly
	PORT_START("p2_cd32_buttons")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("P2 Play/Pause")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("P2 Left Trigger/Rewind")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_NAME("P2 Right Trigger/Forward")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_PLAYER(2) PORT_NAME("P2 Green/Stop")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_PLAYER(2) PORT_NAME("P2 Yellow/Shuffle")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_PLAYER(2) PORT_NAME("P2 Red/Select")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_PLAYER(2) PORT_NAME("P2 Blue/Loop")
INPUT_PORTS_END


//**************************************************************************
//  MACHINE DRIVERS
//**************************************************************************

static void amiga_floppies(device_slot_interface &device)
{
	device.option_add("35dd", FLOPPY_35_DD);
}

// basic elements common to all amigas
void amiga_state::amiga_base(machine_config &config)
{
	// video
	pal_video(config);

	PALETTE(config, m_palette, FUNC(amiga_state::amiga_palette), 4096);

	MCFG_VIDEO_START_OVERRIDE(amiga_state, amiga)

	// cia
	MOS8520(config, m_cia_0, amiga_state::CLK_E_PAL);
	m_cia_0->irq_wr_callback().set(FUNC(amiga_state::cia_0_irq));
	m_cia_0->pa_rd_callback().set_ioport("cia_0_port_a");
	m_cia_0->pa_wr_callback().set(FUNC(amiga_state::cia_0_port_a_write));
	m_cia_0->pb_wr_callback().set("cent_data_out", FUNC(output_latch_device::bus_w));
	m_cia_0->pc_wr_callback().set(m_centronics, FUNC(centronics_device::write_strobe));
	m_cia_0->sp_wr_callback().set("kbd", FUNC(amiga_keyboard_bus_device::kdat_in_w)).invert();

	MOS8520(config, m_cia_1, amiga_state::CLK_E_PAL);
	m_cia_1->irq_wr_callback().set(FUNC(amiga_state::cia_1_irq));
	m_cia_1->pa_rd_callback().set(FUNC(amiga_state::cia_1_port_a_read));
	m_cia_1->pa_wr_callback().set(FUNC(amiga_state::cia_1_port_a_write));
	m_cia_1->pb_wr_callback().set(m_fdc, FUNC(amiga_fdc_device::ciaaprb_w));

	// audio
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();
	PAULA_8364(config, m_paula, amiga_state::CLK_C1_PAL);
	m_paula->add_route(0, "lspeaker", 0.50);
	m_paula->add_route(1, "rspeaker", 0.50);
	m_paula->add_route(2, "rspeaker", 0.50);
	m_paula->add_route(3, "lspeaker", 0.50);
	m_paula->mem_read_cb().set(FUNC(amiga_state::chip_ram_r));
	m_paula->int_cb().set(FUNC(amiga_state::paula_int_w));

	// floppy drives
	AMIGA_FDC(config, m_fdc, amiga_state::CLK_7M_PAL);
	m_fdc->read_dma_callback().set(FUNC(amiga_state::chip_ram_r));
	m_fdc->write_dma_callback().set(FUNC(amiga_state::chip_ram_w));
	m_fdc->dskblk_callback().set(FUNC(amiga_state::fdc_dskblk_w));
	m_fdc->dsksyn_callback().set(FUNC(amiga_state::fdc_dsksyn_w));
	FLOPPY_CONNECTOR(config, "fdc:0", amiga_floppies, "35dd", amiga_fdc_device::floppy_formats).enable_sound(true);
	FLOPPY_CONNECTOR(config, "fdc:1", amiga_floppies, nullptr, amiga_fdc_device::floppy_formats).enable_sound(true);
	FLOPPY_CONNECTOR(config, "fdc:2", amiga_floppies, nullptr, amiga_fdc_device::floppy_formats).enable_sound(true);
	FLOPPY_CONNECTOR(config, "fdc:3", amiga_floppies, nullptr, amiga_fdc_device::floppy_formats).enable_sound(true);

	// rs232
	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, nullptr));
	rs232.rxd_handler().set(FUNC(amiga_state::rs232_rx_w));
	rs232.dcd_handler().set(FUNC(amiga_state::rs232_dcd_w));
	rs232.dsr_handler().set(FUNC(amiga_state::rs232_dsr_w));
	rs232.ri_handler().set(FUNC(amiga_state::rs232_ri_w));
	rs232.cts_handler().set(FUNC(amiga_state::rs232_cts_w));

	// centronics
	CENTRONICS(config, m_centronics, centronics_devices, "printer");
	m_centronics->ack_handler().set(FUNC(amiga_state::centronics_ack_w));
	m_centronics->busy_handler().set(FUNC(amiga_state::centronics_busy_w));
	m_centronics->perror_handler().set(FUNC(amiga_state::centronics_perror_w));
	m_centronics->select_handler().set(FUNC(amiga_state::centronics_select_w));

	output_latch_device &cent_data_out(OUTPUT_LATCH(config, "cent_data_out"));
	m_centronics->set_output_latch(cent_data_out);

	// software
	SOFTWARE_LIST(config, "wb_list").set_original("amiga_workbench");
	SOFTWARE_LIST(config, "hardware_list").set_original("amiga_hardware");
	SOFTWARE_LIST(config, "apps_list").set_original("amiga_apps");
	SOFTWARE_LIST(config, "flop_list").set_original("amiga_flop");
	SOFTWARE_LIST(config, "ocs_list").set_original("amigaocs_flop");
}

void a1000_state::a1000(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", amiga_keyboard_devices, "a1000_us"));
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kclk_handler().append("kbrst", FUNC(a1000_kbreset_device::kbclk_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));
	A1000_KBRESET(config, "kbrst")
			.set_delays(attotime::from_msec(152), attotime::from_usec(176), attotime::from_msec(704))
			.kbrst_cb().set(FUNC(a1000_state::kbreset_w));

	// main cpu
	M68000(config, m_maincpu, amiga_state::CLK_7M_PAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &a1000_state::a1000_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&a1000_state::a1000_overlay_map).set_options(ENDIANNESS_BIG, 16, 22, 0x200000);
	ADDRESS_MAP_BANK(config, "bootrom").set_map(&a1000_state::a1000_bootrom_map).set_options(ENDIANNESS_BIG, 16, 19, 0x40000);

	SOFTWARE_LIST(config, "a1000_list").set_original("amiga_a1000");
}

void a1000_state::a1000n(machine_config &config)
{
	a1000(config);

	m_maincpu->set_clock(amiga_state::CLK_7M_NTSC);
	config.device_remove("screen");
	ntsc_video(config);
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a2000_state::a2000(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", amiga_keyboard_devices, "a2000_us"));
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kclk_handler().append("kbrst", FUNC(a1000_kbreset_device::kbclk_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));
	A1000_KBRESET(config, "kbrst")
			.set_delays(attotime::from_msec(112), attotime::from_msec(74), attotime::from_msec(1294))
			.kbrst_cb().set(FUNC(a2000_state::kbreset_w));

	// main cpu
	M68000(config, m_maincpu, amiga_state::CLK_7M_PAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &a2000_state::a2000_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_512kb_map).set_options(ENDIANNESS_BIG, 16, 22, 0x200000);

	// real-time clock
	MSM6242(config, m_rtc, XTAL(32'768));

	// cpu slot
	EXP_SLOT(config, EXP_SLOT_TAG, 0).set_space(m_maincpu, AS_PROGRAM);
	ZORRO_SLOT(config, "slot", EXP_SLOT_TAG, a2000_expansion_cards, nullptr);

	// zorro slots
	ZORRO2(config, m_zorro, 0);
	m_zorro->set_space(m_maincpu, AS_PROGRAM);
	m_zorro->int2_handler().set(FUNC(a2000_state::zorro2_int2_w));
	m_zorro->int6_handler().set(FUNC(a2000_state::zorro2_int6_w));
	ZORRO_SLOT(config, "zorro1", m_zorro, zorro2_cards, nullptr);
	ZORRO_SLOT(config, "zorro2", m_zorro, zorro2_cards, nullptr);
	ZORRO_SLOT(config, "zorro3", m_zorro, zorro2_cards, nullptr);
	ZORRO_SLOT(config, "zorro4", m_zorro, zorro2_cards, nullptr);
	ZORRO_SLOT(config, "zorro5", m_zorro, zorro2_cards, nullptr);
}

void a2000_state::a2000n(machine_config &config)
{
	a2000(config);

	m_maincpu->set_clock(amiga_state::CLK_7M_NTSC);
	config.device_remove("screen");
	ntsc_video(config);
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a500_state::a500(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", a500_keyboard_devices, "a500_us"));
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));
	kbd.krst_handler().set(FUNC(amiga_state::kbreset_w));

	// main cpu
	M68000(config, m_maincpu, amiga_state::CLK_7M_PAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &a500_state::a500_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_1mb_map).set_options(ENDIANNESS_BIG, 16, 22, 0x200000);

	// cpu slot
	EXP_SLOT(config, m_side, 0).set_space(m_maincpu, AS_PROGRAM);
	m_side->int2_handler().set(FUNC(a500_state::side_int2_w));
	m_side->int6_handler().set(FUNC(a500_state::side_int6_w));
	ZORRO_SLOT(config, "slot", m_side, a500_expansion_cards, nullptr);
}

void a500_state::a500n(machine_config &config)
{
	a500(config);
	m_maincpu->set_clock(amiga_state::CLK_7M_NTSC);
	config.device_remove("screen");
	ntsc_video(config);
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void cdtv_state::cdtv(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", amiga_keyboard_devices, "a2000_us"));
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kclk_handler().append("kbrst", FUNC(a1000_kbreset_device::kbclk_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));
	A1000_KBRESET(config, "kbrst")
			.set_delays(attotime::from_usec(11238), attotime::from_usec(7432), attotime::from_usec(27539))
			.kbrst_cb().set(FUNC(a1000_state::kbreset_w));

	// main cpu
	M68000(config, m_maincpu, amiga_state::CLK_7M_PAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &cdtv_state::cdtv_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	// remote control input converter
	m6502_device &u75(M6502(config, "u75", XTAL(3'000'000)));
	u75.set_addrmap(AS_PROGRAM, &cdtv_state::cdtv_rc_mem);
	u75.set_disable();

	// lcd controller
#if 0
	lc6554_device &u62(LC6554(config, "u62", XTAL(4'000'000))); // device isn't emulated yet
	u62.set_addrmap(AS_PROGRAM, &cdtv_state::lcd_mem);
#endif

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_1mb_map).set_options(ENDIANNESS_BIG, 16, 22, 0x200000);

	// standard sram
	NVRAM(config, "sram", nvram_device::DEFAULT_ALL_0);

	// 256kb memory card
	NVRAM(config, "memcard", nvram_device::DEFAULT_ALL_0);

	// real-time clock
	MSM6242(config, m_rtc, XTAL(32'768));

	// cd-rom controller
	AMIGA_DMAC(config, m_dmac, amiga_state::CLK_7M_PAL);
	m_dmac->scsi_read_handler().set(FUNC(cdtv_state::dmac_scsi_data_read));
	m_dmac->scsi_write_handler().set(FUNC(cdtv_state::dmac_scsi_data_write));
	m_dmac->io_read_handler().set(FUNC(cdtv_state::dmac_io_read));
	m_dmac->io_write_handler().set(FUNC(cdtv_state::dmac_io_write));
	m_dmac->int_handler().set(FUNC(cdtv_state::dmac_int_w));

	TPI6525(config, m_tpi, 0);
	m_tpi->out_irq_cb().set(FUNC(cdtv_state::tpi_int_w));
	m_tpi->out_pb_cb().set(FUNC(cdtv_state::tpi_port_b_write));

	// cd-rom
	CR511B(config, m_cdrom, 0);
	m_cdrom->scor_handler().set(m_tpi, FUNC(tpi6525_device::i1_w)).invert();
	m_cdrom->stch_handler().set(m_tpi, FUNC(tpi6525_device::i2_w)).invert();
	m_cdrom->sten_handler().set(m_tpi, FUNC(tpi6525_device::i3_w));
	m_cdrom->xaen_handler().set(m_tpi, FUNC(tpi6525_device::pb2_w));
	m_cdrom->drq_handler().set(m_dmac, FUNC(amiga_dmac_device::xdreq_w));
	m_cdrom->dten_handler().set(m_dmac, FUNC(amiga_dmac_device::xdreq_w));

	// software
	SOFTWARE_LIST(config, "cd_list").set_original("cdtv");
}

void cdtv_state::cdtvn(machine_config &config)
{
	cdtv(config);
	m_maincpu->set_clock(amiga_state::CLK_7M_NTSC);
	config.device_remove("screen");
	ntsc_video(config);
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_dmac->set_clock(amiga_state::CLK_7M_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a3000_state::a3000(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", amiga_keyboard_devices, "a2000_us"));
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));

	// main cpu
	M68030(config, m_maincpu, XTAL(32'000'000) / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &a3000_state::a3000_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_1mb_map32).set_options(ENDIANNESS_BIG, 32, 22, 0x200000);

	// real-time clock
	RP5C01(config, "rtc", XTAL(32'768));

	// todo: zorro3 slots, super dmac, scsi

	// software
	SOFTWARE_LIST(config, "a3000_list").set_original("amiga_a3000");
	SOFTWARE_LIST(config, "ecs_list").set_original("amigaecs_flop");
}

void a3000_state::a3000n(machine_config &config)
{
	a3000(config);
	config.device_remove("screen");
	ntsc_video(config);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a500p_state::a500p(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", a500_keyboard_devices, "a500_us"));
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));
	kbd.krst_handler().set(FUNC(amiga_state::kbreset_w));

	// main cpu
	M68000(config, m_maincpu, amiga_state::CLK_7M_PAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &a500p_state::a500p_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_1mb_map).set_options(ENDIANNESS_BIG, 16, 22, 0x200000);

	// real-time clock
	MSM6242(config, m_rtc, XTAL(32'768));

	// cpu slot
	EXP_SLOT(config, m_side, 0).set_space(m_maincpu, AS_PROGRAM);
	ZORRO_SLOT(config, "slot", m_side, a500_expansion_cards, nullptr);

	// software
	SOFTWARE_LIST(config, "ecs_list").set_original("amigaecs_flop");
}

void a500p_state::a500pn(machine_config &config)
{
	a500p(config);
	m_maincpu->set_clock(amiga_state::CLK_7M_NTSC);
	config.device_remove("screen");
	ntsc_video(config);
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a600_state::a600(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", a600_keyboard_devices, "a600_us"));
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));
	kbd.krst_handler().set(FUNC(amiga_state::kbreset_w));

	// main cpu
	M68000(config, m_maincpu, amiga_state::CLK_7M_PAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &a600_state::a600_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_2mb_map16).set_options(ENDIANNESS_BIG, 16, 22, 0x200000);

	gayle_device &gayle(GAYLE(config, "gayle", amiga_state::CLK_28M_PAL / 2));
	gayle.set_id(a600_state::GAYLE_ID);
	gayle.int2_handler().set(FUNC(a600_state::gayle_int2_w));
	gayle.cs0_read_handler().set("ata", FUNC(ata_interface_device::cs0_r));
	gayle.cs0_write_handler().set("ata", FUNC(ata_interface_device::cs0_w));
	gayle.cs1_read_handler().set("ata", FUNC(ata_interface_device::cs1_r));
	gayle.cs1_write_handler().set("ata", FUNC(ata_interface_device::cs1_w));

	ata_interface_device &ata(ATA_INTERFACE(config, "ata").options(ata_devices, "hdd", nullptr, false));
	ata.irq_handler().set("gayle", FUNC(gayle_device::ide_interrupt_w));

	// todo: pcmcia

	// software
	SOFTWARE_LIST(config, "ecs_list").set_original("amigaecs_flop");
}

void a600_state::a600n(machine_config &config)
{
	a600(config);
	m_maincpu->set_clock(amiga_state::CLK_7M_NTSC);
	subdevice<gayle_device>("gayle")->set_clock(amiga_state::CLK_28M_NTSC / 2);
	config.device_remove("screen");
	ntsc_video(config);
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a1200_state::a1200(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", amiga_keyboard_devices, "a1200_us")); // FIXME: replace with Amiga 1200 devices when we have mask ROM dump
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));
	kbd.krst_handler().set(FUNC(amiga_state::kbreset_w));

	// main cpu
	M68EC020(config, m_maincpu, amiga_state::CLK_28M_PAL / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &a1200_state::a1200_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_2mb_map32).set_options(ENDIANNESS_BIG, 32, 22, 0x200000);

	m_screen->set_screen_update(FUNC(amiga_state::screen_update_amiga_aga));

	config.device_remove("palette");

	MCFG_VIDEO_START_OVERRIDE(amiga_state, amiga_aga)

	gayle_device &gayle(GAYLE(config, "gayle", amiga_state::CLK_28M_PAL / 2));
	gayle.set_id(a1200_state::GAYLE_ID);
	gayle.int2_handler().set(FUNC(a1200_state::gayle_int2_w));
	gayle.cs0_read_handler().set("ata", FUNC(ata_interface_device::cs0_r));
	gayle.cs0_write_handler().set("ata", FUNC(ata_interface_device::cs0_w));
	gayle.cs1_read_handler().set("ata", FUNC(ata_interface_device::cs1_r));
	gayle.cs1_write_handler().set("ata", FUNC(ata_interface_device::cs1_w));

	ata_interface_device &ata(ATA_INTERFACE(config, "ata").options(ata_devices, "hdd", nullptr, false));
	ata.irq_handler().set("gayle", FUNC(gayle_device::ide_interrupt_w));

	// keyboard
#if 0
	subdevice<amiga_keyboard_bus_device>("kbd").set_default_option("a1200_us");
#endif

	// todo: pcmcia

	// software
	SOFTWARE_LIST(config, "aga_list").set_original("amigaaga_flop");
	SOFTWARE_LIST(config, "ecs_list").set_original("amigaecs_flop");
}

void a1200_state::a1200n(machine_config &config)
{
	a1200(config);
	m_maincpu->set_clock(amiga_state::CLK_28M_NTSC / 2);
	subdevice<gayle_device>("gayle")->set_clock(amiga_state::CLK_28M_NTSC / 2);
	config.device_remove("screen");
	ntsc_video(config);
	m_screen->set_screen_update(FUNC(amiga_state::screen_update_amiga_aga));
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a4000_state::a4000(machine_config &config)
{
	amiga_base(config);

	// keyboard
	auto &kbd(AMIGA_KEYBOARD_INTERFACE(config, "kbd", amiga_keyboard_devices, "a2000_us"));
	kbd.kclk_handler().set("cia_0", FUNC(mos8520_device::cnt_w));
	kbd.kdat_handler().set("cia_0", FUNC(mos8520_device::sp_w));

	// main cpu
	M68040(config, m_maincpu, XTAL(50'000'000) / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &a4000_state::a4000_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_2mb_map32).set_options(ENDIANNESS_BIG, 32, 22, 0x200000);

	m_screen->set_screen_update(FUNC(amiga_state::screen_update_amiga_aga));

	config.device_remove("palette");

	MCFG_VIDEO_START_OVERRIDE(amiga_state, amiga_aga)

	// real-time clock
	RP5C01(config, "rtc", XTAL(32'768));

	// ide
	ata_interface_device &ata(ATA_INTERFACE(config, "ata").options(ata_devices, "hdd", nullptr, false));
	ata.irq_handler().set(FUNC(a4000_state::ide_interrupt_w));

	// todo: zorro3

	// software
	SOFTWARE_LIST(config, "aga_list").set_original("amigaaga_flop");
	SOFTWARE_LIST(config, "ecs_list").set_original("amigaecs_flop");
}

void a4000_state::a4000n(machine_config &config)
{
	a4000(config);

	config.device_remove("screen");
	ntsc_video(config);
	m_screen->set_screen_update(FUNC(amiga_state::screen_update_amiga_aga));
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a4000_state::a400030(machine_config &config)
{
	a4000(config);
	// main cpu
	M68EC030(config.replace(), m_maincpu, XTAL(50'000'000) / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &a4000_state::a400030_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	// todo: ide
}

void a4000_state::a400030n(machine_config &config)
{
	a400030(config);
	config.device_remove("screen");
	ntsc_video(config);
	m_screen->set_screen_update(FUNC(amiga_state::screen_update_amiga_aga));
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void cd32_state::cd32(machine_config &config)
{
	amiga_base(config);

	// main cpu
	M68EC020(config, m_maincpu, amiga_state::CLK_28M_PAL / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &cd32_state::cd32_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	ADDRESS_MAP_BANK(config, "overlay").set_map(&amiga_state::overlay_2mb_map32).set_options(ENDIANNESS_BIG, 32, 22, 0x200000);

	I2CMEM(config, "i2cmem", 0).set_page_size(16).set_data_size(1024);

	akiko_device &akiko(AKIKO(config, "akiko", 0));
	akiko.mem_r_callback().set(FUNC(amiga_state::chip_ram_r));
	akiko.mem_w_callback().set(FUNC(amiga_state::chip_ram_w));
	akiko.int_callback().set(FUNC(cd32_state::akiko_int_w));
	akiko.scl_callback().set("i2cmem", FUNC(i2cmem_device::write_scl));
	akiko.sda_r_callback().set("i2cmem", FUNC(i2cmem_device::read_sda));
	akiko.sda_w_callback().set("i2cmem", FUNC(i2cmem_device::write_sda));

	m_screen->set_screen_update(FUNC(amiga_state::screen_update_amiga_aga));

	config.device_remove("palette");

	MCFG_VIDEO_START_OVERRIDE(amiga_state, amiga_aga)

	CDDA(config, m_cdda);
	m_cdda->add_route(0, "lspeaker", 0.50);
	m_cdda->add_route(1, "rspeaker", 0.50);

	m_cia_0->pa_wr_callback().set(FUNC(cd32_state::akiko_cia_0_port_a_write));
	m_cia_0->sp_wr_callback().set_nop();

	CDROM(config, "cdrom").set_interface("cd32_cdrom");
	SOFTWARE_LIST(config, "cd_list").set_original("cd32");
}

void cd32_state::cd32n(machine_config &config)
{
	cd32(config);

	m_maincpu->set_clock(amiga_state::CLK_28M_NTSC / 2);
	config.device_remove("screen");
	ntsc_video(config);
	m_screen->set_screen_update(FUNC(amiga_state::screen_update_amiga_aga));
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}

void a4000_state::a4000t(machine_config &config)
{
	a4000(config);
	// main cpu
	M68040(config.replace(), m_maincpu, XTAL(50'000'000) / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &a4000_state::a4000t_mem);
	m_maincpu->set_cpu_space(AS_PROGRAM);

	// todo: ide, zorro3, scsi, super dmac
}

void a4000_state::a4000tn(machine_config &config)
{
	a4000(config);

	config.device_remove("screen");
	ntsc_video(config);
	m_screen->set_screen_update(FUNC(amiga_state::screen_update_amiga_aga));
	m_paula->set_clock(amiga_state::CLK_C1_NTSC);
	m_cia_0->set_clock(amiga_state::CLK_E_NTSC);
	m_cia_1->set_clock(amiga_state::CLK_E_NTSC);
	m_fdc->set_clock(amiga_state::CLK_7M_NTSC);
}


//**************************************************************************
//  ROM DEFINITIONS
//**************************************************************************

// Amiga 1000
//
// Shipped with a small bootrom to load kickstart from disk because the
// Kickstart wasn't finished in time. ROM type is 23256, but only the
// first 4kb of it are used.

ROM_START( a1000 )
	ROM_REGION16_BE(0x10000, "bootrom", 0)
	ROM_LOAD16_BYTE("252179-01.u5n", 0x0000, 0x8000, CRC(76bd46ec) SHA1(2155b4887f064c5e01e0a2ebb4a0cc2a3e88d9e8))
	ROM_LOAD16_BYTE("252180-01.u5p", 0x0001, 0x8000, CRC(dd516b6d) SHA1(2c307d02f10ad332a479b50767fd0463efc2844b))

	// PALs, all of type PAL16L8
	ROM_REGION(0x104, "dpalen", 0)
	ROM_LOAD("252128-01.u4t", 0, 0x104, CRC(28209ff2) SHA1(20c03b6b8e7254231f4b3014dc2c4d9274d469d2))
	ROM_REGION(0x104, "dpalcas", 0)
	ROM_LOAD("252128-02.u6p", 0, 0x104, CRC(b928efd2) SHA1(430794a544d9160e1b786e97e0dec5f25502a00a))
	ROM_REGION(0x104, "daugen", 0)
	ROM_LOAD("252128-03.u4s", 0, 0x104, CRC(87747964) SHA1(00d72ec707c582363525fde56176973c7327b1d7))
	ROM_REGION(0x104, "daugcas", 0)
	ROM_LOAD("252128-04.u6n", 0, 0x104, CRC(f903adb4) SHA1(4c8fb696fd1aaf9bb8c9efddeac24bb36f119c5f))
ROM_END

#define rom_a1000n  rom_a1000

// Amiga 2000 and Amiga 500
//
// Early models shipped with Kickstart 1.2, later versions with Kickstart 1.3.
// Kickstart 2.04 and 3.1 upgrade available. The Kickstart 2.04 upgrade was also
// available as a special version that included a jumper wire, which was needed
// for some early motherboard revisions (P/N: 363968-01).

ROM_START( a2000 )
	ROM_REGION16_BE(0x80000, "kickstart", 0)
	ROM_DEFAULT_BIOS("kick13")
	ROM_SYSTEM_BIOS(0, "kick12",  "Kickstart 1.2 (33.180)")
	ROMX_LOAD("315093-01.u2", 0x00000, 0x40000, CRC(a6ce1636) SHA1(11f9e62cf299f72184835b7b2a70a16333fc0d88), ROM_GROUPWORD | ROM_BIOS(0))
	ROM_COPY("kickstart", 0x00000, 0x40000, 0x40000)
	ROM_SYSTEM_BIOS(1, "kick13",  "Kickstart 1.3 (34.5)")
	ROMX_LOAD("315093-02.u2", 0x00000, 0x40000, CRC(c4f0f55f) SHA1(891e9a547772fe0c6c19b610baf8bc4ea7fcb785), ROM_GROUPWORD | ROM_BIOS(1))
	ROM_COPY("kickstart", 0x00000, 0x40000, 0x40000)
	ROM_SYSTEM_BIOS(2, "kick204", "Kickstart 2.04 (37.175)")
	ROMX_LOAD("390979-01.u2", 0x00000, 0x80000, CRC(c3bdb240) SHA1(c5839f5cb98a7a8947065c3ed2f14f5f42e334a1), ROM_GROUPWORD | ROM_BIOS(2))
	ROM_SYSTEM_BIOS(3, "kick31",  "Kickstart 3.1 (40.63)")
	ROMX_LOAD("kick40063.u2", 0x00000, 0x80000, CRC(fc24ae0d) SHA1(3b7f1493b27e212830f989f26ca76c02049f09ca), ROM_GROUPWORD | ROM_BIOS(3))
	ROM_SYSTEM_BIOS(4, "logica2", "Logica Diagnostic 2.0")
	ROMX_LOAD("logica2.u2",   0x00000, 0x80000, CRC(8484f426) SHA1(ba10d16166b2e2d6177c979c99edf8462b21651e), ROM_GROUPWORD | ROM_BIOS(4))
ROM_END

// Amiga 2000CR chip location: U500
#define rom_a2000n  rom_a2000

// Amiga 500 chip location: U6
#define rom_a500   rom_a2000
#define rom_a500n  rom_a2000

// Amiga 500+
//
// Shipped with Kickstart 2.04. Kickstart 3.1 upgrade available.

ROM_START( a500p )
	ROM_REGION16_BE(0x80000, "kickstart", 0)
	ROM_DEFAULT_BIOS("kick204")
	ROM_SYSTEM_BIOS(0, "kick204", "Kickstart 2.04 (37.175)")
	ROMX_LOAD("390979-01.u6", 0x00000, 0x80000, CRC(c3bdb240) SHA1(c5839f5cb98a7a8947065c3ed2f14f5f42e334a1), ROM_GROUPWORD | ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "kick31",  "Kickstart 3.1 (40.63)")
	ROMX_LOAD("kick40063.u6", 0x00000, 0x80000, CRC(fc24ae0d) SHA1(3b7f1493b27e212830f989f26ca76c02049f09ca), ROM_GROUPWORD | ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "logica2", "Logica Diagnostic 2.0")
	ROMX_LOAD("logica2.u6",   0x00000, 0x80000, CRC(8484f426) SHA1(ba10d16166b2e2d6177c979c99edf8462b21651e), ROM_GROUPWORD | ROM_BIOS(2))
ROM_END

#define rom_a500pn  rom_a500p

// Commodore CDTV
//
// Shipped with a standard Kickstart 1.3 and the needed additional drivers
// in two extra chips.

ROM_START( cdtv )
	// cd-rom driver
	ROM_REGION16_BE(0x40000, "cdrom", 0)
	ROM_LOAD16_BYTE("391008-01.u34", 0x00000, 0x20000, CRC(791cb14b) SHA1(277a1778924496353ffe56be68063d2a334360e4))
	ROM_LOAD16_BYTE("391009-01.u35", 0x00001, 0x20000, CRC(accbbc2e) SHA1(41b06d1679c6e6933c3378b7626025f7641ebc5c))

	// standard amiga kickstart 1.3
	ROM_REGION16_BE(0x80000, "kickstart", 0)
	ROMX_LOAD("315093-02.u13", 0x00000, 0x40000, CRC(c4f0f55f) SHA1(891e9a547772fe0c6c19b610baf8bc4ea7fcb785), ROM_GROUPWORD)
	ROM_COPY("kickstart", 0x00000, 0x40000, 0x40000)

	// remote control input converter, mos 6500/1 mcu
	ROM_REGION(0x800, "rcmcu", 0)
	ROM_LOAD("252609-02.u75", 0x000, 0x800, NO_DUMP)

	// lcd controller, sanyo lc6554h
	ROM_REGION(0x1000, "lcd", 0)
	ROM_LOAD("252608-01.u62", 0x0000, 0x1000, NO_DUMP)
ROM_END

#define rom_cdtvn  rom_cdtv

// Amiga 3000
//
// Early models have a special version of Kickstart 1.4/2.0 that boots
// Kickstart 1.3 or 2.0 from hard disk or floppy. Later versions have
// Kickstart 2.04 installed as ROM. Upgrade available for
// Kickstart 3.1.

ROM_START( a3000 )
	ROM_REGION32_BE(0x80000, "kickstart", 0)
	ROM_DEFAULT_BIOS("kick20")
	ROM_SYSTEM_BIOS(0, "kick14", "Kickstart 1.4 (3312.20085?)")
	ROMX_LOAD("390629-01.u182", 0x00000, 0x40000, NO_DUMP, ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("390630-01.u183", 0x00002, 0x40000, NO_DUMP, ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "kick20", "Kickstart 2.0 (36.16)")
	// COPYRIGHT 1990 CAI // ALL RIGHTS RESERVED // ALPHA 5 ROM 0 CS=9713
	ROMX_LOAD("390629-02.u182", 0x00000, 0x40000, CRC(58327536) SHA1(d1713d7f31474a5948e6d488e33686061cf3d1e2), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(1))
	// COPYRIGHT 1990 CAI // ALL RIGHTS RESERVED // ALPHA 5 ROM 1 CS=9B21
	ROMX_LOAD("390630-02.u183", 0x00002, 0x40000, CRC(fe2f7fb9) SHA1(c05c9c52d014c66f9019152b3f2a2adc2c678794), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "kick204", "Kickstart 2.04 (37.175)")
	ROMX_LOAD("390629-03.u182", 0x00000, 0x40000, CRC(a245dbdf) SHA1(83bab8e95d378b55b0c6ae6561385a96f638598f), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(2))
	ROMX_LOAD("390630-03.u183", 0x00002, 0x40000, CRC(7db1332b) SHA1(48f14b31279da6757848df6feb5318818f8f576c), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(2))
	ROM_SYSTEM_BIOS(3, "kick31", "Kickstart 3.1 (40.68)")
	ROMX_LOAD("kick31.u182",    0x00000, 0x40000, CRC(286b9a0d) SHA1(6763a2258ec493f7408cf663110dae9a17803ad1), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(3))
	ROMX_LOAD("kick31.u183",    0x00002, 0x40000, CRC(0b8cde6a) SHA1(5f02e97b48ebbba87d516a56b0400c6fc3434d8d), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(3))
	ROM_SYSTEM_BIOS(4, "logica2", "Logica Diagnostic 2.0")
	ROMX_LOAD("logica2.u182",    0x00000, 0x40000, CRC(566bc3f9) SHA1(891d3b7892843517d800d24593168b1d8f1646ca), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(4))
	ROMX_LOAD("logica2.u183",    0x00002, 0x40000, CRC(aac94759) SHA1(da8a4f9ae1aa84f5e2a5dcc5c9d7e4378a9698b7), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(4))
ROM_END

#define rom_a3000n  rom_a3000


// Amiga 600
//
// According to Greg Donner's Workbench page, very early models shipped with
// Kickstart 2.04.
//
// Kickstart 2.05 differences:
// - 2.05 37.299: No HDD support
// - 2.05 37.300: HDD support
// - 2.05 37.350: HDD size limits removed
//
// Kickstart 3.1 upgrade available.
//
// The keyboard controller is included on the motherboard, still based on the
// 6500/1.

ROM_START( a600 )
	ROM_REGION16_BE(0x80000, "kickstart", 0)
	ROM_DEFAULT_BIOS("kick205-350")
	ROM_SYSTEM_BIOS(0, "kick204", "Kickstart 2.04 (37.175)")
	ROMX_LOAD("390979-01.u6", 0x00000, 0x80000, CRC(c3bdb240) SHA1(c5839f5cb98a7a8947065c3ed2f14f5f42e334a1), ROM_GROUPWORD | ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "kick205-299", "Kickstart 2.05 (37.299)")
	ROMX_LOAD("391388-01.u6", 0x00000, 0x80000, CRC(83028fb5) SHA1(87508de834dc7eb47359cede72d2e3c8a2e5d8db), ROM_GROUPWORD | ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "kick205-300", "Kickstart 2.05 (37.300)")
	ROMX_LOAD("391304-01.u6", 0x00000, 0x80000, CRC(64466c2a) SHA1(f72d89148dac39c696e30b10859ebc859226637b), ROM_GROUPWORD | ROM_BIOS(2))
	ROM_SYSTEM_BIOS(3, "kick205-350", "Kickstart 2.05 (37.350)")
	ROMX_LOAD("391304-02.u6", 0x00000, 0x80000, CRC(43b0df7b) SHA1(02843c4253bbd29aba535b0aa3bd9a85034ecde4), ROM_GROUPWORD | ROM_BIOS(3))
	ROM_SYSTEM_BIOS(4, "kick31",  "Kickstart 3.1 (40.63)")
	ROMX_LOAD("kick40063.u6", 0x00000, 0x80000, CRC(fc24ae0d) SHA1(3b7f1493b27e212830f989f26ca76c02049f09ca), ROM_GROUPWORD | ROM_BIOS(4))
	ROM_SYSTEM_BIOS(5, "logica2", "Logica Diagnostic 2.0")
	ROMX_LOAD("logica2.u6",   0x00000, 0x80000, CRC(8484f426) SHA1(ba10d16166b2e2d6177c979c99edf8462b21651e), ROM_GROUPWORD | ROM_BIOS(5))

	ROM_REGION(0x800, "keyboard", 0)
	ROM_LOAD("391079-01.u13", 0x000, 0x800, NO_DUMP)
ROM_END

#define rom_a600n  rom_a600

// Amiga 1200
//
// Early models shipped with Kickstart 3.0, later versions with
// Kickstart 3.1. Keyboard controller is included on the motherboard,
// but was changed to a 68HC05 core.

ROM_START( a1200 )
	ROM_REGION32_BE(0x80000, "kickstart", 0)
	ROM_DEFAULT_BIOS("kick31")
	ROM_SYSTEM_BIOS(0, "kick30", "Kickstart 3.0 (39.106)")
	ROMX_LOAD("391523-01.u6a", 0x00000, 0x40000, CRC(c742a412) SHA1(999eb81c65dfd07a71ee19315d99c7eb858ab186), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("391524-01.u6b", 0x00002, 0x40000, CRC(d55c6ec6) SHA1(3341108d3a402882b5ef9d3b242cbf3c8ab1a3e9), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "kick31", "Kickstart 3.1 (40.68)")
	ROMX_LOAD("391773-01.u6a", 0x00000, 0x40000, CRC(08dbf275) SHA1(b8800f5f909298109ea69690b1b8523fa22ddb37), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(1))
	ROMX_LOAD("391774-01.u6b", 0x00002, 0x40000, CRC(16c07bf8) SHA1(90e331be1970b0e53f53a9b0390b51b59b3869c2), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "logica2", "Logica Diagnostic 2.0")
	ROMX_LOAD("logica2.u6a", 0x00000, 0x40000, CRC(566bc3f9) SHA1(891d3b7892843517d800d24593168b1d8f1646ca), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(2))
	ROMX_LOAD("logica2.u6b", 0x00002, 0x40000, CRC(aac94759) SHA1(da8a4f9ae1aa84f5e2a5dcc5c9d7e4378a9698b7), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(2))
ROM_END

#define rom_a1200n  rom_a1200

// Amiga 4000
//
// Shipped with Kickstart 3.0, upgradable to Kickstart 3.1.

ROM_START( a4000 )
	ROM_REGION32_BE(0x80000, "kickstart", 0)
	ROM_DEFAULT_BIOS("kick30")
	ROM_SYSTEM_BIOS(0, "kick30", "Kickstart 3.0 (39.106)")
	ROMX_LOAD("391513-02.u175", 0x00000, 0x40000, CRC(36f64dd0) SHA1(196e9f3f9cad934e181c07da33083b1f0a3c702f), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("391514-02.u176", 0x00002, 0x40000, CRC(17266a55) SHA1(42fbed3453d1f11ccbde89a9826f2d1175cca5cc), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "kick31-68", "Kickstart 3.1 (40.68)")
	ROMX_LOAD("kick40068.u175", 0x00000, 0x40000, CRC(b2af34f8) SHA1(24e52b5efc02049517387ab7b1a1475fc540350e), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(1))
	ROMX_LOAD("kick40068.u176", 0x00002, 0x40000, CRC(e65636a3) SHA1(313c7cbda5779e56f19a41d34e760f517626d882), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "kick31-70", "Kickstart 3.1 (40.70)")
	ROMX_LOAD("kick40070.u175", 0x00000, 0x40000, CRC(f9cbecc9) SHA1(138d8cb43b8312fe16d69070de607469b3d4078e), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(2))
	ROMX_LOAD("kick40070.u176", 0x00002, 0x40000, CRC(f8248355) SHA1(c23795479fae3910c185512ca268b82f1ae4fe05), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(2))
	ROM_SYSTEM_BIOS(3, "logica2", "Logica Diagnostic 2.0")
	ROMX_LOAD("logica2.u6a",    0x00000, 0x40000, CRC(566bc3f9) SHA1(891d3b7892843517d800d24593168b1d8f1646ca), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(3))
	ROMX_LOAD("logica2.u6b",    0x00002, 0x40000, CRC(aac94759) SHA1(da8a4f9ae1aa84f5e2a5dcc5c9d7e4378a9698b7), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(3))
ROM_END

#define rom_a4000n    rom_a4000
#define rom_a400030   rom_a4000
#define rom_a400030n  rom_a4000

// Amiga 4000T
//
// Shipped with Kickstart 3.1 (40.70).

ROM_START( a4000t )
	ROM_REGION32_BE(0x80000, "kickstart", 0)
	ROM_DEFAULT_BIOS("kick31")
	ROM_SYSTEM_BIOS(0, "kick31", "Kickstart 3.1 (40.70)")
	ROMX_LOAD("391657-01.u175", 0x00000, 0x40000, CRC(0ca94f70) SHA1(b3806edacb3362fc16a154ce1eeec5bf5bc24789), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(0))
	ROMX_LOAD("391658-01.u176", 0x00002, 0x40000, CRC(dfe03120) SHA1(cd7a706c431b04d87814d3a2d8b397100cf44c0c), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "logica2", "Logica Diagnostic 2.0")
	ROMX_LOAD("logica2.u6a",    0x00000, 0x40000, CRC(566bc3f9) SHA1(891d3b7892843517d800d24593168b1d8f1646ca), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(1))
	ROMX_LOAD("logica2.u6b",    0x00002, 0x40000, CRC(aac94759) SHA1(da8a4f9ae1aa84f5e2a5dcc5c9d7e4378a9698b7), ROM_GROUPWORD | ROM_REVERSE | ROM_SKIP(2) | ROM_BIOS(1))
ROM_END

#define rom_a4000tn  rom_a4000t

// Amiga CD32
//
// Shipped with Kickstart 3.1 and additional software interleaved in a 1MB rom chip.

ROM_START( cd32 )
	ROM_REGION32_BE(0x100000, "kickstart", 0)
//  ROM_LOAD16_WORD("391640-03.u6a", 0x000000, 0x100000, CRC(a4fbc94a) SHA1(816ce6c5077875850c7d43452230a9ba3a2902db)) // todo: this is the real dump
	ROM_LOAD16_WORD("391640-03.u6a", 0x000000, 0x100000, CRC(d3837ae4) SHA1(06807db3181637455f4d46582d9972afec8956d9))
ROM_END

#define rom_cd32n  rom_cd32


//**************************************************************************
//  SYSTEM DRIVERS
//**************************************************************************

// OCS Chipset
COMP( 1985, a1000,    0,      0, a1000,    amiga, a1000_state, init_pal,  "Commodore", "Amiga 1000 (PAL)",      MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1985, a1000n,   a1000,  0, a1000n,   amiga, a1000_state, init_ntsc, "Commodore", "Amiga 1000 (NTSC)",     MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1987, a2000,    0,      0, a2000,    amiga, a2000_state, init_pal,  "Commodore", "Amiga 2000 (PAL)",      MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1987, a2000n,   a2000,  0, a2000n,   amiga, a2000_state, init_ntsc, "Commodore", "Amiga 2000 (NTSC)",     MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1987, a500,     0,      0, a500,     amiga, a500_state,  init_pal,  "Commodore", "Amiga 500 (PAL)",       MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1987, a500n,    a500,   0, a500n,    amiga, a500_state,  init_ntsc, "Commodore", "Amiga 500 (NTSC)",      MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1990, cdtv,     0,      0, cdtv,     amiga, cdtv_state,  init_pal,  "Commodore", "CDTV (PAL)",            MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1990, cdtvn,    cdtv,   0, cdtvn,    amiga, cdtv_state,  init_ntsc, "Commodore", "CDTV (NTSC)",           MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )

// ECS Chipset
COMP( 1990, a3000,    0,      0, a3000,    amiga, a3000_state, init_pal,  "Commodore", "Amiga 3000 (PAL)",      MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1990, a3000n,   a3000,  0, a3000n,   amiga, a3000_state, init_ntsc, "Commodore", "Amiga 3000 (NTSC)",     MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1992, a500p,    0,      0, a500p,    amiga, a500p_state, init_pal,  "Commodore", "Amiga 500 Plus (PAL)",  MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1992, a500pn,   a500p,  0, a500pn,   amiga, a500p_state, init_ntsc, "Commodore", "Amiga 500 Plus (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1992, a600,     0,      0, a600,     amiga, a600_state,  init_pal,  "Commodore", "Amiga 600 (PAL)",       MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1992, a600n,    a600,   0, a600n,    amiga, a600_state,  init_ntsc, "Commodore", "Amiga 600 (NTSC)",      MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )

// AGA Chipset
COMP( 1992, a1200,    0,      0, a1200,    amiga, a1200_state, init_pal,  "Commodore", "Amiga 1200 (PAL)",      MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1992, a1200n,   a1200,  0, a1200n,   amiga, a1200_state, init_ntsc, "Commodore", "Amiga 1200 (NTSC)",     MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1992, a4000,    0,      0, a4000,    amiga, a4000_state, init_pal,  "Commodore", "Amiga 4000/040 (PAL)",  MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1992, a4000n,   a4000,  0, a4000n,   amiga, a4000_state, init_ntsc, "Commodore", "Amiga 4000/040 (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1993, a400030,  a4000,  0, a400030,  amiga, a4000_state, init_pal,  "Commodore", "Amiga 4000/030 (PAL)",  MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1993, a400030n, a4000,  0, a400030n, amiga, a4000_state, init_ntsc, "Commodore", "Amiga 4000/030 (NTSC)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1993, cd32,     0,      0, cd32,     cd32,  cd32_state,  init_pal,  "Commodore", "Amiga CD32 (PAL)",      MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1993, cd32n,    cd32,   0, cd32n,    cd32,  cd32_state,  init_ntsc, "Commodore", "Amiga CD32 (NTSC)",     MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1994, a4000t,   0,      0, a4000t,   amiga, a4000_state, init_pal,  "Commodore", "Amiga 4000T (PAL)",     MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
COMP( 1994, a4000tn,  a4000t, 0, a4000tn,  amiga, a4000_state, init_ntsc, "Commodore", "Amiga 4000T (NTSC)",    MACHINE_NOT_WORKING | MACHINE_IMPERFECT_GRAPHICS )
