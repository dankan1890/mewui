// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay

#ifndef MAME_MACHINE_JAZZ_MCT_ADR_H
#define MAME_MACHINE_JAZZ_MCT_ADR_H

#pragma once

class jazz_mct_adr_device : public device_t
{
public:
	jazz_mct_adr_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration
	template <typename T> void set_bus(T &&tag, int spacenum) { m_bus.set_tag(std::forward<T>(tag), spacenum); }

	auto out_int_dma_cb() { return m_out_int_dma.bind(); }
	auto out_int_device_cb() { return m_out_int_device.bind(); }
	auto out_int_timer_cb() { return m_out_int_timer.bind(); }
	auto eisa_iack_cb() { return m_eisa_iack.bind(); }

	template <unsigned DMA> auto dma_r_cb() { return m_dma_r[DMA].bind(); }
	template <unsigned DMA> auto dma_w_cb() { return m_dma_w[DMA].bind(); }

	template <unsigned IRQ> DECLARE_WRITE_LINE_MEMBER(irq) { set_irq_line(IRQ, state); }
	template <unsigned DRQ> DECLARE_WRITE_LINE_MEMBER(drq) { set_drq_line(DRQ, state); }

	virtual void map(address_map &map);

	u16 isr_r();
	u16 imr_r() { return m_imr; }
	void imr_w(u16 data);

protected:
	// device_t overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	void set_irq_line(int number, int state);
	void set_drq_line(int channel, int state);

	TIMER_CALLBACK_MEMBER(irq_check);
	TIMER_CALLBACK_MEMBER(dma_check);
	TIMER_CALLBACK_MEMBER(interval_timer);

	u32 translate_address(u32 logical_address);

	required_address_space m_bus;

	devcb_write_line m_out_int_dma;
	devcb_write_line m_out_int_device;
	devcb_write_line m_out_int_timer;
	devcb_read32 m_eisa_iack;

	devcb_read8 m_dma_r[4];
	devcb_write8 m_dma_w[4];
	bool m_drq_active[4];

	emu_timer *m_irq_check;
	emu_timer *m_dma_check;
	emu_timer *m_interval_timer;

	enum isr_mask : u16
	{
		ISR_PRINTER  = 0x0001,
		ISR_FLOPPY   = 0x0002,
		ISR_SOUND    = 0x0004,
		ISR_VIDEO    = 0x0008,
		ISR_NETWORK  = 0x0010,
		ISR_SCSI     = 0x0020,
		ISR_KEYBOARD = 0x0040,
		ISR_MOUSE    = 0x0080,
		ISR_SERIAL0  = 0x0100,
		ISR_SERIAL1  = 0x0200,
	};
	u16 m_isr; // local bus interrupt source
	u16 m_imr; // local bus interrupt mask

	u32 m_config;

	enum dma_register : unsigned
	{
		REG_MODE    = 0,
		REG_ENABLE  = 1,
		REG_COUNT   = 2,
		REG_ADDRESS = 3,
	};
	enum dma_mode_mask : u32
	{
		DMA_ACCESS_TIME      = 0x00000007,
		DMA_TRANSFER_WIDTH   = 0x00000018,
		DMA_INTERRUPT_ENABLE = 0x00000020,
		DMA_BURST_MODE       = 0x00000040,
		DMA_FAST_DMA_CYCLE   = 0x00000080,
	};
	enum dma_access_time : u32
	{
		ACCESS_40NS  = 0x00,
		ACCESS_80NS  = 0x01,
		ACCESS_120NS = 0x02,
		ACCESS_160NS = 0x03,
		ACCESS_200NS = 0x04,
		ACCESS_240NS = 0x05,
		ACCESS_280NS = 0x06,
		ACCESS_320NS = 0x07,
	};
	enum dma_transfer_width : u32
	{
		WIDTH_8BITS  = 0x08,
		WIDTH_16BITS = 0x10,
		WIDTH_32BITS = 0x18,
	};
	enum dma_enable_mask : u32
	{
		DMA_ENABLE            = 0x00000001,
		DMA_DIRECTION         = 0x00000002, // 1 == write to device
		DMA_TERMINAL_COUNT    = 0x00000100,
		DMA_MEMORY_ERROR      = 0x00000200,
		DMA_TRANSLATION_ERROR = 0x00000400,
	};
	enum dma_interrupt_source : u32
	{
		DMA_INTERRUPT_PENDING = 0x000000ff,
		DMA_PARITY_ERROR      = 0x00000100,
		DMA_ADDRESS_ERROR     = 0000000200,
		DMA_CACHE_FLUSH_ERROR = 0x00000400,
	};

	u32 m_dma_invalid_address;
	u32 m_trans_tbl_base;
	u32 m_trans_tbl_limit;
	u32 m_ioc_maint;
	u32 m_dma_memory_failed_address;
	u32 m_ioc_physical_tag;
	u32 m_ioc_logical_tag;
	u32 m_ioc_byte_mask;
	u32 m_remote_speed[16];
	u32 m_dma_reg[32];
	u32 m_dma_interrupt_source;
	u32 m_memory_refresh_rate;
	u32 m_nvram_protect;

	bool m_out_int_timer_asserted;
	bool m_out_int_device_asserted;
};

// device type definition
DECLARE_DEVICE_TYPE(JAZZ_MCT_ADR, jazz_mct_adr_device)

#endif // MAME_MACHINE_JAZZ_MCT_ADR_H
