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
PACKAGES-$(PTXCONF_DRIVER_PLAYER2) += driver-player2

#
# Paths and names and versions
#
DRIVER_PLAYER2_VERSION	:= $(call remove_quotes,$(PTXCONF_DRIVER_PLAYER2_VERSION))
DRIVER_PLAYER2		:= player2_$(DRIVER_PLAYER2_VERSION)
DRIVER_PLAYER2_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_PLAYER2)
DRIVER_PLAYER2_DIR		:= $(BUILDDIR)/$(DRIVER_PLAYER2)
DRIVER_PLAYER2_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_PLAYER2
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-player2.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_DRIVER_FRONTEND_CX24116
DRIVER_PLAYER2_EXTRAS += -DFRONTEND_CX24116
endif
ifdef PTXCONF_DRIVER_FRONTEND_AVL2108
DRIVER_PLAYER2_EXTRAS += -DFRONTEND_AVL2108
endif
ifdef PTXCONF_DRIVER_FRONTEND_STV090X
DRIVER_PLAYER2_EXTRAS += -DFRONTEND_STV090X
endif
ifdef PTXCONF_DRIVER_FRONTEND_CORE
DRIVER_PLAYER2_EXTRAS += -DFRONTEND_CORE
endif
ifdef PTXCONF_DRIVER_FRONTEND_SPARK
ifdef PTXCONF_PLATFORM_SPARK
DRIVER_PLAYER2_EXTRAS += -DFRONTEND_STV090X
endif
ifdef PTXCONF_PLATFORM_SPARK7162
DRIVER_PLAYER2_EXTRAS += -DFRONTEND_SPARK7162
endif
endif
ifdef PTXCONF_DRIVER_FRONTEND_MULTITUNER
DRIVER_PLAYER2_EXTRAS += -DFRONTEND_MULTITUNER
endif

ifdef PTXCONF_DRIVER_MULTICOM_V3
DRIVER_PLAYER2_EXTRAS += -DMULTICOM_VERSION=3
endif
ifdef PTXCONF_DRIVER_MULTICOM_V4
DRIVER_PLAYER2_EXTRAS += -DMULTICOM_VERSION=4
endif

$(STATEDIR)/driver-player2.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_PLAYER2_DIR)/linux \
		KERNEL_LOCATION=$(KERNEL_DIR) \
		TREE_ROOT=$(DRIVER_PLAYER2_DIR) \
		CONFIG_MODULES_PATH=$(SYSROOT)/usr/include \
		CONFIG_KERNEL_PATH="$(PTXDIST_SYSROOT_HOST)/usr" \
		CONFIG_KERNEL_BUILD=$(KERNEL_DIR) \
		EXTRA_CFLAGSS="$(DRIVER_PLAYER2_EXTRAS)" \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_PLAYER2_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_PLAYER2_DIR)/linux \
		INSTALL_MOD_PATH=$(DRIVER_PLAYER2_PKGDIR) \
		EXTRA_CFLAGSS="$(DRIVER_PLAYER2_EXTRAS)" \
		modules_install
	
	mkdir -p $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/linux/include/linux/dvb/stm_ioctls.h $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/linux/include/linux/dvb/stm_dvb.h $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/linux/include/linux/dvb/stm_audio.h $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/linux/include/linux/dvb/stm_video.h $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/player/wrapper/player_api.h $(SYSROOT)/usr/include/
	cp $(DRIVER_PLAYER2_DIR)/linux/drivers/media/dvb/stm/dvb/backend.h $(SYSROOT)/usr/include/linux/dvb
ifeq ($(DRIVER_PLAYER2_VERSION),191)
	cp $(DRIVER_PLAYER2_DIR)/linux/drivers/media/dvb/stm/dvb/backend_ops.h $(SYSROOT)/usr/include/linux/dvb
endif
	cp $(DRIVER_PLAYER2_DIR)/linux/drivers/media/dvb/stm/dvb/dvb_module.h $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/linux/drivers/media/dvb/stm/dvb/dvb_audio.h $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/linux/drivers/media/dvb/stm/dvb/dvb_video.h $(SYSROOT)/usr/include/linux/dvb
	cp $(DRIVER_PLAYER2_DIR)/player/standards/dvp.h $(SYSROOT)/usr/include/linux/dvb
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-player2.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-player2)
	@$(call install_fixup, driver-player2, PRIORITY,optional)
	@$(call install_fixup, driver-player2, SECTION,base)
	@$(call install_fixup, driver-player2, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-player2, DESCRIPTION,missing)

	@cd $(DRIVER_PLAYER2_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-player2, 0, 0, 0644, $(DRIVER_PLAYER2_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-player2)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_PLAYER2_INIT) += driver-player2-init

DRIVER_PLAYER2_INIT_VERSION	:= head

$(STATEDIR)/driver-player2-init.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, driver-player2-init)
	@$(call install_fixup, driver-player2-init,PRIORITY,optional)
	@$(call install_fixup, driver-player2-init,SECTION,base)
	@$(call install_fixup, driver-player2-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-player2-init,DESCRIPTION,missing)
	
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-player2-init, 0, 0, 0755, /etc/init.d/player2pre)
	@$(call install_alternative, driver-player2-init, 0, 0, 0755, /etc/init.d/player2)
	
ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_PLAYER2PRE_BBINIT_LINK)),)
	@$(call install_link, driver-player2-init, \
		../init.d/player2pre, \
		/etc/rc.d/$(PTXCONF_DRIVER_PLAYER2PRE_BBINIT_LINK))

ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_PLAYER2_BBINIT_LINK)),)
	@$(call install_link, driver-player2-init, \
		../init.d/player2, \
		/etc/rc.d/$(PTXCONF_DRIVER_PLAYER2_BBINIT_LINK))
endif
endif
endif
	
	@$(call install_finish, driver-player2-init)
	
	@$(call touch)

# vim: syntax=make
