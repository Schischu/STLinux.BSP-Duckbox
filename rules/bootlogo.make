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
ifdef PTXCONF_BOOTLOGO
PACKAGES-$(PTXCONF_BOOTLOGO_ENIGMA2)  += bootlogo-enigma2
PACKAGES-$(PTXCONF_BOOTLOGO_NEUTRINO) += bootlogo-neutrino
PACKAGES-$(PTXCONF_BOOTLOGO_VDR)      += bootlogo-vdr
PACKAGES-$(PTXCONF_BOOTLOGO_XBMC)     += bootlogo-xbmc
endif

BOOTLOGO_ENIGMA2_VERSION	:= 1.0
BOOTLOGO_NEUTRINO_VERSION	:= 1.0
BOOTLOGO_VDR_VERSION		:= 1.0
BOOTLOGO_XBMC_VERSION		:= 1.0

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/bootlogo-enigma2.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, bootlogo-enigma2)
	@$(call install_fixup, bootlogo-enigma2,PRIORITY,optional)
	@$(call install_fixup, bootlogo-enigma2,SECTION,base)
	@$(call install_fixup, bootlogo-enigma2,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, bootlogo-enigma2,DESCRIPTION,missing)
	
	@$(call install_copy, bootlogo-enigma2, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2.mvi, /boot/enigma2.mvi)
	
	@$(call install_finish, bootlogo-enigma2)
	
	@$(call touch)

$(STATEDIR)/bootlogo-neutrino.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, bootlogo-neutrino)
	@$(call install_fixup, bootlogo-neutrino,PRIORITY,optional)
	@$(call install_fixup, bootlogo-neutrino,SECTION,base)
	@$(call install_fixup, bootlogo-neutrino,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, bootlogo-neutrino,DESCRIPTION,missing)
	
	@$(call install_copy, bootlogo-neutrino, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/neutrino.mvi, /boot/neutrino.mvi)
	
	@$(call install_finish, bootlogo-neutrino)
	
	@$(call touch)

$(STATEDIR)/bootlogo-vdr.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, bootlogo-vdr)
	@$(call install_fixup, bootlogo-vdr,PRIORITY,optional)
	@$(call install_fixup, bootlogo-vdr,SECTION,base)
	@$(call install_fixup, bootlogo-vdr,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, bootlogo-vdr,DESCRIPTION,missing)
	
	@$(call install_copy, bootlogo-vdr, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/vdr.mvi, /boot/vdr.mvi)
	
	@$(call install_finish, bootlogo-vdr)
	
	@$(call touch)

$(STATEDIR)/bootlogo-xbmc.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, bootlogo-xbmc)
	@$(call install_fixup, bootlogo-xbmc,PRIORITY,optional)
	@$(call install_fixup, bootlogo-xbmc,SECTION,base)
	@$(call install_fixup, bootlogo-xbmc,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, bootlogo-xbmc,DESCRIPTION,missing)
	
	@$(call install_copy, bootlogo-xbmc, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/xbmc.mvi, /boot/xbmc.mvi)
	
	@$(call install_finish, bootlogo-xbmc)
	
	@$(call touch)
# vim: syntax=make
