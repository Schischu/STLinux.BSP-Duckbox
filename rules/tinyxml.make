# -*-makefile-*-
#
# Copyright (C) 2012 by fga
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TINYXML) += tinyxml

#
# Paths and names
#
TINYXML_VERSION	:= 2_6_2
TINYXML_MD5	:= c1b864c96804a10526540c664ade67f0
TINYXML		:= tinyxml_$(TINYXML_VERSION)
TINYXML_SUFFIX	:= tar.gz
TINYXML_URL	:= http://downloads.sourceforge.net/project/tinyxml/tinyxml/2.6.2/$(TINYXML).$(TINYXML_SUFFIX)
TINYXML_SOURCE	:= $(SRCDIR)/$(TINYXML).$(TINYXML_SUFFIX)
TINYXML_DIR	:= $(BUILDDIR)/$(TINYXML)
TINYXML_LICENSE	:= unknown


$(STATEDIR)/tinyxml.prepare:
	@$(call targetinfo)
	@$(call touch)

TINYXML_MAKE_OPT := PREFIX=$(TINYXML_PKGDIR) INSTALLROOT=$(TINYXML_PKGDIR)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/tinyxml.compile:
	@$(call targetinfo)
	@$(call world/compile, TINYXML)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

TINYXML_INSTALL_OPT := install PREFIX=$(TINYXML_PKGDIR)/usr INSTALLROOT=$(TINYXML_PKGDIR)

$(STATEDIR)/tinyxml.install:
	@$(call targetinfo)
	@$(call world/install, TINYXML)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tinyxml.targetinstall:
	@$(call targetinfo)

	@$(call install_init,   tinyxml)
	@$(call install_fixup,  tinyxml, PRIORITY,    optional)
	@$(call install_fixup,  tinyxml, SECTION,     base)
	@$(call install_fixup,  tinyxml, AUTHOR,      "fga")
	@$(call install_fixup,  tinyxml, DESCRIPTION, missing)

	@$(call install_lib,    tinyxml, 0, 0, 0644, libtinyxml)

	@$(call install_finish, tinyxml)

	@$(call touch)

# vim: syntax=make
