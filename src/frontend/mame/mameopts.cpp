// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    mameopts.cpp

    Options file and command line management.

***************************************************************************/

#include "emu.h"
#include "mameopts.h"

#include "drivenum.h"
#include "screen.h"
#include "softlist_dev.h"
#include "zippath.h"
#include "hashfile.h"
#include "clifront.h"

#include <ctype.h>
#include <stack>


int mame_options::m_slot_options = 0;
int mame_options::m_device_options = 0;

//-------------------------------------------------
//  add_slot_options - add all of the slot
//  options for the configured system
//-------------------------------------------------

bool mame_options::add_slot_options(emu_options &options, value_specifier_func value_specifier)
{
	// look up the system configured by name; if no match, do nothing
	const game_driver *cursystem = system(options);
	if (cursystem == nullptr)
		return false;

	// create the configuration
	machine_config config(*cursystem, options);

	// iterate through all slot devices
	int starting_count = options.options_count();
	for (const device_slot_interface &slot : slot_interface_iterator(config.root_device()))
	{
		// skip fixed slots
		if (slot.fixed())
			continue;

		// retrieve info about the device instance
		const char *name = slot.device().tag() + 1;
		if (!options.exists(name))
		{
			// first device? add the header as to be pretty
			if (m_slot_options++ == 0)
				options.add_entry(nullptr, "SLOT DEVICES", OPTION_HEADER | OPTION_FLAG_DEVICE);

			// add the option
			options.add_entry(name, nullptr, OPTION_STRING | OPTION_FLAG_DEVICE, slot.default_option(), true);
			options.slot_options()[name] = slot_option(slot.default_option());

			// allow opportunity to specify this value
			if (value_specifier)
			{
				std::string specified_value = value_specifier(name);
				if (specified_value != value_specifier_invalid_value())
					options.slot_options()[name].specify(std::move(specified_value));
			}
		}
	}
	return (options.options_count() != starting_count);
}


//-------------------------------------------------
//  update_slot_options - update slot values
//  depending of image mounted
//-------------------------------------------------

void mame_options::update_slot_options(emu_options &options, const software_part *swpart)
{
	// look up the system configured by name; if no match, do nothing
	const game_driver *cursystem = system(options);
	if (cursystem == nullptr)
		return;
	machine_config config(*cursystem, options);

	// iterate through all slot devices
	for (device_slot_interface &slot : slot_interface_iterator(config.root_device()))
	{
		// retrieve info about the device instance
		const char *name = slot.device().tag() + 1;
		if (options.exists(name) && !slot.option_list().empty())
		{
			std::string defvalue = get_default_card_software(slot, options);
			if (defvalue.empty())
			{
				// keep any non-default setting
				if (options.priority(name) > OPTION_PRIORITY_DEFAULT)
					continue;

				// reinstate the actual default value as configured
				if (slot.default_option() != nullptr)
					defvalue.assign(slot.default_option());
			}

			// set the value and hide the option if not selectable
			options.set_default_value(name, defvalue.c_str());
			const device_slot_option *option = slot.option(defvalue.c_str());
			options.set_flag(name, ~OPTION_FLAG_INTERNAL, (option != nullptr && !option->selectable()) ? OPTION_FLAG_INTERNAL : 0);
		}
	}
	add_device_options(options);
}


//-------------------------------------------------
//  get_default_card_software
//-------------------------------------------------

std::string mame_options::get_default_card_software(device_slot_interface &slot, const emu_options &options)
{
	std::string image_path;
	std::function<bool(util::core_file &, std::string&)> get_hashfile_extrainfo;

	// figure out if an image option has been specified, and if so, get the image path out of the options
	device_image_interface *image = dynamic_cast<device_image_interface *>(&slot);
	if (image)
	{
		auto iter = options.image_options().find(image->instance_name());
		if (iter != options.image_options().end())
			image_path = iter->second;

		get_hashfile_extrainfo = [image, &options](util::core_file &file, std::string &extrainfo)
		{
			util::hash_collection hashes = image->calculate_hash_on_file(file);

			return hashfile_extrainfo(
				options.hash_path(),
				image->device().mconfig().gamedrv(),
				hashes,
				extrainfo);
		};
	}

	// create the hook
	get_default_card_software_hook hook(image_path, std::move(get_hashfile_extrainfo));

	// and invoke the slot's implementation of get_default_card_software()
	return slot.get_default_card_software(hook);
}


//-------------------------------------------------
//  add_device_options - add all of the device
//  options for the configured system
//-------------------------------------------------

void mame_options::add_device_options(emu_options &options, value_specifier_func value_specifier)
{
	// look up the system configured by name; if no match, do nothing
	const game_driver *cursystem = system(options);
	if (cursystem == nullptr)
		return;
	machine_config config(*cursystem, options);

	// iterate through all image devices
	for (device_image_interface &image : image_interface_iterator(config.root_device()))
	{
		if (!image.user_loadable())
			continue;

		// add the option
		if (!options.exists(image.instance_name().c_str()))
		{
			// first device? add the header as to be pretty
			if (m_device_options++ == 0)
				options.add_entry(nullptr, "IMAGE DEVICES", OPTION_HEADER | OPTION_FLAG_DEVICE);

			// add the option
			std::string option_name = get_full_option_name(image);
			options.add_entry(option_name.c_str(), nullptr, OPTION_STRING | OPTION_FLAG_DEVICE, nullptr, true);
			options.image_options()[image.instance_name()] = "";

			// allow opportunity to specify this value
			if (value_specifier)
			{
				std::string value = value_specifier(image.instance_name());
				if (value != value_specifier_invalid_value())
					options.image_options()[image.instance_name()] = std::move(value);
			}
		}
	}
}


//-------------------------------------------------
//  remove_device_options - remove device options
//-------------------------------------------------

std::string mame_options::get_full_option_name(const device_image_interface &image)
{
	std::ostringstream option_name;
	util::stream_format(option_name, "%s;%s", image.instance_name(), image.brief_instance_name());
	if (strcmp(image.device_typename(image.image_type()), image.instance_name().c_str()) == 0)
		util::stream_format(option_name, ";%s1;%s1", image.instance_name(), image.brief_instance_name());
	return option_name.str();
}


//-------------------------------------------------
//  remove_device_options - remove device options
//-------------------------------------------------

void mame_options::remove_device_options(emu_options &options)
{
	// iterate through options and remove interesting ones
	core_options::entry *nextentry;
	for (auto *curentry = options.first(); curentry != nullptr; curentry = nextentry)
	{
		// pre-fetch the next entry in case we delete this one
		nextentry = curentry->next();

		// if this is a device option, nuke it
		if ((curentry->flags() & OPTION_FLAG_DEVICE) != 0)
			options.remove_entry(*curentry);
	}

	// take also care of ramsize options
	options.set_default_value(OPTION_RAMSIZE, "");

	// reset counters
	m_slot_options = 0;
	m_device_options = 0;
}


//-------------------------------------------------
//  parse_slot_devices - parse the command line
//  and update slot and image devices
//-------------------------------------------------

void mame_options::parse_slot_devices(emu_options &options, value_specifier_func value_specifier)
{
	bool still_adding = true;
	while (still_adding)
	{
		// keep adding slot options until we stop seeing new stuff
		still_adding = false;
		while (add_slot_options(options, value_specifier))
			still_adding = true;

		// add device options
		add_device_options(options, value_specifier);

		if (reevaluate_slot_options(options))
			still_adding = true;
	}
}


//-------------------------------------------------
//  reevaluate_slot_options - based on recent changes
//  in what images are mounted, give drivers a chance
//  to specify new default slot options
//-------------------------------------------------

bool mame_options::reevaluate_slot_options(emu_options &options)
{
	bool result = false;

	// look up the system configured by name; if no match, do nothing
	const game_driver *cursystem = system(options);
	if (cursystem == nullptr)
		return result;
	machine_config config(*cursystem, options);

	// iterate through all slot devices
	for (device_slot_interface &slot : slot_interface_iterator(config.root_device()))
	{
		// retrieve info about the device instance
		const char *name = slot.device().tag() + 1;
		if (options.exists(name) && !slot.option_list().empty())
		{
			// device_slot_interface::get_default_card_software() is essentially a hook
			// that lets devices provide a feedback loop to force a specified software
			// list entry to be loaded
			//
			// In the repeated cycle of adding slots and slot devices, this gives a chance
			// for devices to "plug in" default software list items.  Of course, the fact
			// that this is all shuffling options is brittle and roundabout, but such is
			// the nature of software lists.
			//
			// In reality, having some sort of hook into the pipeline of slot/device evaluation
			// makes sense, but the fact that it is joined at the hip to device_image_interface
			// and device_slot_interface is unfortunate
			std::string default_card_software = get_default_card_software(slot, options);
			if (!default_card_software.empty())
			{
				// we have default card software - is this resulting in the slot option being mutated?
				if (options.slot_options()[name].default_card_software() != default_card_software)
				{
					options.slot_options()[name].set_default_card_software(std::move(default_card_software));
					result = true;
				}
			}
		}
	}
	return result;
}


//-------------------------------------------------
//  parse_command_line - parse the command line
//  and update the devices
//-------------------------------------------------

bool mame_options::parse_command_line(emu_options &options, std::vector<std::string> &args, std::string &error_string)
{
	// parse the command line
	if (!options.parse_command_line(args, OPTION_PRIORITY_CMDLINE, error_string))
		return false;

	// in order to evaluate softlist options, we need to fish any hashpath variable out of INI files; this is
	// because hashpath in particular can affect softlist evaluation
	if (options.software_name()[0] != '\0' && options.read_config())
		populate_hashpath_from_ini_files(options);

	// identify any options as a result of softlists
	auto softlist_opts = evaluate_initial_softlist_options(options);

	// assemble a "value specifier" that will be used to specify options set up as a consequence
	// of slot and device setup
	auto value_specifier = [&options, &softlist_opts, &args, &error_string](const std::string &arg)
	{
		// first find within the command line
		std::string arg_value;
		bool success = options.pluck_from_command_line(args, arg, arg_value);

		// next try to find within softlist-specified options
		if (!success)
		{
			auto iter = softlist_opts.find(arg);
			if (iter != softlist_opts.end())
			{
				arg_value = iter->second;
				success = true;
			}
		}

		// did we find something?
		return success
			? arg_value
			: value_specifier_invalid_value();
	};

	// some auxillary verbs expect that slot options are specified; and to do this we need to figure
	// out if this is necessary for this particular auxillary verb, and if so, set the system name
	if (!options.command().empty()
		&& cli_frontend::parse_slot_options_for_auxverb(options.command())
		&& !options.command_arguments().empty()
		&& !core_iswildstr(options.command_arguments()[0].c_str()))
	{
		std::string error_string;
		options.set_value(OPTION_SYSTEMNAME, options.command_arguments()[0].c_str(), OPTION_PRIORITY_CMDLINE, error_string);

		const game_driver *system = mame_options::system(options);
		if (!system)
			throw emu_fatalerror(EMU_ERR_NO_SUCH_GAME, "Unknown system '%s'", options.system_name());
	}

	// parse the slot devices
	parse_slot_devices(options, value_specifier);

	// at this point, we should have handled all arguments; the only argument that shouldn't have
	// been handled is the file name
	if (args.size() > 1)
	{
		error_string.append(string_format("Error: unknown option: %s\n", args[1]));
		return false;
	}

	return true;
}


//-------------------------------------------------
//  evaluate_initial_softlist_options
//-------------------------------------------------

std::map<std::string, std::string> mame_options::evaluate_initial_softlist_options(emu_options &options)
{
	std::map<std::string, std::string> results;

	// load software specified at the command line (if any of course)
	std::string software_identifier = options.software_name();
	if (!software_identifier.empty())
	{
		// we have software; first identify the proper game_driver
		const game_driver *system = mame_options::system(options);
		if (system == nullptr && *(options.system_name()) != 0)
			throw emu_fatalerror(EMU_ERR_NO_SUCH_GAME, "Unknown system '%s'", options.system_name());

		// and set up a configuration
		machine_config config(*system, options);
		software_list_device_iterator iter(config.root_device());
		if (iter.count() == 0)
			throw emu_fatalerror(EMU_ERR_FATALERROR, "Error: unknown option: %s\n", options.software_name());

		// and finally set up the stack
		std::stack<std::string> software_identifier_stack;
		software_identifier_stack.push(software_identifier);

		// we need to keep evaluating softlist identifiers until the stack is empty
		while (!software_identifier_stack.empty())
		{
			// pop the identifier
			software_identifier = std::move(software_identifier_stack.top());
			software_identifier_stack.pop();

			// and parse it
			std::string list_name, software_name;
			auto colon_pos = software_identifier.find_first_of(':');
			if (colon_pos != std::string::npos)
			{
				list_name = software_identifier.substr(0, colon_pos);
				software_name = software_identifier.substr(colon_pos + 1);
			}
			else
			{
				software_name = software_identifier;
			}

			// loop through all softlist devices, and try to find one capable of handling the requested software
			bool found = false;
			bool compatible = false;
			for (software_list_device &swlistdev : iter)
			{
				if (list_name.empty() || (list_name == swlistdev.list_name()))
				{
					const software_info *swinfo = swlistdev.find(software_name);
					if (swinfo != nullptr)
					{
						// loop through all parts
						for (const software_part &swpart : swinfo->parts())
						{
							// only load compatible software this way
							if (swlistdev.is_compatible(swpart) == SOFTWARE_IS_COMPATIBLE)
							{
								// we need to find a mountable image slot, but we need to ensure it is a slot
								// for which we have not already distributed a part to
								device_image_interface *image = software_list_device::find_mountable_image(
									config,
									swpart,
									[&results](const device_image_interface &candidate) { return results.count(candidate.instance_name()) == 0; });

								// did we find a slot to put this part into?
								if (image != nullptr)
								{
									// we've resolved this software
									results[image->instance_name()] = string_format("%s:%s:%s", swlistdev.list_name(), software_name, swpart.name());

									// does this software part have a requirement on another part?
									const char *requirement = swpart.feature("requirement");
									if (requirement)
										software_identifier_stack.push(requirement);
								}
								compatible = true;
							}
							found = true;
						}

						// identify other shared features specified as '<<slot name>>_default'
						//
						// example from SMS:
						//
						//  <software name = "alexbmx">
						//      ...
						//      <sharedfeat name = "ctrl1_default" value = "paddle" />
						//  </software>
						for (const feature_list_item &fi : swinfo->shared_info())
						{
							const std::string default_suffix = "_default";
							if (fi.name().size() > default_suffix.size()
								&& fi.name().compare(fi.name().size() - default_suffix.size(), default_suffix.size(), default_suffix) == 0)
							{
								std::string slot_name = fi.name().substr(0, fi.name().size() - default_suffix.size());
								results[slot_name] = fi.value();
							}
						}
					}
				}
				if (compatible)
					break;
			}

			if (!compatible)
			{
				software_list_device::display_matches(config, nullptr, software_name);
				if (!found)
					throw emu_fatalerror(EMU_ERR_FATALERROR, nullptr);
				else
					throw emu_fatalerror(EMU_ERR_FATALERROR, "Software '%s' is incompatible with system '%s'\n", software_name.c_str(), options.system_name());
			}
		}
	}
	return results;
}


//-------------------------------------------------
//  parse_standard_inis - parse the standard set
//  of INI files
//-------------------------------------------------

void mame_options::parse_standard_inis(emu_options &options, std::string &error_string, const game_driver *driver)
{
	// start with an empty string
	error_string.clear();

	// parse the INI file defined by the platform (e.g., "mame.ini")
	// we do this twice so that the first file can change the INI path
	parse_one_ini(options,emulator_info::get_configname(), OPTION_PRIORITY_MAME_INI);
	parse_one_ini(options,emulator_info::get_configname(), OPTION_PRIORITY_MAME_INI, &error_string);

	// debug mode: parse "debug.ini" as well
	if (options.debug())
		parse_one_ini(options,"debug", OPTION_PRIORITY_DEBUG_INI, &error_string);

	// if we have a valid system driver, parse system-specific INI files
	const game_driver *cursystem = (driver == nullptr) ? system(options) : driver;
	if (cursystem == nullptr)
		return;

	// parse "vertical.ini" or "horizont.ini"
	if (cursystem->flags & ORIENTATION_SWAP_XY)
		parse_one_ini(options,"vertical", OPTION_PRIORITY_ORIENTATION_INI, &error_string);
	else
		parse_one_ini(options,"horizont", OPTION_PRIORITY_ORIENTATION_INI, &error_string);

	if (cursystem->flags & MACHINE_TYPE_ARCADE)
		parse_one_ini(options,"arcade", OPTION_PRIORITY_SYSTYPE_INI, &error_string);
	else if (cursystem->flags & MACHINE_TYPE_CONSOLE)
		parse_one_ini(options,"console", OPTION_PRIORITY_SYSTYPE_INI, &error_string);
	else if (cursystem->flags & MACHINE_TYPE_COMPUTER)
		parse_one_ini(options,"computer", OPTION_PRIORITY_SYSTYPE_INI, &error_string);
	else if (cursystem->flags & MACHINE_TYPE_OTHER)
		parse_one_ini(options,"othersys", OPTION_PRIORITY_SYSTYPE_INI, &error_string);

	machine_config config(*cursystem, options);
	for (const screen_device &device : screen_device_iterator(config.root_device()))
	{
		// parse "raster.ini" for raster games
		if (device.screen_type() == SCREEN_TYPE_RASTER)
		{
			parse_one_ini(options,"raster", OPTION_PRIORITY_SCREEN_INI, &error_string);
			break;
		}
		// parse "vector.ini" for vector games
		if (device.screen_type() == SCREEN_TYPE_VECTOR)
		{
			parse_one_ini(options,"vector", OPTION_PRIORITY_SCREEN_INI, &error_string);
			break;
		}
		// parse "lcd.ini" for lcd games
		if (device.screen_type() == SCREEN_TYPE_LCD)
		{
			parse_one_ini(options,"lcd", OPTION_PRIORITY_SCREEN_INI, &error_string);
			break;
		}
	}

	// next parse "source/<sourcefile>.ini"
	std::string sourcename = core_filename_extract_base(cursystem->type.source(), true).insert(0, "source" PATH_SEPARATOR);
	parse_one_ini(options,sourcename.c_str(), OPTION_PRIORITY_SOURCE_INI, &error_string);

	// then parse the grandparent, parent, and system-specific INIs
	int parent = driver_list::clone(*cursystem);
	int gparent = (parent != -1) ? driver_list::clone(parent) : -1;
	if (gparent != -1)
		parse_one_ini(options,driver_list::driver(gparent).name, OPTION_PRIORITY_GPARENT_INI, &error_string);
	if (parent != -1)
		parse_one_ini(options,driver_list::driver(parent).name, OPTION_PRIORITY_PARENT_INI, &error_string);
	parse_one_ini(options,cursystem->name, OPTION_PRIORITY_DRIVER_INI, &error_string);
}


//-------------------------------------------------
//  system - return a pointer to the specified
//  system driver, or nullptr if no match
//-------------------------------------------------

const game_driver *mame_options::system(const emu_options &options)
{
	int index = driver_list::find(core_filename_extract_base(options.system_name(), true).c_str());
	return (index != -1) ? &driver_list::driver(index) : nullptr;
}


//-------------------------------------------------
//  set_system_name - set a new system name
//-------------------------------------------------

void mame_options::set_system_name(emu_options &options, const char *name)
{
	// remember the original system name
	std::string old_system_name(options.system_name());
	bool new_system = old_system_name.compare(name) != 0;

	// if the system name changed, fix up the device options
	if (new_system)
	{
		// first set the new name
		std::string error;
		options.set_value(OPTION_SYSTEMNAME, name, OPTION_PRIORITY_CMDLINE, error);
		assert(error.empty());

		// remove any existing device options
		remove_device_options(options);
	}
	else
	{
		// revert device options set for the old software
		options.revert(OPTION_PRIORITY_SUBCMD, OPTION_PRIORITY_SUBCMD);
	}

	// get the new system
	const game_driver *cursystem = system(options);
	if (cursystem == nullptr)
		return;

	if (*options.software_name() != 0)
	{
		std::string sw_load(options.software_name());
		std::string sw_list, sw_name, sw_part, sw_instance, error_string;
		int left = sw_load.find_first_of(':');
		int middle = sw_load.find_first_of(':', left + 1);
		int right = sw_load.find_last_of(':');

		sw_list = sw_load.substr(0, left);
		sw_name = sw_load.substr(left + 1, middle - left - 1);
		sw_part = sw_load.substr(middle + 1, right - middle - 1);
		sw_instance = sw_load.substr(right + 1);
		sw_load.assign(sw_load.substr(0, right));

		// look up the software part
		machine_config config(*cursystem, options);
		software_list_device *swlist = software_list_device::find_by_name(config, sw_list.c_str());
		const software_info *swinfo = swlist != nullptr ? swlist->find(sw_name.c_str()) : nullptr;
		const software_part *swpart = swinfo != nullptr ? swinfo->find_part(sw_part.c_str()) : nullptr;
		if (swpart == nullptr)
			osd_printf_warning("Could not find %s in software list\n", options.software_name());

		// then add the options
		if (new_system)
		{
			while (add_slot_options(options)) {}
			add_device_options(options);
		}

		options.set_value(OPTION_SOFTWARENAME, sw_name.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
		if (options.exists(sw_instance.c_str()))
			options.set_value(sw_instance.c_str(), sw_load.c_str(), OPTION_PRIORITY_SUBCMD, error_string);

		int num;
		do {
			num = options.options_count();
			update_slot_options(options,swpart);
		} while (num != options.options_count());
	}
	else if (new_system)
	{
		// add the options afresh
		while (add_slot_options(options)) {}
		add_device_options(options);
		int num;
		do {
			num = options.options_count();
			update_slot_options(options);
		} while (num != options.options_count());
	}
}

//-------------------------------------------------
//  parse_one_ini - parse a single INI file
//-------------------------------------------------

bool mame_options::parse_one_ini(emu_options &options, const char *basename, int priority, std::string *error_string)
{
	// don't parse if it has been disabled
	if (!options.read_config())
		return false;

	// open the file; if we fail, that's ok
	emu_file file(options.ini_path(), OPEN_FLAG_READ);
	osd_printf_verbose("Attempting load of %s.ini\n", basename);
	osd_file::error filerr = file.open(basename, ".ini");
	if (filerr != osd_file::error::NONE)
		return false;

	// parse the file
	osd_printf_verbose("Parsing %s.ini\n", basename);
	std::string error;
	bool result = options.parse_ini_file((util::core_file&)file, priority, priority < OPTION_PRIORITY_DRIVER_INI, error);

	// append errors if requested
	if (!error.empty() && error_string)
		error_string->append(string_format("While parsing %s:\n%s\n", file.fullpath(), error));

	return result;
}


//-------------------------------------------------
//  populate_hashpath_from_ini_files
//-------------------------------------------------

void mame_options::populate_hashpath_from_ini_files(emu_options &options)
{
	// create temporary emu_options for the purposes of evaluating the INI files
	emu_options temp_options;
	std::string temp_error_string;
	temp_options.set_value(OPTION_SYSTEMNAME, options.system_name(), OPTION_PRIORITY_MAXIMUM, temp_error_string);
	temp_options.set_value(OPTION_INIPATH, options.ini_path(), OPTION_PRIORITY_MAXIMUM, temp_error_string);

	// read the INIs into temp_options
	parse_standard_inis(temp_options, temp_error_string);

	// and fish out hashpath
	const auto entry = temp_options.get_entry(OPTION_HASHPATH);
	if (entry)
		options.set_value(OPTION_HASHPATH, entry->value(), entry->priority(), temp_error_string);
}
