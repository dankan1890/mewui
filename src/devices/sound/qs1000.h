// license:BSD-3-Clause
// copyright-holders:Philip Bennett
/***************************************************************************

    qs1000.h

    QS1000 device emulator.

***************************************************************************/

#ifndef MAME_SOUND_QS1000_H
#define MAME_SOUND_QS1000_H

#pragma once

#include "cpu/mcs51/mcs51.h"
#include "sound/okiadpcm.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> qs1000_device

class qs1000_device :   public device_t,
						public device_sound_interface,
						public device_rom_interface
{
public:
	static constexpr feature_type imperfect_features() { return feature::SOUND; }

	// construction/destruction
	qs1000_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_external_rom(bool external_rom) { m_external_rom = external_rom; }
	auto p1_in() { return m_in_p1_cb.bind(); }
	auto p2_in() { return m_in_p2_cb.bind(); }
	auto p3_in() { return m_in_p3_cb.bind(); }
	auto p1_out() { return m_out_p1_cb.bind(); }
	auto p2_out() { return m_out_p2_cb.bind(); }
	auto p3_out() { return m_out_p3_cb.bind(); }
	//auto serial_w() { return m_serial_w_cb.bind(); }

	// external
	i8052_device &cpu() const { return *m_cpu; }
	void serial_in(uint8_t data);
	DECLARE_WRITE_LINE_MEMBER( set_irq );

	DECLARE_WRITE8_MEMBER( wave_w );

	DECLARE_READ8_MEMBER( p0_r );
	DECLARE_WRITE8_MEMBER( p0_w );

	DECLARE_READ8_MEMBER( p1_r );
	DECLARE_WRITE8_MEMBER( p1_w );

	DECLARE_READ8_MEMBER( p2_r );
	DECLARE_WRITE8_MEMBER( p2_w );

	DECLARE_READ8_MEMBER( p3_r );
	DECLARE_WRITE8_MEMBER( p3_w );

	void qs1000_io_map(address_map &map);
	void qs1000_prg_map(address_map &map);
protected:
	// device-level overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// device_sound_interface overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples) override;

	// device_rom_interface overrides
	virtual void rom_bank_updated() override;

private:
	static constexpr unsigned QS1000_CHANNELS       = 32;
	static constexpr offs_t   QS1000_ADDRESS_MASK   = 0x00ffffff;

	enum
	{
		QS1000_KEYON   = 1,
		QS1000_PLAYING = 2,
		QS1000_ADPCM   = 4
	};

	void start_voice(int ch);

	bool                    m_external_rom;

	// Callbacks
	devcb_read8             m_in_p1_cb;
	devcb_read8             m_in_p2_cb;
	devcb_read8             m_in_p3_cb;

	devcb_write8            m_out_p1_cb;
	devcb_write8            m_out_p2_cb;
	devcb_write8            m_out_p3_cb;

	//devcb_write8            m_serial_w_cb;

	// Internal state
	sound_stream *                  m_stream;
	required_device<i8052_device>   m_cpu;

	// Wavetable engine
	uint8_t                           m_serial_data_in;
	uint8_t                           m_wave_regs[18];

	struct qs1000_channel
	{
		uint32_t          m_acc;
		int32_t           m_adpcm_signal;
		uint32_t          m_start;
		uint32_t          m_addr;
		uint32_t          m_adpcm_addr;
		uint32_t          m_loop_start;
		uint32_t          m_loop_end;
		uint16_t          m_freq;
		uint16_t          m_flags;

		uint8_t           m_regs[16]; // FIXME

		oki_adpcm_state m_adpcm;
	};

	qs1000_channel                  m_channels[QS1000_CHANNELS];

	DECLARE_READ8_MEMBER( data_to_i8052 );
};


// device type definition
DECLARE_DEVICE_TYPE(QS1000, qs1000_device)

#endif // MAME_SOUND_QS1000_H
