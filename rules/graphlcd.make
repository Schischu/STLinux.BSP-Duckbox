# -*-makefile-*-
#
# Copyright (C) 2003 by Robert Schwebel <r.schwebel@pengutronix.de>
#                       Pengutronix <info@pengutronix.de>, Germany
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GRAPHLCD) += graphlcd

#
# Paths and names
#
GRAPHLCD_VERSION	:= 1e01a8963f9ab95ba40ddb44a6c166b8e546053d
GRAPHLCD		:= graphlcd-$(GRAPHLCD_VERSION)
GRAPHLCD_URL		:= git://projects.vdr-developer.org/graphlcd-base.git
GRAPHLCD_GIT_BRANCH	:= master
GRAPHLCD_GIT_HEAD	:= $(GRAPHLCD_VERSION)
GRAPHLCD_SOURCE		:= $(SRCDIR)/graphlcd-base.git
GRAPHLCD_DIR		:= $(BUILDDIR)/$(GRAPHLCD)
GRAPHLCD_LICENSE	:= graphlcd

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
GRAPHLCD_PATH	:= PATH=$(CROSS_PATH)
GRAPHLCD_CONF_ENV 	:= $(CROSS_ENV) \
	PKG_CONFIG=$(PTXDIST_SYSROOT_HOST)/bin/pkg-config \
	PKG_CONFIG_PATH=$(SYSROOT)/usr/lib/pkgconfig

$(STATEDIR)/graphlcd.compile: $(STATEDIR)/libusb.install.post
	@$(call targetinfo)

	cd $(GRAPHLCD_DIR) && $(GRAPHLCD_PATH) $(GRAPHLCD_CONF_ENV) \
		$(MAKE) all
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

ifdef PTXCONF_GRAPHLCD

PACKAGES-$(PTXCONF_GRAPHLCD_CONF)  += graphlcd-conf
GRAPHLCD_CONF_VERSION              := $(GRAPHLCD_VERSION)
GRAPHLCD_CONF_PKGDIR               := $(GRAPHLCD_PKGDIR)

$(STATEDIR)/graphlcd-conf.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   graphlcd-conf)
	@$(call install_fixup,  graphlcd-conf, PRIORITY,    optional)
	@$(call install_fixup,  graphlcd-conf, SECTION,     base)
	@$(call install_fixup,  graphlcd-conf, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  graphlcd-conf, DESCRIPTION, missing)
	
	@$(call install_copy, graphlcd-conf, 0, 0, 0644, -, /etc/graphlcd.conf)
	
	@$(call install_finish, graphlcd-conf)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_GRAPHLCD_DRIVERS)  += graphlcd-drivers
GRAPHLCD_DRIVERS_VERSION              := $(GRAPHLCD_VERSION)
GRAPHLCD_DRIVERS_PKGDIR               := $(GRAPHLCD_PKGDIR)

$(STATEDIR)/graphlcd-drivers.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   graphlcd-drivers)
	@$(call install_fixup,  graphlcd-drivers, PRIORITY,    optional)
	@$(call install_fixup,  graphlcd-drivers, SECTION,     base)
	@$(call install_fixup,  graphlcd-drivers, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  graphlcd-drivers, DESCRIPTION, missing)
	
	@$(call install_lib, graphlcd-drivers, 0, 0, 0644, libglcddrivers)
	
	@$(call install_finish, graphlcd-drivers)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_GRAPHLCD_GRAPHICS)  += graphlcd-graphics
GRAPHLCD_GRAPHICS_VERSION              := $(GRAPHLCD_VERSION)
GRAPHLCD_GRAPHICS_PKGDIR               := $(GRAPHLCD_PKGDIR)

$(STATEDIR)/graphlcd-graphics.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   graphlcd-graphics)
	@$(call install_fixup,  graphlcd-graphics, PRIORITY,    optional)
	@$(call install_fixup,  graphlcd-graphics, SECTION,     base)
	@$(call install_fixup,  graphlcd-graphics, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  graphlcd-graphics, DESCRIPTION, missing)
	
	@$(call install_lib, graphlcd-graphics, 0, 0, 0644, libglcdgraphics)
	
	@$(call install_finish, graphlcd-graphics)
	
	@$(call touch)

# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_GRAPHLCD_SKIN)  += graphlcd-skin
GRAPHLCD_SKIN_VERSION              := $(GRAPHLCD_VERSION)
GRAPHLCD_SKIN_PKGDIR               := $(GRAPHLCD_PKGDIR)

$(STATEDIR)/graphlcd-skin.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   graphlcd-skin)
	@$(call install_fixup,  graphlcd-skin, PRIORITY,    optional)
	@$(call install_fixup,  graphlcd-skin, SECTION,     base)
	@$(call install_fixup,  graphlcd-skin, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  graphlcd-skin, DESCRIPTION, missing)
	
	@$(call install_lib, graphlcd-skin, 0, 0, 0644, libglcdskin)
	
	@$(call install_finish, graphlcd-skin)
	
	@$(call touch)

endif

# vim: syntax=make
