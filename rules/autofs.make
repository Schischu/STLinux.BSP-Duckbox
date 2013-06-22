# -*-makefile-*-
#
# Copyright (C) 2009 by Robert Schwebel
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_AUTOFS) += autofs

#
# Paths and names
#
AUTOFS_VERSION	:= 4.1.4
AUTOFS		:= autofs-$(AUTOFS_VERSION)
AUTOFS_SUFFIX	:= tar.gz
AUTOFS_MD5	:= c092d27caa36c9ac160466ebb4c1cdfa
AUTOFS_URL	:= http://kernel.org/pub/linux/daemons/autofs/v4/$(AUTOFS).$(AUTOFS_SUFFIX)
AUTOFS_SOURCE	:= $(SRCDIR)/$(AUTOFS).$(AUTOFS_SUFFIX)
AUTOFS_DIR	:= $(BUILDDIR)/$(AUTOFS)
AUTOFS_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

AUTOFS_PATH	:= PATH=$(CROSS_PATH)
AUTOFS_ENV 	:= $(CROSS_ENV)
AUTOFS_MAKEVARS := $(CROSS_ENV_CC) PREFIX=$(AUTOFS_PKGDIR) INSTALLROOT=$(AUTOFS_PKGDIR)

#
# autoconf
#
AUTOFS_CONF_TOOL	:= autoconf
AUTOFS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR)
	
#AUTOFS_INSTALL_OPT	:= \
#	INSTALLROOT=$(AUTOFS_PKGDIR) \
#	install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/autofs.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  autofs)
	@$(call install_fixup, autofs,PACKAGE,autofs)
	@$(call install_fixup, autofs,PRIORITY,optional)
	@$(call install_fixup, autofs,SECTION,base)
	@$(call install_fixup, autofs,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, autofs,DESCRIPTION,missing)
	
	@$(call install_tree, autofs, 0, 0, -, /usr)
	
	@$(call install_finish, autofs)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_AUTOFS_CONFIG) += autofs-config

AUTOFS_CONFIG_VERSION	:= head

$(STATEDIR)/autofs-config.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  autofs-config)
	@$(call install_fixup, autofs-config, PRIORITY,    optional)
	@$(call install_fixup, autofs-config, SECTION,     base)
	@$(call install_fixup, autofs-config, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, autofs-config, DESCRIPTION, missing)
	
	@$(call install_alternative, autofs-config, 0, 0, 0644, /etc/auto.hotplug)
	@$(call install_alternative, autofs-config, 0, 0, 0644, /etc/auto.network)
	
	@$(call install_finish, autofs-config)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_AUTOFS_INIT) += autofs-init

AUTOFS_INIT_VERSION	:= head

$(STATEDIR)/autofs-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, autofs-init)
	@$(call install_fixup, autofs-init,PRIORITY,optional)
	@$(call install_fixup, autofs-init,SECTION,base)
	@$(call install_fixup, autofs-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, autofs-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, autofs-init, 0, 0, 0755, /etc/init.d/autofs)
	
ifneq ($(call remove_quotes,$(PTXCONF_AUTOFS_BBINIT_LINK)),)
	@$(call install_link, autofs-init, \
		../init.d/autofs, \
		/etc/rc.d/$(PTXCONF_AUTOFS_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, autofs-init)
	
	@$(call touch)

# vim: syntax=make
