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
PACKAGES-$(PTXCONF_VSFTPD) += vsftpd

#
# Paths and names
#
VSFTPD_VERSION	:= 2.3.4
VSFTPD		:= vsftpd-$(VSFTPD_VERSION)
VSFTPD_SUFFIX	:= tar.gz
VSFTPD_MD5	:= 0fe274910d56e31a68bf6341b2fe34b2
VSFTPD_URL	:= http://gd.tuwien.ac.at/infosys/servers/ftp/vsftpd/$(VSFTPD).$(VSFTPD_SUFFIX)
VSFTPD_SOURCE	:= $(SRCDIR)/$(VSFTPD).$(VSFTPD_SUFFIX)
VSFTPD_DIR	:= $(BUILDDIR)/$(VSFTPD)
VSFTPD_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

VSFTPD_PATH	:= PATH=$(CROSS_PATH)
VSFTPD_ENV 	:= $(CROSS_ENV)
VSFTPD_MAKEVARS := $(CROSS_ENV_CC) PREFIX=$(VSFTPD_PKGDIR)

#
# autoconf
#
VSFTPD_CONF_TOOL	:= autoconf
VSFTPD_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR)

$(STATEDIR)/vsftpd.prepare:
	@$(call targetinfo)
	@$(call touch)

$(STATEDIR)/vsftpd.compile:
	@$(call targetinfo)
	
	cd $(VSFTPD_DIR) && $(VSFTPD_PATH) $(VSFTPD_CONF_ENV) \
		$(MAKE) clean && \
		$(MAKE) $(VSFTPD_MAKEVARS)
	@$(call touch)


#VSFTPD_INSTALL_OPT := install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/vsftpd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  vsftpd)
	@$(call install_fixup, vsftpd,PACKAGE,vsftpd)
	@$(call install_fixup, vsftpd,PRIORITY,optional)
	@$(call install_fixup, vsftpd,SECTION,base)
	@$(call install_fixup, vsftpd,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, vsftpd,DESCRIPTION,missing)
	
	@$(call install_copy, vsftpd, 0, 0, 0755, -, /usr/sbin/vsftpd)
	@$(call install_alternative, vsftpd, 0, 0, 0644, /etc/vsftpd.conf)
	
	@$(call install_finish, vsftpd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_VSFTPD_INIT) += vsftpd-init

VSFTPD_INIT_VERSION	:= head15

$(STATEDIR)/vsftpd-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, vsftpd-init)
	@$(call install_fixup, vsftpd-init,PRIORITY,optional)
	@$(call install_fixup, vsftpd-init,SECTION,base)
	@$(call install_fixup, vsftpd-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, vsftpd-init,DESCRIPTION,missing)
	
	@$(call install_finish, vsftpd-init)
	
	@$(call touch)

# vim: syntax=make
