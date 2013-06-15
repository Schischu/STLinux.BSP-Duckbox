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
PACKAGES-$(PTXCONF_RTMPDUMP) += rtmpdump

#
# Paths and names
#
RTMPDUMP_VERSION	:= 2.4
RTMPDUMP_MD5		:= 8241345cf6476c1c1b9689494d6e820f
RTMPDUMP		:= rtmpdump-$(RTMPDUMP_VERSION)
RTMPDUMP_SUFFIX		:= tar.gz
RTMPDUMP_URL		:= http://rtmpdump.mplayerhq.hu/download/$(RTMPDUMP).$(RTMPDUMP_SUFFIX)
RTMPDUMP_SOURCE		:= $(SRCDIR)/$(RTMPDUMP).$(RTMPDUMP_SUFFIX)
RTMPDUMP_DIR		:= $(BUILDDIR)/$(RTMPDUMP)
RTMPDUMP_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(RTMPDUMP_SOURCE):
#	@$(call targetinfo)
#	@$(call get, RTMPDUMP)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
RTMPDUMP_ENV	:= $(CROSS_ENV)
RTMPDUMP_PATH	:= PATH=$(CROSS_PATH)

$(STATEDIR)/rtmpdump.prepare:
	@$(call targetinfo)
#	@$(call clean, $(RTMPDUMP_DIR)/config.cache)
#	cd $(RTMPDUMP_DIR) && \
#		$(RTMPDUMP_PATH) $(RTMPDUMP_ENV) \
#		./configure $(RTMPDUMP_CONF_OPT)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/rtmpdump.compile:
	@$(call targetinfo)
	cd $(RTMPDUMP_DIR) && \
		$(RTMPDUMP_PATH) $(RTMPDUMP_ENV) PREFIX=/usr \
		$(MAKE) CROSS_COMPILE=$(COMPILER_PREFIX)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/rtmpdump.install:
#	@$(call targetinfo)
#	@$(call world/install, RTMPDUMP)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/rtmpdump.targetinstall:
	@$(call targetinfo)

	@$(call install_init, rtmpdump)
	@$(call install_fixup, rtmpdump,PRIORITY,optional)
	@$(call install_fixup, rtmpdump,SECTION,base)
	@$(call install_fixup, rtmpdump,AUTHOR,"fga")
	@$(call install_fixup, rtmpdump,DESCRIPTION,missing)

	@$(call install_copy, rtmpdump, 0, 0, 0755, -, /bin/rtmpdump)
	@$(call install_copy, rtmpdump, 0, 0, 0755, -, /sbin/rtmpgw)
	@$(call install_copy, rtmpdump, 0, 0, 0755, -, /sbin/rtmpsrv)
	@$(call install_copy, rtmpdump, 0, 0, 0755, -, /sbin/rtmpsuck)

	@$(call install_lib, rtmpdump, 0, 0, 0644, librtmp)

	@$(call install_finish, rtmpdump)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/rtmpdump.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, RTMPDUMP)

# vim: syntax=make
