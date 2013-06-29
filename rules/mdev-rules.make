##
# We provide this package
#

PACKAGES-$(PTXCONF_MDEV_RULES) 		+= mdev-rules

#
# Paths and names
#
MDEV_RULES_VERSION	:= head1



# ------------------------------------------------------------------------
PACKAGES-$(PTXCONF_MDEV_RULES_MODULE) 		+= mdev-rules-module
MDEV_RULES_MODULE_VERSION				:=1.0
MDEV_RULES_MODULE_PKGDIR              		:= $(MDEV_RULES)

$(STATEDIR)/mdev-rules-module.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   mdev-rules-module)
	@$(call install_fixup,  mdev-rules-module, PRIORITY,    optional)
	@$(call install_fixup,  mdev-rules-module, SECTION,     base)
	@$(call install_fixup,  mdev-rules-module, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  mdev-rules-module, DESCRIPTION, missing)
	
	@$(call install_alternative, mdev-rules-module, 0, 0, 0644, /etc/mdev/module.map)
	
	@$(call install_finish, mdev-rules-module)
	
	@$(call touch)

# --------------------------------------------------------------------------
PACKAGES-$(PTXCONF_MDEV_RULES_USB_HANDLER) 	+= mdev-rules-usb-handler
MDEV_RULES_USB_HANDLER_VERSION				:=1.0
MDEV_RULES_USB_HANDLER_PKGDIR              	:= $(MDEV_RULES)

$(STATEDIR)/mdev-rules-usb-handler.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   mdev-rules-usb-handler)
	@$(call install_fixup,  mdev-rules-usb-handler, PRIORITY,    optional)
	@$(call install_fixup,  mdev-rules-usb-handler, SECTION,     base)
	@$(call install_fixup,  mdev-rules-usb-handler, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  mdev-rules-usb-handler, DESCRIPTION, missing)
	
	@$(call install_alternative,  mdev-rules-usb-handler, 0, 0, 0755, /etc/mdev/usbhandler.sh)
	
	@$(call install_finish, mdev-rules-usb-handler)
	
	@$(call touch)

# --------------------------------------------------------------------------
PACKAGES-$(PTXCONF_MDEV_RULES_WLAN) 		+= mdev-rules-wlan
MDEV_RULES_WLAN_VERSION					:=1.0
MDEV_RULES_WLAN_PKGDIR              		:= $(MDEV_RULES)

$(STATEDIR)/mdev-rules-wlan.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   mdev-rules-wlan)
	@$(call install_fixup,  mdev-rules-wlan, PRIORITY,    optional)
	@$(call install_fixup,  mdev-rules-wlan, SECTION,     base)
	@$(call install_fixup,  mdev-rules-wlan, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  mdev-rules-wlan, DESCRIPTION, missing)
	
	@$(call install_alternative,  mdev-rules-wlan, 0, 0, 0755, /etc/mdev/wlan.sh)
	
	@$(call install_finish, mdev-rules-wlan)
	
	@$(call touch)

# --------------------------------------------------------------------------
PACKAGES-$(PTXCONF_MDEV_RULES_MOUNT) 		+= mdev-rules-mount
MDEV_RULES_MOUNT_VERSION				:=1.0
MDEV_RULES_MOUNT_PKGDIR              		:= $(MDEV_RULES)

$(STATEDIR)/mdev-rules-mount.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   mdev-rules-mount)
	@$(call install_fixup,  mdev-rules-mount, PRIORITY,    optional)
	@$(call install_fixup,  mdev-rules-mount, SECTION,     base)
	@$(call install_fixup,  mdev-rules-mount, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  mdev-rules-mount, DESCRIPTION, missing)
	
	@$(call install_alternative,  mdev-rules-mount, 0, 0, 0755, /etc/mdev/mdev-mount.sh)
	
	@$(call install_finish, mdev-rules-mount)
	
	@$(call touch)

# ---------------------------------------------------------------------------

# vim: syntax=make
