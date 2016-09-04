#pragma once

#ifndef MEWUI_COMBO_H
#define MEWUI_COMBO_H
#include <string>
#include <vector>

namespace mewui
{
	class combo
	{
	public:
		combo();

	private:
		std::vector<std::string> m_items;
	};
}
#endif /* MEWUI_COMBO_H */