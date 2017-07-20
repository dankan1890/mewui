// license:BSD-3-Clause
// copyright-holders:Maurizio Petrarota
/*********************************************************************

    ui/mewui.cpp

    ImGui launch menu.

*********************************************************************/

#include "emu.h"
#include "mewui.h"
#include "drivenum.h"
#include "audit.h"
#include "mame.h"
#include "modules/lib/osdobj_common.h"
#include "ui/miscmenu.h"
#include "imgui/imgui.h"
#include "utils.h"
#include "uiinput.h"
#include "ui/auditmenu.h"

namespace ui {

std::vector<const game_driver *> modern_launcher::m_sortedlist;
int modern_launcher::m_isabios = 0;
bool modern_launcher::reselect = false;

//-------------------------------------------------
//  force the game select menu to be visible
//  and inescapable
//-------------------------------------------------

modern_launcher::modern_launcher(mame_ui_manager &mui, render_container &container) : menu(mui, container)
{
	auto &options = downcast<osd_options &>(mui.machine().options());

	ImGuiIO& io = ImGui::GetIO();

	auto font_name = options.debugger_font();
	auto font_size = options.debugger_font_size();

	if(font_size == 0)
		font_size = 12;

	io.Fonts->Clear();
	if(strcmp(font_name, OSDOPTVAL_AUTO) == 0)
		io.Fonts->AddFontDefault();
	else
		io.Fonts->AddFontFromFileTTF(font_name,font_size);
	auto m_font = imguiCreate();
	imguiSetFont(m_font);
//	io.MouseDrawCursor = true; // FIXME: ???

	init_sorted_list();
}

modern_launcher::~modern_launcher()
{
}

void modern_launcher::force_game_select(mame_ui_manager &mui, render_container &container) {
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
	build_list();

}

void modern_launcher::handle()
{
	u32 width = machine().render().ui_target().width();
	u32 height = machine().render().ui_target().height();
	int32_t m_mouse_x, m_mouse_y;
	bool m_mouse_button;
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	machine().ui_input().find_mouse(&m_mouse_x, &m_mouse_y, &m_mouse_button);
	imguiBeginFrame(m_mouse_x, m_mouse_y, static_cast<uint8_t>(m_mouse_button ? IMGUI_MBUT_LEFT : 0),
					0, static_cast<uint16_t>(width), static_cast<uint16_t>(height));
//	ImGui::ShowTestWindow(nullptr);


	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	std::string title = util::string_format("MEWUI %s", bare_build_version);
	if (!ImGui::Begin(title.c_str(), nullptr, window_flags)) {
		ImGui::End();
		return;
	}

	filters_panel();
	ImGui::SameLine(0, 20);
	machines_panel();

	ImGui::End();

	imguiEndFrame();

	int iptkey = IPT_INVALID;
	if (exclusive_input_pressed(iptkey, IPT_UI_CANCEL, 0)) {
		stack_pop();
	}
}

void modern_launcher::machines_panel()
{
	u32 width = machine().render().ui_target().width();

	static int current = 0;
	static bool error = false;
	ImGui::BeginChild("Games", ImVec2(width - 300, 0), true);

	ImGui::Columns(4, "##mycolumns", false);
//	ImGui::PushStyleColor(ImGuiCol_Text, ImColor(0, 0, 200));
	ImGui::Text("Name"); ImGui::NextColumn();
	ImGui::Text("Romset"); ImGui::NextColumn();
	ImGui::Text("Manufacturer"); ImGui::NextColumn();
	ImGui::Text("Year"); ImGui::NextColumn();
//	ImGui::PopStyleColor(1);
	ImGui::Separator();

	for (int i = 0; i < m_displaylist.size(); ++i) {
		auto & e = m_displaylist[i];
		bool cloneof = strcmp(e->parent, "0");
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
					mame_machine_manager::instance()->schedule_new_driver(*e);
					machine().schedule_hard_reset();
					stack_reset();
					reselect = true;
				} else {
					error = true;
				}
			}
		}

		if (current == i && reselect) {
			ImGui::SetScrollHere();
			reselect = false;
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

	if (error) {
		ImGui::OpenPopup("Error!");
		if (ImGui::BeginPopupModal("Error!", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("The selected machine is missing one or more required ROM or CHD images.\n");
			ImGui::Text("Please select a different machine.\n\n");
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120,0))) { error = false; ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}
	}

	ImGui::EndChild();
}


void modern_launcher::filters_panel()
{
	ImGui::BeginChild("Filters", ImVec2(200, 0), true);
	static int selection_mask = (1 << 0);
	int node_clicked = -1;
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize()*3);
	for (int i = 0; i < main_filters::length; ++i) {
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow
										| ImGuiTreeNodeFlags_OpenOnDoubleClick
										| ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
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
							c_mnfct::actual = s;
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
							c_year::actual = s;
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

	if (node_clicked != -1) {
		selection_mask = (1 << node_clicked);

		if (node_clicked != main_filters::actual) {
			main_filters::actual = node_clicked;
			reset(reset_options::SELECT_FIRST);
		}
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void modern_launcher::custom_render(void *selectedref, float top, float bottom, float x, float y, float x2, float y2)
{

}

void modern_launcher::init_sorted_list()
{
	if (!m_sortedlist.empty())
		return;

	// generate full list
	for (int x = 0; x < driver_list::total(); ++x)
	{
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

void modern_launcher::build_list()
{
	for (auto & s_driver: m_sortedlist)
	{
		switch (main_filters::actual)
		{
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
			case FILTER_CLONES:
			{
				bool cloneof = strcmp(s_driver->parent, "0");
				if (cloneof)
				{
					auto cx = driver_list::find(s_driver->parent);
					if (cx != -1 && ((driver_list::driver(cx).flags & MACHINE_IS_BIOS_ROOT) != 0))
						cloneof = false;
				}

				if (main_filters::actual == FILTER_CLONES && cloneof)
					m_displaylist.push_back(s_driver);
				else if (main_filters::actual == FILTER_PARENT && !cloneof)
					m_displaylist.push_back(s_driver);
			}
				break;
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
//				if (!core_stricmp(filter_text, s_driver->year))
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

			case FILTER_MANUFACTURER:
			{
				std::string name = c_mnfct::getname(s_driver->manufacturer);
//				if (!core_stricmp(filter_text, name.c_str()))
					m_displaylist.push_back(s_driver);
			}
				break;
			case FILTER_CHD:
			{
				auto entries = rom_build_entries(s_driver->rom);
				for (const rom_entry &rom : entries)
					if (ROMENTRY_ISREGION(&rom) && ROMREGION_ISDISKDATA(&rom))
					{
						m_displaylist.push_back(s_driver);
						break;
					}
			}
				break;
			case FILTER_NOCHD:
			{
				auto entries = rom_build_entries(s_driver->rom);
				bool found = false;
				for (const rom_entry &rom : entries)
					if (ROMENTRY_ISREGION(&rom) && ROMREGION_ISDISKDATA(&rom))
					{
						found = true;
						break;
					}
				if (!found)
					m_displaylist.push_back(s_driver);
			}
				break;

			default:
				m_displaylist.push_back(s_driver);
				break;

		}
	}
}

} // namespace ui