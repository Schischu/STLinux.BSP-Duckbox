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
	@$(call install_fixup, driver-cpufrequ, PRIORITY,ocpufrequonal)
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
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-cpufrequ.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_CPUFREQU)

# vim: syntax=make
