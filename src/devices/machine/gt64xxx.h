// license:BSD-3-Clause
// copyright-holders: Aaron Giles, Ted Green
// Galileo GT-64xxx System Controller
// Skeleton code based off seattle machine driver.
// TODO:
// Need PCI to be able to have a target delay (pci bus stall) a dma transfer
// Configurable byte swapping on cpu and pci busses.

#ifndef GT64XXX_H
#define GT64XXX_H

#include "pci.h"
#include "cpu/mips/mips3.h"

// Supports R4600/4650/4700/R5000 CPUs
#define MCFG_GT64010_ADD(_tag,  _cpu_tag, _clock, _irq_num) \
	MCFG_PCI_HOST_ADD(_tag, GT64XXX, 0x014611ab, 0x03, 0x00000000) \
	downcast<gt64xxx_device *>(device)->set_cpu_tag(_cpu_tag); \
	downcast<gt64xxx_device *>(device)->set_clock(_clock); \
	downcast<gt64xxx_device *>(device)->set_irq_num(_irq_num);

// Supports the following 32-bit bus CPUs:
// IDT RC4640 and RC4650 (in 32-bit mode)
// QED RM523X
// NEC/Toshiba VR4300
#define MCFG_GT64111_ADD(_tag,  _cpu_tag, _clock, _irq_num) \
	MCFG_PCI_DEVICE_ADD(_tag, GT64XXX, 0x414611ab, 0x10, 0x058000, 0x00000000) \
	downcast<gt64xxx_device *>(device)->set_cpu_tag(_cpu_tag); \
	downcast<gt64xxx_device *>(device)->set_clock(_clock); \
	downcast<gt64xxx_device *>(device)->set_irq_num(_irq_num);

#define MCFG_GT64XXX_SET_BE_CPU(_be) \
	downcast<gt64xxx_device *>(device)->set_be(_be);

#define MCFG_GT64XXX_IRQ_ADD(_irq_num) \
	downcast<gt64xxx_device *>(device)->set_irq_info(_irq_num);

#define MCFG_GT64XXX_SET_CS(_cs_num, _map) \
	downcast<gt64xxx_device *>(device)->set_cs_map(_cs_num, ADDRESS_MAP_NAME(_map), #_map, owner);

/*************************************
 *
 *  Galileo constants
 *
 *************************************/

#define TIMER_PERIOD            attotime::from_hz(m_clock)
#define PCI_BUS_CLOCK        33000000
// Number of dma words to transfer at a time, real hardware configurable between 8-32
#define DMA_BURST_SIZE       32
#define DMA_TIMER_PERIOD     attotime::from_hz(PCI_BUS_CLOCK / 48)

/* Galileo registers - 0x000-0x3ff */
#define GREG_CPU_CONFIG     (0x000/4)
#define GREG_RAS_1_0_LO     (0x008/4)
#define GREG_RAS_1_0_HI     (0x010/4)
#define GREG_RAS_3_2_LO     (0x018/4)
#define GREG_RAS_3_2_HI     (0x020/4)
#define GREG_CS_2_0_LO      (0x028/4)
#define GREG_CS_2_0_HI      (0x030/4)
#define GREG_CS_3_BOOT_LO   (0x038/4)
#define GREG_CS_3_BOOT_HI   (0x040/4)
#define GREG_PCI_IO_LO      (0x048/4)
#define GREG_PCI_IO_HI      (0x050/4)
#define GREG_PCI_MEM0_LO    (0x058/4)
#define GREG_PCI_MEM0_HI    (0x060/4)
#define GREG_INTERNAL_SPACE (0x068/4)
#define GREG_BUSERR_LO      (0x070/4)
#define GREG_BUSERR_HI      (0x078/4)
// GT-64111 only
#define GREG_PCI_MEM1_LO    (0x080/4)
#define GREG_PCI_MEM1_HI    (0x088/4)

/* Galileo registers - 0x400-0x7ff */
#define GREG_RAS0_LO        (0x400/4)
#define GREG_RAS0_HI        (0x404/4)
#define GREG_RAS1_LO        (0x408/4)
#define GREG_RAS1_HI        (0x40c/4)
#define GREG_RAS2_LO        (0x410/4)
#define GREG_RAS2_HI        (0x414/4)
#define GREG_RAS3_LO        (0x418/4)
#define GREG_RAS3_HI        (0x41c/4)
#define GREG_CS0_LO         (0x420/4)
#define GREG_CS0_HI         (0x424/4)
#define GREG_CS1_LO         (0x428/4)
#define GREG_CS1_HI         (0x42c/4)
#define GREG_CS2_LO         (0x430/4)
#define GREG_CS2_HI         (0x434/4)
#define GREG_CS3_LO         (0x438/4)
#define GREG_CS3_HI         (0x43c/4)
#define GREG_CSBOOT_LO      (0x440/4)
#define GREG_CSBOOT_HI      (0x444/4)
#define GREG_DRAM_CONFIG    (0x448/4)
#define GREG_DRAM_BANK0     (0x44c/4)
#define GREG_DRAM_BANK1     (0x450/4)
#define GREG_DRAM_BANK2     (0x454/4)
#define GREG_DRAM_BANK3     (0x458/4)
#define GREG_DEVICE_BANK0   (0x45c/4)
#define GREG_DEVICE_BANK1   (0x460/4)
#define GREG_DEVICE_BANK2   (0x464/4)
#define GREG_DEVICE_BANK3   (0x468/4)
#define GREG_DEVICE_BOOT    (0x46c/4)
#define GREG_ADDRESS_ERROR  (0x470/4)

/* Galileo registers - 0x800-0xbff */
#define GREG_DMA0_COUNT     (0x800/4)
#define GREG_DMA1_COUNT     (0x804/4)
#define GREG_DMA2_COUNT     (0x808/4)
#define GREG_DMA3_COUNT     (0x80c/4)
#define GREG_DMA0_SOURCE    (0x810/4)
#define GREG_DMA1_SOURCE    (0x814/4)
#define GREG_DMA2_SOURCE    (0x818/4)
#define GREG_DMA3_SOURCE    (0x81c/4)
#define GREG_DMA0_DEST      (0x820/4)
#define GREG_DMA1_DEST      (0x824/4)
#define GREG_DMA2_DEST      (0x828/4)
#define GREG_DMA3_DEST      (0x82c/4)
#define GREG_DMA0_NEXT      (0x830/4)
#define GREG_DMA1_NEXT      (0x834/4)
#define GREG_DMA2_NEXT      (0x838/4)
#define GREG_DMA3_NEXT      (0x83c/4)
#define GREG_DMA0_CONTROL   (0x840/4)
#define GREG_DMA1_CONTROL   (0x844/4)
#define GREG_DMA2_CONTROL   (0x848/4)
#define GREG_DMA3_CONTROL   (0x84c/4)
#define GREG_TIMER0_COUNT   (0x850/4)
#define GREG_TIMER1_COUNT   (0x854/4)
#define GREG_TIMER2_COUNT   (0x858/4)
#define GREG_TIMER3_COUNT   (0x85c/4)
#define GREG_DMA_ARBITER    (0x860/4)
#define GREG_TIMER_CONTROL  (0x864/4)

/* Galileo registers - 0xc00-0xfff */
#define GREG_PCI_COMMAND    (0xc00/4)
#define GREG_PCI_TIMEOUT    (0xc04/4)
#define GREG_PCI_RAS_1_0    (0xc08/4)
#define GREG_PCI_RAS_3_2    (0xc0c/4)
#define GREG_PCI_CS_2_0     (0xc10/4)
#define GREG_PCI_CS_3_BOOT  (0xc14/4)
#define GREG_INT_STATE      (0xc18/4)
#define GREG_INT_MASK       (0xc1c/4)
#define GREG_PCI_INT_MASK   (0xc24/4)
#define GREG_CONFIG_ADDRESS (0xcf8/4)
#define GREG_CONFIG_DATA    (0xcfc/4)

/* Galileo interrupts */
#define GINT_SUMMARY_SHIFT  (0)
#define GINT_MEMOUT_SHIFT   (1)
#define GINT_DMAOUT_SHIFT   (2)
#define GINT_CPUOUT_SHIFT   (3)
#define GINT_DMA0COMP_SHIFT (4)
#define GINT_DMA1COMP_SHIFT (5)
#define GINT_DMA2COMP_SHIFT (6)
#define GINT_DMA3COMP_SHIFT (7)
#define GINT_T0EXP_SHIFT    (8)
#define GINT_T1EXP_SHIFT    (9)
#define GINT_T2EXP_SHIFT    (10)
#define GINT_T3EXP_SHIFT    (11)
#define GINT_MASRDERR_SHIFT (12)
#define GINT_SLVWRERR_SHIFT (13)
#define GINT_MASWRERR_SHIFT (14)
#define GINT_SLVRDERR_SHIFT (15)
#define GINT_ADDRERR_SHIFT  (16)
#define GINT_MEMERR_SHIFT   (17)
#define GINT_MASABORT_SHIFT (18)
#define GINT_TARABORT_SHIFT (19)
#define GINT_RETRYCTR_SHIFT (20)


/*************************************
 *  Structures
 *************************************/
struct galileo_timer
{
	emu_timer *     timer;
	uint32_t          count;
	uint8_t           active;
};

struct galileo_addr_map
{
	uint32_t low_addr;
	uint32_t high_addr;
	address_space* space;
	galileo_addr_map() : low_addr(0xffffffff), high_addr(0x0) {}
};

struct galileo_device_map
{
	bool enable;
	const char *name;
	device_t *device;
	address_map_constructor map;
	galileo_device_map() : enable(false), device(nullptr) {}
};

class gt64xxx_device : public pci_host_device {
public:
	gt64xxx_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void reset_all_mappings() override;
	virtual void map_extra(uint64_t memory_window_start, uint64_t memory_window_end, uint64_t memory_offset, address_space *memory_space,
							uint64_t io_window_start, uint64_t io_window_end, uint64_t io_offset, address_space *io_space) override;

	void set_cpu_tag(const char *tag) { cpu_tag = tag;}
	void set_clock(const uint32_t clock) {m_clock = clock;}
	void set_be(const int be) {m_be = be;}
	void set_autoconfig(const int autoconfig) {m_autoconfig = autoconfig;}
	void set_irq_num(const int irq_num) {m_irq_num = irq_num;}
	virtual DECLARE_ADDRESS_MAP(config_map, 32) override;

	DECLARE_WRITE_LINE_MEMBER(pci_stall);

	// pci bus
	DECLARE_READ32_MEMBER(  pci_config_r);
	DECLARE_WRITE32_MEMBER( pci_config_w);

	// cpu bus
	DECLARE_READ32_MEMBER (cpu_if_r);
	DECLARE_WRITE32_MEMBER(cpu_if_w);

	DECLARE_READ32_MEMBER (master_mem0_r);
	DECLARE_WRITE32_MEMBER(master_mem0_w);

	DECLARE_READ32_MEMBER (master_mem1_r);
	DECLARE_WRITE32_MEMBER(master_mem1_w);

	DECLARE_READ32_MEMBER (master_io_r);
	DECLARE_WRITE32_MEMBER(master_io_w);

	// devices
	DECLARE_READ32_MEMBER (ras_0_r);
	DECLARE_WRITE32_MEMBER(ras_0_w);
	DECLARE_READ32_MEMBER(ras_1_r);
	DECLARE_WRITE32_MEMBER(ras_1_w);
	DECLARE_READ32_MEMBER(ras_2_r);
	DECLARE_WRITE32_MEMBER(ras_2_w);
	DECLARE_READ32_MEMBER(ras_3_r);
	DECLARE_WRITE32_MEMBER(ras_3_w);
	DECLARE_READ32_MEMBER (cs_0_r);
	DECLARE_WRITE32_MEMBER(cs_0_w);

	// Enums
	enum proc_addr_bank {ADDR_RAS1_0, ADDR_RAS3_2, ADDR_CS2_0, ADDR_CS3_BCS, ADDR_PCI_IO, ADDR_PCI_MEM0, ADDR_PCI_MEM1, ADDR_NUM};

	void set_cs_map(int id, address_map_constructor map, const char *name, device_t *device);

protected:
	address_space *m_cpu_space;
	virtual const address_space_config *memory_space_config(address_spacenum spacenum) const override;
	virtual void device_start() override;
	virtual void device_reset() override;


private:
	mips3_device *m_cpu;
	const char *cpu_tag;
	uint32_t m_clock;
	int m_be, m_autoconfig;
	int m_irq_num;

	int m_pci_stall_state;
	int m_retry_count;
	int m_pci_cpu_stalled;
	uint32_t m_cpu_stalled_offset;
	uint32_t m_cpu_stalled_data;
	uint32_t m_cpu_stalled_mem_mask;

	address_space_config m_mem_config, m_io_config;

	required_memory_region m_romRegion;
	optional_memory_region m_updateRegion;

	DECLARE_ADDRESS_MAP(cpu_map, 32);

	void map_cpu_space();

	uint32_t m_prev_addr;
	/* raw register data */
	uint32_t          m_reg[0xd00/4];

	/* timer info */
	galileo_timer   m_timer[4];
	TIMER_CALLBACK_MEMBER(timer_callback);

	/* DMA info */
	int8_t            m_dma_active;

	// Ram
	std::vector<uint32_t> m_ram[4];

	// Chip Select
	galileo_device_map m_cs_map[4];

	template<int id> void map_trampoline(::address_map &map) {
		m_cs_map[id].map(map);
	}
	template <typename T> void install_cs_map(offs_t addrstart, offs_t addrend, void (T::*map)(::address_map &map), const char *name) {
		//address_map_delegate delegate(map, name, static_cast<T *>(this));
		address_map_delegate delegate(map, name, static_cast<T *>(this));
		m_cpu_space->install_device_delegate(addrstart, addrend, *this, delegate);
	}

	void update_irqs();

	int m_last_dma;
	emu_timer* m_dma_timer;
	galileo_addr_map dma_addr_map[proc_addr_bank::ADDR_NUM];
	int dma_fetch_next(address_space &space, int which);
	TIMER_CALLBACK_MEMBER(perform_dma);
	address_space* dma_decode_address(uint32_t &addr);

};

extern const device_type GT64XXX;

#endif
