// license:BSD-3-Clause
// copyright-holders:smf
#ifndef MAME_MACHINE_K573DIO_H
#define MAME_MACHINE_K573DIO_H

#pragma once

#include "machine/k573fpga.h"
#include "machine/ds2401.h"

class k573dio_device : public device_t
{
public:
	k573dio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto output_callback() { return output_cb.bind(); }

	void amap(address_map &map);
	void set_ddrsbm_fpga(bool flag) { is_ddrsbm_fpga = flag; }

	DECLARE_READ16_MEMBER(a00_r);
	DECLARE_READ16_MEMBER(a02_r);
	DECLARE_READ16_MEMBER(a04_r);
	DECLARE_READ16_MEMBER(a06_r);
	DECLARE_READ16_MEMBER(a0a_r);
	DECLARE_WRITE16_MEMBER(a10_w);
	DECLARE_READ16_MEMBER(a80_r);
	DECLARE_READ16_MEMBER(ac4_r);

	DECLARE_WRITE16_MEMBER(mpeg_start_adr_high_w);
	DECLARE_WRITE16_MEMBER(mpeg_start_adr_low_w);
	DECLARE_WRITE16_MEMBER(mpeg_end_adr_high_w);
	DECLARE_WRITE16_MEMBER(mpeg_end_adr_low_w);
	DECLARE_READ16_MEMBER(mpeg_key_1_r);
	DECLARE_WRITE16_MEMBER(mpeg_key_1_w);
	DECLARE_READ16_MEMBER(mas_i2c_r);
	DECLARE_WRITE16_MEMBER(mas_i2c_w);
	DECLARE_READ16_MEMBER(mpeg_ctrl_r);
	DECLARE_WRITE16_MEMBER(mpeg_ctrl_w);
	DECLARE_WRITE16_MEMBER(ram_write_adr_high_w);
	DECLARE_WRITE16_MEMBER(ram_write_adr_low_w);
	DECLARE_READ16_MEMBER(ram_r);
	DECLARE_WRITE16_MEMBER(ram_w);
	DECLARE_WRITE16_MEMBER(ram_read_adr_high_w);
	DECLARE_WRITE16_MEMBER(ram_read_adr_low_w);
	DECLARE_READ16_MEMBER(mp3_frame_count_high_r);
	DECLARE_READ16_MEMBER(mp3_frame_count_low_r);
	DECLARE_WRITE16_MEMBER(output_0_w);
	DECLARE_WRITE16_MEMBER(output_1_w);
	DECLARE_WRITE16_MEMBER(output_7_w);
	DECLARE_WRITE16_MEMBER(output_3_w);
	DECLARE_WRITE16_MEMBER(mpeg_key_2_w);
	DECLARE_WRITE16_MEMBER(mpeg_key_3_w);
	DECLARE_READ16_MEMBER(digital_id_r);
	DECLARE_WRITE16_MEMBER(digital_id_w);
	DECLARE_READ16_MEMBER(fpga_status_r);
	DECLARE_WRITE16_MEMBER(fpga_firmware_w);
	DECLARE_WRITE16_MEMBER(output_4_w);
	DECLARE_WRITE16_MEMBER(output_2_w);
	DECLARE_WRITE16_MEMBER(output_5_w);
	DECLARE_READ16_MEMBER(mp3_unk_r);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	required_device<k573fpga_device> k573fpga;
	required_device<ds2401_device> digital_id;
	required_device<mas3507d_device> mas3507d;
	devcb_write8 output_cb;

	std::unique_ptr<uint16_t[]> ram;
	uint32_t ram_adr, ram_read_adr;
	uint8_t output_data[8];

	void output(int offset, uint16_t data);

	bool is_ddrsbm_fpga;
	u16 crypto_key1;
};

DECLARE_DEVICE_TYPE(KONAMI_573_DIGITAL_IO_BOARD, k573dio_device)

#endif // MAME_MACHINE_K573DIO_H
