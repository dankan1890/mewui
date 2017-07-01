// license:BSD-3-Clause
// copyright-holders:Nathan Woods
/*********************************************************************

    cococart.h

    CoCo/Dragon cartridge management

*********************************************************************/

#ifndef __COCOCART_H__
#define __COCOCART_H__

#include "softlist_dev.h"


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

// ======================> cococart_base_update_delegate

// direct region update handler
typedef delegate<void (uint8_t *)> cococart_base_update_delegate;

#define MCFG_COCO_CARTRIDGE_CART_CB(_devcb) \
	devcb = &cococart_slot_device::static_set_cart_callback(*device, DEVCB_##_devcb);

#define MCFG_COCO_CARTRIDGE_NMI_CB(_devcb) \
	devcb = &cococart_slot_device::static_set_nmi_callback(*device, DEVCB_##_devcb);

#define MCFG_COCO_CARTRIDGE_HALT_CB(_devcb) \
	devcb = &cococart_slot_device::static_set_halt_callback(*device, DEVCB_##_devcb);


// ======================> cococart_slot_device
class device_cococart_interface;

class cococart_slot_device : public device_t,
								public device_slot_interface,
								public device_image_interface
{
public:
	// output lines on the CoCo cartridge slot
	enum class line
	{
		CART,             // connects to PIA1 CB1
		NMI,              // connects to NMI line on CPU
		HALT,             // connects to HALT line on CPU
		SOUND_ENABLE      // sound enable
	};

	// since we have a special value "Q" - we have to use a special enum here
	enum class line_value
	{
		CLEAR,
		ASSERT,
		Q
	};

	// construction/destruction
	cococart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template<class _Object> static devcb_base &static_set_cart_callback(device_t &device, _Object object)  { return downcast<cococart_slot_device &>(device).m_cart_callback.set_callback(object); }
	template<class _Object> static devcb_base &static_set_nmi_callback(device_t &device, _Object object)  { return downcast<cococart_slot_device &>(device).m_nmi_callback.set_callback(object); }
	template<class _Object> static devcb_base &static_set_halt_callback(device_t &device, _Object object)  { return downcast<cococart_slot_device &>(device).m_halt_callback.set_callback(object); }

	// device-level overrides
	virtual void device_start() override;
	virtual void device_config_complete() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// image-level overrides
	virtual image_init_result call_load() override;
	virtual const software_list_loader &get_software_list_loader() const override { return rom_software_list_loader::instance(); }

	virtual iodevice_t image_type() const override { return IO_CARTSLOT; }

	virtual bool is_readable()  const override { return 1; }
	virtual bool is_writeable() const override { return 0; }
	virtual bool is_creatable() const override { return 0; }
	virtual bool must_be_loaded() const override { return 0; }
	virtual bool is_reset_on_load() const override { return 1; }
	virtual const char *image_interface() const override { return "coco_cart"; }
	virtual const char *file_extensions() const override { return "ccc,rom"; }

	// slot interface overrides
	virtual std::string get_default_card_software() override;

	// reading and writing to $FF40-$FF7F
	DECLARE_READ8_MEMBER(read);
	DECLARE_WRITE8_MEMBER(write);

	// sets a cartridge line
	void cart_set_line(line line, line_value value);

	// hack to support twiddling the Q line
	void twiddle_q_lines();

	// cart base
	uint8_t* get_cart_base();
	void set_cart_base_update(cococart_base_update_delegate update);

private:
	// TIMER_POOL: Must be power of two
	static constexpr int TIMER_POOL = 2;

	enum
	{
		TIMER_CART,
		TIMER_NMI,
		TIMER_HALT
	};

	struct coco_cartridge_line
	{
		emu_timer                   *timer[TIMER_POOL];
		int                         timer_index;
		int                         delay;
		line_value                  value;
		int                         line;
		int                         q_count;
		devcb_write_line *          callback;
	};

	// configuration
	coco_cartridge_line         m_cart_line;
	coco_cartridge_line         m_nmi_line;
	coco_cartridge_line         m_halt_line;
public:
	devcb_write_line        m_cart_callback;
	devcb_write_line            m_nmi_callback;
	devcb_write_line            m_halt_callback;
private:
	// cartridge
	device_cococart_interface   *m_cart;

	// methods
	void set_line(const char *line_name, coco_cartridge_line &line, line_value value);
	void set_line_timer(coco_cartridge_line &line, line_value value);
	void twiddle_line_if_q(coco_cartridge_line &line);
	static const char *line_value_string(line_value value);
};

// device type definition
extern const device_type COCOCART_SLOT;

// ======================> device_cococart_interface

class device_cococart_interface : public device_slot_card_interface
{
public:
	// construction/destruction
	device_cococart_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_cococart_interface();

	virtual DECLARE_READ8_MEMBER(read);
	virtual DECLARE_WRITE8_MEMBER(write);

	virtual uint8_t* get_cart_base();
	void set_cart_base_update(cococart_base_update_delegate update);

protected:
	void cart_base_changed(void);

private:
	cococart_base_update_delegate m_update;
};

/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define MCFG_COCO_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot) \
	MCFG_DEVICE_ADD(_tag, COCOCART_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, false)

#define MCFG_COCO_CARTRIDGE_REMOVE(_tag)        \
	MCFG_DEVICE_REMOVE(_tag)

#endif // __COCOCART_H__
