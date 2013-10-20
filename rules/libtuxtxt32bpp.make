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
LIBTUXTXT32BPP_GIT_BRANCH	:= master
LIBTUXTXT32BPP_GIT_HEAD	:= $(LIBTUXTXT32BPP_VERSION)
LIBTUXTXT32BPP_SOURCE	:= $(SRCDIR)/tuxtxt.git
LIBTUXTXT32BPP_DIR	:= $(BUILDDIR)/$(LIBTUXTXT32BPP)/tuxtxt
LIBTUXTXT32BPP_SUBDIR	:= tuxtxt
LIBTUXTXT32BPP_LICENSE	:= libtuxtxt32bpp

$(STATEDIR)/libtuxtxt32bpp.extract.post:
	@$(call targetinfo)
	
	cd $(LIBTUXTXT32BPP_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; autoheader; automake -a; autoconf; 
	
	@$(call world/patchin/post, LIBTUXTXT32BPP)
	
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
	--with-datadir=/usr/share/tuxtxt \
	--with-fontdir=/usr/share/fonts

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
	
	@$(call install_tree, libtuxtxt32bpp, 0, 0, -, /usr/share/)
	@$(call install_tree, libtuxtxt32bpp, 0, 0, -, /etc/tuxtxt/)
	@$(call install_tree, libtuxtxt32bpp, 0, 0, -, /usr/lib/enigma2/python/Plugins/Extensions/Tuxtxt/)
	
	@$(call install_finish, libtuxtxt32bpp)
	
	@$(call touch)

# vim: syntax=make
