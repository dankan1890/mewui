// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nanamame/optionsform.cpp

	NANAMAME options forms.

***************************************************************************/

#include "fileio.h"
#include "ui/ui.h"
#include "emuopts.h"
#include "optionsform.h"
#include "mame.h"
#include "pluginopts.h"
#include "custom.h"

namespace nanamame
{

dir_form::dir_form(window wd, emu_options& _opt, std::unique_ptr<mame_ui_manager>& _mui)
	: form(wd, { 400, 300 }, appear::decorate<>())
	, m_ui(_mui)
	, m_options(_opt)
{
	this->caption("Setup Directories");
	this->bgcolor(color("#232323"));
	auto& pl = this->get_place();

	this->div("<vert<<vert margin=[2,5] <weight=25 comb><weight=5><lbox>><weight=100 <vert margin=[10,5] <vert gap=5 weight=100 abc>>>><weight=45 panel>>");
	pl["panel"] << m_panel;
	pl["comb"] << m_combox;
	pl["lbox"] << m_listbox;
	pl["abc"] << m_b_add << m_b_change << m_b_del;

	m_listbox.append_header("");
	m_listbox.show_header(false);
	m_listbox.enable_single(true, true);
	m_listbox.bgcolor(color("#2C2C2C"));
	m_listbox.fgcolor(colors::white);
	m_listbox.scheme().item_selected = color("#3296AA");
	m_listbox.scheme().item_bordered = false;
	m_listbox.scheme().item_highlighted = color("#2C2C2C");
	m_listbox.borderless(true);
	m_listbox.always_selected(true);

	m_b_add.set_bground(button_renderer()).edge_effects(false).enable_focus_color(false).fgcolor(colors::white);
	m_b_change.set_bground(button_renderer()).edge_effects(false).enable_focus_color(false).fgcolor(colors::white);
	m_b_del.set_bground(button_renderer()).edge_effects(false).enable_focus_color(false).fgcolor(colors::white);

	for (auto &opt : arts_info)
		m_list[opt.first] = opt.second;
	for (auto &opt : extra_path)
		m_list[opt.first] = opt.second;
	for (auto& opt : m_list)
		m_combox.push_back(opt.first);

	this->collocate();
	handle();
	m_combox.option(0);
	m_combox.renderer(&m_cir);
	m_combox.textbox_colors(color("#2C2C2C"), color("#2C2C2C"));
	m_combox.bgcolor(color("#1E1E1E"));
	m_combox.scheme().foreground = colors::white;
	m_combox.scheme().button_color = color("#2C2C2C");

	auto sz = m_listbox.size().width - m_listbox.scheme().text_margin;
	m_listbox.column_at(0).width(sz);
	this->modality();
}

void dir_form::handle()
{
	m_combox.events().selected([this](const arg_combox& ei) {
		auto opt = ei.widget.caption();
		m_listbox.clear(0);
		path_iterator pt{ "" };
		if (m_ui->options().exists(m_list[opt].c_str()))
			pt = path_iterator(m_ui->options().value(m_list[opt].c_str()));
		else
			pt = path_iterator(m_options.value(m_list[opt].c_str()));

		std::string tmp;
		while (pt.next(tmp))
		{
			m_listbox.at(0).append(tmp);
			m_listbox.at(0).back().fgcolor(colors::white);
		}

		if (!m_listbox.empty())
			m_listbox.at(0).at(0).select(true);
		m_listbox.focus();
	});

	m_b_add.events().click([this] {
		auto sel = m_listbox.selected();
		if (!sel.empty())
		{
			folderbox fb{ *this };
			auto path = fb.show();
			if (!path.empty())
				m_listbox.at(0).append(path);

			m_listbox.focus();
		}
	});

	m_b_del.events().click([this] {
		auto sel = m_listbox.selected();
		if (!sel.empty())
		{
			auto e = m_listbox.at(0).at(sel[0].item);
			m_listbox.erase(e);
			m_listbox.focus();
		}
	});

	m_b_change.events().click([this] {
		auto sel = m_listbox.selected();
		if (!sel.empty())
		{
			folderbox fb{ *this };
			auto path = fb.show();
			if (!path.empty())
				m_listbox.at(0).at(sel[0].item).text(0, path);

			m_listbox.focus();
		}
	});

	m_panel.m_cancel.events().click([this] { this->close();	});

	/*
	m_panel.m_ok.events().click([this] {
	for (auto & e : m_list)
	{
	if (m_ui->options().exists(e.second.c_str()))
	{
	m_ui->options().set_value(e.second, tmppath.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
	}
	else
	{

	}
	}
	});
	*/
}


okcancel_panel::okcancel_panel(window wd)
	: panel<true>(wd)
{
	this->bgcolor(color("#232323"));
	m_place.div("<><weight=177 <vert <><weight=24 gap=5 abc><> > ><weight=5>");
	m_place["abc"] << m_ok << m_cancel;
	m_cancel.set_bground(button_renderer()).fgcolor(colors::white);
	m_ok.set_bground(button_renderer()).fgcolor(colors::white);
	m_ok.edge_effects(false);
	m_cancel.edge_effects(false);
}

folderbox::folderform::folderform(window wd)
	: form(wd, { 334, 325 }, appear::decorate<>())
{
	this->div("<vert <weight=30 margin=5 label><margin=5 folders><weight=45 panel>>");
	this->get_place()["folders"] << m_tb;
	this->get_place()["panel"] << m_panel;
	this->get_place()["label"] << m_lbl;
	this->caption("Browse For Folder");
	this->bgcolor(color("#1E1E1E"));
    m_lbl.fgcolor(colors::white);
	m_lbl.transparent(true);

	m_panel.m_ok.events().click.connect_unignorable([this](const arg_click& arg) {
		_m_click(arg);
	});

	m_panel.m_cancel.events().click.connect_unignorable([this](const arg_click& arg) {
		_m_click(arg);
	});

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
		// add one directory
		while ((dirent = path.next()) != nullptr)
			if (dirent->type == osd::directory::entry::entry_type::DIR && strcmp(dirent->name, ".") != 0 && strcmp(dirent->name, "..") != 0)
			{
				m_tb.insert(node, dirent->name, dirent->name);
				break;
			}
	}

	m_tb.events().expanded([&](const arg_treebox& arg) {
		if (!arg.operated) return;

		m_tb.auto_draw(false);
		auto &node = arg.item;
		auto p = m_tb.make_key_path(arg.item, PATH_SEPARATOR);
		file_enumerator path(p);
		const osd::directory::entry* dirent;
		while ((dirent = path.next()) != nullptr)
			if (dirent->type == osd::directory::entry::entry_type::DIR && strcmp(dirent->name, ".") != 0 && strcmp(dirent->name, "..") != 0)
			{
				auto child = m_tb.insert(node, dirent->name, dirent->name);
				if (child.empty()) continue;

				auto txt = p + PATH_SEPARATOR + dirent->name;
				file_enumerator childpath(txt);
				const osd::directory::entry* childdirent;
				while ((childdirent = childpath.next()) != nullptr)
					if (childdirent->type == osd::directory::entry::entry_type::DIR && strcmp(childdirent->name, ".") != 0 && strcmp(childdirent->name, "..") != 0)
					{
						m_tb.insert(child, childdirent->name, childdirent->name);
						break;
					}
			}

		m_tb.auto_draw(true);
	});

    m_tb.bgcolor(color("#232323"));
	m_tb.renderer(treebox_renderer(m_tb.renderer()));
    m_tb.placer(treebox_placer(m_tb.placer()));
	this->collocate();
	this->modality();
}

void folderbox::folderform::_m_click(const arg_click& arg)
{
	pick_.clear();
	if (arg.window_handle == m_panel.m_ok)
	{
		auto sel = m_tb.selected();
		if (!sel.empty())
			pick_ = m_tb.make_key_path(sel, PATH_SEPARATOR);
	}

	this->close();
}


std::string folderbox::show() const
{
	folderform fm{ wd_ };
	fm.show();
	return fm.pick();
}

plugin_form::plugin_form(window wd, emu_options& _opt)
	: form(wd, { 400, 250 }, appear::decorate<>())
	, m_options(_opt)
{
	this->caption("Setup Plugins");
	this->bgcolor(color("#232323"));
	auto& pl = this->get_place();
	//this->div("<vert <<weight=2><vert <weight=2><gbox><weight=5>><weight=2>><weight=20 tb><weight=45 panel>>"); // Testing masked
	this->div("<vert <<weight=2><vert <weight=2><gbox><weight=5>><weight=2>><weight=45 panel>>");

	m_group.radio_mode(false);
	m_group.bgcolor(color("#2C2C2C"));

	auto& plugins = mame_machine_manager::instance()->plugins();
	for (auto &curentry : plugins)
		if (!curentry.is_header())
		{
			auto enabled = std::string(curentry.value()) == "1";
			auto &cb = m_group.add_option(curentry.description());
			cb.check(enabled);
			cb.fgcolor(colors::white);
			cb.bgcolor(color("#2C2C2C"));
		}

	pl["panel"] << m_panel;
	pl["gbox"] << m_group;
//	pl["tb"] << tb;
//	tb.masked("#?#?#");

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

} // namespace nanamame