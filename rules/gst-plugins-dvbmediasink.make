# -*-makefile-*-
#
# Copyright (C) 2010 by Erwin Rol <erwin@erwinrol.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GST_PLUGINS_DVBMEDIASINK) += gst-plugins-dvbmediasink

#
# Paths and names
#
GST_PLUGINS_DVBMEDIASINK_VERSION := 1.0
GST_PLUGINS_DVBMEDIASINK	 := gst-plugins-dvbmediasink
GST_PLUGINS_DVBMEDIASINK_URL	 := lndir://$(PTXDIST_WORKSPACE)/local_src/libs/$(GST_PLUGINS_DVBMEDIASINK)
GST_PLUGINS_DVBMEDIASINK_DIR	 := $(BUILDDIR)/$(GST_PLUGINS_DVBMEDIASINK)

$(STATEDIR)/libmmeimage.prepare: $(STATEDIR)/driver-player2.install

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
GST_PLUGINS_DVBMEDIASINK_CONF_TOOL	:= autoconf
GST_PLUGINS_DVBMEDIASINK_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR)

$(STATEDIR)/gst-plugins-dvbmediasink.prepare: $(STATEDIR)/driver-player2.install
	@$(call targetinfo)
	cd $(GST_PLUGINS_DVBMEDIASINK_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal -Im4; automake -a; autoconf
	@$(call world/prepare, GST_PLUGINS_DVBMEDIASINK)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/gst-plugins-dvbmediasink.targetinstall:
	@$(call targetinfo)

	@$(call install_init, gst-plugins-dvbmediasink)
	@$(call install_fixup, gst-plugins-dvbmediasink,PRIORITY,optional)
	@$(call install_fixup, gst-plugins-dvbmediasink,SECTION,base)
	@$(call install_fixup, gst-plugins-dvbmediasink,AUTHOR,"Erwin Rol <erwin@erwinrol.com>")
	@$(call install_fixup, gst-plugins-dvbmediasink,DESCRIPTION,missing)

	@for plugin in dvbaudiosink dvbvideosink; do \
		$(call install_copy, gst-plugins-dvbmediasink, 0, 0, 0644, -, \
			/usr/lib/gstreamer-0.10/libgst$${plugin}.so); \
	done

	@$(call install_finish, gst-plugins-dvbmediasink)

	@$(call touch)

# vim: syntax=make
