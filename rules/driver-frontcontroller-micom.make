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
PACKAGES-$(PTXCONF_DRIVER_FRONTCONTROLLER_MICOM) += driver-frontcontroller-micom

#
# Paths and names and versions
#
DRIVER_FRONTCONTROLLER_MICOM_VERSION		:= 1.0
DRIVER_FRONTCONTROLLER_MICOM		:= frontcontroller-micom
DRIVER_FRONTCONTROLLER_MICOM_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/frontcontroller/micom
DRIVER_FRONTCONTROLLER_MICOM_DIR		:= $(BUILDDIR)/$(DRIVER_FRONTCONTROLLER_MICOM)
DRIVER_FRONTCONTROLLER_MICOM_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_FRONTCONTROLLER_MICOM
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-frontcontroller-micom.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_PLATFORM_UFS922
DRIVER_FRONTCONTROLLER_MICOM_EXTRAS := EXTRA_CFLAGS=-DUFS922
endif
ifdef PTXCONF_PLATFORM_UFS912
DRIVER_FRONTCONTROLLER_MICOM_EXTRAS := EXTRA_CFLAGS=-DUFS912
endif
ifdef PTXCONF_PLATFORM_UFS913
DRIVER_FRONTCONTROLLER_MICOM_EXTRAS := EXTRA_CFLAGS=-DUFS913
endif


$(STATEDIR)/driver-frontcontroller-micom.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-micom.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTCONTROLLER_MICOM_DIR) \
		$(DRIVER_FRONTCONTROLLER_MICOM_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-micom.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_FRONTCONTROLLER_MICOM_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_FRONTCONTROLLER_MICOM_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_FRONTCONTROLLER_MICOM_PKGDIR) \
		modules_install
	
	#mkdir -p $(SYSROOT)/usr/include/linux
	#cp $(DRIVER_FRONTCONTROLLER_MICOM_DIR)/frontcontroller-micom.h $(SYSROOT)/usr/include/linux/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-frontcontroller-micom.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-frontcontroller-micom)
	@$(call install_fixup, driver-frontcontroller-micom, PRIORITY,ofrontcontroller-micomonal)
	@$(call install_fixup, driver-frontcontroller-micom, SECTION,base)
	@$(call install_fixup, driver-frontcontroller-micom, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-frontcontroller-micom, DESCRIFRONTCONTROLLER_MICOMON,missing)

	@cd $(DRIVER_FRONTCONTROLLER_MICOM_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-frontcontroller-micom, 0, 0, 0644, $(DRIVER_FRONTCONTROLLER_MICOM_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-frontcontroller-micom)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-frontcontroller-micom.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_FRONTCONTROLLER_MICOM)

# vim: syntax=make
