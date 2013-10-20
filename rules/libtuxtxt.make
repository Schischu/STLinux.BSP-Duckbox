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
PACKAGES-$(PTXCONF_LIBTUXTXT) += libtuxtxt

#
# Paths and names
#
LIBTUXTXT_VERSION		:= 4ff8fffd72115130ff6594841e7bad2f85e85f12
LIBTUXTXT				:= libtuxtxt-$(LIBTUXTXT_VERSION)
LIBTUXTXT_URL			:= git://openpli.git.sourceforge.net/gitroot/openpli/tuxtxt
LIBTUXTXT_GIT_BRANCH	:= master
LIBTUXTXT_GIT_HEAD		:= $(LIBTUXTXT_VERSION)
LIBTUXTXT_SOURCE		:= $(SRCDIR)/tuxtxt.git
LIBTUXTXT_DIR			:= $(BUILDDIR)/$(LIBTUXTXT)/libtuxtxt
LIBTUXTXT_LICENSE		:= libtuxtxt

$(STATEDIR)/libtuxtxt.prepare: $(STATEDIR)/driver-avs.install \
                               $(STATEDIR)/driver-stmfb.install

$(STATEDIR)/libtuxtxt.extract.post:
	@$(call targetinfo)
	
	cd $(LIBTUXTXT_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; autoheader; automake -a; autoconf; 
	
	@$(call world/patchin/post, LIBTUXTXT)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBTUXTXT_PATH	:= PATH=$(CROSS_PATH)
LIBTUXTXT_ENV 	:= $(CROSS_ENV)

LIBTUXTXT_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--with-boxtype=generic \
	--with-configdir=/usr \
	--with-datadir=/usr/share/tuxtxt \
	--with-fontdir=/usr/share/fonts

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libtuxtxt.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libtuxtxt)
	@$(call install_fixup, libtuxtxt,PRIORITY,optional)
	@$(call install_fixup, libtuxtxt,SECTION,base)
	@$(call install_fixup, libtuxtxt,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libtuxtxt,DESCRIPTION,missing)

	@$(call install_lib, libtuxtxt, 0, 0, 0644, libtuxtxt)

	@$(call install_finish, libtuxtxt)

	@$(call touch)

# vim: syntax=make
