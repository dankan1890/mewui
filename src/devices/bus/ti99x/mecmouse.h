// license:LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    TI-99 Mechatronic mouse with adapter
    See mecmouse.c for documentation

    Michael Zapf
    October 2010
    January 2012: rewritten as class

*****************************************************************************/

#ifndef __MECMOUSE__
#define __MECMOUSE__

#include "emu.h"
#include "joyport.h"

extern const device_type MECMOUSE;

class mecmouse_device : public joyport_attached_device
{
public:
	mecmouse_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint8_t read_dev() override;
	void  write_dev(uint8_t data) override;

protected:
	virtual void device_start(void) override;
	virtual void device_reset(void) override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	int     m_last_select;
	bool    m_read_y_axis;
	int     m_x;
	int     m_y;
	int     m_x_buf;
	int     m_y_buf;
	int     m_last_mx;
	int     m_last_my;

	emu_timer   *m_poll_timer;
};
#endif
