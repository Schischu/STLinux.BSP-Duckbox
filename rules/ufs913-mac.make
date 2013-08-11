# -*-makefile-*-
#
# Copyright (C) @YEAR@ by @AUTHOR@
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_UFS913_MAC_INIT) += ufs913-mac-init

UFS913_MAC_INIT_VERSION	:= 1.0

$(STATEDIR)/ufs913-mac-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  ufs913-mac-init)
	@$(call install_fixup, ufs913-mac-init, PRIORITY,    optional)
	@$(call install_fixup, ufs913-mac-init, SECTION,     base)
	@$(call install_fixup, ufs913-mac-init, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, ufs913-mac-init, DESCRIPTION, missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, ufs913-mac-init, 0, 0, 0755, /etc/init.d/ufs913-mac)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_PTI_BBINIT_LINK)),)
	@$(call install_link, ufs913-mac-init, \
		../init.d/ufs913-mac, \
		/etc/rc.d/$(PTXCONF_UFS913_MAC_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, ufs913-mac-init)
	
	@$(call touch)

# vim: syntax=make
