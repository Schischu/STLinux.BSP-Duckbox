# -*-makefile-*-
#
# Copyright (C) 2003 by Robert Schwebel <r.schwebel@pengutronix.de>
#                       Pengutronix <info@pengutronix.de>, Germany
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_XBMC) += xbmc

#
# Paths and names
#
ifdef PTXCONF_XBMC_VERSION_201301022234
XBMC_VERSION	:= 7a6cb7f49ae19dca3c48c40fa3bd20dc3c490e60
endif

XBMC		:= xbmc-$(XBMC_VERSION)
XBMC_URL	:= git://github.com/xbmc/xbmc.git
XBMC_SOURCE_GIT	:= $(SRCDIR)/xbmc.git
XBMC_DIR	:= $(BUILDDIR)/$(XBMC)
XBMC_LICENSE	:= xbmc

XBMC_DEV_VERSION	:= $(XBMC_VERSION)
XBMC_DEV_PKGDIR	:= $(XBMC_PKGDIR)

$(STATEDIR)/xbmc.get:
	@$(call targetinfo)
	
		if [ -d $(XBMC_SOURCE_GIT) ]; then \
			cd $(XBMC_SOURCE_GIT); \
			git pull -u origin master 2>&1 > /dev/null; \
			git checkout HEAD 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone  $(XBMC_URL) $(XBMC_SOURCE_GIT) 2>&1 > /dev/null; \
		fi; 2>&1 > /dev/null
	
		if [ ! "$(XBMC_VERSION)" == "HEAD" ]; then \
			cd $(XBMC_SOURCE_GIT); \
			git checkout $(XBMC_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi; 2>&1 > /dev/null
	
	@$(call touch)

PATH_PATCHES = $(subst :, ,$(PTXDIST_PATH_PATCHES))

$(STATEDIR)/xbmc.extract:
	@$(call targetinfo)
	
	rm -rf $(XBMC_DIR); \
	cp -a $(XBMC_SOURCE_GIT) $(XBMC_DIR); \
	rm -rf $(XBMC_DIR)/.git;
	
	@$(call patchin, XBMC)
	#cd $(XBMC_DIR) && sh autogen.sh
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XBMC_PATH	:= PATH=$(CROSS_PATH)
XBMC_ENV 	:= $(CROSS_ENV)

XBMC_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--prefix=/usr \
		TEXTUREPACKER_NATIVE_ROOT=/usr \
		SWIG_EXE=none \
		JRE_EXE=none \
		--disable-gl \
		--enable-glesv1 \
		--disable-gles \
		--disable-sdl \
		--enable-webserver \
		--enable-nfs \
		--disable-x11 \
		--disable-samba \
		--disable-mysql \
		--disable-joystick \
		--disable-rsxs \
		--disable-projectm \
		--disable-goom \
		--disable-afpclient \
		--disable-airplay \
		--disable-airtunes \
		--disable-dvdcss \
		--disable-hal \
		--disable-avahi \
		--disable-optical-drive \
		--disable-libbluray \
		--disable-texturepacker \
		--disable-udev \
		--disable-libusb \
		--disable-libcec \
		--enable-gstreamer \
		--disable-paplayer \
		--enable-gstplayer \
		--enable-dvdplayer \
		--disable-pulse \
		--disable-alsa \
		--disable-ssh \
		PYTHON=$(PTXDIST_SYSROOT_HOST)/bin/python2.7 \
		PY_PATH=$(SYSROOT)/usr \
		PYTHON_VERSION=2.7 \
		PKG_CONFIG=$(PTXDIST_SYSROOT_HOST)/bin/pkg-config \
		PKG_CONFIG_PATH=$(SYSROOT)/usr/lib/pkgconfig

XBMC_MAKE_OPS := $(PARALLELMFLAGS_BROKEN)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xbmc.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  xbmc)
	@$(call install_fixup, xbmc, PRIORITY,    optional)
	@$(call install_fixup, xbmc, SECTION,     base)
	@$(call install_fixup, xbmc, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, xbmc, DESCRIPTION, missing)
	
	@$(call install_copy, xbmc, 0, 0, 755, -, /usr/bin/xbmc)
	
	@$(call install_finish, xbmc)
	
	@$(call touch)

# vim: syntax=make
