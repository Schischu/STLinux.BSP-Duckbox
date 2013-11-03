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
PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS) += xbmc-pvr-addons

#
# Paths and names
#

XBMC_PVR_ADDONS_VERSION	:= frodo
XBMC_PVR_ADDONS		:= xbmc-pvr-addons-$(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_URL	:= git://github.com/opdenkamp/xbmc-pvr-addons
XBMC_PVR_ADDONS_GIT_BRANCH := frodo
XBMC_PVR_ADDONS_GIT_HEAD := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_SOURCE	:= $(SRCDIR)/xbmc-pvr-addons.git
XBMC_PVR_ADDONS_DIR	:= $(BUILDDIR)/$(XBMC_PVR_ADDONS)
XBMC_PVR_ADDONS_LICENSE	:= GPLv3

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XBMC_PVR_ADDONS_CONF_ENV	:= $(CROSS_ENV)
XBMC_PVR_ADDONS_PATH		:= PATH=$(CROSS_PATH)

#
# autoconf
#
XBMC_PVR_ADDONS_CONF_OPT := \
	$(CROSS_AUTOCONF_USR) \
	--libdir=/usr/lib/xbmc/addons \
	--disable-static \
	--disable-mysql \
	--enable-addons-with-dependencies \
	--enable-shared


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xbmc-pvr-addons.targetinstall:
	@$(call targetinfo)

	@$(call install_init, xbmc-pvr-addons)
	@$(call install_fixup, xbmc-pvr-addons,PRIORITY,optional)
	@$(call install_fixup, xbmc-pvr-addons,SECTION,base)
	@$(call install_fixup, xbmc-pvr-addons,AUTHOR,"fabricega")
	@$(call install_fixup, xbmc-pvr-addons,DESCRIPTION,missing)
	@$(call install_finish, xbmc-pvr-addons)

	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR ARGUSTV
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_ARGUSTV)  += xbmc-pvr-addons-argustv
XBMC_PVR_ADDONS_ARGUSTV_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_ARGUSTV_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-argustv.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-argustv)
	@$(call install_fixup,  xbmc-pvr-addons-argustv, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-argustv, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-argustv, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-argustv, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-argustv, 0, 0, -, /usr/lib/xbmc/addons/pvr.argustv)
	@$(call install_tree,   xbmc-pvr-addons-argustv, 0, 0, -, /usr/share/xbmc/addons/pvr.argustv)
	
	@$(call install_finish, xbmc-pvr-addons-argustv)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR DEMO
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_DEMO)  += xbmc-pvr-addons-demo
XBMC_PVR_ADDONS_DEMO_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_DEMO_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-demo.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-demo)
	@$(call install_fixup,  xbmc-pvr-addons-demo, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-demo, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-demo, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-demo, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-demo, 0, 0, -, /usr/lib/xbmc/addons/pvr.demo)
	@$(call install_tree,   xbmc-pvr-addons-demo, 0, 0, -, /usr/share/xbmc/addons/pvr.demo)
	
	@$(call install_finish, xbmc-pvr-addons-demo)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR DVBVIEWER
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_DVBVIEWER)  += xbmc-pvr-addons-dvbviewer
XBMC_PVR_ADDONS_DVBVIEWER_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_DVBVIEWER_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-dvbviewer.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-dvbviewer)
	@$(call install_fixup,  xbmc-pvr-addons-dvbviewer, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-dvbviewer, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-dvbviewer, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-dvbviewer, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-dvbviewer, 0, 0, -, /usr/lib/xbmc/addons/pvr.dvbviewer)
	@$(call install_tree,   xbmc-pvr-addons-dvbviewer, 0, 0, -, /usr/share/xbmc/addons/pvr.dvbviewer)
	
	@$(call install_finish, xbmc-pvr-addons-dvbviewer)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR HTS
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_HTS)  += xbmc-pvr-addons-hts
XBMC_PVR_ADDONS_HTS_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_HTS_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-hts.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-hts)
	@$(call install_fixup,  xbmc-pvr-addons-hts, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-hts, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-hts, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-hts, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-hts, 0, 0, -, /usr/lib/xbmc/addons/pvr.hts)
	@$(call install_tree,   xbmc-pvr-addons-hts, 0, 0, -, /usr/share/xbmc/addons/pvr.hts)
	
	@$(call install_finish, xbmc-pvr-addons-hts)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR IPTVSIMPLE
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_IPTVSIMPLE)  += xbmc-pvr-addons-iptvsimple
XBMC_PVR_ADDONS_IPTVSIMPLE_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_IPTVSIMPLE_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-iptvsimple.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-iptvsimple)
	@$(call install_fixup,  xbmc-pvr-addons-iptvsimple, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-iptvsimple, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-iptvsimple, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-iptvsimple, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-iptvsimple, 0, 0, -, /usr/lib/xbmc/addons/pvr.iptvsimple)
	@$(call install_tree,   xbmc-pvr-addons-iptvsimple, 0, 0, -, /usr/share/xbmc/addons/pvr.iptvsimple)
	
	@$(call install_finish, xbmc-pvr-addons-iptvsimple)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR MEDIAPORTAL_SERVER
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_MEDIAPORTAL_SERVER)  += xbmc-pvr-addons-mediaportal-tvserver
XBMC_PVR_ADDONS_MEDIAPORTAL_SERVER_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_MEDIAPORTAL_SERVER_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-mediaportal-tvserver.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-mediaportal-tvserver)
	@$(call install_fixup,  xbmc-pvr-addons-mediaportal-tvserver, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-mediaportal-tvserver, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-mediaportal-tvserver, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-mediaportal-tvserver, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-mediaportal-tvserver, 0, 0, -, /usr/lib/xbmc/addons/pvr.mediaportal.tvserver)
	@$(call install_tree,   xbmc-pvr-addons-mediaportal-tvserver, 0, 0, -, /usr/share/xbmc/addons/pvr.mediaportal.tvserver)
	
	@$(call install_finish, xbmc-pvr-addons-mediaportal-tvserver)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR NEXTPVR
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_NEXTPVR)  += xbmc-pvr-addons-nextpvr
XBMC_PVR_ADDONS_NEXTPVR_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_NEXTPVR_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-nextpvr.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-nextpvr)
	@$(call install_fixup,  xbmc-pvr-addons-nextpvr, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-nextpvr, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-nextpvr, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-nextpvr, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-nextpvr, 0, 0, -, /usr/lib/xbmc/addons/pvr.nextpvr)
	@$(call install_tree,   xbmc-pvr-addons-nextpvr, 0, 0, -, /usr/share/xbmc/addons/pvr.nextpvr)
	
	@$(call install_finish, xbmc-pvr-addons-nextpvr)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR NJOY
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_NJOY)  += xbmc-pvr-addons-njoy
XBMC_PVR_ADDONS_NJOY_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_NJOY_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-njoy.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-njoy)
	@$(call install_fixup,  xbmc-pvr-addons-njoy, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-njoy, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-njoy, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-njoy, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-njoy, 0, 0, -, /usr/lib/xbmc/addons/pvr.njoy)
	@$(call install_tree,   xbmc-pvr-addons-njoy, 0, 0, -, /usr/share/xbmc/addons/pvr.njoy)
	
	@$(call install_finish, xbmc-pvr-addons-njoy)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR VDR_VNSI
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_VDR_VNSI)  += xbmc-pvr-addons-vdr-vnsi
XBMC_PVR_ADDONS_VDR_VNSI_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_VDR_VNSI_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-vdr-vnsi.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-vdr-vnsi)
	@$(call install_fixup,  xbmc-pvr-addons-vdr-vnsi, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-vdr-vnsi, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-vdr-vnsi, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-vdr-vnsi, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-vdr-vnsi, 0, 0, -, /usr/lib/xbmc/addons/pvr.vdr.vnsi)
	@$(call install_tree,   xbmc-pvr-addons-vdr-vnsi, 0, 0, -, /usr/share/xbmc/addons/pvr.vdr.vnsi)
	
	@$(call install_finish, xbmc-pvr-addons-vdr-vnsi)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# ADDONS PVR VUPLUS
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_XBMC_PVR_ADDONS_VUPLUS)  += xbmc-pvr-addons-vuplus
XBMC_PVR_ADDONS_VUPLUS_VERSION              := $(XBMC_PVR_ADDONS_VERSION)
XBMC_PVR_ADDONS_VUPLUS_PKGDIR               := $(XBMC_PVR_ADDONS_PKGDIR)

$(STATEDIR)/xbmc-pvr-addons-vuplus.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-pvr-addons-vuplus)
	@$(call install_fixup,  xbmc-pvr-addons-vuplus, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-pvr-addons-vuplus, SECTION,     base)
	@$(call install_fixup,  xbmc-pvr-addons-vuplus, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-pvr-addons-vuplus, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-pvr-addons-vuplus, 0, 0, -, /usr/lib/xbmc/addons/pvr.vuplus)
	@$(call install_tree,   xbmc-pvr-addons-vuplus, 0, 0, -, /usr/share/xbmc/addons/pvr.vuplus)
	
	@$(call install_finish, xbmc-pvr-addons-vuplus)
	
	@$(call touch)

# vim: syntax=make