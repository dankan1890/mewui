// license:BSD-3-Clause
// copyright-holders:David Haywood, ElSemi
/* IGS 028 */



class igs028_device : public device_t
{
public:
	igs028_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint16_t* m_sharedprotram;

	void IGS028_handle(void);

protected:
	virtual void device_config_complete() override;
	virtual void device_validity_check(validity_checker &valid) const override;
	virtual void device_start() override;
	virtual void device_reset() override;

	uint32_t olds_prot_addr(uint16_t addr);
	uint32_t olds_read_reg(uint16_t addr);
	void olds_write_reg( uint16_t addr, uint32_t val );
	void IGS028_do_dma(uint16_t src, uint16_t dst, uint16_t size, uint16_t mode);
};



extern const device_type IGS028;
