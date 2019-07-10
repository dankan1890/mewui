// license:BSD-3-Clause
// copyright-holders:R. Belmont
/***************************************************************************

  Sega Model 1 sound board (68000 + 2x 315-5560 "MultiPCM")

  used for Model 1 and early Model 2 games

***************************************************************************/

#include "emu.h"
#include "audio/segam1audio.h"

#include "machine/clock.h"
#include "speaker.h"

void segam1audio_device::segam1audio_map(address_map &map)
{
	map(0x000000, 0x03ffff).rom();
	map(0x080000, 0x09ffff).rom().region("sndcpu", 0x20000); // mirror of upper ROM socket
	map(0xc20000, 0xc20003).rw(m_uart, FUNC(i8251_device::read), FUNC(i8251_device::write)).umask16(0x00ff);
	map(0xc40000, 0xc40007).rw(m_multipcm_1, FUNC(multipcm_device::read), FUNC(multipcm_device::write)).umask16(0x00ff);
	map(0xc40012, 0xc40013).nopw();
	map(0xc50000, 0xc50001).w(FUNC(segam1audio_device::m1_snd_mpcm_bnk1_w));
	map(0xc60000, 0xc60007).rw(m_multipcm_2, FUNC(multipcm_device::read), FUNC(multipcm_device::write)).umask16(0x00ff);
	map(0xc70000, 0xc70001).w(FUNC(segam1audio_device::m1_snd_mpcm_bnk2_w));
	map(0xd00000, 0xd00007).rw(m_ym, FUNC(ym3438_device::read), FUNC(ym3438_device::write)).umask16(0x00ff);
	map(0xf00000, 0xf0ffff).ram();
}

void segam1audio_device::mpcm1_map(address_map &map)
{
	map(0x000000, 0x0fffff).rom();
	map(0x100000, 0x1fffff).bankr(m_mpcmbank1);
}

void segam1audio_device::mpcm2_map(address_map &map)
{
	map(0x000000, 0x0fffff).rom();
	map(0x100000, 0x1fffff).bankr(m_mpcmbank2);
}

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(SEGAM1AUDIO, segam1audio_device, "segam1audio", "Sega Model 1 Sound Board")

//-------------------------------------------------
// device_add_mconfig - add device configuration
//-------------------------------------------------

void segam1audio_device::device_add_mconfig(machine_config &config)
{
	M68000(config, m_audiocpu, 10000000);  // verified on real h/w
	m_audiocpu->set_addrmap(AS_PROGRAM, &segam1audio_device::segam1audio_map);

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	YM3438(config, m_ym, 8000000);
	m_ym->add_route(0, "lspeaker", 0.60);
	m_ym->add_route(1, "rspeaker", 0.60);

	MULTIPCM(config, m_multipcm_1, 8000000);
	m_multipcm_1->set_addrmap(0, &segam1audio_device::mpcm1_map);
	m_multipcm_1->add_route(0, "lspeaker", 1.0);
	m_multipcm_1->add_route(1, "rspeaker", 1.0);

	MULTIPCM(config, m_multipcm_2, 8000000);
	m_multipcm_2->set_addrmap(0, &segam1audio_device::mpcm2_map);
	m_multipcm_2->add_route(0, "lspeaker", 1.0);
	m_multipcm_2->add_route(1, "rspeaker", 1.0);

	I8251(config, m_uart, 8000000); // T82C51, clock unknown
	m_uart->rxrdy_handler().set_inputline(m_audiocpu, M68K_IRQ_2);
	m_uart->txd_handler().set(FUNC(segam1audio_device::output_txd));

	clock_device &uart_clock(CLOCK(config, "uart_clock", 500000)); // 16 times 31.25MHz (standard Sega/MIDI sound data rate)
	uart_clock.signal_handler().set(m_uart, FUNC(i8251_device::write_txc));
	uart_clock.signal_handler().append(m_uart, FUNC(i8251_device::write_rxc));
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  segam1audio_device - constructor
//-------------------------------------------------

segam1audio_device::segam1audio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, SEGAM1AUDIO, tag, owner, clock),
	m_audiocpu(*this, "sndcpu"),
	m_multipcm_1(*this, "pcm1"),
	m_multipcm_2(*this, "pcm2"),
	m_ym(*this, "ymsnd"),
	m_uart(*this, "uart"),
	m_multipcm1_region(*this, "pcm1"),
	m_multipcm2_region(*this, "pcm2"),
	m_mpcmbank1(*this, "m1pcm1_bank"),
	m_mpcmbank2(*this, "m1pcm2_bank"),
	m_rxd_handler(*this)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void segam1audio_device::device_start()
{
	m_rxd_handler.resolve_safe();
	m_mpcmbank1->configure_entries(0, 4, m_multipcm1_region->base(), 0x100000);
	m_mpcmbank2->configure_entries(0, 4, m_multipcm2_region->base(), 0x100000);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void segam1audio_device::device_reset()
{
	m_uart->write_cts(0);
}

WRITE16_MEMBER(segam1audio_device::m1_snd_mpcm_bnk1_w)
{
	m_mpcmbank1->set_entry(data & 3);
}

WRITE16_MEMBER(segam1audio_device::m1_snd_mpcm_bnk2_w)
{
	m_mpcmbank2->set_entry(data & 3);
}

WRITE_LINE_MEMBER(segam1audio_device::write_txd)
{
	m_uart->write_rxd(state);
}

WRITE_LINE_MEMBER(segam1audio_device::output_txd)
{
	m_rxd_handler(state);
}
