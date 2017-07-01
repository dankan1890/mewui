// license:LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    TI-99 SuperAMS memory expansion
    See samsmem.c for documentation

    Michael Zapf
    September 2010

    February 2012: Rewritten as class

*****************************************************************************/

#ifndef __SAMSMEM__
#define __SAMSMEM__

#include "emu.h"
#include "peribox.h"
#include "machine/ram.h"

extern const device_type TI99_SAMSMEM;

class sams_memory_expansion_device : public ti_expansion_card_device
{
public:
	sams_memory_expansion_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	DECLARE_READ8Z_MEMBER(readz) override;
	DECLARE_WRITE8_MEMBER(write) override;

	DECLARE_READ8Z_MEMBER(crureadz) override;
	DECLARE_WRITE8_MEMBER(cruwrite) override;

	machine_config_constructor device_mconfig_additions() const override;

protected:
	void device_start(void) override;
	void device_reset(void) override;

private:
	// Console RAM
	required_device<ram_device> m_ram;
	int     m_mapper[16];
	bool    m_map_mode;
	bool    m_access_mapper;
};
#endif
