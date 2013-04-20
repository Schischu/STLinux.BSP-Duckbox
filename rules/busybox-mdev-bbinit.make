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
PACKAGES-$(PTXCONF_BUSYBOX_MDEV_INIT) += busybox-mdev-init

#
# Paths and names
#
BUSYBOX_MDEV_INIT_VERSION	:= head

$(STATEDIR)/busybox-mdev-init.targetinstall:
	@$(call targetinfo)

	@$(call install_init, busybox_mdev_init)
	@$(call install_fixup, busybox-mdev-init,PRIORITY,optional)
	@$(call install_fixup, busybox-mdev-init,SECTION,base)
	@$(call install_fixup, busybox-mdev-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, busybox-mdev-init,DESCRIPTION,missing)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, busybox-mdev-init, 0, 0, 0755, /etc/init.d/mdev)

ifneq ($(call remove_quotes,$(PTXCONF_BUSYBOX_MDEV_BBINIT_LINK)),)
	@$(call install_link, busybox-mdev-init, \
		../init.d/mdev, \
		/etc/rc.d/$(PTXCONF_BUSYBOX_MDEV_BBINIT_LINK))
endif
endif

	@$(call install_finish, busybox-mdev-init)

	@$(call touch)

# vim: syntax=make
