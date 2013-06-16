/*
 * linuxdvb output/writer handling.
 *
 * konfetti 2010 based on linuxdvb.c code from libeplayer2
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
#define MP3_DEBUG

#ifdef MP3_DEBUG

static short debug_level = 0;

#define mp3_printf(level, fmt, x...) do { \
if (debug_level >= level) printf("[%s:%s] " fmt, __FILE__, __FUNCTION__, ## x); } while (0)
#else
#define mp3_printf(level, fmt, x...)
#endif

#ifndef MP3_SILENT
#define mp3_err(fmt, x...) do { printf("[%s:%s] " fmt, __FILE__, __FUNCTION__, ## x); } while (0)
#else
#define mp3_err(fmt, x...)
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

    unsigned char  PesHeader[PES_MAX_HEADER_SIZE];

    mp3_printf(10, "\n");

    if (call == NULL)
    {
        mp3_err("call data is NULL...\n");
        return 0;
    }

    mp3_printf(10, "AudioPts %lld\n", call->Pts);

    if ((call->data == NULL) || (call->len <= 0))
    {
        mp3_err("parsing NULL Data. ignoring...\n");
        return 0;
    }

    if (call->fd < 0)
    {
        mp3_err("file pointer < 0. ignoring ...\n");
        return 0;
    }

    int HeaderLength = InsertPesHeader (PesHeader, call->len , MPEG_AUDIO_PES_START_CODE, call->Pts, 0);

    int iovcnt = 0;
    struct iovec iov[2];
    iov[iovcnt].iov_base = PesHeader;
    iov[iovcnt].iov_len  = HeaderLength;
    iovcnt++;
    iov[iovcnt].iov_base = call->data;
    iov[iovcnt].iov_len  = call->len;
    iovcnt++;

    int len = writev(call->fd, iov, iovcnt);

    mp3_printf(10, "mp3_Write-< len=%d\n", len);
    return len;
}

/* ***************************** */
/* Writer  Definition            */
/* ***************************** */

static WriterCaps_t caps_mp3 = {
    "mp3",
    eAudio,
    "A_MP3",
    AUDIO_ENCODING_MP3
};

struct Writer_s WriterAudioMP3 = {
    &reset,
    &writeData,
    NULL,
    &caps_mp3
};

static WriterCaps_t caps_mpegl3 = {
    "mpeg/l3",
    eAudio,
    "A_MPEG/L3",
    AUDIO_ENCODING_MPEG2
};

struct Writer_s WriterAudioMPEGL3 = {
    &reset,
    &writeData,
    NULL,
    &caps_mpegl3
};
