/*
 * GStreamer DVB Media Sink
 * Copyright 2006 Felix Domke <tmbinc@elitedvb.net>
 * based on code by:
 * Copyright 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GST_DVBAUDIOSINK_H__
#define __GST_DVBAUDIOSINK_H__

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_DVBAUDIOSINK \
  (gst_dvbaudiosink_get_type())
#define GST_DVBAUDIOSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DVBAUDIOSINK,GstDVBAudioSink))
#define GST_DVBAUDIOSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DVBAUDIOSINK,GstDVBAudioSinkClass))
#define GST_DVBAUDIOSINK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),GST_TYPE_DVBAUDIOSINK,GstDVBAudioSinkClass))
#define GST_IS_DVBAUDIOSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DVBAUDIOSINK))
#define GST_IS_DVBAUDIOSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DVBAUDIOSINK))

typedef struct _GstDVBAudioSink		GstDVBAudioSink;
typedef struct _GstDVBAudioSinkClass	GstDVBAudioSinkClass;
typedef struct _GstDVBAudioSinkPrivate	GstDVBAudioSinkPrivate;

typedef struct queue_entry
{
	struct queue_entry *next;
	size_t bytes;
	size_t offset;
} queue_entry_t;

struct _GstDVBAudioSink
{
	GstBaseSink element;
	gboolean use_set_encoding;

	gboolean initial_header;
	guint8  *initial_header_private_data;
	guint32  initial_header_private_data_size;
	gboolean initial_header_private_data_valid;

	guint8  *runtime_header_data;
	guint32  runtime_header_data_size;

	guint8   aac_adts_header[7];
	gboolean aac_adts_header_valid;

	guint16 pcm_bits_per_sample;
	guint32 pcm_sub_frame_len;
	guint32 pcm_sub_frame_per_pes;
	guint8  pcm_break_buffer[8192];
	guint32 pcm_break_buffer_size;

	gint control_sock[2];

	gchar *dump_filename;
	int fd;
	int dump_fd;

	int skip;
	int bypass;

	int no_write;

	queue_entry_t *queue;

	unsigned long long timestamp;
};

struct _GstDVBAudioSinkClass
{
	GstBaseSinkClass parent_class;
	gint64 (*get_decoder_time) (GstDVBAudioSink *sink);
	int (*async_write) (GstDVBAudioSink *sink, unsigned char *data, unsigned int size);
};

GType gst_dvbaudiosink_get_type (void);

G_END_DECLS

#endif /* __GST_DVBAUDIOSINK_H__ */
