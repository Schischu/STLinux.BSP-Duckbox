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
PACKAGES-$(PTXCONF_NETWORK_IFLOOP_INIT) += network-ifloop-init

#
# Paths and names and versions
#

NETWORK_IFLOOP_INIT_VERSION	:= 1.0


# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------


$(STATEDIR)/network-ifloop-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  network-ifloop-init)
	@$(call install_fixup, network-ifloop-init, PRIORITY,optional)
	@$(call install_fixup, network-ifloop-init, SECTION,base)
	@$(call install_fixup, network-ifloop-init, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, network-ifloop-init, DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, network-ifloop-init, 0, 0, 0755, /etc/init.d/ifloop)
	
ifneq ($(call remove_quotes,$(PTXCONF_NETWORK_IFLOOP_BBINIT_LINK)),)
	@$(call install_link, network-ifloop-init, \
		../init.d/ifloop, \
		/etc/rc.d/$(PTXCONF_NETWORK_IFLOOP_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, network-ifloop-init)
	
	@$(call touch)

# vim: syntax=make
