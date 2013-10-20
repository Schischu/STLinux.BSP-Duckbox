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
PACKAGES-$(PTXCONF_YAJL) += yajl

#
# Paths and names
#
YAJL_VERSION	:= f4b2b1af87483caac60e50e5352fc783d9b2de2d
YAJL		:= yajl-$(YAJL_VERSION)
YAJL_URL	:= git://github.com/lloyd/yajl
YAJL_GIT_BRANCH	:= master
YAJL_GIT_HEAD	:= $(YAJL_VERSION)
YAJL_SOURCE	:= $(SRCDIR)/yajl.git
YAJL_DIR	:= $(BUILDDIR)/$(YAJL)
YAJL_LICENSE	:= yajl

#
# autoconf
#
YAJL_CONF_TOOL	:= autoconf
YAJL_CONF_OPT	:= \
	--prefix="/usr"
	
YAJL_MAKE_OPT	:= distro

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/yajl.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init,   yajl)
	@$(call install_fixup,  yajl, PRIORITY,    optional)
	@$(call install_fixup,  yajl, SECTION,     base)
	@$(call install_fixup,  yajl, AUTHOR,      "Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup,  yajl, DESCRIPTION, missing)
	
	@$(call install_lib,    yajl, 0, 0, 0644, libyajl)
	
	@$(call install_finish, yajl)
	
	@$(call touch)

# vim: syntax=make
