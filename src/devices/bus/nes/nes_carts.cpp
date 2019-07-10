// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    NES carts

**********************************************************************/

#include "emu.h"
#include "nes_carts.h"

// official PCBs
#include "nxrom.h"
#include "mmc1.h"
#include "mmc2.h"
#include "mmc3.h"
#include "mmc5.h"
#include "bandai.h"
#include "datach.h"
#include "discrete.h"
#include "disksys.h"
#include "event.h"
#include "irem.h"
#include "jaleco.h"
#include "karastudio.h"
#include "konami.h"
#include "namcot.h"
#include "pt554.h"
#include "sunsoft.h"
#include "sunsoft_dcs.h"
#include "taito.h"
// unlicensed/bootleg/pirate PCBs
#include "2a03pur.h"
#include "act53.h"
#include "aladdin.h"
#include "ave.h"
#include "benshieng.h"
#include "camerica.h"
#include "cne.h"
#include "cony.h"
#include "ggenie.h"
#include "hes.h"
#include "henggedianzi.h"
#include "hosenkan.h"
#include "jy.h"
#include "kaiser.h"
#include "legacy.h"
#include "nanjing.h"
#include "ntdec.h"
#include "racermate.h"
#include "rcm.h"
#include "rexsoft.h"
#include "sachen.h"
#include "somari.h"
#include "subor.h"
#include "tengen.h"
#include "txc.h"
#include "waixing.h"
#include "zemina.h"
// misc unlicensed/bootleg/pirate PCBs
#include "bootleg.h"
#include "multigame.h"
#include "pirate.h"
#include "mmc3_clones.h"


void nes_cart(device_slot_interface &device)
{
// HROM, NROM, RROM, SROM & STROM
	device.option_add_internal("nrom",             NES_NROM);
// Nintendo Family BASIC pcb (NROM + 2K or 4K WRAM)
	device.option_add_internal("hvc_basic",        NES_FCBASIC);
// Extended NROM-368 board (NROM with 46K PRG)
	device.option_add_internal("nrom368",          NES_NROM368);
// Game Genie
	device.option_add_internal("nrom_gg",          NES_GGENIE);
// UNROM/UOROM
	device.option_add_internal("uxrom",            NES_UXROM);
	device.option_add_internal("unrom_cc",         NES_UXROM_CC);
// CNROM
	device.option_add_internal("cnrom",            NES_CNROM);
// Bandai PT-554 (CNROM boards + special audio chip, used by Aerobics Studio)
	device.option_add_internal("bandai_pt554",     NES_BANDAI_PT554);
// CPROM
	device.option_add_internal("cprom",            NES_CPROM);
// AMROM, ANROM, AOROM
	device.option_add_internal("axrom",            NES_AXROM);
// PxROM
	device.option_add_internal("pxrom",            NES_PXROM);
// FxROM
	device.option_add_internal("fxrom",            NES_FXROM);
// BNROM
	device.option_add_internal("bnrom",            NES_BXROM);
// GNROM & MHROM
	device.option_add_internal("gxrom",            NES_GXROM);
// UN1ROM
	device.option_add_internal("un1rom",           NES_UN1ROM);
// SxROM
	device.option_add_internal("sxrom",            NES_SXROM);
	device.option_add_internal("sorom",            NES_SOROM);
	device.option_add_internal("sxrom_a",          NES_SXROM_A);  // in MMC1-A PRG RAM is always enabled
	device.option_add_internal("sorom_a",          NES_SOROM_A);  // in MMC1-A PRG RAM is always enabled
// TxROM
	device.option_add_internal("txrom",            NES_TXROM);
// HKROM
	device.option_add_internal("hkrom",            NES_HKROM);
// TQROM
	device.option_add_internal("tqrom",            NES_TQROM);
// TxSROM
	device.option_add_internal("txsrom",           NES_TXSROM);
// ExROM
	device.option_add_internal("exrom",            NES_EXROM);
// RAM expansion + Disk System add-on
	device.option_add_internal("disksys",          NES_DISKSYS);
// Nintendo Custom boards
	device.option_add_internal("pal_zz",           NES_ZZ_PCB);
	device.option_add_internal("nes_qj",           NES_QJ_PCB);
	device.option_add_internal("nes_event",        NES_EVENT);
// Discrete Components boards
// IC_74x139x74
	device.option_add_internal("discrete_74x139",  NES_74X139X74);
// IC_74x377
	device.option_add_internal("discrete_74x377",  NES_74X377);
// Discrete board IC_74x161x161x32
	device.option_add_internal("discrete_74x161",  NES_74X161X161X32);
// Discrete board IC_74x161x138
	device.option_add_internal("bitcorp_dis",      NES_74X161X138);
// Bandai boards
	device.option_add_internal("lz93d50",          NES_LZ93D50);
	device.option_add_internal("lz93d50_ep1",      NES_LZ93D50_24C01);
	device.option_add_internal("lz93d50_ep2",      NES_LZ93D50_24C02);
	device.option_add_internal("fcg",              NES_FCG);
	device.option_add_internal("fjump2",           NES_FJUMP2);
	device.option_add_internal("datach",           NES_DATACH);
	device.option_add_internal("karastudio",       NES_KARAOKESTUDIO);
	device.option_add_internal("oekakids",         NES_OEKAKIDS);
// Irem boards
	device.option_add_internal("g101",             NES_G101);
	device.option_add_internal("lrog017",          NES_LROG017);
	device.option_add_internal("h3001",            NES_H3001);
	device.option_add_internal("holydivr",         NES_HOLYDIVR);
	device.option_add_internal("tam_s1",           NES_TAM_S1);
// Jaleco boards
	device.option_add_internal("jf11",             NES_JF11);
	device.option_add_internal("jf13",             NES_JF13);
	device.option_add_internal("jf16",             NES_JF16);
	device.option_add_internal("jf17",             NES_JF17);
	device.option_add_internal("jf17pcm",          NES_JF17_ADPCM);
	device.option_add_internal("jf19",             NES_JF19);
	device.option_add_internal("jf19pcm",          NES_JF19_ADPCM);
	device.option_add_internal("ss88006",          NES_SS88006);
	device.option_add_internal("jf23",             NES_JF23);
	device.option_add_internal("jf24",             NES_JF24);
	device.option_add_internal("jf29",             NES_JF29);
	device.option_add_internal("jf33",             NES_JF33);
// Konami boards
	device.option_add_internal("vrc1",             NES_VRC1);
	device.option_add_internal("vrc2",             NES_VRC2);
	device.option_add_internal("vrc3",             NES_VRC3);
	device.option_add_internal("vrc4",             NES_VRC4);
	device.option_add_internal("vrc6",             NES_VRC6);
	device.option_add_internal("vrc7",             NES_VRC7);
// Namcot boards
	device.option_add_internal("namcot_163",       NES_NAMCOT163);
	device.option_add_internal("namcot_175",       NES_NAMCOT175);
	device.option_add_internal("namcot_340",       NES_NAMCOT340);
	device.option_add_internal("namcot_3433",      NES_NAMCOT3433);  // DxROM is a Nintendo board for US versions of the 3433/3443 games
	device.option_add_internal("namcot_3425",      NES_NAMCOT3425);
	device.option_add_internal("namcot_3446",      NES_NAMCOT3446);
// Sunsoft boards
	device.option_add_internal("sunsoft1",         NES_SUNSOFT_1);
	device.option_add_internal("sunsoft2",         NES_SUNSOFT_2);
	device.option_add_internal("sunsoft3",         NES_SUNSOFT_3);
	device.option_add_internal("sunsoft4",         NES_SUNSOFT_4);
	device.option_add_internal("sunsoft_dcs",      NES_SUNSOFT_DCS);
	device.option_add_internal("sunsoft_fme7",     NES_SUNSOFT_FME7); // JxROM is a Nintendo board for US versions of the Sunsoft FME7 games
	device.option_add_internal("sunsoft5a",        NES_SUNSOFT_5);
	device.option_add_internal("sunsoft5b",        NES_SUNSOFT_5);
// Taito boards
	device.option_add_internal("tc0190fmc",        NES_TC0190FMC);
	device.option_add_internal("tc0190fmcp",       NES_TC0190FMC_PAL16R4);
	device.option_add_internal("tc0350fmr",        NES_TC0190FMC);
	device.option_add_internal("x1_005",           NES_X1_005);   // two variants exist, depending on pin17 & pin31 connections
	device.option_add_internal("x1_017",           NES_X1_017);
// Misc pirate boards (by AVE, Camerica, C&E, Nanjing, NTDEC, JY Company, Sachen, Tengen, TXC, Waixing, Henggendianzi, etc.)
	device.option_add_internal("nina001",          NES_NINA001);
	device.option_add_internal("nina006",          NES_NINA006);
	device.option_add_internal("bf9093",           NES_BF9093);
	device.option_add_internal("bf9096",           NES_BF9096);
	device.option_add_internal("goldenfive",       NES_GOLDEN5);
	device.option_add_internal("ade",              NES_ALADDIN);
	device.option_add_internal("cne_decathl",      NES_CNE_DECATHL);
	device.option_add_internal("cne_fsb",          NES_CNE_FSB);
	device.option_add_internal("cne_shlz",         NES_CNE_SHLZ);
	device.option_add_internal("nanjing",          NES_NANJING);     // mapper 163
	device.option_add_internal("ntdec_asder",      NES_NTDEC_ASDER); // mapper 112
	device.option_add_internal("ntdec_fh",         NES_NTDEC_FH);    // mapper 193
	device.option_add_internal("jyc_a",            NES_JY_TYPEA);    // mapper 90
	device.option_add_internal("jyc_b",            NES_JY_TYPEB);    // mapper 211
	device.option_add_internal("jyc_c",            NES_JY_TYPEC);    // mapper 209
	device.option_add_internal("sa009",            NES_SACHEN_SA009);
	device.option_add_internal("sa0036",           NES_SACHEN_SA0036);
	device.option_add_internal("sa0037",           NES_SACHEN_SA0037);
	device.option_add_internal("sa72007",          NES_SACHEN_SA72007);
	device.option_add_internal("sa72008",          NES_SACHEN_SA72008);
	device.option_add_internal("tca01",            NES_SACHEN_TCA01);
	device.option_add_internal("s8259a",           NES_SACHEN_8259A);
	device.option_add_internal("s8259b",           NES_SACHEN_8259B);
	device.option_add_internal("s8259c",           NES_SACHEN_8259C);
	device.option_add_internal("s8259d",           NES_SACHEN_8259D);
	device.option_add_internal("s74x374",          NES_SACHEN_74X374);
	device.option_add_internal("s74x374a",         NES_SACHEN_74X374_ALT);  // FIXME: Made up boards some different handling
	device.option_add_internal("tcu01",            NES_SACHEN_TCU01);
	device.option_add_internal("tcu02",            NES_SACHEN_TCU02);
	device.option_add_internal("tengen_800008",    NES_TENGEN_800008);   // FIXME: Is this the same as CNROM?
	device.option_add_internal("tengen_800032",    NES_TENGEN_800032);
	device.option_add_internal("tengen_800037",    NES_TENGEN_800037);
	device.option_add_internal("txc_22211",        NES_TXC_22211);
	device.option_add_internal("txc_dumarc",       NES_TXC_DUMARACING);
	device.option_add_internal("txc_mjblock",      NES_TXC_MJBLOCK);
	device.option_add_internal("txc_strikew",      NES_TXC_STRIKEW);
	device.option_add_internal("txc_commandos",    NES_TXC_COMMANDOS);
	device.option_add_internal("waixing_a",        NES_WAIXING_A);
	device.option_add_internal("waixing_a1",       NES_WAIXING_A1);   // FIXME: Made up boards the different CHRRAM banks (see Ji Jia Zhan Shi)
	device.option_add_internal("waixing_b",        NES_WAIXING_B);
	device.option_add_internal("waixing_c",        NES_WAIXING_C);
	device.option_add_internal("waixing_d",        NES_WAIXING_D);
	device.option_add_internal("waixing_e",        NES_WAIXING_E);
	device.option_add_internal("waixing_f",        NES_WAIXING_F);
	device.option_add_internal("waixing_g",        NES_WAIXING_G);
	device.option_add_internal("waixing_h",        NES_WAIXING_H);
	device.option_add_internal("waixing_h1",       NES_WAIXING_H1);   // FIXME: Made up boards the different WRAM protect banks (see Shen Mi Jin San Jiao)
	device.option_add_internal("waixing_i",        NES_WAIXING_I);
	device.option_add_internal("waixing_j",        NES_WAIXING_J);
	device.option_add_internal("waixing_sgz",      NES_WAIXING_SGZ);
	device.option_add_internal("waixing_sgzlz",    NES_WAIXING_SGZLZ);
	device.option_add_internal("waixing_sec",      NES_WAIXING_SEC);
	device.option_add_internal("waixing_ffv",      NES_WAIXING_FFV);
	device.option_add_internal("waixing_wxzs",     NES_WAIXING_WXZS);
	device.option_add_internal("waixing_wxzs2",    NES_WAIXING_WXZS2);
	device.option_add_internal("waixing_dq8",      NES_WAIXING_DQ8);
	device.option_add_internal("waixing_sh2",      NES_WAIXING_SH2);
	device.option_add_internal("fs304",            NES_WAIXING_FS304);  // used in Zelda 3 by Waixing
	device.option_add_internal("cony",             NES_CONY);
	device.option_add_internal("yoko",             NES_YOKO);
	device.option_add_internal("hengg_srich",      NES_HENGG_SRICH);
	device.option_add_internal("hengg_xhzs",       NES_HENGG_XHZS);
	device.option_add_internal("hengg_shjy3",      NES_HENGG_SHJY3); // mapper 253
	device.option_add_internal("hes",              NES_HES);
	device.option_add_internal("hosenkan",         NES_HOSENKAN);
	device.option_add_internal("ks7058",           NES_KS7058);
	device.option_add_internal("ks202",            NES_KS202);      // mapper 56
	device.option_add_internal("ks7022",           NES_KS7022);     // mapper 175
	device.option_add_internal("ks7017",           NES_KS7017);
	device.option_add_internal("ks7032",           NES_KS7032);     //  mapper 142
	device.option_add_internal("ks7012",           NES_KS7012);     // used in Zanac (FDS Conversion);
	device.option_add_internal("ks7013b",          NES_KS7013B);    // used in Highway Star (FDS Conversion);
	device.option_add_internal("ks7031",           NES_KS7031);     //  used in Dracula II (FDS Conversion);
	device.option_add_internal("ks7016",           NES_KS7016);     //  used in Exciting Basket (FDS Conversion);
	device.option_add_internal("ks7037",           NES_KS7037);     //  used in Metroid (FDS Conversion);
	device.option_add_internal("gs2015",           NES_GS2015);
	device.option_add_internal("gs2004",           NES_GS2004);
	device.option_add_internal("gs2013",           NES_GS2013);
	device.option_add_internal("tf9in1",           NES_TF9IN1);
	device.option_add_internal("3dblock",          NES_3DBLOCK);    // NROM + IRQ?
	device.option_add_internal("racermate",        NES_RACERMATE);  // mapper 168
	device.option_add_internal("agci_50282",       NES_AGCI_50282);
	device.option_add_internal("dreamtech01",      NES_DREAMTECH01);
	device.option_add_internal("fukutake",         NES_FUKUTAKE);
	device.option_add_internal("futuremedia",      NES_FUTUREMEDIA);
	device.option_add_internal("magicseries",      NES_MAGSERIES);
	device.option_add_internal("daou_306",         NES_DAOU306);
	device.option_add_internal("subor0",           NES_SUBOR0);
	device.option_add_internal("subor1",           NES_SUBOR1);
	device.option_add_internal("subor2",           NES_SUBOR2);
	device.option_add_internal("cc21",             NES_CC21);
	device.option_add_internal("xiaozy",           NES_XIAOZY);
	device.option_add_internal("edu2k",            NES_EDU2K);
	device.option_add_internal("t230",             NES_T230);
	device.option_add_internal("mk2",              NES_MK2);
	device.option_add_internal("unl_whero",        NES_WHERO);     // mapper 27
	device.option_add_internal("unl_43272",        NES_43272);     // used in Gaau Hok Gwong Cheung
	device.option_add_internal("tf1201",           NES_TF1201);
	device.option_add_internal("unl_cfight",       NES_CITYFIGHT); //  used by City Fighter IV
	device.option_add_internal("zemina",           NES_ZEMINA);    // mapper 190 - Magic Kid GooGoo
// misc bootleg boards
	device.option_add_internal("ax5705",           NES_AX5705);
	device.option_add_internal("sc127",            NES_SC127);
	device.option_add_internal("mariobaby",        NES_MARIOBABY);
	device.option_add_internal("asnicol",          NES_ASN);
	device.option_add_internal("smb3pirate",       NES_SMB3PIRATE);
	device.option_add_internal("btl_dninja",       NES_BTL_DNINJA);
	device.option_add_internal("whirl2706",        NES_WHIRLWIND_2706);
	device.option_add_internal("smb2j",            NES_SMB2J);
	device.option_add_internal("smb2ja",           NES_SMB2JA);
	device.option_add_internal("smb2jb",           NES_SMB2JB);
	device.option_add_internal("09034a",           NES_09034A);
	device.option_add_internal("tobidase",         NES_TOBIDASE);   // mapper 120
	device.option_add_internal("mmalee2",          NES_MMALEE);     // mapper 55?
	device.option_add_internal("unl_2708",         NES_2708);       // mapper 103
	device.option_add_internal("unl_lh32",         NES_LH32);       // used by Monty no Doki Doki Daidassou FDS conversion
	device.option_add_internal("unl_lh10",         NES_LH10);       // used in Fuuun Shaolin Kyo (FDS Conversion);
	device.option_add_internal("unl_lh53",         NES_LH53);       // used in Nazo no Murasamejou (FDS Conversion);
	device.option_add_internal("unl_ac08",         NES_AC08);       // used by Green Beret FDS conversion
	device.option_add_internal("unl_bb",           NES_UNL_BB);     // used by a few FDS conversions
	device.option_add_internal("sgpipe",           NES_SHUIGUAN);   // mapper 183
	device.option_add_internal("rt01",             NES_RT01);
// misc MMC3 clone boards
	device.option_add_internal("dbz5",             NES_REX_DBZ5);
	device.option_add_internal("sl1632",           NES_REX_SL1632);
	device.option_add_internal("somari",           NES_SOMARI); // mapper 116
	device.option_add_internal("nitra",            NES_NITRA);
	device.option_add_internal("ks7057",           NES_KS7057); // mapper 196 alt (for Street Fighter VI / Fight Street VI);
	device.option_add_internal("sbros11",          NES_SBROS11);
	device.option_add_internal("unl_malisb",       NES_MALISB); //  used by Super Mali Splash Bomb
	device.option_add_internal("family4646",       NES_FAMILY4646);
	device.option_add_internal("pikay2k",          NES_PIKAY2K); // mapper 254
	device.option_add_internal("8237",             NES_8237);
	device.option_add_internal("8237a",            NES_NROM);    // UNSUPPORTED
	device.option_add_internal("sg_lionk",         NES_SG_LIONK);
	device.option_add_internal("sg_boog",          NES_SG_BOOG);
	device.option_add_internal("kasing",           NES_KASING);
	device.option_add_internal("kay",              NES_KAY);
	device.option_add_internal("h2288",            NES_H2288);
	device.option_add_internal("unl_6035052",      NES_6035052); // mapper 238?
	device.option_add_internal("txc_tw",           NES_TXC_TW);
	device.option_add_internal("kof97",            NES_KOF97);
	device.option_add_internal("kof96",            NES_KOF96);
	device.option_add_internal("sfight3",          NES_SF3);
	device.option_add_internal("gouder",           NES_GOUDER);
	device.option_add_internal("sa9602b",          NES_SA9602B);
	device.option_add_internal("unl_shero",        NES_SACHEN_SHERO);
// misc multigame cart boards
	device.option_add_internal("benshieng",        NES_BENSHIENG);
	device.option_add_internal("action52",         NES_ACTION52);
	device.option_add_internal("caltron6in1",      NES_CALTRON6IN1);
	device.option_add_internal("maxi15",           NES_MAXI15);        //  mapper 234
	device.option_add_internal("rumblestation",    NES_RUMBLESTATION);    // mapper 46
	device.option_add_internal("svision16",        NES_SVISION16);  // mapper 53
	device.option_add_internal("n625092",          NES_N625092);
	device.option_add_internal("a65as",            NES_A65AS);
	device.option_add_internal("t262",             NES_T262);
	device.option_add_internal("novel1",           NES_NOVEL1);
	device.option_add_internal("novel2",           NES_NOVEL2); // mapper 213... same as BMC-NOVELDIAMOND9999999IN1 board?
	device.option_add_internal("studyngame",       NES_STUDYNGAME); // mapper 39
	device.option_add_internal("sgun20in1",        NES_SUPERGUN20IN1);
	device.option_add_internal("bmc_vt5201",       NES_VT5201); // mapper 60 otherwise
	device.option_add_internal("bmc_d1038",        NES_VT5201); // mapper 60?
	device.option_add_internal("810544c",          NES_810544C);
	device.option_add_internal("ntd03",            NES_NTD03);
	device.option_add_internal("bmc_gb63",         NES_BMC_GB63);
	device.option_add_internal("bmc_gka",          NES_BMC_GKA);
	device.option_add_internal("bmc_gkb",          NES_BMC_GKB);
	device.option_add_internal("bmc_ws",           NES_BMC_WS);
	device.option_add_internal("bmc_g146",         NES_BMC_G146);
	device.option_add_internal("bmc_11160",        NES_BMC_11160);
	device.option_add_internal("bmc_8157",         NES_BMC_8157);
	device.option_add_internal("bmc_hik300",       NES_BMC_HIK300);
	device.option_add_internal("bmc_s700",         NES_BMC_S700);
	device.option_add_internal("bmc_ball11",       NES_BMC_BALL11);
	device.option_add_internal("bmc_22games",      NES_BMC_22GAMES);
	device.option_add_internal("bmc_64y2k",        NES_BMC_64Y2K);
	device.option_add_internal("bmc_12in1",        NES_BMC_12IN1);
	device.option_add_internal("bmc_20in1",        NES_BMC_20IN1);
	device.option_add_internal("bmc_21in1",        NES_BMC_21IN1);
	device.option_add_internal("bmc_31in1",        NES_BMC_31IN1);
	device.option_add_internal("bmc_35in1",        NES_BMC_35IN1);
	device.option_add_internal("bmc_36in1",        NES_BMC_36IN1);
	device.option_add_internal("bmc_64in1",        NES_BMC_64IN1);
	device.option_add_internal("bmc_70in1",        NES_BMC_70IN1);   // mapper 236?
	device.option_add_internal("bmc_72in1",        NES_BMC_72IN1);
	device.option_add_internal("bmc_76in1",        NES_BMC_76IN1);
	device.option_add_internal("bmc_s42in1",       NES_BMC_76IN1);
	device.option_add_internal("bmc_110in1",       NES_BMC_110IN1);
	device.option_add_internal("bmc_150in1",       NES_BMC_150IN1);
	device.option_add_internal("bmc_190in1",       NES_BMC_190IN1);
	device.option_add_internal("bmc_800in1",       NES_BMC_800IN1);     // mapper 236?
	device.option_add_internal("bmc_1200in1",      NES_BMC_1200IN1);
	device.option_add_internal("bmc_gold150",      NES_BMC_GOLD150);    // mapper 235 with 2M PRG
	device.option_add_internal("bmc_gold260",      NES_BMC_GOLD260);    // mapper 235 with 4M PRG
	device.option_add_internal("bmc_power255",     NES_BMC_CH001);      // mapper 63?
	device.option_add_internal("bmc_s22games",     NES_BMC_SUPER22);    // mapper 233
	device.option_add_internal("bmc_reset4",       NES_BMC_4IN1RESET);  // mapper 60 with 64k prg and 32k chr
	device.option_add_internal("bmc_reset42",      NES_BMC_42IN1RESET); // mapper 60? or 226? or 233?
// misc multigame cart MMC3 clone boards
	device.option_add_internal("fk23c",            NES_FK23C);
	device.option_add_internal("fk23ca",           NES_FK23CA);
	device.option_add_internal("s24in1c03",        NES_S24IN1SC03);
	device.option_add_internal("bmc_15in1",        NES_BMC_15IN1);
	device.option_add_internal("bmc_sbig7in1",     NES_BMC_SBIG7);
	device.option_add_internal("bmc_hik8in1",      NES_BMC_HIK8);
	device.option_add_internal("bmc_hik4in1",      NES_BMC_HIK4);
	device.option_add_internal("bmc_mario7in1",    NES_BMC_MARIO7IN1);
	device.option_add_internal("bmc_gold7in1",     NES_BMC_GOLD7IN1);
	device.option_add_internal("bmc_gc6in1",       NES_BMC_GC6IN1);
	device.option_add_internal("bmc_411120c",      NES_BMC_411120C);
	device.option_add_internal("bmc_830118c",      NES_BMC_830118C);
	device.option_add_internal("pjoy84",           NES_PJOY84);
	device.option_add_internal("nocash_nochr",     NES_NOCHR);
	device.option_add_internal("nes_action53",     NES_ACTION53);
	device.option_add_internal("nes_2a03pur",      NES_2A03PURITANS);
// other unsupported...
	device.option_add_internal("ninjaryu",         NES_NROM);    // mapper 111 - UNSUPPORTED
	device.option_add_internal("unl_dance",        NES_NROM);    // UNSUPPORTED
	device.option_add_internal("onebus",           NES_NROM);    // UNSUPPORTED
	device.option_add_internal("pec586",           NES_NROM);    // UNSUPPORTED
	device.option_add_internal("coolboy",          NES_NROM);    // UNSUPPORTED
	device.option_add_internal("bmc_f15",          NES_NROM);    // UNSUPPORTED
	device.option_add_internal("bmc_hp898f",       NES_NROM);    // UNSUPPORTED
	device.option_add_internal("bmc_8in1",         NES_NROM);    // UNSUPPORTED
	device.option_add_internal("unl_eh8813a",      NES_NROM);    // UNSUPPORTED
	device.option_add_internal("unl_158b",         NES_NROM);    // UNSUPPORTED
	device.option_add_internal("unl_drgnfgt",      NES_NROM);    // UNSUPPORTED
// are there dumps of games with these boards?
	device.option_add_internal("bmc_hik_kof",      NES_NROM); // mapper 251 - UNSUPPORTED
	device.option_add_internal("bmc_13in1jy110",   NES_NROM); //  [mentioned in FCEUMM source - we need more info] - UNSUPPORTED
	device.option_add_internal("bmc_gk_192",       NES_NROM); //  [mentioned in FCEUMM source - we need more info] - UNSUPPORTED
	device.option_add_internal("konami_qtai",      NES_NROM); //  [mentioned in FCEUMM source - we need more info] - UNSUPPORTED
	device.option_add_internal("unl_3d_block",     NES_NROM); //  [mentioned in FCEUMM source - we need more info] - UNSUPPORTED
	device.option_add_internal("unl_c_n22m",       NES_NROM); //  [mentioned in FCEUMM source - we need more info] - UNSUPPORTED
	device.option_add_internal("a9746",            NES_NROM); // mapper 219 - UNSUPPORTED (no dump available);
// legacy boards for FFE copier mappers (are there images available to fix/improve emulation?)
	device.option_add_internal("ffe3",             NES_FFE3);
	device.option_add_internal("ffe4",             NES_FFE4);
	device.option_add_internal("ffe8",             NES_FFE8);
	device.option_add_internal("test",             NES_NROM);
//
	device.option_add_internal("unknown",          NES_NROM);  //  a few pirate dumps uses the wrong mapper...
}

void disksys_only(device_slot_interface &device)
{
	// RAM expansion + Disk System add-on
	device.option_add("disksys",                   NES_DISKSYS);
}
