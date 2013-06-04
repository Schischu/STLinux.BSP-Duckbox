# -*-makefile-*-
#
# Copyright (C) 2012 by fabricega
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NTFS_3G) += ntfs-3g

#
# Paths and names
#
NTFS_3G_VERSION	:= 2012.1.15
NTFS_3G_MD5	:= 341acae00a290cab9b00464db65015cc
NTFS_3G		:= ntfs-3g_ntfsprogs-$(NTFS_3G_VERSION)
NTFS_3G_SUFFIX	:= tgz
NTFS_3G_URL	:= http://tuxera.com/opensource/$(NTFS_3G).$(NTFS_3G_SUFFIX)
NTFS_3G_SOURCE	:= $(SRCDIR)/$(NTFS_3G).$(NTFS_3G_SUFFIX)
NTFS_3G_DIR	:= $(BUILDDIR)/$(NTFS_3G)
NTFS_3G_LICENSE	:= GPLv2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
NTFS_3G_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-shared \
	--disable-static \
	--disable-dependency-tracking \
	--disable-library \
	--enable-posix-acls \
	--disable-mtab \
	--enable-ntfsprogs \
	--enable-crypto \
	--with-fuse=external \
	--with-uuid

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ntfs-3g.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ntfs-3g)
	@$(call install_fixup, ntfs-3g,PRIORITY,optional)
	@$(call install_fixup, ntfs-3g,SECTION,base)
	@$(call install_fixup, ntfs-3g,AUTHOR,"fabricega")
	@$(call install_fixup, ntfs-3g,DESCRIPTION,missing)

	@$(call install_copy, ntfs-3g, 0, 0, 0755, -, /bin/ntfs-3g)
	@$(call install_link, ntfs-3g, /bin/ntfs-3g, /sbin/mount.ntfs)
	@$(call install_link, ntfs-3g, /bin/ntfs-3g, /sbin/mount.ntfs-3g)

	@$(call install_finish, ntfs-3g)

	@$(call touch)

# vim: syntax=make
