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
PACKAGES-$(PTXCONF_DRIVER_WIRELESS) += driver-wireless

#
# Paths and names and versions
#
DRIVER_WIRELESS_VERSION		:= 1.0
DRIVER_WIRELESS		:= wireless
DRIVER_WIRELESS_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/wireless
DRIVER_WIRELESS_DIR		:= $(BUILDDIR)/$(DRIVER_WIRELESS)
DRIVER_WIRELESS_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_WIRELESS
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-wireless.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_DIR) \
		$(DRIVER_WIRELESS_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_WIRELESS_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_WIRELESS_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_WIRELESS_PKGDIR) \
		modules_install
	
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-wireless.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-wireless)
	@$(call install_fixup, driver-wireless, PRIORITY,optional)
	@$(call install_fixup, driver-wireless, SECTION,base)
	@$(call install_fixup, driver-wireless, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-wireless, DESCRIWIRELESSON,missing)

	@cd $(DRIVER_WIRELESS_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-wireless, 0, 0, 0644, $(DRIVER_WIRELESS_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-wireless)

	@$(call touch)


# vim: syntax=make
