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
PACKAGES-$(PTXCONF_DRIVER_SIMUBUTTON) += driver-simubutton

#
# Paths and names and versions
#
DRIVER_SIMUBUTTON_VERSION		:= 1.0
DRIVER_SIMUBUTTON		:= simubutton
DRIVER_SIMUBUTTON_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/simu_button
DRIVER_SIMUBUTTON_DIR		:= $(BUILDDIR)/$(DRIVER_SIMUBUTTON)
DRIVER_SIMUBUTTON_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_SIMUBUTTON
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-simubutton.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-simubutton.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-simubutton.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_SIMUBUTTON_DIR) \
		$(DRIVER_SIMUBUTTON_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-simubutton.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_SIMUBUTTON_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_SIMUBUTTON_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_SIMUBUTTON_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_SIMUBUTTON_DIR)/simubutton.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-simubutton.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-simubutton)
	@$(call install_fixup, driver-simubutton, PRIORITY,optional)
	@$(call install_fixup, driver-simubutton, SECTION,base)
	@$(call install_fixup, driver-simubutton, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-simubutton, DESCRIPTION,missing)

	@cd $(DRIVER_SIMUBUTTON_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-simubutton, 0, 0, 0644, $(DRIVER_SIMUBUTTON_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-simubutton)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_SIMUBUTTON_INIT) += driver-simubutton-init

DRIVER_SIMUBUTTON_INIT_VERSION	:= head14

$(STATEDIR)/driver-simubutton-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-simubutton-init)
	@$(call install_fixup, driver-simubutton-init,PRIORITY,optional)
	@$(call install_fixup, driver-simubutton-init,SECTION,base)
	@$(call install_fixup, driver-simubutton-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-simubutton-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-simubutton-init, 0, 0, 0755, /etc/init.d/simubutton)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_SIMUBUTTON_BBINIT_LINK)),)
	@$(call install_link, driver-simubutton-init, \
		../init.d/simubutton, \
		/etc/rc.d/$(PTXCONF_DRIVER_SIMUBUTTON_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-simubutton-init)
	
	@$(call touch)

# vim: syntax=make
