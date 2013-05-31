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
PACKAGES-$(PTXCONF_BPA2DUMP) += bpa2dump

#
# Paths and names
#
BPA2DUMP_VERSION	:=1.0
BPA2DUMP			:= bpa2dump
BPA2DUMP_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/tools/$(BPA2DUMP)
BPA2DUMP_DIR		:= $(BUILDDIR)/$(BPA2DUMP)
BPA2DUMP_LICENSE	:= GPLv2+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

BPA2DUMP_PATH	:= PATH=$(CROSS_PATH)
BPA2DUMP_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
BPA2DUMP_CONF_TOOL := autoconf
BPA2DUMP_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

$(STATEDIR)/bpa2dump.prepare: $(STATEDIR)/driver-bpamem.install
	@$(call targetinfo)
	cd $(BPA2DUMP_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, BPA2DUMP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/bpa2dump.install:
	@$(call targetinfo)
	@$(call world/install, BPA2DUMP)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/bpa2dump.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, bpa2dump)
	@$(call install_fixup, bpa2dump,PRIORITY,optional)
	@$(call install_fixup, bpa2dump,SECTION,base)
	@$(call install_fixup, bpa2dump,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, bpa2dump,DESCRIPTION,missing)
	
	@$(call install_copy, bpa2dump, 0, 0, 0755, -, /usr/bin/$(BPA2DUMP))
	
	@$(call install_finish, bpa2dump)
	
	@$(call touch)

# vim: syntax=make
