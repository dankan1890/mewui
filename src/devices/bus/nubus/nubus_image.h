// license:BSD-3-Clause
// copyright-holders:R. Belmont
#pragma once

#ifndef __NUBUS_IMAGE_H__
#define __NUBUS_IMAGE_H__

#include "emu.h"
#include "nubus.h"
#include "osdcore.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************
class messimg_disk_image_device;

struct nbfilectx {
	uint32_t curcmd;
	uint8_t filename[128];
	uint8_t curdir[1024];
		osd::directory::ptr dirp;
	osd_file::ptr fd;
	uint64_t filelen;
	uint32_t bytecount;
};

// ======================> nubus_image_device

class nubus_image_device :
		public device_t,
		public device_nubus_card_interface
{
public:
		// construction/destruction
		nubus_image_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
		nubus_image_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const override;
		virtual const tiny_rom_entry *device_rom_region() const override;

protected:
		// device-level overrides
		virtual void device_start() override;
		virtual void device_reset() override;

		DECLARE_READ32_MEMBER(image_status_r);
		DECLARE_WRITE32_MEMBER(image_status_w);
		DECLARE_READ32_MEMBER(image_r);
		DECLARE_WRITE32_MEMBER(image_w);
		DECLARE_READ32_MEMBER(image_super_r);
		DECLARE_WRITE32_MEMBER(image_super_w);
		DECLARE_READ32_MEMBER(file_cmd_r);
		DECLARE_WRITE32_MEMBER(file_cmd_w);
		DECLARE_READ32_MEMBER(file_data_r);
		DECLARE_WRITE32_MEMBER(file_data_w);
		DECLARE_READ32_MEMBER(file_len_r);
		DECLARE_WRITE32_MEMBER(file_len_w);
		DECLARE_READ32_MEMBER(file_name_r);
		DECLARE_WRITE32_MEMBER(file_name_w);

public:
	messimg_disk_image_device *m_image;
	struct nbfilectx filectx;
};


// device type definition
extern const device_type NUBUS_IMAGE;

enum {
	kFileCmdGetDir = 1,
	kFileCmdSetDir,
	kFileCmdGetFirstListing,
	kFileCmdGetNextListing,
	kFileCmdGetFile,
	kFileCmdPutFile
};

#endif  /* __NUBUS_IMAGE_H__ */
