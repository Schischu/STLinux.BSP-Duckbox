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
PACKAGES-$(PTXCONF_DRIVER_STARCI) += driver-starci

#
# Paths and names and versions
#
DRIVER_STARCI_VERSION	:= 1.0
DRIVER_STARCI		:= starci
DRIVER_STARCI_URL	:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/cic/starci2win
DRIVER_STARCI_DIR	:= $(BUILDDIR)/$(DRIVER_STARCI)
DRIVER_STARCI_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_STARCI
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-starci.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#ifdef PTXCONF_PLATFORM_PLATFORM_ATEVIO7500
#DRIVER_STARCI_EXTRAS := EXTRA_CFLAGS=-DATEVIO7500
#endif

$(STATEDIR)/driver-starci.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-starci.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_STARCI_DIR) \
		$(DRIVER_STARCI_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-starci.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_STARCI_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_STARCI_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_STARCI_PKGDIR) \
		modules_install

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-starci.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-starci)
	@$(call install_fixup, driver-starci, PRIORITY,optional)
	@$(call install_fixup, driver-starci, SECTION,base)
	@$(call install_fixup, driver-starci, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-starci, DESCRIPTION,missing)

	@cd $(DRIVER_STARCI_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-starci, 0, 0, 0644, $(DRIVER_STARCI_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-starci)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_STARCI_INIT) += driver-starci-init

DRIVER_STARCI_INIT_VERSION := head6

$(STATEDIR)/driver-starci-init.targetinstall:
	@$(call targetinfo)

	@$(call install_init, driver-starci-init)
	@$(call install_fixup, driver-starci-init,PRIORITY,optional)
	@$(call install_fixup, driver-starci-init,SECTION,base)
	@$(call install_fixup, driver-starci-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-starci-init,DESCRIPTION,missing)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-starci-init, 0, 0, 0755, /etc/init.d/starci)

ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_STARCI_BBINIT_LINK)),)
	@$(call install_link, driver-starci-init, \
		../init.d/starci, \
		/etc/rc.d/$(PTXCONF_DRIVER_STARCI_BBINIT_LINK))
endif
endif

	@$(call install_finish, driver-starci-init)

	@$(call touch)

# vim: syntax=make
