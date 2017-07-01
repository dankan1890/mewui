// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/***************************************************************************

    dispatch.h

    Signal dispatching devices.

***************************************************************************/

#pragma once

#ifndef __EMU_H__
#error Dont include this file directly; include emu.h instead.
#endif

#ifndef MAME_EMU_DISPATCH_H
#define MAME_EMU_DISPATCH_H

#define MCFG_LINE_DISPATCH_ADD(_tag, _count) \
	MCFG_DEVICE_ADD(_tag, DEVCB_LINE_DISPATCH_ ## _count, 0)

#define MCFG_LINE_DISPATCH_FWD_CB(_entry, _count, _devcb) \
	devcb = &devcb_line_dispatch_device<_count>::set_fwd_cb(*device, _entry, DEVCB_##_devcb);

extern const device_type DEVCB_LINE_DISPATCH_2;
extern const device_type DEVCB_LINE_DISPATCH_3;
extern const device_type DEVCB_LINE_DISPATCH_4;
extern const device_type DEVCB_LINE_DISPATCH_5;
extern const device_type DEVCB_LINE_DISPATCH_6;

template<int N> class devcb_line_dispatch_device : public device_t {
public:
	devcb_line_dispatch_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
		device_t(mconfig, DEVCB_LINE_DISPATCH_2, "DEVCB_LINE_DISPATCH_2", tag, owner, clock, "devcb_line_dispatch_2", __FILE__) { }

	void init_fwd() {
		for(auto & elem : fwd_cb)
			elem = new devcb_write_line(*this);
	}

	virtual ~devcb_line_dispatch_device() {
		for(auto & elem : fwd_cb)
			delete elem;
	}

	template<class _Object> static devcb_base &set_fwd_cb(device_t &device, int entry, _Object object) { return downcast<devcb_line_dispatch_device<N> &>(device).fwd_cb[entry]->set_callback(object); }

	WRITE_LINE_MEMBER( in_w ) {
		for(auto & elem : fwd_cb)
			(*(elem))(state);
	}

protected:
	virtual void device_start() override {
		for(auto & elem : fwd_cb)
			elem->resolve_safe();
	}

private:
	devcb_write_line *fwd_cb[N];
};

#endif // MAME_EMU_DISPATCH_H
