glib_enum_prefix = gst_dvbsink

include $(top_srcdir)/common/glib-gen.mak

built_sources = gstdvbsink-marshal.c
built_headers = gstdvbsink-marshal.h

BUILT_SOURCES = $(built_sources) $(built_headers)
# plugindir is set in configure

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = gstdvbsink-marshal.list

plugin_LTLIBRARIES = libgstdvbvideosink.la libgstdvbaudiosink.la

# for the next set of variables, rename the prefix if you renamed the .la

# sources used to compile this plug-in
libgstdvbvideosink_la_SOURCES = gstdvbvideosink.c $(built_sources)
libgstdvbaudiosink_la_SOURCES = gstdvbaudiosink.c $(built_sources)

# flags used to compile this plugin
# add other _CFLAGS and _LIBS as needed
libgstdvbvideosink_la_CFLAGS = $(GST_CFLAGS)
libgstdvbvideosink_la_LIBADD = $(GST_LIBS) -lgstbase-0.10 -lgstaudio-0.10
libgstdvbvideosink_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libgstdvbaudiosink_la_CFLAGS = $(GST_CFLAGS)
libgstdvbaudiosink_la_LIBADD = $(GST_LIBS) -lgstbase-0.10 -lgstaudio-0.10
libgstdvbaudiosink_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

# headers we need but don't want installed
noinst_HEADERS = gstdvbvideosink.h gstdvbaudiosink.h

