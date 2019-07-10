// license:BSD-3-Clause
// copyright-holders:David Haywood, ElSemi, Xing Xing

class pgm_arm_type2_state : public pgm_state
{
public:
	pgm_arm_type2_state(const machine_config &mconfig, device_type type, const char *tag)
		: pgm_state(mconfig, type, tag),
			m_arm_ram(*this, "arm_ram"),
			m_arm7_shareram(*this, "arm7_shareram"),
			m_prot(*this, "prot") {
	}
	void init_kov2();
	void init_kov2p();
	void init_martmast();
	void init_ddp2();
	void init_dw2001();
	void init_dwpc();

	void pgm_arm_type2(machine_config &config);
	void pgm_arm_type2_22m(machine_config &config);

private:
	// kov2
	u32        m_kov2_latchdata_68k_w;
	u32        m_kov2_latchdata_arm_w;

	required_shared_ptr<u32> m_arm_ram;
	required_shared_ptr<u32> m_arm7_shareram;

	optional_device<cpu_device> m_prot;

	u32 arm7_latch_arm_r(offs_t offset, u32 mem_mask = ~0);
	void arm7_latch_arm_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 arm7_shareram_r(offs_t offset, u32 mem_mask = ~0);
	void arm7_shareram_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u16 arm7_latch_68k_r(offs_t offset, u16 mem_mask = ~0);
	void arm7_latch_68k_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	u16 arm7_ram_r(offs_t offset, u16 mem_mask = ~0);
	void arm7_ram_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void kov2_latch_init();
	void martmast_arm_region_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	void kov2_arm_region_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	void kov2p_arm_region_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	DECLARE_READ32_MEMBER( ddp2_speedup_r );
	DECLARE_READ16_MEMBER( ddp2_main_speedup_r );
	void _55857F_arm7_map(address_map &map);
	void kov2_mem(address_map &map);
};

/* simulations (or missing) */
INPUT_PORTS_EXTERN( ddp2 );
INPUT_PORTS_EXTERN( kov2 );
INPUT_PORTS_EXTERN( martmast );
INPUT_PORTS_EXTERN( dw2001 );
