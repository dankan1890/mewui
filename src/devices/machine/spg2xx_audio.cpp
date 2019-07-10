// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
/*****************************************************************************

    SunPlus SPG2xx-series SoC peripheral emulation (Audio)

    This is also used for SPG110, although that should be limited to
    just 8 channels and has some things shifted around (phase appears
    to be in the regular register set instead, formats might be fixed
    or at least not per-channel)

    SPG110 Beat interrupt frequency might be different too, seems to
    trigger an FIQ, but music is very slow in jak_spdmo

    GCM394 has 32 channels, and potentially a different register layout
    it looks close but might be different enough to split off

**********************************************************************/

#include "emu.h"
#include "spg2xx_audio.h"

DEFINE_DEVICE_TYPE(SPG2XX_AUDIO, spg2xx_audio_device, "spg2xx_audio", "SPG2xx-series System-on-a-Chip Audio")
DEFINE_DEVICE_TYPE(SPG110_AUDIO, spg110_audio_device, "spg110_audio", "SPG110-series System-on-a-Chip Audio")
DEFINE_DEVICE_TYPE(SUNPLUS_GCM394_AUDIO, sunplus_gcm394_audio_device, "gcm394_audio", "SunPlus GCM394 System-on-a-Chip (Audio)")

#define LOG_SPU_READS       (1U << 0)
#define LOG_SPU_WRITES      (1U << 1)
#define LOG_UNKNOWN_SPU     (1U << 2)
#define LOG_CHANNEL_READS   (1U << 3)
#define LOG_CHANNEL_WRITES  (1U << 4)
#define LOG_ENVELOPES       (1U << 5)
#define LOG_SAMPLES         (1U << 6)
#define LOG_RAMPDOWN        (1U << 7)
#define LOG_BEAT            (1U << 8)

#define LOG_ALL             (LOG_SPU_READS | LOG_SPU_WRITES | LOG_UNKNOWN_SPU | LOG_CHANNEL_READS | LOG_CHANNEL_WRITES \
							| LOG_ENVELOPES | LOG_SAMPLES | LOG_RAMPDOWN | LOG_BEAT)

#define VERBOSE             (0)
#include "logmacro.h"

#define SPG_DEBUG_AUDIO     (0)


spg2xx_audio_device::spg2xx_audio_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, type, tag, owner, clock)
	, device_sound_interface(mconfig, *this)
	, m_space_read_cb(*this)
	, m_irq_cb(*this)
{
}

spg2xx_audio_device::spg2xx_audio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: spg2xx_audio_device(mconfig, SPG2XX_AUDIO, tag, owner, clock)
{
}

spg110_audio_device::spg110_audio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: spg2xx_audio_device(mconfig, SPG110_AUDIO, tag, owner, clock)
{
}

sunplus_gcm394_audio_device::sunplus_gcm394_audio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: spg2xx_audio_device(mconfig, SUNPLUS_GCM394_AUDIO, tag, owner, clock)
{
}


void spg2xx_audio_device::device_start()
{
	m_audio_beat = timer_alloc(TIMER_BEAT);
	m_audio_beat->adjust(attotime::never);

	m_stream = stream_alloc(0, 2, 281250/4);

	m_channel_debug = -1;

	save_item(NAME(m_debug_samples));
	save_item(NAME(m_debug_rates));

	save_item(NAME(m_audio_regs));
	save_item(NAME(m_audio_phase_regs));
	save_item(NAME(m_audio_ctrl_regs));

	save_item(NAME(m_sample_shift));
	save_item(NAME(m_sample_count));
	save_item(NAME(m_sample_addr));
	save_item(NAME(m_channel_rate));
	save_item(NAME(m_channel_rate_accum));
	save_item(NAME(m_rampdown_frame));
	save_item(NAME(m_envclk_frame));
	save_item(NAME(m_envelope_addr));
	save_item(NAME(m_channel_debug));
	save_item(NAME(m_audio_curr_beat_base_count));


	for (int i = 0; i < 16; i++)
	{
		save_item(NAME(m_adpcm[i].m_signal), i);
		save_item(NAME(m_adpcm[i].m_step), i);
	}

	m_space_read_cb.resolve_safe(0);
	m_irq_cb.resolve();
}

void spg2xx_audio_device::device_reset()
{
	memset(m_audio_regs, 0, 0x200 * sizeof(uint16_t));
	memset(m_audio_phase_regs, 0, 0x200 * sizeof(uint16_t));
	memset(m_audio_ctrl_regs, 0, 0x400 * sizeof(uint16_t));

	memset(m_sample_shift, 0, 16);
	memset(m_sample_count, 0, sizeof(uint32_t) * 16);
	memset(m_sample_addr, 0, sizeof(uint32_t) * 16);
	memset(m_channel_rate, 0, sizeof(double) * 16);
	memset(m_channel_rate_accum, 0, sizeof(double) * 16);
	memset(m_rampdown_frame, 0, sizeof(uint32_t) * 16);
	memset(m_envclk_frame, 4, sizeof(uint32_t) * 16);
	memset(m_envelope_addr, 0, sizeof(uint32_t) * 16);

	m_debug_samples = false;
	m_debug_rates = false;
	m_audio_curr_beat_base_count = 0;

	m_audio_ctrl_regs[AUDIO_CHANNEL_REPEAT] = 0x3f;
	m_audio_ctrl_regs[AUDIO_CHANNEL_ENV_MODE] = 0x3f;

	m_audio_beat->adjust(attotime::from_ticks(4, 281250), 0, attotime::from_ticks(4, 281250));
}


void spg2xx_audio_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_BEAT:
		audio_beat_tick();
		break;
	}
}

/***********************
*    Audio Hardware    *
***********************/
void spg2xx_audio_device::check_irqs(const uint16_t changed)
{
	if (changed & (AUDIO_BIS_MASK | AUDIO_BIE_MASK)) // Beat IRQ
	{
		if ((m_audio_ctrl_regs[AUDIO_BEAT_COUNT] & (AUDIO_BIS_MASK | AUDIO_BIE_MASK)) == (AUDIO_BIS_MASK | AUDIO_BIE_MASK))
		{
			LOGMASKED(LOG_BEAT, "Asserting beat IRQ\n");
			m_irq_cb(true);
		}
		else
		{
			LOGMASKED(LOG_BEAT, "Clearing beat IRQ\n");
			m_irq_cb(false);
		}
	}
}

READ16_MEMBER(spg2xx_audio_device::audio_ctrl_r)
{
	uint16_t data = m_audio_ctrl_regs[offset];

	switch (offset)
	{
	case AUDIO_CHANNEL_ENABLE:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel Enable: %04x\n", data);
		break;

	case AUDIO_MAIN_VOLUME:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Main Volume: %04x\n", data);
		break;

	case AUDIO_CHANNEL_FIQ_ENABLE:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel FIQ Enable: %04x\n", data);
		break;

	case AUDIO_CHANNEL_FIQ_STATUS:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel FIQ Acknowledge: %04x\n", data);
		break;

	case AUDIO_BEAT_BASE_COUNT:
		LOGMASKED(LOG_SPU_READS | LOG_BEAT, "audio_ctrl_r: Beat Base Count: %04x\n", data);
		break;

	case AUDIO_BEAT_COUNT:
		LOGMASKED(LOG_SPU_READS | LOG_BEAT, "audio_ctrl_r: Beat Count: %04x\n", data);
		break;

	case AUDIO_ENVCLK0:
	case AUDIO_ENVCLK1:
		LOGMASKED(LOG_SPU_READS | LOG_ENVELOPES, "audio_ctrl_r: Envelope Interval %d (lo): %04x\n", offset == AUDIO_ENVCLK0 ? 0 : 1, data);
		break;

	case AUDIO_ENVCLK0_HIGH:
	case AUDIO_ENVCLK1_HIGH:
		LOGMASKED(LOG_SPU_READS | LOG_ENVELOPES, "audio_ctrl_r: Envelope Interval %d (hi): %04x\n", offset == AUDIO_ENVCLK0_HIGH ? 0 : 1, data);
		break;

	case AUDIO_ENV_RAMP_DOWN:
		LOGMASKED(LOG_SPU_READS | LOG_RAMPDOWN, "audio_ctrl_r: Envelope Fast Ramp Down: %04x\n", data);
		break;

	case AUDIO_CHANNEL_STOP:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel Stop Status: %04x\n", data);
		break;

	case AUDIO_CHANNEL_ZERO_CROSS:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel Zero-Cross Enable: %04x\n", data);
		break;

	case AUDIO_CONTROL:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Control: %04x\n", data);
		break;

	case AUDIO_COMPRESS_CTRL:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Compressor Control: %04x\n", data);
		break;

	case AUDIO_CHANNEL_STATUS:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel Status: %04x\n", data);
		break;

	case AUDIO_WAVE_IN_L:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Wave In (L) / FIFO Write Data: %04x\n", data);
		break;

	case AUDIO_WAVE_IN_R:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Wave In (R) / Software Channel FIFO IRQ Control: %04x\n", data);
		break;

	case AUDIO_WAVE_OUT_L:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Wave Out (L): %04x\n", data);
		break;

	case AUDIO_WAVE_OUT_R:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Wave Out (R): %04x\n", data);
		break;

	case AUDIO_CHANNEL_REPEAT:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel Repeat Enable: %04x\n", data);
		break;

	case AUDIO_CHANNEL_ENV_MODE:
		LOGMASKED(LOG_SPU_READS | LOG_ENVELOPES, "audio_ctrl_r: Channel Envelope Enable: %04x\n", data);
		break;

	case AUDIO_CHANNEL_TONE_RELEASE:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel Tone Release Enable: %04x\n", data);
		break;

	case AUDIO_CHANNEL_ENV_IRQ:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel Envelope IRQ Status: %04x\n", data);
		break;

	case AUDIO_CHANNEL_PITCH_BEND:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Channel Pitch Bend Enable: %04x\n", data);
		break;

	case AUDIO_SOFT_PHASE:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Software Channel Phase: %04x\n", data);
		break;

	case AUDIO_ATTACK_RELEASE:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: Attack/Release Time Control: %04x\n", data);
		break;

	case AUDIO_EQ_CUTOFF10:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: EQ Cutoff Frequency 0/1: %04x\n", data);
		break;

	case AUDIO_EQ_CUTOFF32:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: EQ Cutoff Frequency 2/3: %04x\n", data);
		break;

	case AUDIO_EQ_GAIN10:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: EQ Cutoff Gain 0/1: %04x\n", data);
		break;

	case AUDIO_EQ_GAIN32:
		LOGMASKED(LOG_SPU_READS, "audio_ctrl_r: EQ Cutoff Gain 2/3: %04x\n", data);
		break;

	default:
		LOGMASKED(LOG_UNKNOWN_SPU, "audio_ctrl_r: Unknown register %04x = %04x\n", 0x3000 + offset, data);
		break;
	}

	return data;
}

READ16_MEMBER(spg2xx_audio_device::audio_r)
{
	const uint16_t channel = (offset & 0x01f0) >> 4;
	uint16_t data = m_audio_regs[offset];


	switch (offset & AUDIO_CHAN_OFFSET_MASK)
	{
	case AUDIO_WAVE_ADDR:
		LOGMASKED(LOG_CHANNEL_READS, "audio_r: Channel %d: Wave Addr (lo): %04x\n", channel, data);
		break;

	case AUDIO_MODE:
		LOGMASKED(LOG_CHANNEL_READS, "audio_r: Channel %d: Mode: %04x (ADPCM:%d, 16M:%d, TONE:%d, LADDR_HI:%04x, WADDR_HI:%04x)\n", channel, data,
			get_adpcm_bit(channel), get_16bit_bit(channel), get_tone_mode(channel), get_loop_addr_high(channel), get_wave_addr_high(channel));
		break;

	case AUDIO_LOOP_ADDR:
		LOGMASKED(LOG_CHANNEL_READS, "audio_r: Channel %d: Loop Addr: %04x\n", channel, data);
		break;

	case AUDIO_PAN_VOL:
		LOGMASKED(LOG_CHANNEL_READS, "audio_r: Channel %d: Pan/Vol: %04x (PAN:%02x, VOL:%02x)\n", channel, data,
			get_pan(channel), get_volume(channel));
		break;

	case AUDIO_ENVELOPE0:
		LOGMASKED(LOG_CHANNEL_READS | LOG_ENVELOPES, "audio_r: Channel %d: Envelope0: %04x (RPTPER:%d, TARGET:%02x, SIGN:%d, INC:%02x)\n", channel, data,
			get_repeat_period_bit(channel), get_envelope_target(channel), get_envelope_sign_bit(channel), get_envelope_inc(channel));
		break;

	case AUDIO_ENVELOPE_DATA:
		LOGMASKED(LOG_CHANNEL_READS | LOG_ENVELOPES, "audio_r: Channel %d: Envelope Data: %04x (CNT:%d, EDD:%02x)\n", channel, data,
			get_envelope_count(channel), get_edd(channel));
		break;

	case AUDIO_ENVELOPE1:
		LOGMASKED(LOG_CHANNEL_READS | LOG_ENVELOPES, "audio_r: Channel %d: Envelope1 Data: %04x (RPTCNT:%02x, RPT:%d, LOAD:%02x)\n", channel, data,
			get_envelope_repeat_count(channel), get_envelope_repeat_bit(channel), get_envelope_load(channel));
		break;

	case AUDIO_ENVELOPE_ADDR_HIGH:
		LOGMASKED(LOG_CHANNEL_READS | LOG_ENVELOPES, "audio_r: Channel %d: Envelope Addr (hi): %04x (IRQADDR:%03x, IRQEN:%d, EADDR_HI:%02x)\n", channel, data,
			get_audio_irq_addr(channel), get_audio_irq_enable_bit(channel), get_envelope_addr_high(channel));
		break;

	case AUDIO_ENVELOPE_ADDR:
		LOGMASKED(LOG_CHANNEL_READS | LOG_ENVELOPES, "audio_r: Channel %d: Envelope Addr (lo): %04x \n", channel, data);
		break;

	case AUDIO_WAVE_DATA_PREV:
		LOGMASKED(LOG_CHANNEL_READS, "audio_r: Channel %d: Wave Data Prev: %04x \n", channel, data);
		break;

	case AUDIO_ENVELOPE_LOOP_CTRL:
		LOGMASKED(LOG_CHANNEL_READS | LOG_ENVELOPES, "audio_r: Channel %d: Envelope Loop Ctrl: %04x (RDOFFS:%02x, EAOFFS:%03x)\n", channel, data,
			get_rampdown_offset(channel), get_envelope_eaoffset(channel));
		break;

	case AUDIO_WAVE_DATA:
		LOGMASKED(LOG_CHANNEL_READS, "audio_r: Channel %d: Wave Data: %04x\n", channel, data);
		break;

	case AUDIO_ADPCM_SEL:
		LOGMASKED(LOG_CHANNEL_READS, "audio_r: Channel %d: ADPCM Sel: %04x (ADPCM36:%d, POINTNUM:%02x\n", channel, data,
			get_adpcm36_bit(channel), get_point_number(channel));
		break;

	default:
		LOGMASKED(LOG_UNKNOWN_SPU, "audio_r: Unknown register %04x\n", 0x3000 + offset);
		break;

	}

	return data;
}


READ16_MEMBER(spg2xx_audio_device::audio_phase_r)
{
	const uint16_t channel = (offset & 0x01f0) >> 4;
	uint16_t data = m_audio_phase_regs[offset];

	switch (offset & AUDIO_CHAN_OFFSET_MASK)
	{
	case AUDIO_PHASE_HIGH:
		LOGMASKED(LOG_CHANNEL_READS, "audio_phase_r: Channel %d: Phase High: %04x\n", channel, data);
		break;

	case AUDIO_PHASE_ACCUM_HIGH:
		LOGMASKED(LOG_CHANNEL_READS, "audio_phase_r: Channel %d: Phase Accum High: %04x\n", channel, data);
		break;

	case AUDIO_TARGET_PHASE_HIGH:
		LOGMASKED(LOG_CHANNEL_READS, "audio_phase_r: Channel %d: Target Phase High: %04x\n", channel, data);
		break;

	case AUDIO_RAMP_DOWN_CLOCK:
		LOGMASKED(LOG_CHANNEL_READS | LOG_RAMPDOWN, "audio_phase_r: Channel %d: Rampdown Clock: %04x\n", channel, data);
		break;

	case AUDIO_PHASE:
		LOGMASKED(LOG_CHANNEL_READS, "audio_phase_r: Channel %d: Phase: %04x\n", channel, data);
		break;

	case AUDIO_PHASE_ACCUM:
		LOGMASKED(LOG_CHANNEL_READS, "audio_phase_r: Channel %d: Phase Accum: %04x\n", channel, data);
		break;

	case AUDIO_TARGET_PHASE:
		LOGMASKED(LOG_CHANNEL_READS, "audio_phase_r: Channel %d: Target Phase: %04x\n", channel, data);
		break;

	case AUDIO_PHASE_CTRL:
		LOGMASKED(LOG_CHANNEL_READS, "audio_phase_r: Channel %d: Phase Ctrl: %04x (TIMESTEP:%d, SIGN:%d, OFFSET:%03x\n", channel, data,
			get_phase_time_step(channel), get_phase_sign_bit(channel), get_phase_offset(channel));
		break;

	default:
		LOGMASKED(LOG_UNKNOWN_SPU, "audio_phase_r: Unknown register %04x\n", 0x3000 + offset);
		break;
	}

	return data;
}

WRITE16_MEMBER(spg2xx_audio_device::audio_ctrl_w)
{
	switch (offset)
	{
	case AUDIO_CHANNEL_ENABLE:
	{
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel Enable: %04x\n", data);
		const uint16_t changed = m_audio_ctrl_regs[AUDIO_CHANNEL_ENABLE] ^ data;
		for (uint32_t channel_bit = 0; channel_bit < 16; channel_bit++)
		{
			const uint16_t mask = 1 << channel_bit;
			if (!(changed & mask))
				continue;

			if (data & mask)
			{
				if (!(m_audio_ctrl_regs[AUDIO_CHANNEL_STATUS] & mask))
				{
					LOGMASKED(LOG_SPU_WRITES, "Enabling channel %d\n", channel_bit);
					m_audio_ctrl_regs[offset] |= mask;
					if (!(m_audio_ctrl_regs[AUDIO_CHANNEL_STOP] & mask))
					{
						LOGMASKED(LOG_SPU_WRITES, "Stop not set, starting playback on channel %d, mask %04x\n", channel_bit, mask);
						m_audio_ctrl_regs[AUDIO_CHANNEL_STATUS] |= mask;
						m_sample_addr[channel_bit] = get_wave_addr(channel_bit);
						m_envelope_addr[channel_bit] = get_envelope_addr(channel_bit);
						set_envelope_count(channel_bit, get_envelope_load(channel_bit));
					}
					m_adpcm[channel_bit].reset();
					m_sample_shift[channel_bit] = 0;
					m_sample_count[channel_bit] = 0;
				}
			}
			else
			{
				stop_channel(channel_bit);
				//m_audio_ctrl_regs[offset] &= ~mask;
				//m_audio_ctrl_regs[AUDIO_CHANNEL_STATUS] &= ~mask;
				//m_audio_ctrl_regs[AUDIO_CHANNEL_STOP] |= mask;
				//m_audio_ctrl_regs[AUDIO_CHANNEL_TONE_RELEASE] &= ~mask;
			}
		}
		break;
	}

	case AUDIO_MAIN_VOLUME:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Main Volume: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_MAIN_VOLUME_MASK;
		break;

	case AUDIO_CHANNEL_FIQ_ENABLE:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel FIQ Enable: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_CHANNEL_FIQ_ENABLE_MASK;
		break;

	case AUDIO_CHANNEL_FIQ_STATUS:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel FIQ Acknowledge: %04x\n", data);
		m_audio_ctrl_regs[offset] &= ~(data & AUDIO_CHANNEL_FIQ_STATUS_MASK);
		break;

	case AUDIO_BEAT_BASE_COUNT:
		LOGMASKED(LOG_SPU_WRITES | LOG_BEAT, "audio_ctrl_w: Beat Base Count: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_BEAT_BASE_COUNT_MASK;
		m_audio_curr_beat_base_count = m_audio_ctrl_regs[offset];
		break;

	case AUDIO_BEAT_COUNT:
	{
		LOGMASKED(LOG_SPU_WRITES | LOG_BEAT, "audio_ctrl_w: Beat Count: %04x\n", data);
		const uint16_t old = m_audio_ctrl_regs[offset];
		m_audio_ctrl_regs[offset] &= ~(data & AUDIO_BIS_MASK);
		m_audio_ctrl_regs[offset] &= AUDIO_BIS_MASK;
		m_audio_ctrl_regs[offset] |= data & ~AUDIO_BIS_MASK;
		const uint16_t changed = old ^ m_audio_ctrl_regs[offset];
		if (changed & (AUDIO_BIS_MASK | AUDIO_BIE_MASK))
		{
			LOGMASKED(LOG_BEAT, "BIS mask changed, updating IRQ\n");
			check_irqs(changed & (AUDIO_BIS_MASK | AUDIO_BIE_MASK));
		}
		break;
	}

	case AUDIO_ENVCLK0:
	case AUDIO_ENVCLK1:
	{
		LOGMASKED(LOG_SPU_WRITES | LOG_ENVELOPES, "audio_ctrl_w: Envelope Interval %d (lo): %04x\n", offset == AUDIO_ENVCLK0 ? 0 : 1, data);
		const uint16_t old = m_audio_ctrl_regs[offset];
		m_audio_ctrl_regs[offset] = data;
		const uint16_t changed = old ^ m_audio_ctrl_regs[offset];

		if (!changed)
			break;

		const uint8_t channel_offset = offset == AUDIO_ENVCLK0 ? 0 : 8;
		for (uint8_t channel_bit = 0; channel_bit < 4; channel_bit++)
		{
			const uint8_t shift = channel_bit << 2;
			const uint16_t mask = 0x0f << shift;
			if (changed & mask)
			{
				m_envclk_frame[channel_bit + channel_offset] = get_envclk_frame_count(channel_bit + channel_offset);
			}
		}
		break;
	}

	case AUDIO_ENVCLK0_HIGH:
	case AUDIO_ENVCLK1_HIGH:
	{
		LOGMASKED(LOG_SPU_WRITES | LOG_ENVELOPES, "audio_ctrl_w: Envelope Interval %d (hi): %04x\n", offset == AUDIO_ENVCLK0_HIGH ? 0 : 1, data);
		const uint16_t old = m_audio_ctrl_regs[offset];
		m_audio_ctrl_regs[offset] = data;
		const uint16_t changed = old ^ m_audio_ctrl_regs[offset];
		if (!changed)
			break;

		const uint8_t channel_offset = offset == AUDIO_ENVCLK0_HIGH ? 0 : 8;
		for (uint8_t channel_bit = 0; channel_bit < 4; channel_bit++)
		{
			const uint8_t shift = channel_bit << 2;
			const uint16_t mask = 0x0f << shift;
			if (changed & mask)
			{
				m_envclk_frame[channel_bit + channel_offset + 4] = get_envclk_frame_count(channel_bit + channel_offset);
			}
		}
		break;
	}

	case AUDIO_ENV_RAMP_DOWN:
	{
		LOGMASKED(LOG_SPU_WRITES | LOG_RAMPDOWN, "audio_ctrl_w: Envelope Fast Ramp Down: %04x\n", data);
		const uint16_t old = m_audio_ctrl_regs[offset];
		m_audio_ctrl_regs[offset] = (data & AUDIO_ENV_RAMP_DOWN_MASK) & m_audio_ctrl_regs[AUDIO_CHANNEL_STATUS];
		const uint16_t changed = old ^ m_audio_ctrl_regs[offset];
		if (!changed)
			break;

		for (uint32_t channel_bit = 0; channel_bit < 16; channel_bit++)
		{
			const uint16_t mask = 1 << channel_bit;
			if ((changed & mask) && (data & mask))
			{
				m_rampdown_frame[channel_bit] = get_rampdown_frame_count(channel_bit);
				LOGMASKED(LOG_RAMPDOWN, "Preparing to ramp down channel %d in %d ticks\n", channel_bit, m_rampdown_frame[channel_bit] / 13);
			}
		}
		break;
	}

	case AUDIO_CHANNEL_STOP:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel Stop Status: %04x\n", data);
		m_audio_ctrl_regs[offset] &= ~data;
		break;

	case AUDIO_CHANNEL_ZERO_CROSS:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel Zero-Cross Enable: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_CHANNEL_ZERO_CROSS_MASK;
		break;

	case AUDIO_CONTROL:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Control: %04x (SOFTCH:%d, COMPEN:%d, NOHIGH:%d, NOINT:%d, EQEN:%d, VOLSEL:%d)\n", data
			, (data & AUDIO_CONTROL_SOFTCH_MASK) ? 1 : 0
			, (data & AUDIO_CONTROL_COMPEN_MASK) ? 1 : 0
			, (data & AUDIO_CONTROL_NOHIGH_MASK) ? 1 : 0
			, (data & AUDIO_CONTROL_NOINT_MASK) ? 1 : 0
			, (data & AUDIO_CONTROL_EQEN_MASK) ? 1 : 0
			, (data & AUDIO_CONTROL_VOLSEL_MASK) >> AUDIO_CONTROL_VOLSEL_SHIFT);
		m_audio_ctrl_regs[offset] = data & AUDIO_CONTROL_MASK;
		break;

	case AUDIO_COMPRESS_CTRL:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Compressor Control: %04x\n", data);
		m_audio_ctrl_regs[offset] = data;
		break;

	case AUDIO_CHANNEL_STATUS:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel Status (read-only): %04x\n", data);
		break;

	case AUDIO_WAVE_IN_L:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Wave In (L) / FIFO Write Data: %04x\n", data);
		m_audio_ctrl_regs[offset] = data;
		break;

	case AUDIO_WAVE_IN_R:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Wave In (R) / Software Channel FIFO IRQ Control: %04x\n", data);
		m_audio_ctrl_regs[offset] = data;
		break;

	case AUDIO_WAVE_OUT_L:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Wave Out (L): %04x\n", data);
		m_audio_ctrl_regs[offset] = data;
		break;

	case AUDIO_WAVE_OUT_R:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Wave Out (R): %04x\n", data);
		m_audio_ctrl_regs[offset] = data;
		break;

	case AUDIO_CHANNEL_REPEAT:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel Repeat Enable: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_CHANNEL_REPEAT_MASK;
		break;

	case AUDIO_CHANNEL_ENV_MODE:
		LOGMASKED(LOG_SPU_WRITES | LOG_ENVELOPES, "audio_ctrl_w: Channel Envelope Enable: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_CHANNEL_ENV_MODE_MASK;
		break;

	case AUDIO_CHANNEL_TONE_RELEASE:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel Tone Release Enable: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_CHANNEL_TONE_RELEASE_MASK;
		break;

	case AUDIO_CHANNEL_ENV_IRQ:
		LOGMASKED(LOG_SPU_WRITES | LOG_ENVELOPES, "audio_ctrl_w: Channel Envelope IRQ Acknowledge: %04x\n", data);
		m_audio_ctrl_regs[offset] &= ~data & AUDIO_CHANNEL_ENV_IRQ_MASK;
		break;

	case AUDIO_CHANNEL_PITCH_BEND:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Channel Pitch Bend Enable: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_CHANNEL_PITCH_BEND_MASK;
		break;

	case AUDIO_SOFT_PHASE:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Software Channel Phase: %04x\n", data);
		m_audio_ctrl_regs[offset] = data;
		break;

	case AUDIO_ATTACK_RELEASE:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: Attack/Release Time Control: %04x\n", data);
		m_audio_ctrl_regs[offset] = data;
		break;

	case AUDIO_EQ_CUTOFF10:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: EQ Cutoff Frequency 0/1: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_EQ_CUTOFF10_MASK;
		break;

	case AUDIO_EQ_CUTOFF32:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: EQ Cutoff Frequency 2/3: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_EQ_CUTOFF32_MASK;
		break;

	case AUDIO_EQ_GAIN10:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: EQ Cutoff Gain 0/1: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_EQ_GAIN10_MASK;
		break;

	case AUDIO_EQ_GAIN32:
		LOGMASKED(LOG_SPU_WRITES, "audio_ctrl_w: EQ Cutoff Gain 2/3: %04x\n", data);
		m_audio_ctrl_regs[offset] = data & AUDIO_EQ_GAIN32_MASK;
		break;

	default:
		m_audio_ctrl_regs[offset] = data;
		LOGMASKED(LOG_UNKNOWN_SPU, "audio_ctrl_w: Unknown register %04x = %04x\n", 0x3000 + offset, data);
		break;
	}
}

WRITE16_MEMBER(spg2xx_audio_device::audio_phase_w)
{
	const uint16_t channel = (offset & 0x01f0) >> 4;

	switch (offset & AUDIO_CHAN_OFFSET_MASK)
	{
	case AUDIO_PHASE_HIGH:
		m_audio_phase_regs[offset] = data & AUDIO_PHASE_HIGH_MASK;
		m_channel_rate[channel] = ((double)get_phase(channel) * 140625.0 * 2.0) / (double)(1 << 19);
		m_channel_rate_accum[channel] = 0.0;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_phase_w: Channel %d: Phase High: %04x (rate: %f)\n", channel, data, m_channel_rate[channel]);
		break;

	case AUDIO_PHASE_ACCUM_HIGH:
		m_audio_phase_regs[offset] = data & AUDIO_PHASE_ACCUM_HIGH_MASK;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_phase_w: Channel %d: Phase Accum High: %04x\n", channel, data);
		break;

	case AUDIO_TARGET_PHASE_HIGH:
		m_audio_phase_regs[offset] = data & AUDIO_TARGET_PHASE_HIGH_MASK;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_phase_w: Channel %d: Target Phase High: %04x\n", channel, data);
		break;

	case AUDIO_RAMP_DOWN_CLOCK:
		m_audio_phase_regs[offset] = data & AUDIO_RAMP_DOWN_CLOCK_MASK;
		LOGMASKED(LOG_CHANNEL_WRITES | LOG_RAMPDOWN, "audio_phase_w: Channel %d: Rampdown Clock: %04x\n", channel, data);
		break;

	case AUDIO_PHASE:
		m_audio_phase_regs[offset] = data;
		m_channel_rate[channel] = ((double)get_phase(channel) * 140625.0 * 2.0) / (double)(1 << 19);
		m_channel_rate_accum[channel] = 0.0;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_phase_w: Channel %d: Phase: %04x (rate: %f)\n", channel, data, m_channel_rate[channel]);
		break;

	case AUDIO_PHASE_ACCUM:
		m_audio_phase_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_phase_w: Channel %d: Phase Accum: %04x\n", channel, data);
		break;

	case AUDIO_TARGET_PHASE:
		m_audio_phase_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_phase_w: Channel %d: Target Phase: %04x\n", channel, data);
		break;

	case AUDIO_PHASE_CTRL:
		m_audio_phase_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_phase_w: Channel %d: Phase Ctrl: %04x (TIMESTEP:%d, SIGN:%d, OFFSET:%03x\n", channel, data,
			get_phase_time_step(channel), get_phase_sign_bit(channel), get_phase_offset(channel));
		break;

	default:
		m_audio_phase_regs[offset] = data;
		LOGMASKED(LOG_UNKNOWN_SPU, "audio_phase_w: Unknown register %04x = %04x\n", 0x3000 + offset, data);
		break;
	}
}


WRITE16_MEMBER(spg2xx_audio_device::audio_w)
{
	const uint16_t channel = (offset & 0x01f0) >> 4;

	switch (offset & AUDIO_CHAN_OFFSET_MASK)
	{
	case AUDIO_WAVE_ADDR:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_w: Channel %d: Wave Addr (lo): %04x\n", channel, data);
		break;

	case AUDIO_MODE:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_w: Channel %d: Mode: %04x (ADPCM:%d, 16M:%d, TONE:%d, LADDR_HI:%04x, WADDR_HI:%04x)\n", channel, data,
			get_adpcm_bit(channel), get_16bit_bit(channel), get_tone_mode(channel), get_loop_addr_high(channel), get_wave_addr_high(channel));
		break;

	case AUDIO_LOOP_ADDR:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_w: Channel %d: Loop Addr: %04x\n", channel, data);
		break;

	case AUDIO_PAN_VOL:
		m_audio_regs[offset] = data & AUDIO_PAN_VOL_MASK;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_w: Channel %d: Pan/Vol: %04x (PAN:%02x, VOL:%02x)\n", channel, data,
			get_pan(channel), get_volume(channel));
		break;

	case AUDIO_ENVELOPE0:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES | LOG_ENVELOPES, "audio_w: Channel %d: Envelope0: %04x (RPTPER:%d, TARGET:%02x, SIGN:%d, INC:%02x)\n", channel, data,
			get_repeat_period_bit(channel), get_envelope_target(channel), get_envelope_sign_bit(channel), get_envelope_inc(channel));
		break;

	case AUDIO_ENVELOPE_DATA:
		m_audio_regs[offset] = data & AUDIO_ENVELOPE_DATA_MASK;
		LOGMASKED(LOG_CHANNEL_WRITES | LOG_ENVELOPES, "audio_w: Channel %d: Envelope Data: %04x (CNT:%d, EDD:%02x)\n", channel, data,
			get_envelope_count(channel), get_edd(channel));
		break;

	case AUDIO_ENVELOPE1:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES | LOG_ENVELOPES, "audio_w: Channel %d: Envelope1 Data: %04x (RPTCNT:%02x, RPT:%d, LOAD:%02x)\n", channel, data,
			get_envelope_repeat_count(channel), get_envelope_repeat_bit(channel), get_envelope_load(channel));
		break;

	case AUDIO_ENVELOPE_ADDR_HIGH:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES | LOG_ENVELOPES, "audio_w: Channel %d: Envelope Addr (hi): %04x (IRQADDR:%03x, IRQEN:%d, EADDR_HI:%02x)\n", channel, data,
			get_audio_irq_addr(channel), get_audio_irq_enable_bit(channel), get_envelope_addr_high(channel));
		break;

	case AUDIO_ENVELOPE_ADDR:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES | LOG_ENVELOPES, "audio_w: Channel %d: Envelope Addr (lo): %04x\n", channel, data);
		break;

	case AUDIO_WAVE_DATA_PREV:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_w: Channel %d: Wave Data Prev: %04x \n", channel, data);
		break;

	case AUDIO_ENVELOPE_LOOP_CTRL:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES | LOG_ENVELOPES, "audio_w: Channel %d: Envelope Loop Ctrl: %04x (RDOFFS:%02x, EAOFFS:%03x)\n", channel, data,
			get_rampdown_offset(channel), get_envelope_eaoffset(channel));
		break;

	case AUDIO_WAVE_DATA:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_w: Channel %d: Wave Data: %04x\n", channel, data);
		break;

	case AUDIO_ADPCM_SEL:
		m_audio_regs[offset] = data & AUDIO_ADPCM_SEL_MASK;
		LOGMASKED(LOG_CHANNEL_WRITES, "audio_w: Channel %d: ADPCM Sel: %04x (ADPCM36:%d, POINTNUM:%02x\n", channel, data,
			get_adpcm36_bit(channel), get_point_number(channel));
		break;

	default:
		m_audio_regs[offset] = data;
		LOGMASKED(LOG_UNKNOWN_SPU, "audio_w: Unknown register %04x = %04x\n", 0x3000 + offset, data);
		break;

	}
}

void spg2xx_audio_device::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	stream_sample_t *out_l = outputs[0];
	stream_sample_t *out_r = outputs[1];

	for (int i = 0; i < samples; i++)
	{
		int32_t left_total = 0;
		int32_t right_total = 0;

		for (uint32_t channel = 0; channel < 16; channel++)
		{
			if (!get_channel_status(channel))
			{
				continue;
			}

			if (SPG_DEBUG_AUDIO && m_debug_rates)
				printf("%f:%f ", m_channel_rate[channel], m_channel_rate_accum[channel]);
			bool playing = advance_channel(channel);
			if (playing)
			{
				int32_t sample = (int16_t)(m_audio_regs[(channel << 4) | AUDIO_WAVE_DATA] ^ 0x8000);
				if (!(m_audio_ctrl_regs[AUDIO_CONTROL] & AUDIO_CONTROL_NOINT_MASK))
				{
					int32_t prev_sample = (int16_t)(m_audio_regs[(channel << 4) | AUDIO_WAVE_DATA_PREV] ^ 0x8000);
					int16_t lerp_factor = (int16_t)((m_channel_rate_accum[channel] / 70312.5) * 256.0);
					prev_sample = (prev_sample * (0x100 - lerp_factor)) >> 8;
					sample = (sample * lerp_factor) >> 8;
					sample += prev_sample;
				}

				sample = (sample * (int32_t)get_edd(channel)) >> 7;

				int32_t vol = get_volume(channel);
				int32_t pan = get_pan(channel);

				int32_t pan_left, pan_right;
				if (pan < 0x40)
				{
					pan_left = 0x7f * vol;
					pan_right = pan * 2 * vol;
				}
				else
				{
					pan_left = (0x7f - pan) * 2 * vol;
					pan_right = 0x7f * vol;
				}

				left_total += ((int16_t)sample * (int16_t)pan_left) >> 14;
				right_total += ((int16_t)sample * (int16_t)pan_right) >> 14;

				const uint16_t mask = (1 << channel);
				if (m_audio_ctrl_regs[AUDIO_ENV_RAMP_DOWN] & mask)
				{
					if (m_rampdown_frame[channel] == 0)
					{
						LOGMASKED(LOG_RAMPDOWN, "Ticking rampdown for channel %d\n", channel);
						audio_rampdown_tick(channel);
					}
					m_rampdown_frame[channel]--;
				}
				else if (!(m_audio_ctrl_regs[AUDIO_CHANNEL_ENV_MODE] & mask))
				{
					if (m_envclk_frame[channel] == 0)
					{
						LOGMASKED(LOG_ENVELOPES, "Ticking envelope for channel %d\n", channel);
						audio_envelope_tick(channel);
						m_envclk_frame[channel] = get_envclk_frame_count(channel);
					}
					m_envclk_frame[channel]--;
				}
			}
		}

		switch (get_vol_sel())
		{
			case 0: // 1/16
				left_total >>= 4;
				right_total >>= 4;
				break;
			case 1: // 1/4
			case 2: // 1
			case 3: // 2 // Both x1 and x2 clip like mad even with only 6 voices. Hack it for now.
				left_total >>= 2;
				right_total >>= 2;
				break;
		}
		*out_l++ = (left_total * (int16_t)m_audio_ctrl_regs[AUDIO_MAIN_VOLUME]) >> 7;
		*out_r++ = (right_total * (int16_t)m_audio_ctrl_regs[AUDIO_MAIN_VOLUME]) >> 7;
	}
}

inline void spg2xx_audio_device::stop_channel(const uint32_t channel)
{
	// TODO: IRQs
	m_audio_ctrl_regs[AUDIO_CHANNEL_ENABLE] &= ~(1 << channel);
	m_audio_ctrl_regs[AUDIO_CHANNEL_STATUS] &= ~(1 << channel);
	m_audio_regs[(channel << 4) | AUDIO_MODE] &= ~AUDIO_ADPCM_MASK;
	m_audio_ctrl_regs[AUDIO_CHANNEL_TONE_RELEASE] &= ~(1 << channel);
	m_audio_ctrl_regs[AUDIO_ENV_RAMP_DOWN] &= ~(1 << channel);
}

bool spg2xx_audio_device::advance_channel(const uint32_t channel)
{
	m_channel_rate_accum[channel] += m_channel_rate[channel];
	uint32_t samples_to_advance = 0;
	while (m_channel_rate_accum[channel] >= 70312.5)
	{
		m_channel_rate_accum[channel] -= 70312.5;
		samples_to_advance++;
	}

	if (!samples_to_advance)
		return true;

	bool playing = true;

	for (uint32_t sample = 0; sample < samples_to_advance; sample++)
	{
		playing = fetch_sample(channel);
		if (!playing)
			break;

		if (get_adpcm_bit(channel))
		{
			// ADPCM mode
			m_sample_shift[channel] += 4;
			if (m_sample_shift[channel] >= 16)
			{
				m_sample_shift[channel] = 0;
				m_sample_addr[channel]++;
			}
		}
		else if (get_16bit_bit(channel))
		{
			// 16-bit mode
			m_sample_addr[channel]++;
		}
		else
		{
			// 8-bit mode
			m_sample_shift[channel] += 8;
			if (m_sample_shift[channel] >= 16)
			{
				m_sample_shift[channel] = 0;
				m_sample_addr[channel]++;
			}
		}
	}

	return playing;
}

uint16_t spg2xx_audio_device::read_space(offs_t offset)
{
	return m_space_read_cb(offset);
}

bool spg2xx_audio_device::fetch_sample(const uint32_t channel)
{
	const uint32_t channel_mask = channel << 4;
	m_audio_regs[channel_mask | AUDIO_WAVE_DATA_PREV] = m_audio_regs[channel_mask | AUDIO_WAVE_DATA];

	const uint32_t wave_data_reg = channel_mask | AUDIO_WAVE_DATA;
	const uint16_t tone_mode = get_tone_mode(channel);
	uint16_t raw_sample = tone_mode ? read_space(m_sample_addr[channel]) : m_audio_regs[wave_data_reg];

	LOGMASKED(LOG_SAMPLES, "Channel %d: Raw sample %04x\n", channel, raw_sample);

	if (get_adpcm_bit(channel))
	{
		// ADPCM mode
		if (tone_mode != 0 && raw_sample == 0xffff)
		{
			if (tone_mode == AUDIO_TONE_MODE_HW_ONESHOT)
			{
				LOGMASKED(LOG_SAMPLES, "ADPCM stopped after %d samples\n", m_sample_count[channel]);
				m_sample_count[channel] = 0;
				stop_channel(channel);
				return false;
			}
			else
			{
				LOGMASKED(LOG_SAMPLES, "ADPCM looping after %d samples\n", m_sample_count[channel]);
				m_sample_count[channel] = 0;
				loop_channel(channel);
				m_audio_regs[(channel << 4) | AUDIO_MODE] &= ~AUDIO_ADPCM_MASK;
			}
		}
		else
		{
			m_audio_regs[wave_data_reg] = raw_sample;
			m_audio_regs[wave_data_reg] >>= m_sample_shift[channel];
			const uint8_t adpcm_sample = (uint8_t)(m_audio_regs[wave_data_reg] & 0x000f);
			m_audio_regs[wave_data_reg] = (uint16_t)(m_adpcm[channel].clock(adpcm_sample) << 4) ^ 0x8000;
		}
		m_sample_count[channel]++;
	}
	else if (get_16bit_bit(channel))
	{
		// 16-bit mode
		if (tone_mode != 0 && raw_sample == 0xffff)
		{
			if (tone_mode == AUDIO_TONE_MODE_HW_ONESHOT)
			{
				LOGMASKED(LOG_SAMPLES, "16-bit PCM stopped after %d samples\n", m_sample_count[channel]);
				m_sample_count[channel] = 0;
				stop_channel(channel);
				return false;
			}
			else
			{
				LOGMASKED(LOG_SAMPLES, "16-bit PCM looping after %d samples\n", m_sample_count[channel]);
				m_sample_count[channel] = 0;
				loop_channel(channel);
			}
		}
		else
		{
			m_audio_regs[wave_data_reg] = raw_sample;
		}
		m_sample_count[channel]++;
	}
	else
	{
		// 8-bit mode
		LOGMASKED(LOG_SAMPLES, "Channel %d: Processing as 8-bit sample, tone_mode is %d, sample_shift is %d\n", channel, tone_mode, m_sample_shift[channel]);
		if (tone_mode != 0)
		{
			if (m_sample_shift[channel])
				raw_sample &= 0xff00;
			else
				raw_sample <<= 8;
			raw_sample |= raw_sample >> 8;

			if (raw_sample == 0xffff)
			{
				if (tone_mode == AUDIO_TONE_MODE_HW_ONESHOT)
				{
					LOGMASKED(LOG_SAMPLES, "Channel %d: 8-bit PCM stopped after %d samples\n", channel, m_sample_count[channel]);
					m_sample_count[channel] = 0;
					stop_channel(channel);
					return false;
				}
				else
				{
					LOGMASKED(LOG_SAMPLES, "Channel %d: 8-bit PCM looping after %d samples\n", channel, m_sample_count[channel]);
					m_sample_count[channel] = 0;
					loop_channel(channel);
				}
			}
			else
			{
				m_audio_regs[wave_data_reg] = raw_sample;
			}
		}
		m_sample_count[channel]++;
	}

	return true;
}

inline void spg2xx_audio_device::loop_channel(const uint32_t channel)
{
	m_sample_addr[channel] = get_loop_addr(channel);
	m_sample_shift[channel] = 0;
	LOGMASKED(LOG_SAMPLES, "Channel %d: Looping to address %08x\n", channel, m_sample_addr[channel]);
}

void spg2xx_audio_device::audio_beat_tick()
{
	if (m_audio_curr_beat_base_count == 0)
	{
		LOGMASKED(LOG_BEAT, "Beat base count elapsed, reloading with %d\n", m_audio_ctrl_regs[AUDIO_BEAT_BASE_COUNT]);
		m_audio_curr_beat_base_count = m_audio_ctrl_regs[AUDIO_BEAT_BASE_COUNT];

		uint16_t beat_count = m_audio_ctrl_regs[AUDIO_BEAT_COUNT] & AUDIO_BEAT_COUNT_MASK;
		if (beat_count == 0)
		{
			if (m_audio_ctrl_regs[AUDIO_BEAT_COUNT] & AUDIO_BIE_MASK)
			{
				LOGMASKED(LOG_BEAT, "Beat count elapsed, setting Status bit and checking IRQs\n");
				m_audio_ctrl_regs[AUDIO_BEAT_COUNT] |= AUDIO_BIS_MASK;
				check_irqs(AUDIO_BIS_MASK);
			}
			else
			{
				LOGMASKED(LOG_BEAT, "Beat count elapsed but IRQ not enabled\n");
			}
		}
		else
		{
			beat_count--;
			m_audio_ctrl_regs[AUDIO_BEAT_COUNT] = (m_audio_ctrl_regs[AUDIO_BEAT_COUNT] & ~AUDIO_BEAT_COUNT_MASK) | beat_count;
		}
	}
	m_audio_curr_beat_base_count--;
}

void spg2xx_audio_device::audio_rampdown_tick(const uint32_t channel)
{
	const uint8_t old_edd = get_edd(channel);
	uint8_t new_edd = old_edd - get_rampdown_offset(channel);
	if (new_edd > old_edd)
		new_edd = 0;

	if (new_edd)
	{
		LOGMASKED(LOG_RAMPDOWN, "Channel %d preparing for next rampdown step (%02x)\n", channel, new_edd);
		const uint16_t channel_mask = channel << 4;
		m_audio_regs[channel_mask | AUDIO_ENVELOPE_DATA] &= ~AUDIO_EDD_MASK;
		m_audio_regs[channel_mask | AUDIO_ENVELOPE_DATA] |= new_edd & AUDIO_EDD_MASK;
		m_rampdown_frame[channel] = get_rampdown_frame_count(channel);
	}
	else
	{
		LOGMASKED(LOG_RAMPDOWN, "Stopping channel %d due to rampdown\n", channel);
		const uint16_t channel_mask = 1 << channel;
		m_audio_ctrl_regs[AUDIO_CHANNEL_ENABLE] &= ~channel_mask;
		m_audio_ctrl_regs[AUDIO_CHANNEL_STATUS] &= ~channel_mask;
		m_audio_ctrl_regs[AUDIO_CHANNEL_STOP] |= channel_mask;
		m_audio_ctrl_regs[AUDIO_ENV_RAMP_DOWN] &= ~channel_mask;
		m_audio_ctrl_regs[AUDIO_CHANNEL_TONE_RELEASE] &= ~channel_mask;
	}
}

const uint32_t spg2xx_audio_device::s_rampdown_frame_counts[8] =
{
	13*4, 13*16, 13*64, 13*256, 13*1024, 13*4096, 13*8192, 13*8192
};

uint32_t spg2xx_audio_device::get_rampdown_frame_count(const uint32_t channel)
{
	return s_rampdown_frame_counts[get_rampdown_clock(channel)];
}

const uint32_t spg2xx_audio_device::s_envclk_frame_counts[16] =
{
	4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 8192, 8192, 8192, 8192
};

uint32_t spg2xx_audio_device::get_envclk_frame_count(const uint32_t channel)
{
	return s_envclk_frame_counts[get_envelope_clock(channel)];
}

uint32_t spg2xx_audio_device::get_envelope_clock(const offs_t channel) const
{
	if (channel < 4)
		return (m_audio_ctrl_regs[AUDIO_ENVCLK0] >> (channel << 2)) & 0x000f;
	else if (channel < 8)
		return (m_audio_ctrl_regs[AUDIO_ENVCLK0_HIGH] >> ((channel - 4) << 2)) & 0x000f;
	else if (channel < 12)
		return (m_audio_ctrl_regs[AUDIO_ENVCLK1] >> ((channel - 8) << 2)) & 0x000f;
	else
		return (m_audio_ctrl_regs[AUDIO_ENVCLK1_HIGH] >> ((channel - 12) << 2)) & 0x000f;
}

bool spg2xx_audio_device::audio_envelope_tick(const uint32_t channel)
{
	const uint16_t channel_mask = channel << 4;
	uint16_t new_count = get_envelope_count(channel);
	const uint16_t curr_edd = get_edd(channel);
	LOGMASKED(LOG_ENVELOPES, "envelope %d tick, count is %04x, curr edd is %04x\n", channel, new_count, curr_edd);
	bool edd_changed = false;
	if (new_count == 0)
	{
		const uint16_t target = get_envelope_target(channel);
		uint16_t new_edd = curr_edd;
		const uint16_t inc = get_envelope_inc(channel);

		if (new_edd != target)
		{
			if (get_envelope_sign_bit(channel))
			{
				new_edd -= inc;
				LOGMASKED(LOG_ENVELOPES, "Envelope %d new EDD-: %04x (%04x), dec %04x\n", channel, new_edd, target, inc);
				if (new_edd > curr_edd)
					new_edd = 0;
				else if (new_edd < target)
					new_edd = target;

				if (new_edd == 0)
				{
					LOGMASKED(LOG_ENVELOPES, "Envelope %d at 0, stopping channel\n", channel);
					stop_channel(channel);
					return true;
				}
			}
			else
			{
				new_edd += inc;
				LOGMASKED(LOG_ENVELOPES, "Envelope %d new EDD+: %04x (%04x), inc %04x\n", channel, new_edd, target, inc);
				if (new_edd >= target)
					new_edd = target;
			}
		}

		if (new_edd == target)
		{
			LOGMASKED(LOG_ENVELOPES, "Envelope %d at target %04x\n", channel, target);
			new_edd = target;

			if (get_envelope_repeat_bit(channel))
			{
				const uint16_t repeat_count = get_envelope_repeat_count(channel) - 1;
				LOGMASKED(LOG_ENVELOPES, "Repeating envelope, new repeat count %d\n", repeat_count);
				if (repeat_count == 0)
				{
					m_audio_regs[channel_mask | AUDIO_ENVELOPE0] = read_space(m_envelope_addr[channel]);
					m_audio_regs[channel_mask | AUDIO_ENVELOPE1] = read_space(m_envelope_addr[channel] + 1);
					m_audio_regs[channel_mask | AUDIO_ENVELOPE_LOOP_CTRL] = read_space(m_envelope_addr[channel] + 2);
					m_envelope_addr[channel] = get_envelope_addr(channel) + get_envelope_eaoffset(channel);
					LOGMASKED(LOG_ENVELOPES, "Envelope data after repeat: %04x %04x %04x (%08x)\n", m_audio_regs[channel_mask | AUDIO_ENVELOPE0], m_audio_regs[channel_mask | AUDIO_ENVELOPE1], m_audio_regs[channel_mask | AUDIO_ENVELOPE_LOOP_CTRL], m_envelope_addr[channel]);
				}
				else
				{
					set_envelope_repeat_count(channel, repeat_count);
				}
			}
			else
			{
				LOGMASKED(LOG_ENVELOPES, "Fetching envelope for channel %d from %08x\n", channel, m_envelope_addr[channel]);
				m_audio_regs[channel_mask | AUDIO_ENVELOPE0] = read_space(m_envelope_addr[channel]);
				m_audio_regs[channel_mask | AUDIO_ENVELOPE1] = read_space(m_envelope_addr[channel] + 1);
				LOGMASKED(LOG_ENVELOPES, "Fetched envelopes %04x %04x\n", m_audio_regs[channel_mask | AUDIO_ENVELOPE0], m_audio_regs[channel_mask | AUDIO_ENVELOPE1]);
				m_envelope_addr[channel] += 2;
			}
			new_count = get_envelope_load(channel);
			set_envelope_count(channel, new_count);
		}
		else
		{
			LOGMASKED(LOG_ENVELOPES, "Envelope %d not yet at target %04x (%04x)\n", channel, target, new_edd);
			new_count = get_envelope_load(channel);
			set_envelope_count(channel, new_count);
		}
		LOGMASKED(LOG_ENVELOPES, "Envelope %d new count %04x\n", channel, new_count);

		set_edd(channel, new_edd);
		edd_changed = true;
		LOGMASKED(LOG_ENVELOPES, "Setting channel %d edd to %04x, register is %04x\n", channel, new_edd, m_audio_regs[(channel << 4) | AUDIO_ENVELOPE_DATA]);
	}
	else
	{
		new_count--;
		set_envelope_count(channel, new_count);
	}
	LOGMASKED(LOG_ENVELOPES, "envelope %d post-tick, count is now %04x, register is %04x\n", channel, new_count, m_audio_regs[(channel << 4) | AUDIO_ENVELOPE_DATA]);
	return edd_changed;
}



WRITE16_MEMBER(spg110_audio_device::audio_w)
{
	const uint16_t channel = (offset & 0x00f0) >> 4;

	switch (offset & AUDIO_CHAN_OFFSET_MASK)
	{
	case 0x0e:
		m_audio_regs[offset] = data;
		m_channel_rate[channel] = ((double)get_phase(channel) * 140625.0 * 2.0) / (double)(1 << 19);
		m_channel_rate_accum[channel] = 0.0;
		LOGMASKED(LOG_CHANNEL_WRITES, "spg110_audio_device::audio_w: Channel %d: Phase: %04x (rate: %f)\n", channel, data, m_channel_rate[channel]);
		return;
	}

	spg2xx_audio_device::audio_w(space,offset,data,mem_mask);
}

uint16_t sunplus_gcm394_audio_device::control_group16_r(uint8_t group, uint8_t offset)
{
	LOGMASKED(LOG_SPU_WRITES, "sunplus_gcm394_audio_device::control_group16_r (group %d) offset %02x\n", group, offset);
	return m_control[group][offset];
}

void sunplus_gcm394_audio_device::control_group16_w(uint8_t group, uint8_t offset, uint16_t data)
{
	LOGMASKED(LOG_SPU_WRITES, "sunplus_gcm394_audio_device::control_group16_w (group %d) offset %02x data %04x\n", group, offset, data);
	m_control[group][offset] = data;

	// offset 0x0b = triggers?
}

READ16_MEMBER(sunplus_gcm394_audio_device::control_r)
{
	return control_group16_r(offset & 0x20 ? 1 : 0, offset & 0x1f);
}


WRITE16_MEMBER(sunplus_gcm394_audio_device::control_w)
{
	control_group16_w(offset & 0x20 ? 1 : 0, offset & 0x1f, data);
}

void sunplus_gcm394_audio_device::device_start()
{
	spg2xx_audio_device::device_start();

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 0x20; j++)
			m_control[i][j] = 0x0000;

}
