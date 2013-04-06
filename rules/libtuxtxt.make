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
LIBTUXTXT_VERSION	:= 4ff8fffd72115130ff6594841e7bad2f85e85f12
LIBTUXTXT		:= libtuxtxt-$(LIBTUXTXT_VERSION)
LIBTUXTXT_URL	:= git://openpli.git.sourceforge.net/gitroot/openpli/tuxtxt
LIBTUXTXT_SOURCE_GIT	:= $(SRCDIR)/tuxtxt.git
LIBTUXTXT_DIR	:= $(BUILDDIR)/$(LIBTUXTXT)/libtuxtxt
LIBTUXTXT_LICENSE	:= libtuxtxt

$(STATEDIR)/libtuxtxt.get:
	@$(call targetinfo)
	
	@$(call shell, ( \
		if [ -d $(LIBTUXTXT_SOURCE_GIT) ]; then \
			cd $(LIBTUXTXT_SOURCE_GIT); \
			git pull -u origin master 2>&1 > /dev/null; \
			git checkout HEAD 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone -b HEAD $(LIBTUXTXT_URL) $(LIBTUXTXT_SOURCE_GIT) 2>&1 > /dev/null; \
		fi;) 2>&1 > /dev/null)
	
	@$(call shell, ( \
		if [ ! "$(LIBTUXTXT_VERSION)" == "HEAD" ]; then \
			cd $(LIBTUXTXT_SOURCE_GIT); \
			git checkout $(LIBTUXTXT_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi;) 2>&1 > /dev/null)
	
	@$(call touch)


$(STATEDIR)/libtuxtxt.extract:
	@$(call targetinfo)
	
	@$(call shell, rm -rf $(BUILDDIR)/$(LIBTUXTXT);)
	@$(call shell, cp -a $(LIBTUXTXT_SOURCE_GIT) $(BUILDDIR)/$(LIBTUXTXT);)
	@$(call shell, rm -rf $(BUILDDIR)/$(LIBTUXTXT)/.git;)
	
	@$(call patchin, LIBTUXTXT)
	
	cd $(LIBTUXTXT_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; autoheader; automake -a; autoconf; 
	
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
