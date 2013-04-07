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
PACKAGES-$(PTXCONF_DRIVER_CEC) += driver-cec

#
# Paths and names and versions
#
DRIVER_CEC_VERSION		:= 1.0
DRIVER_CEC		:= cec
DRIVER_CEC_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_CEC)
DRIVER_CEC_DIR		:= $(BUILDDIR)/$(DRIVER_CEC)
DRIVER_CEC_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_CEC
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-cec.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cec.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cec.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_CEC_DIR) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cec.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_CEC_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_CEC_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_CEC_PKGDIR) \
		modules_install
	
	mkdir -p $(SYSROOT)/usr/include/linux
	cp $(DRIVER_CEC_DIR)/cec.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cec.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-cec)
	@$(call install_fixup, driver-cec, PRIORITY,oceconal)
	@$(call install_fixup, driver-cec, SECTION,base)
	@$(call install_fixup, driver-cec, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-cec, DESCRICECON,missing)

	@cd $(DRIVER_CEC_PKGDIR) && \
		find lib -type f -name *.ko | while read file; do \
			$(call install_copy, driver-cec, 0, 0, 0644, -, /$${file}, k) \
	done

	@$(call install_finish, driver-cec)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-cec.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_CEC)

# vim: syntax=make
