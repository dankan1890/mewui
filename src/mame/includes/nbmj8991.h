// license:BSD-3-Clause
// copyright-holders:Takahiro Nogi

#include "machine/nb1413m3.h"
#include "machine/gen_latch.h"
#include "emupal.h"
#include "screen.h"

class nbmj8991_state : public driver_device
{
public:
	nbmj8991_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_nb1413m3(*this, "nb1413m3"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),
		m_soundlatch(*this, "soundlatch"),
		m_generic_paletteram_8(*this, "paletteram") { }

	void nbmjdrv1(machine_config &config);
	void nbmjdrv2(machine_config &config);
	void nbmjdrv3(machine_config &config);
	void tokyogal(machine_config &config);
	void finalbny(machine_config &config);
	void mjlstory(machine_config &config);
	void galkaika(machine_config &config);
	void pstadium(machine_config &config);
	void galkoku(machine_config &config);
	void av2mj2rg(machine_config &config);
	void av2mj1bb(machine_config &config);
	void vanilla(machine_config &config);
	void mcontest(machine_config &config);
	void triplew1(machine_config &config);
	void ntopstar(machine_config &config);
	void tokimbsj(machine_config &config);
	void triplew2(machine_config &config);
	void uchuuai(machine_config &config);
	void hyouban(machine_config &config);
	void qmhayaku(machine_config &config);
	void mjgottub(machine_config &config);

	void init_galkaika();
	void init_tokimbsj();
	void init_tokyogal();
	void init_finalbny();

private:
	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_audiocpu;
	required_device<nb1413m3_device> m_nb1413m3;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	optional_device<generic_latch_8_device> m_soundlatch;

	required_shared_ptr<uint8_t> m_generic_paletteram_8;

	enum
	{
		TIMER_BLITTER
	};

	int m_scrollx;
	int m_scrolly;
	int m_blitter_destx;
	int m_blitter_desty;
	int m_blitter_sizex;
	int m_blitter_sizey;
	int m_blitter_src_addr;
	int m_blitter_direction_x;
	int m_blitter_direction_y;
	int m_gfxrom;
	int m_dispflag;
	int m_flipscreen;
	int m_clutsel;
	int m_screen_refresh;
	bitmap_ind16 m_tmpbitmap;
	std::unique_ptr<uint8_t[]> m_videoram;
	std::unique_ptr<uint8_t[]> m_clut;
	int m_flipscreen_old;
	emu_timer *m_blitter_timer;

	DECLARE_WRITE8_MEMBER(soundbank_w);
	DECLARE_WRITE8_MEMBER(palette_type1_w);
	DECLARE_WRITE8_MEMBER(palette_type2_w);
	DECLARE_WRITE8_MEMBER(palette_type3_w);
	DECLARE_WRITE8_MEMBER(blitter_w);
	DECLARE_READ8_MEMBER(clut_r);
	DECLARE_WRITE8_MEMBER(clut_w);

	virtual void machine_reset() override;
	virtual void video_start() override;

	uint32_t screen_update_type1(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_type2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void vramflip();
	void update_pixel(int x, int y);
	void gfxdraw();

	void postload();

	void av2mj1bb_io_map(address_map &map);
	void av2mj1bb_map(address_map &map);
	void av2mj2rg_map(address_map &map);
	void galkaika_map(address_map &map);
	void galkoku_io_map(address_map &map);
	void galkoku_map(address_map &map);
	void hyouban_io_map(address_map &map);
	void mjlstory_map(address_map &map);
	void nbmj8991_sound_io_map(address_map &map);
	void nbmj8991_sound_map(address_map &map);
	void pstadium_io_map(address_map &map);
	void pstadium_map(address_map &map);
	void tokyogal_map(address_map &map);
	void triplew1_map(address_map &map);
	void triplew2_map(address_map &map);

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
};
