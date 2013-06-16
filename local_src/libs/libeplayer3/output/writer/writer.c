/*
 * linuxdvb output/writer handling.
 *
 * konfetti 2010
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

#include <stdlib.h>
#include <string.h>

#include "writer.h"

/* ***************************** */
/* Makros/Constants              */
/* ***************************** */

#define WRITER_DEBUG

#ifdef WRITER_DEBUG

static short debug_level = 0;

#define writer_printf(level, x...) do { \
if (debug_level >= level) printf(x); } while (0)
#else
#define writer_printf(level, x...)
#endif

#ifndef WRITER_SILENT
#define writer_err(x...) do { printf(x); } while (0)
#else
#define writer_err(x...)
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
/*  Functions                    */
/* ***************************** */

Writer_t* getWriter(char* encoding)
{
    int i;

    for (i = 0; AvailableWriter[i] != NULL; i++)
    {
        if (strcmp(AvailableWriter[i]->caps->textEncoding, encoding) == 0)
        {
            writer_printf(50, "%s: found writer \"%s\" for \"%s\"\n", __func__, AvailableWriter[i]->caps->name, encoding);
            return AvailableWriter[i];
        }
    }

    writer_printf(1, "%s: no writer found for \"%s\"\n", __func__, encoding);

    return NULL;
}

Writer_t* getDefaultVideoWriter()
{
    int i;

    for (i = 0; AvailableWriter[i] != NULL; i++)
    {
        if (strcmp(AvailableWriter[i]->caps->textEncoding, "V_MPEG2") == 0)
        {
            writer_printf(50, "%s: found writer \"%s\"\n", __func__, AvailableWriter[i]->caps->name);
            return AvailableWriter[i];
        }
    }

    writer_printf(1, "%s: no writer found\n", __func__);

    return NULL;
}

Writer_t* getDefaultAudioWriter()
{
    int i;

    for (i = 0; AvailableWriter[i] != NULL; i++)
    {
        if (strcmp(AvailableWriter[i]->caps->textEncoding, "A_MP3") == 0)
        {
            writer_printf(50, "%s: found writer \"%s\"\n", __func__, AvailableWriter[i]->caps->name);
            return AvailableWriter[i];
        }
    }

    writer_printf(1, "%s: no writer found\n", __func__);

    return NULL;
}

Writer_t* getDefaultFramebufferWriter()
{
    int i;

    for (i = 0; AvailableWriter[i] != NULL; i++)
    {
        writer_printf(10, "%s\n", AvailableWriter[i]->caps->textEncoding);
        if (strcmp(AvailableWriter[i]->caps->textEncoding, "framebuffer") == 0)
        {
            writer_printf(50, "%s: found writer \"%s\"\n", __func__, AvailableWriter[i]->caps->name);
            return AvailableWriter[i];
        }
    }

    writer_printf(1, "%s: no writer found\n", __func__);

    return NULL;
}
