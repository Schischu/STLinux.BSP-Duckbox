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
PACKAGES-$(PTXCONF_DRIVER_E2PROC) += driver-e2proc

#
# Paths and names and versions
#
DRIVER_E2PROC_VERSION		:= 1.0
DRIVER_E2PROC		:= e2proc
DRIVER_E2PROC_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/e2_proc
DRIVER_E2PROC_DIR		:= $(BUILDDIR)/$(DRIVER_E2PROC)
DRIVER_E2PROC_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_E2PROC
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-e2proc.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_PLATFORM_ADB
DRIVER_CIC_UFS9XX_EXTRAS := EXTRA_CFLAGS=-DADB
endif
ifdef PTXCONF_PLATFORM_IPBOX9900
DRIVER_CIC_UFS9XX_EXTRAS := EXTRA_CFLAGS=-DIPBOX9900
endif


$(STATEDIR)/driver-e2proc.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-e2proc.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_E2PROC_DIR) \
		$(DRIVER_E2PROC_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-e2proc.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_E2PROC_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_E2PROC_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_E2PROC_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_E2PROC_DIR)/e2proc.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-e2proc.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-e2proc)
	@$(call install_fixup, driver-e2proc, PRIORITY,oe2proconal)
	@$(call install_fixup, driver-e2proc, SECTION,base)
	@$(call install_fixup, driver-e2proc, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-e2proc, DESCRIE2PROCON,missing)

	@cd $(DRIVER_E2PROC_PKGDIR) && \
		find lib -type f -name *.ko | while read file; do \
			$(call install_copy, driver-e2proc, 0, 0, 0644, -, /$${file}, k) \
	done

	@$(call install_finish, driver-e2proc)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-e2proc.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_E2PROC)

# vim: syntax=make
