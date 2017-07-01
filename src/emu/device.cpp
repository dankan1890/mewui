// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    device.c

    Device interface functions.

***************************************************************************/

#include "emu.h"
#include "string.h"
#include "debug/debugcpu.h"


//**************************************************************************
//  LIVE DEVICE MANAGEMENT
//**************************************************************************

//-------------------------------------------------
//  device_t - constructor for a new
//  running device; initial state is derived
//  from the provided config
//-------------------------------------------------

device_t::device_t(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, u32 clock, const char *shortname, const char *source)
	: m_type(type),
		m_name(name),
		m_shortname(shortname),
		m_searchpath(shortname),
		m_source(source),
		m_owner(owner),
		m_next(nullptr),

		m_configured_clock(clock),
		m_unscaled_clock(clock),
		m_clock(clock),
		m_clock_scale(1.0),
		m_attoseconds_per_clock((clock == 0) ? 0 : HZ_TO_ATTOSECONDS(clock)),

		m_machine_config(mconfig),
		m_input_defaults(nullptr),
		m_default_bios_tag(""),

		m_machine(nullptr),
		m_save(nullptr),
		m_basetag(tag),
		m_config_complete(false),
		m_started(false),
		m_auto_finder_list(nullptr)
{
	if (owner != nullptr)
		m_tag.assign((owner->owner() == nullptr) ? "" : owner->tag()).append(":").append(tag);
	else
		m_tag.assign(":");
	static_set_clock(*this, clock);
}


//-------------------------------------------------
//  ~device_t - destructor for a device_t
//-------------------------------------------------

device_t::~device_t()
{
}


//-------------------------------------------------
//  memregion - return a pointer to the region
//  info for a given region
//-------------------------------------------------

memory_region *device_t::memregion(const char *_tag) const
{
	// build a fully-qualified name and look it up
	if (_tag)
	{
		auto search = machine().memory().regions().find(subtag(_tag).c_str());
		if (search != machine().memory().regions().end())
			return search->second.get();
		else
			return nullptr;
	}
	else
		return nullptr;
}


//-------------------------------------------------
//  memshare - return a pointer to the memory share
//  info for a given share
//-------------------------------------------------

memory_share *device_t::memshare(const char *_tag) const
{
	// build a fully-qualified name and look it up
	if (_tag)
	{
		auto search = machine().memory().shares().find(subtag(_tag).c_str());
		if (search != machine().memory().shares().end())
			return search->second.get();
		else
			return nullptr;
	}
	else
		return nullptr;
}


//-------------------------------------------------
//  membank - return a pointer to the memory
//  bank info for a given bank
//-------------------------------------------------

memory_bank *device_t::membank(const char *_tag) const
{
	if (_tag)
	{
		auto search = machine().memory().banks().find(subtag(_tag).c_str());
		if (search != machine().memory().banks().end())
			return search->second.get();
		else
			return nullptr;
	}
	else
		return nullptr;
}


//-------------------------------------------------
//  ioport - return a pointer to the I/O port
//  object for a given port name
//-------------------------------------------------

ioport_port *device_t::ioport(const char *tag) const
{
	// build a fully-qualified name and look it up
	return machine().ioport().port(subtag(tag).c_str());
}


//-------------------------------------------------
//  ioport - return a pointer to the I/O port
//  object for a given port name
//-------------------------------------------------

std::string device_t::parameter(const char *tag) const
{
	// build a fully-qualified name and look it up
	return machine().parameters().lookup(subtag(tag));
}


//-------------------------------------------------
//  static_set_clock - set/change the clock on
//  a device
//-------------------------------------------------

void device_t::static_set_clock(device_t &device, u32 clock)
{
	// derive the clock from our owner if requested
	if ((clock & 0xff000000) == 0xff000000)
	{
		assert(device.m_owner != nullptr);
		clock = device.m_owner->m_configured_clock * ((clock >> 12) & 0xfff) / ((clock >> 0) & 0xfff);
	}

	device.m_clock = device.m_unscaled_clock = device.m_configured_clock = clock;
	device.m_attoseconds_per_clock = (clock == 0) ? 0 : HZ_TO_ATTOSECONDS(clock);
}


//-------------------------------------------------
//  config_complete - called when the
//  configuration of a device is complete
//-------------------------------------------------

void device_t::config_complete()
{
	// first notify the interfaces
	for (device_interface &intf : interfaces())
		intf.interface_config_complete();

	// then notify the device itself
	device_config_complete();

	// then mark ourselves complete
	m_config_complete = true;
}


//-------------------------------------------------
//  validity_check - validate a device after the
//  configuration has been constructed
//-------------------------------------------------

void device_t::validity_check(validity_checker &valid) const
{
	// validate via the interfaces
	for (device_interface &intf : interfaces())
		intf.interface_validity_check(valid);

	// let the device itself validate
	device_validity_check(valid);
}


//-------------------------------------------------
//  reset - reset a device
//-------------------------------------------------

void device_t::reset()
{
	// let the interfaces do their pre-work
	for (device_interface &intf : interfaces())
		intf.interface_pre_reset();

	// reset the device
	device_reset();

	// reset all child devices
	for (device_t &child : subdevices())
		child.reset();

	// now allow for some post-child reset action
	device_reset_after_children();

	// let the interfaces do their post-work
	for (device_interface &intf : interfaces())
		intf.interface_post_reset();
}


//-------------------------------------------------
//  set_unscaled_clock - sets the given device's
//  unscaled clock
//-------------------------------------------------

void device_t::set_unscaled_clock(u32 clock)
{
	m_unscaled_clock = clock;
	m_clock = m_unscaled_clock * m_clock_scale;
	m_attoseconds_per_clock = (m_clock == 0) ? 0 : HZ_TO_ATTOSECONDS(m_clock);
	notify_clock_changed();
}


//-------------------------------------------------
//  set_clock_scale - sets a scale factor for the
//  device's clock
//-------------------------------------------------

void device_t::set_clock_scale(double clockscale)
{
	m_clock_scale = clockscale;
	m_clock = m_unscaled_clock * m_clock_scale;
	m_attoseconds_per_clock = (m_clock == 0) ? 0 : HZ_TO_ATTOSECONDS(m_clock);
	notify_clock_changed();
}


//-------------------------------------------------
//  clocks_to_attotime - converts a number of
//  clock ticks to an attotime
//-------------------------------------------------

attotime device_t::clocks_to_attotime(u64 numclocks) const
{
	if (numclocks < m_clock)
		return attotime(0, numclocks * m_attoseconds_per_clock);
	else
	{
		u32 remainder;
		u32 quotient = divu_64x32_rem(numclocks, m_clock, &remainder);
		return attotime(quotient, u64(remainder) * u64(m_attoseconds_per_clock));
	}
}


//-------------------------------------------------
//  attotime_to_clocks - converts a duration as
//  attotime to CPU clock ticks
//-------------------------------------------------

u64 device_t::attotime_to_clocks(const attotime &duration) const
{
	return mulu_32x32(duration.seconds(), m_clock) + u64(duration.attoseconds()) / u64(m_attoseconds_per_clock);
}


//-------------------------------------------------
//  timer_alloc - allocate a timer for our device
//  callback
//-------------------------------------------------

emu_timer *device_t::timer_alloc(device_timer_id id, void *ptr)
{
	return machine().scheduler().timer_alloc(*this, id, ptr);
}


//-------------------------------------------------
//  timer_set - set a temporary timer that will
//  call our device callback
//-------------------------------------------------

void device_t::timer_set(const attotime &duration, device_timer_id id, int param, void *ptr)
{
	machine().scheduler().timer_set(duration, *this, id, param, ptr);
}


//-------------------------------------------------
//  set_machine - notify that the machine now
//  exists
//-------------------------------------------------

void device_t::set_machine(running_machine &machine)
{
	m_machine = &machine;
	m_save = &machine.save();
}

//-------------------------------------------------
//  findit - search for all objects in auto finder
//  list and return status
//-------------------------------------------------

bool device_t::findit(bool isvalidation) const
{
	bool allfound = true;
	for (finder_base *autodev = m_auto_finder_list; autodev != nullptr; autodev = autodev->next())
	{
		if (isvalidation)
		{
			// sanity checking
			const char *tag = autodev->finder_tag();
			if (tag == nullptr)
			{
				osd_printf_error("Finder tag is null!\n");
				allfound = false;
				continue;
			}
			if (tag[0] == '^' && tag[1] == ':')
			{
				osd_printf_error("Malformed finder tag: %s\n", tag);
				allfound = false;
				continue;
			}
		}
		allfound &= autodev->findit(isvalidation);
	}
	return allfound;
}

//-------------------------------------------------
//  start - start a device
//-------------------------------------------------

void device_t::start()
{
	// prepare the logerror buffer
	if (m_machine->allow_logging())
		m_string_buffer.reserve(1024);

	// find all the registered devices
	if (!findit(false))
		throw emu_fatalerror("Missing some required objects, unable to proceed");

	// let the interfaces do their pre-work
	for (device_interface &intf : interfaces())
		intf.interface_pre_start();

	// remember the number of state registrations
	int state_registrations = machine().save().registration_count();

	// start the device
	device_start();

	// complain if nothing was registered by the device
	state_registrations = machine().save().registration_count() - state_registrations;
	device_execute_interface *exec;
	device_sound_interface *sound;
	if (state_registrations == 0 && (interface(exec) || interface(sound)) && type() != SPEAKER)
	{
		logerror("Device did not register any state to save!\n");
		if ((machine().system().flags & MACHINE_SUPPORTS_SAVE) != 0)
			fatalerror("Device '%s' did not register any state to save!\n", tag());
	}

	// let the interfaces do their post-work
	for (device_interface &intf : interfaces())
		intf.interface_post_start();

	// force an update of the clock
	notify_clock_changed();

	// if we're debugging, create a device_debug object
	if ((machine().debug_flags & DEBUG_FLAG_ENABLED) != 0)
	{
		m_debug = std::make_unique<device_debug>(*this);
		debug_setup();
	}

	// register our save states
	save_item(NAME(m_clock));
	save_item(NAME(m_unscaled_clock));
	save_item(NAME(m_clock_scale));

	// we're now officially started
	m_started = true;
}


//-------------------------------------------------
//  stop - stop a device
//-------------------------------------------------

void device_t::stop()
{
	// let the interfaces do their pre-work
	for (device_interface &intf : interfaces())
		intf.interface_pre_stop();

	// stop the device
	device_stop();

	// let the interfaces do their post-work
	for (device_interface &intf : interfaces())
		intf.interface_post_stop();

	// free any debugging info
	m_debug.reset();

	// we're now officially stopped, and the machine is off-limits
	m_started = false;
	m_machine = nullptr;
}


//-------------------------------------------------
//  debug_setup - set up for debugging
//-------------------------------------------------

void device_t::debug_setup()
{
	// notify the interface
	for (device_interface &intf : interfaces())
		intf.interface_debug_setup();

	// notify the device
	device_debug_setup();
}


//-------------------------------------------------
//  pre_save - tell the device and its interfaces
//  that we are about to save
//-------------------------------------------------

void device_t::pre_save()
{
	// notify the interface
	for (device_interface &intf : interfaces())
		intf.interface_pre_save();

	// notify the device
	device_pre_save();
}


//-------------------------------------------------
//  post_load - tell the device and its interfaces
//  that we just completed a load
//-------------------------------------------------

void device_t::post_load()
{
	// notify the interface
	for (device_interface &intf : interfaces())
		intf.interface_post_load();

	// notify the device
	device_post_load();
}


//-------------------------------------------------
//  notify_clock_changed - notify all interfaces
//  that the clock has changed
//-------------------------------------------------

void device_t::notify_clock_changed()
{
	// first notify interfaces
	for (device_interface &intf : interfaces())
		intf.interface_clock_changed();

	// then notify the device
	device_clock_changed();
}


//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void device_t::device_config_complete()
{
	// do nothing by default
}


//-------------------------------------------------
//  device_validity_check - validate a device after
//  the configuration has been constructed
//-------------------------------------------------

void device_t::device_validity_check(validity_checker &valid) const
{
	// do nothing by default
}


//-------------------------------------------------
//  rom_region - return a pointer to the implicit
//  rom region description for this device
//-------------------------------------------------

const tiny_rom_entry *device_t::device_rom_region() const
{
	// none by default
	return nullptr;
}


//-------------------------------------------------
//  machine_config - return a pointer to a machine
//  config constructor describing sub-devices for
//  this device
//-------------------------------------------------

machine_config_constructor device_t::device_mconfig_additions() const
{
	// none by default
	return nullptr;
}


//-------------------------------------------------
//  input_ports - return a pointer to the implicit
//  input ports description for this device
//-------------------------------------------------

ioport_constructor device_t::device_input_ports() const
{
	// none by default
	return nullptr;
}


//-------------------------------------------------
//  device_reset - actually handle resetting of
//  a device; designed to be overridden by the
//  actual device implementation
//-------------------------------------------------

void device_t::device_reset()
{
	// do nothing by default
}


//-------------------------------------------------
//  device_reset_after_children - hook to do
//  reset logic that must happen after the children
//  are reset; designed to be overridden by the
//  actual device implementation
//-------------------------------------------------

void device_t::device_reset_after_children()
{
	// do nothing by default
}


//-------------------------------------------------
//  device_stop - clean up anything that needs to
//  happen before the running_machine goes away
//-------------------------------------------------

void device_t::device_stop()
{
	// do nothing by default
}


//-------------------------------------------------
//  device_pre_save - called prior to saving the
//  state, so that registered variables can be
//  properly normalized
//-------------------------------------------------

void device_t::device_pre_save()
{
	// do nothing by default
}


//-------------------------------------------------
//  device_post_load - called after the loading a
//  saved state, so that registered variables can
//  be expanded as necessary
//-------------------------------------------------

void device_t::device_post_load()
{
	// do nothing by default
}


//-------------------------------------------------
//  device_clock_changed - called when the
//  device clock is altered in any way; designed
//  to be overridden by the actual device
//  implementation
//-------------------------------------------------

void device_t::device_clock_changed()
{
	// do nothing by default
}


//-------------------------------------------------
//  device_debug_setup - called when the debugger
//  is active to allow for device-specific setup
//-------------------------------------------------

void device_t::device_debug_setup()
{
	// do nothing by default
}


//-------------------------------------------------
//  device_timer - called whenever a device timer
//  fires
//-------------------------------------------------

void device_t::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	// do nothing by default
}


//-------------------------------------------------
//  subdevice_slow - perform a slow name lookup,
//  caching the results
//-------------------------------------------------

device_t *device_t::subdevice_slow(const char *tag) const
{
	// resolve the full path
	std::string fulltag = subtag(tag);

	// we presume the result is a rooted path; also doubled colons mess up our
	// tree walk, so catch them early
	assert(fulltag[0] == ':');
	assert(fulltag.find("::") == -1);

	// walk the device list to the final path
	device_t *curdevice = &mconfig().root_device();
	if (fulltag.length() > 1)
		for (int start = 1, end = fulltag.find_first_of(':', start); start != 0 && curdevice != nullptr; start = end + 1, end = fulltag.find_first_of(':', start))
		{
			std::string part(fulltag, start, (end == -1) ? -1 : end - start);
			curdevice = curdevice->subdevices().find(part);
		}

	// if we got a match, add to the fast map
	if (curdevice != nullptr)
		m_subdevices.m_tagmap.insert(std::make_pair(tag, curdevice));
	return curdevice;
}


//-------------------------------------------------
//  subtag - create a fully resolved path relative
//  to our device based on the provided tag
//-------------------------------------------------

std::string device_t::subtag(const char *tag) const
{
	std::string result;
	// if the tag begins with a colon, ignore our path and start from the root
	if (*tag == ':')
	{
		tag++;
		result.assign(":");
	}

	// otherwise, start with our path
	else
	{
		result.assign(m_tag);
		if (result != ":")
			result.append(":");
	}

	// iterate over the tag, look for special path characters to resolve
	const char *caret;
	while ((caret = strchr(tag, '^')) != nullptr)
	{
		// copy everything up to there
		result.append(tag, caret - tag);
		tag = caret + 1;

		// strip trailing colons
		int len = result.length();
		while (result[--len] == ':')
			result = result.substr(0, len);

		// remove the last path part, leaving the last colon
		if (result != ":")
		{
			int lastcolon = result.find_last_of(':');
			if (lastcolon != -1)
				result = result.substr(0, lastcolon + 1);
		}
	}

	// copy everything else
	result.append(tag);

	// strip trailing colons up to the root
	int len = result.length();
	while (len > 1 && result[--len] == ':')
		result = result.substr(0, len);
	return result;
}


//-------------------------------------------------
//  register_auto_finder - add a new item to the
//  list of stuff to find after we go live
//-------------------------------------------------

finder_base *device_t::register_auto_finder(finder_base &autodev)
{
	// add to this list
	finder_base *old = m_auto_finder_list;
	m_auto_finder_list = &autodev;
	return old;
}

//**************************************************************************
//  LIVE DEVICE INTERFACES
//**************************************************************************

//-------------------------------------------------
//  device_interface - constructor
//-------------------------------------------------

device_interface::device_interface(device_t &device, const char *type)
	: m_interface_next(nullptr),
		m_device(device),
		m_type(type)
{
	device_interface **tailptr;
	for (tailptr = &device.interfaces().m_head; *tailptr != nullptr; tailptr = &(*tailptr)->m_interface_next) { }
	*tailptr = this;
}


//-------------------------------------------------
//  ~device_interface - destructor
//-------------------------------------------------

device_interface::~device_interface()
{
}


//-------------------------------------------------
//  interface_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void device_interface::interface_config_complete()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_validity_check - default validation
//  for a device after the configuration has been
//  constructed
//-------------------------------------------------

void device_interface::interface_validity_check(validity_checker &valid) const
{
}


//-------------------------------------------------
//  interface_pre_start - called before the
//  device's own start function
//-------------------------------------------------

void device_interface::interface_pre_start()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_post_start - called after the
//  device's own start function
//-------------------------------------------------

void device_interface::interface_post_start()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_pre_reset - called before the
//  device's own reset function
//-------------------------------------------------

void device_interface::interface_pre_reset()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_post_reset - called after the
//  device's own reset function
//-------------------------------------------------

void device_interface::interface_post_reset()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_pre_stop - called before the
//  device's own stop function
//-------------------------------------------------

void device_interface::interface_pre_stop()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_post_stop - called after the
//  device's own stop function
//-------------------------------------------------

void device_interface::interface_post_stop()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_pre_save - called prior to saving the
//  state, so that registered variables can be
//  properly normalized
//-------------------------------------------------

void device_interface::interface_pre_save()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_post_load - called after the loading a
//  saved state, so that registered variables can
//  be expaneded as necessary
//-------------------------------------------------

void device_interface::interface_post_load()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_clock_changed - called when the
//  device clock is altered in any way; designed
//  to be overridden by the actual device
//  implementation
//-------------------------------------------------

void device_interface::interface_clock_changed()
{
	// do nothing by default
}


//-------------------------------------------------
//  interface_debug_setup - called to allow
//  interfaces to set up any debugging for this
//  device
//-------------------------------------------------

void device_interface::interface_debug_setup()
{
	// do nothing by default
}


//-------------------------------------------------
// rom_region_vector
//-------------------------------------------------

const std::vector<rom_entry> &device_t::rom_region_vector() const
{
	if (m_rom_entries.empty())
	{
		m_rom_entries = rom_build_entries(device_rom_region());
	}
	return m_rom_entries;
}
