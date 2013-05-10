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
PACKAGES-$(PTXCONF_DRIVER_FRONTEND_STV090X) += driver-frontend-stv090x

#
# Paths and names and versions
#
DRIVER_FRONTEND_STV090X_VERSION		:= 1.0
DRIVER_FRONTEND_STV090X		:= frontend-stv090x
DRIVER_FRONTEND_STV090X_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/frontends/stv090x
DRIVER_FRONTEND_STV090X_DIR		:= $(BUILDDIR)/$(DRIVER_FRONTEND_STV090X)
DRIVER_FRONTEND_STV090X_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_FRONTEND_STV090X
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-frontend-stv090x.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#ifdef PTXCONF_PLATFORM_AT7500
#DRIVER_FRONTEND_STV090X_EXTRAS := AT7500=y EXTRA_CFLAGSS=-DAT7500
#endif
#ifdef PTXCONF_PLATFORM_UFS913
#DRIVER_FRONTEND_STV090X_EXTRAS := UFS913=y EXTRA_CFLAGSS=-DUFS913
#endif
#ifdef PTXCONF_PLATFORM_CUBEREVO
#DRIVER_FRONTEND_STV090X_EXTRAS := CUBEREVO=y EXTRA_CFLAGSS=-DCUBEREVO
#endif
#ifdef PTXCONF_PLATFORM_CUBEREVO_MINI
#DRIVER_FRONTEND_STV090X_EXTRAS := CUBEREVO_MINI=y EXTRA_CFLAGSS=-DCUBEREVO_MINI
#endif
#ifdef PTXCONF_PLATFORM_CUBEREVO_MINI2
#DRIVER_FRONTEND_STV090X_EXTRAS := CUBEREVO_MINI2=y EXTRA_CFLAGSS=-DCUBEREVO_MINI2
#endif
#ifdef PTXCONF_PLATFORM_CUBEREVO_250HD
#DRIVER_FRONTEND_STV090X_EXTRAS := CUBEREVO_250HD=y EXTRA_CFLAGSS=-DCUBEREVO_250HD
#endif
#ifdef PTXCONF_PLATFORM_CUBEREVO_9500HD
#DRIVER_FRONTEND_STV090X_EXTRAS := CUBEREVO_9500HD=y EXTRA_CFLAGSS=-DCUBEREVO_9500HD
#endif
#ifdef PTXCONF_PLATFORM_CUBEREVO_2000HD
#DRIVER_FRONTEND_STV090X_EXTRAS := CUBEREVO_2000HD=y EXTRA_CFLAGSS=-DCUBEREVO_2000HD
#endif
#ifdef PTXCONF_PLATFORM_CUBEREVO_MINI_FTA
#DRIVER_FRONTEND_STV090X_EXTRAS := CUBEREVO_MINI_FTA=y EXTRA_CFLAGSS=-DCUBEREVO_MINI_FTA
#endif


$(STATEDIR)/driver-frontend-stv090x.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-stv090x.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTEND_STV090X_DIR) \
		TREE_ROOT=$(DRIVER_FRONTEND_STV090X_DIR) \
		$(DRIVER_FRONTEND_STV090X_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-stv090x.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_FRONTEND_STV090X_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTEND_STV090X_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_FRONTEND_STV090X_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_FRONTEND_STV090X_DIR)/frontend-stv090x.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontend-stv090x.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-frontend-stv090x)
	@$(call install_fixup, driver-frontend-stv090x, PRIORITY,optional)
	@$(call install_fixup, driver-frontend-stv090x, SECTION,base)
	@$(call install_fixup, driver-frontend-stv090x, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontend-stv090x, DESCRIFRONTEND_STV090XON,missing)

	@cd $(DRIVER_FRONTEND_STV090X_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-frontend-stv090x, 0, 0, 0644, $(DRIVER_FRONTEND_STV090X_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-frontend-stv090x)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_FRONTEND_STV090X_INIT) += driver-frontend-stv090x-init

DRIVER_FRONTEND_STV090X_INIT_VERSION	:= head10

$(STATEDIR)/driver-frontend-stv090x-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,  driver-frontend-stv090x-init)
	@$(call install_fixup, driver-frontend-stv090x-init,PRIORITY,optional)
	@$(call install_fixup, driver-frontend-stv090x-init,SECTION,base)
	@$(call install_fixup, driver-frontend-stv090x-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontend-stv090x-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-frontend-stv090x-init, 0, 0, 0755, /etc/init.d/frontend-stv090x)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_FRONTEND_STV090X_BBINIT_LINK)),)
	@$(call install_link, driver-frontend-stv090x-init, \
		../init.d/frontend-stv090x, \
		/etc/rc.d/$(PTXCONF_DRIVER_FRONTEND_STV090X_BBINIT_LINK))
endif
endif
	
	@$(call install_finish, driver-frontend-stv090x-init)
	
	@$(call touch)

# vim: syntax=make
