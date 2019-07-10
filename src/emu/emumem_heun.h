// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

// handler_entry_read_unmapped/handler_entry_write_unmapped

// Logs an unmapped access

template<int Width, int AddrShift, int Endian> class handler_entry_read_unmapped : public handler_entry_read<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_read<Width, AddrShift, Endian>;

	handler_entry_read_unmapped(address_space *space) : handler_entry_read<Width, AddrShift, Endian>(space, 0) {}
	~handler_entry_read_unmapped() = default;

	uX read(offs_t offset, uX mem_mask) override;

	std::string name() const override;
};

template<int Width, int AddrShift, int Endian> class handler_entry_write_unmapped : public handler_entry_write<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_write<Width, AddrShift, Endian>;

	handler_entry_write_unmapped(address_space *space) : handler_entry_write<Width, AddrShift, Endian>(space, 0) {}
	~handler_entry_write_unmapped() = default;

	void write(offs_t offset, uX data, uX mem_mask) override;

	std::string name() const override;
};



// handler_entry_read_nop/handler_entry_write_nop

// Drops an unmapped access silently

template<int Width, int AddrShift, int Endian> class handler_entry_read_nop : public handler_entry_read<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_read<Width, AddrShift, Endian>;

	handler_entry_read_nop(address_space *space) : handler_entry_read<Width, AddrShift, Endian>(space, 0) {}
	~handler_entry_read_nop() = default;

	uX read(offs_t offset, uX mem_mask) override;

	std::string name() const override;
};

template<int Width, int AddrShift, int Endian> class handler_entry_write_nop : public handler_entry_write<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_write<Width, AddrShift, Endian>;

	handler_entry_write_nop(address_space *space) : handler_entry_write<Width, AddrShift, Endian>(space, 0) {}
	~handler_entry_write_nop() = default;

	void write(offs_t offset, uX data, uX mem_mask) override;

	std::string name() const override;
};
