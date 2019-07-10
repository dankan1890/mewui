// license:BSD-3-Clause
// copyright-holders:R. Belmont
/******************************************************************************
*
*   MIPS DECstation and AlphaStation I/O Gate Array emulation
*   This IC contains some address decoding, an interrupt controller, and
*   a multi-channel DMA engine.
*/

#include "emu.h"
#include "decioga.h"

#define LOG_DMA         (1U << 0)
#define LOG_LANCE_DMA   (1U << 1)

#define VERBOSE (LOG_DMA|LOG_LANCE_DMA)

#include "logmacro.h"

DEFINE_DEVICE_TYPE(DECSTATION_IOGA, dec_ioga_device, "decioga", "DECstation I/O Gate Array")

void dec_ioga_device::map(address_map &map)
{
	map(0x040000, 0x0400ff).rw(FUNC(dec_ioga_device::dmaptr_r), FUNC(dec_ioga_device::dmaptr_w));
	map(0x040100, 0x040103).rw(FUNC(dec_ioga_device::csr_r), FUNC(dec_ioga_device::csr_w));
	map(0x040110, 0x040113).rw(FUNC(dec_ioga_device::intr_r), FUNC(dec_ioga_device::intr_w));
	map(0x040120, 0x040123).rw(FUNC(dec_ioga_device::imsk_r), FUNC(dec_ioga_device::imsk_w));
}

dec_ioga_device::dec_ioga_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, DECSTATION_IOGA, tag, owner, clock),
	m_irq_out_cb(*this)
{
}

void dec_ioga_device::device_start()
{
	m_irq_out_cb.resolve_safe();

	save_item(NAME(m_csr));
	save_item(NAME(m_intr));
	save_item(NAME(m_imsk));
}

void dec_ioga_device::device_reset()
{
	m_csr = m_intr = m_imsk = 0;
}

READ32_MEMBER(dec_ioga_device::csr_r)
{
	return m_csr;
}

WRITE32_MEMBER(dec_ioga_device::csr_w)
{
	COMBINE_DATA(&m_csr);
#if 0
	printf("%08x to CSR: diag LEDs [", m_csr);
	for (int i = 7; i >= 0; i--)
	{
		printf("%c", (m_csr & (1<<i)) ? 'O' : '.');
	}
	printf("]\n");
#endif
}

READ32_MEMBER(dec_ioga_device::intr_r)
{
	uint32_t rv = m_intr;
	m_intr &= ~0x20;        // 5000/133 boot ROM tests that reading clears this bit
	return rv;
}

void dec_ioga_device::recalc_irq()
{
	if ((m_intr & m_imsk) != 0)
	{
		m_irq_out_cb(ASSERT_LINE);
	}
	else
	{
		m_irq_out_cb(CLEAR_LINE);
	}
}

WRITE32_MEMBER(dec_ioga_device::intr_w)
{
	m_intr &= ~data;    // clear bits on write
}

READ32_MEMBER(dec_ioga_device::imsk_r)
{
	return m_imsk;
}

WRITE32_MEMBER(dec_ioga_device::imsk_w)
{
	COMBINE_DATA(&m_imsk);
}

READ32_MEMBER(dec_ioga_device::dmaptr_r)
{
	return m_dmaptrs[offset/4];
}

WRITE32_MEMBER(dec_ioga_device::dmaptr_w)
{
	COMBINE_DATA(&m_dmaptrs[offset/4]);
	LOGMASKED(LOG_DMA, "DECIOGA: %08x to DMA pointer %x (addr %x)\n", data, offset/4, offset*4);
}

WRITE_LINE_MEMBER(dec_ioga_device::rtc_irq_w)
{
	if (state == ASSERT_LINE)
	{
		m_intr |= 0x20; // tested by 5000/133 boot ROM circa BFC027C8
	}
	recalc_irq();
}

WRITE_LINE_MEMBER(dec_ioga_device::lance_irq_w)
{
	if (state == ASSERT_LINE)
	{
		m_intr |= 0x100;
	}
	else
	{
		m_intr &= ~0x100;
	}
	recalc_irq();
}

WRITE_LINE_MEMBER(dec_ioga_device::scc0_irq_w)
{
	if (state == ASSERT_LINE)
	{
		m_intr |= 0x40;
	}
	else
	{
		m_intr &= ~0x40;
	}
	recalc_irq();
}

WRITE_LINE_MEMBER(dec_ioga_device::scc1_irq_w)
{
	if (state == ASSERT_LINE)
	{
		m_intr |= 0x80;
	}
	else
	{
		m_intr &= ~0x80;
	}
	recalc_irq();
}

// DMA handlers
void dec_ioga_device::set_dma_space(address_space *space)
{
	m_maincpu_space = space;
}

READ16_MEMBER(dec_ioga_device::lance_dma_r)
{
	LOGMASKED(LOG_LANCE_DMA, "lance_dma_r: %08x (phys %08x)\n", offset, (offset<<1) + (m_dmaptrs[2]>>3));
	return m_maincpu_space->read_word((offset<<1) + (m_dmaptrs[2]>>3));
}

WRITE16_MEMBER(dec_ioga_device::lance_dma_w)
{
	LOGMASKED(LOG_LANCE_DMA, "lance_dma_w: %04x to %08x\n", data, offset);
	m_maincpu_space->write_word((offset<<1) + (m_dmaptrs[2]>>3), data);
}
