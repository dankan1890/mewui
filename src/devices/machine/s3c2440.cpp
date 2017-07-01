// license:BSD-3-Clause
// copyright-holders:Tim Schuerewegen
/*******************************************************************************

    Samsung S3C2440

    (c) 2010 Tim Schuerewegen

*******************************************************************************/

#include "emu.h"
#include "cpu/arm7/arm7.h"
#include "cpu/arm7/arm7core.h"
#include "machine/s3c2440.h"

#define VERBOSE_LEVEL ( 0 )

static inline void ATTR_PRINTF(3,4) verboselog( device_t &device, int n_level, const char *s_fmt, ...)
{
	if (VERBOSE_LEVEL >= n_level)
	{
		va_list v;
		char buf[32768];
		va_start( v, s_fmt);
		vsprintf( buf, s_fmt, v);
		va_end( v);
		device.logerror( "%s: %s", device.machine().describe_context( ), buf);
	}
}

#define DEVICE_S3C2440
#define S3C24_CLASS_NAME s3c2440_device
#include "machine/s3c24xx.hxx"
#undef DEVICE_S3C2440

uint32_t s3c2440_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	return s3c24xx_video_update( screen, bitmap, cliprect);
}

const device_type S3C2440 = &device_creator<s3c2440_device>;

s3c2440_device::s3c2440_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
		: device_t(mconfig, S3C2440, "Samsung S3C2440", tag, owner, clock, "s3c2440", __FILE__),
		m_palette(*this, finder_base::DUMMY_TAG),
		m_cpu(*this, ":maincpu"),
		m_pin_r_cb(*this),
		m_pin_w_cb(*this),
		m_port_r_cb(*this),
		m_port_w_cb(*this),
		m_scl_w_cb(*this),
		m_sda_r_cb(*this),
		m_sda_w_cb(*this),
		m_data_r_cb(*this),
		m_data_w_cb(*this),
		m_command_w_cb(*this),
		m_address_w_cb(*this),
		m_nand_data_r_cb(*this),
		m_nand_data_w_cb(*this),
		m_flags(0)
{
	memset(m_steppingstone, 0, sizeof(m_steppingstone));
	memset(&m_memcon, 0, sizeof(m_memcon));
	memset(&m_usbhost, 0, sizeof(m_usbhost));
	memset(&m_irq, 0, sizeof(m_irq));
	memset(m_dma, 0, sizeof(m_dma));
	memset(&m_clkpow, 0, sizeof(m_clkpow));
	memset(&m_lcd, 0, sizeof(m_lcd));
	memset(&m_lcdpal, 0, sizeof(m_lcdpal));
	memset(&m_nand, 0, sizeof(m_nand));
	memset(&m_cam, 0, sizeof(m_cam));
	memset(m_uart, 0, sizeof(m_uart));
	memset(&m_pwm, 0, sizeof(m_pwm));
	memset(&m_usbdev, 0, sizeof(m_usbdev));
	memset(&m_wdt, 0, sizeof(m_wdt));
	memset(&m_iic, 0, sizeof(m_iic));
	memset(&m_iis, 0, sizeof(m_iis));
	memset(&m_gpio, 0, sizeof(m_gpio));
	memset(&m_rtc, 0, sizeof(m_rtc));
	memset(&m_adc, 0, sizeof(m_adc));
	memset(m_spi, 0, sizeof(m_spi));
	memset(&m_sdi, 0, sizeof(m_sdi));
	memset(&m_ac97, 0, sizeof(m_ac97));
}

s3c2440_device::~s3c2440_device()
{
}

//-------------------------------------------------
//  static_set_palette_tag: Set the tag of the
//  palette device
//-------------------------------------------------

void s3c2440_device::static_set_palette_tag(device_t &device, const char *tag)
{
	downcast<s3c2440_device &>(device).m_palette.set_tag(tag);
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void s3c2440_device::device_start()
{
	address_space &space = m_cpu->memory().space( AS_PROGRAM);
	space.install_readwrite_handler(0x48000000, 0x4800003b, read32_delegate(FUNC(s3c2440_device::s3c24xx_memcon_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_memcon_w), this));
	space.install_readwrite_handler(0x49000000, 0x4900005b, read32_delegate(FUNC(s3c2440_device::s3c24xx_usb_host_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_usb_host_w), this));
	space.install_readwrite_handler(0x4a000000, 0x4a00001f, read32_delegate(FUNC(s3c2440_device::s3c24xx_irq_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_irq_w), this));
	space.install_readwrite_handler(0x4b000000, 0x4b000023, read32_delegate(FUNC(s3c2440_device::s3c24xx_dma_0_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_dma_0_w), this));
	space.install_readwrite_handler(0x4b000040, 0x4b000063, read32_delegate(FUNC(s3c2440_device::s3c24xx_dma_1_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_dma_1_w), this));
	space.install_readwrite_handler(0x4b000080, 0x4b0000a3, read32_delegate(FUNC(s3c2440_device::s3c24xx_dma_2_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_dma_2_w), this));
	space.install_readwrite_handler(0x4b0000c0, 0x4b0000e3, read32_delegate(FUNC(s3c2440_device::s3c24xx_dma_3_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_dma_3_w), this));
	space.install_readwrite_handler(0x4c000000, 0x4c00001b, read32_delegate(FUNC(s3c2440_device::s3c24xx_clkpow_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_clkpow_w), this));
	space.install_readwrite_handler(0x4d000000, 0x4d000063, read32_delegate(FUNC(s3c2440_device::s3c24xx_lcd_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_lcd_w), this));
	space.install_readwrite_handler(0x4d000400, 0x4d0007ff, read32_delegate(FUNC(s3c2440_device::s3c24xx_lcd_palette_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_lcd_palette_w), this));
	space.install_readwrite_handler(0x4e000000, 0x4e00003f, read32_delegate(FUNC(s3c2440_device::s3c24xx_nand_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_nand_w), this));
	space.install_readwrite_handler(0x4f000000, 0x4f0000a3, read32_delegate(FUNC(s3c2440_device::s3c24xx_cam_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_cam_w), this));
	space.install_readwrite_handler(0x50000000, 0x5000002b, read32_delegate(FUNC(s3c2440_device::s3c24xx_uart_0_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_uart_0_w), this));
	space.install_readwrite_handler(0x50004000, 0x5000402b, read32_delegate(FUNC(s3c2440_device::s3c24xx_uart_1_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_uart_1_w), this));
	space.install_readwrite_handler(0x50008000, 0x5000802b, read32_delegate(FUNC(s3c2440_device::s3c24xx_uart_2_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_uart_2_w), this));
	space.install_readwrite_handler(0x51000000, 0x51000043, read32_delegate(FUNC(s3c2440_device::s3c24xx_pwm_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_pwm_w), this));
	space.install_readwrite_handler(0x52000140, 0x5200026f, read32_delegate(FUNC(s3c2440_device::s3c24xx_usb_device_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_usb_device_w), this));
	space.install_readwrite_handler(0x53000000, 0x5300000b, read32_delegate(FUNC(s3c2440_device::s3c24xx_wdt_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_wdt_w), this));
	space.install_readwrite_handler(0x54000000, 0x54000013, read32_delegate(FUNC(s3c2440_device::s3c24xx_iic_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_iic_w), this));
	space.install_readwrite_handler(0x55000000, 0x55000013, read32_delegate(FUNC(s3c2440_device::s3c24xx_iis_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_iis_w), this));
	space.install_readwrite_handler(0x56000000, 0x560000df, read32_delegate(FUNC(s3c2440_device::s3c24xx_gpio_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_gpio_w), this));
	space.install_readwrite_handler(0x57000040, 0x5700008b, read32_delegate(FUNC(s3c2440_device::s3c24xx_rtc_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_rtc_w), this));
	space.install_readwrite_handler(0x58000000, 0x58000017, read32_delegate(FUNC(s3c2440_device::s3c24xx_adc_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_adc_w), this));
	space.install_readwrite_handler(0x59000000, 0x59000017, read32_delegate(FUNC(s3c2440_device::s3c24xx_spi_0_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_spi_0_w), this));
	space.install_readwrite_handler(0x59000020, 0x59000037, read32_delegate(FUNC(s3c2440_device::s3c24xx_spi_1_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_spi_1_w), this));
	space.install_readwrite_handler(0x5a000000, 0x5a000043, read32_delegate(FUNC(s3c2440_device::s3c24xx_sdi_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_sdi_w), this));
	space.install_readwrite_handler(0x5b000000, 0x5b00001f, read32_delegate(FUNC(s3c2440_device::s3c24xx_ac97_r), this), write32_delegate(FUNC(s3c2440_device::s3c24xx_ac97_w), this));

	s3c24xx_device_start();

	s3c24xx_video_start();
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void s3c2440_device::device_reset()
{
	s3c24xx_device_reset();
}


void s3c2440_device::s3c2440_uart_fifo_w(int uart, uint8_t data)
{
	s3c24xx_uart_fifo_w( uart, data);
}

void s3c2440_device::s3c2440_touch_screen(int state)
{
	s3c24xx_touch_screen( state);
}

void s3c2440_device::s3c2440_request_irq(uint32_t int_type)
{
	s3c24xx_request_irq( int_type);
}

void s3c2440_device::s3c2440_request_eint(uint32_t number)
{
	s3c24xx_request_eint( number);
}

WRITE_LINE_MEMBER( s3c2440_device::frnb_w )
{
	s3c24xx_pin_frnb_w(state);
}
