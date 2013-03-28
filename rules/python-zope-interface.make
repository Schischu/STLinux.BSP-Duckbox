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
PACKAGES-$(PTXCONF_PYTHON_ZOPE_INTERFACE) += python-zope-interface

#
# Paths and names
#
PYTHON_ZOPE_INTERFACE_VERSION	:= 4.0.1
PYTHON_ZOPE_INTERFACE_MD5	:= d171f8f8a6997409c7680f1dbb3b7e45
PYTHON_ZOPE_INTERFACE		:= zope.interface-$(PYTHON_ZOPE_INTERFACE_VERSION)
PYTHON_ZOPE_INTERFACE_SUFFIX	:= tar.gz
PYTHON_ZOPE_INTERFACE_URL	:= http://pypi.python.org/packages/source/z/zope.interface/$(PYTHON_ZOPE_INTERFACE).$(PYTHON_ZOPE_INTERFACE_SUFFIX)
PYTHON_ZOPE_INTERFACE_SOURCE	:= $(SRCDIR)/$(PYTHON_ZOPE_INTERFACE).$(PYTHON_ZOPE_INTERFACE_SUFFIX)
PYTHON_ZOPE_INTERFACE_DIR	:= $(BUILDDIR)/$(PYTHON_ZOPE_INTERFACE)
PYTHON_ZOPE_INTERFACE_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON_ZOPE_INTERFACE_PATH		:= PATH=$(CROSS_PATH)
PYTHON_ZOPE_INTERFACE_CONF_TOOL	:= NO
PYTHON_ZOPE_INTERFACE_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/python-zope-interface.compile:
	@$(call targetinfo)
	cd $(PYTHON_ZOPE_INTERFACE_DIR) && \
		$(PYTHON_ZOPE_INTERFACE_PATH) $(PYTHON_ZOPE_INTERFACE_MAKE_ENV) \
		$(CROSS_PYTHON) setup.py build
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-zope-interface.install:
	@$(call targetinfo)
	cd $(PYTHON_ZOPE_INTERFACE_DIR) && \
		$(PYTHON_ZOPE_INTERFACE_PATH) $(PYTHON_ZOPE_INTERFACE_MAKE_ENV) \
		python$(PYTHON_MAJORMINOR) setup.py install --root=$(PYTHON_ZOPE_INTERFACE_PKGDIR) --prefix=/usr
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-zope-interface.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python-zope-interface)
	@$(call install_fixup, python-zope-interface,PRIORITY,optional)
	@$(call install_fixup, python-zope-interface,SECTION,base)
	@$(call install_fixup, python-zope-interface,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python-zope-interface,DESCRIPTION,missing)

	@for file in `cd $(PYTHON_ZOPE_INTERFACE_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/zope; find .  \
			! -type d ! -name "*.py" -printf "%p\n"`; do \
		$(call install_copy, python-zope-interface, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/zope/$$file); \
	done

	@$(call install_finish, python-zope-interface)

	@$(call touch)

# vim: syntax=make
