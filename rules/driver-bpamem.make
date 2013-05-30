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
PACKAGES-$(PTXCONF_DRIVER_BPAMEM) += driver-bpamem

#
# Paths and names and versions
#
DRIVER_BPAMEM_VERSION		:= 1.0
DRIVER_BPAMEM		:= bpamem
DRIVER_BPAMEM_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_BPAMEM)
DRIVER_BPAMEM_DIR		:= $(BUILDDIR)/$(DRIVER_BPAMEM)
DRIVER_BPAMEM_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_BPAMEM
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-bpamem.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-bpamem.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-bpamem.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_BPAMEM_DIR) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-bpamem.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_BPAMEM_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_BPAMEM_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_BPAMEM_PKGDIR) \
		modules_install
	
	mkdir -p $(SYSROOT)/usr/include
	cp $(DRIVER_BPAMEM_DIR)/bpamem.h $(SYSROOT)/usr/include/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-bpamem.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-bpamem)
	@$(call install_fixup, driver-bpamem, PRIORITY,optional)
	@$(call install_fixup, driver-bpamem, SECTION,base)
	@$(call install_fixup, driver-bpamem, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-bpamem, DESCRIPTION,missing)

	@cd $(DRIVER_BPAMEM_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-bpamem, 0, 0, 0644, $(DRIVER_BPAMEM_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-bpamem)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_BPAMEM_INIT) += driver-bpamem-init

DRIVER_BPAMEM_INIT_VERSION	:= head4

$(STATEDIR)/driver-bpamem-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-bpamem-init)
	@$(call install_fixup, driver-bpamem-init,PRIORITY,optional)
	@$(call install_fixup, driver-bpamem-init,SECTION,base)
	@$(call install_fixup, driver-bpamem-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-bpamem-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-bpamem-init, 0, 0, 0755, /etc/init.d/bpamem)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_BPAMEM_BBINIT_LINK)),)
	@$(call install_link, driver-bpamem-init, \
		../init.d/bpamem, \
		/etc/rc.d/$(PTXCONF_DRIVER_BPAMEM_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-bpamem-init)
	
	@$(call touch)

# vim: syntax=make
