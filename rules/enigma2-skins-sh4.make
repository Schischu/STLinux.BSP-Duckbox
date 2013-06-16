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
PACKAGES-$(PTXCONF_ENIGMA2_SKINS_SH4) += enigma2-skins-sh4

#
# Paths and names
#
ENIGMA2_SKINS_SH4_VERSION	:= HEAD
ENIGMA2_SKINS_SH4		:= enigma2-skins-sh4-$(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKINS_SH4_URL	:= git://github.com/schpuntik/enigma2-skins-sh4.git
ENIGMA2_SKINS_SH4_SOURCE_GIT	:= $(SRCDIR)/schpuntik-skins-enigma2-sh4.git
ENIGMA2_SKINS_SH4_DIR	:= $(BUILDDIR)/$(ENIGMA2_SKINS_SH4) 
ENIGMA2_SKINS_SH4_LICENSE	:= GPLv2

$(STATEDIR)/enigma2-skins-sh4.get:
	@$(call targetinfo)
	
	( \
		if [ -d $(ENIGMA2_SKINS_SH4_SOURCE_GIT) ]; then \
			cd $(ENIGMA2_SKINS_SH4_SOURCE_GIT); \
			git pull -u origin master 2>&1 > /dev/null; \
			git checkout master 2>&1 > /dev/null; \
			git checkout HEAD 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone $(ENIGMA2_SKINS_SH4_URL) $(ENIGMA2_SKINS_SH4_SOURCE_GIT) 2>&1 > /dev/null; \
			cd $(ENIGMA2_SKINS_SH4_SOURCE_GIT); \
			git checkout master 2>&1 > /dev/null; \
		fi; \
	) 2>&1 > /dev/null
	
	( \
		if [ ! "$(ENIGMA2_SKINS_SH4_VERSION)" == "HEAD" ]; then \
			cd $(ENIGMA2_SKINS_SH4_SOURCE_GIT); \
			git checkout $(ENIGMA2_SKINS_SH4_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi; \
	) 2>&1 > /dev/null
	
	@$(call touch)

$(STATEDIR)/enigma2-skins-sh4.extract:
	@$(call targetinfo)
	
	rm -rf $(BUILDDIR)/$(ENIGMA2_SKINS_SH4); \
	cp -a $(ENIGMA2_SKINS_SH4_SOURCE_GIT) $(BUILDDIR)/$(ENIGMA2_SKINS_SH4); \
	rm -rf $(BUILDDIR)/$(ENIGMA2_SKINS_SH4)/.git;
	
	@$(call patchin, ENIGMA2_SKINS_SH4)
	
	cd $(ENIGMA2_SKINS_SH4_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		autoreconf -i; #aclocal -I m4; autoheader; automake -a; autoconf; 
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ENIGMA2_SKINS_SH4_PATH	:= PATH=$(CROSS_PATH)
ENIGMA2_SKINS_SH4_ENV 	:= $(CROSS_ENV)

#ENIGMA2_SKINS_SH4_CONF_TOOL	:= autoconf
ENIGMA2_SKINS_SH4_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--prefix=/usr \
	PYTHON=$(PTXDIST_SYSROOT_HOST)/bin/python2.7 \
	PY_PATH=$(SYSROOT)/usr \
	PKG_CONFIG=$(PTXDIST_SYSROOT_HOST)/bin/pkg-config \
	PKG_CONFIG_PATH=$(SYSROOT)/usr/lib/pkgconfig

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

ifdef PTXCONF_ENIGMA2_SKINS_SH4

# ----------------------------------------------------------------------------
# Extensions
# ----------------------------------------------------------------------------

#for p in `ls .`; do pb=`echo ${p^^} | sed -e 's/[^a-zA-Z0-9]/_/g'`; ps=`echo ${p,,} | sed -e 's/[^a-zA-Z0-9\-]/-/g'`; echo -e "PACKAGES-\$(PTXCONF_ENIGMA2_SKIN_${pb})  += enigma2-skin-${ps}\nENIGMA2_SKIN_${pb}_VERSION              := \$(ENIGMA2_SKINS_SH4_VERSION)\nENIGMA2_SKIN_${pb}_PKGDIR               := \$(ENIGMA2_SKINS_SH4_PKGDIR)\n\n\$(STATEDIR)/enigma2-skin-${ps}.targetinstall:\n\t@\$(call targetinfo)\n\t\n\t@\$(call install_init,   enigma2-skin-${ps})\n\t@\$(call install_fixup,  enigma2-skin-${ps}, PRIORITY,    optional)\n\t@\$(call install_fixup,  enigma2-skin-${ps}, SECTION,     base)\n\t@\$(call install_fixup,  enigma2-skin-${ps}, AUTHOR,      \"Robert Schwebel <r.schwebel@pengutronix.de>\")\n\t@\$(call install_fixup,  enigma2-skin-${ps}, DESCRIPTION, missing)\n\t\n\t@\$(call install_tree,   enigma2-skin-${ps}, 0, 0, -, /usr/share/enigma2/${p})\n\t\n\t@\$(call install_finish, enigma2-skin-${ps})\n\t\n\t@\$(call touch)\n\n# ----------------------------------------------------------------------------"; done

PACKAGES-$(PTXCONF_ENIGMA2_SKIN_BASIC_HD)  += enigma2-skin-basic-hd
ENIGMA2_SKIN_BASIC_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_BASIC_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-basic-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-basic-hd)
	@$(call install_fixup,  enigma2-skin-basic-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-basic-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-basic-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-basic-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-basic-hd, 0, 0, -, /usr/share/enigma2/BASIC-HD)
	
	@$(call install_finish, enigma2-skin-basic-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_BRUSHEDALU_HD)  += enigma2-skin-brushedalu-hd
ENIGMA2_SKIN_BRUSHEDALU_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_BRUSHEDALU_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-brushedalu-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-brushedalu-hd)
	@$(call install_fixup,  enigma2-skin-brushedalu-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-brushedalu-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-brushedalu-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-brushedalu-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-brushedalu-hd, 0, 0, -, /usr/share/enigma2/BrushedAlu-HD)
	
	@$(call install_finish, enigma2-skin-brushedalu-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_DMCONCINNITY_HD)  += enigma2-skin-dmconcinnity-hd
ENIGMA2_SKIN_DMCONCINNITY_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_DMCONCINNITY_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-dmconcinnity-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-dmconcinnity-hd)
	@$(call install_fixup,  enigma2-skin-dmconcinnity-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-dmconcinnity-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-dmconcinnity-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-dmconcinnity-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-dmconcinnity-hd, 0, 0, -, /usr/share/enigma2/DMConcinnity-HD)
	
	@$(call install_finish, enigma2-skin-dmconcinnity-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_DMM_HD)  += enigma2-skin-dmm-hd
ENIGMA2_SKIN_DMM_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_DMM_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-dmm-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-dmm-hd)
	@$(call install_fixup,  enigma2-skin-dmm-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-dmm-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-dmm-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-dmm-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-dmm-hd, 0, 0, -, /usr/share/enigma2/dmm-HD)
	
	@$(call install_finish, enigma2-skin-dmm-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_DREAMMM_HD)  += enigma2-skin-dreammm-hd
ENIGMA2_SKIN_DREAMMM_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_DREAMMM_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-dreammm-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-dreammm-hd)
	@$(call install_fixup,  enigma2-skin-dreammm-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-dreammm-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-dreammm-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-dreammm-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-dreammm-hd, 0, 0, -, /usr/share/enigma2/DreamMM-HD)
	
	@$(call install_finish, enigma2-skin-dreammm-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_DREAMTV_HD)  += enigma2-skin-dreamtv-hd
ENIGMA2_SKIN_DREAMTV_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_DREAMTV_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-dreamtv-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-dreamtv-hd)
	@$(call install_fixup,  enigma2-skin-dreamtv-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-dreamtv-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-dreamtv-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-dreamtv-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-dreamtv-hd, 0, 0, -, /usr/share/enigma2/dreamTV-HD)
	
	@$(call install_finish, enigma2-skin-dreamtv-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_DTV_HD)  += enigma2-skin-dtv-hd
ENIGMA2_SKIN_DTV_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_DTV_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-dtv-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-dtv-hd)
	@$(call install_fixup,  enigma2-skin-dtv-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-dtv-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-dtv-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-dtv-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-dtv-hd, 0, 0, -, /usr/share/enigma2/dTV-HD)
	
	@$(call install_finish, enigma2-skin-dtv-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_DTV_HD_RELOADED)  += enigma2-skin-dtv-hd-reloaded
ENIGMA2_SKIN_DTV_HD_RELOADED_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_DTV_HD_RELOADED_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-dtv-hd-reloaded.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-dtv-hd-reloaded)
	@$(call install_fixup,  enigma2-skin-dtv-hd-reloaded, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-dtv-hd-reloaded, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-dtv-hd-reloaded, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-dtv-hd-reloaded, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-dtv-hd-reloaded, 0, 0, -, /usr/share/enigma2/dTV-HD-Reloaded)
	
	@$(call install_finish, enigma2-skin-dtv-hd-reloaded)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_ELGATO_HD)  += enigma2-skin-elgato-hd
ENIGMA2_SKIN_ELGATO_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_ELGATO_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-elgato-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-elgato-hd)
	@$(call install_fixup,  enigma2-skin-elgato-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-elgato-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-elgato-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-elgato-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-elgato-hd, 0, 0, -, /usr/share/enigma2/Elgato-HD)
	
	@$(call install_finish, enigma2-skin-elgato-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_KERNI_HD1)  += enigma2-skin-kerni-hd1
ENIGMA2_SKIN_KERNI_HD1_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_KERNI_HD1_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-kerni-hd1.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-kerni-hd1)
	@$(call install_fixup,  enigma2-skin-kerni-hd1, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-kerni-hd1, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-kerni-hd1, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-kerni-hd1, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-kerni-hd1, 0, 0, -, /usr/share/enigma2/Kerni-HD1)
	
	@$(call install_finish, enigma2-skin-kerni-hd1)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_KERNI_HD1R2)  += enigma2-skin-kerni-hd1r2
ENIGMA2_SKIN_KERNI_HD1R2_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_KERNI_HD1R2_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-kerni-hd1r2.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-kerni-hd1r2)
	@$(call install_fixup,  enigma2-skin-kerni-hd1r2, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-kerni-hd1r2, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-kerni-hd1r2, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-kerni-hd1r2, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-kerni-hd1r2, 0, 0, -, /usr/share/enigma2/Kerni-HD1R2)
	
	@$(call install_finish, enigma2-skin-kerni-hd1r2)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_BLACKBOX)  += enigma2-skin-nemesis-blackbox
ENIGMA2_SKIN_NEMESIS_BLACKBOX_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_BLACKBOX_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-blackbox.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-blackbox)
	@$(call install_fixup,  enigma2-skin-nemesis-blackbox, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-blackbox, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-blackbox, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-blackbox, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-blackbox, 0, 0, -, /usr/share/enigma2/Nemesis.BlackBox)
	
	@$(call install_finish, enigma2-skin-nemesis-blackbox)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_BLUELINE)  += enigma2-skin-nemesis-blueline
ENIGMA2_SKIN_NEMESIS_BLUELINE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_BLUELINE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-blueline.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-blueline)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-blueline, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-blueline, 0, 0, -, /usr/share/enigma2/Nemesis.Blueline)
	
	@$(call install_finish, enigma2-skin-nemesis-blueline)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_BLUELINE_EXTENDED)  += enigma2-skin-nemesis-blueline-extended
ENIGMA2_SKIN_NEMESIS_BLUELINE_EXTENDED_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_BLUELINE_EXTENDED_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-blueline-extended.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-blueline-extended)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline-extended, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline-extended, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline-extended, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-blueline-extended, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-blueline-extended, 0, 0, -, /usr/share/enigma2/Nemesis.Blueline.Extended)
	
	@$(call install_finish, enigma2-skin-nemesis-blueline-extended)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_BLUELINE_SINGLE)  += enigma2-skin-nemesis-blueline-single
ENIGMA2_SKIN_NEMESIS_BLUELINE_SINGLE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_BLUELINE_SINGLE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-blueline-single.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-blueline-single)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline-single, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline-single, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-blueline-single, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-blueline-single, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-blueline-single, 0, 0, -, /usr/share/enigma2/Nemesis.Blueline.Single)
	
	@$(call install_finish, enigma2-skin-nemesis-blueline-single)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_CHROMELINE)  += enigma2-skin-nemesis-chromeline
ENIGMA2_SKIN_NEMESIS_CHROMELINE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_CHROMELINE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-chromeline.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-chromeline)
	@$(call install_fixup,  enigma2-skin-nemesis-chromeline, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-chromeline, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-chromeline, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-chromeline, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-chromeline, 0, 0, -, /usr/share/enigma2/Nemesis.ChromeLine)
	
	@$(call install_finish, enigma2-skin-nemesis-chromeline)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_CHROMELINE_COBOLT)  += enigma2-skin-nemesis-chromeline-cobolt
ENIGMA2_SKIN_NEMESIS_CHROMELINE_COBOLT_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_CHROMELINE_COBOLT_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-chromeline-cobolt.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-chromeline-cobolt)
	@$(call install_fixup,  enigma2-skin-nemesis-chromeline-cobolt, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-chromeline-cobolt, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-chromeline-cobolt, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-chromeline-cobolt, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-chromeline-cobolt, 0, 0, -, /usr/share/enigma2/Nemesis.ChromeLine.Cobolt)
	
	@$(call install_finish, enigma2-skin-nemesis-chromeline-cobolt)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_FLATLINE)  += enigma2-skin-nemesis-flatline
ENIGMA2_SKIN_NEMESIS_FLATLINE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_FLATLINE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-flatline.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-flatline)
	@$(call install_fixup,  enigma2-skin-nemesis-flatline, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-flatline, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-flatline, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-flatline, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-flatline, 0, 0, -, /usr/share/enigma2/Nemesis.Flatline)
	
	@$(call install_finish, enigma2-skin-nemesis-flatline)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_FLATLINE_BLUE)  += enigma2-skin-nemesis-flatline-blue
ENIGMA2_SKIN_NEMESIS_FLATLINE_BLUE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_FLATLINE_BLUE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-flatline-blue.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-flatline-blue)
	@$(call install_fixup,  enigma2-skin-nemesis-flatline-blue, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-flatline-blue, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-flatline-blue, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-flatline-blue, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-flatline-blue, 0, 0, -, /usr/share/enigma2/Nemesis.Flatline.Blue)
	
	@$(call install_finish, enigma2-skin-nemesis-flatline-blue)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_GLASSLINE)  += enigma2-skin-nemesis-glassline
ENIGMA2_SKIN_NEMESIS_GLASSLINE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_GLASSLINE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-glassline.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-glassline)
	@$(call install_fixup,  enigma2-skin-nemesis-glassline, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-glassline, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-glassline, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-glassline, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-glassline, 0, 0, -, /usr/share/enigma2/Nemesis.GlassLine)
	
	@$(call install_finish, enigma2-skin-nemesis-glassline)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_GREENLINE)  += enigma2-skin-nemesis-greenline
ENIGMA2_SKIN_NEMESIS_GREENLINE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_GREENLINE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-greenline.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-greenline)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-greenline, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-greenline, 0, 0, -, /usr/share/enigma2/Nemesis.Greenline)
	
	@$(call install_finish, enigma2-skin-nemesis-greenline)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_GREENLINE_EXTENDED)  += enigma2-skin-nemesis-greenline-extended
ENIGMA2_SKIN_NEMESIS_GREENLINE_EXTENDED_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_GREENLINE_EXTENDED_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-greenline-extended.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-greenline-extended)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline-extended, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline-extended, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline-extended, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-greenline-extended, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-greenline-extended, 0, 0, -, /usr/share/enigma2/Nemesis.Greenline.Extended)
	
	@$(call install_finish, enigma2-skin-nemesis-greenline-extended)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_GREENLINE_SINGLE)  += enigma2-skin-nemesis-greenline-single
ENIGMA2_SKIN_NEMESIS_GREENLINE_SINGLE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_GREENLINE_SINGLE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-greenline-single.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-greenline-single)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline-single, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline-single, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-greenline-single, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-greenline-single, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-greenline-single, 0, 0, -, /usr/share/enigma2/Nemesis.Greenline.Single)
	
	@$(call install_finish, enigma2-skin-nemesis-greenline-single)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_GREYLINE)  += enigma2-skin-nemesis-greyline
ENIGMA2_SKIN_NEMESIS_GREYLINE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_GREYLINE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-greyline.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-greyline)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-greyline, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-greyline, 0, 0, -, /usr/share/enigma2/Nemesis.Greyline)
	
	@$(call install_finish, enigma2-skin-nemesis-greyline)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_GREYLINE_EXTENDED)  += enigma2-skin-nemesis-greyline-extended
ENIGMA2_SKIN_NEMESIS_GREYLINE_EXTENDED_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_GREYLINE_EXTENDED_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-greyline-extended.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-greyline-extended)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline-extended, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline-extended, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline-extended, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-greyline-extended, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-greyline-extended, 0, 0, -, /usr/share/enigma2/Nemesis.Greyline.Extended)
	
	@$(call install_finish, enigma2-skin-nemesis-greyline-extended)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_GREYLINE_SINGLE)  += enigma2-skin-nemesis-greyline-single
ENIGMA2_SKIN_NEMESIS_GREYLINE_SINGLE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_GREYLINE_SINGLE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-greyline-single.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-greyline-single)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline-single, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline-single, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-greyline-single, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-greyline-single, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-greyline-single, 0, 0, -, /usr/share/enigma2/Nemesis.Greyline.Single)
	
	@$(call install_finish, enigma2-skin-nemesis-greyline-single)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_NEMESIS_SHADOWLINE)  += enigma2-skin-nemesis-shadowline
ENIGMA2_SKIN_NEMESIS_SHADOWLINE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_NEMESIS_SHADOWLINE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-nemesis-shadowline.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-nemesis-shadowline)
	@$(call install_fixup,  enigma2-skin-nemesis-shadowline, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-nemesis-shadowline, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-nemesis-shadowline, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-nemesis-shadowline, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-nemesis-shadowline, 0, 0, -, /usr/share/enigma2/Nemesis.ShadowLine)
	
	@$(call install_finish, enigma2-skin-nemesis-shadowline)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_SIMPLE)  += enigma2-skin-simple
ENIGMA2_SKIN_SIMPLE_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_SIMPLE_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-simple.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-simple)
	@$(call install_fixup,  enigma2-skin-simple, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-simple, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-simple, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-simple, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-simple, 0, 0, -, /usr/share/enigma2/simple)
	
	@$(call install_finish, enigma2-skin-simple)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_SWAIN)  += enigma2-skin-swain
ENIGMA2_SKIN_SWAIN_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_SWAIN_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-swain.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-swain)
	@$(call install_fixup,  enigma2-skin-swain, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-swain, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-swain, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-swain, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-swain, 0, 0, -, /usr/share/enigma2/SWAIN)
	
	@$(call install_finish, enigma2-skin-swain)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_SWAIN_HD)  += enigma2-skin-swain-hd
ENIGMA2_SKIN_SWAIN_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_SWAIN_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-swain-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-swain-hd)
	@$(call install_fixup,  enigma2-skin-swain-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-swain-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-swain-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-swain-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-swain-hd, 0, 0, -, /usr/share/enigma2/SWAIN-HD)
	
	@$(call install_finish, enigma2-skin-swain-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_ULTRAVIOLET)  += enigma2-skin-ultraviolet
ENIGMA2_SKIN_ULTRAVIOLET_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_ULTRAVIOLET_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-ultraviolet.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-ultraviolet)
	@$(call install_fixup,  enigma2-skin-ultraviolet, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-ultraviolet, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-ultraviolet, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-ultraviolet, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-ultraviolet, 0, 0, -, /usr/share/enigma2/UltraViolet)
	
	@$(call install_finish, enigma2-skin-ultraviolet)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_VALI_HD_ATLANTIS)  += enigma2-skin-vali-hd-atlantis
ENIGMA2_SKIN_VALI_HD_ATLANTIS_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_VALI_HD_ATLANTIS_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-vali-hd-atlantis.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-vali-hd-atlantis)
	@$(call install_fixup,  enigma2-skin-vali-hd-atlantis, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-vali-hd-atlantis, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-vali-hd-atlantis, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-vali-hd-atlantis, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-vali-hd-atlantis, 0, 0, -, /usr/share/enigma2/Vali.HD.atlantis)
	
	@$(call install_finish, enigma2-skin-vali-hd-atlantis)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_VALI_HD_NANO)  += enigma2-skin-vali-hd-nano
ENIGMA2_SKIN_VALI_HD_NANO_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_VALI_HD_NANO_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-vali-hd-nano.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-vali-hd-nano)
	@$(call install_fixup,  enigma2-skin-vali-hd-nano, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-vali-hd-nano, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-vali-hd-nano, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-vali-hd-nano, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-vali-hd-nano, 0, 0, -, /usr/share/enigma2/Vali.HD.nano)
	
	@$(call install_finish, enigma2-skin-vali-hd-nano)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_VALI_HD_WARP)  += enigma2-skin-vali-hd-warp
ENIGMA2_SKIN_VALI_HD_WARP_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_VALI_HD_WARP_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-vali-hd-warp.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-vali-hd-warp)
	@$(call install_fixup,  enigma2-skin-vali-hd-warp, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-vali-hd-warp, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-vali-hd-warp, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-vali-hd-warp, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-vali-hd-warp, 0, 0, -, /usr/share/enigma2/Vali.HD.warp)
	
	@$(call install_finish, enigma2-skin-vali-hd-warp)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_VALI_XD)  += enigma2-skin-vali-xd
ENIGMA2_SKIN_VALI_XD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_VALI_XD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-vali-xd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-vali-xd)
	@$(call install_fixup,  enigma2-skin-vali-xd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-vali-xd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-vali-xd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-vali-xd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-vali-xd, 0, 0, -, /usr/share/enigma2/Vali-XD)
	
	@$(call install_finish, enigma2-skin-vali-xd)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_YADS_HD)  += enigma2-skin-yads-hd
ENIGMA2_SKIN_YADS_HD_VERSION              := $(ENIGMA2_SKINS_SH4_VERSION)
ENIGMA2_SKIN_YADS_HD_PKGDIR               := $(ENIGMA2_SKINS_SH4_PKGDIR)

$(STATEDIR)/enigma2-skin-yads-hd.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   enigma2-skin-yads-hd)
	@$(call install_fixup,  enigma2-skin-yads-hd, PRIORITY,    optional)
	@$(call install_fixup,  enigma2-skin-yads-hd, SECTION,     base)
	@$(call install_fixup,  enigma2-skin-yads-hd, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  enigma2-skin-yads-hd, DESCRIPTION, missing)
	
	@$(call install_tree,   enigma2-skin-yads-hd, 0, 0, -, /usr/share/enigma2/YADS-HD)
	
	@$(call install_finish, enigma2-skin-yads-hd)
	
	@$(call touch)

# ----------------------------------------------------------------------------

endif
# vim: syntax=make
