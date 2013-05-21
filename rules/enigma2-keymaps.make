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

ifdef PTXCONF_ENIGMA2_KEYMAPS
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_ADB_BOX)     += enigma2-keymap-adb-box
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_CUBE)        += enigma2-keymap-cube
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_CUBE_SMALL)  += enigma2-keymap-cube-small
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_HL101)       += enigma2-keymap-hl101
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_IPBOX)       += enigma2-keymap-ipbox
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_SPARK)       += enigma2-keymap-spark
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_TF7700HDPVR) += enigma2-keymap-tf7700hdpvr
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_UFS910)      += enigma2-keymap-ufs910
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_UFS912)      += enigma2-keymap-ufs912
PACKAGES-$(PTXCONF_ENIGMA2_KEYMAP_VIP2)        += enigma2-keymap-vip2
endif

ENIGMA2_KEYMAP_ADB_BOX_VERSION   := head
ENIGMA2_KEYMAP_CUBE_VERSION := head
ENIGMA2_KEYMAP_CUBE_SMALL_VERSION := head
ENIGMA2_KEYMAP_HL101_VERSION := head
ENIGMA2_KEYMAP_IPBOX_VERSION := head
ENIGMA2_KEYMAP_SPARK_VERSION := head
ENIGMA2_KEYMAP_TF7700HDPVR_VERSION := head
ENIGMA2_KEYMAP_UFS910_VERSION := head
ENIGMA2_KEYMAP_UFS912_VERSION := head
ENIGMA2_KEYMAP_VIP2_VERSION := head

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/enigma2-keymap-adb-box.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-adb-box)
	@$(call install_fixup, enigma2-keymap-adb-box,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-adb-box,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-adb-box,SECTION,base)
	@$(call install_fixup, enigma2-keymap-adb-box,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-adb-box,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-adb-box, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_adb_box.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-adb-box)

	@$(call touch)


$(STATEDIR)/enigma2-keymap-cube.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-cube)
	@$(call install_fixup, enigma2-keymap-cube,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-cube,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-cube,SECTION,base)
	@$(call install_fixup, enigma2-keymap-cube,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-cube,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-cube, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_cube.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-cube)

	@$(call touch)

$(STATEDIR)/enigma2-keymap-cube-small.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-cube-small)
	@$(call install_fixup, enigma2-keymap-cube-small,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-cube-small,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-cube-small,SECTION,base)
	@$(call install_fixup, enigma2-keymap-cube-small,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-cube-small,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-cube-small, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_cube_small.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-cube-small)

	@$(call touch)

$(STATEDIR)/enigma2-keymap-hl101.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-hl101)
	@$(call install_fixup, enigma2-keymap-hl101,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-hl101,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-hl101,SECTION,base)
	@$(call install_fixup, enigma2-keymap-hl101,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-hl101,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-hl101, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_hl101.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-hl101)

	@$(call touch)

$(STATEDIR)/enigma2-keymap-ipbox.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-ipbox)
	@$(call install_fixup, enigma2-keymap-ipbox,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-ipbox,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-ipbox,SECTION,base)
	@$(call install_fixup, enigma2-keymap-ipbox,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-ipbox,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-ipbox, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_ipbox.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-ipbox)

	@$(call touch)

$(STATEDIR)/enigma2-keymap-spark.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-spark)
	@$(call install_fixup, enigma2-keymap-spark,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-spark,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-spark,SECTION,base)
	@$(call install_fixup, enigma2-keymap-spark,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-spark,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-spark, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_spark.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-spark)

	@$(call touch)

$(STATEDIR)/enigma2-keymap-tf7700hdpvr.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-tf7700hdpvr)
	@$(call install_fixup, enigma2-keymap-tf7700hdpvr,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-tf7700hdpvr,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-tf7700hdpvr,SECTION,base)
	@$(call install_fixup, enigma2-keymap-tf7700hdpvr,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-tf7700hdpvr,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-tf7700hdpvr, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_tf7700.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-tf7700hdpvr)

	@$(call touch)

$(STATEDIR)/enigma2-keymap-ufs910.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-ufs910)
	@$(call install_fixup, enigma2-keymap-ufs910,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-ufs910,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-ufs910,SECTION,base)
	@$(call install_fixup, enigma2-keymap-ufs910,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-ufs910,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-ufs910, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_ufs910.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-ufs910)

	@$(call touch)

$(STATEDIR)/enigma2-keymap-ufs912.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-ufs912)
	@$(call install_fixup, enigma2-keymap-ufs912,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-ufs912,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-ufs912,SECTION,base)
	@$(call install_fixup, enigma2-keymap-ufs912,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-ufs912,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-ufs912, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_ufs912.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-ufs912)

	@$(call touch)

$(STATEDIR)/enigma2-keymap-vip2.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  enigma2-keymap-vip2)
	@$(call install_fixup, enigma2-keymap-vip2,ARCH,noarch)
	@$(call install_fixup, enigma2-keymap-vip2,PRIORITY,optional)
	@$(call install_fixup, enigma2-keymap-vip2,SECTION,base)
	@$(call install_fixup, enigma2-keymap-vip2,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, enigma2-keymap-vip2,DESCRIPTION,missing)

	@$(call install_copy, enigma2-keymap-vip2, 0, 0, 644, $(PTXDIST_WORKSPACE)/local_src/extra/enigma2/keymap_vip2.xml, /usr/local/share/enigma2/keymap.xml, 0)

	@$(call install_finish, enigma2-keymap-vip2)

	@$(call touch)

# vim: syntax=make
