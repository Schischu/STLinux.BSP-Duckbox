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
PACKAGES-$(PTXCONF_LIBDVDREAD) += libdvdread

#
# Paths and names
#
LIBDVDREAD_VERSION	:= 1243
LIBDVDREAD		:= libdvdread-r$(LIBDVDREAD_VERSION)
LIBDVDREAD_URL		:= svn://svn.mplayerhq.hu/dvdnav/trunk/libdvdread
LIBDVDREAD_SVN_REV	:= $(LIBDVDREAD_VERSION)
LIBDVDREAD_SOURCE	:= $(SRCDIR)/libdvdread.svn
LIBDVDREAD_DIR		:= $(BUILDDIR)/$(LIBDVDREAD)
LIBDVDREAD_LICENSE	:= GPLv2

$(STATEDIR)/libdvdread.extract.post:
	@$(call targetinfo)
	
	cd $(LIBDVDREAD_DIR) && autoreconf -v --install
	
	@$(call world/patchin/post, LIBDVDREAD)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBDVDREAD_PATH	:= PATH=$(CROSS_PATH)
LIBDVDREAD_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBDVDREAD_AUTOCONF := $(CROSS_AUTOCONF_USR)

$(STATEDIR)/libdvdread.install:
	@$(call targetinfo)
	@$(call world/install, LIBDVDREAD)
	
	( \
		cp -r $(LIBDVDREAD_PKGDIR)/usr/include/* $(SYSROOT)/usr/include/; \
		cp -r $(LIBDVDREAD_PKGDIR)/usr/lib/* $(SYSROOT)/usr/lib/; \
		sed -i "s,-I\$${prefix}/include,-I\$${SYSROOT}/usr/include,g" $(LIBDVDREAD_PKGDIR)/usr/bin/dvdread-config; \
		sed -i "s,-L/usr/lib,-L\$${SYSROOT}/usr/lib,g" $(LIBDVDREAD_PKGDIR)/usr/bin/dvdread-config; \
	) 2>&1 > /dev/null
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdvdread.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libdvdread)
	@$(call install_fixup, libdvdread,PRIORITY,optional)
	@$(call install_fixup, libdvdread,SECTION,base)
	@$(call install_fixup, libdvdread,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libdvdread,DESCRIPTION,missing)

	@$(call install_lib, libdvdread, 0, 0, 0644, libdvdread)

	@$(call install_finish, libdvdread)

	@$(call touch)

# vim: syntax=make
