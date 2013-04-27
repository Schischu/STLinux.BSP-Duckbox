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
PACKAGES-$(PTXCONF_STMFB_FIRMWARE) += stmfb-firmware

#
# Paths and names
#
STMFB_FIRMWARE_VERSION	:= 1.20
STMFB_FIRMWARE_MINOR_VERSION	:= 1
STMFB_FIRMWARE_MD5	:= 576ad13a6a9d112785fa74332b53dd25
STMFB_FIRMWARE_PREFIX := stlinux24-target
STMFB_FIRMWARE_NAME := stmfb-firmware
STMFB_FIRMWARE		:= $(STMFB_FIRMWARE_PREFIX)-$(STMFB_FIRMWARE_NAME)-$(STMFB_FIRMWARE_VERSION)-$(STMFB_FIRMWARE_MINOR_VERSION)
STMFB_FIRMWARE_SUFFIX	:= src.rpm
STMFB_FIRMWARE_URL	:= ftp://ftp.stlinux.com/pub/stlinux/2.4/updates/SRPMS/$(STMFB_FIRMWARE).$(STMFB_FIRMWARE_SUFFIX)
STMFB_FIRMWARE_SOURCE	:= $(SRCDIR)/$(STMFB_FIRMWARE).$(STMFB_FIRMWARE_SUFFIX)
STMFB_FIRMWARE_DIR	:= $(BUILDDIR)/$(STMFB_FIRMWARE)

$(STATEDIR)/stmfb-firmware.extract:
	@$(call targetinfo)
	@$(clean $(STMFB_FIRMWARE_DIR))
	@$(shell rm -rf $(STMFB_FIRMWARE_DIR); mkdir -p $(STMFB_FIRMWARE_DIR); cd $(STMFB_FIRMWARE_DIR); rpm2cpio $(STMFB_FIRMWARE_SOURCE) | cpio -i; tar -xf $(STMFB_FIRMWARE_NAME)s-v$(STMFB_FIRMWARE_VERSION).tar.gz)
	@$(call touch)

$(STATEDIR)/stmfb-firmware.compile:
	@$(call targetinfo)
	@$(call touch)

$(STATEDIR)/stmfb-firmware.install:
	@$(call targetinfo)
	
	@mkdir -p $(STMFB_FIRMWARE_PKGDIR)/lib/firmware/
	@cp -dp $(STMFB_FIRMWARE_DIR)/firmwares/* $(STMFB_FIRMWARE_PKGDIR)/lib/firmware/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/stmfb-firmware.targetinstall:
	@$(call targetinfo)

	@$(call install_init, stmfb-firmware)
	@$(call install_fixup, stmfb-firmware,PRIORITY,optional)
	@$(call install_fixup, stmfb-firmware,SECTION,base)
	@$(call install_fixup, stmfb-firmware,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, stmfb-firmware,DESCRIPTION,missing)

ifdef PTXCONF_CPU_SUBTYPE_STX7100
endif
ifdef PTXCONF_CPU_SUBTYPE_STX7101
	@$(call install_copy, stmfb-firmware, 0, 0, 644, -, /lib/firmware/component_7109c3.fw, 0)
endif
ifdef PTXCONF_CPU_SUBTYPE_STX7109
	@$(call install_copy, stmfb-firmware, 0, 0, 644, -, /lib/firmware/component_7109c3.fw, 0)
endif

ifdef PTXCONF_CPU_SUBTYPE_STX7105
	@$(call install_copy, stmfb-firmware, 0, 0, 644, -, /lib/firmware/component_7105.fw, 0)
endif
ifdef PTXCONF_CPU_SUBTYPE_STX7111
	@$(call install_copy, stmfb-firmware, 0, 0, 644, -, /lib/firmware/component_7111.fw, 0)
endif

	@$(call install_finish, stmfb-firmware)

	@$(call touch)

# vim: syntax=make
