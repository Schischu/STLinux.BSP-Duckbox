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
PACKAGES-$(PTXCONF_DRIVER_AVS) += driver-avs

#
# Paths and names and versions
#
DRIVER_AVS_VERSION		:= 1.0
DRIVER_AVS		:= avs
DRIVER_AVS_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_AVS)
DRIVER_AVS_DIR		:= $(BUILDDIR)/$(DRIVER_AVS)
DRIVER_AVS_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_AVS
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-avs.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-avs.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-avs.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_AVS_DIR) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-avs.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_AVS_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_AVS_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_AVS_PKGDIR) \
		modules_install
	
	mkdir -p $(SYSROOT)/usr/include/linux
	cp $(DRIVER_AVS_DIR)/avs_core.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-avs.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-avs)
	@$(call install_fixup, driver-avs, PRIORITY,optional)
	@$(call install_fixup, driver-avs, SECTION,base)
	@$(call install_fixup, driver-avs, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-avs, DESCRIPTION,missing)
	
	@cd $(DRIVER_AVS_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-avs, 0, 0, 0644, $(DRIVER_AVS_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done
	
	@$(call install_finish, driver-avs)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_AVS_INIT) += driver-avs-init

DRIVER_AVS_INIT_VERSION	:= head3

$(STATEDIR)/driver-avs-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-avs-init)
	@$(call install_fixup, driver-avs-init, PRIORITY,optional)
	@$(call install_fixup, driver-avs-init, SECTION,base)
	@$(call install_fixup, driver-avs-init, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-avs-init, DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-avs-init, 0, 0, 0755, /etc/init.d/avs)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_AVS_BBINIT_LINK)),)
	@$(call install_link, driver-avs-init, \
		../init.d/avs, \
		/etc/rc.d/$(PTXCONF_DRIVER_AVS_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-avs-init)
	
	@$(call touch)

# vim: syntax=make
