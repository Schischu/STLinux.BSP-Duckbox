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
PACKAGES-$(PTXCONF_BOOTLOGO) += bootlogo

#
# Paths and names
#
BOOTLOGO_VERSION	:= head2
BOOTLOGO := bootlogo
BOOTLOGO_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/extra/$(BOOTLOGO)
BOOTLOGO_DIR		:= $(BUILDDIR)/$(BOOTLOGO)
BOOTLOGO_LICENSE	:= GPLv2

$(STATEDIR)/bootlogo.compile:
	@$(call touch)

$(STATEDIR)/bootlogo.install:
	@$(call targetinfo)
	
	@mkdir -p $(BOOTLOGO_PKGDIR)/boot/
	@cp -dp $(BOOTLOGO_DIR)/$(BOOTLOGO).mvi $(BOOTLOGO_PKGDIR)/boot/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/bootlogo.targetinstall:
	@$(call targetinfo)

	@$(call install_init, bootlogo)
	@$(call install_fixup, bootlogo,PRIORITY,optional)
	@$(call install_fixup, bootlogo,SECTION,base)
	@$(call install_fixup, bootlogo,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, bootlogo,DESCRIPTION,missing)

	@$(call install_copy, bootlogo, 0, 0, 644, -, /boot/$(BOOTLOGO).mvi)


	@$(call install_finish, bootlogo)

	@$(call touch)

# vim: syntax=make
