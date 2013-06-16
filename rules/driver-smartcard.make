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
PACKAGES-$(PTXCONF_DRIVER_SMARTCARD) += driver-smartcard

#
# Paths and names and versions
#
DRIVER_SMARTCARD_VERSION		:= 1.0
DRIVER_SMARTCARD		:= smartcard
DRIVER_SMARTCARD_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_SMARTCARD)
DRIVER_SMARTCARD_DIR		:= $(BUILDDIR)/$(DRIVER_SMARTCARD)
DRIVER_SMARTCARD_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_SMARTCARD
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-smartcard.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-smartcard.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-smartcard.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_SMARTCARD_DIR) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-smartcard.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_SMARTCARD_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_SMARTCARD_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_SMARTCARD_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_SMARTCARD_DIR)/cec.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-smartcard.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-smartcard)
	@$(call install_fixup, driver-smartcard, PRIORITY,optional)
	@$(call install_fixup, driver-smartcard, SECTION,base)
	@$(call install_fixup, driver-smartcard, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-smartcard, DESCRIPTION,missing)

	@cd $(DRIVER_SMARTCARD_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-smartcard, 0, 0, 0644, $(DRIVER_SMARTCARD_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-smartcard)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_SMARTCARD_INIT) += driver-smartcard-init

DRIVER_SMARTCARD_INIT_VERSION	:= head5

$(STATEDIR)/driver-smartcard-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-smartcard-init)
	@$(call install_fixup, driver-smartcard-init,PRIORITY,optional)
	@$(call install_fixup, driver-smartcard-init,SECTION,base)
	@$(call install_fixup, driver-smartcard-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-smartcard-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-smartcard-init, 0, 0, 0755, /etc/init.d/smartcard)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_SMARTCARD_BBINIT_LINK)),)
	@$(call install_link, driver-smartcard-init, \
		../init.d/smartcard, \
		/etc/rc.d/$(PTXCONF_DRIVER_SMARTCARD_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-smartcard-init)
	
	@$(call touch)

# vim: syntax=make
