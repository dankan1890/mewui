// license:BSD-3-Clause
// copyright-holders:Barry Rodewald
/*
    Hitachi HD63450 DMA Controller

    Largely based on documentation of the Sharp X68000
*/

#include "emu.h"
#include "hd63450.h"

//#define VERBOSE 1
#include "logmacro.h"

DEFINE_DEVICE_TYPE(HD63450, hd63450_device, "hd63450", "Hitachi HD63450 DMAC")

hd63450_device::hd63450_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, HD63450, tag, owner, clock),
		m_irq_callback(*this),
		m_dma_end(*this),
		m_dma_read{{*this}, {*this}, {*this}, {*this}},
		m_dma_write{{*this}, {*this}, {*this}, {*this}},
		m_cpu(*this, finder_base::DUMMY_TAG)
{
	for (int i = 0; i < 4; i++)
	{
		memset(&m_reg[i], 0, sizeof(m_reg[i]));
		m_timer[i] = nullptr;

		m_transfer_size[i] = 0;
		m_halted[i] = 0;
		m_drq_state[i] = 0;

		m_our_clock[i] = attotime::zero;
		m_burst_clock[i] = attotime::zero;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void hd63450_device::device_start()
{
	// resolve callbacks
	m_irq_callback.resolve_safe();
	m_dma_end.resolve_safe();
	for (auto &cb : m_dma_read)
		cb.resolve();
	for (auto &cb : m_dma_write)
		cb.resolve();

	// Initialise timers and registers
	for (int x = 0; x < 4; x++)
	{
		m_timer[x] = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(hd63450_device::dma_transfer_timer), this));

		save_item(NAME(m_reg[x].csr), x);
		save_item(NAME(m_reg[x].cer), x);
		save_item(NAME(m_reg[x].dcr), x);
		save_item(NAME(m_reg[x].ocr), x);
		save_item(NAME(m_reg[x].scr), x);
		save_item(NAME(m_reg[x].ccr), x);
		save_item(NAME(m_reg[x].mtc), x);
		save_item(NAME(m_reg[x].mar), x);
		save_item(NAME(m_reg[x].dar), x);
		save_item(NAME(m_reg[x].btc), x);
		save_item(NAME(m_reg[x].niv), x);
		save_item(NAME(m_reg[x].eiv), x);
		save_item(NAME(m_reg[x].mfc), x);
		save_item(NAME(m_reg[x].cpr), x);
		save_item(NAME(m_reg[x].dfc), x);
		save_item(NAME(m_reg[x].bfc), x);
		save_item(NAME(m_reg[x].gcr), x);
	}

	save_item(NAME(m_transfer_size));
	save_item(NAME(m_halted));
	save_item(NAME(m_drq_state));
	save_item(NAME(m_irq_channel));
}

void hd63450_device::device_reset()
{
	// Device is reset by pulling /BEC0-/BEC2 all low for 10 clocks

	for (int x = 0; x < 4; x++)
	{
		m_reg[x].niv = 0x0f;
		m_reg[x].eiv = 0x0f;
		m_reg[x].cpr = 0;
		m_reg[x].dcr = 0;
		m_reg[x].ocr = 0;
		m_reg[x].scr = 0;
		m_reg[x].ccr = 0;
		m_reg[x].csr &= 0x01;
		m_reg[x].cer = 0;
		m_reg[x].gcr = 0;

		m_timer[x]->adjust(attotime::never);
		m_halted[x] = 0;
	}

	m_irq_channel = -1;
	m_irq_callback(CLEAR_LINE);
}

READ16_MEMBER(hd63450_device::read)
{
	int channel,reg;

	channel = (offset & 0x60) >> 5;
	reg = offset & 0x1f;

	switch(reg)
	{
	case 0x00:  // CSR / CER
		return (m_reg[channel].csr << 8) | m_reg[channel].cer;
	case 0x02:  // DCR / OCR
		return (m_reg[channel].dcr << 8) | m_reg[channel].ocr;
	case 0x03:  // SCR / CCR
		return (m_reg[channel].scr << 8) | m_reg[channel].ccr;
	case 0x05:  // MTC
		return m_reg[channel].mtc;
	case 0x06:  // MAR (high)
		return (m_reg[channel].mar & 0xffff0000) >> 16;
	case 0x07:  // MAR (low)
		return (m_reg[channel].mar & 0x0000ffff);
	case 0x0a:  // DAR (high)
		return (m_reg[channel].dar & 0xffff0000) >> 16;
	case 0x0b:  // DAR (low)
		return (m_reg[channel].dar & 0x0000ffff);
	case 0x0d:  // BTC
		return m_reg[channel].btc;
	case 0x0e:  // BAR (high)
		return (m_reg[channel].bar & 0xffff0000) >> 16;
	case 0x0f:  // BAR (low)
		return (m_reg[channel].bar & 0x0000ffff);
	case 0x12:  // NIV
		return m_reg[channel].niv;
	case 0x13:  // EIV
		return m_reg[channel].eiv;
	case 0x14:  // MFC
		return m_reg[channel].mfc;
	case 0x16:  // CPR
		return m_reg[channel].cpr;
	case 0x18:  // DFC
		return m_reg[channel].dfc;
	case 0x1c:  // BFC
		return m_reg[channel].bfc;
	case 0x1f:  // GCR
		return m_reg[channel].gcr;
	}
	return 0xff;
}

WRITE16_MEMBER(hd63450_device::write)
{
	int channel,reg;

	channel = (offset & 0x60) >> 5;
	reg = offset & 0x1f;
	switch(reg)
	{
	case 0x00:  // CSR / CER
		if (ACCESSING_BITS_8_15)
		{
			// Writes to CSR clear all corresponding 1 bits except PCS and ACT
			m_reg[channel].csr &= ~((data & 0xf600) >> 8);
//          LOG("DMA#%i: Channel status write : %02x\n",channel,dmac.reg[channel].csr);

			// Clearing ERR also resets CER (which is otherwise read-only)
			if ((data & 0x1000) != 0)
				m_reg[channel].cer = 0;

			if ((m_reg[channel].csr & 0xf2) == 0)
				clear_irq(channel);
		}
		break;
	case 0x02:  // DCR / OCR
		if (ACCESSING_BITS_8_15)
		{
			m_reg[channel].dcr = (data & 0xff00) >> 8;
			LOG("DMA#%i: Device Control write : %02x\n",channel,m_reg[channel].dcr);
		}
		if (ACCESSING_BITS_0_7)
		{
			m_reg[channel].ocr = data & 0x00ff;
			LOG("DMA#%i: Operation Control write : %02x\n",channel,m_reg[channel].ocr);
		}
		break;
	case 0x03:  // SCR / CCR
		if (ACCESSING_BITS_8_15)
		{
			m_reg[channel].scr = (data & 0xff00) >> 8;
			LOG("DMA#%i: Sequence Control write : %02x\n",channel,m_reg[channel].scr);
		}
		if (ACCESSING_BITS_0_7)
		{
			m_reg[channel].ccr = data & 0x00ff;
			if ((data & 0x0080))// && !m_dma_read[channel] && !m_dma_write[channel])
				dma_transfer_start(channel);
			if (data & 0x0010)  // software abort
				dma_transfer_abort(channel);
			if (data & 0x0020)  // halt operation
				dma_transfer_halt(channel);
			if (data & 0x0040)  // continure operation
				dma_transfer_continue(channel);
			if ((data & 0x0008) == 0)
				clear_irq(channel);
			else if ((m_reg[channel].csr & 0xf2) != 0)
				set_irq(channel);
			LOG("DMA#%i: Channel Control write : %02x\n",channel,m_reg[channel].ccr);
		}
		break;
	case 0x05:  // MTC
		m_reg[channel].mtc = data;
		LOG("DMA#%i:  Memory Transfer Counter write : %04x\n",channel,m_reg[channel].mtc);
		break;
	case 0x06:  // MAR (high)
		m_reg[channel].mar = (m_reg[channel].mar & 0x0000ffff) | (data << 16);
		LOG("DMA#%i:  Memory Address write : %08lx\n",channel,m_reg[channel].mar);
		break;
	case 0x07:  // MAR (low)
		m_reg[channel].mar = (m_reg[channel].mar & 0xffff0000) | (data & 0x0000ffff);
		LOG("DMA#%i:  Memory Address write : %08lx\n",channel,m_reg[channel].mar);
		break;
	case 0x0a:  // DAR (high)
		m_reg[channel].dar = (m_reg[channel].dar & 0x0000ffff) | (data << 16);
		LOG("DMA#%i:  Device Address write : %08lx\n",channel,m_reg[channel].dar);
		break;
	case 0x0b:  // DAR (low)
		m_reg[channel].dar = (m_reg[channel].dar & 0xffff0000) | (data & 0x0000ffff);
		LOG("DMA#%i:  Device Address write : %08lx\n",channel,m_reg[channel].dar);
		break;
	case 0x0d:  // BTC
		m_reg[channel].btc = data;
		LOG("DMA#%i:  Base Transfer Counter write : %04x\n",channel,m_reg[channel].btc);
		break;
	case 0x0e:  // BAR (high)
		m_reg[channel].bar = (m_reg[channel].bar & 0x0000ffff) | (data << 16);
		LOG("DMA#%i:  Base Address write : %08lx\n",channel,m_reg[channel].bar);
		break;
	case 0x0f:  // BAR (low)
		m_reg[channel].bar = (m_reg[channel].bar & 0xffff0000) | (data & 0x0000ffff);
		LOG("DMA#%i:  Base Address write : %08lx\n",channel,m_reg[channel].bar);
		break;
	case 0x12:  // NIV
		m_reg[channel].niv = data & 0xff;
		LOG("DMA#%i:  Normal IRQ Vector write : %02x\n",channel,m_reg[channel].niv);
		break;
	case 0x13:  // EIV
		m_reg[channel].eiv = data & 0xff;
		LOG("DMA#%i:  Error IRQ Vector write : %02x\n",channel,m_reg[channel].eiv);
		break;
	case 0x14:  // MFC
		m_reg[channel].mfc = data & 0xff;
		LOG("DMA#%i:  Memory Function Code write : %02x\n",channel,m_reg[channel].mfc);
		break;
	case 0x16:  // CPR
		m_reg[channel].cpr = data & 0xff;
		LOG("DMA#%i:  Channel Priority write : %02x\n",channel,m_reg[channel].cpr);
		break;
	case 0x18:  // DFC
		m_reg[channel].dfc = data & 0xff;
		LOG("DMA#%i:  Device Function Code write : %02x\n",channel,m_reg[channel].dfc);
		break;
	case 0x1c:  // BFC
		m_reg[channel].bfc = data & 0xff;
		LOG("DMA#%i:  Base Function Code write : %02x\n",channel,m_reg[channel].bfc);
		break;
	case 0x1f:
		m_reg[channel].gcr = data & 0xff;
		LOG("DMA#%i:  General Control write : %02x\n",channel,m_reg[channel].gcr);
		break;
	}
}

void hd63450_device::dma_transfer_start(int channel)
{
	address_space &space = m_cpu->space(AS_PROGRAM);
	m_reg[channel].csr &= ~0xe0;
	m_reg[channel].csr |= 0x08;  // Channel active
	m_reg[channel].csr &= ~0x30;  // Reset Error and Normal termination bits
	if ((m_reg[channel].ocr & 0x0c) != 0x00)  // Array chain or Link array chain
	{
		m_reg[channel].mar = space.read_word(m_reg[channel].bar) << 16;
		m_reg[channel].mar |= space.read_word(m_reg[channel].bar+2);
		m_reg[channel].mtc = space.read_word(m_reg[channel].bar+4);
		if (m_reg[channel].btc > 0)
			m_reg[channel].btc--;
	}

	// Burst transfers will halt the CPU until the transfer is complete
	if ((m_reg[channel].dcr & 0xc0) == 0x00)  // Burst transfer
	{
		m_cpu->set_input_line(INPUT_LINE_HALT, ASSERT_LINE);
		m_timer[channel]->adjust(attotime::zero, channel, m_burst_clock[channel]);
	}
	else if (!(m_reg[channel].ocr & 2))
		m_timer[channel]->adjust(attotime::from_usec(500), channel, m_our_clock[channel]);
	else if ((m_reg[channel].ocr & 3) == 3)
		m_timer[channel]->adjust(attotime::from_usec(500), channel, attotime::never);
	else if ((m_reg[channel].ocr & 3) == 2)
		m_timer[channel]->adjust(attotime::never, channel, attotime::never);

	m_transfer_size[channel] = m_reg[channel].mtc;

	LOG("DMA: Transfer begins: size=0x%08x\n",m_transfer_size[channel]);
}

void hd63450_device::set_timer(int channel, const attotime &tm)
{
	m_our_clock[channel] = tm;
	if (dma_in_progress(channel))
		m_timer[channel]->adjust(attotime::zero, channel, m_our_clock[channel]);
}

TIMER_CALLBACK_MEMBER(hd63450_device::dma_transfer_timer)
{
	if (((m_reg[param].ocr & 3) == 2) && !m_drq_state[param])
		return;
	single_transfer(param);
}

void hd63450_device::dma_transfer_abort(int channel)
{
	if (!dma_in_progress(channel))
		return;

	LOG("DMA#%i: Transfer aborted\n",channel);
	m_timer[channel]->adjust(attotime::never);
	set_error(channel, 0x11);
}

void hd63450_device::dma_transfer_halt(int channel)
{
	m_halted[channel] = 1;
	m_timer[channel]->adjust(attotime::never);
}

void hd63450_device::dma_transfer_continue(int channel)
{
	if (m_halted[channel] != 0)
	{
		m_halted[channel] = 0;
		m_timer[channel]->adjust(attotime::zero, channel, m_our_clock[channel]);
	}
}

void hd63450_device::single_transfer(int x)
{
	address_space &space = m_cpu->space(AS_PROGRAM);
	int data;
	int datasize = 1;

	if (!dma_in_progress(x))  // DMA in progress in channel x
		return;

	if (m_reg[x].ocr & 0x80)  // direction: 1 = device -> memory
	{
		if (!m_dma_read[x].isnull())
		{
			data = m_dma_read[x](m_reg[x].mar);
			if (data == -1)
				return;  // not ready to receive data
			space.write_byte(m_reg[x].mar,data);
			datasize = 1;
		}
		else
		{
			switch(m_reg[x].ocr & 0x30)  // operation size
			{
			case 0x00:  // 8 bit
				data = space.read_byte(m_reg[x].dar);  // read from device address
				space.write_byte(m_reg[x].mar, data);  // write to memory address
				datasize = 1;
				break;
			case 0x10:  // 16 bit
				data = space.read_word(m_reg[x].dar);  // read from device address
				space.write_word(m_reg[x].mar, data);  // write to memory address
				datasize = 2;
				break;
			case 0x20:  // 32 bit
				data = space.read_word(m_reg[x].dar) << 16;  // read from device address
				data |= space.read_word(m_reg[x].dar+2);
				space.write_word(m_reg[x].mar, (data & 0xffff0000) >> 16);  // write to memory address
				space.write_word(m_reg[x].mar+2, data & 0x0000ffff);
				datasize = 4;
				break;
			case 0x30:  // 8 bit packed (?)
				data = space.read_byte(m_reg[x].dar);  // read from device address
				space.write_byte(m_reg[x].mar, data);  // write to memory address
				datasize = 1;
				break;
			}
		}
//              LOG("DMA#%i: byte transfer %08lx -> %08lx  (byte = %02x)\n",x,dmac.reg[x].dar,dmac.reg[x].mar,data);
	}
	else  // memory -> device
	{
		if (!m_dma_write[x].isnull())
		{
			data = space.read_byte(m_reg[x].mar);
			m_dma_write[x]((offs_t)m_reg[x].mar,data);
			datasize = 1;
		}
		else
		{
			switch(m_reg[x].ocr & 0x30)  // operation size
			{
			case 0x00:  // 8 bit
				data = space.read_byte(m_reg[x].mar);  // read from memory address
				space.write_byte(m_reg[x].dar, data);  // write to device address
				datasize = 1;
				break;
			case 0x10:  // 16 bit
				data = space.read_word(m_reg[x].mar);  // read from memory address
				space.write_word(m_reg[x].dar, data);  // write to device address
				datasize = 2;
				break;
			case 0x20:  // 32 bit
				data = space.read_word(m_reg[x].mar) << 16;  // read from memory address
				data |= space.read_word(m_reg[x].mar+2);  // read from memory address
				space.write_word(m_reg[x].dar, (data & 0xffff0000) >> 16);  // write to device address
				space.write_word(m_reg[x].dar+2, data & 0x0000ffff);  // write to device address
				datasize = 4;
				break;
			case 0x30:  // 8 bit packed (?)
				data = space.read_byte(m_reg[x].mar);  // read from memory address
				space.write_byte(m_reg[x].dar, data);  // write to device address
				datasize = 1;
				break;
			}
		}
//              LOG("DMA#%i: byte transfer %08lx -> %08lx\n",x,m_reg[x].mar,m_reg[x].dar);
	}


	// decrease memory transfer counter
	if (m_reg[x].mtc > 0)
		m_reg[x].mtc--;

	// handle change of memory and device addresses
	if ((m_reg[x].scr & 0x03) == 0x01)
		m_reg[x].dar+=datasize;
	else if ((m_reg[x].scr & 0x03) == 0x02)
		m_reg[x].dar-=datasize;

	if ((m_reg[x].scr & 0x0c) == 0x04)
		m_reg[x].mar+=datasize;
	else if ((m_reg[x].scr & 0x0c) == 0x08)
		m_reg[x].mar-=datasize;

	if (m_reg[x].mtc <= 0)
	{
		// End of transfer
		LOG("DMA#%i: End of transfer\n",x);
		if ((m_reg[x].ocr & 0x0c) != 0 && m_reg[x].btc > 0)
		{
			m_reg[x].btc--;
			m_reg[x].bar+=6;
			m_reg[x].mar = space.read_word(m_reg[x].bar) << 16;
			m_reg[x].mar |= space.read_word(m_reg[x].bar+2);
			m_reg[x].mtc = space.read_word(m_reg[x].bar+4);
			return;
		}
		m_timer[x]->adjust(attotime::never);
		m_reg[x].csr |= 0xe0;  // channel operation complete, block transfer complete
		m_reg[x].csr &= ~0x08;  // channel no longer active
		m_reg[x].ccr &= ~0xc0;

		// Burst transfer
		if ((m_reg[x].dcr & 0xc0) == 0x00)
		{
			m_cpu->set_input_line(INPUT_LINE_HALT, CLEAR_LINE);
		}

		m_dma_end((offs_t)x, 0);
		set_irq(x);
	}
}

void hd63450_device::set_error(int channel, uint8_t code)
{
	m_reg[channel].csr |= 0x90;  // channel error
	m_reg[channel].csr &= ~0x08;  // channel no longer active
	m_reg[channel].cer = code;
	m_reg[channel].ccr &= ~0xc0;

	set_irq(channel);
}

WRITE_LINE_MEMBER(hd63450_device::drq0_w)
{
	bool ostate = m_drq_state[0];
	m_drq_state[0] = state;

	if ((m_reg[0].ocr & 2) && (state && !ostate))
	{
		// in cycle steal mode drq is supposed to be edge triggered
		single_transfer(0);
		m_timer[0]->adjust(m_our_clock[0], 0, m_our_clock[0]);
	}
	else if (!state)
		m_timer[0]->adjust(attotime::never);
}

WRITE_LINE_MEMBER(hd63450_device::drq1_w)
{
	bool ostate = m_drq_state[1];
	m_drq_state[1] = state;

	if ((m_reg[1].ocr & 2) && (state && !ostate))
	{
		single_transfer(1);
		m_timer[1]->adjust(m_our_clock[1], 1, m_our_clock[1]);
	}
	else if (!state)
		m_timer[1]->adjust(attotime::never);
}

WRITE_LINE_MEMBER(hd63450_device::drq2_w)
{
	bool ostate = m_drq_state[2];
	m_drq_state[2] = state;

	if ((m_reg[2].ocr & 2) && (state && !ostate))
	{
		single_transfer(2);
		m_timer[2]->adjust(m_our_clock[2], 2, m_our_clock[2]);
	}
	else if (!state)
		m_timer[2]->adjust(attotime::never);
}

WRITE_LINE_MEMBER(hd63450_device::drq3_w)
{
	bool ostate = m_drq_state[3];
	m_drq_state[3] = state;

	if ((m_reg[3].ocr & 2) && (state && !ostate))
	{
		single_transfer(3);
		m_timer[3]->adjust(m_our_clock[3], 3, m_our_clock[3]);
	}
	else if (!state)
		m_timer[3]->adjust(attotime::never);
}

void hd63450_device::set_irq(int channel)
{
	if ((m_reg[channel].ccr & 0x08) == 0)
		return;

	if (m_irq_channel == -1)
	{
		m_irq_channel = channel;
		m_irq_callback(ASSERT_LINE);
	}
	else if ((m_reg[channel].cpr & 0x03) < (m_reg[m_irq_channel].cpr & 0x03))
		m_irq_channel = channel;
}

void hd63450_device::clear_irq(int channel)
{
	if (m_irq_channel != channel)
		return;

	for (int pri = m_reg[channel].cpr & 0x03; pri <= 3; pri++)
	{
		for (int offset = 1; offset <= 3; offset++)
		{
			if ((m_reg[(channel + offset) & 3].ccr & 0x08) != 0 &&
				(m_reg[(channel + offset) & 3].csr & 0xf2) != 0)
			{
				m_irq_channel = (channel + offset) & 3;
				return;
			}
		}
	}

	m_irq_channel = -1;
	m_irq_callback(CLEAR_LINE);
}

uint8_t hd63450_device::iack()
{
	if (m_irq_channel != -1)
	{
		if ((m_reg[m_irq_channel].csr & 0x10) != 0)
			return m_reg[m_irq_channel].eiv;
		else
			return m_reg[m_irq_channel].niv;
	}

	// Spurious interrupt (no response actually)
	return 0x18;
}
