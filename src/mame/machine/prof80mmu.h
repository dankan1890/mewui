// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Conitec PROF-80 Memory Management Unit emulation

**********************************************************************/

#ifndef MAME_MACHINE_PROF80MMU_H
#define MAME_MACHINE_PROF80MMU_H

#pragma once


///*************************************************************************
//  TYPE DEFINITIONS
///*************************************************************************

// ======================> prof80_mmu_device

class prof80_mmu_device : public device_t, public device_memory_interface
{
public:
	prof80_mmu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void z80_program_map(address_map &map);

	DECLARE_WRITE8_MEMBER( par_w );
	DECLARE_WRITE_LINE_MEMBER( mme_w );

	void program_map(address_map &map);
protected:
	// device-level overrides
	virtual void device_start() override;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;

	DECLARE_READ8_MEMBER( program_r );
	DECLARE_WRITE8_MEMBER( program_w );

private:
	const address_space_config m_program_space_config;

	uint8_t m_blk[16];
	bool m_enabled;
};


// device type definition
DECLARE_DEVICE_TYPE(PROF80_MMU, prof80_mmu_device)



#endif // MAME_MACHINE_PROF80MMU_H
