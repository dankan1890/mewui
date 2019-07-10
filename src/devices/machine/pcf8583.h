// license:BSD-3-Clause
// copyright-holders:Tim Schuerewegen, Ryan Holtz
/*********************************************************************

    Philips PCF8583 Clock and Calendar with 240 x 8-bit RAM

**********************************************************************

    The PCF8583 comes in three package configurations:

        PCF8583P  - 8-pin dual-inline package (DIP8)
        PCF8583T  - 8-pin small-outline package (SO8)
        PCF8583BS - 20-pin thin quad-flat package (HVQFN20)

**********************************************************************

    DIP8 pinning:           ____  ____
                           |    \/    |
                  OSCI   1 |          | 8   Vdd
                  OSCI   2 |          | 7   /INT
                    A0   3 | PCF8583P | 6   SCL
                   Vss   4 |          | 5   SDA
                           |__________|

**********************************************************************

    SO8 pinning:            _____  _____
                           | |   \/     |
                  OSCI   1 | |          | 8   Vdd
                  OSCI   2 | |          | 7   /INT
                    A0   3 | | PCF8583P | 6   SCL
                   Vss   4 | |          | 5   SDA
                           |_|__________|

**********************************************************************

    HVQFN20 pinning:
                          NC NC NC NC NC
                          20 19 18 17 16
                        __________________
                       |                  |
                       |    __________    |
                       |   |          |   |
                NC   1 |   |          |   | 15  Vdd
              OSCI   2 |   |          |   | 14  /INT
              OSCO   3 |   | PCF8583BS|   | 13  SCL
                A0   4 |   |          |   | 12  SDA
               Vss   5 |   |          |   | 11  NC
                       |   |__________|   |
                       |                  |
                       |__________________|
                           6  7  8  9 10
                          NC NC NC NC NC

*********************************************************************/

#ifndef MAME_MACHINE_PCF8583_H
#define MAME_MACHINE_PCF8583_H

#pragma once

#include "dirtc.h"


class pcf8583_device :  public device_t,
						public device_rtc_interface,
						public device_nvram_interface
{
public:
	pcf8583_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 32'768);

	auto irq() { return m_irq_callback.bind(); }

	void set_a0(uint8_t a0);

	DECLARE_WRITE_LINE_MEMBER(scl_w);
	DECLARE_WRITE_LINE_MEMBER(sda_w);
	DECLARE_READ_LINE_MEMBER(sda_r);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// device_rtc_interface overrides
	virtual bool rtc_feature_y2k() const override { return true; }
	virtual void rtc_clock_updated(int year, int month, int day, int day_of_week, int hour, int minute, int second) override;

	// device_nvram_interface overrides
	virtual void nvram_default() override;
	virtual void nvram_read(emu_file &file) override;
	virtual void nvram_write(emu_file &file) override;

private:
	enum
	{
		REG_CONTROL             = 0x00,
		REG_HUNDREDTHS          = 0x01,
		REG_SECONDS             = 0x02,
		REG_MINUTES             = 0x03,
		REG_HOURS               = 0x04,
		REG_YEAR_DATE           = 0x05,
		REG_MONTH_DAY           = 0x06,
		REG_TIMER               = 0x07,
		REG_ALARM_CONTROL       = 0x08,
		REG_ALARM_HUNDREDTHS    = 0x09,
		REG_ALARM_SECONDS       = 0x0a,
		REG_ALARM_MINUTES       = 0x0b,
		REG_ALARM_HOURS         = 0x0c,
		REG_ALARM_DATE          = 0x0d,
		REG_ALARM_MONTH         = 0x0e,
		REG_ALARM_TIMER         = 0x0f
	};

	enum
	{
		CONTROL_STOP_BIT    = 0x80
	};

	static const device_timer_id TIMER_TICK = 0;

	// get/set date
	uint8_t get_date_year()             { return (m_data[REG_YEAR_DATE] >> 6) & 3; }
	void set_date_year(uint8_t year)    { m_data[REG_YEAR_DATE] = (m_data[REG_YEAR_DATE] & 0x3f) | ((year % 4) << 6); }
	uint8_t get_date_month()            { return bcd_to_integer(m_data[6] & 0x1f); }
	void set_date_month(uint8_t month)  { m_data[REG_MONTH_DAY] = (m_data[REG_MONTH_DAY] & 0xe0) | (convert_to_bcd(month) & 0x1f); }
	uint8_t get_date_day()              { return bcd_to_integer(m_data[REG_YEAR_DATE] & 0x3f); }
	void set_date_day(uint8_t day)      { m_data[REG_YEAR_DATE] = (m_data[REG_YEAR_DATE] & 0xc0) | (convert_to_bcd(day) & 0x3f); }

	// get/set time
	uint8_t get_time_hour()             { return bcd_to_integer(m_data[REG_HOURS]); }
	void set_time_hour(uint8_t hour)    { m_data[REG_HOURS] = convert_to_bcd(hour); }
	uint8_t get_time_minute()           { return bcd_to_integer(m_data[REG_MINUTES]); }
	void set_time_minute(uint8_t minute){ m_data[REG_MINUTES] = convert_to_bcd(minute); }
	uint8_t get_time_second()           { return bcd_to_integer(m_data[REG_SECONDS]); }
	void set_time_second(uint8_t second){ m_data[REG_SECONDS] = convert_to_bcd(second); }

	void write_register(uint8_t offset, uint8_t data);
	void advance_hundredths();
	void clear_rx_buffer();

	devcb_write_line m_irq_callback;

	// internal state
	uint8_t     m_data[256];
	int         m_scl;
	int         m_sda;
	int         m_inp;
	bool        m_transfer_active;
	int         m_bit_index;
	bool        m_irq;
	uint8_t     m_data_recv_index;
	uint8_t     m_data_recv;
	uint8_t     m_mode;
	uint8_t     m_pos;
	uint8_t     m_write_address;
	uint8_t     m_read_address;
	emu_timer * m_timer;
	enum        { RTC_MODE_NONE, RTC_MODE_SEND, RTC_MODE_RECV };

	static constexpr uint8_t WRITE_ADDRESS_BASE = 0xa0;
	static constexpr uint8_t READ_ADDRESS_BASE = 0xa1;
};

// device type definition
DECLARE_DEVICE_TYPE(PCF8583, pcf8583_device)

#endif // MAME_MACHINE_PCF8583_H
