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
PACKAGES-$(PTXCONF_XBMC) += xbmc

#
# Paths and names
#
ifdef PTXCONF_XBMC_VERSION_201301022234
XBMC_VERSION	:= 7a6cb7f49ae19dca3c48c40fa3bd20dc3c490e60
endif

XBMC		:= xbmc-$(XBMC_VERSION)
XBMC_URL	:= git://github.com/xbmc/xbmc.git
XBMC_SOURCE_GIT	:= $(SRCDIR)/xbmc.git
XBMC_DIR	:= $(BUILDDIR)/$(XBMC)
XBMC_LICENSE	:= xbmc

XBMC_DEV_VERSION	:= $(XBMC_VERSION)
XBMC_DEV_PKGDIR	:= $(XBMC_PKGDIR)

$(STATEDIR)/xbmc.get:
	@$(call targetinfo)
	
		if [ -d $(XBMC_SOURCE_GIT) ]; then \
			cd $(XBMC_SOURCE_GIT); \
			git pull -u origin master 2>&1 > /dev/null; \
			git checkout HEAD 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone  $(XBMC_URL) $(XBMC_SOURCE_GIT) 2>&1 > /dev/null; \
		fi; 2>&1 > /dev/null
	
		if [ ! "$(XBMC_VERSION)" == "HEAD" ]; then \
			cd $(XBMC_SOURCE_GIT); \
			git checkout $(XBMC_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi; 2>&1 > /dev/null
	
	@$(call touch)

PATH_PATCHES = $(subst :, ,$(PTXDIST_PATH_PATCHES))

$(STATEDIR)/xbmc.extract:
	@$(call targetinfo)
	
	rm -rf $(XBMC_DIR); \
	cp -a $(XBMC_SOURCE_GIT) $(XBMC_DIR); \
	rm -rf $(XBMC_DIR)/.git;
	
	@$(call patchin, XBMC)
	#cd $(XBMC_DIR) && sh autogen.sh
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

XBMC_PATH	:= PATH=$(CROSS_PATH)
XBMC_ENV 	:= $(CROSS_ENV)

XBMC_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--prefix=/usr \
		TEXTUREPACKER_NATIVE_ROOT=/usr \
		SWIG_EXE=none \
		JRE_EXE=none \
		--disable-gl \
		--enable-glesv1 \
		--disable-gles \
		--disable-sdl \
		--enable-webserver \
		--enable-nfs \
		--disable-x11 \
		--disable-samba \
		--disable-mysql \
		--disable-joystick \
		--disable-rsxs \
		--disable-projectm \
		--disable-goom \
		--disable-afpclient \
		--disable-airplay \
		--disable-airtunes \
		--disable-dvdcss \
		--disable-hal \
		--disable-avahi \
		--disable-optical-drive \
		--disable-libbluray \
		--disable-texturepacker \
		--disable-udev \
		--disable-libusb \
		--disable-libcec \
		--enable-gstreamer \
		--disable-paplayer \
		--enable-gstplayer \
		--enable-dvdplayer \
		--disable-pulse \
		--disable-alsa \
		--disable-ssh \
		PYTHON=$(PTXDIST_SYSROOT_HOST)/bin/python2.7 \
		PY_PATH=$(SYSROOT)/usr \
		PYTHON_VERSION=2.7 \
		PKG_CONFIG=$(PTXDIST_SYSROOT_HOST)/bin/pkg-config \
		PKG_CONFIG_PATH=$(SYSROOT)/usr/lib/pkgconfig

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/xbmc.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  xbmc)
	@$(call install_fixup, xbmc, PRIORITY,    optional)
	@$(call install_fixup, xbmc, SECTION,     base)
	@$(call install_fixup, xbmc, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, xbmc, DESCRIPTION, missing)
	
	@$(call install_copy, xbmc, 0, 0, 755, -, /usr/bin/xbmc)
	@$(call install_copy, xbmc, 0, 0, 755, -, /usr/bin/xbmc-standalone)
	
	@$(call install_copy, xbmc, 0, 0, 755, -, /usr/lib/xbmc/xbmc.bin, y)
	@$(call install_tree, xbmc, 0, 0, -, /usr/lib/xbmc/addons)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/lib/xbmc/system/hdhomerun-sh.so, y)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/lib/xbmc/system/ImageLib-sh.so, y)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/lib/xbmc/system/libcpluff-sh.so, y)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/lib/xbmc/system/libexif-sh.so, y)
	@$(call install_tree, xbmc, 0, 0, -, /usr/lib/xbmc/system/players/dvdplayer)
	
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/library.xbmc.addon)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/library.xbmc.gui)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/library.xbmc.pvr)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/repository.xbmc.org)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/xbmc.addon)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/xbmc.core)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/xbmc.gui)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/xbmc.json)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/xbmc.metadata)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/xbmc.pvr)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/addons/xbmc.python)
	
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/media)
	@$(call install_copy, xbmc, 0, 0, 755, -, /usr/share/xbmc/system/keymaps/appcommand.xml)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/share/xbmc/system/keymaps/keyboard.xml)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/share/xbmc/system/keymaps/remote.xml)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/system/library)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/system/python)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/share/xbmc/system/colors.xml)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/share/xbmc/system/peripherals.xml)
	@$(call install_copy, xbmc, 0, 0, 644, -, /usr/share/xbmc/system/playercorefactory.xml)
	@$(call install_tree, xbmc, 0, 0, -, /usr/share/xbmc/userdata)
	@$(call install_copy, xbmc, 0, 0, 755, -, /usr/share/xbmc/FEH.py)
	
	@$(call install_finish, xbmc)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Language
# ----------------------------------------------------------------------------

#SAVEIFS=$IFS; IFS=$(echo -en "\n\b"); for p in `ls .`; do pb=`echo ${p^^} | sed -e 's/[()]//g' | sed -e 's/[^a-zA-Z0-9]/_/g'`; ps=`echo ${p,,} | sed -e 's/[()]//g' | sed -e 's/[^a-zA-Z0-9\-]/-/g'`; pf=`echo ${p} | sed -e 's/(/\\\\(/g' | sed -e 's/)/\\\\)/g' | sed -e 's/ /\\\\ /g'`; echo -e "PACKAGES-\$(PTXCONF_XBMC_LANGUAGE_${pb})  += xbmc-language-${ps}\nXBMC_LANGUAGE_${pb}_VERSION              := \$(XBMC_VERSION)\nXBMC_LANGUAGE_${pb}_PKGDIR               := \$(XBMC_PKGDIR)\n\n\$(STATEDIR)/xbmc-language-${ps}.targetinstall:\n\t@\$(call targetinfo)\n\t\n\t@\$(call install_init,   xbmc-language-${ps})\n\t@\$(call install_fixup,  xbmc-language-${ps}, PRIORITY,    optional)\n\t@\$(call install_fixup,  xbmc-language-${ps}, SECTION,     base)\n\t@\$(call install_fixup,  xbmc-language-${ps}, AUTHOR,      \"Robert Schwebel <r.schwebel@pengutronix.de>\")\n\t@\$(call install_fixup,  xbmc-language-${ps}, DESCRIPTION, missing)\n\t\n\t@\$(call install_tree,   xbmc-language-${ps}, 0, 0, -, /usr/share/xbmc/language/${pf})\n\t\n\t@\$(call install_finish, xbmc-language-${ps})\n\t\n\t@\$(call touch)\n\n# ----------------------------------------------------------------------------"; done; IFS=$SAVEIFS

PACKAGES-$(PTXCONF_XBMC_LANGUAGE_AFRIKAANS)  += xbmc-language-afrikaans
XBMC_LANGUAGE_AFRIKAANS_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_AFRIKAANS_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-afrikaans.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-afrikaans)
	@$(call install_fixup,  xbmc-language-afrikaans, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-afrikaans, SECTION,     base)
	@$(call install_fixup,  xbmc-language-afrikaans, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-afrikaans, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-afrikaans, 0, 0, -, /usr/share/xbmc/language/Afrikaans)
	
	@$(call install_finish, xbmc-language-afrikaans)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_AMHARIC)  += xbmc-language-amharic
XBMC_LANGUAGE_AMHARIC_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_AMHARIC_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-amharic.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-amharic)
	@$(call install_fixup,  xbmc-language-amharic, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-amharic, SECTION,     base)
	@$(call install_fixup,  xbmc-language-amharic, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-amharic, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-amharic, 0, 0, -, /usr/share/xbmc/language/Amharic)
	
	@$(call install_finish, xbmc-language-amharic)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_ARABIC)  += xbmc-language-arabic
XBMC_LANGUAGE_ARABIC_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_ARABIC_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-arabic.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-arabic)
	@$(call install_fixup,  xbmc-language-arabic, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-arabic, SECTION,     base)
	@$(call install_fixup,  xbmc-language-arabic, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-arabic, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-arabic, 0, 0, -, /usr/share/xbmc/language/Arabic)
	
	@$(call install_finish, xbmc-language-arabic)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_BASQUE)  += xbmc-language-basque
XBMC_LANGUAGE_BASQUE_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_BASQUE_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-basque.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-basque)
	@$(call install_fixup,  xbmc-language-basque, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-basque, SECTION,     base)
	@$(call install_fixup,  xbmc-language-basque, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-basque, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-basque, 0, 0, -, /usr/share/xbmc/language/Basque)
	
	@$(call install_finish, xbmc-language-basque)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_BELARUSIAN)  += xbmc-language-belarusian
XBMC_LANGUAGE_BELARUSIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_BELARUSIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-belarusian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-belarusian)
	@$(call install_fixup,  xbmc-language-belarusian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-belarusian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-belarusian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-belarusian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-belarusian, 0, 0, -, /usr/share/xbmc/language/Belarusian)
	
	@$(call install_finish, xbmc-language-belarusian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_BOSNIAN)  += xbmc-language-bosnian
XBMC_LANGUAGE_BOSNIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_BOSNIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-bosnian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-bosnian)
	@$(call install_fixup,  xbmc-language-bosnian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-bosnian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-bosnian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-bosnian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-bosnian, 0, 0, -, /usr/share/xbmc/language/Bosnian)
	
	@$(call install_finish, xbmc-language-bosnian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_BULGARIAN)  += xbmc-language-bulgarian
XBMC_LANGUAGE_BULGARIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_BULGARIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-bulgarian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-bulgarian)
	@$(call install_fixup,  xbmc-language-bulgarian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-bulgarian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-bulgarian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-bulgarian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-bulgarian, 0, 0, -, /usr/share/xbmc/language/Bulgarian)
	
	@$(call install_finish, xbmc-language-bulgarian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_CATALAN)  += xbmc-language-catalan
XBMC_LANGUAGE_CATALAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_CATALAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-catalan.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-catalan)
	@$(call install_fixup,  xbmc-language-catalan, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-catalan, SECTION,     base)
	@$(call install_fixup,  xbmc-language-catalan, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-catalan, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-catalan, 0, 0, -, /usr/share/xbmc/language/Catalan)
	
	@$(call install_finish, xbmc-language-catalan)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_CHINESE_SIMPLE)  += xbmc-language-chinese-simple
XBMC_LANGUAGE_CHINESE_SIMPLE_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_CHINESE_SIMPLE_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-chinese-simple.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-chinese-simple)
	@$(call install_fixup,  xbmc-language-chinese-simple, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-chinese-simple, SECTION,     base)
	@$(call install_fixup,  xbmc-language-chinese-simple, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-chinese-simple, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-chinese-simple, 0, 0, -, /usr/share/xbmc/language/Chinese\ \(Simple\))
	
	@$(call install_finish, xbmc-language-chinese-simple)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_CHINESE_TRADITIONAL)  += xbmc-language-chinese-traditional
XBMC_LANGUAGE_CHINESE_TRADITIONAL_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_CHINESE_TRADITIONAL_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-chinese-traditional.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-chinese-traditional)
	@$(call install_fixup,  xbmc-language-chinese-traditional, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-chinese-traditional, SECTION,     base)
	@$(call install_fixup,  xbmc-language-chinese-traditional, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-chinese-traditional, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-chinese-traditional, 0, 0, -, /usr/share/xbmc/language/Chinese\ \(Traditional\))
	
	@$(call install_finish, xbmc-language-chinese-traditional)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_CROATIAN)  += xbmc-language-croatian
XBMC_LANGUAGE_CROATIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_CROATIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-croatian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-croatian)
	@$(call install_fixup,  xbmc-language-croatian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-croatian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-croatian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-croatian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-croatian, 0, 0, -, /usr/share/xbmc/language/Croatian)
	
	@$(call install_finish, xbmc-language-croatian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_CZECH)  += xbmc-language-czech
XBMC_LANGUAGE_CZECH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_CZECH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-czech.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-czech)
	@$(call install_fixup,  xbmc-language-czech, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-czech, SECTION,     base)
	@$(call install_fixup,  xbmc-language-czech, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-czech, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-czech, 0, 0, -, /usr/share/xbmc/language/Czech)
	
	@$(call install_finish, xbmc-language-czech)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_DANISH)  += xbmc-language-danish
XBMC_LANGUAGE_DANISH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_DANISH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-danish.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-danish)
	@$(call install_fixup,  xbmc-language-danish, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-danish, SECTION,     base)
	@$(call install_fixup,  xbmc-language-danish, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-danish, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-danish, 0, 0, -, /usr/share/xbmc/language/Danish)
	
	@$(call install_finish, xbmc-language-danish)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_DUTCH)  += xbmc-language-dutch
XBMC_LANGUAGE_DUTCH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_DUTCH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-dutch.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-dutch)
	@$(call install_fixup,  xbmc-language-dutch, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-dutch, SECTION,     base)
	@$(call install_fixup,  xbmc-language-dutch, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-dutch, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-dutch, 0, 0, -, /usr/share/xbmc/language/Dutch)
	
	@$(call install_finish, xbmc-language-dutch)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_ENGLISH)  += xbmc-language-english
XBMC_LANGUAGE_ENGLISH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_ENGLISH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-english.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-english)
	@$(call install_fixup,  xbmc-language-english, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-english, SECTION,     base)
	@$(call install_fixup,  xbmc-language-english, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-english, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-english, 0, 0, -, /usr/share/xbmc/language/English)
	
	@$(call install_finish, xbmc-language-english)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_ENGLISH_US)  += xbmc-language-english-us
XBMC_LANGUAGE_ENGLISH_US_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_ENGLISH_US_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-english-us.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-english-us)
	@$(call install_fixup,  xbmc-language-english-us, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-english-us, SECTION,     base)
	@$(call install_fixup,  xbmc-language-english-us, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-english-us, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-english-us, 0, 0, -, /usr/share/xbmc/language/English\ \(US\))
	
	@$(call install_finish, xbmc-language-english-us)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_ESPERANTO)  += xbmc-language-esperanto
XBMC_LANGUAGE_ESPERANTO_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_ESPERANTO_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-esperanto.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-esperanto)
	@$(call install_fixup,  xbmc-language-esperanto, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-esperanto, SECTION,     base)
	@$(call install_fixup,  xbmc-language-esperanto, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-esperanto, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-esperanto, 0, 0, -, /usr/share/xbmc/language/Esperanto)
	
	@$(call install_finish, xbmc-language-esperanto)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_ESTONIAN)  += xbmc-language-estonian
XBMC_LANGUAGE_ESTONIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_ESTONIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-estonian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-estonian)
	@$(call install_fixup,  xbmc-language-estonian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-estonian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-estonian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-estonian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-estonian, 0, 0, -, /usr/share/xbmc/language/Estonian)
	
	@$(call install_finish, xbmc-language-estonian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_FINNISH)  += xbmc-language-finnish
XBMC_LANGUAGE_FINNISH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_FINNISH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-finnish.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-finnish)
	@$(call install_fixup,  xbmc-language-finnish, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-finnish, SECTION,     base)
	@$(call install_fixup,  xbmc-language-finnish, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-finnish, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-finnish, 0, 0, -, /usr/share/xbmc/language/Finnish)
	
	@$(call install_finish, xbmc-language-finnish)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_FRENCH)  += xbmc-language-french
XBMC_LANGUAGE_FRENCH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_FRENCH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-french.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-french)
	@$(call install_fixup,  xbmc-language-french, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-french, SECTION,     base)
	@$(call install_fixup,  xbmc-language-french, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-french, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-french, 0, 0, -, /usr/share/xbmc/language/French)
	
	@$(call install_finish, xbmc-language-french)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_GALICIAN)  += xbmc-language-galician
XBMC_LANGUAGE_GALICIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_GALICIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-galician.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-galician)
	@$(call install_fixup,  xbmc-language-galician, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-galician, SECTION,     base)
	@$(call install_fixup,  xbmc-language-galician, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-galician, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-galician, 0, 0, -, /usr/share/xbmc/language/Galician)
	
	@$(call install_finish, xbmc-language-galician)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_GERMAN)  += xbmc-language-german
XBMC_LANGUAGE_GERMAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_GERMAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-german.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-german)
	@$(call install_fixup,  xbmc-language-german, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-german, SECTION,     base)
	@$(call install_fixup,  xbmc-language-german, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-german, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-german, 0, 0, -, /usr/share/xbmc/language/German)
	
	@$(call install_finish, xbmc-language-german)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_GREEK)  += xbmc-language-greek
XBMC_LANGUAGE_GREEK_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_GREEK_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-greek.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-greek)
	@$(call install_fixup,  xbmc-language-greek, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-greek, SECTION,     base)
	@$(call install_fixup,  xbmc-language-greek, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-greek, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-greek, 0, 0, -, /usr/share/xbmc/language/Greek)
	
	@$(call install_finish, xbmc-language-greek)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_HEBREW)  += xbmc-language-hebrew
XBMC_LANGUAGE_HEBREW_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_HEBREW_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-hebrew.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-hebrew)
	@$(call install_fixup,  xbmc-language-hebrew, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-hebrew, SECTION,     base)
	@$(call install_fixup,  xbmc-language-hebrew, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-hebrew, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-hebrew, 0, 0, -, /usr/share/xbmc/language/Hebrew)
	
	@$(call install_finish, xbmc-language-hebrew)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_HINDI_DEVANAGIRI)  += xbmc-language-hindi-devanagiri
XBMC_LANGUAGE_HINDI_DEVANAGIRI_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_HINDI_DEVANAGIRI_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-hindi-devanagiri.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-hindi-devanagiri)
	@$(call install_fixup,  xbmc-language-hindi-devanagiri, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-hindi-devanagiri, SECTION,     base)
	@$(call install_fixup,  xbmc-language-hindi-devanagiri, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-hindi-devanagiri, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-hindi-devanagiri, 0, 0, -, /usr/share/xbmc/language/Hindi\ \(Devanagiri\))
	
	@$(call install_finish, xbmc-language-hindi-devanagiri)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_HUNGARIAN)  += xbmc-language-hungarian
XBMC_LANGUAGE_HUNGARIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_HUNGARIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-hungarian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-hungarian)
	@$(call install_fixup,  xbmc-language-hungarian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-hungarian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-hungarian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-hungarian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-hungarian, 0, 0, -, /usr/share/xbmc/language/Hungarian)
	
	@$(call install_finish, xbmc-language-hungarian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_ICELANDIC)  += xbmc-language-icelandic
XBMC_LANGUAGE_ICELANDIC_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_ICELANDIC_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-icelandic.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-icelandic)
	@$(call install_fixup,  xbmc-language-icelandic, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-icelandic, SECTION,     base)
	@$(call install_fixup,  xbmc-language-icelandic, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-icelandic, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-icelandic, 0, 0, -, /usr/share/xbmc/language/Icelandic)
	
	@$(call install_finish, xbmc-language-icelandic)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_INDONESIAN)  += xbmc-language-indonesian
XBMC_LANGUAGE_INDONESIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_INDONESIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-indonesian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-indonesian)
	@$(call install_fixup,  xbmc-language-indonesian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-indonesian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-indonesian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-indonesian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-indonesian, 0, 0, -, /usr/share/xbmc/language/Indonesian)
	
	@$(call install_finish, xbmc-language-indonesian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_ITALIAN)  += xbmc-language-italian
XBMC_LANGUAGE_ITALIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_ITALIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-italian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-italian)
	@$(call install_fixup,  xbmc-language-italian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-italian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-italian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-italian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-italian, 0, 0, -, /usr/share/xbmc/language/Italian)
	
	@$(call install_finish, xbmc-language-italian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_JAPANESE)  += xbmc-language-japanese
XBMC_LANGUAGE_JAPANESE_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_JAPANESE_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-japanese.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-japanese)
	@$(call install_fixup,  xbmc-language-japanese, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-japanese, SECTION,     base)
	@$(call install_fixup,  xbmc-language-japanese, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-japanese, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-japanese, 0, 0, -, /usr/share/xbmc/language/Japanese)
	
	@$(call install_finish, xbmc-language-japanese)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_KOREAN)  += xbmc-language-korean
XBMC_LANGUAGE_KOREAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_KOREAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-korean.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-korean)
	@$(call install_fixup,  xbmc-language-korean, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-korean, SECTION,     base)
	@$(call install_fixup,  xbmc-language-korean, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-korean, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-korean, 0, 0, -, /usr/share/xbmc/language/Korean)
	
	@$(call install_finish, xbmc-language-korean)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_LITHUANIAN)  += xbmc-language-lithuanian
XBMC_LANGUAGE_LITHUANIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_LITHUANIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-lithuanian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-lithuanian)
	@$(call install_fixup,  xbmc-language-lithuanian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-lithuanian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-lithuanian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-lithuanian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-lithuanian, 0, 0, -, /usr/share/xbmc/language/Lithuanian)
	
	@$(call install_finish, xbmc-language-lithuanian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_MACEDONIAN)  += xbmc-language-macedonian
XBMC_LANGUAGE_MACEDONIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_MACEDONIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-macedonian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-macedonian)
	@$(call install_fixup,  xbmc-language-macedonian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-macedonian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-macedonian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-macedonian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-macedonian, 0, 0, -, /usr/share/xbmc/language/Macedonian)
	
	@$(call install_finish, xbmc-language-macedonian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_MALTESE)  += xbmc-language-maltese
XBMC_LANGUAGE_MALTESE_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_MALTESE_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-maltese.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-maltese)
	@$(call install_fixup,  xbmc-language-maltese, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-maltese, SECTION,     base)
	@$(call install_fixup,  xbmc-language-maltese, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-maltese, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-maltese, 0, 0, -, /usr/share/xbmc/language/Maltese)
	
	@$(call install_finish, xbmc-language-maltese)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_NORWEGIAN)  += xbmc-language-norwegian
XBMC_LANGUAGE_NORWEGIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_NORWEGIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-norwegian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-norwegian)
	@$(call install_fixup,  xbmc-language-norwegian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-norwegian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-norwegian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-norwegian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-norwegian, 0, 0, -, /usr/share/xbmc/language/Norwegian)
	
	@$(call install_finish, xbmc-language-norwegian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_POLISH)  += xbmc-language-polish
XBMC_LANGUAGE_POLISH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_POLISH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-polish.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-polish)
	@$(call install_fixup,  xbmc-language-polish, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-polish, SECTION,     base)
	@$(call install_fixup,  xbmc-language-polish, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-polish, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-polish, 0, 0, -, /usr/share/xbmc/language/Polish)
	
	@$(call install_finish, xbmc-language-polish)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_PORTUGUESE)  += xbmc-language-portuguese
XBMC_LANGUAGE_PORTUGUESE_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_PORTUGUESE_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-portuguese.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-portuguese)
	@$(call install_fixup,  xbmc-language-portuguese, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-portuguese, SECTION,     base)
	@$(call install_fixup,  xbmc-language-portuguese, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-portuguese, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-portuguese, 0, 0, -, /usr/share/xbmc/language/Portuguese)
	
	@$(call install_finish, xbmc-language-portuguese)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_PORTUGUESE_BRAZIL)  += xbmc-language-portuguese-brazil
XBMC_LANGUAGE_PORTUGUESE_BRAZIL_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_PORTUGUESE_BRAZIL_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-portuguese-brazil.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-portuguese-brazil)
	@$(call install_fixup,  xbmc-language-portuguese-brazil, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-portuguese-brazil, SECTION,     base)
	@$(call install_fixup,  xbmc-language-portuguese-brazil, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-portuguese-brazil, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-portuguese-brazil, 0, 0, -, /usr/share/xbmc/language/Portuguese\ \(Brazil\))
	
	@$(call install_finish, xbmc-language-portuguese-brazil)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_ROMANIAN)  += xbmc-language-romanian
XBMC_LANGUAGE_ROMANIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_ROMANIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-romanian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-romanian)
	@$(call install_fixup,  xbmc-language-romanian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-romanian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-romanian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-romanian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-romanian, 0, 0, -, /usr/share/xbmc/language/Romanian)
	
	@$(call install_finish, xbmc-language-romanian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_RUSSIAN)  += xbmc-language-russian
XBMC_LANGUAGE_RUSSIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_RUSSIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-russian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-russian)
	@$(call install_fixup,  xbmc-language-russian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-russian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-russian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-russian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-russian, 0, 0, -, /usr/share/xbmc/language/Russian)
	
	@$(call install_finish, xbmc-language-russian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_SERBIAN)  += xbmc-language-serbian
XBMC_LANGUAGE_SERBIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_SERBIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-serbian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-serbian)
	@$(call install_fixup,  xbmc-language-serbian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-serbian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-serbian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-serbian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-serbian, 0, 0, -, /usr/share/xbmc/language/Serbian)
	
	@$(call install_finish, xbmc-language-serbian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_SERBIAN_CYRILLIC)  += xbmc-language-serbian-cyrillic
XBMC_LANGUAGE_SERBIAN_CYRILLIC_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_SERBIAN_CYRILLIC_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-serbian-cyrillic.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-serbian-cyrillic)
	@$(call install_fixup,  xbmc-language-serbian-cyrillic, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-serbian-cyrillic, SECTION,     base)
	@$(call install_fixup,  xbmc-language-serbian-cyrillic, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-serbian-cyrillic, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-serbian-cyrillic, 0, 0, -, /usr/share/xbmc/language/Serbian\ \(Cyrillic\))
	
	@$(call install_finish, xbmc-language-serbian-cyrillic)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_SLOVAK)  += xbmc-language-slovak
XBMC_LANGUAGE_SLOVAK_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_SLOVAK_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-slovak.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-slovak)
	@$(call install_fixup,  xbmc-language-slovak, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-slovak, SECTION,     base)
	@$(call install_fixup,  xbmc-language-slovak, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-slovak, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-slovak, 0, 0, -, /usr/share/xbmc/language/Slovak)
	
	@$(call install_finish, xbmc-language-slovak)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_SLOVENIAN)  += xbmc-language-slovenian
XBMC_LANGUAGE_SLOVENIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_SLOVENIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-slovenian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-slovenian)
	@$(call install_fixup,  xbmc-language-slovenian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-slovenian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-slovenian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-slovenian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-slovenian, 0, 0, -, /usr/share/xbmc/language/Slovenian)
	
	@$(call install_finish, xbmc-language-slovenian)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_SPANISH)  += xbmc-language-spanish
XBMC_LANGUAGE_SPANISH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_SPANISH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-spanish.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-spanish)
	@$(call install_fixup,  xbmc-language-spanish, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-spanish, SECTION,     base)
	@$(call install_fixup,  xbmc-language-spanish, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-spanish, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-spanish, 0, 0, -, /usr/share/xbmc/language/Spanish)
	
	@$(call install_finish, xbmc-language-spanish)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_SPANISH_ARGENTINA)  += xbmc-language-spanish-argentina
XBMC_LANGUAGE_SPANISH_ARGENTINA_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_SPANISH_ARGENTINA_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-spanish-argentina.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-spanish-argentina)
	@$(call install_fixup,  xbmc-language-spanish-argentina, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-spanish-argentina, SECTION,     base)
	@$(call install_fixup,  xbmc-language-spanish-argentina, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-spanish-argentina, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-spanish-argentina, 0, 0, -, /usr/share/xbmc/language/Spanish\ \(Argentina\))
	
	@$(call install_finish, xbmc-language-spanish-argentina)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_SPANISH_MEXICO)  += xbmc-language-spanish-mexico
XBMC_LANGUAGE_SPANISH_MEXICO_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_SPANISH_MEXICO_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-spanish-mexico.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-spanish-mexico)
	@$(call install_fixup,  xbmc-language-spanish-mexico, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-spanish-mexico, SECTION,     base)
	@$(call install_fixup,  xbmc-language-spanish-mexico, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-spanish-mexico, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-spanish-mexico, 0, 0, -, /usr/share/xbmc/language/Spanish\ \(Mexico\))
	
	@$(call install_finish, xbmc-language-spanish-mexico)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_SWEDISH)  += xbmc-language-swedish
XBMC_LANGUAGE_SWEDISH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_SWEDISH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-swedish.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-swedish)
	@$(call install_fixup,  xbmc-language-swedish, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-swedish, SECTION,     base)
	@$(call install_fixup,  xbmc-language-swedish, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-swedish, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-swedish, 0, 0, -, /usr/share/xbmc/language/Swedish)
	
	@$(call install_finish, xbmc-language-swedish)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_THAI)  += xbmc-language-thai
XBMC_LANGUAGE_THAI_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_THAI_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-thai.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-thai)
	@$(call install_fixup,  xbmc-language-thai, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-thai, SECTION,     base)
	@$(call install_fixup,  xbmc-language-thai, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-thai, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-thai, 0, 0, -, /usr/share/xbmc/language/Thai)
	
	@$(call install_finish, xbmc-language-thai)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_TURKISH)  += xbmc-language-turkish
XBMC_LANGUAGE_TURKISH_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_TURKISH_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-turkish.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-turkish)
	@$(call install_fixup,  xbmc-language-turkish, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-turkish, SECTION,     base)
	@$(call install_fixup,  xbmc-language-turkish, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-turkish, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-turkish, 0, 0, -, /usr/share/xbmc/language/Turkish)
	
	@$(call install_finish, xbmc-language-turkish)
	
	@$(call touch)

# ----------------------------------------------------------------------------
PACKAGES-$(PTXCONF_XBMC_LANGUAGE_UKRAINIAN)  += xbmc-language-ukrainian
XBMC_LANGUAGE_UKRAINIAN_VERSION              := $(XBMC_VERSION)
XBMC_LANGUAGE_UKRAINIAN_PKGDIR               := $(XBMC_PKGDIR)

$(STATEDIR)/xbmc-language-ukrainian.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   xbmc-language-ukrainian)
	@$(call install_fixup,  xbmc-language-ukrainian, PRIORITY,    optional)
	@$(call install_fixup,  xbmc-language-ukrainian, SECTION,     base)
	@$(call install_fixup,  xbmc-language-ukrainian, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  xbmc-language-ukrainian, DESCRIPTION, missing)
	
	@$(call install_tree,   xbmc-language-ukrainian, 0, 0, -, /usr/share/xbmc/language/Ukrainian)
	
	@$(call install_finish, xbmc-language-ukrainian)
	
	@$(call touch)

# ----------------------------------------------------------------------------


# vim: syntax=make
