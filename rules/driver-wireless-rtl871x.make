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
PACKAGES-$(PTXCONF_DRIVER_WIRELESS_RTL871X) += driver-wireless-rtl871x

#
# Paths and names and versions
#
DRIVER_WIRELESS_RTL871X_VERSION	:= 2.6.6.0.20101111
DRIVER_WIRELESS_RTL871X			:= rtl871x
DRIVER_WIRELESS_RTL871X_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/wireless/$(DRIVER_WIRELESS_RTL871X)
DRIVER_WIRELESS_RTL871X_DIR		:= $(BUILDDIR)/$(DRIVER_WIRELESS_RTL871X)
DRIVER_WIRELESS_RTL871X_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_WIRELESS_RTL871X
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-wireless-rtl871x.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rtl871x.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rtl871x.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RTL871X_DIR) \
		$(DRIVER_WIRELESS_RTL871X_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rtl871x.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_WIRELESS_RTL871X_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RTL871X_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_WIRELESS_RTL871X_PKGDIR) \
		modules_install
	
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rtl871x.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-wireless-rtl871x)
	@$(call install_fixup, driver-wireless-rtl871x, PRIORITY,optional)
	@$(call install_fixup, driver-wireless-rtl871x, SECTION,base)
	@$(call install_fixup, driver-wireless-rtl871x, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-wireless-rtl871x, DESCRIPTION,missing)

	@cd $(DRIVER_WIRELESS_RTL871X_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-wireless-rtl871x, 0, 0, 0644, $(DRIVER_WIRELESS_RTL871X_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-wireless-rtl871x)

	@$(call touch)


# vim: syntax=make
