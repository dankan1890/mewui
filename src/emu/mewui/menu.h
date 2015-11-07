// license:BSD-3-Clause
// copyright-holders:Dankan1890
/*****************************************
        MEWUI SECTION
*****************************************/
public:
	int  visible_items;
	bool ui_error;

	// mouse handling
	bool mouse_hit, mouse_button;
	render_target *mouse_target;
	INT32 mouse_target_x, mouse_target_y;
	float mouse_x, mouse_y;

	// draw UME box
	void draw_ume_box(float x1, float y1, float x2, float y2);

	// draw toolbar
	void draw_toolbar(render_container *container, float x1, float y1, float x2, float y2, bool software = false);

	// draw left panel
	virtual float draw_left_panel(float x1, float y1, float x2, float y2) { return 0; }

	// draw right panel
	virtual void draw_right_panel(void *selectedref, float origx1, float origy1, float origx2, float origy2) { };

	// draw star
	void draw_star(render_container *container, float x0, float y0);

	// Global initialization
	static void init_mewui(running_machine &machine);

	// get arrows status
	template <typename _T1, typename _T2, typename _T3>
	UINT32 get_arrow_flags(_T1 min, _T2 max, _T3 actual)
	{
		if (max == 0)
			return 0;
		else
			return ((actual <= min) ? MENU_FLAG_RIGHT_ARROW : (actual >= max ? MENU_FLAG_LEFT_ARROW : (MENU_FLAG_LEFT_ARROW | MENU_FLAG_RIGHT_ARROW)));
	}

protected:
	int topline_datsview;      // right box top line
	int top_line;              // main box top line
	int l_sw_hover;
	int l_hover;
	int totallines;

	// draw right box
	float draw_right_box_title(float x1, float y1, float x2, float y2);

	// draw arrow
	void draw_common_arrow(float origx1, float origy1, float origx2, float origy2, int current, int dmin, int dmax, float title);

	void info_arrow(int ub, float origx1, float origx2, float oy1, float line_height, float text_size, float ud_arrow_width);

	// images render
	std::string arts_render_common(float origx1, float origy1, float origx2, float origy2);
	void arts_render_images(bitmap_argb32 *bitmap, float origx1, float origy1, float origx2, float origy2, bool software);

	int visible_lines;        // main box visible lines
	int right_visible_lines;  // right box lines

	static render_texture *snapx_texture;
	static bitmap_argb32 *snapx_bitmap;

private:
	static bitmap_argb32 *no_avail_bitmap, *bgrnd_bitmap, *star_bitmap;
	static bitmap_rgb32 *hilight_main_bitmap;
	static render_texture *hilight_main_texture, *bgrnd_texture, *star_texture;
	static render_texture *icons_texture[];
	static bitmap_argb32 *icons_bitmap[];

	// toolbar
	static render_texture *toolbar_texture[MEWUI_TOOLBAR_BUTTONS];
	static bitmap_argb32 *toolbar_bitmap[MEWUI_TOOLBAR_BUTTONS];
	static render_texture *sw_toolbar_texture[MEWUI_TOOLBAR_BUTTONS];
	static bitmap_argb32 *sw_toolbar_bitmap[MEWUI_TOOLBAR_BUTTONS];

	// draw game list
	void draw_select_game(bool noinput);

	// draw game list
	void draw_palette_menu();

	void get_title_search(std::string &title, std::string &search);

	// handle keys
	void handle_main_keys(UINT32 flags);

	// handle mouse
	void handle_main_events(UINT32 flags);

	void draw_icon(render_container *container, int linenum, void *selectedref, float x1, float y1);
