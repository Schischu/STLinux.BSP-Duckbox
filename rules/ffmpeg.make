# -*-makefile-*-
#
# Copyright (C) 2006 by Erwin Rol
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#
#
#
# We provide this package
#
PACKAGES-$(PTXCONF_FFMPEG) += ffmpeg

#
# Paths and names
#
FFMPEG_VERSION	:= 1.2.1
FFMPEG_MD5	:= 0d02bf61ceeb030ccb8650638d876d75
FFMPEG		:= ffmpeg-$(FFMPEG_VERSION)
FFMPEG_SUFFIX	:= tar.gz
FFMPEG_URL	:= http://www.ffmpeg.org/releases/$(FFMPEG).$(FFMPEG_SUFFIX)
FFMPEG_SOURCE	:= $(SRCDIR)/$(FFMPEG).$(FFMPEG_SUFFIX)
FFMPEG_DIR	:= $(BUILDDIR)/$(FFMPEG)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

FFMPEG_PATH	:= PATH=$(CROSS_PATH)
FFMPEG_ENV 	:= $(CROSS_ENV)

#
# autoconf
# Carefull, ffmpeg has a home grown configure, and not all autoconf options work!!! :-/
# for example it enables things by default and than only has a --disable-BLA option and no
# --enable-BLA option.
#
FFMPEG_AUTOCONF := --prefix=/usr
FFMPEG_AUTOCONF += --cross-prefix=$(COMPILER_PREFIX)
FFMPEG_AUTOCONF += --extra-cflags="$(CROSS_CPPFLAGS) $(CROSS_CFLAGS) -L$(SYSROOT)/usr/lib"
FFMPEG_AUTOCONF += --extra-ldflags="$(CROSS_LDFLAGS) -L$(SYSROOT)/usr/lib"
FFMPEG_AUTOCONF += --extra-libs="$(CROSS_LIBS) -lm"



ifdef PTXCONF_ARCH_SH_SH4
FFMPEG_AUTOCONF += \
		--cpu=sh4 \
		--target-os=linux \
		--arch=sh4 \
		--disable-doc \
		--disable-htmlpages \
		--disable-manpages \
		--disable-podpages \
		--disable-txtpages \
		--disable-asm \
		--disable-altivec \
		--disable-amd3dnow \
		--disable-amd3dnowext \
		--disable-mmx \
		--disable-mmxext \
		--disable-sse \
		--disable-sse2 \
		--disable-sse3 \
		--disable-ssse3 \
		--disable-sse4 \
		--disable-sse42 \
		--disable-avx \
		--disable-fma4 \
		--disable-armv5te \
		--disable-armv6 \
		--disable-armv6t2 \
		--disable-vfp \
		--disable-neon \
		--disable-vis \
		--disable-inline-asm \
		--disable-yasm \
		--disable-mips32r2 \
		--disable-mipsdspr1 \
		--disable-mipsdspr2 \
		--disable-mipsfpu \
		--disable-fast-unaligned \
		--disable-muxers \
		--enable-muxer=flac \
		--enable-muxer=mp3 \
		--enable-muxer=h261 \
		--enable-muxer=h263 \
		--enable-muxer=h264 \
		--enable-muxer=image2 \
		--enable-muxer=mpeg1video \
		--enable-muxer=mpeg2video \
		--enable-muxer=ogg \
		--disable-encoders \
		--enable-encoder=aac \
		--enable-encoder=h261 \
		--enable-encoder=h263 \
		--enable-encoder=h263p \
		--enable-encoder=ljpeg \
		--enable-encoder=mjpeg \
		--enable-encoder=mpeg1video \
		--enable-encoder=mpeg2video \
		--enable-encoder=png \
		--disable-decoders \
		--enable-decoder=aac \
		--enable-decoder=dvbsub \
		--enable-decoder=flac \
		--enable-decoder=h261 \
		--enable-decoder=h263 \
		--enable-decoder=h263i \
		--enable-decoder=h264 \
		--enable-decoder=iff_byterun1 \
		--enable-decoder=mjpeg \
		--enable-decoder=mp3 \
		--enable-decoder=mpeg1video \
		--enable-decoder=mpeg2video \
		--enable-decoder=png \
		--enable-decoder=theora \
		--enable-decoder=vorbis \
		--enable-parser=mjpeg \
		--enable-demuxer=mjpeg \
		--enable-protocol=file \
		--disable-indevs \
		--disable-outdevs \
		--enable-avresample \
		--enable-pthreads \
		--enable-bzlib \
		--disable-zlib \
		--disable-bsfs \
		--enable-librtmp
endif

ifdef PTXCONF_FFMPEG_SHARED
FFMPEG_AUTOCONF += --enable-shared
else
FFMPEG_AUTOCONF += --disable-shared
endif

ifdef PTXCONF_FFMPEG_STATIC
FFMPEG_AUTOCONF += --enable-static
else
FFMPEG_AUTOCONF += --disable-static
endif

ifdef PTXCONF_FFMPEG_PTHREADS
FFMPEG_AUTOCONF += --enable-pthreads
endif

ifndef PTXCONF_FFMPEG_FFSERVER
FFMPEG_AUTOCONF += --disable-ffserver
endif

ifndef PTXCONF_FFMPEG_FFPLAY
FFMPEG_AUTOCONF += --disable-ffplay
endif

ifdef PTXCONF_FFMPEG_SMALL
FFMPEG_AUTOCONF += --enable-small
endif

ifdef PTXCONF_FFMPEG_MEMALIGN_HACK
FFMPEG_AUTOCONF += --enable-memalign-hack
endif

ifndef PTXCONF_FFMPEG_STRIP
FFMPEG_AUTOCONF += --disable-strip
endif

ifdef PTXCONF_FFMPEG_GPROF
FFMPEG_AUTOCONF += --enable-gprof
endif

ifndef PTXCONF_FFMPEG_DEBUG
FFMPEG_AUTOCONF += --disable-debug
endif

ifdef PTXCONF_FFMPEG_GPL
FFMPEG_AUTOCONF += --enable-gpl
endif

ifdef PTXCONF_FFMPEG_NONFREE
FFMPEG_AUTOCONF += --enable-nonfree
endif

ifdef PTXCONF_FFMPEG_MP3LAME
FFMPEG_AUTOCONF += --enable-libmp3lame
endif

ifdef PTXCONF_FFMPEG_VORBIS
FFMPEG_AUTOCONF += --enable-libvorbis
endif

ifdef PTXCONF_FFMPEG_THEORA
FFMPEG_AUTOCONF += --enable-libtheora
endif

ifdef PTXCONF_FFMPEG_LIBFAAC
FFMPEG_AUTOCONF += --enable-libfaac
endif

ifdef PTXCONF_FFMPEG_LIBGSM
FFMPEG_AUTOCONF += --enable-libgsm
endif

ifdef PTXCONF_FFMPEG_LIBXVID
FFMPEG_AUTOCONF += --enable-libxvid
endif

ifdef PTXCONF_FFMPEG_LIBX264
FFMPEG_AUTOCONF += --enable-libx264
endif

ifndef PTXCONF_FFMPEG_LIBV4L2
FFMPEG_AUTOCONF += --disable-libv4l2
endif

ifndef PTXCONF_FFMPEG_NETWORK
FFMPEG_AUTOCONF += --disable-network
endif

ifndef PTXCONF_FFMPEG_ZLIB
FFMPEG_AUTOCONF += --enable-zlib
endif

ifndef PTXCONF_FFMPEG_PROTOCOL
FFMPEG_AUTOCONF += --disable-protocols
endif

# FIXME selectivly enable/disable decoders to reduce library size

#--disable-encoder=NAME   disables encoder NAME
#--enable-encoder=NAME    enables encoder NAME
#--disable-decoder=NAME   disables decoder NAME
#--enable-decoder=NAME    enables decoder NAME
#--disable-encoders       disables all encoders
#--disable-decoders       disables all decoders
#--disable-muxers         disables all muxers
#--disable-demuxers       disables all demuxers

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ffmpeg.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ffmpeg)
	@$(call install_fixup, ffmpeg,PRIORITY,optional)
	@$(call install_fixup, ffmpeg,SECTION,base)
	@$(call install_fixup, ffmpeg,AUTHOR,"Erwin Rol <ero@pengutronix.de>")
	@$(call install_fixup, ffmpeg,DESCRIPTION,missing)

	@$(call install_lib, ffmpeg, 0, 0, 0644, libavcodec)
	@$(call install_lib, ffmpeg, 0, 0, 0644, libavdevice)
	@$(call install_lib, ffmpeg, 0, 0, 0644, libavfilter)
	@$(call install_lib, ffmpeg, 0, 0, 0644, libavformat)
	@$(call install_lib, ffmpeg, 0, 0, 0644, libavresample)
	@$(call install_lib, ffmpeg, 0, 0, 0644, libavutil)
	@$(call install_lib, ffmpeg, 0, 0, 0644, libswresample)
	@$(call install_lib, ffmpeg, 0, 0, 0644, libswscale)

ifdef PTXCONF_FFMPEG_PP
	@$(call install_lib, ffmpeg, 0, 0, 0644, libpostproc)
endif

	@$(call install_finish, ffmpeg)

	@$(call touch)

# vim: syntax=make
