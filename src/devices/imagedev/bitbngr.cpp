// license:BSD-3-Clause
// copyright-holders:Nathan Woods, Miodrag Milanovic
/*********************************************************************

    bitbngr.c

*********************************************************************/

#include "emu.h"
#include "bitbngr.h"



/***************************************************************************
    IMPLEMENTATION
***************************************************************************/

const device_type BITBANGER = &device_creator<bitbanger_device>;

/*-------------------------------------------------
    ctor
-------------------------------------------------*/

bitbanger_device::bitbanger_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, BITBANGER, "Bitbanger", tag, owner, clock, "bitbanger", __FILE__),
	device_image_interface(mconfig, *this)
{
}



/*-------------------------------------------------
    native_output - outputs data to a file
-------------------------------------------------*/

void bitbanger_device::output(uint8_t data)
{
	if (exists())
	{
		fwrite(&data, 1);
	}
}


/*-------------------------------------------------
    native_input - inputs data from a file
-------------------------------------------------*/

uint32_t bitbanger_device::input(void *buffer, uint32_t length)
{
	if (exists())
	{
		return fread(buffer, length);
	}
	return 0;
}



/*-------------------------------------------------
    device_start
-------------------------------------------------*/

void bitbanger_device::device_start(void)
{
}



/*-------------------------------------------------
    device_config_complete
-------------------------------------------------*/

void bitbanger_device::device_config_complete(void)
{
	update_names(BITBANGER, "bitbngr", "bitb");
}



/*-------------------------------------------------
    call_load
-------------------------------------------------*/

image_init_result bitbanger_device::call_load(void)
{
	/* we don't need to do anything special */
	return image_init_result::PASS;
}

image_init_result bitbanger_device::call_create(int format_type, util::option_resolution *format_options)
{
	/* we don't need to do anything special */
	return image_init_result::PASS;
}

/*-------------------------------------------------
    call_unload
-------------------------------------------------*/

void bitbanger_device::call_unload(void)
{
}
