# -*-makefile-*-
#
# Copyright (C) 2010 by Erwin Rol <erwin@erwinrol.com>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBEPLAYER3) += libeplayer3

#
# Paths and names
#
LIBEPLAYER3_VERSION := 1.0
LIBEPLAYER3	 := libeplayer3
LIBEPLAYER3_URL	 := lndir://$(PTXDIST_WORKSPACE)/local_src/libs/$(LIBEPLAYER3)
LIBEPLAYER3_DIR	 := $(BUILDDIR)/$(LIBEPLAYER3)

$(STATEDIR)/libmmeimage.prepare: $(STATEDIR)/driver-player2.install

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBEPLAYER3_CONF_TOOL	:= autoconf
LIBEPLAYER3_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	 --prefix=/

$(STATEDIR)/libeplayer3.prepare: $(STATEDIR)/driver-player2.install
	@$(call targetinfo)
	cd $(LIBEPLAYER3_DIR); \
		cp $(PTXDIST_SYSROOT_HOST)/share/libtool/config/ltmain.sh .; \
		touch NEWS README AUTHORS ChangeLog; \
		aclocal -Im4; automake -a; autoconf
	@$(call world/prepare, LIBEPLAYER3)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libeplayer3.compile:
	@$(call targetinfo)
	
	cd $(LIBEPLAYER3_DIR) && \
		$(MAKE) $(CROSS_ENV_CC)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libeplayer3.install:
	@$(call targetinfo)
	@$(call world/install, LIBEPLAYER3)
	
	mkdir -p $(SYSROOT)/usr/include/libeplayer
	cp $(LIBEPLAYER3_DIR)/include/*.h $(SYSROOT)/usr/include/libeplayer
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libeplayer3.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libeplayer3)
	@$(call install_fixup, libeplayer3,PRIORITY,optional)
	@$(call install_fixup, libeplayer3,SECTION,base)
	@$(call install_fixup, libeplayer3,AUTHOR,"Erwin Rol <erwin@erwinrol.com>")
	@$(call install_fixup, libeplayer3,DESCRIPTION,missing)

	$(call install_copy, libeplayer3, 0, 0, 0644, -, /usr/lib/libeplayer3.so)
	$(call install_copy, libeplayer3, 0, 0, 0755, -, /bin/eplayer3)
	$(call install_copy, libeplayer3, 0, 0, 0755, -, /bin/meta)
	@$(call install_finish, libeplayer3)

	@$(call touch)

# vim: syntax=make
