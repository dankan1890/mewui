// license:LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    TI-99 Serial and parallel interface card
    See ti_rs232.c for documentation

    Michael Zapf
    February 2012: Rewritten as class

*****************************************************************************/

#ifndef MAME_BUS_TI99_PEB_TI_RS232_H
#define MAME_BUS_TI99_PEB_TI_RS232_H

#pragma once

#include "peribox.h"
#include "machine/74259.h"
#include "machine/tms9902.h"

namespace bus { namespace ti99 { namespace peb {

class ti_pio_attached_device;
class ti_rs232_attached_device;

class ti_rs232_pio_device : public device_t, public device_ti99_peribox_card_interface
{
	friend class ti_pio_attached_device;
	friend class ti_rs232_attached_device;

public:
	ti_rs232_pio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	DECLARE_READ8Z_MEMBER(readz) override;
	void write(offs_t offset, uint8_t data) override;

	DECLARE_READ8Z_MEMBER(crureadz) override;
	void cruwrite(offs_t offset, uint8_t data) override;

protected:
	void device_start() override;
	void device_reset() override;
	void device_stop() override;
	const tiny_rom_entry *device_rom_region() const override;
	void device_add_mconfig(machine_config &config) override;
	ioport_constructor device_input_ports() const override;

private:
	DECLARE_WRITE_LINE_MEMBER(int0_callback);
	DECLARE_WRITE_LINE_MEMBER(int1_callback);
	DECLARE_WRITE_LINE_MEMBER(rcv0_callback);
	DECLARE_WRITE_LINE_MEMBER(rcv1_callback);
	void xmit0_callback(uint8_t data);
	void xmit1_callback(uint8_t data);
	void ctrl0_callback(offs_t offset, uint8_t data);
	void ctrl1_callback(offs_t offset, uint8_t data);

	DECLARE_WRITE_LINE_MEMBER(selected_w);
	DECLARE_WRITE_LINE_MEMBER(pio_direction_in_w);
	DECLARE_WRITE_LINE_MEMBER(pio_handshake_out_w);
	DECLARE_WRITE_LINE_MEMBER(pio_spareout_w);
	DECLARE_WRITE_LINE_MEMBER(flag0_w);
	DECLARE_WRITE_LINE_MEMBER(cts0_w);
	DECLARE_WRITE_LINE_MEMBER(cts1_w);
	DECLARE_WRITE_LINE_MEMBER(led_w);

	void        incoming_dtr(int uartind, line_state value);
	void        transmit_data(int uartind, uint8_t value);
	uint8_t       map_lines_out(int uartind, uint8_t value);
	uint8_t       map_lines_in(int uartind, uint8_t value);
	void        receive_data_or_line_state(int uartind);
	void        set_bit(int uartind, int line, int value);

	void        configure_interface(int uartind, int type, int value);
	void        output_line_state(int uartind, int mask, uint8_t value);
	void        output_exception(int uartind, int param, uint8_t value);
	void        ctrl_callback(int uartind, int type, uint8_t data);

	required_device<ls259_device> m_crulatch;
	// Asynchronous receivers/transmitters
	required_device<tms9902_device> m_uart0;
	required_device<tms9902_device> m_uart1;

	// Connected images (file or socket connection) that represent the
	// devices that are connected to the serial adapters
	required_device<ti_rs232_attached_device> m_serdev0;
	required_device<ti_rs232_attached_device> m_serdev1;

	// Connected image (file) that represents the device connected to the
	// parallel interface
	required_device<ti_pio_attached_device> m_piodev;
	uint8_t*                      m_dsrrom;

	// Input buffer for each UART. We have to copy the contents of sdlsocket here
	// because the buffer in corefile will be lost on the next write operation
	std::unique_ptr<uint8_t[]>      m_recvbuf[2];
	int         m_bufpos[2], m_buflen[2];

	// Latches the state of the output lines for UART0/UART1
	uint8_t   m_signals[2];
	int     m_recv_mode[2];     // May be NORMAL or ESC

	// Baud rate management
	// not part of the real device, but required for the connection to the
	// real UART
	double  m_time_hold[2];

	// PIO flags
	bool    m_pio_direction_in;     // a.k.a. PIOOC pio in output mode if 0
	bool    m_pio_handshakeout;
	bool    m_pio_handshakein;
	bool    m_pio_spareout;
	bool    m_pio_sparein;
	bool    m_flag0;                // spare
	bool    m_led;              // a.k.a. flag3
	int     m_pio_out_buffer;
	int     m_pio_in_buffer;
	bool    m_pio_readable;
	bool    m_pio_writable;
	bool    m_pio_write;            // true if image is to be written to

	/* Keeps the value put on the bus when SENILA becomes active. */
	uint8_t   m_ila;
};

/****************************************************************************/

/*
    Defines the serial serdev. "TI99 RS232 attached serial device"
*/
class ti_rs232_attached_device : public device_t, public device_image_interface
{
public:
	ti_rs232_attached_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	iodevice_t image_type() const override { return IO_SERIAL; }
	bool is_readable()  const override           { return true; }
	bool is_writeable() const override           { return true; }
	bool is_creatable() const override           { return true; }
	bool must_be_loaded() const override         { return false; }
	bool is_reset_on_load() const override       { return false; }
	const char *image_interface() const override { return ""; }
	const char *file_extensions() const override { return ""; }
	void connect(tms9902_device *dev) { m_uart = dev; }

protected:
	void device_start() override { };
	image_init_result    call_load() override;
	void    call_unload() override;

private:
	int get_index_from_tagname();
	tms9902_device* m_uart;
};

/*
    Defines the PIO (parallel IO) "TI99 PIO attached device"
*/
class ti_pio_attached_device : public device_t, public device_image_interface
{
public:
	ti_pio_attached_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	iodevice_t image_type() const override { return IO_PARALLEL; }
	bool is_readable()  const override           { return true; }
	bool is_writeable() const override           { return true; }
	bool is_creatable() const override           { return true; }
	bool must_be_loaded() const override         { return false; }
	bool is_reset_on_load() const override       { return false; }
	const char *image_interface() const override { return ""; }
	const char *file_extensions() const override { return ""; }

protected:
	void    device_start() override { };
	image_init_result    call_load() override;
	void    call_unload() override;
};

} } } // end namespace bus::ti99::peb

DECLARE_DEVICE_TYPE_NS(TI99_RS232,     bus::ti99::peb, ti_rs232_pio_device)
DECLARE_DEVICE_TYPE_NS(TI99_RS232_DEV, bus::ti99::peb, ti_rs232_attached_device)
DECLARE_DEVICE_TYPE_NS(TI99_PIO_DEV,   bus::ti99::peb, ti_pio_attached_device)

#endif // MAME_BUS_TI99_PEB_TI_RS232_H
