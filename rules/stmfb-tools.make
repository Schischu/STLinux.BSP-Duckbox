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
STMFB_TOOLS		:= stmfb-tools-$(STMFB_TOOLS_VERSION)
STMFB_TOOLS_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/stmfb-$(STMFB_TOOLS_VERSION)/linux/tests
STMFB_TOOLS_DIR		:= $(BUILDDIR)/$(STMFB_TOOLS)
STMFB_TOOLS_LICENSE	:= unknown

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

ifdef PTXCONF_STMFB_TOOLS

# ----------------------------------------------------------------------------
# Tools
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_ACP_TRANSMISSION)  += stmfb-tools-acp-transmission
STMFB_TOOLS_ACP_TRANSMISSION_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_ACP_TRANSMISSION_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-acp-transmission.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-acp-transmission)
	@$(call install_fixup,  stmfb-tools-acp-transmission, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-acp-transmission, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-acp-transmission, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-acp-transmission, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-acp-transmission, 0, 0, -, /usr/bin/acp-transmission)
	
	@$(call install_finish, stmfb-tools-acp-transmission)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_V4L2_FLEXVP_CTRL)  += stmfb-tools-v4l2-flexvp-ctrl
STMFB_TOOLS_V4L2_FLEXVP_CTRL_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_V4L2_FLEXVP_CTRL_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-v4l2-flexvp-ctrl.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-v4l2-flexvp-ctrl)
	@$(call install_fixup,  stmfb-tools-v4l2-flexvp-ctrl, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-v4l2-flexvp-ctrl, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-v4l2-flexvp-ctrl, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-v4l2-flexvp-ctrl, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-v4l2-flexvp-ctrl, 0, 0, -, /usr/bin/flexvp_ctrl)
	
	@$(call install_finish, stmfb-tools-v4l2-flexvp-ctrl)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_HDMI_AUDIO_CFG)  += stmfb-tools-hdmi-audio-cfg
STMFB_TOOLS_HDMI_AUDIO_CFG_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_HDMI_AUDIO_CFG_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-hdmi-audio-cfg.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-hdmi-audio-cfg)
	@$(call install_fixup,  stmfb-tools-hdmi-audio-cfg, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-hdmi-audio-cfg, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-hdmi-audio-cfg, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-hdmi-audio-cfg, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-hdmi-audio-cfg, 0, 0, -, /usr/bin/hdmi-audio-cfg)
	
	@$(call install_finish, stmfb-tools-hdmi-audio-cfg)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_HDMI_CONTROL)  += stmfb-tools-hdmi-control
STMFB_TOOLS_HDMI_CONTROL_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_HDMI_CONTROL_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-hdmi-control.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-hdmi-control)
	@$(call install_fixup,  stmfb-tools-hdmi-control, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-hdmi-control, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-hdmi-control, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-hdmi-control, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-hdmi-control, 0, 0, -, /usr/bin/hdmi-control)
	
	@$(call install_finish, stmfb-tools-hdmi-control)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_HDMI_INFO)  += stmfb-tools-hdmi-info
STMFB_TOOLS_HDMI_INFO_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_HDMI_INFO_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-hdmi-info.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-hdmi-info)
	@$(call install_fixup,  stmfb-tools-hdmi-info, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-hdmi-info, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-hdmi-info, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-hdmi-info, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-hdmi-info, 0, 0, -, /usr/bin/hdmi-info)
	
	@$(call install_finish, stmfb-tools-hdmi-info)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_ISRC_TRANSMISSION)  += stmfb-tools-isrc-transmission
STMFB_TOOLS_ISRC_TRANSMISSION_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_ISRC_TRANSMISSION_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-isrc-transmission.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-isrc-transmission)
	@$(call install_fixup,  stmfb-tools-isrc-transmission, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-isrc-transmission, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-isrc-transmission, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-isrc-transmission, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-isrc-transmission, 0, 0, -, /usr/bin/isrc-transmission)
	
	@$(call install_finish, stmfb-tools-isrc-transmission)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_PICTURE_CFG)  += stmfb-tools-picture-cfg
STMFB_TOOLS_PICTURE_CFG_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_PICTURE_CFG_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-picture-cfg.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-picture-cfg)
	@$(call install_fixup,  stmfb-tools-picture-cfg, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-picture-cfg, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-picture-cfg, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-picture-cfg, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-picture-cfg, 0, 0, -, /usr/bin/picture-cfg)
	
	@$(call install_finish, stmfb-tools-picture-cfg)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_DIRECTFB_SCREEN_ALIGNMENT)  += stmfb-tools-directfb-screen-alignment
STMFB_TOOLS_DIRECTFB_SCREEN_ALIGNMENT_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_DIRECTFB_SCREEN_ALIGNMENT_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-directfb-screen-alignment.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-directfb-screen-alignment)
	@$(call install_fixup,  stmfb-tools-directfb-screen-alignment, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-directfb-screen-alignment, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-directfb-screen-alignment, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-directfb-screen-alignment, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-directfb-screen-alignment, 0, 0, -, /usr/bin/screen_alignment)
	
	@$(call install_finish, stmfb-tools-directfb-screen-alignment)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_STFBSET)  += stmfb-tools-stfbset
STMFB_TOOLS_STFBSET_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_STFBSET_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-stfbset.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-stfbset)
	@$(call install_fixup,  stmfb-tools-stfbset, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-stfbset, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-stfbset, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-stfbset, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-stfbset, 0, 0, -, /usr/bin/stfbset)
	
	@$(call install_finish, stmfb-tools-stfbset)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_V4L2_V4L2LAYER_ALIGNMENT)  += stmfb-tools-v4l2-v4l2layer-alignment
STMFB_TOOLS_V4L2_V4L2LAYER_ALIGNMENT_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_V4L2_V4L2LAYER_ALIGNMENT_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-v4l2-v4l2layer-alignment.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-v4l2-v4l2layer-alignment)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2layer-alignment, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2layer-alignment, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2layer-alignment, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2layer-alignment, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-v4l2-v4l2layer-alignment, 0, 0, -, /usr/bin/v4l2layer_alignment)
	
	@$(call install_finish, stmfb-tools-v4l2-v4l2layer-alignment)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_V4L2_V4L2LUT8)  += stmfb-tools-v4l2-v4l2lut8
STMFB_TOOLS_V4L2_V4L2LUT8_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_V4L2_V4L2LUT8_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-v4l2-v4l2lut8.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-v4l2-v4l2lut8)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2lut8, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2lut8, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2lut8, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2lut8, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-v4l2-v4l2lut8, 0, 0, -, /usr/bin/v4l2lut8)
	
	@$(call install_finish, stmfb-tools-v4l2-v4l2lut8)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_V4L2_V4L2STREAM)  += stmfb-tools-v4l2-v4l2stream
STMFB_TOOLS_V4L2_V4L2STREAM_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_V4L2_V4L2STREAM_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-v4l2-v4l2stream.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-v4l2-v4l2stream)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2stream, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2stream, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2stream, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2stream, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-v4l2-v4l2stream, 0, 0, -, /usr/bin/v4l2stream)
	
	@$(call install_finish, stmfb-tools-v4l2-v4l2stream)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_V4L2_V4L2VBI)  += stmfb-tools-v4l2-v4l2vbi
STMFB_TOOLS_V4L2_V4L2VBI_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_V4L2_V4L2VBI_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-v4l2-v4l2vbi.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-v4l2-v4l2vbi)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2vbi, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2vbi, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2vbi, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-v4l2-v4l2vbi, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-v4l2-v4l2vbi, 0, 0, -, /usr/bin/v4l2vbi)
	
	@$(call install_finish, stmfb-tools-v4l2-v4l2vbi)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_V4L2_YUVPLAYER)  += stmfb-tools-v4l2-yuvplayer
STMFB_TOOLS_V4L2_YUVPLAYER_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_V4L2_YUVPLAYER_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-v4l2-yuvplayer.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-v4l2-yuvplayer)
	@$(call install_fixup,  stmfb-tools-v4l2-yuvplayer, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-v4l2-yuvplayer, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-v4l2-yuvplayer, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-v4l2-yuvplayer, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-v4l2-yuvplayer, 0, 0, -, /usr/bin/yuvplayer)
	
	@$(call install_finish, stmfb-tools-v4l2-yuvplayer)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_STMFB_TOOLS_V4L2_ZORDER)  += stmfb-tools-v4l2-zorder
STMFB_TOOLS_V4L2_ZORDER_VERSION              := $(STMFB_TOOLS_VERSION)
STMFB_TOOLS_V4L2_ZORDER_PKGDIR               := $(STMFB_TOOLS_PKGDIR)

$(STATEDIR)/stmfb-tools-v4l2-zorder.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   stmfb-tools-v4l2-zorder)
	@$(call install_fixup,  stmfb-tools-v4l2-zorder, PRIORITY,    optional)
	@$(call install_fixup,  stmfb-tools-v4l2-zorder, SECTION,     base)
	@$(call install_fixup,  stmfb-tools-v4l2-zorder, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  stmfb-tools-v4l2-zorder, DESCRIPTION, missing)
	
	@$(call install_tree,   stmfb-tools-v4l2-zorder, 0, 0, -, /usr/bin/zorder)
	
	@$(call install_finish, stmfb-tools-v4l2-zorder)
	
	@$(call touch)

endif

# vim: syntax=make
