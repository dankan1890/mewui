// license:BSD-3-Clause
// copyright-holders:R. Belmont
#ifndef MAME_MACHINE_ESQVFD_H
#define MAME_MACHINE_ESQVFD_H

#include <memory>
#include <tuple>


class esqvfd_device : public device_t {
public:
	DECLARE_WRITE8_MEMBER( write ) { write_char(data); }

	virtual void write_char(int data) = 0;
	virtual void update_display();
	virtual bool write_contents(std::ostream &o) { return false; }

	// why isn't the font just stored in this order?
	static uint32_t conv_segments(uint16_t segin) { return bitswap<15>(segin, 12, 11, 7, 6, 4, 10, 3, 14, 15, 0, 13, 9, 5, 1, 2); }

protected:
	class output_helper {
	public:
		typedef std::unique_ptr<output_helper> ptr;
		virtual ~output_helper() { }
		virtual void resolve() = 0;
		virtual int32_t set(unsigned n, int32_t value) = 0;
	};

	template <unsigned N> class output_helper_impl : public output_helper, protected output_manager::output_finder<void, N> {
	public:
		output_helper_impl(device_t &device) : output_manager::output_finder<void, N>(device, "vfd%u", 0U) { }
		virtual void resolve() override { output_manager::output_finder<void, N>::resolve(); }
		virtual int32_t set(unsigned n, int32_t value) override { return this->operator[](n) = value; }
	};

	typedef std::tuple<output_helper::ptr, int, int> dimensions_param;

	template <int R, int C> static dimensions_param make_dimensions(device_t &device) { return dimensions_param(std::make_unique<output_helper_impl<R * C> >(device), R, C); }

	esqvfd_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, dimensions_param &&dimensions);

	static constexpr uint8_t AT_NORMAL      = 0x00;
	static constexpr uint8_t AT_BOLD        = 0x01;
	static constexpr uint8_t AT_UNDERLINE   = 0x02;
	static constexpr uint8_t AT_BLINK       = 0x04;
	static constexpr uint8_t AT_BLINKED     = 0x80;   // set when character should be blinked off

	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	output_helper::ptr m_vfds;
	int m_cursx, m_cursy;
	int m_savedx, m_savedy;
	int const m_rows, m_cols;
	uint8_t m_curattr;
	uint8_t m_lastchar;
	uint8_t m_chars[2][40];
	uint8_t m_attrs[2][40];
	uint8_t m_dirty[2][40];
};

class esq1x22_device : public esqvfd_device {
public:
	esq1x22_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void write_char(int data) override;

protected:
	virtual void device_add_mconfig(machine_config &config) override;

private:
};

class esq2x40_device : public esqvfd_device {
public:
	esq2x40_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void write_char(int data) override;
	virtual bool write_contents(std::ostream &o) override;

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class esq2x40_sq1_device : public esqvfd_device {
public:
	esq2x40_sq1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void write_char(int data) override;

protected:
	virtual void device_add_mconfig(machine_config &config) override;

private:
	bool m_wait87shift, m_wait88shift;
};

DECLARE_DEVICE_TYPE(ESQ1X22,     esq1x22_device)
DECLARE_DEVICE_TYPE(ESQ2X40,     esq2x40_device)
DECLARE_DEVICE_TYPE(ESQ2X40_SQ1, esq2x40_sq1_device)

#endif // MAME_MACHINE_ESQVFD_H
