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
PACKAGES-$(PTXCONF_STREAMPROXY) += streamproxy

#
# Paths and names
#
STREAMPROXY_VERSION	:=1.0
STREAMPROXY		:= streamproxy
STREAMPROXY_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/tools/$(STREAMPROXY)
STREAMPROXY_DIR		:= $(BUILDDIR)/$(STREAMPROXY)
STREAMPROXY_LICENSE	:= GPLv2+

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

STREAMPROXY_PATH	:= PATH=$(CROSS_PATH)
STREAMPROXY_ENV 	:= $(CROSS_ENV)

#
# autoconf
#
STREAMPROXY_CONF_TOOL := autoconf
STREAMPROXY_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

$(STATEDIR)/streamproxy.prepare:
	@$(call targetinfo)
	cd $(STREAMPROXY_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, STREAMPROXY)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/streamproxy.compile:
#	@$(call targetinfo)
#	
#	cd $(STREAMPROXY_DIR) && \
#		$(MAKE) $(CROSS_ENV_CC) streamproxy.so
#	
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/streamproxy.install:
	@$(call targetinfo)
	@$(call world/install, STREAMPROXY)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/streamproxy.targetinstall:
	@$(call targetinfo)

	@$(call install_init, streamproxy)
	@$(call install_fixup, streamproxy,PRIORITY,optional)
	@$(call install_fixup, streamproxy,SECTION,base)
	@$(call install_fixup, streamproxy,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, streamproxy,DESCRIPTION,missing)

	@$(call install_copy, streamproxy, 0, 0, 0755, -, /usr/bin/$(STREAMPROXY))

	@$(call install_finish, streamproxy)

	@$(call touch)

# vim: syntax=make
