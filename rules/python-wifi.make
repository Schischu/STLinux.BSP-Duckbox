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
PACKAGES-$(PTXCONF_PYTHON_WIFI) += python-wifi

#
# Paths and names
#
PYTHON_WIFI_VERSION	:=0.5.0
PYTHON_WIFI_MD5	:= 8fe7fd0a4edce1f9bedaff4acb7fd500
PYTHON_WIFI		:= python-wifi-$(PYTHON_WIFI_VERSION)
PYTHON_WIFI_SUFFIX	:= tar.bz2
PYTHON_WIFI_URL	:= http://freefr.dl.sourceforge.net/project/pythonwifi.berlios/$(PYTHON_WIFI).$(PYTHON_WIFI_SUFFIX)
PYTHON_WIFI_SOURCE	:= $(SRCDIR)/$(PYTHON_WIFI).$(PYTHON_WIFI_SUFFIX)
PYTHON_WIFI_DIR	:= $(BUILDDIR)/$(PYTHON_WIFI)
PYTHON_WIFI_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PYTHON_WIFI_PATH		:= PATH=$(CROSS_PATH)
PYTHON_WIFI_CONF_TOOL	:= NO
PYTHON_WIFI_MAKE_ENV		:= $(CROSS_ENV)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/python-wifi.compile:
	@$(call targetinfo)
	cd $(PYTHON_WIFI_DIR) && \
		$(PYTHON_WIFI_PATH) $(PYTHON_WIFI_MAKE_ENV) PYTHONPATH=$(SYSROOT)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages \
		$(CROSS_PYTHON) setup.py build
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-wifi.install:
	@$(call targetinfo)
	cd $(PYTHON_WIFI_DIR) && \
		$(PYTHON_WIFI_PATH) $(PYTHON_WIFI_MAKE_ENV) PYTHONPATH=$(SYSROOT)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages \
		python$(PYTHON_MAJORMINOR) setup.py install --root=$(PYTHON_WIFI_PKGDIR) --prefix=/usr
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/python-wifi.targetinstall:
	@$(call targetinfo)

	@$(call install_init, python-wifi)
	@$(call install_fixup, python-wifi,PRIORITY,optional)
	@$(call install_fixup, python-wifi,SECTION,base)
	@$(call install_fixup, python-wifi,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, python-wifi,DESCRIPTION,missing)

	@for file in `cd $(PYTHON_WIFI_PKGDIR)/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/pythonwifi; find .  \
			! -type d ! -name "*.py" -printf "%p\n"`; do \
		$(call install_copy, python-wifi, 0, 0, 0644, -, \
			/usr/lib/python$(PYTHON_MAJORMINOR)/site-packages/pythonwifi/$$file); \
	done

	@$(call install_finish, python-wifi)

	@$(call touch)

# vim: syntax=make
