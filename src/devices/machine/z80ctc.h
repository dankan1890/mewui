// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
/***************************************************************************

    Z80 CTC (Z8430) implementation

****************************************************************************
                            _____   _____
                    D4   1 |*    \_/     | 28  D3
                    D5   2 |             | 27  D2
                    D6   3 |             | 26  D1
                    D7   4 |             | 25  D0
                   GND   5 |             | 24  +5V
                   _RD   6 |             | 23  CLK/TRG0
                ZC/TOO   7 |   Z80-CTC   | 22  CLK/TRG1
                ZC/TO1   8 |             | 21  CLK/TRG2
                ZC/TO2   9 |             | 20  CLK/TRG3
                 _IORQ  10 |             | 19  CS1
                   IEO  11 |             | 18  CS0
                  _INT  12 |             | 17  _RESET
                   IEI  13 |             | 16  _CE
                   _M1  14 |_____________| 15  CLK

***************************************************************************/

#ifndef MAME_MACHINE_Z80CTC_H
#define MAME_MACHINE_Z80CTC_H

#pragma once

#include "machine/z80daisy.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// forward declaration
class z80ctc_device;

// ======================> z80ctc_channel_device

// a single channel within the CTC
class z80ctc_channel_device : public device_t
{
	friend class z80ctc_device;

public:
	// construction/destruction
	z80ctc_channel_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock = 0);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	u8 read();
	void write(u8 data);

	attotime period() const;
	void trigger(bool state);
	TIMER_CALLBACK_MEMBER(timer_callback);

	required_device<z80ctc_device> m_device; // pointer back to our device
	int             m_index;                // our channel index
	u16             m_mode;                 // current mode
	u16             m_tconst;               // time constant
	u16             m_down;                 // down counter (clock mode only)
	bool            m_extclk;               // current signal from the external clock
	emu_timer *     m_timer;                // array of active timers
	u8              m_int_state;            // interrupt status (for daisy chain)
};

// ======================> z80ctc_device

class z80ctc_device :   public device_t,
						public device_z80daisy_interface
{
	friend class z80ctc_channel_device;

public:
	// construction/destruction
	z80ctc_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	auto intr_callback() { return m_intr_cb.bind(); }
	template <int Channel> auto zc_callback() { return m_zc_cb[Channel].bind(); } // m_zc_cb[3] not supported on a standard ctc, only used for the tmpz84c015
	template <int Channel> void set_clk(u32 clock) { channel_config(Channel).set_clock(clock); }
	template <int Channel> void set_clk(const XTAL &xtal) { channel_config(Channel).set_clock(xtal); }

	// read/write handlers
	uint8_t read(offs_t offset);
	void write(offs_t offset, uint8_t data);
	DECLARE_WRITE_LINE_MEMBER( trg0 );
	DECLARE_WRITE_LINE_MEMBER( trg1 );
	DECLARE_WRITE_LINE_MEMBER( trg2 );
	DECLARE_WRITE_LINE_MEMBER( trg3 );

	u16 get_channel_constant(int ch) const { return m_channel[ch]->m_tconst; }

protected:
	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_resolve_objects() override;
	virtual void device_start() override;
	virtual void device_reset_after_children() override;

	// z80daisy_interface overrides
	virtual int z80daisy_irq_state() override;
	virtual int z80daisy_irq_ack() override;
	virtual void z80daisy_irq_reti() override;

private:
	// internal helpers
	void interrupt_check();

	z80ctc_channel_device &channel_config(int ch) { return *subdevice<z80ctc_channel_device>(m_channel[ch].finder_tag()); }

	// internal state
	devcb_write_line   m_intr_cb;              // interrupt callback
	devcb_write_line   m_zc_cb[4];             // zero crossing/timer output callbacks

	u8                 m_vector;               // interrupt vector

	// subdevice for each channel
	required_device_array<z80ctc_channel_device, 4> m_channel;
};


// device type definitions
DECLARE_DEVICE_TYPE(Z80CTC, z80ctc_device)
DECLARE_DEVICE_TYPE(Z80CTC_CHANNEL, z80ctc_channel_device)

#endif // MAME_MACHINE_Z80CTC_H
