// license:BSD-3-Clause
// copyright-holders:Sandro Ronco
/***************************************************************************

        Philips PCD8544 LCD controller

***************************************************************************/

#pragma once

#ifndef __PCD8544_H__
#define __PCD8544_H__


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

typedef device_delegate<void (device_t &device, bitmap_ind16 &bitmap, const rectangle &cliprect , uint8_t *vram, int inv)> pcd8544_screen_update_delegate;
#define PCD8544_SCREEN_UPDATE(name) void name(device_t &device, bitmap_ind16 &bitmap, const rectangle &cliprect , uint8_t *vram, int inv)

#define MCFG_PCD8544_ADD( _tag ) \
	MCFG_DEVICE_ADD( _tag, PCD8544, 0 )

#define MCFG_PCD8544_SCREEN_UPDATE_CALLBACK(_class, _method) \
	pcd8544_device::static_set_screen_update_cb(*device, pcd8544_screen_update_delegate(&_class::_method, #_class "::" #_method, downcast<_class *>(owner)));


// ======================> pcd8544_device

class pcd8544_device :  public device_t
{
public:
	// construction/destruction
	pcd8544_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	static void static_set_screen_update_cb(device_t &device, pcd8544_screen_update_delegate _cb) { downcast<pcd8544_device &>(device).m_screen_update_cb = _cb; }

	// device interface
	DECLARE_WRITE_LINE_MEMBER(sdin_w);
	DECLARE_WRITE_LINE_MEMBER(sclk_w);
	DECLARE_WRITE_LINE_MEMBER(dc_w);
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	void exec_command(uint8_t cmd);
	void write_data(uint8_t data);

private:
	pcd8544_screen_update_delegate m_screen_update_cb;  // screen update callback
	int     m_sdin;
	int     m_sclk;
	int     m_dc;
	int     m_bits;
	uint8_t   m_mode;
	uint8_t   m_control;
	uint8_t   m_op_vol;
	uint8_t   m_bias;
	uint8_t   m_temp_coef;
	uint8_t   m_indata;
	uint8_t   m_addr_y;
	uint8_t   m_addr_x;
	uint8_t   m_vram[6*84];       // 4032 bit video ram
};

// device type definition
extern const device_type PCD8544;

#endif
