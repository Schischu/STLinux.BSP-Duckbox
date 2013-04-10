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
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/dvb/audio.h>
#include <linux/dvb/video.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

#ifdef __sh__
#include <linux/dvb/stm_ioctls.h>
#endif

#include <gst/gst.h>

#include "gstdvbaudiosink.h"
#include "gstdvbsink-marshal.h"

/* We add a control socket as in fdsrc to make it shutdown quickly when it's blocking on the fd.
 * Poll is used to determine when the fd is ready for use. When the element state is changed,
 * it happens from another thread while fdsink is poll'ing on the fd. The state-change thread 
 * sends a control message, so fdsink wakes up and changes state immediately otherwise
 * it would stay blocked until it receives some data. */

/* the poll call is also performed on the control sockets, that way
 * we can send special commands to unblock the poll call */
#define CONTROL_STOP          'S' /* stop the poll call */
#define CONTROL_SOCKETS(sink) sink->control_sock
#define WRITE_SOCKET(sink)    sink->control_sock[1]
#define READ_SOCKET(sink)     sink->control_sock[0]

#define SEND_COMMAND(sink, command)    \
G_STMT_START {                         \
	unsigned char c; c = command;      \
	write (WRITE_SOCKET(sink), &c, 1); \
} G_STMT_END

#define READ_COMMAND(sink, command, res)        \
G_STMT_START {                                  \
	res = read(READ_SOCKET(sink), &command, 1); \
} G_STMT_END

#ifndef AUDIO_GET_PTS
#define AUDIO_GET_PTS           _IOR('o', 19, gint64)
#endif

#define PROP_LOCATION 99

GST_DEBUG_CATEGORY_STATIC (dvbaudiosink_debug);
#define GST_CAT_DEFAULT dvbaudiosink_debug

enum
{
	SIGNAL_GET_DECODER_TIME,
	LAST_SIGNAL
};

static guint gst_dvbaudiosink_signals[LAST_SIGNAL] = { 0 };

static guint AdtsSamplingRates[] = { 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350, 0 };

static GstStaticPadTemplate sink_factory_ati_xilleon =
GST_STATIC_PAD_TEMPLATE (
	"sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("audio/mpeg, "
		"mpegversion = (int) 1, "
		"layer = (int) [ 1, 2 ], "
		"framed = (boolean) true; "
		"audio/x-ac3, "
		"framed = (boolean) true; "
		"audio/x-private1-ac3, "
		"framed = (boolean) true")
);

static GstStaticPadTemplate sink_factory_broadcom_dts =
GST_STATIC_PAD_TEMPLATE (
	"sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("audio/mpeg, "
		"framed = (boolean) true; "
		"audio/x-ac3, "
		"framed = (boolean) true; "
		"audio/x-private1-ac3, "
		"framed = (boolean) true; "
		"audio/x-dts, "
		"framed = (boolean) true; "
		"audio/x-private1-dts, "
		"framed = (boolean) true; "
		"audio/x-private1-lpcm, "
		"framed = (boolean) true")
);

static GstStaticPadTemplate sink_factory_broadcom =
GST_STATIC_PAD_TEMPLATE (
	"sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS ("audio/mpeg, "
		"framed = (boolean) true; "
		"audio/x-ac3, "
		"framed = (boolean) true; "
		"audio/x-private1-ac3, "
		"framed = (boolean) true")
);

#define SINK_FACTORY_STM_BASE \
GST_STATIC_PAD_TEMPLATE ( \
	"sink", \
	GST_PAD_SINK, \
	GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("audio/mpeg, " \
		"framed = (boolean) true; " \
		"audio/x-ac3, " \
		"framed = (boolean) true; " \
		"audio/x-eac3, " \
		"framed = (boolean) true; " \
		"audio/x-private1-ac3, " \
		"framed = (boolean) true; " \
		"audio/x-dts, " \
		"framed = (boolean) true; " \
		"audio/x-private1-dts, " \
		"framed = (boolean) true; " \
		"audio/x-private1-lpcm, " \
		"framed = (boolean) true; " \
		"audio/x-wma, " \
		"framed = (boolean) true; " \
		"audio/x-ms-wma, " \
		"framed = (boolean) true") \
)

#define SINK_FACTORY_STM_BASE_EXTENDED \
GST_STATIC_PAD_TEMPLATE ( \
	"sink", \
	GST_PAD_SINK, \
	GST_PAD_ALWAYS, \
	GST_STATIC_CAPS ("audio/mpeg, " \
		"framed = (boolean) true; " \
		"audio/x-ac3, " \
		"framed = (boolean) true; " \
		"audio/x-eac3, " \
		"framed = (boolean) true; " \
		"audio/x-private1-ac3, " \
		"framed = (boolean) true; " \
		"audio/x-dts, " \
		"framed = (boolean) true; " \
		"audio/x-private1-dts, " \
		"framed = (boolean) true; " \
		"audio/x-raw-int; " \
		"audio/x-private1-lpcm, " \
		"framed = (boolean) true; " \
		"audio/x-wma, " \
		"framed = (boolean) true; " \
		"audio/x-ms-wma, " \
		"framed = (boolean) true") \
)

// FIRST GENERATION
static GstStaticPadTemplate sink_factory_stm_stx7100 = SINK_FACTORY_STM_BASE_EXTENDED;
static GstStaticPadTemplate sink_factory_stm_stx7101 = SINK_FACTORY_STM_BASE_EXTENDED;
static GstStaticPadTemplate sink_factory_stm_stx7109 = SINK_FACTORY_STM_BASE_EXTENDED;

//#define TODO_ALSASINK_WORKING //Todo: Some iamge have problems with the alsasink, needs investigation.
#if defined(TODO_ALSASINK_WORKING)
// SECOND GENERATION
static GstStaticPadTemplate sink_factory_stm_stx7105 = SINK_FACTORY_STM_BASE;
static GstStaticPadTemplate sink_factory_stm_stx7111 = SINK_FACTORY_STM_BASE;

// THIRD GENERATION
static GstStaticPadTemplate sink_factory_stm_stx7106 = SINK_FACTORY_STM_BASE;
static GstStaticPadTemplate sink_factory_stm_stx7108 = SINK_FACTORY_STM_BASE;
#else
// SECOND GENERATION
static GstStaticPadTemplate sink_factory_stm_stx7105 = SINK_FACTORY_STM_BASE_EXTENDED;
static GstStaticPadTemplate sink_factory_stm_stx7111 = SINK_FACTORY_STM_BASE_EXTENDED;

// THIRD GENERATION
static GstStaticPadTemplate sink_factory_stm_stx7106 = SINK_FACTORY_STM_BASE_EXTENDED;
static GstStaticPadTemplate sink_factory_stm_stx7108 = SINK_FACTORY_STM_BASE_EXTENDED;
#endif

#define DEBUG_INIT(bla) \
	GST_DEBUG_CATEGORY_INIT (dvbaudiosink_debug, "dvbaudiosink", 0, "dvbaudiosink element");

GST_BOILERPLATE_FULL (GstDVBAudioSink, gst_dvbaudiosink, GstBaseSink, GST_TYPE_BASE_SINK, DEBUG_INIT);

static void gst_dvbaudiosink_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_dvbaudiosink_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec);

static gboolean             gst_dvbaudiosink_start (GstBaseSink * sink);
static gboolean             gst_dvbaudiosink_stop (GstBaseSink * sink);
static gboolean             gst_dvbaudiosink_event (GstBaseSink * sink, GstEvent * event);
static GstFlowReturn        gst_dvbaudiosink_render (GstBaseSink * sink, GstBuffer * buffer);
static gboolean             gst_dvbaudiosink_unlock (GstBaseSink * basesink);
static gboolean             gst_dvbaudiosink_unlock_stop (GstBaseSink * basesink);
static gboolean             gst_dvbaudiosink_set_caps (GstBaseSink * sink, GstCaps * caps);
static void                 gst_dvbaudiosink_dispose (GObject * object);
static GstStateChangeReturn gst_dvbaudiosink_change_state (GstElement * element, GstStateChange transition);
static gint64               gst_dvbaudiosink_get_decoder_time (GstDVBAudioSink *self);

#define PES_MAX_HEADER_SIZE 64

typedef enum {  HW_UNKNOWN, 
				DM7025, DM800, DM8000, DM500HD, DM800SE, DM7020HD, 
				STX7100, STX7101, STX7109, STX7105, STX7111, STX7106, STX7108 
} hardware_type_t;

typedef enum { PF_UNKNOWN, DM, HAVANA } platform_type_t;

static hardware_type_t hwtype = HW_UNKNOWN;
static platform_type_t pftype = PF_UNKNOWN;

//Please correct if wrong
#define BYPASS_UNKNOWN  0xFF
#define BYPASS_AC3      0x00
#define BYPASS_MPEG1    0x01
#define BYPASS_DTS      0x02
#define BYPASS_PCMB     0x04
#define BYPASS_PCML     0x05
#define BYPASS_LPCM     0x06
#define BYPASS_MPEG1_L3 0x0A
#define BYPASS_AAC      0x0B
#define BYPASS_WMA      0x0C
#define BYPASS_VORBIS   0x0D
#define BYPASS_FLAC     0x0E

#define AUDIO_ENCODING_UNKNOWN  0xFF

unsigned int bypass_to_encoding (unsigned int bypass)
{
#ifdef AUDIO_SET_ENCODING
	switch(bypass)
	{
	case BYPASS_AC3:
		return AUDIO_ENCODING_AC3;
	case BYPASS_MPEG1:
		return AUDIO_ENCODING_MPEG1;
	case BYPASS_DTS:
		return AUDIO_ENCODING_DTS;
	case BYPASS_PCMB:
	case BYPASS_PCML:
	case BYPASS_LPCM:
		return AUDIO_ENCODING_LPCMA;
	case BYPASS_MPEG1_L3:
		return AUDIO_ENCODING_MP3;
	case BYPASS_AAC:
		return AUDIO_ENCODING_AAC;
	case BYPASS_WMA:
		return AUDIO_ENCODING_WMA;
	case BYPASS_VORBIS:
		return AUDIO_ENCODING_VORBIS;
	case BYPASS_FLAC:
		return AUDIO_ENCODING_FLAC;
	default:
		return AUDIO_ENCODING_UNKNOWN;
	}
#endif
	return AUDIO_ENCODING_UNKNOWN;
}

static void
gst_dvbaudiosink_base_init (gpointer klass)
{
	static GstElementDetails element_details = {
		"A DVB audio sink",
		"Generic/DVBAudioSink",
		"Outputs a MPEG2 PES / ES into a DVB audio device for hardware playback",
		"Felix Domke <tmbinc@elitedvb.net>"
	};
	GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

	//gst_debug_set_active(TRUE);

	int fd = open("/proc/stb/info/model", O_RDONLY);
	if ( fd > 0 )
	{
		gchar string[9] = { 0, };
		ssize_t rd = read(fd, string, 8);
		if ( rd >= 5 )
		{
			string[rd] = 0;
			if ( !strncasecmp(string, "DM7025", 6) ) {
				pftype = DM;
				hwtype = DM7025;
				GST_INFO ("model is DM7025 set ati xilleon caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_ati_xilleon));
			}
			else if ( !strncasecmp(string, "DM8000", 6) ) {
				pftype = DM;
				hwtype = DM8000;
				GST_INFO ("model is DM8000 set broadcom dts caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_broadcom_dts));
			}
			else if ( !strncasecmp(string, "DM800SE", 7) ) {
				pftype = DM;
				hwtype = DM800SE;
				GST_INFO ("model is DM800SE set broadcom dts caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_broadcom_dts));
			}
			else if ( !strncasecmp(string, "DM7020HD", 8) ) {
				pftype = DM;
				hwtype = DM7020HD;
				GST_INFO ("model is DM7020HD set broadcom dts caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_broadcom_dts));
			}
			else if ( !strncasecmp(string, "DM800", 5) ) {
				pftype = DM;
				hwtype = DM800;
				GST_INFO ("model is DM800 set broadcom caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_broadcom));
			}
			else if ( !strncasecmp(string, "DM500HD", 7) ) {
				pftype = DM;
				hwtype = DM500HD;
				GST_INFO ("model is DM500HD set broadcom dts caps");
				gst_element_class_add_pad_template (element_class,
					gst_static_pad_template_get (&sink_factory_broadcom_dts));
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

static int
gst_dvbaudiosink_async_write(GstDVBAudioSink *self, unsigned char *data, unsigned int len);

/* initialize the plugin's class */
static void
gst_dvbaudiosink_class_init (GstDVBAudioSinkClass *klass)
{
	GObjectClass     *gobject_class     = G_OBJECT_CLASS (klass);
	GstBaseSinkClass *gstbasesink_class = GST_BASE_SINK_CLASS (klass);
	GstElementClass  *gelement_class    = GST_ELEMENT_CLASS (klass);

	gobject_class->dispose      = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_dispose);
	gobject_class->set_property = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_set_property);
	gobject_class->get_property = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_get_property);
	g_object_class_install_property (gobject_class, PROP_LOCATION,
		g_param_spec_string ("dump-filename", "Dump File Location",
			"Filename that Packetized Elementary Stream will be written to", NULL,
			G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	gstbasesink_class->start       = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_start);
	gstbasesink_class->stop        = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_stop);
	gstbasesink_class->render      = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_render);
	gstbasesink_class->event       = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_event);
	gstbasesink_class->unlock      = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_unlock);
	gstbasesink_class->unlock_stop = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_unlock_stop);
	gstbasesink_class->set_caps    = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_set_caps);

	gelement_class->change_state   = GST_DEBUG_FUNCPTR (gst_dvbaudiosink_change_state);

	gst_dvbaudiosink_signals[SIGNAL_GET_DECODER_TIME] =
		g_signal_new ("get-decoder-time",
		G_TYPE_FROM_CLASS (klass),
		G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		G_STRUCT_OFFSET (GstDVBAudioSinkClass, get_decoder_time),
		NULL, NULL, gst_dvbsink_marshal_INT64__VOID, G_TYPE_INT64, 0);

	klass->get_decoder_time = gst_dvbaudiosink_get_decoder_time;
	klass->async_write      = gst_dvbaudiosink_async_write;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set functions
 * initialize structure
 */
static void
gst_dvbaudiosink_init (GstDVBAudioSink *klass, GstDVBAudioSinkClass * gclass)
{
// If AUDIO_SET_ENCODING ioctl is available us it, 
// as using AUDIO_SET_BYPASS to change the encoding is not spec conform
#ifdef AUDIO_SET_ENCODING
	klass->use_set_encoding      = TRUE;
#else
	klass->use_set_encoding      = FALSE;
#endif	
	klass->bypass                = BYPASS_UNKNOWN;

	klass->timestamp             = 0;
	klass->aac_adts_header_valid = FALSE;

	klass->initial_header        = TRUE;

	klass->runtime_header_data_size = 0;
	klass->pcm_bits_per_sample      = 0;
	klass->pcm_sub_frame_len        = 0;
	klass->pcm_sub_frame_per_pes    = 0;
	klass->pcm_break_buffer_size    = 0;

	klass->no_write              = 0;
	klass->queue                 = NULL;
	klass->fd                    = -1;
	klass->dump_fd               = -1;
	klass->dump_filename         = NULL;

	gst_base_sink_set_sync (GST_BASE_SINK(klass), FALSE);
	gst_base_sink_set_async_enabled (GST_BASE_SINK(klass), TRUE);
}

static void
gst_dvbaudiosink_dispose (GObject * object)
{
	GstDVBAudioSink *self = GST_DVBAUDIOSINK (object);
	GstState         state, pending;
	GST_DEBUG_OBJECT (self, "dispose");

	// hack start : for gstreamer decodebin2 bug... it tries to dispose .. 
	//              but doesnt set the state to NULL when it is READY
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

	if (self->dump_filename)
	{
			g_free (self->dump_filename);
			self->dump_filename = NULL;
	}

	G_OBJECT_CLASS (parent_class)->dispose (object);
}

static gboolean
gst_dvbaudiosink_set_location (GstDVBAudioSink * sink, const gchar * location)
{
	if (sink->dump_fd)
	{
		g_warning ("Changing the `dump-filename' property during operation is not supported.");
		return FALSE;
	}

	g_free (sink->dump_filename);
	if (location != NULL) {
		/* we store the filename as we received it from the application. On Windows
		 * this should be in UTF8 */
		sink->dump_filename = g_strdup (location);
	} else {
		sink->dump_filename = NULL;
	}

	return TRUE;
}

static void
gst_dvbaudiosink_set_property (GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec)
{
	GstDVBAudioSink *sink = GST_DVBAUDIOSINK (object);

	switch (prop_id) {
		case PROP_LOCATION:
		gst_dvbaudiosink_set_location (sink, g_value_get_string (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
gst_dvbaudiosink_get_property (GObject * object, guint prop_id, GValue * value, GParamSpec * pspec)
{
	GstDVBAudioSink *sink = GST_DVBAUDIOSINK (object);

	switch (prop_id) {
		case PROP_LOCATION:
		g_value_set_string (value, sink->dump_filename);
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static gint64
gst_dvbaudiosink_get_decoder_time (GstDVBAudioSink *self)
{
	if (self->bypass != BYPASS_UNKNOWN) {
		gint64 cur = 0;
		static gint64 last_pos = 0;

		ioctl(self->fd, AUDIO_GET_PTS, &cur);

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

static gboolean
gst_dvbaudiosink_unlock (GstBaseSink * basesink)
{
	GstDVBAudioSink *self = GST_DVBAUDIOSINK (basesink);
	GST_OBJECT_LOCK(self);
	self->no_write |= 2;
	GST_OBJECT_UNLOCK(self);
	SEND_COMMAND (self, CONTROL_STOP);
	GST_DEBUG_OBJECT (basesink, "unlock");
	return TRUE;
}

static gboolean
gst_dvbaudiosink_unlock_stop (GstBaseSink * basesink)
{
	GstDVBAudioSink *self = GST_DVBAUDIOSINK (basesink);
	GST_OBJECT_LOCK(self);
	self->no_write &= ~2;
	GST_OBJECT_UNLOCK(self);
	GST_DEBUG_OBJECT (basesink, "unlock_stop");
	return TRUE;
}

static gboolean
gst_dvbaudiosink_set_caps (GstBaseSink * basesink, GstCaps * caps)
{
	GstDVBAudioSink *self = GST_DVBAUDIOSINK (basesink);
	GstStructure    *structure = gst_caps_get_structure (caps, 0);
	const char      *type = gst_structure_get_name (structure);
	unsigned int     bypass = BYPASS_UNKNOWN;

	self->skip = 0;

	if (!strcmp(type, "audio/mpeg")) {
		gint mpegversion;
		gst_structure_get_int (structure, "mpegversion", &mpegversion);
		switch (mpegversion) {
			case 1:
			{
				gint layer;
				gst_structure_get_int (structure, "layer", &layer);
				if ( layer == 3 )
					bypass = BYPASS_MPEG1_L3;
				else
					bypass = BYPASS_MPEG1;
				GST_INFO_OBJECT (self, "MIMETYPE %s version %d layer %d", type, mpegversion, layer);
				break;
			}
			case 2:
			case 4:
			{
				const gchar *stream_type = gst_structure_get_string (structure, "stream-type");
				if (!stream_type)
					stream_type = gst_structure_get_string (structure, "stream-format");
				if (stream_type && !strcmp(stream_type, "adts"))
					printf("MIMETYPE %s version %d (AAC-ADTS)", type, mpegversion);
				else {
					guint8 channels = 0xFF, rate_idx = 0xFF, obj_type = 0xFF;
					const GValue *codec_data = gst_structure_get_value (structure, "codec_data");
					printf("MIMETYPE %s version %d (AAC-RAW)", type, mpegversion);

					// Get necessary data for header
					if (codec_data) {
						guint8 *h = GST_BUFFER_DATA(gst_value_get_buffer (codec_data));
						printf("have codec data\n");
						obj_type = ((h[0] & 0xC) >> 2) + 1;
						rate_idx = ((h[0] & 0x3) << 1) | ((h[1] & 0x80) >> 7);
						channels = (h[1] & 0x78) >> 3;
					}
					else {
						gint rate_tmp, channels_tmp;
						printf("have no codec data\n");
						if (gst_structure_get_int (structure, "rate", &rate_tmp) && gst_structure_get_int (structure, "channels", &channels_tmp)) {
							do {
								if (AdtsSamplingRates[rate_idx] == rate_tmp)
									break;
								++rate_idx;
							} while (AdtsSamplingRates[rate_idx]);
							if (AdtsSamplingRates[rate_idx]) {
								obj_type = 1; // hardcoded yet.. hopefully this works every time ;)
								channels = (guint8)(channels_tmp&0xFF);
							}
						}
					}

					if (channels != 0xFF && rate_idx != 0xFF && obj_type != 0xFF) {
						printf("have codec data -> obj_type = %d, rate_idx = %d, channels = %d\n",
							obj_type, rate_idx, channels);

						// AAC ADTS Header:
						// 0: SSSSSSSS (S=Sync)
						// 1: SSSS I LL P (I=Id L=Layer P=Protection)
						// 2: OO RRRR P C (O=Profile R=Rate P=Private=0x0 C=Channels)
						// 3: CC ZZZZ LL (Z=Zero=0x0 L=AAC Frame Length)
						// 4: LLLLLLLL
						// 5: LLL DDDDD (D=ADTS Buffer Fullness)
						// 6: DDDDDD NN (N=Count Raw Data Blocks)

						/* Sync point over a full byte */
						self->aac_adts_header[0] = 0xFF; 

						/* Sync point continued over first 4 bits + static 4 bits
						 * (ID, layer, protection)*/
						self->aac_adts_header[1] = 0xF1; 
						if (mpegversion == 2)
							self->aac_adts_header[1] |= 8;

						/* Object type over first 2 bits */ 
						self->aac_adts_header[2] = obj_type << 6; 
						/* rate index over next 4 bits */
						self->aac_adts_header[2] |= rate_idx << 2;
						/* channels over last bit */
						self->aac_adts_header[2] |= (channels & 0x4) >> 2;

						/* channels continued over next 2 bits + 4 bits at zero */
						self->aac_adts_header[3] = (channels & 0x3) << 6; 

						/* Other fields will be set on runtime in gst_dvbaudiosink_render */
						self->aac_adts_header_valid = TRUE;
					}
				}
				bypass = BYPASS_AAC; // always use AAC + ADTS yet..
				break;
			}
			default:
				GST_ELEMENT_ERROR (self, STREAM, FORMAT, (NULL), ("unhandled mpeg version %i", mpegversion));
				break;
		}
	}
	else if (!strcmp(type, "audio/x-ac3") || !strcmp(type, "audio/x-eac3")) {
		GST_INFO_OBJECT (self, "MIMETYPE %s",type);
		bypass = BYPASS_AC3;
	}
	else if (!strcmp(type, "audio/x-private1-dts")) {
		GST_INFO_OBJECT (self, "MIMETYPE %s (DVD Audio - 2 byte skipping)",type);
		bypass = BYPASS_DTS;
		self->skip = 2;
	}
	else if (!strcmp(type, "audio/x-private1-ac3")) {
		GST_INFO_OBJECT (self, "MIMETYPE %s (DVD Audio - 2 byte skipping)",type);
		bypass = BYPASS_AC3;
		self->skip = 2;
	}
	else if (!strcmp(type, "audio/x-private1-lpcm")) {
		GST_INFO_OBJECT (self, "MIMETYPE %s (DVD Audio)",type);
		bypass = BYPASS_LPCM;
	}
	else if (!strcmp(type, "audio/x-raw-int")) {
		printf("X-RAW-INT ->\n");
		GST_INFO_OBJECT (self, "MIMETYPE %s", type);
		gint endianess = G_BIG_ENDIAN;
		gst_structure_get_int (structure, "endianness", &endianess);

		gint number_of_channels;
		gst_structure_get_int (structure, "channels", &number_of_channels);

		gint samples_per_second;
		gst_structure_get_int (structure, "rate", &samples_per_second);

		gst_structure_get_int (structure, "depth", &self->pcm_bits_per_sample);

		const unsigned char clpcm_prv[14] = {
			0xA0,   //sub_stream_id
			0, 0,   //resvd and UPC_EAN_ISRC stuff, unused
			0x0A,   //private header length
			0, 9,   //first_access_unit_pointer
			0x00,   //emph,rsvd,stereo,downmix
			0x0F,   //quantisation word length 1,2
			0x0F,   //audio sampling freqency 1,2
			0,      //resvd, multi channel type
			0,      //bit shift on channel GR2, assignment
			0x80,   //dynamic range control
			0, 0    //resvd for copyright management
		};

		self->runtime_header_data_size = sizeof(clpcm_prv);
		self->runtime_header_data = 
			(guint8*) malloc(sizeof(guint8) * self->runtime_header_data_size);

		memcpy(self->runtime_header_data, clpcm_prv, self->runtime_header_data_size);

		//figure out size of subframe
		//and set up sample rate
		switch(samples_per_second) {
			case 48000:             self->pcm_sub_frame_len = 40;
					                break;
			case 96000:             self->runtime_header_data[8] |= 0x10;
					                self->pcm_sub_frame_len = 80;
					                break;
			case 192000:            self->runtime_header_data[8] |= 0x20;
					                self->pcm_sub_frame_len = 160;
					                break;
			case 44100:             self->runtime_header_data[8] |= 0x80;
					                self->pcm_sub_frame_len = 40;
					                break;
			case 88200:             self->runtime_header_data[8] |= 0x90;
					                self->pcm_sub_frame_len = 80;
					                break;
			case 176400:            self->runtime_header_data[8] |= 0xA0;
					                self->pcm_sub_frame_len = 160;
					                break;
			default:                break;
		}

		self->pcm_sub_frame_len *= number_of_channels;
		self->pcm_sub_frame_len *= (self->pcm_bits_per_sample / 8);

		//rewrite PES size to have as many complete subframes per PES as we can
		self->pcm_sub_frame_per_pes = ((2048-18/*sizeof(lpcm_pes)*/)-14/*sizeof(lpcm_prv)*/)/self->pcm_sub_frame_len;
		self->pcm_sub_frame_len *= self->pcm_sub_frame_per_pes;

		//set number of channels
		self->runtime_header_data[10]  = number_of_channels - 1;

		printf("X-RAW-INT - BITS %d\n", self->pcm_bits_per_sample);

		switch(self->pcm_bits_per_sample) {
			case    16:      break;
			case    24:     self->runtime_header_data[7] |= 0x20;
					        break;
			default:        printf("inappropriate bits per sample (%d) - must be 16 or 24\n", self->pcm_bits_per_sample);
					        break;
		}

		if (endianess == G_BIG_ENDIAN) {
			printf("X-RAW-INT - BIG_ENDIAN\n");
			bypass = BYPASS_PCMB;
		}
		else if (endianess == G_LITTLE_ENDIAN) {
			printf("X-RAW-INT - LITTLE_ENDIAN\n");
			bypass = BYPASS_PCML;
		}
		else 
			return FALSE;

		printf("X-RAW-INT <-\n");
	}
	else if (!strcmp(type, "audio/x-dts")) {
		GST_INFO_OBJECT (self, "MIMETYPE %s",type);
		bypass = BYPASS_DTS;
	}

	else if (!strcmp(type, "audio/x-wma") || !strcmp(type, "audio/x-ms-wma")) {
		GST_INFO_OBJECT (self, "MIMETYPE %s",type);

		const GValue *codec_data = gst_structure_get_value (structure, "codec_data");
		guint8 *h      = GST_BUFFER_DATA(gst_value_get_buffer (codec_data));
		guint32 h_size = GST_BUFFER_SIZE(gst_value_get_buffer (codec_data));

		// type_specific_data
		#define WMA_VERSION_1           0x160
		#define WMA_VERSION_2_9         0x161
		#define WMA_VERSION_9_PRO       0x162
		#define WMA_LOSSLESS            0x163
		guint16 codec_id = 0; 
		gint wmaversion;
		gst_structure_get_int (structure, "wmaversion", &wmaversion);
		//TODO: Need to check posible values
		switch(wmaversion) {
		//TODO: What code for lossless ?
		case 9:
			codec_id = WMA_VERSION_9_PRO;
			break;
		case 2:
			codec_id = WMA_VERSION_2_9 ;
			break;
		case 1:
		default:
			codec_id = WMA_VERSION_1;
			break;
		}

		self->initial_header_private_data_size = 104 + h_size;
		self->initial_header_private_data = 
			(guint8*) malloc(sizeof(guint8) * self->initial_header_private_data_size);
		memset (self->initial_header_private_data, 0, self->initial_header_private_data_size);

		guint8 ASF_Stream_Properties_Object[16] =
			{0x91,0x07,0xDC,0xB7,0xB7,0xA9,0xCF,0x11,0x8E,0xE6,0x00,0xC0,0x0C,0x20,0x53,0x65};
		memcpy(self->initial_header_private_data + 0, ASF_Stream_Properties_Object, 16); // ASF_Stream_Properties_Object

		memcpy(self->initial_header_private_data + 16, &self->initial_header_private_data_size, 4); //FrameDateLength

		guint32 sizehi = 0;
		memcpy(self->initial_header_private_data + 20, &sizehi, 4); // sizehi (not used)

		guint8 ASF_Audio_Media[16] =
			{0x40,0x9E,0x69,0xF8,0x4D,0x5B,0xCF,0x11,0xA8,0xFD,0x00,0x80,0x5F,0x5C,0x44,0x2B}; 
		memcpy(self->initial_header_private_data + 24, ASF_Audio_Media, 16); //ASF_Audio_Media

		guint8 ASF_Audio_Spread[16] =
			{0x50,0xCD,0xC3,0xBF,0x8F,0x61,0xCF,0x11,0x8B,0xB2,0x00,0xAA,0x00,0xB4,0xE2,0x20}; 
		memcpy(self->initial_header_private_data + 40, ASF_Audio_Spread, 16); //ASF_Audio_Spread

		memset(self->initial_header_private_data + 56, 0, 4); // time_offset (not used)
		memset(self->initial_header_private_data + 60, 0, 4); // time_offset_hi (not used)
		
		guint32 type_specific_data_length = 18 + h_size;
		memcpy(self->initial_header_private_data + 64, &type_specific_data_length, 4); //type_specific_data_length

		guint32 error_correction_data_length = 8;
		memcpy(self->initial_header_private_data + 68, &error_correction_data_length, 4); //error_correction_data_length

		guint16 flags = 1; // stream_number + encrypted flags
		memcpy(self->initial_header_private_data + 72, &flags, 2); //flags

		guint32 reserved = 0;
		memcpy(self->initial_header_private_data + 74, &reserved, 4); // reserved


		memcpy(self->initial_header_private_data + 78, &codec_id, 2); //codec_id

		gint16 number_of_channels;
		gst_structure_get_int (structure, "channels", &number_of_channels);
		memcpy(self->initial_header_private_data + 80, &number_of_channels, 2); //number_of_channels

		guint32 samples_per_second;
		gst_structure_get_int (structure, "rate", &samples_per_second);
		printf("samples_per_second = %d\n", samples_per_second);
		memcpy(self->initial_header_private_data + 82, &samples_per_second, 4); //samples_per_second

		guint32 average_number_of_bytes_per_second;
		gst_structure_get_int (structure, "bitrate", &average_number_of_bytes_per_second);
		average_number_of_bytes_per_second /= 8;
		printf("average_number_of_bytes_per_second = %d\n", average_number_of_bytes_per_second);
		memcpy(self->initial_header_private_data + 86, &average_number_of_bytes_per_second, 4); //average_number_of_bytes_per_second

		gint16 block_alignment;
		gst_structure_get_int (structure, "block_align", &block_alignment);
		printf("block_alignment = %d\n", block_alignment);
		memcpy(self->initial_header_private_data + 90, &block_alignment, 2); //block_alignment

		gint16 bits_per_sample;
		gst_structure_get_int (structure, "depth", &bits_per_sample);
		printf("bits_per_sample = %d\n", bits_per_sample);
		memcpy(self->initial_header_private_data + 92, &bits_per_sample, 2); //bits_per_sample

		memcpy(self->initial_header_private_data + 94, &h_size, 2);

		memcpy(self->initial_header_private_data + 96, h, h_size);

		self->initial_header_private_data_valid = TRUE;
		bypass = BYPASS_WMA;
	}
	else if (!strcmp(type, "audio/x-flac")) {
		GST_INFO_OBJECT (self, "MIMETYPE %s",type);
		bypass = BYPASS_FLAC;
	}
	else {
		GST_ELEMENT_ERROR (self, STREAM, TYPE_NOT_FOUND, (NULL), ("unimplemented stream type %s", type));
		return FALSE;
	}

	GST_INFO_OBJECT(self, "setting dvb mode 0x%02x\n", bypass);

	if (self->use_set_encoding)
	{
#ifdef AUDIO_SET_ENCODING
		unsigned int encoding = bypass_to_encoding(bypass);
		if (ioctl(self->fd, AUDIO_SET_ENCODING, encoding) < 0) {
			GST_ELEMENT_WARNING (self, STREAM, DECODE, (NULL), ("hardware decoder can't be set to encoding %i.", encoding));
		}
#endif
	}
	else
	{
		if (ioctl(self->fd, AUDIO_SET_BYPASS_MODE, bypass) < 0) {
			if (bypass == BYPASS_DTS) {
				GST_ELEMENT_ERROR (self, STREAM, TYPE_NOT_FOUND, (NULL), ("hardware decoder can't be set to bypass mode type %s", type));
				return FALSE;
			}
			GST_ELEMENT_WARNING (self, STREAM, DECODE, (NULL), ("hardware decoder can't be set to bypass mode %i.", bypass));
		}
	}
	self->bypass = bypass;
	
	printf("[A] SET_CAPS <- TRUE\n");
	return TRUE;
}

static void
queue_push(queue_entry_t **queue_base, guint8 *data, size_t len)
{
	queue_entry_t *entry = malloc(sizeof(queue_entry_t)+len);
	queue_entry_t *last  = *queue_base;
	guint8        *d     = (guint8*)(entry+1);
	memcpy(d, data, len);
	entry->bytes  = len;
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

static void
queue_pop(queue_entry_t **queue_base)
{
	queue_entry_t *base = *queue_base;
	*queue_base = base->next;
	free(base);
}

static int
queue_front(queue_entry_t **queue_base, guint8 **data, size_t *bytes)
{
	if (!*queue_base) {
		*bytes = 0;
		*data  = 0;
	}
	else {
		queue_entry_t *entry = *queue_base;
		*bytes = entry->bytes - entry->offset;
		*data  = ((guint8*)(entry+1))+entry->offset;
	}
	return *bytes;
}

static gboolean
gst_dvbaudiosink_event (GstBaseSink * sink, GstEvent * event)
{
	GstDVBAudioSink *self = GST_DVBAUDIOSINK (sink);
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
		ioctl(self->fd, AUDIO_CLEAR_BUFFER);
		GST_OBJECT_LOCK(self);
		while(self->queue)
			queue_pop(&self->queue);
		self->timestamp = 0;
		self->no_write &= ~1;
		GST_OBJECT_UNLOCK(self);
		break;
	case GST_EVENT_EOS:
	{
		struct pollfd pfd[2];
		int retval;
		printf("[A] GST_EVENT_EOS\n");

		//Notify the player that no addionional data will be injected
#ifdef AUDIO_FLUSH
		ioctl(self->fd, AUDIO_FLUSH, 1/*NONBLOCK*/);
#endif

		pfd[0].fd = READ_SOCKET(self);
		pfd[0].events = POLLIN;
		pfd[1].fd = self->fd;
		pfd[1].events = POLLIN;

		GST_PAD_PREROLL_UNLOCK (sink->sinkpad);
		while (1) {
			retval = poll(pfd, 2, 250);
			printf("[A] poll %d\n", retval);
			if (retval < 0) {
				printf("poll in EVENT_EOS\n");
				ret=FALSE;
				break;
			}

			if (pfd[0].revents & POLLIN) {
				printf("wait EOS aborted!!\n");
				ret=FALSE;
				break;
			}

			if (pfd[1].revents & POLLIN) {
				printf("got buffer empty from driver!\n");
				break;
			}

			if (sink->flushing) {
				printf("wait EOS flushing!!\n");
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

		if (pftype == DM) //TODO: What is the purpose of this code?
		{
			int video_fd = open("/dev/dvb/adapter0/video0", O_RDWR);
			if (fmt == GST_FORMAT_TIME) {
				if ( rate > 1 )
					skip = (int) rate;
				else if ( rate < 1 )
					repeat = 1.0/rate;
				ret = ioctl(video_fd, VIDEO_SLOWMOTION, repeat);
				ret = ioctl(video_fd, VIDEO_FAST_FORWARD, skip);
				//gst_segment_set_newsegment_full (&dec->segment, update, rate, applied_rate, dformat, cur, stop, time);
			}
			close(video_fd);
		}
		break;
	}

	default:
		break;
	}

	return ret;
}

#define ASYNC_WRITE(data, len) do { \
		switch(gst_dvbaudiosink_async_write(self, data, len)) { \
		case -1: goto poll_error; \
		case -3: goto write_error; \
		default: break; \
		} \
	} while(0)

static int
gst_dvbaudiosink_async_write(GstDVBAudioSink *self, unsigned char *data, unsigned int len)
{
	unsigned int written=0;
	struct pollfd pfd[2];

	pfd[0].fd = READ_SOCKET(self);
	pfd[0].events = POLLIN;
	pfd[1].fd = self->fd;
	pfd[1].events = POLLOUT;

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
#if CHECK_DRAIN
		if (poll(pfd, 2, -1) == -1) {
			if (errno == EINTR)
				continue;
			return -1;
		}
#else
		pfd[1].revents = POLLOUT;
#endif
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
		if (pfd[1].revents & POLLOUT) {
			size_t queue_entry_size;
			guint8 *queue_data;
			GST_OBJECT_LOCK(self);
			if (queue_front(&self->queue, &queue_data, &queue_entry_size)) {
				int wr = write(self->fd, queue_data, queue_entry_size);
				if ( self->dump_fd > 0 )
					write(self->dump_fd, queue_data, queue_entry_size);
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
			if ( self->dump_fd > 0 )
				write(self->dump_fd, data+written, len - written);
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

#define WRITE_COMPLETE_PACKAGE

static inline size_t
buildPesHeader(unsigned char *data, int size, unsigned long long int timestamp, unsigned char stream_id, gboolean late_initial_header, unsigned int pcm_sub_frame_len)
{
	unsigned char *pes_header = data;
	size_t pes_header_size;

	pes_header[0] = 0x00;
	pes_header[1] = 0x00;
	pes_header[2] = 0x01;
	pes_header[3] = stream_id;

	if (stream_id == 0xBD && pcm_sub_frame_len > 0) { //PCM
		//pes_header[4] = 0x07; //pes length
		//pes_header[5] = 0xF1; //pes length
		pes_header[4] =  ((pcm_sub_frame_len+26)>>8) & 0xFF; // ((pcm_sub_frame_len+(18/*sizeof(lpcm_pes)*/-6)+14/*sizeof(lpcm_prv)*/)>>8) & 0xFF;
		pes_header[5] =   (pcm_sub_frame_len+26)    & 0xFF; // (pcm_sub_frame_len+(18/*sizeof(lpcm_pes)*/-6)+14/*sizeof(lpcm_prv)*/)     & 0xFF;
		
		pes_header[6] = 0x81; //fixed
		
		//printf("[A] LATE_INITIAL_HEADER = %d\n", late_initial_header);
		
		pes_header[7] = 0x01;
		pes_header[8] = 0x09; //fixed
		
		pes_header[9] = 0x21; //PTS marker bits
		pes_header[10] = 0x00; //PTS marker bits
		pes_header[11] = 0x01; //PTS marker bits
		pes_header[12] = 0x00; //PTS marker bits
		pes_header[13] = 0x01; //PTS marker bits
		
		pes_header[14] = 0xFF; //first pes only, 0xFF after
		pes_header[15] = 0xFF; //first pes only, 0xFF after
		pes_header[16] = 0xFF; //first pes only, 0xFF after
		
		if (late_initial_header) {
			pes_header[7] = 0x81; //fixed
			
			pes_header[14] = 0x1E; //first pes only, 0xFF after
			pes_header[15] = 0x60; //first pes only, 0xFF after
			pes_header[16] = 0x0A; //first pes only, 0xFF after
		}
		
		pes_header[17] = 0xFF;
		
		pes_header_size = 18;
		
		return pes_header_size;
	}

	pes_header[7] = 0x00;
	pes_header[8] = 0x00;
	pes_header_size = 9;

		/* do we have a timestamp? */
	if (timestamp != GST_CLOCK_TIME_NONE) {
		unsigned long long pts = timestamp * 9LL / 100000 /* convert ns to 90kHz */;

		pes_header[7] = 0x80;
		pes_header[8] = 0x05;

		pes_header[9]  = 0x21 | ((pts >> 29) & 0x0E);
		pes_header[10] = pts >> 22;
		pes_header[11] = 0x01 | ((pts >> 14) & 0xFE);
		pes_header[12] = pts >> 7;
		pes_header[13] = 0x01 | ((pts << 1) & 0xFE);

		pes_header_size = 14;

		if (hwtype == DM7025) {  // DM7025 needs DTS in PES header
			int64_t dts = pts; // what to use as DTS-PTS offset?
			pes_header[7] = 0xC0;
			pes_header[8] = 0x0A;
			pes_header[9] |= 0x10;

			pes_header[14] = 0x11 | ((dts >> 29) & 0x0E);
			pes_header[15] = dts >> 22;
			pes_header[16] = 0x01 | ((dts >> 14) & 0xFE);
			pes_header[17] = dts >> 7;
			pes_header[18] = 0x01 | ((dts << 1) & 0xFE);

			pes_header_size = 19;
		}
	}

	pes_header[4] = (size + pes_header_size - 6) >> 8;
	pes_header[5] = (size  + pes_header_size - 6) & 0xFF;

	pes_header[6] = 0x80;

	return pes_header_size;
}

#define MPEG_AUDIO_PES_START_CODE           0xc0
#define PRIVATE_STREAM_1_PES_START_CODE         0xbd
#define MAX_PES_PACKET_SIZE                     65400

//#define DEBUG_EXT

static GstFlowReturn
gst_dvbaudiosink_render (GstBaseSink * sink, GstBuffer * buffer)
{
	GstDVBAudioSink *self      = GST_DVBAUDIOSINK (sink);
	unsigned int     data_len  = GST_BUFFER_SIZE (buffer) - self->skip;
	unsigned char   *data      = GST_BUFFER_DATA (buffer) + self->skip;
	long long        timestamp = GST_BUFFER_TIMESTAMP(buffer);
	long long        duration  = GST_BUFFER_DURATION(buffer);
	gboolean         late_initial_header = FALSE;

#ifdef DEBUG_EXT
	printf("gst_dvbaudiosink_render 0\n");
#endif

	if (self->bypass == BYPASS_UNKNOWN) {
		GST_ELEMENT_ERROR (self, STREAM, FORMAT, (NULL), ("hardware decoder not setup (no caps in pipeline?)"));
		return GST_FLOW_ERROR;
	}

	if (self->fd < 0)
		return GST_FLOW_OK;

	if (duration != -1 && timestamp != -1) {
		if (self->timestamp == 0)
			self->timestamp = timestamp;
		else
			timestamp = self->timestamp;
		self->timestamp += duration;
	}
	else
		self->timestamp = 0;

	unsigned char start_code = MPEG_AUDIO_PES_START_CODE;
	if (self->bypass == BYPASS_AC3) {
		start_code = PRIVATE_STREAM_1_PES_START_CODE;
	}
	else if (self->bypass == BYPASS_PCML || self->bypass == BYPASS_PCMB) {
		start_code = PRIVATE_STREAM_1_PES_START_CODE;
	}

#ifdef DEBUG_EXT
	printf("gst_dvbaudiosink_render 1\n");
#endif

	if (self->initial_header)
	{
		if (self->bypass == BYPASS_PCML || self->bypass == BYPASS_PCMB)
		{
			late_initial_header = TRUE;
		}
		else if (self->bypass == BYPASS_WMA && self->initial_header_private_data_valid == TRUE)
		{
			unsigned char pes_header_initial[PES_MAX_HEADER_SIZE];
			size_t pes_header_size_initial;
			
			pes_header_size_initial = buildPesHeader(pes_header_initial, self->initial_header_private_data_size, 0, 0, FALSE/*late_initial_header*/, 0/*pcm_sub_frame_len*/);

#ifdef WRITE_COMPLETE_PACKAGE
			//printf("--> %d bytes\n", pes_header_size_initial + self->initial_header_private_data_size);
			int write_buffer_size = pes_header_size_initial + self->initial_header_private_data_size;
			unsigned char *write_buffer = 
				(unsigned char*) malloc(sizeof(unsigned char) * (write_buffer_size));
			memcpy(write_buffer, pes_header_initial, pes_header_size_initial);
			memcpy(write_buffer + pes_header_size_initial, self->initial_header_private_data, self->initial_header_private_data_size);
			ASYNC_WRITE(write_buffer, write_buffer_size);
			free(write_buffer);
#else
			ASYNC_WRITE(pes_header_initial, pes_header_size_initial);
			ASYNC_WRITE(self->initial_header_private_data, self->initial_header_private_data_size);
#endif
			
			free(self->initial_header_private_data);
			self->initial_header_private_data_valid = FALSE;
		}
		self->initial_header = FALSE;
	}

#ifdef DEBUG_EXT
	printf("gst_dvbaudiosink_render - initial header written\n");
	printf("gst_dvbaudiosink_render - write pes packages\n");
#endif

	/* LPCM workaround.. we also need the first two byte of the lpcm header.. (substreamid and num of frames) 
	   i dont know why the mpegpsdemux strips out this two bytes... */
	if (self->bypass == BYPASS_LPCM && (data[0] < 0xA0 || data[0] > 0xAF)) {
		if (data[-2] >= 0xA0 && data[-2] <= 0xAF) {
			data -= 2;
			data_len += 2;
		}
	}

	if (self->bypass == BYPASS_DTS) {  // dts
		int pos=0;
		while((pos+3) < data_len) {
			if (!strcmp((char*)(data+pos), "\x64\x58\x20\x25")) {  // is DTS-HD ?
				data_len = pos;
				break;
			}
			++pos;
		}
	}

	if (self->aac_adts_header_valid)
		data_len += 7;

#if 0
	printf("->Timestamp: %lld\n", timestamp);
#endif

	// The Player tells us that most of the pts is invalid for wma
	//TODO: This can only be a quick hack and has to be investigated further
	if (self->bypass == BYPASS_WMA) 
		timestamp = 0;

	unsigned char pes_header[PES_MAX_HEADER_SIZE];
	//memset (pes_header, '0', PES_MAX_HEADER_SIZE);
	int pes_header_size = 0;

	unsigned int data_position = 0;
	//int i = 0;
	//printf("L ->\n");
	while (data_position < data_len) {
	//printf("L\n");
#define SPLIT_TO_BIG_PACKETS
#ifdef SPLIT_TO_BIG_PACKETS
		unsigned int pes_packet_size = (data_len - data_position) <= MAX_PES_PACKET_SIZE ?
										(data_len - data_position) : MAX_PES_PACKET_SIZE;
#else
		unsigned int pes_packet_size = (data_len - data_position);
#endif

		// For PCM the max package size is not the pes size but the subframelen
		if (self->bypass == BYPASS_PCML || self->bypass == BYPASS_PCMB) {
			if (self->pcm_break_buffer_size > 0)
			{ // The breakbuffer is full, this means we have to attach the buffer in front of the normale data block
				// Create a new buffer, not we have to free this on our own
				// To detect if we have to free it, lets do not reset the pcm_break_buffer_Size
				//  here, but after writing the bytes
#if USE_DATA_TMP
				unsigned char   *tmp_data = (unsigned char*) malloc(self->pcm_sub_frame_len * sizeof(unsigned char));
				memcpy(tmp_data, self->pcm_break_buffer, self->pcm_break_buffer_size);
				memcpy(tmp_data + self->pcm_break_buffer_size, data, self->pcm_sub_frame_len - self->pcm_break_buffer_size);
				data = tmp_data; // TODO: actually we could reuse the breakbuffer for this, will spare us malloc and free calls
#else
				memcpy(self->pcm_break_buffer + self->pcm_break_buffer_size, data, self->pcm_sub_frame_len - self->pcm_break_buffer_size);
				data = self->pcm_break_buffer;
#endif
				pes_packet_size = self->pcm_sub_frame_len;
			}
			
			if (pes_packet_size < self->pcm_sub_frame_len)
			{ //If we dont have enough frames left than save them to the breakbuffer
				self->pcm_break_buffer_size = pes_packet_size;
				memcpy(self->pcm_break_buffer, data + data_position, self->pcm_break_buffer_size);
#ifdef DEBUG_EXT
				printf("PCM %s - Unplayed=%d\n", __FUNCTION__, pes_packet_size);
#endif
				break;
			}
			else
			{ // We have enough data so set the package size to subframelen
				pes_packet_size = self->pcm_sub_frame_len;
			}
		}

		//unsigned char pes_header[PES_MAX_HEADER_SIZE];
		//memset (pes_header, '0', PES_MAX_HEADER_SIZE);
		//int pes_header_size = 0;

#ifdef DEBUG_EXT
		printf("gst_dvbaudiosink_render - build PESHeader\n");
#endif
		pes_header_size = buildPesHeader(pes_header, pes_packet_size, 
			timestamp, start_code, late_initial_header, self->pcm_sub_frame_len);

		if (self->aac_adts_header_valid) {
			self->aac_adts_header[3] &= 0xC0;
			/* frame size over last 2 bits */
			self->aac_adts_header[3] |= (pes_packet_size & 0x1800) >> 11;
			/* frame size continued over full byte */
			self->aac_adts_header[4] = (pes_packet_size & 0x1FF8) >> 3;
			/* frame size continued first 3 bits */
			self->aac_adts_header[5] = (pes_packet_size & 7) << 5;
			/* buffer fullness (0x7FF for VBR) over 5 last bits */
			self->aac_adts_header[5] |= 0x1F;
			/* buffer fullness (0x7FF for VBR) continued over 6 first bits + 2 zeros for
			 * number of raw data blocks */
			self->aac_adts_header[6] = 0xFC;
			memcpy(pes_header + pes_header_size, self->aac_adts_header, 7);
			pes_header_size += 7;
			pes_packet_size -= 7;
		}

#if 0
			printf("--> BEFORE %d\n", pes_header_size + self->runtime_header_data_size + pes_packet_size);
			Hexdump(pes_header, pes_header_size);
			if (self->runtime_header_data_size > 0)
				Hexdump(self->runtime_header_data, self->runtime_header_data_size);
			Hexdump(data + data_position, /*128*/ pes_packet_size);
			printf("<--\n");
#endif

		if (self->bypass == BYPASS_PCML) {
			if (self->pcm_bits_per_sample == 16) {
				int i;
				for(i=0; i<pes_packet_size; i+=2) {
					int n = data_position + i;
					unsigned char tmp;
					tmp=data[n];
					data[n]=data[n+1];
					data[n+1]=tmp;
				}
			} else {
				int i;
				//A1cA1bA1a-B1cB1bB1a-A2cA2bA2a-B2cB2bB2a to A1aA1bB1aB1b.A2aA2bB2aB2b-A1cB1cA2cB2c
				for(i=0; i<pes_packet_size; i+=12) {
					int n = data_position + i;
					unsigned char tmp[12];
					tmp[ 0]=data[n+2];
					tmp[ 1]=data[n+1];
					tmp[ 8]=data[n+0];
					tmp[ 2]=data[n+5];
					tmp[ 3]=data[n+4];
					tmp[ 9]=data[n+3];
					tmp[ 4]=data[n+8];
					tmp[ 5]=data[n+7];
					tmp[10]=data[n+6];
					tmp[ 7]=data[n+11];
					tmp[ 8]=data[n+10];
					tmp[11]=data[n+9];
					memcpy(&data[n],tmp,12);
				}
			}
		}

#if 0
			printf("--> %d\n", pes_header_size + self->runtime_header_data_size + pes_packet_size);
			Hexdump(pes_header, pes_header_size);
			if (self->runtime_header_data_size > 0)
				Hexdump(self->runtime_header_data, self->runtime_header_data_size);
			Hexdump(data + data_position, /*128*/ pes_packet_size);
			printf("<--\n");
#endif

//printf("W\n");

#ifdef WRITE_COMPLETE_PACKAGE
		int write_buffer_size = pes_header_size + self->runtime_header_data_size + pes_packet_size;
		unsigned char *write_buffer = 
			(unsigned char*) malloc(sizeof(unsigned char) * (write_buffer_size));
		memcpy(write_buffer, pes_header, pes_header_size);
		if (self->runtime_header_data_size > 0)
			memcpy(write_buffer + pes_header_size, self->runtime_header_data, self->runtime_header_data_size);
		memcpy(write_buffer + pes_header_size + self->runtime_header_data_size, data + data_position, pes_packet_size);
		ASYNC_WRITE(write_buffer, write_buffer_size);
		free(write_buffer);
#else
		ASYNC_WRITE(pes_header, pes_header_size);
		if (self->runtime_header_data_size > 0)
			ASYNC_WRITE(self->runtime_header_data, self->runtime_header_data_size);
		ASYNC_WRITE(data + data_position, pes_packet_size);
#endif
		if (self->aac_adts_header_valid){
			pes_packet_size += 7;
		}		

#ifdef DEBUG_EXT
	printf("gst_dvbaudiosink_render - pes package written\n");
#endif

		if (late_initial_header) {
			if (self->bypass == BYPASS_PCML || self->bypass == BYPASS_PCMB) {
				late_initial_header = FALSE;
			}
		}
		
		data_position += pes_packet_size;
		
		if (self->bypass == BYPASS_PCML || self->bypass == BYPASS_PCMB) {
		
			//increment err... subframe count?
			self->runtime_header_data[1] = ((self->runtime_header_data[1]+self->pcm_sub_frame_per_pes) & 0x1F);
		
			if (self->pcm_break_buffer_size > 0)
			{
#if USE_DATA_TMP
				free(data);
#endif
				// Reset data pointer
				data      = GST_BUFFER_DATA (buffer) + self->skip;
				
				data_position -= self->pcm_break_buffer_size;
				
				self->pcm_break_buffer_size = 0;
			}
		}
	}
	//printf("L <-\n");

#ifdef DEBUG_EXT
	printf("gst_dvbaudiosink_render - all pes packages written\n");
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
}

static gboolean
gst_dvbaudiosink_start (GstBaseSink * basesink)
{
	GstDVBAudioSink *self = GST_DVBAUDIOSINK (basesink);
	gint control_sock[2];

	GST_DEBUG_OBJECT (self, "start");

	if (socketpair(PF_UNIX, SOCK_STREAM, 0, control_sock) < 0) {
		perror("socketpair");
		goto socket_pair;
	}

	READ_SOCKET (self)  = control_sock[0];
	WRITE_SOCKET (self) = control_sock[1];

	fcntl (READ_SOCKET (self),  F_SETFL, O_NONBLOCK);
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
gst_dvbaudiosink_stop (GstBaseSink * basesink)
{
	GstDVBAudioSink *self = GST_DVBAUDIOSINK (basesink);

	GST_DEBUG_OBJECT (self, "stop");

	if (self->fd >= 0) {
		if (pftype == DM) //TODO: What is the purpose of this code?
		{
			int video_fd = open("/dev/dvb/adapter0/video0", O_RDWR);

			ioctl (self->fd, AUDIO_STOP);
			ioctl (self->fd, AUDIO_SELECT_SOURCE, AUDIO_SOURCE_DEMUX);

			//TODO: This seems to me like a hack?!
			if ( video_fd > 0 ) {
				ioctl (video_fd, VIDEO_SLOWMOTION, 0);
				ioctl (video_fd, VIDEO_FAST_FORWARD, 0);
				close (video_fd);
			}
		}
		close (self->fd);
	}

	if (self->dump_fd > 0)
		close (self->dump_fd);

	while(self->queue)
		queue_pop (&self->queue);

	close (READ_SOCKET (self));
	close (WRITE_SOCKET (self));
	READ_SOCKET (self) = -1;
	WRITE_SOCKET (self) = -1;

	return TRUE;
}

static GstStateChangeReturn
gst_dvbaudiosink_change_state (GstElement * element, GstStateChange transition)
{
	GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
	GstDVBAudioSink *self = GST_DVBAUDIOSINK (element);

	switch (transition) {
	case GST_STATE_CHANGE_NULL_TO_READY:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_NULL_TO_READY");
		break;
	case GST_STATE_CHANGE_READY_TO_PAUSED:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_READY_TO_PAUSED");
		GST_OBJECT_LOCK(self);
		self->no_write |= 4;
		GST_OBJECT_UNLOCK(self);

		if (self->dump_filename)
				self->dump_fd = open(self->dump_filename, O_RDWR|O_CREAT, 0555);

		self->fd = open("/dev/dvb/adapter0/audio0", O_RDWR|O_NONBLOCK);

		if (self->fd) {
			ioctl(self->fd, AUDIO_CLEAR_BUFFER, NULL);
			ioctl(self->fd, AUDIO_SELECT_SOURCE, AUDIO_SOURCE_MEMORY);
			ioctl(self->fd, AUDIO_PLAY);
			ioctl(self->fd, AUDIO_PAUSE);
		}
		break;
	case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
		GST_DEBUG_OBJECT (self,"GST_STATE_CHANGE_PAUSED_TO_PLAYING");
		ioctl(self->fd, AUDIO_CONTINUE);
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
		ioctl(self->fd, AUDIO_PAUSE);
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
	return gst_element_register (plugin, "dvbaudiosink",
						 GST_RANK_PRIMARY,
						 GST_TYPE_DVBAUDIOSINK);
}

/* this is the structure that gstreamer looks for to register plugins
 *
 * exchange the strings 'plugin' and 'Template plugin' with you plugin name and
 * description
 */
GST_PLUGIN_DEFINE (
	GST_VERSION_MAJOR,
	GST_VERSION_MINOR,
	"dvb_audio_out",
	"DVB Audio Output",
	plugin_init,
	VERSION,
	"LGPL",
	"GStreamer",
	"http://gstreamer.net/"
)

