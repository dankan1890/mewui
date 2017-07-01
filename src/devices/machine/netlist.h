// license:GPL-2.0+
// copyright-holders:Couriersud
/***************************************************************************

    netlist.h

    Discrete netlist implementation.

****************************************************************************/

#ifndef NETLIST_H
#define NETLIST_H

#include "emu.h"

#include "netlist/nl_base.h"
#include "netlist/nl_setup.h"

// MAME specific configuration


#define MCFG_NETLIST_SETUP(_setup)                                                  \
	netlist_mame_device_t::static_set_constructor(*device, NETLIST_NAME(_setup));

#define MCFG_NETLIST_ANALOG_INPUT(_basetag, _tag, _name)                            \
	MCFG_DEVICE_ADD(_basetag ":" _tag, NETLIST_ANALOG_INPUT, 0)                     \
	netlist_mame_analog_input_t::static_set_name(*device, _name);

#define MCFG_NETLIST_ANALOG_MULT_OFFSET(_mult, _offset)                             \
	netlist_mame_sub_interface::static_set_mult_offset(*device, _mult, _offset);

#define MCFG_NETLIST_ANALOG_OUTPUT(_basetag, _tag, _IN, _class, _member, _class_tag) \
	MCFG_DEVICE_ADD(_basetag ":" _tag, NETLIST_ANALOG_OUTPUT, 0)                    \
	netlist_mame_analog_output_t::static_set_params(*device, _IN,                   \
				netlist_analog_output_delegate(& _class :: _member,                 \
						# _class "::" # _member, _class_tag, (_class *)nullptr)   );

#define MCFG_NETLIST_LOGIC_INPUT(_basetag, _tag, _name, _shift)              \
	MCFG_DEVICE_ADD(_basetag ":" _tag, NETLIST_LOGIC_INPUT, 0)                      \
	netlist_mame_logic_input_t::static_set_params(*device, _name, _shift);

#define MCFG_NETLIST_INT_INPUT(_basetag, _tag, _name, _shift, _mask)                \
	MCFG_DEVICE_ADD(_basetag ":" _tag, NETLIST_INT_INPUT, 0)                      \
	netlist_mame_int_input_t::static_set_params(*device, _name, _mask, _shift);

#define MCFG_NETLIST_STREAM_INPUT(_basetag, _chan, _name)                           \
	MCFG_DEVICE_ADD(_basetag ":cin" # _chan, NETLIST_STREAM_INPUT, 0)               \
	netlist_mame_stream_input_t::static_set_params(*device, _chan, _name);

#define MCFG_NETLIST_STREAM_OUTPUT(_basetag, _chan, _name)                          \
	MCFG_DEVICE_ADD(_basetag ":cout" # _chan, NETLIST_STREAM_OUTPUT, 0)             \
	netlist_mame_stream_output_t::static_set_params(*device, _chan, _name);


#define NETLIST_LOGIC_PORT_CHANGED(_base, _tag)                                     \
	PORT_CHANGED_MEMBER(_base ":" _tag, netlist_mame_logic_input_t, input_changed, 0)

#define NETLIST_INT_PORT_CHANGED(_base, _tag)                                     \
	PORT_CHANGED_MEMBER(_base ":" _tag, netlist_mame_logic_input_t, input_changed, 0)

#define NETLIST_ANALOG_PORT_CHANGED(_base, _tag)                                    \
	PORT_CHANGED_MEMBER(_base ":" _tag, netlist_mame_analog_input_t, input_changed, 0)


#define MEMREGION_SOURCE(_name) \
		setup.register_source(plib::make_unique_base<netlist::source_t, netlist_source_memregion_t>(_name));

#define NETDEV_ANALOG_CALLBACK_MEMBER(_name) \
	void _name(const double data, const attotime &time)



// ----------------------------------------------------------------------------------------
// Extensions to interface netlist with MAME code ....
// ----------------------------------------------------------------------------------------

class netlist_source_memregion_t : public netlist::source_t
{
public:
	netlist_source_memregion_t(pstring name)
	: netlist::source_t(), m_name(name)
	{
	}

	bool parse(netlist::setup_t &setup, const pstring &name) override;
private:
	pstring m_name;
};

class netlist_mame_device_t;

class netlist_mame_t : public netlist::netlist_t
{
public:

	netlist_mame_t(netlist_mame_device_t &parent, const pstring &aname)
	: netlist::netlist_t(aname),
		m_parent(parent)
	{}
	virtual ~netlist_mame_t() { };

	inline running_machine &machine();

	netlist_mame_device_t &parent() { return m_parent; }

protected:

	void vlog(const plib::plog_level &l, const pstring &ls) const override;

private:
	netlist_mame_device_t &m_parent;
};

// ----------------------------------------------------------------------------------------
// netlist_mame_device_t
// ----------------------------------------------------------------------------------------

class netlist_mame_device_t : public device_t
{
public:

	// construction/destruction
	netlist_mame_device_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	netlist_mame_device_t(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *file);
	virtual ~netlist_mame_device_t() { pstring::resetmem(); }

	static void static_set_constructor(device_t &device, void (*setup_func)(netlist::setup_t &));

	ATTR_HOT inline netlist::setup_t &setup() { return *m_setup; }
	ATTR_HOT inline netlist_mame_t &netlist() { return *m_netlist; }

	ATTR_HOT inline const netlist::netlist_time last_time_update() { return m_old; }
	ATTR_HOT void update_time_x();
	ATTR_HOT void check_mame_abort_slice();

	int m_icount;

protected:
	// Custom to netlist ...

	virtual void nl_register_devices() { };

	// device_t overrides
	virtual void device_config_complete() override;
	virtual void device_start() override;
	virtual void device_stop() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;
	virtual void device_pre_save() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	//virtual void device_debug_setup();
	virtual void device_clock_changed() override;

	netlist::netlist_time m_div;

private:
	void save_state();

	/* timing support here - so sound can hijack it ... */
	netlist::netlist_time        m_rem;
	netlist::netlist_time        m_old;

	netlist_mame_t *    m_netlist;
	netlist::setup_t *   m_setup;

	void (*m_setup_func)(netlist::setup_t &);
};

inline running_machine &netlist_mame_t::machine()
{
	return m_parent.machine();
}

// ----------------------------------------------------------------------------------------
// netlist_mame_cpu_device_t
// ----------------------------------------------------------------------------------------

class netlist_mame_cpu_device_t : public netlist_mame_device_t,
									public device_execute_interface,
									public device_state_interface,
									public device_disasm_interface,
									public device_memory_interface
{
public:

	// construction/destruction
	netlist_mame_cpu_device_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~netlist_mame_cpu_device_t() {}
protected:
	// netlist_mame_device_t
	virtual void nl_register_devices() override;

	// device_t overrides

	//virtual void device_config_complete();
	virtual void device_start() override;
	//virtual void device_stop();
	//virtual void device_reset();
	//virtual void device_post_load();
	//virtual void device_pre_save();
	//virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

	// device_execute_interface overrides

	virtual uint64_t execute_clocks_to_cycles(uint64_t clocks) const override;
	virtual uint64_t execute_cycles_to_clocks(uint64_t cycles) const override;

	ATTR_HOT virtual void execute_run() override;

	// device_disasm_interface overrides
	ATTR_COLD virtual uint32_t disasm_min_opcode_bytes() const override { return 1; }
	ATTR_COLD virtual uint32_t disasm_max_opcode_bytes() const override { return 1; }
	ATTR_COLD virtual offs_t disasm_disassemble(std::ostream &stream, offs_t pc, const uint8_t *oprom, const uint8_t *opram, uint32_t options) override;

	// device_memory_interface overrides

	address_space_config m_program_config;

	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const override
	{
		switch (spacenum)
		{
			case AS_PROGRAM: return &m_program_config;
			case AS_IO:      return nullptr;
			default:         return nullptr;
		}
	}

	//  device_state_interface overrides

	virtual void state_string_export(const device_state_entry &entry, std::string &str) const override
	{
		if (entry.index() >= 0)
		{
			if (entry.index() & 1)
				str = string_format("%10.6f", *((double *)entry.dataptr()));
			else
				str = string_format("%d", *((netlist_sig_t *)entry.dataptr()));
		}
	}

private:

	int m_genPC;

};

class nld_sound_out;
class nld_sound_in;

// ----------------------------------------------------------------------------------------
// netlist_mame_sound_device_t
// ----------------------------------------------------------------------------------------

class netlist_mame_sound_device_t : public netlist_mame_device_t,
									public device_sound_interface
{
public:

	// construction/destruction
	netlist_mame_sound_device_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~netlist_mame_sound_device_t() {}

	inline sound_stream *get_stream() { return m_stream; }


	// device_sound_interface overrides

	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples) override;

protected:
	// netlist_mame_device_t
	virtual void nl_register_devices() override;

	// device_t overrides

	//virtual void device_config_complete();
	virtual void device_start() override;
	//virtual void device_stop();
	//virtual void device_reset();
	//virtual void device_post_load();
	//virtual void device_pre_save();
	//virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

private:

	static const int MAX_OUT = 10;
	nld_sound_out *m_out[MAX_OUT];
	nld_sound_in *m_in;
	sound_stream *m_stream;
	int m_num_inputs;
	int m_num_outputs;

};

// ----------------------------------------------------------------------------------------
// netlist_mame_sub_interface
// ----------------------------------------------------------------------------------------

class netlist_mame_sub_interface
{
public:
	// construction/destruction
	netlist_mame_sub_interface(device_t &aowner)
	: m_offset(0.0), m_mult(1.0)
	{
		m_owner = dynamic_cast<netlist_mame_device_t *>(&aowner);
		m_sound = dynamic_cast<netlist_mame_sound_device_t *>(&aowner);
	}
	virtual ~netlist_mame_sub_interface() { }

	virtual void custom_netlist_additions(netlist::setup_t &setup) { }

	inline netlist_mame_device_t &nl_owner() const { return *m_owner; }

	inline bool is_sound_device() const { return (m_sound != nullptr); }

	inline void update_to_current_time()
	{
		m_sound->get_stream()->update();
	}

	static void static_set_mult_offset(device_t &device, const double mult, const double offset);

protected:
	double m_offset;
	double m_mult;

private:
	netlist_mame_device_t *m_owner;
	netlist_mame_sound_device_t *m_sound;
};

// ----------------------------------------------------------------------------------------
// netlist_mame_analog_input_t
// ----------------------------------------------------------------------------------------

class netlist_mame_analog_input_t : public device_t,
									public netlist_mame_sub_interface
{
public:

	// construction/destruction
	netlist_mame_analog_input_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~netlist_mame_analog_input_t() { }

	static void static_set_name(device_t &device, const char *param_name);

	inline void write(const double val)
	{
		if (is_sound_device())
		{
			update_to_current_time();
			m_param->setTo(val * m_mult + m_offset);
		}
		else
		{
			// FIXME: use device timer ....
			m_param->setTo(val * m_mult + m_offset);
		}
	}

	inline DECLARE_INPUT_CHANGED_MEMBER(input_changed)
	{
		if (m_auto_port)
			write(((double) newval - (double) field.minval())/((double) (field.maxval()-field.minval()) ) );
		else
			write(newval);
	}
	inline DECLARE_WRITE_LINE_MEMBER(write_line)       { write(state);  }
	inline DECLARE_WRITE8_MEMBER(write8)               { write(data);   }
	inline DECLARE_WRITE16_MEMBER(write16)             { write(data);   }
	inline DECLARE_WRITE32_MEMBER(write32)             { write(data);   }
	inline DECLARE_WRITE64_MEMBER(write64)             { write(data);   }

protected:
	// device-level overrides
	virtual void device_start() override;

private:
	netlist::param_double_t *m_param;
	bool   m_auto_port;
	pstring m_param_name;
};

// ----------------------------------------------------------------------------------------
// netlist_mame_analog_output_t
// ----------------------------------------------------------------------------------------

typedef device_delegate<void (const double, const attotime &)> netlist_analog_output_delegate;

class netlist_mame_analog_output_t : public device_t,
										public netlist_mame_sub_interface
{
public:

	// construction/destruction
	netlist_mame_analog_output_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~netlist_mame_analog_output_t() { }

	static void static_set_params(device_t &device, const char *in_name, netlist_analog_output_delegate adelegate);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void custom_netlist_additions(netlist::setup_t &setup) override;

private:
	pstring m_in;
	netlist_analog_output_delegate m_delegate;
};


// ----------------------------------------------------------------------------------------
// netlist_mame_int_input_t
// ----------------------------------------------------------------------------------------

class netlist_mame_int_input_t :  public device_t,
									public netlist_mame_sub_interface
{
public:

	// construction/destruction
	netlist_mame_int_input_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~netlist_mame_int_input_t() { }

	static void static_set_params(device_t &device, const char *param_name, const uint32_t mask, const uint32_t shift);

	inline void write(const uint32_t val)
	{
		const uint32_t v = (val >> m_shift) & m_mask;
		if (v != (*m_param)())
			synchronize(0, v);
	}

	inline DECLARE_INPUT_CHANGED_MEMBER(input_changed) { write(newval); }
	DECLARE_WRITE_LINE_MEMBER(write_line)       { write(state);  }
	DECLARE_WRITE8_MEMBER(write8)               { write(data);   }
	DECLARE_WRITE16_MEMBER(write16)             { write(data);   }
	DECLARE_WRITE32_MEMBER(write32)             { write(data);   }
	DECLARE_WRITE64_MEMBER(write64)             { write(data);   }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override
	{
		if (is_sound_device())
			update_to_current_time();
		m_param->setTo(param);
	}

private:
	netlist::param_int_t *m_param;
	uint32_t m_mask;
	uint32_t m_shift;
	pstring m_param_name;
};

// ----------------------------------------------------------------------------------------
// netlist_mame_logic_input_t
// ----------------------------------------------------------------------------------------

class netlist_mame_logic_input_t :  public device_t,
									public netlist_mame_sub_interface
{
public:

	// construction/destruction
	netlist_mame_logic_input_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~netlist_mame_logic_input_t() { }

	static void static_set_params(device_t &device, const char *param_name, const uint32_t shift);

	inline void write(const uint32_t val)
	{
		const uint32_t v = (val >> m_shift) & 1;
		if (v != (*m_param)())
			synchronize(0, v);
	}

	inline DECLARE_INPUT_CHANGED_MEMBER(input_changed) { write(newval); }
	DECLARE_WRITE_LINE_MEMBER(write_line)       { write(state);  }
	DECLARE_WRITE8_MEMBER(write8)               { write(data);   }
	DECLARE_WRITE16_MEMBER(write16)             { write(data);   }
	DECLARE_WRITE32_MEMBER(write32)             { write(data);   }
	DECLARE_WRITE64_MEMBER(write64)             { write(data);   }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override
	{
		if (is_sound_device())
			update_to_current_time();
		m_param->setTo(param);
	}

private:
	netlist::param_logic_t *m_param;
	uint32_t m_shift;
	pstring m_param_name;
};

// ----------------------------------------------------------------------------------------
// netlist_mame_stream_input_t
// ----------------------------------------------------------------------------------------

class netlist_mame_stream_input_t :  public device_t,
										public netlist_mame_sub_interface
{
public:

	// construction/destruction
	netlist_mame_stream_input_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~netlist_mame_stream_input_t() { }

	static void static_set_params(device_t &device, int channel, const char *param_name);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void custom_netlist_additions(netlist::setup_t &setup) override;
private:
	uint32_t m_channel;
	pstring m_param_name;
};

// ----------------------------------------------------------------------------------------
// netlist_mame_stream_output_t
// ----------------------------------------------------------------------------------------

class netlist_mame_stream_output_t :  public device_t,
										public netlist_mame_sub_interface
{
public:

	// construction/destruction
	netlist_mame_stream_output_t(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~netlist_mame_stream_output_t() { }

	static void static_set_params(device_t &device, int channel, const char *out_name);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void custom_netlist_additions(netlist::setup_t &setup) override;
private:
	uint32_t m_channel;
	pstring m_out_name;
};
// ----------------------------------------------------------------------------------------
// netdev_callback
// ----------------------------------------------------------------------------------------

class NETLIB_NAME(analog_callback) : public netlist::device_t
{
public:
	NETLIB_NAME(analog_callback)(netlist::netlist_t &anetlist, const pstring &name)
	: device_t(anetlist, name)
	, m_in(*this, "IN")
	, m_cpu_device(nullptr)
	, m_last(*this, "m_last", 0)
	{
		m_cpu_device = downcast<netlist_mame_cpu_device_t *>(&downcast<netlist_mame_t &>(netlist()).parent());
	}

	ATTR_COLD void reset() override
	{
		m_last = 0.0;
	}

	ATTR_COLD void register_callback(netlist_analog_output_delegate callback)
	{
		m_callback = callback;
	}

	NETLIB_UPDATEI()
	{
		nl_double cur = m_in();

		// FIXME: make this a parameter
		// avoid calls due to noise
		if (std::fabs(cur - m_last) > 1e-6)
		{
			m_cpu_device->update_time_x();
			m_callback(cur, m_cpu_device->local_time());
			m_cpu_device->check_mame_abort_slice();
			m_last = cur;
		}
	}

private:
	netlist::analog_input_t m_in;
	netlist_analog_output_delegate m_callback;
	netlist_mame_cpu_device_t *m_cpu_device;
	netlist::state_var<nl_double> m_last;
};

// ----------------------------------------------------------------------------------------
// sound_out
// ----------------------------------------------------------------------------------------

class NETLIB_NAME(sound_out) : public netlist::device_t
{
public:
	NETLIB_NAME(sound_out)(netlist::netlist_t &anetlist, const pstring &name)
		: netlist::device_t(anetlist, name)
		, m_channel(*this, "CHAN", 0)
		, m_mult(*this, "MULT", 1000.0)
		, m_offset(*this, "OFFSET", 0.0)
		, m_sample(netlist::netlist_time::from_hz(1)) //sufficiently big enough
		, m_in(*this, "IN")
		, m_last_buffer(*this, "m_last_buffer", netlist::netlist_time::zero())
	{
	}

	static const int BUFSIZE = 2048;

	ATTR_COLD void reset() override
	{
		m_cur = 0.0;
		m_last_pos = 0;
		m_last_buffer = netlist::netlist_time::zero();
	}

	ATTR_HOT void sound_update(const netlist::netlist_time &upto)
	{
		int pos = (upto - m_last_buffer) / m_sample;
		if (pos >= BUFSIZE)
			netlist().log().fatal("sound {1}: exceeded BUFSIZE\n", name().cstr());
		while (m_last_pos < pos )
		{
			m_buffer[m_last_pos++] = (stream_sample_t) m_cur;
		}
	}

	NETLIB_UPDATEI()
	{
		nl_double val = m_in() * m_mult() + m_offset();
		sound_update(netlist().time());
		/* ignore spikes */
		if (std::abs(val) < 32767.0)
			m_cur = val;
		else if (val > 0.0)
			m_cur = 32767.0;
		else
			m_cur = -32767.0;

	}

public:
	ATTR_HOT void buffer_reset(const netlist::netlist_time &upto)
	{
		m_last_pos = 0;
		m_last_buffer = upto;
		m_cur = 0.0;
	}

	netlist::param_int_t m_channel;
	netlist::param_double_t m_mult;
	netlist::param_double_t m_offset;
	stream_sample_t *m_buffer;
	netlist::netlist_time m_sample;

private:
	netlist::analog_input_t m_in;
	double m_cur;
	int m_last_pos;
	netlist::state_var<netlist::netlist_time> m_last_buffer;
};

// ----------------------------------------------------------------------------------------
// sound_in
// ----------------------------------------------------------------------------------------

class NETLIB_NAME(sound_in) : public netlist::device_t
{
public:
	NETLIB_NAME(sound_in)(netlist::netlist_t &anetlist, const pstring &name)
	: netlist::device_t(anetlist, name)
	, m_feedback(*this, "FB") // clock part
	, m_Q(*this, "Q")
	{
		connect_late(m_feedback, m_Q);
		m_inc = netlist::netlist_time::from_nsec(1);


		for (int i = 0; i < MAX_INPUT_CHANNELS; i++)
		{
			m_param_name[i] = std::make_unique<netlist::param_str_t>(*this, plib::pfmt("CHAN{1}")(i), "");
			m_param_mult[i] = std::make_unique<netlist::param_double_t>(*this, plib::pfmt("MULT{1}")(i), 1.0);
			m_param_offset[i] = std::make_unique<netlist::param_double_t>(*this, plib::pfmt("OFFSET{1}")(i), 0.0);
		}
		m_num_channel = 0;
	}

	static const int MAX_INPUT_CHANNELS = 10;

	ATTR_COLD void reset() override
	{
		m_pos = 0;
		for (auto & elem : m_buffer)
			elem = nullptr;
	}

	ATTR_COLD int resolve()
	{
		m_pos = 0;
		for (int i = 0; i < MAX_INPUT_CHANNELS; i++)
		{
			if ((*m_param_name[i])() != pstring(""))
			{
				if (i != m_num_channel)
					netlist().log().fatal("sound input numbering has to be sequential!");
				m_num_channel++;
				m_param[i] = dynamic_cast<netlist::param_double_t *>(setup().find_param((*m_param_name[i])(), true));
			}
		}
		return m_num_channel;
	}

	NETLIB_UPDATEI()
	{
		for (int i=0; i<m_num_channel; i++)
		{
			if (m_buffer[i] == nullptr)
				break; // stop, called outside of stream_update
			const nl_double v = m_buffer[i][m_pos];
			m_param[i]->setTo(v * (*m_param_mult[i])() + (*m_param_offset[i])());
		}
		m_pos++;
		m_Q.push(!m_Q.net().new_Q(), m_inc  );
	}

public:
	ATTR_HOT void buffer_reset()
	{
		m_pos = 0;
	}

	std::unique_ptr<netlist::param_str_t> m_param_name[MAX_INPUT_CHANNELS];
	netlist::param_double_t *m_param[MAX_INPUT_CHANNELS];
	stream_sample_t *m_buffer[MAX_INPUT_CHANNELS];
	std::unique_ptr<netlist::param_double_t> m_param_mult[MAX_INPUT_CHANNELS];
	std::unique_ptr<netlist::param_double_t> m_param_offset[MAX_INPUT_CHANNELS];
	netlist::netlist_time m_inc;

private:
	netlist::logic_input_t m_feedback;
	netlist::logic_output_t m_Q;

	int m_pos;
	int m_num_channel;
};

// device type definition
extern const device_type NETLIST_CORE;
extern const device_type NETLIST_CPU;
extern const device_type NETLIST_SOUND;
extern const device_type NETLIST_ANALOG_INPUT;
extern const device_type NETLIST_LOGIC_INPUT;
extern const device_type NETLIST_INT_INPUT;

extern const device_type NETLIST_ANALOG_OUTPUT;
extern const device_type NETLIST_STREAM_INPUT;
extern const device_type NETLIST_STREAM_OUTPUT;

#endif
