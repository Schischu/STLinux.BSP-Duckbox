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
PACKAGES-$(PTXCONF_DRIVER_FRONTENDS_MULTITUNER) += driver-frontends-multituner

#
# Paths and names and versions
#
DRIVER_FRONTENDS_MULTITUNER_VERSION		:= 1.0
DRIVER_FRONTENDS_MULTITUNER		:= frontends-multituner
DRIVER_FRONTENDS_MULTITUNER_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/frontends/multituner
DRIVER_FRONTENDS_MULTITUNER_DIR		:= $(BUILDDIR)/$(DRIVER_FRONTENDS_MULTITUNER)
DRIVER_FRONTENDS_MULTITUNER_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_FRONTENDS_MULTITUNER
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-frontends-multituner.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_PLATFORM_AT7500
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := AT7500=y EXTRA_CFLAGSS=-DAT7500
endif
ifdef PTXCONF_PLATFORM_UFS913
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := UFS913=y EXTRA_CFLAGSS=-DUFS913
endif
ifdef PTXCONF_PLATFORM_CUBEREVO
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := CUBEREVO=y EXTRA_CFLAGSS=-DCUBEREVO
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_MINI
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := CUBEREVO_MINI=y EXTRA_CFLAGSS=-DCUBEREVO_MINI
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_MINI2
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := CUBEREVO_MINI2=y EXTRA_CFLAGSS=-DCUBEREVO_MINI2
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_250HD
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := CUBEREVO_250HD=y EXTRA_CFLAGSS=-DCUBEREVO_250HD
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_9500HD
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := CUBEREVO_9500HD=y EXTRA_CFLAGSS=-DCUBEREVO_9500HD
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_2000HD
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := CUBEREVO_2000HD=y EXTRA_CFLAGSS=-DCUBEREVO_2000HD
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_MINI_FTA
DRIVER_FRONTENDS_MULTITUNER_EXTRAS := CUBEREVO_MINI_FTA=y EXTRA_CFLAGSS=-DCUBEREVO_MINI_FTA
endif


$(STATEDIR)/driver-frontends-multituner.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontends-multituner.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTENDS_MULTITUNER_DIR) \
		TREE_ROOT=$(DRIVER_FRONTENDS_MULTITUNER_DIR) \
		$(DRIVER_FRONTENDS_MULTITUNER_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontends-multituner.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_FRONTENDS_MULTITUNER_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTENDS_MULTITUNER_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_FRONTENDS_MULTITUNER_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_FRONTENDS_MULTITUNER_DIR)/frontends-multituner.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontends-multituner.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-frontends-multituner)
	@$(call install_fixup, driver-frontends-multituner, PRIORITY,ofrontends-multituneronal)
	@$(call install_fixup, driver-frontends-multituner, SECTION,base)
	@$(call install_fixup, driver-frontends-multituner, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontends-multituner, DESCRIFRONTENDS_MULTITUNERON,missing)

	@cd $(DRIVER_FRONTENDS_MULTITUNER_PKGDIR) && \
		find lib -type f -name *.ko | while read file; do \
			$(call install_copy, driver-frontends-multituner, 0, 0, 0644, -, /$${file}, k) \
	done

	@$(call install_finish, driver-frontends-multituner)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-frontends-multituner.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_FRONTENDS_MULTITUNER)

# vim: syntax=make
