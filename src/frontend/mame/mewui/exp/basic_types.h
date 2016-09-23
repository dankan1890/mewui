#pragma once

#ifndef EXP_BASIC_TYPES_H
#define EXP_BASIC_TYPES_H

#include <vector>
#include <memory>

namespace ui {
class rectangle
{
public:
	// construction/destruction
	rectangle()
		: min_x(0), max_x(0), min_y(0), max_y(0)
	{}
	rectangle(float minx, float maxx, float miny, float maxy)
		: min_x(minx), max_x(maxx), min_y(miny), max_y(maxy)
	{}

	// getters
	float left() const { return min_x; }
	float right() const { return max_x; }
	float top() const { return min_y; }
	float bottom() const { return max_y; }

	// compute intersection with another rect
	rectangle &operator&=(const rectangle &src)
	{
		if (src.min_x > min_x) min_x = src.min_x;
		if (src.max_x < max_x) max_x = src.max_x;
		if (src.min_y > min_y) min_y = src.min_y;
		if (src.max_y < max_y) max_y = src.max_y;
		return *this;
	}

	// compute union with another rect
	rectangle &operator|=(const rectangle &src)
	{
		if (src.min_x < min_x) min_x = src.min_x;
		if (src.max_x > max_x) max_x = src.max_x;
		if (src.min_y < min_y) min_y = src.min_y;
		if (src.max_y > max_y) max_y = src.max_y;
		return *this;
	}

	// comparisons
	bool operator==(const rectangle &rhs) const { return min_x == rhs.min_x && max_x == rhs.max_x && min_y == rhs.min_y && max_y == rhs.max_y; }
	bool operator!=(const rectangle &rhs) const { return min_x != rhs.min_x || max_x != rhs.max_x || min_y != rhs.min_y || max_y != rhs.max_y; }
	bool operator>(const rectangle &rhs) const { return min_x < rhs.min_x && min_y < rhs.min_y && max_x > rhs.max_x && max_y > rhs.max_y; }
	bool operator>=(const rectangle &rhs) const { return min_x <= rhs.min_x && min_y <= rhs.min_y && max_x >= rhs.max_x && max_y >= rhs.max_y; }
	bool operator<(const rectangle &rhs) const { return min_x >= rhs.min_x || min_y >= rhs.min_y || max_x <= rhs.max_x || max_y <= rhs.max_y; }
	bool operator<=(const rectangle &rhs) const { return min_x > rhs.min_x || min_y > rhs.min_y || max_x < rhs.max_x || max_y < rhs.max_y; }

	// other helpers
	bool empty() const { return min_x > max_x || min_y > max_y; }
	bool contains(float x, float y) const { return x >= min_x && x <= max_x && y >= min_y && y <= max_y; }
	bool contains(const rectangle &rect) const { return min_x <= rect.min_x && max_x >= rect.max_x && min_y <= rect.min_y && max_y >= rect.max_y; }
	float width() const { return max_x - min_x; }
	float height() const { return max_y - min_y; }
	float xcenter() const { return (min_x + max_x) / 2; }
	float ycenter() const { return (min_y + max_y) / 2; }

	// setters
	void set(float minx, float maxx, float miny, float maxy) { min_x = minx; max_x = maxx; min_y = miny; max_y = maxy; }
	void setx(float minx, float maxx) { min_x = minx; max_x = maxx; }
	void sety(float miny, float maxy) { min_y = miny; max_y = maxy; }
	void set_width(float width) { max_x = min_x + width; }
	void set_height(float height) { max_y = min_y + height; }
	void set_origin(float x, float y) { max_x += x - min_x; max_y += y - min_y; min_x = x; min_y = y; }
	void set_size(float width, float height) { set_width(width); set_height(height); }

	// offset helpers
	void offset(float xdelta, float ydelta) { min_x += xdelta; max_x += xdelta; min_y += ydelta; max_y += ydelta; }
	void offsetx(float delta) { min_x += delta; max_x += delta; }
	void offsety(float delta) { min_y += delta; max_y += delta; }
	void pair_off(float delta) { min_x += delta; max_x -= delta; min_y += delta; max_y -= delta; }
	void pairxy_off(float xdelta, float ydelta) { min_x += xdelta; max_x -= xdelta; min_y += ydelta; max_y -= ydelta; }

private:
	// internal state
	float           min_x;          // minimum X, or left coordinate
	float           max_x;          // maximum X, or right coordinate (inclusive)
	float           min_y;          // minimum Y, or top coordinate
	float           max_y;          // maximum Y, or bottom coordinate (inclusive)
};

class exp_widget
{
public:

	virtual ~exp_widget() {}
	virtual void draw_internal() {}

	using w_container = std::vector<std::shared_ptr<exp_widget>>;

	// widgets processing
	void process_widgets()
	{
		for (auto & e : v_widgets)
			e->draw_internal();
	};

	template<typename T, typename R, typename M>
	T *add_widget(R &rc, M &mui)
	{
		v_widgets.push_back(std::make_shared<T>(rc, mui));
		return static_cast<T*>(v_widgets.back().get());
	}

	// internal widgets vectors
	w_container v_widgets;
};

} // namespace ui

#endif