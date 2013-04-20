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
PACKAGES-$(PTXCONF_STMFB_TOOLS) += stmfb-tools

#
# Paths and names and versions
#
STMFB_TOOLS_VERSION	:= 3.1_stm24_0104
STMFB_TOOLS		:= stmfb-$(STMFB_TOOLS_VERSION)-tools
STMFB_TOOLS_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/stmfb-$(STMFB_TOOLS_VERSION)/linux/tests
STMFB_TOOLS_DIR		:= $(BUILDDIR)/$(STMFB_TOOLS)
STMFB_TOOLS_LICENSE	:= unknown

ifdef PTXCONF_STMFB_TOOLS_ACP_TRANSMISSION
STMFB_TOOLS_SUBDIR += "acp-transmission"
endif

ifdef PTXCONF_STMFB_TOOLS_V4L2_FLEXVP_CTRL
STMFB_TOOLS_SUBDIR += "flexvp_ctrl"
endif

ifdef PTXCONF_STMFB_TOOLS_HDMI_AUDIO_CFG
STMFB_TOOLS_SUBDIR += "hdmi-audio-cfg"
endif

ifdef PTXCONF_STMFB_TOOLS_HDMI_CONTROL
STMFB_TOOLS_SUBDIR += "hdmi-control"
endif

ifdef PTXCONF_STMFB_TOOLS_HDMI_INFO
STMFB_TOOLS_SUBDIR += "hdmi-info"
endif

ifdef PTXCONF_STMFB_TOOLS_ISRC_TRANSMISSION
STMFB_TOOLS_SUBDIR += "isrc-transmission"
endif

ifdef PTXCONF_STMFB_TOOLS_PICTURE_CFG
STMFB_TOOLS_SUBDIR += "picture-cfg"
endif

ifdef PTXCONF_STMFB_TOOLS_DIRECTFB_SCREEN_ALIGNMENT
STMFB_TOOLS_SUBDIR += "screen_alignment"
endif

ifdef PTXCONF_STMFB_TOOLS_STFBSET
STMFB_TOOLS_SUBDIR += "stfbset"
endif

ifdef PTXCONF_STMFB_TOOLS_V4L2_V4L2LAYER_ALIGNMENT
STMFB_TOOLS_SUBDIR += "v4l2layer_alignment"
endif

ifdef PTXCONF_STMFB_TOOLS_V4L2_V4L2LUT8
STMFB_TOOLS_SUBDIR += "v4l2lut8"
endif

ifdef PTXCONF_STMFB_TOOLS_V4L2_V4L2STREAM
STMFB_TOOLS_SUBDIR += "v4l2stream"
endif

ifdef PTXCONF_STMFB_TOOLS_V4L2_V4L2VBI
STMFB_TOOLS_SUBDIR += "v4l2vbi"
endif

ifdef PTXCONF_STMFB_TOOLS_V4L2_YUVPLAYER
STMFB_TOOLS_SUBDIR += "yuvplayer"
endif

ifdef PTXCONF_STMFB_TOOLS_V4L2_ZORDER
STMFB_TOOLS_SUBDIR += "zorder"
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/stmfb-tools.prepare:
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/stmfb-tools.compile: $(STATEDIR)/driver-stmfb.install
	@$(call targetinfo)
	for dir in $(STMFB_TOOLS_SUBDIR); do \
		cd $(STMFB_TOOLS_DIR)/$${dir}; \
			$(MAKE) $(STMFB_TOOLS_MAKEVARS) \
			STG_TOPDIR=$(DRIVER_STMFB_DIR) \
			all; \
	done
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/stmfb-tools.install:
	@$(call targetinfo)
	mkdir -p $(STMFB_TOOLS_PKGDIR)/usr/bin
	for dir in $(STMFB_TOOLS_SUBDIR); do \
		cp $(STMFB_TOOLS_DIR)/$${dir}/$${dir} $(STMFB_TOOLS_PKGDIR)/usr/bin/; \
		chmod 755 $(STMFB_TOOLS_PKGDIR)/usr/bin/; \
	done
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/stmfb-tools.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  stmfb-tools)
	@$(call install_fixup, stmfb-tools, PRIORITY,optional)
	@$(call install_fixup, stmfb-tools, SECTION,base)
	@$(call install_fixup, stmfb-tools, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, stmfb-tools, DESCRIPTION,missing)

	for dir in $(STMFB_TOOLS_SUBDIR); do \
		$(call install_copy, stmfb-tools, 0, 0, 755, -, /usr/bin/$${dir##.}); \
	done

	@$(call install_finish, stmfb-tools)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/stmfb-tools.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, STMFB_TOOLS)

# vim: syntax=make
