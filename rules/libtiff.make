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
PACKAGES-$(PTXCONF_LIBTIFF) += libtiff

#
# Paths and names
#
LIBTIFF_VERSION	:= 4.0.1
LIBTIFF_MD5	:= fae149cc9da35c598d8be897826dfc63
LIBTIFF		:= tiff-$(LIBTIFF_VERSION)
LIBTIFF_SUFFIX	:= tar.gz
LIBTIFF_URL	:= ftp://ftp.remotesensing.org/pub/libtiff/$(LIBTIFF).$(LIBTIFF_SUFFIX)
LIBTIFF_SOURCE	:= $(SRCDIR)/$(LIBTIFF).$(LIBTIFF_SUFFIX)
LIBTIFF_DIR	:= $(BUILDDIR)/$(LIBTIFF)
LIBTIFF_LICENSE	:= unknown

#
# autoconf
#
LIBTIFF_CONF_TOOL	:= autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libtiff.targetinstall:
	@$(call targetinfo)

	@$(call install_init,   libtiff)
	@$(call install_fixup,  libtiff, PRIORITY,    optional)
	@$(call install_fixup,  libtiff, SECTION,     base)
	@$(call install_fixup,  libtiff, AUTHOR,      "fga")
	@$(call install_fixup,  libtiff, DESCRIPTION, missing)

	@$(call install_lib,    libtiff, 0, 0, 0644, libtiff)

	@$(call install_finish, libtiff)

	@$(call touch)

# vim: syntax=make
