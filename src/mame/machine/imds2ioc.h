// license:BSD-3-Clause
// copyright-holders:F. Ulivi

#ifndef MAME_MACHINE_IMDS2IOC_H
#define MAME_MACHINE_IMDS2IOC_H

#include "cpu/i8085/i8085.h"
#include "cpu/mcs48/mcs48.h"
#include "machine/i8257.h"
#include "video/i8275.h"
#include "sound/beep.h"
#include "machine/pit8253.h"
#include "machine/i8271.h"
#include "imagedev/floppy.h"
#include "bus/centronics/ctronics.h"
#include "emupal.h"

class imds2ioc_device : public device_t
{
public:
	imds2ioc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	auto master_intr_cb() { return m_master_intr_cb.bind(); }
	auto parallel_int_cb() { return m_parallel_int_cb.bind(); }

	DECLARE_READ8_MEMBER(dbb_master_r);
	DECLARE_WRITE8_MEMBER(dbb_master_w);

	DECLARE_READ8_MEMBER(pio_master_r);
	DECLARE_WRITE8_MEMBER(pio_master_w);

protected:
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	virtual void device_resolve_objects() override;
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	DECLARE_WRITE8_MEMBER(miscout_w);
	DECLARE_READ8_MEMBER(miscin_r);
	DECLARE_WRITE_LINE_MEMBER(beep_timer_w);
	DECLARE_WRITE8_MEMBER(start_timer_w);

	DECLARE_READ8_MEMBER(kb_read);
	DECLARE_READ8_MEMBER(kb_port_p2_r);
	DECLARE_WRITE8_MEMBER(kb_port_p1_w);
	DECLARE_READ_LINE_MEMBER(kb_port_t0_r);
	DECLARE_READ_LINE_MEMBER(kb_port_t1_r);

	DECLARE_WRITE8_MEMBER(ioc_dbbout_w);
	DECLARE_WRITE8_MEMBER(ioc_f0_w);
	DECLARE_WRITE8_MEMBER(ioc_set_f1_w);
	DECLARE_WRITE8_MEMBER(ioc_reset_f1_w);
	DECLARE_READ8_MEMBER(ioc_status_r);
	DECLARE_READ8_MEMBER(ioc_dbbin_r);

	DECLARE_WRITE_LINE_MEMBER(hrq_w);
	DECLARE_READ8_MEMBER(ioc_mem_r);
	DECLARE_WRITE8_MEMBER(ioc_mem_w);

	DECLARE_READ8_MEMBER(pio_port_p1_r);
	DECLARE_WRITE8_MEMBER(pio_port_p1_w);
	DECLARE_READ8_MEMBER(pio_port_p2_r);
	DECLARE_WRITE8_MEMBER(pio_port_p2_w);
	DECLARE_WRITE_LINE_MEMBER(pio_lpt_ack_w);
	DECLARE_WRITE_LINE_MEMBER(pio_lpt_busy_w);
	DECLARE_WRITE_LINE_MEMBER(pio_lpt_select_w);

	I8275_DRAW_CHARACTER_MEMBER(crtc_display_pixels);

	void io_map(address_map &map);
	void mem_map(address_map &map);

	void update_beeper();
	void update_printer();

	required_device<i8080a_cpu_device> m_ioccpu;
	required_device<i8257_device> m_iocdma;
	required_device<i8275_device> m_ioccrtc;
	required_device<beep_device> m_iocbeep;
	required_device<pit8253_device> m_ioctimer;
	required_device<i8271_device> m_iocfdc;
	required_device<floppy_connector> m_flop0;
	required_device<i8041a_device> m_iocpio;
	required_device<i8741a_device> m_kbcpu;
	required_device<palette_device> m_palette;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<centronics_device> m_centronics;
	required_ioport_array<8> m_io_key;
	required_ioport m_ioc_options;

	devcb_write_line m_master_intr_cb;
	devcb_write_line m_parallel_int_cb;

	// Character generator
	required_region_ptr<uint8_t> m_chargen;

	// MISCOUT state
	uint8_t m_miscout;

	// Beeper timer line
	int m_beeper_timer;

	// Keyboard state
	uint8_t m_kb_p1;

	// IPC to IOC buffer
	uint8_t m_ioc_ibf;

	// IOC to IPC buffer
	uint8_t m_ioc_obf;

	// IPC/IOC status
	uint8_t m_ipc_ioc_status;

	// PIO port 1
	uint8_t m_pio_port1;

	// PIO port 2
	uint8_t m_pio_port2;

	// PIO device status byte
	uint8_t m_device_status_byte;
};

DECLARE_DEVICE_TYPE(IMDS2IOC, imds2ioc_device)

#endif // MAME_MACHINE_IMDS2IOC_H
