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
HOST_PACKAGES-$(PTXCONF_HOST_MUP) += host-mup

#
# Paths and names
#
HOST_MUP_VERSION	:= head
HOST_MUP		:= mup
HOST_MUP_URL	:= lndir://$(PTXDIST_WORKSPACE)/local_src/host/$(HOST_MUP)
HOST_MUP_DIR	:= $(HOST_BUILDDIR)/$(HOST_MUP)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_MUP_CONF_TOOL	:= autoconf
HOST_MUP_CONF_OPT	:= \
	$(HOST_AUTOCONF)

$(STATEDIR)/host-mup.prepare: $(STATEDIR)/host-mup.get $(STATEDIR)/host-mup.extract
	@$(call targetinfo)
	cd $(HOST_MUP_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; automake -a; autoconf
	@$(call world/prepare, HOST_MUP)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-mup.install: $(STATEDIR)/host-mup.prepare $(STATEDIR)/host-mup.compile
	@$(call targetinfo)
	cp $(HOST_MUP_DIR)/mup $(PTXCONF_SYSROOT_HOST)/bin
	@$(call touch)

$(STATEDIR)/host-mup.install.post: $(STATEDIR)/host-mup.install

# vim: syntax=make
