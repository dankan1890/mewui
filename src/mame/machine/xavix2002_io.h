// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_MACHINE_XAVIX2002_IO_H
#define MAME_MACHINE_XAVIX2002_IO_H


class xavix2002_io_device : public device_t
{
public:
	xavix2002_io_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto read_0_callback() { return m_in0_cb.bind(); }
	auto read_1_callback() { return m_in1_cb.bind(); }
	auto read_2_callback() { return m_in2_cb.bind(); }

	auto write_0_callback() { return m_out0_cb.bind(); }
	auto write_1_callback() { return m_out1_cb.bind(); }
	auto write_2_callback() { return m_out2_cb.bind(); }

	DECLARE_WRITE8_MEMBER(pio_dir_w);
	DECLARE_READ8_MEMBER(pio_dir_r);

	DECLARE_WRITE8_MEMBER(pio_out_w);
	DECLARE_READ8_MEMBER(pio_out_r);

	DECLARE_READ8_MEMBER(pio_in_r);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	devcb_read8 m_in0_cb;
	devcb_read8 m_in1_cb;
	devcb_read8 m_in2_cb;

	devcb_write8 m_out0_cb;
	devcb_write8 m_out1_cb;
	devcb_write8 m_out2_cb;

	uint8_t m_sx_pio_dir[3];
	uint8_t m_sx_pio_out[3];
};

DECLARE_DEVICE_TYPE(XAVIX2002IO, xavix2002_io_device)

#endif // MAME_MACHINE_XAVIX2002_IO_H
