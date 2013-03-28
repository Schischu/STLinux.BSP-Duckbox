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
PACKAGES-$(PTXCONF_DUCKBOX_DRIVERS) += duckbox-drivers

#
# Paths and names and versions
#
DUCKBOX_DRIVERS_VERSION	:= @VERSION@
DUCKBOX_DRIVERS		:= duckbox-drivers-$(DUCKBOX_DRIVERS_VERSION)
DUCKBOX_DRIVERS_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/$(DUCKBOX_DRIVERS)
DUCKBOX_DRIVERS_DIR		:= $(BUILDDIR)/$(DUCKBOX_DRIVERS)
DUCKBOX_DRIVERS_LICENSE	:= unknown

ifdef PTXCONF_DUCKBOX_DRIVERS
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/duckbox-drivers.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/duckbox-drivers.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/duckbox-drivers.compile:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DUCKBOX_DRIVERS_DIR) \
		modules
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/duckbox-drivers.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/duckbox-drivers.targetinstall:
	@$(call targetinfo)
	$(KERNEL_PATH) $(KERNEL_ENV) $(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DUCKBOX_DRIVERS_DIR) \
		modules_install
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/duckbox-drivers.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DUCKBOX_DRIVERS)

# vim: syntax=make
