// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/***************************************************************************

    Sinclair ZX Spectrum

    Opus Discovery disk image formats

***************************************************************************/

#include "opd_dsk.h"


opd_format::opd_format() : wd177x_format(formats)
{
}

const char *opd_format::name() const
{
	return "opd";
}

const char *opd_format::description() const
{
	return "Opus Discovery disk image";
}

const char *opd_format::extensions() const
{
	return "opd,opu";
}

int opd_format::identify(io_generic *io, uint32_t form_factor)
{
	int type = find_size(io, form_factor);

	if (type != -1)
		return 90;
	return 0;
}

int opd_format::get_image_offset(const format &f, int head, int track)
{
	return (f.track_count * head + track) * compute_track_size(f);
}

const opd_format::format opd_format::formats[] =
{
	{ // 180k 40 track single sided double density - gaps unverified
		floppy_image::FF_35, floppy_image::SSSD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, 0, {}, 36, 22, 27
	},
	{ // 360k 40 track double sided double density - gaps unverified
		floppy_image::FF_35, floppy_image::DSSD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, 0, {}, 36, 22, 27
	},
	{}
};


const floppy_format_type FLOPPY_OPD_FORMAT = &floppy_image_format_creator<opd_format>;
