# -*-makefile-*-
#
# Copyright (C) 2003-2006 by Robert Schwebel <r.schwebel@pengutronix.de>
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
PACKAGES-$(PTXCONF_AIO_GRAB) += aio-grab

#
# Paths and names
#
AIO_GRAB_VERSION	:= 47983a78821c0fdbef3aab36113d5c6fc34bb11b
AIO_GRAB		:= aio-grab-$(AIO_GRAB_VERSION)
AIO_GRAB_URL		:= git://openpli.git.sourceforge.net/gitroot/openpli/aio-grab
AIO_GRAB_GIT_BRANCH     := master
AIO_GRAB_GIT_HEAD	:= $(AIO_GRAB_VERSION)
AIO_GRAB_SOURCE		:= $(SRCDIR)/aio-grab.git
AIO_GRAB_DIR		:= $(BUILDDIR)/$(AIO_GRAB)
AIO_GRAB_LICENSE	:= GPLv2


$(STATEDIR)/aio-grab.extract.post:
	@$(call targetinfo)
	
	@$(call world/patchin/post, AIO_GRAB)
	
	cd $(AIO_GRAB_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal; autoheader; automake -a; autoconf; 
	
	@$(call touch)

#
# autoconf
#
AIO_GRAB_AUTOCONF := $(CROSS_AUTOCONF_USR)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/aio-grab.targetinstall:
	@$(call targetinfo)

	@$(call install_init, aio-grab)
	@$(call install_fixup, aio-grab,PRIORITY,optional)
	@$(call install_fixup, aio-grab,SECTION,base)
	@$(call install_fixup, aio-grab,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, aio-grab,DESCRIPTION,missing)

	@$(call install_copy, aio-grab, 0, 0, 755, -, /usr/bin/grab)

	@$(call install_finish, aio-grab)

	@$(call touch)

# vim: syntax=make
