// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/xdf_dsk.h

    x68k bare-bones formats

*********************************************************************/
#ifndef MAME_FORMATS_XDF_DSK_H
#define MAME_FORMATS_XDF_DSK_H

#pragma once

#include "upd765_dsk.h"

class xdf_format : public upd765_format
{
public:
	xdf_format();

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_XDF_FORMAT;

#endif // MAME_FORMATS_XDF_DSK_H
