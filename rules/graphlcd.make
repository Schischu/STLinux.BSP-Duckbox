# -*-makefile-*-
#
# Copyright (C) 2003 by Robert Schwebel <r.schwebel@pengutronix.de>
#                       Pengutronix <info@pengutronix.de>, Germany
#               2009, 2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_GRAPHLCD) += graphlcd

#
# Paths and names
#
GRAPHLCD_VERSION	:= 1e01a8963f9ab95ba40ddb44a6c166b8e546053d
GRAPHLCD		:= graphlcd-$(GRAPHLCD_VERSION)
GRAPHLCD_URL	:= git://projects.vdr-developer.org/graphlcd-base.git
GRAPHLCD_SOURCE_GIT	:= $(SRCDIR)/graphlcd-base.git
GRAPHLCD_DIR	:= $(BUILDDIR)/$(GRAPHLCD)
GRAPHLCD_LICENSE	:= graphlcd

$(STATEDIR)/graphlcd.prepare: $(STATEDIR)/libusb.install

$(STATEDIR)/graphlcd.get:
	@$(call targetinfo)
	
		if [ -d $(GRAPHLCD_SOURCE_GIT) ]; then \
			cd $(GRAPHLCD_SOURCE_GIT); \
			git pull -u origin touchcol 2>&1 > /dev/null; \
			git checkout $(GRAPHLCD_VERSION) 2>&1 > /dev/null; \
			cd -; \
		else \
			git clone $(GRAPHLCD_URL) $(GRAPHLCD_SOURCE_GIT) 2>&1 > /dev/null; \
		fi; 2>&1 > /dev/null
	
		if [ ! "$(GRAPHLCD_VERSION)" == "HEAD" ]; then \
			cd $(GRAPHLCD_SOURCE_GIT); \
			git checkout $(GRAPHLCD_VERSION) 2>&1 > /dev/null; \
			cd -; \
		fi; 2>&1 > /dev/null
	
	@$(call touch)


$(STATEDIR)/graphlcd.extract:
	@$(call targetinfo)
	
	rm -rf $(BUILDDIR)/$(GRAPHLCD); \
	cp -a $(GRAPHLCD_SOURCE_GIT) $(BUILDDIR)/$(GRAPHLCD); \
	rm -rf $(BUILDDIR)/$(GRAPHLCD)/.git;
	
	@$(call patchin, GRAPHLCD)	
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
GRAPHLCD_PATH	:= PATH=$(CROSS_PATH)
GRAPHLCD_CONF_ENV 	:= $(CROSS_ENV)

$(STATEDIR)/graphlcd.compile:
	@$(call targetinfo)

	cd $(GRAPHLCD_DIR) && $(GRAPHLCD_PATH) $(GRAPHLCD_CONF_ENV) \
		$(MAKE) all
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/graphlcd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, graphlcd)
	@$(call install_fixup, graphlcd,PRIORITY,optional)
	@$(call install_fixup, graphlcd,SECTION,base)
	@$(call install_fixup, graphlcd,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, graphlcd,DESCRIPTION,missing)

	@$(call install_lib, graphlcd, 0, 0, 0644, libglcddrivers)
	@$(call install_lib, graphlcd, 0, 0, 0644, libglcdgraphics)
	@$(call install_lib, graphlcd, 0, 0, 0644, libglcdskin)
	@$(call install_copy, graphlcd, 0, 0, 0644, -, /etc/graphlcd.conf)

	@$(call install_finish, graphlcd)

	@$(call touch)

# vim: syntax=make
