# -*-makefile-*-
#
# Copyright (C) 2007 by Robert Schwebel
#               2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBSIGCPP) += libsigcpp

#
# Paths and names
#
LIBSIGCPP_VERSION	:= 1.2.5
LIBSIGCPP_MD5		:= 1f29decfaa3793f6e55ae905727588be
LIBSIGCPP		:= libsigc++-$(LIBSIGCPP_VERSION)
LIBSIGCPP_SUFFIX	:= tar.bz2
LIBSIGCPP_URL		:= http://ftp.gnome.org/pub/GNOME/sources/libsigc++/1.2/$(LIBSIGCPP).$(LIBSIGCPP_SUFFIX)
LIBSIGCPP_SOURCE	:= $(SRCDIR)/$(LIBSIGCPP).$(LIBSIGCPP_SUFFIX)
LIBSIGCPP_DIR		:= $(BUILDDIR)/$(LIBSIGCPP)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBSIGCPP_PATH	:= PATH=$(CROSS_PATH)
LIBSIGCPP_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBSIGCPP_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--disable-documentation

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libsigcpp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libsigcpp)
	@$(call install_fixup, libsigcpp,PRIORITY,optional)
	@$(call install_fixup, libsigcpp,SECTION,base)
	@$(call install_fixup, libsigcpp,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libsigcpp,DESCRIPTION,missing)

	@$(call install_lib, libsigcpp, 0, 0, 0644, libsigc-1.2)

	@$(call install_finish, libsigcpp)

	@$(call touch)

# vim: syntax=make
