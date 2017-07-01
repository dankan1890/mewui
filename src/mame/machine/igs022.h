// license:BSD-3-Clause
// copyright-holders:David Haywood, ElSemi
/* IGS022 */


class igs022_device : public device_t
{
public:
	igs022_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint16_t* m_sharedprotram;
	void IGS022_handle_command();

protected:
	virtual void device_config_complete() override;
	virtual void device_validity_check(validity_checker &valid) const override;
	virtual void device_start() override;
	virtual void device_reset() override;

	uint32_t        m_kb_regs[0x100];

	void IGS022_do_dma(uint16_t src, uint16_t dst, uint16_t size, uint16_t mode);
	void IGS022_reset();

};



extern const device_type IGS022;
