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
PACKAGES-$(PTXCONF_LIBGIF) += libgif

#
# Paths and names
#
LIBGIF_VERSION	:= 4.1.6
LIBGIF_MD5	:= 7125644155ae6ad33dbc9fc15a14735f
LIBGIF		:= giflib-$(LIBGIF_VERSION)
LIBGIF_SUFFIX	:= tar.bz2
LIBGIF_URL	:= http://heanet.dl.sourceforge.net/sourceforge/giflib/$(LIBGIF).$(LIBGIF_SUFFIX)
LIBGIF_SOURCE	:= $(SRCDIR)/$(LIBGIF).$(LIBGIF_SUFFIX)
LIBGIF_DIR	:= $(BUILDDIR)/$(LIBGIF)
LIBGIF_LICENSE	:= libgif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBGIF_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--without-x

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libgif.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libgif)
	@$(call install_fixup, libgif,PRIORITY,optional)
	@$(call install_fixup, libgif,SECTION,base)
	@$(call install_fixup, libgif,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libgif,DESCRIPTION,missing)

	@$(call install_lib, libgif, 0, 0, 0644, libgif)

	@$(call install_finish, libgif)

	@$(call touch)

# vim: syntax=make
