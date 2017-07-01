// license:GPL-2.0+
// copyright-holders:Juergen Buchmueller, Frank Palazzolo, Sean Riddle
/*****************************************************************************
 *
 * audio/channelf.h
 *
 ****************************************************************************/

#ifndef CHANNELF_SOUND_H_
#define CHANNELF_SOUND_H_

class channelf_sound_device : public device_t,
									public device_sound_interface
{
public:
	channelf_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void sound_w(int mode);
protected:
	// device-level overrides
	virtual void device_config_complete() override;
	virtual void device_start() override;

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples) override;
private:
	// internal state
	sound_stream *m_channel;
	int m_sound_mode;
	int m_incr;
	float m_decay_mult;
	int m_envelope;
	uint32_t m_sample_counter;
	int m_forced_ontime;           //  added for improved sound
	int m_min_ontime;              //  added for improved sound

};

extern const device_type CHANNELF_SOUND;


#endif /* CHANNELF_SOUND_H_ */
