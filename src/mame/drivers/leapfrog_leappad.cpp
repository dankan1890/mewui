// license:BSD-3-Clause
// copyright-holders:David Haywood
/******************************************************************************

    LEAPPAD:
    Example-Video: https://www.youtube.com/watch?v=LtUhENu5TKc
    The LEAPPAD is basically compareable to the SEGA PICO, but without
    Screen-Output! Each "Game" consists of two parts (Book + Cartridge).
    Insert the cartridge into the system and add the Book on the Top of the
    "console" and you can click on each pages and hear sounds or
    learning-stuff on each page...

    MY FIRST LEAPPAD:
    Basically the same as the LEAPPAD, but for even younger kids! (Cartridge
    internal PCB's are identical to LEAPPAD).
    Example Video: https://www.youtube.com/watch?v=gsf8XYV1Tpg

    LITTLE TOUCH LEAPPAD:
    Same as the other LEAPPAD models, but aimed at babies.

    Don't get confused by the name "LEAPPAD", as it looks like Leapfrog
    also released some kind of Tablet with this name, and they even released
    a new "LEAPPAD" in around 2016:
    https://www.youtube.com/watch?v=MXFSgj6xLTU , which nearly looks like the
    same, but is most likely techically completely different..

*******************************************************************************/

#include "emu.h"

#include "bus/generic/slot.h"
#include "bus/generic/carts.h"

#include "screen.h"
#include "softlist.h"
#include "speaker.h"

class leapfrog_leappad_state : public driver_device
{
public:
	leapfrog_leappad_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_cart(*this, "cartslot")
		, m_cart_region(nullptr)
	{ }

	void leapfrog_leappad(machine_config &config);
	void leapfrog_mfleappad(machine_config &config);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(cart_load);

	required_device<generic_slot_device> m_cart;
	memory_region *m_cart_region;
};



void leapfrog_leappad_state::machine_start()
{
	// if there's a cart, override the standard mapping
	if (m_cart && m_cart->exists())
	{
		std::string region_tag;
		m_cart_region = memregion(region_tag.assign(m_cart->tag()).append(GENERIC_ROM_REGION_TAG).c_str());
	}
}

void leapfrog_leappad_state::machine_reset()
{
}

DEVICE_IMAGE_LOAD_MEMBER(leapfrog_leappad_state::cart_load)
{
	uint32_t size = m_cart->common_get_size("rom");

	m_cart->rom_alloc(size, GENERIC_ROM16_WIDTH, ENDIANNESS_LITTLE);
	m_cart->common_load_rom(m_cart->get_rom_base(), size, "rom");

	return image_init_result::PASS;
}

static INPUT_PORTS_START( leapfrog_leappad )
INPUT_PORTS_END



void leapfrog_leappad_state::leapfrog_leappad(machine_config &config)
{
	//ARCA5(config, m_maincpu, 96000000/10); //  LeapPad Leapfrog 05-9-01 FS80A363  (doesn't appear to be Arcompact, what is it?)
	//m_maincpu->set_addrmap(AS_PROGRAM, &leapfrog_leappad_state::map);

	// screenless

	GENERIC_CARTSLOT(config, m_cart, generic_plain_slot, "leapfrog_leappad_cart");
	m_cart->set_width(GENERIC_ROM16_WIDTH);
	m_cart->set_device_load(FUNC(leapfrog_leappad_state::cart_load), this);

	SOFTWARE_LIST(config, "cart_list").set_original("leapfrog_leappad_cart");
}

void leapfrog_leappad_state::leapfrog_mfleappad(machine_config &config)
{
	//ARCA5(config, m_maincpu, 96000000/10); //  LeapPad Leapfrog 05-9-01 FS80A363  (doesn't appear to be Arcompact, what is it?)
	//m_maincpu->set_addrmap(AS_PROGRAM, &leapfrog_leappad_state::map);

	// screenless

	GENERIC_CARTSLOT(config, m_cart, generic_plain_slot, "leapfrog_mfleappad_cart");
	m_cart->set_width(GENERIC_ROM16_WIDTH);
	m_cart->set_device_load(FUNC(leapfrog_leappad_state::cart_load), this);

	SOFTWARE_LIST(config, "cart_list").set_original("leapfrog_mfleappad_cart");
}

// All of these contain the string "Have you copied our ROM?" near the date codes

ROM_START( leappad )
	ROM_REGION( 0x100000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "leappadbios.bin", 0x000000, 0x100000, CRC(c886cddc) SHA1(f8a83b156feb28315d2321758678e141600a0d4e) ) // contains "Aug 06 2001.16:33:16.155-00450.LeapPad ILA2 Universal Base ROM" and "Copyright (c) 1998-2001 Knowledge Kids Enterprises, Inc."
ROM_END

ROM_START( leappadca )
	ROM_REGION( 0x200000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "leappadbioscanada.bin", 0x000000, 0x200000, CRC(cc12e3db) SHA1(adf52232adcfd4de5d8e31c0e0c09be61718a9d4) ) // contains "Jan 23 2004 11:28:40 152-10620 2MB Canada Full Base ROM" and "Copyright (c) 2000-2004 LeapFrog Enterprises, Inc."
ROM_END

ROM_START( mfleappad )
	ROM_REGION( 0x100000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "myfirstleappadinternational.bin", 0x000000, 0x100000, CRC(4dc0c4d5) SHA1(573ecf2efaccf70e619cf54d63be9169e469ee6f) ) // contains "May 07 2002 10:53:14 152-00932 MFLP International base ROM V1.3" and "Copyright (c) 2002 LeapFrog Enterprises, Inc."
ROM_END

ROM_START( mfleappadus )
	ROM_REGION( 0x400000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "myfirstleappadbios.bin", 0x000000, 0x400000, CRC(19174c16) SHA1(e0ba644fdf38fd5f91ab8c4b673c4a658cc3e612) ) // contains "Feb 13 2004.10:58:53.152-10573.MFLP US Base ROM - 2004" and "Copyright (c) 2004 LeapFrog Enterprises, Inc."
ROM_END

//    year, name,        parent,    compat, machine,            input,            class,                  init,       company,    fullname,                         flags
CONS( 2001, leappad,     0,         0,      leapfrog_leappad,   leapfrog_leappad, leapfrog_leappad_state, empty_init, "LeapFrog", "LeapPad (World)",                MACHINE_IS_SKELETON )
CONS( 2004, leappadca,   leappad,   0,      leapfrog_leappad,   leapfrog_leappad, leapfrog_leappad_state, empty_init, "LeapFrog", "LeapPad (Canada)",               MACHINE_IS_SKELETON )
CONS( 2002, mfleappad,   0,         0,      leapfrog_mfleappad, leapfrog_leappad, leapfrog_leappad_state, empty_init, "LeapFrog", "My First LeapPad (World, V1.3)", MACHINE_IS_SKELETON )
CONS( 2004, mfleappadus, mfleappad, 0,      leapfrog_mfleappad, leapfrog_leappad, leapfrog_leappad_state, empty_init, "LeapFrog", "My First LeapPad (US)",          MACHINE_IS_SKELETON )
