// license:BSD-3-Clause
// copyright-holders:Lee Ward, Dirk Best, Curt Coder
/*************************************************************************

    Memotech MTX 500, MTX 512 and RS 128

*************************************************************************/

#ifndef MAME_INCLUDES_MTX_H
#define MAME_INCLUDES_MTX_H

#include "imagedev/snapquik.h"
#include "imagedev/cassette.h"
#include "bus/centronics/ctronics.h"
#include "bus/generic/slot.h"
#include "bus/generic/carts.h"
#include "bus/mtx/exp.h"
#include "cpu/z80/z80.h"
#include "machine/z80daisy.h"
#include "machine/z80dart.h"
#include "machine/z80ctc.h"
#include "sound/sn76496.h"
#include "machine/ram.h"
#include "machine/timer.h"


class mtx_state : public driver_device
{
public:
	mtx_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_cassold(0)
		, m_maincpu(*this, "z80")
		, m_sn(*this, "sn76489a")
		, m_z80ctc(*this, "z80ctc")
		, m_z80dart(*this, "z80dart")
		, m_cassette(*this, "cassette")
		, m_centronics(*this, "centronics")
		, m_ram(*this, RAM_TAG)
		, m_exp(*this, "exp")
		, m_extrom(*this, "extrom")
		, m_rompak(*this, "rompak")
	{ }

	void rs128(machine_config &config);
	void mtx500(machine_config &config);
	void mtx512(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	bool m_cassold;
	required_device<z80_device> m_maincpu;
	required_device<sn76489a_device> m_sn;
	required_device<z80ctc_device> m_z80ctc;
	optional_device<z80dart_device> m_z80dart;
	required_device<cassette_image_device> m_cassette;
	required_device<centronics_device> m_centronics;
	required_device<ram_device> m_ram;
	required_device<mtx_exp_slot_device> m_exp;
	required_device<generic_slot_device> m_extrom;
	required_device<generic_slot_device> m_rompak;

	/* keyboard state */
	uint8_t m_key_sense;

	/* video state */
	uint8_t *m_video_ram;
	uint8_t *m_attr_ram;

	/* sound state */
	uint8_t m_sound_latch;

	/* timers */
	device_t *m_cassette_timer;

	int m_centronics_busy;
	int m_centronics_fault;
	int m_centronics_perror;
	int m_centronics_select;

	DECLARE_WRITE8_MEMBER(mtx_subpage_w);
	DECLARE_WRITE8_MEMBER(mtx_bankswitch_w);
	DECLARE_WRITE8_MEMBER(mtx_sound_latch_w);
	DECLARE_WRITE8_MEMBER(mtx_sense_w);
	DECLARE_READ8_MEMBER(mtx_key_lo_r);
	DECLARE_READ8_MEMBER(mtx_key_hi_r);
	DECLARE_WRITE8_MEMBER(hrx_address_w);
	DECLARE_READ8_MEMBER(hrx_data_r);
	DECLARE_WRITE8_MEMBER(hrx_data_w);
	DECLARE_READ8_MEMBER(hrx_attr_r);
	DECLARE_WRITE8_MEMBER(hrx_attr_w);
	TIMER_DEVICE_CALLBACK_MEMBER(ctc_tick);
	TIMER_DEVICE_CALLBACK_MEMBER(cassette_tick);
	DECLARE_WRITE_LINE_MEMBER(ctc_trg1_w);
	DECLARE_WRITE_LINE_MEMBER(ctc_trg2_w);
	DECLARE_WRITE_LINE_MEMBER(mtx_tms9929a_interrupt);
	DECLARE_READ8_MEMBER(mtx_strobe_r);
	DECLARE_READ8_MEMBER(mtx_sound_strobe_r);
	DECLARE_WRITE8_MEMBER(mtx_cst_w);
	DECLARE_WRITE8_MEMBER(mtx_cst_motor_w);
	DECLARE_READ8_MEMBER(mtx_prt_r);
	DECLARE_WRITE_LINE_MEMBER(write_centronics_busy);
	DECLARE_WRITE_LINE_MEMBER(write_centronics_fault);
	DECLARE_WRITE_LINE_MEMBER(write_centronics_perror);
	DECLARE_WRITE_LINE_MEMBER(write_centronics_select);
	void bankswitch(uint8_t data);
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(extrom_load);
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(rompak_load);
	DECLARE_QUICKLOAD_LOAD_MEMBER(quickload_cb);
	DECLARE_SNAPSHOT_LOAD_MEMBER(snapshot_cb);

	void mtx_io(address_map &map);
	void mtx_mem(address_map &map);
	void rs128_io(address_map &map);
};

#endif // MAME_INCLUDES_MTX_H
