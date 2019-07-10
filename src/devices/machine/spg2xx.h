// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/*****************************************************************************

    SunPlus SPG2xx-series SoC peripheral emulation

    TODO:
        - Serial UART
        - I2C
        - SPI

    Known SunPlus SPG2xx/u'nSP-based systems:

         D - SPG240 - Radica Skateboarder (Sunplus QL8041C die)
        ND - SPG243 - Some form of Leapfrog "edutainment" system
        ND - SPG243 - Star Wars: Clone Wars
        ND - SPG243 - Toy Story
        ND - SPG243 - Animal Art Studio
        ND - SPG243 - Finding Nemo
         D - SPG243 - The Batman
         D - SPG243 - Wall-E
         D - SPG243 - KenSingTon / Siatronics / Jungle Soft Vii
 Partial D - SPG200 - VTech V.Smile
        ND - unknown - Zone 40
         D - SPG243 - Zone 60
         D - SPG243 - Wireless 60
        ND - unknown - Wireless Air 60
        ND - Likely many more

**********************************************************************/

#ifndef MAME_MACHINE_SPG2XX_H
#define MAME_MACHINE_SPG2XX_H

#pragma once

#include "cpu/unsp/unsp.h"
#include "spg2xx_audio.h"
#include "spg2xx_io.h"
#include "spg2xx_sysdma.h"
#include "spg2xx_video.h"
#include "screen.h"

class spg2xx_device : public unsp_device, public device_mixer_interface
{
public:
	void set_pal(bool pal) { m_pal_flag = pal ? 1 : 0; }
	void set_rowscroll_offset(int offset) { m_rowscrolloffset = offset; }

	auto porta_out() { return m_porta_out.bind(); }
	auto portb_out() { return m_portb_out.bind(); }
	auto portc_out() { return m_portc_out.bind(); }
	auto porta_in() { return m_porta_in.bind(); }
	auto portb_in() { return m_portb_in.bind(); }
	auto portc_in() { return m_portc_in.bind(); }

	template <size_t Line> auto adc_in() { return m_adc_in[Line].bind(); }

	auto eeprom_w() { return m_eeprom_w.bind(); }
	auto eeprom_r() { return m_eeprom_r.bind(); }

	auto uart_tx() { return m_uart_tx.bind(); }

	auto chip_select() { return m_chip_sel.bind(); }

	required_device<spg2xx_audio_device> m_spg_audio;
	required_device<spg2xx_io_device> m_spg_io;
	required_device<spg2xx_sysdma_device> m_spg_sysdma;
	required_device<spg2xx_video_device> m_spg_video;

	void extint_w(int channel, bool state) { m_spg_io->extint_w(channel, state); }
	void uart_rx(uint8_t data) { m_spg_io->uart_rx(data); }

	DECLARE_WRITE_LINE_MEMBER(vblank) { m_spg_video->vblank(state); }
	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect) { return m_spg_video->screen_update(screen, bitmap, cliprect); }

protected:
	spg2xx_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, uint16_t sprite_limit, address_map_constructor internal);

	void internal_map(address_map &map);

	DECLARE_WRITE_LINE_MEMBER(videoirq_w);
	DECLARE_WRITE_LINE_MEMBER(audioirq_w);
	DECLARE_WRITE_LINE_MEMBER(timerirq_w);
	DECLARE_WRITE_LINE_MEMBER(uartirq_w);
	DECLARE_WRITE_LINE_MEMBER(extirq_w);
	DECLARE_WRITE_LINE_MEMBER(ffreq1_w);
	DECLARE_WRITE_LINE_MEMBER(ffreq2_w);

	DECLARE_READ16_MEMBER(space_r);

	virtual void device_start() override;
	virtual void device_reset() override;

	// TODO: these are fixed values, put them in relevant devices?
	uint16_t m_sprite_limit;
	uint16_t m_rowscrolloffset; // auto racing in 'zone60' minigames needs this to be 15, the JAKKS games (Star Wars Revenge of the sith - Gunship Battle, Wheel of Fortune, Namco Ms. Pac-Man 5-in-1 Pole Position) need it to be 0, where does it come from?
	uint16_t m_pal_flag;
	DECLARE_READ16_MEMBER(get_sprlimit) { return m_sprite_limit; }
	DECLARE_READ16_MEMBER(get_rowscrolloffset) { return m_rowscrolloffset; }
	DECLARE_READ16_MEMBER(get_pal_ntsc) { return m_pal_flag; }

	devcb_write16 m_porta_out;
	devcb_write16 m_portb_out;
	devcb_write16 m_portc_out;
	devcb_read16 m_porta_in;
	devcb_read16 m_portb_in;
	devcb_read16 m_portc_in;

	devcb_read16 m_adc_in[2];

	devcb_write8 m_eeprom_w;
	devcb_read8 m_eeprom_r;

	devcb_write8 m_uart_tx;

	devcb_write8 m_chip_sel;

	emu_timer *m_screenpos_timer;

	required_device<screen_device> m_screen;

	void configure_spg_io(spg2xx_io_device* io);

	DECLARE_READ16_MEMBER(porta_r) { return m_porta_in(); }
	DECLARE_READ16_MEMBER(portb_r) { return m_portb_in(); }
	DECLARE_READ16_MEMBER(portc_r) { return m_portc_in(); }
	DECLARE_WRITE16_MEMBER(porta_w) { m_porta_out(offset, data, mem_mask); }
	DECLARE_WRITE16_MEMBER(portb_w) { m_portb_out(offset, data, mem_mask); }
	DECLARE_WRITE16_MEMBER(portc_w) { m_portc_out(offset, data, mem_mask); }
	template <size_t Line> DECLARE_READ16_MEMBER(adc_r) { return m_adc_in[Line](); }

	DECLARE_WRITE8_MEMBER(eepromx_w) { m_eeprom_w(offset, data, mem_mask); }
	DECLARE_READ8_MEMBER(eepromx_r) { return m_eeprom_r(); };

	DECLARE_WRITE8_MEMBER(tx_w) { m_uart_tx(offset, data, mem_mask); }
	DECLARE_WRITE8_MEMBER(cs_w) { m_chip_sel(offset, data, mem_mask); }
};

class spg24x_device : public spg2xx_device
{
public:
	template <typename T>
	spg24x_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&screen_tag)
		: spg24x_device(mconfig, tag, owner, clock)
	{
		m_screen.set_tag(std::forward<T>(screen_tag));
	}

	spg24x_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void device_add_mconfig(machine_config &config) override;

};

class spg28x_device : public spg2xx_device
{
public:
	template <typename T>
	spg28x_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&screen_tag)
		: spg28x_device(mconfig, tag, owner, clock)
	{
		m_screen.set_tag(std::forward<T>(screen_tag));
	}

	spg28x_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void device_add_mconfig(machine_config &config) override;

};

DECLARE_DEVICE_TYPE(SPG24X, spg24x_device)
DECLARE_DEVICE_TYPE(SPG28X, spg28x_device)

#endif // MAME_MACHINE_SPG2XX_H
