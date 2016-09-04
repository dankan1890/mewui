#pragma once

#ifndef MEWUI_FORM_H
#define MEWUI_FORM_H
#include "ui/menu.h"

namespace mewui
{
	class form : public ui::menu
	{
	public:
		form(mame_ui_manager& mui, render_container& container)
			: menu(mui, container)
		{}

		~form() override;

	private:
		void populate() override;
		void handle() override;

	};
	
}
#endif /* MEWUI_FORM_H */