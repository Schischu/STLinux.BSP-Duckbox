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
PACKAGES-$(PTXCONF_AV_FIRMWARE) += av-firmware

#
# Paths and names
#
AV_FIRMWARE_VERSION	:= head2
AV_FIRMWARE 		:= av-firmware
AV_FIRMWARE_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/extra/$(AV_FIRMWARE)
AV_FIRMWARE_DIR		:= $(BUILDDIR)/$(AV_FIRMWARE)
AV_FIRMWARE_LICENSE	:= GPLv2

ifdef PTXCONF_CPU_SUBTYPE_STX7100
	VIDEO_ELF := video_7100
	AUDIO_ELF := audio_7100
endif

ifdef PTXCONF_CPU_SUBTYPE_STX7101
	VIDEO_ELF := video_7101
	AUDIO_ELF := audio_7101
endif

ifdef PTXCONF_CPU_SUBTYPE_STX7109
	VIDEO_ELF := video_7109
	AUDIO_ELF := audio_7109
endif

ifdef PTXCONF_CPU_SUBTYPE_STX7105
	VIDEO_ELF := video_7105
	AUDIO_ELF := audio_7105
endif

ifdef PTXCONF_CPU_SUBTYPE_STX7111
	VIDEO_ELF := video_7111
	AUDIO_ELF := audio_7111
endif

$(STATEDIR)/av-firmware.compile:
	@$(call touch)

$(STATEDIR)/av-firmware.install:
	@$(call targetinfo)
	
	@mkdir -pv  $(AV_FIRMWARE_PKGDIR)/lib/ $(AV_FIRMWARE_PKGDIR)/boot/ $(AV_FIRMWARE_PKGDIR)/lib/firmware/
	@cp -f $(AV_FIRMWARE_DIR)/$(VIDEO_ELF).elf $(AV_FIRMWARE_PKGDIR)/boot/
	@cp -f $(AV_FIRMWARE_DIR)/$(AUDIO_ELF).elf $(AV_FIRMWARE_PKGDIR)/boot/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/av-firmware.targetinstall:
	@$(call targetinfo)

	@$(call install_init, av-firmware)
	@$(call install_fixup, av-firmware,PRIORITY,optional)
	@$(call install_fixup, av-firmware,SECTION,base)
	@$(call install_fixup, av-firmware,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, av-firmware,DESCRIPTION,missing)

	@$(call install_copy, av-firmware, 0, 0, 644, $(AV_FIRMWARE_PKGDIR)/boot/$(VIDEO_ELF).elf, /boot/video.elf, 0)
	@$(call install_copy, av-firmware, 0, 0, 644, $(AV_FIRMWARE_PKGDIR)/boot/$(AUDIO_ELF).elf, /boot/audio.elf, 0)
	@$(call install_link, av-firmware, /boot/video.elf, /lib/firmware/video.elf)
	@$(call install_link, av-firmware, /boot/audio.elf, /lib/firmware/audio.elf)


	@$(call install_finish, av-firmware)

	@$(call touch)
# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/av-firmware.clean:
	@$(call targetinfo)
	@$(call clean_pkg, AV_FIRMWARE)

# vim: syntax=make
