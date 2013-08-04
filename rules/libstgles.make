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
PACKAGES-$(PTXCONF_LIBSTGLES) += libstgles

#
# Paths and names
#
LIBSTGLES_VERSION	:= 1.0
LIBSTGLES			:= libstgles
LIBSTGLES_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/libs/$(LIBSTGLES)
LIBSTGLES_DIR		:= $(BUILDDIR)/$(LIBSTGLES)
LIBSTGLES_LICENSE	:= GPLv2+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBSTGLES_PATH	:= PATH=$(CROSS_PATH)
LIBSTGLES_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
LIBSTGLES_CONF_TOOL := autoconf
LIBSTGLES_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

$(STATEDIR)/libstgles.prepare:
	@$(call targetinfo)
	cd $(LIBSTGLES_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, LIBSTGLES)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libstgles.install:
	@$(call targetinfo)
	@$(call world/install, LIBSTGLES)
	
	mkdir -p $(SYSROOT)/usr/include
	cp -r $(LIBSTGLES_DIR)/includes/* $(SYSROOT)/usr/include/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libstgles.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libstgles)
	@$(call install_fixup, libstgles,PRIORITY,optional)
	@$(call install_fixup, libstgles,SECTION,base)
	@$(call install_fixup, libstgles,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libstgles,DESCRIPTION,missing)

	@$(call install_lib, libstgles, 0, 0, 0644, $(LIBSTGLES))

	@$(call install_finish, libstgles)

	@$(call touch)

# vim: syntax=make
