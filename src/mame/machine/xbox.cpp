// license:BSD-3-Clause
// copyright-holders:Samuele Zannoli

#include "emu.h"
#include "machine/pci.h"
#include "includes/xbox_pci.h"
#include "includes/xbox.h"

#include "cpu/i386/i386.h"
#include "debug/debugcon.h"
#include "debug/debugcmd.h"

#include "debugger.h"
#include "romload.h"

#include <functional>

const xbox_base_state::debugger_constants xbox_base_state::debugp[] = {
	{ 0x66232714, {0x8003aae0, 0x5c, 0x1c, 0x28, 0x210, 8, 0x28, 0x1c} },
	{ 0x49d8055a, {0x8003aae0, 0x5c, 0x1c, 0x28, 0x210, 8, 0x28, 0x1c} }
};

int xbox_base_state::find_bios_index()
{
	u8 sb = system_bios();
	return sb;
}

bool xbox_base_state::find_bios_hash(int bios, uint32_t &crc32)
{
	uint32_t crc = 0;
	const std::vector<rom_entry> &rev = rom_region_vector();

	for (rom_entry const &re : rev)
	{
		if (ROMENTRY_ISFILE(re))
		{
			if (ROM_GETBIOSFLAGS(re) == (bios + 1))
			{
				const std::string &h = re.hashdata();
				util::hash_collection hc(h.c_str());
				if (hc.crc(crc) == true)
				{
					crc32 = crc;
					return true;
				}
			}
		}
	}
	return false;
}

void xbox_base_state::find_debug_params()
{
	uint32_t crc;
	int sb;

	sb = (int)find_bios_index();
	debugc_bios = debugp;
	if (find_bios_hash(sb - 1, crc) == true)
	{
		for (int n = 0; n < 2; n++)
			if (debugp[n].id == crc)
			{
				debugc_bios = &debugp[n];
				break;
			}
	}
}

void xbox_base_state::dump_string_command(int ref, const std::vector<std::string> &params)
{
	debugger_cpu &cpu = machine().debugger().cpu();
	debugger_console &con = machine().debugger().console();
	address_space &space = m_maincpu->space();
	uint64_t addr;
	offs_t address;

	if (params.size() < 2)
		return;

	if (!machine().debugger().commands().validate_number_parameter(params[1], addr))
		return;

	address = (offs_t)addr;
	if (!m_maincpu->translate(AS_PROGRAM, TRANSLATE_READ_DEBUG, address))
	{
		con.printf("Address is unmapped.\n");
		return;
	}
	address = (offs_t)addr;

	uint32_t length = cpu.read_word(space, address, true);
	uint32_t maximumlength = cpu.read_word(space, address + 2, true);
	offs_t buffer = cpu.read_dword(space, address + 4, true);
	con.printf("Length %d word\n", length);
	con.printf("MaximumLength %d word\n", maximumlength);
	con.printf("Buffer %08X byte* ", buffer);

	// limit the number of characters to avoid flooding
	if (length > 256)
		length = 256;

	for (int a = 0; a < length; a++)
	{
		uint8_t c = cpu.read_byte(space, buffer + a, true);
		con.printf("%c", c);
	}
	con.printf("\n");
}

void xbox_base_state::dump_process_command(int ref, const std::vector<std::string> &params)
{
	debugger_cpu &cpu = machine().debugger().cpu();
	debugger_console &con = machine().debugger().console();
	address_space &space = m_maincpu->space();
	uint64_t addr;
	offs_t address;

	if (params.size() < 2)
		return;

	if (!machine().debugger().commands().validate_number_parameter(params[1], addr))
		return;

	address = (offs_t)addr;
	if (!m_maincpu->translate(AS_PROGRAM, TRANSLATE_READ_DEBUG, address))
	{
		con.printf("Address is unmapped.\n");
		return;
	}
	address = (offs_t)addr;

	con.printf("ReadyListHead {%08X,%08X} _LIST_ENTRY\n", cpu.read_dword(space, address, true), cpu.read_dword(space, address + 4, true));
	con.printf("ThreadListHead {%08X,%08X} _LIST_ENTRY\n", cpu.read_dword(space, address + 8, true), cpu.read_dword(space, address + 12, true));
	con.printf("StackCount %d dword\n", cpu.read_dword(space, address + 16, true));
	con.printf("ThreadQuantum %d dword\n", cpu.read_dword(space, address + 20, true));
	con.printf("BasePriority %d byte\n", cpu.read_byte(space, address + 24, true));
	con.printf("DisableBoost %d byte\n", cpu.read_byte(space, address + 25, true));
	con.printf("DisableQuantum %d byte\n", cpu.read_byte(space, address + 26, true));
	con.printf("_padding %d byte\n", cpu.read_byte(space, address + 27, true));
}

void xbox_base_state::dump_list_command(int ref, const std::vector<std::string> &params)
{
	debugger_cpu &cpu = machine().debugger().cpu();
	debugger_console &con = machine().debugger().console();
	address_space &space = m_maincpu->space();
	uint64_t addr;
	offs_t address;

	if (params.size() < 2)
		return;

	if (!machine().debugger().commands().validate_number_parameter(params[1], addr))
		return;

	uint64_t offs = 0;
	offs_t offset = 0;
	if (params.size() >= 3)
	{
		if (!machine().debugger().commands().validate_number_parameter(params[2], offs))
			return;
		offset = (offs_t)offs;
	}

	uint64_t start = addr;
	address = (offs_t)addr;
	if (!m_maincpu->translate(AS_PROGRAM, TRANSLATE_READ_DEBUG, address))
	{
		con.printf("Address is unmapped.\n");
		return;
	}
	address = (offs_t)addr;
	if (params.size() >= 3)
		con.printf("Entry    Object\n");
	else
		con.printf("Entry\n");

	uint64_t old;
	for (int num = 0; num < 32; num++)
	{
		if (params.size() >= 3)
			con.printf("%08X %08X\n", (uint32_t)addr, (offs_t)addr - offset);
		else
			con.printf("%08X\n", (uint32_t)addr);
		old = addr;
		addr = cpu.read_dword(space, address, true);
		if (addr == start)
			break;
		if (addr == old)
			break;
		address = (offs_t)addr;
		if (!m_maincpu->translate(AS_PROGRAM, TRANSLATE_READ_DEBUG, address))
			break;
		address = (offs_t)addr;
	}
}

void xbox_base_state::dump_dpc_command(int ref, const std::vector<std::string> &params)
{
	debugger_cpu &cpu = machine().debugger().cpu();
	debugger_console &con = machine().debugger().console();
	address_space &space = m_maincpu->space();
	uint64_t addr;
	offs_t address;

	if (params.size() < 2)
		return;

	if (!machine().debugger().commands().validate_number_parameter(params[1], addr))
		return;

	address = (offs_t)addr;
	if (!m_maincpu->translate(AS_PROGRAM, TRANSLATE_READ_DEBUG, address))
	{
		con.printf("Address is unmapped.\n");
		return;
	}
	address = (offs_t)addr;
	con.printf("Type %d word\n", cpu.read_word(space, address, true));
	con.printf("Inserted %d byte\n", cpu.read_byte(space, address + 2, true));
	con.printf("Padding %d byte\n", cpu.read_byte(space, address + 3, true));
	con.printf("DpcListEntry {%08X,%08X} _LIST_ENTRY\n", cpu.read_dword(space, address + 4, true), cpu.read_dword(space, address + 8, true));
	con.printf("DeferredRoutine %08X dword\n", cpu.read_dword(space, address + 12, true));
	con.printf("DeferredContext %08X dword\n", cpu.read_dword(space, address + 16, true));
	con.printf("SystemArgument1 %08X dword\n", cpu.read_dword(space, address + 20, true));
	con.printf("SystemArgument2 %08X dword\n", cpu.read_dword(space, address + 24, true));
}

void xbox_base_state::dump_timer_command(int ref, const std::vector<std::string> &params)
{
	debugger_cpu &cpu = machine().debugger().cpu();
	debugger_console &con = machine().debugger().console();
	address_space &space = m_maincpu->space();
	uint64_t addr;
	offs_t address;

	if (params.size() < 2)
		return;

	if (!machine().debugger().commands().validate_number_parameter(params[1], addr))
		return;

	address = (offs_t)addr;
	if (!m_maincpu->translate(AS_PROGRAM, TRANSLATE_READ_DEBUG, address))
	{
		con.printf("Address is unmapped.\n");
		return;
	}
	address = (offs_t)addr;
	con.printf("Header.Type %d byte\n", cpu.read_byte(space, address, true));
	con.printf("Header.Absolute %d byte\n", cpu.read_byte(space, address + 1, true));
	con.printf("Header.Size %d byte\n", cpu.read_byte(space, address + 2, true));
	con.printf("Header.Inserted %d byte\n", cpu.read_byte(space, address + 3, true));
	con.printf("Header.SignalState %08X dword\n", cpu.read_dword(space, address + 4, true));
	con.printf("Header.WaitListEntry {%08X,%08X} _LIST_ENTRY\n", cpu.read_dword(space, address + 8, true), cpu.read_dword(space, address + 12, true));
	con.printf("%s", string_format("DueTime %x qword\n", (int64_t)cpu.read_qword(space, address + 16, true)).c_str());
	con.printf("TimerListEntry {%08X,%08X} _LIST_ENTRY\n", cpu.read_dword(space, address + 24, true), cpu.read_dword(space, address + 28, true));
	con.printf("Dpc %08X dword\n", cpu.read_dword(space, address + 32, true));
	con.printf("Period %d dword\n", cpu.read_dword(space, address + 36, true));
}

void xbox_base_state::curthread_command(int ref, const std::vector<std::string> &params)
{
	debugger_cpu &cpu = machine().debugger().cpu();
	debugger_console &con = machine().debugger().console();
	address_space &space = m_maincpu->space();
	offs_t address;

	uint64_t fsbase = m_maincpu->state_int(44); // base of FS register
	address = (offs_t)fsbase + (offs_t)debugc_bios->parameter[7-1];
	if (!m_maincpu->translate(AS_PROGRAM, TRANSLATE_READ_DEBUG, address))
	{
		con.printf("Address is unmapped.\n");
		return;
	}
	address = (offs_t)fsbase + (offs_t)debugc_bios->parameter[7-1];

	uint32_t kthrd = cpu.read_dword(space, address, true);
	con.printf("Current thread is %08X\n", kthrd);
	address = (offs_t)(kthrd + debugc_bios->parameter[8-1]);
	uint32_t topstack = cpu.read_dword(space, address, true);
	con.printf("Current thread stack top is %08X\n", topstack);
	address = (offs_t)(kthrd + debugc_bios->parameter[4-1]);
	uint32_t tlsdata = cpu.read_dword(space, address, true);
	if (tlsdata == 0)
		address = (offs_t)(topstack - debugc_bios->parameter[5-1] - debugc_bios->parameter[6-1]);
	else
		address = (offs_t)(tlsdata - debugc_bios->parameter[6-1]);
	con.printf("Current thread function is %08X\n", cpu.read_dword(space, address, true));
}

void xbox_base_state::threadlist_command(int ref, const std::vector<std::string> &params)
{
	address_space &space = m_maincpu->space();
	debugger_cpu &cpu = machine().debugger().cpu();
	debugger_console &con = machine().debugger().console();

	con.printf("Pri. _KTHREAD   Stack  Function\n");
	con.printf("-------------------------------\n");
	for (int pri = 0; pri < 16; pri++)
	{
		uint32_t curr = debugc_bios->parameter[1 - 1] + pri * 8;
		uint32_t next = cpu.read_dword(space, curr, true);

		while ((next != curr) && (next != 0))
		{
			uint32_t kthrd = next - debugc_bios->parameter[2 - 1];
			uint32_t topstack = cpu.read_dword(space, kthrd + debugc_bios->parameter[3 - 1], true);
			uint32_t tlsdata = cpu.read_dword(space, kthrd + debugc_bios->parameter[4 - 1], true);
			uint32_t function;
			if (tlsdata == 0)
				function = cpu.read_dword(space, topstack - debugc_bios->parameter[5 - 1] - debugc_bios->parameter[6 - 1], true);
			else
				function = cpu.read_dword(space, tlsdata - debugc_bios->parameter[6 - 1], true);
			con.printf(" %02d  %08x %08x %08x\n", pri, kthrd, topstack, function);
			next = cpu.read_dword(space, next, true);
		}
	}
}

void xbox_base_state::generate_irq_command(int ref, const std::vector<std::string> &params)
{
	uint64_t irq;

	if (params.size() < 2)
		return;
	if (!machine().debugger().commands().validate_number_parameter(params[1], irq))
		return;
	if (irq > 15)
		return;
	if (irq == 2)
		return;
	debug_generate_irq((int)irq, true);
}

void xbox_base_state::nv2a_combiners_command(int ref, const std::vector<std::string> &params)
{
	debugger_console &con = machine().debugger().console();
	bool en = nvidia_nv2a->toggle_register_combiners_usage();
	if (en == true)
		con.printf("Register combiners enabled\n");
	else
		con.printf("Register combiners disabled\n");
}

void xbox_base_state::nv2a_wclipping_command(int ref, const std::vector<std::string> &params)
{
	debugger_console &con = machine().debugger().console();
	bool en = nvidia_nv2a->toggle_clipping_w_support();
	if (en == true)
		con.printf("W clipping enabled\n");
	else
		con.printf("W clipping disabled\n");
}

void xbox_base_state::waitvblank_command(int ref, const std::vector<std::string> &params)
{
	debugger_console &con = machine().debugger().console();
	bool en = nvidia_nv2a->toggle_wait_vblank_support();
	if (en == true)
		con.printf("Vblank method enabled\n");
	else
		con.printf("Vblank method disabled\n");
}

void xbox_base_state::grab_texture_command(int ref, const std::vector<std::string> &params)
{
	uint64_t type;

	if (params.size() < 3)
		return;
	if (!machine().debugger().commands().validate_number_parameter(params[1], type))
		return;
	if (params[2].empty() || params[2].length() > 127)
		return;
	nvidia_nv2a->debug_grab_texture((int)type, params[2].c_str());
}

void xbox_base_state::grab_vprog_command(int ref, const std::vector<std::string> &params)
{
	uint32_t instruction[4];
	FILE *fil;

	if (params.size() < 2)
		return;
	if (params[1].empty() || params[1].length() > 127)
		return;
	if ((fil = fopen(params[1].c_str(), "wb")) == nullptr)
		return;
	for (int n = 0; n < 136; n++)
	{
		nvidia_nv2a->debug_grab_vertex_program_slot(n, instruction);
		fwrite(instruction, sizeof(uint32_t), 4, fil);
	}
	fclose(fil);
}

void xbox_base_state::vprogdis_command(int ref, const std::vector<std::string> &params)
{
	address_space &space = m_maincpu->space();

	if (params.size() < 3)
		return;

	uint64_t address;
	if (!machine().debugger().commands().validate_number_parameter(params[1], address))
		return;

	uint64_t length;
	if (!machine().debugger().commands().validate_number_parameter(params[2], length))
		return;

	uint64_t type = 0;
	if (params.size() > 3)
		if (!machine().debugger().commands().validate_number_parameter(params[3], type))
			return;

	vertex_program_disassembler vd;
	while (length > 0)
	{
		uint32_t instruction[4];
		if (type == 1)
		{
			offs_t addr = (offs_t)address;
			if (!m_maincpu->translate(AS_PROGRAM, TRANSLATE_READ_DEBUG, addr))
				return;
			instruction[0] = space.read_dword_unaligned(address);
			instruction[1] = space.read_dword_unaligned(address + 4);
			instruction[2] = space.read_dword_unaligned(address + 8);
			instruction[3] = space.read_dword_unaligned(address + 12);
		}
		else
		{
			nvidia_nv2a->debug_grab_vertex_program_slot((int)address, instruction);
		}

		char line[64];
		while (vd.disassemble(instruction, line) != 0)
			machine().debugger().console().printf("%s\n", line);

		if (type == 1)
			address = address + 4 * 4;
		else
			address++;

		length--;
	}
}

void xbox_base_state::help_command(int ref, const std::vector<std::string> &params)
{
	debugger_console &con = machine().debugger().console();

	con.printf("Available Xbox commands:\n");
	con.printf("  xbox dump_string,<address> -- Dump _STRING object at <address>\n");
	con.printf("  xbox dump_process,<address> -- Dump _PROCESS object at <address>\n");
	con.printf("  xbox dump_list,<address>[,<offset>] -- Dump _LIST_ENTRY chain starting at <address>\n");
	con.printf("  xbox dump_dpc,<address> -- Dump _KDPC object at <address>\n");
	con.printf("  xbox dump_timer,<address> -- Dump _KTIMER object at <address>\n");
	con.printf("  xbox curthread -- Print information about current thread\n");
	con.printf("  xbox threadlist -- list of currently active threads\n");
	con.printf("  xbox irq,<number> -- Generate interrupt with irq number 0-15\n");
	con.printf("  xbox nv2a_combiners -- Toggle use of register combiners\n");
	con.printf("  xbox nv2a_wclipping -- Toggle use of negative w vertex clipping\n");
	con.printf("  xbox waitvblank -- Toggle support for wait vblank method\n");
	con.printf("  xbox grab_texture,<type>,<filename> -- Save to <filename> the next used texture of type <type>\n");
	con.printf("  xbox grab_vprog,<filename> -- save current vertex program instruction slots to <filename>\n");
	con.printf("  xbox vprogdis,<address>,<length>[,<type>] -- disassemble <lenght> vertex program instructions at <address> of <type>\n");
	con.printf("  xbox help -- this list\n");
}

void xbox_base_state::xbox_debug_commands(int ref, const std::vector<std::string> &params)
{
	if (params.size() < 1)
		return;
	if (params[0] == "dump_string")
		dump_string_command(ref, params);
	else if (params[0] == "dump_process")
		dump_process_command(ref, params);
	else if (params[0] == "dump_list")
		dump_list_command(ref, params);
	else if (params[0] == "dump_dpc")
		dump_dpc_command(ref, params);
	else if (params[0] == "dump_timer")
		dump_timer_command(ref, params);
	else if (params[0] == "curthread")
		curthread_command(ref, params);
	else if (params[0] == "threadlist")
		threadlist_command(ref, params);
	else if (params[0] == "irq")
		generate_irq_command(ref, params);
	else if (params[0] == "nv2a_combiners")
		nv2a_combiners_command(ref, params);
	else if (params[0] == "nv2a_wclipping")
		nv2a_wclipping_command(ref, params);
	else if (params[0] == "waitvblank")
		waitvblank_command(ref, params);
	else if (params[0] == "grab_texture")
		grab_texture_command(ref, params);
	else if (params[0] == "grab_vprog")
		grab_vprog_command(ref, params);
	else if (params[0] == "vprogdis")
		vprogdis_command(ref, params);
	else
		help_command(ref, params);
}

void xbox_base_state::debug_generate_irq(int irq, bool active)
{
	int state;

	if (active)
	{
		debug_irq_active = true;
		debug_irq_number = irq;
		state = 1;
	}
	else
	{
		debug_irq_active = false;
		state = 0;
	}
	mcpxlpc->debug_generate_irq(irq, state);
}

WRITE_LINE_MEMBER(xbox_base_state::vblank_callback)
{
	nvidia_nv2a->vblank_callback(state);
}

uint32_t xbox_base_state::screen_update_callback(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	return nvidia_nv2a->screen_update_callback(screen, bitmap, cliprect);
}

/*
 * PIC & PIT
 */

WRITE_LINE_MEMBER(xbox_base_state::maincpu_interrupt)
{
	m_maincpu->set_input_line(0, state ? HOLD_LINE : CLEAR_LINE);
}

IRQ_CALLBACK_MEMBER(xbox_base_state::irq_callback)
{
	int r;
	r = mcpxlpc->acknowledge();
	if (debug_irq_active)
		debug_generate_irq(debug_irq_number, false);
	return r;
}

WRITE_LINE_MEMBER(xbox_base_state::ohci_usb_interrupt_changed)
{
	mcpxlpc->irq1(state);
}

WRITE_LINE_MEMBER(xbox_base_state::nv2a_interrupt_changed)
{
	mcpxlpc->irq3(state);
}

WRITE_LINE_MEMBER(xbox_base_state::smbus_interrupt_changed)
{
	mcpxlpc->irq11(state);
}

WRITE_LINE_MEMBER(xbox_base_state::ide_interrupt_changed)
{
	mcpxlpc->irq14(state);
}

/*
 * SMbus devices
 */

/*
 * PIC16LC
 */

DEFINE_DEVICE_TYPE(XBOX_PIC16LC, xbox_pic16lc_device, "pic16lc", "XBOX PIC16LC")

xbox_pic16lc_device::xbox_pic16lc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, XBOX_PIC16LC, tag, owner, clock)
{
}

int xbox_pic16lc_device::execute_command(int command, int rw, int data)
{
	if (rw == 1) { // read
		if (command == 0)
		{
			if (buffer[0] == 'D')
				buffer[0] = 'X';
			else if (buffer[0] == 'X')
				buffer[0] = 'B';
			else if (buffer[0] == 'B')
				buffer[0] = 'D';
		}
		logerror("pic16lc: %d %d %d\n", command, rw, buffer[command]);
		return buffer[command];
	}
	else
		if (command == 0)
			buffer[0] = 'B';
		else
			buffer[command] = (uint8_t)data;
	logerror("pic16lc: %d %d %d\n", command, rw, data);
	return 0;
}

void xbox_pic16lc_device::device_start()
{
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 'B';
	buffer[4] = 0; // A/V connector, 0=scart 2=vga 4=svideo 7=none
	// PIC challenge handshake data
	buffer[0x1c] = 0x0c;
	buffer[0x1d] = 0x0d;
	buffer[0x1e] = 0x0e;
	buffer[0x1f] = 0x0f;
}

void xbox_pic16lc_device::device_reset()
{
}

/*
 * CX25871
 */

DEFINE_DEVICE_TYPE(XBOX_CX25871, xbox_cx25871_device, "cx25871", "XBOX CX25871")

xbox_cx25871_device::xbox_cx25871_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, XBOX_CX25871, tag, owner, clock)
{
}

int xbox_cx25871_device::execute_command(int command, int rw, int data)
{
	logerror("cx25871: %d %d %d\n", command, rw, data);
	return 0;
}

void xbox_cx25871_device::device_start()
{
}

void xbox_cx25871_device::device_reset()
{
}

/*
 * EEPROM
 */

// let's try to fake the missing eeprom, make sure its ntsc, otherwise chihiro will show an error
static int dummyeeprom[256] = {
	0x39, 0xe3, 0xcc, 0x81, 0xb0, 0xa9, 0x97, 0x09, 0x57, 0xac, 0x57, 0x12, 0xf7, 0xc2, 0xc0, 0x21, 0xce, 0x0d, 0x0a, 0xdb, 0x20, 0x7a, 0xf3, 0xff,
	0xdf, 0x67, 0xed, 0xf4, 0xf8, 0x95, 0x5c, 0xd0, 0x9b, 0xef, 0x7b, 0x81, 0xda, 0xd5, 0x98, 0xc1, 0xb1, 0xb3, 0x74, 0x18, 0x86, 0x05, 0xe2, 0x7c,
	0xd1, 0xad, 0xc9, 0x90, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x00, 0x00,
	0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xab, 0xcd, 0xef, 0xba, 0xdc, 0xfe, 0xa1, 0xb2, 0xc3, 0xd3, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

DEFINE_DEVICE_TYPE(XBOX_EEPROM, xbox_eeprom_device, "eeprom", "XBOX EEPROM")

xbox_eeprom_device::xbox_eeprom_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, XBOX_EEPROM, tag, owner, clock)
{
}

int xbox_eeprom_device::execute_command(int command, int rw, int data)
{
	if (command >= 112)
		return 0;
	if (rw == 1) // if reading
	{
		// hack to avoid hanging if eeprom contents are not correct
		// removing this would need dumping the serial eeprom on the chihiro xbox board
		if (command == 0)
		{
			hack_eeprom();
		}
		data = dummyeeprom[command] + dummyeeprom[command + 1] * 256;
		logerror("eeprom: %d %d %d\n", command, rw, data);
		return data;
	}
	logerror("eeprom: %d %d %d\n", command, rw, data);
	dummyeeprom[command] = data;
	return 0;
}

void xbox_eeprom_device::device_start()
{
}

void xbox_eeprom_device::device_reset()
{
}

/*
 * Super-io connected to lpc bus
 */

DEFINE_DEVICE_TYPE(XBOX_SUPERIO, xbox_superio_device, "superio_device", "XBOX debug SuperIO")

xbox_superio_device::xbox_superio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, XBOX_SUPERIO, tag, owner, clock)
	, configuration_mode(false)
	, index(0)
	, selected(0)
{

}

void xbox_superio_device::internal_io_map(address_map &map)
{
	map(0x002e, 0x002f).rw(FUNC(xbox_superio_device::read), FUNC(xbox_superio_device::write));
	map(0x03f8, 0x03ff).rw(FUNC(xbox_superio_device::read_rs232), FUNC(xbox_superio_device::write_rs232));
}

void xbox_superio_device::map_extra(address_space *memory_space, address_space *io_space)
{
	memspace = memory_space;
	iospace = io_space;
	io_space->install_device(0, 0xffff, *this, &xbox_superio_device::internal_io_map);
}

void xbox_superio_device::set_host(int index, lpcbus_host_interface *host)
{
	lpchost = host;
	lpcindex = index;
}

void xbox_superio_device::device_start()
{
	memset(registers, 0, sizeof(registers));
	registers[0][0x26] = 0x2e; // Configuration port address byte 0
}

READ8_MEMBER(xbox_superio_device::read)
{
	if (configuration_mode == false)
		return 0;
	if (offset == 0) // index port 0x2e
		return index;
	if (offset == 1)
	{
		// data port 0x2f
		if (index < 0x30)
			return registers[0][index];
		return registers[selected][index];
	}
	return 0;
}

WRITE8_MEMBER(xbox_superio_device::write)
{
	if (configuration_mode == false)
	{
		// config port 0x2e
		if ((offset == 0) && (data == 0x55))
			configuration_mode = true;
		return;
	}
	if ((offset == 0) && (data == 0xaa))
	{
		// config port 0x2e
		configuration_mode = false;
		return;
	}
	if (offset == 0)
	{
		// index port 0x2e
		index = data;
	}
	if (offset == 1)
	{
		// data port 0x2f
		if (index < 0x30)
		{
			registers[0][index] = data;
			selected = registers[0][7];
		}
		else
		{
			registers[selected][index] = data;
			//if ((superiost.selected == 4) && (superiost.index == 0x30) && (data != 0))
			//  ; // add handlers 0x3f8- +7
		}
	}
}

READ8_MEMBER(xbox_superio_device::read_rs232)
{
	if (offset == 5)
		return 0x20;
	return 0;
}

WRITE8_MEMBER(xbox_superio_device::write_rs232)
{
	if (offset == 0)
	{
		printf("%c", data);
	}
}

void xbox_base_state::machine_start()
{
	find_debug_params();
	nvidia_nv2a = subdevice<nv2a_gpu_device>("pci:1e.0:00.0")->debug_get_renderer();
	if (machine().debug_flags & DEBUG_FLAG_ENABLED)
	{
		using namespace std::placeholders;
		machine().debugger().console().register_command("xbox", CMDFLAG_CUSTOM_HELP, 0, 1, 4, std::bind(&xbox_base_state::xbox_debug_commands, this, _1, _2));
	}
	subdevice<xbox_eeprom_device>("pci:01.1:154")->hack_eeprom =
		[&](void)
	{
		hack_eeprom();
	};
	subdevice<mcpx_ohci_device>("pci:02.0")->set_hack_callback(
		[&](void)
	{
		hack_usb();
	}
	);
	// savestates
	save_item(NAME(debug_irq_active));
	save_item(NAME(debug_irq_number));
}

#if 0
void xbox_base_state::xbox_base_map(address_map &map)
{
	map(0x00000000, 0x07ffffff).ram(); // 128 megabytes
	map(0xf0000000, 0xf7ffffff).ram().share("nv2a_share"); // 3d accelerator wants this
	map(0xfd000000, 0xfdffffff).ram().rw(FUNC(xbox_base_state::geforce_r), FUNC(xbox_base_state::geforce_w));
	map(0xfed00000, 0xfed003ff).rw(FUNC(xbox_base_state::ohci_usb_r), FUNC(xbox_base_state::ohci_usb_w));
	map(0xfed08000, 0xfed083ff).rw(FUNC(xbox_base_state::ohci_usb2_r), FUNC(xbox_base_state::ohci_usb2_w));
	map(0xfe800000, 0xfe87ffff).rw(FUNC(xbox_base_state::audio_apu_r), FUNC(xbox_base_state::audio_apu_w));
	map(0xfec00000, 0xfec00fff).rw(FUNC(xbox_base_state::audio_ac93_r), FUNC(xbox_base_state::audio_ac93_w));
	map(0xfef00000, 0xfef003ff).rw(FUNC(xbox_base_state::network_r), FUNC(xbox_base_state::network_w));
}

void xbox_base_state::xbox_base_map_io(address_map &map)
{
	map(0x01f0, 0x01f7).rw(":pci:09.0:ide1", FUNC(bus_master_ide_controller_device::cs0_r), FUNC(bus_master_ide_controller_device::cs0_w));
	map(0x002e, 0x002f).rw(FUNC(xbox_base_state::superio_read), FUNC(xbox_base_state::superio_write));
	map(0x03f8, 0x03ff).rw(FUNC(xbox_base_state::superiors232_read), FUNC(xbox_base_state::superiors232_write));
	map(0x0cf8, 0x0cff).rw("pcibus", FUNC(pci_bus_legacy_device::read), FUNC(pci_bus_legacy_device::write));
	map(0x8000, 0x80ff).rw(FUNC(xbox_base_state::dummy_r), FUNC(xbox_base_state::dummy_w)); // lpc bridge
	map(0xc000, 0xc00f).rw(FUNC(xbox_base_state::smbus_r), FUNC(xbox_base_state::smbus_w));
	map(0xc200, 0xc21f).rw(FUNC(xbox_base_state::smbus2_r), FUNC(xbox_base_state::smbus2_w));
	map(0xd000, 0xd0ff).noprw(); // ac97
	map(0xd200, 0xd27f).noprw(); // ac97
	map(0xe000, 0xe007).rw(FUNC(xbox_base_state::networkio_r), FUNC(xbox_base_state::networkio_w));
	map(0xff60, 0xff6f).rw("ide", FUNC(bus_master_ide_controller_device::bmdma_r), FUNC(bus_master_ide_controller_device::bmdma_w));
}
#endif

void xbox_base_state::xbox_base(machine_config &config)
{
	/* basic machine hardware */
	PENTIUM3(config, m_maincpu, 733333333); /* Wrong! family 6 model 8 stepping 10 */
	m_maincpu->set_irq_acknowledge_callback(FUNC(xbox_base_state::irq_callback));

	config.m_minimum_quantum = attotime::from_hz(6000);

	PCI_ROOT(config,        ":pci", 0);
	NV2A_HOST(config,       ":pci:00.0", 0, m_maincpu);
	NV2A_RAM(config,        ":pci:00.3", 0, 128); // 128 megabytes
	MCPX_ISALPC(config,     ":pci:01.0", 0, 0).interrupt_output().set(FUNC(xbox_base_state::maincpu_interrupt));
	XBOX_SUPERIO(config,    ":pci:01.0:0", 0);
	MCPX_SMBUS(config,      ":pci:01.1", 0).interrupt_handler().set(FUNC(xbox_base_state::smbus_interrupt_changed));
	XBOX_PIC16LC(config,    ":pci:01.1:110", 0); // these 3 are on smbus number 1
	XBOX_CX25871(config,    ":pci:01.1:145", 0);
	XBOX_EEPROM(config,     ":pci:01.1:154", 0);
	MCPX_OHCI(config,       ":pci:02.0", 0).interrupt_handler().set(FUNC(xbox_base_state::ohci_usb_interrupt_changed));
	MCPX_OHCI(config,       ":pci:03.0", 0);
	MCPX_ETH(config,        ":pci:04.0", 0);
	MCPX_APU(config,        ":pci:05.0", 0, m_maincpu);
	MCPX_AC97_AUDIO(config, ":pci:06.0", 0);
	MCPX_AC97_MODEM(config, ":pci:06.1", 0);
	PCI_BRIDGE(config,      ":pci:08.0", 0, 0x10de01b8, 0);
	MCPX_IDE(config,        ":pci:09.0", 0).pri_interrupt_handler().set(FUNC(xbox_base_state::ide_interrupt_changed));
	NV2A_AGP(config,        ":pci:1e.0", 0, 0x10de01b7, 0);
	NV2A_GPU(config,        ":pci:1e.0:00.0", 0, m_maincpu).interrupt_handler().set(FUNC(xbox_base_state::nv2a_interrupt_changed));

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500));  /* not accurate */
	screen.set_size(640, 480);
	screen.set_visarea(0, 639, 0, 479);
	screen.set_screen_update(FUNC(xbox_base_state::screen_update_callback));
	screen.screen_vblank().set(FUNC(xbox_base_state::vblank_callback));
}
