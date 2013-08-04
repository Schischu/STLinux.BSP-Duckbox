# -*-makefile-*-
#
# Copyright (C) 2012 by fga
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBSAMPLERATE) += libsamplerate

#
# Paths and names
#
LIBSAMPLERATE_VERSION	:= 0.1.8
LIBSAMPLERATE_MD5	:= 1c7fb25191b4e6e3628d198a66a84f47
LIBSAMPLERATE		:= libsamplerate-$(LIBSAMPLERATE_VERSION)
LIBSAMPLERATE_SUFFIX	:= tar.gz
LIBSAMPLERATE_URL	:= http://www.mega-nerd.com/SRC/$(LIBSAMPLERATE).$(LIBSAMPLERATE_SUFFIX)
LIBSAMPLERATE_SOURCE	:= $(SRCDIR)/$(LIBSAMPLERATE).$(LIBSAMPLERATE_SUFFIX)
LIBSAMPLERATE_DIR	:= $(BUILDDIR)/$(LIBSAMPLERATE)
LIBSAMPLERATE_LICENSE	:= unknown

#
# autoconf
#
LIBSAMPLERATE_CONF_TOOL	:= autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libsamplerate.targetinstall:
	@$(call targetinfo)

	@$(call install_init,   libsamplerate)
	@$(call install_fixup,  libsamplerate, PRIORITY,    optional)
	@$(call install_fixup,  libsamplerate, SECTION,     base)
	@$(call install_fixup,  libsamplerate, AUTHOR,      "fga")
	@$(call install_fixup,  libsamplerate, DESCRIPTION, missing)

	@$(call install_lib,    libsamplerate, 0, 0, 0644, libsamplerate)

	@$(call install_finish, libsamplerate)

	@$(call touch)

# vim: syntax=make
