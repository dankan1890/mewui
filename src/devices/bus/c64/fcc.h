// license:BSD-3-Clause
// copyright-holders:Curt Coder, hap
/**********************************************************************

    Tasc Final ChessCard cartridge emulation

**********************************************************************/

#ifndef MAME_BUS_C64_FCC_H
#define MAME_BUS_C64_FCC_H

#pragma once

#include "exp.h"
#include "cpu/m6502/m65sc02.h"
#include "machine/gen_latch.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_final_chesscard_device

class c64_final_chesscard_device : public device_t,
									public device_c64_expansion_card_interface,
									public device_nvram_interface
{
public:
	// construction/destruction
	c64_final_chesscard_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

	// device_nvram_interface overrides
	virtual void nvram_default() override { }
	virtual void nvram_read(emu_file &file) override { if (m_nvram != nullptr) { file.read(m_nvram, m_nvram.bytes()); } }
	virtual void nvram_write(emu_file &file) override { if (m_nvram != nullptr) { file.write(m_nvram, m_nvram.bytes()); } }

	// device_c64_expansion_card_interface overrides
	virtual uint8_t c64_cd_r(offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;
	virtual void c64_cd_w(offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;

private:
	required_device<m65sc02_device> m_maincpu;
	required_device<generic_latch_8_device> m_mainlatch;
	required_device<generic_latch_8_device> m_sublatch;

	uint8_t m_bank;
	int m_hidden;

	DECLARE_WRITE_LINE_MEMBER(mainlatch_int) { m_slot->nmi_w(state); }
	DECLARE_READ8_MEMBER(rom_r) { return m_romx[offset]; } // cartridge cpu rom
	DECLARE_READ8_MEMBER(nvram_r) { return m_nvram[offset & m_nvram.mask()]; }
	DECLARE_WRITE8_MEMBER(nvram_w) { m_nvram[offset & m_nvram.mask()] = data; }

	void c64_fcc_map(address_map &map);
};


// device type definition
DECLARE_DEVICE_TYPE(C64_FCC, c64_final_chesscard_device)


#endif // MAME_BUS_C64_FCC_H
