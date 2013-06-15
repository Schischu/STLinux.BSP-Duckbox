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
PACKAGES-$(PTXCONF_DRIVER_FRONTEND_MULTITUNER) += driver-frontend-multituner

#
# Paths and names and versions
#
DRIVER_FRONTEND_MULTITUNER_VERSION	:= 1.0
DRIVER_FRONTEND_MULTITUNER		:= frontend-multituner
DRIVER_FRONTEND_MULTITUNER_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/frontends/multituner
DRIVER_FRONTEND_MULTITUNER_DIR		:= $(BUILDDIR)/$(DRIVER_FRONTEND_MULTITUNER)
DRIVER_FRONTEND_MULTITUNER_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_FRONTEND_MULTITUNER
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-frontend-multituner.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_PLATFORM_ATEVIO7500
DRIVER_FRONTEND_MULTITUNER_EXTRAS := ATEVIO7500=y EXTRA_CFLAGSS=-DATEVIO7500
endif
ifdef PTXCONF_PLATFORM_UFS912
DRIVER_FRONTEND_MULTITUNER_EXTRAS := UFS912=y EXTRA_CFLAGSS=-DUFS912
endif
ifdef PTXCONF_PLATFORM_UFS913
DRIVER_FRONTEND_MULTITUNER_EXTRAS := UFS913=y EXTRA_CFLAGSS=-DUFS913
endif
ifdef PTXCONF_PLATFORM_CUBEREVO
DRIVER_FRONTEND_MULTITUNER_EXTRAS := CUBEREVO=y EXTRA_CFLAGSS=-DCUBEREVO
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_MINI
DRIVER_FRONTEND_MULTITUNER_EXTRAS := CUBEREVO_MINI=y EXTRA_CFLAGSS=-DCUBEREVO_MINI
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_MINI2
DRIVER_FRONTEND_MULTITUNER_EXTRAS := CUBEREVO_MINI2=y EXTRA_CFLAGSS=-DCUBEREVO_MINI2
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_250HD
DRIVER_FRONTEND_MULTITUNER_EXTRAS := CUBEREVO_250HD=y EXTRA_CFLAGSS=-DCUBEREVO_250HD
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_9500HD
DRIVER_FRONTEND_MULTITUNER_EXTRAS := CUBEREVO_9500HD=y EXTRA_CFLAGSS=-DCUBEREVO_9500HD
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_2000HD
DRIVER_FRONTEND_MULTITUNER_EXTRAS := CUBEREVO_2000HD=y EXTRA_CFLAGSS=-DCUBEREVO_2000HD
endif
ifdef PTXCONF_PLATFORM_CUBEREVO_MINI_FTA
DRIVER_FRONTEND_MULTITUNER_EXTRAS := CUBEREVO_MINI_FTA=y EXTRA_CFLAGSS=-DCUBEREVO_MINI_FTA
endif


$(STATEDIR)/driver-frontend-multituner.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-multituner.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTEND_MULTITUNER_DIR) \
		TREE_ROOT=$(DRIVER_FRONTEND_MULTITUNER_DIR) \
		$(DRIVER_FRONTEND_MULTITUNER_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-multituner.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_FRONTEND_MULTITUNER_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTEND_MULTITUNER_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_FRONTEND_MULTITUNER_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_FRONTEND_MULTITUNER_DIR)/frontend-multituner.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-multituner.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-frontend-multituner)
	@$(call install_fixup, driver-frontend-multituner, PRIORITY,optional)
	@$(call install_fixup, driver-frontend-multituner, SECTION,base)
	@$(call install_fixup, driver-frontend-multituner, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontend-multituner, DESCRIPTION,missing)

	@cd $(DRIVER_FRONTEND_MULTITUNER_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-frontend-multituner, 0, 0, 0644, $(DRIVER_FRONTEND_MULTITUNER_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-frontend-multituner)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_FRONTEND_MULTITUNER_INIT) += driver-frontend-multituner-init

DRIVER_FRONTEND_MULTITUNER_INIT_VERSION	:= head

$(STATEDIR)/driver-frontend-multituner-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  driver-frontend-multituner-init)
	@$(call install_fixup, driver-frontend-multituner-init,PRIORITY,optional)
	@$(call install_fixup, driver-frontend-multituner-init,SECTION,base)
	@$(call install_fixup, driver-frontend-multituner-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontend-multituner-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-frontend-multituner-init, 0, 0, 0755, /etc/init.d/frontend-multituner)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_FRONTEND_MULTITUNER_BBINIT_LINK)),)
	@$(call install_link, driver-frontend-multituner-init, \
		../init.d/frontend-multituner, \
		/etc/rc.d/$(PTXCONF_DRIVER_FRONTEND_MULTITUNER_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-frontend-multituner-init)
	
	@$(call touch)

# vim: syntax=make
