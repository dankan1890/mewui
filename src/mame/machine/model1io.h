// license: BSD-3-Clause
// copyright-holders: Dirk Best
/***************************************************************************

    Sega Model 1/2 I/O Board

    837-8950-01
    837-8936
    837-10539

***************************************************************************/

#ifndef MAME_MACHINE_MODEL1IO_H
#define MAME_MACHINE_MODEL1IO_H

#pragma once

#include "machine/eepromser.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class model1io_device : public device_t
{
public:
	// construction/destruction
	model1io_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration
	auto read_callback() { return m_read_cb.bind(); }
	auto write_callback() { return m_write_cb.bind(); }
	template <unsigned N> auto in_callback() { return m_in_cb[N].bind(); }
	auto drive_read_callback() { return m_drive_read_cb.bind(); }
	auto drive_write_callback() { return m_drive_write_cb.bind(); }
	template <unsigned N> auto an_callback() { return m_an_cb[N].bind(); }
	auto output_callback() { return m_output_cb.bind(); }

	void mem_map(address_map &map);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	required_device<eeprom_serial_93cxx_device> m_eeprom;
	required_ioport m_buttons;
	required_ioport_array<3> m_dsw;

	DECLARE_READ8_MEMBER(io_r);
	DECLARE_WRITE8_MEMBER(io_w);

	DECLARE_WRITE8_MEMBER(io_pa_w);
	DECLARE_READ8_MEMBER(io_pb_r);
	DECLARE_READ8_MEMBER(io_pc_r);
	DECLARE_READ8_MEMBER(io_pd_r);
	DECLARE_READ8_MEMBER(io_pe_r);
	DECLARE_WRITE8_MEMBER(io_pe_w);
	DECLARE_WRITE8_MEMBER(io_pf_w);
	DECLARE_READ8_MEMBER(io_pg_r);

	ioport_value analog0_r();
	ioport_value analog1_r();
	ioport_value analog2_r();
	ioport_value analog3_r();

	devcb_read8 m_read_cb;
	devcb_write8 m_write_cb;
	devcb_read8 m_in_cb[3];
	devcb_read8 m_drive_read_cb;
	devcb_write8 m_drive_write_cb;
	devcb_read8 m_an_cb[8];
	devcb_write8 m_output_cb;

	bool m_secondary_controls;
};

// device type definition
DECLARE_DEVICE_TYPE(SEGA_MODEL1IO, model1io_device)

#endif // MAME_MACHINE_MODEL1IO_H
