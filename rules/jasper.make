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
PACKAGES-$(PTXCONF_JASPER) += jasper

#
# Paths and names
#
JASPER_VERSION	:= 1.900.1
JASPER_MD5		:= a342b2b4495b3e1394e161eb5d85d754
JASPER			:= jasper-$(JASPER_VERSION)
JASPER_SUFFIX	:= zip
JASPER_URL		:= http://www.ece.uvic.ca/~frodo/jasper/software/$(JASPER).$(JASPER_SUFFIX)
JASPER_SOURCE	:= $(SRCDIR)/$(JASPER).$(JASPER_SUFFIX)
JASPER_DIR		:= $(BUILDDIR)/$(JASPER)
JASPER_LICENSE	:= unknown

#
# autoconf
#
JASPER_CONF_TOOL	:= autoconf

# vim: syntax=make
