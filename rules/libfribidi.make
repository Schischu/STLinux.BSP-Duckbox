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
PACKAGES-$(PTXCONF_LIBFRIBIDI) += libfribidi

#
# Paths and names
#
LIBFRIBIDI_VERSION	:=0.19.5
LIBFRIBIDI_MD5		:= 925bafb97afee8a2fc2d0470c072a155
LIBFRIBIDI		:= fribidi-$(LIBFRIBIDI_VERSION)
LIBFRIBIDI_SUFFIX	:= tar.bz2
LIBFRIBIDI_SOURCE	:= $(SRCDIR)/$(LIBFRIBIDI).$(LIBFRIBIDI_SUFFIX)
LIBFRIBIDI_DIR		:= $(BUILDDIR)/$(LIBFRIBIDI)
LIBFRIBIDI_LICENSE	:= BSD,GPLv2+

LIBFRIBIDI_URL := \
	http://fribidi.org/download/$(LIBFRIBIDI).$(LIBFRIBIDI_SUFFIX)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBFRIBIDI_PATH	:= PATH=$(CROSS_PATH)
LIBFRIBIDI_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBFRIBIDI_AUTOCONF := $(CROSS_AUTOCONF_USR)
# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libfribidi.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libfribidi)
	@$(call install_fixup, libfribidi,PRIORITY,optional)
	@$(call install_fixup, libfribidi,SECTION,base)
	@$(call install_fixup, libfribidi,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libfribidi,DESCRIPTION,missing)

	@$(call install_lib, libfribidi, 0, 0, 0644, libfribidi)

	@$(call install_finish, libfribidi)

	@$(call touch)

# vim: syntax=make
