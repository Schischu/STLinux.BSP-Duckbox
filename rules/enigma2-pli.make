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
PACKAGES-$(PTXCONF_ENIGMA2_PLI) += enigma2-pli

#
# Paths and names
#
ENIGMA2_PLI_VERSION	:= 839e96b79600aba73f743fd39628f32bc1628f4c
ENIGMA2_PLI		:= enigma2-pli-$(ENIGMA2_PLI_VERSION)
ENIGMA2_PLI_URL	:= git://openpli.git.sourceforge.net/gitroot/openpli/enigma2
ENIGMA2_PLI_SOURCE_GIT	:= $(SRCDIR)/enigma2-pli.git
ENIGMA2_PLI_DIR	:= $(BUILDDIR)/$(ENIGMA2_PLI)
ENIGMA2_PLI_LICENSE	:= enigma2-pli

$(STATEDIR)/enigma2-pli.get:
	@$(call targetinfo)
	
	@$(call shell, ( \
		if [ -d $(ENIGMA2_PLI_SOURCE_GIT) ]; then \
			cd $(ENIGMA2_PLI_SOURCE_GIT); \
			git pull -u origin master 2>&1 > /dev/null; \
			git checkout HEAD 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone -b HEAD $(ENIGMA2_PLI_URL) $(ENIGMA2_PLI_SOURCE_GIT) 2>&1 > /dev/null; \
		fi;) 2>&1 > /dev/null)
	
	@$(call shell, ( \
		if [ ! "$(ENIGMA2_PLI_VERSION)" == "HEAD" ]; then \
			cd $(ENIGMA2_PLI_SOURCE_GIT); \
			git checkout $(ENIGMA2_PLI_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi;) 2>&1 > /dev/null)
	
	@$(call touch)


$(STATEDIR)/enigma2-pli.extract:
	@$(call targetinfo)
	
	@$(call shell, rm -rf $(ENIGMA2_PLI_DIR);)
	@$(call shell, cp -a $(ENIGMA2_PLI_SOURCE_GIT) $(ENIGMA2_PLI_DIR);)
	@$(call shell, rm -rf $(ENIGMA2_PLI_DIR)/.git;)
	
	@$(call patchin, ENIGMA2_PLI)
	
	#sed -e 's|#!/usr/bin/python|#!$(PTXDIST_SYSROOT_CROSS)/bin/python|' -i $(ENIGMA2_PLI_DIR)/po/xml2po.py
	cd $(ENIGMA2_PLI_DIR) && sh autogen.sh
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ENIGMA2_PLI_PATH	:= PATH=$(CROSS_PATH)
ENIGMA2_PLI_ENV 	:= $(CROSS_ENV)

ENIGMA2_PLI_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
		--prefix=/usr/local \
		--without-libsdl \
		PY_PATH=$(SYSROOT)/usr \
		PKG_CONFIG=$(PTXDIST_SYSROOT_HOST)/bin/pkg-config \
		PKG_CONFIG_PATH=$(SYSROOT)/usr/lib/pkgconfig

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/enigma2-pli.targetinstall:
	@$(call targetinfo)

	@$(call install_init, enigma2-pli)
	@$(call install_fixup, enigma2-pli,PRIORITY,optional)
	@$(call install_fixup, enigma2-pli,SECTION,base)
	@$(call install_fixup, enigma2-pli,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-pli,DESCRIPTION,missing)

	@$(call install_tree, enigma2-pli, 0, 0, -, /usr/local/bin/)

ifdef PTXCONF_ENIGMA2_PLI_INSTALL_PY
	@cd $(ENIGMA2_PLI_PKGDIR) && \
		find ./usr/lib/enigma2 -name "*.py" | \
		while read file; do \
		$(call install_copy, enigma2-pli, 0, 0, 644, -, $${file##.}); \
	done
endif

	@cd $(ENIGMA2_PLI_PKGDIR) && \
		find ./usr/lib/enigma2 \
		! -type d -a ! \( -name "*.py" -o -name "*.pyc" \) | \
		while read file; do \
		$(call install_copy, enigma2-pli, 0, 0, 644, -, $${file##.}); \
	done

	@$(call install_tree, enigma2-pli, 0, 0, -, /usr/local/share/)

	@$(call install_finish, enigma2-pli)

	@$(call touch)

# vim: syntax=make
