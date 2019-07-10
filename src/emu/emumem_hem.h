// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

// handler_entry_read_memory/handler_entry_write_memory

// Accesses fixed memory (non-banked rom or ram)

template<int Width, int AddrShift, int Endian> class handler_entry_read_memory : public handler_entry_read_address<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_read_address<Width, AddrShift, Endian>;

	handler_entry_read_memory(address_space *space) : handler_entry_read_address<Width, AddrShift, Endian>(space, 0) {}
	~handler_entry_read_memory() = default;

	uX read(offs_t offset, uX mem_mask) override;
	void *get_ptr(offs_t offset) const override;

	inline void set_base(uX *base) { m_base = base; }

	std::string name() const override;

private:
	uX *m_base;
};

template<int Width, int AddrShift, int Endian> class handler_entry_write_memory : public handler_entry_write_address<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_write_address<Width, AddrShift, Endian>;

	handler_entry_write_memory(address_space *space) : handler_entry_write_address<Width, AddrShift, Endian>(space, 0) {}
	~handler_entry_write_memory() = default;

	void write(offs_t offset, uX data, uX mem_mask) override;
	void *get_ptr(offs_t offset) const override;

	inline void set_base(uX *base) { m_base = base; }

	std::string name() const override;

private:
	uX *m_base;
};


// handler_entry_read_memory_bank/handler_entry_write_memory_bank

// Accesses banked memory, associated to a memory_bank

template<int Width, int AddrShift, int Endian> class handler_entry_read_memory_bank : public handler_entry_read_address<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_read_address<Width, AddrShift, Endian>;

	handler_entry_read_memory_bank(address_space *space, memory_bank &bank) : handler_entry_read_address<Width, AddrShift, Endian>(space, 0), m_bank(bank) {}
	~handler_entry_read_memory_bank() = default;

	uX read(offs_t offset, uX mem_mask) override;
	void *get_ptr(offs_t offset) const override;

	std::string name() const override;

private:
	memory_bank &m_bank;
};

template<int Width, int AddrShift, int Endian> class handler_entry_write_memory_bank : public handler_entry_write_address<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_write_address<Width, AddrShift, Endian>;

	handler_entry_write_memory_bank(address_space *space, memory_bank &bank) : handler_entry_write_address<Width, AddrShift, Endian>(space, 0), m_bank(bank) {}
	~handler_entry_write_memory_bank() = default;

	void write(offs_t offset, uX data, uX mem_mask) override;
	void *get_ptr(offs_t offset) const override;

	std::string name() const override;

private:
	memory_bank &m_bank;
};
