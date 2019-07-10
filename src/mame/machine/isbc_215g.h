// license:BSD-3-Clause
// copyright-holders:Carl
#ifndef MAME_MACHINE_ISBC_215G_H
#define MAME_MACHINE_ISBC_215G_H

#pragma once

#include "cpu/i8089/i8089.h"
#include "bus/isbx/isbx.h"
#include "imagedev/harddriv.h"

class isbc_215g_device : public device_t
{
public:
	template <typename T>
	isbc_215g_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, uint16_t wakeup, T &&cpu_tag)
		: isbc_215g_device(mconfig, tag, owner, clock)
	{
		m_wakeup = wakeup;
		m_maincpu.set_tag(std::forward<T>(cpu_tag));
	}

	isbc_215g_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto irq_callback() { return m_out_irq_func.bind(); }

	DECLARE_WRITE8_MEMBER(write);
	DECLARE_READ16_MEMBER(io_r);
	DECLARE_WRITE16_MEMBER(io_w);
	DECLARE_READ16_MEMBER(mem_r);
	DECLARE_WRITE16_MEMBER(mem_w);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	const tiny_rom_entry *device_rom_region() const override;

	void isbc_215g_io(address_map &map);
	void isbc_215g_mem(address_map &map);

private:
	void find_sector();
	uint16_t read_sector();
	bool write_sector(uint16_t data);

	required_device<cpu_device> m_maincpu;
	required_device<i8089_device> m_dmac;
	required_device<harddisk_image_device> m_hdd0;
	required_device<harddisk_image_device> m_hdd1;
	required_device<isbx_slot_device> m_sbx1;
	required_device<isbx_slot_device> m_sbx2;

	devcb_write_line m_out_irq_func;

	int m_reset;
	uint16_t m_wakeup;
	uint16_t m_secoffset;
	uint16_t m_sector[512];
	address_space *m_maincpu_mem;
	uint32_t m_lba[2];
	uint16_t m_cyl[2];
	uint8_t m_idcompare[4];
	uint8_t m_drive;
	uint8_t m_head;
	uint8_t m_index;
	int8_t m_format_bytes;
	bool m_idfound;
	bool m_stepdir;
	bool m_wrgate;
	bool m_rdgate;
	bool m_amsrch;

	bool m_isbx_irq[4];
	bool m_fdctc;
	bool m_step;
	bool m_format;

	const struct hard_disk_info* m_geom[2];

	DECLARE_WRITE_LINE_MEMBER(isbx_irq_00_w);
	DECLARE_WRITE_LINE_MEMBER(isbx_irq_01_w);
	DECLARE_WRITE_LINE_MEMBER(isbx_irq_10_w);
	DECLARE_WRITE_LINE_MEMBER(isbx_irq_11_w);
};

DECLARE_DEVICE_TYPE(ISBC_215G, isbc_215g_device)

#endif // MAME_MACHINE_ISBC_215G_H
