// license:BSD-3-Clause
// copyright-holders:S. Smith,David Haywood,Fabio Priuli


#pragma once

#ifndef __KOF2K2_PROT__
#define __KOF2K2_PROT__

extern const device_type KOF2002_PROT;

#define MCFG_KOF2002_PROT_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, KOF2002_PROT, 0)


class kof2002_prot_device :  public device_t
{
public:
	// construction/destruction
	kof2002_prot_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void kof2002_decrypt_68k(uint8_t* cpurom, uint32_t cpurom_size);
	void matrim_decrypt_68k(uint8_t* cpurom, uint32_t cpurom_size);
	void samsho5_decrypt_68k(uint8_t* cpurom, uint32_t cpurom_size);
	void samsh5sp_decrypt_68k(uint8_t* cpurom, uint32_t cpurom_size);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
};

#endif
