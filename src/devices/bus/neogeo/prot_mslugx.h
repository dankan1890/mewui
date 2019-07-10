// license:BSD-3-Clause
// copyright-holders:S. Smith,David Haywood,Fabio Priuli

#ifndef MAME_BUS_NEOGEO_PROT_MSLUGX_H
#define MAME_BUS_NEOGEO_PROT_MSLUGX_H

#pragma once


DECLARE_DEVICE_TYPE(NG_MSLUGX_PROT, mslugx_prot_device)


class mslugx_prot_device :  public device_t
{
public:
	// construction/destruction
	mslugx_prot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	DECLARE_WRITE16_MEMBER( protection_w );
	DECLARE_READ16_MEMBER( protection_r );

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	uint16_t     m_counter;
	uint16_t     m_command;
};

#endif // MAME_BUS_NEOGEO_PROT_MSLUGX_H
