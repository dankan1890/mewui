// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Shugart SA1403D Winchester Disk Controller emulation

**********************************************************************/

#pragma once

#ifndef __SA1403D__
#define __SA1403D__

#include "scsihd.h"
#include "imagedev/harddriv.h"

class sa1403d_device  : public scsihd_device
{
public:
	// construction/destruction
	sa1403d_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual machine_config_constructor device_mconfig_additions() const override;
	virtual ioport_constructor device_input_ports() const override;

	virtual void ExecCommand() override;
	virtual void WriteData( uint8_t *data, int dataLength ) override;
};


// device type definition
extern const device_type SA1403D;

#endif
