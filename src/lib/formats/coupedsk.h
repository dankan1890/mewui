// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*************************************************************************

    formats/coupedsk.h

    SAM Coupe disk image formats

**************************************************************************/
#ifndef MAME_FORMATS_COUPEDSK_H
#define MAME_FORMATS_COUPEDSK_H

#pragma once

#include "flopimg.h"

class mgt_format : public floppy_image_format_t
{
public:
	mgt_format();

	virtual int identify(io_generic *io, uint32_t form_factor) override;
	virtual bool load(io_generic *io, uint32_t form_factor, floppy_image *image) override;
	virtual bool save(io_generic *io, floppy_image *image) override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;

	static const floppy_image_format_t::desc_e desc_10[];
};

extern const floppy_format_type FLOPPY_MGT_FORMAT;

#endif // MAME_FORMATS_COUPEDSK_H
