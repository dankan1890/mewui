// license:BSD-3-Clause
// copyright-holders:Sergey Svishchev
/*********************************************************************

    formats/dvk_mx_dsk.h

*********************************************************************/

#ifndef DVK_MX_DSK_H_
#define DVK_MX_DSK_H_

#pragma once

#include "flopimg.h"
#include "imageutl.h"

class dvk_mx_format : public floppy_image_format_t
{
public:
	dvk_mx_format();

	virtual int identify(io_generic *io, uint32_t form_factor) override;
	virtual bool load(io_generic *io, uint32_t form_factor, floppy_image *image) override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;

	static const desc_e dvk_mx_old_desc[];
	static const desc_e dvk_mx_new_desc[];

private:
	void find_size(io_generic *io, uint8_t &track_count, uint8_t &head_count, uint8_t &sector_count);
};

extern const floppy_format_type FLOPPY_DVK_MX_FORMAT;

#endif /* DVK_MX_DSK_H_ */
