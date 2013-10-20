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
PACKAGES-$(PTXCONF_LIBNFS) += libnfs

#
# Paths and names
#
LIBNFS_VERSION	:= c0ebf57b212ffefe83e2a50358499f68e7289e93
LIBNFS		:= libnfs-$(LIBNFS_VERSION)
LIBNFS_URL	:= git://github.com/sahlberg/libnfs.git
LIBNFS_GIT_BRANCH := master
LIBNFS_GIT_HEAD	:= $(LIBNFS_VERSION)
LIBNFS_SOURCE	:= $(SRCDIR)/libnfs.git
LIBNFS_DIR	:= $(BUILDDIR)/$(LIBNFS)
LIBNFS_LICENSE	:= libnfs

#
# autoconf
#
LIBNFS_CONF_TOOL	:= autoconf


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libnfs.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   libnfs)
	@$(call install_fixup,  libnfs, PRIORITY,    optional)
	@$(call install_fixup,  libnfs, SECTION,     base)
	@$(call install_fixup,  libnfs, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  libnfs, DESCRIPTION, missing)
	
	@$(call install_lib,    libnfs, 0, 0, 0644, libnfs)
	
	@$(call install_finish, libnfs)
	
	@$(call touch)

# vim: syntax=make
