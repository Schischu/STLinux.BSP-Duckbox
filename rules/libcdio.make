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
PACKAGES-$(PTXCONF_LIBCDIO) += libcdio

#
# Paths and names
#
LIBCDIO_VERSION	:= 0.83
LIBCDIO_MD5	:= b9e0f1bccb142e697cd834fe56b6e6fb
LIBCDIO		:= libcdio-$(LIBCDIO_VERSION)
LIBCDIO_SUFFIX	:= tar.gz
LIBCDIO_URL	:= ftp://ftp.gnu.org/gnu/libcdio/$(LIBCDIO).$(LIBCDIO_SUFFIX)
LIBCDIO_SOURCE	:= $(SRCDIR)/$(LIBCDIO).$(LIBCDIO_SUFFIX)
LIBCDIO_DIR	:= $(BUILDDIR)/$(LIBCDIO)
LIBCDIO_LICENSE	:= unknown

#
# autoconf
#
LIBCDIO_CONF_TOOL	:= autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcdio.targetinstall:
	@$(call targetinfo)

	@$(call install_init,   libcdio)
	@$(call install_fixup,  libcdio, PRIORITY,    optional)
	@$(call install_fixup,  libcdio, SECTION,     base)
	@$(call install_fixup,  libcdio, AUTHOR,      "fga")
	@$(call install_fixup,  libcdio, DESCRIPTION, missing)

	@$(call install_lib,    libcdio, 0, 0, 0644, libcdio)

	@$(call install_finish, libcdio)

	@$(call touch)

# vim: syntax=make
