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
PACKAGES-$(PTXCONF_DRIVER_PTI) += driver-pti

#
# Paths and names and versions
#
DRIVER_PTI_VERSION		:= 1.0
DRIVER_PTI		:= pti
DRIVER_PTI_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_PTI)
DRIVER_PTI_DIR		:= $(BUILDDIR)/$(DRIVER_PTI)
DRIVER_PTI_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_PTI
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-pti.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-pti.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-pti.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_PTI_DIR) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-pti.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_PTI_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_PTI_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_PTI_PKGDIR) \
		modules_install
	
	mkdir -p $(SYSROOT)/usr/include/linux
	cp $(DRIVER_PTI_DIR)/pti_public.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-pti.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-pti)
	@$(call install_fixup, driver-pti, PRIORITY,optional)
	@$(call install_fixup, driver-pti, SECTION,base)
	@$(call install_fixup, driver-pti, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-pti, DESCRIPTION,missing)

	@cd $(DRIVER_PTI_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-pti, 0, 0, 0644, $(DRIVER_PTI_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-pti)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-pti.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_PTI)

# vim: syntax=make
