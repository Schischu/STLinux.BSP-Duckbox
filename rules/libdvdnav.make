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
PACKAGES-$(PTXCONF_LIBDVDNAV) += libdvdnav

#
# Paths and names
#
LIBDVDNAV_VERSION	:= 1243
LIBDVDNAV		:= libdvdnav-r$(LIBDVDNAV_VERSION)
LIBDVDNAV_URL		:= svn://svn.mplayerhq.hu/dvdnav/trunk/libdvdnav
LIBDVDNAV_SVN_REV	:= $(LIBDVDNAV_VERSION)
LIBDVDNAV_SOURCE	:= $(SRCDIR)/libdvdnav.svn
LIBDVDNAV_DIR		:= $(BUILDDIR)/$(LIBDVDNAV)
LIBDVDNAV_LICENSE	:= GPLv2

$(STATEDIR)/libdvdnav.extract.post:
	@$(call targetinfo)
	
	cd $(LIBDVDNAV_DIR) && autoreconf -v --install
	
	@$(call world/patchin/post, LIBDVDNAV)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdvdnav.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libdvdnav)
	@$(call install_fixup, libdvdnav,PRIORITY,optional)
	@$(call install_fixup, libdvdnav,SECTION,base)
	@$(call install_fixup, libdvdnav,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libdvdnav,DESCRIPTION,missing)

	@$(call install_lib, libdvdnav, 0, 0, 0644, libdvdnav)

	@$(call install_finish, libdvdnav)

	@$(call touch)

# vim: syntax=make
