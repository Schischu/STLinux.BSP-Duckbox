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
PACKAGES-$(PTXCONF_ENIGMA2_SKIN_MAGIC) += enigma2-skin-magic

#
# Paths and names
#
ENIGMA2_SKIN_MAGIC_VERSION	:= HEAD
ENIGMA2_SKIN_MAGIC		:= enigma2-skin-magic-$(ENIGMA2_SKIN_MAGIC_VERSION)
ENIGMA2_SKIN_MAGIC_URL	:= git://git.code.sf.net/p/openpli/skin-magic
ENIGMA2_SKIN_MAGIC_URLOLD := git://openpli.git.sourceforge.net/gitroot/openpli/skin-magic
ENIGMA2_SKIN_MAGIC_SOURCE_GIT	:= $(SRCDIR)/enigma2-skin-magic.git
ENIGMA2_SKIN_MAGIC_DIR	:= $(BUILDDIR)/$(ENIGMA2_SKIN_MAGIC)
ENIGMA2_SKIN_MAGIC_LICENSE	:= enigma2-skin-magic

ENIGMA2_SKIN_MAGIC_DEV_VERSION	:= $(ENIGMA2_SKIN_MAGIC_VERSION)
ENIGMA2_SKIN_MAGIC_DEV_PKGDIR	:= $(ENIGMA2_SKIN_MAGIC_PKGDIR)

$(STATEDIR)/enigma2-skin-magic.get:
	@$(call targetinfo)
	
		if [ -d $(ENIGMA2_SKIN_MAGIC_SOURCE_GIT) ]; then \
			cd $(ENIGMA2_SKIN_MAGIC_SOURCE_GIT); \
			git pull -u origin master 2>&1 > /dev/null; \
			git checkout HEAD 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone  $(ENIGMA2_SKIN_MAGIC_URL) $(ENIGMA2_SKIN_MAGIC_SOURCE_GIT) 2>&1 > /dev/null; \
		fi; 2>&1 > /dev/null
	
		if [ ! "$(ENIGMA2_SKIN_MAGIC_VERSION)" == "HEAD" ]; then \
			cd $(ENIGMA2_SKIN_MAGIC_SOURCE_GIT); \
			git checkout $(ENIGMA2_SKIN_MAGIC_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi; 2>&1 > /dev/null
	
	@$(call touch)

PATH_PATCHES = $(subst :, ,$(PTXDIST_PATH_PATCHES))

$(STATEDIR)/enigma2-skin-magic.extract:
	@$(call targetinfo)
	
	rm -rf $(ENIGMA2_SKIN_MAGIC_DIR); \
	cp -a $(ENIGMA2_SKIN_MAGIC_SOURCE_GIT) $(ENIGMA2_SKIN_MAGIC_DIR); \
	rm -rf $(ENIGMA2_SKIN_MAGIC_DIR)/.git;
	
	@$(call patchin, ENIGMA2_SKIN_MAGIC)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/enigma2-skin-magic.compile:
	@$(call touch)

$(STATEDIR)/enigma2-skin-magic.install:
	@$(call targetinfo)
	
	mkdir -p $(ENIGMA2_SKIN_MAGIC_PKGDIR)/usr
	cp -a $(ENIGMA2_SKIN_MAGIC_DIR)/usr/* $(ENIGMA2_SKIN_MAGIC_PKGDIR)/usr/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/enigma2-skin-magic.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  enigma2-skin-magic)
	@$(call install_fixup, enigma2-skin-magic, PRIORITY,    optional)
	@$(call install_fixup, enigma2-skin-magic, SECTION,     base)
	@$(call install_fixup, enigma2-skin-magic, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-skin-magic, DESCRIPTION, missing)
	
	@cd $(ENIGMA2_SKIN_MAGIC_PKGDIR) && \
		find ./usr \
		! -type d -a ! \( -name "valis_enigma.ttf" \) | \
		while read file; do \
		$(call install_copy, enigma2-skin-magic, 0, 0, 644, -, $${file##.}); \
	done
	#@$(call install_tree,  enigma2-skin-magic, 0, 0, -, /usr)
	
	@$(call install_finish, enigma2-skin-magic)
	
	@$(call touch)

# vim: syntax=make
