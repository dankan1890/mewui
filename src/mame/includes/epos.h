// license:BSD-3-Clause
// copyright-holders:Zsolt Vasvari
/*************************************************************************

    Epos games

**************************************************************************/

class epos_state : public driver_device
{
public:
	epos_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_maincpu(*this, "maincpu") { }

	/* memory pointers */
	required_shared_ptr<uint8_t> m_videoram;

	/* video-related */
	uint8_t    m_palette;

	/* misc */
	int      m_counter;
	DECLARE_WRITE8_MEMBER(dealer_decrypt_rom);
	DECLARE_WRITE8_MEMBER(port_1_w);
	DECLARE_WRITE8_MEMBER(write_prtc);
	DECLARE_WRITE8_MEMBER(flip_screen_w);
	DECLARE_DRIVER_INIT(dealer);
	virtual void machine_reset() override;
	DECLARE_MACHINE_START(epos);
	DECLARE_MACHINE_START(dealer);
	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void get_pens( pen_t *pens );
	required_device<cpu_device> m_maincpu;
};
