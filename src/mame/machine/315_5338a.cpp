// license: BSD-3-Clause
// copyright-holders: Dirk Best
/***************************************************************************

    Sega 315-5338A

    I/O Controller

***************************************************************************/

#include "emu.h"
#include "315_5338a.h"

//#define VERBOSE 1
#include "logmacro.h"


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(SEGA_315_5338A, sega_315_5338a_device, "315_5338a", "Sega 315-5338A I/O Controller")


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  sega_315_5338a_device - constructor
//-------------------------------------------------

sega_315_5338a_device::sega_315_5338a_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, SEGA_315_5338A, tag, owner, clock),
	m_read_cb(*this), m_write_cb(*this),
	m_in_port_cb{ {*this}, {*this}, {*this}, {*this}, {*this}, {*this}, {*this} },
	m_out_port_cb{ {*this}, {*this}, {*this}, {*this}, {*this}, {*this}, {*this} },
	m_port_config(0), m_serial_output(0), m_address(0)
{
	std::fill(std::begin(m_port_value), std::end(m_port_value), 0xff);
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void sega_315_5338a_device::device_start()
{
	// resolve callbacks
	m_read_cb.resolve_safe(0xff);
	m_write_cb.resolve_safe();

	for (unsigned i = 0; i < 7; i++)
	{
		m_in_port_cb[i].resolve_safe(0xff);
		m_out_port_cb[i].resolve_safe();
	}

	// register for save states
	save_pointer(NAME(m_port_value), 7);
	save_item(NAME(m_port_config));
	save_item(NAME(m_serial_output));
	save_item(NAME(m_address));
}


//**************************************************************************
//  INTERFACE
//**************************************************************************

READ8_MEMBER( sega_315_5338a_device::read )
{
	uint8_t data = 0xff;

	switch (offset)
	{
	// port 0 to 6
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
		if (BIT(m_port_config, offset))
			data = m_in_port_cb[offset](0);
		else
			data = m_port_value[offset];
		break;

	case 0x08: data = m_port_config; break;

	// serial data read back?
	case 0x0a: data = m_serial_output; break;

	// serial data input
	case 0x0c: data = m_read_cb(m_address); break;

	// status register
	case 0x0d:
		// 7654----  unknown
		// ----3---  transfer finished?
		// -----21-  unknown
		// -------0  command acknowledged (0 = ack)
		data = 0x08;
		break;
	}

	LOG("RD %02x = %02x\n", offset, data);

	return data;
}

WRITE8_MEMBER( sega_315_5338a_device::write )
{
	LOG("WR %02x = %02x\n", offset, data);

	switch (offset)
	{
	// port 0 to 6
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
		m_port_value[offset] = data;
		if (BIT(m_port_config, offset) == 0)
			m_out_port_cb[offset](data);
		break;

	// port direction register (0 = output, 1 = input)
	case 0x08:
		// handle ports that were previously set to input and are now output
		{
			uint8_t changed = data ^ m_port_config;
			for (unsigned i = 0; i < 7; i++)
				if (BIT(changed, i) && BIT(data, i) == 0)
					m_out_port_cb[i](m_port_value[i]);
		}

		m_port_config = data;
		break;

	// command register
	case 0x09:
		switch (data)
		{
		case 0x00:
			m_address = (m_address & 0xff00) | (m_serial_output << 0);
			break;
		case 0x01:
			m_address = (m_address & 0x00ff) | (m_serial_output << 8);
			break;
		case 0x07:
			m_write_cb(m_address, m_serial_output, 0xff);
			break;
		case 0x87:
			// sent after setting up the address and when wanting to receive serial data
			break;
		default:
			logerror("Unknown command: %02x\n", data);
		}
		break;

	// serial data output
	case 0x0a: m_serial_output = data; break;
	}
}
