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
PACKAGES-$(PTXCONF_DRIVER_WIRELESS_RT2870) += driver-wireless-rt2870

#
# Paths and names and versions
#
DRIVER_WIRELESS_RT2870_VERSION	:= 2.4.0.1
DRIVER_WIRELESS_RT2870			:= rt2870
DRIVER_WIRELESS_RT2870_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/wireless/$(DRIVER_WIRELESS_RT2870)sta
DRIVER_WIRELESS_RT2870_DIR		:= $(BUILDDIR)/$(DRIVER_WIRELESS_RT2870)
DRIVER_WIRELESS_RT2870_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_WIRELESS_RT2870
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-wireless-rt2870.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt2870.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt2870.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RT2870_DIR) \
		$(DRIVER_WIRELESS_RT2870_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt2870.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_WIRELESS_RT2870_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RT2870_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_WIRELESS_RT2870_PKGDIR) \
		modules_install
	
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt2870.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-wireless-rt2870)
	@$(call install_fixup, driver-wireless-rt2870, PRIORITY,optional)
	@$(call install_fixup, driver-wireless-rt2870, SECTION,base)
	@$(call install_fixup, driver-wireless-rt2870, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-wireless-rt2870, DESCRIWIRELESSON,missing)

	@cd $(DRIVER_WIRELESS_RT2870_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-wireless-rt2870, 0, 0, 0644, $(DRIVER_WIRELESS_RT2870_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-wireless-rt2870)

	@$(call touch)


# vim: syntax=make
