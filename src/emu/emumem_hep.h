// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

// handler_entry_read_passthrough/handler_entry_write_passthrough

// parent class for handlers which want to tap the access and usually pass it on to another handler

template<int Width, int AddrShift, int Endian> class handler_entry_read_passthrough : public handler_entry_read<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;

	handler_entry_read_passthrough(address_space *space, memory_passthrough_handler &mph) : handler_entry_read<Width, AddrShift, Endian>(space, handler_entry::F_PASSTHROUGH), m_mph(mph), m_next(nullptr) {}
	~handler_entry_read_passthrough();

	virtual handler_entry_read_passthrough<Width, AddrShift, Endian> *instantiate(handler_entry_read<Width, AddrShift, Endian> *next) const = 0;

	handler_entry_read<Width, AddrShift, Endian> *get_subhandler() const { return m_next; }

	void detach(const std::unordered_set<handler_entry *> &handlers) override;

protected:
	memory_passthrough_handler &m_mph;
	handler_entry_read<Width, AddrShift, Endian> *m_next;

	handler_entry_read_passthrough(address_space *space, memory_passthrough_handler &mph, handler_entry_read<Width, AddrShift, Endian> *next) : handler_entry_read<Width, AddrShift, Endian>(space, handler_entry::F_PASSTHROUGH), m_mph(mph), m_next(next) { next->ref(); mph.add_handler(this); }
};

template<int Width, int AddrShift, int Endian> class handler_entry_write_passthrough : public handler_entry_write<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;

	handler_entry_write_passthrough(address_space *space, memory_passthrough_handler &mph) : handler_entry_write<Width, AddrShift, Endian>(space, handler_entry::F_PASSTHROUGH), m_mph(mph), m_next(nullptr) {}
	~handler_entry_write_passthrough();

	virtual handler_entry_write_passthrough<Width, AddrShift, Endian> *instantiate(handler_entry_write<Width, AddrShift, Endian> *next) const = 0;

	handler_entry_write<Width, AddrShift, Endian> *get_subhandler() const { return m_next; }

	void detach(const std::unordered_set<handler_entry *> &handlers) override;

protected:
	memory_passthrough_handler &m_mph;
	handler_entry_write<Width, AddrShift, Endian> *m_next;

	handler_entry_write_passthrough(address_space *space, memory_passthrough_handler &mph, handler_entry_write<Width, AddrShift, Endian> *next) : handler_entry_write<Width, AddrShift, Endian>(space, handler_entry::F_PASSTHROUGH), m_mph(mph), m_next(next) { next->ref(); mph.add_handler(this); }
};
