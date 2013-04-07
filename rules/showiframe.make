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
PACKAGES-$(PTXCONF_SHOWIFRAME) += showiframe

#
# Paths and names
#
SHOWIFRAME_VERSION	:=1.0
SHOWIFRAME		:= showiframe
SHOWIFRAME_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/tools/$(SHOWIFRAME)
SHOWIFRAME_DIR		:= $(BUILDDIR)/$(SHOWIFRAME)
SHOWIFRAME_LICENSE	:= GPLv2+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

SHOWIFRAME_PATH	:= PATH=$(CROSS_PATH)
SHOWIFRAME_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
SHOWIFRAME_CONF_TOOL := autoconf
SHOWIFRAME_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

$(STATEDIR)/showiframe.prepare:
	@$(call targetinfo)
	cd $(SHOWIFRAME_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, SHOWIFRAME)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/showiframe.compile:
#	@$(call targetinfo)
#	
#	cd $(SHOWIFRAME_DIR) && \
#		$(MAKE) $(CROSS_ENV_CC) showiframe.so
#	
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/showiframe.install:
	@$(call targetinfo)
	@$(call world/install, SHOWIFRAME)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/showiframe.targetinstall:
	@$(call targetinfo)

	@$(call install_init, showiframe)
	@$(call install_fixup, showiframe,PRIORITY,optional)
	@$(call install_fixup, showiframe,SECTION,base)
	@$(call install_fixup, showiframe,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, showiframe,DESCRIPTION,missing)

	@$(call install_copy, showiframe, 0, 0, 0755, -, /usr/bin/$(SHOWIFRAME))

	@$(call install_finish, showiframe)

	@$(call touch)

# vim: syntax=make
