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
PACKAGES-$(PTXCONF_PYTHON_LXML) += python-lxml

#
# Paths and names
#
PYTHON_LXML_MAJOR_VERSION	:= 2.2
PYTHON_LXML_VERSION	:= $(PYTHON_LXML_MAJOR_VERSION).8
PYTHON_LXML_MD5	:= d6c612d63a84d79440912a1b29d3b981
PYTHON_LXML		:= lxml-$(PYTHON_LXML_VERSION)
PYTHON_LXML_SUFFIX	:= tgz
PYTHON_LXML_URL	:= http://launchpad.net/lxml/$(PYTHON_LXML_MAJOR_VERSION)/$(PYTHON_LXML_VERSION)/+download/$(PYTHON_LXML).$(PYTHON_LXML_SUFFIX)
PYTHON_LXML_SOURCE	:= $(SRCDIR)/$(PYTHON_LXML).$(PYTHON_LXML_SUFFIX)
PYTHON_LXML_DIR	:= $(BUILDDIR)/$(PYTHON_LXML)
PYTHON_LXML_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON_LXML_PATH		:= PATH=$(CROSS_PATH)
PYTHON_LXML_CONF_TOOL	:= NO
PYTHON_LXML_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/python-lxml.compile:
	@$(call targetinfo)
	cd $(PYTHON_LXML_DIR) && \
		$(PYTHON_LXML_PATH) $(PYTHON_LXML_MAKE_ENV) \
		$(CROSS_PYTHON) setup.py build \
			--with-xml2-config=$(PTXDIST_SYSROOT_CROSS)/bin/xml2-config \
			--with-xslt-config=$(PTXDIST_SYSROOT_CROSS)/bin/xslt-config
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-lxml.install:
	@$(call targetinfo)
	cd $(PYTHON_LXML_DIR) && \
		$(PYTHON_LXML_PATH) $(PYTHON_LXML_MAKE_ENV) \
		python$(PYTHON_MAJORMINOR) setup.py install --root=$(PYTHON_LXML_PKGDIR) --prefix=/usr
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-lxml.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python-lxml)
	@$(call install_fixup, python-lxml,PRIORITY,optional)
	@$(call install_fixup, python-lxml,SECTION,base)
	@$(call install_fixup, python-lxml,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python-lxml,DESCRIPTION,missing)

	@for file in `cd $(PYTHON_LXML_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/lxml; find .  \
			! -type d ! -name "*.py" -printf "%p\n"`; do \
		$(call install_copy, python-lxml, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/lxml/$$file); \
	done

	@$(call install_finish, python-lxml)

	@$(call touch)

# vim: syntax=make
