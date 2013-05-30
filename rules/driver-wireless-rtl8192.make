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
PACKAGES-$(PTXCONF_DRIVER_WIRELESS_RTL8192) += driver-wireless-rtl8192

#
# Paths and names and versions
#
DRIVER_WIRELESS_RTL8192_VERSION	:= 2.0.939.20100726
DRIVER_WIRELESS_RTL8192			:= rtl8192
DRIVER_WIRELESS_RTL8192_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/wireless/$(DRIVER_WIRELESS_RTL8192)cu
DRIVER_WIRELESS_RTL8192_DIR		:= $(BUILDDIR)/$(DRIVER_WIRELESS_RTL8192)
DRIVER_WIRELESS_RTL8192_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_WIRELESS_RTL8192
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-wireless-rtl8192.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rtl8192.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rtl8192.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RTL8192_DIR) \
		$(DRIVER_WIRELESS_RTL8192_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rtl8192.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_WIRELESS_RTL8192_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RTL8192_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_WIRELESS_RTL8192_PKGDIR) \
		modules_install
	
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rtl8192.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-wireless-rtl8192)
	@$(call install_fixup, driver-wireless-rtl8192, PRIORITY,optional)
	@$(call install_fixup, driver-wireless-rtl8192, SECTION,base)
	@$(call install_fixup, driver-wireless-rtl8192, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-wireless-rtl8192, DESCRIPTION,missing)

	@cd $(DRIVER_WIRELESS_RTL8192_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-wireless-rtl8192, 0, 0, 0644, $(DRIVER_WIRELESS_RTL8192_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-wireless-rtl8192)

	@$(call touch)


# vim: syntax=make
