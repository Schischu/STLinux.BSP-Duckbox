# -*-makefile-*-
#
# Copyright (C) 2012 by fga
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBASS) += libass

#
# Paths and names
#
LIBASS_VERSION	:= 0.10.1
LIBASS_MD5	:= 6cace482a013a3c4bf3b31a68ac66026
LIBASS		:= libass-$(LIBASS_VERSION)
LIBASS_SUFFIX	:= tar.gz
LIBASS_URL	:= http://libass.googlecode.com/files/$(LIBASS).$(LIBASS_SUFFIX)
LIBASS_SOURCE	:= $(SRCDIR)/$(LIBASS).$(LIBASS_SUFFIX)
LIBASS_DIR	:= $(BUILDDIR)/$(LIBASS)
LIBASS_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(LIBASS_SOURCE):
#	@$(call targetinfo)
#	@$(call get, LIBASS)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBASS_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
LIBASS_CONF_TOOL	:= autoconf
#LIBASS_CONF_OPT	:= $(CROSS_AUTOCONF_USR)

#$(STATEDIR)/libass.prepare:
#	@$(call targetinfo)
#	@$(call clean, $(LIBASS_DIR)/config.cache)
#	cd $(LIBASS_DIR) && \
#		$(LIBASS_PATH) $(LIBASS_ENV) \
#		./configure $(LIBASS_CONF_OPT)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libass.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBASS)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/libass.install:
#	@$(call targetinfo)
#	@$(call world/install, LIBASS)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libass.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libass)
	@$(call install_fixup, libass,PRIORITY,optional)
	@$(call install_fixup, libass,SECTION,base)
	@$(call install_fixup, libass,AUTHOR,"fga")
	@$(call install_fixup, libass,DESCRIPTION,missing)

	@$(call install_lib, libass, 0, 0, 0644, libass)

	@$(call install_finish, libass)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/libass.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIBASS)

# vim: syntax=make
