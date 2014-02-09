# -*-makefile-*-
#
# Copyright (C) 2008 by Robert Schwebel
#               2010 by Marc Kleine-Budde <mkl@penutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#

PACKAGES-$(PTXCONF_NETFLASH) += netflash
NETFLASH_VERSION	:= 1.0

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/netflash.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   netflash)
	@$(call install_fixup,  netflash, PRIORITY,optional)
	@$(call install_fixup,  netflash, SECTION,base)
	@$(call install_fixup,  netflash, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  netflash, DESCRIPTION,missing)
	
	@$(call install_copy,   netflash, 0, 0, 755, $(PTXDIST_WORKSPACE)/local_src/tools/netflash/netflash, /etc/init.d/netflash)
	@$(call install_copy,   netflash, 0, 0, 755, $(PTXDIST_WORKSPACE)/local_src/tools/netflash/netflash_pivot, /bin/netflash_pivot)
	
	@$(call install_finish, netflash)
	
	@$(call touch)

# vim: syntax=make
