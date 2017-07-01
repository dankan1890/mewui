// license:BSD-3-Clause
// copyright-holders:Sandro Ronco
#pragma once

#ifndef __KC_RAM_H__
#define __KC_RAM_H__

#include "emu.h"
#include "kc.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> kc_m011_device

class kc_m011_device :
		public device_t,
		public device_kcexp_interface
{
public:
	// construction/destruction
	kc_m011_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	kc_m011_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// kcexp_interface overrides
	virtual uint8_t module_id_r() override { return 0xf6; }
	virtual void control_w(uint8_t data) override;
	virtual void read(offs_t offset, uint8_t &data) override;
	virtual void write(offs_t offset, uint8_t data) override;
	virtual DECLARE_WRITE_LINE_MEMBER( mei_w ) override;

protected:
	kcexp_slot_device *m_slot;

	// internal state
	int     m_mei;          // module enable line
	uint8_t * m_ram;
	uint8_t   m_enabled;
	uint8_t   m_write_enabled;
	uint16_t  m_base;
	uint8_t   m_segment;

private:
	// internal helpers
	virtual uint32_t get_ram_size() const { return 0x10000; }
};


// ======================> kc_m022_device

class kc_m022_device :
		public kc_m011_device
{
public:
	// construction/destruction
	kc_m022_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// kcexp_interface overrides
	virtual uint8_t module_id_r() override { return 0xf4; }
	virtual void read(offs_t offset, uint8_t &ata) override;
	virtual void write(offs_t offset, uint8_t data) override;

private:
	// internal helpers
	virtual uint32_t get_ram_size() const override { return 0x4000; }
};


// ======================> kc_m032_device

class kc_m032_device :
		public kc_m011_device
{
public:
	// construction/destruction
	kc_m032_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_reset() override;

	// kcexp_interface overrides
	virtual uint8_t module_id_r() override { return 0x79; }
	virtual void control_w(uint8_t data) override;
	virtual void read(offs_t offset, uint8_t &data) override;
	virtual void write(offs_t offset, uint8_t data) override;

private:
	// internal helpers
	virtual uint32_t get_ram_size() const override { return 0x40000; }
};


// ======================> kc_m034_device

class kc_m034_device :
		public kc_m011_device
{
public:
	// construction/destruction
	kc_m034_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_reset() override;

	// kcexp_interface overrides
	virtual uint8_t module_id_r() override { return 0x7a; }
	virtual void control_w(uint8_t data) override;
	virtual void read(offs_t offset, uint8_t &data) override;
	virtual void write(offs_t offset, uint8_t data) override;

private:
	// internal helpers
	virtual uint32_t get_ram_size() const override { return 0x80000; }
};


// ======================> kc_m035_device

class kc_m035_device :
		public kc_m011_device
{
public:
	// construction/destruction
	kc_m035_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// kcexp_interface overrides
	virtual uint8_t module_id_r() override { return 0x7b; }
	virtual void control_w(uint8_t data) override;
	virtual void read(offs_t offset, uint8_t &data) override;
	virtual void write(offs_t offset, uint8_t data) override;

private:
	// internal helpers
	virtual uint32_t get_ram_size() const override { return 0x100000; }
};


// ======================> kc_m036_device

class kc_m036_device :
		public kc_m011_device
{
public:
	// construction/destruction
	kc_m036_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_reset() override;

	// kcexp_interface overrides
	virtual uint8_t module_id_r() override { return 0x78; }
	virtual void control_w(uint8_t data) override;
	virtual void read(offs_t offset, uint8_t &data) override;
	virtual void write(offs_t offset, uint8_t data) override;

private:
	// internal helpers
	virtual uint32_t get_ram_size() const override { return 0x20000; }
};

// device type definition
extern const device_type KC_M011;
extern const device_type KC_M022;
extern const device_type KC_M032;
extern const device_type KC_M034;
extern const device_type KC_M035;
extern const device_type KC_M036;

#endif  /* __KC_RAM_H__ */
