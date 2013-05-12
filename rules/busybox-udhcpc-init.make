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
PACKAGES-$(PTXCONF_BUSYBOX_UDHCPC_INIT) += busybox-udhcpc-init

#
# Paths and names
#
BUSYBOX_UDHCPC_INIT_VERSION	:= head3

$(STATEDIR)/busybox-udhcpc-init.targetinstall:
	@$(call targetinfo)

	@$(call install_init, busybox-udhcpc-init)
	@$(call install_fixup, busybox-udhcpc-init,PRIORITY,optional)
	@$(call install_fixup, busybox-udhcpc-init,SECTION,base)
	@$(call install_fixup, busybox-udhcpc-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, busybox-udhcpc-init,DESCRIPTION,missing)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, busybox-udhcpc-init, 0, 0, 0755, /etc/init.d/udhcpc)

ifneq ($(call remove_quotes,$(PTXCONF_BUSYBOX_UDHCPC_BBINIT_LINK)),)
	@$(call install_link, busybox-udhcpc-init, \
		../init.d/udhcpc, \
		/etc/rc.d/$(PTXCONF_BUSYBOX_UDHCPC_BBINIT_LINK))
endif
endif

	@$(call install_finish, busybox-udhcpc-init)

	@$(call touch)

# vim: syntax=make
