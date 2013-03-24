# -*-makefile-*-
#
# Copyright (C) 2003-2006 Robert Schwebel <r.schwebel@pengutronix.de>
#                         Pengutronix <info@pengutronix.de>, Germany
#               2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_STLINUX_TOOLCHAIN_FIX) += stlinux-toolchain-fix

$(STATEDIR)/stlinux-toolchain-fix.install:
	@$(call targetinfo)
	cp -a `readlink $(PTXDIST_PLATFORMDIR)/selected_toolchain`/../sysroot-$(PTXCONF_GNU_TARGET)/* $(PTXCONF_SYSROOT_HOST)/
	ln -s `readlink $(PTXDIST_PLATFORMDIR)/selected_toolchain`/../lib $(PTXCONF_SYSROOT_HOST)/lib/lib
	ln -s `readlink $(PTXDIST_PLATFORMDIR)/selected_toolchain`/../libexec $(PTXCONF_SYSROOT_HOST)/lib/libexec
	@$(call touch)

# vim: syntax=make
