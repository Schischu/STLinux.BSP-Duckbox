/*
 * @brief ufs912_platform.c
 *
 * @author konfetti
 *
 * 	Copyright (C) 2013 duckbox
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/version.h>
#include <linux/dvb/version.h>
#include <linux/dvb/frontend.h>

#include "stv090x_platform.h"

#include "socket.h"
#include "tuner.h"
#include "lnb.h"

#ifndef _ufs912_123
#define _ufs912_123

struct stv090x_private_data_s stv090x_tuner_priv = {
    .usedLNB              = cLNB_LNBH221,
    .usedTuner            = cTUNER_EXT_STV6110X,  
    .alternativePath      = cNONE,                
    .shouldSleep          = 1,
    .device               = STX7111,
    .demod                = STV090x_DEMODULATOR_0,
    .demod_mode           = STV090x_DUAL,
    .clk_mode             = STV090x_CLK_EXT,
    .xtal                 = 30000000,
    .ref_clk              = 16000000,
    .ts1_mode             = STV090x_TSMODE_DVBCI,
    .ts2_mode             = STV090x_TSMODE_SERIAL_CONTINUOUS,
    .ts1_clk              = 0,
    .ts2_clk              = 0,
    .repeater_level       = STV090x_RPTLEVEL_64,
    .tuner_bbgain         = 10,                        
    .adc1_range           = STV090x_ADC_1Vpp,
    .adc2_range           = STV090x_ADC_2Vpp,
    .diseqc_envelope_mode = false,
    .tuner_refclk         = 16000000,
};

struct platform_frontend_config_s stv090x_frontend = {
            .name               = "stv090x",
            .demod_i2c          = 0x68,
            .tuner_i2c          = 0x60,
            .private            = &stv090x_tuner_priv,
};

struct tunersocket_s ufs912_socket = {
    .numSockets = 1,
    .socketList = (struct socket_s[]) {
        [0] = {
            .name               = "socket-1",
            .tuner_enable       = {2, 4, 1},
	    .lnb                = {0, 0x0a, 0x08, 0xd0, 0xd4, 0xdc},
            .i2c_bus            = 3,
        },
    },
};

struct platform_device stv090x_frontend_device = {
    .name    = "stv090x",
    .id      = -1,
    .dev     = {
        .platform_data = &stv090x_frontend,
    },
    .num_resources        = 0,
    .resource             = NULL,
};

struct platform_device ufs912_socket_device = {
    .name    = "socket",
    .id      = -1,
    .dev     = {
        .platform_data = &ufs912_socket,
    },
    .num_resources        = 0,
    .resource             = NULL,
};

struct platform_device *platform[] __initdata = {
    &stv090x_frontend_device,
    &ufs912_socket_device,
};

#endif
