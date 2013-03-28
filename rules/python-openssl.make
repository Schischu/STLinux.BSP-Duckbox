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
PACKAGES-$(PTXCONF_PYTHON_OPENSSL) += python-openssl

#
# Paths and names
#
PYTHON_OPENSSL_VERSION	:= 0.9
PYTHON_OPENSSL_MD5	:= 5bf282b2d6a03af921920c34079580f2
PYTHON_OPENSSL		:= pyOpenSSL-$(PYTHON_OPENSSL_VERSION)
PYTHON_OPENSSL_SUFFIX	:= tar.gz
PYTHON_OPENSSL_URL	:= http://sourceforge.net/projects/pyopenssl/files/pyopenssl/$(PYTHON_OPENSSL_VERSION)/$(PYTHON_OPENSSL).$(PYTHON_OPENSSL_SUFFIX)
PYTHON_OPENSSL_SOURCE	:= $(SRCDIR)/$(PYTHON_OPENSSL).$(PYTHON_OPENSSL_SUFFIX)
PYTHON_OPENSSL_DIR	:= $(BUILDDIR)/$(PYTHON_OPENSSL)
PYTHON_OPENSSL_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON_OPENSSL_PATH		:= PATH=$(CROSS_PATH)
PYTHON_OPENSSL_CONF_TOOL	:= NO
PYTHON_OPENSSL_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/python-openssl.compile:
	@$(call targetinfo)
	cd $(PYTHON_OPENSSL_DIR) && \
		$(PYTHON_OPENSSL_PATH) $(PYTHON_OPENSSL_MAKE_ENV) \
		$(CROSS_PYTHON) setup.py build
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-openssl.install:
	@$(call targetinfo)
	cd $(PYTHON_OPENSSL_DIR) && \
		$(PYTHON_OPENSSL_PATH) $(PYTHON_OPENSSL_MAKE_ENV) \
		python$(PYTHON_MAJORMINOR) setup.py install --root=$(PYTHON_OPENSSL_PKGDIR) --prefix=/usr
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-openssl.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python-openssl)
	@$(call install_fixup, python-openssl,PRIORITY,optional)
	@$(call install_fixup, python-openssl,SECTION,base)
	@$(call install_fixup, python-openssl,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python-openssl,DESCRIPTION,missing)

	@for file in `cd $(PYTHON_OPENSSL_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/OpenSSL; find .  \
			! -type d ! -name "*.py" -printf "%p\n"`; do \
		$(call install_copy, python-openssl, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/OpenSSL/$$file); \
	done

	@$(call install_finish, python-openssl)

	@$(call touch)

# vim: syntax=make
