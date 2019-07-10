// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Batteries Included BusCard cartridge emulation

**********************************************************************/

#ifndef MAME_BUS_C64_BUSCARD_H
#define MAME_BUS_C64_BUSCARD_H

#pragma once


#include "bus/c64/exp.h"
#include "bus/centronics/ctronics.h"
#include "bus/ieee488/ieee488.h"
#include "machine/ds75160a.h"
#include "machine/ds75161a.h"
#include "machine/i8255.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> buscard_t

class buscard_t : public device_t,
				  public device_c64_expansion_card_interface
{
public:
	// construction/destruction
	buscard_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

	// device_c64_expansion_card_interface overrides
	virtual uint8_t c64_cd_r(offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;
	virtual void c64_cd_w(offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;
	virtual int c64_game_r(offs_t offset, int sphi2, int ba, int rw) override;
	virtual int c64_exrom_r(offs_t offset, int sphi2, int ba, int rw) override;

private:
	required_device<i8255_device> m_ppi;
	required_device<ds75160a_device> m_ieee1;
	required_device<ds75161a_device> m_ieee2;
	required_device<ieee488_device> m_bus;
	required_device<centronics_device> m_centronics;
	required_device<c64_expansion_slot_device> m_exp;
	required_ioport m_s1;
	required_memory_region m_rom;

	bool m_te;
	int m_bank;
	bool m_basic;
	bool m_dipsw;
	bool m_busy;

	DECLARE_READ8_MEMBER( ppi_pa_r );
	DECLARE_WRITE8_MEMBER( ppi_pa_w );
	DECLARE_WRITE8_MEMBER( ppi_pb_w );
	DECLARE_READ8_MEMBER( ppi_pc_r );
	DECLARE_WRITE8_MEMBER( ppi_pc_w );
	DECLARE_WRITE_LINE_MEMBER( busy_w );

	bool pd_pgm1(offs_t offset, int sphi2);
	bool pd_pgm234(offs_t offset, int sphi2, int bank);
};


// device type definition
DECLARE_DEVICE_TYPE(C64_BUSCARD, buscard_t)


#endif // MAME_BUS_C64_BUSCARD_H
