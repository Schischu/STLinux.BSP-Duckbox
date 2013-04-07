# -*-makefile-*-
#
# Copyright (C) 2003-2006 by Robert Schwebel <r.schwebel@pengutronix.de>
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
PACKAGES-$(PTXCONF_LIBMMEIMAGE) += libmmeimage

#
# Paths and names
#
LIBMMEIMAGE_VERSION	:=1.0
LIBMMEIMAGE		:= libmmeimage
LIBMMEIMAGE_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/libs/$(LIBMMEIMAGE)
LIBMMEIMAGE_DIR		:= $(BUILDDIR)/$(LIBMMEIMAGE)
LIBMMEIMAGE_LICENSE	:= GPLv2+

$(STATEDIR)/libmmeimage.prepare: $(STATEDIR)/driver-multicom.install \
                                     $(STATEDIR)/driver-bpamem.install \
                                     $(STATEDIR)/driver-stmfb.install

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBMMEIMAGE_PATH	:= PATH=$(CROSS_PATH)
LIBMMEIMAGE_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBMMEIMAGE_CONF_TOOL := autoconf
LIBMMEIMAGE_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

$(STATEDIR)/libmmeimage.prepare:
	@$(call targetinfo)
	cd $(LIBMMEIMAGE_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, LIBMMEIMAGE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libmmeimage.compile:
#	@$(call targetinfo)
#	
#	cd $(LIBMMEIMAGE_DIR) && \
#		$(MAKE) $(CROSS_ENV_CC) libmmeimage.so
#	
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmmeimage.install:
	@$(call targetinfo)
	@$(call world/install, LIBMMEIMAGE)
	
	mkdir -p $(SYSROOT)/usr/include
	cp $(LIBMMEIMAGE_DIR)/libmmeimage.h $(SYSROOT)/usr/include/
	cp $(LIBMMEIMAGE_DIR)/libmmeimg_error.h $(SYSROOT)/usr/include/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmmeimage.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libmmeimage)
	@$(call install_fixup, libmmeimage,PRIORITY,optional)
	@$(call install_fixup, libmmeimage,SECTION,base)
	@$(call install_fixup, libmmeimage,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libmmeimage,DESCRIPTION,missing)

	@$(call install_lib, libmmeimage, 0, 0, 0644, libmmeimage)

	@$(call install_finish, libmmeimage)

	@$(call touch)

# vim: syntax=make
