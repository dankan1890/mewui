// license:BSD-3-Clause
// copyright-holders:Wilbert Pol, Angelo Salese
/***************************************************************************

    Hudson/NEC HuC6272 "King" device

***************************************************************************/

#ifndef MAME_VIDEO_HUC6272_H
#define MAME_VIDEO_HUC6272_H

#pragma once

#include "bus/scsi/scsi.h"
#include "bus/scsi/scsicd.h"
#include "video/huc6271.h"
#include "speaker.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> huc6272_device

class huc6272_device :  public device_t,
						public device_memory_interface
{
public:
	// construction/destruction
	huc6272_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto irq_changed_callback() { return m_irq_changed_cb.bind(); }
	template <typename T> void set_rainbow_tag(T &&tag) { m_huc6271.set_tag(std::forward<T>(tag)); }

	// I/O operations
	DECLARE_WRITE32_MEMBER( write );
	DECLARE_READ32_MEMBER( read );

	// ADPCM operations
	DECLARE_READ8_MEMBER( adpcm_update_0 );
	DECLARE_READ8_MEMBER( adpcm_update_1 );

	// CD-DA operations
	DECLARE_WRITE8_MEMBER( cdda_update );

	static void cdrom_config(device_t *device);

protected:
	// device-level overrides
	virtual void device_validity_check(validity_checker &valid) const override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual space_config_vector memory_space_config() const override;

private:
	required_device<huc6271_device> m_huc6271;
	required_device<speaker_device> m_cdda_l;
	required_device<speaker_device> m_cdda_r;

	uint8_t m_register;
	uint32_t m_kram_addr_r, m_kram_addr_w;
	uint16_t m_kram_inc_r,m_kram_inc_w;
	uint8_t m_kram_page_r,m_kram_page_w;
	uint32_t m_page_setting;

	struct{
		uint32_t bat_address;
		uint32_t cg_address;
		uint8_t mode;
		uint16_t height;
		uint16_t width;
		uint16_t xscroll;
		uint16_t yscroll;
		uint8_t priority;
	}m_bg[4];

	struct{
		uint32_t bat_address;
		uint32_t cg_address;
		uint16_t height;
		uint16_t width;
	}m_bg0sub;

	struct{
		uint8_t index;
		uint8_t ctrl;
	}m_micro_prg;

	struct{
		uint8_t rate;
		uint32_t status;
		int interrupt;
		uint8_t playing[2];
		uint8_t control[2];
		uint32_t start[2];
		uint32_t end[2];
		uint32_t imm[2];
		uint32_t input[2];
		int nibble[2];
		uint32_t pos[2];
		uint32_t addr[2];
	}m_adpcm;

	const address_space_config      m_program_space_config;
	const address_space_config      m_data_space_config;
	required_shared_ptr<uint16_t>   m_microprg_ram;
	required_shared_ptr<uint32_t>   m_kram_page0;
	required_shared_ptr<uint32_t>   m_kram_page1;
	required_device<scsi_port_device> m_scsibus;
	required_device<input_buffer_device> m_scsi_data_in;
	required_device<output_latch_device> m_scsi_data_out;
	required_device<input_buffer_device> m_scsi_ctrl_in;

	/* Callback for when the irq line may have changed (mandatory) */
	devcb_write_line    m_irq_changed_cb;

	uint32_t read_dword(offs_t address);
	void write_dword(offs_t address, uint32_t data);
	void write_microprg_data(offs_t address, uint16_t data);

	uint8_t adpcm_update(int chan);
	void interrupt_update();

	void kram_map(address_map &map);
	void microprg_map(address_map &map);
};

// device type definition
DECLARE_DEVICE_TYPE(HUC6272, huc6272_device)

#endif // MAME_VIDEO_HUC6272_H
