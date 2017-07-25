// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/*********************************************************************

    ui/mewui.cpp

    ImGui launch menu.

*********************************************************************/

#include "emu.h"
#include "mewui.h"
#include "drivenum.h"
#include "mame.h"
#include "modules/lib/osdobj_common.h"
#include "ui/miscmenu.h"
#include "imgui/imgui.h"
#include "bgfx/3rdparty/ocornut-imgui/imgui_internal.h"
#include "softlist_dev.h"
#include "utils.h"
#include "uiinput.h"
#include "ui/auditmenu.h"
#include "audit.h"

namespace ui {

//-------------------------------------------------
//  force the game select menu to be visible
//  and inescapable
//-------------------------------------------------

modern_launcher::modern_launcher(mame_ui_manager &mui, render_container &container) : menu(mui, container)
{

	imguiCreate();
	init_sorted_list();
}

modern_launcher::~modern_launcher()
{
}

void modern_launcher::force_game_select(mame_ui_manager &mui, render_container &container)
{
	// reset the menu stack
	menu::stack_reset(mui.machine());

	// add the quit entry followed by the game select entry
	menu::stack_push_special_main<menu_quit_game>(mui, container);
	menu::stack_push<modern_launcher>(mui, container);

	// force the menus on
	mui.show_menu();

	// make sure MAME is paused
	mui.machine().pause();
}

void modern_launcher::populate(float &customtop, float &custombottom)
{
	m_displaylist.clear();
	switch (m_current_filter) {
		case FILTER_YEAR:
			if (sub_node_year != -1)
				build_list(c_year::ui[sub_node_year]);
			else
				build_list();
			break;
		case FILTER_MANUFACTURER:
			if (sub_node_manuf != -1)
				build_list(c_mnfct::ui[sub_node_manuf]);
			else
				build_list();
			break;
		default:
			build_list();
			break;
	}
}

void modern_launcher::handle()
{
	u32 width = machine().render().ui_target().width();
	u32 height = machine().render().ui_target().height();

	s32 m_mouse_x, m_mouse_y;
	bool m_mouse_button, m_mouse_rbutton = false;
	static s32 zdelta = 0;

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_MenuBar;

	imguihandle_mouse(m_mouse_x, m_mouse_y, zdelta, m_mouse_button, m_mouse_rbutton);

	imguiBeginFrame(m_mouse_x, m_mouse_y, static_cast<u8>(m_mouse_button ? IMGUI_MBUT_LEFT : 0)
										  | static_cast<u8>(m_mouse_rbutton ? IMGUI_MBUT_RIGHT : 0),
					zdelta, static_cast<u16>(width), static_cast<u16>(height));

	std::string title = util::string_format("MEWUI %s", bare_build_version);

	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	if (!ImGui::Begin(title.c_str(), nullptr, window_flags)) {
		ImGui::End();
		return;
	}

	menubar();
	bool treset = filters_panel();
	if (treset) zdelta = 0;
	ImGui::SameLine(0, 20);
	machines_panel(treset);
	ImGui::End();

	// Test
//	bool opened = true;
//	ImGui::ShowTestWindow(&opened);

	imguiEndFrame();
}

void modern_launcher::machines_panel(bool f_reset)
{
	auto width = ImGui::GetWindowContentRegionWidth();
	// FIXME: get without internal
	ImGuiContext* cc = ImGui::GetCurrentContext();
	auto *wc = cc->CurrentWindow;
	auto height = ImGui::GetWindowHeight() - wc->TitleBarHeight() - wc->MenuBarHeight();

	static int current = 0;
	if (f_reset) current = 0;
	static bool error = false;
	static bool reselect = false;
	static std::string error_text;
	bool launch = false;
	ImGui::BeginChild("Frames", ImVec2(width - 300, 0));
	ImGui::BeginChild("Games", ImVec2(width - 300, height - 220), true);

	ImGui::Columns(4, "##mycolumns", false);
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("Romset"); ImGui::NextColumn();
	ImGui::Text("Manufacturer"); ImGui::NextColumn();
	ImGui::Text("Year"); ImGui::NextColumn();
	ImGui::Separator();

	for (int i = 0; i < m_displaylist.size(); ++i) {
		auto & e = m_displaylist[i];
		bool cloneof = static_cast<bool>(strcmp(e->parent, "0"));
		if (cloneof) {
			auto cx = driver_list::find(e->parent);
			if (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0))
				cloneof = false;
		}

		if (cloneof) {
			ImGui::Indent();
			ImGui::PushStyleColor(ImGuiCol_Text, ImColor(UI_CLONE_COLOR));
		}

		if (ImGui::Selectable(e->type.fullname(), current == i,
							  ImGuiSelectableFlags_AllowDoubleClick
							  | ImGuiSelectableFlags_SpanAllColumns)) {
			current = i;
			if (ImGui::IsMouseDoubleClicked(0)) {
				// audit the game first to see if we're going to work
				driver_enumerator enumerator(machine().options(), *e);
				enumerator.next();
				media_auditor auditor(enumerator);
				media_auditor::summary summary = auditor.audit_media(AUDIT_VALIDATE_FAST);

				// if everything looks good, schedule the new driver
				if (summary == media_auditor::CORRECT || summary == media_auditor::BEST_AVAILABLE ||
					summary == media_auditor::NONE_NEEDED) {
					machine().options().set_system_name(e->name);
					mame_machine_manager::instance()->schedule_new_driver(*e);
					machine().schedule_hard_reset();
					stack_reset();
					launch = true;
				} else {
					error_text = make_error_text(media_auditor::NOTFOUND != summary, auditor, false);
					error = true;
				}
			}
		}

		if (current == i && (reselect || f_reset)) {
			ImGui::SetScrollHere();
		}

		ImGui::NextColumn(); ImGui::Text(e->name);
		ImGui::NextColumn(); ImGui::Text(e->manufacturer);
		ImGui::NextColumn(); ImGui::Text(e->year);
		ImGui::NextColumn();

		if (cloneof) {
			ImGui::Unindent();
			ImGui::PopStyleColor(1);
		}
	}

	ImGui::Columns(1);
	imguihandle_keys(current, m_displaylist.size());

	if (error) { show_error(error_text, error);	}

	ImGui::EndChild(); // Games

	if (software_panel(m_displaylist[current])) { launch = true; }

	reselect = launch;

	ImGui::EndChild(); // Frame
}

void modern_launcher::imguihandle_mouse(s32 &m_mouse_x, s32 &m_mouse_y, s32 &zdelta, bool &m_mouse_button,
										bool &m_mouse_rbutton)
{
	ui_event event = {};
	while (machine().ui_input().pop_event(&event)) {
		switch (event.event_type) {
			case UI_EVENT_MOUSE_WHEEL:
				zdelta += event.zdelta;
				break;
			case UI_EVENT_MOUSE_RDOWN:
				m_mouse_rbutton = true;
				break;
			default:
				break;
		}
	}
	machine().ui_input().find_mouse(&m_mouse_x, &m_mouse_y, &m_mouse_button);
}

void modern_launcher::imguihandle_keys(int &current, int max)
{
	// Global keys
	int iptkey = IPT_INVALID;
	if (exclusive_input_pressed(iptkey, IPT_UI_CANCEL, 0)) {
		stack_pop();
		return;
	}

	if (!ImGui::IsWindowFocused()) return;

	if (exclusive_input_pressed(iptkey, IPT_UI_DOWN, 6)) {
		if (current < max - 1) {
			current++;
			return;
		}
	}

	if (exclusive_input_pressed(iptkey, IPT_UI_UP, 6)) {
		if (current > 0) {
			current--;
		}
	}
}


std::string modern_launcher::make_error_text(bool summary, media_auditor const &auditor, bool software)
{
	std::ostringstream str;
	std::string extra_text = (software) ? "software" : "machine";
	str << util::string_format("The selected %s is missing one or more required ROM or CHD images.\n"
									   "Please select a different %s.\n\n", extra_text, extra_text);
	if (summary) {
		auditor.summarize(nullptr, &str);
		str << "\n";
	}
	return str.str();
}

void modern_launcher::show_error(std::string &error_text, bool &error)
{
	ImGui::OpenPopup("Error!");
	if (ImGui::BeginPopupModal("Error!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text(error_text.c_str());
		ImGui::Separator();
		if (ImGui::Button("OK", ImVec2(120,0))) {
			error = false;
			error_text.clear();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

bool modern_launcher::software_panel(const game_driver *drv)
{
	static bool reselect = false;
	bool launch = false;
	static const game_driver *m_current_driver = nullptr;
	auto width = ImGui::GetWindowContentRegionWidth();
	static int current = 0;
	if (m_current_driver != drv) { m_current_driver = drv ; current = 0; }
	int i = 0;
	static bool error = false;
	static std::string error_text;

	ImGui::BeginChild("Software", ImVec2(width, 200), true);

	ImGui::Columns(4, "##mycolumns", false);
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("Romset"); ImGui::NextColumn();
	ImGui::Text("Manufacturer"); ImGui::NextColumn();
	ImGui::Text("Year"); ImGui::NextColumn();
	ImGui::Separator();

	std::unordered_set<std::string> list_map;
	driver_enumerator drivlist(machine().options(), *drv);
	while (drivlist.next()) {
		for (software_list_device &swlistdev : software_list_device_iterator(drivlist.config()->root_device()))
			if (list_map.insert(swlistdev.list_name()).second)
				if (!swlistdev.get_info().empty()) {
					for (const software_info &swinfo : swlistdev.get_info()) {
						if (ImGui::Selectable(swinfo.longname().c_str(), current == i,
											  ImGuiSelectableFlags_AllowDoubleClick
											  | ImGuiSelectableFlags_SpanAllColumns)) {
							current = i;
							if (ImGui::IsMouseDoubleClicked(0)) {
								media_auditor auditor(drivlist);
								media_auditor::summary const summary = auditor.audit_software(swlistdev.list_name(),
																							  &swinfo,
																							  AUDIT_VALIDATE_FAST);
								if (summary == media_auditor::CORRECT || summary == media_auditor::BEST_AVAILABLE
									|| summary == media_auditor::NONE_NEEDED) {
									machine().options().set_system_name(drv->name);

									ui().machine().options().set_value(OPTION_SOFTWARENAME,
																	   util::string_format("%s:%s",
																						   swlistdev.list_name(),
																						   swinfo.shortname()),
																	   OPTION_PRIORITY_CMDLINE);

									mame_machine_manager::instance()->schedule_new_driver(*drv);
									machine().schedule_hard_reset();
									stack_reset();
									launch = true;
								} else {
									error_text = make_error_text(media_auditor::NOTFOUND != summary, auditor, true);
									error = true;
								}
							}
						}
						ImGui::NextColumn(); ImGui::Text(swinfo.shortname().c_str());
						ImGui::NextColumn(); ImGui::Text(swinfo.publisher().c_str());
						ImGui::NextColumn(); ImGui::Text(swinfo.year().c_str());
						ImGui::NextColumn();

						if (current == i && reselect) {
							ImGui::SetScrollHere();
						}
						i++;
					}
				}
	}
	ImGui::Columns(1);
	imguihandle_keys(current, i);

	if (error) { show_error(error_text, error);	}

	reselect = launch;

	ImGui::EndChild();

	return launch;
}

bool modern_launcher::filters_panel()
{
	bool freset = false;
	ImGui::BeginChild("Filters", ImVec2(200, 0), true);
	static int selection_mask = 0;
	int node_clicked = -1;
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 2);
	for (int i = 0; i < main_filters::length; ++i) {
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
										| ImGuiTreeNodeFlags_OpenOnDoubleClick
										| ((selection_mask == i) ? ImGuiTreeNodeFlags_Selected : 0);
		switch (i) {
			case FILTER_MANUFACTURER: {
				// Node
				static int sele = -1;
				bool node_open = ImGui::TreeNodeEx((void *) (intptr_t) i,
												   node_flags,
												   main_filters::text[i]);
				if (ImGui::IsItemClicked())
					node_clicked = i;
				if (node_open) {
					for (int s = 0; s < c_mnfct::ui.size(); ++s)
						if (ImGui::Selectable(c_mnfct::ui[s].c_str(), sele == s)) {
							sub_node_manuf = s;
							sele = s;
						}
					ImGui::TreePop();
				}
				break;
			}
			case FILTER_YEAR: {
				static int sele = -1;
				bool node_open = ImGui::TreeNodeEx((void *) (intptr_t) i,
												   node_flags,
												   main_filters::text[i]);
				if (ImGui::IsItemClicked())
					node_clicked = i;
				if (node_open) {
					for (int s = 0; s < c_year::ui.size(); ++s)
						if (ImGui::Selectable(c_year::ui[s].c_str(), sele == s)) {
							sub_node_year = s;
							sele = s;
						}
					ImGui::TreePop();
				}
				break;
			}
			default:
				ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags | ImGuiTreeNodeFlags_Leaf
													  | ImGuiTreeNodeFlags_NoTreePushOnOpen,
								  main_filters::text[i]);
				if (ImGui::IsItemClicked())
					node_clicked = i;
				break;

		}
	}

	if (node_clicked != -1) { selection_mask = node_clicked; }

	imguihandle_keys(selection_mask, main_filters::length);

	if (selection_mask != m_current_filter) {
		m_current_filter = static_cast<u16>(selection_mask);
		reset(reset_options::SELECT_FIRST);
		freset = true;
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();

	return freset;
}

void modern_launcher::menubar()
{
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::MenuItem("Exit")) {
				stack_pop();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Options")) {
			if (ImGui::BeginMenu("Performance")) {
				if (ImGui::MenuItem("Auto frame skip", nullptr, true)) {}
				if (ImGui::BeginMenu("Frame skip")) {
					static int fs = 0;
					if (ImGui::VSliderInt("##Frame skip", ImVec2(18,160), &fs, 0, 10)) {}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Throttle", nullptr, true)) {}
				if (ImGui::MenuItem("Sleep", nullptr, true)) {}
				if (ImGui::BeginMenu("Speed")) {
					static float s = 0.5f;
					if (ImGui::VSliderFloat("##Speed", ImVec2(18,160), &s, 0.0f, 1.0f)) {}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem("Refresh speed", nullptr, true)) {}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Artwork")) {
				if (ImGui::MenuItem("Artwork Crop", nullptr, true)) {}
				if (ImGui::MenuItem("Backdrops", nullptr, true)) {}
				if (ImGui::MenuItem("Overlays", nullptr, true)) {}
				if (ImGui::MenuItem("Bezels", nullptr, true)) {}
				if (ImGui::MenuItem("Control Panels", nullptr, true)) {}
				if (ImGui::MenuItem("Marquees", nullptr, true)) {}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void modern_launcher::custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2)
{
}

void modern_launcher::init_sorted_list()
{
	// generate full list
	for (int x = 0; x < driver_list::total(); ++x) {
		const game_driver *driver = &driver_list::driver(x);
		if (driver == &GAME_NAME(___empty))
			continue;
		if (driver->flags & MACHINE_IS_BIOS_ROOT)
			m_isabios++;

		m_sortedlist.push_back(driver);
		c_mnfct::set(driver->manufacturer);
		c_year::set(driver->year);
	}

	for (auto & e : c_mnfct::uimap)
		c_mnfct::ui.emplace_back(e.first);

	// sort manufacturers - years and driver
	std::stable_sort(c_mnfct::ui.begin(), c_mnfct::ui.end());
	std::stable_sort(c_year::ui.begin(), c_year::ui.end());
	std::stable_sort(m_sortedlist.begin(), m_sortedlist.end(), sorted_game_list);
}

void modern_launcher::build_list(const std::string &text)
{
	for (auto & s_driver: m_sortedlist) {
		switch (m_current_filter) {
			case FILTER_WORKING:
				if (!(s_driver->flags & MACHINE_NOT_WORKING))
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_NOT_MECHANICAL:
				if (!(s_driver->flags & MACHINE_MECHANICAL))
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_BIOS:
				if (s_driver->flags & MACHINE_IS_BIOS_ROOT)
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_PARENT:
			case FILTER_CLONES: {
				bool cloneof = static_cast<bool>(strcmp(s_driver->parent, "0"));
				if (cloneof) {
					auto cx = driver_list::find(s_driver->parent);
					if (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0))
						cloneof = false;
				}

				if (m_current_filter == FILTER_CLONES && cloneof)
					m_displaylist.push_back(s_driver);
				else if (m_current_filter == FILTER_PARENT && !cloneof)
					m_displaylist.push_back(s_driver);
				break;
			}
			case FILTER_NOT_WORKING:
				if (s_driver->flags & MACHINE_NOT_WORKING)
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_MECHANICAL:
				if (s_driver->flags & MACHINE_MECHANICAL)
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_SAVE:
				if (s_driver->flags & MACHINE_SUPPORTS_SAVE)
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_NOSAVE:
				if (!(s_driver->flags & MACHINE_SUPPORTS_SAVE))
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_YEAR:
				if (text.empty() || text == s_driver->year)
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_VERTICAL:
				if (s_driver->flags & ORIENTATION_SWAP_XY)
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_HORIZONTAL:
				if (!(s_driver->flags & ORIENTATION_SWAP_XY))
					m_displaylist.push_back(s_driver);
				break;

			case FILTER_MANUFACTURER: {
				if (text.empty()) {
					m_displaylist.push_back(s_driver);
				} else {
					std::string name = c_mnfct::getname(s_driver->manufacturer);
					if (text == name)
						m_displaylist.push_back(s_driver);
				}
				break;
			}
			case FILTER_CHD: {
				auto entries = rom_build_entries(s_driver->rom);
				for (const rom_entry &rom : entries)
					if (ROMENTRY_ISREGION(&rom) && ROMREGION_ISDISKDATA(&rom)) {
						m_displaylist.push_back(s_driver);
						break;
					}
				break;
			}
			case FILTER_NOCHD: {
				auto entries = rom_build_entries(s_driver->rom);
				bool found = false;
				for (const rom_entry &rom : entries)
					if (ROMENTRY_ISREGION(&rom) && ROMREGION_ISDISKDATA(&rom)) {
						found = true;
						break;
					}
				if (!found)
					m_displaylist.push_back(s_driver);
				break;
			}

			default:
				m_displaylist.push_back(s_driver);
				break;
		}
	}
}

} // namespace ui