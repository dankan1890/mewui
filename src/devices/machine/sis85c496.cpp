// license:BSD-3-Clause
// copyright-holders:R. Belmont, O. Galibert
/***************************************************************************

    sis85c496.cpp - SiS 85C496/497 PCI chipset
    by R. Belmont (based on i82439hx.cpp/i82439tx.cpp by O. Galibert)

    Unlike Intel chipsets, the southbridge is not a PCI device;
    it connects via a proprietary bus to the northbridge, and the two
    chips appear to software/the BIOS as a single chip.  Thus we emulate
    them in a single file.

***************************************************************************/

#include "emu.h"
#include "sis85c496.h"
#include "bus/pc_kbd/keyboards.h"
#include "speaker.h"

DEFINE_DEVICE_TYPE(SIS85C496, sis85c496_host_device, "sis85c496", "SiS 85C496/497 chipset")

void sis85c496_host_device::config_map(address_map &map)
{
	pci_host_device::config_map(map);
	map(0x40, 0x40).rw(FUNC(sis85c496_host_device::dram_config_r), FUNC(sis85c496_host_device::dram_config_w));
	map(0x44, 0x45).rw(FUNC(sis85c496_host_device::shadow_config_r), FUNC(sis85c496_host_device::shadow_config_w));
	map(0x5a, 0x5a).rw(FUNC(sis85c496_host_device::smram_ctrl_r), FUNC(sis85c496_host_device::smram_ctrl_w));
	map(0xc8, 0xcb).rw(FUNC(sis85c496_host_device::mailbox_r), FUNC(sis85c496_host_device::mailbox_w));
	map(0xd0, 0xd0).rw(FUNC(sis85c496_host_device::bios_config_r), FUNC(sis85c496_host_device::bios_config_w));
	map(0xd1, 0xd1).rw(FUNC(sis85c496_host_device::isa_decoder_r), FUNC(sis85c496_host_device::isa_decoder_w));
}

void sis85c496_host_device::internal_io_map(address_map &map)
{
	pci_host_device::io_configuration_access_map(map);
	map(0x0000, 0x001f).rw("dma8237_1", FUNC(am9517a_device::read), FUNC(am9517a_device::write));
	map(0x0020, 0x003f).rw("pic8259_master", FUNC(pic8259_device::read), FUNC(pic8259_device::write));
	map(0x0040, 0x005f).rw("pit8254", FUNC(pit8254_device::read), FUNC(pit8254_device::write));
	map(0x0060, 0x0063).rw(FUNC(sis85c496_host_device::at_keybc_r), FUNC(sis85c496_host_device::at_keybc_w));
	map(0x0064, 0x0067).rw("keybc", FUNC(at_keyboard_controller_device::status_r), FUNC(at_keyboard_controller_device::command_w));
	map(0x0070, 0x007f).rw("rtc", FUNC(ds12885_device::read), FUNC(ds12885_device::write));
	map(0x0080, 0x009f).rw(FUNC(sis85c496_host_device::at_page8_r), FUNC(sis85c496_host_device::at_page8_w));
	map(0x00a0, 0x00bf).rw("pic8259_slave", FUNC(pic8259_device::read), FUNC(pic8259_device::write));
	map(0x00c0, 0x00df).rw(FUNC(sis85c496_host_device::at_dma8237_2_r), FUNC(sis85c496_host_device::at_dma8237_2_w));
	map(0x00e0, 0x00ef).noprw();
}

void sis85c496_host_device::device_add_mconfig(machine_config &config)
{
	PIT8254(config, m_pit8254, 0);
	m_pit8254->set_clk<0>(4772720/4); // heartbeat IRQ
	m_pit8254->out_handler<0>().set(FUNC(sis85c496_host_device::at_pit8254_out0_changed));
	m_pit8254->set_clk<1>(4772720/4); // DRAM refresh
	m_pit8254->out_handler<1>().set(FUNC(sis85c496_host_device::at_pit8254_out1_changed));
	m_pit8254->set_clk<2>(4772720/4); // PIO port C pin 4, and speaker polling enough
	m_pit8254->out_handler<2>().set(FUNC(sis85c496_host_device::at_pit8254_out2_changed));

	AM9517A(config, m_dma8237_1, XTAL(14'318'181)/3);
	m_dma8237_1->out_hreq_callback().set(m_dma8237_2, FUNC(am9517a_device::dreq0_w));
	m_dma8237_1->out_eop_callback().set(FUNC(sis85c496_host_device::at_dma8237_out_eop));
	m_dma8237_1->in_memr_callback().set(FUNC(sis85c496_host_device::pc_dma_read_byte));
	m_dma8237_1->out_memw_callback().set(FUNC(sis85c496_host_device::pc_dma_write_byte));
	m_dma8237_1->in_ior_callback<0>().set(FUNC(sis85c496_host_device::pc_dma8237_0_dack_r));
	m_dma8237_1->in_ior_callback<1>().set(FUNC(sis85c496_host_device::pc_dma8237_1_dack_r));
	m_dma8237_1->in_ior_callback<2>().set(FUNC(sis85c496_host_device::pc_dma8237_2_dack_r));
	m_dma8237_1->in_ior_callback<3>().set(FUNC(sis85c496_host_device::pc_dma8237_3_dack_r));
	m_dma8237_1->out_iow_callback<0>().set(FUNC(sis85c496_host_device::pc_dma8237_0_dack_w));
	m_dma8237_1->out_iow_callback<1>().set(FUNC(sis85c496_host_device::pc_dma8237_1_dack_w));
	m_dma8237_1->out_iow_callback<2>().set(FUNC(sis85c496_host_device::pc_dma8237_2_dack_w));
	m_dma8237_1->out_iow_callback<3>().set(FUNC(sis85c496_host_device::pc_dma8237_3_dack_w));
	m_dma8237_1->out_dack_callback<0>().set(FUNC(sis85c496_host_device::pc_dack0_w));
	m_dma8237_1->out_dack_callback<1>().set(FUNC(sis85c496_host_device::pc_dack1_w));
	m_dma8237_1->out_dack_callback<2>().set(FUNC(sis85c496_host_device::pc_dack2_w));
	m_dma8237_1->out_dack_callback<3>().set(FUNC(sis85c496_host_device::pc_dack3_w));

	AM9517A(config, m_dma8237_2, XTAL(14'318'181)/3);
	m_dma8237_2->out_hreq_callback().set(FUNC(sis85c496_host_device::pc_dma_hrq_changed));
	m_dma8237_2->in_memr_callback().set(FUNC(sis85c496_host_device::pc_dma_read_word));
	m_dma8237_2->out_memw_callback().set(FUNC(sis85c496_host_device::pc_dma_write_word));
	m_dma8237_2->in_ior_callback<1>().set(FUNC(sis85c496_host_device::pc_dma8237_5_dack_r));
	m_dma8237_2->in_ior_callback<2>().set(FUNC(sis85c496_host_device::pc_dma8237_6_dack_r));
	m_dma8237_2->in_ior_callback<3>().set(FUNC(sis85c496_host_device::pc_dma8237_7_dack_r));
	m_dma8237_2->out_iow_callback<1>().set(FUNC(sis85c496_host_device::pc_dma8237_5_dack_w));
	m_dma8237_2->out_iow_callback<2>().set(FUNC(sis85c496_host_device::pc_dma8237_6_dack_w));
	m_dma8237_2->out_iow_callback<3>().set(FUNC(sis85c496_host_device::pc_dma8237_7_dack_w));
	m_dma8237_2->out_dack_callback<0>().set(FUNC(sis85c496_host_device::pc_dack4_w));
	m_dma8237_2->out_dack_callback<1>().set(FUNC(sis85c496_host_device::pc_dack5_w));
	m_dma8237_2->out_dack_callback<2>().set(FUNC(sis85c496_host_device::pc_dack6_w));
	m_dma8237_2->out_dack_callback<3>().set(FUNC(sis85c496_host_device::pc_dack7_w));

	PIC8259(config, m_pic8259_master, 0);
	m_pic8259_master->out_int_callback().set(FUNC(sis85c496_host_device::cpu_int_w));
	m_pic8259_master->in_sp_callback().set_constant(1);
	m_pic8259_master->read_slave_ack_callback().set(FUNC(sis85c496_host_device::get_slave_ack));

	PIC8259(config, m_pic8259_slave, 0);
	m_pic8259_slave->out_int_callback().set(m_pic8259_master, FUNC(pic8259_device::ir2_w));
	m_pic8259_slave->in_sp_callback().set_constant(0);

	AT_KEYBOARD_CONTROLLER(config, m_keybc, XTAL(12'000'000));
	m_keybc->hot_res().set(FUNC(sis85c496_host_device::cpu_reset_w));
	m_keybc->gate_a20().set(FUNC(sis85c496_host_device::cpu_a20_w));
	m_keybc->kbd_irq().set("pic8259_master", FUNC(pic8259_device::ir1_w));
	m_keybc->kbd_clk().set("pc_kbdc", FUNC(pc_kbdc_device::clock_write_from_mb));
	m_keybc->kbd_data().set("pc_kbdc", FUNC(pc_kbdc_device::data_write_from_mb));

	PC_KBDC(config, m_pc_kbdc, 0);
	m_pc_kbdc->out_clock_cb().set("keybc", FUNC(at_keyboard_controller_device::kbd_clk_w));
	m_pc_kbdc->out_data_cb().set("keybc", FUNC(at_keyboard_controller_device::kbd_data_w));
	PC_KBDC_SLOT(config, "kbd", pc_at_keyboards, STR_KBD_MICROSOFT_NATURAL).set_pc_kbdc_slot(subdevice("pc_kbdc"));

	DS12885(config, m_ds12885);
	m_ds12885->irq().set(m_pic8259_slave, FUNC(pic8259_device::ir0_w));
	m_ds12885->set_century_index(0x32);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
	SPEAKER_SOUND(config, m_speaker).add_route(ALL_OUTPUTS, "mono", 0.50);
}


sis85c496_host_device::sis85c496_host_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: pci_host_device(mconfig, SIS85C496, tag, owner, clock),
	m_maincpu(*this, finder_base::DUMMY_TAG),
	m_pic8259_master(*this, "pic8259_master"),
	m_pic8259_slave(*this, "pic8259_slave"),
	m_dma8237_1(*this, "dma8237_1"),
	m_dma8237_2(*this, "dma8237_2"),
	m_pit8254(*this, "pit8254"),
	m_keybc(*this, "keybc"),
	m_speaker(*this, "speaker"),
	m_ds12885(*this, "rtc"),
	m_pc_kbdc(*this, "pc_kbdc"),
	m_at_spkrdata(0), m_pit_out2(0), m_dma_channel(0), m_cur_eop(false), m_dma_high_byte(0), m_at_speaker(0), m_refresh(false), m_channel_check(0), m_nmi_enabled(0)
{
}

void sis85c496_host_device::set_cpu_tag(const char *cpu_tag)
{
	m_maincpu.set_tag(cpu_tag);
}

void sis85c496_host_device::set_ram_size(int _ram_size)
{
	ram_size = _ram_size;
}

void sis85c496_host_device::device_start()
{
	pci_host_device::device_start();

	memory_space = &m_maincpu->space(AS_PROGRAM);
	io_space = &m_maincpu->space(AS_IO);

	memory_window_start = 0;
	memory_window_end   = 0xffffffff;
	memory_offset       = 0;
	io_window_start = 0;
	io_window_end   = 0xffff;
	io_offset       = 0;
	status = 0x0010;

	m_bios_config = 0x78;
	m_dram_config = 0;
	m_isa_decoder = 0xff;
	m_shadctrl = 0;
	m_smramctrl = 0;

	ram.resize(ram_size/4);
}

void sis85c496_host_device::reset_all_mappings()
{
	pci_host_device::reset_all_mappings();
}

void sis85c496_host_device::device_reset()
{
	pci_host_device::device_reset();

	m_at_spkrdata = 0;
	m_pit_out2 = 1;
	m_dma_channel = -1;
	m_cur_eop = false;
	m_nmi_enabled = 0;
	m_refresh = false;

	m_bios_config = 0x78;
	m_dram_config = 0;
	m_isa_decoder = 0xff;
	m_shadctrl = 0;
	m_smramctrl = 0;
}

void sis85c496_host_device::map_bios(address_space *memory_space, uint32_t start, uint32_t end)
{
	uint32_t mask = m_region->bytes() - 1;
	memory_space->install_rom(start, end, m_region->base() + (start & mask));
}

void sis85c496_host_device::map_shadowram(address_space *memory_space, offs_t addrstart, offs_t addrend, void *baseptr)
{
	if (m_shadctrl & 0x100) // write protected?
	{
		memory_space->install_rom(addrstart, addrend, baseptr);
	}
	else
	{
		memory_space->install_ram(addrstart, addrend, baseptr);
	}
}

void sis85c496_host_device::map_extra(uint64_t memory_window_start, uint64_t memory_window_end, uint64_t memory_offset, address_space *memory_space,
									 uint64_t io_window_start, uint64_t io_window_end, uint64_t io_offset, address_space *io_space)
{
	logerror("SiS496: mapping!\n");
	io_space->install_device(0, 0xffff, *this, &sis85c496_host_device::internal_io_map);

	// is SMRAM at e0000?  overrides shadow if so
	if ((m_smramctrl & 0x16) == 0x16)
	{
		if (m_smramctrl & 0x08)
		{
			memory_space->install_ram(0x000e0000, 0x000effff, &ram[0x000b0000/4]);
			logerror("Sis496: SMRAM at Exxxx, phys Bxxxx\n");
		}
		else
		{
			memory_space->install_ram(0x000e0000, 0x000effff, &ram[0x000a0000/4]);
			logerror("Sis496: SMRAM at Exxxx, phys Axxxx\n");
		}

		// map the high BIOS at FFFExxxx if enabled
		if (m_bios_config & 0x40)
		{
			map_bios(memory_space, 0xfffe0000, 0xfffeffff);
		}
	}
	else
	{
		// does shadow RAM actually require this to be set?  can't tell w/Megatouch BIOS.
		if (m_bios_config & 0x40)
		{
			logerror("SiS496: BIOS at Exxxx\n");
			map_bios(memory_space, 0xfffe0000, 0xfffeffff);

			if ((m_shadctrl & 0x30) == 0)
			{
				map_bios(memory_space, 0x000e0000, 0x000effff);
			}
			else    // at least one 32K block has shadow memory
			{
				if (m_shadctrl & 0x20)
				{
					logerror("SiS496: shadow RAM at e8000\n");
					map_shadowram(memory_space, 0x000e8000, 0x000effff, &ram[0x000e8000/4]);
				}

				if (m_shadctrl & 0x10)
				{
					logerror("SiS496: shadow RAM at e0000\n");
					map_shadowram(memory_space, 0x000e0000, 0x000e7fff, &ram[0x000e0000/4]);
				}
			}
		}
	}
	if (m_bios_config & 0x20)
	{
		map_bios(memory_space, 0xffff0000, 0xffffffff);

		if ((m_shadctrl & 0xc0) == 0)
		{
			map_bios(memory_space, 0x000f0000, 0x000fffff);
			logerror("SiS496: BIOS at Fxxxx\n");
		}
		else    // at least one 32K block has shadow memory
		{
			if (m_shadctrl & 0x80)
			{
				logerror("SiS496: shadow RAM at f8000\n");
				map_shadowram(memory_space, 0x000f8000, 0x000fffff, &ram[0x000f8000/4]);
			}

			if (m_shadctrl & 0x40)
			{
				logerror("SiS496: shadow RAM at f0000\n");
				map_shadowram(memory_space, 0x000f0000, 0x000f7fff, &ram[0x000f0000/4]);
			}
		}
	}

	if (m_shadctrl & 0x08)
	{
		logerror("SiS496: shadow RAM at d8000\n");
		memory_space->install_ram(0x000d8000, 0x000dffff, &ram[0x000d8000/4]);
	}
	if (m_shadctrl & 0x04)
	{
		logerror("SiS496: shadow RAM at d0000\n");
		memory_space->install_ram(0x000d0000, 0x000d7fff, &ram[0x000d0000/4]);
	}
	if (m_shadctrl & 0x02)
	{
		logerror("SiS496: shadow RAM at c8000\n");
		memory_space->install_ram(0x000c8000, 0x000cffff, &ram[0x000c8000/4]);
	}
	if (m_shadctrl & 0x01)
	{
		logerror("SiS496: shadow RAM at d8000\n");
		memory_space->install_ram(0x000c0000, 0x000c7fff, &ram[0x000c0000/4]);
	}

	// is SMRAM enabled at 6xxxx?
	if ((m_smramctrl & 0x12) == 0x02)
	{
		fatalerror("Sis486: SMRAM enabled at 6xxxx, not yet supported!\n");
	}

	if (m_isa_decoder & 0x01)
	{
		logerror("SiS496: ISA base 640K enabled\n");
		memory_space->install_ram(0x00000000, 0x0009ffff, &ram[0x00000000/4]);
	}

	// 32 megs of RAM (todo: don't hardcode)
	memory_space->install_ram(0x00100000, 0x01ffffff, &ram[0x00100000/4]);
}

// Southbridge
READ8_MEMBER( sis85c496_host_device::get_slave_ack )
{
	if (offset==2) // IRQ = 2
		return m_pic8259_slave->acknowledge();

	return 0x00;
}

void sis85c496_host_device::at_speaker_set_spkrdata(uint8_t data)
{
	m_at_spkrdata = data ? 1 : 0;
	m_speaker->level_w(m_at_spkrdata & m_pit_out2);
}



WRITE_LINE_MEMBER( sis85c496_host_device::at_pit8254_out0_changed )
{
	if (m_pic8259_master)
		m_pic8259_master->ir0_w(state);
}

WRITE_LINE_MEMBER( sis85c496_host_device::at_pit8254_out1_changed )
{
	if(state)
		m_refresh = !m_refresh;
}

WRITE_LINE_MEMBER( sis85c496_host_device::at_pit8254_out2_changed )
{
	m_pit_out2 = state ? 1 : 0;
	m_speaker->level_w(m_at_spkrdata & m_pit_out2);
}

READ8_MEMBER( sis85c496_host_device::at_page8_r )
{
	uint8_t data = m_at_pages[offset % 0x10];

	switch(offset % 8)
	{
	case 1:
		data = m_dma_offset[BIT(offset, 3)][2];
		break;
	case 2:
		data = m_dma_offset[BIT(offset, 3)][3];
		break;
	case 3:
		data = m_dma_offset[BIT(offset, 3)][1];
		break;
	case 7:
		data = m_dma_offset[BIT(offset, 3)][0];
		break;
	}
	return data;
}


WRITE8_MEMBER( sis85c496_host_device::at_page8_w )
{
	m_at_pages[offset % 0x10] = data;

	switch(offset % 8)
	{
	case 0:
		//m_boot_state_hook((offs_t)0, data);
		break;
	case 1:
		m_dma_offset[BIT(offset, 3)][2] = data;
		break;
	case 2:
		m_dma_offset[BIT(offset, 3)][3] = data;
		break;
	case 3:
		m_dma_offset[BIT(offset, 3)][1] = data;
		break;
	case 7:
		m_dma_offset[BIT(offset, 3)][0] = data;
		break;
	}
}


WRITE_LINE_MEMBER( sis85c496_host_device::pc_dma_hrq_changed )
{
	m_maincpu->set_input_line(INPUT_LINE_HALT, state ? ASSERT_LINE : CLEAR_LINE);

	/* Assert HLDA */
	m_dma8237_2->hack_w( state );
}

READ8_MEMBER(sis85c496_host_device::pc_dma_read_byte)
{
	address_space& prog_space = m_maincpu->space(AS_PROGRAM); // get the right address space
	if(m_dma_channel == -1)
		return 0xff;
	uint8_t result;
	offs_t page_offset = ((offs_t) m_dma_offset[0][m_dma_channel]) << 16;

	result = prog_space.read_byte(page_offset + offset);
	return result;
}


WRITE8_MEMBER(sis85c496_host_device::pc_dma_write_byte)
{
	address_space& prog_space = m_maincpu->space(AS_PROGRAM); // get the right address space
	if(m_dma_channel == -1)
		return;
	offs_t page_offset = ((offs_t) m_dma_offset[0][m_dma_channel]) << 16;

	prog_space.write_byte(page_offset + offset, data);
}


READ8_MEMBER(sis85c496_host_device::pc_dma_read_word)
{
	address_space& prog_space = m_maincpu->space(AS_PROGRAM); // get the right address space
	if(m_dma_channel == -1)
		return 0xff;
	uint16_t result;
	offs_t page_offset = ((offs_t) m_dma_offset[1][m_dma_channel & 3]) << 16;

	result = prog_space.read_word((page_offset & 0xfe0000) | (offset << 1));
	m_dma_high_byte = result & 0xFF00;

	return result & 0xFF;
}


WRITE8_MEMBER(sis85c496_host_device::pc_dma_write_word)
{
	address_space& prog_space = m_maincpu->space(AS_PROGRAM); // get the right address space
	if(m_dma_channel == -1)
		return;
	offs_t page_offset = ((offs_t) m_dma_offset[1][m_dma_channel & 3]) << 16;

	prog_space.write_word((page_offset & 0xfe0000) | (offset << 1), m_dma_high_byte | data);
}


READ8_MEMBER( sis85c496_host_device::pc_dma8237_0_dack_r ) { return 0; } //m_isabus->dack_r(0); }
READ8_MEMBER( sis85c496_host_device::pc_dma8237_1_dack_r ) { return 0; } //m_isabus->dack_r(1); }
READ8_MEMBER( sis85c496_host_device::pc_dma8237_2_dack_r ) { return 0; } //m_isabus->dack_r(2); }
READ8_MEMBER( sis85c496_host_device::pc_dma8237_3_dack_r ) { return 0; } //m_isabus->dack_r(3); }
READ8_MEMBER( sis85c496_host_device::pc_dma8237_5_dack_r ) { return 0; } //m_isabus->dack_r(5); }
READ8_MEMBER( sis85c496_host_device::pc_dma8237_6_dack_r ) { return 0; } //m_isabus->dack_r(6); }
READ8_MEMBER( sis85c496_host_device::pc_dma8237_7_dack_r ) { return 0; } //m_isabus->dack_r(7); }


WRITE8_MEMBER( sis85c496_host_device::pc_dma8237_0_dack_w ){ } //m_isabus->dack_w(0, data); }
WRITE8_MEMBER( sis85c496_host_device::pc_dma8237_1_dack_w ){ } //m_isabus->dack_w(1, data); }
WRITE8_MEMBER( sis85c496_host_device::pc_dma8237_2_dack_w ){ } //m_isabus->dack_w(2, data); }
WRITE8_MEMBER( sis85c496_host_device::pc_dma8237_3_dack_w ){ } //m_isabus->dack_w(3, data); }
WRITE8_MEMBER( sis85c496_host_device::pc_dma8237_5_dack_w ){ } //m_isabus->dack_w(5, data); }
WRITE8_MEMBER( sis85c496_host_device::pc_dma8237_6_dack_w ){ } //m_isabus->dack_w(6, data); }
WRITE8_MEMBER( sis85c496_host_device::pc_dma8237_7_dack_w ){ } //m_isabus->dack_w(7, data); }

WRITE_LINE_MEMBER( sis85c496_host_device::at_dma8237_out_eop )
{
	m_cur_eop = state == ASSERT_LINE;
	//if(m_dma_channel != -1)
//      m_isabus->eop_w(m_dma_channel, m_cur_eop ? ASSERT_LINE : CLEAR_LINE );
}

void sis85c496_host_device::pc_select_dma_channel(int channel, bool state)
{
	//m_isabus->dack_line_w(channel, state);

	if(!state) {
		m_dma_channel = channel;
		//if(m_cur_eop)
//          m_isabus->eop_w(channel, ASSERT_LINE );

	} else if(m_dma_channel == channel) {
		m_dma_channel = -1;
		//if(m_cur_eop)
//          m_isabus->eop_w(channel, CLEAR_LINE );
	}
}


WRITE_LINE_MEMBER( sis85c496_host_device::pc_dack0_w ) { pc_select_dma_channel(0, state); }
WRITE_LINE_MEMBER( sis85c496_host_device::pc_dack1_w ) { pc_select_dma_channel(1, state); }
WRITE_LINE_MEMBER( sis85c496_host_device::pc_dack2_w ) { pc_select_dma_channel(2, state); }
WRITE_LINE_MEMBER( sis85c496_host_device::pc_dack3_w ) { pc_select_dma_channel(3, state); }
WRITE_LINE_MEMBER( sis85c496_host_device::pc_dack4_w ) { m_dma8237_1->hack_w( state ? 0 : 1); } // it's inverted
WRITE_LINE_MEMBER( sis85c496_host_device::pc_dack5_w ) { pc_select_dma_channel(5, state); }
WRITE_LINE_MEMBER( sis85c496_host_device::pc_dack6_w ) { pc_select_dma_channel(6, state); }
WRITE_LINE_MEMBER( sis85c496_host_device::pc_dack7_w ) { pc_select_dma_channel(7, state); }

READ8_MEMBER( sis85c496_host_device::at_portb_r )
{
	uint8_t data = m_at_speaker;
	data &= ~0xd0; /* AT BIOS don't likes this being set */

	/* 0x10 is the dram refresh line bit on the 5170, just a timer here, 15.085us. */
	data |= m_refresh ? 0x10 : 0;

	if (m_pit_out2)
		data |= 0x20;
	else
		data &= ~0x20; /* ps2m30 wants this */

	return data;
}

WRITE8_MEMBER( sis85c496_host_device::at_portb_w )
{
	m_at_speaker = data;
	m_pit8254->write_gate2(BIT(data, 0));
	at_speaker_set_spkrdata( BIT(data, 1));
	m_channel_check = BIT(data, 3);
	//m_isabus->set_nmi_state((m_nmi_enabled==0) && (m_channel_check==0));
}

READ8_MEMBER( sis85c496_host_device::at_dma8237_2_r )
{
	return m_dma8237_2->read( offset / 2);
}

WRITE8_MEMBER( sis85c496_host_device::at_dma8237_2_w )
{
	m_dma8237_2->write( offset / 2, data);
}

READ8_MEMBER( sis85c496_host_device::at_keybc_r )
{
	switch (offset)
	{
	case 0: return m_keybc->data_r();
	case 1: return at_portb_r(space, 0);
	}

	return 0xff;
}

WRITE8_MEMBER( sis85c496_host_device::at_keybc_w )
{
	switch (offset)
	{
	case 0: m_keybc->data_w(data); break;
	case 1: at_portb_w(space, 0, data); break;
	}
}


WRITE8_MEMBER( sis85c496_host_device::write_rtc )
{
	if (offset==0) {
		m_nmi_enabled = BIT(data,7);
		//m_isabus->set_nmi_state((m_nmi_enabled==0) && (m_channel_check==0));
		m_ds12885->write(0,data);
	}
	else {
		m_ds12885->write(offset,data);
	}
}

WRITE_LINE_MEMBER(sis85c496_host_device::cpu_int_w)
{
	m_maincpu->set_input_line(0, state);
}

WRITE_LINE_MEMBER(sis85c496_host_device::cpu_a20_w)
{
	m_maincpu->set_input_line(INPUT_LINE_A20, state);
}

WRITE_LINE_MEMBER(sis85c496_host_device::cpu_reset_w)
{
	m_maincpu->set_input_line(INPUT_LINE_RESET, state);
}

/*

after decompress to shadow RAM:

config_write 00:05.0:40 00000004 @ 000000ff
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 28058 = 00120000 & 00FF0000 SMRAM: e0000 to SMRAM, enable
config_write 00:05.0:58 00120000 @ 00ff0000
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 28058 = 00040000 & 00FF0000 SMRAM: always enable
config_write 00:05.0:58 00040000 @ 00ff0000
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 28058 = 00000000 & 00FF0000 SMRAM: disable
config_write 00:05.0:58 00000000 @ 00ff0000
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 280A0 = 0000FF00 & 0000FF00 SMI: clear all requests
config_write 00:05.0:a0 0000ff00 @ 0000ff00
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 280A0 = 000000FF & 000000FF SMI: clear all requests
config_write 00:05.0:a0 000000ff @ 000000ff
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 2808C = 00000500 & 0000FF00 SMI: timer count
config_write 00:05.0:8c 00000500 @ 0000ff00
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 2809C = 00020000 & 00FF0000 SMI: start countdown timer
config_write 00:05.0:9c 00020000 @ 00ff0000
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 28084 = 00000006 & 000000FF clear deturbo and break switch blocks
config_write 00:05.0:84 00000006 @ 000000ff
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 28080 = 00000004 & 000000FF enable soft-SMI
config_write 00:05.0:80 00000004 @ 000000ff
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 280A0 = 00100000 & 00FF0000 select software SMI request
config_write 00:05.0:a0 00100000 @ 00ff0000
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 2809C = 00010000 & 00FF0000 assert SMI
config_write 00:05.0:9c 00010000 @ 00ff0000
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 280C4 = 00080000 & 00FF0000 IRQ routing: undocumented value
config_write 00:05.0:c4 00080000 @ 00ff0000
[:pci:05.0] ':maincpu' (000FF6D8): unmapped configuration_space memory write to 28080 = 00000000 & 000000FF clear all SMI
config_write 00:05.0:80 00000000 @ 000000ff



*/
