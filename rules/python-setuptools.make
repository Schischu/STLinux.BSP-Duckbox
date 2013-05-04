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
PACKAGES-$(PTXCONF_PYTHON_SETUPTOOLS) += python-setuptools

#
# Paths and names
#
PYTHON_SETUPTOOLS_VERSION	:= 0.6c11
PYTHON_SETUPTOOLS_MD5	:= 7df2a529a074f613b509fb44feefe74e
PYTHON_SETUPTOOLS		:= setuptools-$(PYTHON_SETUPTOOLS_VERSION)
PYTHON_SETUPTOOLS_SUFFIX	:= tar.gz
PYTHON_SETUPTOOLS_URL	:= http://pypi.python.org/packages/source/s/setuptools/$(PYTHON_SETUPTOOLS).$(PYTHON_SETUPTOOLS_SUFFIX)
PYTHON_SETUPTOOLS_SOURCE	:= $(SRCDIR)/$(PYTHON_SETUPTOOLS).$(PYTHON_SETUPTOOLS_SUFFIX)
PYTHON_SETUPTOOLS_DIR	:= $(BUILDDIR)/$(PYTHON_SETUPTOOLS)
PYTHON_SETUPTOOLS_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON_SETUPTOOLS_PATH		:= PATH=$(CROSS_PATH)
PYTHON_SETUPTOOLS_CONF_TOOL	:= NO
PYTHON_SETUPTOOLS_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/python-setuptools.compile:
	@$(call targetinfo)
	cd $(PYTHON_SETUPTOOLS_DIR) && \
		$(PYTHON_SETUPTOOLS_PATH) $(PYTHON_SETUPTOOLS_MAKE_ENV) \
		$(CROSS_PYTHON) setup.py build
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-setuptools.install:
	@$(call targetinfo)
	cd $(PYTHON_SETUPTOOLS_DIR) && \
		$(PYTHON_SETUPTOOLS_PATH) $(PYTHON_SETUPTOOLS_MAKE_ENV) \
		python$(PYTHON_MAJORMINOR) setup.py install --root=$(PYTHON_SETUPTOOLS_PKGDIR) --prefix=/usr
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-setuptools.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python-setuptools)
	@$(call install_fixup, python-setuptools,PRIORITY,optional)
	@$(call install_fixup, python-setuptools,SECTION,base)
	@$(call install_fixup, python-setuptools,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python-setuptools,DESCRIPTION,missing)

	@for file in easy_install easy_install-$(PYTHON_MAJORMINOR) ; do \
		$(call install_copy, python-setuptools, 0, 0, 0755, -, \
			/usr/bin/$$file); \
	done

	@for file in easy_install pkg_resources site ; do \
		$(call install_copy, python-setuptools, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/$$file.py[co]); \
	done

	@for file in `cd $(PYTHON_SETUPTOOLS_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/setuptools; find .  \
			! -type d ! -name "*.py" -printf "%p\n"`; do \
		$(call install_copy, python-setuptools, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/setuptools/$$file); \
	done

	@$(call install_finish, python-setuptools)

	@$(call touch)

# vim: syntax=make
