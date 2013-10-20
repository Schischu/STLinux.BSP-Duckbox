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
PACKAGES-$(PTXCONF_PYTHON_GDATA) += python-gdata

#
# Paths and names
#
PYTHON_GDATA_VERSION	:= 2.0.17
PYTHON_GDATA_MD5	:= 8c3600cf02c6c228e28e366b2e5d5c32
PYTHON_GDATA		:= gdata-$(PYTHON_GDATA_VERSION)
PYTHON_GDATA_SUFFIX	:= tar.gz
PYTHON_GDATA_URL	:= http://gdata-python-client.googlecode.com/files/$(PYTHON_GDATA).$(PYTHON_GDATA_SUFFIX)
PYTHON_GDATA_SOURCE	:= $(SRCDIR)/$(PYTHON_GDATA).$(PYTHON_GDATA_SUFFIX)
PYTHON_GDATA_DIR	:= $(BUILDDIR)/$(PYTHON_GDATA)
PYTHON_GDATA_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON_GDATA_PATH		:= PATH=$(CROSS_PATH)
PYTHON_GDATA_CONF_TOOL	:= NO
PYTHON_GDATA_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/python-gdata.compile:
	@$(call targetinfo)
	cd $(PYTHON_GDATA_DIR) && \
		$(PYTHON_GDATA_PATH) $(PYTHON_GDATA_MAKE_ENV) \
		$(CROSS_PYTHON) setup.py build
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-gdata.install:
	@$(call targetinfo)
	cd $(PYTHON_GDATA_DIR) && \
		$(PYTHON_GDATA_PATH) $(PYTHON_GDATA_MAKE_ENV) \
		python$(PYTHON_MAJORMINOR) setup.py install --root=$(PYTHON_GDATA_PKGDIR) --prefix=/usr
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-gdata.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python-gdata)
	@$(call install_fixup, python-gdata,PRIORITY,optional)
	@$(call install_fixup, python-gdata,SECTION,base)
	@$(call install_fixup, python-gdata,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python-gdata,DESCRIPTION,missing)

	@for file in `cd $(PYTHON_GDATA_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/atom; find .  \
			! -type d ! -name "*.py" -printf "%p\n"`; do \
		$(call install_copy, python-gdata, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/atom/$$file); \
	done
	@for file in `cd $(PYTHON_GDATA_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/gdata; find .  \
			! -type d ! -name "*.py" -printf "%p\n"`; do \
		$(call install_copy, python-gdata, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/gdata/$$file); \
	done

	@$(call install_finish, python-gdata)

	@$(call touch)

# vim: syntax=make
