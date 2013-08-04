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
PACKAGES-$(PTXCONF_TAGLIB) += taglib

#
# Paths and names
#
TAGLIB_VERSION	:= 1.8
TAGLIB_MD5	:= dcb8bd1b756f2843e18b1fdf3aaeee15
TAGLIB		:= taglib-$(TAGLIB_VERSION)
TAGLIB_SUFFIX	:= tar.gz
TAGLIB_URL	:= https://github.com/downloads/taglib/taglib/$(TAGLIB).$(TAGLIB_SUFFIX)
TAGLIB_SOURCE	:= $(SRCDIR)/$(TAGLIB).$(TAGLIB_SUFFIX)
TAGLIB_DIR	:= $(BUILDDIR)/$(TAGLIB)
TAGLIB_LICENSE	:= unknown

TAGLIB_CONF_TOOL	:= cmake
TAGLIB_CONF_OPT	:= $(CROSS_CMAKE_USR) -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_RELEASE_TYPE=Release

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/taglib.targetinstall:
	@$(call targetinfo)

	@$(call install_init,   taglib)
	@$(call install_fixup,  taglib, PRIORITY,    optional)
	@$(call install_fixup,  taglib, SECTION,     base)
	@$(call install_fixup,  taglib, AUTHOR,      "fga")
	@$(call install_fixup,  taglib, DESCRIPTION, missing)

	@$(call install_lib,    taglib, 0, 0, 0644, libtag)

	@$(call install_finish, taglib)

	@$(call touch)

# vim: syntax=make
