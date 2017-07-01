// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
#ifndef __TELEPRINTER_H__
#define __TELEPRINTER_H__

#include "machine/terminal.h"

#define TELEPRINTER_WIDTH 80
#define TELEPRINTER_HEIGHT 50


/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/
#define TELEPRINTER_TAG "teleprinter"
#define TELEPRINTER_SCREEN_TAG "tty_screen"

#define MCFG_GENERIC_TELEPRINTER_KEYBOARD_CB(_devcb) \
	devcb = &generic_terminal_device::set_keyboard_callback(*device, DEVCB_##_devcb);

/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

class teleprinter_device : public generic_terminal_device
{
public:
	teleprinter_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	uint32_t tp_update(screen_device &device, bitmap_rgb32 &bitmap, const rectangle &cliprect);
protected:
	virtual void term_write(uint8_t data) override;
	virtual void device_reset() override;
	virtual machine_config_constructor device_mconfig_additions() const override;
private:
	void scroll_line();
	void write_char(uint8_t data);
	void clear();
};

extern const device_type TELEPRINTER;

#endif /* __TELEPRINTER_H__ */
