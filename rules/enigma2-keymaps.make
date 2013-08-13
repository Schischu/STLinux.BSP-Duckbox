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

PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP)     += enigma2-keymap

#
# Paths and names
#

ENIGMA2_KEYMAP_BOX_VERSION      := $(PTXCONF_PLATFORM)
ENIGMA2_KEYMAP_HEAD_VERSION     := 1.0
ENIGMA2_KEYMAP_VERSION          := $(ENIGMA2_KEYMAP_BOX_VERSION).$(ENIGMA2_KEYMAP_HEAD_VERSION)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/enigma2-keymap.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap)
	@$(call install_fixup, enigma2-keymap,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap,SECTION,base)
	@$(call install_fixup, enigma2-keymap,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap,DESCRIPTION,missing)

	@$(call install_alternative, enigma2-keymap, 0, 0, 644, /usr/share/enigma2/keymap.xml)

	@$(call install_finish, enigma2-keymap)

	@$(call touch)


# vim: syntax=make
