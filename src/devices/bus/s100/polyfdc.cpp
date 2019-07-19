// license:BSD-3-Clause
// copyright-holders:AJR
/***************************************************************************

    PolyMorphic Systems Disk Controller

    This board controls up to three Shugart SA-400 drives using only
    generic serial and parallel interface chips and TTL (and an onboard
    4 MHz XTAL). No schematics for this board have been found.

****************************************************************************/

#include "emu.h"
#include "polyfdc.h"

#include "machine/i8255.h"
#include "machine/mc6852.h"

class poly_fdc_device : public device_t, public device_s100_card_interface
{
public:
	// construction/destruction
	poly_fdc_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	static constexpr feature_type unemulated_features() { return feature::DISK; }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;

	// device_s100_card_interface overrides
	virtual u8 s100_sinp_r(offs_t offset) override;
	virtual void s100_sout_w(offs_t offset, u8 data) override;

private:
	void pa_w(u8 data);
	u8 pb_r();
	void pc_w(u8 data);

	// object finders
	required_device<mc6852_device> m_ssda;
	required_device<i8255_device> m_ppi;
};

DEFINE_DEVICE_TYPE_PRIVATE(S100_POLY_FDC, device_s100_card_interface, poly_fdc_device, "polyfdc", "PolyMorphic Systems Disk Controller")

poly_fdc_device::poly_fdc_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, S100_POLY_FDC, tag, owner, clock)
	, device_s100_card_interface(mconfig, *this)
	, m_ssda(*this, "ssda")
	, m_ppi(*this, "ppi")
{
}

void poly_fdc_device::device_start()
{
}

u8 poly_fdc_device::s100_sinp_r(offs_t offset)
{
	if ((offset & 0xf000) == 0x2000)
	{
		if (BIT(offset, 11))
			return m_ppi->read(offset & 3);
		else
			return m_ssda->read(offset & 1);
	}

	return 0xff;
}

void poly_fdc_device::s100_sout_w(offs_t offset, u8 data)
{
	if ((offset & 0xf000) == 0x2000)
	{
		if (BIT(offset, 11))
			m_ppi->write(offset & 3, data);
		else
			m_ssda->write(offset & 1, data);
	}
}

void poly_fdc_device::pa_w(u8 data)
{
}

u8 poly_fdc_device::pb_r()
{
	return 0xff;
}

void poly_fdc_device::pc_w(u8 data)
{
	// Port B interrupt
	m_bus->vi5_w(BIT(data, 0) ? ASSERT_LINE : CLEAR_LINE);
}

void poly_fdc_device::device_add_mconfig(machine_config &config)
{
	MC6852(config, m_ssda, 4_MHz_XTAL / 4); // actual clock unknown

	I8255(config, m_ppi);
	m_ppi->out_pa_callback().set(FUNC(poly_fdc_device::pa_w));
	m_ppi->in_pb_callback().set(FUNC(poly_fdc_device::pb_r));
	m_ppi->out_pc_callback().set(FUNC(poly_fdc_device::pc_w));
	m_ppi->tri_pc_callback().set_constant(0xfe);
}
