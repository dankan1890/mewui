// license:BSD-3-Clause
// copyright-holders:hap, Jonathan Gevaryahu
/*

  Sharp SM5xx MCU family disassembler

*/

#include "emu.h"
#include "debugger.h"
#include "sm510.h"


// common lookup tables

enum e_mnemonics
{
	// SM510 common
	mILL /* 0! */, mEXT,
	mLB, mLBL, mSBM, mEXBLA, mINCB, mDECB,
	mATPL, mRTN0, mRTN1, mTL, mTML, mTM, mT,
	mEXC, mBDC, mEXCI, mEXCD, mLDA, mLAX, mPTW, mWR, mWS,
	mKTA, mATBP, mATX, mATL, mATFC, mATR,
	mADD, mADD11, mADX, mCOMA, mROT, mRC, mSC,
	mTB, mTC, mTAM, mTMI, mTA0, mTABL, mTIS, mTAL, mTF1, mTF4,
	mRM, mSM,
	mPRE, mSME, mRME, mTMEL,
	mSKIP, mCEND, mIDIV, mDR, mDTA, mCLKLO, mCLKHI,

	// SM500 common
	mCOMCB, mRTN, mRTNS, mSSR, mTR, mTRS, mRBM,
	mADDC, mPDTW, mTW, mDTW,
	mATS, mEXKSA, mEXKFA,
	mRMF, mSMF, mCOMCN,
	mTA, mTM2, mTG,

	// SM590 aliases
	mNOP, mCCTRL, mINBL, mDEBL, mXBLA, mADCS, mTR7,
	// SM590 uniques
	mTAX, mLBLX, mMTR, mSTR, mINBM, mDEBM, mRTA, mBLTA, mEXAX, mTBA, mADS, mADC, mLBMX, mTLS
};

static const char *const s_mnemonics[] =
{
	"?", "",
	"LB", "LBL", "SBM", "EXBLA", "INCB", "DECB",
	"ATPL", "RTN0", "RTN1", "TL", "TML", "TM", "T",
	"EXC", "BDC", "EXCI", "EXCD", "LDA", "LAX", "PTW", "WR", "WS",
	"KTA", "ATBP", "ATX", "ATL", "ATFC", "ATR",
	"ADD", "ADD11", "ADX", "COMA", "ROT", "RC", "SC",
	"TB", "TC", "TAM", "TMI", "TA0", "TABL", "TIS", "TAL", "TF1", "TF4",
	"RM", "SM",
	"PRE", "SME", "RME", "TMEL",
	"SKIP", "CEND", "IDIV", "DR", "DTA", "CLKLO", "CLKHI",

	//
	"COMCB", "RTN", "RTNS", "SSR", "TR", "TRS", "RBM",
	"ADDC", "PDTW", "TW", "DTW",
	"ATS", "EXKSA", "EXKFA",
	"RMF", "SMF", "COMCN",
	"TA", "TM", "TG",

	//
	"NOP", "CCTRL", "INBL", "DEBL", "XBLA", "ADCS", "TR",
	//
	"TAX", "LBLX", "MTR", "STR", "INBM", "DEBM", "RTA", "BLTA", "EXAX", "TBA", "ADS", "ADC", "LBMX", "TLS"
};

// number of bits per opcode parameter, 8 or larger means 2-byte opcode
static const u8 s_bits[] =
{
	0, 8,
	4, 8, 0, 0, 0, 0,
	0, 0, 0, 4+8, 2+8, 6, 6,
	2, 0, 2, 2, 2, 4, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0, 0, 4, 0, 0, 0, 0,
	0, 0, 0, 2, 0, 0, 0, 0, 0, 0,
	2, 2,
	8, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0,

	//
	0, 0, 0, 4, 6, 6, 0,
	0, 0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 2, 0,

	//
	0, 0, 0, 0, 0, 0, 7,
	//
	4, 4, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 2+8
};

#define _OVER DASMFLAG_STEP_OVER
#define _OUT  DASMFLAG_STEP_OUT

static const u32 s_flags[] =
{
	0, 0,
	0, 0, 0, 0, 0, 0,
	0, _OUT, _OUT, 0, _OVER, _OVER, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0,
	0, 0, 0, 0,
	0, _OVER, 0, 0, 0, 0, 0,

	//
	0, _OUT, _OUT, 0, 0, _OVER, 0,
	0, 0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,

	//
	0, 0, 0, 0, 0, 0, _OVER,
	//
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _OVER
};


// common disasm

static offs_t sm510_common_disasm(const u8 *lut_mnemonic, const u8 *lut_extended, std::ostream &stream, offs_t pc, const u8 *oprom, const u8 *opram)
{
	// get raw opcode
	u8 op = oprom[0];
	u8 instr = lut_mnemonic[op];
	int len = 1;

	int bits = s_bits[instr];
	u8 mask = op & ((1 << (bits & 7)) - 1);
	u16 param = mask;
	if (bits >= 8)
	{
		param = oprom[1];
		len++;
	}

	// extended opcode
	bool is_extended = (instr == mEXT);
	if (is_extended)
		instr = lut_extended[param];

	// disassemble it
	util::stream_format(stream, "%-6s ", s_mnemonics[instr]);
	if (bits > 0)
	{
		if (bits <= 4)
		{
			if (param < 10)
				util::stream_format(stream, "%d", param);
			else
				util::stream_format(stream, "$%X", param);
		}
		else if (bits <= 8)
		{
			if (!is_extended)
				util::stream_format(stream, "$%02X", param);
		}
		else
		{
			u16 address = (param << 4 & 0xc00) | (mask << 6 & 0x3c0) | (param & 0x03f);
			util::stream_format(stream, "$%03X", address);
		}
	}

	return len | s_flags[instr] | DASMFLAG_SUPPORTED;
}


// SM510 disasm

static const u8 sm510_mnemonic[0x100] =
{
/*  0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F  */
	mSKIP, mATBP, mSBM,  mATPL, mRM,   mRM,   mRM,   mRM,   mADD,  mADD11,mCOMA, mEXBLA,mSM,   mSM,   mSM,   mSM,   // 0
	mEXC,  mEXC,  mEXC,  mEXC,  mEXCI, mEXCI, mEXCI, mEXCI, mLDA,  mLDA,  mLDA,  mLDA,  mEXCD, mEXCD, mEXCD, mEXCD, // 1
	mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  // 2
	mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  // 3 - note: $3A has synonym DC(decimal correct)

	mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   // 4
	0,     mTB,   mTC,   mTAM,  mTMI,  mTMI,  mTMI,  mTMI,  mTIS,  mATL,  mTA0,  mTABL, 0,     mCEND, mTAL,  mLBL,  // 5
	mATFC, mATR,  mWR,   mWS,   mINCB, mIDIV, mRC,   mSC,   mTF1,  mTF4,  mKTA,  mROT,  mDECB, mBDC,  mRTN0, mRTN1, // 6
	mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTML,  mTML,  mTML,  mTML,  // 7

	mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    // 8
	mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    // 9
	mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    // A
	mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    // B

	mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   // C
	mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   // D
	mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   // E
	mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM    // F
};

CPU_DISASSEMBLE(sm510)
{
	return sm510_common_disasm(sm510_mnemonic, nullptr, stream, pc, oprom, opram);
}


// SM511 disasm

static const u8 sm511_mnemonic[0x100] =
{
/*  0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F  */
	mROT,  mDTA,  mSBM,  mATPL, mRM,   mRM,   mRM,   mRM,   mADD,  mADD11,mCOMA, mEXBLA,mSM,   mSM,   mSM,   mSM,   // 0
	mEXC,  mEXC,  mEXC,  mEXC,  mEXCI, mEXCI, mEXCI, mEXCI, mLDA,  mLDA,  mLDA,  mLDA,  mEXCD, mEXCD, mEXCD, mEXCD, // 1
	mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  // 2
	mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  // 3

	mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   // 4
	mKTA,  mTB,   mTC,   mTAM,  mTMI,  mTMI,  mTMI,  mTMI,  mTIS,  mATL,  mTA0,  mTABL, mATX,  mCEND, mTAL,  mLBL,  // 5
	mEXT,  mPRE,  mWR,   mWS,   mINCB, mDR,   mRC,   mSC,   mTML,  mTML,  mTML,  mTML,  mDECB, mPTW,  mRTN0, mRTN1, // 6
	mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   mTL,   // 7

	mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    // 8
	mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    // 9
	mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    // A
	mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    mT,    // B

	mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   // C
	mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   // D
	mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   // E
	mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM,   mTM    // F
};

static const u8 sm511_extended[0x10] =
{
	mRME,  mSME,  mTMEL, mATFC, mBDC,  mATBP, mCLKHI,mCLKLO,0,     0,     0,     0,     0,     0,     0,     0      // 60 3
};

CPU_DISASSEMBLE(sm511)
{
	// create extended opcode table
	u8 ext[0x100];
	memset(ext, 0, 0x100);
	memcpy(ext + 0x30, sm511_extended, 0x10);

	return sm510_common_disasm(sm511_mnemonic, ext, stream, pc, oprom, opram);
}


// SM500 disasm

static const u8 sm500_mnemonic[0x100] =
{
/*  0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F  */
	mSKIP, mATR,  mEXKSA,mATBP, mRM,   mRM,   mRM,   mRM,   mADD,  mADDC, mCOMA, mEXBLA,mSM,   mSM,   mSM,   mSM,   // 0
	mEXC,  mEXC,  mEXC,  mEXC,  mEXCI, mEXCI, mEXCI, mEXCI, mLDA,  mLDA,  mLDA,  mLDA,  mEXCD, mEXCD, mEXCD, mEXCD, // 1
	mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  // 2
	mATS,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  // 3

	mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   // 4
	mTA,   mTB,   mTC,   mTAM,  mTM2,  mTM2,  mTM2,  mTM2,  mTG,   mPTW,  mTA0,  mTABL, mTW,   mDTW,  mEXT,  mLBL,  // 5
	mCOMCN,mPDTW, mWR,   mWS,   mINCB, mIDIV, mRC,   mSC,   mRMF,  mSMF,  mKTA,  mEXKFA,mDECB, mCOMCB,mRTN,  mRTNS, // 6
	mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  // 7

	mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   // 8
	mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   // 9
	mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   // A
	mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   // B

	mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  // C
	mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  // D
	mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  // E
	mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS   // F
};

static const u8 sm500_extended[0x10] =
{
	mCEND, 0,     0,     0,     mDTA,  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0      // 5E 0
};

CPU_DISASSEMBLE(sm500)
{
	// create extended opcode table
	u8 ext[0x100];
	memset(ext, 0, 0x100);
	memcpy(ext + 0x00, sm500_extended, 0x10);

	return sm510_common_disasm(sm500_mnemonic, ext, stream, pc, oprom, opram);
}


// SM5A disasm

static const u8 sm5a_mnemonic[0x100] =
{
/*  0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F  */
	mSKIP, mATR,  mSBM,  mATBP, mRM,   mRM,   mRM,   mRM,   mADD,  mADDC, mCOMA, mEXBLA,mSM,   mSM,   mSM,   mSM,   // 0
	mEXC,  mEXC,  mEXC,  mEXC,  mEXCI, mEXCI, mEXCI, mEXCI, mLDA,  mLDA,  mLDA,  mLDA,  mEXCD, mEXCD, mEXCD, mEXCD, // 1
	mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  // 2
	mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  // 3

	mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   mLB,   // 4
	mTA,   mTB,   mTC,   mTAM,  mTM2,  mTM2,  mTM2,  mTM2,  mTG,   mPTW,  mTA0,  mTABL, mTW,   mDTW,  mEXT,  mLBL,  // 5
	mCOMCN,mPDTW, mWR,   mWS,   mINCB, mIDIV, mRC,   mSC,   mRMF,  mSMF,  mKTA,  mRBM,  mDECB, mCOMCB,mRTN,  mRTNS, // 6
	mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  mSSR,  // 7

	mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   // 8
	mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   // 9
	mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   // A
	mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   mTR,   // B

	mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  // C
	mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  // D
	mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  // E
	mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS,  mTRS   // F
};

static const u8 sm5a_extended[0x10] =
{
	mCEND, 0,     0,     0,     mDTA,  0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0      // 5E 0
};

CPU_DISASSEMBLE(sm5a)
{
	// create extended opcode table
	u8 ext[0x100];
	memset(ext, 0, 0x100);
	memcpy(ext + 0x00, sm5a_extended, 0x10);

	return sm510_common_disasm(sm5a_mnemonic, ext, stream, pc, oprom, opram);
}


// SM590 disasm

static const u8 sm590_mnemonic[0x100] =
{
/*  0      1      2      3      4      5      6      7      8      9      A      B      C      D      E      F  */
	mNOP,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  mADX,  // 0
	mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  mTAX,  // 1
	mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, mLBLX, // 2
	mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  mLAX,  // 3

	mLDA,  mEXC,  mEXCI, mEXCD, mCOMA, mTAM,  mATR,  mMTR,  mRC,   mSC,   mSTR,  mCCTRL,mRTN,  mRTNS, 0,     0,     // 4
	mINBM, mDEBM, mINBL, mDEBL, mTC,   mRTA,  mBLTA, mXBLA, 0,     0,     0,     0,     mATX,  mEXAX, 0,     0,     // 5
	mTMI,  mTMI,  mTMI,  mTMI,  mTBA,  mTBA,  mTBA,  mTBA,  mRM,   mRM,   mRM,   mRM,   mSM,   mSM,   mSM,   mSM,   // 6
	mADD,  mADS,  mADC,  mADCS, mLBMX, mLBMX, mLBMX, mLBMX, mTL,   mTL,   mTL,   mTL,   mTLS,  mTLS,  mTLS,  mTLS,  // 7

	mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  // 8
	mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  // 9
	mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  // A
	mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  // B

	mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  // C
	mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  // D
	mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  // E
	mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7,  mTR7   // F
};


CPU_DISASSEMBLE(sm590)
{
	return sm510_common_disasm(sm590_mnemonic, nullptr, stream, pc, oprom, opram);
}
