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
HOST_PACKAGES-$(PTXCONF_HOST_PAD) += host-pad

#
# Paths and names
#
HOST_PAD_VERSION	:= head
HOST_PAD		:= pad
HOST_PAD_URL	:= lndir://$(PTXDIST_WORKSPACE)/local_src/host/$(HOST_PAD)
HOST_PAD_DIR	:= $(HOST_BUILDDIR)/$(HOST_PAD)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_PAD_CONF_TOOL	:= autoconf
HOST_PAD_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--enable-largefile

# vim: syntax=make
