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
PACKAGES-$(PTXCONF_DRIVER_MULTICOM) += driver-multicom

#
# Paths and names and versions
#
DRIVER_MULTICOM_VERSION	:= 4.0.6
DRIVER_MULTICOM		:= multicom-$(DRIVER_MULTICOM_VERSION)
DRIVER_MULTICOM_SUFFIX		:= tar.gz
DRIVER_MULTICOM_MD5	:= 7f262e7e01046be8fc5595c3cd37a74a
DRIVER_MULTICOM_URL		:= ftp://sts-tools:sts-tools@ftp.st.com/products/multicom4/R$(DRIVER_MULTICOM_VERSION)/stm-multicom.406-$(DRIVER_MULTICOM_VERSION)-noarch.$(DRIVER_MULTICOM_SUFFIX)
DRIVER_MULTICOM_SOURCE	:= $(SRCDIR)/stm-multicom.406-$(DRIVER_MULTICOM_VERSION)-noarch.$(DRIVER_MULTICOM_SUFFIX)
DRIVER_MULTICOM_DIR		:= $(BUILDDIR)/$(DRIVER_MULTICOM)
DRIVER_MULTICOM_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_MULTICOM
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-multicom.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-multicom.prepare:
	@$(call targetinfo)
	chmod 644 $(DRIVER_MULTICOM_DIR)/linux/source/include/mme.h
	chmod 644 $(DRIVER_MULTICOM_DIR)/linux/source/include/mme/*.h
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-multicom.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_MULTICOM_DIR)/linux/source \
		INSTALL_MOD_PATH=$(DRIVER_MULTICOM_PKGDIR) \
		modules
	
	cd $(DRIVER_MULTICOM_DIR)/linux/source && \
		$(MAKE) $(CROSS_ENV_CC) -f Makefile.linux
	
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-multicom.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_MULTICOM_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_MULTICOM_DIR)/linux/source \
		INSTALL_MOD_PATH=$(DRIVER_MULTICOM_PKGDIR) \
		modules_install
	
	mkdir -p $(SYSROOT)/usr/lib
	cp $(DRIVER_MULTICOM_DIR)/linux/source/lib/linux/st40/sh4-linux-/*.so $(SYSROOT)/usr/lib/
	
	mkdir -p $(SYSROOT)/usr/include
	cp $(DRIVER_MULTICOM_DIR)/linux/source/include/mme.h $(SYSROOT)/usr/include/
	cp -r $(DRIVER_MULTICOM_DIR)/linux/source/include/mme $(SYSROOT)/usr/include/
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-multicom.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-multicom)
	@$(call install_fixup, driver-multicom, PRIORITY,optional)
	@$(call install_fixup, driver-multicom, SECTION,base)
	@$(call install_fixup, driver-multicom, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-multicom, DESCRIPTION,missing)

	@cd $(DRIVER_MULTICOM_PKGDIR) && \
		find lib -type f | while read file; do \
			$(call install_copy, driver-multicom, 0, 0, 0644, -, /$${file}, k) \
	done

	@$(call install_finish, driver-multicom)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/driver-multicom.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DRIVER_MULTICOM)

# vim: syntax=make
