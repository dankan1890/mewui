// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/***************************************************************************

	nanamame/mainform.cpp

	NANAMAME Main Form.

***************************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "drivenum.h"
#include "emuopts.h"
#include "datfile.h"
#include "softlist.h"
#include "softlist_dev.h"
#include "audit.h"
#include "luaengine.h"
#include "mainform.h"
#include "mame.h"
#include "pluginopts.h"
#include "optionsform.h"
#include <nana/paint/pixel_buffer.hpp>
#include <nana/gui/widgets/scroll.hpp>
#include <set>

namespace nanamame
{

static const std::map<std::string, std::pair<std::function<bool(std::uint32_t&)>, int>> filters_option_ex =
{
	{ "All", { [](std::uint32_t &a) { return false; }, -1 } },
	{ "Working", { [](std::uint32_t &a) { return (a & MACHINE_NOT_WORKING) != 0; }, -1 } },
	{ "Not Working", { [](std::uint32_t &a) { return (a & MACHINE_NOT_WORKING) == 0; }, -1 } },
	{ "Parents", { [](std::uint32_t &a) { return false; }, -1 } },
	{ "Clones", { [](std::uint32_t &a) { return false; }, -1 } },
	{ "Horizontal", { [](std::uint32_t &a) { return (a & ORIENTATION_SWAP_XY) != 0; }, -1 } },
	{ "Vertical", { [](std::uint32_t &a) { return (a & ORIENTATION_SWAP_XY) == 0; }, -1 } },
	{ "Save State", { [](std::uint32_t &a) { return (a & MACHINE_SUPPORTS_SAVE) != 0; }, -1 } },
	{ "Mechanical", { [](std::uint32_t &a) { return (a & MACHINE_MECHANICAL) == 0; }, -1 } },
	{ "Not Mechanical", { [](std::uint32_t &a) { return (a & MACHINE_MECHANICAL) != 0; }, -1 } },
	{ "Manufacturer", { [](std::uint32_t &a) { return false; }, 0 } },
	{ "Year", { [](std::uint32_t &a) { return false; } , 1 } },
	{ "Source", { [](std::uint32_t &a) { return false; }, 2 } },
};

const std::pair<std::string, std::string> submenu[] = {
	{ "Software Packages", "soft" },
	{ "Status Bar", "statusbar" },
	{ "Filters", "treebox" },
	{ "Right Panel", "rpanel" }
};

static const std::unordered_map<std::string, std::string> soft_type =
{
	{ "unknown", "unkn" },
	{ "cartridge", "cart" },
	{ "floppydisk", "flop" },
	{ "harddisk", "hard" },
	{ "cylinder", "cyln" },
	{ "cassette", "cass" },
	{ "punchcard", "pcrd" },
	{ "punchtape", "ptap" },
	{ "printer", "prin" },
	{ "serial", "serl" },
	{ "parallel", "parl" },
	{ "snapshot", "dump" },
	{ "quickload", "quik" },
	{ "memcard", "memc" },
	{ "cdrom", "cdrm" },
	{ "magtape", "magt" },
	{ "romimage", "rom" },
	{ "midiin", "min" },
	{ "midiout", "mout" }
};

main_form::main_form(running_machine& machine, const game_driver** _system, emu_options& _options, std::unique_ptr<mame_ui_manager>& _mui, std::string& exename)
	: form(nana::rectangle(_mui->options().form_x(), _mui->options().form_y(), _mui->options().form_width(), _mui->options().form_heigth())
		   , appear::decorate<appear::minimize, appear::maximize, appear::sizable>())
	, m_system(_system)
	, m_options(_options)
	, m_latest_machine(_mui->options().last_used_machine())
	, m_ui(_mui)
	, m_exename(exename)
{
	
	// Set default scroll scheme
	drawerbase::scroll::scheme m_scroll_scheme;
	m_scroll_scheme.bgrnd = color("#1E1E1E");
	m_scroll_scheme.button = m_scroll_scheme.button_checked = colors::white;
	m_scroll_scheme.flat = true;
	m_scroll_scheme.button_actived = color("#3296AA");
	scroll<true> sc;
	sc.set_default_scheme(m_scroll_scheme);
	sc.use_default_scheme(true);

	
	// Load DATs data
	m_datfile = nullptr;
	auto& plugins = mame_machine_manager::instance()->plugins();
	auto pred = [](core_options::entry &i) { return !i.is_header() && std::string(i.name()) == "data"; };
	auto it = std::find_if(plugins.begin(), plugins.end(), pred);
	if (it != plugins.end() && std::string(it->value()) == "1")
		m_datfile = std::make_unique<datfile_manager>(machine, m_ui->options());

	// Main title
	auto maintitle = string_format("NANAMAME %s", emulator_info::get_bare_build_version());
	this->caption(maintitle);
	this->bgcolor(color("#1E1E1E"));
	if (m_ui->options().form_max()) this->zoom(true); // Maximize


#if defined(NANA_WINDOWS)
	// Tray
	m_notif.icon(m_exename);
	m_notif.text(maintitle);
	m_notif.events().mouse_down([this] {
		msgbox mb{ *this, "Tray Test" };
		mb.icon(mb.icon_information) << "Some msg";
		mb.show();
	});
#endif
	// Main layout
	if (std::string(m_ui->options().form_layout()).empty() || m_ui->options().form_layout_version() != m_layout_version)
		this->div("<vert <weight=25 margin=2 <menu><weight=200 search><weight=25 s_button>><weight=5><<weight=5><weight=200 treebox>|<vert <machinebox>|<vert weight=20% soft <weight=20 swtabf><swtab>>>|<vert weight=400 rpanel <weight=20 tab><tab_frame>><weight=5>><weight=20 statusbar>>");
	else
		this->div(m_ui->options().form_layout());

	// Initialize GUI widgets
	init_context_menu();
	init_filters();
	init_machinebox();
	init_tabbar();
	this->get_place()["statusbar"] << m_statusbar;
	init_menubar();

	// Save default DATs font
	m_font = m_textpage.m_textbox.typeface();

	// Events
	handle_events();

	// Finalize layout
	m_search.tip_string("Search:").multi_lines(false);
	m_search.tooltip("Enter a term to search...");
	m_search.scheme().background = color("#2C2C2C");
	m_search.scheme().foreground = colors::white;
	m_search_button.bgcolor(colors::white);

	paint::image img;
	if (img.open(pic_search, ARRAY_LENGTH(pic_search)))
	{
		m_search_button.icon(img);
	}
	m_search_button.tooltip("Search");
	this->get_place()["search"] << m_search;
	this->get_place()["s_button"] << m_search_button;

	this->collocate();
	auto cat = m_machinebox.at(0);
	if (cat.size() > 0)
	{
		cat.at(m_resel).select(true, true);
		m_machinebox.focus();
		refresh_icons();
	}
	update_selection();
}

void main_form::handle_events()
{
	// Main listbox events
	auto& events = m_machinebox.events();
	events.scrolled([this](const arg_listbox_scroll& arg) {
		m_machinebox.auto_draw(false);
		for (auto &e : arg.item_pairs)
		{
			auto ip = m_machinebox.at(e);
			if (ip.icon()) continue; // icon already loaded
			auto drv = &driver_list::driver(driver_list::find(ip.text(2).c_str()));
			auto icon = load_icon(drv);
			if (!icon.empty())
				ip.icon(icon);
			else
			{
				auto clone = driver_list::non_bios_clone(*drv);
				if (clone != -1)
				{
					drv = &driver_list::driver(clone);
					icon = load_icon(drv);
					if (!icon.empty())
						ip.icon(icon);
				}
			}
		}
		m_machinebox.auto_draw(true);
	});

	events.selected([this](const arg_listbox& arg) {
		if (arg.item.selected())
			update_selection(arg.item);
	});

	events.mouse_down([this](const arg_mouse& arg) {
		// right click, show context menu
		if (arg.right_button)
			menu_popuper(m_context_menu)(arg);
	});

	events.key_release([this](const arg_keyboard& arg) {
		static auto prev_sel = listbox::index_pair();
		auto sel = m_machinebox.selected();
		if (!sel.empty())
		{
			if (arg.key == keyboard::enter || arg.key == keyboard::enter_n)
			{
				auto game = m_machinebox.at(0).at(sel[0].item).text(2);
				start_machine(game);
			}
			else if (sel[0] != prev_sel)
			{
				prev_sel = sel[0];
				auto mm = m_machinebox.at(prev_sel);
				update_selection(mm);
			}
		}
	});

	events.dbl_click([this] {
		auto sel = m_machinebox.selected();
		if (!sel.empty())
		{
			auto game = m_machinebox.at(0).at(sel[0].item).text(2);
			start_machine(game);
		}
	});

	m_swpage.m_softwarebox.events().selected([this](const arg_listbox& arg) {
		load_sw_data(arg.item.text(5), arg.item.text(0), std::string());
	});
		
	// Search events
	m_search_button.events().click([this] { perform_search(); });
	m_search.events().key_char([this](const arg_keyboard& arg) {
		if (arg.key == keyboard::enter || arg.key == keyboard::enter_n)
			perform_search();
	});

	// Filters event
	m_filters.m_treebox.events().selected([this](const arg_treebox& ei) {
		if (!ei.operated) return;
		auto filt = ei.item.text();
		if (ei.item.level() == 1)
		{
			populate_listbox(filt);
		}
		else
		{
			auto ow = ei.item.owner().text();
			populate_listbox(ow, filt);
		}
		update_selection();
		refresh_icons();
	});

	// DATs events
	m_textpage.m_combox.events().selected([this] { update_selection(); });

	// Software events
	m_swpage.m_softwarebox.events().dbl_click([this] { start_software(); });

	// Image events
	m_imgpage.m_combox.events().selected([this] { update_selection(); });

	// Main exit event
	this->events().unload([this](const arg_unload& ei) {
		msgbox mb(*this, "Quit", msgbox::yes_no);
		mb.icon(mb.icon_question) << "Are you sure you want to exit?";
		ei.cancel = mb.show() == mb.pick_no;
		if (!ei.cancel) save_options();
	});
}

void main_form::perform_search()
{
	if (m_search.caption().empty()) return;
	auto text = m_search.caption();
	strmakelower(text);
	for (auto& elem : m_machinebox.at(0))
	{
		auto comp = elem.text(0);
		strmakelower(comp);
		if (comp.find(text) != std::string::npos)
		{
			elem.select(true, true);
			refresh_icons();
			break;
		}
	}
}

void main_form::refresh_icons()
{
	threads::pool_push(m_pool, [&] {
		auto ss = m_machinebox.selected();
		if (ss.empty()) return;
		auto cat = m_machinebox.at(0);
		m_machinebox.auto_draw(false);
		auto sel = ss.at(0).item;
		auto ind = listbox::index_pair{ 0, sel };
		for (auto count = 0; count < 2; ++count)
			for (auto x = sel; count ? x != std::numeric_limits<size_t>::max() : x < cat.size(); count ? --x : ++x)
			{
				ind.item = x;
				auto ip = m_machinebox.at(ind);
				if (!ip.displayed()) break;

				auto drv = &driver_list::driver(driver_list::find(ip.text(2).c_str()));
				auto clone = driver_list::non_bios_clone(*drv);
				auto desc = clone == -1 ? drv->description : driver_list::driver(clone).description;
				auto &parent = m_sortedlist[desc];
				for (auto &e : parent)
				{
					if (e.first != drv) continue;
					if (e.second.empty())
					{
						e.second = load_icon(drv);
						if (!e.second.empty())
						{
							ip.icon(e.second);
						}
						else if (clone != -1)
						{
							drv = &driver_list::driver(clone);
							e.second = load_icon(drv);
							if (!e.second.empty())
							{
								ip.icon(e.second);
							}
						}
					}
					else
					{
						ip.icon(e.second);
					}
					break;
				}
			}
		m_machinebox.auto_draw(true);
	})();
}


void main_form::save_options()
{
	std::string err_str;
	auto& mui = m_ui->options();
	mui.set_value(MEWUI_FORM_HEIGHT, static_cast<int>(this->size().height), OPTION_PRIORITY_CMDLINE, err_str);
	mui.set_value(MEWUI_FORM_WIDTH, static_cast<int>(this->size().width), OPTION_PRIORITY_CMDLINE, err_str);
	mui.set_value(MEWUI_FORM_X, this->pos().x, OPTION_PRIORITY_CMDLINE, err_str);
	mui.set_value(MEWUI_FORM_Y, this->pos().y, OPTION_PRIORITY_CMDLINE, err_str);
	mui.set_value(MEWUI_FORM_MAX, this->is_zoomed(true), OPTION_PRIORITY_CMDLINE, err_str);
	auto sel = m_machinebox.selected();
	if (!sel.empty())
	{
		auto game = m_machinebox.at(0).at(sel[0].item).text(2);
		mui.set_value(OPTION_LAST_USED_MACHINE, game.c_str(), OPTION_PRIORITY_CMDLINE, err_str);
	}
	auto tsel = m_filters.m_treebox.selected();
	if (tsel.level() == 1)
		mui.set_value(OPTION_LAST_USED_FILTER, m_filters.m_treebox.selected().text().c_str(), OPTION_PRIORITY_CMDLINE, err_str);
	else
		mui.set_value(OPTION_LAST_USED_FILTER, m_filters.m_treebox.selected().owner().text().c_str(), OPTION_PRIORITY_CMDLINE, err_str);
	auto &pl = this->get_place().div();
	mui.set_value(MEWUI_LAYOUT, pl.c_str(), OPTION_PRIORITY_CMDLINE, err_str);
    mui.set_value(MEWUI_LAYOUT_VERSION, m_layout_version, OPTION_PRIORITY_CMDLINE, err_str);
	m_ui->save_ui_options();
}

void main_form::init_filters()
{
    std::set<std::string> manuf, year, src;
	for (auto x = 0; x < driver_list::total(); x++)
	{
		auto drv = &driver_list::driver(x);
		if (drv == &GAME_NAME(___empty)) continue;

		manuf.insert(drv->manufacturer);
		year.insert(drv->year);
		src.insert(core_filename_extract_base(drv->source_file));
	}

	for (auto & m : filters_option_ex)
	{
		auto node = m_filters.m_treebox.insert(m.first, m.first);
		switch (m.second.second)
		{
			case 0: // Manufacturers
				for (auto & e : manuf)
					node.append(e, e);
				break;
			
			case 1: // Years
				for (auto & e : year)
					node.append(e, e);
				break;
			
			case 2: // Sources
				for (auto & e : src)
					node.append(e, e);
				break;
			
			default:
				break;
		}
	}

	auto node = m_filters.m_treebox.find(m_ui->options().last_used_filter());
	if (node.level() == 0)
		node = m_filters.m_treebox.find("All");

	node.select(true);
	this->get_place()["treebox"] << m_filters;
}

void main_form::init_tabbar()
{
	this->get_place()["tab"] << m_tabbar;
	this->get_place()["tab_frame"].fasten(m_textpage).fasten(m_imgpage);
	m_tabbar.append("Images", m_imgpage).append("Info", m_textpage);
	m_tabbar.bgcolor(m_bgcolor);
	for (auto x = 0; x < m_tabbar.length(); ++x)
	{
		m_tabbar.tab_bgcolor(x, m_menubar_color);
		m_tabbar.tab_fgcolor(x, colors::white);
	}
	m_tabbar.activated(0);
	m_tabbar.renderer(tabbar_renderer(m_tabbar.renderer()));

	this->get_place()["swtabf"] << m_tabsw;
	this->get_place()["swtab"].fasten(m_swpage);

	m_tabsw.append("Software Packages", m_swpage);
	m_tabsw.bgcolor(m_bgcolor);
	m_tabsw.tab_bgcolor(0, m_menubar_color);
	m_tabsw.tab_fgcolor(0, colors::white);
	m_tabsw.activated(0);
	m_tabsw.renderer(tabbar_renderer(m_tabsw.renderer()));
}

void main_form::init_machinebox()
{
	// Add column header
	m_machinebox.append_header("Machine", 256);
	m_machinebox.append_header("Status", 48);
	m_machinebox.append_header("Romset");
	m_machinebox.append_header("Manufacturer", 256);
	m_machinebox.append_header("Year");
	m_machinebox.append_header("Source");

	for (auto x = 0; x < driver_list::total(); ++x)
	{
		auto drv = &driver_list::driver(x);
		if (drv == &GAME_NAME(___empty)) continue;

		if (driver_list::non_bios_clone(x) == -1)
			m_sortedlist[drv->description].emplace_back(drv, paint::image());
	}

	for (auto x = 0; x < driver_list::total(); ++x)
	{
		auto drv = &driver_list::driver(x);
		if (drv == &GAME_NAME(___empty)) continue;

		auto id = driver_list::non_bios_clone(x);
		if (id != -1)
			m_sortedlist[driver_list::driver(id).description].emplace_back(drv, paint::image());
	}

	populate_listbox(m_filters.m_treebox.selected().text());

	// Place into the layout
	this->get_place()["machinebox"] << m_machinebox;

	m_machinebox.enable_single(true, true);
	auto &sc = m_machinebox.scheme();
	sc.header_bgcolor = m_menubar_color;
	sc.header_fgcolor = colors::white;
	sc.header_highlighted = sc.item_selected = color("#3296AA");
	sc.item_highlighted = m_bgcolor;
	sc.item_bordered = false;
	m_machinebox.bgcolor(m_bgcolor);
	m_machinebox.borderless(true);
	m_machinebox.always_selected(true);
	m_machinebox.at(0).inline_factory(1, pat::make_factory<status_widget>());
}

void main_form::start_software()
{
	auto sel = m_machinebox.selected();
	if (sel.empty()) return;

	std::string error_string;
	auto game = m_machinebox.at(0).at(sel[0].item).text(2);
	driver_enumerator drivlist(m_options, game.c_str());
	media_auditor auditor(drivlist);
	drivlist.next();

	auto selection = m_swpage.m_softwarebox.selected();
	if (selection.empty()) return;
	auto name = m_swpage.m_softwarebox.at(0).at(selection[0].item).text(1);
	software_list_device_iterator iter(drivlist.config().root_device());
	for (auto& swlistdev : iter)
	{
		auto swinfo = swlistdev.find(name.c_str());
		if (swinfo != nullptr)
		{
			// loop through all parts
			for (const auto& swpart : swinfo->parts())
			{
				// only load compatible software this way
				if (swlistdev.is_compatible(swpart) == SOFTWARE_IS_COMPATIBLE)
				{
					auto image = software_list_device::find_mountable_image(drivlist.config(), swpart);
					if (image != nullptr)
					{
						// audit software
						auto summary = auditor.audit_software(swlistdev.list_name(), swinfo, AUDIT_VALIDATE_FAST);
						if (summary == media_auditor::CORRECT || summary == media_auditor::BEST_AVAILABLE || summary == media_auditor::NONE_NEEDED)
						{
							auto string_list = string_format("%s:%s:%s:%s", swlistdev.list_name(), name, swpart.name(), image->instance_name());
							m_options.set_value(OPTION_SOFTWARENAME, string_list.c_str(), OPTION_PRIORITY_CMDLINE, error_string);
							start_machine(game);
						}
						else
						{
							// otherwise, display an error
							msgbox mb(*this, "Error");
							mb.icon(mb.icon_error) << "The selected software is missing one or more required files.";
							mb.show();
						}
					}
				}
			}
		}
	}
}

void main_form::start_machine(std::string& game)
{
	// audit the game first to see if we're going to work
	driver_enumerator enumerator(m_options, game.c_str());
	enumerator.next();
	media_auditor auditor(enumerator);
	auto summary = auditor.audit_media(AUDIT_VALIDATE_FAST);

	// if everything looks good, schedule the new driver
	if (summary == media_auditor::CORRECT || summary == media_auditor::BEST_AVAILABLE || summary == media_auditor::NONE_NEEDED)
	{
		*m_system = &enumerator.driver();
		save_options();
		API::exit();
	}
	else
	{
		// otherwise, display an error
		msgbox mb(*this, "Error");
		mb.icon(mb.icon_error) << "The selected machine is missing one or more required files.";
		mb.show();
	}
}

void main_form::populate_listbox(const std::string& filter, const std::string& subfilter)
{
	m_machinebox.auto_draw(false);
	if (!m_machinebox.empty())
		m_machinebox.clear(0);

	std::vector<const game_driver*> games;

	auto value_translator = [](const std::vector<nana::listbox::cell>& cells) {
		const game_driver* p = nullptr;
		return p;
	};

	auto cell_translator = [](const game_driver* p) {
		std::vector<nana::listbox::cell> cells;
		auto work = (p->flags & MACHINE_NOT_WORKING) ? "Not Working" : "Working";
		cells.emplace_back(p->description);
		cells.emplace_back(work);
		cells.emplace_back(p->name);
		cells.emplace_back(p->manufacturer);
		cells.emplace_back(p->year);
		cells.emplace_back(core_filename_extract_base(p->source_file));
		color fgcolor{colors::white};
		
		bool cloneof = strcmp(p->parent, "0");
		if (cloneof)
		{
			auto cx = driver_list::find(p->parent);
			if (cx != -1 && (driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) == 0)
				fgcolor = colors::gray;
		}

		for (auto & e : cells)
			e.custom_format = std::make_unique<listbox::cell::format>(color("#232323"), fgcolor);

		return cells;
	};

	auto cat = m_machinebox.at(0);
	auto index = 0;
	for (auto & parent : m_sortedlist)
	{
		for (auto & drv : parent.second)
		{
			auto game = drv.first;
			if (filter != "All")
			{
				if (subfilter.empty())
				{
					if (filter == "Parents" && parent.first != game->description) continue;
					if (filter == "Clones" && parent.first == game->description) continue;
					auto tmp = game->flags;
					if (filters_option_ex.at(filter).first(tmp)) continue;
				}
				else
				{
					switch (filters_option_ex.at(filter).second)
					{
						case 0: if (subfilter != game->manufacturer) continue; break;
						case 1: if (subfilter != game->year) continue; break;
						case 2: if (subfilter != core_filename_extract_base(game->source_file)) continue; break;
						default: break;
					}
				}
			}

			games.push_back(game);
			if (m_latest_machine == game->name) m_resel = index;
			index++;
		}
	}
	cat.model<std::recursive_mutex>(std::move(games), value_translator, cell_translator);
	m_machinebox.auto_draw(true);
	m_latest_machine.clear();
}

paint::image main_form::load_icon(const game_driver* drv) const
{
	paint::image img;
	// get search path
	path_iterator path(m_ui->options().icons_directory());
	std::string curpath;
	std::string searchstr(m_ui->options().icons_directory());

	// iterate over path and add path for zipped formats
	while (path.next(curpath))
		searchstr.append(";").append(curpath.c_str()).append(PATH_SEPARATOR).append("icons");

	emu_file snapfile(std::move(searchstr), OPEN_FLAG_READ);
	std::string fname;
	using osd_err = osd_file::error;
	fname.assign(drv->name).append(".ico");

	auto filerr = snapfile.open(fname);
	if (filerr != osd_err::NONE) return img;

	auto length = static_cast<std::uint32_t>(snapfile.size());
	auto data = new std::uint32_t[length];
	if (snapfile.read(data, length) > 0)
	{
		if (img.open(data, length))
		{
			delete[] data;
			return img;
		}
	}
	delete[] data;
	return img;
}

void main_form::update_selection()
{
	auto sel = m_machinebox.selected();
	if (!sel.empty())
	{
		auto mm = m_machinebox.at(sel[0]);
		update_selection(mm);
	}
	else if (m_machinebox.at(0).size() > 0)
	{
		m_machinebox.at(0).at(0).select(true);
	}
}


void main_form::update_selection(listbox::item_proxy &sel)
{
	auto game = sel.text(2);
	auto drv = &driver_list::driver(driver_list::find(game.c_str()));

	load_image(drv); // Load image
	load_data(drv); // Load data from DATs

	// Update menu item
	m_menubar.at(0).text(0, string_format("&Play %s", drv->description));
	m_context_menu.text(0, string_format("Play %s", drv->description));
	//m_context_menu.text(16, string_format("Properties for %s", core_filename_extract_base(drv->source_file)));

	// Update software
	m_swpage.m_softwarebox.auto_draw(false);
	auto cat = m_swpage.m_softwarebox.at(0);

	if (!m_swpage.m_softwarebox.empty())
		m_swpage.m_softwarebox.clear(0);

	std::unordered_set<std::string> list_map;
	driver_enumerator drivlist(m_options, game.c_str());
	while (drivlist.next())
	{
		for (auto& swlistdev : software_list_device_iterator(drivlist.config().root_device()))
			if (list_map.insert(swlistdev.list_name()).second && !swlistdev.get_info().empty())
				for (const auto& swinfo : swlistdev.get_info())
				{
					auto& part = swinfo.parts().front();
					auto pred = [&part](const std::pair<std::string, std::string>& i) {
						return part.name().find(i.second) != std::string::npos;
					};
					auto it = std::find_if(soft_type.begin(), soft_type.end(), pred);
					std::string part_name = (it != soft_type.end()) ? it->first : "unknown";
					cat.append({ swinfo.longname(), swinfo.shortname(), swinfo.publisher(), swinfo.year(), part_name, swlistdev.list_name() });
					swinfo.parentname().empty() ? cat.back().fgcolor(colors::white) : cat.back().fgcolor(colors::gray);
				}
	}
	m_swpage.m_softwarebox.auto_draw(true);
	m_statusbar.update(m_machinebox.at(0).size()); // Update status bar
}

void main_form::load_image(const game_driver* drv)
{
	auto opt = m_imgpage.m_combox.option();
	std::string addpath, searchstr;
	std::array<std::string, 3> exte = { ".png", ".jpg", ".jpeg" };

	if (opt == 0)
	{
		emu_options moptions;
		searchstr = m_options.value(arts_info[opt].second.c_str());
		addpath = moptions.value(arts_info[opt].second.c_str());
	}
	else
	{
		ui_options moptions;
		searchstr = m_ui->options().value(arts_info[opt].second.c_str());
		addpath = moptions.value(arts_info[opt].second.c_str());
	}

	path_iterator path(searchstr);
	path_iterator path_iter(addpath);
	std::string c_path, curpath;

	// iterate over path and add path for zipped formats
	while (path.next(curpath))
	{
		path_iter.reset();
		while (path_iter.next(c_path))
			searchstr.append(";").append(curpath).append(PATH_SEPARATOR).append(c_path);
	}

	emu_file snapfile(std::move(searchstr), OPEN_FLAG_READ);
	snapfile.set_restrict_to_mediapath(true);
	std::string fname;
	using osd_err = osd_file::error;
	auto filerr = osd_err::FAILURE;

	for (auto& e : exte)
	{
		fname.assign(drv->name).append(e);
		filerr = snapfile.open(fname);
		if (filerr == osd_err::NONE) break;
	}

	if (filerr != osd_err::NONE && driver_list::non_bios_clone(*drv) != -1)
		for (auto& e : exte)
		{
			fname.assign(drv->parent).append(e);
			filerr = snapfile.open(fname);
			if (filerr == osd_err::NONE) break;
		}

	if (filerr == osd_err::NONE)
	{
		auto length = static_cast<std::uint32_t>(snapfile.size());
		auto data = new std::uint32_t[length];
		if (snapfile.read(data, length) > 0)
		{
			m_imgpage.m_picture.show();
			paint::image img;
			if (img.open(data, length))
			{
				m_imgpage.m_picture.load(img);
				m_imgpage.m_picture.stretchable(true);
				m_imgpage.m_picture.align(align::center, align_v::center);
			}
		}
		delete[] data;
	}
	else
		m_imgpage.m_picture.hide();
}

void main_form::load_data(const game_driver* drv)
{
	if (m_datfile == nullptr) return;

	std::string buffer;
	if (m_textpage.m_combox.caption() != "Commands")
		m_datfile->load_data_info(drv, buffer, m_textpage.m_combox.caption());
	else
	{
		std::vector<std::string> m_item;
		m_datfile->command_sub_menu(drv, m_item);
		for (auto& elem : m_item)
		{
			std::string t_buffer;
			buffer.append(elem).append("\n");
			m_datfile->load_command_info(t_buffer, elem);
			if (!t_buffer.empty())
				buffer.append(t_buffer).append("\n");
		}
	}

	if (!buffer.empty())
	{
		m_textpage.m_textbox.typeface(m_font);
		m_textpage.m_textbox.reset(buffer, false);
		m_textpage.m_textbox.line_wrapped(true);
	}
	else
	{
		auto tmp = paint::font(m_font.name(), m_font.size(), true, true);
		m_textpage.m_textbox.typeface(tmp);
		m_textpage.m_textbox.reset("No info available", false);
	}
}

void main_form::load_sw_data(std::string list, std::string name, std::string parent)
{
	if (m_datfile == nullptr) return;

	std::string buffer;
	if (m_datfile->has_software(list, name, parent))
	{
		m_datfile->load_software_info(list, buffer, name, parent);

	}

	if (!buffer.empty())
	{
		m_textpage.m_textbox.typeface(m_font);
		m_textpage.m_textbox.reset(buffer, false);
		m_textpage.m_textbox.line_wrapped(true);
	}
	else
	{
		auto tmp = paint::font(m_font.name(), m_font.size(), true, true);
		m_textpage.m_textbox.typeface(tmp);
		m_textpage.m_textbox.reset("No info available", false);
	}
}
void main_form::init_menubar()
{
	m_menubar.bgcolor(m_menubar_color);
	auto &sc = m_menubar.scheme();
	sc.text_fgcolor = colors::light_grey;
	sc.body_highlight = sc.border_highlight = sc.body_selected = sc.border_selected = color{ "#3296AA" };
	sc.has_corner = false;
	sc.text_selected = sc.text_highlight = colors::white;

	// Initialize menu
	init_file_menu();
	init_view_menu();
	init_options_menu();
	init_help_menu();
}

void main_form::init_file_menu()
{
	auto& menu = m_menubar.push_back("&File");
	menu.append("&Play XXX", [this](menu::item_proxy& ip) {
		auto sel = m_machinebox.selected();
		if (!sel.empty())
		{
			auto game = m_machinebox.at(0).at(sel[0].item).text(2);
			start_machine(game);
		}
	});

/*	menu.append_splitter();
	menu.append("Play and &Record Input", [this](menu::item_proxy& ip) {});
	menu.append("P&layback Input", [this](menu::item_proxy& ip) {});
	menu.append_splitter();
	menu.append("Play and Record &Wave Output", [this](menu::item_proxy& ip) {});
	menu.append("Play and Record &MNG Output", [this](menu::item_proxy& ip) {});
	menu.append("Play and Record &uncompressed AVI Output", [this](menu::item_proxy& ip) {});
	menu.append_splitter();
	menu.append("Loa&d Savestate", [this](menu::item_proxy& ip) {});
	menu.append_splitter();
	menu.append("Pr&opertis", [this](menu::item_proxy& ip) {});
	menu.append_splitter();
	menu.append("Audi&t existing Sets", [this](menu::item_proxy& ip) {});
	menu.append("&Audit All Sets", [this](menu::item_proxy& ip) {}); 
*/
	menu.append_splitter();
	menu.append("E&xit", [this](menu::item_proxy& ip) { this->close(); });
	menu.renderer(menu_renderer(menu.renderer()));
}

void main_form::init_view_menu()
{
	auto& menu = m_menubar.push_back("View");

	for (auto & e: submenu)
	{
		auto item = menu.append(e.first, [this, &e](menu::item_proxy& ip) {
			this->get_place().field_display(e.second.c_str(), ip.checked());
			this->collocate();
		});
		item.check_style(menu::checks::highlight);
		item.checked(this->get_place().field_display(e.second.c_str()));
	}

	menu.renderer(menu_renderer(menu.renderer()));
}

void main_form::init_options_menu()
{
	auto& menu = m_menubar.push_back("Options");
//	menu.append("Interface Options", [this](menu::item_proxy& ip) {});
//	menu.append("Global Machine Options", [this](menu::item_proxy& ip) {});
	menu.append("Directories", [this](menu::item_proxy& ip) {
		dir_form form{ *this, m_options, m_ui };
		form.show();
	});

	menu.append("Plugins", [this](menu::item_proxy& ip) {
		plugin_form form{ *this, m_options };
		form.show();
	});

	menu.append("Filebox", [this](menu::item_proxy& ip) {
		filebox form{ *this, true };
		form.show();
	});

//	menu.append_splitter();
//	menu.append("Machine List Font", [this](menu::item_proxy& ip) {});
//	menu.append("Machine List Clone Color", [this](menu::item_proxy& ip) {});
//	menu.append_splitter();
//	menu.append("Background Image", [this](menu::item_proxy& ip) {});
//	menu.append_splitter();
//	menu.append("Reset to Default", [this](menu::item_proxy& ip) {});
	menu.renderer(menu_renderer(menu.renderer()));
}

void main_form::init_help_menu()
{
	auto& menu = m_menubar.push_back("Help");
	menu.append("About", [this](menu::item_proxy& ip) {
		form fm{ *this, API::make_center(400, 200), appear::decorate<>() };
		fm.caption("About NANAMAME");
		fm.bgcolor(colors::white);
		fm.div("<vert <><weight=128 <weight=128 logo> <vert <label><labelurl> > ><>>");
		label lbl{ fm }, lblurl{ fm };
		lbl.transparent(true);
		lblurl.transparent(true);
		lblurl.format(true);
		lbl.caption(string_format("NANAMAME %s by Dankan1890", emulator_info::get_bare_build_version()));
//		lblurl.caption("<url=\"http://nanapro.org\">Nana</>");
		picture pct{ fm };
		paint::image img;
		img.open(logo_nanamame, ARRAY_LENGTH(logo_nanamame));
		pct.load(img);
		pct.transparent(true);
		fm["logo"] << pct;
		fm["label"] << lbl;
		fm["labelurl"] << lblurl;
		fm.collocate();
		fm.show();
		fm.modality();
	});
	menu.renderer(menu_renderer(menu.renderer()));
}

void main_form::init_context_menu()
{
	m_context_menu.append("Play XXX", [this](menu::item_proxy& ip) {
		auto sel = m_machinebox.selected();
		if (!sel.empty())
		{
			auto game = m_machinebox.at(0).at(sel[0].item).text(2);
			start_machine(game);
		}
	});
	m_context_menu.renderer(menu_renderer(m_context_menu.renderer()));

/*	m_context_menu.append_splitter();
	m_context_menu.append("Play and Record Input", [this](menu::item_proxy& ip) {});
	m_context_menu.append_splitter();
	m_context_menu.append("Add to Custom Folder", [this](menu::item_proxy& ip) {});
	auto item = m_context_menu.append("Remove from this Folder", [this](menu::item_proxy& ip) {});
	item.enabled(false);
	m_context_menu.append("Custom Filter", [this](menu::item_proxy& ip) {});
	m_context_menu.append_splitter();
	m_context_menu.append("Select Random Machine", [this](menu::item_proxy& ip) {});
	m_context_menu.append_splitter();
	m_context_menu.append("Reset Play Count", [this](menu::item_proxy& ip) {});
	m_context_menu.append("Reset Play Time", [this](menu::item_proxy& ip) {});
	m_context_menu.append_splitter();
	m_context_menu.append("Audit", [this](menu::item_proxy& ip) {});
	m_context_menu.append_splitter();
	m_context_menu.append("Properties", [this](menu::item_proxy& ip) {});
	m_context_menu.append("Properties for XXX.cpp", [this](menu::item_proxy& ip) {});
	m_context_menu.append("Vector Properties", [this](menu::item_proxy& ip) {}); 
*/
}

tab_page_picturebox::tab_page_picturebox(window wd)
	: panel<true>(wd)
{
	this->bgcolor(color("#2C2C2C"));
	m_picture.bgcolor(color("#232323"));
	for (auto& elem : arts_info)
		m_combox.push_back(elem.first);

	m_combox.option(0);
	m_combox.renderer(&m_cir);
	m_combox.textbox_colors(color("#2C2C2C"), color("#2C2C2C"));
	m_combox.bgcolor(color("#1E1E1E"));
	m_combox.scheme().foreground = colors::white;
	m_combox.scheme().button_color = color("#2C2C2C");

	// Layout
	m_place.div("vert margin=5<weight=20 combobox><weight=5><pct>");
	m_place["combobox"] << m_combox;
	m_place["pct"] << m_picture;
}

tab_page_softwarebox::tab_page_softwarebox(window wd)
	: panel<true>(wd)
{
	this->bgcolor(color("#232323"));
	m_softwarebox.append_header("Name", 180);
	m_softwarebox.append_header("Set", 70);
	m_softwarebox.append_header("Publisher", 180);
	m_softwarebox.append_header("Year", 70);
	m_softwarebox.append_header("Type", 70);
	m_softwarebox.append_header("List", 180);

	// Layout
	m_place.div("<swbox>");
	m_place["swbox"] << m_softwarebox;
	auto &sc = m_softwarebox.scheme();
	sc.header_bgcolor = color("#2C2C2C");
	sc.header_fgcolor = colors::white;
	sc.header_highlighted = sc.item_selected = color("#3296AA");
	sc.item_highlighted = color("#232323");
	sc.item_bordered = false;
	m_softwarebox.bgcolor(sc.item_highlighted);
	m_softwarebox.borderless(true);
}

tab_page_textbox::tab_page_textbox(window wd)
	: panel<true>(wd)
{
	this->bgcolor(color("#232323"));
	m_textbox.editable(false);
	m_textbox.multi_lines(true);
	m_textbox.focus_behavior(widgets::skeletons::text_focus_behavior::none);
	m_textbox.bgcolor(color("#2C2C2C"));
	m_textbox.fgcolor(colors::white);

	m_combox.push_back("History").push_back("MameInfo").push_back("SysInfo").push_back("MessInfo");
	m_combox.push_back("Commands").push_back("Machine Init").push_back("MameScore");
	m_combox.option(0);
	m_combox.renderer(&m_cir);
	m_combox.textbox_colors(color("#2C2C2C"), color("#2C2C2C"));
	m_combox.bgcolor(color("#1E1E1E"));
	m_combox.scheme().foreground = colors::white;
	m_combox.scheme().button_color = color("#2C2C2C");

	// Layout
	m_place.div("vert margin=5 <weight=20 combobox><weight=5><txt>");
	m_place["combobox"] << m_combox;
	m_place["txt"] << m_textbox;
}

panel_filter::panel_filter(window wd)
	: panel<true>(wd)
{
	// Layout
	m_place.div("<vert <weight=20 title><treebox>>");
	m_place["title"] << m_title;
	m_place["treebox"] << m_treebox;

	m_title.bgcolor(color("#2C2C2C"));
	m_title.fgcolor(colors::white);
	m_title.text_align(align::left, align_v::center);
	m_treebox.renderer(treebox_renderer(m_treebox.renderer()));
	m_treebox.placer(treebox_placer(m_treebox.placer()));
	m_treebox.bgcolor(color("#232323"));
}

statusbar::statusbar(window wd)
	: panel<true>(wd)
{
	auto bgcolor = color("#3296AA");
	this->bgcolor(bgcolor);
	m_machines.bgcolor(bgcolor);
	m_machines.fgcolor(colors::white);

	// Layout
	m_place.div("<margin=[0,5] machines>");
	m_place["machines"] << m_machines;
}

void statusbar::update(int m)
{
	m_machines.caption(string_format("Machines %d / %d", m, driver_list::total() - 1));
}

} // namespace nanamame
