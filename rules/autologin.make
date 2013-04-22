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
PACKAGES-$(PTXCONF_AUTOLOGIN) += autologin

#
# Paths and names
#
AUTOLOGIN_VERSION	:= head1
AUTOLOGIN := autologin
AUTOLOGIN_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/tools/$(AUTOLOGIN)
AUTOLOGIN_DIR		:= $(BUILDDIR)/$(AUTOLOGIN)
AUTOLOGIN_LICENSE	:= GPLv2

$(STATEDIR)/autologin.compile:
	@$(call touch)

$(STATEDIR)/autologin.install:
	@$(call targetinfo)
	
	@mkdir -p $(AUTOLOGIN_PKGDIR)/bin/
	@cp -dp $(AUTOLOGIN_DIR)/$(AUTOLOGIN) $(AUTOLOGIN_PKGDIR)/bin/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/autologin.targetinstall:
	@$(call targetinfo)

	@$(call install_init, autologin)
	@$(call install_fixup, autologin,PRIORITY,optional)
	@$(call install_fixup, autologin,SECTION,base)
	@$(call install_fixup, autologin,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, autologin,DESCRIPTION,missing)

	@$(call install_copy, autologin, 0, 0, 755, -, /bin/$(AUTOLOGIN))


	@$(call install_finish, autologin)

	@$(call touch)

# vim: syntax=make
