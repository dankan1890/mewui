// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

// handler_entry_write_dispatch

// dispatches an access among multiple handlers indexed on part of the address

template<int HighBits, int Width, int AddrShift, int Endian> class handler_entry_write_dispatch : public handler_entry_write<Width, AddrShift, Endian>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using inh = handler_entry_write<Width, AddrShift, Endian>;
	using mapping = typename inh::mapping;

	handler_entry_write_dispatch(address_space *space, const handler_entry::range &init, handler_entry_write<Width, AddrShift, Endian> *handler);
	~handler_entry_write_dispatch();

	void write(offs_t offset, uX data, uX mem_mask) override;
	void *get_ptr(offs_t offset) const override;
	void lookup(offs_t address, offs_t &start, offs_t &end, handler_entry_write<Width, AddrShift, Endian> *&handler) const override;

	void dump_map(std::vector<memory_entry> &map) const override;

	std::string name() const override;

	void populate_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write<Width, AddrShift, Endian> *handler) override;
	void populate_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write<Width, AddrShift, Endian> *handler) override;
	void populate_mismatched_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, u8 rkey, std::vector<mapping> &mappings) override;
	void populate_mismatched_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, std::vector<mapping> &mappings) override;
	void populate_passthrough_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings) override;
	void populate_passthrough_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings) override;
	void detach(const std::unordered_set<handler_entry *> &handlers) override;
	void range_cut_before(offs_t address, int start = COUNT);
	void range_cut_after(offs_t address, int start = -1);

	void enumerate_references(handler_entry::reflist &refs) const override;

protected:
	static constexpr u32    LowBits  = emu::detail::handler_entry_dispatch_lowbits(HighBits, Width, AddrShift);
	static constexpr u32    BITCOUNT = HighBits > LowBits ? HighBits - LowBits : 0;
	static constexpr u32    COUNT    = 1 << BITCOUNT;
	static constexpr offs_t BITMASK  = make_bitmask<offs_t>(BITCOUNT);
	static constexpr offs_t LOWMASK  = make_bitmask<offs_t>(LowBits);
	static constexpr offs_t HIGHMASK = make_bitmask<offs_t>(HighBits) ^ LOWMASK;
	static constexpr offs_t UPMASK   = ~make_bitmask<offs_t>(HighBits);

	handler_entry_write<Width, AddrShift, Endian> *m_dispatch[COUNT];
	handler_entry::range m_ranges[COUNT];

private:
	void populate_nomirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write<Width, AddrShift, Endian> *handler);
	void populate_mirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write<Width, AddrShift, Endian> *handler);

	void populate_mismatched_nomirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, u8 rkey, std::vector<mapping> &mappings);
	void populate_mismatched_mirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, std::vector<mapping> &mappings);
	void mismatched_patch(const memory_units_descriptor<Width, AddrShift, Endian> &descriptor, u8 rkey, std::vector<mapping> &mappings, handler_entry_write<Width, AddrShift, Endian> *&target);

	void populate_passthrough_nomirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings);
	void populate_passthrough_mirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings);
	void passthrough_patch(handler_entry_write_passthrough<Width, AddrShift, Endian> *handler, std::vector<mapping> &mappings, handler_entry_write<Width, AddrShift, Endian> *&target);
};
