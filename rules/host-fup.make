# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_FUP) += host-fup

#
# Paths and names
#
HOST_FUP_VERSION	:= 1.0.0
HOST_FUP			:= fup
HOST_FUP_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/host/$(HOST_FUP).src
HOST_FUP_DIR		:= $(HOST_BUILDDIR)/$(HOST_FUP).src

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_FUP_CONF_TOOL	:= autoconf
HOST_FUP_CONF_OPT	:= \
	$(HOST_AUTOCONF)

$(STATEDIR)/host-fup.prepare: $(STATEDIR)/host-fup.get $(STATEDIR)/host-fup.extract
	@$(call targetinfo)
	cd $(HOST_FUP_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, HOST_FUP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-fup.install: $(STATEDIR)/host-fup.prepare $(STATEDIR)/host-fup.compile
	@$(call targetinfo)
	cp $(HOST_FUP_DIR)/fup $(PTXCONF_SYSROOT_HOST)/bin
	@$(call touch)

$(STATEDIR)/host-fup.install.post: $(STATEDIR)/host-fup.install

# vim: syntax=make
