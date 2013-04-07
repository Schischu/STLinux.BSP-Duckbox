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
PACKAGES-$(PTXCONF_DRIVER_STMFB) += driver-stmfb

#
# Paths and names and versions
#
DRIVER_STMFB_VERSION	:= 3.1_stm24_0104
DRIVER_STMFB		:= stmfb-$(DRIVER_STMFB_VERSION)
DRIVER_STMFB_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_STMFB)
DRIVER_STMFB_DIR		:= $(BUILDDIR)/$(DRIVER_STMFB)
DRIVER_STMFB_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_STMFB
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-stmfb.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-stmfb.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-stmfb.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_STMFB_DIR)/linux/kernel \
		STMFB_ORIGINAL_SOURCE_PATH=$(DRIVER_STMFB_DIR) \
		STG_TOPDIR=$(DRIVER_STMFB_DIR) \
		modules
		
#	@cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) \
#		$(KERNEL_MAKEVARS) $(KERNEL_IMAGE) $(PTXCONF_KERNEL_MODULES_BUILD)
		
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-stmfb.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_STMFB_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_STMFB_DIR)/linux/kernel \
		STMFB_ORIGINAL_SOURCE_PATH=$(DRIVER_STMFB_DIR) \
		STG_TOPDIR=$(DRIVER_STMFB_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_STMFB_PKGDIR) \
		modules_install
	
	mkdir -p $(SYSROOT)/usr/include
	mkdir -p $(SYSROOT)/usr/include/linux/stm
	cp $(DRIVER_STMFB_DIR)/include/* $(SYSROOT)/usr/include/linux/
	cp $(DRIVER_STMFB_DIR)/linux/kernel/include/linux/stm/* $(SYSROOT)/usr/include/linux/stm/
	#cp $(DRIVER_STMFB_DIR)/linux/kernel/drivers/video/stmcoredisplay.h $(SYSROOT)/usr/include/linux/
	cp $(DRIVER_STMFB_DIR)/linux/kernel/drivers/video/stmfb.h $(SYSROOT)/usr/include/linux/
	cp $(DRIVER_STMFB_DIR)/linux/kernel/drivers/media/video/stm_v4l2.h $(SYSROOT)/usr/include/linux/
	cp $(DRIVER_STMFB_DIR)/linux/kernel/drivers/media/video/stmvout.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-stmfb.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-stmfb)
	@$(call install_fixup, driver-stmfb, PRIORITY,optional)
	@$(call install_fixup, driver-stmfb, SECTION,base)
	@$(call install_fixup, driver-stmfb, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-stmfb, DESCRIPTION,missing)

	@cd $(DRIVER_STMFB_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-stmfb, 0, 0, 0644, $(DRIVER_STMFB_PKGDIR)/$${file}, /lib/modules/$(call remove_quotes, $(PTXCONF_KERNEL_VERSION)).$(call remove_quotes, $(PTXCONF_KERNEL_LOCALVERSION))/extra/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-stmfb)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-stmfb.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_STMFB)

# vim: syntax=make
