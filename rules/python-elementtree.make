# -*-makefile-*-
#
# Copyright (C) 2011 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PYTHON_ELEMENTTREE) += python-elementtree

#
# Paths and names
#
PYTHON_ELEMENTTREE_VERSION	:= 1.2.6-20050316
PYTHON_ELEMENTTREE_MD5	:= e1b21716be8bfff8bf192fc3880ad008
PYTHON_ELEMENTTREE		:= elementtree-$(PYTHON_ELEMENTTREE_VERSION)
PYTHON_ELEMENTTREE_SUFFIX	:= tar.gz
PYTHON_ELEMENTTREE_URL	:= http://effbot.org/media/downloads/$(PYTHON_ELEMENTTREE).$(PYTHON_ELEMENTTREE_SUFFIX)
PYTHON_ELEMENTTREE_SOURCE	:= $(SRCDIR)/$(PYTHON_ELEMENTTREE).$(PYTHON_ELEMENTTREE_SUFFIX)
PYTHON_ELEMENTTREE_DIR	:= $(BUILDDIR)/$(PYTHON_ELEMENTTREE)
PYTHON_ELEMENTTREE_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON_ELEMENTTREE_PATH		:= PATH=$(CROSS_PATH)
PYTHON_ELEMENTTREE_CONF_TOOL	:= NO
PYTHON_ELEMENTTREE_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/python-elementtree.compile:
	@$(call targetinfo)
	cd $(PYTHON_ELEMENTTREE_DIR) && \
		$(PYTHON_ELEMENTTREE_PATH) $(PYTHON_ELEMENTTREE_MAKE_ENV) \
		$(CROSS_PYTHON) setup.py build
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-elementtree.install:
	@$(call targetinfo)
	cd $(PYTHON_ELEMENTTREE_DIR) && \
		$(PYTHON_ELEMENTTREE_PATH) $(PYTHON_ELEMENTTREE_MAKE_ENV) \
		python$(PYTHON_MAJORMINOR) setup.py install --root=$(PYTHON_ELEMENTTREE_PKGDIR) --prefix=/usr
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-elementtree.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python-elementtree)
	@$(call install_fixup, python-elementtree,PRIORITY,optional)
	@$(call install_fixup, python-elementtree,SECTION,base)
	@$(call install_fixup, python-elementtree,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python-elementtree,DESCRIPTION,missing)

	@for file in `find $(PYTHON_ELEMENTTREE_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/elementtree  \
			! -type d ! -name "*.py" -printf "%f\n"`; do \
		$(call install_copy, python-elementtree, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/elementtree/$$file); \
	done

	@$(call install_finish, python-elementtree)

	@$(call touch)

# vim: syntax=make
