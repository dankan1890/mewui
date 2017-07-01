// license:BSD-3-Clause
// copyright-holders:smf
/***************************************************************************

    Dempa Shinbunsha Joystick

***************************************************************************/

#ifndef __CENTRONICS_DSJOY_H__
#define __CENTRONICS_DSJOY_H__

#pragma once

#include "ctronics.h"

class dempa_shinbunsha_joystick_device : public device_t,
	public device_centronics_peripheral_interface
{
public:
	// construction/destruction
	dempa_shinbunsha_joystick_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual ioport_constructor device_input_ports() const override;

	virtual DECLARE_WRITE_LINE_MEMBER( input_data0 ) override { if (state) m_data |= 0x01; else m_data &= ~0x01; update_perror(); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data1 ) override { if (state) m_data |= 0x02; else m_data &= ~0x02; update_perror(); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data2 ) override { if (state) m_data |= 0x04; else m_data &= ~0x04; update_perror(); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data3 ) override { if (state) m_data |= 0x08; else m_data &= ~0x08; update_perror(); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data4 ) override { if (state) m_data |= 0x10; else m_data &= ~0x10; update_perror(); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data5 ) override { if (state) m_data |= 0x20; else m_data &= ~0x20; update_perror(); }

private:
	required_ioport m_lptjoy;

	void update_perror();

	uint8_t m_data;
	int m_perror;
};

// device type definition
extern const device_type DEMPA_SHINBUNSHA_JOYSTICK;

#endif
