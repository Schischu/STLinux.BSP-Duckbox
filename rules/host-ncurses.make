# -*-makefile-*-
#
# Copyright (C) 2008 by mol@pengutronix.de
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_NCURSES) += host-ncurses

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_NCURSES_CONF_TOOL	:= autoconf
HOST_NCURSES_CONF_OPT	= \
	$(HOST_AUTOCONF) \
	$(NCURSES_AUTOCONF_SHARED)

$(STATEDIR)/host-ncurses.compile:
	@$(call targetinfo)

	cd $(HOST_NCURSES_DIR) && $(HOST_NCURSES_PATH) $(HOST_NCURSES_CONF_ENV) \
		$(MAKE) $(PARALLELMFLAGS_BROKEN) $(HOST_NCURSES_MAKEVARS) all
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/host-ncurses.install:
	@$(call targetinfo)
	@$(call world/install, HOST_NCURSES)
#	# don't install headers, so packages like the kernel don't use it
	@rm -r $(HOST_NCURSES_PKGDIR)/include
	@$(call touch)

# vim: syntax=make
