
/* GStreamer DVB Media Sink
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-plugin
 *
 * <refsect2>
 * <title>Example launch line</title>
 * <para>
 * <programlisting>
 * gst-launch -v -m audiotestsrc ! plugin ! fakesink silent=TRUE
 * </programlisting>
 * </para>
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/dvb/video.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>

#ifdef __sh__
#include <linux/dvb/stm_ioctls.h>
#endif

#include <gst/gst.h>

#include "gstdvbvideosink.h"
#include "gstdvbsink-marshal.h"

#ifndef VIDEO_GET_PTS
#define VIDEO_GET_PTS              _IOR('o', 57, gint64)
#endif

/* We add a control socket as in fdsrc to make it shutdown quickly when it's blocking on the fd.
 * Poll is used to determine when the fd is ready for use. When the element state is changed,
 * it happens from another thread while fdsink is poll'ing on the fd. The state-change thread 
 * sends a control message, so fdsink wakes up and changes state immediately otherwise
 * it would stay blocked until it receives some data. */

/* the poll call is also performed on the control sockets, that way
 * we can send special commands to unblock the poll call */
#define CONTROL_STOP		'S'			/* stop the poll call */
#define CONTROL_SOCKETS(sink)	sink->control_sock
#define WRITE_SOCKET(sink)	sink->control_sock[1]
#define READ_SOCKET(sink)	sink->control_sock[0]

#define SEND_COMMAND(sink, command)    \
G_STMT_START {                         \
	unsigned char c; c = command;      \
	write (WRITE_SOCKET(sink), &c, 1); \
} G_STMT_END

#define READ_COMMAND(sink, command, res)        \
G_STMT_START {                                  \
	res = read(READ_SOCKET(sink), &command, 1); \
} G_STMT_END

GST_DEBUG_CATEGORY_STATIC (dvbvideosink_debug);
#define GST_CAT_DEFAULT dvbvideosink_debug

#define COMMON_VIDEO_CAPS         \
  "width = (int) [ 16, 4096 ], "  \
  "height = (int) [ 16, 4096 ], " \
  "framerate = (fraction) [ 0, MAX ]"

#define MPEG4V2_LIMITED_CAPS     \
  "width = (int) [ 16, 800 ], "  \
  "height = (int) [ 16, 600 ], " \
  "framerate = (fraction) [ 0, MAX ]"

enum
{
	SIGNAL_GET_DECODER_TIME,
	LAST_SIGNAL
};

static guint gst_dvb_videosink_signals[LAST_SIGNAL] = { 0 };

static GstStaticPadTemplate sink_factory_bcm7400 =
GST_STATIC_PAD_TEMPLATE (
	"sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS (
		"video/mpeg, "
		"mpegversion = (int) { 1, 2, 4 }, "
		"systemstream = (boolean) false, "
	COMMON_VIDEO_CAPS "; "
		"video/x-h264, "
	COMMON_VIDEO_CAPS "; "
		"video/x-h263, "
	COMMON_VIDEO_CAPS "; "
		"video/x-msmpeg, "
	MPEG4V2_LIMITED_CAPS ", mspegversion = (int) 43; "
		"video/x-divx, "
	MPEG4V2_LIMITED_CAPS ", divxversion = (int) [ 3, 5 ]; "
		"video/x-xvid, "
	MPEG4V2_LIMITED_CAPS "; "
		"video/x-3ivx, "
	MPEG4V2_LIMITED_CAPS "; ")
);

static GstStaticPadTemplate sink_factory_bcm7405 =
GST_STATIC_PAD_TEMPLATE (
	"sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS (
		"video/mpeg, "
		"mpegversion = (int) { 1, 2, 4 }, "
		"systemstream = (boolean) false, "
	COMMON_VIDEO_CAPS "; "
		"video/x-msmpeg, "
	COMMON_VIDEO_CAPS ", mspegversion = (int) 43; "
		"video/x-h264, "
	COMMON_VIDEO_CAPS "; "
		"video/x-h263, "
	COMMON_VIDEO_CAPS "; "
		"video/x-divx, "
	COMMON_VIDEO_CAPS ", divxversion = (int) [ 3, 5 ]; "
		"video/x-xvid, "
	COMMON_VIDEO_CAPS "; "
		"video/x-3ivx, "
	COMMON_VIDEO_CAPS "; ")
);


static GstStaticPadTemplate sink_factory_bcm7401 =
GST_STATIC_PAD_TEMPLATE (
	"sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS (
		"video/mpeg, "
		"mpegversion = (int) { 1, 2, 4 }, "
		"systemstream = (boolean) false, "
	COMMON_VIDEO_CAPS "; "
		"video/x-h264, "
	COMMON_VIDEO_CAPS "; "
		"video/x-h263, "
	COMMON_VIDEO_CAPS "; ")
);

static GstStaticPadTemplate sink_factory_ati_xilleon =
GST_STATIC_PAD_TEMPLATE (
	"sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS (
		"video/mpeg, "
		"mpegversion = (int) { 1, 2 }, "
		"systemstream = (boolean) false, "
	COMMON_VIDEO_CAPS "; ")
);

#define SINK_FACTORY_STM_BASE \
GST_STATIC_PAD_TEMPLATE ( \
	"sink", \
	GST_PAD_SINK, \
	GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ( \
		"  video/mpeg, " \
		"  mpegversion = (int) { 1, 2, 4 }, " \
		"  systemstream = (boolean) false; " \
		\
		"video/x-h263; " \
		"video/x-h264; " \
		\
		"video/x-divx, " \
		"  divxversion = (int) { 3, 4, 5 }; " \
		"video/x-xvid; " \
		"video/x-3ivx; ") \
)

#define SINK_FACTORY_STM_BASE_EXTENDED \
GST_STATIC_PAD_TEMPLATE ( \
	"sink", \
	GST_PAD_SINK, \
	GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ( \
		"  video/mpeg, " \
		"  mpegversion = (int) { 1, 2, 4 }, " \
		"  systemstream = (boolean) false; " \
		\
		"video/x-h263; " \
		"video/x-h264; " \
		\
		"video/x-divx, " \
		"  divxversion = (int) { 3, 4, 5 }; " \
		"video/x-xvid; " \
		"video/x-3ivx; " \
		\
		"video/x-wmv, " \
		"  wmvversion = (int) {1, 2, 3}; " \
		\
		"video/x-wmv, " \
		"  wmvversion = (int) 3, " \
		"  format = (fourcc) WVC1; ") \
)

//TODO: All but 7100 and 7101 have wmv and vc1 capability. Need to add it
// FIRST GENERATION
static GstStaticPadTemplate sink_factory_stm_stx7100 = SINK_FACTORY_STM_BASE;
static GstStaticPadTemplate sink_factory_stm_stx7101 = SINK_FACTORY_STM_BASE;
static GstStaticPadTemplate sink_factory_stm_stx7109 = SINK_FACTORY_STM_BASE_EXTENDED;

// SECOND GENERATION
static GstStaticPadTemplate sink_factory_stm_stx7105 = SINK_FACTORY_STM_BASE_EXTENDED;
static GstStaticPadTemplate sink_factory_stm_stx7111 = SINK_FACTORY_STM_BASE_EXTENDED;

// THIRD GENERATION
static GstStaticPadTemplate sink_factory_stm_stx7106 = SINK_FACTORY_STM_BASE_EXTENDED;
static GstStaticPadTemplate sink_factory_stm_stx7108 = SINK_FACTORY_STM_BASE_EXTENDED;

#define DEBUG_INIT(bla) \
	GST_DEBUG_CATEGORY_INIT (dvbvideosink_debug, "dvbvideosink", 0, "dvbvideosink element");

GST_BOILERPLATE_FULL (GstDVBVideoSink, gst_dvbvideosink, GstBaseSink,
	GST_TYPE_BASE_SINK, DEBUG_INIT);

static gboolean             gst_dvbvideosink_start (GstBaseSink * sink);
static gboolean             gst_dvbvideosink_stop (GstBaseSink * sink);
static gboolean             gst_dvbvideosink_event (GstBaseSink * sink, GstEvent * event);
static GstFlowReturn        gst_dvbvideosink_render (GstBaseSink * sink, GstBuffer * buffer);
static gboolean             gst_dvbvideosink_set_caps (GstBaseSink * sink, GstCaps * caps);
static gboolean             gst_dvbvideosink_unlock (GstBaseSink * basesink);
static gboolean             gst_dvbvideosink_unlock_stop (GstBaseSink * basesink);
static void                 gst_dvbvideosink_dispose (GObject * object);
static GstStateChangeReturn gst_dvbvideosink_change_state (GstElement * element, GstStateChange transition);
static gint64               gst_dvbvideosink_get_decoder_time (GstDVBVideoSink *self);

#define PES_MAX_HEADER_SIZE 64

typedef enum {  HW_UNKNOWN, 
				DM7025, DM800, DM8000, DM500HD, DM800SE, DM7020HD, 
				STX7100, STX7101, STX7109, STX7105, STX7111, STX7106, STX7108 
} hardware_type_t;

typedef enum { PF_UNKNOWN, DM, HAVANA } platform_type_t;

static hardware_type_t hwtype = HW_UNKNOWN;
static platform_type_t pftype = PF_UNKNOWN;

//Please correct if wrong
#define STREAMTYPE_UNKNOWN  0xFF
#define STREAMTYPE_MPEG2    0x00
#define STREAMTYPE_H264     0x01
#define STREAMTYPE_H263     0x02
#define STREAMTYPE_MPEG4_P2 0x04
#define STREAMTYPE_MPEG1    0x06
#define STREAMTYPE_XVID     0x0A
#define STREAMTYPE_DIVX311  0x0C
#define STREAMTYPE_DIVX4    0x0E
#define STREAMTYPE_DIVX5    0x0F
#define STREAMTYPE_FLV1     0x10
#define STREAMTYPE_WMV      0x11
#define STREAMTYPE_VC1      0x12

#define VIDEO_ENCODING_UNKNOWN  0xFF

unsigned int streamtype_to_encoding (unsigned int streamtype)
{
#ifdef VIDEO_SET_ENCODING
	switch(streamtype)
	{
	case STREAMTYPE_MPEG2:
		return VIDEO_ENCODING_AUTO;
	case STREAMTYPE_H264:
		return VIDEO_ENCODING_H264;
	case STREAMTYPE_H263:
		return VIDEO_ENCODING_H263;
	case STREAMTYPE_MPEG4_P2:
		return VIDEO_ENCODING_MPEG4P2;
	case STREAMTYPE_MPEG1:
		return VIDEO_ENCODING_AUTO;
	case STREAMTYPE_XVID:
		return VIDEO_ENCODING_MPEG4P2;
	case STREAMTYPE_DIVX311:
		return VIDEO_ENCODING_MPEG4P2;
	case STREAMTYPE_DIVX4:
		return VIDEO_ENCODING_MPEG4P2;
	case STREAMTYPE_DIVX5:
		return VIDEO_ENCODING_MPEG4P2;
	case STREAMTYPE_FLV1:
		return VIDEO_ENCODING_FLV1;
	case STREAMTYPE_WMV:
		return VIDEO_ENCODING_WMV;
	case STREAMTYPE_VC1:
		return VIDEO_ENCODING_VC1;

	default:
		return VIDEO_ENCODING_UNKNOWN;
	}
#endif
	return VIDEO_ENCODING_UNKNOWN;
}

static void
gst_dvbvideosink_base_init (gpointer klass)
{
	static GstElementDetails element_details = {
		"A DVB video sink",
		"Generic/DVBVideoSink",
		"Output video PES / ES into a DVB video device for hardware playback",
		"Felix Domke <tmbinc@elitedvb.net>"
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	int fd = open("/proc/stb/info/model", O_RDONLY);
	if ( fd > 0 )
	{
		gchar string[9] = { 0, };
		ssize_t rd = read(fd, string, 8);
		if ( rd >= 5 )
		{
			if ( !strncasecmp(string, "DM7025", 6) ) {
				hwtype = DM7025;
				pftype = DM;
				GST_INFO ("model is DM7025... set ati xilleon caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_ati_xilleon));
			} else if ( !strncasecmp(string, "DM500HD", 7) ) {
				hwtype = DM500HD;
				pftype = DM;
				GST_INFO ("model is DM500HD... set bcm7405 caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_bcm7405));
			} else if ( !strncasecmp(string, "DM800SE", 7) ) {
				hwtype = DM800SE;
				pftype = DM;
				GST_INFO ("model is DM800SE... set bcm7405 caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_bcm7405));
			} else if ( !strncasecmp(string, "DM7020HD", 8) ) {
				hwtype = DM7020HD;
				pftype = DM;
				GST_INFO ("model is DM7020HD... set bcm7405 caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_bcm7405));
			} else if ( !strncasecmp(string, "DM8000", 6) ) {
				hwtype = DM8000;
				pftype = DM;
				GST_INFO ("model is DM8000... set bcm7400 caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_bcm7400));
			} else if ( !strncasecmp(string, "DM800", 5) ) {
				hwtype = DM800;
				pftype = DM;
				GST_INFO ("model is DM800 set bcm7401 caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_bcm7401));
			}
		}
		close(fd);
	}

	if (hwtype == HW_UNKNOWN) {
		// Unfortunately we dont have sysinfo available so doing it the hard way
		char line[256];
		char *processor;
		FILE *file = fopen("/proc/cpuinfo", "r");
		while (fgets(line, sizeof(line) - 1, file) != NULL)
		{
			if (!strncmp(line, "cpu type", 8))
			{
				strtok (line,":");
				processor = strtok (NULL,":");
				while(processor[0] == ' ') processor++;
				break;
			}
		}
		fclose(file);
		
		printf("Processor: %s\n", processor);

		// FIRST GENERATION
		if( !strncasecmp(processor, "STX7100", 7) || 
			!strncasecmp(processor, "STB7100", 7) || 
			!strncasecmp(processor, "STI7100", 7)) {
			pftype = HAVANA;
			hwtype = STX7100;
			GST_INFO ("setting STX7100 caps");
			gst_element_class_add_pad_template (element_class,
				gst_static_pad_template_get (&sink_factory_stm_stx7100));
		}
		else if(!strncasecmp(processor, "STX7101", 7)) {
			pftype = HAVANA;
			hwtype = STX7101;
			GST_INFO ("setting STX7101 caps");
			gst_element_class_add_pad_template (element_class,
				gst_static_pad_template_get (&sink_factory_stm_stx7101));
		}
		else if(!strncasecmp(processor, "STX7109", 7)) {
			pftype = HAVANA;
			hwtype = STX7109;
			GST_INFO ("setting STX7109 caps");
			gst_element_class_add_pad_template (element_class,
				gst_static_pad_template_get (&sink_factory_stm_stx7109));
		}
		// SECOND GENERATIONad_template_get (&sink_factory_stm_stx7111));
		else if(!strncasecmp(processor, "STX7105", 7)) {
			pftype = HAVANA;
			hwtype = STX7105;
			GST_INFO ("setting STX7105 caps");
			gst_element_class_add_pad_template (element_class,
				gst_static_pad_template_get (&sink_factory_stm_stx7105));
		}
		else if(!strncasecmp(processor, "STX7111", 7)) {
			pftype = HAVANA;
			hwtype = STX7111;
			GST_INFO ("setting STX7111 caps");
			gst_element_class_add_pad_template (element_class,
				gst_static_pad_template_get (&sink_factory_stm_stx7111));
		}
		// THIRD GENERATION
		else if(!strncasecmp(processor, "STX7106", 7)) {
			pftype = HAVANA;
			hwtype = STX7106;
			GST_INFO ("setting STX7106 caps");
			gst_element_class_add_pad_template (element_class,
				gst_static_pad_template_get (&sink_factory_stm_stx7106));
		}
		else if(!strncasecmp(processor, "STX7108", 7)) {
			pftype = HAVANA;
			hwtype = STX7108;
			GST_INFO ("setting STX7108 caps");
			gst_element_class_add_pad_template (element_class,
				gst_static_pad_template_get (&sink_factory_stm_stx7108));
		}
	}

	gst_element_class_set_details (element_class, &element_details);
}

/* initialize the plugin's class */
static void
gst_dvbvideosink_class_init (GstDVBVideoSinkClass *klass)
{
	GObjectClass     *gobject_class = G_OBJECT_CLASS (klass);
	GstBaseSinkClass *gstbasesink_class = GST_BASE_SINK_CLASS (klass);
	GstElementClass  *element_class = GST_ELEMENT_CLASS (klass);

	gobject_class->dispose         = GST_DEBUG_FUNCPTR (gst_dvbvideosink_dispose);

	gstbasesink_class->start       = GST_DEBUG_FUNCPTR (gst_dvbvideosink_start);
	gstbasesink_class->stop        = GST_DEBUG_FUNCPTR (gst_dvbvideosink_stop);
	gstbasesink_class->render      = GST_DEBUG_FUNCPTR (gst_dvbvideosink_render);
	gstbasesink_class->event       = GST_DEBUG_FUNCPTR (gst_dvbvideosink_event);
	gstbasesink_class->unlock      = GST_DEBUG_FUNCPTR (gst_dvbvideosink_unlock);
	gstbasesink_class->unlock_stop = GST_DEBUG_FUNCPTR (gst_dvbvideosink_unlock_stop);
	gstbasesink_class->set_caps    = GST_DEBUG_FUNCPTR (gst_dvbvideosink_set_caps);

	element_class->change_state    = GST_DEBUG_FUNCPTR (gst_dvbvideosink_change_state);

	gst_dvb_videosink_signals[SIGNAL_GET_DECODER_TIME] =
		g_signal_new ("get-decoder-time",
		G_TYPE_FROM_CLASS (klass),
		G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		G_STRUCT_OFFSET (GstDVBVideoSinkClass, get_decoder_time),
		NULL, NULL, gst_dvbsink_marshal_INT64__VOID, G_TYPE_INT64, 0);

	klass->get_decoder_time = gst_dvbvideosink_get_decoder_time;
}

#define H264_BUFFER_SIZE (64*1024+2048)

/* initialize the new element
 * instantiate pads and add them to element
 * set functions
 * initialize structure
 */
static void
gst_dvbvideosink_init (GstDVBVideoSink *klass, GstDVBVideoSinkClass * gclass)
{
	FILE *f = fopen("/proc/stb/vmpeg/0/fallback_framerate", "r");
#ifdef VIDEO_SET_ENCODING
	klass->use_set_encoding      = TRUE;
#else
	klass->use_set_encoding      = FALSE;
#endif	
	klass->streamtype            = STREAMTYPE_UNKNOWN;
	klass->initial_header        = TRUE;
	klass->initial_header_private_data_size = 0;

	klass->runtime_header_data_size        = 0;

	klass->dec_running           = FALSE;
	klass->must_send_header      = 1;
	klass->h264_buffer           = NULL;
	klass->h264_nal_len_size     = 0;
	klass->codec_data            = NULL;
	klass->no_write              = 0;
	klass->queue                 = NULL;
	klass->fd                    = -1;

	if (f) {
		fgets(klass->saved_fallback_framerate, 16, f);
		fclose(f);
	}

	gst_base_sink_set_sync(GST_BASE_SINK (klass), FALSE);
	gst_base_sink_set_async_enabled(GST_BASE_SINK (klass), TRUE);
}

static void gst_dvbvideosink_dispose (GObject * object)
{
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (object);
	GstState state, pending;
	GST_DEBUG_OBJECT(self, "dispose");

	// hack start : for decodebin2 bug.. decodebin2 tries to dispose ..
	//              but doesnt set state to NULL when it is READY
	switch(gst_element_get_state(GST_ELEMENT(object), &state, &pending, GST_CLOCK_TIME_NONE))
	{
	case GST_STATE_CHANGE_SUCCESS:
		GST_DEBUG_OBJECT(self, "success");
		if (state != GST_STATE_NULL) {
			GST_DEBUG_OBJECT(self, "state %d in dispose.. set it to NULL (decodebin2 bug?)", state);
			if (gst_element_set_state(GST_ELEMENT(object), GST_STATE_NULL) == GST_STATE_CHANGE_ASYNC) {
				GST_DEBUG_OBJECT(self, "set state returned async... wait!");
				gst_element_get_state(GST_ELEMENT(object), &state, &pending, GST_CLOCK_TIME_NONE);
			}
		}
		break;
	case GST_STATE_CHANGE_ASYNC:
		GST_DEBUG_OBJECT(self, "async");
		break;
	case GST_STATE_CHANGE_FAILURE:
		GST_DEBUG_OBJECT(self, "failure");
		break;
	case GST_STATE_CHANGE_NO_PREROLL:
		GST_DEBUG_OBJECT(self, "no preroll");
		break;
	default:
		break;
	}
	// hack end

	GST_DEBUG_OBJECT(self, "state in dispose %d, pending %d", state, pending);

	G_OBJECT_CLASS (parent_class)->dispose (object);
}

static gint64 gst_dvbvideosink_get_decoder_time (GstDVBVideoSink *self)
{
	if (self->dec_running) {
		gint64 cur = 0;
		static gint64 last_pos = 0;

		ioctl(self->fd, VIDEO_GET_PTS, &cur);

		/* workaround until driver fixed */
		if (cur)
			last_pos = cur;
		else
			cur = last_pos;

		cur *= 11111;

		return cur;
	}
	return GST_CLOCK_TIME_NONE;
}

static gboolean gst_dvbvideosink_unlock (GstBaseSink * basesink)
{
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (basesink);
	GST_OBJECT_LOCK(self);
	self->no_write |= 2;
	GST_OBJECT_UNLOCK(self);
	SEND_COMMAND (self, CONTROL_STOP);
	GST_DEBUG_OBJECT (basesink, "unlock");
	return TRUE;
}

static gboolean gst_dvbvideosink_unlock_stop (GstBaseSink * basesink)
{
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (basesink);
	GST_OBJECT_LOCK(self);
	self->no_write &= ~2;
	GST_OBJECT_UNLOCK(self);
	GST_DEBUG_OBJECT (basesink, "unlock_stop");
	return TRUE;
}

void queue_push(queue_entry_t **queue_base, guint8 *data, size_t len)
{
	queue_entry_t *entry = malloc(sizeof(queue_entry_t)+len);
	queue_entry_t *last = *queue_base;
	guint8 *d = (guint8*)(entry+1);
	memcpy(d, data, len);
	entry->bytes = len;
	entry->offset = 0;
	if (!last)
		*queue_base = entry;
	else {
		while(last->next)
			last = last->next;
		last->next = entry;
	}
	entry->next = NULL;
}

void queue_pop(queue_entry_t **queue_base)
{
	queue_entry_t *base = *queue_base;
	*queue_base = base->next;
	free(base);
}

int queue_front(queue_entry_t **queue_base, guint8 **data, size_t *bytes)
{
	if (!*queue_base) {
		*bytes = 0;
		*data = 0;
	}
	else {
		queue_entry_t *entry = *queue_base;
		*bytes = entry->bytes - entry->offset;
		*data = ((guint8*)(entry+1))+entry->offset;
	}
	return *bytes;
}

static gboolean
gst_dvbvideosink_event (GstBaseSink * sink, GstEvent * event)
{
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (sink);
	GST_DEBUG_OBJECT (self, "EVENT %s", gst_event_type_get_name(GST_EVENT_TYPE (event)));
	int ret=TRUE;

	switch (GST_EVENT_TYPE (event)) {
	case GST_EVENT_FLUSH_START:
		GST_OBJECT_LOCK(self);
		self->no_write |= 1;
		GST_OBJECT_UNLOCK(self);
		SEND_COMMAND (self, CONTROL_STOP);
		break;
	case GST_EVENT_FLUSH_STOP:
		ioctl(self->fd, VIDEO_CLEAR_BUFFER);
		GST_OBJECT_LOCK(self);
		self->must_send_header = 1;
		if (hwtype == DM7025)
			++self->must_send_header;  // we must send the sequence header twice on dm7025... 
		while (self->queue)
			queue_pop(&self->queue);
		self->no_write &= ~1;
		GST_OBJECT_UNLOCK(self);
		break;
	case GST_EVENT_EOS:
	{
		struct pollfd pfd[2];
		int retval;
		printf("[V] GST_EVENT_EOS\n");

		//Notify the player that no addionional data will be injected
#ifdef VIDEO_FLUSH
		ioctl(self->fd, VIDEO_FLUSH, 1/*NONBLOCK*/);
#endif

		pfd[0].fd = READ_SOCKET(self);
		pfd[0].events = POLLIN;
		pfd[1].fd = self->fd;
		pfd[1].events = POLLIN;

		GST_PAD_PREROLL_UNLOCK (sink->sinkpad);
		while (1) {
			retval = poll(pfd, 2, 250);
			printf("[V] poll %d\n", retval);
			if (retval < 0) {
				perror("poll in EVENT_EOS");
				ret=FALSE;
				break;
			}

			if (pfd[0].revents & POLLIN) {
				GST_DEBUG_OBJECT (self, "wait EOS aborted!!\n");
				ret=FALSE;
				break;
			}

			if (pfd[1].revents & POLLIN) {
				GST_DEBUG_OBJECT (self, "got buffer empty from driver!\n");
				break;
			}

			if (sink->flushing) {
				GST_DEBUG_OBJECT (self, "wait EOS flushing!!\n");
				ret=FALSE;
				break;
			}
		}
		GST_PAD_PREROLL_LOCK (sink->sinkpad);

		break;
	}
	case GST_EVENT_NEWSEGMENT:{
		GstFormat fmt;
		gboolean update;
		gdouble rate, applied_rate;
		gint64 cur, stop, time;
		int skip = 0, repeat = 0, ret;
		gst_event_parse_new_segment_full (event, &update, &rate, &applied_rate,	&fmt, &cur, &stop, &time);
		GST_DEBUG_OBJECT (self, "GST_EVENT_NEWSEGMENT rate=%f applied_rate=%f\n", rate, applied_rate);
		
		if (fmt == GST_FORMAT_TIME)
		{	
			if ( rate > 1 )
				skip = (int) rate;
			else if ( rate < 1 )
				repeat = 1.0/rate;

			ret = ioctl(self->fd, VIDEO_SLOWMOTION, repeat);
			ret = ioctl(self->fd, VIDEO_FAST_FORWARD, skip);
// 			gst_segment_set_newsegment_full (&dec->segment, update, rate, applied_rate, dformat, cur, stop, time);
		}
		break;
	}

	default:
		break;
	}

	return ret;
}

#define ASYNC_WRITE(data, len) do { \
		switch(AsyncWrite(sink, self, data, len)) { \
		case -1: goto poll_error; \
		case -3: goto write_error; \
		default: break; \
		} \
	} while(0)

static int AsyncWrite(GstBaseSink * sink, GstDVBVideoSink *self, unsigned char *data, unsigned int len)
{
	unsigned int written=0;
	struct pollfd pfd[2];

	pfd[0].fd = READ_SOCKET(self);
	pfd[0].events = POLLIN;
	pfd[1].fd = self->fd;
	pfd[1].events = POLLOUT | POLLPRI;

	do {
loop_start:
		if (self->no_write & 1) {
			GST_DEBUG_OBJECT (self, "skip %d bytes", len - written);
			break;
		}
		else if (self->no_write & 6) {
			// directly push to queue
			GST_OBJECT_LOCK(self);
			queue_push(&self->queue, data + written, len - written);
			GST_OBJECT_UNLOCK(self);
			GST_DEBUG_OBJECT (self, "pushed %d bytes to queue", len - written);
			break;
		}
		else
			GST_LOG_OBJECT (self, "going into poll, have %d bytes to write", len - written);
		if (poll(pfd, 2, -1) == -1) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		if (pfd[0].revents & POLLIN) {
			/* read all stop commands */
			while (TRUE) {
				gchar command;
				int res;
				READ_COMMAND (self, command, res);
				if (res < 0) {
					GST_DEBUG_OBJECT (self, "no more commands");
					/* no more commands */
					goto loop_start;
				}
			}
		}
		if (pfd[1].revents & POLLPRI) {
			GstStructure *s;
			GstMessage *msg;
			struct video_event evt;
			if (ioctl(self->fd, VIDEO_GET_EVENT, &evt) < 0)
				g_warning ("failed to ioctl VIDEO_GET_EVENT!");
			else {
				GST_INFO_OBJECT (self, "VIDEO_EVENT %d", evt.type);
				if (evt.type == VIDEO_EVENT_SIZE_CHANGED) {
					s = gst_structure_new ("eventSizeChanged",
						"aspect_ratio", G_TYPE_INT, evt.u.size.aspect_ratio == 0 ? 2 : 3,
						"width", G_TYPE_INT, evt.u.size.w,
						"height", G_TYPE_INT, evt.u.size.h, NULL);
					msg = gst_message_new_element (GST_OBJECT (sink), s);
					gst_element_post_message (GST_ELEMENT (sink), msg);
				} else if (evt.type == VIDEO_EVENT_FRAME_RATE_CHANGED) {
					s = gst_structure_new ("eventFrameRateChanged",
						"frame_rate", G_TYPE_INT, evt.u.frame_rate, NULL);
					msg = gst_message_new_element (GST_OBJECT (sink), s);
					gst_element_post_message (GST_ELEMENT (sink), msg);
				} else if (evt.type == 16 /*VIDEO_EVENT_PROGRESSIVE_CHANGED*/) {
					s = gst_structure_new ("eventProgressiveChanged",
						"progressive", G_TYPE_INT, evt.u.frame_rate, NULL);
					msg = gst_message_new_element (GST_OBJECT (sink), s);
					gst_element_post_message (GST_ELEMENT (sink), msg);
				} else
					g_warning ("unhandled DVBAPI Video Event %d", evt.type);
			}
		}
		if (pfd[1].revents & POLLOUT) {
			size_t queue_entry_size;
			guint8 *queue_data;
			GST_OBJECT_LOCK(self);
			if (queue_front(&self->queue, &queue_data, &queue_entry_size)) {
				int wr = write(self->fd, queue_data, queue_entry_size);
				if (wr < 0) {
					switch (errno) {
						case EINTR:
						case EAGAIN:
							break;
						default:
							GST_OBJECT_UNLOCK(self);
							return -3;
					}
				}
				else if (wr == queue_entry_size) {
					queue_pop(&self->queue);
					GST_DEBUG_OBJECT (self, "written %d queue bytes... pop entry", wr);
				}
				else {
					self->queue->offset += wr;
					GST_DEBUG_OBJECT (self, "written %d queue bytes... update offset", wr);
				}
				GST_OBJECT_UNLOCK(self);
				continue;
			}
			GST_OBJECT_UNLOCK(self);
			int wr = write(self->fd, data+written, len - written);
			if (wr < 0) {
				switch (errno) {
					case EINTR:
					case EAGAIN:
						continue;
					default:
						return -3;
				}
			}
			written += wr;
		}
	} while (written != len);

	return 0;
}

static inline void Hexdump(unsigned char *Data, int length)
{

    int k;
    for (k = 0; k < length; k++)
    {
        printf("%02x ", Data[k]);
        if (((k+1)&31)==0)
            printf("\n");
    }
    printf("\n");

}

//#define SEND_DTS
#define DYNAMIC_PIC
#define WRITE_COMPLETE_PACKAGE

//WMV does not like this!
//#define RESEND_TIMESTAMP_EVERY_SUB_FRAME


//#define INJECT_XVID_DIRECTLY

static size_t
buildPesHeader(unsigned char *data, int size, unsigned long long int timestamp, 
	unsigned char stream_id, unsigned int pic_start_code, unsigned int video_private_plus_size)
{
	unsigned char *pes_header = data;
	size_t pes_header_size;

	pes_header[0] = 0x00;
	pes_header[1] = 0x00;
	pes_header[2] = 0x01;
	pes_header[3] = stream_id;

	//pes_header[4] = size >> 8;
	//pes_header[5] = size & 0xFF;

	pes_header[6] = 0x80;

	// No special flags
	pes_header[7] = 0x00; // Flag
	pes_header[8] = 0x00; // Len of additional data field
	pes_header_size = 9;

		/* do we have a timestamp? */
	if (timestamp != GST_CLOCK_TIME_NONE) {
		unsigned long long pts = timestamp * 9LL / 100000 /* convert ns to 90kHz */;

		// Timestamp present
		pes_header[7] = 0x80; // Flag
		pes_header[8] = 0x05; // Len of additional data field
		
		pes_header[9] =  0x21 | ((pts >> 29) & 0xE);
		pes_header[10] = pts >> 22;
		pes_header[11] = 0x01 | ((pts >> 14) & 0xFE);
		pes_header[12] = pts >> 7;
		pes_header[13] = 0x01 | ((pts << 1) & 0xFE);

		pes_header_size += 5;

#ifdef SEND_DTS
		// At the moment it seems that dts has only to be set for dm7025 
		// and than only for audio
#endif

	}

	if (video_private_plus_size) {
		int i;
#define PES_EXTENSION_DATA_PRESENT              0x01
#define PES_PRIVATE_DATA_LENGTH                 8
#define PES_PRIVATE_DATA_FLAG                   0x80

		pes_header[7] |= PES_EXTENSION_DATA_PRESENT ;
		pes_header[8] += (PES_PRIVATE_DATA_LENGTH+1);

		pes_header[pes_header_size + 0] = PES_PRIVATE_DATA_FLAG;
#if 1
		pes_header[pes_header_size + 1] =  video_private_plus_size & 0xFF;
		pes_header[pes_header_size + 2] = (video_private_plus_size >> 8) & 0xFF;
		pes_header[pes_header_size + 3] = (video_private_plus_size >> 16) & 0xFF;
#else
		pes_header[pes_header_size + 1] =  size & 0xFF;
		pes_header[pes_header_size + 2] = (size >> 8) & 0xFF;
		pes_header[pes_header_size + 3] = (size >> 16) & 0xFF;
#endif
		for (i = 4; i < (PES_PRIVATE_DATA_LENGTH+1); i++)
			pes_header[pes_header_size + i] = 0x00;

		pes_header_size += (PES_PRIVATE_DATA_LENGTH+1);
	}

	// FIXME: VC1 wants the size without the pic startcode but divx wants with size
	// So these are different fields and should maybe be splitted up
	if (pic_start_code <= 0xFF) {
		pes_header[4] = (size + pes_header_size - 6) >> 8;
		pes_header[5] = (size + pes_header_size - 6) & 0xFF;
	}

	if (pic_start_code) {
		pes_header[pes_header_size + 0] = 0x00;
		pes_header[pes_header_size + 1] = 0x00;
		pes_header[pes_header_size + 2] = 0x01;
		pes_header[pes_header_size + 3] = pic_start_code & 0xFF; // 00, for picture start

#ifdef DYNAMIC_PIC
		pes_header_size += 4;
		if (pic_start_code > 0xFF) {
			pes_header[pes_header_size] = (pic_start_code >> 8) & 0xFF; // For any extra information (like in mpeg4p2, the pic_start_code)
			pes_header_size += 1;
		}
#else
		pes_header[pes_header_size + 4] = (pic_start_code >> 8) & 0xFF; // For any extra information (like in mpeg4p2, the pic_start_code)
		pes_header_size += 5;
#endif
	}

	//FIXME: See above
	if (pic_start_code > 0xFF) {
		pes_header[4] = (size + pes_header_size - 6) >> 8;
		pes_header[5] = (size + pes_header_size - 6) & 0xFF;
	}

	return pes_header_size;
}

#define VC1_VIDEO_PES_START_CODE                0xfd
#define VC1_FRAME_START_CODE                    0x0d
#define PES_VERSION_FAKE_START_CODE             0x31
#define MPEG_VIDEO_PES_START_CODE               0xe0
#define H263_VIDEO_PES_START_CODE               0xfe
#define MAX_PES_PACKET_SIZE                     65400

//#define DEBUG_EXT

static GstFlowReturn
gst_dvbvideosink_render (GstBaseSink * sink, GstBuffer * buffer)
{
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (sink);
	unsigned char *data = GST_BUFFER_DATA(buffer);
	unsigned int data_len = GST_BUFFER_SIZE (buffer);
	long long        timestamp = GST_BUFFER_TIMESTAMP(buffer);

	if (self->streamtype == STREAMTYPE_UNKNOWN) {
		GST_ELEMENT_ERROR (self, STREAM, FORMAT, (NULL), ("hardware decoder not setup (no caps in pipeline?)"));
		return GST_FLOW_ERROR;
	}

	if (self->fd < 0)
		return GST_FLOW_OK;

#ifdef DEBUG_EXT
	printf("gst_dvbvideosink_render 1\n");
#endif

	unsigned char start_code = MPEG_VIDEO_PES_START_CODE;
	unsigned int pic_start_code = 0;
	unsigned int insertVideoPrivateDataHeader = 0;

	if (self->streamtype == STREAMTYPE_WMV) {
		insertVideoPrivateDataHeader = 1;
		start_code = VC1_VIDEO_PES_START_CODE;
	}
	else if (self->streamtype == STREAMTYPE_VC1) {
		start_code = VC1_VIDEO_PES_START_CODE;
		pic_start_code = VC1_FRAME_START_CODE;
	}
	else if (self->streamtype == STREAMTYPE_DIVX4) {
		unsigned char version = 4;
		pic_start_code = (version << 8) | PES_VERSION_FAKE_START_CODE;
	}
	else if (self->streamtype == STREAMTYPE_DIVX5) {
		unsigned char version = 5;
		pic_start_code = (version << 8) | PES_VERSION_FAKE_START_CODE;
	}
	else if (self->streamtype == STREAMTYPE_H263) {
		insertVideoPrivateDataHeader = 1;
		start_code = H263_VIDEO_PES_START_CODE;
	}

#ifdef DEBUG_EXT
	printf("gst_dvbvideosink_render - write initial header\n");
#endif

	if (self->initial_header)
	{
		if (self->initial_header_private_data_size > 0)
		{
			unsigned char pes_header_initial[PES_MAX_HEADER_SIZE];
			size_t pes_header_size_initial;
			
			pes_header_size_initial = buildPesHeader(pes_header_initial, self->initial_header_private_data_size, GST_CLOCK_TIME_NONE, start_code, 0, 0);

#if 0
			printf("-->\n");
			Hexdump(pes_header_initial, pes_header_size_initial);
			Hexdump(self->initial_header_private_data, self->initial_header_private_data_size);
			printf("<--\n");
#endif

#ifdef WRITE_COMPLETE_PACKAGE
			//printf("--> %d bytes\n", pes_header_size_initial + self->initial_header_private_data_size);
			unsigned char *write_buffer = (unsigned char*) malloc(sizeof(unsigned char) * (pes_header_size_initial + self->initial_header_private_data_size));
			memcpy(write_buffer, pes_header_initial, pes_header_size_initial);
			memcpy(write_buffer + pes_header_size_initial, self->initial_header_private_data, self->initial_header_private_data_size);
			ASYNC_WRITE(write_buffer, pes_header_size_initial + self->initial_header_private_data_size);
			free(write_buffer);
#else
			ASYNC_WRITE(pes_header_initial, pes_header_size_initial);
			ASYNC_WRITE(self->initial_header_private_data, self->initial_header_private_data_size);
#endif
			free(self->initial_header_private_data);
			self->initial_header_private_data_size = 0;
		}

		if (self->streamtype == STREAMTYPE_VC1)
		{
			unsigned char pes_header_initial[PES_MAX_HEADER_SIZE];
			size_t pes_header_size_initial;
			

			/* For VC1 the codec private data is a standard vc1 sequence header so we just copy it to the output */
			guint32 codec_data_len = GST_BUFFER_SIZE(self->codec_data);

			pes_header_size_initial = buildPesHeader(pes_header_initial, codec_data_len, GST_CLOCK_TIME_NONE, start_code, 0, 0);

#if 0
			printf("-->\n");
			Hexdump(pes_header_initial, pes_header_size_initial);
			Hexdump(GST_BUFFER_DATA (self->codec_data), codec_data_len);
			printf("<--\n");
#endif

#ifdef WRITE_COMPLETE_PACKAGE
			{
			unsigned char *write_buffer = (unsigned char*) malloc(sizeof(unsigned char) * (pes_header_size_initial + codec_data_len));
			memcpy(write_buffer, pes_header_initial, pes_header_size_initial);
			memcpy(write_buffer + pes_header_size_initial, GST_BUFFER_DATA (self->codec_data), codec_data_len);
			ASYNC_WRITE(write_buffer, pes_header_size_initial + codec_data_len);
			free(write_buffer);
			}
#else
			ASYNC_WRITE(pes_header_initial, pes_header_size_initial);
			ASYNC_WRITE(GST_BUFFER_DATA (self->codec_data), codec_data_len);
#endif
		}
		self->initial_header = FALSE;
	}

#ifdef DEBUG_EXT
	printf("gst_dvbvideosink_render - initial header written\n");
	printf("gst_dvbvideosink_render - write pes packages\n");
#endif

	unsigned int data_position = 0;
	//int i = 0;
	while (data_position < data_len) {
#define SPLIT_TO_BIG_PACKETS
#ifdef SPLIT_TO_BIG_PACKETS
		unsigned int pes_packet_size = (data_len - data_position) <= MAX_PES_PACKET_SIZE ?
										(data_len - data_position) : MAX_PES_PACKET_SIZE;
#else
		unsigned int pes_packet_size = (data_len - data_position);
#endif

		//if (data_position != 0) {
			//printf("> %d pos=%u size=%u max=%u len=%u remaining=%u\n", i++, data_position, pes_packet_size, MAX_PES_PACKET_SIZE, data_len, data_len - data_position);
		//}

		unsigned char       pes_header[PES_MAX_HEADER_SIZE];
		memset (pes_header, '0', PES_MAX_HEADER_SIZE);
		int pes_header_size = 0;

		if (self->streamtype == STREAMTYPE_VC1 && data_position == 0 && pic_start_code != 0) {
			const unsigned char Vc1FrameStartCode[] = {0, 0, 1, VC1_FRAME_START_CODE};
			if(data_len > 3 && (memcmp (data, Vc1FrameStartCode, 4) == 0)) {
				printf("GOT IT!\n");
				pic_start_code = 0;
			}
		}
		else if (self->streamtype == STREAMTYPE_H264 && data_position == 0 && self->h264_nal_len_size > 0) {
			//This code replaces all nal length bytes with HEAD
#ifdef DEBUG_EXT
			printf("gst_dvbvideosink_render - h264 replace nal lenght %d\n", self->h264_nal_len_size);
#endif
			unsigned int pos = 0;
			if (self->h264_nal_len_size == 4) {
				while(TRUE) {
					unsigned int pack_len = (data[pos] << 24) | (data[pos+1] << 16) | (data[pos+2] << 8) | data[pos+3];
					memcpy(data+pos, "\x00\x00\x00\x01", 4);
					pos += 4;
					if ((pos + pack_len) >= data_len)
						break;
					pos += pack_len;
				}
			}
			else if (self->h264_nal_len_size == 3) {
				while(TRUE) {
					unsigned int pack_len = (data[pos] << 16) | (data[pos+1] << 8) | data[pos+2];
					memcpy(data+pos, "\x00\x00\x01", 3);
					pos += 3;
					if ((pos + pack_len) >= data_len)
						break;
					pos += pack_len;
				}
			}
			else {
				unsigned char *dest = GST_BUFFER_DATA (self->h264_buffer);
				unsigned int dest_pos = 0;
				while(TRUE) {
					unsigned int pack_len = self->h264_nal_len_size == 2 ? (data[pos] << 8) | data[pos+1] : data[pos];
					if (dest_pos + pack_len <= H264_BUFFER_SIZE) {
						memcpy(dest+dest_pos, "\x00\x00\x01", 3);
						dest_pos += 3;
						pos += self->h264_nal_len_size;
						memcpy(dest+dest_pos, data+pos, pack_len);
						dest_pos += pack_len;
						if ((pos + pack_len) >= data_len)
							break;
						pos += pack_len;
					}
					else {
						g_error("BUG!!!!!!!! H264 buffer to small skip video data!!.. please report!\n");
						break;
					}
				}
				data = dest;
				data_len = dest_pos;
			}
#ifdef DEBUG_EXT
			printf("gst_dvbvideosink_render - h264 nal lenght replaced\n");
#endif
		}

#ifdef DEBUG_EXT
		printf("gst_dvbvideosink_render - build PESHeader\n");
#endif
		if (insertVideoPrivateDataHeader) insertVideoPrivateDataHeader = data_len;
		pes_header_size = buildPesHeader(pes_header, pes_packet_size, 
			timestamp, start_code, data_position==0?pic_start_code:0, 
			insertVideoPrivateDataHeader);

		insertVideoPrivateDataHeader = 0;

#ifdef DEBUG_EXT
		printf("gst_dvbvideosink_render - PESHeader built\n");
#endif

#if 0
		printf("--> %d\n", self->runtime_header_data_size);
		Hexdump(pes_header, pes_header_size);
		if (self->runtime_header_data_size > 0)
			Hexdump(self->runtime_header_data, self->runtime_header_data_size);
		Hexdump(data + data_position, pes_packet_size>16?16:pes_packet_size);
		printf("<--\n");
#endif

#ifdef WRITE_COMPLETE_PACKAGE
		{
		//printf("--> %d bytes\n", pes_header_size + pes_packet_size);
		unsigned char *write_buffer = (unsigned char*) malloc(sizeof(unsigned char) * (pes_header_size + self->runtime_header_data_size + pes_packet_size));
		memcpy(write_buffer, pes_header, pes_header_size);
		if (self->runtime_header_data_size > 0)
			memcpy(write_buffer + pes_header_size, self->runtime_header_data, self->runtime_header_data_size);
		memcpy(write_buffer + pes_header_size + self->runtime_header_data_size, data + data_position, pes_packet_size);
		ASYNC_WRITE(write_buffer, pes_header_size + self->runtime_header_data_size + pes_packet_size);
		free(write_buffer);
		}
#else //FIXME
		ASYNC_WRITE(pes_header, pes_header_size);
		if (self->runtime_header_data_size > 0)
			ASYNC_WRITE(self->runtime_header_data, self->runtime_header_data_size);
		ASYNC_WRITE(data + data_position, pes_packet_size - self->runtime_header_data_size);
#endif
#ifdef RESEND_TIMESTAMP_EVERY_SUB_FRAME
#else
		timestamp = GST_CLOCK_TIME_NONE;
#endif
		data_position += pes_packet_size;
		//printf("< pos=%u size=%u\n", data_position, pes_packet_size);
	}

#ifdef DEBUG_EXT
	printf("gst_dvbvideosink_render - pes packages written\n");
#endif

	return GST_FLOW_OK;

poll_error:
	{
		GST_ELEMENT_ERROR (self, RESOURCE, READ, (NULL),
				("poll on file descriptor: %s.", g_strerror (errno)));
		GST_WARNING_OBJECT (self, "Error during poll");
		return GST_FLOW_ERROR;
	}
write_error:
	{
		GST_ELEMENT_ERROR (self, RESOURCE, READ, (NULL),
				("write on file descriptor: %s.", g_strerror (errno)));
		GST_WARNING_OBJECT (self, "Error during write");
		return GST_FLOW_ERROR;
	}
	self->must_send_header = 1;
	if (hwtype == DM7025)
		++self->must_send_header;  // we must send the sequence header twice on dm7025...
}

static gboolean 
gst_dvbvideosink_set_caps (GstBaseSink * basesink, GstCaps * caps)
{
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (basesink);
	GstStructure    *structure = gst_caps_get_structure (caps, 0);
	const char      *mimetype = gst_structure_get_name (structure);
	int              streamtype = STREAMTYPE_UNKNOWN;

#ifdef DEBUG_EXT
	printf("gst_dvbvideosink_set_caps ->\n");
#endif

	printf("\nMIMETYPE: %s\n", mimetype);

	if (!strcmp (mimetype, "video/mpeg")) {
		gint mpegversion;
		gst_structure_get_int (structure, "mpegversion", &mpegversion);
		switch (mpegversion) {
			case 1:
				streamtype = STREAMTYPE_MPEG1;
				GST_INFO_OBJECT (self, "MIMETYPE video/mpeg1 -> VIDEO_SET_STREAMTYPE, 6");
			break;
			case 2:
				streamtype = STREAMTYPE_MPEG2;
				GST_INFO_OBJECT (self, "MIMETYPE video/mpeg2 -> VIDEO_SET_STREAMTYPE, 0");
			break;
			case 4: //TODO
			{
				const GValue *codec_data = gst_structure_get_value (structure, "codec_data");
				if (codec_data) {
					GST_INFO_OBJECT (self, "MPEG4 have codec data");
					self->codec_data = gst_value_get_buffer (codec_data);
					gst_buffer_ref (self->codec_data);
				}
				streamtype = STREAMTYPE_MPEG4_P2;
				GST_INFO_OBJECT (self, "MIMETYPE video/mpeg4 -> VIDEO_SET_STREAMTYPE, 4");
			}
			break;
			default:
				GST_ELEMENT_ERROR (self, STREAM, FORMAT, (NULL), ("unhandled mpeg version %i", mpegversion));
			break;
		}

	} else if (!strcmp (mimetype, "video/x-3ivx")) {
		const GValue *codec_data = gst_structure_get_value (structure, "codec_data");
		if (codec_data) {
			GST_INFO_OBJECT (self, "have 3ivx codec... handle as CT_MPEG4_PART2");
			self->codec_data = gst_value_get_buffer (codec_data);
			gst_buffer_ref (self->codec_data);
		}
		streamtype = STREAMTYPE_MPEG4_P2;
		GST_INFO_OBJECT (self, "MIMETYPE video/x-3ivx -> VIDEO_SET_STREAMTYPE, 4");

	} else if (!strcmp (mimetype, "video/x-h264")) {
		const GValue *cd_data = gst_structure_get_value (structure, "codec_data");
		streamtype = STREAMTYPE_H264;
		if (cd_data) {
#ifdef DEBUG_EXT
			printf("[V] video/x-h264 has codec_data\n");
#endif
			unsigned char tmp[2048];
			unsigned int tmp_len = 0;
			GstBuffer *codec_data = gst_value_get_buffer (cd_data);
			unsigned char *data = GST_BUFFER_DATA (codec_data);
			unsigned int cd_len = GST_BUFFER_SIZE (codec_data);
			unsigned int cd_pos = 0;
			GST_INFO_OBJECT (self, "H264 have codec data..!");

#define AVCC_VERSION 0
#define AVCC_PROFILE 1
#define AVCC_COMPATIBILITY 2
#define AVCC_LEVEL 3
#define AVCC NAL_LENGTH_MINUS_ONE 4
#define AVCC_NUM_PARAM_SETS 5

			if (cd_len > 7 && data[AVCC_VERSION] == 1) { // avcC version
				unsigned short len = (data[6] << 8) | data[7]; // Length of first sequence param set
				if (cd_len >= (len + 8)) {
					unsigned int i=0;
					uint8_t profile_num[] = { 66, 77, 88, 100 };
					uint8_t profile_cmp[2] = { 0x67, 0x00 };
					const char *profile_str[] = { "baseline", "main", "extended", "high" };
					memcpy(tmp, "\x00\x00\x00\x01", 4); // Copy HEAD
					tmp_len += 4;
					memcpy(tmp+tmp_len, data+8, len); // Copy body of sequence paramter
					for (i = 0; i < 4; ++i) { // Check every type and fix if necessary
						profile_cmp[1] = profile_num[i];
						if (!memcmp(tmp+tmp_len, profile_cmp, 2)) {
							uint8_t level_org = tmp[tmp_len+3];
							if (level_org > 0x29) {
								GST_INFO_OBJECT (self, "H264 %s profile@%d.%d patched down to 4.1!", profile_str[i], level_org / 10 , level_org % 10);
								tmp[tmp_len+3] = 0x29; // level 4.1
							}
							else
								GST_INFO_OBJECT (self, "H264 %s profile@%d.%d", profile_str[i], level_org / 10 , level_org % 10);
							break;
						}
					}
					tmp_len += len;
					cd_pos = 8 + len;
					if (cd_len > (cd_pos + 2)) {
						len = (data[cd_pos+1] << 8) | data[cd_pos+2]; // Length of first picture param set
						cd_pos += 3;
						if (cd_len >= (cd_pos+len)) {
							memcpy(tmp+tmp_len, "\x00\x00\x00\x01", 4); // Copy HEAD
							tmp_len += 4;
							memcpy(tmp+tmp_len, data+cd_pos, len); // Copy body of picture paramter
							tmp_len += len;

							// Copy created pivate header
							self->initial_header_private_data_size = tmp_len;
							self->initial_header_private_data = 
								(guint8*) malloc(sizeof(guint8) * self->initial_header_private_data_size);
							memcpy(self->initial_header_private_data, tmp, tmp_len);
							self->h264_nal_len_size = (data[4] & 0x03) + 1; // Whereas data[4] is nallengthMinusOne
							if (self->h264_nal_len_size < 3)
								self->h264_buffer = gst_buffer_new_and_alloc(H264_BUFFER_SIZE);
						}
						else
							GST_WARNING_OBJECT (self, "codec_data to short(4)");
					}
					else
						GST_WARNING_OBJECT (self, "codec_data to short(3)");
				}
				else
					GST_WARNING_OBJECT (self, "codec_data to short(2)");
			}
			else if (cd_len <= 7)
				GST_WARNING_OBJECT (self, "codec_data to short(1)");
			else
				GST_WARNING_OBJECT (self, "wrong avcC version %d!", data[0]);
		}
		else
			self->h264_nal_len_size = 0;
		GST_INFO_OBJECT (self, "MIMETYPE video/x-h264 VIDEO_SET_STREAMTYPE, 1");

	} else if (!strcmp (mimetype, "video/x-h263")) {
		streamtype = STREAMTYPE_H263;
		GST_INFO_OBJECT (self, "MIMETYPE video/x-h263 VIDEO_SET_STREAMTYPE, 2");
		const GValue *codec_data = gst_structure_get_value (structure, "codec_data");
		self->codec_data = gst_value_get_buffer (codec_data);
		gst_buffer_ref (self->codec_data);

#ifdef INJECT_XVID_DIRECTLY
	} else if (!strcmp (mimetype, "video/x-xvid")) {
		streamtype = STREAMTYPE_XVID;
		GST_INFO_OBJECT (self, "MIMETYPE video/x-xvid -> VIDEO_SET_STREAMTYPE, 10");

	} else if (!strcmp (mimetype, "video/x-divx") || !strcmp (mimetype, "video/x-msmpeg")) {
#else
	} else if (!strcmp (mimetype, "video/x-divx") || !strcmp (mimetype, "video/x-msmpeg") || !strcmp (mimetype, "video/x-xvid")) {
#endif
		gint divxversion = -1;
#ifndef INJECT_XVID_DIRECTLY
		if(!strcmp (mimetype, "video/x-xvid"))
			divxversion = 5;
		else
#endif
		if (!gst_structure_get_int (structure, "divxversion", &divxversion) && !gst_structure_get_int (structure, "msmpegversion", &divxversion))
			;

		printf("\nDIVX %d\n", divxversion);

		switch (divxversion) {
			case 0:
			break;
			case 3:
			case 43:
			{
				#define B_GET_BITS(w,e,b)  (((w)>>(b))&(((unsigned)(-1))>>((sizeof(unsigned))*8-(e+1-b))))
				#define B_SET_BITS(name,v,e,b)  (((unsigned)(v))<<(b))
				static const guint8 brcm_divx311_sequence_header[] = {
					0x00, 0x00, 0x01, 0xE0, 0x00, 0x34, 0x80, 0x80, // PES HEADER
					0x05, 0x2F, 0xFF, 0xFF, 0xFF, 0xFF, 
					0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x20, /* 0 .. 7 */
					0x08, 0xC8, 0x0D, 0x40, 0x00, 0x53, 0x88, 0x40, /* 8 .. 15 */
					0x0C, 0x40, 0x01, 0x90, 0x00, 0x97, 0x53, 0x0A, /* 16 .. 24 */
					0x00, 0x00, 0x00, 0x00,
					0x30, 0x7F, 0x00, 0x00, 0x01, 0xB2, 0x44, 0x69, /* 0 .. 7 */
					0x76, 0x58, 0x33, 0x31, 0x31, 0x41, 0x4E, 0x44  /* 8 .. 15 */
				};
				self->codec_data = gst_buffer_new_and_alloc(63);
				guint8 *data = GST_BUFFER_DATA(self->codec_data);
				gint height, width;
				gst_structure_get_int (structure, "height", &height);
				gst_structure_get_int (structure, "width", &width);
				memcpy(data, brcm_divx311_sequence_header, 63);
				data += 38;
				data[0] = B_GET_BITS(width,11,4);
				data[1] = B_SET_BITS("width [3..0]", B_GET_BITS(width,3,0), 7, 4) |
					B_SET_BITS("'10'", 0x02, 3, 2) |
					B_SET_BITS("height [11..10]", B_GET_BITS(height,11,10), 1, 0);
				data[2] = B_GET_BITS(height,9,2);
				data[3]= B_SET_BITS("height [1.0]", B_GET_BITS(height,1,0), 7, 6) |
					B_SET_BITS("'100000'", 0x20, 5, 0);
				streamtype = STREAMTYPE_DIVX311;
				GST_INFO_OBJECT (self, "MIMETYPE video/x-divx vers. 3 -> VIDEO_SET_STREAMTYPE, 13");
			}
			break;
			case 4:
				GST_INFO_OBJECT (self, "MIMETYPE video/x-divx vers. 4 -> VIDEO_SET_STREAMTYPE, 14");
				streamtype = STREAMTYPE_DIVX4;
			break;
			case 5:
			case 6:
				GST_INFO_OBJECT (self, "MIMETYPE video/x-divx vers. 5 -> VIDEO_SET_STREAMTYPE, 15");
				streamtype = STREAMTYPE_DIVX5;
			break;
			default:
				GST_ELEMENT_ERROR (self, STREAM, FORMAT, (NULL), ("unhandled divx version %i", divxversion));
			break;
		}

		if (streamtype == STREAMTYPE_DIVX4 || streamtype == STREAMTYPE_DIVX5) {
			printf("\nAdding Divx4/5/6 fake header\n");
			self->runtime_header_data_size = 17; //Fakeheader size
			self->runtime_header_data = (unsigned char*)malloc(sizeof(unsigned char) * self->runtime_header_data_size);
			memset(self->runtime_header_data, 0, self->runtime_header_data_size);

			unsigned int startcode = 0x1b0;
			self->runtime_header_data[0] = (startcode >> 24) & 0xFF;
			self->runtime_header_data[1] = (startcode >> 16) & 0xFF;
			self->runtime_header_data[2] = (startcode >> 8) & 0xFF;
			self->runtime_header_data[3] =  startcode & 0xFF;

			self->runtime_header_data[4] = 0x00; //Profile = reserved

			unsigned int startcode_user_data = 0x1b2;
			self->runtime_header_data[5] = (startcode_user_data >> 24) & 0xFF;
			self->runtime_header_data[6] = (startcode_user_data >> 16) & 0xFF;
			self->runtime_header_data[7] = (startcode_user_data >> 8) & 0xFF;
			self->runtime_header_data[8] =  startcode_user_data & 0xFF;

			unsigned int sttc = 0x53545443;
			self->runtime_header_data[9]  = (sttc >> 24) & 0xFF;
			self->runtime_header_data[10] = (sttc >> 16) & 0xFF;
			self->runtime_header_data[11] = (sttc >> 8) & 0xFF;
			self->runtime_header_data[12] =  sttc & 0xFF;

			guint rate, rate_nu = 0, rate_de = 0;
			gst_structure_get_fraction (structure, "framerate", &rate_nu, &rate_de);

			if (rate_nu != 0)
				rate = (1000000000) / ((rate_nu * 1000)/rate_de);
			else 
				rate = 0;

			//rate = rate_de / 10;
			
			printf("\nRate: %d %d -> %u\n", rate_nu, rate_de, rate);
			self->runtime_header_data[13] = (rate >> 24) & 0xFF;
			self->runtime_header_data[14] = (rate >> 16) & 0xFF;
			self->runtime_header_data[15] = (rate >> 8) & 0xFF;
			self->runtime_header_data[16] =  rate & 0xFF;
		}

	} else if (!strcmp (mimetype, "video/x-wmv")) {
		gint wmvversion = -1, rate, rate_nu = -1, rate_de = -1, width = -1, height = -1;
		guint32 format = 0xFF;

		#define WMV3_PRIVATE_DATA_LENGTH        4
		#define VC1_SEQUENCE_LAYER_METADATA_START_CODE          0x80
		#define VC1_FRAME_START_CODE                            0x0d

		#define METADATA_STRUCT_A_START             12
		#define METADATA_STRUCT_B_START             24
		#define METADATA_STRUCT_B_FRAMERATE_START   32
		#define METADATA_STRUCT_C_START             8

		const unsigned char  SequenceLayerStartCode[]          = {0x00,    0x00,   0x01,   VC1_SEQUENCE_LAYER_METADATA_START_CODE};

		const unsigned char  Metadata[]          =
		{
			0x00,    0x00,   0x00,   0xc5,
			0x04,    0x00,   0x00,   0x00,
			0xc0,    0x00,   0x00,   0x00,   /* Struct C set for for advanced profile*/
			0x00,    0x00,   0x00,   0x00,   /* Struct A */
			0x00,    0x00,   0x00,   0x00,
			0x0c,    0x00,   0x00,   0x00,
			0x60,    0x00,   0x00,   0x00,   /* Struct B */
			0x00,    0x00,   0x00,   0x00,
			0x00,    0x00,   0x00,   0x00
		};

		gst_structure_get_int      (structure, "wmvversion", &wmvversion);
		gst_structure_get_fourcc      (structure, "format", &format);
		gst_structure_get_fraction (structure, "framerate", &rate_nu, &rate_de);
		gst_structure_get_int      (structure, "width", &width);
		gst_structure_get_int      (structure, "height", &height);


		rate = rate_de;
		printf("Rate: %d -> %d\n", rate_de, rate);
		printf("Format: %u\n", format);

#define FOURCC_WMV3 0x33564D57
#define FOURCC_WVC1 0x31435657

		const GValue *codec_data = gst_structure_get_value (structure, "codec_data");
		guint8 *h      = GST_BUFFER_DATA(gst_value_get_buffer (codec_data));
		guint32 h_size = GST_BUFFER_SIZE(gst_value_get_buffer (codec_data));

		guint8 METADATA_START = 0;
		if (format == FOURCC_WVC1) {
			self->codec_data = gst_value_get_buffer (codec_data);
			gst_buffer_ref (self->codec_data);

			self->initial_header_private_data_size = sizeof(SequenceLayerStartCode) + sizeof(Metadata);
			METADATA_START = sizeof(SequenceLayerStartCode);
		}
		else if (format == FOURCC_WMV3) {
			self->initial_header_private_data_size = sizeof(Metadata);
		}

		self->initial_header_private_data = 
			(guint8*) malloc(sizeof(guint8) * self->initial_header_private_data_size);
		memset (self->initial_header_private_data, 0, self->initial_header_private_data_size);

		if (format == FOURCC_WVC1)
			memcpy (self->initial_header_private_data, SequenceLayerStartCode, sizeof(SequenceLayerStartCode));

		memcpy(self->initial_header_private_data + METADATA_START , Metadata, sizeof(Metadata));

		if (format == FOURCC_WMV3)
			memcpy(self->initial_header_private_data + METADATA_START + METADATA_STRUCT_C_START, h, WMV3_PRIVATE_DATA_LENGTH);

		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_A_START + 0] = (height >>  0) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_A_START + 1] = (height >>  8) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_A_START + 2] = (height >> 16) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_A_START + 3] =  height >> 24;

		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_A_START + 4] = (width >>  0) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_A_START + 5] = (width >>  8) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_A_START + 6] = (width >> 16) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_A_START + 7] =  width >> 24;

		//Note: The rate returned by ffmpeg is slightly different (24 instead of 29,97)
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_B_START + 8] = (rate >>  0) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_B_START + 9] = (rate >>  8) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_B_START + 10] = (rate >> 16) & 0xFF;
		self->initial_header_private_data[METADATA_START + METADATA_STRUCT_B_START + 11] =  rate >> 24;

		if (format == FOURCC_WVC1)
			streamtype = STREAMTYPE_VC1;
		else if (format == FOURCC_WMV3)
			streamtype = STREAMTYPE_WMV;
	}
	if (streamtype != -STREAMTYPE_UNKNOWN) {
		gint numerator, denominator;
		if (gst_structure_get_fraction (structure, "framerate", &numerator, &denominator)) {
			FILE *f = fopen("/proc/stb/vmpeg/0/fallback_framerate", "w");
			if (f) {
				int valid_framerates[] = { 23976, 24000, 25000, 29970, 30000, 50000, 59940, 60000 };
				int framerate = (int)(((double)numerator * 1000) / denominator);
				int diff = 60000;
				int best = 0;
				int i = 0;
				for (; i < 7; ++i) {
					int ndiff = abs(framerate - valid_framerates[i]);
					if (ndiff < diff) {
						diff = ndiff;
						best = i;
					}
				}
				fprintf(f, "%d", valid_framerates[best]);
				fclose(f);
			}
		}
		if (self->dec_running) {
			ioctl(self->fd, VIDEO_STOP, 0);
			self->dec_running = FALSE;
		}

		if (self->use_set_encoding)
		{
#ifdef VIDEO_SET_ENCODING
			unsigned int encoding = streamtype_to_encoding(streamtype);
			if (ioctl(self->fd, VIDEO_SET_ENCODING, encoding) < 0) {
				GST_ELEMENT_WARNING (self, STREAM, DECODE, (NULL), ("hardware decoder can't be set to encoding %i.", encoding));
			}
#endif
		}
		else
		{
			if (ioctl(self->fd, VIDEO_SET_STREAMTYPE, streamtype) < 0 )
				if ( streamtype != 0 && streamtype != 6 )
					GST_ELEMENT_ERROR (self, STREAM, CODEC_NOT_FOUND, (NULL), ("hardware decoder can't handle streamtype %i", streamtype));
		}
		self->streamtype = streamtype;

		ioctl(self->fd, VIDEO_PLAY);
		self->dec_running = TRUE;
	} else
		GST_ELEMENT_ERROR (self, STREAM, TYPE_NOT_FOUND, (NULL), ("unimplemented stream type %s", mimetype));

#ifdef DEBUG_EXT
	printf("gst_dvbvideosink_set_caps <-\n");
#endif

	return TRUE;
}

static int readMpegProc(char *str, int decoder)
{
	int val = -1;
	char tmp[64];
	sprintf(tmp, "/proc/stb/vmpeg/%d/%s", decoder, str);
	FILE *f = fopen(tmp, "r");
	if (f)
	{
		fscanf(f, "%x", &val);
		fclose(f);
	}
	return val;
}

static int readApiSize(int fd, int *xres, int *yres, int *aspect)
{
	video_size_t size;
	if (!ioctl(fd, VIDEO_GET_SIZE, &size))
	{
		*xres = size.w;
		*yres = size.h;
		*aspect = size.aspect_ratio == 0 ? 2 : 3;  // convert dvb api to etsi
		return 0;
	}
	return -1;
}

static int readApiFrameRate(int fd, int *framerate)
{
	unsigned int frate;
	if (!ioctl(fd, VIDEO_GET_FRAME_RATE, &frate))
	{
		*framerate = frate;
		return 0;
	}
	return -1;
}

static gboolean
gst_dvbvideosink_start (GstBaseSink * basesink)
{
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (basesink);
	gint control_sock[2];

	GST_DEBUG_OBJECT (self, "start");


	if (socketpair(PF_UNIX, SOCK_STREAM, 0, control_sock) < 0) {
		perror("socketpair");
		goto socket_pair;
	}

	READ_SOCKET (self) = control_sock[0];
	WRITE_SOCKET (self) = control_sock[1];

	fcntl (READ_SOCKET (self), F_SETFL, O_NONBLOCK);
	fcntl (WRITE_SOCKET (self), F_SETFL, O_NONBLOCK);

	return TRUE;
	/* ERRORS */
socket_pair:
	{
		GST_ELEMENT_ERROR (self, RESOURCE, OPEN_READ_WRITE, (NULL),
				GST_ERROR_SYSTEM);
		return FALSE;
	}
}

static gboolean
gst_dvbvideosink_stop (GstBaseSink * basesink)
{
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (basesink);
	FILE *f = fopen("/proc/stb/vmpeg/0/fallback_framerate", "w");
	GST_DEBUG_OBJECT (self, "stop");
	if (self->fd >= 0)
	{
		if (self->dec_running) {
			ioctl(self->fd, VIDEO_STOP);
			self->dec_running = FALSE;
		}
		ioctl(self->fd, VIDEO_SLOWMOTION, 0);
		ioctl(self->fd, VIDEO_FAST_FORWARD, 0);
		ioctl(self->fd, VIDEO_SELECT_SOURCE, VIDEO_SOURCE_DEMUX);
		close(self->fd);
	}

	if (self->codec_data)
		gst_buffer_unref(self->codec_data);

	if (self->h264_buffer)
		gst_buffer_unref(self->h264_buffer);


	while(self->queue)
		queue_pop(&self->queue);

	if (f) {
		fputs(self->saved_fallback_framerate, f);
		fclose(f);
	}

	close (READ_SOCKET  (self));
	close (WRITE_SOCKET (self));
	READ_SOCKET (self)  = -1;
	WRITE_SOCKET (self) = -1;

	return TRUE;
}

static GstStateChangeReturn
gst_dvbvideosink_change_state (GstElement * element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstDVBVideoSink *self = GST_DVBVIDEOSINK (element);

	switch (transition) {
	case GST_STATE_CHANGE_NULL_TO_READY:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_NULL_TO_READY");
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_READY_TO_PAUSED");

		self->fd = open("/dev/dvb/adapter0/video0", O_RDWR|O_NONBLOCK);
//		self->fd = open("/dump.pes", O_RDWR|O_CREAT|O_TRUNC, 0555);

		GST_OBJECT_LOCK(self);
		self->no_write |= 4;
		GST_OBJECT_UNLOCK(self);

		if (self->fd >= 0) {
			GstStructure *s = 0;
			GstMessage *msg = 0;
			int aspect = -1, width = -1, height = -1, framerate = -1,
				progressive = readMpegProc("progressive", 0);

			if (readApiSize(self->fd, &width, &height, &aspect) == -1) {
				aspect = readMpegProc("aspect", 0);
				width = readMpegProc("xres", 0);
				height = readMpegProc("yres", 0);
			} else
				aspect = aspect == 0 ? 2 : 3; // dvb api to etsi
			if (readApiFrameRate(self->fd, &framerate) == -1)
				framerate = readMpegProc("framerate", 0);

			s = gst_structure_new ("eventSizeAvail",
				"aspect_ratio", G_TYPE_INT, aspect == 0 ? 2 : 3,
				"width", G_TYPE_INT, width,
				"height", G_TYPE_INT, height, NULL);
			msg = gst_message_new_element (GST_OBJECT (element), s);
			gst_element_post_message (GST_ELEMENT (element), msg);
			s = gst_structure_new ("eventFrameRateAvail",
				"frame_rate", G_TYPE_INT, framerate, NULL);
			msg = gst_message_new_element (GST_OBJECT (element), s);
			gst_element_post_message (GST_ELEMENT (element), msg);
			s = gst_structure_new ("eventProgressiveAvail",
				"progressive", G_TYPE_INT, progressive, NULL);
			msg = gst_message_new_element (GST_OBJECT (element), s);
			gst_element_post_message (GST_ELEMENT (element), msg);
			ioctl(self->fd, VIDEO_SELECT_SOURCE, VIDEO_SOURCE_MEMORY);
			ioctl(self->fd, VIDEO_FREEZE);
		}
		break;
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_PAUSED_TO_PLAYING");
		ioctl(self->fd, VIDEO_CONTINUE);
		GST_OBJECT_LOCK(self);
		self->no_write &= ~4;
		GST_OBJECT_UNLOCK(self);
		break;
	default:
		break;
	}

	ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

	switch (transition) {
	case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_PLAYING_TO_PAUSED");
		GST_OBJECT_LOCK(self);
		self->no_write |= 4;
		GST_OBJECT_UNLOCK(self);
		ioctl(self->fd, VIDEO_FREEZE);
		SEND_COMMAND (self, CONTROL_STOP);
		break;
	case GST_STATE_CHANGE_PAUSED_TO_READY:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_PAUSED_TO_READY");
		break;
	case GST_STATE_CHANGE_READY_TO_NULL:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_READY_TO_NULL");
		break;
	default:
		break;
	}

	return ret;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and pad templates
 * register the features
 *
 * exchange the string 'plugin' with your elemnt name
 */
static gboolean
plugin_init (GstPlugin *plugin)
{
	return gst_element_register (plugin, "dvbvideosink",
						 GST_RANK_PRIMARY,
						 GST_TYPE_DVBVIDEOSINK);
}

/* this is the structure that gstreamer looks for to register plugins
 *
 * exchange the strings 'plugin' and 'Template plugin' with you plugin name and
 * description
 */
GST_PLUGIN_DEFINE (
	GST_VERSION_MAJOR,
	GST_VERSION_MINOR,
	"dvb_video_out",
	"DVB Video Output",
	plugin_init,
	VERSION,
	"LGPL",
	"GStreamer",
	"http://gstreamer.net/"
)
