// license:BSD-3-Clause
// copyright-holders:Tomasz Slanina
#include "sound/dac.h"
#include <algorithm>

#define RLT_NUM_BLITTER_REGS    8
#define RLT_NUM_BITMAPS         8

class rltennis_state : public driver_device
{
public:
	rltennis_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_dac(*this, "dac%u", 1U),
		m_samples(*this, "samples%u", 1U),
		m_gfx(*this, "gfx"),
		m_data760000(0), m_data740000(0), m_dac_counter(0),
		m_offset_shift(0)
	{
		std::fill(std::begin(m_sample_rom_offset), std::end(m_sample_rom_offset), 0);
	}

	void rltennis(machine_config &config);

private:
	required_device<cpu_device> m_maincpu;
	required_device_array<dac_byte_interface, 2> m_dac;
	required_region_ptr_array<uint8_t, 2> m_samples;
	required_region_ptr<uint8_t> m_gfx;

	uint16_t m_blitter[RLT_NUM_BLITTER_REGS];
	int32_t m_data760000;
	int32_t m_data740000;
	int32_t m_dac_counter;
	int32_t m_sample_rom_offset[2];
	int32_t m_offset_shift;
	int32_t m_unk_counter;
	std::unique_ptr<bitmap_ind16> m_tmp_bitmap[RLT_NUM_BITMAPS];
	emu_timer *m_timer;

	DECLARE_READ16_MEMBER(io_r);
	DECLARE_WRITE16_MEMBER(snd1_w);
	DECLARE_WRITE16_MEMBER(snd2_w);
	DECLARE_WRITE16_MEMBER(blitter_w);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	INTERRUPT_GEN_MEMBER(interrupt);
	TIMER_CALLBACK_MEMBER(sample_player);
	void ramdac_map(address_map &map);
	void rltennis_main(address_map &map);
};
