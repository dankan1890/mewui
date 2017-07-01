// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*********************************************************************

    dvdisasm.c

    Disassembly debugger view.

***************************************************************************/

#include "emu.h"
#include "debugvw.h"
#include "dvdisasm.h"
#include "debugcpu.h"
#include "debugger.h"


//**************************************************************************
//  DEBUG VIEW DISASM SOURCE
//**************************************************************************

//-------------------------------------------------
//  debug_view_disasm_source - constructor
//-------------------------------------------------

debug_view_disasm_source::debug_view_disasm_source(const char *name, device_t &device)
	: debug_view_source(name, &device),
		m_disasmintf(dynamic_cast<device_disasm_interface *>(&device)),
		m_space(device.memory().space(AS_PROGRAM)),
		m_decrypted_space(device.memory().has_space(AS_DECRYPTED_OPCODES) ? device.memory().space(AS_DECRYPTED_OPCODES) : device.memory().space(AS_PROGRAM))
{
}



//**************************************************************************
//  DEBUG VIEW DISASM
//**************************************************************************

const int debug_view_disasm::DEFAULT_DASM_LINES, debug_view_disasm::DEFAULT_DASM_WIDTH, debug_view_disasm::DASM_MAX_BYTES;


//-------------------------------------------------
//  debug_view_disasm - constructor
//-------------------------------------------------

debug_view_disasm::debug_view_disasm(running_machine &machine, debug_view_osd_update_func osdupdate, void *osdprivate)
	: debug_view(machine, DVT_DISASSEMBLY, osdupdate, osdprivate),
		m_right_column(DASM_RIGHTCOL_RAW),
		m_backwards_steps(3),
		m_dasm_width(DEFAULT_DASM_WIDTH),
		m_last_direct_raw(nullptr),
		m_last_direct_decrypted(nullptr),
		m_last_change_count(0),
		m_last_pcbyte(0),
		m_divider1(0),
		m_divider2(0),
		m_divider3(0),
		m_expression(machine)
{
	// fail if no available sources
	enumerate_sources();
	if (m_source_list.count() == 0)
		throw std::bad_alloc();

	// count the number of comments
	int total_comments = 0;
	for (const debug_view_source &source : m_source_list)
	{
		const debug_view_disasm_source &dasmsource = downcast<const debug_view_disasm_source &>(source);
		total_comments += dasmsource.device()->debug()->comment_count();
	}

	// configure the view
	m_total.y = DEFAULT_DASM_LINES;
	m_supports_cursor = true;
}


//-------------------------------------------------
//  ~debug_view_disasm - destructor
//-------------------------------------------------

debug_view_disasm::~debug_view_disasm()
{
}


//-------------------------------------------------
//  enumerate_sources - enumerate all possible
//  sources for a disassembly view
//-------------------------------------------------

void debug_view_disasm::enumerate_sources()
{
	// start with an empty list
	m_source_list.reset();

	// iterate over devices with disassembly interfaces
	std::string name;
	for (device_disasm_interface &dasm : disasm_interface_iterator(machine().root_device()))
	{
		name = string_format("%s '%s'", dasm.device().name(), dasm.device().tag());
		if (dasm.device().memory().space_config(AS_PROGRAM)!=nullptr)
			m_source_list.append(*global_alloc(debug_view_disasm_source(name.c_str(), dasm.device())));
	}

	// reset the source to a known good entry
	set_source(*m_source_list.first());
}


//-------------------------------------------------
//  view_notify - handle notification of updates
//  to cursor changes
//-------------------------------------------------

void debug_view_disasm::view_notify(debug_view_notification type)
{
	if (type == VIEW_NOTIFY_CURSOR_CHANGED)
		adjust_visible_y_for_cursor();

	else if (type == VIEW_NOTIFY_SOURCE_CHANGED)
		m_expression.set_context(&downcast<const debug_view_disasm_source *>(m_source)->device()->debug()->symtable());
}


//-------------------------------------------------
//  view_char - handle a character typed within
//  the current view
//-------------------------------------------------

void debug_view_disasm::view_char(int chval)
{
	debug_view_xy origcursor = m_cursor;
	u8 end_buffer = 3;
	s32 temp;

	switch (chval)
	{
		case DCH_UP:
			if (m_cursor.y > 0)
				m_cursor.y--;
			break;

		case DCH_DOWN:
			if (m_cursor.y < m_total.y - 1)
				m_cursor.y++;
			break;

		case DCH_PUP:
			temp = m_cursor.y - (m_visible.y - end_buffer);
			if (temp < 0)
				m_cursor.y = 0;
			else
				m_cursor.y = temp;
			break;

		case DCH_PDOWN:
			temp = m_cursor.y + (m_visible.y - end_buffer);
			if (temp > m_total.y - 1)
				m_cursor.y = m_total.y - 1;
			else
				m_cursor.y = temp;
			break;

		case DCH_HOME:              // set the active column to the PC
		{
			const debug_view_disasm_source &source = downcast<const debug_view_disasm_source &>(*m_source);
			offs_t pc = source.m_space.address_to_byte(source.device()->safe_pcbase()) & source.m_space.logbytemask();

			// figure out which row the pc is on
			for (unsigned int curline = 0; curline < m_byteaddress.size(); curline++)
				if (m_byteaddress[curline] == pc)
					m_cursor.y = curline;
			break;
		}

		case DCH_CTRLHOME:
			m_cursor.y = 0;
			break;

		case DCH_CTRLEND:
			m_cursor.y = m_total.y - 1;
			break;
	}

	/* send a cursor changed notification */
	if (m_cursor.y != origcursor.y)
	{
		begin_update();
		view_notify(VIEW_NOTIFY_CURSOR_CHANGED);
		m_update_pending = true;
		end_update();
	}
}


//-------------------------------------------------
//  view_click - handle a mouse click within the
//  current view
//-------------------------------------------------

void debug_view_disasm::view_click(const int button, const debug_view_xy& pos)
{
	const debug_view_xy origcursor = m_cursor;
	m_cursor = pos;

	/* cursor popup|toggle */
	bool cursorVisible = true;
	if (m_cursor.y == origcursor.y)
	{
		cursorVisible = !m_cursor_visible;
	}

	/* send a cursor changed notification */
	begin_update();
	m_cursor_visible = cursorVisible;
	view_notify(VIEW_NOTIFY_CURSOR_CHANGED);
	m_update_pending = true;
	end_update();
}


//-------------------------------------------------
//  find_pc_backwards - back up the specified
//  number of instructions from the given PC
//-------------------------------------------------

offs_t debug_view_disasm::find_pc_backwards(offs_t targetpc, int numinstrs)
{
	const debug_view_disasm_source &source = downcast<const debug_view_disasm_source &>(*m_source);

	// compute the increment
	int minlen = source.m_space.byte_to_address(source.m_disasmintf->min_opcode_bytes());
	if (minlen == 0) minlen = 1;
	int maxlen = source.m_space.byte_to_address(source.m_disasmintf->max_opcode_bytes());
	if (maxlen == 0) maxlen = 1;

	// start off numinstrs back
	offs_t curpc = targetpc - minlen * numinstrs;
	if (curpc > targetpc)
		curpc = 0;

	/* loop until we find what we are looking for */
	offs_t targetpcbyte = source.m_space.address_to_byte(targetpc) & source.m_space.logbytemask();
	offs_t fillpcbyte = targetpcbyte;
	offs_t lastgoodpc = targetpc;
	while (1)
	{
		// fill the buffer up to the target
		offs_t curpcbyte = source.m_space.address_to_byte(curpc) & source.m_space.logbytemask();
		u8 opbuf[1024], argbuf[1024];
		while (curpcbyte < fillpcbyte)
		{
			fillpcbyte--;
			opbuf[1000 + fillpcbyte - targetpcbyte] = machine().debugger().cpu().read_opcode(source.m_decrypted_space, fillpcbyte, 1);
			argbuf[1000 + fillpcbyte - targetpcbyte] = machine().debugger().cpu().read_opcode(source.m_space, fillpcbyte, 1);
		}

		// loop until we get past the target instruction
		int instcount = 0;
		int instlen;
		offs_t scanpc;
		for (scanpc = curpc; scanpc < targetpc; scanpc += instlen)
		{
			offs_t scanpcbyte = source.m_space.address_to_byte(scanpc) & source.m_space.logbytemask();
			offs_t physpcbyte = scanpcbyte;

			// get the disassembly, but only if mapped
			instlen = 1;
			if (source.m_space.device().memory().translate(source.m_space.spacenum(), TRANSLATE_FETCH, physpcbyte))
			{
				std::ostringstream dasmbuffer;
				instlen = source.m_disasmintf->disassemble(dasmbuffer, scanpc, &opbuf[1000 + scanpcbyte - targetpcbyte], &argbuf[1000 + scanpcbyte - targetpcbyte]) & DASMFLAG_LENGTHMASK;
			}

			// count this one
			instcount++;
		}

		// if we ended up right on targetpc, this is a good candidate
		if (scanpc == targetpc && instcount <= numinstrs)
			lastgoodpc = curpc;

		// we're also done if we go back too far
		if (targetpc - curpc >= numinstrs * maxlen)
			break;

		// and if we hit 0, we're done
		if (curpc == 0)
			break;

		// back up one more and try again
		curpc -= minlen;
		if (curpc > targetpc)
			curpc = 0;
	}

	return lastgoodpc;
}


//-------------------------------------------------
//  generate_bytes - generate the opcode byte
//  values
//-------------------------------------------------

void debug_view_disasm::generate_bytes(offs_t pcbyte, int numbytes, int minbytes, int maxchars, bool encrypted)
{
	const debug_view_disasm_source &source = downcast<const debug_view_disasm_source &>(*m_source);
	const int char_num = source.m_space.is_octal() ? 3 : 2;
	const int base = m_dasm.tellp();

	// output the first value
	int offset = 0;
	if (maxchars >= char_num * minbytes)
		offset += util::stream_format(m_dasm, source.m_space.is_octal() ? "%0*o" : "%0*X", minbytes * char_num, machine().debugger().cpu().read_opcode(source.m_decrypted_space, pcbyte, minbytes));

	// output subsequent values
	int byte;
	for (byte = minbytes; byte < numbytes && offset + 1 + char_num * minbytes < maxchars; byte += minbytes)
		offset += util::stream_format(m_dasm, source.m_space.is_octal() ? " %0*o" : " %0*X", minbytes * char_num, machine().debugger().cpu().read_opcode(encrypted ? source.m_space : source.m_decrypted_space, pcbyte + byte, minbytes));

	// if we ran out of room, indicate more
	if ((byte < numbytes) && (byte != minbytes) && (maxchars > 3))
		m_dasm.seekp(base + maxchars - 4) << "...";
}


//-------------------------------------------------
//  recompute - recompute selected info for the
//  disassembly view
//-------------------------------------------------

bool debug_view_disasm::recompute(offs_t pc, int startline, int lines)
{
	util::ovectorstream buffer;
	bool changed = false;
	const debug_view_disasm_source &source = downcast<const debug_view_disasm_source &>(*m_source);
	const int char_num = source.m_space.is_octal() ? 3 : 2;

	// determine how many characters we need for an address and set the divider
	m_divider1 = 1 + (source.m_space.logaddrchars()/2*char_num) + 1;

	// assume a fixed number of characters for the disassembly
	m_divider2 = m_divider1 + 1 + m_dasm_width + 1;

	// determine how many bytes we might need to display
	const int minbytes = source.m_disasmintf->min_opcode_bytes();
	const int maxbytes = source.m_disasmintf->max_opcode_bytes();

	// ensure that the PC is aligned to the minimum opcode size
	pc &= ~source.m_space.byte_to_address_end(minbytes - 1);

	// set the width of the third column according to display mode
	if (m_right_column == DASM_RIGHTCOL_RAW || m_right_column == DASM_RIGHTCOL_ENCRYPTED)
	{
		int const maxbytes_clamped = (std::min)(maxbytes, DASM_MAX_BYTES);
		m_total.x = m_divider2 + 1 + char_num * maxbytes_clamped + (maxbytes_clamped / minbytes - 1) + 1;
	}
	else if (m_right_column == DASM_RIGHTCOL_COMMENTS)
		m_total.x = m_divider2 + 1 + 50;        // DEBUG_COMMENT_MAX_LINE_LENGTH
	else
		m_total.x = m_divider2 + 1;

	// allocate address array
	m_byteaddress.resize(m_total.y);

	// allocate disassembly buffer
	const auto total_bytes = m_total.x * m_total.y;
	m_dasm.clear();
	m_dasm.reserve(total_bytes).seekp(total_bytes);

	// iterate over lines
	for (int line = 0; line < lines; line++)
	{
		// convert PC to a byte offset
		const offs_t pcbyte = source.m_space.address_to_byte(pc) & source.m_space.logbytemask();

		// save a copy of the previous line as a backup if we're only doing one line
		const auto instr = startline + line;
		const auto base = instr * m_total.x;
		char oldbuf[100];
		if (lines == 1)
			std::memcpy(oldbuf, &m_dasm.vec()[base], (std::min<std::size_t>)(sizeof(oldbuf), m_total.x));

		// convert back and set the address of this instruction
		m_byteaddress[instr] = pcbyte;
		m_dasm.clear();
		util::stream_format(m_dasm.seekp(base),
			source.m_space.is_octal() ? " %0*o  " : " %0*X  ",
			source.m_space.logaddrchars()/2*char_num, source.m_space.byte_to_address(pcbyte));

		// make sure we can translate the address, and then disassemble the result
		buffer.clear();
		buffer.seekp(0);
		int numbytes = 0;
		offs_t physpcbyte = pcbyte;
		if (source.m_space.device().memory().translate(source.m_space.spacenum(), TRANSLATE_FETCH_DEBUG, physpcbyte))
		{
			u8 opbuf[64], argbuf[64];

			// fetch the bytes up to the maximum
			for (numbytes = 0; numbytes < maxbytes; numbytes++)
			{
				opbuf[numbytes] = machine().debugger().cpu().read_opcode(source.m_decrypted_space, pcbyte + numbytes, 1);
				argbuf[numbytes] = machine().debugger().cpu().read_opcode(source.m_space, pcbyte + numbytes, 1);
			}

			// disassemble the result
			pc += numbytes = source.m_disasmintf->disassemble(buffer, pc & source.m_space.logaddrmask(), opbuf, argbuf) & DASMFLAG_LENGTHMASK;
		}
		else
			buffer << "<unmapped>";

		buffer.put('\0');

		// append the disassembly to the buffer
		util::stream_format(m_dasm.seekp(base + m_divider1 + 1), "%2$-*1$.*1$s  ", m_dasm_width, &buffer.vec()[0]);

		// output the right column
		if (m_right_column == DASM_RIGHTCOL_RAW || m_right_column == DASM_RIGHTCOL_ENCRYPTED)
		{
			// get the bytes
			numbytes = source.m_space.address_to_byte(numbytes) & source.m_space.logbytemask();
			m_dasm.seekp(base + m_divider2);
			generate_bytes(pcbyte, numbytes, minbytes, m_total.x - m_divider2, m_right_column == DASM_RIGHTCOL_ENCRYPTED);
		}
		else if (m_right_column == DASM_RIGHTCOL_COMMENTS)
		{
			// get and add the comment, if present
			const offs_t comment_address = source.m_space.byte_to_address(m_byteaddress[instr]);
			const char *const text = source.device()->debug()->comment_text(comment_address);
			if (text != nullptr)
				util::stream_format(m_dasm.seekp(base + m_divider2), "// %.*s", m_total.x - m_divider2 - 4, text);
		}
		m_dasm.put('\0');

		// see if the line changed at all
		if (lines == 1 && strncmp(oldbuf, &m_dasm.vec()[base], (std::min<std::size_t>)(sizeof(oldbuf), m_total.x)) != 0)
			changed = true;
	}

	// update opcode base information
	m_last_direct_decrypted = source.m_decrypted_space.direct().ptr();
	m_last_direct_raw = source.m_space.direct().ptr();
	m_last_change_count = source.device()->debug()->comment_change_count();

	// no longer need to recompute
	m_recompute = false;
	return changed;
}


//-------------------------------------------------
//  view_update - update the contents of the
//  disassembly view
//-------------------------------------------------

void debug_view_disasm::view_update()
{
	const debug_view_disasm_source &source = downcast<const debug_view_disasm_source &>(*m_source);

	offs_t pc = source.device()->safe_pcbase();
	offs_t pcbyte = source.m_space.address_to_byte(pc) & source.m_space.logbytemask();

	// update our context; if the expression is dirty, recompute
	if (m_expression.dirty())
		m_recompute = true;

	// if we're tracking a value, make sure it is visible
	u64 previous = m_expression.last_value();
	u64 result = m_expression.value();
	if (result != previous)
	{
		offs_t resultbyte = source.m_space.address_to_byte(result) & source.m_space.logbytemask();

		// see if the new result is an address we already have
		u32 row;
		for (row = 0; row < m_byteaddress.size(); row++)
			if (m_byteaddress[row] == resultbyte)
				break;

		// if we didn't find it, or if it's really close to the bottom, recompute
		if (row == m_byteaddress.size() || row >= m_total.y - m_visible.y)
			m_recompute = true;

		// otherwise, if it's not visible, adjust the view so it is
		else if (row < m_topleft.y || row >= m_topleft.y + m_visible.y - 2)
			m_topleft.y = (row > 3) ? row - 3 : 0;
	}

	// if the opcode base has changed, rework things
	if (source.m_decrypted_space.direct().ptr() != m_last_direct_decrypted || source.m_space.direct().ptr() != m_last_direct_raw)
		m_recompute = true;

	// if the comments have changed, redo it
	if (m_last_change_count != source.device()->debug()->comment_change_count())
		m_recompute = true;

	// if we need to recompute, do it
	bool recomputed_this_time = false;
recompute:
	if (m_recompute)
	{
		// recompute the view
		if (!m_byteaddress.empty() && m_last_change_count != source.device()->debug()->comment_change_count())
		{
			// smoosh us against the left column, but not the top row
			m_topleft.x = 0;

			// recompute from where we last recomputed!
			recompute(source.m_space.byte_to_address(m_byteaddress[0]), 0, m_total.y);
		}
		else
		{
			// determine the addresses of what we will display
			offs_t backpc = find_pc_backwards(u32(m_expression.value()), m_backwards_steps);

			// put ourselves back in the top left
			m_topleft.y = 0;
			m_topleft.x = 0;

			recompute(backpc, 0, m_total.y);
		}
		recomputed_this_time = true;
	}

	// figure out the row where the PC is and recompute the disassembly
	if (pcbyte != m_last_pcbyte)
	{
		// find the row with the PC on it
		for (u32 row = 0; row < m_visible.y; row++)
		{
			u32 effrow = m_topleft.y + row;
			if (effrow >= m_byteaddress.size())
				break;
			if (pcbyte == m_byteaddress[effrow])
			{
				// see if we changed
				bool changed = recompute(pc, effrow, 1);
				if (changed && !recomputed_this_time)
				{
					m_recompute = true;
					goto recompute;
				}

				// set the effective row and PC
				m_cursor.y = effrow;
				view_notify(VIEW_NOTIFY_CURSOR_CHANGED);
			}
		}
		m_last_pcbyte = pcbyte;
	}

	// loop over visible rows
	debug_view_char *dest = &m_viewdata[0];
	for (u32 row = 0; row < m_visible.y; row++)
	{
		u32 effrow = m_topleft.y + row;
		u32 col = 0;

		// if this visible row is valid, add it to the buffer
		u8 attrib = DCA_NORMAL;
		if (effrow < m_byteaddress.size())
		{
			// if we're on the line with the PC, recompute and hilight it
			if (pcbyte == m_byteaddress[effrow])
				attrib = DCA_CURRENT;

			// if we're on a line with a breakpoint, tag it changed
			else
			{
				for (device_debug::breakpoint *bp = source.device()->debug()->breakpoint_first(); bp != nullptr; bp = bp->next())
					if (m_byteaddress[effrow] == (source.m_space.address_to_byte(bp->address()) & source.m_space.logbytemask()))
						attrib = DCA_CHANGED;
			}

			// if we're on the active column and everything is couth, highlight it
			if (m_cursor_visible && effrow == m_cursor.y)
				attrib |= DCA_SELECTED;

			// if we've visited this pc, mark it as such
			if (source.device()->debug()->track_pc_visited(m_byteaddress[effrow]))
				attrib |= DCA_VISITED;

			// get the effective string
			const char *data = &m_dasm.vec()[effrow * m_total.x];
			u32 len = u32(strlen(data));

			// copy data
			u32 effcol = m_topleft.x;
			while (col < m_visible.x && effcol < len)
			{
				dest->byte = data[effcol++];
				dest->attrib = (effcol <= m_divider1 || effcol >= m_divider2) ? (attrib | DCA_ANCILLARY) : attrib;

				// comments are just green for now - maybe they shouldn't even be this?
				if (effcol >= m_divider2 && m_right_column == DASM_RIGHTCOL_COMMENTS)
					attrib |= DCA_COMMENT;

				dest++;
				col++;
			}
		}

		// fill the rest with blanks
		while (col < m_visible.x)
		{
			dest->byte = ' ';
			dest->attrib = (effrow < m_total.y) ? (attrib | DCA_ANCILLARY) : attrib;
			dest++;
			col++;
		}
	}
}


//-------------------------------------------------
//  selected_address - return the PC of the
//  currently selected address in the view
//-------------------------------------------------

offs_t debug_view_disasm::selected_address()
{
	flush_updates();
	return downcast<const debug_view_disasm_source &>(*m_source).m_space.byte_to_address(m_byteaddress[m_cursor.y]);
}


//-------------------------------------------------
//  set_sexpression - set the expression string
//  describing the home address
//-------------------------------------------------

void debug_view_disasm::set_expression(const char *expression)
{
	begin_update();
	m_expression.set_string(expression);
	m_recompute = m_update_pending = true;
	end_update();
}


//-------------------------------------------------
//  set_right_column - set the contents of the
//  right column
//-------------------------------------------------

void debug_view_disasm::set_right_column(disasm_right_column contents)
{
	begin_update();
	m_right_column = contents;
	m_recompute = m_update_pending = true;
	end_update();
}


//-------------------------------------------------
//  set_backward_steps - set the number of
//  instructions displayed before the home address
//-------------------------------------------------

void debug_view_disasm::set_backward_steps(u32 steps)
{
	begin_update();
	m_backwards_steps = steps;
	m_recompute = m_update_pending = true;
	end_update();
}


//-------------------------------------------------
//  set_disasm_width - set the width in characters
//  of the main disassembly section
//-------------------------------------------------

void debug_view_disasm::set_disasm_width(u32 width)
{
	begin_update();
	m_dasm_width = width;
	m_recompute = m_update_pending = true;
	end_update();
}


//-------------------------------------------------
//  set_selected_address - set the PC of the
//  currently selected address in the view
//-------------------------------------------------

void debug_view_disasm::set_selected_address(offs_t address)
{
	const debug_view_disasm_source &source = downcast<const debug_view_disasm_source &>(*m_source);
	offs_t byteaddress = source.m_space.address_to_byte(address) & source.m_space.logbytemask();
	for (int line = 0; line < m_total.y; line++)
		if (m_byteaddress[line] == byteaddress)
		{
			m_cursor.y = line;
			set_cursor_position(m_cursor);
			break;
		}
}
