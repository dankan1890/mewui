// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

        Poly-88 machine by Miodrag Milanovic

        18/05/2009 Initial implementation

****************************************************************************/

#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "includes/poly88.h"


// bits 0-3 baud rate; bit 4 (0=cassette, 1=rs232); bit 5 (1=disable rom and ram)
void poly88_state::baud_rate_w(uint8_t data)
{
	logerror("poly88_baud_rate_w %02x\n",data);
	m_brg->control_w(data & 15);
}

IRQ_CALLBACK_MEMBER(poly88_state::poly88_irq_callback)
{
	return m_int_vector;
}

TIMER_DEVICE_CALLBACK_MEMBER( poly88_state::kansas_r )
{
	if (BIT(m_linec->read(), 7))
		return;

	// no tape - set uart to idle
	m_cass_data[1]++;
	if (m_dtr || (m_cass_data[1] > 32))
	{
		m_cass_data[1] = 32;
		m_rxd = 1;
	}

	// turn 1200/2400Hz to a bit
	uint8_t cass_ws = (m_cassette->input() > +0.04) ? 1 : 0;

	if (cass_ws != m_cass_data[0])
	{
		m_cass_data[0] = cass_ws;
		m_rxd = (m_cass_data[1] < 12) ? 1 : 0;
		m_cass_data[1] = 0;
	}
}

WRITE_LINE_MEMBER(poly88_state::cassette_clock_w)
{
	// incoming @4800Hz (bit), 2400Hz (polyphase)
	if (BIT(m_linec->read(), 7))
	{
		// polyphase
		// SAVE
		if (!m_rts)
			m_cassette->output((m_txd ^ state) ? 1.0 : -1.0);
		// LOAD
		if (!m_dtr)
		{
			u8 cass_ws = (m_cassette->input() > +0.04) ? 1 : 0;
			if (state)
				m_usart->write_rxd(cass_ws);
		}
	}
	else
	{
		// byte mode 300 baud Kansas City format
		u8 twobit = m_cass_data[3] & 3;

		// SAVE
		if (m_rts && (twobit == 0))
		{
			m_cassette->output(0);
			m_cass_data[3] = 0;     // reset waveforms
		}
		else
		if (state)
		{
			if (twobit == 0)
				m_cassold = m_txd;

			if (m_cassold)
				m_cassette->output(BIT(m_cass_data[3], 0) ? -1.0 : +1.0); // 2400Hz
			else
				m_cassette->output(BIT(m_cass_data[3], 1) ? -1.0 : +1.0); // 1200Hz

			m_cass_data[3]++;
		}

		// LOAD
		if (state && !m_dtr && (twobit == 0))
			m_usart->write_rxd(m_rxd);
	}

	if (!m_rts)
		m_usart->write_txc(state);

	if (!m_dtr)
		m_usart->write_rxc(state);
}


void poly88_state::init_poly88()
{
}

void poly88_state::machine_reset()
{
	m_last_code = 0;
	m_usart->write_rxd(1);
	m_usart->write_cts(0);
	m_brg->control_w(0);
	m_dtr = m_rts = m_txd = m_rxd = m_cassold = 1;
}

INTERRUPT_GEN_MEMBER(poly88_state::poly88_interrupt)
{
	m_int_vector = 0xf7;
	device.execute().set_input_line(0, HOLD_LINE);
}

WRITE_LINE_MEMBER(poly88_state::usart_ready_w)
{
	if (state)
	{
		m_int_vector = 0xe7;
		m_maincpu->set_input_line(0, HOLD_LINE);
	}
}

uint8_t poly88_state::keyboard_r()
{
	uint8_t retVal = m_last_code;
	m_maincpu->set_input_line(0, CLEAR_LINE);
	m_last_code = 0x00;
	return retVal;
}

void poly88_state::intr_w(uint8_t data)
{
	m_maincpu->set_input_line(0, CLEAR_LINE);
}

SNAPSHOT_LOAD_MEMBER(poly88_state::snapshot_cb)
{
	address_space &space = m_maincpu->space(AS_PROGRAM);
	uint8_t* data= auto_alloc_array(machine(), uint8_t, snapshot_size);
	uint16_t recordNum;
	uint16_t recordLen;
	uint16_t address;
	uint8_t  recordType;

	int pos = 0x300;
	char name[9];
	int i = 0;
	int theend = 0;

	image.fread( data, snapshot_size);

	while (pos<snapshot_size) {
		for(i=0;i<9;i++) {
			name[i] = (char) data[pos + i];
		}
		pos+=8;
		name[8] = 0;


		recordNum = data[pos]+ data[pos+1]*256; pos+=2;
		recordLen = data[pos]; pos++;
		if (recordLen==0) recordLen=0x100;
		address = data[pos] + data[pos+1]*256; pos+=2;
		recordType = data[pos]; pos++;

		logerror("Block :%s number:%d length: %d address=%04x type:%d\n",name,recordNum,recordLen,address, recordType);
		switch(recordType) {
			case 0 :
					/* 00 Absolute */
					memcpy(space.get_read_ptr(address ), data + pos ,recordLen);
					break;
			case 1 :
					/* 01 Comment */
					break;
			case 2 :
					/* 02 End */
					theend = 1;
					break;
			case 3 :
					/* 03 Auto Start @ Address */
					m_maincpu->set_state_int(i8080_cpu_device::I8085_PC, address);
					theend = 1;
					break;
			case 4 :
					/* 04 Data ( used by Assembler ) */
					logerror("ASM load unsupported\n");
					theend = 1;
					break;
			case 5 :
					/* 05 BASIC program file */
					logerror("BASIC load unsupported\n");
					theend = 1;
					break;
			case 6 :
					/* 06 End ( used by Assembler? ) */
					theend = 1;
					break;
			default: break;
		}

		if (theend) {
			break;
		}
		pos+=recordLen;
	}
	m_usart->reset();
	return image_init_result::PASS;
}
