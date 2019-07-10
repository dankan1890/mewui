// license:BSD-3-Clause
// copyright-holders:Samuele Zannoli
#ifndef MAME_INCLUDES_XBOX_H
#define MAME_INCLUDES_XBOX_H

#pragma once

#include "xbox_nv2a.h"
#include "xbox_usb.h"

#include "machine/idectrl.h"
#include "machine/pic8259.h"

/*
 * PIC16LC connected to SMBus
 */

class xbox_pic16lc_device : public device_t, public smbus_interface
{
public:
	xbox_pic16lc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual int execute_command(int command, int rw, int data) override;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	uint8_t buffer[0xff];
};

DECLARE_DEVICE_TYPE(XBOX_PIC16LC, xbox_pic16lc_device)

/*
 * CX25871 connected to SMBus
 */

class xbox_cx25871_device : public device_t, public smbus_interface
{
public:
	xbox_cx25871_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual int execute_command(int command, int rw, int data) override;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
};

DECLARE_DEVICE_TYPE(XBOX_CX25871, xbox_cx25871_device)

/*
 * EEPROM connected to SMBus
 */

class xbox_eeprom_device : public device_t, public smbus_interface
{
public:
	xbox_eeprom_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual int execute_command(int command, int rw, int data) override;

	std::function<void(void)> hack_eeprom;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
};

DECLARE_DEVICE_TYPE(XBOX_EEPROM, xbox_eeprom_device)

/*
 * Super-io connected to lpc bus used as a rs232 debug port
 */

class xbox_superio_device : public device_t, public lpcbus_device_interface
{
public:
	xbox_superio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual void map_extra(address_space *memory_space, address_space *io_space) override;
	virtual void set_host(int index, lpcbus_host_interface *host) override;

	DECLARE_READ8_MEMBER(read);
	DECLARE_WRITE8_MEMBER(write);
	DECLARE_READ8_MEMBER(read_rs232);
	DECLARE_WRITE8_MEMBER(write_rs232);

protected:
	virtual void device_start() override;

private:
	void internal_io_map(address_map &map);

	lpcbus_host_interface *lpchost;
	int lpcindex;
	address_space *memspace;
	address_space *iospace;
	bool configuration_mode;
	int index;
	int selected;
	uint8_t registers[16][256]; // 256 registers for up to 16 devices, registers 0-0x2f common to all
};

DECLARE_DEVICE_TYPE(XBOX_SUPERIO, xbox_superio_device)

/*
 * Base
 */

class xbox_base_state : public driver_device
{
public:
	xbox_base_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		nvidia_nv2a(nullptr),
		debug_irq_active(false),
		debug_irq_number(0),
		m_maincpu(*this, "maincpu"),
		mcpxlpc(*this, ":pci:01.0"),
		ide(*this, ":pci:09.0:ide1"),
		debugc_bios(nullptr) { }

	void xbox_base(machine_config &config);

protected:
	void debug_generate_irq(int irq, bool active);
	virtual void hack_eeprom() {};
	virtual void hack_usb() {};

	DECLARE_WRITE_LINE_MEMBER(vblank_callback);
	uint32_t screen_update_callback(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	virtual void machine_start() override;
	DECLARE_WRITE_LINE_MEMBER(maincpu_interrupt);
	DECLARE_WRITE_LINE_MEMBER(ohci_usb_interrupt_changed);
	DECLARE_WRITE_LINE_MEMBER(smbus_interrupt_changed);
	DECLARE_WRITE_LINE_MEMBER(ide_interrupt_changed);
	DECLARE_WRITE_LINE_MEMBER(nv2a_interrupt_changed);
	IRQ_CALLBACK_MEMBER(irq_callback);

	nv2a_renderer *nvidia_nv2a;
	bool debug_irq_active;
	int debug_irq_number;
	required_device<cpu_device> m_maincpu;
	required_device<mcpx_isalpc_device> mcpxlpc;
	required_device<bus_master_ide_controller_device> ide;
	static const struct debugger_constants
	{
		uint32_t id;
		uint32_t parameter[8]; // c c c ? ? ? x x
	} debugp[];
	const debugger_constants *debugc_bios;

private:
	void dump_string_command(int ref, const std::vector<std::string> &params);
	void dump_process_command(int ref, const std::vector<std::string> &params);
	void dump_list_command(int ref, const std::vector<std::string> &params);
	void dump_dpc_command(int ref, const std::vector<std::string> &params);
	void dump_timer_command(int ref, const std::vector<std::string> &params);
	void curthread_command(int ref, const std::vector<std::string> &params);
	void threadlist_command(int ref, const std::vector<std::string> &params);
	void generate_irq_command(int ref, const std::vector<std::string> &params);
	void nv2a_combiners_command(int ref, const std::vector<std::string> &params);
	void nv2a_wclipping_command(int ref, const std::vector<std::string> &params);
	void waitvblank_command(int ref, const std::vector<std::string> &params);
	void grab_texture_command(int ref, const std::vector<std::string> &params);
	void grab_vprog_command(int ref, const std::vector<std::string> &params);
	void vprogdis_command(int ref, const std::vector<std::string> &params);
	void help_command(int ref, const std::vector<std::string> &params);
	void xbox_debug_commands(int ref, const std::vector<std::string> &params);
	int find_bios_index();
	bool find_bios_hash(int bios, uint32_t &crc32);
	void find_debug_params();
};

#endif // MAME_INCLUDES_XBOX_H
