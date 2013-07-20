# -*-makefile-*-
#
# Copyright (C) 2003-2009 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_AVAHI_SCRIPT_INIT) += avahi-script-init

#
# Paths and names
#
AVAHI_SCRIPT_INIT_VERSION	:= 0.1

$(STATEDIR)/avahi-script-init.targetinstall:
	@$(call targetinfo)

	@$(call install_init, avahi-script-init)
	@$(call install_fixup, avahi-script-init,PRIORITY,optional)
	@$(call install_fixup, avahi-script-init,SECTION,base)
	@$(call install_fixup, avahi-script-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, avahi-script-init,DESCRIPTION,missing)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, avahi-script-init, 0, 0, 0755, /etc/init.d/avahi-daemon)

ifneq ($(call remove_quotes,$(PTXCONF_AVAHI_SCRIPT_BBINIT_LINK)),)
	@$(call install_link, avahi-script-init, \
		../init.d/avahi-daemon, \
		/etc/rc.d/$(PTXCONF_AVAHI_SCRIPT_BBINIT_LINK))
endif
endif

	@$(call install_finish, avahi-script-init)

	@$(call touch)

# vim: syntax=make
