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
PACKAGES-$(PTXCONF_LIBDREAMDVD) += libdreamdvd

#
# Paths and names
#
#ee7b0b4c1b35264ed7e003782b501040b7b435fd
#http://schwerkraft.elitedvb.net/anonscm/git/libdreamdvd/libdreamdvd.git
LIBDREAMDVD_VERSION	:= 6aa22dd3f530ca4be49946e07e4a0bfe60427bdf
LIBDREAMDVD		:= libdreamdvd-$(LIBDREAMDVD_VERSION)
LIBDREAMDVD_URL	:= git://github.com/mirakels/libdreamdvd.git
LIBDREAMDVD_SOURCE_GIT	:= $(SRCDIR)/libdreamdvd.git
LIBDREAMDVD_DIR	:= $(BUILDDIR)/$(LIBDREAMDVD)
LIBDREAMDVD_LICENSE	:= GPLv2

$(STATEDIR)/libdreamdvd.get:
	@$(call targetinfo)
	
	( \
		if [ -d $(LIBDREAMDVD_SOURCE_GIT) ]; then \
			cd $(LIBDREAMDVD_SOURCE_GIT); \
			git pull -u origin master 2>&1 > /dev/null; \
			git checkout HEAD 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone $(LIBDREAMDVD_URL) $(LIBDREAMDVD_SOURCE_GIT) 2>&1 > /dev/null; \
		fi; \
	) 2>&1 > /dev/null
	
	( \
		if [ ! "$(LIBDREAMDVD_VERSION)" == "HEAD" ]; then \
			cd $(LIBDREAMDVD_SOURCE_GIT); \
			git checkout $(LIBDREAMDVD_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi; \
	) 2>&1 > /dev/null
	
	@$(call touch)


$(STATEDIR)/libdreamdvd.extract:
	@$(call targetinfo)
	
	rm -rf $(BUILDDIR)/$(LIBDREAMDVD); \
	cp -a $(LIBDREAMDVD_SOURCE_GIT) $(BUILDDIR)/$(LIBDREAMDVD); \
	rm -rf $(BUILDDIR)/$(LIBDREAMDVD)/.git;
	
	@$(call patchin, LIBDREAMDVD)
	
	cd $(LIBDREAMDVD_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; autoheader; automake -a; autoconf; 
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBDREAMDVD_PATH	:= PATH=$(CROSS_PATH)
LIBDREAMDVD_ENV 	:= $(CROSS_ENV)

LIBDREAMDVD_CONF_TOOL	:= autoconf
LIBDREAMDVD_AUTOCONF := \
	$(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libdreamdvd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libdreamdvd)
	@$(call install_fixup, libdreamdvd,PRIORITY,optional)
	@$(call install_fixup, libdreamdvd,SECTION,base)
	@$(call install_fixup, libdreamdvd,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libdreamdvd,DESCRIPTION,missing)

	@$(call install_lib, libdreamdvd, 0, 0, 0644, libdreamdvd)

	@$(call install_finish, libdreamdvd)

	@$(call touch)

# vim: syntax=make
