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
PACKAGES-$(PTXCONF_HOTPLUG) += hotplug

#
# Paths and names
#
HOTPLUG_VERSION	:=1.0
HOTPLUG		:= hotplug
HOTPLUG_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/tools/$(HOTPLUG)
HOTPLUG_DIR		:= $(BUILDDIR)/$(HOTPLUG)
HOTPLUG_LICENSE	:= GPLv2+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOTPLUG_PATH	:= PATH=$(CROSS_PATH)
HOTPLUG_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
HOTPLUG_CONF_TOOL := autoconf
HOTPLUG_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	bindir=/sbin

$(STATEDIR)/hotplug.prepare:
	@$(call targetinfo)
	cd $(HOTPLUG_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, HOTPLUG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/hotplug.compile:
#	@$(call targetinfo)
#	
#	cd $(HOTPLUG_DIR) && \
#		$(MAKE) $(CROSS_ENV_CC) hotplug.so
#	
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/hotplug.install:
	@$(call targetinfo)
	@$(call world/install, HOTPLUG)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/hotplug.targetinstall:
	@$(call targetinfo)

	@$(call install_init, hotplug)
	@$(call install_fixup, hotplug,PRIORITY,optional)
	@$(call install_fixup, hotplug,SECTION,base)
	@$(call install_fixup, hotplug,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, hotplug,DESCRIPTION,missing)

	@$(call install_copy, hotplug, 0, 0, 0755, -, /sbin/$(HOTPLUG))

	@$(call install_finish, hotplug)

	@$(call touch)

# vim: syntax=make
