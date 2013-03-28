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
PACKAGES-$(PTXCONF_LIBXMLCCWRAP) += libxmlccwrap

#
# Paths and names
#
LIBXMLCCWRAP_VERSION	:= 0.0.12
LIBXMLCCWRAP_MD5	:= 9f8bbad3452d704603246273b2dda758
LIBXMLCCWRAP		:= libxmlccwrap-$(LIBXMLCCWRAP_VERSION)
LIBXMLCCWRAP_SUFFIX	:= tar.gz
LIBXMLCCWRAP_URL	:= http://www.ant.uni-bremen.de/whomes/rinas/libxmlccwrap/download/$(LIBXMLCCWRAP).$(LIBXMLCCWRAP_SUFFIX)
LIBXMLCCWRAP_SOURCE	:= $(SRCDIR)/$(LIBXMLCCWRAP).$(LIBXMLCCWRAP_SUFFIX)
LIBXMLCCWRAP_DIR	:= $(BUILDDIR)/$(LIBXMLCCWRAP)
LIBXMLCCWRAP_LICENSE	:= libxmlccwrap

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBXMLCCWRAP_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libxmlccwrap.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libxmlccwrap)
	@$(call install_fixup, libxmlccwrap,PRIORITY,optional)
	@$(call install_fixup, libxmlccwrap,SECTION,base)
	@$(call install_fixup, libxmlccwrap,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libxmlccwrap,DESCRIPTION,missing)

	@$(call install_tree, libxmlccwrap, 0, 0, -, /usr/lib/)

	@$(call install_finish, libxmlccwrap)

	@$(call touch)

# vim: syntax=make
