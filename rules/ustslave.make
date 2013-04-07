# -*-makefile-*-
#
# Copyright (C) 2003-2006 by Robert Schwebel <r.schwebel@pengutronix.de>
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_USTSLAVE) += ustslave

#
# Paths and names
#
USTSLAVE_VERSION	:=1.0
USTSLAVE		:= ustslave
USTSLAVE_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/tools/$(USTSLAVE)
USTSLAVE_DIR		:= $(BUILDDIR)/$(USTSLAVE)
USTSLAVE_LICENSE	:= GPLv2+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

USTSLAVE_PATH	:= PATH=$(CROSS_PATH)
USTSLAVE_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
USTSLAVE_CONF_TOOL := autoconf
USTSLAVE_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	bindir=/bin

$(STATEDIR)/ustslave.prepare:
	@$(call targetinfo)
	cd $(USTSLAVE_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, USTSLAVE)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/ustslave.compile:
#	@$(call targetinfo)
#	
#	cd $(USTSLAVE_DIR) && \
#		$(MAKE) $(CROSS_ENV_CC) ustslave.so
#	
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ustslave.install:
	@$(call targetinfo)
	@$(call world/install, USTSLAVE)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ustslave.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ustslave)
	@$(call install_fixup, ustslave,PRIORITY,optional)
	@$(call install_fixup, ustslave,SECTION,base)
	@$(call install_fixup, ustslave,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, ustslave,DESCRIPTION,missing)

	@$(call install_copy, ustslave, 0, 0, 0755, -, /bin/$(USTSLAVE))

	@$(call install_finish, ustslave)

	@$(call touch)

# vim: syntax=make
