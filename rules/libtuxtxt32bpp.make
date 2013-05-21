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
PACKAGES-$(PTXCONF_LIBTUXTXT32BPP) += libtuxtxt32bpp

#
# Paths and names
#
LIBTUXTXT32BPP_VERSION	:= 4ff8fffd72115130ff6594841e7bad2f85e85f12
LIBTUXTXT32BPP		:= libtuxtxt32bpp-$(LIBTUXTXT32BPP_VERSION)
LIBTUXTXT32BPP_URL	:= git://openpli.git.sourceforge.net/gitroot/openpli/tuxtxt
LIBTUXTXT32BPP_SOURCE_GIT	:= $(SRCDIR)/tuxtxt.git
LIBTUXTXT32BPP_DIR	:= $(BUILDDIR)/$(LIBTUXTXT32BPP)/tuxtxt
LIBTUXTXT32BPP_LICENSE	:= libtuxtxt32bpp

$(STATEDIR)/libtuxtxt32bpp.get:
	@$(call targetinfo)
	
		if [ -d $(LIBTUXTXT32BPP_SOURCE_GIT) ]; then \
			cd $(LIBTUXTXT32BPP_SOURCE_GIT); \
			git pull -u origin master 2>&1 > /dev/null; \
			git checkout $(LIBTUXTXT32BPP_VERSION) 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone $(LIBTUXTXT32BPP_URL) $(LIBTUXTXT32BPP_SOURCE_GIT) 2>&1 > /dev/null; \
		fi; 2>&1 > /dev/null
	
		if [ ! "$(LIBTUXTXT32BPP_VERSION)" == "HEAD" ]; then \
			cd $(LIBTUXTXT32BPP_SOURCE_GIT); \
			git checkout $(LIBTUXTXT32BPP_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi; 2>&1 > /dev/null
	
	@$(call touch)


$(STATEDIR)/libtuxtxt32bpp.extract:
	@$(call targetinfo)
	
	rm -rf $(BUILDDIR)/$(LIBTUXTXT32BPP); \
	cp -a $(LIBTUXTXT32BPP_SOURCE_GIT) $(BUILDDIR)/$(LIBTUXTXT32BPP); \
	rm -rf $(BUILDDIR)/$(LIBTUXTXT32BPP)/.git;
	
	@$(call patchin, LIBTUXTXT32BPP)
	
	cd $(LIBTUXTXT32BPP_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; autoheader; automake -a; autoconf; 
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBTUXTXT32BPP_PATH	:= PATH=$(CROSS_PATH)
LIBTUXTXT32BPP_ENV 	:= $(CROSS_ENV)

LIBTUXTXT32BPP_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--with-boxtype=generic \
	--with-configdir=/usr \
	--with-datadir=/usr/local/share/tuxtxt \
	--with-fontdir=/usr/local/share/fonts

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libtuxtxt32bpp.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, libtuxtxt32bpp)
	@$(call install_fixup, libtuxtxt32bpp,PRIORITY,optional)
	@$(call install_fixup, libtuxtxt32bpp,SECTION,base)
	@$(call install_fixup, libtuxtxt32bpp,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libtuxtxt32bpp,DESCRIPTION,missing)
	
	@$(call install_lib, libtuxtxt32bpp, 0, 0, 0644, libtuxtxt32bpp)
	
	@$(call install_tree, libtuxtxt32bpp, 0, 0, -, /usr/local/share/)
	
	@$(call install_finish, libtuxtxt32bpp)
	
	@$(call touch)

# vim: syntax=make
