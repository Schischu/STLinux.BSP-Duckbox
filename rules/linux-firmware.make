# -*-makefile-*-
#
# Copyright (C) 2012 by fabricega
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LINUX_FIRMWARE) += linux-firmware

ifdef PTXCONF_LINUX_FIRMWARE
ifeq ($(shell test -x $(shell which git) || echo no),no)
    $(warning *** git is mandatory to get linux-firmware)
    $(warning *** please install git 'sudo apt-get install git')
    $(error )
endif
endif

#
# Paths and names
#
LINUX_FIRMWARE_VERSION	:= bbede96f3d68ac056ae5d9f6ac56819b6462f4f5
LINUX_FIRMWARE		:= linux-firmware-$(LINUX_FIRMWARE_VERSION)
LINUX_FIRMWARE_URL	:= http://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git
LINUX_FIRMWARE_SRCDIR	:= $(SRCDIR)/$(LINUX_FIRMWARE)
LINUX_FIRMWARE_DIR	:= $(BUILDDIR)/$(LINUX_FIRMWARE)
LINUX_FIRMWARE_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/linux-firmware.get:
	@$(call targetinfo)
	@if [ ! -e $(LINUX_FIRMWARE_SRCDIR) ]; then \
		cd $(SRCDIR); \
		git clone $(LINUX_FIRMWARE_URL) $(LINUX_FIRMWARE_SRCDIR) && \
		cd $(LINUX_FIRMWARE_SRCDIR) && \
		git checkout $(LINUX_FIRMWARE_VERSION); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/linux-firmware.extract:
	@$(call targetinfo)
	@mkdir -p $(LINUX_FIRMWARE_DIR) && \
		cp -a $(LINUX_FIRMWARE_SRCDIR)/* $(LINUX_FIRMWARE_DIR)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/linux-firmware.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/linux-firmware.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/linux-firmware.install:
	@$(call targetinfo)
	@mkdir -p $(LINUX_FIRMWARE_PKGDIR)/lib/firmware && \
		cp -ar $(LINUX_FIRMWARE_DIR)/* $(LINUX_FIRMWARE_PKGDIR)/lib/firmware
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/linux-firmware.targetinstall:
	@$(call targetinfo)

	@$(call install_init, linux-firmware)
	@$(call install_fixup, linux-firmware,PRIORITY,optional)
	@$(call install_fixup, linux-firmware,SECTION,base)
	@$(call install_fixup, linux-firmware,AUTHOR,"fabricega")
	@$(call install_fixup, linux-firmware,DESCRIPTION,missing)

ifdef PTXCONF_LINUX_FIRMWARE_RALINK
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/rt2561.bin, n)
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/rt2561s.bin, n)
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/rt2661.bin, n)
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/rt2860.bin, n)
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/rt2870.bin, n)
	@$(call install_link, linux-firmware, rt2870.bin , /lib/firmware/rt3070.bin, n)
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/rt3071.bin, n)
	@$(call install_link, linux-firmware, rt2860.bin , /lib/firmware/rt3090.bin, n)
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/rt3290.bin, n)
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/rt73.bin, n)
endif

ifdef PTXCONF_LINUX_FIRMWARE_DVB_USB_DIB0700
	@$(call install_copy, linux-firmware, 0, 0, 0644, -, /lib/firmware/dvb-usb-dib0700-1.20.fw, n)
endif

#	# FIXME: add more firmware files

	@$(call install_finish, linux-firmware)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/linux-firmware.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LINUX_FIRMWARE)

# vim: syntax=make
