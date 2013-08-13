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

PACKAGES-$(PTXCONF_CUSTOM_TIME)  += custom-time

CUSTOM_TIME_VERSION	:= 1.0

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/custom-time.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, custom-time)
	@$(call install_fixup, custom-time,PRIORITY,optional)
	@$(call install_fixup, custom-time,SECTION,base)
	@$(call install_fixup, custom-time,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, custom-time,DESCRIPTION,missing)

	@$(call install_alternative, custom-time, 0, 0, 0755, /etc/timezone.xml)
	@$(call install_finish, custom-time)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_CUSTOM_TIME_INIT) += custom-time-init

CUSTOM_TIME_INIT_VERSION	:= head1

$(STATEDIR)/custom-time-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, custom-time-init)
	@$(call install_fixup, custom-time-init,PRIORITY,optional)
	@$(call install_fixup, custom-time-init,SECTION,base)
	@$(call install_fixup, custom-time-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, custom-time-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, custom-time-init, 0, 0, 0755, /etc/init.d/rdate)
	
ifneq ($(call remove_quotes,$(PTXCONF_CUSTOM_TIME_BBINIT_LINK)),)
	@$(call install_link, custom-time-init, \
		../init.d/rdate, \
		/etc/rc.d/$(PTXCONF_CUSTOM_TIME_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, custom-time-init)
	
	@$(call touch)

# vim: syntax=make
