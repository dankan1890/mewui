// license:LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    Common definitions for TI family
    Should be included in each component.

    Michael Zapf

*****************************************************************************/

#ifndef __TI99DEFS__
#define __TI99DEFS__

// TI-99/4(A)
#define TISOUNDCHIP_TAG "soundchip"
#define GROMPORT_TAG    "gromport"
#define PERIBOX_TAG     "peb"
#define VDP_TAG         "vdp"
#define TMS9901_TAG     "tms9901"
#define SCREEN_TAG      "screen"
#define GROM0_TAG       "console_grom_0"
#define GROM1_TAG       "console_grom_1"
#define GROM2_TAG       "console_grom_2"
#define JOYPORT_TAG     "joyport"
#define EVPC_CONN_TAG   "evpc_conn"
#define DATAMUX_TAG     "datamux_16_8"
#define PADRAM_TAG       "scratchpad"
#define EXPRAM_TAG       "internal_32k_mod"
#define DSRROM          "dsrrom"
#define CONSOLEROM      "console_rom"
#define CONSOLEGROM     "cons_grom"

#define VDPFREQ XTAL_10_738635MHz
#define GROMFREQ VDPFREQ/24

// TI-99/8
#define SRAM_TAG        "sram8"
#define DRAM_TAG        "dram8"
#define MAPPER_TAG      "mapper"
#define MAINBOARD8_TAG  "mainboard8"
#define SPEECHSYN_TAG     "speech"

#define ROM0_REG        "rom0_region"
#define ROM1_REG        "rom1_region"
#define PASCAL_REG      "pascal_region"
#define SYSGROM_REG     "sysgrom_region"
#define GROMLIB1_REG    "gromlib1_region"
#define GROMLIB2_REG    "gromlib2_region"
#define GROMLIB3_REG    "gromlib3_region"
#define SPEECHROM_REG       "speech_region"

#define GROMLIB_TAG "gromlib"
#define SYSGROM_TAG GROMLIB_TAG "0"
#define SYSGROM0_TAG SYSGROM_TAG "_0"
#define SYSGROM1_TAG SYSGROM_TAG "_1"
#define SYSGROM2_TAG SYSGROM_TAG "_2"

#define GLIB1_TAG GROMLIB_TAG "1"
#define GLIB10_TAG GLIB1_TAG "_0"
#define GLIB11_TAG GLIB1_TAG "_1"
#define GLIB12_TAG GLIB1_TAG "_2"
#define GLIB13_TAG GLIB1_TAG "_3"
#define GLIB14_TAG GLIB1_TAG "_4"
#define GLIB15_TAG GLIB1_TAG "_5"
#define GLIB16_TAG GLIB1_TAG "_6"
#define GLIB17_TAG GLIB1_TAG "_7"

#define GLIB2_TAG GROMLIB_TAG "2"
#define GLIB20_TAG GLIB2_TAG "_0"
#define GLIB21_TAG GLIB2_TAG "_1"
#define GLIB22_TAG GLIB2_TAG "_2"
#define GLIB23_TAG GLIB2_TAG "_3"
#define GLIB24_TAG GLIB2_TAG "_4"
#define GLIB25_TAG GLIB2_TAG "_5"
#define GLIB26_TAG GLIB2_TAG "_6"
#define GLIB27_TAG GLIB2_TAG "_7"

#define GLIB3_TAG GROMLIB_TAG "3"
#define GLIB30_TAG GLIB3_TAG "_0"
#define GLIB31_TAG GLIB3_TAG "_1"
#define GLIB32_TAG GLIB3_TAG "_2"



// Geneve
#define GKEYBOARD_TAG   "gkeyboard"
#define GMAPPER_TAG     "gmapper"
#define GMOUSE_TAG      "gmouse"
#define GCLOCK_TAG      "mm58274c"
#define PFM512_TAG      "pfm512"
#define PFM512A_TAG     "pfm512a"

#define READ16Z_MEMBER(name)            void name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset,  ATTR_UNUSED uint16_t *value, ATTR_UNUSED uint16_t mem_mask)
#define DECLARE_READ16Z_MEMBER(name)    void name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED uint16_t *value, ATTR_UNUSED uint16_t mem_mask = 0xffff)
#define READ8Z_MEMBER(name)             void name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED uint8_t *value, ATTR_UNUSED uint8_t mem_mask)
#define DECLARE_READ8Z_MEMBER(name)     void name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED uint8_t *value, ATTR_UNUSED uint8_t mem_mask = 0xff)

/*
    For almost all applications of setoffset, we also need the data bus
    direction. This line is called DBIN on the TI CPUs, but as we do not assume
    that this is a general rule, we use new macros here which contain the
    DBIN setting.
*/
#define SETADDRESS_DBIN_MEMBER(name)          void  name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED int state)
#define DECLARE_SETADDRESS_DBIN_MEMBER(name)  void  name(ATTR_UNUSED address_space &space, ATTR_UNUSED offs_t offset, ATTR_UNUSED int state)

#define GENMOD 0x01

/*
    Base class for all devices that have a special read method. This read
    methods differs from the standard READ8/16 by allowing for a "high-impedance"
    state "Z" which means that the bus lines are not changed. That way, we can
    built busses with parallel devices as in the real machine, and only the
    active device changes the bus lines.
*/
class bus8z_device : public device_t
{
public:
	bus8z_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source)
	: device_t(mconfig, type, name, tag, owner, clock, shortname, source) { }
	virtual DECLARE_READ8Z_MEMBER(readz) =0;
	virtual DECLARE_WRITE8_MEMBER(write) =0;
	virtual DECLARE_SETADDRESS_DBIN_MEMBER( setaddress_dbin ) { };
};

class bus16z_device : device_t
{
public:
	bus16z_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, uint32_t clock, const char *shortname, const char *source)
		: device_t(mconfig, type, name, tag, owner, clock, shortname, source) { }
	virtual DECLARE_READ16Z_MEMBER(read16z) =0;
	virtual DECLARE_WRITE16_MEMBER(write16) =0;
	virtual DECLARE_SETADDRESS_DBIN_MEMBER( setaddress_dbin ) { };
};

/****************************************************************************
    Connector from EVPC
    We need this for the TI-99/4A console as well as for the SGCPU, so we have
    to use callbacks

    This is actually a separate cable lead going from
    the EPVC in the PEB to a pin inside the console. This cable sends the
    video interrupt from the v9938 on the EVPC into the console.
    This workaround must be done on the real system because the peripheral
    box and its connector were not designed to deliver a video interrupt signal.
    This was fixed with the EVPC2 which uses the external interrupt EXTINT
    with a special firmware (DSR).

    Emulation detail: We are using a separate device class in order to avoid
    exposing the console class to the external class.

****************************************************************************/
class ti99_4x_state;

class evpc_clock_connector;

const device_type EVPC_CONN = &device_creator<evpc_clock_connector>;

class evpc_clock_connector : public device_t
{
public:
	evpc_clock_connector(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, EVPC_CONN, "EVPC clock connector", tag, owner, clock, "ti99_evpc_clock", __FILE__),
		m_vdpint(*this) { };

	template<class _Object> static devcb_base &static_set_vdpint_callback(device_t &device, _Object object)
	{
		return downcast<evpc_clock_connector &>(device).m_vdpint.set_callback(object);
	}

	WRITE_LINE_MEMBER( vclock_line ) { m_vdpint(state); }
	void device_start() override { m_vdpint.resolve();  }

private:
	// VDPINT line to the CPU
	devcb_write_line m_vdpint;
};


#define MCFG_ADD_EVPC_CONNECTOR( _tag, _vdpint ) \
	MCFG_DEVICE_ADD(_tag, EVPC_CONN, 0) \
	devcb = &evpc_clock_connector::static_set_vdpint_callback( *device, DEVCB_##_vdpint );

/****************************************************************************
    Constants
****************************************************************************/

enum
{
	GM_TURBO = 1,
	GM_TIM = 2
};

enum
{
	GENEVE_098 = 0,
	GENEVE_100,
	GENEVE_PFM512,
	GENEVE_PFM512A
};

#endif
