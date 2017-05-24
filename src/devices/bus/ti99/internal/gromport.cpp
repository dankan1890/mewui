// license:LGPL-2.1+
// copyright-holders:Michael Zapf
/***************************************************************************
    GROM port - the cartridge port of the TI-99/4, TI-99/4A, and
    TI-99/8 console.

    The name refers to the main intended application scenario, that is,
    to host cartridges with GROMs. The second, wider port of the console is
    called I/O port and connects to the Peripheral Expansion System
    (see peribox.h).

          LEFT

  /RESET  1||2   GND
      D7  3||4   CRUCLK
      D6  5||6   CRUIN
      D5  7||8   A15/CRUOUT
      D4  9||10  A13
      D3 11||12  A12
      D2 13||14  A11
      D1 15||16  A10
      D0 17||18  A9
     +5V 19||20  A8
     /GS 21||22  A7
     A14 23||24  A3
    DBIN 25||26  A6
  GRMCLK 27||28  A5
     -5V 29||30  A4
   READY 31||32  /WE
     GND 33||34  /ROMG
     GND 35||36  GND

         RIGHT

    Address bus line ordering, according to TI convention, is A0 (MSB) ... A15 (LSB).
    A0, A1, and A2 are not delivered to the port but decoded in the console:

    /ROMG is asserted for A0/A1/A2 = 011 (addresses 6000 - 7fff)
    /GS is asserted for A0...A5 = 100110 (addresses 9800 - 9bff)

    This means that a maximum of 8 KiB of direct memory space can be accessed.
    The /GS line is used to enable GROM circuits on the board (serial ROMs with
    own address counter, see tmc0430.h).

    When a cartridge is inserted the /RESET line is pulled to ground, which
    via a R/C component pulls down the /RESET input of the timer circuit for
    a short time, which in turn resets the CPU. In order to dump cartridges,
    a common procedure was to tape the /RESET line of the cartridge. However,
    inserting a cartridge without resetting often caused so much data bus noise
    that the console usually locked up.

    ----------------

    The TI-99/4A computer was strictly designed for cartridge usage. The basic
    console had only little directly accessible RAM and offered no ways to
    write machine language programs; cartridges were intended to add various
    capabilities, or just for running games.

    Beside the seemingly simple handling, Texas Instruments had own intentions
    behind their cartridge strategy. With only 8 KiB of direct access memory, a
    major part of the cartridge code had to be stored in GROMs, which had to be
    licensed from Texas Instruments. Thus they kept firm control over all
    software development.

    Over the years, and with the increasingly difficult market situations,
    TI's policies seem to have changed. This may be the reason that the built-in
    operating system actually allowed for running ROM-only cartridges until TI
    clipped out this part in the OS, banning cartridges without GROMs. Consoles
    with this modification were produced in 1983, TI's last year in the home
    computer business.

    Although only 8 KiB were available for direct addressing, clever techniques
    were invented by third-party manufacturers. The first extension was utilized
    by TI themselves in the Extended Basic cartridge which offers two banks
    of ROM contents. Switching between the banks is achieved by writing a value
    to the ROM space at 6000 or 6002. Later, cartridges with much more memory
    space were created, up to the Super Space II cartridge with 128 KiB of
    buffered SRAM.

    ----------------

    From the console case layout the GROM port was intended for a single
    cartridge only. Although never officially released, the operating system
    of the TI console supported a multi-cartridge extender with software
    switching. There were also extenders based on hardware switching (like
    the Navarone Widget).

    This emulation offers both variants as slot options:

    -gromport single   : default single cartridge connector
    -gromport multi    : software-switchable 4-slot cartridge extender
    -gromport gkracker : GRAM Kracker

    The last option enables another popular device, the GRAM Kracker. This is
    a device to be plugged into the cartridge slot with five manual switches
    at the front and an own cartridge slot at its top. It contains buffered
    SRAM, a built-in ROM, and a GROM simulator which simulates GROM behaviour
    when accessing the buffered RAM. Its main use is to provide editable
    storage for the read-only cartridge contents. Cartridges can be plugged
    into its slot; their contents can be read and written to disk, modified as
    needed, and loaded into the buffered RAM. Even the console GROMs can be
    copied into the device; despite running in parallel, the GROM simulator
    is able to override the console GROMs, thus allowing the user to install
    a customized OS.

***************************************************************************/
#include "emu.h"
#include "gromport.h"
#include "emuopts.h"
#include "image.h"
#include "softlist.h"
#include "unzip.h"
#include "xmlfile.h"

DEFINE_DEVICE_TYPE_NS(TI99_GROMPORT, 		bus::ti99::internal, gromport_device, "gromport", "TI-99 Cartridge port")
DEFINE_DEVICE_TYPE_NS(TI99_GROMPORT_SINGLE, bus::ti99::internal, ti99_single_cart_conn_device, "ti99_scartconn", "TI-99 Standard cartridge connector")
DEFINE_DEVICE_TYPE_NS(TI99_GROMPORT_MULTI,  bus::ti99::internal, ti99_multi_cart_conn_device,  "ti99_mcartconn", "TI-99 Multi-cartridge extender")
DEFINE_DEVICE_TYPE_NS(TI99_GROMPORT_GK,     bus::ti99::internal, ti99_gkracker_device,         "ti99_gkracker",  "Miller's Graphics GRAM Kracker")
DEFINE_DEVICE_TYPE_NS(TI99_CART, 			bus::ti99::internal, ti99_cartridge_device, "ti99cart", "TI-99 cartridge")

namespace bus { namespace ti99 { namespace internal {

#define TRACE_RPK 0
#define TRACE_CHANGE 0
#define TRACE_ILLWRITE 1
#define TRACE_CONFIG 0
#define TRACE_READ 0
#define TRACE_WRITE 0
#define TRACE_GROM 0
#define TRACE_GKRACKER 0
#define TRACE_CRU 0
#define TRACE_BANKSWITCH 0

#define GROM3_TAG "grom3"
#define GROM4_TAG "grom4"
#define GROM5_TAG "grom5"
#define GROM6_TAG "grom6"
#define GROM7_TAG "grom7"

#define CARTGROM_TAG "grom_contents"
#define CARTROM_TAG "rom_contents"
#define GKRACKER_ROM_TAG "gkracker_rom"
#define GKRACKER_NVRAM_TAG "gkracker_nvram"

namespace {

enum rpk_open_error
{
	RPK_OK,
	RPK_NOT_ZIP_FORMAT,
	RPK_CORRUPT,
	RPK_OUT_OF_MEMORY,
	RPK_XML_ERROR,
	RPK_INVALID_FILE_REF,
	RPK_ZIP_ERROR,
	RPK_ZIP_UNSUPPORTED,
	RPK_MISSING_RAM_LENGTH,
	RPK_INVALID_RAM_SPEC,
	RPK_UNKNOWN_RESOURCE_TYPE,
	RPK_INVALID_RESOURCE_REF,
	RPK_INVALID_LAYOUT,
	RPK_MISSING_LAYOUT,
	RPK_NO_PCB_OR_RESOURCES,
	RPK_UNKNOWN_PCB_TYPE
};


const char *const error_text[16] =
{
	"No error",
	"Not a RPK (zip) file",
	"Module definition corrupt",
	"Out of memory",
	"XML format error",
	"Invalid file reference",
	"Zip file error",
	"Unsupported zip version",
	"Missing RAM length",
	"Invalid RAM specification",
	"Unknown resource type",
	"Invalid resource reference",
	"layout.xml not valid",
	"Missing layout",
	"No pcb or resource found",
	"Unknown pcb type"
};


class rpk_exception
{
public:
	rpk_exception(rpk_open_error value): m_err(value), m_detail(nullptr) { }
	rpk_exception(rpk_open_error value, const char* detail) : m_err(value), m_detail(detail) { }

	const char* to_string()
	{
		// FIXME: this leaks memory - in some cases it returns a new buffer, in other cases it returns a pointer to a static string, so the caller can't know whether it needs to be cleaned up
		if (m_detail==nullptr) return error_text[(int)m_err];
		std::string errormsg = std::string(error_text[(int)m_err]).append(": ").append(m_detail);
		return core_strdup(errormsg.c_str());
	}

private:
	rpk_open_error m_err;
	const char* m_detail;
};

struct pcb_type
{
	int id;
	const char* name;
};

} // anonymous namespace


/*************************************************************************
    RPK support
*************************************************************************/

class ti99_cartridge_device::rpk_socket
{
public:
	rpk_socket(const char *id, int length, uint8_t *contents);
	rpk_socket(const char *id, int length, uint8_t *contents, const char *pathname);
	~rpk_socket() {}

	const char*     id() { return m_id; }
	int             get_content_length() { return m_length; }
	uint8_t*          get_contents() { return m_contents; }
	bool            persistent_ram() { return m_pathname != nullptr; }
	const char*     get_pathname() { return m_pathname; }
	void            cleanup() { if (m_contents != nullptr) global_free_array(m_contents); }

private:
	const char*     m_id;
	uint32_t          m_length;
	uint8_t*          m_contents;
	const char*     m_pathname;
};

class ti99_cartridge_device::rpk_reader
{
public:
	rpk_reader(const pcb_type *types) : m_types(types) { }

	rpk *open(emu_options &options, const char *filename, const char *system_name);

private:
	int find_file(util::archive_file &zip, const char *filename, uint32_t crc);
	std::unique_ptr<rpk_socket> load_rom_resource(util::archive_file &zip, util::xml::data_node const* rom_resource_node, const char* socketname);
	std::unique_ptr<rpk_socket> load_ram_resource(emu_options &options, util::xml::data_node const* ram_resource_node, const char* socketname, const char* system_name);
	const pcb_type* m_types;
};

class ti99_cartridge_device::rpk
{
	friend class rpk_reader;
public:
	rpk(emu_options& options, const char* sysname);
	~rpk();

	int         get_type(void) { return m_type; }
	uint8_t*      get_contents_of_socket(const char *socket_name);
	int         get_resource_length(const char *socket_name);
	void        close();

private:
	emu_options&            m_options;      // need this to find the path to the nvram files
	int                     m_type;
	//const char*             m_system_name;  // need this to find the path to the nvram files
	std::unordered_map<std::string,std::unique_ptr<rpk_socket>> m_sockets;

	void add_socket(const char* id, std::unique_ptr<rpk_socket> newsock);
};

gromport_device::gromport_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	:   bus8z_device(mconfig, TI99_GROMPORT, tag, owner, clock),
		device_slot_interface(mconfig, *this),
		m_connector(nullptr),
		m_reset_on_insert(true),
		m_console_ready(*this),
		m_console_reset(*this)
{ }

/*
    Reading via the GROM port. Only 13 address lines are passed through
    on the TI-99/4A, and 14 lines on the TI-99/8.
*/
READ8Z_MEMBER(gromport_device::readz)
{
	if (m_connector != nullptr)
	{
		m_connector->readz(space, offset & m_mask, value);
		if (TRACE_READ) if (m_romgq) logerror("Read %04x -> %02x\n", offset | 0x6000, *value);
	}
}

/*
    Writing via the GROM port. Only 13 address lines are passed through
    on the TI-99/4A, and 14 lines on the TI-99/8.
*/
WRITE8_MEMBER(gromport_device::write)
{
	if (m_connector != nullptr)
	{
		if (TRACE_WRITE) if (m_romgq) logerror("Write %04x <- %02x\n", offset | 0x6000, data);
		m_connector->write(space, offset & m_mask, data);
	}
}

READ8Z_MEMBER(gromport_device::crureadz)
{
	if (m_connector != nullptr)
		m_connector->crureadz(space, offset, value);
}

WRITE8_MEMBER(gromport_device::cruwrite)
{
	if (m_connector != nullptr)
		m_connector->cruwrite(space, offset, data);
}

WRITE_LINE_MEMBER(gromport_device::ready_line)
{
	m_console_ready(state);
}

/*
    Asserted when the console addresses cartridge rom.
*/
WRITE_LINE_MEMBER(gromport_device::romgq_line)
{
	m_romgq = state;
	if (m_connector != nullptr)
		m_connector->romgq_line(state);
}

WRITE_LINE_MEMBER(gromport_device::gclock_in)
{
	if (m_connector != nullptr)
		m_connector->gclock_in(state);
}

/*
    Combined GROM control lines.
*/
WRITE8_MEMBER( gromport_device::set_gromlines )
{
	if (m_connector != nullptr)
		m_connector->set_gromlines(space, offset, data);
}

void gromport_device::device_start()
{
	m_console_ready.resolve();
	m_console_reset.resolve();

	save_item(NAME(m_romgq));
}

void gromport_device::device_reset()
{
	m_reset_on_insert = (ioport("CARTRESET")->read()==0x01);
}

/*
    Shall we reset the console when a cartridge has been inserted?
    This is triggered by the cartridge by pulling down /RESET via a capacitor.
    Accordingly, when we put a tape over the /RESET contact we can avoid the
    reset, which is useful when we want to swap the cartridges while a program
    is runnning.
*/
void gromport_device::cartridge_inserted()
{
	if (m_reset_on_insert)
	{
		m_console_reset(ASSERT_LINE);
		m_console_reset(CLEAR_LINE);
	}
}

/*
    Find out whether the GROMs in the cartridge are idle. In that case,
    cut the clock line.
*/
bool gromport_device::is_grom_idle()
{
	if (m_connector != nullptr)
		return m_connector->is_grom_idle();
	else
		return false;
}

void gromport_device::device_config_complete()
{
	m_connector = downcast<cartridge_connector_device*>(subdevices().first());
}

INPUT_PORTS_START(gromport)
	PORT_START( "CARTRESET" )
	PORT_CONFNAME( 0x01, 0x01, "RESET on cartridge insert" )
		PORT_CONFSETTING(    0x00, DEF_STR( Off ) )
		PORT_CONFSETTING(    0x01, DEF_STR( On ) )
INPUT_PORTS_END

ioport_constructor gromport_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(gromport);
}

/***************************************************************************
    Different versions of cartridge connections

    single: the standard console connector, one cartridge
    multi:  a multi-cart expander, up to 4 cartridges with software selection
    gkracker: GRAMKracker, a device with NVRAM which allows the user to copy
              the contents of the cartridge plugged into its slot into the NVRAM
              and to modify it.

***************************************************************************/

cartridge_connector_device::cartridge_connector_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: bus8z_device(mconfig, type, tag, owner, clock),
	m_gromport(nullptr)
{
}

WRITE_LINE_MEMBER( cartridge_connector_device::ready_line )
{
	m_gromport->ready_line(state);
}

void cartridge_connector_device::device_config_complete()
{
	m_gromport = static_cast<gromport_device*>(owner());
}

ti99_single_cart_conn_device::ti99_single_cart_conn_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: cartridge_connector_device(mconfig, TI99_GROMPORT_SINGLE, tag, owner, clock),
	m_cartridge(nullptr)
{
}

READ8Z_MEMBER(ti99_single_cart_conn_device::readz)
{
	// Pass through
	m_cartridge->readz(space, offset, value);
}

WRITE8_MEMBER(ti99_single_cart_conn_device::write)
{
	// Pass through
	m_cartridge->write(space, offset, data);
}

READ8Z_MEMBER(ti99_single_cart_conn_device::crureadz)
{
	// Pass through
	m_cartridge->crureadz(space, offset, value);
}

WRITE8_MEMBER(ti99_single_cart_conn_device::cruwrite)
{
	// Pass through
	m_cartridge->cruwrite(space, offset, data);
}

WRITE_LINE_MEMBER(ti99_single_cart_conn_device::romgq_line)
{
	// Pass through
	m_cartridge->romgq_line(state);
}

/*
    Combined select lines
*/
WRITE8_MEMBER(ti99_single_cart_conn_device::set_gromlines)
{
	// Pass through
	m_cartridge->set_gromlines(space, offset, data);
}


WRITE_LINE_MEMBER(ti99_single_cart_conn_device::gclock_in)
{
	// Pass through
	m_cartridge->gclock_in(state);
}

/*
    Check whether the GROMs are idle.
*/
bool ti99_single_cart_conn_device::is_grom_idle()
{
	return m_cartridge->is_grom_idle();
}

void ti99_single_cart_conn_device::device_start()
{
	m_cartridge = static_cast<ti99_cartridge_device*>(subdevices().first());
}

void ti99_single_cart_conn_device::device_reset()
{
	m_cartridge->set_slot(0);
}

static MACHINE_CONFIG_START( single_slot )
	MCFG_DEVICE_ADD("cartridge", TI99_CART, 0)
MACHINE_CONFIG_END

machine_config_constructor ti99_single_cart_conn_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( single_slot );
}

/********************************************************/

/*
    The multi-cartridge extender

    This is a somewhat mythical device which was never available for the normal
    customer, but there are reports of the existence of such a device
    in development labs or demonstrations.

    The interesting thing about this is that the OS of the console
    fully supports this multi-cartridge extender, providing a selection
    option on the screen to switch between different plugged-in
    cartridges.

    The switching is possible by decoding address lines that are reserved
    for GROM access. GROMs are accessed via four separate addresses
    9800, 9802, 9C00, 9C02. The addressing scheme looks like this:

    1001 1Wxx xxxx xxM0        W = write(1), read(0), M = address(1), data(0)

    This leaves 8 bits (256 options) which are not decoded inside the
    console. As the complete address is routed to the port, some circuit
    just needs to decode the xxx lines and turn on the respective slot.

    One catch must be considered: Some cartridges contain ROMs which are
    directly accessed and not via ports. This means that the ROMs must
    be activated according to the slot that is selected.

    Another issue: Each GROM contains an own address counter and an ID.
    According to the ID the GROM only delivers data if the address counter
    is within the ID area (0 = 0000-1fff, 1=2000-3fff ... 7=e000-ffff).
    Thus it is essential that all GROMs stay in sync with their address
    counters. We have to route all address settings to all slots and their
    GROMs, even when the slot has not been selected before. The selected
    just shows its effect when data is read. In this case, only the
    data from the selected slot will be delivered.

    This may be considered as a design flaw within the complete cartridge system
    which eventually led to TI not manufacturing that device for the broad
    market.
*/

#define AUTO -1

ti99_multi_cart_conn_device::ti99_multi_cart_conn_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: cartridge_connector_device(mconfig, TI99_GROMPORT_MULTI, tag, owner, clock),
	m_active_slot(0),
	m_fixed_slot(0),
	m_next_free_slot(0)
{
}

/*
    Activates a slot in the multi-cartridge extender.
    Setting the slot is done by accessing the GROM ports using a
    specific address:

    slot 0:  0x9800 (0x9802, 0x9c00, 0x9c02)   : cartridge1
    slot 1:  0x9804 (0x9806, 0x9c04, 0x9c06)   : cartridge2
    ...
    slot 15: 0x983c (0x983e, 0x9c3c, 0x9c3e)   : cartridge16

    Scheme:

    1001 1Wxx xxxx xxM0 (M=mode; M=0: data, M=1: address; W=write)

    The following addresses are theoretically available, but the
    built-in OS does not use them; i.e. cartridges will not be
    included in the selection list, and their features will not be
    found by lookup, but they could be accessed directly by user
    programs.
    slot 16: 0x9840 (0x9842, 0x9c40, 0x9c42)
    ...
    slot 255:  0x9bfc (0x9bfe, 0x9ffc, 0x9ffe)

    Setting the GROM base should select one cartridge, but the ROMs in the
    CPU space must also be switched. As there is no known special mechanism
    we assume that by switching the GROM base, the ROM is automatically
    switched.

    Caution: This means that cartridges which do not have at least one
    GROM cannot be switched with this mechanism.

    We assume that the slot number is already calculated in the caller:
    slotnumber>=0 && slotnumber<=255

    NOTE: The OS will stop searching when it finds slots 1 and 2 empty.
    Interestingly, cartridge subroutines are found nevertheless, even when
    the cartridge is plugged into a higher slot.
*/
void ti99_multi_cart_conn_device::set_slot(int slotnumber)
{
	if (TRACE_CHANGE)
		if (m_active_slot != slotnumber) logerror("Setting cartslot to %d\n", slotnumber);

	if (m_fixed_slot==AUTO)
		m_active_slot = slotnumber;
	else
		m_active_slot = m_fixed_slot;
}

int ti99_multi_cart_conn_device::get_active_slot(bool changebase, offs_t offset)
{
	int slot;
	if (changebase)
	{
		// GROM selected?
		if (m_grom_selected) set_slot((offset>>2) & 0x00ff);
	}
	slot = m_active_slot;
	return slot;
}

void ti99_multi_cart_conn_device::insert(int index, ti99_cartridge_device* cart)
{
	if (TRACE_CHANGE) logerror("Insert slot %d\n", index);
	m_cartridge[index] = cart;
	m_gromport->cartridge_inserted();
}

void ti99_multi_cart_conn_device::remove(int index)
{
	if (TRACE_CHANGE) logerror("Remove slot %d\n", index);
	m_cartridge[index] = nullptr;
}

WRITE_LINE_MEMBER(ti99_multi_cart_conn_device::romgq_line)
{
	m_readrom = state;

	// Propagate to all slots
	for (int i=0; i < NUMBER_OF_CARTRIDGE_SLOTS; i++)
	{
		if (m_cartridge[i] != nullptr)
		{
			m_cartridge[i]->romgq_line(state);
		}
	}
}

/*
    Combined select lines
*/
WRITE8_MEMBER(ti99_multi_cart_conn_device::set_gromlines)
{
	// GROM selected?
	m_grom_selected = (data != 0);

	// Propagate to all slots
	for (int i=0; i < NUMBER_OF_CARTRIDGE_SLOTS; i++)
	{
		if (m_cartridge[i] != nullptr)
		{
			m_cartridge[i]->set_gromlines(space, offset, data);
		}
	}
}

WRITE_LINE_MEMBER(ti99_multi_cart_conn_device::gclock_in)
{
	// Propagate to all slots
	for (int i=0; i < NUMBER_OF_CARTRIDGE_SLOTS; i++)
	{
		if (m_cartridge[i] != nullptr)
		{
			m_cartridge[i]->gclock_in(state);
		}
	}
}

READ8Z_MEMBER(ti99_multi_cart_conn_device::readz)
{
	int slot = get_active_slot(true, offset);

	// If we have a GROM access, we need to send the read request to all
	// attached cartridges so the slot is irrelevant here. Each GROM
	// contains an internal address counter, and we must make sure they all stay in sync.
	if (m_grom_selected)
	{
		for (int i=0; i < NUMBER_OF_CARTRIDGE_SLOTS; i++)
		{
			if (m_cartridge[i] != nullptr)
			{
				uint8_t newval = *value;
				m_cartridge[i]->readz(space, offset, &newval, 0xff);
				if (i==slot)
				{
					*value = newval;
				}
			}
		}
	}
	else
	{
		if (slot < NUMBER_OF_CARTRIDGE_SLOTS && m_cartridge[slot] != nullptr)
		{
			m_cartridge[slot]->readz(space, offset, value, 0xff);
		}
	}
}

WRITE8_MEMBER(ti99_multi_cart_conn_device::write)
{
	// Same issue as above (read)
	// We don't have GRAM cartridges, anyway, so it's just used for setting the address.
	if (m_grom_selected)
	{
		for (auto & elem : m_cartridge)
		{
			if (elem != nullptr)
			{
				elem->write(space, offset, data, 0xff);
			}
		}
	}
	else
	{
		int slot = get_active_slot(true, offset);
		if (slot < NUMBER_OF_CARTRIDGE_SLOTS && m_cartridge[slot] != nullptr)
		{
			// logerror("writing %04x (slot %d) <- %02x\n", offset, slot, data);
			m_cartridge[slot]->write(space, offset, data, 0xff);
		}
	}
}

READ8Z_MEMBER(ti99_multi_cart_conn_device::crureadz)
{
	int slot = get_active_slot(false, offset);
	/* Sanity check. Higher slots are always empty. */
	if (slot >= NUMBER_OF_CARTRIDGE_SLOTS)
		return;

	if (m_cartridge[slot] != nullptr)
	{
		m_cartridge[slot]->crureadz(space, offset, value);
	}
}

WRITE8_MEMBER(ti99_multi_cart_conn_device::cruwrite)
{
	int slot = get_active_slot(true, offset);

	/* Sanity check. Higher slots are always empty. */
	if (slot >= NUMBER_OF_CARTRIDGE_SLOTS)
		return;

	if (m_cartridge[slot] != nullptr)
	{
		m_cartridge[slot]->cruwrite(space, offset, data);
	}
}

/*
    Check whether the GROMs are idle. Just ask the currently
    active cartridge.
*/
bool ti99_multi_cart_conn_device::is_grom_idle()
{
	/* Sanity check. Higher slots are always empty. */
	if (m_active_slot >= NUMBER_OF_CARTRIDGE_SLOTS)
		return false;

	if (m_cartridge[m_active_slot] != nullptr)
		return m_cartridge[m_active_slot]->is_grom_idle();

	return false;
}

void ti99_multi_cart_conn_device::device_start()
{
	m_next_free_slot = 0;
	m_active_slot = 0;
	for (auto & elem : m_cartridge)
	{
		elem = nullptr;
	}
	save_item(NAME(m_readrom));
	save_item(NAME(m_active_slot));
	save_item(NAME(m_fixed_slot));
	save_item(NAME(m_next_free_slot));
}

void ti99_multi_cart_conn_device::device_reset(void)
{
	m_active_slot = 0;
	m_fixed_slot = ioport("CARTSLOT")->read() - 1;
	m_grom_selected = false;
}

static MACHINE_CONFIG_START( multi_slot )
	MCFG_DEVICE_ADD("cartridge1", TI99_CART, 0)
	MCFG_DEVICE_ADD("cartridge2", TI99_CART, 0)
	MCFG_DEVICE_ADD("cartridge3", TI99_CART, 0)
	MCFG_DEVICE_ADD("cartridge4", TI99_CART, 0)
MACHINE_CONFIG_END

machine_config_constructor ti99_multi_cart_conn_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( multi_slot );
}

INPUT_CHANGED_MEMBER( ti99_multi_cart_conn_device::switch_changed )
{
	if (TRACE_CHANGE) logerror("Slot changed %d - %d\n", (int)((uint64_t)param & 0x07), newval);
	m_active_slot = m_fixed_slot = newval - 1;
}

INPUT_PORTS_START(multi_slot)
	PORT_START( "CARTSLOT" )
	PORT_DIPNAME( 0x0f, 0x00, "Multi-cartridge slot" ) PORT_CHANGED_MEMBER(DEVICE_SELF, ti99_multi_cart_conn_device, switch_changed, 0)
		PORT_DIPSETTING(    0x00, "Auto" )
		PORT_DIPSETTING(    0x01, "Slot 1" )
		PORT_DIPSETTING(    0x02, "Slot 2" )
		PORT_DIPSETTING(    0x03, "Slot 3" )
		PORT_DIPSETTING(    0x04, "Slot 4" )
INPUT_PORTS_END

ioport_constructor ti99_multi_cart_conn_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(multi_slot);
}

/**************************************************************************

    The GRAM Kracker was manufactured by Miller's Graphics and designed to
    fit into the cartridge slot.

    It offers one own cartridge slot at the top side and a row of switches
    at its front. It contains buffered SRAM circuits; the base version has
    56 KiB, and the extended version has 80 KiB.

    The operation of the GRAM Kracker is a bit complex and most likely
    bound to fail when you have no manual. Accordingly, this emulation is
    neither simpler nor more difficult to use.

    Concept of operation:

    Loader: The GRAM Kracker contains a small loader utility
    which allows you to dump cartridges and to load the contents into the
    SRAM of the GK. This loader utility is active when the switch 5 is put
    into "Loader on" state. The activated loader hides the TI BASIC
    interpreter in the console.

    Cartridges: When a cartridge is plugged into the GK the contents may be
    dumped and saved to disk by the loader. They cannot be directly copied
    into the GK because the memory locations are hidden by the cartridge.

    Loading the cartridge into the SRAM: With the cartridge unplugged, dumps
    can be loaded into the SRAM using the loader. This is one major use case
    of the GK, that is, to load dumps from disk, and in particular modified
    dumps. (There is no checksum, so contents may be freely changed.)

    Console dump: The GK is also able to dump the console GROMs and also to
    load them into the SRAM (only in the extended version). Due to a
    peculiarity of the TI console design it is possible to override the
    console GROMs with the contents in the cartridge slot.

    A standard procedure for use with the GK:

    Save cartridge:

    - Put switches to [Normal | OpSys | TI BASIC | W/P | Loader On]
    - Insert a disk image into disk drive 1
    - Plug in a cartridge
    - Reset the console (done automatically here)
    - Visit the option screen, press 1 for GRAM KRACKER
    - In the GK loader, select 2 for Save Module
    - Follow the on-screen instructions. Switches are set via the dip switch menu.
    - Enter a target file name
    - Saving is complete when the Save operation has been unmarked.

    Load cartridge:

    - Put switches to [Normal | OpSys | TI BASIC | W/P | Loader On]
    - Insert a disk image into disk drive 1
    - Make sure no cartridge is plugged in
    - Press 1 for GRAM KRACKER
    - Press 3 for Init Module space; follow instructions
    - Press 1 for Load Module; specify file name on disk
    - Loading is complete when the Load operation has been unmarked.

    Memory organisation:

    The console has three GROMs with 6 KiB size and occupying 8 KiB of address
    space each. These are called GROMs 0, 1, and 2. GROM 0 contains the common
    routines for the computer operation; GROMs 1 and 2 contain TI BASIC.

    Memory locations 6000-7fff are assigned to cartridge ROMs; in some
    cartridges, a second ROM bank can be used by writing a value to a special
    ROM access. This way, instead of 8 KiB we often have 16 KiB at these
    locations.

    Each cartridge can host up to 5 GROMs (called GROM 3, 4, 5, 6, and 7).
    As in the console, each one occupies 6 KiB in an 8 KiB window.

    The GRAM Kracker offers

    - a loader in an own GROM 1 (which hides the console GROM 1 when active,
    so we have no BASIC anymore). The contents of the loader must be found
    by the emulator in a file named ti99_gkracker.zip.

    - a complete set of 8 (simulated) GRAMs with full 8 KiB each (done by a
    simple addressing circuit); the basic version only offered GRAMs 3-7

    - 16 KiB of RAM memory space for the 6000-7fff area (called "bank 1" and "bank 2")

    Notes:

    - it is mandatory to turn off the loader when loading into GRAM 1, but only
    after prompted in the on-screen instructions, or the loader will crash
    - GRAM0 must be properly loaded if switch 2 is set to GRAM0 and the computer is reset
    - Switch 4 must not be in W/P position (write protect) when loading data
    into the GK (either other position will do).


***************************************************************************/
enum
{
	GK_OFF = 0,
	GK_NORMAL = 1,
	GK_GRAM0 = 0,
	GK_OPSYS = 1,
	GK_GRAM12 = 0,
	GK_TIBASIC = 1,
	GK_BANK1 = 0,
	GK_WP = 1,
	GK_BANK2 = 2,
	GK_LDON = 0,
	GK_LDOFF = 1
};

#define GKSWITCH1_TAG "GKSWITCH1"
#define GKSWITCH2_TAG "GKSWITCH2"
#define GKSWITCH3_TAG "GKSWITCH3"
#define GKSWITCH4_TAG "GKSWITCH4"
#define GKSWITCH5_TAG "GKSWITCH5"

ti99_gkracker_device::ti99_gkracker_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	:   cartridge_connector_device(mconfig, TI99_GROMPORT_GK, tag, owner, clock),
		device_nvram_interface(mconfig, *this),
		m_romspace_selected(false),
		m_ram_page(0),
		m_grom_address(0),
		m_ram_ptr(nullptr),
		m_grom_ptr(nullptr),
		m_waddr_LSB(false),
		m_cartridge(nullptr)
{
}

WRITE_LINE_MEMBER(ti99_gkracker_device::romgq_line)
{
	m_romspace_selected = (state==ASSERT_LINE);
	// Propagate to the guest
	if (m_cartridge != nullptr) m_cartridge->romgq_line(state);
}

/*
    Combined select lines
*/
WRITE8_MEMBER(ti99_gkracker_device::set_gromlines)
{
	m_grom_selected = (data != 0);
	if (m_cartridge != nullptr) m_cartridge->set_gromlines(space, offset, data);
}

WRITE_LINE_MEMBER(ti99_gkracker_device::gclock_in)
{
	if (m_cartridge != nullptr) m_cartridge->gclock_in(state);
}

/*
    Check whether the GROMs are idle.
*/
bool ti99_gkracker_device::is_grom_idle()
{
	return (m_cartridge != nullptr)? m_cartridge->is_grom_idle() : false;
}

READ8Z_MEMBER(ti99_gkracker_device::readz)
{
	if (m_grom_selected)
	{
		// Reads from the GRAM space of the GRAM Kracker.
		int id = ((m_grom_address & 0xe000)>>13)&0x07;

		// The GK does not have a readable address counter, but the console
		// GROMs and the GROMs of the guest cartridge will keep our address
		// counter up to date.
		if ((offset & 0x0002)==0)
		{
			// Reading data
			if    (((id==0) && (m_gk_switch[2]==GK_GRAM0))
				|| ((id==1) && (m_gk_switch[5]==GK_LDOFF) && (m_gk_switch[3]==GK_GRAM12))
				|| ((id==2) && (m_gk_switch[3]==GK_GRAM12))
				|| ((id>=3) && (m_gk_switch[1]==GK_NORMAL)))
				*value = m_ram_ptr[m_grom_address];

			if ((id==1) && (m_gk_switch[5]==GK_LDON))
				*value = m_grom_ptr[m_grom_address & 0x1fff];

			// The GK GROM emulation does not wrap at 8K boundaries.
			m_grom_address = (m_grom_address + 1) & 0xffff;

			// Reset the write address flipflop.
			m_waddr_LSB = false;
			if (TRACE_GKRACKER) logerror("GROM read -> %02x\n", *value);
		}
	}

	if (m_romspace_selected)
	{
		// Reads from the RAM space of the GRAM Kracker.

		// RAM is stored behind the GRAM area
		// Note that offset is 0000...1fff
		// When switch in middle position (WP) do bank select according to page flag
		if (m_gk_switch[1] == GK_NORMAL)
		{
			int base = ((m_gk_switch[4]==GK_BANK1) || ((m_gk_switch[4]==GK_WP) && (m_ram_page==0)))? 0x10000 : 0x12000;
			*value = m_ram_ptr[offset | base];
			if (TRACE_GKRACKER) logerror("Read %04x -> %02x\n", offset | 0x6000, *value);
		}
	}

	// If the guest has GROMs or ROMs they will override the GK contents
	if (m_cartridge != nullptr)
	{
		// For debugging
		uint8_t val1 = *value;

		// Read from the guest cartridge.
		m_cartridge->readz(space, offset, value, mem_mask);
		if (TRACE_GKRACKER)
			if (val1 != *value) logerror("Read (from guest) %04x -> %02x\n", offset, *value);
	}
}

WRITE8_MEMBER(ti99_gkracker_device::write)
{
	// write to the guest cartridge if present
	if (m_cartridge != nullptr)
	{
		m_cartridge->write(space, offset, data, mem_mask);
	}

	if (m_grom_selected)
	{
		// Write to the GRAM space of the GRAM Kracker.
		if ((offset & 0x0002)==0x0002)
		{
			// Set address
			if (m_waddr_LSB == true)
			{
				// Accept low address byte (second write)
				m_grom_address = (m_grom_address & 0xff00) | data;
				m_waddr_LSB = false;
				if (TRACE_GKRACKER) logerror("Set GROM address %04x\n", m_grom_address);
			}
			else
			{
				// Accept high address byte (first write)
				m_grom_address = (m_grom_address & 0x00ff) | (data << 8);
				m_waddr_LSB = true;
			}
		}
		else
		{
			// Write data byte to GRAM area.
			if (TRACE_GKRACKER) logerror("GROM write %04x(%04x) <- %02x\n", offset, m_grom_address, data);

			// According to manual:
			// Writing to GRAM 0: switch 2 set to GRAM 0 + Write protect switch (4) in 1 or 2 position
			// Writing to GRAM 1: switch 3 set to GRAM 1-2 + Loader off (5); write prot has no effect
			// Writing to GRAM 2: switch 3 set to GRAM 1-2 (write prot has no effect)
			// Writing to GRAM 3-7: switch 1 set to GK_NORMAL, no cartridge inserted
			// GK_NORMAL switch has no effect on GRAM 0-2

			int id = ((m_grom_address & 0xe000)>>13)&0x07;

			if    ((id==0 && m_gk_switch[2]==GK_GRAM0 && m_gk_switch[4]!=GK_WP)
				|| (id==1 && m_gk_switch[3]==GK_GRAM12 && m_gk_switch[5]==GK_LDOFF)
				|| (id==2 && m_gk_switch[3]==GK_GRAM12)
				|| (id>=3 && m_gk_switch[1]==GK_NORMAL))
				m_ram_ptr[m_grom_address] = data;

			// The GK GROM emulation does not wrap at 8K boundaries.
			m_grom_address = (m_grom_address + 1) & 0xffff;

			// Reset the write address flipflop.
			m_waddr_LSB = false;
		}
	}

	if (m_romspace_selected)
	{
		// Write to the RAM space of the GRAM Kracker
		if (TRACE_GKRACKER) logerror("Write %04x <- %02x\n", offset | 0x6000, data);

		if (m_gk_switch[1] == GK_NORMAL)
		{
			if (m_gk_switch[4]==GK_BANK1) m_ram_ptr[offset | 0x10000] = data;
			else if (m_gk_switch[4]==GK_BANK2) m_ram_ptr[offset | 0x12000] = data;
			// Switch in middle position (WP, implies auto-select according to the page flag)
			// This is handled like in Extended Basic (using addresses)
			else m_ram_page = (offset >> 1) & 1;
		}
	}
}

READ8Z_MEMBER( ti99_gkracker_device::crureadz )
{
	if (m_cartridge != nullptr) m_cartridge->crureadz(space, offset, value);
}

WRITE8_MEMBER( ti99_gkracker_device::cruwrite )
{
	if (m_cartridge != nullptr) m_cartridge->cruwrite(space, offset, data);
}

INPUT_CHANGED_MEMBER( ti99_gkracker_device::gk_changed )
{
	if (TRACE_GKRACKER) logerror("Input changed %d - %d\n", (int)((uint64_t)param & 0x07), newval);
	m_gk_switch[(uint64_t)param & 0x07] = newval;
}

void ti99_gkracker_device::insert(int index, ti99_cartridge_device* cart)
{
	if (TRACE_CHANGE) logerror("Insert cartridge\n");
	m_cartridge = cart;
	// Switch 1 has a third location for resetting. We do the reset by default
	// here. It can be turned off in the configuration.
	m_gromport->cartridge_inserted();
}

void ti99_gkracker_device::remove(int index)
{
	if (TRACE_CHANGE) logerror("Remove cartridge\n");
	m_cartridge = nullptr;
}

void ti99_gkracker_device::gk_install_menu(const char* menutext, int len, int ptr, int next, int start)
{
	const int base = 0x0000;
	m_ram_ptr[base + ptr] = (uint8_t)((next >> 8) & 0xff);
	m_ram_ptr[base + ptr+1] = (uint8_t)(next & 0xff);
	m_ram_ptr[base + ptr+2] = (uint8_t)((start >> 8) & 0xff);
	m_ram_ptr[base + ptr+3] = (uint8_t)(start & 0xff);

	m_ram_ptr[base + ptr+4] = (uint8_t)(len & 0xff);
	memcpy(m_ram_ptr + base + ptr+5, menutext, len);
}

/*
    Define the default for the GRAM Kracker device. The memory is preset with
    some sample entries which shall indicate that the memory has been tested
    by the manufacturer.
*/
void ti99_gkracker_device::nvram_default()
{
	if (TRACE_GKRACKER) logerror("Creating default NVRAM\n");
	memset(m_ram_ptr, 0, 81920);

	m_ram_ptr[0x6000] = 0xaa;
	m_ram_ptr[0x6001] = 0x01;
	m_ram_ptr[0x6002] = 0x01;

	m_ram_ptr[0x6006] = 0x60;
	m_ram_ptr[0x6007] = 0x20;

	gk_install_menu("GROM 3 OK", 9, 0x60e0, 0, 0x6100);
	gk_install_menu("GROM 4 OK", 9, 0x60c0, 0x60e0, 0x6100);
	gk_install_menu("GROM 5 OK", 9, 0x60a0, 0x60c0, 0x6100);
	gk_install_menu("GROM 6 OK", 9, 0x6080, 0x60a0, 0x6100);
	gk_install_menu("PROM   OK", 9, 0x6060, 0x6080, 0x6100);
	gk_install_menu("RAMS   OK", 9, 0x6040, 0x6060, 0x6100);
	gk_install_menu("OPTION GRAMS OK", 15, 0x6020, 0x6040, 0x6100);

	m_ram_ptr[0x6100] = 0x0b;       // GPL EXIT
}

void ti99_gkracker_device::nvram_read(emu_file &file)
{
	int readsize = file.read(m_ram_ptr, 81920);
	if (TRACE_GKRACKER) logerror("Reading NVRAM\n");
	// If we increased the size, fill the remaining parts with 0
	if (readsize < 81920)
	{
		memset(m_ram_ptr + readsize, 0, 81920-readsize);
	}
}

void ti99_gkracker_device::nvram_write(emu_file &file)
{
	if (TRACE_GKRACKER) logerror("Writing NVRAM\n");
	file.write(m_ram_ptr, 81920);
}

void ti99_gkracker_device::device_start()
{
	m_ram_ptr = memregion(GKRACKER_NVRAM_TAG)->base();
	m_grom_ptr = memregion(GKRACKER_ROM_TAG)->base();
	m_cartridge = nullptr;
	for (int i=1; i < 6; i++) m_gk_switch[i] = 0;
	save_pointer(NAME(m_gk_switch),6);
	save_item(NAME(m_romspace_selected));
	save_item(NAME(m_ram_page));
	save_item(NAME(m_grom_address));
	save_item(NAME(m_waddr_LSB));
}

void ti99_gkracker_device::device_reset()
{
	m_gk_switch[1] = ioport(GKSWITCH1_TAG)->read();
	m_gk_switch[2] = ioport(GKSWITCH2_TAG)->read();
	m_gk_switch[3] = ioport(GKSWITCH3_TAG)->read();
	m_gk_switch[4] = ioport(GKSWITCH4_TAG)->read();
	m_gk_switch[5] = ioport(GKSWITCH5_TAG)->read();
	m_grom_address = 0; // for the GROM emulation
	m_ram_page = 0;
	m_waddr_LSB = false;
	m_grom_selected = false;
}

static MACHINE_CONFIG_START( gkracker_slot )
	MCFG_DEVICE_ADD("cartridge", TI99_CART, 0)
MACHINE_CONFIG_END

/*
    The GRAMKracker ROM
*/
ROM_START( gkracker_rom )
	ROM_REGION(0x14000, GKRACKER_NVRAM_TAG, ROMREGION_ERASE00)
	ROM_REGION(0x2000, GKRACKER_ROM_TAG, 0)
	ROM_LOAD("gkracker.bin", 0x0000, 0x2000, CRC(86eaaf9f) SHA1(a3bd5257c63e190800921b52dbe3ffa91ad91113))
ROM_END

const tiny_rom_entry *ti99_gkracker_device::device_rom_region() const
{
	return ROM_NAME( gkracker_rom );
}

machine_config_constructor ti99_gkracker_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( gkracker_slot );
}

INPUT_PORTS_START(gkracker)
	PORT_START( GKSWITCH1_TAG )
	PORT_DIPNAME( 0x01, 0x01, "GK switch 1" ) PORT_CHANGED_MEMBER(DEVICE_SELF, ti99_gkracker_device, gk_changed, 1)
		PORT_DIPSETTING(    0x00, "GK Off" )
		PORT_DIPSETTING(    0x01, DEF_STR( Normal ) )

	PORT_START( GKSWITCH2_TAG )
	PORT_DIPNAME( 0x01, 0x01, "GK switch 2" ) PORT_CHANGED_MEMBER(DEVICE_SELF, ti99_gkracker_device, gk_changed, 2)
		PORT_DIPSETTING(    0x00, "GRAM 0" )
		PORT_DIPSETTING(    0x01, "Op Sys" )

	PORT_START( GKSWITCH3_TAG )
	PORT_DIPNAME( 0x01, 0x01, "GK switch 3" ) PORT_CHANGED_MEMBER(DEVICE_SELF, ti99_gkracker_device, gk_changed, 3)
		PORT_DIPSETTING(    0x00, "GRAM 1-2" )
		PORT_DIPSETTING(    0x01, "TI BASIC" )

	PORT_START( GKSWITCH4_TAG )
	PORT_DIPNAME( 0x03, 0x01, "GK switch 4" ) PORT_CHANGED_MEMBER(DEVICE_SELF, ti99_gkracker_device, gk_changed, 4)
		PORT_DIPSETTING(    0x00, "Bank 1" )
		PORT_DIPSETTING(    0x01, "W/P" )
		PORT_DIPSETTING(    0x02, "Bank 2" )

	PORT_START( GKSWITCH5_TAG )
	PORT_DIPNAME( 0x01, 0x00, "GK switch 5" ) PORT_CHANGED_MEMBER(DEVICE_SELF, ti99_gkracker_device, gk_changed, 5)
		PORT_DIPSETTING(    0x00, "Loader On" )
		PORT_DIPSETTING(    0x01, "Loader Off" )
INPUT_PORTS_END

ioport_constructor ti99_gkracker_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(gkracker);
}

/***************************************************************************
    Cartridge implementation

    Every cartridge is an instance of ti99_cartridge_device, implementing the
    device_image_interface. This means it is capable of loading cartridge
    data into its memory locations. All memory locations are organised as
    regions.

    The different cartridge versions are realised by different PCB instances.
    All PCBs are subclassed from ti99_cartridge_pcb.

***************************************************************************/
enum
{
	PCB_STANDARD=1,
	PCB_PAGED12K,
	PCB_PAGED16K,
	PCB_MINIMEM,
	PCB_SUPER,
	PCB_MBX,
	PCB_PAGED379I,
	PCB_PAGED378,
	PCB_PAGED377,
	PCB_PAGEDCRU,
	PCB_GROMEMU
};

static const pcb_type pcbdefs[] =
{
	{ PCB_STANDARD, "standard" },
	{ PCB_PAGED12K, "paged12k" },
	{ PCB_PAGED16K, "paged" },
	{ PCB_MINIMEM, "minimem" },
	{ PCB_SUPER, "super" },
	{ PCB_MBX, "mbx" },
	{ PCB_PAGED379I, "paged379i" },
	{ PCB_PAGED378, "paged378" },
	{ PCB_PAGED377, "paged377" },
	{ PCB_PAGEDCRU, "pagedcru" },
	{ PCB_GROMEMU, "gromemu" },
	{ 0, nullptr}
};

// Softlists do not support the cartridges with RAM yet
static const pcb_type sw_pcbdefs[] =
{
	{ PCB_STANDARD, "standard" },
	{ PCB_PAGED12K, "paged12k" },
	{ PCB_PAGED16K, "paged16k" },
	{ PCB_MINIMEM, "minimem" },
	{ PCB_SUPER, "super" },
	{ PCB_MBX, "mbx" },
	{ PCB_GROMEMU, "gromemu" },
	{ 0, nullptr}
};

ti99_cartridge_device::ti99_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
:   bus8z_device(mconfig, TI99_CART, tag, owner, clock),
	device_image_interface(mconfig, *this),
	m_pcbtype(0),
	m_slot(0),
	m_pcb(nullptr),
	m_connector(nullptr),
	m_rpk(nullptr)
{
}

void ti99_cartridge_device::prepare_cartridge()
{
	int rom2_length;

	uint8_t* grom_ptr;
	uint8_t* rom_ptr;

	memory_region *regg;
	memory_region *regr;

	// Initialize some values.
	m_pcb->m_rom_page = 0;
	m_pcb->m_rom_ptr = nullptr;
	m_pcb->m_ram_size = 0;
	m_pcb->m_ram_ptr = nullptr;
	m_pcb->m_ram_page = 0;

	for (int i=0; i < 5; i++) m_pcb->m_grom[i] = nullptr;

	m_pcb->m_grom_size = loaded_through_softlist() ? get_software_region_length("grom") : m_rpk->get_resource_length("grom_socket");
	if (TRACE_CONFIG) logerror("grom_socket.size=0x%04x\n", m_pcb->m_grom_size);

	if (m_pcb->m_grom_size > 0)
	{
		regg = memregion(CARTGROM_TAG);
		grom_ptr = loaded_through_softlist() ? get_software_region("grom") : m_rpk->get_contents_of_socket("grom_socket");
		memcpy(regg->base(), grom_ptr, m_pcb->m_grom_size);
		m_pcb->m_grom_ptr = regg->base();   // for gromemu
		m_pcb->m_grom_address = 0;          // for gromemu

		// Find the GROMs and keep their pointers
		m_pcb->set_grom_pointer(0, subdevice(GROM3_TAG));
		if (m_pcb->m_grom_size > 0x2000) m_pcb->set_grom_pointer(1, subdevice(GROM4_TAG));
		if (m_pcb->m_grom_size > 0x4000) m_pcb->set_grom_pointer(2, subdevice(GROM5_TAG));
		if (m_pcb->m_grom_size > 0x6000) m_pcb->set_grom_pointer(3, subdevice(GROM6_TAG));
		if (m_pcb->m_grom_size > 0x8000) m_pcb->set_grom_pointer(4, subdevice(GROM7_TAG));
	}

	m_pcb->m_rom_size = loaded_through_softlist() ? get_software_region_length("rom") : m_rpk->get_resource_length("rom_socket");
	if (m_pcb->m_rom_size > 0)
	{
		if (TRACE_CONFIG) logerror("rom size=0x%04x\n", m_pcb->m_rom_size);
		regr = memregion(CARTROM_TAG);
		rom_ptr = loaded_through_softlist() ? get_software_region("rom") : m_rpk->get_contents_of_socket("rom_socket");
		memcpy(regr->base(), rom_ptr, m_pcb->m_rom_size);
		// Set both pointers to the same region for now
		m_pcb->m_rom_ptr = regr->base();
	}

	// Softlist uses only one ROM area, no second socket
	if (!loaded_through_softlist())
	{
		rom2_length = m_rpk->get_resource_length("rom2_socket");
		if (rom2_length > 0)
		{
			// sizes do not differ between rom and rom2
			// We use the large cartrom space for the second bank as well
			regr = memregion(CARTROM_TAG);
			rom_ptr = m_rpk->get_contents_of_socket("rom2_socket");
			memcpy(regr->base() + 0x2000, rom_ptr, rom2_length);
		}
	}

	// (NV)RAM cartridges
	if (loaded_through_softlist())
	{
		// Do we have NVRAM?
		if (get_software_region("nvram")!=nullptr)
		{
			m_pcb->m_ram_size = get_software_region_length("nvram");
			m_pcb->m_nvram.resize(m_pcb->m_ram_size);
			m_pcb->m_ram_ptr = &m_pcb->m_nvram[0];
			battery_load(m_pcb->m_ram_ptr, m_pcb->m_ram_size, 0xff);
		}

		// Do we have RAM?
		if (get_software_region("ram")!=nullptr)
		{
			m_pcb->m_ram_size = get_software_region_length("ram");
			m_pcb->m_ram.resize(m_pcb->m_ram_size);
			m_pcb->m_ram_ptr = &m_pcb->m_ram[0];
		}
	}
	else
	{
		m_pcb->m_ram_size = m_rpk->get_resource_length("ram_socket");
		if (m_pcb->m_ram_size > 0)
		{
			// TODO: Consider to use a region as well. If so, do not forget to memcpy.
			m_pcb->m_ram_ptr = m_rpk->get_contents_of_socket("ram_socket");
		}
	}
}

/*
    Find the index of the cartridge name. We assume the format
    <name><number>, i.e. the number is the longest string from the right
    which can be interpreted as a number. Subtract 1.
*/
int ti99_cartridge_device::get_index_from_tagname()
{
	const char *mytag = tag();
	int maxlen = strlen(mytag);
	int i;

	for (i=maxlen-1; i >=0; i--)
		if (mytag[i] < 48 || mytag[i] > 57) break;

	if (i==maxlen-1) return 0;
	return atoi(mytag+i+1)-1;
}

image_init_result ti99_cartridge_device::call_load()
{
	// File name is in m_basename
	// return true = error
	if (TRACE_CHANGE) logerror("Loading %s in slot %s\n", basename());

	if (loaded_through_softlist())
	{
		if (TRACE_CONFIG) logerror("Using softlists\n");
		int i = 0;
		const char* pcb = get_feature("pcb");
		do
		{
			if (strcmp(pcb, sw_pcbdefs[i].name)==0)
			{
				m_pcbtype = sw_pcbdefs[i].id;
				break;
			}
			i++;
		} while (sw_pcbdefs[i].id != 0);
		if (TRACE_CONFIG) logerror("Cartridge type is %s (%d)\n", pcb, m_pcbtype);
		m_rpk = nullptr;
	}
	else
	{
		auto reader = new rpk_reader(pcbdefs);
		try
		{
			m_rpk = reader->open(machine().options(), filename(), machine().system().name);
			m_pcbtype = m_rpk->get_type();
		}
		catch (rpk_exception& err)
		{
			logerror("Failed to load cartridge '%s': %s\n", basename(), err.to_string());
			m_rpk = nullptr;
			m_err = IMAGE_ERROR_INVALIDIMAGE;
			return image_init_result::FAIL;
		}
	}

	switch (m_pcbtype)
	{
	case PCB_STANDARD:
		if (TRACE_CONFIG) logerror("Standard PCB\n");
		m_pcb = std::make_unique<ti99_standard_cartridge>();
		break;
	case PCB_PAGED12K:
		if (TRACE_CONFIG) logerror("Paged PCB 12K\n");
		m_pcb = std::make_unique<ti99_paged12k_cartridge>();
		break;
	case PCB_PAGED16K:
		if (TRACE_CONFIG) logerror("Paged PCB 16K\n");
		m_pcb = std::make_unique<ti99_paged16k_cartridge>();
		break;
	case PCB_MINIMEM:
		if (TRACE_CONFIG) logerror("Minimem PCB\n");
		m_pcb = std::make_unique<ti99_minimem_cartridge>();
		break;
	case PCB_SUPER:
		if (TRACE_CONFIG) logerror("Superspace PCB\n");
		m_pcb = std::make_unique<ti99_super_cartridge>();
		break;
	case PCB_MBX:
		if (TRACE_CONFIG) logerror("MBX PCB\n");
		m_pcb = std::make_unique<ti99_mbx_cartridge>();
		break;
	case PCB_PAGED379I:
		if (TRACE_CONFIG) logerror("Paged379i PCB\n");
		m_pcb = std::make_unique<ti99_paged379i_cartridge>();
		break;
	case PCB_PAGED378:
		if (TRACE_CONFIG) logerror("Paged378 PCB\n");
		m_pcb = std::make_unique<ti99_paged378_cartridge>();
		break;
	case PCB_PAGED377:
		if (TRACE_CONFIG) logerror("Paged377 PCB\n");
		m_pcb = std::make_unique<ti99_paged377_cartridge>();
		break;
	case PCB_PAGEDCRU:
		if (TRACE_CONFIG) logerror("PagedCRU PCB\n");
		m_pcb = std::make_unique<ti99_pagedcru_cartridge>();
		break;
	case PCB_GROMEMU:
		if (TRACE_CONFIG) logerror("Grom Emulation PCB\n");
		m_pcb = std::make_unique<ti99_gromemu_cartridge>();
		break;
	}

	prepare_cartridge();
	m_pcb->set_cartridge(this);
	m_pcb->set_tag(tag());
	m_slot = get_index_from_tagname();
	m_connector->insert(m_slot, this);
	return image_init_result::PASS;
}

void ti99_cartridge_device::call_unload()
{
	if (TRACE_CHANGE) logerror("Unload\n");
	if (m_rpk != nullptr)
	{
		m_rpk->close(); // will write NVRAM contents
		delete m_rpk;
	}
	else
	{
		// Softlist
		bool has_nvram = (get_software_region("nvram")!=nullptr);
		if (has_nvram)
		{
			int nvsize = get_software_region_length("nvram");
			battery_save(m_pcb->m_ram_ptr, nvsize);
		}
	}

	m_pcb = nullptr;
	m_connector->remove(m_slot);
}

void ti99_cartridge_device::set_slot(int i)
{
	m_slot = i;
}

READ8Z_MEMBER(ti99_cartridge_device::readz)
{
	if (m_pcb != nullptr)
		m_pcb->readz(space, offset, value);
}

WRITE8_MEMBER(ti99_cartridge_device::write)
{
	if (m_pcb != nullptr)
		m_pcb->write(space, offset, data);
}

READ8Z_MEMBER(ti99_cartridge_device::crureadz)
{
	if (m_pcb != nullptr) m_pcb->crureadz(space, offset, value);
}

WRITE8_MEMBER(ti99_cartridge_device::cruwrite)
{
	if (m_pcb != nullptr) m_pcb->cruwrite(space, offset, data);
}

WRITE_LINE_MEMBER( ti99_cartridge_device::ready_line )
{
	m_connector->ready_line(state);
}

WRITE_LINE_MEMBER( ti99_cartridge_device::romgq_line )
{
	if (m_pcb != nullptr)
	{
		m_pcb->romgq_line(state);
		m_readrom = state;
	}
}

/*
    Combined select lines
*/
WRITE8_MEMBER(ti99_cartridge_device::set_gromlines)
{
	if (m_pcb != nullptr) m_pcb->set_gromlines(space, offset, data);
}

WRITE_LINE_MEMBER(ti99_cartridge_device::gclock_in)
{
	if (m_pcb != nullptr) m_pcb->gclock_in(state);
}

bool ti99_cartridge_device::is_grom_idle()
{
	return (m_pcb != nullptr)? m_pcb->is_grom_idle() : false;
}

void ti99_cartridge_device::device_config_complete()
{
	m_connector = static_cast<cartridge_connector_device*>(owner());
}

/*
    5 GROMs that may be contained in a cartridge
*/
static MACHINE_CONFIG_START( ti99_cartridge )
	MCFG_GROM_ADD( GROM3_TAG, 3, CARTGROM_TAG, 0x0000, WRITELINE(ti99_cartridge_device, ready_line))
	MCFG_GROM_ADD( GROM4_TAG, 4, CARTGROM_TAG, 0x2000, WRITELINE(ti99_cartridge_device, ready_line))
	MCFG_GROM_ADD( GROM5_TAG, 5, CARTGROM_TAG, 0x4000, WRITELINE(ti99_cartridge_device, ready_line))
	MCFG_GROM_ADD( GROM6_TAG, 6, CARTGROM_TAG, 0x6000, WRITELINE(ti99_cartridge_device, ready_line))
	MCFG_GROM_ADD( GROM7_TAG, 7, CARTGROM_TAG, 0x8000, WRITELINE(ti99_cartridge_device, ready_line))
MACHINE_CONFIG_END

machine_config_constructor ti99_cartridge_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( ti99_cartridge );
}

/*
    Memory area for one cartridge. For most cartridges we only need 8 KiB for
    ROM contents, but cartridges of the "paged377" type have up to 2 MiB
    organised as selectable banks, so we must be sure there is enough space.
*/
ROM_START( cartridge_memory )
	ROM_REGION(0xa000, CARTGROM_TAG, ROMREGION_ERASE00)
	ROM_REGION(0x200000, CARTROM_TAG, ROMREGION_ERASE00)
ROM_END

const tiny_rom_entry *ti99_cartridge_device::device_rom_region() const
{
	return ROM_NAME( cartridge_memory );
}

/***************************************************************************
    Cartridge types
    Cartridges differ by the circuits on their PCB which hosts the ROMs.
    Some cartridges also have RAM, and some allow for switching between
    ROMs.

    Standard cartridge

    GROM space
    6000     77ff   8000     97ff   a000     b7ff   c000     d7ff   e000     f7ff
    |== GROM3 ==|...|== GROM4 ==|...|== GROM5 ==|...|== GROM6 ==|...|== GROM7 ==|


    ROM space
    6000          7000        7fff
    |             |              |
    |========== ROM1 ============|

***************************************************************************/

ti99_cartridge_pcb::ti99_cartridge_pcb()
	: m_cart(nullptr),
		m_grom_idle(false),
		m_grom_size(0),
		m_rom_size(0),
		m_ram_size(0),
		m_rom_ptr(nullptr),
		m_ram_ptr(nullptr),
		m_romspace_selected(false),
		m_rom_page(0),
		m_grom_ptr(nullptr),
		m_grom_address(0),
		m_ram_page(0),
		m_tag(nullptr)
{
}

void ti99_cartridge_pcb::set_cartridge(ti99_cartridge_device *cart)
{
	m_cart = cart;
}

READ8Z_MEMBER(ti99_cartridge_pcb::gromreadz)
{
	for (auto & elem : m_grom)
	{
		if (elem != nullptr)
		{
			elem->readz(space, offset, value, mem_mask);
		}
	}
}

WRITE8_MEMBER(ti99_cartridge_pcb::gromwrite)
{
	for (auto & elem : m_grom)
	{
		if (elem != nullptr)
		{
			elem->write(space, offset, data, mem_mask);
		}
	}
}

/*
    TI-99/4A cartridges can only occupy 8 KiB of CPU RAM space. For TI-99/8
    cartridges with up to 16 KiB we need a new PCB type. Unfortunately, such
    cartridges were never developed.
*/
READ8Z_MEMBER(ti99_cartridge_pcb::readz)
{
	if (m_romspace_selected)
	{
		if (m_rom_ptr!=nullptr)
		{
			*value = m_rom_ptr[offset & 0x1fff];
		}
	}
	else
	{
		// Will not return anything when not selected (preceding gsq=ASSERT)
		gromreadz(space, offset, value, mem_mask);
	}
}

WRITE8_MEMBER(ti99_cartridge_pcb::write)
{
	if (m_romspace_selected)
	{
		if (TRACE_ILLWRITE) m_cart->logerror("Cannot write to ROM space at %04x\n", offset);
	}
	else
	{
		// Will not change anything when not selected (preceding gsq=ASSERT)
		gromwrite(space, offset, data, mem_mask);
	}
}

READ8Z_MEMBER(ti99_cartridge_pcb::crureadz)
{
}

WRITE8_MEMBER(ti99_cartridge_pcb::cruwrite)
{
}

void ti99_cartridge_pcb::set_grom_pointer(int number, device_t *dev)
{
	m_grom[number] = static_cast<tmc0430_device*>(dev);
}


WRITE_LINE_MEMBER( ti99_cartridge_pcb::romgq_line )
{
	m_romspace_selected = (state==ASSERT_LINE);
}

// Propagate to all GROMs

/*
    Combined select lines
*/
WRITE8_MEMBER(ti99_cartridge_pcb::set_gromlines)
{
	for (auto& elem : m_grom)
	{
		if (elem != nullptr)
		{
			elem->set_lines(space, offset, data);
			if (data==ASSERT_LINE) m_grom_idle = false;
		}
	}
}

WRITE_LINE_MEMBER(ti99_cartridge_pcb::gclock_in)
{
	for (auto& elem : m_grom)
	{
		if (elem != nullptr)
		{
			elem->gclock_in(state);
			m_grom_idle = elem->idle();
		}
	}
}


/*****************************************************************************
  Cartridge type: Paged (12K, Extended Basic)

  The Extended Basic cartridge consists of several GROMs which are
  treated in the usual way, and two ROMs (4K and 8K).

  GROM space
  6000     77ff   8000     97ff   a000     b7ff   c000     d7ff   e000     f7ff
  |== GROM3 ==|...|== GROM4 ==|...|== GROM5 ==|...|== GROM6 ==|...|== GROM7 ==|

  ROM space
  6000          7000        7fff
  |             |              |
  |             |==== ROM2a ===|     Bank 0    write to 6000, 6004, ... 7ffc
  |=== ROM1 ====|              |
                |==== ROM2b ===|     Bank 1    write to 6002, 6006, ... 7ffe

  The 4K ROM is mapped into the 6000-6FFF space.
  The 8K ROM is actually composed of two banks of 4K which are mapped into
  the 7000-7FFF space. Bank 0 is visible after a write access to 6000 / 6004 /
  6008 ... , while bank 1 is visible after writing to 6002 / 6006 / 600A / ...

******************************************************************************/

READ8Z_MEMBER(ti99_paged12k_cartridge::readz)
{
	if (m_romspace_selected)
	{
		// rom_ptr: 0000-0fff = rom1
		//          2000-2fff = rom2a
		//          3000-3fff = rom2b
		if ((offset & 0x1000)==0)
			*value = m_rom_ptr[offset & 0x0fff];
		else
			*value = m_rom_ptr[(offset & 0x0fff) | 0x2000 | (m_rom_page << 12)];
	}
	else
	{
		// Will not return anything when not selected (preceding gsq=ASSERT)
		gromreadz(space, offset, value, mem_mask);
	}
}

WRITE8_MEMBER(ti99_paged12k_cartridge::write)
{
	if (m_romspace_selected)
	{
		m_rom_page = (offset >> 1) & 1;
		if (TRACE_BANKSWITCH) if ((offset & 1)==0) m_cart->logerror("Set ROM page = %d (writing to %04x)\n", m_rom_page, (offset | 0x6000));
	}
	else
	{
		// Will not change anything when not selected (preceding gsq=ASSERT)
		gromwrite(space, offset, data, mem_mask);
	}
}

/*****************************************************************************
  Cartridge type: Paged (16K)

  GROM space
  6000     77ff   8000     97ff   a000     b7ff   c000     d7ff   e000     f7ff
  |== GROM3 ==|...|== GROM4 ==|...|== GROM5 ==|...|== GROM6 ==|...|== GROM7 ==|

  ROM space
  6000         7000        7fff
  |             |             |
  |========== ROM1 ===========|     Bank 0    write to 6000, 6004, ... 7ffc
  |             |             |
  |========== ROM2 ===========|     Bank 1    write to 6002, 6006, ... 7ffe

  This cartridge consists of GROM memory and 2 pages of standard ROM.
    The page is set by writing any value to a location in
    the address area, where an even word offset sets the page to 0 and an
    odd word offset sets the page to 1 (e.g. 6000 = bank 0, and
    6002 = bank 1).
******************************************************************************/

READ8Z_MEMBER(ti99_paged16k_cartridge::readz)
{
	if (m_romspace_selected)
	{
		*value = m_rom_ptr[(offset & 0x1fff) | (m_rom_page << 13)];
	}
	else
	{
		// Will not return anything when not selected (preceding gsq=ASSERT)
		gromreadz(space, offset, value, mem_mask);
	}
}

WRITE8_MEMBER(ti99_paged16k_cartridge::write)
{
	if (m_romspace_selected)
	{
		m_rom_page = (offset >> 1) & 1;
		if (TRACE_BANKSWITCH) if ((offset & 1)==0) m_cart->logerror("Set ROM page = %d (writing to %04x)\n", m_rom_page, (offset | 0x6000));
	}
	else
	{
		// Will not change anything when not selected (preceding gsq=ASSERT)
		gromwrite(space, offset, data, mem_mask);
	}
}

/*****************************************************************************
  Cartridge type: Mini Memory
    GROM: 6 KiB (occupies G>6000 to G>77ff)
    ROM: 4 KiB (6000-6fff)
    RAM: 4 KiB (7000-7fff, battery-backed)

    GROM space
    6000     77ff
    |== GROM3 ==|

    ROM space
    6000         7000        7fff
    |             |             |
    |=== ROM1 ====|             |
                  |=== NVRAM ===|

******************************************************************************/

/* Read function for the minimem cartridge. */
READ8Z_MEMBER(ti99_minimem_cartridge::readz)
{
	if (m_romspace_selected)
	{
		if ((offset & 0x1000)==0x0000)
		{
			if (m_rom_ptr!=nullptr)    // Super-Minimem seems to have no ROM
			{
				*value = m_rom_ptr[offset & 0x0fff];
			}
		}
		else
		{
			*value = m_ram_ptr[offset & 0x0fff];
		}
	}
	else
	{
		gromreadz(space, offset, value, mem_mask);
	}
}

/* Write function for the minimem cartridge. */
WRITE8_MEMBER(ti99_minimem_cartridge::write)
{
	if (m_romspace_selected)
	{
		if ((offset & 0x1000)==0x0000)
		{
			if (TRACE_ILLWRITE) m_cart->logerror("Write access to cartridge ROM at address %04x ignored", offset);
		}
		else
		{
			m_ram_ptr[offset & 0x0fff] = data;
		}
	}
	else
	{
		gromwrite(space, offset, data, mem_mask);
	}
}

/*****************************************************************************
    Cartridge type: SuperSpace II

    SuperSpace is intended as a user-definable blank cartridge containing
    buffered RAM. It has an Editor/Assembler GROM which helps the user to load
    the user program into the cartridge. If the user program has a suitable
    header, the console recognizes the cartridge as runnable, and
    assigns a number in the selection screen. Switching the RAM banks in this
    cartridge is achieved by setting CRU bits (the system serial interface).

    GROM:    Editor/Assembler GROM
    ROM:     none
    RAM:     32 KiB (0x6000-0x7fff, 4 banks)
    Banking: via CRU write

    GROM space
    6000                       77ff
    |==== GROM3 (Editor/Assm) ====|

    ROM space
    6000         7000        7fff
    |             |             |
    |======== NVRAM 0 ==========|      Bank 0       CRU>0802
    |             |             |
    |======== NVRAM 1 ==========|      Bank 1       CRU>0806
    |             |             |
    |======== NVRAM 2 ==========|      Bank 2       CRU>080a
    |             |             |
    |======== NVRAM 3 ==========|      Bank 3       CRU>080e

******************************************************************************/

/* Read function for the super cartridge. */
READ8Z_MEMBER(ti99_super_cartridge::readz)
{
	if (m_romspace_selected)
	{
		if (m_ram_ptr != nullptr)
		{
			*value = m_ram_ptr[(m_ram_page << 13) | (offset & 0x1fff)];
		}
	}
	else
	{
		gromreadz(space, offset, value, mem_mask);
	}
}

/* Write function for the super cartridge. */
WRITE8_MEMBER(ti99_super_cartridge::write)
{
	if (m_romspace_selected)
	{
		m_ram_ptr[(m_ram_page << 13) | (offset & 0x1fff)] = data;
	}
	else
	{
		gromwrite(space, offset, data, mem_mask);
	}
}

READ8Z_MEMBER(ti99_super_cartridge::crureadz)
{
	// offset is the bit number. The CRU base address is already divided  by 2.

	// ram_page contains the bank number. We have a maximum of
	// 4 banks; the Super Space II manual says:
	//
	// Banks are selected by writing a bit pattern to CRU address >0800:
	//
	// Bank #   Value
	// 0        >02  = 0000 0010
	// 1        >08  = 0000 1000
	// 2        >20  = 0010 0000
	// 3        >80  = 1000 0000
	//
	// With the bank number (0, 1, 2, or 3) in R0:
	//
	// BNKSW   LI    R12,>0800   Set CRU address
	//         LI    R1,2        Load Shift Bit
	//         SLA   R0,1        Align Bank Number
	//         JEQ   BNKS1       Skip shift if Bank 0
	//         SLA   R1,0        Align Shift Bit
	// BNKS1   LDCR  R1,0        Switch Banks
	//         SRL   R0,1        Restore Bank Number (optional)
	//         RT

	// Our implementation in MESS always gets 8 bits in one go. Also, the address
	// is twice the bit number. That is, the offset value is always a multiple
	// of 0x10.

	if ((offset & 0xfff0) == 0x0800)
	{
		if (TRACE_CRU) m_cart->logerror("CRU accessed at %04x\n", offset);
		uint8_t val = 0x02 << (m_ram_page << 1);
		*value = (val >> ((offset - 0x0800)>>1)) & 0xff;
	}
}

WRITE8_MEMBER(ti99_super_cartridge::cruwrite)
{
	if ((offset & 0xfff0) == 0x0800)
	{
		if (TRACE_CRU) m_cart->logerror("CRU accessed at %04x\n", offset);
		if (data != 0)
		{
			m_ram_page = (offset-0x0802)>>2;
			if (TRACE_BANKSWITCH) if ((offset & 1)==0) m_cart->logerror("Set RAM page = %d (CRU address %04x)\n", m_ram_page, offset);
		}
	}
}

/*****************************************************************************
  Cartridge type: MBX
    GROM: up to 5 GROMs (sockets for a maximum of 3 GROMs, but may be stacked)
    ROM: up to 16 KiB (in up to 2 banks of 8KiB each)
    RAM: 1022 B (0x6c00-0x6ffd, overrides ROM in that area)
    ROM mapper: 6ffe

    TODO: Some MBX cartridges assume the presence of the MBX system
    (special user interface box with speech input/output)
    and will not run without it. This MBX hardware is not emulated yet.

    GROM space
    6000     77ff   8000     97ff   a000     b7ff   c000     d7ff   e000     f7ff
    |== GROM3 ==|...|== GROM4 ==|...|== GROM5 ==|...|== GROM6 ==|...|== GROM7 ==|

    ROM space
    6000             6c00    7000                 7fff
    |                 |       |                     |
    |                 |       |===== ROM bank 0 ====|       6ffe = 00
    |                 |= RAM =|                     |
    |=== ROM bank 0 ==|       |===== ROM bank 1 ====|       6ffe = 01
                              |                     |
                              |===== ROM bank 3 ====|       6ffe = 02
                              |                     |
                              |===== ROM bank 3 ====|       6ffe = 03

    The 16K ROM is composed of four 4K banks, which can be selected by writing
    the bank number to address 6ffe. This also affects the RAM so that the
    bank number is stored in RAM and may also be read from there.

    The mapper does not decode the LSB of the address, so it changes value when
    a write operation is done on 6FFF. Since the TI console always writes the
    odd byte first, then the even byte, the last byte written is actually 6FFE.

    ROM bank 0 (ROM area 0000-0fff) is always visible in the space 6000-6bff.

    RAM is implemented by two 1024x4 RAM circuits and is not affected by banking.

******************************************************************************/

/* Read function for the mbx cartridge. */
READ8Z_MEMBER(ti99_mbx_cartridge::readz)
{
	if (m_romspace_selected)
	{
		if (m_ram_ptr != nullptr && (offset & 0x1c00)==0x0c00)
		{
			// Also reads the value of 6ffe
			*value = m_ram_ptr[offset & 0x03ff];
			if (TRACE_READ) m_cart->logerror("%04x (RAM) -> %02x\n", offset + 0x6000, *value);
		}
		else
		{
			if (m_rom_ptr!=nullptr)
			{
				if ((offset & 0x1000)==0)  // 6000 area
					*value = m_rom_ptr[offset];
				else  // 7000 area
					*value = m_rom_ptr[(offset & 0x0fff) | (m_rom_page << 12)];

				if (TRACE_READ) m_cart->logerror("%04x(%04x) -> %02x\n", offset + 0x6000, offset | (m_rom_page<<13), *value);
			}
		}
	}
	else
	{
		gromreadz(space, offset, value, mem_mask);
	}
}

/* Write function for the mbx cartridge. */
WRITE8_MEMBER(ti99_mbx_cartridge::write)
{
	if (m_romspace_selected)
	{
		if ((offset & 0x1c00)==0x0c00)  // RAM area
		{
			if ((offset & 0x0ffe) == 0x0ffe)   // Mapper, backed by RAM; reacts to bots 6fff and 6ffe
			{
				// Valid values are 0, 1, 2, 3
				m_rom_page = data & 3;
				if (TRACE_BANKSWITCH) if ((offset & 1)==0) m_cart->logerror("Set ROM page = %d (writing to %04x)\n", m_rom_page, (offset | 0x6000));
			}

			if (m_ram_ptr != nullptr)
				m_ram_ptr[offset & 0x03ff] = data;
			else
				if (TRACE_ILLWRITE) m_cart->logerror("Write access to %04x but no RAM present\n", offset+0x6000);
		}
	}
	else
	{
		gromwrite(space, offset, data, mem_mask);
	}
}

/*****************************************************************************
  Cartridge type: paged379i
    This cartridge consists of one 16 KiB, 32 KiB, 64 KiB, or 128 KiB EEPROM
    which is organised in 2, 4, 8, or 16 pages of 8 KiB each. The complete
    memory contents must be stored in one dump file.
    The pages are selected by writing a value to some memory locations. Due to
    using the inverted outputs of the LS379 latch, setting the inputs of the
    latch to all 0 selects the highest bank, while setting to all 1 selects the
    lowest. There are some cartridges (16 KiB) which are using this scheme, and
    there are new hardware developments mainly relying on this scheme.

    Writing to       selects page (16K/32K/64K/128K)
    >6000            1 / 3 / 7 / 15
    >6002            0 / 2 / 6 / 14
    >6004            1 / 1 / 5 / 13
    >6006            0 / 0 / 4 / 12
    >6008            1 / 3 / 3 / 11
    >600A            0 / 2 / 2 / 10
    >600C            1 / 1 / 1 / 9
    >600E            0 / 0 / 0 / 8
    >6010            1 / 3 / 7 / 7
    >6012            0 / 2 / 6 / 6
    >6014            1 / 1 / 5 / 5
    >6016            0 / 0 / 4 / 4
    >6018            1 / 3 / 3 / 3
    >601A            0 / 2 / 2 / 2
    >601C            1 / 1 / 1 / 1
    >601E            0 / 0 / 0 / 0

    The paged379i cartrige does not have any GROMs.

    ROM space
    6000         7000        7fff
    |             |             |
    |========== ROM 1 ==========|      Bank 0      Write to 601e
    |             |             |
    |========== ROM 2 ==========|      Bank 1      Write to 601c
    |             |             |
    |            ...            |
    |             |             |
    |========== ROM 16 =========|      Bank 15     Write to 6000


******************************************************************************/

/* Read function for the paged379i cartridge. */
READ8Z_MEMBER(ti99_paged379i_cartridge::readz)
{
	if (m_romspace_selected)
		*value = m_rom_ptr[(m_rom_page<<13) | (offset & 0x1fff)];
}

/* Write function for the paged379i cartridge. Only used to set the bank. */
WRITE8_MEMBER(ti99_paged379i_cartridge::write)
{
	// Bits: 011x xxxx xxxb bbbx
	// x = don't care, bbbb = bank
	if (m_romspace_selected)
	{
		// This is emulation magic to automatically adapt to different ROM sizes

		// Each block has 8 KiB. We assume that m_rom_size is a power of 2.
		// Thus the number of blocks is also a power of 2.
		// To get the required number of address lines, we just have to subtract 1.
		// The SN74LS379 only has four flipflops, so we limit the lines to 4.
		int mask = ((m_rom_size / 8192) - 1) & 0x0f;

		// The page is determined by the inverted outputs.
		m_rom_page = (~offset)>>1 & mask;
		if (TRACE_BANKSWITCH) if ((offset & 1)==0) m_cart->logerror("Set ROM page = %d (writing to %04x)\n", m_rom_page, (offset | 0x6000));
	}
}

/*****************************************************************************
  Cartridge type: paged378
  This type is intended for high-capacity cartridges of up to 512 KiB
  plus GROM space of 120KiB (not supported yet)

  Due to its huge GROM space it is also called the "UberGROM"

  The cartridge may also be used without GROM.

    ROM space
    6000         7000        7fff
    |             |             |
    |========== ROM 1 ==========|      Bank 0      Write to 6000
    |             |             |
    |========== ROM 2 ==========|      Bank 1      Write to 6002
    |             |             |
    |            ...            |
    |             |             |
    |========== ROM 64 =========|      Bank 63     Write to 607e

******************************************************************************/

/* Read function for the paged378 cartridge. */
READ8Z_MEMBER(ti99_paged378_cartridge::readz)
{
	if (m_romspace_selected)
		*value = m_rom_ptr[(m_rom_page<<13) | (offset & 0x1fff)];
}

/* Write function for the paged378 cartridge. Only used to set the bank. */
WRITE8_MEMBER(ti99_paged378_cartridge::write)
{
	// Bits: 011x xxxx xbbb bbbx
	// x = don't care, bbbb = bank
	if (m_romspace_selected)
	{
		m_rom_page = ((offset >> 1)&0x003f);
		if (TRACE_BANKSWITCH) if ((offset & 1)==0) m_cart->logerror("Set ROM page = %d (writing to %04x)\n", m_rom_page, (offset | 0x6000));
	}
}

/*****************************************************************************
  Cartridge type: paged377
  This type is intended for high-capacity cartridges of up to 2 MiB

  The paged379i cartrige does not have any GROMs.

    ROM space
    6000         7000        7fff
    |             |             |
    |========== ROM 1 ==========|      Bank 0      Write to 6000
    |             |             |
    |========== ROM 2 ==========|      Bank 1      Write to 6002
    |             |             |
    |            ...            |
    |             |             |
    |========== ROM 256 ========|      Bank 255    Write to 61fe


******************************************************************************/

/* Read function for the paged377 cartridge. */
READ8Z_MEMBER(ti99_paged377_cartridge::readz)
{
	if (m_romspace_selected)
		*value = m_rom_ptr[(m_rom_page<<13) | (offset & 0x1fff)];
}

/* Write function for the paged377 cartridge. Only used to set the bank. */
WRITE8_MEMBER(ti99_paged377_cartridge::write)
{
	// Bits: 011x xxxb bbbb bbbx
	// x = don't care, bbbb = bank
	if (m_romspace_selected)
	{
		m_rom_page = ((offset >> 1)&0x00ff);
		if (TRACE_BANKSWITCH) if ((offset & 1)==0) m_cart->logerror("Set ROM page = %d (writing to %04x)\n", m_rom_page, (offset | 0x6000));
	}
}

/*****************************************************************************
  Cartridge type: pagedcru
    This cartridge consists of one 16 KiB, 32 KiB, or 64 KiB EEPROM which is
    organised in 2, 4, or 8 pages of 8 KiB each. We assume there is only one
    dump file of the respective size.
    The pages are selected by writing a value to the CRU. This scheme is
    similar to the one used for the SuperSpace cartridge, with the exception
    that we are using ROM only, and we can have up to 8 pages.

    Bank     Value written to CRU>0800
    0      >0002  = 0000 0000 0000 0010
    1      >0008  = 0000 0000 0000 1000
    2      >0020  = 0000 0000 0010 0000
    3      >0080  = 0000 0000 1000 0000
    4      >0200  = 0000 0010 0000 0000
    5      >0800  = 0000 1000 0000 0000
    6      >2000  = 0010 0000 0000 0000
    7      >8000  = 1000 0000 0000 0000

    No GROMs used in this type.

    ROM space
    6000         7000        7fff
    |             |             |
    |=========  ROM 1 ==========|      Bank 0       CRU>0802
    |             |             |
    |=========  ROM 2 ==========|      Bank 1       CRU>0806
    |             |             |
                 ...
    |             |             |
    |=========  ROM 8 ==========|      Bank 7       CRU>081e

******************************************************************************/

/* Read function for the pagedcru cartridge. */
READ8Z_MEMBER(ti99_pagedcru_cartridge::readz)
{
	if (m_romspace_selected)
		*value = m_rom_ptr[(m_rom_page<<13) | (offset & 0x1fff)];
}

/* Write function for the pagedcru cartridge. No effect. */
WRITE8_MEMBER(ti99_pagedcru_cartridge::write)
{
	return;
}

READ8Z_MEMBER(ti99_pagedcru_cartridge::crureadz)
{
	int page = m_rom_page;
	if ((offset & 0xf800)==0x0800)
	{
		int bit = (offset & 0x001e)>>1;
		if (bit != 0)
		{
			page = page-(bit/2);  // 4 page flags per 8 bits
		}
		*value = 1 << (page*2+1);
	}
}

WRITE8_MEMBER(ti99_pagedcru_cartridge::cruwrite)
{
	if ((offset & 0xf800)==0x0800)
	{
		int bit = (offset & 0x001e)>>1;
		if (data != 0 && bit > 0)
		{
			m_rom_page = (bit-1)/2;
			if (TRACE_BANKSWITCH) m_cart->logerror("Set ROM page = %d (CRU address %d)\n", m_rom_page, offset);
		}
	}
}

/*****************************************************************************
  Cartridge type: GROM emulation/paged

  This cartridge offers GROM address space without real GROM circuits. The GROMs
  are emulated by a normal EPROM with a circuit that mimics GROM behavior.
  Each simulated GROM offers 8K (real GROMs only offer 6K).

  Some assumptions:
  - No readable address counter. This means the parallel console GROMs
    will deliver the address when reading.
  - No wait states. Reading is generally faster than with real GROMs.
  - No wrapping at 8K boundaries.
  - Two pages of ROM at address 6000

  If any of these fails, the cartridge will crash, so we'll see.

  Typical cartridges: Third-party cartridges

  For the sake of simplicity, we register GROMs like the other PCB types, but
  we implement special access methods for the GROM space.

  Still not working:
     rxb1002 (Set page to 1 (6372 <- 00), lockup)
     rxb237 (immediate reset)
     rxbv555 (repeating reset on Master Title Screen)
     superxb (lockup, fix: add RAM at 7c00)

  Super-MiniMemory is also included here. We assume a RAM area at addresses
  7000-7fff for this cartridge.


  GROM space
  6000     77ff   8000     97ff   a000     b7ff   c000     d7ff   e000    ffff
  |=========================== emulated GROM ================================|

  ROM space
  6000         7000        7fff
  |             |             |
  |========== ROM1 ===========|     Bank 0    write to 6000, 6004, ... 7ffc
  |             |             |
  |========== ROM2 ===========|     Bank 1    write to 6002, 6006, ... 7ffe


******************************************************************************/

WRITE8_MEMBER(ti99_gromemu_cartridge::set_gromlines)
{
	if (m_grom_ptr != nullptr)
	{
		m_grom_selected = (data != CLEAR_LINE);
		m_grom_read_mode = ((offset & GROM_M_LINE)!=0);
		m_grom_address_mode = ((offset & GROM_MO_LINE)!=0);
	}
}

READ8Z_MEMBER(ti99_gromemu_cartridge::readz)
{
	if (m_grom_selected)
	{
		if (m_grom_read_mode) gromemureadz(space, offset, value, mem_mask);
	}
	else
	{
		if (m_ram_ptr != nullptr)
		{
			// Variant of the cartridge which emulates MiniMemory. We don't introduce
			// another type for this single cartridge.
			if ((offset & 0x1000)==0x1000)
			{
				*value = m_ram_ptr[offset & 0x0fff];
				return;
			}
		}

		if (m_rom_ptr == nullptr) return;
		*value = m_rom_ptr[(offset & 0x1fff) | (m_rom_page << 13)];
	}
}

WRITE8_MEMBER(ti99_gromemu_cartridge::write)
{
	if (m_romspace_selected)
	{
		if (m_ram_ptr != nullptr)
		{
			// Lines for Super-Minimem; see above
			if ((offset & 0x1000)==0x1000) {
				m_ram_ptr[offset & 0x0fff] = data;
			}
			return; // no paging
		}
		m_rom_page = (offset >> 1) & 1;
		if (TRACE_BANKSWITCH) if ((offset & 1)==0) m_cart->logerror("Set ROM page = %d (writing to %04x)\n", m_rom_page, (offset | 0x6000));
	}
	else
	{
		// Will not change anything when not selected (preceding gsq=ASSERT)
		if (m_grom_selected)
		{
			if (!m_grom_read_mode) gromemuwrite(space, offset, data, mem_mask);
		}
	}
}

READ8Z_MEMBER(ti99_gromemu_cartridge::gromemureadz)
{
	// Similar to the GKracker implemented above, we do not have a readable
	// GROM address counter but use the one from the console GROMs.
	if (m_grom_address_mode) return;

	int id = ((m_grom_address & 0xe000)>>13)&0x07;
	if (id > 2)
	{
		// Cartridge space (0x6000 - 0xffff)
		*value = m_grom_ptr[m_grom_address-0x6000]; // use the GROM memory
	}

	// The GROM emulation does not wrap at 8K boundaries.
	m_grom_address = (m_grom_address + 1) & 0xffff;

	// Reset the write address flipflop.
	m_waddr_LSB = false;
}

WRITE8_MEMBER(ti99_gromemu_cartridge::gromemuwrite)
{
	// Set GROM address
	if (m_grom_address_mode)
	{
		if (m_waddr_LSB == true)
		{
			// Accept low address byte (second write)
			m_grom_address = (m_grom_address << 8) | data;
			m_waddr_LSB = false;
		}
		else
		{
			// Accept high address byte (first write)
			m_grom_address = data;
			m_waddr_LSB = true;
		}
	}
	else
	{
		if (TRACE_ILLWRITE) m_cart->logerror("Ignoring write to GROM area at address %04x\n", m_grom_address);
	}
}

/****************************************************************************

    RPK loader

    RPK format support

    A RPK file ("rompack") contains a collection of dump files and a layout
    file that defines the kind of circuit board (PCB) used in the cartridge
    and the mapping of dumps to sockets on the board.

Example:
    <?xml version="1.0" encoding="utf-8"?>
    <romset>
        <resources>
            <rom id="gromimage" file="ed-assmg.bin" />
        </resources>
        <configuration>
            <pcb type="standard">
                <socket id="grom_socket" uses="gromimage"/>
            </pcb>
        </configuration>
    </romset>

DTD:
    <!ELEMENT romset (resources, configuration)>
    <!ELEMENT resources (rom|ram)+>
    <!ELEMENT rom EMPTY>
    <!ELEMENT ram EMPTY>
    <!ELEMENT configuration (pcb)>
    <!ELEMENT pcb (socket)+>
    <!ELEMENT socket EMPTY>
    <!ATTLIST romset version CDATA #IMPLIED>
    <!ATTLIST rom id ID #REQUIRED
    <!ATTLIST rom file CDATA #REQUIRED>
    <!ATTLIST rom crc CDATA #IMPLIED>
    <!ATTLIST rom sha1 CDATA #IMPLIED>
    <!ATTLIST ram id ID #REQUIRED>
    <!ATTLIST ram type (volatile|persistent) #IMPLIED>
    <!ATTLIST ram store (internal|external) #IMPLIED>
    <!ATTLIST ram file CDATA #IMPLIED>
    <!ATTLIST ram length CDATA #REQUIRED>
    <!ATTLIST pcb type CDATA #REQUIRED>
    <!ATTLIST socket id ID #REQUIRED>
    <!ATTLIST socket uses IDREF #REQUIRED>

****************************************************************************/

/****************************************
    RPK class
****************************************/
/*
    Constructor.
*/
ti99_cartridge_device::rpk::rpk(emu_options& options, const char* sysname)
	:m_options(options), m_type(0)
//,m_system_name(sysname)
{
	m_sockets.clear();
}

ti99_cartridge_device::rpk::~rpk()
{
	if (TRACE_RPK) printf("gromport/RPK: Destroy RPK\n");
}

/*
    Deliver the contents of the socket by name of the socket.
*/
uint8_t* ti99_cartridge_device::rpk::get_contents_of_socket(const char *socket_name)
{
	auto socket = m_sockets.find(socket_name);
	if (socket == m_sockets.end()) return nullptr;
	return socket->second->get_contents();
}

/*
    Deliver the length of the contents of the socket by name of the socket.
*/
int ti99_cartridge_device::rpk::get_resource_length(const char *socket_name)
{
	auto socket = m_sockets.find(socket_name);
	if (socket == m_sockets.end()) return 0;
	return socket->second->get_content_length();
}

void ti99_cartridge_device::rpk::add_socket(const char* id, std::unique_ptr<rpk_socket> newsock)
{
	m_sockets.emplace(id, std::move(newsock));
}

/*-------------------------------------------------
    rpk_close - closes a rpk
    Saves the contents of the NVRAMs and frees all memory.
-------------------------------------------------*/

void ti99_cartridge_device::rpk::close()
{
	// Save the NVRAM contents
	for(auto &socket : m_sockets)
	{
		if (socket.second->persistent_ram())
		{
			// try to open the battery file and write it if possible
			assert_always(socket.second->get_contents() && (socket.second->get_content_length() > 0), "Buffer is null or length is 0");

			emu_file file(m_options.nvram_directory(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);
			osd_file::error filerr = file.open(socket.second->get_pathname());
			if (filerr == osd_file::error::NONE)
				file.write(socket.second->get_contents(), socket.second->get_content_length());

		}
		socket.second->cleanup();
	}
}

/**************************************************************
    RPK socket (location in the PCB where a chip is plugged in;
    not a network socket)
***************************************************************/

ti99_cartridge_device::rpk_socket::rpk_socket(const char* id, int length, uint8_t* contents, const char *pathname)
	: m_id(id), m_length(length), m_contents(contents), m_pathname(pathname)
{
}

ti99_cartridge_device::rpk_socket::rpk_socket(const char* id, int length, uint8_t* contents)
	: rpk_socket(id, length, contents, nullptr)
{
}

/*
    Locate a file in the ZIP container
*/
int ti99_cartridge_device::rpk_reader::find_file(util::archive_file &zip, const char *filename, uint32_t crc)
{
	for (int header = zip.first_file(); header >= 0; header = zip.next_file())
	{
		// Ignore directories
		if (!zip.current_is_directory())
		{
			// We don't check for CRC == 0.
			if (crc != 0)
			{
				// if the CRC and name both match, we're good
				// if the CRC matches and the name doesn't, we're still good
				if (zip.current_crc() == crc)
					return header;
			}
			else
			{
				if (core_stricmp(zip.current_name().c_str(), filename) == 0)
				{
					return header;
				}
			}
		}
	}
	return -1;
}

/*
    Load a rom resource and put it in a pcb socket instance.
*/
std::unique_ptr<ti99_cartridge_device::rpk_socket> ti99_cartridge_device::rpk_reader::load_rom_resource(util::archive_file &zip, util::xml::data_node const* rom_resource_node, const char* socketname)
{
	const char* file;
	const char* crcstr;
	const char* sha1;
	util::archive_file::error ziperr;
	uint32_t crc;
	int length;
	uint8_t* contents;
	int header;

	// find the file attribute (required)
	file = rom_resource_node->get_attribute_string("file", nullptr);
	if (file == nullptr) throw rpk_exception(RPK_INVALID_LAYOUT, "<rom> must have a 'file' attribute");

	if (TRACE_RPK) printf("gromport/RPK: Loading ROM contents for socket '%s' from file %s\n", socketname, file);

	// check for crc
	crcstr = rom_resource_node->get_attribute_string("crc", nullptr);
	if (crcstr==nullptr)
	{
		// no CRC, just find the file in the RPK
		header = find_file(zip, file, 0);
	}
	else
	{
		crc = strtoul(crcstr, nullptr, 16);
		header = find_file(zip, file, crc);
	}
	if (header < 0) throw rpk_exception(RPK_INVALID_FILE_REF, "File not found or CRC check failed");

	length = zip.current_uncompressed_length();

	// Allocate storage
	contents = global_alloc_array_clear<uint8_t>(length);
	if (contents==nullptr) throw rpk_exception(RPK_OUT_OF_MEMORY);

	// and unzip file from the zip file
	ziperr = zip.decompress(contents, length);
	if (ziperr != util::archive_file::error::NONE)
	{
		if (ziperr == util::archive_file::error::UNSUPPORTED) throw rpk_exception(RPK_ZIP_UNSUPPORTED);
		else throw rpk_exception(RPK_ZIP_ERROR);
	}

	// check for sha1
	sha1 = rom_resource_node->get_attribute_string("sha1", nullptr);
	if (sha1 != nullptr)
	{
		util::hash_collection actual_hashes;
		actual_hashes.compute((const uint8_t *)contents, length, util::hash_collection::HASH_TYPES_CRC_SHA1);

		util::hash_collection expected_hashes;
		expected_hashes.add_from_string(util::hash_collection::HASH_SHA1, sha1, strlen(sha1));

		if (actual_hashes != expected_hashes) throw rpk_exception(RPK_INVALID_FILE_REF, "SHA1 check failed");
	}

	// Create a socket instance
	return std::make_unique<rpk_socket>(socketname, length, contents);
}

/*
    Load a ram resource and put it in a pcb socket instance.
*/
std::unique_ptr<ti99_cartridge_device::rpk_socket> ti99_cartridge_device::rpk_reader::load_ram_resource(emu_options &options, util::xml::data_node const* ram_resource_node, const char* socketname, const char* system_name)
{
	const char* length_string;
	const char* ram_type;
	const char* ram_filename;
	const char* ram_pname;
	unsigned int length;
	uint8_t* contents;

	// find the length attribute
	length_string = ram_resource_node->get_attribute_string("length", nullptr);
	if (length_string == nullptr) throw rpk_exception(RPK_MISSING_RAM_LENGTH);

	// parse it
	char suffix = '\0';
	sscanf(length_string, "%u%c", &length, &suffix);
	switch(tolower(suffix))
	{
		case 'k': // kilobytes
			length *= 1024;
			break;

		case 'm':
			/* megabytes */
			length *= 1024*1024;
			break;

		case '\0':
			break;

		default:  // failed
			throw rpk_exception(RPK_INVALID_RAM_SPEC);
	}

	// Allocate memory for this resource
	contents = global_alloc_array_clear<uint8_t>(length);
	if (contents==nullptr) throw rpk_exception(RPK_OUT_OF_MEMORY);

	if (TRACE_RPK) printf("gromport/RPK: Allocating RAM buffer (%d bytes) for socket '%s'\n", length, socketname);

	ram_pname = nullptr;

	// That's it for pure RAM. Now check whether the RAM is "persistent", i.e. NVRAM.
	// In that case we must load it from the NVRAM directory.
	// The file name is given in the RPK file; the subdirectory is the system name.
	ram_type = ram_resource_node->get_attribute_string("type", nullptr);
	if (ram_type != nullptr)
	{
		if (strcmp(ram_type, "persistent")==0)
		{
			// Get the file name (required if persistent)
			ram_filename = ram_resource_node->get_attribute_string("file", nullptr);
			if (ram_filename==nullptr)
			{
				global_free_array(contents);
				throw rpk_exception(RPK_INVALID_RAM_SPEC, "<ram type='persistent'> must have a 'file' attribute");
			}
			std::string ram_pathname = std::string(system_name).append(PATH_SEPARATOR).append(ram_filename);
			ram_pname = core_strdup(ram_pathname.c_str());
			// load, and fill rest with 00
			if (TRACE_RPK) printf("gromport/RPK: Loading NVRAM contents from '%s'\n", ram_pname);

			// Load the NVRAM contents
			int bytes_read = 0;
			assert_always(contents && (length > 0), "Buffer is null or length is 0");

			// try to open the battery file and read it if possible
			emu_file file(options.nvram_directory(), OPEN_FLAG_READ);
			osd_file::error filerr = file.open(ram_pname);
			if (filerr == osd_file::error::NONE)
				bytes_read = file.read(contents, length);

			// fill remaining bytes (if necessary)
			memset(((char *) contents) + bytes_read, 0x00, length - bytes_read);
		}
	}

	// Create a socket instance
	return std::make_unique<rpk_socket>(socketname, length, contents, ram_pname);
}

/*-------------------------------------------------
    rpk_open - open a RPK file
    options - parameters from the settings; we need it only for the NVRAM directory
    system_name - name of the driver (also just for NVRAM handling)
-------------------------------------------------*/

ti99_cartridge_device::rpk* ti99_cartridge_device::rpk_reader::open(emu_options &options, const char *filename, const char *system_name)
{
	util::archive_file::error ziperr;

	util::archive_file::ptr zipfile;

	std::vector<char> layout_text;
	util::xml::data_node *layout_xml = nullptr;

	int i;

	auto newrpk = new rpk(options, system_name);

	try
	{
		/* open the ZIP file */
		ziperr = util::archive_file::open_zip(filename, zipfile);
		if (ziperr != util::archive_file::error::NONE) throw rpk_exception(RPK_NOT_ZIP_FORMAT);

		/* find the layout.xml file */
		if (find_file(*zipfile, "layout.xml", 0) < 0) throw rpk_exception(RPK_MISSING_LAYOUT);

		/* reserve space for the layout file contents (+1 for the termination) */
		layout_text.resize(zipfile->current_uncompressed_length() + 1);

		/* uncompress the layout text */
		ziperr = zipfile->decompress(&layout_text[0], zipfile->current_uncompressed_length());
		if (ziperr != util::archive_file::error::NONE)
		{
			if (ziperr == util::archive_file::error::UNSUPPORTED) throw rpk_exception(RPK_ZIP_UNSUPPORTED);
			else throw rpk_exception(RPK_ZIP_ERROR);
		}

		layout_text[zipfile->current_uncompressed_length()] = '\0';  // Null-terminate

		/* parse the layout text */
		layout_xml = util::xml::data_node::string_read(&layout_text[0], nullptr);
		if (!layout_xml) throw rpk_exception(RPK_XML_ERROR);

		// Now we work within the XML tree

		// romset is the root node
		util::xml::data_node const *const romset_node = layout_xml->get_child("romset");
		if (!romset_node) throw rpk_exception(RPK_INVALID_LAYOUT, "document element must be <romset>");

		// resources is a child of romset
		util::xml::data_node const *const resources_node = romset_node->get_child("resources");
		if (!resources_node) throw rpk_exception(RPK_INVALID_LAYOUT, "<romset> must have a <resources> child");

		// configuration is a child of romset; we're actually interested in ...
		util::xml::data_node const *const configuration_node = romset_node->get_child("configuration");
		if (!configuration_node) throw rpk_exception(RPK_INVALID_LAYOUT, "<romset> must have a <configuration> child");

		// ... pcb, which is a child of configuration
		util::xml::data_node const *const pcb_node = configuration_node->get_child("pcb");
		if (!pcb_node) throw rpk_exception(RPK_INVALID_LAYOUT, "<configuration> must have a <pcb> child");

		// We'll try to find the PCB type on the provided type list.
		char const *const pcb_type = pcb_node->get_attribute_string("type", nullptr);
		if (!pcb_type) throw rpk_exception(RPK_INVALID_LAYOUT, "<pcb> must have a 'type' attribute");
		if (TRACE_RPK) printf("gromport/RPK: Cartridge says it has PCB type '%s'\n", pcb_type);

		i=0;
		do
		{
			if (strcmp(pcb_type, m_types[i].name)==0)
			{
				newrpk->m_type = m_types[i].id;
				break;
			}
			i++;
		} while (m_types[i].id != 0);

		if (m_types[i].id==0) throw rpk_exception(RPK_UNKNOWN_PCB_TYPE);

		// Find the sockets and load their respective resource
		for (util::xml::data_node const *socket_node = pcb_node->get_first_child();  socket_node != nullptr; socket_node = socket_node->get_next_sibling())
		{
			if (strcmp(socket_node->get_name(), "socket")!=0) throw rpk_exception(RPK_INVALID_LAYOUT, "<pcb> element has only <socket> children");
			char const *const id = socket_node->get_attribute_string("id", nullptr);
			if (!id) throw rpk_exception(RPK_INVALID_LAYOUT, "<socket> must have an 'id' attribute");
			char const *const uses_name = socket_node->get_attribute_string("uses", nullptr);
			if (!uses_name) throw rpk_exception(RPK_INVALID_LAYOUT, "<socket> must have a 'uses' attribute");

			bool found = false;
			// Locate the resource node
			for (util::xml::data_node const *resource_node = resources_node->get_first_child(); resource_node != nullptr; resource_node = resource_node->get_next_sibling())
			{
				char const *const resource_name = resource_node->get_attribute_string("id", nullptr);
				if (!resource_name) throw rpk_exception(RPK_INVALID_LAYOUT, "resource node must have an 'id' attribute");

				if (strcmp(resource_name, uses_name)==0)
				{
					// found it
					if (strcmp(resource_node->get_name(), "rom")==0)
					{
						newrpk->add_socket(id, load_rom_resource(*zipfile, resource_node, id));
					}
					else
					{
						if (strcmp(resource_node->get_name(), "ram")==0)
						{
							newrpk->add_socket(id, load_ram_resource(options, resource_node, id, system_name));
						}
						else throw rpk_exception(RPK_INVALID_LAYOUT, "resource node must be <rom> or <ram>");
					}
					found = true;
				}
			}
			if (!found) throw rpk_exception(RPK_INVALID_RESOURCE_REF, uses_name);
		}
	}
	catch (rpk_exception &)
	{
		newrpk->close();
		if (layout_xml) layout_xml->file_free();

		// rethrow the exception
		throw;
	}

	if (layout_xml) layout_xml->file_free();

	return newrpk;
}

} } } // end namespace bus::ti99::internal

SLOT_INTERFACE_START( gromport4 )
	SLOT_INTERFACE("single",   TI99_GROMPORT_SINGLE)
	SLOT_INTERFACE("multi",    TI99_GROMPORT_MULTI)
	SLOT_INTERFACE("gkracker", TI99_GROMPORT_GK)
SLOT_INTERFACE_END

SLOT_INTERFACE_START( gromport8 )
	SLOT_INTERFACE("single", TI99_GROMPORT_SINGLE)
	SLOT_INTERFACE("multi",  TI99_GROMPORT_MULTI)
SLOT_INTERFACE_END
