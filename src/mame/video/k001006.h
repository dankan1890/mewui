// license:BSD-3-Clause
// copyright-holders:David Haywood
#ifndef MAME_VIDEO_K001006_H
#define MAME_VIDEO_K001006_H

#pragma once



class k001006_device : public device_t
{
public:
	k001006_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	~k001006_device() {}

	// configuration
	template <typename T> void set_gfx_region(T &&tag) { m_gfxrom.set_tag(std::forward<T>(tag)); }
	void set_tex_layout(int layout) { m_tex_layout = layout; }

	uint32_t fetch_texel(int page, int pal_index, int u, int v);
	void preprocess_texture_data(uint8_t *dst, uint8_t *src, int length, int gticlub);

	DECLARE_READ32_MEMBER( read );
	DECLARE_WRITE32_MEMBER( write );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	// internal state
	std::unique_ptr<uint16_t[]>      m_pal_ram;
	std::unique_ptr<uint16_t[]>     m_unknown_ram;
	uint32_t       m_addr;
	int          m_device_sel;

	std::unique_ptr<uint8_t[]>     m_texrom;

	std::unique_ptr<uint32_t[]>     m_palette;

	required_region_ptr<uint8_t> m_gfxrom;
	//int m_tex_width;
	//int m_tex_height;
	//int m_tex_mirror_x;
	//int m_tex_mirror_y;
	int m_tex_layout;
};


DECLARE_DEVICE_TYPE(K001006, k001006_device)

#endif // MAME_VIDEO_K001006_H
