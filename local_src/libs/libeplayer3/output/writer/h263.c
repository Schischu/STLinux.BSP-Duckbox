/*
 * linuxdvb output/writer handling.
 *
 * crow 2010
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/* ***************************** */
/* Includes                      */
/* ***************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <linux/dvb/video.h>
#include <linux/dvb/audio.h>
#include <memory.h>
#include <asm/types.h>
#include <pthread.h>
#include <errno.h>

#include "common.h"
#include "output.h"
#include "debug.h"
#include "stm_ioctls.h"
#include "misc.h"
#include "pes.h"
#include "writer.h"

/* ***************************** */
/* Makros/Constants              */
/* ***************************** */
#define H263_DEBUG

#ifdef H263_DEBUG

static short debug_level = 0;

#define h263_printf(level, fmt, x...) do { \
if (debug_level >= level) printf("[%s:%s] " fmt, __FILE__, __FUNCTION__, ## x); } while (0)
#else
#define h263_printf(level, fmt, x...)
#endif

#ifndef H263_SILENT
#define h263_err(fmt, x...) do { printf("[%s:%s] " fmt, __FILE__, __FUNCTION__, ## x); } while (0)
#else
#define h263_err(fmt, x...)
#endif
/* ***************************** */
/* Types                         */
/* ***************************** */

/* ***************************** */
/* Varaibles                     */
/* ***************************** */

/* ***************************** */
/* Prototypes                    */
/* ***************************** */

/* ***************************** */
/* MISC Functions                */
/* ***************************** */

static int reset()
{
    return 0;
}

static int writeData(void* _call)
{
    WriterAVCallData_t* call = (WriterAVCallData_t*) _call;

    unsigned char PesHeader[PES_MAX_HEADER_SIZE];

    h263_printf(10, "\n");

    if (call == NULL)
    {
        h263_err("call data is NULL...\n");
        return 0;
    }

    h263_printf(10, "VideoPts %lld\n", call->Pts);

    if ((call->data == NULL) || (call->len <= 0))
    {
        h263_err("NULL Data. ignoring...\n");
        return 0;
    }

    if (call->fd < 0)
    {
        h263_err("file pointer < 0. ignoring ...\n");
        return 0;
    }

    int HeaderLength = InsertPesHeader(PesHeader, call->len, H263_VIDEO_PES_START_CODE, call->Pts,0);

    int PrivateHeaderLength = InsertVideoPrivateDataHeader (&PesHeader[HeaderLength], call->len);

    int PesLength = PesHeader[PES_LENGTH_BYTE_0] + (PesHeader[PES_LENGTH_BYTE_1] << 8) + PrivateHeaderLength;

    PesHeader[PES_LENGTH_BYTE_0]            = PesLength & 0xff;
    PesHeader[PES_LENGTH_BYTE_1]            = (PesLength >> 8) & 0xff;
    PesHeader[PES_HEADER_DATA_LENGTH_BYTE] += PrivateHeaderLength;
    PesHeader[PES_FLAGS_BYTE]              |= PES_EXTENSION_DATA_PRESENT;

    HeaderLength                           += PrivateHeaderLength;

    int iovcnt = 0;
    struct iovec iov[2];
    iov[iovcnt].iov_base = PesHeader;
    iov[iovcnt].iov_len  = HeaderLength;
    iovcnt++;
    iov[iovcnt].iov_base = call->data;
    iov[iovcnt].iov_len  = call->len;
    iovcnt++;

    int len = writev(call->fd, iov, iovcnt);
    h263_printf(10, "< len %d\n", len);
    return len;
}

/* ***************************** */
/* Writer  Definition            */
/* ***************************** */

static WriterCaps_t caps_h263 = {
    "h263",
    eVideo,
    "V_H263",
    VIDEO_ENCODING_H263
};

struct Writer_s WriterVideoH263 = {
    &reset,
    &writeData,
    NULL,
    &caps_h263
};

static WriterCaps_t caps_flv = {
    "FLV",
    eVideo,
    "V_FLV",
    VIDEO_ENCODING_FLV1
};

struct Writer_s WriterVideoFLV = {
    &reset,
    &writeData,
    NULL,
    &caps_flv
};
