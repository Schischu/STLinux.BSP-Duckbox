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
PACKAGES-$(PTXCONF_DRIVER_CPUFREQU) += driver-cpufrequ

#
# Paths and names and versions
#
DRIVER_CPUFREQU_VERSION		:= 1.0
DRIVER_CPUFREQU		:= cpufrequ
DRIVER_CPUFREQU_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/cpu_frequ
DRIVER_CPUFREQU_DIR		:= $(BUILDDIR)/$(DRIVER_CPUFREQU)
DRIVER_CPUFREQU_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_CPUFREQU
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-cpufrequ.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cpufrequ.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cpufrequ.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_CPUFREQU_DIR) \
		$(DRIVER_CPUFREQU_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cpufrequ.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_CPUFREQU_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_CPUFREQU_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_CPUFREQU_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_CPUFREQU_DIR)/cpufrequ.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-cpufrequ.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-cpufrequ)
	@$(call install_fixup, driver-cpufrequ, PRIORITY,optional)
	@$(call install_fixup, driver-cpufrequ, SECTION,base)
	@$(call install_fixup, driver-cpufrequ, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-cpufrequ, DESCRICPUFREQUON,missing)

	@cd $(DRIVER_CPUFREQU_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-cpufrequ, 0, 0, 0644, $(DRIVER_CPUFREQU_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-cpufrequ)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_CPUFREQU_INIT) += driver-cpufrequ-init

DRIVER_CPUFREQU_INIT_VERSION	:= head7

$(STATEDIR)/driver-cpufrequ-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-cpufrequ-init)
	@$(call install_fixup, driver-cpufrequ-init,PRIORITY,optional)
	@$(call install_fixup, driver-cpufrequ-init,SECTION,base)
	@$(call install_fixup, driver-cpufrequ-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-cpufrequ-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-cpufrequ-init, 0, 0, 0755, /etc/init.d/cpufrequ)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_CPUFREQU_BBINIT_LINK)),)
	@$(call install_link, driver-cpufrequ-init, \
		../init.d/cpufrequ, \
		/etc/rc.d/$(PTXCONF_DRIVER_CPUFREQU_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-cpufrequ-init)
	
	@$(call touch)

# vim: syntax=make
