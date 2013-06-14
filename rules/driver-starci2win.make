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
PACKAGES-$(PTXCONF_DRIVER_STARCI2WIN) += driver-starci2win

#
# Paths and names and versions
#
DRIVER_STARCI2WIN_VERSION	:= 1.0
DRIVER_STARCI2WIN		:= starci2win
DRIVER_STARCI2WIN_URL		:= lndir://$(PTXDIST_WORKSPACE)/local_src/driver/cic/starci2win
DRIVER_STARCI2WIN_DIR		:= $(BUILDDIR)/$(DRIVER_STARCI2WIN)
DRIVER_STARCI2WIN_LICENSE	:= unknown

ifdef PTXCONF_DRIVER_STARCI2WIN
$(STATEDIR)/kernel.targetinstall.post: $(STATEDIR)/driver-starci2win.targetinstall
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#ifdef PTXCONF_PLATFORM_PLATFORM_ATEVIO7500
#DRIVER_STARCI2WIN_EXTRAS := EXTRA_CFLAGS=-DATEVIO7500
#endif

$(STATEDIR)/driver-starci2win.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-starci2win.compile:
	@$(call targetinfo)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_STARCI2WIN_DIR) \
		$(DRIVER_STARCI2WIN_EXTRAS) \
		modules
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-starci2win.install:
	@$(call targetinfo)
	mkdir -p $(DRIVER_STARCI2WIN_PKGDIR)
	cd $(KERNEL_DIR) && $(KERNEL_PATH) $(KERNEL_ENV) \
		$(MAKE) $(KERNEL_MAKEVARS) \
		-C $(KERNEL_DIR) \
		M=$(DRIVER_STARCI2WIN_DIR) \
		INSTALL_MOD_PATH=$(DRIVER_STARCI2WIN_PKGDIR) \
		modules_install

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/driver-starci2win.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  driver-starci2win)
	@$(call install_fixup, driver-starci2win, PRIORITY,optional)
	@$(call install_fixup, driver-starci2win, SECTION,base)
	@$(call install_fixup, driver-starci2win, AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-starci2win, DESCRIPTION,missing)

	@cd $(DRIVER_STARCI2WIN_PKGDIR) && \
		find lib -type f -name "*.ko" | while read file; do \
			$(call install_copy, driver-starci2win, 0, 0, 0644, $(DRIVER_STARCI2WIN_PKGDIR)/$${file}, /lib/modules/`basename $${file}`, k) \
	done

	@$(call install_finish, driver-starci2win)

	@$(call touch)

# ----------------------------------------------------------------------------
# Init
# ----------------------------------------------------------------------------

PACKAGES-$(PTXCONF_DRIVER_STARCI2WIN_INIT) += driver-starci2win-init

DRIVER_STARCI2WIN_INIT_VERSION := head6

$(STATEDIR)/driver-starci2win-init.targetinstall:
	@$(call targetinfo)

	@$(call install_init, driver-starci2win-init)
	@$(call install_fixup, driver-starci2win-init,PRIORITY,optional)
	@$(call install_fixup, driver-starci2win-init,SECTION,base)
	@$(call install_fixup, driver-starci2win-init,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, driver-starci2win-init,DESCRIPTION,missing)

ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_alternative, driver-starci2win-init, 0, 0, 0755, /etc/init.d/starci2win)

ifneq ($(call remove_quotes,$(PTXCONF_DRIVER_STARCI2WIN_BBINIT_LINK)),)
	@$(call install_link, driver-starci2win-init, \
		../init.d/starci2win, \
		/etc/rc.d/$(PTXCONF_DRIVER_STARCI2WIN_BBINIT_LINK))
endif
endif

	@$(call install_finish, driver-starci2win-init)

	@$(call touch)

# vim: syntax=make
