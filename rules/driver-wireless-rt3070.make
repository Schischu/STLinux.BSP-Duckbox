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
PACKAGES-$(PTXCONF_DRIVER_WIRELESS_RT3070) += driver-wireless-rt3070

#
# Paths and names and versions
#
DRIVER_WIRELESS_RT3070_VERSION	:= 2.3.0.4
DRIVER_WIRELESS_RT3070			:= rt3070
DRIVER_WIRELESS_RT3070_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/wireless/$(DRIVER_WIRELESS_RT3070)sta
DRIVER_WIRELESS_RT3070_DIR		:= $(BUILDDIR)/$(DRIVER_WIRELESS_RT3070)
DRIVER_WIRELESS_RT3070_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_WIRELESS_RT3070
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-wireless-rt3070.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt3070.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt3070.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RT3070_DIR) \
		$(DRIVER_WIRELESS_RT3070_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt3070.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_WIRELESS_RT3070_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RT3070_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_WIRELESS_RT3070_PKGDIR) \
		modules_install
	
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt3070.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-wireless-rt3070)
	@$(call install_fixup, driver-wireless-rt3070, PRIORITY,optional)
	@$(call install_fixup, driver-wireless-rt3070, SECTION,base)
	@$(call install_fixup, driver-wireless-rt3070, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-wireless-rt3070, DESCRIWIRELESSON,missing)

	@cd $(DRIVER_WIRELESS_RT3070_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-wireless-rt3070, 0, 0, 0644, $(DRIVER_WIRELESS_RT3070_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-wireless-rt3070)

	@$(call touch)


# vim: syntax=make
