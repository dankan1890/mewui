// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/******************************************************************************

    V-Tech V.Smile console emulation
    V-Tech V.Smile Baby console emulation

    Similar Systems:

        V.Smile Pocket
        V.Smile Cyber Pocket
        V.Smile PC Pal
        V-Motion Active Learning System
        V.Flash
        V.Baby
        Leapfrog Leapster

*******************************************************************************/

#ifndef MAME_INCLUDES_VSMILE_H
#define MAME_INCLUDES_VSMILE_H

#include "bus/vsmile/vsmile_ctrl.h"
#include "bus/vsmile/vsmile_slot.h"
#include "bus/vsmile/rom.h"

#include "cpu/unsp/unsp.h"

#include "machine/bankdev.h"
#include "machine/spg2xx.h"

#include "screen.h"

class vsmile_base_state : public driver_device
{
public:
	vsmile_base_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_screen(*this, "screen")
		, m_bankdev(*this, "bank")
		, m_cart(*this, "cartslot")
		, m_system_region(*this, "maincpu")
	{ }

	void vsmile_base(machine_config &config);

protected:
	virtual void machine_start() override;

	void mem_map(address_map &map);

	DECLARE_WRITE8_MEMBER(chip_sel_w);

	DECLARE_READ16_MEMBER(bank3_r);

	required_device<spg2xx_device> m_maincpu;
	required_device<screen_device> m_screen;
	required_device<address_map_bank_device> m_bankdev;
	required_device<vsmile_cart_slot_device> m_cart;
	required_memory_region m_system_region;
};

class vsmile_state : public vsmile_base_state
{
public:
	vsmile_state(const machine_config &mconfig, device_type type, const char *tag)
		: vsmile_base_state(mconfig, type, tag)
		, m_ctrl(*this, "ctrl%u", 1U)
		, m_dsw_region(*this, "REGION")
	{ }

	void vsmile(machine_config &config);
	void vsmilep(machine_config &config);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void banked_map(address_map &map);

	DECLARE_WRITE8_MEMBER(ctrl_tx_w);
	template <int Which> DECLARE_WRITE_LINE_MEMBER(ctrl_rts_w);

	DECLARE_READ16_MEMBER(portb_r);
	DECLARE_WRITE16_MEMBER(portb_w);
	DECLARE_READ16_MEMBER(portc_r);
	DECLARE_WRITE16_MEMBER(portc_w);

	DECLARE_WRITE8_MEMBER(uart_rx);

	enum
	{
		VSMILE_PORTB_CS1 =      0x01,
		VSMILE_PORTB_CS2 =      0x02,
		VSMILE_PORTB_CART =     0x04,
		VSMILE_PORTB_RESET =    0x08,
		VSMILE_PORTB_FRONT24 =  0x10,
		VSMILE_PORTB_OFF =      0x20,
		VSMILE_PORTB_OFF_SW =   0x40,
		VSMILE_PORTB_ON_SW =    0x80,

		VSMILE_PORTC_VER =      0x0f,
		VSMILE_PORTC_LOGO =     0x10,
		VSMILE_PORTC_TEST =     0x20,
		VSMILE_PORTC_AMP =      0x40,
		VSMILE_PORTC_SYSRESET = 0x80,
	};

	required_device_array<vsmile_ctrl_port_device, 2> m_ctrl;
	required_ioport m_dsw_region;

	bool m_ctrl_rts[2];
	bool m_ctrl_select[2];
};

class vsmilem_state : public vsmile_state
{
public:
	vsmilem_state(const machine_config &mconfig, device_type type, const char *tag)
		: vsmile_state(mconfig, type, tag)
	{ }

	void vsmilem(machine_config &config);

protected:
	DECLARE_WRITE16_MEMBER(porta_w);
	DECLARE_READ16_MEMBER(porta_r);
};

class vsmileb_state : public vsmile_base_state
{
public:
	vsmileb_state(const machine_config &mconfig, device_type type, const char *tag)
		: vsmile_base_state(mconfig, type, tag)
		, m_io_logo(*this, "LOGO")
	{ }

	void vsmileb(machine_config &config);
	void vsmilebp(machine_config &config);

	enum : uint16_t
	{
		BUTTON_YELLOW   = 0x01fe,
		BUTTON_BLUE     = 0x03ee,
		BUTTON_ORANGE   = 0x03de,
		BUTTON_GREEN    = 0x03be,
		BUTTON_RED      = 0x02fe,
		BUTTON_CLOUD    = 0x03f6,
		BUTTON_BALL     = 0x03fa,
		BUTTON_EXIT     = 0x03fc
	};

	DECLARE_INPUT_CHANGED_MEMBER(pad_button_changed);

	// make slide switches usable on a keyboard
	template <uint16_t V> DECLARE_INPUT_CHANGED_MEMBER(sw_mode);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void banked_map(address_map &map);

	DECLARE_READ16_MEMBER(porta_r);
	DECLARE_READ16_MEMBER(portb_r);

	required_ioport m_io_logo;

	uint16_t m_mode;
};

#endif // MAME_INCLUDES_VSMILE_H
