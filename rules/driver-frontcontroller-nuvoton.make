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
PACKAGES-$(PTXCONF_DRIVER_FRONTCONTROLLER_NUVOTON) += driver-frontcontroller-nuvoton

#
# Paths and names and versions
#
DRIVER_FRONTCONTROLLER_NUVOTON_VERSION	:= 1.0
DRIVER_FRONTCONTROLLER_NUVOTON		:= frontcontroller-nuvoton
DRIVER_FRONTCONTROLLER_NUVOTON_URL	:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/frontcontroller/nuvoton
DRIVER_FRONTCONTROLLER_NUVOTON_DIR	:= $(BUILDDIR)/$(DRIVER_FRONTCONTROLLER_NUVOTON)
DRIVER_FRONTCONTROLLER_NUVOTON_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_FRONTCONTROLLER_NUVOTON
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-frontcontroller-nuvoton.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-nuvoton.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-nuvoton.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTCONTROLLER_NUVOTON_DIR) \
		$(DRIVER_FRONTCONTROLLER_NUVOTON_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-nuvoton.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_FRONTCONTROLLER_NUVOTON_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTCONTROLLER_NUVOTON_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_FRONTCONTROLLER_NUVOTON_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_FRONTCONTROLLER_NUVOTON_DIR)/frontcontroller-NUVOTON.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-nuvoton.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-frontcontroller-nuvoton)
	@$(call install_fixup, driver-frontcontroller-nuvoton, PRIORITY,optional)
	@$(call install_fixup, driver-frontcontroller-nuvoton, SECTION,base)
	@$(call install_fixup, driver-frontcontroller-nuvoton, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontcontroller-nuvoton, DESCRIPTION,missing)

	@cd $(DRIVER_FRONTCONTROLLER_NUVOTON_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-frontcontroller-nuvoton, 0, 0, 0644, $(DRIVER_FRONTCONTROLLER_NUVOTON_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-frontcontroller-nuvoton)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_FRONTCONTROLLER_NUVOTON_INIT) += driver-frontcontroller-nuvoton-init

DRIVER_FRONTCONTROLLER_NUVOTON_INIT_VERSION := head9

$(STATEDIR)/driver-frontcontroller-nuvoton-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-frontcontroller-nuvoton-init)
	@$(call install_fixup, driver-frontcontroller-nuvoton-init,PRIORITY,optional)
	@$(call install_fixup, driver-frontcontroller-nuvoton-init,SECTION,base)
	@$(call install_fixup, driver-frontcontroller-nuvoton-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontcontroller-nuvoton-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-frontcontroller-nuvoton-init, 0, 0, 0755, /etc/init.d/frontcontroller-nuvoton)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_FRONTCONTROLLER_NUVOTON_BBINIT_LINK)),)
	@$(call install_link, driver-frontcontroller-nuvoton-init, \
		../init.d/frontcontroller-nuvoton, \
		/etc/rc.d/$(PTXCONF_DRIVER_FRONTCONTROLLER_NUVOTON_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-frontcontroller-nuvoton-init)
	
	@$(call touch)

# vim: syntax=make
