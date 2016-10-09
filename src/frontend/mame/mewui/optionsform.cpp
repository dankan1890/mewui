// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	mewui/options.cpp

	MEWUI options forms.

***************************************************************************/

#include "fileio.h"
#include "ui/ui.h"
#include "emuopts.h"
#include "mewui/optionsform.h"
#include "mame.h"
#include "pluginopts.h"

namespace mewui {

dir_form::dir_form(window wd, emu_options& _opt, std::unique_ptr<mame_ui_manager>& _mui)
	: form(wd, { 400, 200 }, appear::decorate<>())
	, m_ui(_mui)
	, m_options(_opt)
{
	this->caption("Setup Directories");
	this->bgcolor(color(214, 219, 233));
	auto& pl = this->get_place();

	this->div("vert <<weight=2><vert <weight=5><weight=25 comb><weight=5><lbox><weight=5>><weight=2>><weight=45 panel>");
	pl["panel"] << m_panel;
	pl["comb"] << m_combox;
	pl["lbox"] << m_listbox;
	m_listbox.append_header("");
	m_listbox.show_header(false);

	for (auto& opt : arts_info)
		m_combox.push_back(opt.first);

	m_combox.events().selected([this](const arg_combox& ei) {
		auto opt = ei.widget.option();
		m_listbox.clear(0);
		path_iterator pt{ "" };
		if (m_ui->options().exists(arts_info[opt].second.c_str()))
			pt = path_iterator(m_ui->options().value(arts_info[opt].second.c_str()));
		else
			pt = path_iterator(m_options.value(arts_info[opt].second.c_str()));

		std::string tmp;
		while (pt.next(tmp))
			m_listbox.at(0).append(tmp);

		m_listbox.at(0).append("<Add>");
		m_listbox.at(0).at(0).select(true);
		m_listbox.focus();
	});

	m_listbox.events().dbl_click([this] {
		auto sel = m_listbox.selected();
		if (!sel.empty() && m_listbox.at(0).at(sel[0].item).text(0) == "<Add>")
		{
			folderform fb{ *this };
			fb.show();
		}
	});

	m_panel.m_cancel.events().click([this] {
		this->close();
	});

	m_combox.option(0);

	this->collocate();
	auto sz = m_listbox.size().width - m_listbox.scheme().text_margin;
	m_listbox.column_at(0).width(sz);
	this->modality();
}

okcancel_panel::okcancel_panel(window wd)
	: panel<true>(wd)
{
	this->bgcolor(colors::azure);
	m_place.div("<><weight=210 vert <><weight=25 gap=10 abc><>> <weight=10>");
	m_place["abc"] << m_ok << m_cancel;
	m_ok.bgcolor(colors::azure);
	m_cancel.bgcolor(colors::azure);
}

folderform::folderform(window wd)
	: form{ wd }
{
	this->div("<weight=5><vert <weight=5><weight=20 label><weight=5><folders><weight=5><weight=25 gap=10 buttons><weight=5>><weight=5>");
	this->get_place()["folders"] << m_tb;
	this->get_place()["buttons"] << m_ok << m_cancel;
	this->get_place()["label"] << m_lbl;
	this->caption("Browse For Folder");

	// configure the starting path
	std::string exepath;
	osd_get_full_path(exepath, ".");
	const char* volume_name = nullptr;
	// add the drives
	for (auto i = 0; (volume_name = osd_get_volume_name(i)) != nullptr; ++i)
	{
		std::string v_name(!strcmp(volume_name, "/") ? "FS.ROOT" : volume_name);
		auto node = m_tb.insert(v_name, volume_name);
		file_enumerator path(volume_name);
		const osd::directory::entry* dirent;
		// add the directories
		while ((dirent = path.next()) != nullptr)
		{
			if (dirent->type == osd::directory::entry::entry_type::DIR && strcmp(dirent->name, ".") != 0 && strcmp(dirent->name, "..") != 0)
				m_tb.insert(node, dirent->name, dirent->name);
		}
	}
	this->collocate();
	this->modality();
}


plugin_form::plugin_form(window wd, emu_options& _opt, std::unique_ptr<mame_ui_manager>& _mui)
	: form(wd, { 400, 200 }, appear::decorate<>())
	, m_ui(_mui)
	, m_options(_opt)
{
	this->caption("Setup Plugins");
	this->bgcolor(color(214, 219, 233));
	auto& pl = this->get_place();
	this->div("vert <<weight=2><vert <weight=5><gbox><weight=5>><weight=2>><weight=45 panel>");

	m_group.radio_mode(false);
	m_group.bgcolor(color(214, 219, 233));

	auto& plugins = mame_machine_manager::instance()->plugins();
	for (auto &curentry : plugins)
		if (!curentry.is_header())
		{
			auto enabled = std::string(curentry.value()) == "1";
			auto &cb = m_group.add_option(curentry.description());
			cb.check(enabled);
		}

	pl["panel"] << m_panel;
	pl["gbox"] << m_group;

	m_panel.m_cancel.events().click([this] {
		this->close();
	});

	m_panel.m_ok.events().click([this] {
		auto& plugin = mame_machine_manager::instance()->plugins();
		std::string error_string;
		auto index = 0;
		for (auto &curentry : plugin)
			if (!curentry.is_header())
			{
				auto checked = m_group.option_checked(index++);
				plugin.set_value(curentry.name(), checked ? 1 : 0, OPTION_PRIORITY_CMDLINE, error_string);
			}

		emu_file file_plugin(m_options.ini_path(), OPEN_FLAG_WRITE | OPEN_FLAG_CREATE | OPEN_FLAG_CREATE_PATHS);
		if (file_plugin.open("plugin.ini") != osd_file::error::NONE)
			return;

		// generate the updated INI
		file_plugin.puts(plugin.output_ini().c_str());
		this->close();
	});

	this->collocate();
	this->modality();
}

} // namespace mewui