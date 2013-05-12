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
PACKAGES-$(PTXCONF_DRIVER_FRONTCONTROLLER_AOTOM) += driver-frontcontroller-aotom

#
# Paths and names and versions
#
DRIVER_FRONTCONTROLLER_AOTOM_VERSION	:= 1.0
DRIVER_FRONTCONTROLLER_AOTOM		:= frontcontroller-aotom
DRIVER_FRONTCONTROLLER_AOTOM_URL	:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/frontcontroller/aotom
DRIVER_FRONTCONTROLLER_AOTOM_DIR	:= $(BUILDDIR)/$(DRIVER_FRONTCONTROLLER_AOTOM)
DRIVER_FRONTCONTROLLER_AOTOM_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_FRONTCONTROLLER_AOTOM
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-frontcontroller-aotom.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/driver-frontcontroller-aotom.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-aotom.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTCONTROLLER_AOTOM_DIR) \
		$(DRIVER_FRONTCONTROLLER_AOTOM_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-aotom.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_FRONTCONTROLLER_AOTOM_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTCONTROLLER_AOTOM_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_FRONTCONTROLLER_AOTOM_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_FRONTCONTROLLER_AOTOM_DIR)/aotom_main.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-aotom.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-frontcontroller-aotom)
	@$(call install_fixup, driver-frontcontroller-aotom, PRIORITY,optional)
	@$(call install_fixup, driver-frontcontroller-aotom, SECTION,base)
	@$(call install_fixup, driver-frontcontroller-aotom, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontcontroller-aotom, DESCRIFRONTCONTROLLER_AOTOMON,missing)

	@cd $(DRIVER_FRONTCONTROLLER_AOTOM_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-frontcontroller-aotom, 0, 0, 0644, $(DRIVER_FRONTCONTROLLER_AOTOM_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-frontcontroller-aotom)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_FRONTCONTROLLER_AOTOM_INIT) += driver-frontcontroller-aotom-init

DRIVER_FRONTCONTROLLER_AOTOM_INIT_VERSION	:= head1

$(STATEDIR)/driver-frontcontroller-aotom-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-frontcontroller-aotom-init)
	@$(call install_fixup, driver-frontcontroller-aotom-init,PRIORITY,optional)
	@$(call install_fixup, driver-frontcontroller-aotom-init,SECTION,base)
	@$(call install_fixup, driver-frontcontroller-aotom-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontcontroller-aotom-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-frontcontroller-aotom-init, 0, 0, 0755, /etc/init.d/frontcontroller-aotom)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_FRONTCONTROLLER_AOTOM_BBINIT_LINK)),)
	@$(call install_link, driver-frontcontroller-aotom-init, \
		../init.d/frontcontroller-aotom, \
		/etc/rc.d/$(PTXCONF_DRIVER_FRONTCONTROLLER_AOTOM_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-frontcontroller-aotom-init)
	
	@$(call touch)

# vim: syntax=make
