# -*-makefile-*-
#
# Copyright (C) @YEAR@ by @AUTHOR@
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DRIVER_PLAYER2) += driver-player2

#
# Paths and names and versions
#
DRIVER_PLAYER2_VERSION	:= 191
DRIVER_PLAYER2		:= player2_$(DRIVER_PLAYER2_VERSION)
DRIVER_PLAYER2_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_PLAYER2)
DRIVER_PLAYER2_DIR		:= $(BUILDDIR)/$(DRIVER_PLAYER2)
DRIVER_PLAYER2_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_PLAYER2
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-player2.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_PLAYER2_DIR)/linux \
		KERNEL_LOCATION=$(KERNEL_DIR) \
		TREE_ROOT=$(DRIVER_PLAYER2_DIR) \
		CONFIG_MODULES_PATH=$(SYSROOT)/usr/include \
		CONFIG_KERNEL_PATH="$(PTXDIST_SYSROOT_HOST)/usr" \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_PLAYER2_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_PLAYER2_DIR)/linux \
		INSTALL_MOD_PATH=$(DRIVER_PLAYER2_PKGDIR) \
		modules_install
	
	mkdir -p $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/linux/include/linux/dvb/stm_ioctls.h $(SYSROOT)/usr/include/linux/dvb
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-player2)
	@$(call install_fixup, driver-player2, PRIORITY,optional)
	@$(call install_fixup, driver-player2, SECTION,base)
	@$(call install_fixup, driver-player2, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-player2, DESCRIPTION,missing)

	@cd $(DRIVER_PLAYER2_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-player2, 0, 0, 0644, $(DRIVER_PLAYER2_PKGDIR)/$${file}, /lib/modules/$(call remove_quotes, $(PTXCONF_KERNEL_VERSION)).$(call remove_quotes, $(PTXCONF_KERNEL_LOCALVERSION))/extra/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-player2)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-player2.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_PLAYER2)

# vim: syntax=make
