// license:BSD-3-Clause
// copyright-holders:David Haywood, Luca Elia
/* Serial Flash */

#ifndef MAME_MACHINE_SERFLASH_H
#define MAME_MACHINE_SERFLASH_H

#pragma once

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


// ======================> serflash_device

class serflash_device : public device_t, public device_nvram_interface
{
public:
	// custom initialization for default state
	typedef device_delegate<void (serflash_device &, void *, size_t)> init_delegate;

	// construction/destruction
	serflash_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration
	void set_flash_page_size(uint16_t size) { m_flash_page_size = size; }

	DECLARE_READ8_MEMBER( flash_ready_r );
	DECLARE_READ8_MEMBER( flash_io_r );
	DECLARE_WRITE8_MEMBER( flash_addr_w );
	DECLARE_WRITE8_MEMBER( flash_data_w );
	DECLARE_WRITE8_MEMBER( flash_cmd_w );
	DECLARE_WRITE8_MEMBER( flash_enab_w );
	void flash_hard_reset();

	DECLARE_READ8_MEMBER(n3d_flash_r);
	DECLARE_WRITE8_MEMBER(n3d_flash_cmd_w);
	DECLARE_WRITE8_MEMBER(n3d_flash_addr_w);

protected:
	enum class flash_state_t : u8 { IDLE = 0, READ, READ_ID, READ_STATUS, BLOCK_ERASE, PAGE_PROGRAM };

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_nvram_interface overrides
	virtual void nvram_default() override;
	virtual void nvram_read(emu_file &file) override;
	virtual void nvram_write(emu_file &file) override;

	void flash_change_state(flash_state_t state);

	// runtime state
	size_t                      m_length;
	uint8_t* m_region;

	uint32_t                    m_row_num;
	uint16_t                    m_flash_page_size;

	flash_state_t m_flash_state;

	uint8_t m_flash_enab;

	uint8_t m_flash_cmd_seq;
	uint32_t m_flash_cmd_prev;

	uint8_t m_flash_addr_seq;
	uint8_t m_flash_read_seq;

	uint32_t m_flash_row;
	uint16_t m_flash_col;
	int m_flash_page_addr;
	uint32_t m_flash_page_index;


	std::vector<uint8_t> m_flashwritemap;

	uint8_t m_last_flash_cmd;

	uint32_t m_flash_addr;

	std::vector<uint8_t> m_flash_page_data;
};


// device type definition
DECLARE_DEVICE_TYPE(SERFLASH, serflash_device)

#endif // MAME_MACHINE_SERFLASH_H
