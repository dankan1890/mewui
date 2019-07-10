// license:BSD-3-Clause
// copyright-holders:AJR
/**********************************************************************

    VS9209 (4L01F1429) QFP80 I/O chip

**********************************************************************/

#ifndef MAME_MACHINE_VS9209_H
#define MAME_MACHINE_VS9209_H

#pragma once

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> vs9209_device

class vs9209_device : public device_t
{
public:
	// construction/destruction
	vs9209_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	// configuration
	auto porta_input_cb() { return m_input_cb[0].bind(); }
	auto portb_input_cb() { return m_input_cb[1].bind(); }
	auto portc_input_cb() { return m_input_cb[2].bind(); }
	auto portd_input_cb() { return m_input_cb[3].bind(); }
	auto porte_input_cb() { return m_input_cb[4].bind(); }
	auto portf_input_cb() { return m_input_cb[5].bind(); }
	auto portg_input_cb() { return m_input_cb[6].bind(); }
	auto porth_input_cb() { return m_input_cb[7].bind(); }
#ifdef VS9209_PROBABLY_NONEXISTENT_OUTPUTS
	auto porta_output_cb() { return m_output_cb[0].bind(); }
	auto portb_output_cb() { return m_output_cb[1].bind(); }
	auto portc_output_cb() { return m_output_cb[2].bind(); }
	auto portd_output_cb() { return m_output_cb[3].bind(); }
#endif // VS9209_PROBABLY_NONEXISTENT_OUTPUTS
	auto porte_output_cb() { return m_output_cb[4].bind(); }
	auto portf_output_cb() { return m_output_cb[5].bind(); }
	auto portg_output_cb() { return m_output_cb[6].bind(); }
	auto porth_output_cb() { return m_output_cb[7].bind(); }

	// memory handlers
	DECLARE_READ8_MEMBER(read);
	DECLARE_WRITE8_MEMBER(write);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	// input/output callbacks
	devcb_read8         m_input_cb[8];
	devcb_write8        m_output_cb[8];

	// internal state
	u8                  m_data_latch[8];
	u8                  m_data_dir[8];
};

// device type definition
DECLARE_DEVICE_TYPE(VS9209, vs9209_device)

#endif // MAME_MACHINE_VS9209_H
