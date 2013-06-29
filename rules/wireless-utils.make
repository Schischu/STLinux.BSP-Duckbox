##
# We provide this package
#

PACKAGES-$(PTXCONF_WIRELESS_UTILS) 		+= wireless-utils

#
# Paths and names
#
WIRELESS_UTILS_VERSION	:= head1



# ------------------------------------------------------------------------
PACKAGES-$(PTXCONF_WIRELESS_UTILS_FIRMWARE_RT73) 		+= wireless-utils-firmware-rt73
WIRELESS_UTILS_FIRMWARE_RT73_VERSION				:=1.0
WIRELESS_UTILS_FIRMWARE_RT73_PKGDIR              		:= $(WIRELESS_UTILS)

$(STATEDIR)/wireless-utils-firmware-rt73.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   wireless-utils-firmware-rt73)
	@$(call install_fixup,  wireless-utils-firmware-rt73, PRIORITY,    optional)
	@$(call install_fixup,  wireless-utils-firmware-rt73, SECTION,     base)
	@$(call install_fixup,  wireless-utils-firmware-rt73, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  wireless-utils-firmware-rt73, DESCRIPTION, missing)
	
	@$(call install_alternative, wireless-utils-firmware-rt73, 0, 0, 0644, /etc/Wireless/RT73STA/rt73sta.dat)
	
	@$(call install_finish, wireless-utils-firmware-rt73)
	
	@$(call touch)

# ------------------------------------------------------------------------
PACKAGES-$(PTXCONF_WIRELESS_UTILS_FIRMWARE_RT2870) 		+= wireless-utils-firmware-rt2870
WIRELESS_UTILS_FIRMWARE_RT2870_VERSION				:=1.0
WIRELESS_UTILS_FIRMWARE_RT2870_PKGDIR              		:= $(WIRELESS_UTILS)

$(STATEDIR)/wireless-utils-firmware-rt2870.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   wireless-utils-firmware-rt2870)
	@$(call install_fixup,  wireless-utils-firmware-rt2870, PRIORITY,    optional)
	@$(call install_fixup,  wireless-utils-firmware-rt2870, SECTION,     base)
	@$(call install_fixup,  wireless-utils-firmware-rt2870, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  wireless-utils-firmware-rt2870, DESCRIPTION, missing)
	
	@$(call install_alternative, wireless-utils-firmware-rt2870, 0, 0, 0644, /etc/Wireless/RT2870STA/RT2870STA.dat)
	
	@$(call install_finish, wireless-utils-firmware-rt2870)
	
	@$(call touch)

# ------------------------------------------------------------------------
PACKAGES-$(PTXCONF_WIRELESS_UTILS_FIRMWARE_RT3070) 		+= wireless-utils-firmware-rt3070
WIRELESS_UTILS_FIRMWARE_RT3070_VERSION				:=1.0
WIRELESS_UTILS_FIRMWARE_RT3070_PKGDIR              		:= $(WIRELESS_UTILS)

$(STATEDIR)/wireless-utils-firmware-rt3070.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   wireless-utils-firmware-rt3070)
	@$(call install_fixup,  wireless-utils-firmware-rt3070, PRIORITY,    optional)
	@$(call install_fixup,  wireless-utils-firmware-rt3070, SECTION,     base)
	@$(call install_fixup,  wireless-utils-firmware-rt3070, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  wireless-utils-firmware-rt3070, DESCRIPTION, missing)
	
	@$(call install_alternative, wireless-utils-firmware-rt3070, 0, 0, 0644, /etc/Wireless/RT3070STA/RT3070STA.dat)
	
	@$(call install_finish, wireless-utils-firmware-rt3070)
	
	@$(call touch)

# vim: syntax=make
