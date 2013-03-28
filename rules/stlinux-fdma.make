# -*-makefile-*-
#
# Copyright (C) 2008 by Robert Schwebel
#               2010 by Marc Kleine-Budde <mkl@penutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_STLINUX_FDMA) += stlinux-fdma

#
# Paths and names
#
STLINUX_FDMA_VERSION	:= 20120404
STLINUX_FDMA_MINOR_VERSION	:= 8
STLINUX_FDMA_MD5	:= 84059121f7e8bd302296504418696771
STLINUX_FDMA_PREFIX := stlinux24-target
STLINUX_FDMA_NAME := fdma-firmware
STLINUX_FDMA		:= $(STLINUX_FDMA_PREFIX)-$(STLINUX_FDMA_NAME)-$(STLINUX_FDMA_VERSION)-$(STLINUX_FDMA_MINOR_VERSION)
STLINUX_FDMA_SUFFIX	:= src.rpm
STLINUX_FDMA_URL	:= ftp://ftp.stlinux.com/pub/stlinux/2.4/updates/SRPMS/$(STLINUX_FDMA).$(STLINUX_FDMA_SUFFIX)
STLINUX_FDMA_SOURCE	:= $(SRCDIR)/$(STLINUX_FDMA).$(STLINUX_FDMA_SUFFIX)
STLINUX_FDMA_DIR	:= $(BUILDDIR)/$(STLINUX_FDMA)

$(STATEDIR)/stlinux-fdma.extract:
	@$(call targetinfo)
	@$(clean $(STLINUX_FDMA_DIR))
	@$(shell rm -rf $(STLINUX_FDMA_DIR); mkdir -p $(STLINUX_FDMA_DIR); cd $(STLINUX_FDMA_DIR); rpm2cpio $(STLINUX_FDMA_SOURCE) | cpio -i; tar -xf $(STLINUX_FDMA_NAME)-$(STLINUX_FDMA_VERSION).tar.bz2)
	@$(call touch)

$(STATEDIR)/stlinux-fdma.compile:
	@$(call targetinfo)
	@$(call touch)

$(STATEDIR)/stlinux-fdma.install:
	@$(call targetinfo)
	
	@mkdir -p $(STLINUX_FDMA_PKGDIR)/lib/fw/
	@cp -dp $(STLINUX_FDMA_DIR)/$(STLINUX_FDMA_NAME)/* $(STLINUX_FDMA_PKGDIR)/lib/fw/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/stlinux-fdma.targetinstall:
	@$(call targetinfo)

	@$(call install_init, stlinux-fdma)
	@$(call install_fixup, stlinux-fdma,PRIORITY,optional)
	@$(call install_fixup, stlinux-fdma,SECTION,base)
	@$(call install_fixup, stlinux-fdma,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, stlinux-fdma,DESCRIPTION,missing)

	@$(call install_tree, stlinux-fdma, 0, 0, -, /lib/fw/, 0)

	@$(call install_finish, stlinux-fdma)

	@$(call touch)

# vim: syntax=make
