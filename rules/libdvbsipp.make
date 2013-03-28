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
PACKAGES-$(PTXCONF_LIBDVBSIPP) += libdvbsipp

#
# Paths and names
#
LIBDVBSIPP_VERSION	:= 0.3.6
LIBDVBSIPP_MD5	:= 4e9fb95c3ab8bb31ff051ed1aa98c8c5
LIBDVBSIPP		:= libdvbsi++-$(LIBDVBSIPP_VERSION)
LIBDVBSIPP_SUFFIX	:= tar.bz2
LIBDVBSIPP_URL	:= http://www.saftware.de/libdvbsi++/$(LIBDVBSIPP).$(LIBDVBSIPP_SUFFIX)
LIBDVBSIPP_SOURCE	:= $(SRCDIR)/$(LIBDVBSIPP).$(LIBDVBSIPP_SUFFIX)
LIBDVBSIPP_DIR	:= $(BUILDDIR)/$(LIBDVBSIPP)
LIBDVBSIPP_LICENSE	:= libdvbsipp

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBDVBSIPP_PATH	:= PATH=$(CROSS_PATH)
LIBDVBSIPP_ENV 	:= $(CROSS_ENV)

LIBDVBSIPP_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdvbsipp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libdvbsipp)
	@$(call install_fixup, libdvbsipp,PRIORITY,optional)
	@$(call install_fixup, libdvbsipp,SECTION,base)
	@$(call install_fixup, libdvbsipp,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libdvbsipp,DESCRIPTION,missing)

	@$(call install_lib, libdvbsipp, 0, 0, 0644, libdvbsipp)

	@$(call install_finish, libdvbsipp)

	@$(call touch)

# vim: syntax=make
