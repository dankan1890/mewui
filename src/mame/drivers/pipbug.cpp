// license:BSD-3-Clause
// copyright-holders:Robbbert
/***************************************************************************

PIPBUG

2010-04-08 Skeleton driver.
2012-05-16 Connected to serial terminal.. working
2019-07-17 Added cassette onto L command

All input must be in UPPER case.

Commands:
A - See and alter memory
B - Set breakpoint (2 permitted)
C - Clear breakpoint
D - Dump memory to paper tape
G - Go to address, run
L - Load memory from paper tape
S - See and alter registers

PIPBUG isn't a computer; it is a the name of the bios used
in a number of small 2650-based computers from 1976 to 1978.
Examples include Baby 2650, Eurocard 2650, etc., plus Signetics
own PC1001, PC1500, and KT9500 systems. PIPBUG was written by Signetics.

The sole means of communication is via a serial terminal.
PIPBUG uses the SENSE and FLAG pins as serial lines, thus
there is no need for a UART. The baud rate is 110.

The Baby 2650 (featured in Electronics Australia magazine in
March 1977) has 256 bytes of RAM.

The terminal is expected to have a papertape device attached, and
use it to save and load programs. PIPBUG still thinks it is talking
to the terminal, when in fact the data is flowing to the papertape
reader and punch.

Cassette:
There is software available at 110 baud, using 1200/2400 Hz. This has
been hooked up to the otherwise-useless L command. Baud rate = 110.
After load completes, G440 to run.

****************************************************************************/

#include "emu.h"
#include "bus/rs232/rs232.h"
#include "cpu/s2650/s2650.h"
#include "imagedev/cassette.h"
#include "machine/terminal.h"
#include "imagedev/snapquik.h"
#include "machine/timer.h"
#include "speaker.h"


class pipbug_state : public driver_device
{
public:
	pipbug_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_rs232(*this, "rs232")
		, m_maincpu(*this, "maincpu")
		, m_cass(*this, "cassette")
	{ }

	void pipbug(machine_config &config);

private:
	DECLARE_WRITE8_MEMBER(pipbug_ctrl_w);
	DECLARE_READ_LINE_MEMBER(serial_r);
	TIMER_DEVICE_CALLBACK_MEMBER(kansas_r);
	required_device<rs232_port_device> m_rs232;
	required_device<s2650_device> m_maincpu;
	required_device<cassette_image_device> m_cass;
	DECLARE_QUICKLOAD_LOAD_MEMBER(quickload_cb);
	void pipbug_data(address_map &map);
	void pipbug_mem(address_map &map);
	u8 m_cass_data[4];
	bool m_cassold, m_cassinbit;
};

WRITE8_MEMBER( pipbug_state::pipbug_ctrl_w )
{
// 0x80 is written here - not connected in the baby 2650
}

TIMER_DEVICE_CALLBACK_MEMBER( pipbug_state::kansas_r )
{
	// no tape - set to idle
	m_cass_data[1]++;
	if (m_cass_data[1] > 32)
	{
		m_cass_data[1] = 32;
		m_cassinbit = 1;
	}

	if ((m_cass->get_state() & CASSETTE_MASK_UISTATE) != CASSETTE_PLAY)
		return;

	/* cassette - turn 1200/2400Hz to a bit */
	bool cass_ws = (m_cass->input() > +0.04) ? 1 : 0;

	if (cass_ws != m_cassold)
	{
		m_cassold = cass_ws;
		m_cassinbit = (m_cass_data[1] < 12) ? 1 : 0;
		m_cass_data[1] = 0;
	}
}

READ_LINE_MEMBER( pipbug_state::serial_r )
{
	return m_rs232->rxd_r() & m_cassinbit;
}

void pipbug_state::pipbug_mem(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x03ff).rom();
	map(0x0400, 0x7fff).ram();
}

void pipbug_state::pipbug_data(address_map &map)
{
//  ADDRESS_MAP_UNMAP_HIGH
	map(S2650_CTRL_PORT, S2650_CTRL_PORT).w(FUNC(pipbug_state::pipbug_ctrl_w));
}

/* Input ports */
static INPUT_PORTS_START( pipbug )
INPUT_PORTS_END

static DEVICE_INPUT_DEFAULTS_START( terminal )
	DEVICE_INPUT_DEFAULTS( "RS232_TXBAUD", 0xff, RS232_BAUD_110 )
	DEVICE_INPUT_DEFAULTS( "RS232_RXBAUD", 0xff, RS232_BAUD_110 )
	DEVICE_INPUT_DEFAULTS( "RS232_STARTBITS", 0xff, RS232_STARTBITS_1 )
	DEVICE_INPUT_DEFAULTS( "RS232_DATABITS", 0xff, RS232_DATABITS_7 )
	DEVICE_INPUT_DEFAULTS( "RS232_PARITY", 0xff, RS232_PARITY_EVEN )
	DEVICE_INPUT_DEFAULTS( "RS232_STOPBITS", 0xff, RS232_STOPBITS_1 )
DEVICE_INPUT_DEFAULTS_END

QUICKLOAD_LOAD_MEMBER(pipbug_state::quickload_cb)
{
	address_space &space = m_maincpu->space(AS_PROGRAM);
	int i;
	int quick_addr = 0x440;
	int exec_addr;
	int quick_length;
	std::vector<uint8_t> quick_data;
	int read_;
	image_init_result result = image_init_result::FAIL;

	quick_length = image.length();
	if (quick_length < 0x0444)
	{
		image.seterror(IMAGE_ERROR_INVALIDIMAGE, "File too short");
		image.message(" File too short");
	}
	else if (quick_length > 0x8000)
	{
		image.seterror(IMAGE_ERROR_INVALIDIMAGE, "File too long");
		image.message(" File too long");
	}
	else
	{
		quick_data.resize(quick_length);
		read_ = image.fread( &quick_data[0], quick_length);
		if (read_ != quick_length)
		{
			image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Cannot read the file");
			image.message(" Cannot read the file");
		}
		else if (quick_data[0] != 0xc4)
		{
			image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Invalid header");
			image.message(" Invalid header");
		}
		else
		{
			exec_addr = quick_data[1] * 256 + quick_data[2];

			if (exec_addr >= quick_length)
			{
				image.seterror(IMAGE_ERROR_INVALIDIMAGE, "Exec address beyond end of file");
				image.message(" Exec address beyond end of file");
			}
			else
			{
				for (i = quick_addr; i < read_; i++)
					space.write_byte(i, quick_data[i]);

				/* display a message about the loaded quickload */
				image.message(" Quickload: size=%04X : exec=%04X",quick_length,exec_addr);

				// Start the quickload
				m_maincpu->set_state_int(S2650_PC, exec_addr);

				result = image_init_result::PASS;
			}
		}
	}

	return result;
}

void pipbug_state::pipbug(machine_config &config)
{
	/* basic machine hardware */
	S2650(config, m_maincpu, XTAL(1'000'000));
	m_maincpu->set_addrmap(AS_PROGRAM, &pipbug_state::pipbug_mem);
	m_maincpu->set_addrmap(AS_DATA, &pipbug_state::pipbug_data);
	m_maincpu->flag_handler().set("rs232", FUNC(rs232_port_device::write_txd));
	m_maincpu->sense_handler().set(FUNC(pipbug_state::serial_r));

	/* video hardware */
	RS232_PORT(config, m_rs232, default_rs232_devices, "terminal");
	m_rs232->set_option_device_input_defaults("terminal", DEVICE_INPUT_DEFAULTS_NAME(terminal));

	/* quickload */
	QUICKLOAD(config, "quickload", "pgm", attotime::from_seconds(1)).set_load_callback(FUNC(pipbug_state::quickload_cb), this);

	SPEAKER(config, "mono").front_center();

	/* Cassette */
	CASSETTE(config, m_cass);
	m_cass->set_default_state(CASSETTE_STOPPED | CASSETTE_MOTOR_ENABLED | CASSETTE_SPEAKER_ENABLED);
	m_cass->add_route(ALL_OUTPUTS, "mono", 0.05);
	TIMER(config, "kansas_r").configure_periodic(FUNC(pipbug_state::kansas_r), attotime::from_hz(40000));
}


/* ROM definition */
ROM_START( pipbug )
	ROM_REGION( 0x8000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "pipbug.rom", 0x0000, 0x0400, CRC(f242b93e) SHA1(f82857cc882e6b5fc9f00b20b375988024f413ff))
ROM_END

/* Driver */

//    YEAR  NAME    PARENT  COMPAT  MACHINE  INPUT   CLASS         INIT        COMPANY      FULLNAME  FLAGS
COMP( 1979, pipbug, 0,      0,      pipbug,  pipbug, pipbug_state, empty_init, "Signetics", "PIPBUG", MACHINE_NO_SOUND_HW )
