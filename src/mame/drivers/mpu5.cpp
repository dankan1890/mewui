// license:BSD-3-Clause
// copyright-holders:David Haywood
/* MPU5 hardware emulation */

/* This file contains the hardware emulation, the mpu5.c contains the set listings */

/*  Many of the games in here also seem to exist on other hardware.

    MPU5

    Skeleton Driver

     -- there are a wide range of titles running on this hardware

     -- the driver does nothing, and currently only serves to act as a placeholder to document what existed on this hardware

     -- the main CPU is a 68340, which is a 32-bit 680xx variant with modified opcodes etc. (CPU32 core)

     -- Much of the communication is done via a 68681 DUART.

     -- The ASIC acts as the main I/O control, including an interface to a Z89321 DSP and a 16bit DAC.

     -- Help wanted, the MFME sources (which are based on MAME anyway) should be of some help here, if somebody
        in the FM emu community wants to adopt this driver they're welcome to it.

     -- As a result of games being on multiple systems, and some of the original sets being a mess there could be one or two
        out of position here (eg MPU4 video instead of MPU5) or with missing roms if there was extra hardware (nothing has been
        removed from the rom loading comments, so if there were extra roms present they're still commented)

        Some duplicate roms have been commented out for now, please don't remove these lines until the sets are properly sorted.

        Some games weren't even in the right zips, Eg the Red Hot Fever (MPU4) cotnained a mislabled MPU5 'Raise The Roof' set
        with extra roms, probably actually from the MPU4 Red Hot Fever.  The game names are usually stored somewhat as plain
        ASCII so spotting such problems is easy enough.

        In general things have been added here if the rom structure and initial code looks like the MPU5 boot code


    15/07/11 - rom loading for most games added, still some missing tho and clones still need sorting out properly.
*/

/* BARCREST MPU5 XP53922-v1
  ______________________________________________________________________
  | _________  _______________ ____________________                 __  |
  | |__SW1___| ||||BUTTONS|||| ||LAMPS_1|||||||||||                 | | |
  |           ________________ ____________________                 | | |
  |           |||||PAYOUT||||| ||LAMPS_2|||||||||||                 | | |
  | __                                                              |L| |
  | |S|                                                             |E| |
  | |W|                                                             |D| |
  | |I|                                                             |S| |
  | |T|                                                             | | |
  | |_|                                                             | | |
  | __                                                              | | |
  | |D| _________  _________  _________  _________   _________      |_| |
  | |A| |4116R LF| TPIC6B273N TPIC6B273N TPIC6B273N  |4116R LF|     __  |
  | |T| _________  _________  _________  __________  _________      |M| |
  | |A| |4116R LF| M74HC257B1 M74HC257B1 |ATMEL    | TPIC6B273N     |E| |
  | |P|            ____________________  |BARCREST | _________      |T| |
  | |_| _________  |BARCREST 881687    | |881685 X | TPIC6B273N     |E| |
  | __  |CNY74-4 | |_0605_FU___________| |_0544____|                |R| |
  | |C|              ________________________________               |S| |
  | |O|              |.....CON1 (to game cart).......|              | | |
  | |I|              |_______________________________|              |_| |
  | |_| _________      _________  _________  _________              __  |
  | __  |MC1489PG      |74ACT541  |74ACT541  |74ACT541              |B| |
  | |A| _________  ____________   _________  _________              |A| |
  | |U| |MC1488P|  |_GAL20V8B__|  |74ACT541  |74ACT541              |R| |
  | |X|                 XTAL1                                       |B| |
  | |_|            ________________     _______________             |U| |
  | __             | SCC68681C1N40 |    |BS62LV1027PCP55  __        |_| |
  | |A|            |__NXP__________|    |__BSI_________|SN75176BP F     |
  | |U|                                 _______________           U  o -> LED -12
  | |X|                                 |BS62LV1027PCP55          S  o -> LED +12
  | |P|                  __________     |__BSI_________|          E  o -> LED +24
  | |O|                  |MC68340AG16E                               o -> LED + +34
  | |R|                  |L75M     |                              F     |
  | |T|                  |QGR0633  |   ______     __              U  o -> LED STATUS
  | |_|                  |_________|   M74HC08N MS6610BSG         S  o -> LED HALT
  | __      ____   __                                             E __  |
  | |A|     BATT  SWBAT                                             |A| |
  | |L|                                                             |U| |
  | |P|  ______________                                        ___  |D| |
  | |_|  |||VEND-BUS|||                                       |MIC  |_| |
  |_____________________________________________________________________|

SW1 = 8 dipswitches
SWBAT = Single jumper for battery

XTAL1 = 3.6864MHz
MIC = Microphone

SWIT = SWITCHES
DATAP = DATAPORT (female DB25)
COI = COINS
AUX = AUX-RS232
ALP = ALPHA
AUD = AUDIO
BARBU = BARBUS

Buttons    Payout      Lamps 1    Lamps 2     Leds          Meters      Audio
 1 - H3     1 - PAY0    1 - H0     1 \         1 - C7 \ C    1 - 0V      1 - R+
 2 - H2     2 - PAY1    2 - H1     2 |         2 - C6 | A    2 - GAP     2 - L-
 3 - H1     3 - PAY2    3 - H2     3 | HIGH    3 - C5 | T    3 - M0      3 - R-
 4 - H0     4 - PAY3    4 - H3     4 | SIDE    4 - C4 | H    4 - M1      4 - L+
 5 - GAP    5 - 0V      5 - H4     5 | 34V     5 - C3 | O    5 - M2      5 - 0V
 6 - L3     6 - GAP     6 - H5     6 |         6 - C2 | D    6 - M3      6 - GAP
 7 - L2     7 - +24V    7 - GAP    7 |         7 - C1 | E    7 - M4      7 - RIN
 8 - L1     8 - IN0     8 - H6     8 |         8 - C0 / S    8 - M5      8 - LIN
 9 - L0     9 - IN1     9 - H7     9 /         9 - A7 \      9 - M6      9 - MIC
10 - SW0   10 - IN2    10 - L0    10 \        10 - A6 | A   10 - M7
11 - SW1   11 - IN3    11 - L1    11 |        11 - A5 | N   11 \ SENSE
12 - SW2   12 - +12V   12 - L2    12 | LOW    12 - A4 | O   12 /
13 - SW3   13 - +5V    13 - L3    13 | SIDE   13 - GAP| D   13 +12
           14 - 0V     14 - L4    14 | 0V     14 - A3 | E
                       15 - L5    15 |        15 - A2 | S
                       16 - L6    16 |        16 - A1 |
                       17 - L7    17 /        17 - A0 /

Switches   Aux-RS232   Aux Port   Alpha      Vend-Bus
 1 - SW0    1 - +12     1 - AX7    1 - CLK    1 - -34
 2 - GAP    2 - TX      2 - AX6    2 - D      2 - +12
 3 - SW1    3 - RX      3 - AX5    3 - RES    3 - GAP
 4 - SW2    4 - RTS     4 - AX4    4 - 0V     4 - TX \
 5 - SW3    5 - CTS     5 - AX3    5 - GAP    5 - RX / RS232
 6 - L7     6 - GAP     6 - AX2    6 - 0V     6 - RX \
 7 - L6     7 - 0V      7 - AX1    7 - +12    7 - TX / VEND
 8 - L5                 8 - AX0               8 - TX - O.C.
 9 - L4                 9 - 11                9 - 0V
                       10 - 10
                       11 - ST
                       12 - +5
                       13 - +12
                       14 - GAP
                       15 - 0V
*/


#include "emu.h"

#include "machine/68340.h"
#include "machine/sec.h"
#include "speaker.h"

#include "mpu5.lh"

// MFME2MAME layouts:
#include "m5addams.lh"
#include "m5all41d.lh"
#include "m5arab.lh"
#include "m5austin11.lh"
#include "m5barkng.lh"
#include "m5barmy.lh"
#include "m5baxe04.lh"
#include "m5bbro.lh"
#include "m5bbrocl.lh"
#include "m5beansa.lh"
#include "m5bigchs.lh"
#include "m5biggam.lh"
#include "m5bling.lh"
#include "m5blkwht11.lh"
#include "m5bnzclb.lh"
#include "m5btlbnk.lh"
#include "m5bttf.lh"
#include "m5bwaves.lh"
#include "m5carou.lh"
#include "m5cashat.lh"
#include "m5cashrn.lh"
#include "m5cbw.lh"
#include "m5centcl.lh"
#include "m5circlb33.lh"
#include "m5circus0a.lh"
#include "m5clifhn.lh"
#include "m5clown11.lh"
#include "m5codft.lh"
#include "m5cosclb.lh"
#include "m5crzkni.lh"
#include "m5cshkcb.lh"
#include "m5cshstx.lh"
#include "m5dblqtsb.lh"
#include "m5devil.lh"
#include "m5dick10.lh"
#include "m5doshpk05.lh"
#include "m5egr.lh"
#include "m5elband.lh"
#include "m5elim.lh"
#include "m5evgrhr.lh"
#include "m5ewn.lh"
#include "m5extrm.lh"
#include "m5fiddle.lh"
#include "m5fire.lh"
#include "m5firebl.lh"
#include "m5flipcr.lh"
#include "m5fortby.lh"
#include "m5frnzy.lh"
#include "m5funsun.lh"
#include "m5gdrag.lh"
#include "m5ggems20.lh"
#include "m5gimmie.lh"
#include "m5grush.lh"
#include "m5grush5.lh"
#include "m5gsstrk07.lh"
#include "m5gstrik.lh"
#include "m5hellrz.lh"
#include "m5hgl14.lh"
#include "m5hiclau.lh"
#include "m5hifly.lh"
#include "m5hilok.lh"
#include "m5hisprt.lh"
#include "m5hlsumo.lh"
#include "m5holy.lh"
#include "m5hopidl.lh"
#include "m5hotslt.lh"
#include "m5hotstf.lh"
#include "m5hypvip.lh"
#include "m5jackbx.lh"
#include "m5jackp2.lh"
#include "m5jackpt.lh"
#include "m5jlyjwl.lh"
#include "m5jmpgem01.lh"
#include "m5kingqc06.lh"
#include "m5kkebab.lh"
#include "m5korma.lh"
#include "m5loony.lh"
#include "m5loot.lh"
#include "m5lotta.lh"
#include "m5martns07.lh"
#include "m5mega.lh"
#include "m5mmak06.lh"
#include "m5monmst.lh"
#include "m5mpfc.lh"
#include "m5mprio.lh"
#include "m5neptun.lh"
#include "m5nnww.lh"
#include "m5oohaah.lh"
#include "m5oohrio.lh"
#include "m5openbx05.lh"
#include "m5overld.lh"
#include "m5peepsh.lh"
#include "m5piefac.lh"
#include "m5piefcr.lh"
#include "m5ppussy.lh"
#include "m5psyccl01.lh"
#include "m5psycho.lh"
#include "m5ptyani.lh"
#include "m5qdrawb.lh"
#include "m5qshot04.lh"
#include "m5ratpka.lh"
#include "m5razdz10.lh"
#include "m5redbal.lh"
#include "m5redrcka.lh"
#include "m5resfrg.lh"
#include "m5revo13.lh"
#include "m5rfymc.lh"
#include "m5rgclb12.lh"
#include "m5rhrgt02.lh"
#include "m5ritj.lh"
#include "m5rollup.lh"
#include "m5rollx.lh"
#include "m5rthh.lh"
#include "m5rub.lh"
#include "m5rwb.lh"
#include "m5scharg.lh"
#include "m5seven.lh"
#include "m5shark.lh"
#include "m5sheik.lh"
#include "m5skulcl20.lh"
#include "m5sondra.lh"
#include "m5speccl.lh"
#include "m5spiker.lh"
#include "m5spins.lh"
#include "m5squids06.lh"
#include "m5sstrk.lh"
#include "m5starcl.lh"
#include "m5stars26.lh"
#include "m5stax.lh"
#include "m5supnov.lh"
#include "m5supro.lh"
#include "m5tbird.lh"
#include "m5tempcl.lh"
#include "m5tempp.lh"
#include "m5tempt2.lh"
#include "m5tictacbwb.lh"
#include "m5trail.lh"
#include "m5ultimo04.lh"
#include "m5upover.lh"
#include "m5vampup.lh"
#include "m5vertgo.lh"
#include "m5wking05.lh"
#include "m5wonga.lh"
#include "m5wthing20.lh"
#include "m5xchn.lh"
#include "m5xfact11.lh"


class mpu5_state : public driver_device
{
public:
	mpu5_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu")
	{ }

	void mpu5(machine_config &config);

private:
	DECLARE_READ32_MEMBER(mpu5_mem_r);
	DECLARE_WRITE32_MEMBER(mpu5_mem_w);

	DECLARE_READ32_MEMBER(asic_r32);
	DECLARE_READ8_MEMBER(asic_r8);
	DECLARE_WRITE32_MEMBER(asic_w32);
	DECLARE_WRITE8_MEMBER(asic_w8);

	DECLARE_READ32_MEMBER(pic_r);
	DECLARE_WRITE32_MEMBER(pic_w);

	virtual void machine_start() override;
	void mpu5_map(address_map &map);

	uint32_t* m_cpuregion;
	std::unique_ptr<uint32_t[]> m_mainram;
	SEC sec;

	uint8_t m_led_strobe_temp;
	uint8_t m_led_strobe;
	uint8_t m_pic_clk;
	bool  m_pic_transfer_in_progress;
	uint8_t m_pic_bit1;
	uint8_t m_pic_data;
	uint8_t m_pic_clocked_bits;
	uint8_t m_pic_stored_input;
	uint8_t m_pic_output_bit;
	uint8_t m_input_strobe;

	// devices
	required_device<m68340_cpu_device> m_maincpu;
};

READ8_MEMBER(mpu5_state::asic_r8)
{
	switch (offset)
	{
		case 0x01:
		{
			return 0x99;
		}

		case 0x02:
		{
			//send init and ready for now - need to work on full DSP
			return 0x85;
		}

		case 0x0b:
		{
			return 0;
		}
		default:
		{
			int pc = m_maincpu->pc();
			logerror("%08x maincpu read from ASIC - offset %01x\n", pc, offset);
			return 0;
		}
	}
}


READ32_MEMBER(mpu5_state::asic_r32)
{
	uint32_t retdata = 0;
	if (ACCESSING_BITS_24_31) retdata |= asic_r8(space,(offset*4)+0) <<24;
	if (ACCESSING_BITS_16_23) retdata |= asic_r8(space,(offset*4)+1) <<16;
	if (ACCESSING_BITS_8_15) retdata |= asic_r8(space,(offset*4)+2) <<8;
	if (ACCESSING_BITS_0_7) retdata |= asic_r8(space,(offset*4)+3) <<0;
	return retdata;
}

READ32_MEMBER(mpu5_state::mpu5_mem_r)
{
	int pc = m_maincpu->pc();
	int addr = offset *4;
	int cs = m_maincpu->get_cs(addr);

	switch ( cs )
	{
		case 2:
		{
			switch (addr & 0xf0)
			{
				case 0xd0:
				{
					logerror("%08x PIC read\n", pc);
					break;
				}
				case 0xe0:
				{
					logerror("%08x DUART read\n", pc);
					break;
				}

				case 0xf0:
				{
					return asic_r32(space, offset&3,mem_mask);
				}

				default:
				logerror("%08x maincpu read access offset %08x mem_mask %08x cs %d\n", pc, offset*4, mem_mask, cs);
				break;
			}
		}
		break;

		case 3:
		case 4:
			offset &=0x3fff;
			return (m_mainram[offset]);

		case 1:if (offset < 0x100000) // make sure to log an error instead of crashing when reading beyond end of region
			return m_cpuregion[offset];


		default:
			logerror("%08x maincpu read access offset %08x mem_mask %08x cs %d\n", pc, offset*4, mem_mask, cs);

	}

	return 0x0000;
}

// Each board is fitted with an ASIC that does most of the heavy lifting, including sound playback.
WRITE8_MEMBER(mpu5_state::asic_w8)
{
	switch (offset)
	{
		case 0x03:
		{
			if (m_led_strobe_temp != data)
			{
				m_led_strobe_temp = data;

				switch (m_led_strobe_temp)
				{
					case 0x00:
					m_led_strobe = 0;
					break;
					case 0x01:
					m_led_strobe = 1;
					break;
					case 0x02:
					m_led_strobe = 2;
					break;
					case 0x04:
					m_led_strobe = 3;
					break;
					case 0x08:
					m_led_strobe = 4;
					break;
					case 0x10:
					m_led_strobe = 5;
					break;
					case 0x20:
					m_led_strobe = 6;
					break;
					case 0x40:
					m_led_strobe = 7;
					break;
					case 0x80:
					m_led_strobe = 8;
					break;
				}
			}
			break;
		}

		case 0x09:
		{
			//Assume SEC fitted for now
			sec.write_data_line(~data&0x01);
			sec.write_clock_line(~data&0x02);
			sec.write_cs_line(~data&0x04);
		}
		case 0x0b:
		{
			output().set_value("statuslamp1", ((data&0x10) != 0));

			output().set_value("statuslamp2", ((data&0x20) != 0));

			if (data & 0x40)
			{
//              m_dsp_pin =1;
			}
		}
		break;
		default:
		{
			int pc = m_maincpu->pc();
			logerror("%08x maincpu write to ASIC - offset %01x data %02x\n", pc, offset, data);
		}
	}
}


WRITE32_MEMBER(mpu5_state::asic_w32)
{
	if (ACCESSING_BITS_24_31) asic_w8(space,(offset*4)+0, (data>>24)&0xff);
	if (ACCESSING_BITS_16_23) asic_w8(space,(offset*4)+1, (data>>16)&0xff);
	if (ACCESSING_BITS_8_15) asic_w8(space,(offset*4)+2, (data>>8) &0xff);
	if (ACCESSING_BITS_0_7) asic_w8(space,(offset*4)+3, (data>>0) &0xff);
}


READ32_MEMBER(mpu5_state::pic_r)
{
	int pc = m_maincpu->pc();
	logerror("%08x maincpu read from PIC - offset %01x\n", pc, offset);
	return m_pic_output_bit;
}

WRITE32_MEMBER(mpu5_state::pic_w)
{
	switch (offset)
	{
		case 0x04:
		{
			if (m_pic_clk)
			{
				m_pic_transfer_in_progress = true;
			}
			m_pic_bit1 = (data & 0x01);
			break;
		}

		case 0x06:
		case 0x07:
		{
			if ( (!data) && (m_pic_transfer_in_progress) && (m_pic_clk))
			{
				//clock in the stored bit (rudimentary protection here)
				m_pic_data = (m_pic_data << 1);
				m_pic_data |= m_pic_bit1;
				m_pic_stored_input <<= 1;
				m_pic_clocked_bits ++;

				if (m_pic_clocked_bits >=8)
				{
					m_pic_data =0;
					m_pic_clocked_bits =0;

					if (m_input_strobe <4)
					{
						m_input_strobe +=1;
					}

				}
			}
			else
			{
				m_pic_output_bit = BIT(m_pic_stored_input,7);
			}
			m_pic_transfer_in_progress = false;
			m_pic_clk = (data != 0);
			break;
		}
		default:
		{
			int pc = m_maincpu->pc();
			logerror("%08x maincpu write to PIC - offset %01x data %02x\n", pc, offset, data);
			break;
		}
	}

}

WRITE32_MEMBER(mpu5_state::mpu5_mem_w)
{
	int pc = m_maincpu->pc();
	int addr = offset *4;
	int cs = m_maincpu->get_cs(addr);

	switch ( cs )
	{
		case 2:
		{
			switch (addr & 0xf0)
			{
				case 0xd0:
				{
					pic_w(space, (addr& 0x0f),data,mem_mask);
					break;
				}
				case 0xe0:
				{
					logerror("%08x DUART write\n", pc);
					break;
				}

				case 0xf0:
				{
					asic_w32(space, offset&3,data,mem_mask);
					break;
				}

				default:
					logerror("%08x maincpu write access offset %08x data %08x mem_mask %08x cs %d\n", pc, offset*4, data, mem_mask, cs);
				break;
			}
		}
		break;

		case 3:
		case 4:
			offset &=0x3fff;
			COMBINE_DATA(&m_mainram[offset]);
			break;



		default:
			logerror("%08x maincpu write access offset %08x data %08x mem_mask %08x cs %d\n", pc, offset*4, data, mem_mask, cs);

	}

}

void mpu5_state::mpu5_map(address_map &map)
{
	map(0x00000000, 0xffffffff).rw(FUNC(mpu5_state::mpu5_mem_r), FUNC(mpu5_state::mpu5_mem_w));
}

INPUT_PORTS_START(  mpu5 )
INPUT_PORTS_END


void mpu5_state::machine_start()
{
	m_cpuregion = (uint32_t*)memregion( "maincpu" )->base();
	m_mainram = make_unique_clear<uint32_t[]>(0x10000);
	m_pic_output_bit =0;
}


void mpu5_state::mpu5(machine_config &config)
{
	M68340(config, m_maincpu, 16000000);    // ?
	m_maincpu->set_addrmap(AS_PROGRAM, &mpu5_state::mpu5_map);

	config.set_default_layout(layout_mpu5);

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();
	/* unknown sound */
}

#include "mpu5.hxx"
