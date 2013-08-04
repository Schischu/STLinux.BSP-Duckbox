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
PACKAGES-$(PTXCONF_LIBMODPLUG) += libmodplug

#
# Paths and names
#
LIBMODPLUG_VERSION	:= 0.8.8.4
LIBMODPLUG_MD5	:= fddc3c704c5489de2a3cf0fedfec59db
LIBMODPLUG		:= libmodplug-$(LIBMODPLUG_VERSION)
LIBMODPLUG_SUFFIX	:= tar.gz
LIBMODPLUG_URL	:= http://downloads.sourceforge.net/project/modplug-xmms/libmodplug/0.8.8.4/$(LIBMODPLUG).$(LIBMODPLUG_SUFFIX)
LIBMODPLUG_SOURCE	:= $(SRCDIR)/$(LIBMODPLUG).$(LIBMODPLUG_SUFFIX)
LIBMODPLUG_DIR	:= $(BUILDDIR)/$(LIBMODPLUG)
LIBMODPLUG_LICENSE	:= unknown

#
# autoconf
#
LIBMODPLUG_CONF_TOOL	:= autoconf

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libmodplug.targetinstall:
	@$(call targetinfo)

	@$(call install_init,   libmodplug)
	@$(call install_fixup,  libmodplug, PRIORITY,    optional)
	@$(call install_fixup,  libmodplug, SECTION,     base)
	@$(call install_fixup,  libmodplug, AUTHOR,      "fga")
	@$(call install_fixup,  libmodplug, DESCRIPTION, missing)

	@$(call install_lib,    libmodplug, 0, 0, 0644, libmodplug)

	@$(call install_finish, libmodplug)

	@$(call touch)

# vim: syntax=make
