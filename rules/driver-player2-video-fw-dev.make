# -*-makefile-*-
#
# Copyright (C) @YEAR@ by @AUTHOR@
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DRIVER_PLAYER2_VIDEO_FW_DEV) += driver-player2-video-fw-dev

#
# Paths and names and versions
#
DRIVER_PLAYER2_VIDEO_FW_DEV_VERSION := $(call remove_quotes,$(PTXCONF_DRIVER_PLAYER2_VIDEO_FW_DEV_VERSION))
DRIVER_PLAYER2_VIDEO_FW_DEV         := vid_firmware-dev-$(DRIVER_PLAYER2_VIDEO_FW_DEV_VERSION)
DRIVER_PLAYER2_VIDEO_FW_DEV_SUFFIX  := tgz
DRIVER_PLAYER2_VIDEO_FW_DEV_SOURCE  := ${PTXDIST_WORKSPACE}/src_ext/$(DRIVER_PLAYER2_VIDEO_FW_DEV).$(DRIVER_PLAYER2_VIDEO_FW_DEV_SUFFIX)
DRIVER_PLAYER2_VIDEO_FW_DEV_DIR     := $(BUILDDIR)/$(DRIVER_PLAYER2_VIDEO_FW_DEV)
DRIVER_PLAYER2_VIDEO_FW_DEV_LICENSE := unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2-video-fw-dev.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2-video-fw-dev.prepare:
	@$(call targetinfo)
	
	find $(DRIVER_PLAYER2_VIDEO_FW_DEV_DIR) -name "*.h*" -exec chmod 644 {} \;
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2-video-fw-dev.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2-video-fw-dev.install:
	@$(call targetinfo)
	
	mkdir -p $(SYSROOT)/usr/include/linux
	cp -r $(DRIVER_PLAYER2_VIDEO_FW_DEV_DIR)/* $(SYSROOT)/usr/include/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2-video-fw-dev.targetinstall:
	@$(call targetinfo)
	@$(call touch)

# vim: syntax=make
