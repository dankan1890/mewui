// license:BSD-3-Clause
// copyright-holders: Dirk Best
/*************************************************************************

    RAM device

    Provides a configurable amount of RAM to drivers

**************************************************************************/

#ifndef __RAM_H__
#define __RAM_H__


/***************************************************************************
    CONSTANTS
***************************************************************************/

#define RAM_TAG             "ram"


/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define MCFG_RAM_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, RAM, 0)

#define MCFG_RAM_REMOVE(_tag) \
	MCFG_DEVICE_REMOVE(_tag)

#define MCFG_RAM_MODIFY(_tag) \
	MCFG_DEVICE_MODIFY(_tag)    \
	ram_device::static_set_extra_options(*device, nullptr);

#define MCFG_RAM_DEFAULT_SIZE(_default_size) \
	ram_device::static_set_default_size(*device, _default_size);

#define MCFG_RAM_EXTRA_OPTIONS(_extra_options) \
	ram_device::static_set_extra_options(*device, _extra_options);

#define MCFG_RAM_DEFAULT_VALUE(_default_value) \
	ram_device::static_set_default_value(*device, _default_value);


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

class ram_device :  public device_t
{
public:
	// construction/destruction
	ram_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// accessors
	uint32_t size(void) const { return m_size; }
	uint32_t mask(void) const { return m_size - 1; }
	uint8_t *pointer(void) { return &m_pointer[0]; }
	static uint32_t parse_string(const char *s);
	uint32_t default_size(void) const;
	const char *extra_options(void) const { return m_extra_options; }

	// read/write
	uint8_t read(offs_t offset)               { return m_pointer[offset % m_size]; }
	void write(offs_t offset, uint8_t data)   { m_pointer[offset % m_size] = data; }

	// inline configuration helpers
	static void static_set_default_size(device_t &device, const char *default_size)     { downcast<ram_device &>(device).m_default_size = default_size; }
	static void static_set_extra_options(device_t &device, const char *extra_options)   { downcast<ram_device &>(device).m_extra_options = extra_options; }
	static void static_set_default_value(device_t &device, uint8_t default_value)         { downcast<ram_device &>(device).m_default_value = default_value; }

protected:
	virtual void device_start(void) override;
	virtual void device_validity_check(validity_checker &valid) const override;

private:
	// device state
	uint32_t m_size;
	std::vector<uint8_t> m_pointer;

	// device config
	const char *m_default_size;
	const char *m_extra_options;
	uint8_t m_default_value;
};


// device type definition
extern const device_type RAM;

// device iterator
typedef device_type_iterator<&device_creator<ram_device>, ram_device> ram_device_iterator;

#endif /* __RAM_H__ */
