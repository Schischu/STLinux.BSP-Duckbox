/*
	STV0900/0903 Multistandard Broadcast Frontend driver
	Copyright (C) Manu Abraham <abraham.manu@gmail.com>

	Copyright (C) ST Microelectronics

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/dvb/version.h>
#include <asm/io.h> /* ctrl_xy */
#include <linux/dvb/frontend.h>
#include <linux/dvb/dvb_frontend.h>
#include "stv6110x.h" /* for demodulator internal modes */
#include "stv090x_reg.h"
#include "stv090x.h"
#include "stv090x_priv.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
#  include <linux/stpio.h>
#else
#  include <linux/stm/pio.h>
#endif

extern short paramDebug;
#define TAGDEBUG "[stv090x] "

#define dprintk(level, x...) do { \
if ((paramDebug) && (paramDebug > level)) printk(TAGDEBUG x); \
} while (0)

static unsigned int verbose = FE_DEBUGREG;

#if defined(SPARK)
static struct stpio_pin*	fe_lnb_13_18;
static struct stpio_pin*	fe_lnb_14_19;
static struct stpio_pin*	fe_lnb_on_off;
#endif

#if defined(CONFIG_KERNELVERSION)  && defined(FORTIS_HDBOX)/* STLinux 2.3 */
void ctrl_fn_using_non_p3_address(void)
{
	printk("ctrl_fn_using_non_p3_address FRONTEND OK\n");
}
#endif

struct mutex demod_lock;

/* DVBS1 and DSS C/N Lookup table */
static const struct stv090x_tab stv090x_s1cn_tab[] = {
	{   0, 8917 }, /*  0.0dB */
	{   5, 8801 }, /*  0.5dB */
	{  10, 8667 }, /*  1.0dB */
	{  15, 8522 }, /*  1.5dB */
	{  20, 8355 }, /*  2.0dB */
	{  25, 8175 }, /*  2.5dB */
	{  30, 7979 }, /*  3.0dB */
	{  35, 7763 }, /*  3.5dB */
	{  40, 7530 }, /*  4.0dB */
	{  45, 7282 }, /*  4.5dB */
	{  50, 7026 }, /*  5.0dB */
	{  55, 6781 }, /*  5.5dB */
	{  60, 6514 }, /*  6.0dB */
	{  65, 6241 }, /*  6.5dB */
	{  70, 5965 }, /*  7.0dB */
	{  75, 5690 }, /*  7.5dB */
	{  80, 5424 }, /*  8.0dB */
	{  85, 5161 }, /*  8.5dB */
	{  90, 4902 }, /*  9.0dB */
	{  95, 4654 }, /*  9.5dB */
	{ 100, 4417 }, /* 10.0dB */
	{ 105, 4186 }, /* 10.5dB */
	{ 110, 3968 }, /* 11.0dB */
	{ 115, 3757 }, /* 11.5dB */
	{ 120, 3558 }, /* 12.0dB */
	{ 125, 3366 }, /* 12.5dB */
	{ 130, 3185 }, /* 13.0dB */
	{ 135, 3012 }, /* 13.5dB */
	{ 140, 2850 }, /* 14.0dB */
	{ 145, 2698 }, /* 14.5dB */
	{ 150, 2550 }, /* 15.0dB */
	{ 160, 2283 }, /* 16.0dB */
	{ 170, 2042 }, /* 17.0dB */
	{ 180, 1827 }, /* 18.0dB */
	{ 190, 1636 }, /* 19.0dB */
	{ 200, 1466 }, /* 20.0dB */
	{ 210, 1315 }, /* 21.0dB */
	{ 220, 1181 }, /* 22.0dB */
	{ 230, 1064 }, /* 23.0dB */
	{ 240,	960 }, /* 24.0dB */
	{ 250,	869 }, /* 25.0dB */
	{ 260,	792 }, /* 26.0dB */
	{ 270,	724 }, /* 27.0dB */
	{ 280,	665 }, /* 28.0dB */
	{ 290,	616 }, /* 29.0dB */
	{ 300,	573 }, /* 30.0dB */
	{ 310,	537 }, /* 31.0dB */
	{ 320,	507 }, /* 32.0dB */
	{ 330,	483 }, /* 33.0dB */
	{ 400,	398 }, /* 40.0dB */
	{ 450,	381 }, /* 45.0dB */
	{ 500,	377 }  /* 50.0dB */
};

/* DVBS2 C/N Lookup table */
static const struct stv090x_tab stv090x_s2cn_tab[] = {
	{ -30, 13348 }, /* -3.0dB */
	{ -20, 12640 }, /* -2d.0B */
	{ -10, 11883 }, /* -1.0dB */
	{   0, 11101 }, /* -0.0dB */
	{   5, 10718 }, /*  0.5dB */
	{  10, 10339 }, /*  1.0dB */
	{  15,  9947 }, /*  1.5dB */
	{  20,  9552 }, /*  2.0dB */
	{  25,  9183 }, /*  2.5dB */
	{  30,  8799 }, /*  3.0dB */
	{  35,  8422 }, /*  3.5dB */
	{  40,  8062 }, /*  4.0dB */
	{  45,  7707 }, /*  4.5dB */
	{  50,  7353 }, /*  5.0dB */
	{  55,  7025 }, /*  5.5dB */
	{  60,  6684 }, /*  6.0dB */
	{  65,  6331 }, /*  6.5dB */
	{  70,  6036 }, /*  7.0dB */
	{  75,  5727 }, /*  7.5dB */
	{  80,  5437 }, /*  8.0dB */
	{  85,  5164 }, /*  8.5dB */
	{  90,  4902 }, /*  9.0dB */
	{  95,  4653 }, /*  9.5dB */
	{ 100,  4408 }, /* 10.0dB */
	{ 105,  4187 }, /* 10.5dB */
	{ 110,  3961 }, /* 11.0dB */
	{ 115,  3751 }, /* 11.5dB */
	{ 120,  3558 }, /* 12.0dB */
	{ 125,  3368 }, /* 12.5dB */
	{ 130,  3191 }, /* 13.0dB */
	{ 135,  3017 }, /* 13.5dB */
	{ 140,  2862 }, /* 14.0dB */
	{ 145,  2710 }, /* 14.5dB */
	{ 150,  2565 }, /* 15.0dB */
	{ 160,  2300 }, /* 16.0dB */
	{ 170,  2058 }, /* 17.0dB */
	{ 180,  1849 }, /* 18.0dB */
	{ 190,  1663 }, /* 19.0dB */
	{ 200,  1495 }, /* 20.0dB */
	{ 210,  1349 }, /* 21.0dB */
	{ 220,  1222 }, /* 22.0dB */
	{ 230,  1110 }, /* 23.0dB */
	{ 240,  1011 }, /* 24.0dB */
	{ 250,   925 }, /* 25.0dB */
	{ 260,   853 }, /* 26.0dB */
	{ 270,   789 }, /* 27.0dB */
	{ 280,   734 }, /* 28.0dB */
	{ 290,   690 }, /* 29.0dB */
	{ 300,   650 }, /* 30.0dB */
	{ 310,   619 }, /* 31.0dB */
	{ 320,   593 }, /* 32.0dB */
	{ 330,   571 }, /* 33.0dB */
	{ 400,   498 }, /* 40.0dB */
	{ 450,	 484 }, /* 45.0dB */
	{ 500,	 481 }	/* 50.0dB */
};

/* RF level C/N lookup table */
static const struct stv090x_tab stv090x_rf_tab[] = {
	{  -5, 0xcaa1 }, /*  -5dBm */
	{ -10, 0xc229 }, /* -10dBm */
	{ -15, 0xbb08 }, /* -15dBm */
	{ -20, 0xb4bc }, /* -20dBm */
	{ -25, 0xad5a }, /* -25dBm */
	{ -30, 0xa298 }, /* -30dBm */
	{ -35, 0x98a8 }, /* -35dBm */
	{ -40, 0x8389 }, /* -40dBm */
	{ -45, 0x59be }, /* -45dBm */
	{ -50, 0x3a14 }, /* -50dBm */
	{ -55, 0x2d11 }, /* -55dBm */
	{ -60, 0x210d }, /* -60dBm */
	{ -65, 0xa14f }, /* -65dBm */
	{ -70, 0x07aa }	 /* -70dBm */
};

#if 0
static struct stv090x_reg stv090x_defval[] = {

	{ STV090x_MID,			0x20 }, /* MID */
	{ STV090x_DACR1,		0x00 }, /* DACR1 */
	{ STV090x_DACR2,		0x00 }, /* DACR2 */
	{ STV090x_OUTCFG,		0x00 }, /* OUTCFG */
	{ STV090x_MODECFG,		0xff }, /* MODECFG */
	{ STV090x_IRQSTATUS3,		0x00 }, /* IRQSTATUS3 */
	{ STV090x_IRQSTATUS2,    	0x00 }, /* IRQSTATUS2 */
	{ STV090x_IRQSTATUS1,    	0x00 }, /* IRQSTATUS1 */
	{ STV090x_IRQSTATUS0,    	0x00 }, /* IRQSTATUS0 */
	{ STV090x_IRQMASK3,    		0x3f }, /* IRQMASK3 */
	{ STV090x_IRQMASK2,    		0xff }, /* IRQMASK2 */
	{ STV090x_IRQMASK1,    		0xff }, /* IRQMASK1 */
	{ STV090x_IRQMASK0,    		0xff }, /* IRQMASK0 */
	{ STV090x_I2CCFG,    		0x08 }, /* I2CCFG */
	{ STV090x_P1_I2CRPT,    	0x44 }, /* P1_I2CRPT */
	{ STV090x_P2_I2CRPT,    	0x44 }, /* P2_I2CRPT */
	{ STV090x_CLKI2CFG,    		0x82 }, /* CLKI2CFG */
	{ STV090x_GPIO1CFG,    		0x82 }, /* GPIO1CFG */
	{ STV090x_GPIO2CFG,    		0x82 }, /* GPIO2CFG */
	{ STV090x_GPIO3CFG,    		0x82 }, /* GPIO3CFG */
	{ STV090x_GPIO4CFG,    		0x82 }, /* GPIO4CFG */
	{ STV090x_GPIO5CFG,    		0x82 }, /* GPIO5CFG */
	{ STV090x_GPIO6CFG,    		0x82 }, /* GPIO6CFG */
	{ STV090x_GPIO7CFG,    		0x82 }, /* GPIO7CFG */
	{ STV090x_GPIO8CFG,    		0x82 }, /* GPIO8CFG */
	{ STV090x_GPIO9CFG,    		0x82 }, /* GPIO9CFG */
	{ STV090x_GPIO10CFG,    	0x82 }, /* GPIO10CFG */
	{ STV090x_GPIO11CFG,   		0x82 }, /* GPIO11CFG */
	{ STV090x_GPIO12CFG,    	0x82 }, /* GPIO12CFG */
	{ STV090x_GPIO13CFG,    	0x82 }, /* GPIO13CFG */
	{ STV090x_CS0CFG,    		0x82 }, /* CS0CFG */
	{ STV090x_CS1CFG,    		0x82 }, /* CS1CFG */
	{ STV090x_STDBYCFG,    		0x82 }, /* STDBYCFG */
	{ STV090x_DIRCLKCFG,    	0x82 }, /* DIRCLKCFG */
	{ STV090x_AGCRF1CFG,    	0x11 }, /* AGCRF1CFG */
	{ STV090x_SDAT1CFG,    		0xb0 }, /* SDAT1CFG */
	{ STV090x_SCLT1CFG,    		0xb2 }, /* SCLT1CFG */
	{ STV090x_DISEQCO1CFG,   	0x14 }, /* DISEQCO1CFG */
	{ STV090x_AGCRF2CFG,    	0x13 }, /* AGCRF2CFG */
	{ STV090x_SDAT2CFG,    		0xb4 }, /* SDAT2CFG */
	{ STV090x_SCLT2CFG,    		0xb6 }, /* SCLT2CFG */
	{ STV090x_DISEQCO2CFG,   	0x16 }, /* DISEQCO2CFG */
	{ STV090x_CLKOUT27CFG,   	0x7e }, /* ERROR1CFG */
	{ STV090x_DPN1CFG,    		0xc0 }, /* DPN1CFG */
	{ STV090x_STROUT1CFG,    	0xc2 }, /* STROUT1CFG */
	{ STV090x_CLKOUT1CFG,    	0xc6 }, /* CLKOUT1CFG */
	{ STV090x_DATA71CFG,    	0xc8 }, /* DATA71CFG */
	{ STV090x_ERROR2CFG,    	0xce }, /* ERROR2CFG */
	{ STV090x_DPN2CFG,    		0xca }, /* DPN2CFG */
	{ STV090x_STROUT2CFG,    	0xcc }, /* STROUT2CFG */
	{ STV090x_CLKOUT2CFG,    	0xd0 }, /* CLKOUT2CFG */
	{ STV090x_DATA72CFG,    	0xd2 }, /* DATA72CFG */
	{ STV090x_ERROR3CFG,    	0xd8 }, /* ERROR3CFG */
	{ STV090x_DPN3CFG,    		0xd4 }, /* DPN3CFG */
	{ STV090x_STROUT3CFG,    	0xd6 }, /* STROUT3CFG */
	{ STV090x_CLKOUT3CFG,    	0xda }, /* CLKOUT3CFG */
	{ STV090x_DATA73CFG,    	0xdc }, /* DATA73CFG */
	{ STV090x_FSKTFC2,    		0x8c }, /* FSKTFC2 */
	{ STV090x_FSKTFC1,    		0x75 }, /* FSKTFC1 */
	{ STV090x_FSKTFC0,    		0xc2 }, /* FSKTFC0 */
	{ STV090x_FSKTDELTAF1,   	0x02 }, /* FSKTDELTAF1 */
	{ STV090x_FSKTDELTAF0,   	0x0c }, /* FSKTDELTAF0 */
	{ STV090x_FSKTCTRL,    		0x04 }, /* FSKTCTRL */
	{ STV090x_FSKRFC2,    		0x10 }, /* FSKRFC2 */
	{ STV090x_FSKRFC1,    		0x75 }, /* FSKRFC1 */
	{ STV090x_FSKRFC0,    		0xc2 }, /* FSKRFC0 */
	{ STV090x_FSKRK1,    		0x3a }, /* FSKRK1 */
	{ STV090x_FSKRK2,    		0x74 }, /* FSKRK2 */
	{ STV090x_FSKRAGCR,    		0x28 }, /* FSKRAGCR */
	{ STV090x_FSKRAGC,    		0xff }, /* FSKRAGC */
	{ STV090x_FSKRALPHA,    	0x17 }, /* FSKRALPHA */
	{ STV090x_FSKRPLTH1,    	0x80 }, /* FSKRPLTH1 */
	{ STV090x_FSKRPLTH0,    	0x00 }, /* FSKRPLTH0 */
	{ STV090x_FSKRDF1,    		0x80 }, /* FSKRDF1 */
	{ STV090x_FSKRDF0,    		0x00 }, /* FSKRDF0 */
	{ STV090x_FSKRSTEPP,    	0x30 }, /* FSKRSTEPP */
	{ STV090x_FSKRSTEPM,    	0x70 }, /* FSKRSTEPM */
	{ STV090x_FSKRDET1,    		0x00 }, /* FSKRDET1 */
	{ STV090x_FSKRDET0,    		0x00 }, /* FSKRDET0 */
	{ STV090x_FSKRDTH1,    		0x11 }, /* FSKRDTH1 */
	{ STV090x_FSKRDTH0,    		0x89 }, /* FSKRDTH0 */
	{ STV090x_FSKRLOSS,    		0x06 }, /* FSKRLOSS */
	{ STV090x_P2_DISTXCTL,   	0x22 }, /* P2_DISTXCTL */
	{ STV090x_P2_DISRXCTL,   	0x80 }, /* P2_DISRXCTL */
	{ STV090x_P2_DISRX_ST0,  	0x04 }, /* P2_DISRX_ST0 */
	{ STV090x_P2_DISRX_ST1,  	0x00 }, /* P2_DISRX_ST1 */
	{ STV090x_P2_DISRXDATA,  	0x00 }, /* P2_DISRXDATA */
	{ STV090x_P2_DISTXDATA,  	0x00 }, /* P2_DISTXDATA */
	{ STV090x_P2_DISTXSTATUS,	0x20 }, /* P2_DISTXSTATUS */
	{ STV090x_P2_F22TX,    		0xc0 }, /* P2_F22TX */
	{ STV090x_P2_F22RX,    		0xc0 }, /* P2_F22RX */
	{ STV090x_P2_ACRPRESC,   	0x01 }, /* P2_ACRPRESC */
	{ STV090x_P2_ACRDIV,    	0x14 }, /* P2_ACRDIV */
	{ STV090x_P1_DISTXCTL,   	0x22 }, /* P1_DISTXCTL */
	{ STV090x_P1_DISRXCTL,   	0x80 }, /* P1_DISRXCTL */
	{ STV090x_P1_DISRX_ST0,  	0x14 }, /* P1_DISRX_ST0 */
	{ STV090x_P1_DISRX_ST1,  	0x80 }, /* P1_DISRX_ST1 */
	{ STV090x_P1_DISRXDATA,  	0x00 }, /* P1_DISRXDATA */
	{ STV090x_P1_DISTXDATA,  	0x00 }, /* P1_DISTXDATA */
	{ STV090x_P1_DISTXSTATUS,	0x20 }, /* P1_DISTXSTATUS */
	{ STV090x_P1_F22TX,    		0xc0 }, /* P1_F22TX */
	{ STV090x_P1_F22RX,    		0xc0 }, /* P1_F22RX */
	{ STV090x_P1_ACRPRESC,   	0x01 }, /* P1_ACRPRESC */
	{ STV090x_P1_ACRDIV,    	0x14 }, /* P1_ACRDIV */
	{ STV090x_NCOARSE,    		0x13 }, /* NCOARSE */
	{ STV090x_SYNTCTRL,    		0x22 }, /* SYNTCTRL (default CLKI) */
	{ STV090x_FILTCTRL,    		0x01 }, /* FILTCTRL */
	{ STV090x_PLLSTAT,    		0xc0 }, /* PLLSTAT */
	{ STV090x_STOPCLK1,    		0x00 }, /* STOPCLK1 */
	{ STV090x_STOPCLK2,    		0x00 }, /* STOPCLK2 */
	{ STV090x_TSTTNR0,    		0x04 }, /* TSTTNR0 */
	{ STV090x_TSTTNR1,    		0x27 }, /* TSTTNR1 */
	{ STV090x_TSTTNR2,    		0x21 }, /* TSTTNR2 */
	{ STV090x_TSTTNR3,    		0x27 }, /* TSTTNR3 */
	{ STV090x_TSTTNR4,    		0x21 }, /* TSTTNR4 */
	{ STV090x_P2_IQCONST,    	0x00 }, /* P2_IQCONST */
	{ STV090x_P2_NOSCFG,    	0x1c }, /* P2_NOSCFG */
	{ STV090x_P2_ISYMB,    		0x27 }, /* P2_ISYMB */
	{ STV090x_P2_QSYMB,    		0x2a }, /* P2_QSYMB */
	{ STV090x_P2_AGC1CFG,    	0x54 }, /* P2_AGC1CFG */
	{ STV090x_P2_AGC1CN,    	0xd9 }, /* P2_AGC1CN */
	{ STV090x_P2_AGC1REF,    	0x58 }, /* P2_AGC1REF */
	{ STV090x_P2_IDCCOMP,    	0x00 }, /* P2_IDCCOMP */
	{ STV090x_P2_QDCCOMP,    	0xfd }, /* P2_QDCCOMP */
	{ STV090x_P2_POWERI,    	0x0e }, /* P2_POWERI */
	{ STV090x_P2_POWERQ,   		0x0c }, /* P2_POWERQ */
	{ STV090x_P2_AGC1AMM,    	0x19 }, /* P2_AGC1AMM */
	{ STV090x_P2_AGC1QUAD,   	0xf9 }, /* P2_AGC1QUAD */
	{ STV090x_P2_AGCIQIN1,   	0x00 }, /* P2_AGCIQIN1 */
	{ STV090x_P2_AGCIQIN0,   	0x00 }, /* P2_AGCIQIN0 */
	{ STV090x_P2_DEMOD,    		0x08 }, /* P2_DEMOD */
	{ STV090x_P2_DMDMODCOD,  	0x10 }, /* P2_DMDMODCOD */
	{ STV090x_P2_DSTATUS,    	0x10 }, /* P2_DSTATUS */
	{ STV090x_P2_DSTATUS2,   	0x88 }, /* P2_DSTATUS2 */
	{ STV090x_P2_DMDCFGMD,   	0xf9 }, /* P2_DMDCFGMD */
	{ STV090x_P2_DMDCFG2,    	0x3b }, /* P2_DMDCFG2 */
	{ STV090x_P2_DMDISTATE,  	0x5c }, /* P2_DMDISTATE */
	{ STV090x_P2_DMDTOM,    	0x20 }, /* P2_DMDT0M */
	{ STV090x_P2_DMDSTATE,   	0x1c }, /* P2_DMDSTATE */
	{ STV090x_P2_DMDFLYW,    	0x00 }, /* P2_DMDFLYW */
	{ STV090x_P2_DSTATUS3,   	0x00 }, /* P2_DSTATUS3 */
	{ STV090x_P2_DMDCFG3,    	0x68 }, /* P2_DMDCFG3 */
	{ STV090x_P2_DMDCFG4,    	0x10 }, /* P2_DMDCFG4 */
	{ STV090x_P2_CORRELMANT, 	0x70 }, /* P2_CORRELMANT */
	{ STV090x_P2_CORRELABS,  	0x88 }, /* P2_CORRELABS */
	{ STV090x_P2_CORRELEXP,  	0xaa }, /* P2_CORRELEXP */
	{ STV090x_P2_PLHMODCOD,  	0x42 }, /* P2_PLHMODCOD */
	{ STV090x_P2_AGCK32,   		0x00 }, /* P2_AGCK32 */
	{ STV090x_P2_AGC2O,    		0x5b }, /* P2_AGC2O */
	{ STV090x_P2_AGC2REF,    	0x38 }, /* P2_AGC2REF */
	{ STV090x_P2_AGC1ADJ,    	0x58 }, /* P2_AGC1ADJ */
	{ STV090x_P2_AGC2I1,    	0x00 }, /* P2_AGC2I1 */
	{ STV090x_P2_AGC2I0,    	0x60 }, /* P2_AGC2I0 */
	{ STV090x_P2_CARCFG,    	0xe4 }, /* P2_CARCFG */
	{ STV090x_P2_ACLC,    		0x1a }, /* P2_ACLC */
	{ STV090x_P2_BCLC,    		0x09 }, /* P2_BCLC */
	{ STV090x_P2_CARFREQ,    	0x38 }, /* P2_CARFREQ */
	{ STV090x_P2_CARHDR,    	0x20 }, /* P2_CARHDR */
	{ STV090x_P2_LDT,    		0xd0 }, /* P2_LDT */
	{ STV090x_P2_LDT2,    		0xb0 }, /* P2_LDT2 */
	{ STV090x_P2_CFRICFG,    	0xf8 }, /* P2_CFRICFG */
	{ STV090x_P2_CFRUP1,    	0x3f }, /* P2_CFRUP1 */
	{ STV090x_P2_CFRUP0,    	0xff }, /* P2_CFRUP0 */
	{ STV090x_P2_CFRLOW1,    	0xc0 }, /* P2_CFRLOW1 */
	{ STV090x_P2_CFRLOW0,    	0x01 }, /* P2_CFRLOW0 */
	{ STV090x_P2_CFRINIT1,   	0xfc }, /* P2_CFRINIT1 */
	{ STV090x_P2_CFRINIT0,   	0xff }, /* P2_CFRINIT0 */
	{ STV090x_P2_CFRINC1,    	0x03 }, /* P2_CFRINC1 */
	{ STV090x_P2_CFRINC0,    	0x00 }, /* P2_CFRINC0 */
	{ STV090x_P2_CFR2,    		0xfc }, /* P2_CFR2 */
	{ STV090x_P2_CFR1,    		0xff }, /* P2_CFR1 */
	{ STV090x_P2_CFR0,    		0x00 }, /* P2_CFR0 */
	{ STV090x_P2_LDI,    		0x80 }, /* P2_LDI */
	{ STV090x_P2_TMGCFG,    	0xd2 }, /* P2_TMGCFG */
	{ STV090x_P2_RTC,    		0x88 }, /* P2_RTC */
	{ STV090x_P2_RTCS2,    		0x66 }, /* P2_RTCS2 */
	{ STV090x_P2_TMGTHRISE,  	0x20 }, /* P2_TMGTHRISE */
	{ STV090x_P2_TMGTHFALL,  	0x00 }, /* P2_TMGTHFALL */
	{ STV090x_P2_SFRUPRATIO, 	0xf0 }, /* P2_SFRUPRATIO */
	{ STV090x_P2_SFRLOWRATIO,	0x70 }, /* P2_SFRLOWRATIO */
	{ STV090x_P2_KREFTMG,    	0x31 }, /* P2_KREFTMG */
	{ STV090x_P2_SFRSTEP,    	0x58 }, /* P2_SFRSTEP */
	{ STV090x_P2_TMGCFG2,    	0x01 }, /* P2_TMGCFG2 */
	{ STV090x_P2_SFRINIT1,   	0x03 }, /* P2_SFRINIT1 */
	{ STV090x_P2_SFRINIT0,   	0x01 }, /* P2_SFRINIT0 */
	{ STV090x_P2_SFRUP1,    	0x83 }, /* P2_SFRUP1 */
	{ STV090x_P2_SFRUP0,    	0xc0 }, /* P2_SFRUP0 */
	{ STV090x_P2_SFRLOW1,    	0x82 }, /* P2_SFRLOW1 */
	{ STV090x_P2_SFRLOW0,    	0xa0 }, /* P2_SFRLOW0 */
	{ STV090x_P2_SFR3,    		0x03 }, /* P2_SFR3 */
	{ STV090x_P2_SFR2,    		0x01 }, /* P2_SFR2 */
	{ STV090x_P2_SFR1,    		0x00 }, /* P2_SFR1 */
	{ STV090x_P2_SFR0,    		0x00 }, /* P2_SFR0 */
	{ STV090x_P2_TMGREG2,    	0x00 }, /* P2_TMGREG2 */
	{ STV090x_P2_TMGREG1,    	0x00 }, /* P2_TMGREG1 */
	{ STV090x_P2_TMGREG0,    	0x00 }, /* P2_TMGREG0 */
	{ STV090x_P2_TMGLOCK1,   	0x00 }, /* P2_TMGLOCK1 */
	{ STV090x_P2_TMGLOCK0,   	0x00 }, /* P2_TMGLOCK0 */
	{ STV090x_P2_TMGOBS,    	0x10 }, /* P2_TMGOBS */
	{ STV090x_P2_EQUALCFG,   	0x41 }, /* P2_EQUALCFG */
	{ STV090x_P2_EQUAI1,   		0xf0 }, /* P2_EQUAI1 */
	{ STV090x_P2_EQUAQ1,    	0x00 }, /* P2_EQUAQ1 */
	{ STV090x_P2_EQUAI2,    	0xf0 }, /* P2_EQUAI2 */
	{ STV090x_P2_EQUAQ2,    	0x00 }, /* P2_EQUAQ2 */
	{ STV090x_P2_EQUAI3,    	0xf0 }, /* P2_EQUAI3 */
	{ STV090x_P2_EQUAQ3,    	0x00 }, /* P2_EQUAQ3 */
	{ STV090x_P2_EQUAI4,    	0xf0 }, /* P2_EQUAI4 */
	{ STV090x_P2_EQUAQ4,    	0x00 }, /* P2_EQUAQ4 */
	{ STV090x_P2_EQUAI5,    	0xf0 }, /* P2_EQUAI5 */
	{ STV090x_P2_EQUAQ5,    	0x00 }, /* P2_EQUAQ5 */
	{ STV090x_P2_EQUAI6,    	0xf0 }, /* P2_EQUAI6 */
	{ STV090x_P2_EQUAQ6,    	0x00 }, /* P2_EQUAQ6 */
	{ STV090x_P2_EQUAI7,    	0xf0 }, /* P2_EQUAI7 */
	{ STV090x_P2_EQUAQ7,    	0x00 }, /* P2_EQUAQ7 */
	{ STV090x_P2_EQUAI8,    	0xf0 }, /* P2_EQUAI8 */
	{ STV090x_P2_EQUAQ8,    	0x00 }, /* P2_EQUAQ8 */
	{ STV090x_P2_NNOSDATAT1, 	0x01 }, /* P2_NNOSDATAT1 */
	{ STV090x_P2_NNOSDATAT0, 	0x65 }, /* P2_NNOSDATAT0 */
	{ STV090x_P2_NNOSDATA1,  	0x00 }, /* P2_NNOSDATA1 */
	{ STV090x_P2_NNOSDATA0,  	0x0d }, /* P2_NNOSDATA0 */
	{ STV090x_P2_NNOSPLHT1,  	0x4d }, /* P2_NNOSPLHT1 */
	{ STV090x_P2_NNOSPLHT0,  	0x6a }, /* P2_NNOSPLHT0 */
	{ STV090x_P2_NNOSPLH1,   	0x57 }, /* P2_NNOSPLH1 */
	{ STV090x_P2_NNOSPLH0,   	0x52 }, /* P2_NNOSPLH0 */
	{ STV090x_P2_NOSDATAT1,  	0x01 }, /* P2_NOSDATAT1 */
	{ STV090x_P2_NOSDATAT0,  	0x39 }, /* P2_NOSDATAT0 */
	{ STV090x_P2_NOSDATA1,   	0x00 }, /* P2_NOSDATA1 */
	{ STV090x_P2_NOSDATA0,   	0x0a }, /* P2_NOSDATA0 */
	{ STV090x_P2_NOSPLHT1,   	0x43 }, /* P2_NOSPLHT1 */
	{ STV090x_P2_NOSPLHT0,   	0xbd }, /* P2_NOSPLHT0 */
	{ STV090x_P2_NNOSPLH1,    	0x42 }, /* P2_NOSPLH1 */
	{ STV090x_P2_NNOSPLH0,    	0xdb }, /* P2_NOSPLH0 */
	{ STV090x_P2_CAR2CFG,    	0x26 }, /* P2_CAR2CFG */
	{ STV090x_P2_ACLC2,    		0x28 }, /* P2_ACLC2 */
	{ STV090x_P2_BCLC2,    		0x25 }, /* P2_BCLC2 */
#if 0
	{ STV090x_P2_CFR22,    		0x00 }, /* P2_CFR22 */
	{ STV090x_P2_CFR21,    		0x00 }, /* P2_CFR21 */
	{ STV090x_P2_CFR20,    		0x00 }, /* P2_CFR20 */
#endif
	{ STV090x_P2_ACLC2S2Q,   	0x6a }, /* P2_ACLC2S2Q */
	{ STV090x_P2_ACLC2S28,   	0x58 }, /* P2_ACLC2S28 */
	{ STV090x_P2_ACLC2S216A, 	0x68 }, /* P2_ACLC2S216A */
	{ STV090x_P2_ACLC2S232A, 	0x68 }, /* P2_ACLC2S232A */

	{ STV090x_P2_BCLC2S2Q,   	0x86 }, /* P2_BCLC2S2Q */
	{ STV090x_P2_BCLC2S28,   	0x86 }, /* P2_BCLC2S28 */
	{ STV090x_P2_BCLC2S216A, 	0xa5 }, /* P2_BCLC2S216A */
	{ STV090x_P2_BCLC2S232A, 	0xa5 }, /* P2_BCLC2S232A */

	{ STV090x_P2_PLROOT2,    	0x00 }, /* P2_PLROOT2 */
	{ STV090x_P2_FECM,    		0x10 }, /* P2_FECM */
	{ STV090x_P2_PLROOT1,    	0x00 }, /* P2_PLROOT1 */
	{ STV090x_P2_PLROOT0,    	0x01 }, /* P2_PLROOT0 */
	{ STV090x_P2_MODCODLST0, 	0xff }, /* P2_MODCODLST0 */
	{ STV090x_P2_MODCODLST1, 	0xff }, /* P2_MODCODLST1 */
	{ STV090x_P2_MODCODLST2, 	0xff }, /* P2_MODCODLST2 */
	{ STV090x_P2_MODCODLST3, 	0xff }, /* P2_MODCODLST3 */
	{ STV090x_P2_MODCODLST4, 	0xff }, /* P2_MODCODLST4 */
	{ STV090x_P2_MODCODLST5, 	0xff }, /* P2_MODCODLST5 */
	{ STV090x_P2_MODCODLST6, 	0xff }, /* P2_MODCODLST6 */
	{ STV090x_P2_MODCODLST7, 	0xcc }, /* P2_MODCODLST7 */
	{ STV090x_P2_MODCODLST8, 	0xcc }, /* P2_MODCODLST8 */
	{ STV090x_P2_MODCODLST9, 	0xcc }, /* P2_MODCODLST9 */
	{ STV090x_P2_MODCODLSTA, 	0xcc }, /* P2_MODCODLSTA */
	{ STV090x_P2_MODCODLSTB, 	0xcc }, /* P2_MODCODLSTB */
	{ STV090x_P2_MODCODLSTC, 	0xcc }, /* P2_MODCODLSTC */
	{ STV090x_P2_MODCODLSTD, 	0xcc }, /* P2_MODCODLSTD */
	{ STV090x_P2_MODCODLSTE, 	0xcc }, /* P2_MODCODLSTE */
	{ STV090x_P2_MODCODLSTF, 	0xcf }, /* P2_MODCODLSTF */
	{ STV090x_P2_DMDRESCFG,  	0x29 }, /* P2_DMDRESCFG */
	{ STV090x_P2_DMDRESADR,  	0x11 }, /* P2_DMDRESADR */
	{ STV090x_P2_DMDRESDATA7,	0x47 }, /* P2_DMDRESDATA7 */
	{ STV090x_P2_DMDRESDATA6,	0x82 }, /* P2_DMDRESDATA6 */
	{ STV090x_P2_DMDRESDATA5,	0x00 }, /* P2_DMDRESDATA5 */
	{ STV090x_P2_DMDRESDATA4,	0x00 }, /* P2_DMDRESDATA4 */
	{ STV090x_P2_DMDRESDATA3,	0x92 }, /* P2_DMDRESDATA3 */
	{ STV090x_P2_DMDRESDATA2,	0x4c }, /* P2_DMDRESDATA2 */
	{ STV090x_P2_DMDRESDATA1,	0x00 }, /* P2_DMDRESDATA1 */
	{ STV090x_P2_DMDRESDATA0,	0x07 }, /* P2_DMDRESDATA0 */
	{ STV090x_P2_FFEI1,  		0x00 }, /* P2_FFEI1 */
	{ STV090x_P2_FFEQ1,  		0x00 }, /* P2_FFEQ1 */
	{ STV090x_P2_FFEI2,  		0x00 }, /* P2_FFEI2 */
	{ STV090x_P2_FFEQ2,  		0x00 }, /* P2_FFEQ2 */
	{ STV090x_P2_FFEI3,  		0x00 }, /* P2_FFEI3 */
	{ STV090x_P2_FFEQ3,  		0x00 }, /* P2_FFEQ3 */
	{ STV090x_P2_FFEI4,  		0x00 }, /* P2_FFEI4 */
	{ STV090x_P2_FFEQ4,  		0x00 }, /* P2_FFEQ4 */
	{ STV090x_P2_FFECFG,    	0x31 }, /* P2_FFECFG */
#if 0
	{ STV090x_P2_TNRCFG,    	0xef }, /* P2_TNRCFG */
	{ STV090x_P2_TNRCFG2,    	0x02 }, /* P2_TNRCFG2 */
	{ STV090x_P2_TNRXTAL,    	0x1b }, /* P2_TNRXTAL */
	{ STV090x_P2_TNRSTEPS,   	0x87 }, /* P2_TNRSTEPS */
	{ STV090x_P2_TNRGAIN,    	0x09 }, /* P2_TNRGAIN */
	{ STV090x_P2_TNRRF1,    	0x47 }, /* P2_TNRRF1 */
	{ STV090x_P2_TNRRF0,    	0x82 }, /* P2_TNRRF0 */
	{ STV090x_P2_TNRBW,    		0x24 }, /* P2_TNRBW */
	{ STV090x_P2_TNRADJ,    	0x1f }, /* P2_TNRADJ */
	{ STV090x_P2_TNRCTL2,    	0x37 }, /* P2_TNRCTL2 */
	{ STV090x_P2_TNRCFG3,    	0x02 }, /* P2_TNRCFG3 */
	{ STV090x_P2_TNRLAUNCH,  	0x00 }, /* P2_TNRLAUNCH */
	{ STV090x_P2_TNRLD,    		0x00 }, /* P2_TNRLD */
	{ STV090x_P2_TNROBSL,    	0x50 }, /* P2_TNROBSL */
	{ STV090x_P2_TNRRESTE,   	0x00 }, /* P2_TNRRESTE */
#endif
	{ STV090x_P2_SMAPCOEF7,  	0x06 }, /* P2_SMAPCOEF7 */
	{ STV090x_P2_SMAPCOEF6,  	0x00 }, /* P2_SMAPCOEF6 */
	{ STV090x_P2_SMAPCOEF5,  	0x04 }, /* P2_SMAPCOEF5 */
	{ STV090x_P2_DMDPLHSTAT, 	0x00 }, /* P2_DMDPLHSTAT */
	{ STV090x_P2_LOCKTIME3,  	0xff }, /* P2_LOCKTIME3 */
	{ STV090x_P2_LOCKTIME2,  	0xf0 }, /* P2_LOCKTIME2 */
	{ STV090x_P2_LOCKTIME1,  	0x5a }, /* P2_LOCKTIME1 */
	{ STV090x_P2_LOCKTIME0,  	0x86 }, /* P2_LOCKTIME0 */
	{ STV090x_P2_VITSCALE,   	0x00 }, /* P2_VITSCALE */
	{ STV090x_P2_FECM,    		0x10 }, /* P2_FECM */
	{ STV090x_P2_VTH12,  		0xd0 }, /* P2_VTH12 */
	{ STV090x_P2_VTH23,  		0x7d }, /* P2_VTH23 */
	{ STV090x_P2_VTH34,  		0x53 }, /* P2_VTH34 */
	{ STV090x_P2_VTH56,  		0x2f }, /* P2_VTH56 */
	{ STV090x_P2_VTH67,  		0x24 }, /* P2_VTH67 */
	{ STV090x_P2_VTH78,  		0x1f }, /* P2_VTH78 */
	{ STV090x_P2_VITCURPUN,  	0x0d }, /* P2_VITCURPUN */
	{ STV090x_P2_VERROR,    	0xff }, /* P2_VERROR */
	{ STV090x_P2_PRVIT,    		0x3f }, /* P2_PRVIT */
	{ STV090x_P2_VAVSRVIT,   	0x00 }, /* P2_VAVSRVIT */
	{ STV090x_P2_VSTATUSVIT, 	0xa7 }, /* P2_VSTATUSVIT */
	{ STV090x_P2_VTHINUSE,   	0x01 }, /* P2_VTHINUSE */
	{ STV090x_P2_KDIV12,  		0x27 }, /* P2_KDIV12 */
	{ STV090x_P2_KDIV23,  		0x32 }, /* P2_KDIV23 */
	{ STV090x_P2_KDIV34,  		0x32 }, /* P2_KDIV34 */
	{ STV090x_P2_KDIV56,  		0x32 }, /* P2_KDIV56 */
	{ STV090x_P2_KDIV67,  		0x32 }, /* P2_KDIV67 */
	{ STV090x_P2_KDIV78,  		0x50 }, /* P2_KDIV78 */
	{ STV090x_P2_PDELCTRL1,  	0x00 }, /* P2_PDELCTRL1 */
	{ STV090x_P2_PDELCTRL2,  	0x20 }, /* P2_PDELCTRL2 */
	{ STV090x_P2_HYSTTHRESH, 	0x41 }, /* P2_HYSTTHRESH */
	{ STV090x_P2_ISIENTRY,   	0x00 }, /* P2_ISIENTRY */
	{ STV090x_P2_ISIBITENA,  	0x00 }, /* P2_ISIBITENA */
	{ STV090x_P2_MATSTR1,    	0xf0 }, /* P2_MATSTR1 */
	{ STV090x_P2_MATSTR0,    	0x00 }, /* P2_MATSTR0 */
	{ STV090x_P2_UPLSTR1,    	0x05 }, /* P2_UPLSTR1 */
	{ STV090x_P2_UPLSTR0,    	0xe0 }, /* P2_UPLSTR0 */
	{ STV090x_P2_DFLSTR1,    	0x7d }, /* P2_DFLSTR1 */
	{ STV090x_P2_DFLSTR0,    	0x80 }, /* P2_DFLSTR0 */
	{ STV090x_P2_SYNCSTR,    	0x47 }, /* P2_SYNCSTR */
	{ STV090x_P2_SYNCDSTR1,  	0x00 }, /* P2_SYNCDSTR1 */
	{ STV090x_P2_SYNCDSTR0,  	0x00 }, /* P2_SYNCDSTR0 */
	{ STV090x_P2_PDELSTATUS1,	0x94 }, /* P2_PDELSTATUS1 */
	{ STV090x_P2_PDELSTATUS2,	0x90 }, /* P2_PDELSTATUS2 */
	{ STV090x_P2_BBFCRCKO1,  	0x00 }, /* P2_BBFCRCKO1 */
	{ STV090x_P2_BBFCRCKO0,  	0x00 }, /* P2_BBFCRCKO0 */
	{ STV090x_P2_UPCRCKO1,   	0x00 }, /* P2_UPCRCKO1 */
	{ STV090x_P2_UPCRCKO0,   	0x00 }, /* P2_UPCRCKO0 */
	{ STV090x_P2_TSSTATEM,   	0xb0 }, /* P2_TSSTATEM */
	{ STV090x_P2_TSCFGH,    	0x40 }, /* P2_TSCFGH */
	{ STV090x_P2_TSCFGM,    	0x00 }, /* P2_TSCFGM */
	{ STV090x_P2_TSCFGL,    	0x20 }, /* P2_TSCFGL */
	{ STV090x_P2_TSINSDELH,  	0x00 }, /* P2_TSINSDELH */
	{ STV090x_P2_TSSPEED,    	0xff }, /* P2_TSSPEED */
	{ STV090x_P2_TSSTATUS,   	0x52 }, /* P2_TSSTATUS */
	{ STV090x_P2_TSSTATUS2,  	0xea }, /* P2_TSSTATUS2 */
	{ STV090x_P2_TSBITRATE1, 	0x00 }, /* P2_TSBITRATE1 */
	{ STV090x_P2_TSBITRATE0, 	0x00 }, /* P2_TSBITRATE0 */
	{ STV090x_P2_ERRCTRL1,   	0x35 }, /* P2_ERRCTRL1 */
	{ STV090x_P2_ERRCNT12,   	0x80 }, /* P2_ERRCNT12 */
	{ STV090x_P2_ERRCNT11,   	0x00 }, /* P2_ERRCNT11 */
	{ STV090x_P2_ERRCNT10,   	0x00 }, /* P2_ERRCNT10 */
	{ STV090x_P2_ERRCTRL2,   	0xc1 }, /* P2_ERRCTRL2 */
	{ STV090x_P2_ERRCNT22,   	0x00 }, /* P2_ERRCNT22 */
	{ STV090x_P2_ERRCNT21,   	0x00 }, /* P2_ERRCNT21 */
	{ STV090x_P2_ERRCNT20,   	0x00 }, /* P2_ERRCNT20 */
	{ STV090x_P2_FECSPY,    	0xa8 }, /* P2_FECSPY */
	{ STV090x_P2_FSPYCFG,    	0x2c }, /* P2_FSPYCFG */
	{ STV090x_P2_FSPYDATA,   	0x3a }, /* P2_FSPYDATA */
	{ STV090x_P2_FSPYOUT,    	0x07 }, /* P2_FSPYOUT */
	{ STV090x_P2_FSTATUS,    	0x00 }, /* P2_FSTATUS */
	{ STV090x_P2_FBERCPT4,   	0x00 }, /* P2_FBERCPT4 */
	{ STV090x_P2_FBERCPT3,   	0x00 }, /* P2_FBERCPT3 */
	{ STV090x_P2_FBERCPT2,   	0x00 }, /* P2_FBERCPT2 */
	{ STV090x_P2_FBERCPT1,   	0x00 }, /* P2_FBERCPT1 */
	{ STV090x_P2_FBERCPT0,   	0x00 }, /* P2_FBERCPT0 */
	{ STV090x_P2_FBERERR2,   	0x00 }, /* P2_FBERERR2 */
	{ STV090x_P2_FBERERR1,   	0x00 }, /* P2_FBERERR1 */
	{ STV090x_P2_FBERERR0,   	0x00 }, /* P2_FBERERR0 */
	{ STV090x_P2_FSPYBER,    	0x10 }, /* P2_FSPYBER */
	{ STV090x_P1_IQCONST,    	0x00 }, /* P1_IQCONST */
	{ STV090x_P1_NOSCFG,   		0x1c }, /* P1_NOSCFG */
	{ STV090x_P1_ISYMB,    		0x29 }, /* P1_ISYMB */
	{ STV090x_P1_QSYMB,    		0x29 }, /* P1_QSYMB */
	{ STV090x_P1_AGC1CFG,    	0x54 }, /* P1_AGC1CFG */
	{ STV090x_P1_AGC1CN,    	0x99 }, /* P1_AGC1CN */
	{ STV090x_P1_AGC1REF,    	0x58 }, /* P1_AGC1REF */
	{ STV090x_P1_IDCCOMP,    	0x03 }, /* P1_IDCCOMP */
	{ STV090x_P1_QDCCOMP,    	0xfe }, /* P1_QDCCOMP */
	{ STV090x_P1_POWERI,    	0x91 }, /* P1_POWERI */
	{ STV090x_P1_POWERQ,    	0x86 }, /* P1_POWERQ */
	{ STV090x_P1_AGC1AMM,    	0x08 }, /* P1_AGC1AMM */
	{ STV090x_P1_AGC1QUAD,   	0x02 }, /* P1_AGC1QUAD */
	{ STV090x_P1_AGCIQIN1,   	0x68 }, /* P1_AGCIQIN1 */
	{ STV090x_P1_AGCIQIN0,   	0x20 }, /* P1_AGCIQIN0 */
	{ STV090x_P1_DEMOD,    		0x08 }, /* P1_DEMOD */
	{ STV090x_P1_DMDMODCOD,  	0x10 }, /* P1_DMDMODCOD */
	{ STV090x_P1_DSTATUS,    	0x10 }, /* P1_DSTATUS */
	{ STV090x_P1_DSTATUS2,   	0x80 }, /* P1_DSTATUS2 */
	{ STV090x_P1_DMDCFGMD,   	0xf9 }, /* P1_DMDCFGMD */
	{ STV090x_P1_DMDCFG2,    	0x3b }, /* P1_DMDCFG2 */
	{ STV090x_P1_DMDISTATE,  	0x5c }, /* P1_DMDISTATE */
	{ STV090x_P1_DMDTOM,    	0x20 }, /* P1_DMDT0M */
	{ STV090x_P1_DMDSTATE,   	0x1c }, /* P1_DMDSTATE */
	{ STV090x_P1_DMDFLYW,    	0x00 }, /* P1_DMDFLYW */
	{ STV090x_P1_DSTATUS3,   	0x00 }, /* P1_DSTATUS3 */
	{ STV090x_P1_DMDCFG3,    	0x68 }, /* P1_DMDCFG3 */
	{ STV090x_P1_DMDCFG4,    	0x10 }, /* P1_DMDCFG4 */
	{ STV090x_P1_CORRELMANT, 	0x70 }, /* P1_CORRELMANT */
	{ STV090x_P1_CORRELABS,  	0x88 }, /* P1_CORRELABS */
	{ STV090x_P1_CORRELEXP,  	0xaa }, /* P1_CORRELEXP */
	{ STV090x_P1_PLHMODCOD,  	0x42 }, /* P1_PLHMODCOD */
	{ STV090x_P1_AGCK32,    	0x00 }, /* P1_AGCK32 */
	{ STV090x_P1_AGC2O,    		0x5b }, /* P1_AGC2O */
	{ STV090x_P1_AGC2REF,    	0x38 }, /* P1_AGC2REF */
	{ STV090x_P1_AGC1ADJ,    	0x58 }, /* P1_AGC1ADJ */
	{ STV090x_P1_AGC2I1,    	0x00 }, /* P1_AGC2I1 */
	{ STV090x_P1_AGC2I0,    	0x60 }, /* P1_AGC2I0 */
	{ STV090x_P1_CARCFG,    	0xe4 }, /* P1_CARCFG */
	{ STV090x_P2_AGC2REF,		0x38 },
	{ STV090x_P1_ACLC,    		0x1a }, /* P1_ACLC */
	{ STV090x_P1_BCLC,    		0x09 }, /* P1_BCLC */
	{ STV090x_P1_CARFREQ,    	0x38 }, /* P1_CARFREQ */
	{ STV090x_P1_CARHDR,    	0x20 }, /* P1_CARHDR */
	{ STV090x_P1_LDT,    		0xd0 }, /* P1_LDT */
	{ STV090x_P1_LDT2,    		0xb0 }, /* P1_LDT2 */
	{ STV090x_P1_CFRICFG,    	0xf8 }, /* P1_CFRICFG */
	{ STV090x_P1_CFRUP1,    	0x3f }, /* P1_CFRUP1 */
	{ STV090x_P1_CFRUP0,    	0xff }, /* P1_CFRUP0 */
	{ STV090x_P1_CFRLOW1,    	0xc0 }, /* P1_CFRLOW1 */
	{ STV090x_P1_CFRLOW0,    	0x01 }, /* P1_CFRLOW0 */
	{ STV090x_P1_CFRINIT1,   	0x03 }, /* P1_CFRINIT1 */
	{ STV090x_P1_CFRINIT0,   	0x00 }, /* P1_CFRINIT0 */
	{ STV090x_P1_CFRINC1,    	0x03 }, /* P1_CFRINC1 */
	{ STV090x_P1_CFRINC0,    	0x00 }, /* P1_CFRINC0 */
	{ STV090x_P1_CFR2,    		0x03 }, /* P1_CFR2 */
	{ STV090x_P1_CFR1,    		0x00 }, /* P1_CFR1 */
	{ STV090x_P1_CFR0,    		0x00 }, /* P1_CFR0 */
	{ STV090x_P1_LDI,    		0x80 }, /* P1_LDI */
	{ STV090x_P1_TMGCFG,    	0xd3 }, /* P1_TMGCFG */
	{ STV090x_P1_RTC,    		0x88 }, /* P1_RTC */
	{ STV090x_P1_RTCS2,    		0x66 }, /* P1_RTCS2 */
	{ STV090x_P1_TMGTHRISE,  	0x20 }, /* P1_TMGTHRISE */
	{ STV090x_P1_TMGTHFALL,  	0x00 }, /* P1_TMGTHFALL */
	{ STV090x_P1_SFRUPRATIO, 	0xf0 }, /* P1_SFRUPRATIO */
	{ STV090x_P1_SFRLOWRATIO,	0x70 }, /* P1_SFRLOWRATIO */
	{ STV090x_P1_KREFTMG,    	0x31 }, /* P1_KREFTMG */
	{ STV090x_P1_SFRSTEP,    	0x58 }, /* P1_SFRSTEP */
	{ STV090x_P1_TMGCFG2,    	0x01 }, /* P1_TMGCFG2 */
	{ STV090x_P1_SFRINIT1,   	0x03 }, /* P1_SFRINIT1 */
	{ STV090x_P1_SFRINIT0,   	0x01 }, /* P1_SFRINIT0 */
	{ STV090x_P1_SFRUP1,    	0x83 }, /* P1_SFRUP1 */
	{ STV090x_P1_SFRUP0,    	0xc0 }, /* P1_SFRUP0 */
	{ STV090x_P2_AGC2REF,		0x38 },
	{ STV090x_P1_SFRLOW1,    	0x82 }, /* P1_SFRLOW1 */
	{ STV090x_P1_SFRLOW0,    	0xa0 }, /* P1_SFRLOW0 */
	{ STV090x_P1_SFR3,   		0x03 }, /* P1_SFR3 */
	{ STV090x_P1_SFR2,   		0x01 }, /* P1_SFR2 */
	{ STV090x_P1_SFR1,   		0x00 }, /* P1_SFR1 */
	{ STV090x_P1_SFR0,   		0x00 }, /* P1_SFR0 */
	{ STV090x_P1_TMGREG2,    	0x00 }, /* P1_TMGREG2 */
	{ STV090x_P1_TMGREG1,    	0x00 }, /* P1_TMGREG1 */
	{ STV090x_P1_TMGREG0,    	0x00 }, /* P1_TMGREG0 */
	{ STV090x_P1_TMGLOCK1,   	0x00 }, /* P1_TMGLOCK1 */
	{ STV090x_P1_TMGLOCK0,   	0x00 }, /* P1_TMGLOCK0 */
	{ STV090x_P1_TMGOBS,   		0x10 }, /* P1_TMGOBS */
	{ STV090x_P1_EQUALCFG, 		0x41 }, /* P1_EQUALCFG */
	{ STV090x_P1_EQUAI1,   		0xf1 }, /* P1_EQUAI1 */
	{ STV090x_P1_EQUAQ1,   		0x00 }, /* P1_EQUAQ1 */
	{ STV090x_P1_EQUAI2,   		0xf1 }, /* P1_EQUAI2 */
	{ STV090x_P1_EQUAQ2,   		0x00 }, /* P1_EQUAQ2 */
	{ STV090x_P1_EQUAI3,   		0xf1 }, /* P1_EQUAI3 */
	{ STV090x_P1_EQUAQ3,   		0x00 }, /* P1_EQUAQ3 */
	{ STV090x_P1_EQUAI4,   		0xf1 }, /* P1_EQUAI4 */
	{ STV090x_P1_EQUAQ4,   		0x00 }, /* P1_EQUAQ4 */
	{ STV090x_P1_EQUAI5,   		0xf1 }, /* P1_EQUAI5 */
	{ STV090x_P1_EQUAQ5,   		0x00 }, /* P1_EQUAQ5 */
	{ STV090x_P1_EQUAI6,   		0xf1 }, /* P1_EQUAI6 */
	{ STV090x_P1_EQUAQ6,   		0x00 }, /* P1_EQUAQ6 */
	{ STV090x_P1_EQUAI7,   		0xf1 }, /* P1_EQUAI7 */
	{ STV090x_P1_EQUAQ7,   		0x00 }, /* P1_EQUAQ7 */
	{ STV090x_P1_EQUAI8,   		0xf1 }, /* P1_EQUAI8 */
	{ STV090x_P1_EQUAQ8,   		0x00 }, /* P1_EQUAQ8 */
	{ STV090x_P1_NNOSDATAT1, 	0x01 }, /* P1_NNOSDATAT1 */
	{ STV090x_P1_NNOSDATAT0, 	0xab }, /* P1_NNOSDATAT0 */
	{ STV090x_P1_NNOSDATA1,  	0x00 }, /* P1_NNOSDATA1 */
	{ STV090x_P1_NNOSDATA0,  	0x12 }, /* P1_NNOSDATA0 */
	{ STV090x_P1_NNOSPLHT1,  	0x4d }, /* P1_NNOSPLHT1 */
	{ STV090x_P1_NNOSPLHT0,  	0x7b }, /* P1_NNOSPLHT0 */
	{ STV090x_P1_NNOSPLH1,   	0x57 }, /* P1_NNOSPLH1 */
	{ STV090x_P1_NNOSPLH0,   	0x52 }, /* P1_NNOSPLH0 */
	{ STV090x_P1_NOSDATAT1,  	0x01 }, /* P1_NOSDATAT1 */
	{ STV090x_P1_NOSDATAT0,  	0x76 }, /* P1_NOSDATAT0 */
	{ STV090x_P1_NOSDATA1,   	0x00 }, /* P1_NOSDATA1 */
	{ STV090x_P1_NOSDATA0,   	0x0e }, /* P1_NOSDATA0 */
	{ STV090x_P1_NOSPLHT1,   	0x43 }, /* P1_NOSPLHT1 */
	{ STV090x_P1_NOSPLHT0,   	0xcc }, /* P1_NOSPLHT0 */
	{ STV090x_P1_NNOSPLH1,    	0x42 }, /* P1_NOSPLH1 */
	{ STV090x_P1_NNOSPLH0,    	0xdb }, /* P1_NOSPLH0 */
	{ STV090x_P1_CAR2CFG,    	0x26 }, /* P1_CAR2CFG */
	{ STV090x_P1_ACLC2,    		0x28 }, /* P1_ACLC2 */
	{ STV090x_P1_BCLC2,    		0x25 }, /* P1_BCLC2 */
#if 0
	{ STV090x_P1_CFR22,    		0x00 }, /* P2_CFR22 */
	{ STV090x_P1_CFR21,    		0x00 }, /* P2_CFR21 */
	{ STV090x_P1_CFR20,    		0x00 }, /* P2_CFR20 */
#endif
	{ STV090x_P1_ACLC2S2Q,   	0x6a }, /* P1_ACLC2S2Q */
	{ STV090x_P1_ACLC2S28,   	0x58 }, /* P1_ACLC2S28 */
	{ STV090x_P1_ACLC2S216A, 	0x68 }, /* P2_ACLC2S216A */
	{ STV090x_P1_ACLC2S232A, 	0x68 }, /* P2_ACLC2S232A */
	{ STV090x_P1_BCLC2S2Q,   	0x86 }, /* P1_BCLC2S2Q */
	{ STV090x_P1_BCLC2S28,   	0x86 }, /* P1_BCLC2S28 */
	{ STV090x_P1_BCLC2S216A, 	0xa5 }, /* P2_BCLC2S216A */
	{ STV090x_P1_BCLC2S232A, 	0xa5 }, /* P2_BCLC2S232A */

	{ STV090x_P1_PLROOT2,    	0x00 }, /* P1_PLROOT2 */
	{ STV090x_P1_PLROOT1,    	0x00 }, /* P1_PLROOT1 */
	{ STV090x_P1_PLROOT0,    	0x01 }, /* P1_PLROOT0 */
	{ STV090x_P1_MODCODLST0, 	0xff }, /* P1_MODCODLST0 */
	{ STV090x_P1_MODCODLST1, 	0xff }, /* P1_MODCODLST1 */
	{ STV090x_P1_MODCODLST2, 	0xff }, /* P1_MODCODLST2 */
	{ STV090x_P1_MODCODLST3, 	0xff }, /* P1_MODCODLST3 */
	{ STV090x_P1_MODCODLST4, 	0xff }, /* P1_MODCODLST4 */
	{ STV090x_P1_MODCODLST5, 	0xff }, /* P1_MODCODLST5 */
	{ STV090x_P1_MODCODLST6, 	0xff }, /* P1_MODCODLST6 */
	{ STV090x_P1_MODCODLST7, 	0xcc }, /* P1_MODCODLST7 */
	{ STV090x_P1_MODCODLST8, 	0xcc }, /* P1_MODCODLST8 */
	{ STV090x_P1_MODCODLST9, 	0xcc }, /* P1_MODCODLST9 */
	{ STV090x_P1_MODCODLSTA, 	0xcc }, /* P1_MODCODLSTA */
	{ STV090x_P1_MODCODLSTB, 	0xcc }, /* P1_MODCODLSTB */
	{ STV090x_P1_MODCODLSTC, 	0xcc }, /* P1_MODCODLSTC */
	{ STV090x_P1_MODCODLSTD, 	0xcc }, /* P1_MODCODLSTD */
	{ STV090x_P1_MODCODLSTE, 	0xcc }, /* P1_MODCODLSTE */
	{ STV090x_P1_MODCODLSTF, 	0xcf }, /* P1_MODCODLSTF */
	{ STV090x_P1_DMDRESCFG,  	0x29 }, /* P1_DMDRESCFG */
	{ STV090x_P1_DMDRESADR,  	0x11 }, /* P1_DMDRESADR */
	{ STV090x_P1_DMDRESDATA7,	0x4e }, /* P1_DMDRESDATA7 */
	{ STV090x_P1_DMDRESDATA6,	0x7f }, /* P1_DMDRESDATA6 */
	{ STV090x_P1_DMDRESDATA5,	0x00 }, /* P1_DMDRESDATA5 */
	{ STV090x_P1_DMDRESDATA4,	0x04 }, /* P1_DMDRESDATA4 */
	{ STV090x_P1_DMDRESDATA3,	0x92 }, /* P1_DMDRESDATA3 */
	{ STV090x_P1_DMDRESDATA2,	0xcc }, /* P1_DMDRESDATA2 */
	{ STV090x_P1_DMDRESDATA1,	0x0a }, /* P1_DMDRESDATA1 */
	{ STV090x_P1_DMDRESDATA0,	0x07 }, /* P1_DMDRESDATA0 */
	{ STV090x_P1_FFEI1,  		0x00 }, /* P1_FFEI1 */
	{ STV090x_P1_FFEQ1,  		0x00 }, /* P1_FFEQ1 */
	{ STV090x_P1_FFEI2,  		0x00 }, /* P1_FFEI2 */
	{ STV090x_P1_FFEQ2,  		0x00 }, /* P1_FFEQ2 */
	{ STV090x_P1_FFEI3,  		0x00 }, /* P1_FFEI3 */
	{ STV090x_P1_FFEQ3,  		0x00 }, /* P1_FFEQ3 */
	{ STV090x_P1_FFEI4,  		0x00 }, /* P1_FFEI4 */
	{ STV090x_P1_FFEQ4,  		0x00 }, /* P1_FFEQ4 */
	{ STV090x_P1_FFECFG,    	0x31 }, /* P1_FFECFG */
#if 0
	{ STV090x_P1_TNRCFG,    	0xec }, /* P1_TNRCFG */
	{ STV090x_P1_TNRCFG2,    	0x82 }, /* P1_TNRCFG2 */
	{ STV090x_P1_TNRXTAL,    	0x1b }, /* P1_TNRXTAL */
	{ STV090x_P1_TNRSTEPS,   	0x87 }, /* P1_TNRSTEPS */
	{ STV090x_P1_TNRGAIN,    	0x09 }, /* P1_TNRGAIN */
	{ STV090x_P1_TNRRF1,    	0x4e }, /* P1_TNRRF1 */
	{ STV090x_P1_TNRRF0,   		0x7f }, /* P1_TNRRF0 */
	{ STV090x_P1_TNRBW,    		0x64 }, /* P1_TNRBW */
	{ STV090x_P1_TNRADJ,    	0x1f }, /* P1_TNRADJ */
	{ STV090x_P1_TNRCTL2,    	0x37 }, /* P1_TNRCTL2 */
	{ STV090x_P1_TNRCFG3,    	0x02 }, /* P1_TNRCFG3 */
	{ STV090x_P1_TNRLAUNCH,  	0x00 }, /* P1_TNRLAUNCH */
	{ STV090x_P1_TNRLD,    		0x00 }, /* P1_TNRLD */
	{ STV090x_P1_TNROBSL,    	0x50 }, /* P1_TNROBSL */
	{ STV090x_P1_TNRRESTE,   	0x40 }, /* P1_TNRRESTE */
#endif
	{ STV090x_P1_SMAPCOEF7,  	0x06 }, /* P1_SMAPCOEF7 */
	{ STV090x_P1_SMAPCOEF6,  	0x00 }, /* P1_SMAPCOEF6 */
	{ STV090x_P1_SMAPCOEF5,  	0x04 }, /* P1_SMAPCOEF5 */
	{ STV090x_P1_DMDPLHSTAT, 	0x00 }, /* P1_DMDPLHSTAT */
	{ STV090x_P1_LOCKTIME3,  	0xff }, /* P1_LOCKTIME3 */
	{ STV090x_P1_LOCKTIME2,  	0xf0 }, /* P1_LOCKTIME2 */
	{ STV090x_P1_LOCKTIME1,  	0x73 }, /* P1_LOCKTIME1 */
	{ STV090x_P1_LOCKTIME0,  	0x27 }, /* P1_LOCKTIME0 */
	{ STV090x_P1_VITSCALE,   	0x00 }, /* P1_VITSCALE */
	{ STV090x_P1_FECM,    		0x10 }, /* P1_FECM */
	{ STV090x_P1_VTH12,  		0xd0 }, /* P1_VTH12 */
	{ STV090x_P1_VTH23,  		0x7d }, /* P1_VTH23 */
	{ STV090x_P1_VTH34,  		0x53 }, /* P1_VTH34 */
	{ STV090x_P1_VTH56,  		0x2f }, /* P1_VTH56 */
	{ STV090x_P1_VTH67,  		0x24 }, /* P1_VTH67 */
	{ STV090x_P1_VTH78,  		0x1f }, /* P1_VTH78 */
	{ STV090x_P1_VITCURPUN,  	0x0d }, /* P1_VITCURPUN */
	{ STV090x_P1_VERROR,    	0xff }, /* P1_VERROR */
	{ STV090x_P1_PRVIT,    		0x3f }, /* P1_PRVIT */
	{ STV090x_P1_VAVSRVIT,   	0x00 }, /* P1_VAVSRVIT */
	{ STV090x_P1_VSTATUSVIT, 	0xa5 }, /* P1_VSTATUSVIT */
	{ STV090x_P1_VTHINUSE,   	0x01 }, /* P1_VTHINUSE */
	{ STV090x_P1_KDIV12,  		0x27 }, /* P1_KDIV12 */
	{ STV090x_P1_KDIV23,  		0x32 }, /* P1_KDIV23 */
	{ STV090x_P1_KDIV34,  		0x32 }, /* P1_KDIV34 */
	{ STV090x_P2_AGC2REF,		0x38 },
	{ STV090x_P1_KDIV56,  		0x32 }, /* P1_KDIV56 */
	{ STV090x_P1_KDIV67,  		0x32 }, /* P1_KDIV67 */
	{ STV090x_P1_KDIV78,  		0x50 }, /* P1_KDIV78 */
	{ STV090x_P1_PDELCTRL1,  	0x00 }, /* P1_PDELCTRL1 */
	{ STV090x_P1_PDELCTRL2,  	0x00 }, /* P1_PDELCTRL2 */
	{ STV090x_P1_HYSTTHRESH, 	0x41 }, /* P1_HYSTTHRESH */
	{ STV090x_P1_ISIENTRY,   	0x00 }, /* P1_ISIENTRY */
	{ STV090x_P1_ISIBITENA,  	0x00 }, /* P1_ISIBITENA */
	{ STV090x_P1_MATSTR1,    	0xf0 }, /* P1_MATSTR1 */
	{ STV090x_P1_MATSTR0,    	0x00 }, /* P1_MATSTR0 */
	{ STV090x_P1_UPLSTR1,    	0x05 }, /* P1_UPLSTR1 */
	{ STV090x_P1_UPLSTR0,    	0xe0 }, /* P1_UPLSTR0 */
	{ STV090x_P1_DFLSTR1,    	0x7d }, /* P1_DFLSTR1 */
	{ STV090x_P1_DFLSTR0,    	0x80 }, /* P1_DFLSTR0 */
	{ STV090x_P1_SYNCSTR,    	0x47 }, /* P1_SYNCSTR */
	{ STV090x_P1_SYNCDSTR1,  	0x00 }, /* P1_SYNCDSTR1 */
	{ STV090x_P1_SYNCDSTR0,  	0x00 }, /* P1_SYNCDSTR0 */
	{ STV090x_P1_PDELSTATUS1,	0x94 }, /* P1_PDELSTATUS1 */
	{ STV090x_P1_PDELSTATUS2,	0x10 }, /* P1_PDELSTATUS2 */
	{ STV090x_P1_BBFCRCKO1,  	0x00 }, /* P1_BBFCRCKO1 */
	{ STV090x_P1_BBFCRCKO0,  	0x00 }, /* P1_BBFCRCKO0 */
	{ STV090x_P1_UPCRCKO1,   	0x00 }, /* P1_UPCRCKO1 */
	{ STV090x_P1_UPCRCKO0,   	0x00 }, /* P1_UPCRCKO0 */
	{ STV090x_P1_TSSTATEM,   	0xb0 }, /* P1_TSSTATEM */
	{ STV090x_P1_TSCFGH,  		0x40 }, /* P1_TSCFGH */
	{ STV090x_P1_TSCFGM,  		0x00 }, /* P1_TSCFGM */
	{ STV090x_P1_TSCFGL,  		0x20 }, /* P1_TSCFGL */
	{ STV090x_P1_TSINSDELH,  	0x00 }, /* P1_TSINSDELH */
	{ STV090x_P1_TSSPEED,    	0xff }, /* P1_TSSPEED */
	{ STV090x_P1_TSSTATUS,   	0x52 }, /* P1_TSSTATUS */
	{ STV090x_P1_TSSTATUS2,  	0x6a }, /* P1_TSSTATUS2 */
	{ STV090x_P1_TSBITRATE1, 	0x00 }, /* P1_TSBITRATE1 */
	{ STV090x_P1_TSBITRATE0, 	0x00 }, /* P1_TSBITRATE0 */
	{ STV090x_P1_ERRCTRL1,   	0x35 }, /* P1_ERRCTRL1 */
	{ STV090x_P1_ERRCNT12,   	0x80 }, /* P1_ERRCNT12 */
	{ STV090x_P1_ERRCNT11,   	0x00 }, /* P1_ERRCNT11 */
	{ STV090x_P1_ERRCNT10,   	0x00 }, /* P1_ERRCNT10 */
	{ STV090x_P1_ERRCTRL2,   	0xc1 }, /* P1_ERRCTRL2 */
	{ STV090x_P1_ERRCNT22,  	0x00 }, /* P1_ERRCNT22 */
	{ STV090x_P1_ERRCNT21,   	0x00 }, /* P1_ERRCNT21 */
	{ STV090x_P1_ERRCNT20,   	0x00 }, /* P1_ERRCNT20 */
	{ STV090x_P1_FECSPY,    	0xa8 }, /* P1_FECSPY */
	{ STV090x_P1_FSPYCFG,    	0x2c }, /* P1_FSPYCFG */
	{ STV090x_P1_FSPYDATA,   	0x3a }, /* P1_FSPYDATA */
	{ STV090x_P1_FSPYOUT,    	0x07 }, /* P1_FSPYOUT */
	{ STV090x_P2_AGC2REF,		0x38 },
	{ STV090x_P1_FSTATUS,    	0x00 }, /* P1_FSTATUS */
	{ STV090x_P1_FBERCPT4,   	0x00 }, /* P1_FBERCPT4 */
	{ STV090x_P1_FBERCPT3,   	0x00 }, /* P1_FBERCPT3 */
	{ STV090x_P1_FBERCPT2,   	0x00 }, /* P1_FBERCPT2 */
	{ STV090x_P1_FBERCPT1,   	0x00 }, /* P1_FBERCPT1 */
	{ STV090x_P1_FBERCPT0,   	0x00 }, /* P1_FBERCPT0 */
	{ STV090x_P1_FBERERR2,   	0x00 }, /* P1_FBERERR2 */
	{ STV090x_P1_FBERERR1,   	0x00 }, /* P1_FBERERR1 */
	{ STV090x_P1_FBERERR0,   	0x00 }, /* P1_FBERERR0 */
	{ STV090x_P1_FSPYBER,    	0x10 }, /* P1_FSPYBER */
	{ STV090x_RCCFGH,    		0x20 }, /* RCCFGH */
	{ STV090x_TSGENERAL,    	0x14 }, /* TSGENERAL */
	{ STV090x_TSGENERAL1X,   	0x00 }, /* TSGENERAL1X */
	{ STV090x_NBITER_NF4,    	0x37 }, /* NBITER_NF4 */
	{ STV090x_AGCRF2CFG,		0x13 },

	{ STV090x_NBITER_NF5,    	0x29 }, /* NBITER_NF5 */
	{ STV090x_NBITER_NF6,    	0x37 }, /* NBITER_NF6 */
	{ STV090x_NBITER_NF7,    	0x33 }, /* NBITER_NF7 */
	{ STV090x_NBITER_NF8,    	0x31 }, /* NBITER_NF8 */
	{ STV090x_NBITER_NF9,    	0x2f }, /* NBITER_NF9 */
	{ STV090x_NBITER_NF10,   	0x39 }, /* NBITER_NF10 */
	{ STV090x_NBITER_NF11,   	0x3a }, /* NBITER_NF11 */
	{ STV090x_NBITER_NF12,   	0x29 }, /* NBITER_NF12 */
	{ STV090x_NBITER_NF13,   	0x37 }, /* NBITER_NF13 */
	{ STV090x_P2_AGC2REF,		0x38 },
	{ STV090x_NBITER_NF14,   	0x33 }, /* NBITER_NF14 */
	{ STV090x_NBITER_NF15,   	0x2f }, /* NBITER_NF15 */
	{ STV090x_NBITER_NF16,   	0x39 }, /* NBITER_NF16 */
	{ STV090x_NBITER_NF17,   	0x3a }, /* NBITER_NF17 */
	{ STV090x_NBITERNOERR,   	0x04 }, /* NBITERNOERR */
	{ STV090x_GAINLLR_NF4,   	0x21 }, /* GAINLLR_NF4 */
	{ STV090x_GAINLLR_NF5,   	0x21 }, /* GAINLLR_NF5 */
	{ STV090x_GAINLLR_NF6,   	0x20 }, /* GAINLLR_NF6 */
	{ STV090x_GAINLLR_NF7,   	0x1f }, /* GAINLLR_NF7 */
	{ STV090x_GAINLLR_NF8,   	0x1e }, /* GAINLLR_NF8 */
	{ STV090x_GAINLLR_NF9,   	0x1e }, /* GAINLLR_NF9 */
	{ STV090x_GAINLLR_NF10,  	0x1d }, /* GAINLLR_NF10 */
	{ STV090x_GAINLLR_NF11,  	0x1b }, /* GAINLLR_NF11 */
	{ STV090x_GAINLLR_NF12,  	0x20 }, /* GAINLLR_NF12 */
	{ STV090x_GAINLLR_NF13,  	0x20 }, /* GAINLLR_NF13 */
	{ STV090x_GAINLLR_NF14,  	0x20 }, /* GAINLLR_NF14 */
	{ STV090x_GAINLLR_NF15,  	0x20 }, /* GAINLLR_NF15 */
	{ STV090x_GAINLLR_NF16,  	0x20 }, /* GAINLLR_NF16 */
	{ STV090x_GAINLLR_NF17,  	0x21 }, /* GAINLLR_NF17 */
	{ STV090x_CFGEXT,    		0x01 }, /* CFGEXT */
	{ STV090x_GENCFG,    		0x1d }, /* GENCFG */
	{ STV090x_P2_AGC2REF,		0x38 },
	{ STV090x_LDPCERR1,    		0x00 }, /* LDPCERR1 */
	{ STV090x_LDPCERR0,    		0x00 }, /* LDPCERR0 */
	{ STV090x_BCHERR,    		0x00 }, /* BCHERR */
	{ STV090x_TSTRES0,    		0x00 }, /* TSTRES0 */
	{ STV090x_P2_TSTDISRX,   	0x00 }, /* P2_TSTDISRX */
	{ STV090x_P1_TSTDISRX,   	0x00 }, /* P1_TSTDISRX */
};
#endif

static struct stv090x_reg stv0900_initval[] = {

	{ STV090x_OUTCFG,		0x00 },
	{ STV090x_MODECFG,		0xff },
	{ STV090x_AGCRF1CFG,		0x11 },
	{ STV090x_AGCRF2CFG,		0x13 },
	{ STV090x_TSGENERAL1X,		0x14 },
	{ STV090x_TSTTNR2,		0x21 },
	{ STV090x_TSTTNR4,		0x21 },
	{ STV090x_P2_DISTXCTL,		0x22 },
	{ STV090x_P2_F22TX,		0xc0 },
	{ STV090x_P2_F22RX,		0xc0 },
	{ STV090x_P2_DISRXCTL,		0x00 },
#if 0
	{ STV090x_P2_TNRSTEPS,		0x87 },
	{ STV090x_P2_TNRGAIN,		0x09 },
#endif
	{ STV090x_P2_DMDCFGMD,		0xF9 },
	{ STV090x_P2_DEMOD,		0x08 },
	{ STV090x_P2_DMDCFG3,		0xc4 },
	{ STV090x_P2_CARFREQ,		0xed },
#if 0
	{ STV090x_P2_TNRCFG2,		0x02 },
	{ STV090x_P2_TNRCFG3,		0x02 },
#endif
	{ STV090x_P2_LDT,		0xd0 },
	{ STV090x_P2_LDT2,		0xb8 },
	{ STV090x_P2_TMGCFG,		0xd2 },
	{ STV090x_P2_TMGTHRISE,		0x20 },
	{ STV090x_P1_TMGCFG,		0xd2 },

	{ STV090x_P2_TMGTHFALL,		0x00 },
	{ STV090x_P2_FECSPY,		0x88 },
	{ STV090x_P2_FSPYDATA,		0x3a },
	{ STV090x_P2_FBERCPT4,		0x00 },
	{ STV090x_P2_FSPYBER,		0x10 },
	{ STV090x_P2_ERRCTRL1,		0x35 },
	{ STV090x_P2_ERRCTRL2,		0xc1 },
	{ STV090x_P2_CFRICFG,		0xf8 },
	{ STV090x_P2_NOSCFG,		0x1c },
	{ STV090x_P2_DMDTOM,		0x20 },
	{ STV090x_P2_CORRELMANT,	0x70 },
	{ STV090x_P2_CORRELABS,		0x88 },
	{ STV090x_P2_AGC2O,		0x5b },
	{ STV090x_P2_AGC2REF,		0x38 },
	{ STV090x_P2_CARCFG,		0xe4 },
	{ STV090x_P2_ACLC,		0x1A },
	{ STV090x_P2_BCLC,		0x09 },
	{ STV090x_P2_CARHDR,		0x08 },
	{ STV090x_P2_KREFTMG,		0xc1 },
	{ STV090x_P2_SFRUPRATIO,	0xf0 },
	{ STV090x_P2_SFRLOWRATIO,	0x70 },
	{ STV090x_P2_SFRSTEP,		0x58 },
	{ STV090x_P2_TMGCFG2,		0x01 },
	{ STV090x_P2_CAR2CFG,		0x26 },
	{ STV090x_P2_BCLC2S2Q,		0x86 },
	{ STV090x_P2_BCLC2S28,		0x86 },
	{ STV090x_P2_SMAPCOEF7,		0x77 },
	{ STV090x_P2_SMAPCOEF6,		0x85 },
	{ STV090x_P2_SMAPCOEF5,		0x77 },
	{ STV090x_P2_TSCFGL,		0x20 },
	{ STV090x_P2_DMDCFG2,		0x3b },
	{ STV090x_P2_MODCODLST0,	0xff },
	{ STV090x_P2_MODCODLST1,	0xff },
	{ STV090x_P2_MODCODLST2,	0xff },
	{ STV090x_P2_MODCODLST3,	0xff },
	{ STV090x_P2_MODCODLST4,	0xff },
	{ STV090x_P2_MODCODLST5,	0xff },
	{ STV090x_P2_MODCODLST6,	0xff },
	{ STV090x_P2_MODCODLST7,	0xcc },
	{ STV090x_P2_MODCODLST8,	0xcc },
	{ STV090x_P2_MODCODLST9,	0xcc },
	{ STV090x_P2_MODCODLSTA,	0xcc },
	{ STV090x_P2_MODCODLSTB,	0xcc },
	{ STV090x_P2_MODCODLSTC,	0xcc },
	{ STV090x_P2_MODCODLSTD,	0xcc },
	{ STV090x_P2_MODCODLSTE,	0xcc },
	{ STV090x_P2_MODCODLSTF,	0xcf },
	{ STV090x_P1_DISTXCTL,		0x22 },
	{ STV090x_P1_F22TX,		0xc0 },
	{ STV090x_P1_F22RX,		0xc0 },
	{ STV090x_P1_DISRXCTL,		0x00 },
#if 0
	{ STV090x_P1_TNRSTEPS,		0x87 },
	{ STV090x_P1_TNRGAIN,		0x09 },
#endif
	{ STV090x_P1_DMDCFGMD,		0xf9 },
	{ STV090x_P1_DEMOD,		0x08 },
	{ STV090x_P1_DMDCFG3,		0xc4 },
	{ STV090x_P1_DMDTOM,		0x20 },
	{ STV090x_P1_CARFREQ,		0xed },
#if 0
	{ STV090x_P1_TNRCFG2,		0x82 },
	{ STV090x_P1_TNRCFG3,		0x02 },
#endif
	{ STV090x_P1_LDT,		0xd0 },
	{ STV090x_P1_LDT2,		0xb8 },
	{ STV090x_P1_TMGCFG,		0xd2 },
	{ STV090x_P1_TMGTHRISE,		0x20 },
	{ STV090x_P1_TMGTHFALL,		0x00 },
	{ STV090x_P1_SFRUPRATIO,	0xf0 },
	{ STV090x_P1_SFRLOWRATIO,	0x70 },
	{ STV090x_P1_TSCFGL,		0x20 },
	{ STV090x_P1_FECSPY,		0x88 },
	{ STV090x_P1_FSPYDATA,		0x3a },
	{ STV090x_P1_FBERCPT4,		0x00 },
	{ STV090x_P1_FSPYBER,		0x10 },
	{ STV090x_P1_ERRCTRL1,		0x35 },
	{ STV090x_P1_ERRCTRL2,		0xc1 },
	{ STV090x_P1_CFRICFG,		0xf8 },
	{ STV090x_P1_NOSCFG,		0x1c },
	{ STV090x_P1_CORRELMANT,	0x70 },
	{ STV090x_P1_CORRELABS,		0x88 },
	{ STV090x_P1_AGC2O,		0x5b },
	{ STV090x_P1_AGC2REF,		0x38 },
	{ STV090x_P1_CARCFG,		0xe4 },
	{ STV090x_P1_ACLC,		0x1A },
	{ STV090x_P1_BCLC,		0x09 },
	{ STV090x_P1_CARHDR,		0x08 },
	{ STV090x_P1_KREFTMG,		0xc1 },
	{ STV090x_P1_SFRSTEP,		0x58 },
	{ STV090x_P1_TMGCFG2,		0x01 },
	{ STV090x_P1_CAR2CFG,		0x26 },
	{ STV090x_P1_BCLC2S2Q,		0x86 },
	{ STV090x_P1_BCLC2S28,		0x86 },
	{ STV090x_P1_SMAPCOEF7,		0x77 },
	{ STV090x_P1_SMAPCOEF6,		0x85 },
	{ STV090x_P1_SMAPCOEF5,		0x77 },
	{ STV090x_P1_DMDCFG2,		0x3b },
	{ STV090x_P1_MODCODLST0,	0xff },
	{ STV090x_P1_MODCODLST1,	0xff },
	{ STV090x_P1_MODCODLST2,	0xff },
	{ STV090x_P1_MODCODLST3,	0xff },
	{ STV090x_P1_MODCODLST4,	0xff },
	{ STV090x_P1_MODCODLST5,	0xff },
	{ STV090x_P1_MODCODLST6,	0xff },
	{ STV090x_P1_MODCODLST7,	0xcc },
	{ STV090x_P1_MODCODLST8,	0xcc },
	{ STV090x_P1_MODCODLST9,	0xcc },
	{ STV090x_P1_MODCODLSTA,	0xcc },
	{ STV090x_P1_MODCODLSTB,	0xcc },
	{ STV090x_P1_MODCODLSTC,	0xcc },
	{ STV090x_P1_MODCODLSTD,	0xcc },
	{ STV090x_P1_MODCODLSTE,	0xcc },
	{ STV090x_P1_MODCODLSTF,	0xcf },
	{ STV090x_GENCFG,		0x1d },
	{ STV090x_NBITER_NF4,		0x37 },
	{ STV090x_NBITER_NF5,		0x29 },
	{ STV090x_NBITER_NF6,		0x37 },
	{ STV090x_NBITER_NF7,		0x33 },
	{ STV090x_NBITER_NF8,		0x31 },
	{ STV090x_NBITER_NF9,		0x2f },
	{ STV090x_NBITER_NF10,		0x39 },
	{ STV090x_NBITER_NF11,		0x3a },
	{ STV090x_NBITER_NF12,		0x29 },
	{ STV090x_NBITER_NF13,		0x37 },
	{ STV090x_NBITER_NF14,		0x33 },
	{ STV090x_NBITER_NF15,		0x2f },
	{ STV090x_NBITER_NF16,		0x39 },
	{ STV090x_NBITER_NF17,		0x3a },
	{ STV090x_NBITERNOERR,		0x04 },
	{ STV090x_GAINLLR_NF4,		0x0C },
	{ STV090x_GAINLLR_NF5,		0x0F },
	{ STV090x_GAINLLR_NF6,		0x11 },
	{ STV090x_GAINLLR_NF7,		0x14 },
	{ STV090x_GAINLLR_NF8,		0x17 },
	{ STV090x_GAINLLR_NF9,		0x19 },
	{ STV090x_GAINLLR_NF10,		0x20 },
	{ STV090x_GAINLLR_NF11,		0x21 },
	{ STV090x_GAINLLR_NF12,		0x0D },
	{ STV090x_GAINLLR_NF13,		0x0F },
	{ STV090x_GAINLLR_NF14,		0x13 },
	{ STV090x_GAINLLR_NF15,		0x1A },
	{ STV090x_GAINLLR_NF16,		0x1F },
	{ STV090x_GAINLLR_NF17,		0x21 },
	{ STV090x_RCCFGH,		0x20 },
	{ STV090x_P1_FECM,		0x01 }, /* disable DSS modes */
	{ STV090x_P2_FECM,		0x01 }, /* disable DSS modes */
	{ STV090x_P1_PRVIT,		0x2F }, /* disable PR 6/7 */
	{ STV090x_P2_PRVIT,		0x2F }, /* disable PR 6/7 */
};

#define STV090x_P1_TNRSTEPS     0xf4e7
#define STV090x_P2_TNRSTEPS     0xf2e7
#define STV090x_P1_TNRGAIN      0xf4e8
#define STV090x_P2_TNRGAIN      0xf2e8
#define STV090x_P1_TNRCFG3      0xf4ee
#define STV090x_P2_TNRCFG3      0xf2ee

static struct stv090x_reg stx7111_initval[] = {
/* demod2 */
      { STV090x_OUTCFG,        0x00 },
      { STV090x_AGCRF1CFG,     0x10 },	// for ix7306
//      { STV090x_AGCRF1CFG,     0x11 },
      { STV090x_AGCRF2CFG,     0x13 },
      { STV090x_TSGENERAL,     0x00 },
      { STV090x_P2_DISTXCTL,   0x22 },
      { STV090x_P2_F22TX,      0xc0 },
      { STV090x_P2_F22RX,      0xc0 },
      { STV090x_P2_DISRXCTL,   0x00 },
      { STV090x_P2_TNRSTEPS,   0x87 },
      { STV090x_P2_TNRGAIN,    0x09 },
      { STV090x_P2_DMDCFGMD,   0xf9 },
      { STV090x_P2_DEMOD,      0x0E },
      { STV090x_P2_DMDCFG3,    0x48 },
      { STV090x_P2_CARFREQ,    0x88 },
      { STV090x_P2_TNRCFG2,    0x02 },
      { STV090x_P2_TNRCFG3,    0x02 },

      { STV090x_P2_LDT,        0xd0 },
      { STV090x_P2_LDT2,       0xb0 },
      { STV090x_P2_TMGCFG,     0xd3 },
      { STV090x_P2_TMGTHRISE,  0x20 },
      { STV090x_P2_TMGTHFALL,  0x00 },

      { STV090x_P2_FECSPY,     0x88 },
      { STV090x_P2_FSPYDATA,   0x3a },
      { STV090x_P2_FBERCPT4,   0x00 },
      { STV090x_P2_FSPYBER,    0x10 },
      { STV090x_P2_TSCFGH,     0x40 },
      { STV090x_P2_ERRCTRL1,   0x35 },
      { STV090x_P2_ERRCTRL2,   0xc1 },
      { STV090x_P2_CFRICFG,    0xf8 },
      { STV090x_P2_NOSCFG,     0x0c },
      { STV090x_P2_DMDTOM,     0x20 },
      { STV090x_P2_AGC2O,      0x5b },
      { STV090x_P2_AGC2REF,    0x38 },
      { STV090x_P2_CARCFG,     0xe4 },
      { STV090x_P2_ACLC,       0x1A },
      { STV090x_P2_BCLC,       0x09 },
      { 0xf43b           ,     0x00 },
      { 0xf43c           ,     0xc0 },
      { STV090x_P2_CARHDR,     0x20 },
#if 0
      { STV090x_P2_TMGTHRISE,  0x20 },
#endif
      { STV090x_P2_KREFTMG,    0x87 },
      { STV090x_P2_SFRUPRATIO, 0xf0 },
      { STV090x_P2_SFRLOWRATIO,0x70 },

#if 0
      { STV090x_P2_TMGTHFALL,  0x00 },
#endif

      { STV090x_P2_SFRSTEP,    0x58 },
      { STV090x_P2_CAR2CFG,    0x26 },
#if 0
      { STV090x_P2_BCLC2S2Q,   0x86 },
      { STV090x_P2_BCLC2S28,   0x86 },
#else
      { STV090x_P2_BCLC2S2Q,   0xa5 },
      { STV090x_P2_BCLC2S28,   0xa5 },
#endif
      { STV090x_P2_DMDRESCFG,  0xa9 },
      { STV090x_P2_SMAPCOEF7,  0xfe },
      { STV090x_P2_SMAPCOEF6,  0x00 },
      { STV090x_P2_SMAPCOEF5,  0xff },
      { STV090x_P2_DMDCFG2,    0x3b },
      { STV090x_P2_MODCODLST0, 0xff },
      { STV090x_P2_MODCODLST1, 0xff },
      { STV090x_P2_MODCODLST2, 0xff },
      { STV090x_P2_MODCODLST3, 0xff },
      { STV090x_P2_MODCODLST4, 0xff },
      { STV090x_P2_MODCODLST5, 0xff },
      { STV090x_P2_MODCODLST6, 0xff },
      { STV090x_P2_MODCODLST7, 0xcc },
      { STV090x_P2_MODCODLST8, 0xcc },
      { STV090x_P2_MODCODLST9, 0xcc },
      { STV090x_P2_MODCODLSTA, 0xcc },
      { STV090x_P2_MODCODLSTB, 0xcc },
      { STV090x_P2_MODCODLSTC, 0xcc },
      { STV090x_P2_MODCODLSTD, 0xcc },
      { STV090x_P2_MODCODLSTE, 0xff },
      { STV090x_P2_MODCODLSTF, 0xff },
/* demod1 */
      { STV090x_P1_DISTXCTL,   0x22 },
      { STV090x_P1_F22TX,      0xc0 },
      { STV090x_P1_F22RX,      0xc0 },
      { STV090x_P1_DISRXCTL,   0x00 },

      { STV090x_P1_TNRSTEPS,   0x87 },
      { STV090x_P1_TNRGAIN,    0x09 },
      { STV090x_P1_DMDCFGMD,   0xf9 },
      { STV090x_P1_DEMOD,      0x0E },
      { STV090x_P1_DMDCFG3,    0x48 },
      { STV090x_P1_DMDTOM,     0x20 },
      { STV090x_P1_CARFREQ,    0x88 },
      { STV090x_P1_TNRCFG2,    0x02 },
      { STV090x_P1_TNRCFG3,    0x02 },

      { STV090x_P1_LDT,        0xd0 },
      { STV090x_P1_LDT2,       0xb0 },
      { STV090x_P1_TMGCFG,     0xd3 },
      { STV090x_P1_TMGTHRISE,  0x20 },
      { STV090x_P1_TMGTHFALL,  0x00 },
      { STV090x_P2_SFRUPRATIO, 0xf0 },
      { STV090x_P2_SFRLOWRATIO,0x70 },

      { STV090x_P1_FECSPY,     0x88 },
      { STV090x_P1_FSPYDATA,   0x3a },
      { STV090x_P1_FBERCPT4,   0x00 },
      { STV090x_P1_FSPYBER,    0x10 },

#if 0
      { STV090x_P1_TSCFGH,     0x40 },
#else
      { STV090x_P1_TSCFGH,     0x90 },
#endif
      { STV090x_P1_ERRCTRL1,   0x35 },
      { STV090x_P1_ERRCTRL2,   0xc1 },
      { STV090x_P1_CFRICFG,    0xf8 },
      { STV090x_P1_NOSCFG,     0x0c },
      { STV090x_P1_DMDTOM,     0x20 },
      { STV090x_P1_AGC2O,      0x5b },
      { STV090x_P1_AGC2REF,    0x38 },
      { STV090x_P1_CARCFG,     0xe4 },
      { STV090x_P1_ACLC,       0x1A },
      { STV090x_P1_BCLC,       0x09 },
      { 0xf43b           ,     0x00 },
      { 0xf43c           ,     0xc0 },
      { STV090x_P1_CARHDR,     0x20 },
      { STV090x_P1_TMGTHRISE,  0x20 },
      { STV090x_P1_KREFTMG,    0x87 },
      { STV090x_P1_TMGTHFALL,  0x00 },
      { STV090x_P1_SFRSTEP,    0x58 },
      { STV090x_P1_CAR2CFG,    0x26 },
      { STV090x_P1_BCLC2S2Q,   0x86 },
      { STV090x_P1_BCLC2S28,   0x86 },

      { STV090x_P1_DMDRESCFG,  0xa9 },
      { STV090x_P1_SMAPCOEF7,  0xfe },
      { STV090x_P1_SMAPCOEF6,  0x00 },
      { STV090x_P1_SMAPCOEF5,  0xff },
      { STV090x_P1_DMDCFG2,    0x3b },
      { STV090x_P1_MODCODLST0, 0xff },
      { STV090x_P1_MODCODLST1, 0xff },
      { STV090x_P1_MODCODLST2, 0xff },
      { STV090x_P1_MODCODLST3, 0xff },
      { STV090x_P1_MODCODLST4, 0xff },
      { STV090x_P1_MODCODLST5, 0xff },
      { STV090x_P1_MODCODLST6, 0xff },
      { STV090x_P1_MODCODLST7, 0xcc },
      { STV090x_P1_MODCODLST8, 0xcc },
      { STV090x_P1_MODCODLST9, 0xcc },
      { STV090x_P1_MODCODLSTA, 0xcc },
      { STV090x_P1_MODCODLSTB, 0xcc },
      { STV090x_P1_MODCODLSTC, 0xcc },
      { STV090x_P1_MODCODLSTD, 0xcc },
      { STV090x_P1_MODCODLSTE, 0xff },
      { STV090x_P1_MODCODLSTF, 0xff },

      { STV090x_NBITERNOERR,   0x04 },
      { STV090x_GAINLLR_NF4,   0x0f },
      { STV090x_GAINLLR_NF5,   0x13 },
      { STV090x_GAINLLR_NF6,   0x15 },
      { STV090x_GAINLLR_NF7,   0x1a },
      { STV090x_GAINLLR_NF8,   0x1F },
      { STV090x_GAINLLR_NF9,   0x20 },
      { STV090x_GAINLLR_NF10,  0x26 },
      { STV090x_GAINLLR_NF11,  0x28 },
      { STV090x_GAINLLR_NF12,  0x0D },
      { STV090x_GAINLLR_NF13,  0x0F },
      { STV090x_GAINLLR_NF14,  0x13 },
      { STV090x_GAINLLR_NF15,  0x19 },
      { STV090x_GAINLLR_NF16,  0x20 },
      { STV090x_GAINLLR_NF17,  0x20 },
      { STV090x_NBITER_NF4,    0x38 },
      { STV090x_NBITER_NF5,    0x2C },
      { STV090x_NBITER_NF6,    0x3b },
      { STV090x_NBITER_NF7,    0x38 },
      { STV090x_NBITER_NF8,    0x36 },
      { STV090x_NBITER_NF9,    0x35 },

      { STV090x_NBITER_NF10,   0x41 },
      { STV090x_NBITER_NF11,   0x41 },
      { STV090x_NBITER_NF12,   0x1d },
      { STV090x_NBITER_NF13,   0x27 },
      { STV090x_NBITER_NF14,   0x25 },
      { STV090x_NBITER_NF15,   0x23 },
      { STV090x_NBITER_NF16,   0x2b },
      { STV090x_NBITER_NF17,   0x2b },

      { STV090x_P2_GAUSSR0,    0xac },
      { STV090x_P2_CCIR0,      0x2c },
      { STV090x_P2_CCIQUANT,   0xac },
      { 0xf2c3             ,   0x00 },

      { STV090x_P1_GAUSSR0,    0xac },
      { STV090x_P1_CCIR0,      0x2c },
      { STV090x_P1_CCIQUANT,   0xac },
      { 0xf4c3             ,   0x00 },

      { STV090x_P2_TSCFGL,     0x30 },
      { STV090x_P1_TSCFGL,     0x30 },
};

static struct stv090x_reg stv0903_initval[] = {
	{ STV090x_OUTCFG,		0x00 },
	{ STV090x_AGCRF1CFG,		0x11 },
	{ STV090x_STOPCLK1,		0x48 },
	{ STV090x_STOPCLK2,		0x14 },
	{ STV090x_TSTTNR1,		0x27 },
	{ STV090x_TSTTNR2,		0x21 },
	{ STV090x_P1_DISTXCTL,		0x22 },
	{ STV090x_P1_F22TX,		0xc0 },
	{ STV090x_P1_F22RX,		0xc0 },
	{ STV090x_P1_DISRXCTL,		0x00 },
/* __TDT__*/
	{ STV090x_P1_TNRSTEPS,		0x87 },
	{ STV090x_P1_TNRGAIN,		0x09 },

/* TDT	{ STV090x_P1_DMDCFGMD,		0xF9 },*/
	{ STV090x_P1_DMDCFGMD,		0xc9 },
	{ STV090x_P1_DEMOD,		0x08 },
	{ STV090x_P1_DMDCFG3,		0xc4 },
	{ STV090x_P1_CARFREQ,		0xed },
	{ STV090x_P1_TNRCFG2,		0x82 },
/* __TDT__ */
	{ STV090x_P1_TNRCFG3,		0x03 },

	{ STV090x_P1_LDT,		0xd0 },
	{ STV090x_P1_LDT2,		0xb8 },
	{ STV090x_P1_TMGCFG,		0xd2 },
	{ STV090x_P1_TMGTHRISE,		0x20 },
	{ STV090x_P1_TMGTHFALL,		0x00 },
	{ STV090x_P1_SFRUPRATIO,	0xf0 },
	{ STV090x_P1_SFRLOWRATIO,	0x70 },
	{ STV090x_P1_TSCFGL,		0x20 },
	{ STV090x_P1_FECSPY,		0x88 },
	{ STV090x_P1_FSPYDATA,		0x3a },
	{ STV090x_P1_FBERCPT4,		0x00 },
	{ STV090x_P1_FSPYBER,		0x10 },
	{ STV090x_P1_ERRCTRL1,		0x35 },
	{ STV090x_P1_ERRCTRL2,		0xc1 },
	{ STV090x_P1_CFRICFG,		0xf8 },
	{ STV090x_P1_NOSCFG,		0x1c },
	{ STV090x_P1_DMDTOM,		0x20 },
	{ STV090x_P1_CORRELMANT,	0x70 },
	{ STV090x_P1_CORRELABS,		0x88 },
	{ STV090x_P1_AGC2O,		0x5b },
	{ STV090x_P1_AGC2REF,		0x38 },
	{ STV090x_P1_CARCFG,		0xe4 },
	{ STV090x_P1_ACLC,		0x1A },
	{ STV090x_P1_BCLC,		0x09 },
	{ STV090x_P1_CARHDR,		0x08 },
	{ STV090x_P1_KREFTMG,		0xc1 },
	{ STV090x_P1_SFRSTEP,		0x58 },
	{ STV090x_P1_TMGCFG2,		0x01 },
	{ STV090x_P1_CAR2CFG,		0x26 },
	{ STV090x_P1_BCLC2S2Q,		0x86 },
	{ STV090x_P1_BCLC2S28,		0x86 },
	{ STV090x_P1_SMAPCOEF7,		0x77 },
	{ STV090x_P1_SMAPCOEF6,		0x85 },
	{ STV090x_P1_SMAPCOEF5,		0x77 },
	{ STV090x_P1_DMDCFG2,		0x3b },
	{ STV090x_P1_MODCODLST0,	0xff },
	{ STV090x_P1_MODCODLST1,	0xff },
	{ STV090x_P1_MODCODLST2,	0xff },
	{ STV090x_P1_MODCODLST3,	0xff },
	{ STV090x_P1_MODCODLST4,	0xff },
	{ STV090x_P1_MODCODLST5,	0xff },
	{ STV090x_P1_MODCODLST6,	0xff },
	{ STV090x_P1_MODCODLST7,	0xcc },
	{ STV090x_P1_MODCODLST8,	0xcc },
	{ STV090x_P1_MODCODLST9,	0xcc },
	{ STV090x_P1_MODCODLSTA,	0xcc },
	{ STV090x_P1_MODCODLSTB,	0xcc },
	{ STV090x_P1_MODCODLSTC,	0xcc },
	{ STV090x_P1_MODCODLSTD,	0xcc },
	{ STV090x_P1_MODCODLSTE,	0xcc },
	{ STV090x_P1_MODCODLSTF,	0xcf },
	{ STV090x_GENCFG,		0x1c },
	{ STV090x_NBITER_NF4,		0x37 },
	{ STV090x_NBITER_NF5,		0x29 },
	{ STV090x_NBITER_NF6,		0x37 },
	{ STV090x_NBITER_NF7,		0x33 },
	{ STV090x_NBITER_NF8,		0x31 },
	{ STV090x_NBITER_NF9,		0x2f },
	{ STV090x_NBITER_NF10,		0x39 },
	{ STV090x_NBITER_NF11,		0x3a },
	{ STV090x_NBITER_NF12,		0x29 },
	{ STV090x_NBITER_NF13,		0x37 },
	{ STV090x_NBITER_NF14,		0x33 },
	{ STV090x_NBITER_NF15,		0x2f },
	{ STV090x_NBITER_NF16,		0x39 },
	{ STV090x_NBITER_NF17,		0x3a },
	{ STV090x_NBITERNOERR,		0x04 },
	{ STV090x_GAINLLR_NF4,		0x0C },
	{ STV090x_GAINLLR_NF5,		0x0F },
	{ STV090x_GAINLLR_NF6,		0x11 },
	{ STV090x_GAINLLR_NF7,		0x14 },
	{ STV090x_GAINLLR_NF8,		0x17 },
	{ STV090x_GAINLLR_NF9,		0x19 },
	{ STV090x_GAINLLR_NF10,		0x20 },
	{ STV090x_GAINLLR_NF11,		0x21 },
	{ STV090x_GAINLLR_NF12,		0x0D },
	{ STV090x_GAINLLR_NF13,		0x0F },
	{ STV090x_GAINLLR_NF14,		0x13 },
	{ STV090x_GAINLLR_NF15,		0x1A },
	{ STV090x_GAINLLR_NF16,		0x1F },
	{ STV090x_GAINLLR_NF17,		0x21 },
	{ STV090x_RCCFGH,		0x20 },
	{ STV090x_P1_FECM,		0x01 }, /*disable the DSS mode */
	{ STV090x_P1_PRVIT,		0x2f }  /*disable puncture rate 6/7*/
};

static struct stv090x_reg stv0900_cut20_val[] = {

	{ STV090x_P2_DMDCFG3,		0xe8 },
	{ STV090x_P2_DMDCFG4,		0x10 },
	{ STV090x_P2_CARFREQ,		0x38 },
	{ STV090x_P2_CARHDR,		0x20 },
	{ STV090x_P2_KREFTMG,		0x5a },
	{ STV090x_P2_SMAPCOEF7,		0x06 },
	{ STV090x_P2_SMAPCOEF6,		0x00 },
	{ STV090x_P2_SMAPCOEF5,		0x04 },
	{ STV090x_P2_NOSCFG,		0x0c },
	{ STV090x_P1_DMDCFG3,		0xe8 },
	{ STV090x_P1_DMDCFG4,		0x10 },
	{ STV090x_P1_CARFREQ,		0x38 },
	{ STV090x_P1_CARHDR,		0x20 },
	{ STV090x_P1_KREFTMG,		0x5a },
	{ STV090x_P1_SMAPCOEF7,		0x06 },
	{ STV090x_P1_SMAPCOEF6,		0x00 },
	{ STV090x_P1_SMAPCOEF5,		0x04 },
	{ STV090x_P1_NOSCFG,		0x0c },
	{ STV090x_GAINLLR_NF4,		0x21 },
	{ STV090x_GAINLLR_NF5,		0x21 },
	{ STV090x_GAINLLR_NF6,		0x20 },
	{ STV090x_GAINLLR_NF7,		0x1F },
	{ STV090x_GAINLLR_NF8,		0x1E },
	{ STV090x_GAINLLR_NF9,		0x1E },
	{ STV090x_GAINLLR_NF10,		0x1D },
	{ STV090x_GAINLLR_NF11,		0x1B },
	{ STV090x_GAINLLR_NF12,		0x20 },
	{ STV090x_GAINLLR_NF13,		0x20 },
	{ STV090x_GAINLLR_NF14,		0x20 },
	{ STV090x_GAINLLR_NF15,		0x20 },
	{ STV090x_GAINLLR_NF16,		0x20 },
	{ STV090x_GAINLLR_NF17,		0x21 },
};

static struct stv090x_reg stv0903_cut20_val[] = {
	{ STV090x_P1_DMDCFG3,		0xe8 },
	{ STV090x_P1_DMDCFG4,		0x10 },
	{ STV090x_P1_CARFREQ,		0x38 },
	{ STV090x_P1_CARHDR,		0x20 },
	{ STV090x_P1_KREFTMG,		0x5a },
	{ STV090x_P1_SMAPCOEF7,		0x06 },
	{ STV090x_P1_SMAPCOEF6,		0x00 },
	{ STV090x_P1_SMAPCOEF5,		0x04 },
	{ STV090x_P1_NOSCFG,		0x0c },
	{ STV090x_GAINLLR_NF4,		0x21 },
	{ STV090x_GAINLLR_NF5,		0x21 },
	{ STV090x_GAINLLR_NF6,		0x20 },
	{ STV090x_GAINLLR_NF7,		0x1F },
	{ STV090x_GAINLLR_NF8,		0x1E },
	{ STV090x_GAINLLR_NF9,		0x1E },
	{ STV090x_GAINLLR_NF10,		0x1D },
	{ STV090x_GAINLLR_NF11,		0x1B },
	{ STV090x_GAINLLR_NF12,		0x20 },
	{ STV090x_GAINLLR_NF13,		0x20 },
	{ STV090x_GAINLLR_NF14,		0x20 },
	{ STV090x_GAINLLR_NF15,		0x20 },
	{ STV090x_GAINLLR_NF16,		0x20 },
	{ STV090x_GAINLLR_NF17,		0x21 }
};

/* Cut 2.0 Long Frame Tracking CR loop */
static struct stv090x_long_frame_crloop stv090x_s2_crl_cut20[] = {
	/* MODCOD  2MPon 2MPoff 5MPon 5MPoff 10MPon 10MPoff 20MPon 20MPoff 30MPon 30MPoff */
	{ STV090x_QPSK_12,  0x1f, 0x3f, 0x1e, 0x3f, 0x3d, 0x1f, 0x3d, 0x3e, 0x3d, 0x1e },
	{ STV090x_QPSK_35,  0x2f, 0x3f, 0x2e, 0x2f, 0x3d, 0x0f, 0x0e, 0x2e, 0x3d, 0x0e },
	{ STV090x_QPSK_23,  0x2f, 0x3f, 0x2e, 0x2f, 0x0e, 0x0f, 0x0e, 0x1e, 0x3d, 0x3d },
	{ STV090x_QPSK_34,  0x3f, 0x3f, 0x3e, 0x1f, 0x0e, 0x3e, 0x0e, 0x1e, 0x3d, 0x3d },
	{ STV090x_QPSK_45,  0x3f, 0x3f, 0x3e, 0x1f, 0x0e, 0x3e, 0x0e, 0x1e, 0x3d, 0x3d },
	{ STV090x_QPSK_56,  0x3f, 0x3f, 0x3e, 0x1f, 0x0e, 0x3e, 0x0e, 0x1e, 0x3d, 0x3d },
	{ STV090x_QPSK_89,  0x3f, 0x3f, 0x3e, 0x1f, 0x1e, 0x3e, 0x0e, 0x1e, 0x3d, 0x3d },
	{ STV090x_QPSK_910, 0x3f, 0x3f, 0x3e, 0x1f, 0x1e, 0x3e, 0x0e, 0x1e, 0x3d, 0x3d },
	{ STV090x_8PSK_35,  0x3c, 0x3e, 0x1c, 0x2e, 0x0c, 0x1e, 0x2b, 0x2d, 0x1b, 0x1d },
	{ STV090x_8PSK_23,  0x1d, 0x3e, 0x3c, 0x2e, 0x2c, 0x1e, 0x0c, 0x2d, 0x2b, 0x1d },
	{ STV090x_8PSK_34,  0x0e, 0x3e, 0x3d, 0x2e, 0x0d, 0x1e, 0x2c, 0x2d, 0x0c, 0x1d },
	{ STV090x_8PSK_56,  0x2e, 0x3e, 0x1e, 0x2e, 0x2d, 0x1e, 0x3c, 0x2d, 0x2c, 0x1d },
	{ STV090x_8PSK_89,  0x3e, 0x3e, 0x1e, 0x2e, 0x3d, 0x1e, 0x0d, 0x2d, 0x3c, 0x1d },
	{ STV090x_8PSK_910, 0x3e, 0x3e, 0x1e, 0x2e, 0x3d, 0x1e, 0x1d, 0x2d, 0x0d, 0x1d }
};

/* Cut 2.0 Long Frame Tracking CR loop */
static struct stv090x_long_frame_crloop stx7111_s2_crl_cut20[] = {
	/* MODCOD  2MPon 2MPoff 5MPon 5MPoff 10MPon 10MPoff 20MPon 20MPoff 30MPon 30MPoff */
	{STV090x_QPSK_12,  0x1c, 0x3c, 0x1b, 0x3c, 0x3a, 0x1c, 0x0b, 0x1c, 0x0b, 0x3b},
	{STV090x_QPSK_35,  0x2c, 0x3c, 0x2b, 0x2c, 0x3a, 0x0c, 0x0b, 0x1c, 0x0b, 0x3b},
	{STV090x_QPSK_23,  0x2c, 0x3c, 0x2b, 0x2c, 0x0b, 0x0c, 0x0b, 0x0c, 0x0b, 0x2b},
	{STV090x_QPSK_34,  0x3c, 0x3c, 0x3b, 0x1c, 0x0b, 0x3b, 0x0b, 0x0c, 0x0b, 0x2b},
	{STV090x_QPSK_45,  0x3c, 0x3c, 0x3b, 0x1c, 0x0b, 0x3b, 0x0b, 0x0c, 0x0b, 0x2b},
	{STV090x_QPSK_56,  0x3c, 0x3c, 0x3b, 0x1c, 0x0b, 0x3b, 0x0b, 0x0c, 0x0b, 0x2b},
	{STV090x_QPSK_89,  0x3c, 0x3c, 0x3b, 0x1c, 0x1b, 0x3b, 0x1b, 0x0c, 0x0b, 0x2b},
	{STV090x_QPSK_910, 0x0d, 0x3c, 0x3b, 0x1c, 0x1b, 0x3b, 0x1b, 0x0c, 0x0b, 0x2b},
	{STV090x_8PSK_35,  0x39, 0x19, 0x19, 0x09, 0x09, 0x38, 0x09, 0x09, 0x38, 0x29},
	{STV090x_8PSK_23,  0x1a, 0x19, 0x39, 0x19, 0x29, 0x38, 0x29, 0x29, 0x19, 0x38},
	{STV090x_8PSK_34,  0x0b, 0x28, 0x3a, 0x19, 0x0a, 0x28, 0x1a, 0x28, 0x0a, 0x28},
	{STV090x_8PSK_56,  0x2b, 0x0c, 0x1b, 0x2b, 0x2a, 0x1b, 0x3a, 0x2b, 0x2a, 0x1b},
	{STV090x_8PSK_89,  0x0c, 0x0c, 0x1b, 0x2b, 0x3a, 0x1b, 0x0b, 0x3b, 0x3a, 0x1b},
	{STV090x_8PSK_910, 0x0c, 0x0c, 0x1b, 0x2b, 0x3a, 0x1b, 0x0b, 0x3b, 0x3a, 0x1b}
};

/* Cut 3.0 Long Frame Tracking CR loop */
static	struct stv090x_long_frame_crloop stv090x_s2_crl_cut30[] = {
	/* MODCOD  2MPon 2MPoff 5MPon 5MPoff 10MPon 10MPoff 20MPon 20MPoff 30MPon 30MPoff */
	{ STV090x_QPSK_12,  0x3c, 0x2c, 0x0c, 0x2c, 0x1b, 0x2c, 0x1b, 0x1c, 0x0b, 0x3b },
	{ STV090x_QPSK_35,  0x0d, 0x0d, 0x0c, 0x0d, 0x1b, 0x3c, 0x1b, 0x1c, 0x0b, 0x3b },
	{ STV090x_QPSK_23,  0x1d, 0x0d, 0x0c, 0x1d, 0x2b, 0x3c, 0x1b, 0x1c, 0x0b, 0x3b },
	{ STV090x_QPSK_34,  0x1d, 0x1d, 0x0c, 0x1d, 0x2b, 0x3c, 0x1b, 0x1c, 0x0b, 0x3b },
	{ STV090x_QPSK_45,  0x2d, 0x1d, 0x1c, 0x1d, 0x2b, 0x3c, 0x2b, 0x0c, 0x1b, 0x3b },
	{ STV090x_QPSK_56,  0x2d, 0x1d, 0x1c, 0x1d, 0x2b, 0x3c, 0x2b, 0x0c, 0x1b, 0x3b },
	{ STV090x_QPSK_89,  0x3d, 0x2d, 0x1c, 0x1d, 0x3b, 0x3c, 0x2b, 0x0c, 0x1b, 0x3b },
	{ STV090x_QPSK_910, 0x3d, 0x2d, 0x1c, 0x1d, 0x3b, 0x3c, 0x2b, 0x0c, 0x1b, 0x3b },
	{ STV090x_8PSK_35,  0x39, 0x29, 0x39, 0x19, 0x19, 0x19, 0x19, 0x19, 0x09, 0x19 },
	{ STV090x_8PSK_23,  0x2a, 0x39, 0x1a, 0x0a, 0x39, 0x0a, 0x29, 0x39, 0x29, 0x0a },
	{ STV090x_8PSK_34,  0x2b, 0x3a, 0x1b, 0x1b, 0x3a, 0x1b, 0x1a, 0x0b, 0x1a, 0x3a },
	{ STV090x_8PSK_56,  0x0c, 0x1b, 0x3b, 0x3b, 0x1b, 0x3b, 0x3a, 0x3b, 0x3a, 0x1b },
	{ STV090x_8PSK_89,  0x0d, 0x3c, 0x2c, 0x2c, 0x2b, 0x0c, 0x0b, 0x3b, 0x0b, 0x1b },
	{ STV090x_8PSK_910, 0x0d, 0x0d, 0x2c, 0x3c, 0x3b, 0x1c, 0x0b, 0x3b, 0x0b, 0x1b }
};

/* Cut 2.0 Long Frame Tracking CR Loop */
static struct stv090x_long_frame_crloop stv090x_s2_apsk_crl_cut20[] = {
	/* MODCOD  2MPon 2MPoff 5MPon 5MPoff 10MPon 10MPoff 20MPon 20MPoff 30MPon 30MPoff */
	{ STV090x_16APSK_23,  0x0c, 0x0c, 0x0c, 0x0c, 0x1d, 0x0c, 0x3c, 0x0c, 0x2c, 0x0c },
	{ STV090x_16APSK_34,  0x0c, 0x0c, 0x0c, 0x0c, 0x0e, 0x0c, 0x2d, 0x0c, 0x1d, 0x0c },
	{ STV090x_16APSK_45,  0x0c, 0x0c, 0x0c, 0x0c, 0x1e, 0x0c, 0x3d, 0x0c, 0x2d, 0x0c },
	{ STV090x_16APSK_56,  0x0c, 0x0c, 0x0c, 0x0c, 0x1e, 0x0c, 0x3d, 0x0c, 0x2d, 0x0c },
	{ STV090x_16APSK_89,  0x0c, 0x0c, 0x0c, 0x0c, 0x2e, 0x0c, 0x0e, 0x0c, 0x3d, 0x0c },
	{ STV090x_16APSK_910, 0x0c, 0x0c, 0x0c, 0x0c, 0x2e, 0x0c, 0x0e, 0x0c, 0x3d, 0x0c },
	{ STV090x_32APSK_34,  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c },
	{ STV090x_32APSK_45,  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c },
	{ STV090x_32APSK_56,  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c },
	{ STV090x_32APSK_89,  0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c },
	{ STV090x_32APSK_910, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c }
};

/* Cut 3.0 Long Frame Tracking CR Loop */
static struct stv090x_long_frame_crloop	stv090x_s2_apsk_crl_cut30[] = {
	/* MODCOD  2MPon 2MPoff 5MPon 5MPoff 10MPon 10MPoff 20MPon 20MPoff 30MPon 30MPoff */
	{ STV090x_16APSK_23,  0x0a, 0x0a, 0x0a, 0x0a, 0x1a, 0x0a, 0x3a, 0x0a, 0x2a, 0x0a },
	{ STV090x_16APSK_34,  0x0a, 0x0a, 0x0a, 0x0a, 0x0b, 0x0a, 0x3b, 0x0a, 0x1b, 0x0a },
	{ STV090x_16APSK_45,  0x0a, 0x0a, 0x0a, 0x0a, 0x1b, 0x0a, 0x3b, 0x0a, 0x2b, 0x0a },
	{ STV090x_16APSK_56,  0x0a, 0x0a, 0x0a, 0x0a, 0x1b, 0x0a, 0x3b, 0x0a, 0x2b, 0x0a },
	{ STV090x_16APSK_89,  0x0a, 0x0a, 0x0a, 0x0a, 0x2b, 0x0a, 0x0c, 0x0a, 0x3b, 0x0a },
	{ STV090x_16APSK_910, 0x0a, 0x0a, 0x0a, 0x0a, 0x2b, 0x0a, 0x0c, 0x0a, 0x3b, 0x0a },
	{ STV090x_32APSK_34,  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a },
	{ STV090x_32APSK_45,  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a },
	{ STV090x_32APSK_56,  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a },
	{ STV090x_32APSK_89,  0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a },
	{ STV090x_32APSK_910, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a }
};

static struct stv090x_long_frame_crloop stv090x_s2_lowqpsk_crl_cut20[] = {
	/* MODCOD  2MPon 2MPoff 5MPon 5MPoff 10MPon 10MPoff 20MPon 20MPoff 30MPon 30MPoff */
	{ STV090x_QPSK_14,  0x0f, 0x3f, 0x0e, 0x3f, 0x2d, 0x2f, 0x2d, 0x1f, 0x3d, 0x3e },
	{ STV090x_QPSK_13,  0x0f, 0x3f, 0x0e, 0x3f, 0x2d, 0x2f, 0x3d, 0x0f, 0x3d, 0x2e },
	{ STV090x_QPSK_25,  0x1f, 0x3f, 0x1e, 0x3f, 0x3d, 0x1f, 0x3d, 0x3e, 0x3d, 0x2e }
};

static struct stv090x_long_frame_crloop	stv090x_s2_lowqpsk_crl_cut30[] = {
	/* MODCOD  2MPon 2MPoff 5MPon 5MPoff 10MPon 10MPoff 20MPon 20MPoff 30MPon 30MPoff */
	{ STV090x_QPSK_14,  0x0c, 0x3c, 0x0b, 0x3c, 0x2a, 0x2c, 0x2a, 0x1c, 0x3a, 0x3b },
	{ STV090x_QPSK_13,  0x0c, 0x3c, 0x0b, 0x3c, 0x2a, 0x2c, 0x3a, 0x0c, 0x3a, 0x2b },
	{ STV090x_QPSK_25,  0x1c, 0x3c, 0x1b, 0x3c, 0x3a, 0x1c, 0x3a, 0x3b, 0x3a, 0x2b }
};

/* Cut 2.0 Short Frame Tracking CR Loop */
static struct stv090x_short_frame_crloop stv090x_s2_short_crl_cut20[] = {
	/* MODCOD	  2M    5M    10M   20M   30M */
	{ STV090x_QPSK,   0x2f, 0x2e, 0x0e, 0x0e, 0x3d },
	{ STV090x_8PSK,   0x3e, 0x0e, 0x2d, 0x0d, 0x3c },
	{ STV090x_16APSK, 0x1e, 0x1e, 0x1e, 0x3d, 0x2d },
	{ STV090x_32APSK, 0x1e, 0x1e, 0x1e, 0x3d, 0x2d }
};

/* Cut 3.0 Short Frame Tracking CR Loop */
static struct stv090x_short_frame_crloop stv090x_s2_short_crl_cut30[] = {
	/* MODCOD  	  2M	5M    10M   20M	  30M */
	{ STV090x_QPSK,   0x2C, 0x2B, 0x0B, 0x0B, 0x3A },
	{ STV090x_8PSK,   0x3B, 0x0B, 0x2A, 0x0A, 0x39 },
	{ STV090x_16APSK, 0x1B, 0x1B, 0x1B, 0x3A, 0x2A },
	{ STV090x_32APSK, 0x1B, 0x1B, 0x1B, 0x3A, 0x2A }
};

static inline s32 comp2(s32 __x, s32 __width)
{
	if (__width == 32)
		return __x;
	else
		return (__x >= (1 << (__width - 1))) ? (__x - (1 << __width)) : __x;
}

static int stv090x_read_reg(struct stv090x_state *state, unsigned int reg)
{
	const struct stv090x_config *config = state->config;
	int ret;

	u8 b0[] = { reg >> 8, reg & 0xff };
	u8 buf;
	struct i2c_msg msg[] = {
		{ .addr	= config->address, .flags	= 0, 		.buf = b0,   .len = 2 },
		{ .addr	= config->address, .flags	= I2C_M_RD,	.buf = &buf, .len = 1 }
	};

	dprintk(150, "stv090x_read_reg config->address=0x%x \n",config->address);

	ret = i2c_transfer(state->i2c, msg, 2);
	if (ret != 2) {
		if (ret != -ERESTARTSYS)
			printk(	"Read error, Reg=[0x%02x], Status=%d\n",reg, ret);

		return ret < 0 ? ret : -EREMOTEIO;
	}

	dprintk(200, "%s: reg=0x%02x , result=0x%02x\n",__func__, reg, buf);
	return (unsigned int) buf;
}

static int stv090x_write_regs(struct stv090x_state *state, unsigned int reg, u8 *data, u32 count)
{
	const struct stv090x_config *config = state->config;
	int ret;
	u8 buf[2 + count];
	struct i2c_msg i2c_msg = { .addr = config->address, .flags = 0, .buf = buf, .len = 2 + count };

	buf[0] = reg >> 8;
	buf[1] = reg & 0xff;
	memcpy(&buf[2], data, count);

	{
		int i;

		dprintk(150, "%s [reg = 0x%04x, count = %d]:", __func__, reg, count);
		dprintk(150, "%s 0x%02x, 0x%02x", __func__, (reg >> 8) & 0xff, reg & 0xff );
		for (i = 0; i < count; i++)
		   dprintk(150, " 0x%02x", data[i]);
		dprintk(150, "\n");
	}

	ret = i2c_transfer(state->i2c, &i2c_msg, 1);

	if (ret != 1) {
		if (ret != -ERESTARTSYS)
			printk("Reg=[0x%04x], Data=[0x%02x ...], Count=%u, Status=%d\n",
				reg, data[0], count, ret);
		return ret < 0 ? ret : -EREMOTEIO;
	}

	return 0;
}

static int stv090x_write_reg(struct stv090x_state *state, unsigned int reg, u8 data)
{
	return stv090x_write_regs(state, reg, &data, 1);
}

static int stv090x_i2c_gate_ctrl(struct dvb_frontend *fe, int enable)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg;

	dprintk(200, "state->i2c->nr %d\n", state->i2c->nr);

	reg = STV090x_READ_DEMOD(state, I2CRPT);
	if (enable) {
		if (!(reg & 0x80))
		{
		   dprintk(250, "Enable Gate\n");
		   STV090x_SETFIELD_Px(reg, I2CT_ON_FIELD, 1);
		   if (STV090x_WRITE_DEMOD(state, I2CRPT, reg) < 0)
			   goto err;
		}
	} else {
		if (state->device != STX7111)
		{
		   dprintk(250, "Disable Gate\n");
		   STV090x_SETFIELD_Px(reg, I2CT_ON_FIELD, 0);
		   if ((STV090x_WRITE_DEMOD(state, I2CRPT, reg)) < 0)
			   goto err;

		   msleep(50);
       }
	}

	return 0;
err:
	printk("stv090x_i2c_gate_ctrl: I/O error\n");
	return -1;
}

static void stv090x_get_lock_tmg(struct stv090x_state *state)
{
	dprintk(100, "%s >\n", __func__);

	switch (state->algo) {
	case STV090x_BLIND_SEARCH:
		dprintk(100, "Blind Search\n");
		if (state->srate <= 1500000) {  /*10Msps< SR <=15Msps*/
			state->DemodTimeout = 1500;
			state->FecTimeout = 400;
		} else if (state->srate <= 5000000) {  /*10Msps< SR <=15Msps*/
			state->DemodTimeout = 1000;
			state->FecTimeout = 300;
		} else {  /*SR >20Msps*/
			state->DemodTimeout = 700;
			state->FecTimeout = 100;
		}
		break;

	case STV090x_COLD_SEARCH:
	case STV090x_WARM_SEARCH:
	default:
		dprintk(100, "Normal Search\n");
		if (state->srate <= 1000000) {  /*SR <=1Msps*/
			state->DemodTimeout = 4500;
			state->FecTimeout = 1700;
		} else if (state->srate <= 2000000) { /*1Msps < SR <= 2Msps */
			state->DemodTimeout = 2500;
			state->FecTimeout = 1100;
		} else if (state->srate <= 5000000) { /*2Msps < SR <= 5Msps */
			state->DemodTimeout = 1000;
			state->FecTimeout = 550;
		} else if (state->srate <= 10000000) { /*5Msps < SR <= 10Msps */
			state->DemodTimeout = 700;
			state->FecTimeout = 250;
		} else if (state->srate <= 20000000) { /*10Msps < SR <= 20Msps */
			state->DemodTimeout = 400;
			state->FecTimeout = 130;
		} else {   /*SR >20Msps*/
			state->DemodTimeout = 300;
			state->FecTimeout = 100;
		}
		break;
	}

	if (state->algo == STV090x_WARM_SEARCH)
		state->DemodTimeout /= 2;
	dprintk(100, "%s <\n", __func__);
}

static int stv090x_set_srate(struct stv090x_state *state, u32 srate)
{
	u32 sym;

	dprintk(10, "%s >\n", __func__);

	if (srate > 60000000) {
		sym  = (srate << 4); /* SR * 2^16 / master_clk */
		sym /= (state->mclk >> 12);
	} else if (srate > 6000000) {
		sym  = (srate << 6);
		sym /= (state->mclk >> 10);
	} else {
		sym  = (srate << 9);
		sym /= (state->mclk >> 7);
	}


        dprintk(100, "0x%x\n", (sym >> 8) & 0xff);
        dprintk(100, "0x%x\n", (sym & 0xff));

	if (STV090x_WRITE_DEMOD(state, SFRINIT1, (sym >> 8) & 0x7f) < 0) /* MSB */
		goto err;
	if (STV090x_WRITE_DEMOD(state, SFRINIT0, (sym & 0xff)) < 0) /* LSB */
		goto err;

	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_set_srate: I/O error\n");
	return -1;
}

static int stv090x_set_max_srate(struct stv090x_state *state, u32 clk, u32 srate)
{
	u32 sym;

	dprintk(10, "%s >\n", __func__);

	srate = 105 * (srate / 100);
	if (srate > 60000000) {
		sym  = (srate << 4); /* SR * 2^16 / master_clk */
		sym /= (state->mclk >> 12);
	} else if (srate > 6000000) {
		sym  = (srate << 6);
		sym /= (state->mclk >> 10);
	} else {
		sym  = (srate << 9);
		sym /= (state->mclk >> 7);
	}

	if (sym < 0x7fff) {
		if (STV090x_WRITE_DEMOD(state, SFRUP1, (sym >> 8) & 0x7f) < 0) /* MSB */
			goto err;
		if (STV090x_WRITE_DEMOD(state, SFRUP0, sym & 0xff) < 0) /* LSB */
			goto err;
	} else {
		if (STV090x_WRITE_DEMOD(state, SFRUP1, 0x7f) < 0) /* MSB */
			goto err;
		if (STV090x_WRITE_DEMOD(state, SFRUP0, 0xff) < 0) /* LSB */
			goto err;
	}

	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_set_max_srate: I/O error\n");
	return -1;
}

static int stv090x_set_min_srate(struct stv090x_state *state, u32 clk, u32 srate)
{
	u32 sym;

	dprintk(10, "%s >\n", __func__);

	srate = 95 * (srate / 100);
	if (srate > 60000000) {
		sym  = (srate << 4); /* SR * 2^16 / master_clk */
		sym /= (state->mclk >> 12);
	} else if (srate > 6000000) {
		sym  = (srate << 6);
		sym /= (state->mclk >> 10);
	} else {
		sym  = (srate << 9);
		sym /= (state->mclk >> 7);
	}

	if (STV090x_WRITE_DEMOD(state, SFRLOW1, ((sym >> 8) & 0xff)) < 0) /* MSB */
		goto err;
	if (STV090x_WRITE_DEMOD(state, SFRLOW0, (sym & 0xff)) < 0) /* LSB */
		goto err;

	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_set_min_srate: I/O error\n");
	return -1;
}

static u32 stv090x_car_width(u32 srate, enum stv090x_rolloff rolloff)
{
	u32 ro;

	dprintk(10, "%s >\n", __func__);

	switch (rolloff) {
	case STV090x_RO_20:
		ro = 20;
		break;
	case STV090x_RO_25:
		ro = 25;
		break;
	case STV090x_RO_35:
	default:
		ro = 35;
		break;
	}

	dprintk(10, "%s <\n", __func__);
	return srate + (srate * ro) / 100;
}

static int stv090x_set_vit_thacq(struct stv090x_state *state)
{
	dprintk(10, "%s >\n", __func__);
	if (STV090x_WRITE_DEMOD(state, VTH12, 0x96) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH23, 0x64) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH34, 0x36) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH56, 0x23) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH67, 0x1e) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH78, 0x19) < 0)
		goto err;
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_set_vit_thacq: I/O error\n");
	return -1;
}

static int stv090x_set_vit_thtracq(struct stv090x_state *state)
{
	dprintk(10, "%s >\n", __func__);
	if (STV090x_WRITE_DEMOD(state, VTH12, 0xd0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH23, 0x7d) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH34, 0x53) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH56, 0x2f) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH67, 0x24) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, VTH78, 0x1f) < 0)
		goto err;
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_set_vit_thtracq: I/O error\n");
	return -1;
}

static int stv090x_set_viterbi(struct stv090x_state *state)
{
	dprintk(10, "%s >\n", __func__);
	switch (state->search_mode) {
	case STV090x_SEARCH_AUTO:

		if (STV090x_WRITE_DEMOD(state, FECM, 0x10) < 0) /* DVB-S and DVB-S2 */
			goto err;
		if (STV090x_WRITE_DEMOD(state, PRVIT, 0x3f) < 0) /* all puncture rate */
			goto err;

		if (state->device != STX7111)
		{
		   if (STV090x_WRITE_DEMOD(state, FECM, 0x00) < 0) /* DVB-S and DVB-S2 */
			   goto err;
		   if (STV090x_WRITE_DEMOD(state, PRVIT, 0x2f) < 0) /* all puncture rate */
			   goto err;
        }
		break;
	case STV090x_SEARCH_DVBS1:
		if (STV090x_WRITE_DEMOD(state, FECM, 0x00) < 0) /* disable DSS */
			goto err;
		switch (state->fec) {
		case STV090x_PR12:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x01) < 0)
				goto err;
			break;

		case STV090x_PR23:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x02) < 0)
				goto err;
			break;

		case STV090x_PR34:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x04) < 0)
				goto err;
			break;

		case STV090x_PR56:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x08) < 0)
				goto err;
			break;

		case STV090x_PR78:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x20) < 0)
				goto err;
			break;

		default:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x2f) < 0) /* all */
				goto err;
			break;
		}
		break;
	case STV090x_SEARCH_DSS:
		if (STV090x_WRITE_DEMOD(state, FECM, 0x80) < 0)
			goto err;
		switch (state->fec) {
		case STV090x_PR12:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x01) < 0)
				goto err;
			break;

		case STV090x_PR23:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x02) < 0)
				goto err;
			break;

		case STV090x_PR67:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x10) < 0)
				goto err;
			break;

		default:
			if (STV090x_WRITE_DEMOD(state, PRVIT, 0x13) < 0) /* 1/2, 2/3, 6/7 */
				goto err;
			break;
		}
		break;
	default:
		break;
	}
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_set_viterbi: I/O error\n");
	return -1;
}

static int stv090x_stop_modcod(struct stv090x_state *state)
{
	dprintk(10, "%s >\n", __func__);
	if (STV090x_WRITE_DEMOD(state, MODCODLST0, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST1, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST2, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST3, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST4, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST5, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST6, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST7, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST8, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST9, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTA, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTB, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTC, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTD, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTE, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTF, 0xff) < 0)
		goto err;
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_stop_modcod: I/O error\n");
	return -1;
}

static int stv090x_activate_modcod(struct stv090x_state *state)
{
	dprintk(10, "%s >\n", __func__);

    if (state->device == STX7111)
	{
	   if (STV090x_WRITE_DEMOD(state, MODCODLST0, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST1, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST2, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST3, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST4, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST5, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST6, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST7, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST8, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST9, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTA, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTB, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTC, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTD, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTE, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTF, 0xff) < 0)
		   goto err;

    }
	else
	{
	   if (STV090x_WRITE_DEMOD(state, MODCODLST0, 0xff) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST1, 0xfc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST2, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST3, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST4, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST5, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST6, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST7, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST8, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLST9, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTA, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTB, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTC, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTD, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTE, 0xcc) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, MODCODLSTF, 0xcf) < 0)
		   goto err;
	}

	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_activate_modcod: I/O error\n");
	return -1;
}

static int stv090x_activate_modcod_single(struct stv090x_state *state)
{
	dprintk(10, "%s >\n", __func__);

	if (STV090x_WRITE_DEMOD(state, MODCODLST0, 0xff) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST1, 0xf0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST2, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST3, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST4, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST5, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST6, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST7, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST8, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLST9, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTA, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTB, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTC, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTD, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTE, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, MODCODLSTF, 0x0f) < 0)
		goto err;

	dprintk(10, "%s <\n", __func__);
	return 0;

err:
	printk("stv090x_activate_modcod_single: I/O error\n");
	return -1;
}

static int stv090x_vitclk_ctl(struct stv090x_state *state, int enable)
{
	u32 reg;

	dprintk(10, "%s >\n", __func__);

	switch (state->demod) {
	case STV090x_DEMODULATOR_0:
		mutex_lock(&demod_lock);
		reg = stv090x_read_reg(state, STV090x_STOPCLK2);
		STV090x_SETFIELD(reg, STOP_CLKVIT1_FIELD, enable);
		if (stv090x_write_reg(state, STV090x_STOPCLK2, reg) < 0)
			goto err;
		mutex_unlock(&demod_lock);
		break;

	case STV090x_DEMODULATOR_1:
		mutex_lock(&demod_lock);
		reg = stv090x_read_reg(state, STV090x_STOPCLK2);
		STV090x_SETFIELD(reg, STOP_CLKVIT2_FIELD, enable);
		if (stv090x_write_reg(state, STV090x_STOPCLK2, reg) < 0)
			goto err;
		mutex_unlock(&demod_lock);
		break;

	default:
		printk("Wrong demodulator!\n");
		break;
	}
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	mutex_unlock(&demod_lock);
	printk("stv090x_vitclk_ctl: I/O error\n");
	return -1;
}

static int stv090x_dvbs_track_crl(struct stv090x_state *state)
{
	dprintk(10, "%s >\n", __func__);
	if (state->dev_ver >= 0x30) {
		/* Set ACLC BCLC optimised value vs SR */
		if (state->srate >= 15000000) {
			if (STV090x_WRITE_DEMOD(state, ACLC, 0x2b) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, BCLC, 0x1a) < 0)
				goto err;
		} else if ((state->srate >= 7000000) && (15000000 > state->srate)) {
			if (STV090x_WRITE_DEMOD(state, ACLC, 0x0c) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, BCLC, 0x1b) < 0)
				goto err;
		} else if (state->srate < 7000000) {
			if (STV090x_WRITE_DEMOD(state, ACLC, 0x2c) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, BCLC, 0x1c) < 0)
				goto err;
		}

	} else {
		/* Cut 2.0 */
		if (state->device == STX7111)
		{
		   if (STV090x_WRITE_DEMOD(state, ACLC, 0x2b) < 0)
			   goto err;
		   if (STV090x_WRITE_DEMOD(state, BCLC, 0x1a) < 0)
			   goto err;
        }
		else
		{
		   if (STV090x_WRITE_DEMOD(state, ACLC, 0x1a) < 0)
			   goto err;
		   if (STV090x_WRITE_DEMOD(state, BCLC, 0x09) < 0)
			   goto err;
		}
	}
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_dvbs_track_crl: I/O error\n");
	return -1;
}

static int stv090x_delivery_search(struct stv090x_state *state)
{
	u32 reg;

	dprintk(10, "%s >\n", __func__);

	switch (state->search_mode) {
	case STV090x_SEARCH_DVBS1:
	case STV090x_SEARCH_DSS:
		reg = STV090x_READ_DEMOD(state, DMDCFGMD);
		STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 1);
		STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 0);
		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;


		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;

		/* Activate Viterbi decoder in legacy search,
		 * do not use FRESVIT1, might impact VITERBI2
		 */
		if (stv090x_vitclk_ctl(state, 0) < 0)
			goto err;

		if (stv090x_dvbs_track_crl(state) < 0)
			goto err;

		if (STV090x_WRITE_DEMOD(state, CAR2CFG, 0x22) < 0) /* disable DVB-S2 */
			goto err;

		if (stv090x_set_vit_thacq(state) < 0)
			goto err;
		if (stv090x_set_viterbi(state) < 0)
			goto err;
		break;

	case STV090x_SEARCH_DVBS2:
		reg = STV090x_READ_DEMOD(state, DMDCFGMD);
		STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 0);
		STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 0);
		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;
		STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 1);
		STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 1);
		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;

		if (stv090x_vitclk_ctl(state, 1) < 0)
			goto err;

		if (STV090x_WRITE_DEMOD(state, ACLC, 0x1a) < 0) /* stop DVB-S CR loop */
			goto err;
		if (STV090x_WRITE_DEMOD(state, BCLC, 0x09) < 0)
			goto err;

		if (state->dev_ver <= 0x20) {
			/* enable S2 carrier loop */
			if (STV090x_WRITE_DEMOD(state, CAR2CFG, 0x26) < 0)
				goto err;
		} else {
			/* > Cut 3: Stop carrier 3 */
			if (STV090x_WRITE_DEMOD(state, CAR2CFG, 0x66) < 0)
				goto err;
		}

		if (state->demod_mode != STV090x_SINGLE) {
			/* Cut 2: enable link during search */
			if (stv090x_activate_modcod(state) < 0)
				goto err;
		} else {
			/* Single demodulator
			 * Authorize SHORT and LONG frames,
			 * QPSK, 8PSK, 16APSK and 32APSK
			 */
			if (stv090x_activate_modcod_single(state) < 0)
				goto err;
		}

		break;

	case STV090x_SEARCH_AUTO:
	default:
		/* enable DVB-S2 and DVB-S2 in Auto MODE */

                if (state->device == STX7111)
		{
		   /* first disable ... */
		   reg = STV090x_READ_DEMOD(state, DMDCFGMD);
		   STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 0);
		   STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 0);
		   if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			   goto err;
                }

		reg = STV090x_READ_DEMOD(state, DMDCFGMD);
		STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 1);
		STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 1);
		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;

                if (state->device == STX7111)
		{
		   if (stv090x_vitclk_ctl(state, 0) < 0)
			   goto err;
                }

		if (stv090x_dvbs_track_crl(state) < 0)
			goto err;

		if (state->dev_ver <= 0x20) {
			/* enable S2 carrier loop */
			if (STV090x_WRITE_DEMOD(state, CAR2CFG, 0x26) < 0)
				goto err;
		} else {
			/* > Cut 3: Stop carrier 3 */
			if (STV090x_WRITE_DEMOD(state, CAR2CFG, 0x66) < 0)
				goto err;
		}

                if (state->device != STX7111)
		{

		   if (state->demod_mode != STV090x_SINGLE) {
			   /* Cut 2: enable link during search */
			   if (stv090x_activate_modcod(state) < 0)
				   goto err;
		   } else {
			   /* Single demodulator
			    * Authorize SHORT and LONG frames,
			    * QPSK, 8PSK, 16APSK and 32APSK
			    */
			   if (stv090x_activate_modcod_single(state) < 0)
				   goto err;
		   }

		   if (state->srate >= 2000000) {
			   /* Srate >= 2MSPS, Viterbi threshold to acquire */
			   if (stv090x_set_vit_thacq(state) < 0)
				   goto err;
		   } else {
			   /* Srate < 2MSPS, Reset Viterbi thresholdto track
			    * and then re-acquire
			    */
			   if (stv090x_set_vit_thtracq(state) < 0)
				   goto err;
		   }

		   if (stv090x_set_viterbi(state) < 0)
			   goto err;
                } else
		{
		   if (state->srate >= 2000000) {
			   /* Srate >= 2MSPS, Viterbi threshold to acquire */
			   if (stv090x_set_vit_thacq(state) < 0)
				   goto err;
		   } else {
			   /* Srate < 2MSPS, Reset Viterbi thresholdto track
			    * and then re-acquire
			    */
			   if (stv090x_set_vit_thtracq(state) < 0)
				   goto err;
		   }

		   if (stv090x_set_viterbi(state) < 0)
			   goto err;

		   if (state->demod_mode != STV090x_SINGLE) {
			   /* Cut 2: enable link during search */
			   if (stv090x_activate_modcod(state) < 0)
				   goto err;
		   } else {
			   /* Single demodulator
			    * Authorize SHORT and LONG frames,
			    * QPSK, 8PSK, 16APSK and 32APSK
			    */
			   if (stv090x_activate_modcod_single(state) < 0)
				   goto err;
		   }
		}
		break;
	}
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_delivery_search: I/O error\n");
	return -1;
}

static int stv090x_start_search(struct stv090x_state *state)
{
	u32 reg, freq_abs;
	s16 freq;

	dprintk(10, "%s >\n", __func__);

	/* Reset demodulator */
	reg = STV090x_READ_DEMOD(state, DMDISTATE);
	STV090x_SETFIELD_Px(reg, I2C_DEMOD_MODE_FIELD, 0x1f);
	if (STV090x_WRITE_DEMOD(state, DMDISTATE, reg) < 0)
		goto err;

	if (state->dev_ver <= 0x20) {
		if (state->srate <= 5000000) {
			if (state->device == STX7111)
			{
			   if (STV090x_WRITE_DEMOD(state, CARCFG, 0x46) < 0)
				   goto err;
            } else
			{
			   if (STV090x_WRITE_DEMOD(state, CARCFG, 0x44) < 0)
				   goto err;
			}

			if (STV090x_WRITE_DEMOD(state, CFRUP1, 0x0f) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, CFRUP1, 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, CFRLOW1, 0xf0) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, CFRLOW0, 0x00) < 0)
				goto err;

			/*enlarge the timing bandwith for Low SR*/
			if (STV090x_WRITE_DEMOD(state, RTCS2, 0x68) < 0)
				goto err;
		} else {
			if (state->device == STX7111)
			{
			   /* If the symbol rate is >5 Msps
			   Set The carrier search up and low to auto mode */
			   if (STV090x_WRITE_DEMOD(state, CARCFG, 0xc6) < 0)
				   goto err;
			} else
			{
			   /* If the symbol rate is >5 Msps
			   Set The carrier search up and low to auto mode */
			   if (STV090x_WRITE_DEMOD(state, CARCFG, 0xc4) < 0)
				   goto err;
			}
			/*reduce the timing bandwith for high SR*/
			if (STV090x_WRITE_DEMOD(state, RTCS2, 0x44) < 0)
				goto err;
		}
	} else {
		/* >= Cut 3 */
		if (state->srate <= 5000000) {
			/* enlarge the timing bandwith for Low SR */
			STV090x_WRITE_DEMOD(state, RTCS2, 0x68);
		} else {
			/* reduce timing bandwith for high SR */
			STV090x_WRITE_DEMOD(state, RTCS2, 0x44);
		}

		/* Set CFR min and max to manual mode */
		STV090x_WRITE_DEMOD(state, CARCFG, 0x46);

		if (state->algo == STV090x_WARM_SEARCH) {
			/* WARM Start
			 * CFR min = -1MHz,
			 * CFR max = +1MHz
			 */
			freq_abs  = 1000 << 16;
			freq_abs /= (state->mclk / 1000);
			freq      = (s16) freq_abs;
		} else {
			/* COLD Start
			 * CFR min =- (SearchRange / 2 + 600KHz)
			 * CFR max = +(SearchRange / 2 + 600KHz)
			 * (600KHz for the tuner step size)
			 */
			freq_abs  = (state->search_range / 2000) + 600;
			freq_abs  = freq_abs << 16;
			freq_abs /= (state->mclk / 1000);
			freq      = (s16) freq_abs;
		}

		if (STV090x_WRITE_DEMOD(state, CFRUP1, MSB(freq)) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRUP1, LSB(freq)) < 0)
			goto err;

		freq *= -1;

		if (STV090x_WRITE_DEMOD(state, CFRLOW1, MSB(freq)) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRLOW0, LSB(freq)) < 0)
			goto err;

	}

	if (STV090x_WRITE_DEMOD(state, CFRINIT1, 0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, CFRINIT0, 0) < 0)
		goto err;

	if (state->dev_ver >= 0x20) {
		if (STV090x_WRITE_DEMOD(state, EQUALCFG, 0x41) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, FFECFG, 0x41) < 0)
			goto err;

		if ((state->search_mode == (enum stv090x_search)STV090x_DVBS1)	||
			(state->search_mode == (enum stv090x_search)STV090x_DSS)	||
			(state->search_mode == STV090x_SEARCH_AUTO)) {

			if (STV090x_WRITE_DEMOD(state, VITSCALE, 0x82) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, VAVSRVIT, 0x00) < 0)
				goto err;
		}
	}

	if (STV090x_WRITE_DEMOD(state, SFRSTEP, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, TMGTHRISE, 0xe0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, TMGTHFALL, 0xc0) < 0)
		goto err;

	reg = STV090x_READ_DEMOD(state, DMDCFGMD);
	STV090x_SETFIELD_Px(reg, SCAN_ENABLE_FIELD, 0);
	STV090x_SETFIELD_Px(reg, CFR_AUTOSCAN_FIELD, 0);
	if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
		goto err;
	reg = STV090x_READ_DEMOD(state, DMDCFG2);
	STV090x_SETFIELD_Px(reg, S1S2_SEQUENTIAL_FIELD, 0x0);
	if (STV090x_WRITE_DEMOD(state, DMDCFG2, reg) < 0)
		goto err;

    if (state->device == STX7111)
	{
	   if (STV090x_WRITE_DEMOD(state, RTC, 0x88) < 0)
		   goto err;
    }

	if (state->dev_ver >= 0x20) {
		/*Frequency offset detector setting*/
		if (state->srate < 2000000) {
			if (state->dev_ver <= 0x20) {
				/* Cut 2 */
				if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x39) < 0)
					goto err;
			} else {
				/* Cut 2 */
				if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x89) < 0)
					goto err;
			}
			if (STV090x_WRITE_DEMOD(state, CARHDR, 0x40) < 0)
				goto err;
		}

		if (state->srate < 10000000) {
			if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x4c) < 0)
				goto err;
		} else {
			if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x4b) < 0)
				goto err;
		}
	} else {
		if (state->srate < 10000000) {
			if (STV090x_WRITE_DEMOD(state, CARFREQ, 0xef) < 0)
				goto err;
		} else {
			if (STV090x_WRITE_DEMOD(state, CARFREQ, 0xed) < 0)
				goto err;
		}
	}

	switch (state->algo) {
	case STV090x_WARM_SEARCH:
		/* The symbol rate and the exact
		 * carrier Frequency are known
		 */
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1f) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x15) < 0)
			goto err;
		break;

	case STV090x_COLD_SEARCH:
		/* The symbol rate is known */
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1f) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x15) < 0)
			goto err;
		break;

	default:
		break;
	}
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_start_search: I/O error\n");
	return -1;
}

static int stv090x_get_agc2_min_level(struct stv090x_state *state)
{
	u32 agc2_min = 0, agc2 = 0, freq_init, freq_step, reg;
	s32 i, j, steps, dir;

	dprintk(10, "%s >\n", __func__);

	if (STV090x_WRITE_DEMOD(state, AGC2REF, 0x38) < 0)
		goto err;
	reg = STV090x_READ_DEMOD(state, DMDCFGMD);
	STV090x_SETFIELD_Px(reg, SCAN_ENABLE_FIELD, 1);
	STV090x_SETFIELD_Px(reg, CFR_AUTOSCAN_FIELD, 1);
	if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
		goto err;

	if (STV090x_WRITE_DEMOD(state, SFRUP1, 0x83) < 0) /* SR = 65 Msps Max */
		goto err;
	if (STV090x_WRITE_DEMOD(state, SFRUP0, 0xc0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, SFRLOW1, 0x82) < 0) /* SR= 400 ksps Min */
		goto err;
	if (STV090x_WRITE_DEMOD(state, SFRLOW0, 0xa0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, DMDTOM, 0x00) < 0) /* stop acq @ coarse carrier state */
		goto err;
	if (stv090x_set_srate(state, 1000000) < 0)
		goto err;

	steps  = -1 + state->search_range / 1000000;
	steps /= 2;
	steps  = (2 * steps) + 1;
	if (steps < 0)
		steps = 1;

	dir = 1;
	freq_step = (1000000 * 256) / (state->mclk / 256);
	freq_init = 0;

	for (i = 0; i < steps; i++) {
		if (dir > 0)
			freq_init = freq_init + (freq_step * i);
		else
			freq_init = freq_init - (freq_step * i);

		dir = -1;

		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x5c) < 0) /* Demod RESET */
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRINIT1, (freq_init >> 8) & 0xff) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRINIT0, freq_init & 0xff) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x58) < 0) /* Demod RESET */
			goto err;
		msleep(10);
		for (j = 0; j < 10; j++) {
			agc2 += STV090x_READ_DEMOD(state, AGC2I1) << 8;
			agc2 |= STV090x_READ_DEMOD(state, AGC2I0);
		}
		agc2 /= 10;
		agc2_min = 0xffff;
		if (agc2 < 0xffff)
			agc2_min = agc2;
	}

	dprintk(10, "%s <\n", __func__);
	return agc2_min;
err:
	printk("stv090x_get_agc2_min_level: I/O error\n");
	return -1;
}

static u32 stv090x_get_srate(struct stv090x_state *state, u32 clk)
{
	u8 r3, r2, r1, r0;
	s32 srate, int_1, int_2, tmp_1, tmp_2;

	dprintk(10, "%s >\n", __func__);

	r3 = STV090x_READ_DEMOD(state, SFR3);
	r2 = STV090x_READ_DEMOD(state, SFR2);
	r1 = STV090x_READ_DEMOD(state, SFR1);
	r0 = STV090x_READ_DEMOD(state, SFR0);

	srate = ((r3 << 24) | (r2 << 16) | (r1 <<  8) | r0);

	int_1 = clk >> 16;
	int_2 = srate >> 16;

	tmp_1 = clk % 0x10000;
	tmp_2 = srate % 0x10000;

	srate = (int_1 * int_2) +
		((int_1 * tmp_2) >> 16) +
		((int_2 * tmp_1) >> 16);

	dprintk(10, "%s srate %d<\n", __func__, srate);
	return srate;
}

static u32 stv090x_srate_srch_coarse(struct stv090x_state *state)
{
	struct dvb_frontend *fe = &state->frontend;

	int tmg_lock = 0, i;
	s32 tmg_cpt = 0, dir = 1, steps, cur_step = 0, freq;
	u32 srate_coarse = 0, agc2 = 0, car_step = 1200, reg;

	dprintk(10, "%s >\n", __func__);

	reg = STV090x_READ_DEMOD(state, DMDISTATE);
	STV090x_SETFIELD_Px(reg, I2C_DEMOD_MODE_FIELD, 0x1f); /* Demod RESET */
	if (STV090x_WRITE_DEMOD(state, DMDISTATE, reg) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, TMGCFG, 0x12) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, TMGTHRISE, 0xf0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, TMGTHFALL, 0xe0) < 0)
		goto err;
	reg = STV090x_READ_DEMOD(state, DMDCFGMD);
	STV090x_SETFIELD_Px(reg, SCAN_ENABLE_FIELD, 1);
	STV090x_SETFIELD_Px(reg, CFR_AUTOSCAN_FIELD, 1);
	if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
		goto err;

	if (STV090x_WRITE_DEMOD(state, SFRUP1, 0x83) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, SFRUP0, 0xc0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, SFRLOW1, 0x82) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, SFRLOW0, 0xa0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, DMDTOM, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, AGC2REF, 0x60) < 0)
		goto err;

	if (state->dev_ver >= 0x30) {
		if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x99) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, SFRSTEP, 0x95) < 0)
			goto err;

	} else if (state->dev_ver >= 0x20) {
		if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x6a) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, SFRSTEP, 0x95) < 0)
			goto err;
	}

	if (state->srate <= 2000000)
		car_step = 1000;
	else if (state->srate <= 5000000)
		car_step = 2000;
	else if (state->srate <= 12000000)
		car_step = 3000;
	else
		car_step = 5000;

	steps  = -1 + ((state->search_range / 1000) / car_step);
	steps /= 2;
	steps  = (2 * steps) + 1;
	if (steps < 0)
		steps = 1;
	else if (steps > 10) {
		steps = 11;
		car_step = (state->search_range / 1000) / 10;
	}
	cur_step = 0;
	dir = 1;
	freq = state->frequency;

	while ((!tmg_lock) && (cur_step < steps)) {
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x5f) < 0) /* Demod RESET */
			goto err;
		reg = STV090x_READ_DEMOD(state, DMDISTATE);
		STV090x_SETFIELD_Px(reg, I2C_DEMOD_MODE_FIELD, 0x00); /* trigger acquisition */
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, reg) < 0)
			goto err;
		msleep(50);
		for (i = 0; i < 10; i++) {
			reg = STV090x_READ_DEMOD(state, DSTATUS);
			if (STV090x_GETFIELD_Px(reg, TMGLOCK_QUALITY_FIELD) >= 2)
				tmg_cpt++;
			agc2 += STV090x_READ_DEMOD(state, AGC2I1) << 8;
			agc2 |= STV090x_READ_DEMOD(state, AGC2I0);
		}
		agc2 /= 10;
		srate_coarse = stv090x_get_srate(state, state->mclk);
		cur_step++;
		dir *= -1;
		if ((tmg_cpt >= 5) && (agc2 < 0x1f00) && (srate_coarse < 55000000) && (srate_coarse > 850000))
			tmg_lock = 1;
		else if (cur_step < steps) {
			if (dir > 0)
				freq += cur_step * car_step;
			else
				freq -= cur_step * car_step;

			/* Setup tuner */
			if (stv090x_i2c_gate_ctrl(fe, 1) < 0)
				goto err;

			if (state->config->tuner_set_frequency) {
				if (state->config->tuner_set_frequency(fe, state->frequency) < 0)
					goto err;
			}

			if (state->config->tuner_set_bandwidth) {
				if (state->config->tuner_set_bandwidth(fe, state->tuner_bw) < 0)
					goto err;
			}

			if (stv090x_i2c_gate_ctrl(fe, 0) < 0)
				goto err;

			msleep(50);

			if (stv090x_i2c_gate_ctrl(fe, 1) < 0)
				goto err;

			if (state->config->tuner_get_status) {
				if (state->config->tuner_get_status(fe, &reg) < 0)
					goto err;
			}

			if (reg)
				dprintk(50, "2. Tuner phase locked\n");
			else
				dprintk(50, "2. Tuner unlocked\n");

			if (stv090x_i2c_gate_ctrl(fe, 0) < 0)
				goto err;

		}
	}
	if (!tmg_lock)
		srate_coarse = 0;
	else
		srate_coarse = stv090x_get_srate(state, state->mclk);

	dprintk(50, "%s srate_coarse %d<\n", __func__, srate_coarse);

	return srate_coarse;
err:
	printk("stv090x_srate_srch_coarse: I/O error\n");
	return -1;
}

static u32 stv090x_srate_srch_fine(struct stv090x_state *state)
{
	u32 srate_coarse, freq_coarse, sym, reg;

	srate_coarse = stv090x_get_srate(state, state->mclk);
	freq_coarse  = STV090x_READ_DEMOD(state, CFR2) << 8;
	freq_coarse |= STV090x_READ_DEMOD(state, CFR1);
	sym = 13 * (srate_coarse / 10); /* SFRUP = SFR + 30% */

	dprintk(10, "%s >\n", __func__);

	if (sym < state->srate)
		srate_coarse = 0;
	else {
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1f) < 0) /* Demod RESET */
			goto err;
		if (STV090x_WRITE_DEMOD(state, TMGCFG2, 0x01) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, TMGTHRISE, 0x20) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, TMGTHFALL, 0x00) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, TMGCFG, 0xd2) < 0)
			goto err;
		reg = STV090x_READ_DEMOD(state, DMDCFGMD);
		STV090x_SETFIELD_Px(reg, CFR_AUTOSCAN_FIELD, 0x00);
		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;

		if (state->dev_ver >= 0x30) {
			if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x79) < 0)
				goto err;
		} else if (state->dev_ver >= 0x20) {
			if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x49) < 0)
				goto err;
		}

		if (srate_coarse > 3000000) {
			sym  = 13 * (srate_coarse / 10); /* SFRUP = SFR + 30% */
			sym  = (sym / 1000) * 65536;
			sym /= (state->mclk / 1000);
			if (STV090x_WRITE_DEMOD(state, SFRUP1, (sym >> 8) & 0x7f) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, SFRUP0, sym & 0xff) < 0)
				goto err;
			sym  = 10 * (srate_coarse / 13); /* SFRLOW = SFR - 30% */
			sym  = (sym / 1000) * 65536;
			sym /= (state->mclk / 1000);
			if (STV090x_WRITE_DEMOD(state, SFRLOW1, (sym >> 8) & 0x7f) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, SFRLOW0, sym & 0xff) < 0)
				goto err;
			sym  = (srate_coarse / 1000) * 65536;
			sym /= (state->mclk / 1000);
			if (STV090x_WRITE_DEMOD(state, SFRINIT1, (sym >> 8) & 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, SFRINIT0, sym & 0xff) < 0)
				goto err;
		} else {
			sym  = 13 * (srate_coarse / 10); /* SFRUP = SFR + 30% */
			sym  = (sym / 100) * 65536;
			sym /= (state->mclk / 100);
			if (STV090x_WRITE_DEMOD(state, SFRUP1, (sym >> 8) & 0x7f) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, SFRUP0, sym & 0xff) < 0)
				goto err;
			sym  = 10 * (srate_coarse / 14); /* SFRLOW = SFR - 30% */
			sym  = (sym / 100) * 65536;
			sym /= (state->mclk / 100);
			if (STV090x_WRITE_DEMOD(state, SFRLOW1, (sym >> 8) & 0x7f) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, SFRLOW0, sym & 0xff) < 0)
				goto err;
			sym  = (srate_coarse / 100) * 65536;
			sym /= (state->mclk / 100);
			if (STV090x_WRITE_DEMOD(state, SFRINIT1, (sym >> 8) & 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, SFRINIT0, sym & 0xff) < 0)
				goto err;
		}
		if (STV090x_WRITE_DEMOD(state, DMDTOM, 0x20) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRINIT1, (freq_coarse >> 8) & 0xff) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRINIT0, freq_coarse & 0xff) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x15) < 0) /* trigger acquisition */
			goto err;
	}

	dprintk(10, "%s <\n", __func__);
	return srate_coarse;

err:
	printk("stv090x_srate_srch_fine: I/O error\n");
	return -1;
}

static int stv090x_get_dmdlock(struct stv090x_state *state, s32 timeout)
{
	s32 timer = 0, lock = 0;
	u32 reg;
	u8 stat;

	dprintk(50, "%s >\n", __func__);
	while ((timer < timeout) && (!lock)) {
		reg = STV090x_READ_DEMOD(state, DMDSTATE);
		stat = STV090x_GETFIELD_Px(reg, HEADER_MODE_FIELD);

                dprintk(100, "demod stat = %d\n", stat);
		switch (stat) {
		case 0: /* searching */
		case 1: /* first PLH detected */
		default:
			dprintk(150, "Demodulator searching ..\n");
			lock = 0;
			break;
		case 2: /* DVB-S2 mode */
		case 3: /* DVB-S1/legacy mode */
			reg = STV090x_READ_DEMOD(state, DSTATUS);
			lock = STV090x_GETFIELD_Px(reg, LOCK_DEFINITIF_FIELD);
			break;
		}

		if (!lock)
			msleep(10);
		else
			printk("Demodulator acquired LOCK\n");

		timer += 10;
	}
	if (lock)
	   dprintk(50, "%s lock %d<\n", __func__, lock);
	return lock;
}

static int stv090x_blind_search(struct stv090x_state *state)
{
	u32 agc2, reg, srate_coarse;
	s32 timeout_dmd = 500, cpt_fail, agc2_ovflw, i;
	u8 k_ref, k_max, k_min;
	int coarse_fail, lock;

	dprintk(10, "%s >\n", __func__);

	k_max = 120;
	k_min = 30;

	agc2 = stv090x_get_agc2_min_level(state);

	if (agc2 > STV090x_SEARCH_AGC2_TH(state->dev_ver)) {
		lock = 0;
	} else {

		if (state->dev_ver <= 0x20) {
			if (STV090x_WRITE_DEMOD(state, CARCFG, 0xc4) < 0)
				goto err;
		} else {
			/* > Cut 3 */
			if (STV090x_WRITE_DEMOD(state, CARCFG, 0x06) < 0)
				goto err;
		}

		if (STV090x_WRITE_DEMOD(state, RTCS2, 0x44) < 0)
			goto err;

		if (state->dev_ver >= 0x20) {
			if (STV090x_WRITE_DEMOD(state, EQUALCFG, 0x41) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, FFECFG, 0x41) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, VITSCALE, 0x82) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, VAVSRVIT, 0x00) < 0) /* set viterbi hysteresis */
				goto err;
		}

		k_ref = k_max;
		do {
			if (STV090x_WRITE_DEMOD(state, KREFTMG, k_ref) < 0)
				goto err;
			if (stv090x_srate_srch_coarse(state) != 0) {
				srate_coarse = stv090x_srate_srch_fine(state);
				if (srate_coarse != 0) {
					stv090x_get_lock_tmg(state);
					lock = stv090x_get_dmdlock(state, timeout_dmd);
				} else {
					lock = 0;
				}
			} else {
				cpt_fail = 0;
				agc2_ovflw = 0;
				for (i = 0; i < 10; i++) {
					agc2  = STV090x_READ_DEMOD(state, AGC2I1) << 8;
					agc2 |= STV090x_READ_DEMOD(state, AGC2I0);
					if (agc2 >= 0xff00)
						agc2_ovflw++;
					reg = STV090x_READ_DEMOD(state, DSTATUS2);
					if ((STV090x_GETFIELD_Px(reg, CFR_OVERFLOW_FIELD) == 0x01) &&
					    (STV090x_GETFIELD_Px(reg, DEMOD_DELOCK_FIELD) == 0x01))

						cpt_fail++;
				}
				if ((cpt_fail > 7) || (agc2_ovflw > 7))
					coarse_fail = 1;

				lock = 0;
			}
			k_ref -= 30;
		} while ((k_ref >= k_min) && (!lock) && (!coarse_fail));
	}

	dprintk(10, "%s <\n", __func__);
	return lock;

err:
	printk("stv090x_blind_search:I/O error\n");
	return -1;
}

static int stv090x_chk_tmg(struct stv090x_state *state)
{
	u32 reg;
	s32 tmg_cpt = 0, i;
	u8 freq, tmg_thh, tmg_thl;
	int tmg_lock = 0;

	dprintk(10, "%s >\n", __func__);

	freq = STV090x_READ_DEMOD(state, CARFREQ);
	tmg_thh = STV090x_READ_DEMOD(state, TMGTHRISE);
	tmg_thl = STV090x_READ_DEMOD(state, TMGTHFALL);
	if (STV090x_WRITE_DEMOD(state, TMGTHRISE, 0x20) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, TMGTHFALL, 0x00) < 0)
		goto err;

	reg = STV090x_READ_DEMOD(state, DMDCFGMD);
	STV090x_SETFIELD_Px(reg, CFR_AUTOSCAN_FIELD, 0x00); /* stop carrier offset search */
	if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, RTC, 0x80) < 0)
		goto err;

	if (STV090x_WRITE_DEMOD(state, RTCS2, 0x40) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x00) < 0)
		goto err;

	if (STV090x_WRITE_DEMOD(state, CFRINIT1, 0x00) < 0) /* set car ofset to 0 */
		goto err;
	if (STV090x_WRITE_DEMOD(state, CFRINIT0, 0x00) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, AGC2REF, 0x65) < 0)
		goto err;

	if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x18) < 0) /* trigger acquisition */
		goto err;
	msleep(10);

	for (i = 0; i < 10; i++) {
		reg = STV090x_READ_DEMOD(state, DSTATUS);
		if (STV090x_GETFIELD_Px(reg, TMGLOCK_QUALITY_FIELD) >= 2)
			tmg_cpt++;
		msleep(1);
	}
	if (tmg_cpt >= 3)
		tmg_lock = 1;

	if (STV090x_WRITE_DEMOD(state, AGC2REF, 0x38) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, RTC, 0x88) < 0) /* DVB-S1 timing */
		goto err;
	if (STV090x_WRITE_DEMOD(state, RTCS2, 0x68) < 0) /* DVB-S2 timing */
		goto err;

	if (STV090x_WRITE_DEMOD(state, CARFREQ, freq) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, TMGTHRISE, tmg_thh) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, TMGTHFALL, tmg_thl) < 0)
		goto err;

	dprintk(10, "%s <\n", __func__);
	return	tmg_lock;

err:
	printk("stv090x_chk_tmg: I/O error\n");
	return -1;
}

static int stv090x_get_coldlock(struct stv090x_state *state, s32 timeout_dmd)
{
	struct dvb_frontend *fe = &state->frontend;

	u32 reg;
	s32 car_step, steps, cur_step, dir, freq, timeout_lock;
	int lock = 0;

	dprintk(10, "%s >\n", __func__);

	if (state->srate >= 10000000)
		timeout_lock = timeout_dmd / 3;
	else
		timeout_lock = timeout_dmd / 2;

	lock = stv090x_get_dmdlock(state, timeout_lock); /* cold start wait */
	if (!lock) {
		if (state->srate >= 10000000) {
			if (stv090x_chk_tmg(state)) {
				if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1f) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x15) < 0)
					goto err;
				lock = stv090x_get_dmdlock(state, timeout_dmd);
			} else {
				lock = 0;
			}
		} else {
			if (state->srate <= 4000000)
				car_step = 1000;
			else if (state->srate <= 7000000)
				car_step = 2000;
			else if (state->srate <= 10000000)
				car_step = 3000;
			else
				car_step = 5000;

			steps  = (state->search_range / 1000) / car_step;
			steps /= 2;
			steps  = 2 * (steps + 1);
			if (steps < 0)
				steps = 2;
			else if (steps > 12)
				steps = 12;

			cur_step = 1;
			dir = 1;

			if (!lock) {
				freq = state->frequency;
				state->tuner_bw = stv090x_car_width(state->srate, state->rolloff) + state->srate;
				while ((cur_step <= steps) && (!lock)) {
					if (dir > 0)
						freq += cur_step * car_step;
					else
						freq -= cur_step * car_step;

					/* Setup tuner */
					if (stv090x_i2c_gate_ctrl(fe, 1) < 0)
						goto err;

					if (state->config->tuner_set_frequency) {
						if (state->config->tuner_set_frequency(fe, state->frequency) < 0)
							goto err;
					}

					if (state->config->tuner_set_bandwidth) {
						if (state->config->tuner_set_bandwidth(fe, state->tuner_bw) < 0)
							goto err;
					}

					if (stv090x_i2c_gate_ctrl(fe, 0) < 0)
						goto err;

					msleep(50);

					if (stv090x_i2c_gate_ctrl(fe, 1) < 0)
						goto err;

					if (state->config->tuner_get_status) {
						if (state->config->tuner_get_status(fe, &reg) < 0)
							goto err;
					}

					if (reg)
						dprintk(10, "3. Tuner phase locked\n");
					else
						dprintk(10, "3. Tuner unlocked\n");

					if (stv090x_i2c_gate_ctrl(fe, 0) < 0)
						goto err;

					STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1c);
					if (state->delsys == STV090x_DVBS2) {
						reg = STV090x_READ_DEMOD(state, DMDCFGMD);
						STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 0);
						STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 0);
						if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
							goto err;
						STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 1);
						STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 1);
						if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
							goto err;
					}
					if (STV090x_WRITE_DEMOD(state, CFRINIT1, 0x00) < 0)
						goto err;
					if (STV090x_WRITE_DEMOD(state, CFRINIT0, 0x00) < 0)
						goto err;
					if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1f) < 0)
						goto err;
					if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x15) < 0)
						goto err;
					lock = stv090x_get_dmdlock(state, (timeout_dmd / 3));

					dir *= -1;
					cur_step++;
				}
			}
		}
	}

	dprintk(10, "%s <\n", __func__);
	return lock;

err:
	printk("stv090x_get_coldlock: I/O error\n");
	return -1;
}

static int stv090x_get_loop_params(struct stv090x_state *state, s32 *freq_inc, s32 *timeout_sw, s32 *steps)
{
	s32 timeout, inc, steps_max, srate, car_max;

	dprintk(10, "%s >\n", __func__);
	srate = state->srate;
	car_max = state->search_range / 1000;
	car_max += car_max / 10;
	car_max  = 65536 * (car_max / 2);
	car_max /= (state->mclk / 1000);

	if (car_max > 0x4000)
		car_max = 0x4000 ; /* maxcarrier should be<= +-1/4 Mclk */

	inc  = srate;
	inc /= state->mclk / 1000;
	inc *= 256;
	inc *= 256;
	inc /= 1000;

	switch (state->search_mode) {
	case STV090x_SEARCH_DVBS1:
	case STV090x_SEARCH_DSS:
		inc *= 3; /* freq step = 3% of srate */
		timeout = 20;
		break;

	case STV090x_SEARCH_DVBS2:
		inc *= 4;
		timeout = 25;
		break;

	case STV090x_SEARCH_AUTO:
	default:
		inc *= 3;
		timeout = 25;
		break;
	}
	inc /= 100;
	if ((inc > car_max) || (inc < 0))
		inc = car_max / 2; /* increment <= 1/8 Mclk */

	timeout *= 27500; /* 27.5 Msps reference */
	if (srate > 0)
		timeout /= (srate / 1000);

	if ((timeout > 100) || (timeout < 0))
		timeout = 100;

	steps_max = (car_max / inc) + 1; /* min steps = 3 */
	if ((steps_max > 100) || (steps_max < 0)) {
		steps_max = 100; /* max steps <= 100 */
		inc = car_max / steps_max;
	}
	*freq_inc = inc;
	*timeout_sw = timeout;
	*steps = steps_max;

	dprintk(10, "%s <\n", __func__);
	return 0;
}

static int stv090x_chk_signal(struct stv090x_state *state)
{
	s32 offst_car, agc2, car_max;
	int no_signal;

	dprintk(10, "%s >\n", __func__);
	offst_car  = STV090x_READ_DEMOD(state, CFR2) << 8;
	offst_car |= STV090x_READ_DEMOD(state, CFR1);
	offst_car = comp2(offst_car, 16);

	agc2  = STV090x_READ_DEMOD(state, AGC2I1) << 8;
	agc2 |= STV090x_READ_DEMOD(state, AGC2I0);
	car_max = state->search_range / 1000;

	car_max += (car_max / 10); /* 10% margin */
	car_max  = (65536 * car_max / 2);
	car_max /= state->mclk / 1000;

	if (car_max > 0x4000)
		car_max = 0x4000;

	if ((agc2 > 0x2000) || (offst_car > 2 * car_max) || (offst_car < -2 * car_max)) {
		no_signal = 1;
		dprintk(1, "No Signal\n");
	} else {
		no_signal = 0;
		dprintk(1, "Found Signal\n");
	}

	dprintk(10, "%s no_signal %d>\n", __func__, no_signal);
	return no_signal;
}

static int stv090x_search_car_loop(struct stv090x_state *state, s32 inc, s32 timeout, int zigzag, s32 steps_max)
{
	int no_signal, lock = 0;
	s32 cpt_step = 0, offst_freq, car_max;
	u32 reg;

	dprintk(10, "%s timeout inc %d, %d, zigzag %d, setps_max %d>\n", __func__, inc, timeout, zigzag, steps_max);

	car_max  = state->search_range / 1000;
	car_max += (car_max / 10);
	car_max  = (65536 * car_max / 2);
	car_max /= (state->mclk / 1000);
	if (car_max > 0x4000)
		car_max = 0x4000;

	if (zigzag)
		offst_freq = 0;
	else
		offst_freq = -car_max + inc;

	do {
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1c) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRINIT1, ((offst_freq / 256) & 0xff)) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRINIT0, offst_freq & 0xff) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x18) < 0)
			goto err;

		reg = STV090x_READ_DEMOD(state, PDELCTRL1);
		STV090x_SETFIELD_Px(reg, ALGOSWRST_FIELD, 0x1); /* stop DVB-S2 packet delin */
		if (STV090x_WRITE_DEMOD(state, PDELCTRL1, reg) < 0)
			goto err;

		if (zigzag) {
			if (offst_freq >= 0)
				offst_freq = -offst_freq - 2 * inc;
			else
				offst_freq = -offst_freq;
		} else {
			offst_freq += 2 * inc;
		}

		cpt_step++;

		lock = stv090x_get_dmdlock(state, timeout);
		no_signal = stv090x_chk_signal(state);


		dprintk(100, "%s: no_signal  = %d\n", __func__, no_signal);
		dprintk(100, "%s: lock       = %d\n", __func__, lock);
		dprintk(100, "%s: offst_freq = %d\n", __func__, offst_freq);
		dprintk(100, "%s: inc        = %d\n", __func__, inc);
		dprintk(100, "%s: car_max    = %d\n", __func__, car_max);
		dprintk(100, "%s: cpt_step   = %d\n", __func__, cpt_step);
		dprintk(100, "%s: steps_max  = %d\n", __func__, steps_max);

	} while ((!lock) &&
		 (!no_signal) &&
		  ((offst_freq - inc) < car_max) &&
		  ((offst_freq + inc) > -car_max) &&
		  (cpt_step < steps_max));

	reg = STV090x_READ_DEMOD(state, PDELCTRL1);
	STV090x_SETFIELD_Px(reg, ALGOSWRST_FIELD, 0);
	if (STV090x_WRITE_DEMOD(state, PDELCTRL1, reg) < 0)
			goto err;

	dprintk(10, "%s <\n", __func__);

	return lock;
err:
	printk("stv090x_search_car_loop: I/O error\n");
	return -1;
}

static int stv090x_sw_algo(struct stv090x_state *state)
{
	int no_signal, zigzag, lock = 0;
	u32 reg;

	s32 dvbs2_fly_wheel;
	s32 inc, timeout_step, trials, steps_max;

	dprintk(10, "%s >\n", __func__);

	/* get params */
	stv090x_get_loop_params(state, &inc, &timeout_step, &steps_max);

	switch (state->search_mode) {
	case STV090x_SEARCH_DVBS1:
	case STV090x_SEARCH_DSS:
		/* accelerate the frequency detector */
		if (state->dev_ver >= 0x20) {
			if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x3B) < 0)
				goto err;
		}

		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, 0x49) < 0)
			goto err;
		zigzag = 0;
		break;

	case STV090x_SEARCH_DVBS2:
		if (state->dev_ver >= 0x20) {
			if (STV090x_WRITE_DEMOD(state, CORRELABS, 0x79) < 0)
				goto err;
		}

		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, 0x89) < 0)
			goto err;
		zigzag = 1;
		break;

	case STV090x_SEARCH_AUTO:
	default:
		/* accelerate the frequency detector */
		if (state->dev_ver >= 0x20) {
			if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x3b) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, CORRELABS, 0x79) < 0)
				goto err;
		}

		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, 0xc9) < 0)
			goto err;
		zigzag = 0;
		break;
	}

	trials = 0;
	do {
		lock = stv090x_search_car_loop(state, inc, timeout_step, zigzag, steps_max);
		no_signal = stv090x_chk_signal(state);
		trials++;

		/*run the SW search 2 times maximum*/
		if (lock || no_signal || (trials == 2)) {
			/*Check if the demod is not losing lock in DVBS2*/
			if (state->dev_ver >= 0x20) {
				if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x49) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, CORRELABS, 0x9e) < 0)
					goto err;
			}

			reg = STV090x_READ_DEMOD(state, DMDSTATE);
			if ((lock) && (STV090x_GETFIELD_Px(reg, HEADER_MODE_FIELD) == STV090x_DVBS2)) {
				/*Check if the demod is not losing lock in DVBS2*/
				msleep(timeout_step);
				reg = STV090x_READ_DEMOD(state, DMDFLYW);
				dvbs2_fly_wheel = STV090x_GETFIELD_Px(reg, FLYWHEEL_CPT_FIELD);
				if (dvbs2_fly_wheel < 0xd) {	 /*if correct frames is decrementing */
					msleep(timeout_step);
					reg = STV090x_READ_DEMOD(state, DMDFLYW);
					dvbs2_fly_wheel = STV090x_GETFIELD_Px(reg, FLYWHEEL_CPT_FIELD);
				}
				if (dvbs2_fly_wheel < 0xd) {
					/*FALSE lock, The demod is loosing lock */
					lock = 0;
					if (trials < 2) {
						if (state->dev_ver >= 0x20) {
							if (STV090x_WRITE_DEMOD(state, CORRELABS, 0x79) < 0)
								goto err;
						}

						if (STV090x_WRITE_DEMOD(state, DMDCFGMD, 0x89) < 0)
							goto err;
					}
				}
			}
		}

		dprintk(100, "%s: no_signal  = %d\n", __func__, no_signal);
		dprintk(100, "%s: lock       = %d\n", __func__, lock);
		dprintk(100, "%s: trials     = %d\n", __func__, trials);
	} while ((!lock) && (trials < 2) && (!no_signal));

	dprintk(10, "%s lock %d<\n", __func__, lock);
	return lock;
err:
	printk("stv090x_sw_algo: I/O error\n");
	return -1;
}

static enum stv090x_delsys stv090x_get_std(struct stv090x_state *state)
{
	u32 reg;
	enum stv090x_delsys delsys;

	dprintk(10, "%s >\n", __func__);

	reg = STV090x_READ_DEMOD(state, DMDSTATE);
	if (STV090x_GETFIELD_Px(reg, HEADER_MODE_FIELD) == 2)
		delsys = STV090x_DVBS2;
	else if (STV090x_GETFIELD_Px(reg, HEADER_MODE_FIELD) == 3) {
		reg = STV090x_READ_DEMOD(state, FECM);
		if (STV090x_GETFIELD_Px(reg, DSS_DVB_FIELD) == 1)
			delsys = STV090x_DSS;
		else
			delsys = STV090x_DVBS1;
	} else {
		delsys = STV090x_ERROR;
	}

	dprintk(10, "%s delsys %d <\n", __func__, delsys);
	return delsys;
}

/* in Hz */
static s32 stv090x_get_car_freq(struct stv090x_state *state, u32 mclk)
{
	s32 derot, int_1, int_2, tmp_1, tmp_2;

	dprintk(10, "%s >\n", __func__);

	derot  = STV090x_READ_DEMOD(state, CFR2) << 16;
	derot |= STV090x_READ_DEMOD(state, CFR1) <<  8;
	derot |= STV090x_READ_DEMOD(state, CFR0);

	derot = comp2(derot, 24);
	int_1 = state->mclk >> 12;
	int_2 = derot >> 12;

	/* carrier_frequency = MasterClock * Reg / 2^24 */
	tmp_1 = state->mclk % 0x1000;
	tmp_2 = derot % 0x1000;

	derot = (int_1 * int_2) +
		((int_1 * tmp_2) >> 12) +
		((int_1 * tmp_1) >> 12);

	dprintk(10, "%s derot %d <\n", __func__, derot);
	return derot;
}

static int stv090x_get_viterbi(struct stv090x_state *state)
{
	u32 reg, rate;

	dprintk(10, "%s >\n", __func__);

	reg = STV090x_READ_DEMOD(state, VITCURPUN);
	rate = STV090x_GETFIELD_Px(reg, VIT_CURPUN_FIELD);

	switch (rate) {
	case 13:
		state->fec = STV090x_PR12;
		break;

	case 18:
		state->fec = STV090x_PR23;
		break;

	case 21:
		state->fec = STV090x_PR34;
		break;

	case 24:
		state->fec = STV090x_PR56;
		break;

	case 25:
		state->fec = STV090x_PR67;
		break;

	case 26:
		state->fec = STV090x_PR78;
		break;

	default:
		state->fec = STV090x_PRERR;
		break;
	}

	dprintk(10, "%s <\n", __func__);
	return 0;
}

static enum stv090x_signal_state stv090x_get_sig_params(struct stv090x_state *state)
{
	struct dvb_frontend *fe = &state->frontend;

	u8 tmg;
	u32 reg;
	s32 i = 0, offst_freq;

	dprintk(10, "%s: >\n", __func__);

	msleep(5);

	if (state->algo == STV090x_BLIND_SEARCH) {
		tmg = STV090x_READ_DEMOD(state, TMGREG2);
		STV090x_WRITE_DEMOD(state, SFRSTEP, 0x5c);
		while ((i <= 50) && (tmg != 0) && (tmg != 0xff)) {
			tmg = STV090x_READ_DEMOD(state, TMGREG2);
			msleep(5);
			i += 5;
		}
	}
	state->delsys = stv090x_get_std(state);

	dprintk(50, "delsys from hw = %d\n", state->delsys);

	if (stv090x_i2c_gate_ctrl(fe, 1) < 0)
		goto err;

	if (state->config->tuner_get_frequency) {
		if (state->config->tuner_get_frequency(fe, &state->frequency) < 0)
			goto err;
	}

	if (stv090x_i2c_gate_ctrl(fe, 0) < 0)
		goto err;

	offst_freq = stv090x_get_car_freq(state, state->mclk) / 1000;
	state->frequency += offst_freq;

	if (stv090x_get_viterbi(state) < 0)
		goto err;

	reg = STV090x_READ_DEMOD(state, DMDMODCOD);
	state->modcod = STV090x_GETFIELD_Px(reg, DEMOD_MODCOD_FIELD);

	dprintk(50, "%s: modcod %d\n", __func__, state->modcod);

	state->pilots = STV090x_GETFIELD_Px(reg, DEMOD_TYPE_FIELD) & 0x01;

	dprintk(50, "%s: pilots %d\n", __func__, state->pilots);

	state->frame_len = STV090x_GETFIELD_Px(reg, DEMOD_TYPE_FIELD) >> 1;

	dprintk(50, "%s: frame_len %d\n", __func__, state->frame_len);

	reg = STV090x_READ_DEMOD(state, TMGOBS);
	state->rolloff = STV090x_GETFIELD_Px(reg, ROLLOFF_STATUS_FIELD);

	dprintk(50, "%s: rolloff %d\n", __func__, state->rolloff);

	if (state->device != STX7111)
	{
	   reg = STV090x_READ_DEMOD(state, FECM);
     	   state->inversion = STV090x_GETFIELD_Px(reg, IQINV_FIELD);
        }
	else
	{
	    state->inversion = STV090x_IQ_AUTO;
	}
	dprintk(50, "%s: inversion %d\n", __func__, state->inversion);

	if ((state->algo == STV090x_BLIND_SEARCH) || (state->srate < 10000000)) {
		int car_width;
		dprintk(100, "%s: 1.\n", __func__);

		if (stv090x_i2c_gate_ctrl(fe, 1) < 0)
			goto err;

		if (state->config->tuner_get_frequency) {
			if (state->config->tuner_get_frequency(fe, &state->frequency) < 0)
				goto err;
		}

		if (stv090x_i2c_gate_ctrl(fe, 0) < 0)
			goto err;

		if (abs(offst_freq) <= ((state->search_range / 2000) + 500))
		{
			dprintk(100, "%s: rangeok1\n", __func__);
			return STV090x_RANGEOK;
		}
		else if (abs(offst_freq) <= (car_width = stv090x_car_width(state->srate, state->rolloff) / 2000))
		{
			dprintk(100, "%s: rangeok2\n", __func__);
			return STV090x_RANGEOK;
		}
		else
		{
			dprintk(10, "%s: out of range %ld > %d\n", __func__, abs(offst_freq), car_width);
			return STV090x_OUTOFRANGE; /* Out of Range */
		}
	} else {
		dprintk(100, "%s: 2.\n", __func__);

		if (abs(offst_freq) <= ((state->search_range / 2000) + 500))
		{
			dprintk(100, "%s: rangeok\n", __func__);
			return STV090x_RANGEOK;
		}
		else
		{
			dprintk(100, "%s: out of range %ld > %d\n", __func__, abs(offst_freq), (state->search_range / 2000) + 500);

			return STV090x_OUTOFRANGE;
		}
	}

	dprintk(10, "%s: out of range <\n", __func__);
	return STV090x_OUTOFRANGE;
err:
	printk("stv090x_signal_state stv090x_get_sig_params: I/O error\n");
	return -1;
}

static u32 stv090x_get_tmgoffst(struct stv090x_state *state, u32 srate)
{
	s32 offst_tmg;

	dprintk(10, "%s >\n", __func__);
	offst_tmg  = STV090x_READ_DEMOD(state, TMGREG2) << 16;
	offst_tmg |= STV090x_READ_DEMOD(state, TMGREG1) <<  8;
	offst_tmg |= STV090x_READ_DEMOD(state, TMGREG0);

	offst_tmg = comp2(offst_tmg, 24); /* 2's complement */
	if (!offst_tmg)
		offst_tmg = 1;

	offst_tmg  = ((s32) srate * 10) / ((s32) 0x1000000 / offst_tmg);
	offst_tmg /= 320;

	dprintk(10, "%s <\n", __func__);
	return offst_tmg;
}

static u8 stv090x_optimize_carloop(struct stv090x_state *state, enum stv090x_modcod modcod, s32 pilots)
{
	u8 aclc = 0x29;
	s32 i;
	struct stv090x_long_frame_crloop *car_loop, *car_loop_qpsk_low, *car_loop_apsk_low;

	dprintk(10, "%s >\n", __func__);

/* fixme: we should warn in other cut cases if device is stx7111 because values
 * are unknown
 */
	if ((state->dev_ver == 0x20) && (state->device == STX7111))
	{
                printk("%s STX7111 cut 0x20 handling (modcod %d, pilots %d)\n", __func__, modcod, pilots);
		car_loop		= stx7111_s2_crl_cut20;
#warning stv090x: fixme fixme dont know lowqpsk crl
		car_loop_qpsk_low	= stv090x_s2_lowqpsk_crl_cut20;
/* fixme: dont know apsk values */
		car_loop_apsk_low	= stv090x_s2_apsk_crl_cut20;
	}
	else
	if (state->dev_ver == 0x20) {
		car_loop		= stv090x_s2_crl_cut20;
		car_loop_qpsk_low	= stv090x_s2_lowqpsk_crl_cut20;
		car_loop_apsk_low	= stv090x_s2_apsk_crl_cut20;
	} else {
		/* >= Cut 3 */
		car_loop		= stv090x_s2_crl_cut30;
		car_loop_qpsk_low	= stv090x_s2_lowqpsk_crl_cut30;
		car_loop_apsk_low	= stv090x_s2_apsk_crl_cut30;
	}

	if (modcod < STV090x_QPSK_12) {
		i = 0;
		while ((i < 3) && (modcod != car_loop_qpsk_low[i].modcod))
			i++;

		if (i >= 3)
			i = 2;

	} else {
		i = 0;
		while ((i < 14) && (modcod != car_loop[i].modcod))
			i++;

		if (i >= 14) {
			i = 0;
			while ((i < 11) && (modcod != car_loop_apsk_low[i].modcod))
				i++;

			if (i >= 11)
				i = 10;
		}
	}

	if (modcod <= STV090x_QPSK_25) {
		if (pilots) {
			if (state->srate <= 3000000)
				aclc = car_loop_qpsk_low[i].crl_pilots_on_2;
			else if (state->srate <= 7000000)
				aclc = car_loop_qpsk_low[i].crl_pilots_on_5;
			else if (state->srate <= 15000000)
				aclc = car_loop_qpsk_low[i].crl_pilots_on_10;
			else if (state->srate <= 25000000)
				aclc = car_loop_qpsk_low[i].crl_pilots_on_20;
			else
				aclc = car_loop_qpsk_low[i].crl_pilots_on_30;
		} else {
			if (state->srate <= 3000000)
				aclc = car_loop_qpsk_low[i].crl_pilots_off_2;
			else if (state->srate <= 7000000)
				aclc = car_loop_qpsk_low[i].crl_pilots_off_5;
			else if (state->srate <= 15000000)
				aclc = car_loop_qpsk_low[i].crl_pilots_off_10;
			else if (state->srate <= 25000000)
				aclc = car_loop_qpsk_low[i].crl_pilots_off_20;
			else
				aclc = car_loop_qpsk_low[i].crl_pilots_off_30;
		}

	} else if (modcod <= STV090x_8PSK_910) {
		if (pilots) {
			if (state->srate <= 3000000)
				aclc = car_loop[i].crl_pilots_on_2;
			else if (state->srate <= 7000000)
				aclc = car_loop[i].crl_pilots_on_5;
			else if (state->srate <= 15000000)
				aclc = car_loop[i].crl_pilots_on_10;
			else if (state->srate <= 25000000)
				aclc = car_loop[i].crl_pilots_on_20;
			else
				aclc = car_loop[i].crl_pilots_on_30;
		} else {
			if (state->srate <= 3000000)
				aclc = car_loop[i].crl_pilots_off_2;
			else if (state->srate <= 7000000)
				aclc = car_loop[i].crl_pilots_off_5;
			else if (state->srate <= 15000000)
				aclc = car_loop[i].crl_pilots_off_10;
			else if (state->srate <= 25000000)
				aclc = car_loop[i].crl_pilots_off_20;
			else
				aclc = car_loop[i].crl_pilots_off_30;
		}
	} else { /* 16APSK and 32APSK */
		if (state->srate <= 3000000)
			aclc = car_loop_apsk_low[i].crl_pilots_on_2;
		else if (state->srate <= 7000000)
			aclc = car_loop_apsk_low[i].crl_pilots_on_5;
		else if (state->srate <= 15000000)
			aclc = car_loop_apsk_low[i].crl_pilots_on_10;
		else if (state->srate <= 25000000)
			aclc = car_loop_apsk_low[i].crl_pilots_on_20;
		else
			aclc = car_loop_apsk_low[i].crl_pilots_on_30;
	}

	dprintk(10, "%s <\n", __func__);
	return aclc;
}

static u8 stv090x_optimize_carloop_short(struct stv090x_state *state)
{
	struct stv090x_short_frame_crloop *short_crl = NULL;
	s32 index = 0;
	u8 aclc = 0x0b;

	dprintk(10, "%s >\n", __func__);
	switch (state->modulation) {
	case STV090x_QPSK:
	default:
		index = 0;
		break;
	case STV090x_8PSK:
		index = 1;
		break;
	case STV090x_16APSK:
		index = 2;
		break;
	case STV090x_32APSK:
		index = 3;
		break;
	}

	if (state->dev_ver >= 0x30) {
		/* Cut 3.0 and up */
		short_crl = stv090x_s2_short_crl_cut30;
	} else {
		/* Cut 2.0 and up: we don't support cuts older than 2.0 */
		short_crl = stv090x_s2_short_crl_cut20;
	}

	if (state->srate <= 3000000)
		aclc = short_crl[index].crl_2;
	else if (state->srate <= 7000000)
		aclc = short_crl[index].crl_5;
	else if (state->srate <= 15000000)
		aclc = short_crl[index].crl_10;
	else if (state->srate <= 25000000)
		aclc = short_crl[index].crl_20;
	else
		aclc = short_crl[index].crl_30;

	dprintk(10, "%s <\n", __func__);
	return aclc;
}

static int stv090x_optimize_track(struct stv090x_state *state)
{
	struct dvb_frontend *fe = &state->frontend;

	enum stv090x_rolloff rolloff;
	enum stv090x_modcod modcod;

	s32 srate, pilots, aclc, f_1, f_0, i = 0, blind_tune = 0;
	u32 reg;

	dprintk(10, "%s >\n", __func__);

	srate  = stv090x_get_srate(state, state->mclk);
	srate += stv090x_get_tmgoffst(state, srate);

	switch (state->delsys) {
	case STV090x_DVBS1:
	case STV090x_DSS:

		dprintk(50, "STV090x_DVBS1\n");
		if (state->algo == (enum stv090x_algo)STV090x_SEARCH_AUTO) {
			reg = STV090x_READ_DEMOD(state, DMDCFGMD);
			STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 1);
			STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 0);
			if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
				goto err;
		}
		reg = STV090x_READ_DEMOD(state, DEMOD);
		STV090x_SETFIELD_Px(reg, ROLLOFF_CONTROL_FIELD, state->rolloff);
		STV090x_SETFIELD_Px(reg, MANUAL_SXROLLOFF_FIELD, 0x01);
		if (STV090x_WRITE_DEMOD(state, DEMOD, reg) < 0)
			goto err;

		if (state->dev_ver >= 0x30) {
			if (stv090x_get_viterbi(state) < 0)
				goto err;

			if (state->fec == STV090x_PR12) {
				if (STV090x_WRITE_DEMOD(state, GAUSSR0, 0x98) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, CCIR0, 0x18) < 0)
					goto err;
			} else {
				if (STV090x_WRITE_DEMOD(state, GAUSSR0, 0x18) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, CCIR0, 0x18) < 0)
					goto err;
			}
		}

		if (STV090x_WRITE_DEMOD(state, ERRCTRL1, 0x75) < 0)
			goto err;
		break;

	case STV090x_DVBS2:
        dprintk(50, "STV090x_DVBS2\n");

		reg = STV090x_READ_DEMOD(state, DMDCFGMD);
		STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 0);
		STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 1);
		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;

        if (STV090x_WRITE_DEMOD(state, ACLC, 0) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, BCLC, 0) < 0)
			goto err;

		if (state->frame_len == STV090x_LONG_FRAME) {
			reg = STV090x_READ_DEMOD(state, DMDMODCOD);
			modcod = STV090x_GETFIELD_Px(reg, DEMOD_MODCOD_FIELD);
			pilots = STV090x_GETFIELD_Px(reg, DEMOD_TYPE_FIELD) & 0x01;
			aclc = stv090x_optimize_carloop(state, modcod, pilots);
			if (modcod <= STV090x_QPSK_910) {
				STV090x_WRITE_DEMOD(state, ACLC2S2Q, aclc);
			} else if (modcod <= STV090x_8PSK_910) {
				if (STV090x_WRITE_DEMOD(state, ACLC2S2Q, 0x2a) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, ACLC2S28, aclc) < 0)
					goto err;
			}
			if ((state->demod_mode == STV090x_SINGLE) && (modcod > STV090x_8PSK_910)) {
				if (modcod <= STV090x_16APSK_910) {
					if (STV090x_WRITE_DEMOD(state, ACLC2S2Q, 0x2a) < 0)
						goto err;
					if (STV090x_WRITE_DEMOD(state, ACLC2S216A, aclc) < 0)
						goto err;
				} else {
					if (STV090x_WRITE_DEMOD(state, ACLC2S2Q, 0x2a) < 0)
						goto err;
					if (STV090x_WRITE_DEMOD(state, ACLC2S232A, aclc) < 0)
						goto err;
				}
			}
		} else {
			/*Carrier loop setting for short frame*/
			aclc = stv090x_optimize_carloop_short(state);
			if (state->modulation == STV090x_QPSK) {
				if (STV090x_WRITE_DEMOD(state, ACLC2S2Q, aclc) < 0)
					goto err;
			} else if (state->modulation == STV090x_8PSK) {
				if (STV090x_WRITE_DEMOD(state, ACLC2S2Q, 0x2a) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, ACLC2S28, aclc) < 0)
					goto err;
			} else if (state->modulation == STV090x_16APSK) {
				if (STV090x_WRITE_DEMOD(state, ACLC2S2Q, 0x2a) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, ACLC2S216A, aclc) < 0)
					goto err;
			} else if (state->modulation == STV090x_32APSK)  {
				if (STV090x_WRITE_DEMOD(state, ACLC2S2Q, 0x2a) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, ACLC2S232A, aclc) < 0)
					goto err;
			}
		}

		if (state->device == STX7111)
		{
		   if (stv090x_write_reg(state, STV090x_GENCFG, 0x16) < 0)
			goto err;

		   reg = stv090x_read_reg(state, STV090x_TSTRES0);
		   STV090x_SETFIELD(reg, FRESFEC_FIELD, 0x01); /* ldpc reset */
		   if (stv090x_write_reg(state, STV090x_TSTRES0, reg) < 0)
			   goto err;

		   STV090x_SETFIELD(reg, FRESFEC_FIELD, 0);
		   if (stv090x_write_reg(state, STV090x_TSTRES0, reg) < 0)
			goto err;

		   STV090x_WRITE_DEMOD(state, ERRCTRL1, 0x63); /* PER */

		   reg = STV090x_READ_DEMOD(state, DEMOD);
		   STV090x_SETFIELD_Px(reg, MANUAL_SXROLLOFF_FIELD, 1); /* auto rolloff */
		   if (STV090x_WRITE_DEMOD(state, DEMOD, reg) < 0)
			goto err;

   		   STV090x_WRITE_DEMOD(state, GAUSSR0, 0xac);
   		   STV090x_WRITE_DEMOD(state, CCIR0, 0x2c);
		} else
   		   STV090x_WRITE_DEMOD(state, ERRCTRL1, 0x67); /* PER */

		break;

	//case STV090x_UNKNOWN:
	default:
        dprintk(50, "STV090x_UNKNOWN\n");
		reg = STV090x_READ_DEMOD(state, DMDCFGMD);
		STV090x_SETFIELD_Px(reg, DVBS1_ENABLE_FIELD, 1);
		STV090x_SETFIELD_Px(reg, DVBS2_ENABLE_FIELD, 1);
		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;
		break;
	}

	if (STV090x_WRITE_DEMOD(state, ERRCTRL1, 0x75) < 0) /* PER */
		goto err;

	f_1 = STV090x_READ_DEMOD(state, CFR2);
	f_0 = STV090x_READ_DEMOD(state, CFR1);
	reg = STV090x_READ_DEMOD(state, TMGOBS);
	rolloff = STV090x_GETFIELD_Px(reg, ROLLOFF_STATUS_FIELD);

	if (state->algo == STV090x_BLIND_SEARCH) {
		STV090x_WRITE_DEMOD(state, SFRSTEP, 0x00);
		reg = STV090x_READ_DEMOD(state, DMDCFGMD);
		STV090x_SETFIELD_Px(reg, SCAN_ENABLE_FIELD, 0x00);
		STV090x_SETFIELD_Px(reg, CFR_AUTOSCAN_FIELD, 0x00);
		if (STV090x_WRITE_DEMOD(state, DMDCFGMD, reg) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, TMGCFG2, 0xc1) < 0)
			goto err;

		if (stv090x_set_srate(state, srate) < 0)
			goto err;
#if 0
		if (stv090x_set_max_srate(state, state->mclk, srate) < 0)
			goto err;
		if (stv090x_set_min_srate(state, state->mclk, srate) < 0)
			goto err;
#endif
		blind_tune = 1;
	}

	if (state->dev_ver >= 0x20) {
		if ((state->search_mode == STV090x_SEARCH_DVBS1)	||
		    (state->search_mode == STV090x_SEARCH_DSS)		||
		    (state->search_mode == STV090x_SEARCH_AUTO)) {

			if (STV090x_WRITE_DEMOD(state, VAVSRVIT, 0x0a) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, VITSCALE, 0x00) < 0)
				goto err;
		}
	}

	if (STV090x_WRITE_DEMOD(state, AGC2REF, 0x38) < 0)
		goto err;

	/* AUTO tracking MODE */
	if (STV090x_WRITE_DEMOD(state, SFRUP1, 0x80) < 0)
		goto err;
	/* AUTO tracking MODE */
	if (STV090x_WRITE_DEMOD(state, SFRLOW1, 0x80) < 0)
		goto err;

	if ((state->dev_ver >= 0x20) || (blind_tune == 1) || (state->srate < 10000000)) {
		/* update initial carrier freq with the found freq offset */

        dprintk(1, "f_1 0x%x\n", f_1);
        dprintk(1, "f_0 0x%x\n", f_0);

		if (STV090x_WRITE_DEMOD(state, CFRINIT1, f_1) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, CFRINIT0, f_0) < 0)
			goto err;
		state->tuner_bw = stv090x_car_width(srate, state->rolloff) + 10000000;

		if ((state->dev_ver >= 0x20) || (blind_tune == 1)) {

			if (state->algo != STV090x_WARM_SEARCH) {

				if (state->config->tuner_set_bandwidth) {
					if (state->config->tuner_set_bandwidth(fe, state->tuner_bw) < 0)
						goto err;
				}
			}
		}
		if ((state->algo == STV090x_BLIND_SEARCH) || (state->srate < 10000000))
			msleep(50); /* blind search: wait 50ms for SR stabilization */
		else
			msleep(5);

		stv090x_get_lock_tmg(state);

		if (!(stv090x_get_dmdlock(state, (state->DemodTimeout / 2)))) {
			if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1f) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, CFRINIT1, f_1) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, CFRINIT0, f_0) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x18) < 0)
				goto err;

			i = 0;

			while ((!(stv090x_get_dmdlock(state, (state->DemodTimeout / 2)))) && (i <= 2)) {

				if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x1f) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, CFRINIT1, f_1) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, CFRINIT0, f_0) < 0)
					goto err;
				if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x18) < 0)
					goto err;
				i++;
			}
		}

	}

	if (state->dev_ver >= 0x20) {
		if (STV090x_WRITE_DEMOD(state, CARFREQ, 0x49) < 0)
			goto err;
	}

	if ((state->delsys == STV090x_DVBS1) || (state->delsys == STV090x_DSS) ||
	        state->device == STX7111)
		stv090x_set_vit_thtracq(state);

	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_optimize_track: I/O error\n");
	return -1;
}

static int stv090x_get_feclock(struct stv090x_state *state, s32 timeout)
{
	s32 timer = 0, lock = 0, stat;
	u32 reg;

	dprintk(10, "%s >\n", __func__);
	while ((timer < timeout) && (!lock)) {
		reg = STV090x_READ_DEMOD(state, DMDSTATE);
		stat = STV090x_GETFIELD_Px(reg, HEADER_MODE_FIELD);

                dprintk(10, "reg = 0x%x, stat = %d\n", reg, stat);

		switch (stat) {
		case 0: /* searching */
		case 1: /* first PLH detected */
		default:
			lock = 0;
                        dprintk(20, "%s searching, plh detected or default\n", __func__);
			break;

		case 2: /* DVB-S2 mode */
			reg = STV090x_READ_DEMOD(state, PDELSTATUS1);
			lock = STV090x_GETFIELD_Px(reg, PKTDELIN_LOCK_FIELD);

                        dprintk(20, "%s dvb-s2 mode: reg = 0x%x, lock = %d\n", __func__, reg, lock);
			break;

		case 3: /* DVB-S1/legacy mode */
			reg = STV090x_READ_DEMOD(state, VSTATUSVIT);
			lock = STV090x_GETFIELD_Px(reg, LOCKEDVIT_FIELD);

                        dprintk(20, "%s dvb-s1 mode: reg = 0x%x, lock = %d\n", __func__, reg, lock);
			break;
		}
		if (!lock) {
			msleep(10);
			timer += 10;
		}
	}
	dprintk(10, "%s lock %d<\n", __func__, lock);
	return lock;
}

static int stv090x_get_lock(struct stv090x_state *state, s32 timeout_dmd, s32 timeout_fec)
{
	u32 reg;
	s32 timer = 0;
	int lock;

	dprintk(10, "%s >\n", __func__);

	lock = stv090x_get_dmdlock(state, timeout_dmd);
	if (lock)
		lock = stv090x_get_feclock(state, timeout_fec);

	if (lock) {
		lock = 0;

		while ((timer < timeout_fec) && (!lock)) {
			reg = STV090x_READ_DEMOD(state, TSSTATUS);
			lock = STV090x_GETFIELD_Px(reg, TSFIFO_LINEOK_FIELD);
			msleep(1);
			timer++;
		}
	}

	dprintk(10, "%s lock %d<\n", __func__, lock);
	return lock;
}

static int stv090x_set_s2rolloff(struct stv090x_state *state)
{
	u32 reg;

	dprintk(10, "%s >\n", __func__);
	if (state->dev_ver <= 0x20) {
		/* rolloff to auto mode if DVBS2 */
		reg = STV090x_READ_DEMOD(state, DEMOD);
		STV090x_SETFIELD_Px(reg, MANUAL_SXROLLOFF_FIELD, 0x00);
		if (STV090x_WRITE_DEMOD(state, DEMOD, reg) < 0)
			goto err;
	} else {
		/* DVB-S2 rolloff to auto mode if DVBS2 */
		reg = STV090x_READ_DEMOD(state, DEMOD);
		STV090x_SETFIELD_Px(reg, MANUAL_S2ROLLOFF_FIELD, 0x00);
		if (STV090x_WRITE_DEMOD(state, DEMOD, reg) < 0)
			goto err;
	}
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_set_s2rolloff: I/O error\n");
	return -1;
}

static enum stv090x_signal_state stv090x_algo(struct stv090x_state *state)
{
	struct dvb_frontend *fe = &state->frontend;
	enum stv090x_signal_state signal_state = STV090x_NOCARRIER;
	u32 reg;
	s32 timeout_dmd = 500, timeout_fec = 50, agc1_power, power_iq = 0, i;
	int lock = 0, low_sr = 0, no_signal = 0;

	dprintk(10, "%s >\n", __func__);

    if (state->device != STX7111)
	{
	   reg = STV090x_READ_DEMOD(state, TSCFGH);
	   STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 1); /* Stop path 1 stream merger */
	   if (STV090x_WRITE_DEMOD(state, TSCFGH, reg) < 0)
		   goto err;
    }

	if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x5c) < 0) /* Demod stop */
		goto err;

    if (state->device == STX7111)
	{
	   reg = STV090x_READ_DEMOD(state, PDELCTRL1);
	   STV090x_SETFIELD_Px(reg, ALGOSWRST_FIELD, 1);
	   if (STV090x_WRITE_DEMOD(state, PDELCTRL1, reg) < 0)
		   goto err;
    }

	if (state->dev_ver >= 0x20) {
		if (STV090x_WRITE_DEMOD(state, CORRELABS, 0x9e) < 0) /* cut 2.0 */
			goto err;
	}

    if (state->device == STX7111)
	{
	   reg = STV090x_READ_DEMOD(state, TSCFGH);
	   STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 1); /* Stop path 1 stream merger */
	   if (STV090x_WRITE_DEMOD(state, TSCFGH, reg) < 0)
		   goto err;
    }

	stv090x_get_lock_tmg(state);

	if (state->algo == STV090x_BLIND_SEARCH) {
		state->tuner_bw = 2 * 36000000; /* wide bw for unknown srate */
		if (STV090x_WRITE_DEMOD(state, TMGCFG2, 0xc0) < 0) /* wider srate scan */
			goto err;
		if (STV090x_WRITE_DEMOD(state, CORRELMANT, 0x70) < 0)
			goto err;
		if (stv090x_set_srate(state, 1000000) < 0) /* inital srate = 1Msps */
			goto err;
	} else {
		/* known srate */
		if (STV090x_WRITE_DEMOD(state, DMDTOM, 0x20) < 0)
			goto err;
		if (STV090x_WRITE_DEMOD(state, TMGCFG, 0xd2) < 0)
			goto err;

        if (state->device != STX7111)
		{
		   if (state->srate < 2000000) {
			   /* SR < 2MSPS */
			   if (STV090x_WRITE_DEMOD(state, CORRELMANT, 0x63) < 0)
				   goto err;
		   } else {
			   /* SR >= 2Msps */
			   if (STV090x_WRITE_DEMOD(state, CORRELMANT, 0x70) < 0)
				   goto err;
		   }
        }

		if (STV090x_WRITE_DEMOD(state, AGC2REF, 0x38) < 0)
			goto err;

		if (state->dev_ver >= 0x20) {
			if (STV090x_WRITE_DEMOD(state, KREFTMG, 0x5a) < 0)
				goto err;

			if (state->algo == STV090x_COLD_SEARCH)
				state->tuner_bw = (15 * (stv090x_car_width(state->srate, state->rolloff) + 10000000)) / 10;
			else if (state->algo == STV090x_WARM_SEARCH)
				state->tuner_bw = stv090x_car_width(state->srate, state->rolloff) + 10000000;
		}

		/* if cold start or warm  (Symbolrate is known)
		 * use a Narrow symbol rate scan range
		 */
		if (STV090x_WRITE_DEMOD(state, TMGCFG2, 0x01) < 0) /* narrow srate scan */
			goto err;

		if (stv090x_set_srate(state, state->srate) < 0)
			goto err;

		if (stv090x_set_max_srate(state, state->mclk, state->srate) < 0)
			goto err;
		if (stv090x_set_min_srate(state, state->mclk, state->srate) < 0)
			goto err;

		if (state->srate >= 10000000)
			low_sr = 0;
		else
			low_sr = 1;
	}

	/* Setup tuner */
	if (stv090x_i2c_gate_ctrl(fe, 1) < 0)
		goto err;

	if (state->config->tuner_set_bbgain) {
	}

	if (state->config->tuner_set_bandwidth) {
		if (state->config->tuner_set_bandwidth(fe, state->tuner_bw) < 0)
			goto err;
	}

	if (state->config->tuner_set_frequency) {
		if (state->config->tuner_set_frequency(fe, state->frequency) < 0)
			goto err;
	}

	if (state->config->tuner_set_bandwidth) {
		if (state->config->tuner_set_bandwidth(fe, state->tuner_bw) < 0)
			goto err;
	}

	if (state->config->tuner_get_status) {
		if (state->config->tuner_get_status(fe, &reg) < 0)
			goto err;
	}

	if (reg)
		dprintk(10, "1. Tuner phase locked\n");
	else
		dprintk(10, "1. Tuner unlocked\n");

	agc1_power = MAKEWORD16(STV090x_READ_DEMOD(state, AGCIQIN1),
				STV090x_READ_DEMOD(state, AGCIQIN0));

	dprintk(50, "agc1_power = %d\n", agc1_power);

	if (agc1_power == 0) {
		/* If AGC1 integrator value is 0
		 * then read POWERI, POWERQ
		 */
		for (i = 0; i < 5; i++) {
			power_iq += (STV090x_READ_DEMOD(state, POWERI) +
				     STV090x_READ_DEMOD(state, POWERQ)) >> 1;
		}
		power_iq /= 5;
	}

	if ((agc1_power == 0) && (power_iq < STV090x_IQPOWER_THRESHOLD)) {
		dprintk(50, "No Signal: POWER_IQ=0x%02x\n", power_iq);
		lock = 0;

	} else
	{
		reg = STV090x_READ_DEMOD(state, DEMOD);
		STV090x_SETFIELD_Px(reg, SPECINV_CONTROL_FIELD, state->inversion);

		if (state->dev_ver <= 0x20) {

			/* rolloff to auto mode if DVBS2 */
			STV090x_SETFIELD_Px(reg, MANUAL_SXROLLOFF_FIELD, 1);
		} else {

			/* DVB-S2 rolloff to auto mode if DVBS2 */
			STV090x_SETFIELD_Px(reg, MANUAL_S2ROLLOFF_FIELD, 1);
		}
		if (STV090x_WRITE_DEMOD(state, DEMOD, reg) < 0)
			goto err;

		if (stv090x_delivery_search(state) < 0)
			goto err;

		if (state->algo != STV090x_BLIND_SEARCH) {
			if (stv090x_start_search(state) < 0)
				goto err;
		}

        if (state->device == STX7111)
		{
		   reg = STV090x_READ_DEMOD(state, PDELCTRL1);
		   STV090x_SETFIELD_Px(reg, ALGOSWRST_FIELD, 0);
		   if (STV090x_WRITE_DEMOD(state, PDELCTRL1, reg) < 0)
			   goto err;

#warning fixme fixme 0xf3d0 ??? !!!
           if (stv090x_write_reg(state, 0xf5d0, 0x8) < 0)
			   goto err;

           if (stv090x_write_reg(state, 0xf5d0, 0x0) < 0)
			   goto err;
        }
	}

    if (state->device != STX7111)
	{
	   reg = stv090x_read_reg(state, STV090x_P2_TSCFGH);
	   STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0x01);
	   if (stv090x_write_reg(state, STV090x_P2_TSCFGH, reg) < 0)
		   goto err;
	   STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0x00);
	   if (stv090x_write_reg(state, STV090x_P2_TSCFGH, reg) < 0)
  		   goto err;
    }
	else
	{
           /* respect demod setting on stx7111 */
           reg = STV090x_READ_DEMOD(state, TSCFGH);
	   STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0x01);
	   if (STV090x_WRITE_DEMOD(state, TSCFGH, reg) < 0)
		   goto err;

	   STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0x00);
	   if (STV090x_WRITE_DEMOD(state, TSCFGH, reg) < 0)
  		   goto err;
	}

	/* need to check for AGC1 state */

	if (state->algo == STV090x_BLIND_SEARCH)
		lock = stv090x_blind_search(state);

	else if (state->algo == STV090x_COLD_SEARCH)
	{
		lock = stv090x_get_coldlock(state, timeout_dmd);
		dprintk(10, "cold_search ->lock = %d\n", lock);
	}
	else if (state->algo == STV090x_WARM_SEARCH)
		lock = stv090x_get_dmdlock(state, timeout_dmd);

	if ((!lock) && (state->algo == STV090x_COLD_SEARCH)) {
		if (!low_sr) {
			if (stv090x_chk_tmg(state))
				lock = stv090x_sw_algo(state);
		dprintk(10, "->lock = %d\n", lock);
		}
	}

	if (lock)
		signal_state = stv090x_get_sig_params(state);

	if ((lock) && (signal_state == STV090x_RANGEOK)) { /* signal within Range */
		dprintk(10, "lock && rangeok\n");

		stv090x_optimize_track(state);

		if (state->dev_ver >= 0x20) {
			/* >= Cut 2.0 :release TS reset after
			 * demod lock and optimized Tracking
			 */
			reg = STV090x_READ_DEMOD(state, TSCFGH);
			STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0); /* release merger reset */
			if (STV090x_WRITE_DEMOD(state, TSCFGH, reg) < 0)
				goto err;

			msleep(3);

			STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 1); /* merger reset */
			if (STV090x_WRITE_DEMOD(state, TSCFGH, reg) < 0)
				goto err;

			STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0); /* release merger reset */
			if (STV090x_WRITE_DEMOD(state, TSCFGH, reg) < 0)
				goto err;
		}

		if (stv090x_get_lock(state, timeout_fec, timeout_fec)) {
		dprintk(10, "get_lock ->lock\n");
			lock = 1;
			if (state->delsys == STV090x_DVBS2) {
				stv090x_set_s2rolloff(state);

				reg = STV090x_READ_DEMOD(state, PDELCTRL2);
				STV090x_SETFIELD_Px(reg, RESET_UPKO_COUNT, 1);
				if (STV090x_WRITE_DEMOD(state, PDELCTRL2, reg) < 0)
					goto err;
				/* Reset DVBS2 packet delinator error counter */
				reg = STV090x_READ_DEMOD(state, PDELCTRL2);
				STV090x_SETFIELD_Px(reg, RESET_UPKO_COUNT, 0);
				if (STV090x_WRITE_DEMOD(state, PDELCTRL2, reg) < 0)
					goto err;

				if (STV090x_WRITE_DEMOD(state, ERRCTRL1, 0x67) < 0) /* PER */
					goto err;
			} else {
				if (STV090x_WRITE_DEMOD(state, ERRCTRL1, 0x75) < 0)
					goto err;
			}

			/* Reset the Total packet counter */
			if (STV090x_WRITE_DEMOD(state, FBERCPT4, 0x00) < 0)
				goto err;
			/* Reset the packet Error counter2 */
			if (STV090x_WRITE_DEMOD(state, ERRCTRL2, 0xc1) < 0)
				goto err;
		} else {
		        dprintk(10, "get_lock ->no lock\n");
			lock = 0;
			signal_state = STV090x_NODATA;
			no_signal = stv090x_chk_signal(state);
		        dprintk(10, "no_signal = %d\n", no_signal);
		}
	}
	dprintk(10, "%s signal_state %d<\n", __func__, signal_state);
	return signal_state;

err:
	printk("stv090x_algo: I/O error\n");
	return -1;
}

#if DVB_API_VERSION < 5
static enum dvbfe_search stv090x_search(struct dvb_frontend *fe, struct dvbfe_params *p)
{
	struct stv090x_state *state = fe->demodulator_priv;
	enum stv090x_signal_state algo_state;

	dprintk(10, "%s: freq %d, symbol %d, inversion %d, rolloff %d, modulation %d, fec %d, delsys %d\n", __func__,
			p->frequency, p->delsys.dvbs.symbol_rate, p->inversion, p->delsys.dvbs.rolloff,
			p->delsys.dvbs.modulation, p->delsys.dvbs.fec,
			p->delivery);

    if ((p->frequency == 0) && (p->delsys.dvbs.symbol_rate == 0) && (p->inversion == 0) &&
        (p->delsys.dvbs.rolloff == 0) && (p->delsys.dvbs.modulation == 0) &&
        (p->delsys.dvbs.fec == 0) && (p->delivery == 0))
    {
       printk("[stv090x] -EINVAL\n");
       return DVBFE_ALGO_SEARCH_FAILED;
    }

    if (p->delivery == DVBFE_DELSYS_DVBS2)
        state->delsys = STV090x_DVBS2;
    else
    if (p->delivery == DVBFE_DELSYS_DVBS)
        state->delsys = STV090x_DVBS1;
    else
    if (p->delivery == DVBFE_DELSYS_DSS)
        state->delsys = STV090x_DSS;
    else
        state->delsys = STV090x_ERROR;

    state->frequency = p->frequency;
	state->srate = p->delsys.dvbs.symbol_rate;
	state->algo = STV090x_COLD_SEARCH;
	state->search_mode = STV090x_SEARCH_AUTO;
	state->fec = STV090x_PRERR;

	if (state->srate > 10000000) {
		dprintk(1, "Search range: 10 MHz");
		state->search_range = 10000000;
	} else {
		dprintk(1, "Search range: 5 MHz");
		state->search_range = 5000000;
	}

	algo_state = stv090x_algo(state);

	if (algo_state == STV090x_RANGEOK) {
		dprintk(1, "Search success!");
		return DVBFE_ALGO_SEARCH_SUCCESS;
	} else {
		dprintk(1, "Search failed! %d", algo_state);
		return DVBFE_ALGO_SEARCH_FAILED;
	}

	return DVBFE_ALGO_SEARCH_ERROR;
}
#else
static enum dvbfe_search stv090x_search(struct dvb_frontend *fe, struct dvb_frontend_parameters *p)
{
    struct stv090x_state *state = fe->demodulator_priv;
    struct dtv_frontend_properties *props = &fe->dtv_property_cache;

    if (p->frequency == 0)
        return DVBFE_ALGO_SEARCH_INVALID;

    state->delsys = props->delivery_system;
    state->frequency = p->frequency;
    state->srate = p->u.qpsk.symbol_rate;
    state->search_mode = STV090x_SEARCH_AUTO;
    state->algo = STV090x_COLD_SEARCH;
    state->fec = STV090x_PRERR;
    if (state->srate > 10000000) {
        dprintk(1, "Search range: 10 MHz");
        state->search_range = 10000000;
    } else {
        dprintk(1, "Search range: 5 MHz");
        state->search_range = 5000000;
    }

    if (stv090x_algo(state) == STV090x_RANGEOK) {
        dprintk(1, "Search success!");
        return DVBFE_ALGO_SEARCH_SUCCESS;
    } else {
        dprintk(1, "Search failed!");
        return DVBFE_ALGO_SEARCH_FAILED;
    }

    return DVBFE_ALGO_SEARCH_ERROR;
}
#endif

static int stv090x_read_status(struct dvb_frontend *fe, enum fe_status *status)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg;
	u8 search_state;

	dprintk(10, "%s >\n", __FUNCTION__);

	reg = STV090x_READ_DEMOD(state, DMDSTATE);
	search_state = STV090x_GETFIELD_Px(reg, HEADER_MODE_FIELD);

	switch (search_state) {
	case 0: /* searching */
	case 1: /* first PLH detected */
	default:
		dprintk(50, "Status: Unlocked (Searching ..)\n");
		*status = 0;
		break;

	case 2: /* DVB-S2 mode */
		dprintk(50, "Delivery system: DVB-S2\n");
		reg = STV090x_READ_DEMOD(state, DSTATUS);
		if (STV090x_GETFIELD_Px(reg, LOCK_DEFINITIF_FIELD)) {
			reg = STV090x_READ_DEMOD(state, PDELSTATUS1);
			if (STV090x_GETFIELD_Px(reg, PKTDELIN_LOCK_FIELD)) {
			   reg = STV090x_READ_DEMOD(state, TSSTATUS);
			   if (STV090x_GETFIELD_Px(reg, TSFIFO_LINEOK_FIELD)) {
					*status = FE_HAS_SIGNAL |
						  FE_HAS_CARRIER |
						  FE_HAS_VITERBI |
						  FE_HAS_SYNC |
						  FE_HAS_LOCK;
			   }
            }
		}
		break;

	case 3: /* DVB-S1/legacy mode */
		dprintk(50, "Delivery system: DVB-S\n");
		reg = STV090x_READ_DEMOD(state, DSTATUS);
		if (STV090x_GETFIELD_Px(reg, LOCK_DEFINITIF_FIELD)) {
			reg = STV090x_READ_DEMOD(state, VSTATUSVIT);
			if (STV090x_GETFIELD_Px(reg, LOCKEDVIT_FIELD)) {
				reg = STV090x_READ_DEMOD(state, TSSTATUS);
				if (STV090x_GETFIELD_Px(reg, TSFIFO_LINEOK_FIELD)) {
					*status = FE_HAS_SIGNAL |
						  FE_HAS_CARRIER |
						  FE_HAS_VITERBI |
						  FE_HAS_SYNC |
						  FE_HAS_LOCK;
				}
			}
		}
		break;
	}

	dprintk(10, "%s status = %d<\n", __FUNCTION__, *status);
	return 0;
}

static int stv090x_read_per(struct dvb_frontend *fe, u32 *per)
{
	struct stv090x_state *state = fe->demodulator_priv;

	s32 count_4, count_3, count_2, count_1, count_0, count;
	u32 reg, h, m, l;
	enum fe_status status;

	dprintk(10, "%s >\n", __FUNCTION__);

	stv090x_read_status(fe, &status);
	if (!(status & FE_HAS_LOCK)) {
		*per = 1 << 23; /* Max PER */
	} else {
		/* Counter 2 */
		reg = STV090x_READ_DEMOD(state, ERRCNT22);
		h = STV090x_GETFIELD_Px(reg, ERR_CNT2_FIELD);

		reg = STV090x_READ_DEMOD(state, ERRCNT21);
		m = STV090x_GETFIELD_Px(reg, ERR_CNT21_FIELD);

		reg = STV090x_READ_DEMOD(state, ERRCNT20);
		l = STV090x_GETFIELD_Px(reg, ERR_CNT20_FIELD);

		*per = ((h << 16) | (m << 8) | l);

		count_4 = STV090x_READ_DEMOD(state, FBERCPT4);
		count_3 = STV090x_READ_DEMOD(state, FBERCPT3);
		count_2 = STV090x_READ_DEMOD(state, FBERCPT2);
		count_1 = STV090x_READ_DEMOD(state, FBERCPT1);
		count_0 = STV090x_READ_DEMOD(state, FBERCPT0);

		if ((!count_4) && (!count_3)) {
			count  = (count_2 & 0xff) << 16;
			count |= (count_1 & 0xff) <<  8;
			count |=  count_0 & 0xff;
		} else {
			count = 1 << 24;
		}
		if (count == 0)
			*per = 1;
	}
	if (STV090x_WRITE_DEMOD(state, FBERCPT4, 0) < 0)
		goto err;
	if (STV090x_WRITE_DEMOD(state, ERRCTRL2, 0xc1) < 0)
		goto err;

	dprintk(10, "%s per = %d<\n", __FUNCTION__, *per);

	return 0;
err:
	printk("stv090x_read_per: I/O error\n");
	return -1;
}

/* powarman's vdr version, because orig is buggy */
static int stv090x_table_lookup(const struct stv090x_tab *tab, int max, int val)
{
	int res = 0;
	int min = 0, med;

	if ((val >= tab[min].read && val < tab[max].read) ||
        (val >= tab[max].read && val < tab[min].read)) {
		while ((max - min) > 1) {
			med = (max + min) / 2;
			if ((val >= tab[min].read && val < tab[med].read) ||
                (val >= tab[med].read && val < tab[min].read))
				max = med;
			else
				min = med;
		}
		res = ((val - tab[min].read) *
		       (tab[max].real - tab[min].real) /
		       (tab[max].read - tab[min].read)) +
			tab[min].real;
	} else {
		if (tab[min].read < tab[max].read) {
			if (val < tab[min].read)
				res = tab[min].real;
			else if (val >= tab[max].read)
				res = tab[max].real;
		} else {
			if (val >= tab[min].read)
				res = tab[min].real;
			else if (val < tab[max].read)
				res = tab[max].real;
		}
	}

	return res;
}

static int stv090x_read_signal_strength(struct dvb_frontend *fe, u16 *strength)
{
	struct stv090x_state *state = fe->demodulator_priv;
	s32 agc;

	dprintk(10, "%s >\n", __FUNCTION__);

        *strength = 0;
/*

	reg = STV090x_READ_DEMOD(state, AGCIQIN1);
	agc = STV090x_GETFIELD_Px(reg, AGCIQ_VALUE_FIELD);
*/
//TDT
	agc = MAKEWORD16(STV090x_READ_DEMOD(state, AGCIQIN1),
				STV090x_READ_DEMOD(state, AGCIQIN0));

        dprintk(50, "agc = 0x%04x\n", agc);

	*strength = stv090x_table_lookup(stv090x_rf_tab, ARRAY_SIZE(stv090x_rf_tab) - 1, agc);
	if (agc > stv090x_rf_tab[0].read)
		*strength = 5;
	else if (agc < stv090x_rf_tab[ARRAY_SIZE(stv090x_rf_tab) - 1].read)
		*strength = -100;

//TDT from powarman's vdr version
	*strength = (*strength + 100) * 0xFFFF / 100;

	dprintk(10, "%s strength %d <\n", __FUNCTION__, *strength);
	return 0;
}

static int stv090x_read_cnr(struct dvb_frontend *fe, u16 *cnr)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg_0, reg_1, reg, i;
	s32 val_0, val_1, val = 0;
	u8 lock_f;
	s32 snr;
	s32 div;

    switch (state->delsys) {
	case STV090x_DVBS2:
		reg = STV090x_READ_DEMOD(state, DSTATUS);
		lock_f = STV090x_GETFIELD_Px(reg, LOCK_DEFINITIF_FIELD);
		if (lock_f) {
			msleep(5);
			for (i = 0; i < 16; i++) {
				reg_1 = STV090x_READ_DEMOD(state, NNOSPLHT1);
				val_1 = STV090x_GETFIELD_Px(reg_1, NOSPLHT_NORMED_FIELD);
				reg_0 = STV090x_READ_DEMOD(state, NNOSPLHT0);
				val_0 = STV090x_GETFIELD_Px(reg_0, NOSPLHT_NORMED_FIELD);
				val  += MAKEWORD16(val_1, val_0);
				msleep(1);
			}
			val /= 16;
			snr = stv090x_table_lookup(stv090x_s2cn_tab, ARRAY_SIZE(stv090x_s2cn_tab) - 1, val);
			div = stv090x_s2cn_tab[0].read - stv090x_s2cn_tab[ARRAY_SIZE(stv090x_s2cn_tab) - 1].read;
			*cnr = 0xFFFF - ((val * 0xFFFF) / div);
			/* *cnr = snr * 0xFFFF / stv090x_s2cn_tab[ARRAY_SIZE(stv090x_s2cn_tab) - 1].real; */
		}
		break;

	case STV090x_DVBS1:
	case STV090x_DSS:
		reg = STV090x_READ_DEMOD(state, DSTATUS);
		lock_f = STV090x_GETFIELD_Px(reg, LOCK_DEFINITIF_FIELD);
		if (lock_f) {
			msleep(5);
			for (i = 0; i < 16; i++) {
				reg_1 = STV090x_READ_DEMOD(state, NOSDATAT1);
				val_1 = STV090x_GETFIELD_Px(reg_1, NOSDATAT_UNNORMED_FIELD);
				reg_0 = STV090x_READ_DEMOD(state, NOSDATAT0);
				val_0 = STV090x_GETFIELD_Px(reg_0, NOSDATAT_UNNORMED_FIELD);
				val  += MAKEWORD16(val_1, val_0);
				msleep(1);
			}
			val /= 16;
			snr = stv090x_table_lookup(stv090x_s1cn_tab, ARRAY_SIZE(stv090x_s1cn_tab) - 1, val);
			div = stv090x_s1cn_tab[0].read - stv090x_s1cn_tab[ARRAY_SIZE(stv090x_s1cn_tab) - 1].read;
			*cnr = 0xFFFF - ((val * 0xFFFF) / div);
		}
		break;
	default:

	    *cnr = 0;
		break;
	}

	dprintk(10, "%s cnr %d <\n", __FUNCTION__, *cnr);
	return 0;
}

static int stv090x_set_tone(struct dvb_frontend *fe, fe_sec_tone_mode_t tone)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg;

	dprintk(10, "%s >\n", __FUNCTION__);

	reg = STV090x_READ_DEMOD(state, DISTXCTL);
	switch (tone) {
	case SEC_TONE_ON:
		STV090x_SETFIELD_Px(reg, DISTX_MODE_FIELD, 0);
		STV090x_SETFIELD_Px(reg, DISEQC_RESET_FIELD, 1);
		if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
			goto err;
		STV090x_SETFIELD_Px(reg, DISEQC_RESET_FIELD, 0);
		if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
			goto err;
		break;

	case SEC_TONE_OFF:
		STV090x_SETFIELD_Px(reg, DISTX_MODE_FIELD, 0);
		STV090x_SETFIELD_Px(reg, DISEQC_RESET_FIELD, 1);
		if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
			goto err;
		break;
	default:
		return -EINVAL;
	}

	dprintk(10, "%s <\n", __FUNCTION__);

	return 0;
err:
	printk("stv090x_set_tone: I/O error\n");
	return -1;
}


static enum dvbfe_algo stv090x_frontend_algo(struct dvb_frontend *fe)
{
	return DVBFE_ALGO_CUSTOM;
}

static int stv090x_send_diseqc_msg(struct dvb_frontend *fe, struct dvb_diseqc_master_cmd *cmd)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg, idle = 0, fifo_full = 1;
	int i;

	dprintk(10, "%s >\n", __FUNCTION__);

	reg = STV090x_READ_DEMOD(state, DISTXCTL);

	STV090x_SETFIELD_Px(reg, DISTX_MODE_FIELD, 2);
	STV090x_SETFIELD_Px(reg, DISEQC_RESET_FIELD, 1);
	if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
		goto err;
	STV090x_SETFIELD_Px(reg, DISEQC_RESET_FIELD, 0);
	if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
		goto err;

	STV090x_SETFIELD_Px(reg, DIS_PRECHARGE_FIELD, 1);
	if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
		goto err;

	for (i = 0; i < cmd->msg_len; i++) {

		while (fifo_full) {
			reg = STV090x_READ_DEMOD(state, DISTXSTATUS);
			fifo_full = STV090x_GETFIELD_Px(reg, FIFO_FULL_FIELD);
		}

		if (STV090x_WRITE_DEMOD(state, DISTXDATA, cmd->msg[i]) < 0)
			goto err;
	}
	reg = STV090x_READ_DEMOD(state, DISTXCTL);
	STV090x_SETFIELD_Px(reg, DIS_PRECHARGE_FIELD, 0);
	if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
		goto err;

	i = 0;

	while ((!idle) && (i < 10)) {
		reg = STV090x_READ_DEMOD(state, DISTXSTATUS);
		idle = STV090x_GETFIELD_Px(reg, TX_IDLE_FIELD);
		msleep(10);
		i++;
	}

	dprintk(10, "%s <\n", __FUNCTION__);
	return 0;
err:
	printk("stv090x_send_diseqc_msg: I/O error\n");
	return -1;
}

static int stv090x_send_diseqc_burst(struct dvb_frontend *fe, fe_sec_mini_cmd_t burst)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg, idle = 0, fifo_full = 1;
	u8 mode, value;
	int i;

	dprintk(10, "%s >\n", __FUNCTION__);
	reg = STV090x_READ_DEMOD(state, DISTXCTL);

	if (burst == SEC_MINI_A) {
		mode = 3;
		value = 0x00;
	} else {
		mode = 2;
		value = 0xFF;
	}

	STV090x_SETFIELD_Px(reg, DISTX_MODE_FIELD, mode);
	STV090x_SETFIELD_Px(reg, DISEQC_RESET_FIELD, 1);
	if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
		goto err;
	STV090x_SETFIELD_Px(reg, DISEQC_RESET_FIELD, 0);
	if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
		goto err;

	STV090x_SETFIELD_Px(reg, DIS_PRECHARGE_FIELD, 1);
	if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
		goto err;

	while (fifo_full) {
		reg = STV090x_READ_DEMOD(state, DISTXSTATUS);
		fifo_full = STV090x_GETFIELD_Px(reg, FIFO_FULL_FIELD);
	}

	if (STV090x_WRITE_DEMOD(state, DISTXDATA, value) < 0)
		goto err;

	reg = STV090x_READ_DEMOD(state, DISTXCTL);
	STV090x_SETFIELD_Px(reg, DIS_PRECHARGE_FIELD, 0);
	if (STV090x_WRITE_DEMOD(state, DISTXCTL, reg) < 0)
		goto err;

	i = 0;

	while ((!idle) && (i < 10)) {
		reg = STV090x_READ_DEMOD(state, DISTXSTATUS);
		idle = STV090x_GETFIELD_Px(reg, TX_IDLE_FIELD);
		msleep(10);
		i++;
	}

	dprintk(10, "%s <\n", __FUNCTION__);
	return 0;
err:
	printk("stv090x_send_diseqc_burst: I/O error\n");
	return -1;
}

static int stv090x_recv_slave_reply(struct dvb_frontend *fe, struct dvb_diseqc_slave_reply *reply)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg = 0, i = 0, rx_end = 0;

	dprintk(10, "%s >\n", __FUNCTION__);

	while ((rx_end != 1) && (i < 10)) {
		msleep(10);
		i++;
		reg = STV090x_READ_DEMOD(state, DISRX_ST0);
		rx_end = STV090x_GETFIELD_Px(reg, RX_END_FIELD);
	}

	if (rx_end) {
		reply->msg_len = STV090x_GETFIELD_Px(reg, FIFO_BYTENBR_FIELD);
		for (i = 0; i < reply->msg_len; i++)
			reply->msg[i] = STV090x_READ_DEMOD(state, DISRXDATA);
	}

	dprintk(10, "%s <\n", __FUNCTION__);
	return 0;
}

static int stv090x_sleep(struct dvb_frontend *fe)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg;

	if (state->config->tuner_sleep) {
		if (state->config->tuner_sleep(fe) < 0)
			goto err;
	}

	dprintk(10, "Set %s to sleep\n",
		state->device == STV0900 ? "STV0900" : "STV0903");

	reg = stv090x_read_reg(state, STV090x_SYNTCTRL);
	STV090x_SETFIELD(reg, STANDBY_FIELD, 0x01);
	if (stv090x_write_reg(state, STV090x_SYNTCTRL, reg) < 0)
		goto err;

	reg = stv090x_read_reg(state, STV090x_TSTTNR1);
	STV090x_SETFIELD(reg, ADC1_PON_FIELD, 0);
	if (stv090x_write_reg(state, STV090x_TSTTNR1, reg) < 0)
		goto err;

	return 0;
err:
	printk("stv090x_sleep: I/O error\n");
	return -1;
}

static int stv090x_wakeup(struct dvb_frontend *fe)
{
	struct stv090x_state *state = fe->demodulator_priv;
	u32 reg;

	dprintk(10, "Wake %s from standby\n",
		state->device == STV0900 ? "STV0900" : "STV0903");

	reg = stv090x_read_reg(state, STV090x_SYNTCTRL);
	STV090x_SETFIELD(reg, STANDBY_FIELD, 0x00);
	if (stv090x_write_reg(state, STV090x_SYNTCTRL, reg) < 0)
		goto err;

	reg = stv090x_read_reg(state, STV090x_TSTTNR1);
	STV090x_SETFIELD(reg, ADC1_PON_FIELD, 1);
	if (stv090x_write_reg(state, STV090x_TSTTNR1, reg) < 0)
		goto err;

	return 0;
err:
	printk("stv090x_wakeup: I/O error\n");
	return -1;
}

static void stv090x_release(struct dvb_frontend *fe)
{
	struct stv090x_state *state = fe->demodulator_priv;

	kfree(state);
}

static int stv090x_ldpc_mode(struct stv090x_state *state, enum stv090x_mode ldpc_mode)
{
	u32 reg = 0;

	dprintk(10, "%s >\n", __func__);

	reg = stv090x_read_reg(state, STV090x_GENCFG);

	switch (ldpc_mode) {
	case STV090x_DUAL:
	default:
		if ((state->demod_mode != STV090x_DUAL) || (STV090x_GETFIELD(reg, DDEMOD_FIELD) != 1)) {
			/* set LDPC to dual mode */
			if (stv090x_write_reg(state, STV090x_GENCFG, 0x1d) < 0)
				goto err;

			dprintk(10, "setting to dual mode\n");

			state->demod_mode = STV090x_DUAL;

			reg = stv090x_read_reg(state, STV090x_TSTRES0);
			STV090x_SETFIELD(reg, FRESFEC_FIELD, 0x1);
			if (stv090x_write_reg(state, STV090x_TSTRES0, reg) < 0)
				goto err;
			STV090x_SETFIELD(reg, FRESFEC_FIELD, 0x0);
			if (stv090x_write_reg(state, STV090x_TSTRES0, reg) < 0)
				goto err;

			if (STV090x_WRITE_DEMOD(state, MODCODLST0, 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLST1, 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLST2, 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLST3, 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLST4, 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLST5, 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLST6, 0xff) < 0)
				goto err;

			if (STV090x_WRITE_DEMOD(state, MODCODLST7, 0xcc) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLST8, 0xcc) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLST9, 0xcc) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLSTA, 0xcc) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLSTB, 0xcc) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLSTC, 0xcc) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLSTD, 0xcc) < 0)
				goto err;

			if (STV090x_WRITE_DEMOD(state, MODCODLSTE, 0xff) < 0)
				goto err;
			if (STV090x_WRITE_DEMOD(state, MODCODLSTF, 0xcf) < 0)
				goto err;
		}
		break;

	case STV090x_SINGLE:

		dprintk(10, "setting to single mode\n");

		if (stv090x_stop_modcod(state) < 0)
			goto err;
		if (stv090x_activate_modcod_single(state) < 0)
			goto err;

		if (state->demod == STV090x_DEMODULATOR_1) {
			if (stv090x_write_reg(state, STV090x_GENCFG, 0x06) < 0) /* path 2 */
				goto err;
		} else {
			if (stv090x_write_reg(state, STV090x_GENCFG, 0x04) < 0) /* path 1 */
				goto err;
		}

		reg = stv090x_read_reg(state, STV090x_TSTRES0);
		STV090x_SETFIELD(reg, FRESFEC_FIELD, 0x1);
		if (stv090x_write_reg(state, STV090x_TSTRES0, reg) < 0)
			goto err;
		STV090x_SETFIELD(reg, FRESFEC_FIELD, 0x0);
		if (stv090x_write_reg(state, STV090x_TSTRES0, reg) < 0)
			goto err;

		reg = STV090x_READ_DEMOD(state, PDELCTRL1);
		STV090x_SETFIELD_Px(reg, ALGOSWRST_FIELD, 0x01);
		if (STV090x_WRITE_DEMOD(state, PDELCTRL1, reg) < 0)
			goto err;
		STV090x_SETFIELD_Px(reg, ALGOSWRST_FIELD, 0x00);
		if (STV090x_WRITE_DEMOD(state, PDELCTRL1, reg) < 0)
			goto err;
		break;
	}

	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_ldpc_mode: I/O error\n");
	return -1;
}

/* return (Hz), clk in Hz*/
static u32 stv090x_get_mclk(struct stv090x_state *state)
{
	const struct stv090x_config *config = state->config;

	if  (state->device == STX7111)
    {
	   u32 n_div = 0;
	   u32 m_div = 0;
       u32 mclk = 0;
       u32 reg;

	   reg = stv090x_read_reg(state, STV090x_NCOARSE);
	   m_div = STV090x_GETFIELD(reg, M_DIV_FIELD);

	   reg = stv090x_read_reg(state, STV090x_NCOARSE1);
	   n_div = STV090x_GETFIELD(reg, N_DIV_FIELD);

	   dprintk(10, "n_div = %d, m_div =%d\n", n_div, m_div);

	   if (m_div == 0)
		m_div = 1;

	   if (n_div == 0)
		n_div = 1;
	   mclk = n_div * (config->xtal / 100);

	   mclk /= (m_div * 2);
	   mclk = mclk * 100;

	   return mclk;

    } else
	{
	   u32 div, reg;
	   u8 ratio;

	   dprintk(10, "%s >\n", __func__);

	   div = stv090x_read_reg(state, STV090x_NCOARSE);
	   reg = stv090x_read_reg(state, STV090x_SYNTCTRL);
	   ratio = STV090x_GETFIELD(reg, SELX1RATIO_FIELD) ? 4 : 6;

	   dprintk(10, "%s <(div = %d, ratio %d, xtal %d)\n", __func__, div, ratio, config->xtal);
	   return (div + 1) * (config->xtal / 2) / ratio; /* kHz */
        }
}


static int stv090x_set_mclk(struct stv090x_state *state, u32 mclk, u32 clk)
{
	u32 reg, div, clk_sel;

	dprintk(10, "%s >\n", __func__);

	if  (state->device == STX7111)
        {
	    reg = stv090x_read_reg(state, STV090x_NCOARSE);
	    STV090x_SETFIELD(reg, M_DIV_FIELD, 0x06);
	    if (stv090x_write_reg(state, STV090x_NCOARSE, reg) < 0)
		    goto err;

	    msleep(2);

	    reg = stv090x_read_reg(state, STV090x_NCOARSE1);
	    STV090x_SETFIELD(reg, N_DIV_FIELD, 0x37);
	    if (stv090x_write_reg(state, STV090x_NCOARSE1, reg) < 0)
		    goto err;

	    state->mclk = stv090x_get_mclk(state);

	    dprintk(10, "%s: reading the masterclock = %d\n", __func__, state->mclk);

	}
	else
	{
	   reg = stv090x_read_reg(state, STV090x_SYNTCTRL);
	   clk_sel = ((STV090x_GETFIELD(reg, SELX1RATIO_FIELD) == 1) ? 4 : 6);

	   div = ((clk_sel * mclk) / clk) - 1;

	   reg = stv090x_read_reg(state, STV090x_NCOARSE);
	   STV090x_SETFIELD(reg, M_DIV_FIELD, div);
	   if (stv090x_write_reg(state, STV090x_NCOARSE, reg) < 0)
		   goto err;

	   state->mclk = stv090x_get_mclk(state);

	   dprintk(10, "%s: reading the masterclock = %d\n", __func__, state->mclk);

	   /*Set the DiseqC frequency to 22KHz */

	   div = state->mclk / 704000;

	   dprintk(10, "%d 0x%02x\n", div, div);

	   if (STV090x_WRITE_DEMOD(state, F22TX, div) < 0)
		   goto err;
	   if (STV090x_WRITE_DEMOD(state, F22RX, div) < 0)
		   goto err;
        }
	dprintk(10, "%s <\n", __func__);
	return 0;
err:
	printk("stv090x_set_mclk: I/O error\n");
	return -1;
}

static int stv090x_set_tspath(struct stv090x_state *state)
{
	u32 reg;

	dprintk(10, "%s >\n", __FUNCTION__);

	dprintk(20, "\tts path1 %d\n", state->config->ts1_mode);
	dprintk(20, "\tts path2 %d\n", state->config->ts2_mode);

	if (state->dev_ver >= 0x20)
	{
		switch (state->config->ts1_mode)
		{
		case STV090x_TSMODE_PARALLEL_PUNCTURED:
		case STV090x_TSMODE_DVBCI:

			switch (state->config->ts2_mode)
			{
			case STV090x_TSMODE_SERIAL_PUNCTURED:
			case STV090x_TSMODE_SERIAL_CONTINUOUS:
			default:
				stv090x_write_reg(state, STV090x_TSGENERAL, 0x00);
				break;

			case STV090x_TSMODE_PARALLEL_PUNCTURED:
			case STV090x_TSMODE_DVBCI:
				if (stv090x_write_reg(state, STV090x_TSGENERAL, 0x06) < 0) /* Mux'd stream mode */
					goto err;
				reg = stv090x_read_reg(state, STV090x_P1_TSCFGM);
				STV090x_SETFIELD_Px(reg, TSFIFO_MANSPEED_FIELD, 3);
				if (stv090x_write_reg(state, STV090x_P1_TSCFGM, reg) < 0)
					goto err;
				reg = stv090x_read_reg(state, STV090x_P2_TSCFGM);
				STV090x_SETFIELD_Px(reg, TSFIFO_MANSPEED_FIELD, 3);
				if (stv090x_write_reg(state, STV090x_P2_TSCFGM, reg) < 0)
					goto err;
				if (stv090x_write_reg(state, STV090x_P1_TSSPEED, 0x14) < 0)
					goto err;
				if (stv090x_write_reg(state, STV090x_P2_TSSPEED, 0x28) < 0)
					goto err;
				break;
			}
			break;

		case STV090x_TSMODE_SERIAL_PUNCTURED:
		case STV090x_TSMODE_SERIAL_CONTINUOUS:
		default:
			switch (state->config->ts2_mode) {
			case STV090x_TSMODE_SERIAL_PUNCTURED:
			case STV090x_TSMODE_SERIAL_CONTINUOUS:
			default:
				if (stv090x_write_reg(state, STV090x_TSGENERAL, 0x0c) < 0)
					goto err;
				break;

			case STV090x_TSMODE_PARALLEL_PUNCTURED:
			case STV090x_TSMODE_DVBCI:
				if (stv090x_write_reg(state, STV090x_TSGENERAL, 0x0a) < 0)
					goto err;
				break;
			}
			break;
		}
	} else {
		switch (state->config->ts1_mode) {
		case STV090x_TSMODE_PARALLEL_PUNCTURED:
		case STV090x_TSMODE_DVBCI:
			switch (state->config->ts2_mode) {
			case STV090x_TSMODE_SERIAL_PUNCTURED:
			case STV090x_TSMODE_SERIAL_CONTINUOUS:
			default:
				stv090x_write_reg(state, STV090x_TSGENERAL1X, 0x10);
				break;

			case STV090x_TSMODE_PARALLEL_PUNCTURED:
			case STV090x_TSMODE_DVBCI:
				stv090x_write_reg(state, STV090x_TSGENERAL1X, 0x16);
				reg = stv090x_read_reg(state, STV090x_P1_TSCFGM);
				STV090x_SETFIELD_Px(reg, TSFIFO_MANSPEED_FIELD, 3);
				if (stv090x_write_reg(state, STV090x_P1_TSCFGM, reg) < 0)
					goto err;
				reg = stv090x_read_reg(state, STV090x_P1_TSCFGM);
				STV090x_SETFIELD_Px(reg, TSFIFO_MANSPEED_FIELD, 0);
				if (stv090x_write_reg(state, STV090x_P1_TSCFGM, reg) < 0)
					goto err;
				if (stv090x_write_reg(state, STV090x_P1_TSSPEED, 0x14) < 0)
					goto err;
				if (stv090x_write_reg(state, STV090x_P2_TSSPEED, 0x28) < 0)
					goto err;
				break;
			}
			break;

		case STV090x_TSMODE_SERIAL_PUNCTURED:
		case STV090x_TSMODE_SERIAL_CONTINUOUS:
		default:
			switch (state->config->ts2_mode) {
			case STV090x_TSMODE_SERIAL_PUNCTURED:
			case STV090x_TSMODE_SERIAL_CONTINUOUS:
			default:
				stv090x_write_reg(state, STV090x_TSGENERAL1X, 0x14);
				break;

			case STV090x_TSMODE_PARALLEL_PUNCTURED:
			case STV090x_TSMODE_DVBCI:
				stv090x_write_reg(state, STV090x_TSGENERAL1X, 0x12);
				break;
			}
			break;
		}
	}

	switch (state->config->ts1_mode) {
	case STV090x_TSMODE_PARALLEL_PUNCTURED:
		reg = stv090x_read_reg(state, STV090x_P1_TSCFGH);
		STV090x_SETFIELD_Px(reg, TSFIFO_SERIAL_FIELD, 0x00);
		STV090x_SETFIELD_Px(reg, TSFIFO_DVBCI_FIELD, 0x00);
		if (stv090x_write_reg(state, STV090x_P1_TSCFGH, reg) < 0)
			goto err;
		break;

	case STV090x_TSMODE_DVBCI:
		reg = stv090x_read_reg(state, STV090x_P1_TSCFGH);
		STV090x_SETFIELD_Px(reg, TSFIFO_SERIAL_FIELD, 0x00);
		STV090x_SETFIELD_Px(reg, TSFIFO_DVBCI_FIELD, 0x01);
		if (stv090x_write_reg(state, STV090x_P1_TSCFGH, reg) < 0)
			goto err;
		break;

	case STV090x_TSMODE_SERIAL_PUNCTURED:
		reg = stv090x_read_reg(state, STV090x_P1_TSCFGH);
		STV090x_SETFIELD_Px(reg, TSFIFO_SERIAL_FIELD, 0x01);
		STV090x_SETFIELD_Px(reg, TSFIFO_DVBCI_FIELD, 0x00);
		if (stv090x_write_reg(state, STV090x_P1_TSCFGH, reg) < 0)
			goto err;
		break;

	case STV090x_TSMODE_SERIAL_CONTINUOUS:
		reg = stv090x_read_reg(state, STV090x_P1_TSCFGH);
		STV090x_SETFIELD_Px(reg, TSFIFO_SERIAL_FIELD, 0x01);
		STV090x_SETFIELD_Px(reg, TSFIFO_DVBCI_FIELD, 0x01);
		if (stv090x_write_reg(state, STV090x_P1_TSCFGH, reg) < 0)
			goto err;
		break;

	default:
		break;
	}

	switch (state->config->ts2_mode) {
	case STV090x_TSMODE_PARALLEL_PUNCTURED:
		reg = stv090x_read_reg(state, STV090x_P2_TSCFGH);
		STV090x_SETFIELD_Px(reg, TSFIFO_SERIAL_FIELD, 0x00);
		STV090x_SETFIELD_Px(reg, TSFIFO_DVBCI_FIELD, 0x00);
		if (stv090x_write_reg(state, STV090x_P2_TSCFGH, reg) < 0)
			goto err;
		break;

	case STV090x_TSMODE_DVBCI:
		reg = stv090x_read_reg(state, STV090x_P2_TSCFGH);
		STV090x_SETFIELD_Px(reg, TSFIFO_SERIAL_FIELD, 0x00);
		STV090x_SETFIELD_Px(reg, TSFIFO_DVBCI_FIELD, 0x01);
		if (stv090x_write_reg(state, STV090x_P2_TSCFGH, reg) < 0)
			goto err;
		break;

	case STV090x_TSMODE_SERIAL_PUNCTURED:
		reg = stv090x_read_reg(state, STV090x_P2_TSCFGH);
		STV090x_SETFIELD_Px(reg, TSFIFO_SERIAL_FIELD, 0x01);
		STV090x_SETFIELD_Px(reg, TSFIFO_DVBCI_FIELD, 0x00);
		if (stv090x_write_reg(state, STV090x_P2_TSCFGH, reg) < 0)
			goto err;
		break;

	case STV090x_TSMODE_SERIAL_CONTINUOUS:
		reg = stv090x_read_reg(state, STV090x_P2_TSCFGH);
		STV090x_SETFIELD_Px(reg, TSFIFO_SERIAL_FIELD, 0x01);
		STV090x_SETFIELD_Px(reg, TSFIFO_DVBCI_FIELD, 0x01);
		if (stv090x_write_reg(state, STV090x_P2_TSCFGH, reg) < 0)
			goto err;
		break;

	default:
		break;
	}

	if (state->config->ts1_clk > 0) {
		u32 speed;

		switch (state->config->ts1_mode) {
		case STV090x_TSMODE_PARALLEL_PUNCTURED:
		case STV090x_TSMODE_DVBCI:
		default:
			speed = state->mclk / (state->config->ts1_clk / 4);
			if (speed < 0x08)
				speed = 0x08;
			if (speed > 0xFF)
				speed = 0xFF;
			break;
		case STV090x_TSMODE_SERIAL_PUNCTURED:
		case STV090x_TSMODE_SERIAL_CONTINUOUS:
			speed = state->mclk / (state->config->ts1_clk / 32);
			if (speed < 0x20)
				speed = 0x20;
			if (speed > 0xFF)
				speed = 0xFF;
			break;
		}
		reg = stv090x_read_reg(state, STV090x_P1_TSCFGM);
		STV090x_SETFIELD_Px(reg, TSFIFO_MANSPEED_FIELD, 3);
		if (stv090x_write_reg(state, STV090x_P1_TSCFGM, reg) < 0)
			goto err;
		if (stv090x_write_reg(state, STV090x_P1_TSSPEED, speed) < 0)
			goto err;
	}

	if (state->config->ts2_clk > 0) {
		u32 speed;

		switch (state->config->ts2_mode) {
		case STV090x_TSMODE_PARALLEL_PUNCTURED:
		case STV090x_TSMODE_DVBCI:
		default:
			speed = state->mclk / (state->config->ts2_clk / 4);
			if (speed < 0x08)
				speed = 0x08;
			if (speed > 0xFF)
				speed = 0xFF;
			break;
		case STV090x_TSMODE_SERIAL_PUNCTURED:
		case STV090x_TSMODE_SERIAL_CONTINUOUS:
			speed = state->mclk / (state->config->ts2_clk / 32);
			if (speed < 0x20)
				speed = 0x20;
			if (speed > 0xFF)
				speed = 0xFF;
			break;
		}
		reg = stv090x_read_reg(state, STV090x_P2_TSCFGM);
		STV090x_SETFIELD_Px(reg, TSFIFO_MANSPEED_FIELD, 3);
		if (stv090x_write_reg(state, STV090x_P2_TSCFGM, reg) < 0)
			goto err;
		if (stv090x_write_reg(state, STV090x_P2_TSSPEED, speed) < 0)
			goto err;
	}

	reg = stv090x_read_reg(state, STV090x_P2_TSCFGH);
	STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0x01);
	if (stv090x_write_reg(state, STV090x_P2_TSCFGH, reg) < 0)
		goto err;
	STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0x00);
	if (stv090x_write_reg(state, STV090x_P2_TSCFGH, reg) < 0)
		goto err;

	reg = stv090x_read_reg(state, STV090x_P1_TSCFGH);
	STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0x01);
	if (stv090x_write_reg(state, STV090x_P1_TSCFGH, reg) < 0)
		goto err;
	STV090x_SETFIELD_Px(reg, RST_HWARE_FIELD, 0x00);
	if (stv090x_write_reg(state, STV090x_P1_TSCFGH, reg) < 0)
		goto err;

	dprintk(10, "%s <\n", __FUNCTION__);

	return 0;
err:
	printk("stv090x_set_tspath: I/O error\n");
	return -1;
}

static int stv090x_init(struct dvb_frontend *fe)
{
	struct stv090x_state *state = fe->demodulator_priv;
	const struct stv090x_config *config = state->config;
	u32 reg;

	dprintk(10, "%s >\n", __FUNCTION__);

	if (state->mclk == 0) {
		/* call tuner init to configure the tuner's clock output
		   divider directly before setting up the master clock of
		   the stv090x. */
		if (config->tuner_init) {
			if (config->tuner_init(fe) < 0)
				goto err;
		}

	    stv090x_set_mclk(state, 135000000, config->xtal); /* 135 Mhz */

		msleep(1);
		if (stv090x_write_reg(state, STV090x_SYNTCTRL,
				      0x20 | config->clk_mode) < 0)
			goto err;
		stv090x_get_mclk(state);
	}

	if (stv090x_wakeup(fe) < 0) {
		printk("Error waking device\n");
		goto err;
	}

	if (stv090x_ldpc_mode(state, state->demod_mode) < 0)
		goto err;

	reg = STV090x_READ_DEMOD(state, TNRCFG2);
	STV090x_SETFIELD_Px(reg, TUN_IQSWAP_FIELD, state->inversion);
	if (STV090x_WRITE_DEMOD(state, TNRCFG2, reg) < 0)
		goto err;

	reg = STV090x_READ_DEMOD(state, DEMOD);
	STV090x_SETFIELD_Px(reg, ROLLOFF_CONTROL_FIELD, state->rolloff);
	if (STV090x_WRITE_DEMOD(state, DEMOD, reg) < 0)
		goto err;

	if (config->tuner_set_mode) {
		if (config->tuner_set_mode(fe, TUNER_WAKE) < 0)
			goto err;
	}

	if (config->tuner_init) {
		if (config->tuner_init(fe) < 0)
			goto err;
	}

	if (stv090x_set_tspath(state) < 0)
		goto err;

	dprintk(10, "%s <\n", __FUNCTION__);
	return 0;
err:
	printk("stv090x_init: I/O error\n");
	return -1;
}

static int stv090x_setup(struct dvb_frontend *fe)
{
	struct stv090x_state *state = fe->demodulator_priv;
	const struct stv090x_config *config = state->config;
	const struct stv090x_reg *stv090x_initval = NULL;
	const struct stv090x_reg *stv090x_cut20_val = NULL;
	unsigned long t1_size = 0, t2_size = 0;
	u32 reg = 0;

	int i;

	if (state->device == STV0900) {
		dprintk(10, "Initializing STV0900\n");
		stv090x_initval = stv0900_initval;
		t1_size = ARRAY_SIZE(stv0900_initval);
		stv090x_cut20_val = stv0900_cut20_val;
		t2_size = ARRAY_SIZE(stv0900_cut20_val);
	} else if (state->device == STV0903) {
		dprintk(10, "Initializing STV0903\n");
		stv090x_initval = stv0903_initval;
		t1_size = ARRAY_SIZE(stv0903_initval);
		stv090x_cut20_val = stv0903_cut20_val;
		t2_size = ARRAY_SIZE(stv0903_cut20_val);
	} else
	if  (state->device == STX7111)
	{
		dprintk(10, "Initializing STX7111\n");
		stv090x_initval = stx7111_initval;
		t1_size = ARRAY_SIZE(stx7111_initval);

	        t2_size = 0;
	}

	/* STV090x init */

	if (STV090x_WRITE_DEMOD(state, DMDISTATE, 0x5c) < 0) /* Stop Demod */
		goto err;

	msleep(5);

	if (STV090x_WRITE_DEMOD(state, TNRCFG, 0x6c) < 0) /* check register ! (No Tuner Mode) */
		goto err;

	STV090x_SETFIELD_Px(reg, ENARPT_LEVEL_FIELD, config->repeater_level);
	STV090x_SETFIELD_Px(reg, STOP_ENABLE_FIELD, 1);

	if (STV090x_WRITE_DEMOD(state, I2CRPT, reg) < 0) /* repeater OFF */
		goto err;

	if (stv090x_write_reg(state, STV090x_NCOARSE, 0x13) < 0) /* set PLL divider */
		goto err;
	msleep(5);
	if (stv090x_write_reg(state, STV090x_I2CCFG, 0x08) < 0) /* 1/41 oversampling */
		goto err;
	if (stv090x_write_reg(state, STV090x_SYNTCTRL, 0x10 | config->clk_mode) < 0) /* enable PLL */
		goto err;

	msleep(5);

	/* write initval */
	dprintk(10, "Setting up initial values\n");
	for (i = 0; i < t1_size; i++) {
		if (stv090x_write_reg(state, stv090x_initval[i].addr, stv090x_initval[i].data) < 0)
			goto err;
	}

	state->dev_ver = stv090x_read_reg(state, STV090x_MID);
	if (state->dev_ver >= 0x20) {
		if (stv090x_write_reg(state, STV090x_TSGENERAL, 0x0c) < 0)
			goto err;

		/* write cut20_val*/
		dprintk(10, "Setting up Cut 2.0 initial values\n");
		for (i = 0; i < t2_size; i++) {
			if (stv090x_write_reg(state, stv090x_cut20_val[i].addr, stv090x_cut20_val[i].data) < 0)
				goto err;
		}

	} else if (state->dev_ver < 0x20) {
		printk("ERROR: Unsupported Cut: 0x%02x!\n",
			state->dev_ver);

		goto err;
	} else if (state->dev_ver > 0x30) {
		/* we shouldn't bail out from here */
		printk("INFO: Cut: 0x%02x probably incomplete support!\n",
			state->dev_ver);
	}

	if (stv090x_write_reg(state, STV090x_TSTRES0, 0x80) < 0)
		goto err;
	if (stv090x_write_reg(state, STV090x_TSTRES0, 0x00) < 0)
		goto err;

	if  (state->device == STX7111)
	   stv090x_set_mclk(state, 135000000, config->xtal); /* 135 Mhz */
	else
	   stv090x_set_mclk(state, 135000000, config->xtal/2); /* 135 Mhz */

	msleep(5);
#if defined(UFS912) || defined(SPARK)
	if (stv090x_write_reg(state, STV090x_SYNTCTRL, 0x10 | config->clk_mode) < 0)
		goto err;
#else
	if (stv090x_write_reg(state, STV090x_SYNTCTRL, 0x20 | config->clk_mode) < 0)
		goto err;
#endif
	stv090x_get_mclk(state);

	return 0;
err:
	printk("stv090x_setup: I/O error\n");
	return -1;
}

#if DVB_API_VERSION < 5

static struct dvbfe_info dvbs_info = {
  .name = "STV090x Multistandard",
  .delivery = DVBFE_DELSYS_DVBS,
  .delsys = {
             .dvbs.modulation = DVBFE_MOD_QPSK,
             .dvbs.fec = DVBFE_FEC_1_2 | DVBFE_FEC_2_3 |
             DVBFE_FEC_3_4 | DVBFE_FEC_4_5 |
             DVBFE_FEC_5_6 | DVBFE_FEC_6_7 | DVBFE_FEC_7_8 | DVBFE_FEC_AUTO},
  .frequency_min = 950000,
  .frequency_max = 2150000,
  .frequency_step = 0,
  .frequency_tolerance = 0,
  .symbol_rate_min = 1000000,
  .symbol_rate_max = 45000000
};

static const struct dvbfe_info dvbs2_info = {
  .name = "STV090x Multistandard",
  .delivery = DVBFE_DELSYS_DVBS2,
  .delsys = {
             .dvbs2.modulation = DVBFE_MOD_QPSK | DVBFE_MOD_8PSK,

             /* TODO: Review these */
             .dvbs2.fec = DVBFE_FEC_1_4 | DVBFE_FEC_1_3 |
             DVBFE_FEC_2_5 | DVBFE_FEC_1_2 |
             DVBFE_FEC_3_5 | DVBFE_FEC_2_3 |
             DVBFE_FEC_3_4 | DVBFE_FEC_4_5 |
             DVBFE_FEC_5_6 | DVBFE_FEC_8_9 | DVBFE_FEC_9_10,
             },

  .frequency_min = 950000,
  .frequency_max = 2150000,
  .frequency_step = 0,
  .symbol_rate_min = 1000000,
  .symbol_rate_max = 45000000,
  .symbol_rate_tolerance = 0
};

static int stv090x_get_info (struct dvb_frontend *fe, struct dvbfe_info *fe_info)
{
  switch (fe_info->delivery)
  {
  case DVBFE_DELSYS_DVBS:
    memcpy (fe_info, &dvbs_info, sizeof (dvbs_info));
    break;
  case DVBFE_DELSYS_DVBS2:
    memcpy (fe_info, &dvbs2_info, sizeof (dvbs2_info));
    break;
  default:
    printk ("%s() invalid arg\n", __FUNCTION__);
    return -EINVAL;
  }

  return 0;
}
#else
static int stv090x_get_property(struct dvb_frontend *fe, struct dtv_property* tvp)
{
	/* get delivery system info */
	if(tvp->cmd==DTV_DELIVERY_SYSTEM){
		switch (tvp->u.data) {
		case SYS_DVBS2:
		case SYS_DVBS:
		case SYS_DSS:
			break;
		default:
			return -EINVAL;
		}
	}

	dprintk(20, "%s()\n", __func__);
	return 0;
}

#endif

    int hdbox_set_voltage(struct dvb_frontend *fe, enum fe_sec_voltage voltage)
    {
       struct stv090x_state *state = fe->demodulator_priv;

       u8 res = ctrl_inb(0xa2800000);

       dprintk(10, "%s >Tuner:%d in:0x%x\n", __func__,state->tuner ,res);

       switch (voltage) {
       case SEC_VOLTAGE_OFF:
          dprintk(10, "set_voltage_off\n");
          if (state->tuner == STV090x_TUNER1)
               res |= 0x10;
          else
               res |= 0x20;
       break;

       case SEC_VOLTAGE_13: /* vertical */
          dprintk(10, "set_voltage_vertical \n");
          if (state->tuner == STV090x_TUNER1)
          {
             /* lnb power on */
             res &=~0x10;
             /* 13 V */
             res |=0x1;
          }
          else
            {
             /* lnb power on */
             res &=~0x20;
             /* 13 V */
             res |=0x2;
          }
          break;

       case SEC_VOLTAGE_18: /* horizontal */
            dprintk(10, "set_voltage_horizontal\n");
            if (state->tuner == STV090x_TUNER1)
            {
             /* lnb power on */
             res &=~0x10;
             /* 18 V */
             res &=~0x1;
          }
          else
            {
             /* lnb power on */
             res &=~0x20;
             /* 18 V */
             res &=~0x2;
          }
          break;
       default:
          break;
       }
       ctrl_outb(res, 0xa2800000);
       dprintk(10, "%s <out:0x%x\n", __func__,res);
       return 0;
    }

static int	spark_set_voltage(struct dvb_frontend *fe, enum fe_sec_voltage voltage)
{
   struct stv090x_state *state = fe->demodulator_priv;

   dprintk(10, "%s >Tuner:%d\n", __func__,state->tuner);

   switch (voltage) 
   {
      case SEC_VOLTAGE_OFF:
        dprintk(10, "set_voltage_off\n");
		stpio_set_pin(fe_lnb_on_off, 0);
        break;

      case SEC_VOLTAGE_13: /* vertical */
        dprintk(10, "set_voltage_vertical \n");
		stpio_set_pin(fe_lnb_on_off, 1);
		stpio_set_pin(fe_lnb_13_18, 1);
        break;
      case SEC_VOLTAGE_18: /* horizontal */
        dprintk(10, "set_voltage_horizontal\n");
		stpio_set_pin(fe_lnb_on_off, 1);
		stpio_set_pin(fe_lnb_13_18, 0);
      break;
      default:
	 break;
   }

   dprintk(10, "%s <\n", __func__);
   return 0;
}

static struct dvb_frontend_ops stv090x_ops = {

	.info = {
		.name			= "STV090x Multistandard",
		.type			= FE_QPSK,
		.frequency_min		= 950000,
		.frequency_max 		= 2150000,
		.frequency_stepsize	= 0,
		.frequency_tolerance	= 0,
		.symbol_rate_min 	= 1000000,
		.symbol_rate_max 	= 70000000,
		.caps			= FE_CAN_INVERSION_AUTO |
					  FE_CAN_FEC_AUTO       |
					  FE_CAN_QPSK           /*|
					  FE_CAN_2G_MODULATION*/
	},

	.release			= stv090x_release,
	.init				= stv090x_init,

//workaround for tuner failed, a frontend open does not allways wakeup the tuner
#ifndef FORTIS_HDBOX
	.sleep				= stv090x_sleep,
#endif
	.get_frontend_algo		= stv090x_frontend_algo,

	.i2c_gate_ctrl			= stv090x_i2c_gate_ctrl,

	.diseqc_send_master_cmd		= stv090x_send_diseqc_msg,
	.diseqc_send_burst		= stv090x_send_diseqc_burst,
	.diseqc_recv_slave_reply	= stv090x_recv_slave_reply,
	.set_tone			= stv090x_set_tone,

	.search				= stv090x_search,
	.read_status			= stv090x_read_status,
	.read_ber			= stv090x_read_per,
	.read_signal_strength		= stv090x_read_signal_strength,
	.read_snr			= stv090x_read_cnr,
#if DVB_API_VERSION < 5
	.get_info                       = stv090x_get_info,
#else
	.get_property = stv090x_get_property,
#endif

	.set_voltage			= spark_set_voltage,
};


struct dvb_frontend *stv090x_attach(const struct stv090x_config *config,
				    struct i2c_adapter *i2c,
				    enum stv090x_demodulator demod,
			            enum stv090x_tuner tuner)
{
	struct stv090x_state *state = NULL;

	state = kzalloc(sizeof (struct stv090x_state), GFP_KERNEL);
	if (state == NULL)
		goto error;

	state->verbose				= &verbose;
	state->config				= config;
	state->i2c				= i2c;
	state->frontend.ops			= stv090x_ops;
	state->frontend.demodulator_priv	= state;
	state->demod				= demod;
	state->demod_mode 			= config->demod_mode; /* Single or Dual mode */
	state->device				= config->device;
	state->rolloff				= STV090x_RO_35; /* default */
	state->tuner				= tuner;

	dprintk(10, "i2c adapter = %p\n", state->i2c);

	printk("[stv090x_attach]i2c adapter = %p\n i2c addr=%d\n", state->i2c,config->address);

#if defined(SPARK)
	fe_lnb_13_18 = stpio_request_pin(6, 6, "lnb 13/18", STPIO_OUT);
	fe_lnb_14_19 = stpio_request_pin(5, 5, "lnb 14/19", STPIO_OUT);
	fe_lnb_on_off= stpio_request_pin(6, 5, "lnb onoff", STPIO_OUT);
	if(!fe_lnb_13_18 || !fe_lnb_14_19 || !fe_lnb_on_off)
	{
		printk("lnb request pin failed\n");
		if(fe_lnb_13_18)
		{
			stpio_free_pin(fe_lnb_13_18);
		}
		if(fe_lnb_14_19)
		{
			stpio_free_pin(fe_lnb_14_19);
		}
		if(fe_lnb_on_off)
		{
			stpio_free_pin(fe_lnb_on_off);
		}
		return -1;
	}
#endif
	
	mutex_init(&demod_lock);

	if (stv090x_sleep(&state->frontend) < 0) {
		printk("Error putting device to sleep\n");
		goto error;
	}

	if (stv090x_setup(&state->frontend) < 0) {
		printk("Error setting up device\n");
		goto error;
	}
	if (stv090x_wakeup(&state->frontend) < 0) {
		printk("Error waking device\n");
		goto error;
	}

	dprintk(10, "Attaching %s demodulator(%d) Cut=0x%02x\n",
	       state->device == STV0900 ? "STV0900" : "STV0903",
	       demod,
	       state->dev_ver);

	return &state->frontend;

error:
	kfree(state);
	return NULL;
}
EXPORT_SYMBOL(stv090x_attach);

