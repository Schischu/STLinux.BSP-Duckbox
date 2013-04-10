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

#ifndef __GST_DVBVIDEOSINK_H__
#define __GST_DVBVIDEOSINK_H__

#define PACK_UNPACKED_XVID_DIVX5_BITSTREAM

#include <gst/gst.h>
#include <gst/base/gstbasesink.h>

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_DVBVIDEOSINK \
  (gst_dvbvideosink_get_type())
#define GST_DVBVIDEOSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_DVBVIDEOSINK,GstDVBVideoSink))
#define GST_DVBVIDEOSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_DVBVIDEOSINK,GstDVBVideoSinkClass))
#define GST_IS_DVBVIDEOSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_DVBVIDEOSINK))
#define GST_IS_DVBVIDEOSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_DVBVIDEOSINK))

typedef struct _GstDVBVideoSink		GstDVBVideoSink;
typedef struct _GstDVBVideoSinkClass	GstDVBVideoSinkClass;
typedef struct _GstDVBVideoSinkPrivate	GstDVBVideoSinkPrivate;

typedef struct queue_entry
{
	struct queue_entry *next;
	size_t bytes;
	size_t offset;
} queue_entry_t;

struct _GstDVBVideoSink
{
	GstBaseSink element;
	gboolean use_set_encoding;

	gint control_sock[2];

	gint must_send_header;

	gboolean initial_header;
	guint8  *initial_header_private_data;
	guint32  initial_header_private_data_size;

	guint8  *runtime_header_data;
	guint32  runtime_header_data_size;

	GstBuffer *h264_buffer;
	gint h264_nal_len_size;

	GstBuffer *codec_data;

#ifdef PACK_UNPACKED_XVID_DIVX5_BITSTREAM
	/* data needed to pack bitstream (divx5 / xvid) */
	gint num_non_keyframes, must_pack_bitstream, time_inc_bits, time_inc;
	GstBuffer *prev_frame;
#endif

	char saved_fallback_framerate[16];

	int fd;
	gboolean dec_running;

	int streamtype;

	int no_write;

	queue_entry_t *queue;
};

struct _GstDVBVideoSinkClass 
{
  GstBaseSinkClass parent_class;
  gint64 (*get_decoder_time) (GstDVBVideoSink *sink);
};

GType gst_dvbvideosink_get_type (void);

G_END_DECLS

#endif /* __GST_DVBVIDEOSINK_H__ */
