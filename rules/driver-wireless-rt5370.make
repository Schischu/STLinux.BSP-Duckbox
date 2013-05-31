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
PACKAGES-$(PTXCONF_DRIVER_WIRELESS_RT5370) += driver-wireless-rt5370

#
# Paths and names and versions
#
DRIVER_WIRELESS_RT5370_VERSION	:= 2.5.0.3
DRIVER_WIRELESS_RT5370			:= rt5370
DRIVER_WIRELESS_RT5370_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/wireless/$(DRIVER_WIRELESS_RT5370)sta
DRIVER_WIRELESS_RT5370_DIR		:= $(BUILDDIR)/$(DRIVER_WIRELESS_RT5370)
DRIVER_WIRELESS_RT5370_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_WIRELESS_RT5370
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-wireless-rt5370.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt5370.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt5370.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RT5370_DIR) \
		$(DRIVER_WIRELESS_RT5370_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt5370.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_WIRELESS_RT5370_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_RT5370_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_WIRELESS_RT5370_PKGDIR) \
		modules_install
	
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless-rt5370.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-wireless-rt5370)
	@$(call install_fixup, driver-wireless-rt5370, PRIORITY,optional)
	@$(call install_fixup, driver-wireless-rt5370, SECTION,base)
	@$(call install_fixup, driver-wireless-rt5370, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-wireless-rt5370, DESCRIPTION,missing)

	@cd $(DRIVER_WIRELESS_RT5370_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-wireless-rt5370, 0, 0, 0644, $(DRIVER_WIRELESS_RT5370_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-wireless-rt5370)

	@$(call touch)


# vim: syntax=make
