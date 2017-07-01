// license:BSD-3-Clause
// copyright-holders:smf
/***************************************************************************

    atapicdr.h

    ATAPI CDROM

***************************************************************************/

#pragma once

#ifndef __ATAPICDR_H__
#define __ATAPICDR_H__

#include "atapihle.h"
#include "t10mmc.h"

class atapi_cdrom_device : public atapi_hle_device,
	public t10mmc
{
public:
	atapi_cdrom_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	atapi_cdrom_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock,const char *shortname, const char *source);

	uint16_t *identify_device_buffer() { return m_identify_buffer; }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual machine_config_constructor device_mconfig_additions() const override;

	virtual void perform_diagnostic() override;
	virtual void identify_packet_device() override;
	virtual void process_buffer() override;
	virtual void ExecCommand() override;
	bool m_media_change;
};

class atapi_fixed_cdrom_device : public atapi_cdrom_device
{
public:
	atapi_fixed_cdrom_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
protected:
	virtual void device_reset() override;
};

// device type definition
extern const device_type ATAPI_CDROM;
extern const device_type ATAPI_FIXED_CDROM;

#endif
