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
PACKAGES-$(PTXCONF_PYTHON_TWISTED) += python-twisted

#
# Paths and names
#
PYTHON_TWISTED_MAJOR_VERSION	:= 12.1
PYTHON_TWISTED_VERSION	:= $(PYTHON_TWISTED_MAJOR_VERSION).0
PYTHON_TWISTED_MD5	:= f396f1d6f5321e869c2f89b2196a9eb5
PYTHON_TWISTED		:= Twisted-$(PYTHON_TWISTED_VERSION)
PYTHON_TWISTED_SUFFIX	:= tar.bz2
PYTHON_TWISTED_URL	:= http://twistedmatrix.com/Releases/Twisted/$(PYTHON_TWISTED_MAJOR_VERSION)/$(PYTHON_TWISTED).$(PYTHON_TWISTED_SUFFIX)
PYTHON_TWISTED_SOURCE	:= $(SRCDIR)/$(PYTHON_TWISTED).$(PYTHON_TWISTED_SUFFIX)
PYTHON_TWISTED_DIR	:= $(BUILDDIR)/$(PYTHON_TWISTED)
PYTHON_TWISTED_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON_TWISTED_PATH		:= PATH=$(CROSS_PATH)
PYTHON_TWISTED_CONF_TOOL	:= NO
PYTHON_TWISTED_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/python-twisted.compile:
	@$(call targetinfo)
	cd $(PYTHON_TWISTED_DIR) && \
		$(PYTHON_TWISTED_PATH) $(PYTHON_TWISTED_MAKE_ENV) \
		$(CROSS_PYTHON) setup.py build
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-twisted.install:
	@$(call targetinfo)
	cd $(PYTHON_TWISTED_DIR) && \
		$(PYTHON_TWISTED_PATH) $(PYTHON_TWISTED_MAKE_ENV) \
		python$(PYTHON_MAJORMINOR) setup.py install --root=$(PYTHON_TWISTED_PKGDIR) --prefix=/usr
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-twisted.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python-twisted)
	@$(call install_fixup, python-twisted,PRIORITY,optional)
	@$(call install_fixup, python-twisted,SECTION,base)
	@$(call install_fixup, python-twisted,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python-twisted,DESCRIPTION,missing)

	@for file in `cd $(PYTHON_TWISTED_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/twisted; find .  \
			! -type d ! -name "*.py" -printf "%p\n"`; do \
		$(call install_copy, python-twisted, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/twisted/$$file); \
	done

	@$(call install_finish, python-twisted)

	@$(call touch)

# vim: syntax=make
