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
PACKAGES-$(PTXCONF_DRIVER_PTI_DEV) += driver-pti-dev

#
# Paths and names and versions
#
DRIVER_PTI_VERSION		:= 1.0
DRIVER_PTI		:= pti
DRIVER_PTI_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_PTI)
DRIVER_PTI_DIR		:= $(BUILDDIR)/$(DRIVER_PTI)
DRIVER_PTI_LICENSE	:= unknown

DRIVER_PTI_DEV_VERSION	:= $(DRIVER_PTI_VERSION)
DRIVER_PTI_DEV			:= pti-dev
DRIVER_PTI_DEV_URL		:= $(DRIVER_PTI_URL)
DRIVER_PTI_DEV_DIR		:= $(DRIVER_PTI_DIR)
DRIVER_PTI_DEV_LICENSE	:= unknown

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
		EXTRA_CFLAGSS=-DPLAYER_$(call remove_quotes,$(PTXCONF_DRIVER_PLAYER2_VERSION)) \
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
		EXTRA_CFLAGSS=-DPLAYER_$(call remove_quotes,$(PTXCONF_DRIVER_PLAYER2_VERSION)) \
		modules_install
	
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
# driver-pti-dev
# ----------------------------------------------------------------------------
$(STATEDIR)/driver-pti-dev.prepare:
	@$(call touch)
	
$(STATEDIR)/driver-pti-dev.compile:
	@$(call touch)

$(STATEDIR)/driver-pti-dev.install:
	@$(call targetinfo)
	
	mkdir -p $(SYSROOT)/usr/include/linux
	cp $(DRIVER_PTI_DIR)/pti_public.h $(SYSROOT)/usr/include/linux/
	cp $(DRIVER_PTI_DIR)/pti_hal_public.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_PTI_INIT) += driver-pti-init

DRIVER_PTI_INIT_VERSION	:= head13

$(STATEDIR)/driver-pti-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-pti-init)
	@$(call install_fixup, driver-pti-init,PRIORITY,optional)
	@$(call install_fixup, driver-pti-init,SECTION,base)
	@$(call install_fixup, driver-pti-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-pti-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-pti-init, 0, 0, 0755, /etc/init.d/pti)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_PTI_BBINIT_LINK)),)
	@$(call install_link, driver-pti-init, \
		../init.d/pti, \
		/etc/rc.d/$(PTXCONF_DRIVER_PTI_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-pti-init)
	
	@$(call touch)

# vim: syntax=make
