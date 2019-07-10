// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/**********************************************************************

    SGI HPC3 "High-performance Peripheral Controller" emulation

**********************************************************************/

#ifndef MAME_MACHINE_HPC3_H
#define MAME_MACHINE_HPC3_H

#pragma once

#include "machine/hal2.h"

class hpc3_device : public device_t, public device_memory_interface
{
public:
	enum pbus_space
	{
		AS_PIO0 = 0,
		AS_PIO1,
		AS_PIO2,
		AS_PIO3,
		AS_PIO4,
		AS_PIO5,
		AS_PIO6,
		AS_PIO7,
		AS_PIO8,
		AS_PIO9
	};

	hpc3_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template <typename T>
	hpc3_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&hal2_tag)
		: hpc3_device(mconfig, tag, owner, (uint32_t)0)
	{
		set_hal2_tag(std::forward<T>(hal2_tag));
	}

	template <typename T> void set_gio64_space(T &&tag, int spacenum) { m_gio64_space.set_tag(std::forward<T>(tag), spacenum); }
	template <typename T> void set_hal2_tag(T &&tag) { m_hal2.set_tag(std::forward<T>(tag)); }

	auto enet_rd_cb() { return m_enet_rd_cb.bind(); }
	auto enet_wr_cb() { return m_enet_wr_cb.bind(); }
	auto enet_rxrd_cb() { return m_enet_rxrd_cb.bind(); }
	auto enet_txwr_cb() { return m_enet_txwr_cb.bind(); }
	auto enet_d8_rd_cb() { return m_enet_d8_rd_cb.bind(); }
	auto enet_d8_wr_cb() { return m_enet_d8_wr_cb.bind(); }
	auto enet_reset_cb() { return m_enet_reset_cb.bind(); }
	auto enet_loopback_cb() { return m_enet_loopback_cb.bind(); }
	auto enet_intr_out_cb() { return m_enet_intr_out_cb.bind(); }
	template <int N> auto hd_rd_cb() { return m_hd_rd_cb[N].bind(); }
	template <int N> auto hd_wr_cb() { return m_hd_wr_cb[N].bind(); }
	template <int N> auto hd_dma_rd_cb() { return m_hd_dma_rd_cb[N].bind(); }
	template <int N> auto hd_dma_wr_cb() { return m_hd_dma_wr_cb[N].bind(); }
	template <int N> auto hd_reset_cb() { return m_hd_reset_cb[N].bind(); }
	auto bbram_rd_cb() { return m_bbram_rd_cb.bind(); }
	auto bbram_wr_cb() { return m_bbram_wr_cb.bind(); }
	auto eeprom_dati_cb() { return m_eeprom_dati_cb.bind(); }
	auto eeprom_dato_cb() { return m_eeprom_dato_cb.bind(); }
	auto eeprom_clk_cb() { return m_eeprom_clk_cb.bind(); }
	auto eeprom_cs_cb() { return m_eeprom_cs_cb.bind(); }
	auto eeprom_pre_cb() { return m_eeprom_pre_cb.bind(); }
	auto dma_complete_int_cb() { return m_dma_complete_int_cb.bind(); }

	void map(address_map &map);

	DECLARE_WRITE_LINE_MEMBER(enet_txrdy_w);
	DECLARE_WRITE_LINE_MEMBER(enet_rxrdy_w);
	DECLARE_WRITE_LINE_MEMBER(enet_rxdc_w);
	DECLARE_WRITE_LINE_MEMBER(enet_txret_w);
	DECLARE_WRITE_LINE_MEMBER(enet_intr_in_w);

	DECLARE_WRITE_LINE_MEMBER(scsi0_drq);
	DECLARE_WRITE_LINE_MEMBER(scsi1_drq);

protected:
	virtual void device_resolve_objects() override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	virtual space_config_vector memory_space_config() const override;

	enum fifo_type_t : uint32_t
	{
		FIFO_PBUS,
		FIFO_SCSI0,
		FIFO_SCSI1,
		FIFO_ENET_RECV,
		FIFO_ENET_XMIT
	};

	DECLARE_READ32_MEMBER(enet_r);
	DECLARE_WRITE32_MEMBER(enet_w);
	DECLARE_READ32_MEMBER(hd_enet_r);
	DECLARE_WRITE32_MEMBER(hd_enet_w);
	template <uint32_t index> DECLARE_READ32_MEMBER(hd_r);
	template <uint32_t index> DECLARE_WRITE32_MEMBER(hd_w);
	template <fifo_type_t Type> DECLARE_READ32_MEMBER(fifo_r);
	template <fifo_type_t Type> DECLARE_WRITE32_MEMBER(fifo_w);
	DECLARE_READ32_MEMBER(intstat_r);
	uint32_t misc_r();
	void misc_w(uint32_t data);
	uint32_t eeprom_r();
	void eeprom_w(uint32_t data);
	uint32_t pio_data_r(offs_t offset);
	void pio_data_w(offs_t offset, uint32_t data);
	DECLARE_READ32_MEMBER(pbusdma_r);
	DECLARE_WRITE32_MEMBER(pbusdma_w);

	DECLARE_READ32_MEMBER(dma_config_r);
	DECLARE_WRITE32_MEMBER(dma_config_w);
	DECLARE_READ32_MEMBER(pio_config_r);
	DECLARE_WRITE32_MEMBER(pio_config_w);
	uint32_t bbram_r(offs_t offset);
	void bbram_w(offs_t offset, uint32_t data);

	void do_pbus_dma(uint32_t channel);
	void do_scsi_dma(int channel);

	void dump_chain(uint32_t base);
	void fetch_chain(int channel);
	void decrement_chain(int channel);
	void scsi_fifo_flush(int channel);
	void scsi_drq(bool state, int channel);
	//void scsi_dma(int channel);

	static const device_timer_id TIMER_PBUS_DMA = 0;

	struct pbus_dma_t
	{
		bool m_active;
		uint32_t m_cur_ptr;
		uint32_t m_desc_ptr;
		uint32_t m_desc_flags;
		uint32_t m_next_ptr;
		uint32_t m_bytes_left;
		uint32_t m_config;
		uint32_t m_control;
		emu_timer *m_timer;
	};

	enum
	{
		PBUS_CTRL_ENDIAN    = 0x00000002,
		PBUS_CTRL_RECV      = 0x00000004,
		PBUS_CTRL_FLUSH     = 0x00000008,
		PBUS_CTRL_DMASTART  = 0x00000010,
		PBUS_CTRL_LOAD_EN   = 0x00000020,
		PBUS_CTRL_REALTIME  = 0x00000040,
		PBUS_CTRL_HIGHWATER = 0x0000ff00,
		PBUS_CTRL_FIFO_BEG  = 0x003f0000,
		PBUS_CTRL_FIFO_END  = 0x3f000000,
	};

	enum
	{
		PBUS_DMADESC_EOX  = 0x80000000,
		PBUS_DMADESC_EOXP = 0x40000000,
		PBUS_DMADESC_XIE  = 0x20000000,
		PBUS_DMADESC_IPG  = 0x00ff0000,
		PBUS_DMADESC_TXD  = 0x00008000,
		PBUS_DMADESC_BC   = 0x00003fff,
	};

	enum
	{
		HPC3_DMACTRL_IRQ    = 0x01,
		HPC3_DMACTRL_ENDIAN = 0x02,
		HPC3_DMACTRL_DIR    = 0x04,
		HPC3_DMACTRL_FLUSH  = 0x08,
		HPC3_DMACTRL_ENABLE = 0x10,
		HPC3_DMACTRL_WRMASK = 0x20,
	};

	enum
	{
		ENET_RECV = 0,
		ENET_XMIT = 1
	};

	const address_space_config m_pio_space_config[10];

	required_address_space m_gio64_space;
	address_space *m_pio_space[10];
	required_device<hal2_device> m_hal2;

	devcb_read8 m_enet_rd_cb;
	devcb_write8 m_enet_wr_cb;
	devcb_read8 m_enet_rxrd_cb;
	devcb_write8 m_enet_txwr_cb;
	devcb_read_line m_enet_d8_rd_cb;
	devcb_write_line m_enet_d8_wr_cb;
	devcb_write_line m_enet_reset_cb;
	devcb_write_line m_enet_loopback_cb;
	devcb_write_line m_enet_intr_out_cb;
	devcb_read8 m_hd_rd_cb[2];
	devcb_write8 m_hd_wr_cb[2];
	devcb_read8 m_hd_dma_rd_cb[2];
	devcb_write8 m_hd_dma_wr_cb[2];
	devcb_write_line m_hd_reset_cb[2];
	devcb_read8 m_bbram_rd_cb;
	devcb_write8 m_bbram_wr_cb;
	devcb_read_line m_eeprom_dati_cb;
	devcb_write_line m_eeprom_dato_cb;
	devcb_write_line m_eeprom_clk_cb;
	devcb_write_line m_eeprom_cs_cb;
	devcb_write_line m_eeprom_pre_cb;
	devcb_write_line m_dma_complete_int_cb;

	uint32_t m_intstat;
	uint32_t m_misc;
	uint32_t m_cpu_aux_ctrl;

	struct scsi_dma_t
	{
		uint32_t m_cbp;
		uint32_t m_nbdp;
		uint8_t m_ctrl;
		uint32_t m_bc;
		uint16_t m_count;
		uint32_t m_dmacfg;
		uint32_t m_piocfg;
		bool m_drq;
		bool m_big_endian;
		bool m_to_device;
		bool m_active;
	};

	struct enet_dma_t
	{
		uint32_t m_cbp;
		uint32_t m_nbdp;
		uint32_t m_bc;
		uint32_t m_ctrl;
		uint32_t m_gio_fifo_ptr;
		uint32_t m_dev_fifo_ptr;
	};

	enet_dma_t m_enet_dma[2];
	uint32_t m_enet_reset;
	uint32_t m_enet_dmacfg;
	uint32_t m_enet_piocfg;

	scsi_dma_t m_scsi_dma[2];
	pbus_dma_t m_pbus_dma[8];
	uint32_t m_pio_config[10];

	std::unique_ptr<uint32_t[]> m_pbus_fifo;
	std::unique_ptr<uint32_t[]> m_scsi_fifo[2];
	std::unique_ptr<uint32_t[]> m_enet_fifo[2];
};

DECLARE_DEVICE_TYPE(SGI_HPC3, hpc3_device)

#endif // MAME_MACHINE_HPC3_H
