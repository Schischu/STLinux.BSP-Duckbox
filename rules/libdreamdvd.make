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
PACKAGES-$(PTXCONF_LIBDREAMDVD) += libdreamdvd

#
# Paths and names
#
#ee7b0b4c1b35264ed7e003782b501040b7b435fd
#http://schwerkraft.elitedvb.net/anonscm/git/libdreamdvd/libdreamdvd.git
LIBDREAMDVD_VERSION	:= 6aa22dd3f530ca4be49946e07e4a0bfe60427bdf
LIBDREAMDVD		:= libdreamdvd-$(LIBDREAMDVD_VERSION)
LIBDREAMDVD_URL		:= git://github.com/mirakels/libdreamdvd.git
LIBDREAMDVD_GIT_BRANCH	:= master
LIBDREAMDVD_GIT_HEAD	:= $(LIBDREAMDVD_VERSION)
LIBDREAMDVD_SOURCE	:= $(SRCDIR)/libdreamdvd.git
LIBDREAMDVD_DIR		:= $(BUILDDIR)/$(LIBDREAMDVD)
LIBDREAMDVD_LICENSE	:= GPLv2

$(STATEDIR)/libdreamdvd.extract.post:
	@$(call targetinfo)
	
	cd $(LIBDREAMDVD_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; autoheader; automake -a; autoconf; 
	
	@$(call world/patchin/post, LIBDREAMDVD)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBDREAMDVD_PATH	:= PATH=$(CROSS_PATH)
LIBDREAMDVD_ENV 	:= $(CROSS_ENV)

LIBDREAMDVD_CONF_TOOL	:= autoconf
LIBDREAMDVD_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdreamdvd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libdreamdvd)
	@$(call install_fixup, libdreamdvd,PRIORITY,optional)
	@$(call install_fixup, libdreamdvd,SECTION,base)
	@$(call install_fixup, libdreamdvd,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libdreamdvd,DESCRIPTION,missing)

	@$(call install_lib, libdreamdvd, 0, 0, 0644, libdreamdvd)

	@$(call install_finish, libdreamdvd)

	@$(call touch)

# vim: syntax=make
