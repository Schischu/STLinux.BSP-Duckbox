# -*-makefile-*-
#
# Copyright (C) 2008 by Robert Schwebel
#               2010 by Marc Kleine-Budde <mkl@penutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#

ifdef PTXCONF_DVB_FIRMWARE
PACKAGES-$(PTXCONF_DVB_FIRMWARE_AS102)   += dvb-firmware-as102
PACKAGES-$(PTXCONF_DVB_FIRMWARE_AVL2108) += dvb-firmware-avl2108
PACKAGES-$(PTXCONF_DVB_FIRMWARE_AVL6222) += dvb-firmware-avl6222
PACKAGES-$(PTXCONF_DVB_FIRMWARE_CX21143) += dvb-firmware-cx21143
PACKAGES-$(PTXCONF_DVB_FIRMWARE_CX24116) += dvb-firmware-cx24116
PACKAGES-$(PTXCONF_DVB_FIRMWARE_STV6306) += dvb-firmware-stv6306
endif

DVB_FIRMWARE_AS102_VERSION   := head
DVB_FIRMWARE_AVL2108_VERSION := head
DVB_FIRMWARE_AVL6222_VERSION := head
DVB_FIRMWARE_CX21143_VERSION := head
DVB_FIRMWARE_CX24116_VERSION := head
DVB_FIRMWARE_STV6306_VERSION := head

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/dvb-firmware-as102.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  dvb-firmware-as102)
	@$(call install_fixup, dvb-firmware-as102,ARCH,noarch)
	@$(call install_fixup, dvb-firmware-as102,PRIORITY,optional)
	@$(call install_fixup, dvb-firmware-as102,SECTION,base)
	@$(call install_fixup, dvb-firmware-as102,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dvb-firmware-as102,DESCRIPTION,missing)

	@$(call install_alternative, dvb-firmware-as102, 0, 0, 644, /lib/firmware/as102_data1_st.hex, 0)
	@$(call install_alternative, dvb-firmware-as102, 0, 0, 644, /lib/firmware/as102_data2_st.hex, 0)

	@$(call install_finish, dvb-firmware-as102)

	@$(call touch)

$(STATEDIR)/dvb-firmware-avl2108.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  dvb-firmware-avl2108)
	@$(call install_fixup, dvb-firmware-avl2108,PRIORITY,optional)
	@$(call install_fixup, dvb-firmware-avl2108,SECTION,base)
	@$(call install_fixup, dvb-firmware-avl2108,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dvb-firmware-avl2108,DESCRIPTION,missing)

	@$(call install_alternative, dvb-firmware-avl2108, 0, 0, 644, /lib/firmware/dvb-fe-avl2108.fw, 0)

	@$(call install_finish, dvb-firmware-avl2108)

	@$(call touch)

$(STATEDIR)/dvb-firmware-avl6222.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  dvb-firmware-avl6222)
	@$(call install_fixup, dvb-firmware-avl6222,PRIORITY,optional)
	@$(call install_fixup, dvb-firmware-avl6222,SECTION,base)
	@$(call install_fixup, dvb-firmware-avl6222,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dvb-firmware-avl6222,DESCRIPTION,missing)

	@$(call install_alternative, dvb-firmware-avl6222, 0, 0, 644, /lib/firmware/dvb-fe-avl6222.fw, 0)

	@$(call install_finish, dvb-firmware-avl6222)

	@$(call touch)

$(STATEDIR)/dvb-firmware-cx21143.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  dvb-firmware-cx21143)
	@$(call install_fixup, dvb-firmware-cx21143,PRIORITY,optional)
	@$(call install_fixup, dvb-firmware-cx21143,SECTION,base)
	@$(call install_fixup, dvb-firmware-cx21143,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dvb-firmware-cx21143,DESCRIPTION,missing)

	@$(call install_alternative, dvb-firmware-cx21143, 0, 0, 644, /lib/firmware/dvb-fe-cx21143.fw, 0)

	@$(call install_finish, dvb-firmware-cx21143)

	@$(call touch)

$(STATEDIR)/dvb-firmware-cx24116.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  dvb-firmware-cx24116)
	@$(call install_fixup, dvb-firmware-cx24116,PRIORITY,optional)
	@$(call install_fixup, dvb-firmware-cx24116,SECTION,base)
	@$(call install_fixup, dvb-firmware-cx24116,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dvb-firmware-cx24116,DESCRIPTION,missing)

	@$(call install_alternative, dvb-firmware-cx24116, 0, 0, 644, /lib/firmware/dvb-fe-cx24116.fw, 0)

	@$(call install_finish, dvb-firmware-cx24116)

	@$(call touch)

$(STATEDIR)/dvb-firmware-stv6306.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  dvb-firmware-stv6306)
	@$(call install_fixup, dvb-firmware-stv6306,PRIORITY,optional)
	@$(call install_fixup, dvb-firmware-stv6306,SECTION,base)
	@$(call install_fixup, dvb-firmware-stv6306,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, dvb-firmware-stv6306,DESCRIPTION,missing)

	@$(call install_alternative, dvb-firmware-stv6306, 0, 0, 644, /lib/firmware/dvb-fe-stv6306.fw, 0)

	@$(call install_finish, dvb-firmware-stv6306)

	@$(call touch)

# vim: syntax=make
