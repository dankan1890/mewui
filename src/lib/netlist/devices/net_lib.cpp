// license:GPL-2.0+
// copyright-holders:Couriersud
/***************************************************************************

    netlib.c

    Discrete netlist implementation.

****************************************************************************/

#include "net_lib.h"
#include "nld_system.h"
#include "nl_factory.h"
#include "solver/nld_solver.h"

NETLIST_START(diode_models)
	NET_MODEL("D _(IS=1e-15 N=1)")

	NET_MODEL("1N914 D(Is=2.52n Rs=.568 N=1.752 Cjo=4p M=.4 tt=20n Iave=200m Vpk=75 mfg=OnSemi type=silicon)")
	NET_MODEL("1N4001 D(Is=14.11n N=1.984 Rs=33.89m Ikf=94.81 Xti=3 Eg=1.11 Cjo=25.89p M=.44 Vj=.3245 Fc=.5 Bv=75 Ibv=10u Tt=5.7u Iave=1 Vpk=50 mfg=GI type=silicon)")
	NET_MODEL("1N4148 D(Is=2.52n Rs=.568 N=1.752 Cjo=4p M=.4 tt=20n Iave=200m Vpk=75 mfg=OnSemi type=silicon)")
	NET_MODEL("1S1588 D(Is=2.52n Rs=.568 N=1.752 Cjo=4p M=.4 tt=20n Iave=200m Vpk=75)")
	NET_MODEL("LedRed D(IS=93.2p RS=42M N=3.73 BV=4 IBV=10U CJO=2.97P VJ=.75 M=.333 TT=4.32U Iave=40m Vpk=4 type=LED)")
	NET_MODEL("LedGreen D(IS=93.2p RS=42M N=4.61 BV=4 IBV=10U CJO=2.97P VJ=.75 M=.333 TT=4.32U Iave=40m Vpk=4 type=LED)")
	NET_MODEL("LedBlue D(IS=93.2p RS=42M N=7.47 BV=5 IBV=10U CJO=2.97P VJ=.75 M=.333 TT=4.32U Iave=40m Vpk=5 type=LED)")
	NET_MODEL("LedWhite D(Is=0.27n Rs=5.65 N=6.79 Cjo=42p Iave=30m Vpk=5 type=LED)")

NETLIST_END()

NETLIST_START(bjt_models)
	NET_MODEL("NPN _(IS=1e-15 BF=100 NF=1 BR=1 NR=1)")
	NET_MODEL("PNP _(IS=1e-15 BF=100 NF=1 BR=1 NR=1)")

	NET_MODEL("2SA1015 PNP(Is=295.1E-18 Xti=3 Eg=1.11 Vaf=100 Bf=110 Xtb=1.5 Br=10.45 Rc=15 Cjc=66.2p Mjc=1.054 Vjc=.75 Fc=.5 Cje=5p Mje=.3333 Vje=.75 Tr=10n Tf=1.661n VCEO=45V ICrating=150M MFG=Toshiba)")
	NET_MODEL("2SC1815 NPN(Is=2.04f Xti=3 Eg=1.11 Vaf=6 Bf=400 Ikf=20m Xtb=1.5 Br=3.377 Rc=1 Cjc=1p Mjc=.3333 Vjc=.75 Fc=.5 Cje=25p Mje=.3333 Vje=.75 Tr=450n Tf=20n Itf=0 Vtf=0 Xtf=0 VCEO=45V ICrating=150M MFG=Toshiba)")

	NET_MODEL("2N3643 NPN(IS=14.34E-15 ISE=14.34E-15 ISC=0 XTI=3 BF=255.9 BR=6.092 IKF=0.2847 IKR=0 XTB=1.5 VAF=74.03 VAR=28 VJE=0.65 VJC=0.65 RE=0.1 RC=1 RB=10 CJE=22.01E-12 CJC=7.306E-12 XCJC=0.75 FC=0.5 NF=1 NR=1 NE=1.307 NC=2 MJE=0.377 MJC=0.3416 TF=411.1E-12 TR=46.91E-9 ITF=0.6 VTF=1.7 XTF=3 EG=1.11 KF=0 AF=1 VCEO=30 ICRATING=500m MFG=NSC)")
	NET_MODEL("2N3645 PNP(IS=650.6E-18 ISE=54.81E-15 ISC=0 XTI=3 BF=231.7 BR=3.563 IKF=1.079 IKR=0 XTB=1.5 VAF=115.7 VAR=35 VJE=0.65 VJC=0.65 RE=0.15 RC=0.715 RB=10 CJE=19.82E-12 CJC=14.76E-12 XCJC=0.75 FC=0.5 NF=1 NR=1 NE=1.829 NC=2 MJE=0.3357 MJC=0.5383 TF=603.7E-12 TR=111.3E-9 ITF=0.65 VTF=5 XTF=1.7 EG=1.11 KF=0 AF=1 VCEO=60 ICRATING=500m MFG=NSC)")
	// 3644 = 3645 Difference between 3644 and 3645 is voltage rating. 3644: VCBO=45, 3645: VCBO=60
	NET_MODEL("2N3644 PNP(IS=650.6E-18 ISE=54.81E-15 ISC=0 XTI=3 BF=231.7 BR=3.563 IKF=1.079 IKR=0 XTB=1.5 VAF=115.7 VAR=35 VJE=0.65 VJC=0.65 RE=0.15 RC=0.715 RB=10 CJE=19.82E-12 CJC=14.76E-12 XCJC=0.75 FC=0.5 NF=1 NR=1 NE=1.829 NC=2 MJE=0.3357 MJC=0.5383 TF=603.7E-12 TR=111.3E-9 ITF=0.65 VTF=5 XTF=1.7 EG=1.11 KF=0 AF=1 VCEO=60 ICRATING=500m MFG=NSC)")
	// 2N5190 = BC817-25
	NET_MODEL("2N5190 NPN(IS=9.198E-14 NF=1.003 ISE=4.468E-16 NE=1.65 BF=338.8 IKF=0.4913 VAF=107.9 NR=1.002 ISC=5.109E-15 NC=1.071 BR=29.48 IKR=0.193 VAR=25 RB=1 IRB=1000 RBM=1 RE=0.2126 RC=0.143 XTB=0 EG=1.11 XTI=3 CJE=3.825E-11 VJE=0.7004 MJE=0.364 TF=5.229E-10 XTF=219.7 VTF=3.502 ITF=7.257 PTF=0 CJC=1.27E-11 VJC=0.4431 MJC=0.3983 XCJC=0.4555 TR=7E-11 CJS=0 VJS=0.75 MJS=0.333 FC=0.905 Vceo=45 Icrating=500m mfg=Philips)")
	NET_MODEL("2SC945 NPN(IS=3.577E-14 BF=2.382E+02 NF=1.01 VAF=1.206E+02 IKF=3.332E-01 ISE=3.038E-16 NE=1.205 BR=1.289E+01 NR=1.015 VAR=1.533E+01 IKR=2.037E-01 ISC=3.972E-14 NC=1.115 RB=3.680E+01 IRB=1.004E-04 RBM=1 RE=8.338E-01 RC=1.557E+00 CJE=1.877E-11 VJE=7.211E-01 MJE=3.486E-01 TF=4.149E-10 XTF=1.000E+02 VTF=9.956 ITF=5.118E-01 PTF=0 CJC=6.876p VJC=3.645E-01 MJC=3.074E-01 TR=5.145E-08 XTB=1.5 EG=1.11 XTI=3 FC=0.5 Vceo=50 Icrating=100m MFG=NEC)")

	NET_MODEL("BC237B NPN(IS=1.8E-14 ISE=5.0E-14 ISC=1.72E-13 XTI=3 BF=400 BR=35.5 IKF=0.14 IKR=0.03 XTB=1.5 VAF=80 VAR=12.5 VJE=0.58 VJC=0.54 RE=0.6 RC=0.25 RB=0.56 CJE=13E-12 CJC=4E-12 XCJC=0.75 FC=0.5 NF=0.9955 NR=1.005 NE=1.46 NC=1.27 MJE=0.33 MJC=0.33 TF=0.64E-9 TR=50.72E-9 EG=1.11 KF=0 AF=1 VCEO=45 ICRATING=100M MFG=ZETEX)")
	NET_MODEL("BC556B PNP(IS=3.83E-14 NF=1.008 ISE=1.22E-14 NE=1.528 BF=344.4 IKF=0.08039 VAF=21.11 NR=1.005 ISC=2.85E-13 NC=1.28 BR=14.84 IKR=0.047 VAR=32.02 RB=1 IRB=1.00E-06 RBM=1 RE=0.6202 RC=0.5713 XTB=0 EG=1.11 XTI=3 CJE=1.23E-11 VJE=0.6106 MJE=0.378 TF=5.60E-10 XTF=3.414 VTF=5.23 ITF=0.1483 PTF=0 CJC=1.08E-11 VJC=0.1022 MJC=0.3563 XCJC=0.6288 TR=1.00E-32 CJS=0 VJS=0.75 MJS=0.333 FC=0.8027 Vceo=65 Icrating=100m mfg=Philips)")
	NET_MODEL("BC548C NPN(IS=1.95E-14 ISE=1.31E-15 ISC=1.0E-13 XTI=3 BF=466 BR=2.42 IKF=0.18 IKR=1 XTB=1.5 VAF=91.7 VAR=24.7 VJE=0.632 VJC=0.339 RE=1 RC=1.73 RB=26.5 RBM=10 IRB=10 CJE=1.33E-11 CJC=5.17E-12 XCJC=1 FC=0.9 NF=0.993 NR=1.2 NE=1.32 NC=2.00 MJE=0.326 MJC=0.319 TF=6.52E-10 TR=0 PTF=0 ITF=1.03 VTF=1.65 XTF=100 EG=1.11 KF=1E-9 AF=1 VCEO=40 ICrating=800M MFG=Siemens)")
	NET_MODEL("BC817-25 NPN(IS=9.198E-14 NF=1.003 ISE=4.468E-16 NE=1.65 BF=338.8 IKF=0.4913 VAF=107.9 NR=1.002 ISC=5.109E-15 NC=1.071 BR=29.48 IKR=0.193 VAR=25 RB=1 IRB=1000 RBM=1 RE=0.2126 RC=0.143 XTB=0 EG=1.11 XTI=3 CJE=3.825E-11 VJE=0.7004 MJE=0.364 TF=5.229E-10 XTF=219.7 VTF=3.502 ITF=7.257 PTF=0 CJC=1.27E-11 VJC=0.4431 MJC=0.3983 XCJC=0.4555 TR=7E-11 CJS=0 VJS=0.75 MJS=0.333 FC=0.905 Vceo=45 Icrating=500m mfg=Philips)")
NETLIST_END()

NETLIST_START(family_models)
	NET_MODEL("FAMILY _(TYPE=CUSTOM IVL=0.8 IVH=2.0 OVL=0.1 OVH=4.0 ORL=1.0 ORH=130.0)")
	NET_MODEL("OPAMP _()")

	NET_MODEL("74XXOC FAMILY(IVL=0.8 IVH=2.0 OVL=0.1 OVH=4.95 ORL=10.0 ORH=1.0e8)")
	NET_MODEL("74XX FAMILY(TYPE=TTL)")
	NET_MODEL("CD4XXX FAMILY(TYPE=CD4XXX)")
NETLIST_END()


#define xstr(s) # s
#define ENTRY1(nic, name, defparam) factory.register_device<nic>( # name, xstr(nic), defparam );
#define ENTRY(nic, name, defparam) ENTRY1(NETLIB_NAME(nic), name, defparam)

#define NETLIB_DEVICE_DECL(chip) extern factory_creator_ptr_t decl_ ## chip;

#define ENTRYX1(nic, name, defparam, decl) factory.register_device( decl (# name, xstr(nic), defparam) );
#define ENTRYX(nic, name, defparam) { NETLIB_DEVICE_DECL(nic) ENTRYX1(NETLIB_NAME(nic), name, defparam, decl_ ## nic) }

namespace netlist
{
	namespace devices
	{
static void initialize_factory(factory_list_t &factory)
{
	ENTRY(R,                    RES,                    "R")
	ENTRY(POT,                  POT,                    "R")
	ENTRY(POT2,                 POT2,                   "R")
	ENTRY(C,                    CAP,                    "C")
	ENTRY(D,                    DIODE,                  "MODEL")
	ENTRY(VCVS,                 VCVS,                   "-")
	ENTRY(VCCS,                 VCCS,                   "-")
	ENTRY(CCCS,                 CCCS,                   "-")
	ENTRY(LVCCS,                LVCCS,                  "-")
	ENTRY(VS,                   VS,                     "V")
	ENTRY(CS,                   CS,                     "I")
	ENTRY(OPAMP,                OPAMP,                  "MODEL")
	ENTRYX(dummy_input,         DUMMY_INPUT,            "-")
	ENTRYX(frontier,            FRONTIER_DEV,           "+I,G,Q")   // not intended to be used directly
	ENTRYX(function,            AFUNC,                  "N,FUNC")   // only for macro devices - NO FEEDBACK loops
	ENTRY(QBJT_EB,              QBJT_EB,                "MODEL")
	ENTRY(QBJT_switch,          QBJT_SW,                "MODEL")
	ENTRYX(logic_input,         TTL_INPUT,              "IN")
	ENTRYX(logic_input,         LOGIC_INPUT,            "IN,FAMILY")
	ENTRYX(analog_input,        ANALOG_INPUT,           "IN")
	ENTRYX(log,                 LOG,                    "+I")
	ENTRYX(logD,                LOGD,                   "+I,I2")
	ENTRYX(clock,               CLOCK,                  "FREQ")
	ENTRYX(extclock,            EXTCLOCK,               "FREQ")
	ENTRYX(mainclock,           MAINCLOCK,              "FREQ")
	ENTRYX(gnd,                 GND,                    "-")
	ENTRYX(netlistparams,       PARAMETER,              "-")
	ENTRY(solver,               SOLVER,                 "FREQ")
	ENTRYX(res_sw,              RES_SWITCH,             "+IN,P1,P2")
	ENTRY(switch1,              SWITCH,                 "-")
	ENTRY(switch2,              SWITCH2,                "-")
	ENTRYX(nicRSFF,             NETDEV_RSFF,            "+S,R")
	ENTRYX(nicDelay,            NETDEV_DELAY,            "-")
	ENTRYX(7450,                TTL_7450_ANDORINVERT,   "+A,B,C,D")
	ENTRYX(7448,                TTL_7448,               "+A,B,C,D,LTQ,BIQ,RBIQ")
	ENTRYX(7474,                TTL_7474,               "+CLK,D,CLRQ,PREQ")
	ENTRYX(7483,                TTL_7483,               "+A1,A2,A3,A4,B1,B2,B3,B4,C0")
	ENTRYX(7490,                TTL_7490,               "+A,B,R1,R2,R91,R92")
	ENTRYX(7493,                TTL_7493,               "+CLKA,CLKB,R1,R2")
	ENTRYX(74107,               TTL_74107,              "+CLK,J,K,CLRQ")
	ENTRYX(74107A,              TTL_74107A,             "+CLK,J,K,CLRQ")
	ENTRYX(74123,               TTL_74123,              "-")
	ENTRYX(74153,               TTL_74153,              "+C0,C1,C2,C3,A,B,G")
	ENTRYX(74175,               TTL_74175,              "-")
	ENTRYX(74192,               TTL_74192,              "-")
	ENTRYX(74193,               TTL_74193,              "-")
	//ENTRY(74279,              TTL_74279,              "-") // only dip available
	ENTRYX(SN74LS629,           SN74LS629,              "CAP")
	ENTRYX(82S16,               TTL_82S16,              "-")
	ENTRYX(9310,                TTL_9310,               "-")
	ENTRYX(9312,                TTL_9312,               "-")
	ENTRYX(9316,                TTL_9316,               "+CLK,ENP,ENT,CLRQ,LOADQ,A,B,C,D")
	ENTRYX(CD4020,              CD4020,                 "")
	ENTRYX(CD4066_GATE,         CD4066_GATE,            "")
	/* entries with suffix WI are legacy only */
	ENTRYX(CD4020,              CD4020_WI,              "+IP,RESET,VDD,VSS")
	//ENTRY(4066,                 CD_4066,              "+A,B")
	ENTRYX(NE555,               NE555,                  "-")
	ENTRYX(r2r_dac,             R2R_DAC,                "+VIN,R,N")
	ENTRYX(4538_dip,            CD4538_DIP,             "-")
	ENTRYX(7448_dip,            TTL_7448_DIP,           "-")
	ENTRYX(7450_dip,            TTL_7450_DIP,           "-")
	ENTRYX(7474_dip,            TTL_7474_DIP,           "-")
	ENTRYX(7483_dip,            TTL_7483_DIP,           "-")
	ENTRYX(7490_dip,            TTL_7490_DIP,           "-")
	ENTRYX(7493_dip,            TTL_7493_DIP,           "-")
	ENTRYX(74107_dip,           TTL_74107_DIP,          "-")
	ENTRYX(74123_dip,           TTL_74123_DIP,          "-")
	ENTRYX(74153_dip,           TTL_74153_DIP,          "-")
	ENTRYX(74175_dip,           TTL_74175_DIP,          "-")
	ENTRYX(74192_dip,           TTL_74192_DIP,          "-")
	ENTRYX(74193_dip,           TTL_74193_DIP,          "-")
	ENTRYX(74279_dip,           TTL_74279_DIP,          "-")
	ENTRYX(82S16_dip,           TTL_82S16_DIP,          "-")
	ENTRYX(9602_dip,            TTL_9602_DIP,           "-")
	ENTRYX(9310_dip,            TTL_9310_DIP,           "-")
	ENTRYX(9312_dip,            TTL_9312_DIP,           "-")
	ENTRYX(9316_dip,            TTL_9316_DIP,           "-")
	ENTRYX(SN74LS629_dip,       SN74LS629_DIP,          "1.CAP1,2.CAP2")
	ENTRYX(NE555_dip,           NE555_DIP,              "-")
	ENTRYX(MM5837_dip,          MM5837_DIP,             "-")
}

	} //namespace devices
} // namespace netlist

namespace netlist
{
	void initialize_factory(factory_list_t &factory)
	{
		devices::initialize_factory(factory);
	}
}
