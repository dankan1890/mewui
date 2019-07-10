// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

        BBC Micro Floppy Disc Controller slot emulation

**********************************************************************/

#ifndef MAME_BUS_BBC_FDC_FDC_H
#define MAME_BUS_BBC_FDC_FDC_H

#pragma once



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> bbc_fdc_slot_device

class device_bbc_fdc_interface;

class bbc_fdc_slot_device : public device_t, public device_slot_interface
{
public:
	// construction/destruction
	template <typename T>
	bbc_fdc_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock, T &&slot_options, const char *default_option)
		: bbc_fdc_slot_device(mconfig, tag, owner, clock)
	{
		option_reset();
		slot_options(*this);
		set_default_option(default_option);
		set_fixed(false);
	}

	bbc_fdc_slot_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock);

	// callbacks
	auto intrq_wr_callback() { return m_intrq_cb.bind(); }
	auto drq_wr_callback() { return m_drq_cb.bind(); }

	uint8_t read(offs_t offset);
	void write(offs_t offset, uint8_t data);

	DECLARE_WRITE_LINE_MEMBER( intrq_w ) { m_intrq_cb(state); }
	DECLARE_WRITE_LINE_MEMBER( drq_w) { m_drq_cb(state); }

protected:
	// device-level overrides
	virtual void device_validity_check(validity_checker &valid) const override;
	virtual void device_start() override;
	virtual void device_reset() override;

	device_bbc_fdc_interface *m_card;

private:
	devcb_write_line m_intrq_cb;
	devcb_write_line m_drq_cb;
};


// ======================> device_bbc_fdc_interface

class device_bbc_fdc_interface : public device_slot_card_interface
{
public:
	virtual uint8_t read(offs_t offset) { return 0xff; }
	virtual void write(offs_t offset, uint8_t data) { }

protected:
	device_bbc_fdc_interface(const machine_config &mconfig, device_t &device);

	bbc_fdc_slot_device *m_slot;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_FDC_SLOT, bbc_fdc_slot_device)

void bbc_fdc_devices(device_slot_interface &device);


#endif // MAME_BUS_BBC_FDC_FDC_H
