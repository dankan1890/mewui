// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    midioutport.h

    MIDI Out port - glues the image device to the pluggable midi port

*********************************************************************/

#ifndef _MIDIOUTPORT_H_
#define _MIDIOUTPORT_H_

#include "midi.h"
#include "imagedev/midiout.h"

class midiout_port_device : public device_t,
	public device_midi_port_interface
{
public:
	midiout_port_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual machine_config_constructor device_mconfig_additions() const override;

	virtual DECLARE_WRITE_LINE_MEMBER( input_txd ) override { if (started()) m_midiout->tx(state); }

protected:
	virtual void device_start() override { }
	virtual void device_reset() override { }

private:
	required_device<midiout_device> m_midiout;
};

extern const device_type MIDIOUT_PORT;

#endif
