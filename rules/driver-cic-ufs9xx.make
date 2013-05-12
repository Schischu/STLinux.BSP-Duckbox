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
PACKAGES-$(PTXCONF_DRIVER_CIC_UFS9XX) += driver-cic-ufs9xx

#
# Paths and names and versions
#
DRIVER_CIC_UFS9XX_VERSION		:= 1.0
DRIVER_CIC_UFS9XX		:= cic-ufs9xx
DRIVER_CIC_UFS9XX_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/cic/ufs9xx_cic
DRIVER_CIC_UFS9XX_DIR		:= $(BUILDDIR)/$(DRIVER_CIC_UFS9XX)
DRIVER_CIC_UFS9XX_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_CIC_UFS9XX
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-cic-ufs9xx.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#ifdef PTXCONF_PLATFORM_UFS922
#DRIVER_CIC_UFS9XX_EXTRAS := EXTRA_CFLAGS=-DUFS922
#endif
#ifdef PTXCONF_PLATFORM_UFS912
#DRIVER_CIC_UFS9XX_EXTRAS := EXTRA_CFLAGS=-DUFS912
#endif
#ifdef PTXCONF_PLATFORM_UFS913
#DRIVER_CIC_UFS9XX_EXTRAS := EXTRA_CFLAGS=-DUFS913
#endif


$(STATEDIR)/driver-cic-ufs9xx.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cic-ufs9xx.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_CIC_UFS9XX_DIR) \
		$(DRIVER_CIC_UFS9XX_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cic-ufs9xx.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_CIC_UFS9XX_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_CIC_UFS9XX_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_CIC_UFS9XX_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_CIC_UFS9XX_DIR)/cic-ufs9xx.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cic-ufs9xx.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-cic-ufs9xx)
	@$(call install_fixup, driver-cic-ufs9xx, PRIORITY,optional)
	@$(call install_fixup, driver-cic-ufs9xx, SECTION,base)
	@$(call install_fixup, driver-cic-ufs9xx, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-cic-ufs9xx, DESCRICIC_UFS9XXON,missing)

	@cd $(DRIVER_CIC_UFS9XX_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-cic-ufs9xx, 0, 0, 0644, $(DRIVER_CIC_UFS9XX_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-cic-ufs9xx)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_CIC_UFS9XX_INIT) += driver-cic-ufs9xx-init

DRIVER_CIC_UFS9XX_INIT_VERSION	:= head6

$(STATEDIR)/driver-cic-ufs9xx-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-cic-ufs9xx-init)
	@$(call install_fixup, driver-cic-ufs9xx-init,PRIORITY,optional)
	@$(call install_fixup, driver-cic-ufs9xx-init,SECTION,base)
	@$(call install_fixup, driver-cic-ufs9xx-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-cic-ufs9xx-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-cic-ufs9xx-init, 0, 0, 0755, /etc/init.d/cic-ufs9xx)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_CIC_UFS9XX_BBINIT_LINK)),)
	@$(call install_link, driver-cic-ufs9xx-init, \
		../init.d/cic-ufs9xx, \
		/etc/rc.d/$(PTXCONF_DRIVER_CIC_UFS9XX_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-cic-ufs9xx-init)
	
	@$(call touch)

# vim: syntax=make
