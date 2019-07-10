// license:BSD-3-Clause
// copyright-holders:Bryan McPhail
/***************************************************************************

    decrmc3.h

    Data East custom palette device.

****************************************************************************

    Data East RM-C3 is a custom resistor pack used to convert a digital
    palette to an analog output.  The conversion is non-linear with high bits
    weighted a little more than low bits compared to linear.

******************************************************************************/

#ifndef MAME_VIDEO_DECORMC3_H
#define MAME_VIDEO_DECORMC3_H

#pragma once


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// device type definition
DECLARE_DEVICE_TYPE(DECO_RMC3, deco_rmc3_device)

typedef device_delegate<void (deco_rmc3_device &)> deco_rmc3_palette_init_delegate;

class deco_rmc3_device : public device_t, public device_palette_interface
{
public:
	// construction/destruction
	deco_rmc3_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock, u32 entries)
		: deco_rmc3_device(mconfig, tag, owner, clock)
	{
		set_entries(entries);
	}

	deco_rmc3_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	// configuration
	void set_init(deco_rmc3_palette_init_delegate init) { m_init = init; }
	template <class FunctionClass> void set_init(const char *devname, void (FunctionClass::*callback)(deco_rmc3_device &), const char *name)
	{
		set_init(deco_rmc3_palette_init_delegate(callback, name, devname, static_cast<FunctionClass *>(nullptr)));
	}
//  void set_membits(int membits);
//  void set_endianness(endianness_t endianness);
	void set_entries(u32 entries) { m_entries = entries; }
	void set_indirect_entries(u32 entries) { m_indirect_entries = entries; }
	void set_prom_region(const char *region) { m_prom_region.set_tag(region); }
	template <typename T> void set_prom_region(T &&tag) { m_prom_region.set_tag(std::forward<T>(tag)); }

	// palette RAM accessors
	memory_array &basemem() { return m_paletteram; }
	memory_array &extmem() { return m_paletteram_ext; }

	// raw entry reading
	u32 read_entry(pen_t pen) const
	{
		u32 data = m_paletteram.read(pen);
		if (m_paletteram_ext.base() != nullptr)
			data |= m_paletteram_ext.read(pen) << (8 * m_paletteram.bytes_per_entry());
		return data;
	}

	// generic read/write handlers
	DECLARE_READ8_MEMBER(read8);
	DECLARE_WRITE8_MEMBER(write8);
	DECLARE_WRITE8_MEMBER(write8_ext);
	DECLARE_WRITE8_MEMBER(write_indirect);
	DECLARE_WRITE8_MEMBER(write_indirect_ext);
	DECLARE_READ16_MEMBER(read16);
	DECLARE_WRITE16_MEMBER(write16);
	DECLARE_WRITE16_MEMBER(write16_ext);
	DECLARE_READ32_MEMBER(read32);
	DECLARE_WRITE32_MEMBER(write32);

	void palette_init_proms(deco_rmc3_device &palette);

	// helper to update palette when data changed
	void update() { if (!m_init.isnull()) m_init(*this); }

protected:
	// device-level overrides
	virtual void device_start() override;

	// device_palette_interface overrides
	virtual u32 palette_entries() const override { return m_entries; }
	virtual u32 palette_indirect_entries() const override { return m_indirect_entries; }

private:
	void update_for_write(offs_t byte_offset, int bytes_modified, bool indirect = false);
	rgb_t deco_rgb_decoder(u32 raw);

	// configuration state
	u32                 m_entries;              // number of entries in the palette
	u32                 m_indirect_entries;     // number of indirect colors in the palette
//  bool                m_enable_shadows;       // are shadows enabled?
//  bool                m_enable_hilights;      // are hilights enabled?
//  int                 m_membits;              // width of palette RAM, if different from native
//  bool                m_membits_supplied;     // true if membits forced in static config
//  endianness_t        m_endianness;           // endianness of palette RAM, if different from native
//  bool                m_endianness_supplied;  // true if endianness forced in static config
	optional_memory_region m_prom_region;       // region where the color PROMs are
	deco_rmc3_palette_init_delegate m_init;

	// palette RAM
	memory_array        m_paletteram;           // base memory
	memory_array        m_paletteram_ext;       // extended memory
};

#endif  // MAME_VIDEO_DECORMC3_H
