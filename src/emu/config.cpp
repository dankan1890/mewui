// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria, Aaron Giles
/***************************************************************************

    config.c

    Configuration file I/O.
***************************************************************************/

#include "emu.h"
#include "emuopts.h"
#include "drivenum.h"
#include "config.h"
#include "xmlfile.h"

#define DEBUG_CONFIG        0

//**************************************************************************
//  CONFIGURATION MANAGER
//**************************************************************************

//-------------------------------------------------
//  configuration_manager - constructor
//-------------------------------------------------


configuration_manager::configuration_manager(running_machine &machine)
	: m_machine(machine)
{
}

/*************************************
 *
 *  Register to be involved in config
 *  save/load
 *
 *************************************/

void configuration_manager::config_register(const char* nodename, config_saveload_delegate load, config_saveload_delegate save)
{
	config_element element;
	element.name = nodename;
	element.load = load;
	element.save = save;

	m_typelist.push_back(element);
}



/*************************************
 *
 *  Settings save/load frontend
 *
 *************************************/

int configuration_manager::load_settings()
{
	const char *controller = machine().options().ctrlr();
	int loaded = 0;

	/* loop over all registrants and call their init function */
	for (auto type : m_typelist)
		type.load(config_type::CONFIG_TYPE_INIT, nullptr);

	/* now load the controller file */
	if (controller[0] != 0)
	{
		/* open the config file */
		emu_file file(machine().options().ctrlr_path(), OPEN_FLAG_READ);
		osd_file::error filerr = file.open(controller, ".cfg");

		if (filerr != osd_file::error::NONE)
			throw emu_fatalerror("Could not load controller file %s.cfg", controller);

		/* load the XML */
		if (!load_xml(file, config_type::CONFIG_TYPE_CONTROLLER))
			throw emu_fatalerror("Could not load controller file %s.cfg", controller);
	}

	/* next load the defaults file */
	emu_file file(machine().options().cfg_directory(), OPEN_FLAG_READ);
	osd_file::error filerr = file.open("default.cfg");
	if (filerr == osd_file::error::NONE)
		load_xml(file, config_type::CONFIG_TYPE_DEFAULT);

	/* finally, load the game-specific file */
	filerr = file.open(machine().basename(), ".cfg");
	if (filerr == osd_file::error::NONE)
		loaded = load_xml(file, config_type::CONFIG_TYPE_GAME);

	/* loop over all registrants and call their final function */
	for (auto type : m_typelist)
		type.load(config_type::CONFIG_TYPE_FINAL, nullptr);

	/* if we didn't find a saved config, return 0 so the main core knows that it */
	/* is the first time the game is run and it should diplay the disclaimer. */
	return loaded;
}


void configuration_manager::save_settings()
{
	/* loop over all registrants and call their init function */
	for (auto type : m_typelist)
		type.save(config_type::CONFIG_TYPE_INIT, nullptr);

	/* save the defaults file */
	emu_file file(machine().options().cfg_directory(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);
	osd_file::error filerr = file.open("default.cfg");
	if (filerr == osd_file::error::NONE)
		save_xml(file, config_type::CONFIG_TYPE_DEFAULT);

	/* finally, save the game-specific file */
	filerr = file.open(machine().basename(), ".cfg");
	if (filerr == osd_file::error::NONE)
		save_xml(file, config_type::CONFIG_TYPE_GAME);

	/* loop over all registrants and call their final function */
	for (auto type : m_typelist)
		type.save(config_type::CONFIG_TYPE_FINAL, nullptr);
}



/*************************************
 *
 *  XML file load
 *
 *************************************/

int configuration_manager::load_xml(emu_file &file, config_type which_type)
{
	xml_data_node *root, *confignode, *systemnode;
	const char *srcfile;
	int version, count;

	/* read the file */
	root = xml_data_node::file_read(file, nullptr);
	if (!root)
		goto error;

	/* find the config node */
	confignode = root->get_child("mameconfig");
	if (!confignode)
		goto error;

	/* validate the config data version */
	version = confignode->get_attribute_int("version", 0);
	if (version != CONFIG_VERSION)
		goto error;

	/* strip off all the path crap from the source filename */
	srcfile = strrchr(machine().system().source_file, '/');
	if (!srcfile)
		srcfile = strrchr(machine().system().source_file, '\\');
	if (!srcfile)
		srcfile = strrchr(machine().system().source_file, ':');
	if (!srcfile)
		srcfile = machine().system().source_file;
	else
		srcfile++;

	/* loop over all system nodes in the file */
	count = 0;
	for (systemnode = confignode->get_child("system"); systemnode; systemnode = systemnode->get_next_sibling("system"))
	{
		/* look up the name of the system here; skip if none */
		const char *name = systemnode->get_attribute_string("name", "");

		/* based on the file type, determine whether we have a match */
		switch (which_type)
		{
			case config_type::CONFIG_TYPE_GAME:
				/* only match on the specific game name */
				if (strcmp(name, machine().system().name) != 0)
					continue;
				break;

			case config_type::CONFIG_TYPE_DEFAULT:
				/* only match on default */
				if (strcmp(name, "default") != 0)
					continue;
				break;

			case config_type::CONFIG_TYPE_CONTROLLER:
			{
				int clone_of;
				/* match on: default, game name, source file name, parent name, grandparent name */
				if (strcmp(name, "default") != 0 &&
					strcmp(name, machine().system().name) != 0 &&
					strcmp(name, srcfile) != 0 &&
					((clone_of = driver_list::clone(machine().system())) == -1 || strcmp(name, driver_list::driver(clone_of).name) != 0) &&
					(clone_of == -1 || ((clone_of = driver_list::clone(clone_of)) == -1) || strcmp(name, driver_list::driver(clone_of).name) != 0))
					continue;
				break;
			}
			default:
				break;
		}

		/* log that we are processing this entry */
		if (DEBUG_CONFIG)
			osd_printf_debug("Entry: %s -- processing\n", name);

		/* loop over all registrants and call their load function */
		for (auto type : m_typelist)
			type.load(which_type, systemnode->get_child(type.name.c_str()));
		count++;
	}

	/* error if this isn't a valid game match */
	if (count == 0)
		goto error;

	/* free the parser */
	root->file_free();
	return 1;

error:
	if (root)
		root->file_free();
	return 0;
}



/*************************************
 *
 *  XML file save
 *
 *************************************/

int configuration_manager::save_xml(emu_file &file, config_type which_type)
{
	xml_data_node *const root = xml_data_node::file_create();
	xml_data_node *confignode, *systemnode;

	/* if we don't have a root, bail */
	if (!root)
		return 0;

	/* create a config node */
	confignode = root->add_child("mameconfig", nullptr);
	if (!confignode)
		goto error;
	confignode->set_attribute_int("version", CONFIG_VERSION);

	/* create a system node */
	systemnode = confignode->add_child("system", nullptr);
	if (!systemnode)
		goto error;
	systemnode->set_attribute("name", (which_type == config_type::CONFIG_TYPE_DEFAULT) ? "default" : machine().system().name);

	/* create the input node and write it out */
	/* loop over all registrants and call their save function */
	for (auto type : m_typelist)
	{
		xml_data_node *curnode = systemnode->add_child(type.name.c_str(), nullptr);
		if (!curnode)
			goto error;
		type.save(which_type, curnode);

		/* if nothing was added, just nuke the node */
		if (!curnode->get_value() && !curnode->get_first_child())
			curnode->delete_node();
	}

	/* flush the file */
	root->file_write(file);

	/* free and get out of here */
	root->file_free();
	return 1;

error:
	root->file_free();
	return 0;
}
