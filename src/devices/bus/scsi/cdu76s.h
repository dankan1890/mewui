// license:BSD-3-Clause
// copyright-holders:smf
/***************************************************************************

    cdu76s.h

    Sony CDU-76S

***************************************************************************/

#pragma once

#ifndef __CDU76S_H__
#define __CDU76S_H__

#include "scsicd.h"
#include "machine/t10mmc.h"

class sony_cdu76s_device : public scsicd_device
{
public:
	sony_cdu76s_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void ExecCommand() override;
	virtual void ReadData( uint8_t *data, int dataLength ) override;
};

// device type definition
extern const device_type CDU76S;

#endif
