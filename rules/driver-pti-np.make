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
PACKAGES-$(PTXCONF_DRIVER_PTI_NP) += driver-pti-np

#
# Paths and names and versions
#
DRIVER_PTI_NP_VERSION		:= 1.0
DRIVER_PTI_NP		:= pti_np
DRIVER_PTI_NP_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/$(DRIVER_PTI_NP)
DRIVER_PTI_NP_DIR		:= $(BUILDDIR)/$(DRIVER_PTI_NP)
DRIVER_PTI_NP_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_PTI_NP
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-pti-np.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-pti-np.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-pti-np.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_PTI_NP_DIR) \
		EXTRA_CFLAGSS=-DPLAYER2_VERSION=$(call remove_quotes,$(PTXCONF_DRIVER_PLAYER2_VERSION)) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-pti-np.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_PTI_NP_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_PTI_NP_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_PTI_NP_PKGDIR) \
		EXTRA_CFLAGSS=-DPLAYER2_VERSION=$(call remove_quotes,$(PTXCONF_DRIVER_PLAYER2_VERSION)) \
		modules_install
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-pti-np.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-pti-np)
	@$(call install_fixup, driver-pti-np, PRIORITY,optional)
	@$(call install_fixup, driver-pti-np, SECTION,base)
	@$(call install_fixup, driver-pti-np, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-pti-np, DESCRIPTION,missing)

	@cd $(DRIVER_PTI_NP_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-pti-np, 0, 0, 0644, $(DRIVER_PTI_NP_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-pti-np)

	@$(call touch)

# vim: syntax=make
